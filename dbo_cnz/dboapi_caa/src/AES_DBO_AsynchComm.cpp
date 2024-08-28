
//*******************************************************************
// 
// .NAME
//  	AES_DBO_AsynchComm - Manages the connection to the server
// .LIBRARY 3C++
// .PAGENAME AES_DBO_AsynchComm
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_AsynchComm.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
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
// 	This class is used by the other API classes to be able to
//  communicate with the server.
//  

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 1300

// AUTHOR 
// 	2002-05-30 by UAB/KB/AU Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R1A_DMDN6.lib

// .SEE ALSO 
//  

//*******************************************************************
#include "AES_DBO_AsynchComm.h"
#include "AES_DBO_EventTarget.h"
#include "CmdClient.h"
#include "CommandCode.h"

#include <aes_gcc_tracer.h>
#include <aes_gcc_eventcodes.h>

#include <ace/Thread_Semaphore.h>
#include <ace/Time_Value_T.h>

#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include <sys/syscall.h>

namespace DBOAPI{
	const char DBO_DSD_Address[] = "AES_DBO_server:AES";
	const int WAIT_TIME_SEC = 10U;
}


// ACS Trace definition
AES_GCC_TRACE_DEFINE(AES_DBO_AsynchComm);


//*******************************************************************
//
bool AES_DBO_AsynchComm::connect()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

    // Session is closed
    bool connectionResult = false;

    // Create a client to handle the communication to DBO server
    client_ = new (std::nothrow) CmdClient(DBOAPI::DBO_DSD_Address);

    if( NULL != client_)
    {
    	AES_GCC_TRACE_MESSAGE("Open connection to DBO");

		// Connect to the server
		if( client_->connect() )
		{
			// Connection up
			AES_GCC_TRACE_MESSAGE("Connection up towards DBO, send CMD_OPENSESSION");

			ACS_APGCC_Command cmd;
			cmd.cmdCode = CMD_OPENSESSION;

			// Send the first command open session
			if( client_->send(cmd) )
			{

				AES_GCC_TRACE_MESSAGE("Waiting answer on CMD_OPENSESSION");
				// Command sent wait DBO server answer
				if (client_->receive(cmd, DBOAPI::WAIT_TIME_SEC) == CmdClient::COK)
				{
					AES_GCC_TRACE_MESSAGE("Received answer of CMD_OPENSESSION");

					// Check if all is OK
					if( cmd.result == AES_NOERRORCODE )
					{
						AES_GCC_TRACE_MESSAGE("Session towards DBO Established");
						// Session opened with success
						connectionResult = true;
					}
					else
					{
						// Seems that DBO server answer to the CMD_OPENSESSION always with
						// AES_NOERRORCODE, so this code is never reached
						AES_GCC_TRACE_MESSAGE("Session towards DBO not open, error:<%d>", cmd.result );
					}
				}
				else
				{
					AES_GCC_TRACE_MESSAGE("Failed to get answer on CMD_OPENSESSION");
				}
			}
			else
			{
				AES_GCC_TRACE_MESSAGE("Failed to send  CMD_OPENSESSION");
			}
		}
		else
		{
			AES_GCC_TRACE_MESSAGE("Failed to open connection to DBO");
		}

		if(!connectionResult)
		{
			client_->close();
			delete client_;
			client_ = NULL;
		}
    }
    AES_GCC_TRACE_MESSAGE("...Leaving, connection:<%s>", (connectionResult ? "OK": "NOT OK") );
    return connectionResult;
}


//*******************************************************************
//
AES_DBO_AsynchComm::AES_DBO_AsynchComm(AES_DBO_EventTarget *target, std::string tqName)
: tqName_(tqName),
 sendMutex_(),
 target_(target),
 client_(NULL),
 sessionOpen_(false),
 m_EventMutex(),
 m_EventList(),
 m_CmdReplyReady(new (std::nothrow) ACE_Thread_Semaphore(0 ,NULL, NULL, 1)),
 m_CmdReply(NULL),
 m_svcRun(1)
{

}


//*******************************************************************
//
AES_DBO_AsynchComm::~AES_DBO_AsynchComm()
{
	// Remove the client
    if(NULL != client_)
    {
        client_->close();
        delete client_;
        client_ = NULL;
    }

    if(NULL != m_CmdReplyReady)
    {
    	delete m_CmdReplyReady;
    	m_CmdReplyReady = NULL;
    }

    if( NULL != m_CmdReply)
    {
    	delete m_CmdReply;
    	m_CmdReply = NULL;
    }
}

