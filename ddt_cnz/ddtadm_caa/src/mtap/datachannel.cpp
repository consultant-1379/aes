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
#include "mtap/datachannel.h"

#include "mtap/pduhandler.h"
#include "mtap/protocol.h"

#include "common/programconstants.h"
#include "common/tracer.h"
#include "common/logger.h"

#include <ACS_DSD_Session.h>
#include <ACS_CS_API.h>


AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_DataChannel)

namespace mtap
{
	namespace
	{
		const size_t MAX_DSD_BUFFER = 65000U;
	}

	DataChannel::DataChannel(int32_t cp_system_id, const std::string& dataSourceName)
	: m_cp_system_id (cp_system_id),
	  m_dataSourceName(dataSourceName),
	  m_CpName(),
	  m_channelInfo(),
	  m_reactorImpl(),
	  m_reactor(&m_reactorImpl),
	  m_activeSessions(),
	  m_activeSessionsMutex(),
	  m_pduHandler(this)
	{
		AES_DDT_TRACE_FUNCTION;

		char tmpStr[256] = {0};
		snprintf(tmpStr, sizeof(tmpStr) - 1, "DS:<%s> CP ID:<%d>", m_dataSourceName.c_str(), m_cp_system_id);
		m_channelInfo.assign(tmpStr);

		setDefaultCpName();
	}

	int DataChannel::addSession(boost::shared_ptr<ACS_DSD_Session> mtap_session)
	{
		AES_DDT_TRACE_FUNCTION;
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_activeSessionsMutex);

		int result = common::errorCode::ERR_MTAPSESSION_ADDING_FAILURE;
		ACE_HANDLE sessionHandle = ACE_INVALID_HANDLE;

