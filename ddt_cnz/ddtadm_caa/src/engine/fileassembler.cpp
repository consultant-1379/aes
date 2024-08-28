//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************

#include "engine/fileassembler.h"

#include "engine/context.h"
#include "engine/workingset.h"
#include "engine/datasink.h"
#include "engine/fileformat.h"
#include "store/handlerinterface.h"

#include "common/programconstants.h"
#include "common/macros.h"
#include "common/utility.h"
#include "common/tracer.h"
#include "common/logger.h"

#include <sys/eventfd.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_FileAssembler)

namespace engine
{
	FileAssembler::FileAssembler(const std::string& dataSourceName, const std::string& producerName, const int& stopEvent, DataSink* sink)
	: DataAssembler(dataSourceName, producerName, stopEvent, sink),
	  m_refreshData(false),
	  m_fileFormat(),
	  m_remoteFile()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	FileAssembler::~FileAssembler()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int FileAssembler::svc()
	{
		AES_DDT_TRACE_FUNCTION;

		m_stopRequested = false;
		m_running = true;

		int result = common::errorCode::ERR_NO_ERRORS;

		//Logging
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler started", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
		AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler started", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());

		// get file format data
		m_fileFormat = workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName)->getFileFormat();

		// GET THE STORE READER
		m_storeReader = workingSet_t::instance()->getStoringManager().getReader(m_dataSourceName, m_producerName);

