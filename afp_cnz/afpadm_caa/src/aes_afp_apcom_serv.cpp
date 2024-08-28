/*=================================================================== */
/**
	@file   aes_afp_apcom_serv.cpp

	@brief

	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       08/09/2011     XNADNAR       Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_afp_apcom_hand.h>
#include <aes_afp_apcom_serv.h>
#include <aes_afp_services.h>
#include <ace/Handle_Set.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"

AES_AFP_TRACE_DEFINE(AES_AFP_apcom_serv);

namespace afp_dsd
{
	const std::string SERVERNAME("AES_AFP_server");
	const std::string DOMAINNAME("AES");

}
/*===================================================================
                        FARWARD DECLARATION SECTION
=================================================================== */
class aes_afp_apcom_hand;
ACE_Recursive_Thread_Mutex aes_afp_apcom_serv::afpMutex;

/*===================================================================
					   ROUTINE: CONSTRUCTOR
=================================================================== */
aes_afp_apcom_serv::aes_afp_apcom_serv ()
 :running_(true),
  m_DSDServer(NULL), // HY27277 
  convSession_(NULL)
{

}
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
aes_afp_apcom_serv::~aes_afp_apcom_serv()
{
	this->wait();
	if (m_DSDServer != NULL)  // HY27277 
        {
                delete m_DSDServer;
                m_DSDServer = NULL;
        }

}

/*===================================================================
                    ROUTINE: open
=================================================================== */
int aes_afp_apcom_serv::open (ACE_Thread_Manager* thrMgr)
{
	AES_AFP_TRACE_MESSAGE( "Entering");
	this->thr_mgr(thrMgr);
	AES_AFP_TRACE_MESSAGE( "Leaving");
	return this->activate(THR_NEW_LWP | THR_JOINABLE);
}

bool aes_afp_apcom_serv::publishToDSD()  // HY27277 
{
	AES_AFP_TRACE_MESSAGE( "Entering");
	bool result = false;

	m_DSDServer = new (std::nothrow) ACS_DSD_Server(acs_dsd::SERVICE_MODE_INET_SOCKET);
	
	if (m_DSDServer != NULL)
	{
		if (m_DSDServer->open() !=  acs_dsd::ERR_NO_ERRORS)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed  DSD server open(), errorMsg:<%s>, errno:<%d>", m_DSDServer->last_error_text(), m_DSDServer->last_error());
			AES_AFP_TRACE_MESSAGE("ERROR: Failed  DSD server open(), errorMsg:<%s>, errno:<%d>", m_DSDServer->last_error_text(), m_DSDServer->last_error());
			delete m_DSDServer;
			m_DSDServer = NULL;
		}
		else
		{
			if (m_DSDServer->publish(afp_dsd::SERVERNAME, afp_dsd::DOMAINNAME) ==  acs_dsd::ERR_NO_ERRORS)
			{
				result = true;
				AES_AFP_LOG(LOG_LEVEL_INFO, "########### Publish to DSD successful");
				AES_AFP_TRACE_MESSAGE("########### Publish to DSD successful");
			}
			else
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to publish to DSD, errno:<%d>, errorMsg:<%s>", m_DSDServer->last_error(), m_DSDServer->last_error_text());
				AES_AFP_TRACE_MESSAGE( "Failed to publish to DSD, errno:<%d>, errorMsg:<%s>", m_DSDServer->last_error(), m_DSDServer->last_error_text());
				//Close the previous handle.
				m_DSDServer->close();
				delete m_DSDServer;
				m_DSDServer = NULL;
			}
		}
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to create the ACS_DSD_Server instance");
		AES_AFP_TRACE_MESSAGE( "Unable to create the ACS_DSD_Server instance");
	}
	
	AES_AFP_TRACE_MESSAGE( "Leaving");
	return result;
}


void aes_afp_apcom_serv::unpublishToDSD()  // HY27277
{
	AES_AFP_TRACE_MESSAGE( "Entering");

	if (m_DSDServer != NULL)
	{
		if (m_DSDServer->unregister() !=  acs_dsd::ERR_NO_ERRORS)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to unregister to DSD, errno:<%d>, errorMsg:<%s>", m_DSDServer->last_error(), m_DSDServer->last_error_text());
			AES_AFP_TRACE_MESSAGE( "Failed to unregister to DSD, errno:<%d>, errorMsg:<%s>", m_DSDServer->last_error(), m_DSDServer->last_error_text());
		}

		m_DSDServer->close();
	}

	AES_AFP_TRACE_MESSAGE( "Leaving");
}
/*===================================================================
                    ROUTINE: close
=================================================================== */
int aes_afp_apcom_serv::close (unsigned long flags)
{
	AES_AFP_TRACE_MESSAGE("flags:%lu",flags);
	running_ = false;
	sendHangUpToHand();
	unpublishToDSD();
	return 0;
}