		// Get handles
		if( isSessionSlotAvailable() &&	getSessionHandle(mtap_session, sessionHandle))
		{
			// Populate internal map first !!

			// Add handle and session info to the internal map : <Handle, <Dsd session, time> >
			m_activeSessions[sessionHandle] = std::make_pair(mtap_session, time(0));

			// Register handle to the reactor now...
			// VERY IMPORTANT:	register the handle into the reactor only after it has been added to m_activeSessions.
			//					Otherwise, if you do the contrary, the handle_input is signalled before the m_activeSessions is populated.
			if (registerHandleToReactor(sessionHandle))
			{
				result = common::errorCode::ERR_NO_ERRORS;

				//Add the incoming session to the internal active session map
				AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>] Added new session, Active Sessions are <%zu>",
						m_dataSourceName.c_str(), m_cp_system_id, m_activeSessions.size());
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP ID:<%d>] Added new session, Active Sessions are <%zu>",
						m_dataSourceName.c_str(), m_cp_system_id, m_activeSessions.size());
			}
			else
			{
				//Remove the handle from the map
				m_activeSessions.erase(sessionHandle);
			}
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>] Failed to add a new session", m_dataSourceName.c_str(), m_cp_system_id);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[DS:<%s> CP ID:<%d>] Failed to add a new session", m_dataSourceName.c_str(), m_cp_system_id);
		}

		return result;
	}

	int DataChannel::stop()
	{
		AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], Stopping reactor event loop", m_dataSourceName.c_str(), m_cp_system_id);
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP ID:<%d>], Stopping reactor event loop", m_dataSourceName.c_str(), m_cp_system_id);

		// Stop Reactor, so that a new event will be no more handled
		m_reactor.end_reactor_event_loop();

		AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], Waiting for svc thread termination", m_dataSourceName.c_str(), m_cp_system_id);
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP ID:<%d>], Waiting for svc thread termination", m_dataSourceName.c_str(), m_cp_system_id);
		// Wait for svc thread termination, so that last event is properly handled
		wait();

		AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], Graceful closure of all active sessions", m_dataSourceName.c_str(), m_cp_system_id);
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP ID:<%d>], Graceful closure of all active sessions", m_dataSourceName.c_str(), m_cp_system_id);

		{	//-----------------------------------------
			// Graceful closure of all active sessions

			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_activeSessionsMutex);
			activeMtapSessions_t::const_iterator element;

			// for each element: [< ACE_HANDLE, <DSDSession*, time> >] into the map
			for(element = m_activeSessions.begin(); m_activeSessions.end() != element; ++element)
			{
				// remove the session handler from the reactor avoiding that the handle_close hook is called
				m_reactor.remove_handler(element->first, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);

				//send stop primitive towards the active session
				unsigned char *stopBuffer = 0;
				ssize_t stopBufferLength = 0;
				m_pduHandler.getStopMessage(stopBuffer, stopBufferLength);
				printf(" --- STOP fd <%d> --- \n", element->first);
				send_data(element->first, stopBuffer, stopBufferLength);


				// close session
				element->second.first->close();
			}

			// All shared objects will be deleted
			m_activeSessions.clear();

		}

		AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], SUCCESSFULLY CLOSED", m_dataSourceName.c_str(), m_cp_system_id);
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP ID:<%d>], SUCCESSFULLY CLOSED", m_dataSourceName.c_str(), m_cp_system_id);

		return common::errorCode::ERR_NO_ERRORS;
	}

	int DataChannel::start()
	{
		AES_DDT_TRACE_MESSAGE("Starting DataChannel for DS:<%s>", m_dataSourceName.c_str());
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Starting DataChannel for DS:<%s>", m_dataSourceName.c_str());

		// initialize the pdu handler
		int result = m_pduHandler.initialize();

		if(common::errorCode::ERR_NO_ERRORS == result)
		{
			// starting the svc thread
			int result = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED);

			if( common::errorCode::ERR_NO_ERRORS != result )
			{
				// Failure occurs or Task is already an active object
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to start svc thread for DS:<%s>, result:<%d>",
						m_dataSourceName.c_str(), result);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to start svc thread for DS:<%s>, result:<%d>, errno: %d",
						m_dataSourceName.c_str(), result, errno);
				result = common::errorCode::ERR_SVC_ACTIVATE;
			}

		}

		AES_DDT_TRACE_MESSAGE("result:<%d>", result);
		return result;
	}

	int DataChannel::svc(void)
	{
		AES_DDT_TRACE_MESSAGE("DataChannel THREAD for DS:<%s> STARTED", m_dataSourceName.c_str());
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "DataChannel THREAD for DS:<%s> STARTED", m_dataSourceName.c_str());

		// Run the event loop until the ACE_Reactor::handle_events()
		// method returns -1 or the end_reactor_event_loop() method is invoked.
		m_reactor.run_reactor_event_loop();

		AES_DDT_TRACE_MESSAGE("DataChannel THREAD for DS:<%s> STOPED", m_dataSourceName.c_str());
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "DataChannel THREAD for DS:<%s> STOPED", m_dataSourceName.c_str());

		return common::errorCode::ERR_NO_ERRORS;
	}

	int DataChannel::handle_close(ACE_HANDLE fd, ACE_Reactor_Mask mask)
	{
		AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], Close Handle:<%d> mask:<%ld>", m_dataSourceName.c_str(), m_cp_system_id, fd, mask);
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_activeSessionsMutex);

		// graceful shutdown of DSD session
		m_activeSessions[fd].first->close();
		m_activeSessions.erase(fd);

		m_pduHandler.cleanSessionData(fd);
		return 0;
	}

	//------------------------------------------------------------------------
	// handle_input
	// 		Note that in case the handle_input returns -1,
	// 		the handle_close is invoked by the Reactor on the same handle.
	//		This mechanism is used here...
	//------------------------------------------------------------------------
	int DataChannel::handle_input(ACE_HANDLE fd)
	{
		AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], INPUT on handle:<%d>", m_dataSourceName.c_str(), m_cp_system_id, fd);
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_activeSessionsMutex);

		int result = -1;

		unsigned char inBuffer[MAX_DSD_BUFFER] = {0};
		ssize_t inBufferLength = m_activeSessions[fd].first->recv(inBuffer, MAX_DSD_BUFFER );

		if(inBufferLength >= acs_dsd::ERR_NO_ERRORS )
		{
			// Success Case
			unsigned char* outBuffer = 0;
			ssize_t outBufferLength = 0;

			if(m_pduHandler.handleRequest(fd, inBuffer, inBufferLength, outBuffer, outBufferLength) != protocol::ERR_MALFORMED_PACKAGE)
			{
				if(0 == outBufferLength)
				{
					AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>] NO ANSWER: HANDLING WINDOW..." , m_dataSourceName.c_str(), m_cp_system_id);
					result = 0;
				}
				else
				{
					//Answer is available
					ssize_t sentBytes = m_activeSessions[fd].first->send(outBuffer, outBufferLength);

					// deallocate the output buffer now!
					delete[] outBuffer;

					// Check send result
					result = (sentBytes == outBufferLength ? 0 : -1);

					if(result)
					{
						// Error
						AES_DDT_TRACE_MESSAGE("ERROR: [DS:<%s> CP ID:<%d>] CANNOT SEND DATA. CLOSING SOCKET!!\n"
								"##### Bytes sent/expected: <%zd> / <%zd>\n"
								"##### Last DSD Error: <%d>, <%s>",
								m_dataSourceName.c_str(), m_cp_system_id, sentBytes, outBufferLength, m_activeSessions[fd].first->last_error(), m_activeSessions[fd].first->last_error_text());

						AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR,
										  "[DS:<%s> CP ID:<%d>] CANNOT SEND DATA. CLOSING SOCKET!!\n"
										  "##### Bytes sent/expected: <%zd> / <%zd>\n"
										  "##### Last DSD Error: <%d>, <%s>",
										  m_dataSourceName.c_str(), m_cp_system_id, sentBytes, outBufferLength, m_activeSessions[fd].first->last_error(), m_activeSessions[fd].first->last_error_text());
					}
				}
			}
		}
		else
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR,
					"[DS:<%s> CP ID:<%d>] CANNOT RECEIVE DATA. CLOSING SOCKET!! Last DSD Error: <%d>, <%s>",
					m_dataSourceName.c_str(), m_cp_system_id, m_activeSessions[fd].first->last_error(), m_activeSessions[fd].first->last_error_text());

			// Error Case
			AES_DDT_TRACE_MESSAGE("ERROR: [DS:<%s> CP ID:<%d>] CANNOT RECEIVE DATA. CLOSING SOCKET!! Last DSD Error: <%d>, <%s>",
					m_dataSourceName.c_str(), m_cp_system_id, m_activeSessions[fd].first->last_error(), m_activeSessions[fd].first->last_error_text());
		}

		return result;
	}

	void DataChannel::setDefaultCpName()
	{
		ACS_CS_API_Name CPName;
		ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getDefaultCPName(m_cp_system_id, CPName);

		if( ACS_CS_API_NS::Result_Success == result)
		{
			size_t length = CPName.length();
			char tmpName[length+1];
			memset(tmpName, 0, sizeof(tmpName));

			CPName.getName(tmpName, length);
			m_CpName.assign(tmpName);
		}
		else
		{
			// Error Case
			AES_DDT_TRACE_MESSAGE("ERROR: [%s] Failed to get Default Cp Name, CS error:<%d>", m_channelInfo.c_str(), result);
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Failed to get Default Cp Name, CS error:<%d>", m_channelInfo.c_str(), result);
			// try to make it yourself
			int32_t CpBaseValue = 1000U;
			char tmpName[256] = {0};
			if(CpBaseValue > m_cp_system_id )
				snprintf(tmpName, sizeof(tmpName) - 1, "BC%d", m_cp_system_id);
			else
				snprintf(tmpName, sizeof(tmpName) - 1, "CP%d", (m_cp_system_id - CpBaseValue));

			m_CpName.assign(tmpName);
		}

		AES_DDT_TRACE_MESSAGE("[%s] Cp Name:<%s>", m_channelInfo.c_str(), m_CpName.c_str());
	}

	int DataChannel::send_data(ACE_HANDLE fd, unsigned char* outBuffer, ssize_t& outBufferLength)
	{
		AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>] send <%zu> bytes on handle:<%d>", m_dataSourceName.c_str(), m_cp_system_id, outBufferLength, fd);

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_activeSessionsMutex);

		ssize_t sentBytes = m_activeSessions[fd].first->send(outBuffer, outBufferLength);

		// deallocate the output buffer now!
		delete[] outBuffer;

		// Check send result
		int result = (sentBytes == outBufferLength ? 0 : -1);

		if(result)
		{
			// Error
			AES_DDT_TRACE_MESSAGE("ERROR: [DS:<%s> CP ID:<%d>] CANNOT SEND DATA. SOCKET TO BE CLOSED!!\n"
					"##### Bytes sent/expected: <%zu> / <%zu>\n"
					"##### Last DSD Error: <%d>, <%s>",
					m_dataSourceName.c_str(), m_cp_system_id, sentBytes, outBufferLength, m_activeSessions[fd].first->last_error(), m_activeSessions[fd].first->last_error_text());

			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR,
							  "[DS:<%s> CP ID:<%d>] CANNOT SEND DATA. SOCKET TO BE CLOSED!!\n"
							  "##### Bytes sent/expected: <%zu> / <%zu>\n"
							  "##### Last DSD Error: <%d>, <%s>",
							  m_dataSourceName.c_str(), m_cp_system_id, sentBytes, outBufferLength, m_activeSessions[fd].first->last_error(), m_activeSessions[fd].first->last_error_text());
		}
		return result;
	}

	void DataChannel::closeOtherSessions(ACE_HANDLE fd)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_activeSessionsMutex);

		for(activeMtapSessions_t::iterator element = m_activeSessions.begin(); m_activeSessions.end() != element;)
		{
			if(element->first ^ fd)
			{
				AES_DDT_TRACE_MESSAGE("[%s] (ON SYNCHRONIZE) CLOSING SOCKET ID <%d>", getInfo(), element->first);
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] (ON SYNCHRONIZE) CLOSING SOCKET ID <%d>", getInfo(), element->first);

				// remove the session handler from the reactor avoiding that the handle_close hook is called
				m_reactor.remove_handler(element->first, ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL);

				//send stop primitive towards the active session
				unsigned char *stopBuffer = 0;
				ssize_t stopBufferLength = 0;
				m_pduHandler.getStopMessage(stopBuffer, stopBufferLength);

				printf("(on Synchronize) --- STOP fd <%d> --- \n", element->first);
				send_data(element->first, stopBuffer, stopBufferLength);

				// close session
				element->second.first->close();

				m_activeSessions.erase(element++);
			}
			else
			{
				++element;
			}
		}

	}

	bool DataChannel::registerHandleToReactor(ACE_HANDLE& handle)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = true;

		int aceResult = m_reactor.register_handler(handle, this, ACE_Event_Handler::READ_MASK);

		if( aceResult < 0 )
		{
			result = false;
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[DS:<%s> CP ID:<%d>], Failed to register DSD handles<%d> into reactor",
					m_dataSourceName.c_str(), m_cp_system_id, handle);
			AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], Failed to register DSD handles<%d> into reactor",
					m_dataSourceName.c_str(), m_cp_system_id, handle);
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], DSD handle:<%d> added to reactor",
					m_dataSourceName.c_str(), m_cp_system_id, handle );
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP ID:<%d>], DSD handle:<%d> added to reactor",
					m_dataSourceName.c_str(), m_cp_system_id, handle );
		}

		return result;
	}

	bool DataChannel::getSessionHandle(const boost::shared_ptr<ACS_DSD_Session>& session, ACE_HANDLE& handle) const
	{
		AES_DDT_TRACE_FUNCTION;

		bool result = true;
		int numOfHandles = 1;

		int dsdResult = session->get_handles(&handle, numOfHandles);

		if( acs_dsd::ERR_NO_ERRORS == dsdResult )
		{
			AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], DSD handle: <%d>",	m_dataSourceName.c_str(), m_cp_system_id, handle );
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[DS:<%s> CP ID:<%d>], DSD handle: <%d>",	m_dataSourceName.c_str(), m_cp_system_id, handle );
		}
		else
		{
			// Failed to get DSD session Handle
			result = false;

			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "DS:<%s>, Failed to get DSD Session Handle for CP ID:<%d>, error:<%d>",
					m_dataSourceName.c_str(), m_cp_system_id, dsdResult);
			AES_DDT_TRACE_MESSAGE("DS:<%s>, Failed to get DSD Session Handle for CP ID:<%d>, error:<%d> ",
					m_dataSourceName.c_str(), m_cp_system_id, dsdResult);
		}

		return result;
	}

	bool DataChannel::isSessionSlotAvailable()
	{
		AES_DDT_TRACE_FUNCTION;
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_activeSessionsMutex);

		bool result = mtap::protocol::DEFAULT_NUM_OF_CONNECTIONS > m_activeSessions.size();
		if(!result)
		{
			AES_DDT_TRACE_MESSAGE("[DS:<%s> CP ID:<%d>], REACHED THE MAX NUMBER OF <%zu> SESSIONS",
					m_dataSourceName.c_str(), m_cp_system_id, m_activeSessions.size() );
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[DS:<%s> CP ID:<%d>], REACHED THE MAX NUMBER OF <%zu> SESSIONS",
					m_dataSourceName.c_str(), m_cp_system_id, m_activeSessions.size() );
		}

		return result;
	}


} /* namespace mtap */
