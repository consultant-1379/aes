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
#include "mtap/slidingwindow.h"

#include "mtap/pduhandler.h"
#include "mtap/protocol.h"
#include "mtap/putrecord.h"
#include "mtap/synchronize.h"

#ifndef CUTE_TEST
	#include "mtap/datachannel.h"
	#include "store/handlerinterface.h"
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/workingset_stub.h"
	#include "stubs/macro_stub.h"
	#include "stubs/mtap_stubs.h"
	#include "stubs/imm_stubs.h"
#endif

#include "common/programconstants.h"
#include <boost/make_shared.hpp>
#include <boost/pointer_cast.hpp>
#include <iostream>
#include <string>

AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_SlidingWindow)

namespace mtap
{

	SlidingWindow::SlidingWindow(PDUHandler* pduHandler)
	: m_pduHandler(pduHandler),
	  m_writer(),
	  m_sortedPutRecords(protocol::DEFAULT_WIN_SIZE),
	  m_baseOffsetRecNr(0)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	SlidingWindow::~SlidingWindow()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int SlidingWindow::initialize()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		std::string dataSourceName(m_pduHandler->getDataChannel()->getDataSourceName());
		std::string producerName(m_pduHandler->getDataChannel()->getDefaultCpName());

		AES_DDT_TRACE_MESSAGE("**** [DS:<%s> CP:<%s>] get writer ****", dataSourceName.c_str(), producerName.c_str() );

		// get writer from the storing manager for this specific data source and cp
		m_writer = engine::workingSet_t::instance()->getStoringManager().getWriter(dataSourceName, producerName);