/*===================================================================
                    ROUTINE: svc
=================================================================== */
int aes_afp_apcom_serv::svc (void )
{
	AES_AFP_TRACE_MESSAGE("Entering");
	int result = -1;
	int noOfHandles = 4;
	int handles[4];
	aes_afp_msgblock *mess_b = 0;
	ACE_Message_Block *ace_mb = 0;

	// Publish towards DSD server
	while(!publishToDSD() && !aes_afp_services::afpStopEvt)
	{
		// to avoid race condition
		sleep(1);
	}

	// Handling DSD connections
	while (1)
	{

		if( aes_afp_services::afpStopEvt )
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "stop flag is true, exit the loop ");
			AES_AFP_TRACE_MESSAGE("stop flag is true, exit the loop ");
			return 0;
		}

		if (m_DSDServer == NULL) // HY27277 
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "ACS_DSD_Server instance not found, exit the loop ");
			AES_AFP_TRACE_MESSAGE("ACS_DSD_Server instance not found, exit the loop ");
			return -1;
		}

		result = m_DSDServer->get_handles(handles,noOfHandles);  // HY27277

		if ((result < 0 ) || (noOfHandles <= 0))
		{
			Event::report(EVENT_STARTAPCOM, "SERVICE START FAULT", "API init fault", "DSD get handle failed");

			return -1;
		}

		ACE_Handle_Set dsdHandleSet;

		for( int i = 0 ;  i < noOfHandles ; i++)
		{
			dsdHandleSet.set_bit(handles[i]);
		}
		ACE_Time_Value seltv(2,0);

		int res = ACE::select(dsdHandleSet.max_set() + 1, &dsdHandleSet, 0, 0,&seltv);

		if (res <= 0)
		{
			if (res < 0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "select system call failed , errno : %d",ACE_OS::last_error());
				AES_AFP_TRACE_MESSAGE("select system call failed , errno : %d",ACE_OS::last_error());
				ACE_OS::sleep(1);
				handles[0] = 0;

			}
			else	// WAIT_TIMEOUT
			{
				// don't do nothing...
			}

			ACE_Time_Value_T<ACE_Monotonic_Time_Policy> tv (this->gettimeofday ());
			tv += ACE_Time_Value (1,0); // Now + 1 sec
			// check task's queue
			int qres = this->getq(ace_mb, &tv);	// 1 sec timeout, but will not wait??!

			if (qres != -1)
			{
				AES_AFP_TRACE_MESSAGE("getq returned not equal to -1");
				mess_b = (aes_afp_msgblock*)ace_mb;
				switch( mess_b->getMsgType() )
				{
				case HANGUP:
				{
					AES_AFP_LOG(LOG_LEVEL_INFO, "HANGUP recieved");
					AES_AFP_TRACE_MESSAGE("HANGUP recieved");
					ace_mb->release();
					return 0;
				}

				default:
				{
					AES_AFP_LOG(LOG_LEVEL_INFO, "Unknown msg type = %d", mess_b->getMsgType() );
					AES_AFP_TRACE_MESSAGE("Unknown msg type = %d", mess_b->getMsgType() );
					ace_mb->release();
					continue;
					break;
				}
				}
			}
			continue;
		}

		AES_AFP_TRACE_MESSAGE("Create new api handler");
		convSession_ = new ACS_DSD_Session;
		result = m_DSDServer->accept(*convSession_);  // HY27277

		if (result < 0)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "ConvService->accept failed, error = <%d>, error text:<%s>",
					m_DSDServer->last_error(), m_DSDServer->last_error_text() );
			AES_AFP_TRACE_MESSAGE("ConvService->accept failed, error = <%d>, error text: <%s>",
					m_DSDServer->last_error(), m_DSDServer->last_error_text() );
			continue;
		}

		aes_afp_apcom_serv::afpMutex.acquire();

		// create new handler that will take care of the new connection
		aes_afp_apcom_hand* tempHand = new aes_afp_apcom_hand( this);
		char stname[32];
		ACE_OS::sprintf(stname, "HAND%p", tempHand);

		handPair_.first = stname;
		handPair_.second = tempHand;
		handMap_.insert(handPair_);

		// try to start the service thread of the new handler
		res = tempHand->open(convSession_, this->thr_mgr());
		if (res != 0)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "tempHand->open failed");
			AES_AFP_TRACE_MESSAGE("tempHand->open failed");
			// safe to delete object since it didn't start anyway...
			delete tempHand;

			// remove from map
			std::map<std::string, aes_afp_apcom_hand*>::iterator itr;
			itr = handMap_.find(stname);
			if (itr == handMap_.end() )
			{
				// could not find the new handler in map!
				AES_AFP_LOG(LOG_LEVEL_INFO, "Could not find faulty handler in map, %s", stname);
				AES_AFP_TRACE_MESSAGE("Could not find faulty handler in map, %s", stname);
			}
			else
			{
				handMap_.erase(itr);
			}
		}
		aes_afp_apcom_serv::afpMutex.release();
	}

	return 0;
}
void aes_afp_apcom_serv::sendHangUpToHand()
{
	AES_AFP_TRACE_MESSAGE( "Entering");
	ACE_Message_Block* A_msgBlock;
	aes_afp_msgblock* msgBlock;
	std::map<std::string, aes_afp_apcom_hand*>::iterator itr;

	aes_afp_apcom_serv::afpMutex.acquire();
	itr = handMap_.begin();
	AES_AFP_LOG(LOG_LEVEL_INFO, "Sending HANGUP to all apcom handlers..");
	AES_AFP_TRACE_MESSAGE("Sending HANGUP to all apcom handlers..");
	while (itr != handMap_.end())	// loop through all elements
	{
		msgBlock = new aes_afp_msgblock(0,0);
		msgBlock->setMsgType(HANGUP);
		A_msgBlock = msgBlock;
		(*itr).second->putq(A_msgBlock);	// tell that object to close down
		++itr;
	}
	//handMap_.clear();
	aes_afp_apcom_serv::afpMutex.release();
	AES_AFP_TRACE_MESSAGE("Leaving");
}
