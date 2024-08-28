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
#include "common/programconstants.h"
#include "common/macros.h"

#include "engine/datasource.h"

#ifndef CUTE_TEST
	#include "common/utility.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "engine/workingset.h"
	#include <sys/eventfd.h>
	#include <poll.h>
	#include "operation/creator.h"
	#include <boost/lexical_cast.hpp>
#else
	#include "stubs/creator_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_DataSource)

namespace engine
{
	namespace
	{
		std::string MTAP_DOMAIN_NAME("MTAP");
		const int RETRY_TIMEOUT_SEC = 1;
	}

	DataSource::DataSource(const std::string& name, const uint32_t& recordSize, const std::string& dataSourceDN)
	: m_messageStore(name),
	  m_recordSize(recordSize),
	  m_mtapServer(acs_dsd::SERVICE_MODE_INET_SOCKET),
	  m_dsdHandleSet(),
	  m_dataChannelMap(),
	  m_dataSourceDN(dataSourceDN),
	  m_timerId(-1),
	  m_mutex()
	{
		AES_DDT_TRACE_MESSAGE("DATA SOURCE IS GOING LIVE <%s> (rec size: %u)", m_messageStore.c_str(), m_recordSize);
		printf("DATA SOURCE IS GOING LIVE <%s> (rec size: %u)\n", m_messageStore.c_str(), m_recordSize);
	}
	DataSource::~DataSource()
	{
		AES_DDT_TRACE_FUNCTION;
		m_mtapServer.unregister();
		m_mtapServer.close();
	}


