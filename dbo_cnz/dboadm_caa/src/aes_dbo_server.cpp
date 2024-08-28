//******************************************************************************
//
// NAME
//      aes_dbo_server.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//      <General description of the class>

// DOCUMENT NO
//      19089-CAA 109 0423

// AUTHOR 
//      1999-12-01 by UAB/I/LN  Urban Sderberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010530  qabhall Changed due to new requirements INGO3 GOH.
//			011123	qabulfg	Declaration of Handle removed
//          020320  qabhaak Abrupt termination
//			020402	qabulfg cease all dfo alarm when closing the server

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include "aes_dbo_server.h"
#include <aes_dbo_daemon.h>
#include "aes_dbo_clientsession.h"
#include "aes_dbo_alarmdatabase.h"
#include "aes_dbo_tqdatabase.h"
#include "aes_dbo_opentqsessions.h"
namespace DSDConfig{
        const int DSDSuccess = 0;
}
#include "aes_dbo_macros.h"
#include "CommandCode.h"

#include <aes_gcc_tracer.h>


// Declaration Section
aes_dbo_daemon * dboManager_ = 0;
ACE_Event * aes_dbo_server::StopEvent;
bool aes_dbo_server::isStopEventSignalled = false;


//TRA object
AES_GCC_TRACE_DEFINE(AES_DBO_server);

//******************************************************************************
// Constructor
aes_dbo_server::aes_dbo_server():
m_poBlockTransferMCmdHandler(0),
m_poBlockTransferQueueCmdHandler(0),
m_serverOnLine(false),
m_DSDInitiate(false),
_reactor(0),
m_ShutDown(eventfd(0,0))
{
	AES_GCC_TRACE_MESSAGE("Creating DBO Server");

	// Instance a Reactor to handle DSD events
	ACE_TP_Reactor* tp_reactor_impl = new ACE_TP_Reactor();

	// the reactor will delete the implementation on destruction
	_reactor = new ACE_Reactor(tp_reactor_impl, true);

	// Initialize the ACE_Reactor
	_reactor->open(1);

	StopEvent = new (std::nothrow) ACE_Event();
}

aes_dbo_server::~aes_dbo_server()
{
	AES_GCC_TRACE_MESSAGE("Destroying DBO Server");
	delete StopEvent;
	delete _reactor;
	ACE_OS::close(m_ShutDown);
}

int aes_dbo_server::run_reactor_service()
{
	AES_GCC_TRACE_MESSAGE("In");

	while(!aes_dbo_server::isStopEventSignalled && !m_DSDInitiate)
	{
		if( initDSDServer() && registerDSDHandles() )
		{
			// set the flag to exit by the while loop;
			m_DSDInitiate = true;
		}
		else
		{
			AES_GCC_TRACE_MESSAGE("Error on initDSDServer: retry after 1 sec");
			stopDSDServer();
			// wait 1 sec. between each retry
			ACE_OS::sleep(1);
		}
	}
	if (_reactor && !aes_dbo_server::isStopEventSignalled)
	{
		AES_GCC_TRACE_MESSAGE("Waiting on run_reactor_event_loop...");

		int result = _reactor->run_reactor_event_loop();
		if (result < 0)
			AES_GCC_TRACE_MESSAGE(" ERROR : run_reactor_event_loop failed! result = %d", result);
		else
		{
			AES_GCC_TRACE_MESSAGE("Signaled end_reactor_reactor_loop - result = %d", result);
		}
	}
	else
	{ // ERROR: no reactor inside
		AES_GCC_TRACE_MESSAGE(" ERROR : No reactor inside");
	}
	return 0;
}

int aes_dbo_server::stop_reactor_service()
{
	AES_GCC_TRACE_MESSAGE("In");
	removeDSDHandles();
	_reactor->end_reactor_event_loop();
	usleep(500 * 1000);
	AES_GCC_TRACE_MESSAGE("Out");
	return 0;
}

int aes_dbo_server::start_service ()
{
	AES_GCC_TRACE_MESSAGE("Starting Services...");
	if(!dboManager_)
	{
		//LOG ERR
		return -1;
	}
	dboManager_->start_activity();

	//Start the OI threads for BlockTransferManager and BlockTransferQueue class.
	if( StartOIThreads() == ACS_CC_FAILURE )
	{
		AES_GCC_TRACE_MESSAGE("Error occured while starting the OI Threads on the active node");
		return -1;
	}
	AES_GCC_TRACE_MESSAGE("Start Service completed!");
	return 0;
}