		//Logging
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] Connected to the store reader: %s", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), (m_storeReader ? "YES" : "NO"));
		AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] Connected to the store reader: %s", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), (m_storeReader ? "YES" : "NO"));

		// Set handles
		nfds_t nfds = 2;
		struct pollfd fds[nfds];

		// Initialize the pollfd structure
		ACE_OS::memset(fds, 0 , sizeof(fds));

		time_t secs = 0;
		suseconds_t usecs = 0;
		ACE_Time_Value timeout(secs, usecs);

		fds[stop_pos].fd = m_stopEvent;
		fds[stop_pos].events = POLLIN;

		fds[eventNotify_pos].fd = getChangeEventHandle();
		fds[eventNotify_pos].events = POLLIN;

		// Thread loop
		while(m_running)
		{
			// Instant check for shutdown event, timeout is zero in order to causes poll() to return immediately,
			// even if no events are ready.
			int pollresult = ACE_OS::poll(fds, nfds, &timeout);

			// check poll result
			if(pollresult > 0)
			{
				// check for shutdown
				if( (fds[stop_pos].revents & POLLIN) || m_stopRequested )
				{
					//Shutdown raised...

					//Stop the timer
					m_timer.stop();

					// reset the current remote file info. NOTE: reset will set m_remoteFile as "closed"
					// Whatever is in the buffer will be lost
					m_remoteFile.reset();

					// Set received shutdown event
					m_running = false;

					//Logging
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler received shutdown request", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler received shutdown request", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());

					break;
				}

				// check for updated parameter
				if( fds[eventNotify_pos].revents & POLLIN )
				{
					//The User modified some MO attribute...

					//reset the event
					eventfd_t readEvent = 0U;
					eventfd_read(fds[eventNotify_pos].fd, &readEvent);

					//remember to refresh asap
					m_refreshData = true;

					//Logging
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] User changed something. Configuration will be refreshed as the current file will be transmitted.", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] User changed something. Configuration will be refreshed as the current file will be transmitted.", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
				}
			}

			//------------------------------------------------------------------
			// Try to assemble a chunk...
			uint32_t chunkSize = m_outputFormat->getDataChunkSize();
			uint32_t freeChunkSpace = chunkSize;
			uint32_t recordCounter = NO_RECORD;
			unsigned char chunk[chunkSize];
			memset(chunk, 0, chunkSize);

			// Get Chunk tries to return the entire chunk available in the checkpoint
			// NOTE: one chunk can contains more records
			int readerError = getChunk(chunk, freeChunkSpace, recordCounter);

			// the holdtime must start also in case the remote file is not open yet but some record has been received
			if(NO_RECORD != recordCounter)
			{
				// start the timer if not already started
				m_timer.start(m_outputFormat->getHoldTimeSeconds());
			}

			//Get Chunk failed: unable to read a full chunk
			if(common::errorCode::ERR_NO_ERRORS != readerError &&
					common::errorCode::ERR_CKPT_SECTIONS_EMPTY != readerError &&
					common::errorCode::ERR_CKPT_WAIT_COMMIT != readerError)
			{
				bool dataReady = true;

				if(common::errorCode::ERR_CKPT_NOSECTIONS_TRYAGAIN == readerError )
				{
					//HU94423-START: if any timer is started use a safe timeout
					bool useSafeTimer = (NO_RECORD == recordCounter && m_timer.isDisarmed());
					if(useSafeTimer) m_timer.start(300);
					//HU94423-END

					// wait for: new data, time elapsed or shutdown
					dataReady = waitNewDataAvailable();

					if(useSafeTimer) m_timer.stop();//HU94423: cleanup
				}
				else
				{
					handleError(readerError);

					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] Iterator Read Error: <%d>, ready records:<%u>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), readerError, recordCounter);
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] Iterator Read Error :<%d>, ready records:<%u>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), readerError, recordCounter);

					ACE_Time_Value waitBeforeRetry(0, ERROR_MAX_WAIT_TIME_MICROSEC);
					ACE_OS::poll(fds, nfds, &waitBeforeRetry);
				}

				if(dataReady || m_stopRequested)
				{
					m_storeReader->commit(NO_RECORD);
					m_remoteFile.resetFileBuffer();
					continue;
				}
				// here the m_timer isExpired... go on and try to assemble tha last chunk before transmission...
			}

			// Something was read from checkpoint or there is something waiting to be sent in the buffer
			if( NO_RECORD != recordCounter || m_remoteFile.getBufferSizeInByte() > 0 )
			{

				if (NO_RECORD != recordCounter)
				{
					//Chunk read, format it...
					chunkSize = formatDataChunk(chunk, freeChunkSpace);
				}

				// Transmit the partial buffer if something went wrong during previous read and checkpoint shall be read again from the beginning
				BufferTransmission requestedTransmission = COMPLETE_FILE;

				// Just logging...
				if(common::errorCode::ERR_CKPT_WAIT_COMMIT == readerError)
				{
					requestedTransmission = PARTIAL_BUFFER;

					//Logging
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FORCE FILE TRANSMISSION ON 'ERR_CKPT_WAIT_COMMIT'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FORCE FILE TRANSMISSION ON 'ERR_CKPT_WAIT_COMMIT'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					printf("[%s --> %s@%s] FORCE FILE TRANSMISSION ON 'ERR_CKPT_WAIT_COMMIT'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
				}

				// Just logging...
				if(m_timer.isExpired())
				{
					requestedTransmission = PARTIAL_BUFFER;

					//Logging
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FORCE FILE TRANSMISSION ON 'TIMEOUT EXPIRED'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FORCE FILE TRANSMISSION ON 'TIMEOUT EXPIRED'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					printf("[%s --> %s@%s] FORCE FILE TRANSMISSION ON 'TIMEOUT EXPIRED'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
				}

				//Transmit it...
				int transmissionResult = transmitFile(chunk, chunkSize, recordCounter, requestedTransmission);

				if(common::errorCode::ERR_NO_VALID_PEER == transmissionResult)
				{
					// No valid peer available, stop timer and wait for a valid peer
					m_timer.stop();

					(void)waitValidPeerAvailable();

					// check for shutdown
					if( (fds[stop_pos].revents & POLLIN) || m_stopRequested )
					{
						//Shutdown raised...
						// reset the current remote file info. NOTE: reset will set m_remoteFile as "closed"
						m_remoteFile.reset();

						// Set received shutdown event
						m_running = false;

						//Logging
						AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler received shutdown event on TransmitChunk failure due to ERR_NO_VALID_PEER.", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
						AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler received shutdown event on TransmitChunk failure due to ERR_NO_VALID_PEER.", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					}
				}

			}

		}//main loop

		//Logging
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler Task stopped", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
		AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler Task stopped", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());

		m_stopRequested = true;
		m_running = false;

		return result;
	}

	int FileAssembler::transmitFile(unsigned char* chunk, const uint32_t& chunkSize, const uint32_t& recordCounter, const BufferTransmission& requestedTransmission)
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		m_remoteFile.addChunk(chunk, chunkSize, recordCounter);

		// Logging
		if(m_remoteFile.isMaxSizeInByteReached(m_fileFormat->getFileSizeInByte() - m_outputFormat->getDataChunkSize()))
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FORCE FILE TRANSMISSION WHEN 'MaxSizeInByteReached'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FORCE FILE TRANSMISSION WHEN 'MaxSizeInByteReached'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
			printf("[%s --> %s@%s] FORCE FILE TRANSMISSION WHEN 'MaxSizeInByteReached'\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
		}

		if( PARTIAL_BUFFER == requestedTransmission ||
				m_remoteFile.isMaxSizeInByteReached(m_fileFormat->getFileSizeInByte() - m_outputFormat->getDataChunkSize()) )
		{

			printf("    _________\n");
			printf("   |\\       /|\n");
			printf("   | \\     / |\n");
			printf("   |  `...'  |\n");
			printf("   |__/___\\__|\n");

			result = onOutgoingFile();
			if (common::errorCode::ERR_NO_ERRORS == result)
			{
				//-----------
				// SEND DATA

				// Logging
				if(PARTIAL_BUFFER == requestedTransmission) AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Pushing PARTIAL_BUFFER with <%lu> bytes", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getBufferSizeInByte());
				else AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Pushing COMPLETE_FILE with <%lu> bytes", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getBufferSizeInByte());

				// Send and close file
				result = m_sink->pushData(m_remoteFile.getBuffer(), m_remoteFile.getBufferSizeInByte(), m_producerName.c_str(), m_remoteFile.getNameAsChar());
				if (common::errorCode::ERR_NO_ERRORS == result)
				{
					// commit the stored records
					result = onTransmittedFile(m_remoteFile.getRecordCounter());
				}
			}
		}

		if (common::errorCode::ERR_NO_ERRORS != result)
		{
			// no records sent, reset reader position to the first read record
			m_storeReader->commit(NO_RECORD);

			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] Cannot transmit chunk containing %u records",
					m_producerName.c_str(), m_dataSourceName.c_str(), recordCounter);

			AES_DDT_TRACE_MESSAGE("[%s@%s] ERROR: Cannot transmit chunk containing %u records",
					m_producerName.c_str(), m_dataSourceName.c_str(), recordCounter);

			// Stop timer
			m_timer.stop();

			// reset the buffer, keep the name
			m_remoteFile.resetFileBuffer();
		}

		return result;
	}

	int FileAssembler::onOutgoingFile()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		m_timer.stop();

		if(!m_remoteFile.isOpen())
		{
			// assemble the new file name. NOTE: setName will set m_remoteFile as "open"
			m_remoteFile.setName( m_fileFormat->buildFileName(m_producerName) );
		}
		//else m_remoteFile is already open because this is a retry

		// Open the remote file, it tries indefinitely until one of the following condition is reached:
		// 1) remote file successfully opened
		// 2) shutdown request
		do
		{
			result = openRemoteFile();
		} while(!m_stopRequested && (((common::errorCode::ERR_NO_VALID_PEER == result) && waitValidPeerAvailable()) || (common::errorCode::ERR_NO_ERRORS != result)));

		return result;
	}

	int FileAssembler::onTransmittedFile(const uint32_t& recordCounter)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		AES_DDT_TRACE_MESSAGE("[%s@%s] committing <%u> records", m_producerName.c_str(), m_dataSourceName.c_str(), recordCounter);

		//TODO loop on commit error!!
		m_storeReader->commit(recordCounter);

		// Stop timer in order to can reschedule it later
		m_timer.stop();

		// reset the current remote file info. NOTE: reset will set m_remoteFile as "closed"
		m_remoteFile.reset();

		return result;
	}

	int FileAssembler::openRemoteFile()
	{
		//Logging
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Opening Remote File:<%s>",	m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar());
		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Opening Remote File:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar());

		refreshFormattingData();

		int result = m_sink->openRemoteFile(m_producerName, m_remoteFile.getName() );

		//Logging
		if(common::errorCode::ERR_NO_ERRORS != result)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s --> %s@%s] Cannot open remote file:<%s>. Result:<%d>",	m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar(), result);
			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] ERROR: Cannot open remote file:<%s>. Result:<%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar(), result);
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] Successfully Created remote file:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar());
		}

		return result;
	}

	int FileAssembler::closeRemoteFile()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if( m_remoteFile.isOpen() )
		{
			result = m_sink->closeRemoteFile(m_producerName, m_remoteFile.getName());

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] Closing Remote File<%s> size:<%u>, result:<%d>",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar(), m_remoteFile.getSizeInByte(), result);

			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] Closing Remote File<%s> size:<%u>, result:<%d>",
								  m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar(), m_remoteFile.getSizeInByte(), result);

			if(common::errorCode::ERR_NO_ERRORS != result)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s --> %s@%s] Failed to Close Remote File<%s> size:<%u>, result:<%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar(), m_remoteFile.getSizeInByte(), result);
				AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] ERROR: Failed to Close Remote File<%s> size:<%u>, result:<%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar(), m_remoteFile.getSizeInByte(), result);
			}
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s --> %s@%s] Close invoked on a file never opened <%s>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar());
			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] ERROR: ] Close invoked on a file never opened <%s>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_remoteFile.getNameAsChar());
		}

		return result;
	}

	void FileAssembler::refreshFormattingData()
	{
		if(m_refreshData)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler reloading OutputFormat info, old values:\n"
						"#####    dataChunkSize: <%u>\n"
						"#####    dataLengthType: <%u>\n"
						"#####    holdTime: <%u>\n"
						"#####    paddingChar: <0x%02X>\n",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
						m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());

			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler reloading OutputFormat info, old values:\n"
					"#####    dataChunkSize: <%u>\n"
					"#####    dataLengthType: <%u>\n"
					"#####    holdTime: <%u>\n"
					"#####    paddingChar: <0x%02X>\n",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
					m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());

			//Refresh Output format parameters
			m_outputFormat = workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName)->getOutputFormat();

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler reloaded OutputFormat info, new values:\n"
						"#####    dataChunkSize: <%u>\n"
						"#####    dataLengthType: <%u>\n"
						"#####    holdTime: <%u>\n"
						"#####    paddingChar: <0x%02X>\n",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
						m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());

			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler reloaded OutputFormat info, new values:\n"
					"#####    dataChunkSize: <%u>\n"
					"#####    dataLengthType: <%u>\n"
					"#####    holdTime: <%u>\n"
					"#####    paddingChar: <0x%02X>\n",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
					m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler reloading FileFormat info, old values:\n"
						"#####    fileSize: <%u>\n",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_fileFormat->getFileSize());

			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler reloading FileFormat info, old values:\n"
					"#####    fileSize: <%u>\n",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_fileFormat->getFileSize());

			// Refresh File format parameters
			m_fileFormat = workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName)->getFileFormat();

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] FileAssembler reloading FileFormat info, new values:\n"
						"#####    fileSize: <%u>\n",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_fileFormat->getFileSize());

			AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] FileAssembler reloading FileFormat info, new values:\n"
					"#####    fileSize: <%u>\n",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), m_fileFormat->getFileSize());

			m_refreshData = false;
		}
	}

	void FileAssembler::handleError(int readerError)
	{
		switch(readerError)
		{
			//case common::errorCode::ERR_CKPT_ITERATOR_FAILURE: //on saCkptSectionIterationNext (see Reader::getNextSectionToRead)
			case common::errorCode::ERR_CKPT_ITERATOR_INIT_FAILURE: // on saCkptSectionIterationInitialize
			{
				AES_DDT_LOG(LOG_LEVEL_WARN, "[%s::%s@%s] Reset my reader on error <%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), readerError);
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] WARNING: Reset my reader on error <%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), readerError);

				// RESET THE STORE READER
				m_storeReader = workingSet_t::instance()->getStoringManager().resetReader(m_dataSourceName, m_producerName);
			}
			break;

			default:
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Ignore error <%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), readerError);
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Ignore error <%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), readerError);
			}
		}

		sleepOnInternalError(common::datatransfer::ASSEMBLING_ERROR_TIMOUT_SECONDS);
	}

	bool FileAssembler::sleepOnInternalError(int seconds)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s] sleep on internal error", m_producerName.c_str(), m_dataSourceName.c_str());
		AES_DDT_TRACE_MESSAGE("[%s::%s] sleep on internal error", m_producerName.c_str(), m_dataSourceName.c_str());

		bool noStopReceived = true;

		struct pollfd fds[1];
		nfds_t nfds =1;

		ACE_Time_Value timeout;

		// Initialize the pollfd structure
		ACE_OS::memset(fds, 0 , sizeof(fds));

		fds[0].fd = m_stopEvent;
		fds[0].events = POLLIN;

		__time_t secs = seconds;
		__suseconds_t usecs = 0;
		timeout.set(secs, usecs);

		int pollresult = ACE_OS::poll(fds, nfds, &timeout);

		if( -1 == pollresult )
		{
			int pollError = errno;
			// Error on poll
			if( EINTR == pollError)
			{
				AES_DDT_TRACE_MESSAGE("[%s::%s] error on stop event poll, Interrupted System Call. error:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), ::strerror(pollError));
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s]  error on stop event poll, Interrupted System Call. error:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), ::strerror(pollError));
			}

			AES_DDT_TRACE_MESSAGE("[%s::%s] error on stop event poll. error:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), ::strerror(pollError));
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s] error on stop event poll. error:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), ::strerror(pollError));
			return noStopReceived;
		}

		if( 0 ==  pollresult)
		{
			// Timeout out happened
			AES_DDT_TRACE_MESSAGE("[%s::%s] timeour expired. Continue...", m_producerName.c_str(), m_dataSourceName.c_str());
			return noStopReceived;
		}

		// check stop event
		if( (fds[0].revents & POLLIN) )
		{
			// Received shutdown event
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s] received shutdown event", m_producerName.c_str(), m_dataSourceName.c_str());
			noStopReceived = m_running = false;
			return noStopReceived;
		}

		return noStopReceived;
	}


} /* namespace engine */