	int DataSource::open(void* /*args*/)
	{
		AES_DDT_TRACE_FUNCTION;

		int result = common::errorCode::ERR_NO_ERRORS;

		if(activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED) != common::errorCode::ERR_NO_ERRORS)
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot start svc thread");
			AES_DDT_TRACE_MESSAGE("ERROR: cannot start svc thread. errno: %d", errno);
			result = common::errorCode::ERR_SVC_ACTIVATE;
		}

		return result;
	}

	int DataSource::svc(void)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		if ((setupMtapServer() == common::errorCode::ERR_NO_ERRORS) && (registerDsdHandles() == common::errorCode::ERR_NO_ERRORS))
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] DataSource Task is up & running", m_messageStore.c_str());
			AES_DDT_TRACE_MESSAGE("[%s] DataSource Task is up & running", m_messageStore.c_str());
		}
		else
		{
			// Retry timeout
			ACE_Time_Value interval(RETRY_TIMEOUT_SEC);

			// Schedule a timer event that will expire after an <delay> amount of time
			m_timerId = engine::workingSet_t::instance()->getMainReactor().schedule_timer(this, 0, interval);
		}

		return result;
	}

	int DataSource::start()
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

	int DataSource::stop()
	{
		AES_DDT_TRACE_FUNCTION;
		// Wait for svc thread termination
		wait();

		{
			// Lock in exclusive mode
			// Allow only one thread at the time to enter this code.
			// guard is destroyed when out of scope, automatically releasing the lock.
			ACE_Guard<ACE_Thread_Mutex> guard(m_mutex);
			if (-1 != m_timerId)
			{
				engine::workingSet_t::instance()->getMainReactor().cancel_timer(m_timerId);
			}
		}

		// Check if handles have been registered to reactor
		// unregister dsd handles from reactor
		engine::workingSet_t::instance()->getMainReactor().remove_handler(m_dsdHandleSet, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);

		//Cannot accept new connections from CP: remove DSD registration
		m_mtapServer.unregister();

		//loop through m_dataChannelMap
		stopDataChannels();

		return 0;
	}

	int DataSource::remove()
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;
		stop();
		return result;
	}


	int DataSource::setupMtapServer()
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_INFO, "Starting MTAP Server '%s'", m_messageStore.c_str());

		int result = m_mtapServer.open();
		if (common::errorCode::ERR_NO_ERRORS != result)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] DSD_Server open() failed. Result: <%d>. Error: <%d>. Error Text: '%s'",
					m_messageStore.c_str(),
					result,
					m_mtapServer.last_error(),
					m_mtapServer.last_error_text());

			return common::errorCode::ERR_API_CALL;
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG,
					"[%s] Requested to DSD to allocate a port number in the range: <%d, %d>",
					m_messageStore.c_str(),
					acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MIN,
					acs_dsd::CONFIG_INET_IP4_SERVICE_PORT_MAX);
		}

		result = m_mtapServer.publish(m_messageStore.c_str(), MTAP_DOMAIN_NAME.c_str(), acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE);
		if (acs_dsd::ERR_NO_ERRORS != result)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] DSD_Server publish() failed. Result: <%d>. Error: <%d>. Error Text: '%s'",
					m_messageStore.c_str(),
					result,
					m_mtapServer.last_error(),
					m_mtapServer.last_error_text());

			m_mtapServer.close();
			return common::errorCode::ERR_API_CALL;
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG,
					"Service published on DSD: <%s, %s>",
					m_messageStore.c_str(), MTAP_DOMAIN_NAME.c_str());
		}

		return common::errorCode::ERR_NO_ERRORS;
	}

	int DataSource::registerDsdHandles()
	{
		AES_DDT_TRACE_FUNCTION;

		//Get DSD Server handles
		int dsdHandleCount = 0;

		// To get the number of handle
		int dsdResult = m_mtapServer.get_handles(NULL, dsdHandleCount);

		if(acs_dsd::ERR_NOT_ENOUGH_SPACE != dsdResult)
		{
			// Some error happens
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] Failed to get number of DSD handles. Error: %d, <%s>", m_messageStore.c_str(), dsdResult, m_mtapServer.last_error_text() );
			AES_DDT_TRACE_MESSAGE("ERROR: [%s] Failed to get number of DSD handles. Error: %d, <%s>", m_messageStore.c_str(), dsdResult, m_mtapServer.last_error_text() );

			return common::errorCode::ERR_API_CALL;
		}

		// Now dsdHandleCount has the correct number of handles to retrieve
		acs_dsd::HANDLE dsdHandles[dsdHandleCount];

		dsdResult = m_mtapServer.get_handles(dsdHandles, dsdHandleCount);
		if( acs_dsd::ERR_NO_ERRORS == dsdResult )
		{
			ACE_Handle_Set tmpDSDHandleSet;
			for(int idx=0; idx < dsdHandleCount; ++idx)
			{
				tmpDSDHandleSet.set_bit( dsdHandles[idx] );
			}

			int result = engine::workingSet_t::instance()->getMainReactor().register_handler(tmpDSDHandleSet, this, ACE_Event_Handler::ACCEPT_MASK);

			if( result < 0 )
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Failed to register DSD handles into reactor", m_messageStore.c_str());
				AES_DDT_TRACE_MESSAGE("[%s] Failed to register DSD handles into reactor. errno: <%d>", m_messageStore.c_str(), errno);
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("[%s] %d DSD handles have been registered to reactor", m_messageStore.c_str(), dsdHandleCount);

				// Copy the DSD handle for remove handle on closure
				for(int idx=0; idx < dsdHandleCount; ++idx)
				{
					m_dsdHandleSet.set_bit( dsdHandles[idx] );
				}
			}
		}
		else
		{
			m_mtapServer.unregister();
			m_mtapServer.close();
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] Call 'get_handles' failed. Result: <%d>. Error: <%d>. Error Text: '%s'",
					m_messageStore.c_str(),
					dsdResult,
					m_mtapServer.last_error(),
					m_mtapServer.last_error_text());

			return common::errorCode::ERR_API_CALL;
		}

		return common::errorCode::ERR_NO_ERRORS;
	}

	int DataSource::handle_timeout(const ACE_Time_Value&, const void* arg)
	{
		AES_DDT_TRACE_FUNCTION;

		//To avoid warning
		UNUSED(arg);

		// Lock in exclusive mode
		ACE_Guard<ACE_Thread_Mutex> guard(m_mutex);

		if ((setupMtapServer() == common::errorCode::ERR_NO_ERRORS) && (registerDsdHandles() == common::errorCode::ERR_NO_ERRORS))
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] DataSource Task is up & running", m_messageStore.c_str());
			AES_DDT_TRACE_MESSAGE("[%s] DataSource Task is up & running", m_messageStore.c_str());
			m_timerId = -1;
		}
		else
		{
			// Retry timeout
			ACE_Time_Value interval(RETRY_TIMEOUT_SEC);

			// Schedule a timer event that will expire after an <delay> amount of time
			m_timerId = engine::workingSet_t::instance()->getMainReactor().schedule_timer(this, 0, interval);
		}

		return common::errorCode::ERR_NO_ERRORS;
	}


	int DataSource::handle_close (ACE_HANDLE fd, ACE_Reactor_Mask mask)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] handle_close", m_messageStore.c_str());
		AES_DDT_TRACE_MESSAGE("[%s] handle_close", m_messageStore.c_str());

		UNUSED(fd);
		UNUSED(mask);
		return 0;
	}

	int DataSource::handle_input (ACE_HANDLE /*fd*/)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] CONNECTION REQUEST FROM CP", m_messageStore.c_str());
		AES_DDT_TRACE_MESSAGE("[%s] CONNECTION REQUEST FROM CP", m_messageStore.c_str());

		boost::shared_ptr<ACS_DSD_Session> mtap_session = boost::make_shared<ACS_DSD_Session>();
		int call_result = 0;

		if( (call_result = m_mtapServer.accept(*mtap_session.get())) )
		{
			// Error case
			// ERROR: Accepting a client connection
			AES_DDT_LOG(LOG_LEVEL_ERROR,
						"[%s] Cannot accept a new connection.\n"
						"## Call Result: <%d>, errno: <%d>\n"
						"## ERROR: Accepting a client connection request\n"
						"     Error code == %d\n"
						"     Error text == '%s'\n", m_messageStore.c_str(), call_result, errno, m_mtapServer.last_error(), m_mtapServer.last_error_text());

			//NEVER RETURN -1 THAT WILL CAUSE THE INVOKATION OF HANDLE_CLOSE
			// -1 says to the Reactor to remove this Event Handler
			return 0;
		}
		else
		{
			// Success case

			// Fetch Default CP Name
			ACS_DSD_Node remoteNode;
			int ret = mtap_session->get_remote_node(remoteNode);
			if (ret < 0)
			{
				mtap_session->close();
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] Call 'get_remote_node' failed with error code <%d>", m_messageStore.c_str(), ret);
				return 0;
			}

			boost::shared_ptr<mtap::DataChannel> mtapChannel = getMTAPChannelManager(remoteNode.system_id);
			if (!mtapChannel)
			{
				mtap_session->close();
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] Call 'getMTAPChannelManager' failed", m_messageStore.c_str());
				return 0;
			}


			//Can add the session to channel manager
			ret = mtapChannel->addSession(mtap_session);

			if (ret != common::errorCode::ERR_NO_ERRORS)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] Call 'addSession' failed, return code is %d", m_messageStore.c_str(), ret);
				mtap_session->close();
				return 0;
			}

			//LOG
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "New MTAP session added for the couple (DS: <%s>, CP ID: <%d>)", m_messageStore.c_str(), remoteNode.system_id);
		}

		return 0;
	}

	boost::shared_ptr<mtap::DataChannel> DataSource::getMTAPChannelManager(int32_t cp_system_id)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Get Data Channel for CP ID: <%d>", m_messageStore.c_str(), cp_system_id);
		boost::shared_ptr<mtap::DataChannel> channel;

		dataChannelMap_t::const_iterator it = m_dataChannelMap.find(cp_system_id);

		if (m_dataChannelMap.end() == it)
		{
			//Not found, create and add the new data channel
			channel = createDataChannel(cp_system_id);

		}
		else
		{
			//Get channel
			channel = it->second;
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Found Data Channel for CP ID: <%d>", m_messageStore.c_str(), cp_system_id);
		}

		return channel;
	}

	boost::shared_ptr<mtap::DataChannel> DataSource::createDataChannel(int32_t cp_system_id)
	{
		AES_DDT_TRACE_MESSAGE("DS:[%s] Creating DataChannel for CP ID: <%d>", m_messageStore.c_str(), cp_system_id);
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "DS:[%s] Creating DataChannel CP ID: <%d>", m_messageStore.c_str(), cp_system_id);

		boost::shared_ptr<mtap::DataChannel> channel = boost::make_shared<mtap::DataChannel>(cp_system_id, m_messageStore);

		//START THE new DATA CHANNEL
		if( common::errorCode::ERR_NO_ERRORS == channel->start() && createTransferProgressMO(channel->getDefaultCpName()))
		{
			//ADD TO THE MAP
			m_dataChannelMap.insert(dataChannelMap_t::value_type(cp_system_id, channel));
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Created new Data Channel for CP ID: <%d>", m_messageStore.c_str(), cp_system_id);
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Failed to create new Data Channel for CP ID: <%d>", m_messageStore.c_str(), cp_system_id);
			channel.reset();
		}

		return channel;
	}

	void DataSource::stopDataChannels ()
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] STOPPING <%zu> DATA CHANNELS", m_messageStore.c_str(), m_dataChannelMap.size());

		boost::shared_ptr<mtap::DataChannel> dataChannel;
		dataChannelMap_t::iterator it = m_dataChannelMap.begin();

		//Propagate the stop signal to all MTAP data channels
		while (it != m_dataChannelMap.end())
		{
			if ((dataChannel = (it++)->second))
			{
				int stop_result = dataChannel->stop();
				if (common::errorCode::ERR_NO_ERRORS != stop_result)
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR,
								"[%s] Cannot Stop Channel!!\n"
								"       ##### error: <%d>, channel id: <%d>\n", m_messageStore.c_str(), stop_result, dataChannel->getCpId());

					AES_DDT_TRACE_MESSAGE(
								"ERROR: [%s] Cannot Stop Channel!!\n"
								"       ##### error: <%d>, channel id: <%d>\n", m_messageStore.c_str(), stop_result, dataChannel->getCpId());
				}
			}
		}

		m_dataChannelMap.clear();

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] STOP REQUEST PROPAGATED", m_messageStore.c_str());
	}

	bool DataSource::createTransferProgressMO(const char* cpName)
	{
		AES_DDT_TRACE_FUNCTION;
		int status = true;

		operation::transferProgressInfo op_info;

		op_info.dataSourceName.assign(m_messageStore);
		op_info.producerName.assign(cpName);
		op_info.dataSourceDN.assign(m_dataSourceDN);

		operation::Creator factoryCreator;
		factoryCreator.schedule(operation::TRANSFERPROGRESS_ADD, &op_info);
		AES_DDT_TRACE_MESSAGE("[%s] New TransferProgress MO for CP : <%s> scheduled", m_messageStore.c_str(), cpName);

		return status;
	}
}
