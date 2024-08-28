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

#include "engine/blockassembler.h"

#include "engine/context.h"
#include "engine/workingset.h"
#include "engine/datasink.h"

#include "common/programconstants.h"
#include "common/macros.h"
#include "common/utility.h"
#include "common/tracer.h"
#include "common/logger.h"
#include "store/handlerinterface.h"

#include <sys/eventfd.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_BlockAssembler)

namespace engine
{

	BlockAssembler::BlockAssembler(const std::string& dataSourceName, const std::string& producerName, const int& stopEvent, DataSink* sink)
	: DataAssembler(dataSourceName, producerName, stopEvent, sink)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	BlockAssembler::~BlockAssembler()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int BlockAssembler::svc()
	{
		AES_DDT_TRACE_FUNCTION;
		m_running = true;
		int result = common::errorCode::ERR_NO_ERRORS;

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] BlockAssembler Task is up & running", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
		AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] BlockAssembler Task is up & running", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
		printf("[%s --> %s@%s] BlockAssembler Task is up & running\n", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());

		// GET THE STORE READER
		m_storeReader = workingSet_t::instance()->getStoringManager().getReader(m_dataSourceName, m_producerName);

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] Connected to the store reader: %s", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), (m_storeReader ? "YES" : "NO"));
		AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] Connected to the store reader: %s", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), (m_storeReader ? "YES" : "NO"));

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

		while(m_running)
		{

			// Check for shutdown event, timeout is zero in order to causes poll() to return immediately,
			// even if no events are ready.
			int pollresult = ACE_OS::poll(fds, nfds, &timeout);

			// check poll result
			if(pollresult > 0)
			{
				// check for shutdown
				if( fds[stop_pos].revents & POLLIN )
				{
					// Received shutdown event
					m_running = false;
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] BlockAssembler receives shutdown event", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] BlockAssembler receives shutdown event", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
					break;
				}

				// check for updated parameter
				if( fds[eventNotify_pos].revents & POLLIN )
				{
					// Parameters changed reset event
					eventfd_t readEvent = 0U;
					eventfd_read(fds[eventNotify_pos].fd, &readEvent);

					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] BlockAssembler reloading OutputFormat info, old values:\n"
							"#####    dataChunkSize: <%u>\n"
							"#####    dataLengthType: <%u>\n"
							"#####    holdTime: <%u>\n"
							"#####    paddingChar: <0x%02X>\n",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
							m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());

					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] BlockAssembler reloading OutputFormat info, old values:\n"
							"#####    dataChunkSize: <%u>\n"
							"#####    dataLengthType: <%u>\n"
							"#####    holdTime: <%u>\n"
							"#####    paddingChar: <0x%02X>\n",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
							m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());

					//Refresh Output format parameters
					m_outputFormat = workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName)->getOutputFormat();

					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] BlockAssembler reloaded OutputFormat info, new values:\n"
							"#####    dataChunkSize: <%u>\n"
							"#####    dataLengthType: <%u>\n"
							"#####    holdTime: <%u>\n"
							"#####    paddingChar: <0x%02X>\n",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
							m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());

					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] BlockAssembler reloaded OutputFormat info, new values:\n"
							"#####    dataChunkSize: <%u>\n"
							"#####    dataLengthType: <%u>\n"
							"#####    holdTime: <%u>\n"
							"#####    paddingChar: <0x%02X>\n",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(),
							m_outputFormat->getDataChunkSize(), m_outputFormat->getDataLengthType(), m_outputFormat->getHoldTimeSeconds(), m_outputFormat->getPaddingChar());
				}
			}

			uint32_t chunkSize = m_outputFormat->getDataChunkSize();
			uint32_t freeChunkSpace = chunkSize;
			uint32_t recordCounter = NO_RECORD;
			unsigned char chunk[chunkSize];
			memset(chunk, 0, chunkSize);

			// Get Chunk tries to return the entire chunk available in the checkpoint
			// NOTE: one chunk contains many records
			int getResult = getChunk(chunk, freeChunkSpace, recordCounter);

			//Get Chunk failed: unable to read a full chunk
			if( (common::errorCode::ERR_NO_ERRORS != getResult) &&
					(common::errorCode::ERR_CKPT_SECTIONS_EMPTY != getResult) &&
					(common::errorCode::ERR_CKPT_WAIT_COMMIT != getResult))
			{
				// iterator error wait and try again
				if(common::errorCode::ERR_CKPT_NOSECTIONS_TRYAGAIN != getResult )
				{
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] Error on read:<%d>, ready records:<%u>",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), getResult, recordCounter);
					ACE_Time_Value waitBeforeRetry(0, ERROR_MAX_WAIT_TIME_MICROSEC);
					ACE_OS::poll(fds, nfds, &waitBeforeRetry);
					continue;
				}

				// No more data available, wait for new data or for holdTime to elapse
				int timerDurationSec = (recordCounter > NO_RECORD) ? m_outputFormat->getHoldTimeSeconds() : NODATA_MAX_WAIT_TIME_SEC;
				m_timer.start(timerDurationSec);

				AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] wait data with time-out:<%d>, ready records:<%u>",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName(), timerDurationSec, recordCounter);

				// wait for new data or time elapsed
				bool newDataToRead = waitNewDataAvailable();

				if(NO_RECORD == recordCounter)
				{
					// New data received or wait time elapsed.
					// In both cases reset the timer and try to read again since no record has been previously read.
					m_timer.stop();
					continue;
				}

				// New data received, read again
				if(newDataToRead)
				{
					continue;
				}
			}

			//Data are ready to send
			m_timer.stop();
			chunkSize = formatDataChunk(chunk, freeChunkSpace);

			// Transmit failed because no valid peer was found. Wait to be notified whenever a new valid peer is available
			if( common::errorCode::ERR_NO_VALID_PEER == transmitChunk(chunk, chunkSize, recordCounter) )
			{
				waitValidPeerAvailable();
			}

		}

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] DataPusher Task stopped", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());
		AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] DataPusher Task stopped", m_producerName.c_str(), m_dataSourceName.c_str(), m_outputFormat->getDataSinkName());

		return result;
	}

	int BlockAssembler::transmitChunk(unsigned char* chunk, const uint32_t& chunkSize, const uint32_t& recordCounter)
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		result = m_sink->pushData(chunk, chunkSize, m_producerName.c_str());

		if (common::errorCode::ERR_NO_ERRORS == result)
		{
			AES_DDT_TRACE_MESSAGE("[%s@%s] committing <%u> records", m_producerName.c_str(), m_dataSourceName.c_str(), recordCounter);

			//TODO loop on commit error!!
			m_storeReader->commit(recordCounter);
		}
		else
		{
			// no records sent, reset reader position to the first read record
			m_storeReader->commit(NO_RECORD);

			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] Cannot transmit chunk containing %u records",
					m_producerName.c_str(), m_dataSourceName.c_str(), recordCounter);

			AES_DDT_TRACE_MESSAGE("[%s@%s] ERROR: Cannot transmit chunk containing %u records",
					m_producerName.c_str(), m_dataSourceName.c_str(), recordCounter);
		}

		return result;
	}

} /* namespace engine */