//******************************************************************************
// Method open
ACE_INT32 aes_dbo_server::open(int /*argc*/, char **/*argv[]*/)
{

	AES_GCC_TRACE_MESSAGE("Entering aes_dbo_server::open ");

	signal(SIGPIPE, SIG_IGN);

	if( omHandler.Init() == ACS_CC_FAILURE )
	{
		AES_GCC_TRACE_MESSAGE("OmHandler initialize failed.");
		return -1;
    }

	//Check the blockTransferManagerId=1 object in IMM.
	if( checkBlockTransferObjInIMM() == false )
	{
		AES_GCC_TRACE_MESSAGE("Block Transfer Object is not retreived from IMM");
		return -1;
	}

	dboManager_ = new (std::nothrow) aes_dbo_daemon("AES_DBO_server", DBO_DEFAULT_THR);
	if(!dboManager_)
	{
		//LOG ERR
		AES_GCC_TRACE_MESSAGE("FATAL - dboManager_ memory not allocated!");
		return -1;
	}

	// Initialize AES_DBO_DataBase
	AES_GCC_TRACE_MESSAGE("Calling RecoverTQ");
	if (recoverTQs() < -1)
	{
		AES_GCC_TRACE_MESSAGE("Database initialization failure");
		return -2;  // Error, bail out
	}
	AES_GCC_TRACE_MESSAGE("RecoverTQ done!");

	return 0;
}


//******************************************************************************
// Method close
ACE_INT32 aes_dbo_server::close()
{
	AES_GCC_TRACE_MESSAGE("Entering");

	StopOIThreads();

	AES_GCC_TRACE_MESSAGE("Stopping DBO Manager activities!");
	dboManager_->stop_activity(true);
	AES_GCC_TRACE_MESSAGE("DBO Manager activities stopped!");

	stop_reactor_service();

	eventfd_t stopEvent = 1U;
	eventfd_write(m_ShutDown, stopEvent);

	// Cease all DBO alarms
	AES_GCC_TRACE_MESSAGE("Ceasing all alarms!");
	aes_dbo_alarmdatabase::instance()->ceaseAll();
	AES_GCC_TRACE_MESSAGE("Alarms ceased!");

	//Erase the contents of the alarm list_
	aes_dbo_alarmdatabase::instance()->clearAlarmListAtShutdown();

	aes_dbo_opentqsessions::instance()->terminateAll();

	//Erase the contents of the transfer queue list_
	aes_dbo_tqdatabase::instance()->clearTQListAtShutdown();

	delete dboManager_;
	dboManager_ = 0;

	omHandler.Finalize();

	AES_GCC_TRACE_MESSAGE("Leaving");
	return 0;
}


int aes_dbo_server::handle_input(ACE_HANDLE fd)
{
     UNUSED(fd);
     aes_dbo_clientsession* client = new (std::nothrow) aes_dbo_clientsession(m_ShutDown); 
     if (NULL != client)
     {
        if( m_CmdServerAcceptor.accept(client->getStream()) == 0 )
        {
             client->open();
        }
        else
        {
            delete client;
        }
     }
     return 0; 
}


/*===================================================================
 *    ROUTINE:StartOIThreads
 =================================================================== */
