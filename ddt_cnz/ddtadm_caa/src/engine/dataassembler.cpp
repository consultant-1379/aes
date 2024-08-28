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
#include "engine/dataassembler.h"

#include "common/programconstants.h"
#include "common/macros.h"
#include "common/utility.h"
#include "engine/context.h"

#include <boost/make_shared.hpp>
#include <boost/thread/locks.hpp>

#include <sys/eventfd.h>


#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "engine/workingset.h"
	#include "engine/datasink.h"
	#include "store/handlerinterface.h"
#else
	#include "stubs/macro_stub.h"
	#include "stubs/workingset_stub.h"
	#include "stubs/store_stub.h"
	#include "stubs/ACE_Task_Base_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_DataPusher)

namespace engine
{

	DataAssembler::DataAssembler(const std::string& dataSourceName, const std::string& producerName, const int& stopEvent, DataSink* sink)
	: m_storeReader(),
	  m_dataSourceName(dataSourceName),
	  m_producerName(producerName),
	  m_outputFormat(),
	  m_stopEvent(stopEvent),
	  m_stopRequested(false),
	  m_running(false),
	  m_sink(sink),
	  m_timer(),
	  m_changeEvent(common::event::INVALID),
	  m_validPeerEvent(common::event::INVALID),
	  m_eventMutex()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	DataAssembler::~DataAssembler()
	{
		AES_DDT_TRACE_FUNCTION;
		::close(m_changeEvent);
		::close(m_validPeerEvent);
	}

	int DataAssembler::open(void* /*args*/)
	{
		AES_DDT_TRACE_FUNCTION;

		int result = common::errorCode::ERR_NO_ERRORS;

		//Get Output format parameters
		m_outputFormat = workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName)->getOutputFormat();