//*******************************************************************
//
unsigned int AES_DBO_AsynchComm::sendCmd(ACS_APGCC_Command &cmd)
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // This method has to store its return value into
    // cmd.result. The calling methods depends on it.
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    AES_GCC_TRACE_MESSAGE("Entering..., send cmd:<%d>, session open:<%s>",
    						cmd.cmdCode, (sessionOpen_ ? "TRUE" : "FALSE") );

    // Make sure that we are alone
    ACE_Guard<ACE_Thread_Mutex> guard(sendMutex_);

    // Send the cmd to DBO server
    if( (NULL != client_) && client_->send(cmd) )
    {
		// Get command answer and check if it went OK

		AES_GCC_TRACE_MESSAGE("Command send wait for answer...");

		ACE_Time_Value timeout(ACE_OS::gettimeofday());
		timeout += ACE_Time_Value(DBOAPI::WAIT_TIME_SEC);

		// wait for a max of cmdTimeout time
		// will return 0 on Success -1 on failure or timeout
		int retValue = m_CmdReplyReady->acquire(timeout);

		// Check error condition
		if( (AES_NOERRORCODE != retValue) || (NULL == m_CmdReply) )
		{
			// Timeout occurred or some other error happens
			AES_GCC_TRACE_MESSAGE("Failed to get answer error:<%d>", errno);
		    cmd.result = AES_NOSERVERACCESS;
		}
		else
		{
			AES_GCC_TRACE_MESSAGE("Answer received!");
			// copy the answer to the caller
			cmd = (*m_CmdReply);
			// free allocated memory
			delete m_CmdReply;
			m_CmdReply = NULL;
		}
    }
    else
    {
    	AES_GCC_TRACE_MESSAGE("Failed to send command");
     	cmd.result = AES_NOSERVERACCESS;
    }

	AES_GCC_TRACE_MESSAGE("...Leaving, command:<%d> sent result:<%d>", cmd.cmdCode, cmd.result);
    return cmd.result;
}


//*******************************************************************
//
int AES_DBO_AsynchComm::svc()
{
	pid_t threadId = syscall(SYS_gettid);
	AES_GCC_TRACE_MESSAGE("Connection thread started, id:<%d>", threadId);

    unsigned int rCode;
    // The thread loop
    ACS_APGCC_Command *pCmd;

    boost::thread_group eventDispatcherThrds;

    while(__sync_fetch_and_and(&m_svcRun, 1) != 0)
    {
    	AES_GCC_TRACE_MESSAGE("Thread loop begin, id:<%d>", threadId);

		// Read command replies and events from server
		pCmd = new (std::nothrow) ACS_APGCC_Command();

		if( (pCmd != NULL) && (NULL != client_) )
		{
			AES_GCC_TRACE_MESSAGE("start waiting on receive..., id:<%d>", threadId);

			rCode = client_->receive(*pCmd);

			AES_GCC_TRACE_MESSAGE("...waiting on receive end!, id:<%d>", threadId);
		}
		else
		{
			AES_GCC_TRACE_MESSAGE("pCmd or client_ are NULL, id:<%d>", threadId);
			rCode = CmdClient::CERROR;
		}

        if (rCode == CmdClient::COK)
        {
        	if (pCmd->cmdCode == CMD_EVENT)
            {
        		AES_GCC_TRACE_MESSAGE("Received a CMD_EVENT from DBO, id:<%d>", threadId);

                // It is an event
                unsigned int evCode = pCmd->data[0];
                unsigned int blNumber = pCmd->data[1];

                delete pCmd;
				pCmd = 0;

                // Check if it is an application or api event
                if(evCode & AES_APIEVENT)
                {
                	AES_GCC_TRACE_MESSAGE("Internal API-Event received:<%d>, id:<%d>", (evCode ^ AES_APIEVENT), threadId);
                    // Dispatch the API-event through a thread
                	eventDispatcherThrds.create_thread( boost::bind(&AES_DBO_AsynchComm::apiEvent, this, (evCode ^ AES_APIEVENT) ));
                }
                else
                {
                	AES_GCC_TRACE_MESSAGE("Application API-Event received:<%d>, id:<%d>", (evCode), threadId);
                	{
                		// Make sure that we are alone
                		ACE_Guard<ACE_Thread_Mutex> guard(m_EventMutex);
                		eventMsg newEvent = {evCode, blNumber};
                		// Put it on the queue and report to application if any
                		m_EventList.push_back(newEvent);
                	}

                    if(target_)
                    {
                    	AES_GCC_TRACE_MESSAGE("Notify it to the Application target, id:<%d>", threadId);
                        // Dispatch the event through a thread
                    	eventDispatcherThrds.create_thread( boost::bind(&AES_DBO_EventTarget::event, target_, evCode ));
                    }
                }
            }
            else
            {
            	int cmdCode = pCmd->cmdCode;

            	AES_GCC_TRACE_MESSAGE("Answer on command:<%d> received, id:<%d>", cmdCode, threadId);
                // It is a command reply
                if(cmdCode == (CMD_CLOSESESSION + CMD_OK))
                {
                	AES_GCC_TRACE_MESSAGE("Answer on command CMD_CLOSESESSION, stop thread, id:<%d>", threadId);;
                    // Stop while loop
                    __sync_fetch_and_and(&m_svcRun, 0);
		        }

                // Set current cmd reply
                m_CmdReply = pCmd;
                // Signal the sender to get answer
                m_CmdReplyReady->release();

                AES_GCC_TRACE_MESSAGE("Sender notified, id:<%d>", threadId);
            }

            // Answer handled continue with listening
            continue;
        }
        else
        {
        	AES_GCC_TRACE_MESSAGE("Receive failed return error:<%d>, id:<%d>", rCode, threadId);
            // Something is wrong with the CmdClient
            if(NULL != pCmd)
            	delete pCmd;
        }
        
        // Check if an error has occurred
        if (rCode == CmdClient::CERROR)
        {
            // Check if a event target exists
            if(target_)
            {
            	AES_GCC_TRACE_MESSAGE("Send event Lost Server to the Application, id:<%d>", threadId);

                // Dispatch the lost-server event directly
                eventMsg newEvent = {AES_EVELOSTSERVER, 0};
                {
            	    ACE_Guard<ACE_Thread_Mutex> guard(m_EventMutex);
            	    // Put it on the queue and report to application if any
            	    m_EventList.push_back(newEvent);
                }

                target_->event(AES_EVELOSTSERVER);
            }

            AES_GCC_TRACE_MESSAGE("Stop thread, id:<%d>", threadId);;

            // Stop while loop
            __sync_fetch_and_and(&m_svcRun, 0);
        }
    }

    AES_GCC_TRACE_MESSAGE("Waiting Event Dispatcher Threads end before to terminate, id:<%d>", threadId);;
    // wait that all event thread are terminated
    eventDispatcherThrds.join_all();

    AES_GCC_TRACE_MESSAGE("Connection thread terminated!, id:<%d>", threadId);

    return 0;
}