ACS_CC_ReturnType aes_dbo_server::StartOIThreads()
{
  AES_GCC_TRACE_MESSAGE("Entering aes_dbo_server::StartOIThreads.");

	//Start the OI thread at BlockTransferM
	m_poBlockTransferMCmdHandler = new(std::nothrow) AES_DBO_BlockTransferMCmdHandler
											(AES_DBO_BLOCK_TRANSFERM_CLASS_NM,
											AES_DBO_BLOCK_TRANSFERM_IMPLEMENTER,
											ACS_APGCC_ONE);
	if( m_poBlockTransferMCmdHandler == 0 )
	{
		AES_GCC_TRACE_MESSAGE("Error occured while allocating memory for Block TransferM Handler");
		AES_GCC_TRACE_MESSAGE(" Leaving aes_dbo_server::StartOIThreads.");
		return ACS_CC_FAILURE;
	}

	if( m_poBlockTransferMCmdHandler->setupBlockTransferMOIThread( m_poBlockTransferMCmdHandler, dboManager_->_main_reactor) == ACS_CC_FAILURE )
	{
			AES_GCC_TRACE_MESSAGE("Error occured while starting the OI Thread for handling Block TransferM");
			AES_GCC_TRACE_MESSAGE(" Leaving aes_dbo_server::StartOIThreads.");
			return ACS_CC_FAILURE;
	}


	//Start the OI thread for BlockTransferQueue class.

	m_poBlockTransferQueueCmdHandler = new(std::nothrow) AES_DBO_BlockTransferQueueCmdHandler
										 ( AES_DBO_BLOCK_TRANSFERQ_CLASS_NM,
										   AES_DBO_BLOCK_TRANSFERQ_IMPLEMENTER,
                                            ACS_APGCC_ONE);
	if( m_poBlockTransferQueueCmdHandler == 0 )
	{
		AES_GCC_TRACE_MESSAGE("Error occured while allocating memory for Block TransferQueue Handler");
		AES_GCC_TRACE_MESSAGE(" Leaving aes_dbo_server::StartOIThreads.");
        return ACS_CC_FAILURE;
    }

	if( m_poBlockTransferQueueCmdHandler->setupBlockTransferQueueOIThread( m_poBlockTransferQueueCmdHandler,  dboManager_->_main_reactor) == ACS_CC_FAILURE )
    {
            AES_GCC_TRACE_MESSAGE("Error occured while starting the OI Thread for handling Block TransferQueue ");
			AES_GCC_TRACE_MESSAGE(" Leaving aes_dbo_server::StartOIThreads.");
            return ACS_CC_FAILURE;
    }
	AES_GCC_TRACE_MESSAGE(" Leaving aes_dbo_server::StartOIThreads.");
	return ACS_CC_SUCCESS;
}


/*===================================================================
 *    ROUTINE:StopOIThreads
 *    =================================================================== */
ACS_CC_ReturnType aes_dbo_server::StopOIThreads()
{
	AES_GCC_TRACE_MESSAGE("Entering");

	if( m_poBlockTransferMCmdHandler != 0 )
	{
		m_poBlockTransferMCmdHandler->shutdown();
	}
	if( m_poBlockTransferQueueCmdHandler != 0 )
	{
		m_poBlockTransferQueueCmdHandler->shutdown();
	}

	delete m_poBlockTransferMCmdHandler;
	m_poBlockTransferMCmdHandler = 0;

	delete m_poBlockTransferQueueCmdHandler;
	m_poBlockTransferQueueCmdHandler = 0;

	AES_GCC_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;
}


/*===================================================================
        ROUTINE: checkBlockTransferObjInIMM
=================================================================== */
bool aes_dbo_server::checkBlockTransferObjInIMM()
{
	OmHandler omhandler;
	ACE_Time_Value tv(0, 10000);
	AES_GCC_TRACE_MESSAGE("Trying to initialize OmHandler");
	for (  ; ( omhandler.Init() == ACS_CC_FAILURE)  ; )
	{
		ACE_INT32 ret = StopEvent->wait(&tv, 0);
		if (ret < 0)
		{
			int errnum = ACE_OS::last_error();
			if( errnum == 62 )
			{
				continue;
			}
			return false;
		}
		else if(ret == 0)
		{
			AES_GCC_TRACE_MESSAGE("Stop Event caught. Service stopped");
			AES_GCC_TRACE_MESSAGE("Leaving checkBlockTransferObjInIMM().");
			return false;
		}

	}
	AES_GCC_TRACE_MESSAGE("OmHandler initialized successfully. Trying to fetch the file tranfer object from IMM");
	for ( ; (AES_GCC_Util::fetchDnOfBlockTransferObjFromIMM(omhandler)) == -1 ; )
	{
		ACE_INT32 ret = StopEvent->wait(&tv, 0);
		if (ret < 0)
		{
			int errnum = ACE_OS::last_error();
			if( errnum == 62 )
			{
				continue;
			}
			omhandler.Finalize();
			return false;
		}
		else if(ret == 0)
		{
			AES_GCC_TRACE_MESSAGE("Stop Event caught. Service stopped");
			AES_GCC_TRACE_MESSAGE("Leaving checkBlockTransferObjInIMM().");
			omhandler.Finalize();
			return false;
		}
	}
	omhandler.Finalize();
	AES_GCC_TRACE_MESSAGE("Fetched the block tranfer object from IMM");
	return true;
}