		if(activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED) != common::errorCode::ERR_NO_ERRORS)
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot start svc thread");
			AES_DDT_TRACE_MESSAGE("ERROR: cannot start svc thread. errno: %d", errno);
			result = common::errorCode::ERR_SVC_ACTIVATE;
		}

		return result;
	}

	int DataAssembler::start()
	{
		AES_DDT_TRACE_FUNCTION;

		int result = common::errorCode::ERR_NO_ERRORS;

		if( thr_count() > 0U )
		{
			//ERROR CASE
			AES_DDT_LOG(LOG_LEVEL_ERROR, "thread already running. thr_count(): %zu", thr_count());
			AES_DDT_TRACE_MESSAGE("ERROR: thread already running. thr_count(): %zu", thr_count());
			result = common::errorCode::ERR_OPEN;
		}
		else
		{
			result = open();
		}

		return result;
	}

	int DataAssembler::stop()
	{
		AES_DDT_TRACE_FUNCTION;

		m_stopRequested = true;

		eventfd_t shutEventOn = 1U;
		if(eventfd_write(m_stopEvent, shutEventOn) != 0)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] FAILED to set shutdown event, fd:<%d>, error:<%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_stopEvent, errno);
			AES_DDT_TRACE_MESSAGE("[%s@%s] FAILED to set shutdown event, fd:<%d>, error:<%d>", m_producerName.c_str(), m_dataSourceName.c_str(), m_stopEvent, errno);
			// Try to force thread shutdown
			m_running = false;
		}

		// Wait for svc thread termination
		wait();
		return 0;
	}

	void DataAssembler::notifyChange()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_eventMutex);

		if (common::event::INVALID != getChangeEventHandle())
		{
			if (0 != eventfd_write(m_changeEvent, 1U))
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] FAILED to signal modifyEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_changeEvent);
				AES_DDT_TRACE_MESSAGE("[%s@%s] FAILED to signal modifyEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_changeEvent);
			}
		}
	}

	void DataAssembler::notifyValidPeer()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_eventMutex);

		if (common::event::INVALID != getValidPeerEventHandle())
		{
			if (0 != eventfd_write(m_validPeerEvent, 1U))
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] FAILED to signal validPeerEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_validPeerEvent);
				AES_DDT_TRACE_MESSAGE("[%s@%s] FAILED to signal validPeerEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_validPeerEvent);
			}
		}
	}

	int DataAssembler::getChunk(unsigned char* chunk, uint32_t& freeChunkSpace, uint32_t& recordCounter)
	{
		int readResult = common::errorCode::ERR_NO_ERRORS;
		uint32_t recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(m_dataSourceName);

		unsigned char* tmpBuffer = chunk;

		while( freeChunkSpace >= recordSize )
		{
			// READ
			uint32_t bufferLenght = recordSize;
			readResult = m_storeReader->read(tmpBuffer, bufferLenght);

			if( common::errorCode::ERR_NO_ERRORS == readResult)
			{
				// Successfully read
				freeChunkSpace -= bufferLenght;
				tmpBuffer += bufferLenght;
				recordCounter++;
			}
			else
			{
				//ERROR CASE
				AES_DDT_LOG(LOG_LEVEL_TRACE, "!!!!! STORE READ ERROR: %u !!!!!", readResult);
				AES_DDT_TRACE_MESSAGE("!!!!! STORE READ ERROR: %u !!!!!", readResult);
				break;
			}
		}

		AES_DDT_TRACE_MESSAGE("CHUNK IS %s <record counter: %u>", ((common::errorCode::ERR_NO_ERRORS == readResult) ? "FULL" : "NOT FULL"), recordCounter);

		return readResult;
	}

	uint32_t DataAssembler::formatDataChunk(unsigned char* chunk, uint32_t& freeChunkSpace)
	{
		uint32_t chunkSize = m_outputFormat->getDataChunkSize();
		uint32_t chunkOffset = chunkSize - freeChunkSpace;

		AES_DDT_TRACE_MESSAGE("Chunk Size:<%u> Bytes, Free Space:<%u> Bytes; Chunk Offset<%u>", chunkSize, freeChunkSpace, chunkOffset);

		if( (m_outputFormat->getDataLengthType() == common::FIXED) && (chunkOffset > 0U) && (freeChunkSpace > 0U) )
		{
			// Fixed Length dataChunk
			//uint8	PaddingChar { 0..255 }
			unsigned char padding = m_outputFormat->getPaddingChar();
			memset(chunk+chunkOffset, padding, freeChunkSpace);
			freeChunkSpace = 0U;
		}
		else if( (m_outputFormat->getDataLengthType() == common::EVEN) && (chunkOffset > 0U) &&
				(freeChunkSpace > 0U) && ( (chunkOffset % 2) != 0) )
		{
			// Even Length dataChunk
			unsigned char padding = m_outputFormat->getPaddingChar();
			memset(chunk+chunkOffset, padding, 1U);
			freeChunkSpace--;
		}

		// Calculate the chunk size to send
		chunkSize -= freeChunkSpace;

		AES_DDT_TRACE_MESSAGE("Chunk Size to send:<%u> Bytes, Free Space left:<%u>", chunkSize, freeChunkSpace);

		return chunkSize;
	}

	bool DataAssembler::waitNewDataAvailable()
	{
		AES_DDT_TRACE_FUNCTION;

		//AES_DDT_LOG(LOG_LEVEL_DEBUG, "\n############################################\n### [%s@%s] waiting for data...\n############################################\n", m_producerName.c_str(), m_dataSourceName.c_str());

		bool dataReady = false;

		if(m_stopRequested) return dataReady;

		nfds_t nfds = 3;
		struct pollfd fds[nfds];

		// Initialize the pollfd structure
		ACE_OS::memset(fds, 0, sizeof(fds));

		fds[stop_pos].fd = m_stopEvent;
		fds[stop_pos].events = POLLIN;

		fds[dataReady_pos].fd = m_storeReader->getReadEvent();
		fds[dataReady_pos].events = POLLIN;

		fds[holdTime_pos].fd = m_timer.getHandle();
		fds[holdTime_pos].events = POLLIN;

		do
		{
			int pollResult = ACE_OS::poll(fds, nfds, NULL);

			// poll error check
			if(pollResult < 0)
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] Error on poll!", m_producerName.c_str(), m_dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("[%s@%s] Error on poll!", m_producerName.c_str(), m_dataSourceName.c_str() );
				break;
			}

			// check for shutdown
			if( fds[stop_pos].revents & POLLIN )
			{
				AES_DDT_TRACE_MESSAGE("[%s@%s] ShutDown request!", m_producerName.c_str(), m_dataSourceName.c_str());
				break;
			}

			// check for data available
			if( fds[dataReady_pos].revents & POLLIN )
			{
				// New Data available reset event
				eventfd_t recordReady = 0U;
				eventfd_read(fds[dataReady_pos].fd, &recordReady);
				AES_DDT_TRACE_MESSAGE("[%s@%s] Data ready event received. Records available: <%lu>", m_producerName.c_str(), m_dataSourceName.c_str(), recordReady);
				dataReady = true;
				break;
			}

			// check for hold time elapsed
			if( fds[holdTime_pos].revents & POLLIN )
			{
				AES_DDT_TRACE_MESSAGE("[%s@%s] Hold Time elapsed.", m_producerName.c_str(), m_dataSourceName.c_str());
				dataReady = false;
				break;
			}

			// Check error of handle
			for(unsigned int idx = 0U; idx < nfds; ++idx)
			{
				// Compare with : Error condition or Hung up or Invalid polling request
				if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] poll on handle pos:<%d> failed, revents:<%d>",
							m_producerName.c_str(), m_dataSourceName.c_str(), idx, fds[idx].revents );
					AES_DDT_TRACE_MESSAGE("[%s@%s] poll on handle pos:<%d> failed, revents:<%d>",
							m_producerName.c_str(), m_dataSourceName.c_str(), idx, fds[idx].revents);
					return dataReady;
				}
			}

		}while(true);

		return dataReady;
	}

	bool DataAssembler::waitValidPeerAvailable()
	{
		AES_DDT_TRACE_FUNCTION;

		bool validPeerFound = false;


		if(m_stopRequested) return validPeerFound;

		nfds_t nfds = 2;
		struct pollfd fds[nfds];

		// Initialize the pollfd structure
		ACE_OS::memset(fds, 0, sizeof(fds));

		fds[stop_pos].fd = m_stopEvent;
		fds[stop_pos].events = POLLIN;

		fds[eventNotify_pos].fd = getValidPeerEventHandle();
		fds[eventNotify_pos].events = POLLIN;

		do
		{
			int pollResult = ACE_OS::poll(fds, nfds, NULL);

			// poll error check
			if(pollResult < 0)
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] Error on poll!", m_producerName.c_str(), m_dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("[%s@%s] Error on poll!", m_producerName.c_str(), m_dataSourceName.c_str() );
				break;
			}

			// check for shutdown
			if( fds[stop_pos].revents & POLLIN )
			{
				AES_DDT_TRACE_MESSAGE("[%s@%s] ShutDown request!", m_producerName.c_str(), m_dataSourceName.c_str());
				break;
			}

			// check for data available
			if( fds[eventNotify_pos].revents & POLLIN )
			{
				// New Data available reset event
				eventfd_t validPeer = 0U;
				eventfd_read(fds[eventNotify_pos].fd, &validPeer);
				AES_DDT_TRACE_MESSAGE("[%s@%s] Valid Peer event received.", m_producerName.c_str(), m_dataSourceName.c_str());
				validPeerFound = true;
				break;
			}

			// Check error of handle
			for(unsigned int idx = 0U; idx < nfds; ++idx)
			{
				// Compare with : Error condition or Hung up or Invalid polling request
				if(fds[idx].revents & (POLLERR | POLLHUP | POLLNVAL))
				{
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] poll on handle pos:<%d> failed, revents:<%d>",
							m_producerName.c_str(), m_dataSourceName.c_str(), idx, fds[idx].revents );
					AES_DDT_TRACE_MESSAGE("[%s@%s] poll on handle pos:<%d> failed, revents:<%d>",
							m_producerName.c_str(), m_dataSourceName.c_str(), idx, fds[idx].revents);
					return validPeerFound;
				}
			}

		}while(true);

		return validPeerFound;
	}

	int DataAssembler::getChangeEventHandle()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_eventMutex);

		if (common::event::INVALID == m_changeEvent)
		{
			m_changeEvent = eventfd(common::event::INITIAL_VALUE, common::event::FLAGS);

			if(common::event::INVALID == m_changeEvent)
			{
				// error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] Failed to create a read event", m_dataSourceName.c_str(), m_producerName.c_str() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s@%s] Failed to create a read event", m_dataSourceName.c_str(), m_producerName.c_str() );
			}
		}

		return m_changeEvent;
	}

	int DataAssembler::getValidPeerEventHandle()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_eventMutex);

		if (common::event::INVALID == m_validPeerEvent)
		{
			m_validPeerEvent = eventfd(common::event::INITIAL_VALUE, common::event::FLAGS);

			if(common::event::INVALID == m_validPeerEvent)
			{
				// error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] Failed to create a read event", m_dataSourceName.c_str(), m_producerName.c_str() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s@%s] Failed to create a read event", m_dataSourceName.c_str(), m_producerName.c_str() );
			}
		}

		return m_validPeerEvent;
	}




} /* namespace engine */