//*******************************************************************
//
void AES_DBO_AsynchComm::destroy()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

    // Close connection
	ACS_APGCC_Command cmd;
	cmd.cmdCode = CMD_CLOSESESSION;

	AES_GCC_TRACE_MESSAGE("Send CMD_CLOSESESSION to DBO");
	// send cmd to close session to DBO
	sendCmd(cmd);

	// Check result code of answer
	if( AES_NOSERVERACCESS == cmd.result )
	{
		AES_GCC_TRACE_MESSAGE("Failed to receive answer on CMD_CLOSESESSION, force shutdown");
		// force internal thread stop since DBO can not send ack to the close command
		stopSvcThread();
	}

	AES_GCC_TRACE_MESSAGE("waiting on thread termination...");
	// wait on internal thread closure
	wait();

	AES_GCC_TRACE_MESSAGE("...Leaving");
}


//*******************************************************************
//
void AES_DBO_AsynchComm::apiEvent(unsigned int evCode)
{
    //To avoid unused warning
    UNUSED(evCode);
    AES_GCC_TRACE_MESSAGE("Nothing to do!");
}

int AES_DBO_AsynchComm::open(void *args)
{
	AES_GCC_TRACE_MESSAGE("Entering...");
	//To avoid unused warning
	UNUSED(args);
	int result = AES_NOSERVERACCESS;

	// check if already opened
	if(sessionOpen_)
	{
		AES_GCC_TRACE_MESSAGE("Session already open!");
		result = AES_NOERRORCODE;
	}
	else
	{
		// Open connection to DBO and start internal thread
		if((NULL != m_CmdReplyReady) && connect())
		{
			AES_GCC_TRACE_MESSAGE("Session opened");

			m_svcRun = 1;

			// Start the worker thread joinable
			if( activate() == 0)
			{
				AES_GCC_TRACE_MESSAGE("Internal thread started");
				result = AES_NOERRORCODE;
				sessionOpen_ = true;
			}
			else
			{
				AES_GCC_TRACE_MESSAGE("Failed to start internal thread!");
				// Fail to start internal thread
				// Close connection towards DBO server
				ACS_APGCC_Command cmd;
				cmd.cmdCode = CMD_CLOSESESSION;
				client_->send(cmd);
				// Close old Connection without wait the DBO answer
				client_->close();
				delete client_;
				client_ = NULL;
				AES_GCC_TRACE_MESSAGE("Session closed!");
			}
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", result);
	return result;
}

int AES_DBO_AsynchComm::stopSvcThread()
{
	AES_GCC_TRACE_MESSAGE("Entering...");
	// Set the internal thread to terminate
	int currentState = __sync_fetch_and_and(&m_svcRun, 0);

	if(NULL != client_)
	{
		AES_GCC_TRACE_MESSAGE("force client connection closure");
		client_->shutDown();
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, thread previous state:<%d>", currentState);
	// Return the previous thread state
	return currentState;
}

//
void AES_DBO_AsynchComm::close()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

	// Check if the connection already opened
	if(sessionOpen_)
	{
		AES_GCC_TRACE_MESSAGE("Close current session");

		// Terminate old faulty connection
		// Stop internal thread
		stopSvcThread();

		// wait on thread termination
		wait();

		// Close old Connection
		client_->close();
		delete client_;
		client_ = NULL;

		sessionOpen_ = false;

		AES_GCC_TRACE_MESSAGE("current session closed!");
	}

	AES_GCC_TRACE_MESSAGE("...Leaving");
}