/*===================================================================
        ROUTINE: recoverTQs
=================================================================== */

ACE_INT32 aes_dbo_server::recoverTQs()
{
	string tqName;
	std::vector<std::string> tqDnList;
	std::vector<std::string>::iterator tqDnListItr;
	string myClassName;
	OmHandler myOmHandler;

	if(myOmHandler.Init() == ACS_CC_FAILURE)
	{
		AES_GCC_TRACE_MESSAGE("recoverTQs Error occurred while initializing OmHandler");
		AES_GCC_TRACE_MESSAGE("recoverTQs Leaving aes_dbo_server::recoverTQs");
		return -1;
	}
	AES_GCC_TRACE_MESSAGE("Init ok!");
	myClassName = AES_DBO_BLOCK_TRANSFERQ_CLASS_NM;
	
	myOmHandler.getClassInstances(myClassName.c_str(),tqDnList);
	
	AES_GCC_TRACE_MESSAGE("aes_dbo_server::recoverTQs Instances found for %s = %zd", myClassName.c_str(), tqDnList.size());

	for(tqDnListItr = tqDnList.begin(); tqDnListItr != tqDnList.end(); ++tqDnListItr)
	{
		string myTqName;
		string myTqNameDn;
		string myDestSetName;
		string rdn = (*tqDnListItr);
		ACE_INT32 removeDelay = -1;
		bool mirrored(false);
		//Invoke getObject for dn and retrieve all the parameters.
		ACS_APGCC_ImmObject myTqObj;
		myTqObj.objName = (*tqDnListItr);

		if( myOmHandler.getObject(&myTqObj) == ACS_CC_FAILURE)
		{
			AES_GCC_TRACE_MESSAGE("recoverTQs  Unable to retrieve attributes for %s", myTqObj.objName.c_str());
			continue;
		}
		std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr;
		for(attributesListItr = myTqObj.attributes.begin(); attributesListItr != myTqObj.attributes.end();++attributesListItr)
		{
			//get TQ Name
			if((*attributesListItr).attrName == AES_DBO_BLOCK_TRANSFER_QUEUE_RDN)
			{
				myTqNameDn =  reinterpret_cast<char *>((*attributesListItr).attrValues[0]);
				AES_GCC_TRACE_MESSAGE("recoverTQs myTqNameDn = %s", myTqNameDn.c_str());
				string::size_type pos = string::npos;
				unsigned int len = myTqNameDn.length();
				pos = myTqNameDn.find_first_of("=");
				if((pos != string::npos) && pos < len)
				{
					myTqName = myTqNameDn.substr(pos + 1, len - pos-1);
				AES_GCC_TRACE_MESSAGE("recoverTQs myTqName = %s", myTqName.c_str());
				}
			}
			else if((*attributesListItr).attrName == AES_DBO_MIRRORED_ATTR)
			{
				mirrored =  *reinterpret_cast<ACE_INT32*>((*attributesListItr).attrValues[0]);
				AES_GCC_TRACE_MESSAGE("aes_dbo_server::recoverTQs mirrored = %d", mirrored);
			}
			else if((*attributesListItr).attrName == AES_DBO_REMOVE_DELAY_ATTR)
			{
				if((*attributesListItr).attrValuesNum !=0 )
				{
					removeDelay =  *reinterpret_cast<ACE_INT32*>((*attributesListItr).attrValues[0]);
					AES_GCC_TRACE_MESSAGE("aes_dbo_server::recoverTQs removeDelay = %d", removeDelay);
				}
			}
			else if((*attributesListItr).attrName == AES_DBO_BLOCK_DESTINATION_SET_ATTR)
			{
				myDestSetName = reinterpret_cast<char*>((*attributesListItr).attrValues[0]);	
				AES_GCC_TRACE_MESSAGE("aes_dbo_server::recoverTQs myDestSetName = %s", myDestSetName.c_str());
			}

		}
		if(aes_dbo_tqdatabase::instance()->cmpImmInfoInsTQ(myTqName,mirrored,removeDelay,myDestSetName) == -1)
		{
			AES_GCC_TRACE_MESSAGE("Database initialization failure");
			return -2;  // Error, bail out
		}
	}
	return 0;
}