		if(!m_writer)
		{
			result = common::errorCode::ERR_GET_WRITER_FAILURE;
			AES_DDT_LOG(LOG_LEVEL_ERROR, "**** [DS:<%s> CP:<%s>] ERROR: FAILED TO CREATE WRITER ****", dataSourceName.c_str(), producerName.c_str());
			AES_DDT_TRACE_MESSAGE("**** [DS:<%s> CP:<%s>] ERROR: FAILED TO CREATE WRITER ****", dataSourceName.c_str(), producerName.c_str());
		}
		else
		{
			// The number of records written represents also the next record number to be received
			int getResult = m_writer->getNumberOfWrittenRecord(m_baseOffsetRecNr);

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP:<%s>] Last Record Number:<%u>, error:<%d>",
					dataSourceName.c_str(), producerName.c_str(), m_baseOffsetRecNr, getResult);
			AES_DDT_TRACE_MESSAGE("[DS:<%s> CP:<%s>] Last Record Number:<%u>, error:<%d>",
					dataSourceName.c_str(), producerName.c_str(), m_baseOffsetRecNr, getResult);
		}

		return result;
	}

	protocol::ErrorCode SlidingWindow::add(boost::shared_ptr<PDU> pdu, ACE_HANDLE fd)
	{
		AES_DDT_TRACE_FUNCTION;
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;

		uint32_t pduRecordNumber = pdu->getRecordNumber();
		int32_t index = pduRecordNumber - m_baseOffsetRecNr;
		//printf("##### ADD PDU <%u> - Offset = <%u> #####\n", pduRecordNumber, m_baseOffsetRecNr);

		if (protocol::ERR_NO_ERROR == (result = checkRange(pduRecordNumber)))
		{
			if (m_sortedPutRecords[index].first)
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s][PDU ALREADY STORED/EXIST] Replace Record Nr: <%u>. Base Offset: <%u>",
													  m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);

				AES_DDT_TRACE_MESSAGE("[%s][PDU ALREADY STORED/EXIST] Replace Record Nr: <%u>. Base Offset: <%u>",
									  m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);
			}

			m_sortedPutRecords[index] = std::make_pair(pdu, fd);

			store();
		}

		return result;
	}

	protocol::ErrorCode SlidingWindow::unlink(bool unlinkAll)
	{
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;

		//loop through the vector
		uint32_t vectorIdx = 0U;
		for(; vectorIdx < m_sortedPutRecords.size(); ++vectorIdx)
		{
			if(m_sortedPutRecords[vectorIdx].first)
			{
				// write data to the checkpoint
				result = writeData(vectorIdx);

				//send ack
				unsigned char* outBuffer = 0;
				ssize_t outBufferSize = 0;
				m_sortedPutRecords[vectorIdx].first->createResponse(result, outBuffer, outBufferSize);

				m_pduHandler->getDataChannel()->send_data(m_sortedPutRecords[vectorIdx].second, outBuffer, outBufferSize);

				//clean entry
				m_sortedPutRecords[vectorIdx].first.reset();
				m_sortedPutRecords[vectorIdx].second = ACE_INVALID_HANDLE;
			}
		}

		AES_DDT_TRACE_MESSAGE("[%s] UNLINK DONE. Old Base Value: <%u>.", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr);

		// in case of an unlink all
		if(unlinkAll)
		{
			int rc = 0;
			for(int retrycounter = 0; (retrycounter < 10 && ((rc= m_writer->forceStoreChange()) == common::errorCode::ERR_CKPT_INFOSECTION_WRITE_FAILURE)) ; ++retrycounter)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] on MTAP UNLINK cannot close the Checkpoint! return code: %d",	m_pduHandler->getDataChannel()->getInfo(), rc);
				AES_DDT_TRACE_MESSAGE("[%s] on MTAP UNLINK cannot close the Checkpoint! return code: %d", m_pduHandler->getDataChannel()->getInfo(), rc);
				usleep(500);
			}

			if(rc != common::errorCode::ERR_NO_ERRORS)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] MEMORY FULL. FAILED TO UNLINK ALL. Checkpoint return code: %d",	m_pduHandler->getDataChannel()->getInfo(), rc);
				AES_DDT_TRACE_MESSAGE("[%s] MEMORY FULL. FAILED TO UNLINK ALL. Checkpoint return code: %d", m_pduHandler->getDataChannel()->getInfo(), rc);

				result = protocol::ERR_DISK_SPACE_EXHAUSTED;
			}

		}

		m_baseOffsetRecNr = 0U;
		return result;
	}

	protocol::ErrorCode SlidingWindow::synchronize(boost::shared_ptr<PDU> pdu, ACE_HANDLE fd)
	{
		AES_DDT_TRACE_FUNCTION;
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;

		const uint32_t recordNumber = pdu->getRecordNumber();

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] SYNCHRONIZE REQUESTED. Current Base Offset: <%u>. Synch Value: <%u>",
							  m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr, recordNumber);

		AES_DDT_TRACE_MESSAGE("[%s] SYNCHRONIZE REQUESTED. Current Base Offset: <%u>. Synch Value: <%u>",
									  m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr, recordNumber);

		std::string dataSourceName(m_pduHandler->getDataChannel()->getDataSourceName());
		std::string producerName(m_pduHandler->getDataChannel()->getDefaultCpName());

		// The number of records written represents also the next record number to be received
		if( m_writer->getNumberOfWrittenRecord(m_baseOffsetRecNr) != common::errorCode::ERR_NO_ERRORS )
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] MEMORY FULL. FAILED TO GET Current Base Offset. Synch Value: <%u>",
					m_pduHandler->getDataChannel()->getInfo(), recordNumber);

			AES_DDT_TRACE_MESSAGE("[%s] MEMORY FULL. FAILED TO GET Current Base Offset. Synch Value: <%u>",
					m_pduHandler->getDataChannel()->getInfo(), recordNumber);
			result = protocol::ERR_DISK_SPACE_EXHAUSTED;
		}
		else
		{
			// next record number to be received taken from the checkpoint
			if((0U == m_baseOffsetRecNr) && engine::workingSet_t::instance()->getStoringManager().getNumberOfStores(dataSourceName, producerName) <= 1)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] SYNCHRONIZE REQUESTED. NO INFO ABOUT PREVIOUS STATE", m_pduHandler->getDataChannel()->getInfo());

				AES_DDT_TRACE_MESSAGE("[%s] SYNCHRONIZE REQUESTED. NO INFO ABOUT PREVIOUS STATE", m_pduHandler->getDataChannel()->getInfo());

				// set the last acknowledge to the recodNumber received into the synch request
				m_baseOffsetRecNr = recordNumber;
			}
			else
			{

				AES_DDT_LOG(LOG_LEVEL_WARN, "[%s] PROCESSING SYNCHRONIZE. BASE OFFSET READ FROM CHKPT: <%u>", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr);
				AES_DDT_TRACE_MESSAGE("WARNING: [%s] PROCESSING SYNCHRONIZE. BASE OFFSET READ FROM CHKPT: <%u>", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr);
				printf("WARNING: [%s] PROCESSING SYNCHRONIZE. BASE OFFSET READ FROM CHKPT: <%u>\n", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr);

				// set the value to the last acknowledge
				--m_baseOffsetRecNr;

				// check value outside the win size
				if( (recordNumber > m_baseOffsetRecNr) && ( (recordNumber - m_baseOffsetRecNr + 1U) > protocol::DEFAULT_WIN_SIZE) )
				{
					// set the max diff to the window size
					m_baseOffsetRecNr = recordNumber - protocol::DEFAULT_WIN_SIZE + 1U;
					AES_DDT_LOG(LOG_LEVEL_WARN, "[%s] PROCESSING SYNCHRONIZE. Set the max diff to the window size! BASE OFFSET: <%u>", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr);
					AES_DDT_TRACE_MESSAGE("WARNING: [%s] PROCESSING SYNCHRONIZE. Set the max diff to the window size! BASE OFFSET: <%u>", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr);
					printf("WARNING: [%s] PROCESSING SYNCHRONIZE. Set the max diff to the window size! BASE OFFSET: <%u>\n", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr);
				}
			}

			// set the last received record number
			boost::shared_ptr<Synchronize> syncPDU = boost::dynamic_pointer_cast<Synchronize>(pdu);
			syncPDU->setLastStored(m_baseOffsetRecNr);

			// The next record to handle will be plus 1, since last acknowledge was m_baseOffsetRecNr
			m_baseOffsetRecNr++;

			// Update the writer message index to the MTAP record number
			m_writer->setMessageIndex(m_baseOffsetRecNr);
		}

		// Clean old records coming from other sessions
		for(uint32_t vectorIdx = 0; vectorIdx < m_sortedPutRecords.size(); ++vectorIdx)
		{
			if((m_sortedPutRecords[vectorIdx].first) && (m_sortedPutRecords[vectorIdx].second ^ fd))
			{
				//clean entry
				m_sortedPutRecords[vectorIdx].first.reset();
				m_sortedPutRecords[vectorIdx].second = ACE_INVALID_HANDLE;
			}
		}

		printf("[%s] SYNCHRONIZE REQUESTED. Current Base Offset: <%u>. Synch Value: <%u>\n", m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr, recordNumber);

		return result;
	}

	void SlidingWindow::clean(const ACE_HANDLE& fd)
	{
		AES_DDT_TRACE_FUNCTION;
		for(uint32_t vectorIdx = 0; vectorIdx < m_sortedPutRecords.size(); ++vectorIdx)
		{
			if((m_sortedPutRecords[vectorIdx].first) && (fd == m_sortedPutRecords[vectorIdx].second))
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] CLEAN ENTRY. Socked <%d>. Record Number: <%u>. File Number: <%u>",
							m_pduHandler->getDataChannel()->getInfo(), fd, m_sortedPutRecords[vectorIdx].first->getRecordNumber(), m_sortedPutRecords[vectorIdx].first->getFileNumber());

				//clean entry
				m_sortedPutRecords[vectorIdx].first.reset();
				m_sortedPutRecords[vectorIdx].second = ACE_INVALID_HANDLE;
			}
		}
	}

	void SlidingWindow::store()
	{
		//loop through the vector
		uint32_t vectorIdx = 0;
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;

		for(; ( vectorIdx < m_sortedPutRecords.size() && m_sortedPutRecords[vectorIdx].first ); ++vectorIdx)
		{
			// write data to the checkpoint
			result = writeData(vectorIdx);

			//send ACK back to the sender
			unsigned char* outBuffer = 0;
			ssize_t outBufferSize = 0;
			m_sortedPutRecords[vectorIdx].first->createResponse(result, outBuffer, outBufferSize);

			m_pduHandler->getDataChannel()->send_data(m_sortedPutRecords[vectorIdx].second, outBuffer, outBufferSize);

			//clean entry
			m_sortedPutRecords[vectorIdx].first.reset();
			m_sortedPutRecords[vectorIdx].second = ACE_INVALID_HANDLE;

			if(protocol::ERR_NO_ERROR != result)
			{
				// Error on write record
				AES_DDT_LOG(LOG_LEVEL_ERROR, "**** [%s] FAILURE OCCURRED IN WRITING. BASE OFFSET NOW IS:<%u>  ****",
						m_pduHandler->getDataChannel()->getInfo(), m_baseOffsetRecNr + vectorIdx);
				break;
			}
		}

		m_baseOffsetRecNr += vectorIdx;

		//rotate the vector: the 'pivot' is the first element not sent, which is the value of vectorIdx
		std::rotate(m_sortedPutRecords.begin(), m_sortedPutRecords.begin()+vectorIdx, m_sortedPutRecords.end());
	}

	protocol::ErrorCode SlidingWindow::writeData(const uint32_t& index)
	{
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;

		unsigned char* dataBuffer;
		uint32_t dataLength;

		m_sortedPutRecords[index].first->getRecordData(dataBuffer, dataLength);

		if(0U != dataLength)
		{
			//store to core mw
			if( m_writer->write(dataBuffer, dataLength) != common::errorCode::ERR_NO_ERRORS)
			{
				// write error
				result = protocol::ERR_DISK_SPACE_EXHAUSTED;
				// Empty data buffer
				AES_DDT_LOG(LOG_LEVEL_ERROR, "**** [%s] FAILED TO WRITE RECORD:<%u>  ****",
						m_pduHandler->getDataChannel()->getInfo(), m_sortedPutRecords[index].first->getRecordNumber());

				AES_DDT_TRACE_MESSAGE("ERROR: **** [%s] FAILED TO WRITE RECORD:<%u> ****",
						m_pduHandler->getDataChannel()->getInfo(), m_sortedPutRecords[index].first->getRecordNumber());
			}
		}
		else
		{
			// Empty data buffer
			AES_DDT_LOG(LOG_LEVEL_ERROR, "**** [%s] RECORD:<%u> has empty data buffer ****",
					m_pduHandler->getDataChannel()->getInfo(), m_sortedPutRecords[index].first->getRecordNumber());
			AES_DDT_TRACE_MESSAGE("ERROR: **** [%s] RECORD:<%u> has empty data buffer ****",
					m_pduHandler->getDataChannel()->getInfo(), m_sortedPutRecords[index].first->getRecordNumber());
		}

		return result;
	}

	protocol::ErrorCode SlidingWindow::checkRange(const uint32_t &pduRecordNumber) const
	{
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;
		int32_t index = pduRecordNumber - m_baseOffsetRecNr;

		if (m_baseOffsetRecNr > pduRecordNumber) //stands for if index < 0
		{
			// Record already stored in the past
			result = protocol::ERR_RECORD_ALREADY_STORED;

			AES_DDT_LOG(LOG_LEVEL_ERROR,
						"[%s] RECORD ALREADY STORED. Received: <%u>. Base Offset: <%u>",
						m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);

			AES_DDT_TRACE_MESSAGE("ERROR: [%s] RECORD ALREADY STORED. Received: <%u>. Base Offset: <%u>",
						m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);

			printf("ERROR: [%s] RECORD ALREADY STORED. Received: <%u>. Base Offset: <%u>\n",
					m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);
			//print_table("ERR_RECORD_ALREADY_STORED", pduRecordNumber);
		}
		else if(index >= m_sortedPutRecords.size())
		{
			//OutOfRange;
			result = protocol::ERR_RECORD_OUT_OF_RANGE;

			AES_DDT_LOG(LOG_LEVEL_ERROR,
						"[%s] RECORD OUT OF RANGE. Received: <%u>. Base Offset: <%u>",
						m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);

			AES_DDT_TRACE_MESSAGE("ERROR: [%s] RECORD OUT OF RANGE. Received: <%u>. Base Offset: <%u>",
								  m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);

			printf("ERROR: [%s] RECORD OUT OF RANGE. Received: <%u>. Base Offset: <%u>\n",
					  m_pduHandler->getDataChannel()->getInfo(), pduRecordNumber, m_baseOffsetRecNr);
		}

		return result;
	}

	//------------------------------
	// print_table
	//	USED FOR DEBUG ONLY
	//------------------------------
	void SlidingWindow::print_table(const char * TAG, uint32_t pduRecordNumber)
	{
		//std::cout << std::string(100, '\n');

		char out[4096] = {0};
		char * writingOut = out;
		int writeIndex = 0;


		if(TAG) writeIndex += sprintf( (writingOut + writeIndex), "%s <%u>\n", TAG, pduRecordNumber);
		writeIndex += sprintf((writingOut + writeIndex), "BASE OFFSET: %u\n", m_baseOffsetRecNr);

		for (int i = 0; i < m_sortedPutRecords.size(); ++i)
		{
			writeIndex += sprintf((writingOut + writeIndex), "[%02d]\t\t= <%u>\n", i, (m_sortedPutRecords[i].first ? m_sortedPutRecords[i].first->getRecordNumber() : 0));
		}

		printf("%s", out);
	}

} /* namespace mtap */