/*============================================================================
        ROUTINE: initDSDServer
 ============================================================================ */
bool aes_dbo_server::initDSDServer()
{
        AES_GCC_TRACE_MESSAGE("Entering in initDSDServer()");

        // to avoid some erroneous publishing
        if(!m_serverOnLine)
        {
                AES_GCC_TRACE_MESSAGE("initDSDServer(), publish by DSD");

                int result = m_CmdServerAcceptor.open("AES_DBO_server", "AES");

                if(DSDConfig::DSDSuccess == result)
                {
                        AES_GCC_TRACE_MESSAGE("initDSDServer(), DSD server published, applName=AES_DBO_SERVER, domain=AES");
                        // set online flag
                        m_serverOnLine = true;
                }
                else
                {
			AES_GCC_TRACE_MESSAGE("initDSDServer(), DSD server published FAILED");
                }

        }

        AES_GCC_TRACE_MESSAGE("Leaving initDSDServer()");
        return m_serverOnLine;
}

/*============================================================================
        ROUTINE: registerDSDHandles
 ============================================================================ */
bool aes_dbo_server::registerDSDHandles()
{
        AES_GCC_TRACE_MESSAGE("Entering registerDSDHandles()");
        bool result = true;
        int idx = 0;
        ACE_HANDLE dsdHnl;

        fdList.clear();

        // get the first DSD handle
        dsdHnl =  m_CmdServerAcceptor.get_handle(idx);

        // Check if a valid handle
        while( ACE_INVALID_HANDLE != dsdHnl )
        {
                AES_GCC_TRACE_MESSAGE("registerDSDHandles(), handle=%i registered to reactor", idx);
                // Register to reactor the DSD handle
                _reactor->register_handler(dsdHnl,  this, ACE_Event_Handler::ACCEPT_MASK );
                // Store the handle for remove it from reactor
                fdList.push_back(dsdHnl);
                // get the next DSD handle
                dsdHnl =  m_CmdServerAcceptor.get_handle(++idx);
        }

        // Checks if at least one handle is valid
    if(0 == idx)
    {
        AES_GCC_TRACE_MESSAGE("registerDSDHandles(), no valid DSD handles");
        result = false;
    }

        AES_GCC_TRACE_MESSAGE("Leaving registerDSDHandles()");
        return result;
}

/*============================================================================
        ROUTINE: removeDSDHandles
 ============================================================================ */
void aes_dbo_server::removeDSDHandles()
{
        AES_GCC_TRACE_MESSAGE("Entering removeDSDHandles()");

        std::vector<ACE_HANDLE>::iterator it;

        // Remove all registered Handle from reactor
        for( it = fdList.begin(); it != fdList.end(); ++it )
        {
                _reactor->remove_handler( (*it), ACE_Event_Handler::ACCEPT_MASK );
        }

        fdList.clear();

        AES_GCC_TRACE_MESSAGE("Leaving removeDSDHandles()");
}

/*============================================================================
        ROUTINE: stopDSDServer
 ============================================================================ */
void aes_dbo_server::stopDSDServer()
{
        AES_GCC_TRACE_MESSAGE("Entering in stopDSDServer()");
        if(m_serverOnLine)
        {
                // Close the DSD server
                int result = m_CmdServerAcceptor.close();

                // check the closure result
                if(DSDConfig::DSDSuccess == result)
                {
                        AES_GCC_TRACE_MESSAGE("stopDSDServer(), DSD server closed");
                        // set online flag
                        m_serverOnLine = false;
                }
                else
                {
                        AES_GCC_TRACE_MESSAGE("stopDSDServer(), error on DSD server closure");
                }
        }
        else
        {
                AES_GCC_TRACE_MESSAGE("stopDSDServer(), DSD server already closed");
        }
        AES_GCC_TRACE_MESSAGE("Leaving stopDSDServer()");
}

/*============================================================================
        ROUTINE: handle_close
 ============================================================================ */
int aes_dbo_server::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask)
{
        AES_GCC_TRACE_MESSAGE("In");
        // DSD server closure
        stopDSDServer();
        AES_GCC_TRACE_MESSAGE("Out");
        return 0;
}
