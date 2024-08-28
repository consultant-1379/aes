/*=================================================================== */
/**
   @file   aes_cdh_asynchreceiver.cpp

   Class method implementationn for AES_CDH_AsynchReceiver type module.

   This module contains the implementation of class declared in
   the aes_cdh_asynchreceiver.h module

   Creates a thread that handles receving of replies from CDH server during
   a send file task or a send record file task.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD     Initial Release
==================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_cdh_asynchreceiver.h>
#include <aes_cdh_destinationsetimplementation.h>
#include <ace/OS_NS_sys_select.h>
#include <aes_cdh_cmdclient.h>
#include <sys/eventfd.h>
#include "aes_cdh_tracer.h"

namespace CDHAPI
{
	const char CDH_DSD_Address[] = "AES_CDH_server:AES";
	const int WAIT_TIME_SEC = 5U;
}

AES_CDH_TRACE_DEFINE(AES_CDH_AsynchReceiver);
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
/*===================================================================
   ROUTINE: AES_CDH_AsynchReceiver
=================================================================== */
AES_CDH_AsynchReceiver::AES_CDH_AsynchReceiver()
: client(0),
  sessionOpen(false),
  closeSessionAckIsReceived(false),
  stopTasksAckIsReceived(false),
  closeSessionSem(0),
  stopTasksSem(0),
  destSetPtr(0),
  destinationSetName(),
  m_StopEvent(ACE_INVALID_HANDLE),
  m_TransBegin(0,NULL,NULL,1),
  m_TransEnd(0,NULL,NULL,1),
  m_TransTerminate(0,NULL,NULL,1),
  m_TransCommit(0,NULL,NULL,1),
  m_GetLastCommittedBlock(0,NULL,NULL,1),
  m_CmdReply(NULL),
  m_CmdMutex(),transactionCommit(false)
{

}

/*===================================================================
   ROUTINE: ~AES_CDH_AsynchReceiver
=================================================================== */
AES_CDH_AsynchReceiver::~AES_CDH_AsynchReceiver()
{
	ACE_OS::close(m_StopEvent);
	if( NULL != client)
	{
		delete client;
		client = NULL;
	}
}

/*===================================================================
   ROUTINE: openSession
=================================================================== */
AES_CDH_ResultCode AES_CDH_AsynchReceiver::openSession(AES_CDH_DestinationSetImplementation& destSet, const string destSetName)
{
	AES_CDH_TRACE_MESSAGE("Entering....");

	if(!sessionOpen)
    {
    	// create shutdown handle
    	m_StopEvent = eventfd(0,0);

    	AES_CDH_ResultCode rcode;
        destSetPtr = &destSet;
        destinationSetName = destSetName;
        ACS_APGCC_Command cmd;
        closeSessionAckIsReceived = false;
        stopTasksAckIsReceived = false;
        
        cmd.cmdCode = ServR::CMD_OPENSESSION; 
        cmd.data[0] = destinationSetName;
        cmd.data[1] = (int)ACE_OS::getpid();
        cmd.data[2] = (long)this;

        AES_CDH_TRACE_MESSAGE("destinationSetName:<%s>", destinationSetName.c_str());

        rcode = AES_CDH_RC_NOSERVER;

        // create the client
        client = new (std::nothrow) AES_CDH_CmdClient(CDHAPI::CDH_DSD_Address);
        
        // connect to server
	    if( (NULL != client) && ( client->connect() != -1) )
        {
	    	AES_CDH_TRACE_MESSAGE("Client connected, destinationSetName:<%s>", destinationSetName.c_str());

	    	// send to server
            if(client->send(cmd) != -1)
            {
            	AES_CDH_TRACE_MESSAGE("Send CMD_OPENSESSION waiting on CDH answer, destinationSetName:<%s>", destinationSetName.c_str());

                if(client->receive(cmd) != -1)
                {
                	AES_CDH_TRACE_MESSAGE("Answer of CMD_OPENSESSION received, destinationSetName:<%s>", destinationSetName.c_str());

                    if(cmd.result == AES_CDH_RC_OK)
                    {
                    	// Start the worker thread joinable
                        if( 0 == activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED) )
                        {
                            sessionOpen = true;
                            AES_CDH_TRACE_MESSAGE("Session opened and thread started");
                            return AES_CDH_RC_OK;
                        }
                        
                        rcode = AES_CDH_RC_INTPROGERR;
                    }
                    else
                        rcode = (AES_CDH_ResultCode)cmd.result;
                }
            }
        }

	    // Failed !  Clean up.
	if(client != NULL)
	{
		delete client;
		client = NULL;
	}
        return rcode;
    }
    else
    	AES_CDH_TRACE_MESSAGE("Session already opened");

    return AES_CDH_RC_INCUSAGE;
}

/*===================================================================
   ROUTINE: closeSession
=================================================================== */
AES_CDH_ResultCode AES_CDH_AsynchReceiver::closeSession(void)
{
	AES_CDH_TRACE_MESSAGE("Entering..., destinationSetName:<%s>", destinationSetName.c_str() );
	AES_CDH_ResultCode rcode = AES_CDH_RC_NOSERVER;

	if(sessionOpen)
	{
		ACS_APGCC_Command cmd;

		cmd.cmdCode = ServR::CMD_CLOSESESSION;
		cmd.data[0] = (int)ACE_OS::getpid();
		cmd.data[1] = (long)this;

		closeSessionAckReceived(true);

		bool stopOnGoing = true;

		// send to server
		rcode = sendCmd(cmd);

		if( rcode == AES_CDH_RC_NOSERVER )
		{
			AES_CDH_TRACE_MESSAGE("Failed to send CMD_CLOSESESSION command, destinationSetName:<%s>", destinationSetName.c_str() );
			stopOnGoing = shutDown();
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Waiting answer of CMD_CLOSESESSION command, destinationSetName:<%s>", destinationSetName.c_str() );

			int secOnWait = 5;
			ACE_Time_Value timeout(ACE_OS::gettimeofday());
			timeout += ACE_Time_Value(secOnWait);

			// wait for a max of cmdTimeout time
			// will return 0 on Success -1 on failure or timeout
			if( 0 != closeSessionSem.acquire(timeout))
			{
				AES_CDH_TRACE_MESSAGE("Answer of CMD_CLOSESESSION command not received after <%d> sec, destinationSetName:<%s>", secOnWait, destinationSetName.c_str() );
				// time-out elapsed but not answer received yet from CDH
				// force thread shutdown
				stopOnGoing = shutDown();
			}
			else
			{
				AES_CDH_TRACE_MESSAGE("Answer of CMD_CLOSESESSION received, destinationSetName:<%s>", destinationSetName.c_str() );
			}
		}

		// As Thread is joinable, it is required to call wait() always to ensure close of thread
		AES_CDH_TRACE_MESSAGE("Waiting on svc thread termination, destinationSetName:<%s>", destinationSetName.c_str() );
		wait();
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, destinationSetName:<%s>", destinationSetName.c_str() );

    return rcode;
}

/*===================================================================
   ROUTINE: closeSesssionAckReceived
=================================================================== */
bool AES_CDH_AsynchReceiver::closeSessionAckReceived(bool set) 
{
    if(set)
        closeSessionAckIsReceived = true;


    return closeSessionAckIsReceived;
}

/*===================================================================
   ROUTINE: stopTasksAckReceived
=================================================================== */
bool AES_CDH_AsynchReceiver::stopTasksAckReceived(bool set) 
{
	 AES_CDH_TRACE_MESSAGE("Entering..., to <%s>", (set ? "SET" : "GET"));
     if(set)
     {
    	 stopTasksAckIsReceived = true;
     }
     else
     {
    	 AES_CDH_TRACE_MESSAGE("Waiting on CMD_STOPTASKS reply..., destinationSetName:<%s>", destinationSetName.c_str() );
    	 ACE_Time_Value tv(ACE_OS::gettimeofday () + ACE_Time_Value(ACE_Time_Value(CDHAPI::WAIT_TIME_SEC)));
         stopTasksSem.acquire(tv);
     }
     AES_CDH_TRACE_MESSAGE("...Leaving, stopTasksAckIsReceived <%s>", (stopTasksAckIsReceived ? "TRUE" : "FALSE"));
     return stopTasksAckIsReceived;
}

/*===================================================================
   ROUTINE: sendCmdAndWait
=================================================================== */
AES_CDH_ResultCode AES_CDH_AsynchReceiver::sendCmdAndWait(ACS_APGCC_Command &cmd)
{
	 AES_CDH_TRACE_MESSAGE("Entering...");
	 AES_CDH_ResultCode sendResult = AES_CDH_RC_NOSERVER;

	 if(sessionOpen)
	 {
		 // set pointer to get answer
		 m_CmdReply = &cmd;

		 if(client->send(cmd) != -1)
		 {
			 int retValue = 0;
			 ACE_Time_Value timeout(ACE_OS::gettimeofday());
			 timeout += ACE_Time_Value(CDHAPI::WAIT_TIME_SEC);

			 AES_CDH_TRACE_MESSAGE("Command send waiting for answer...");
			 switch(cmd.cmdCode)
			 {
			 	 case ServR::CMD_TRANSACTIONBEGIN:
			 	 {
			 		// wait on Transaction begin reply from CDH
			 	    // wait for a max of WAIT_TIME_SEC time
					// will return 0 on Success -1 on failure or timeout
					retValue = m_TransBegin.acquire(timeout);

					{
						// Make sure that we are alone
					    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
						m_CmdReply = NULL;
					}

					// Check condition
					if( 0 == retValue)
					{
						AES_CDH_TRACE_MESSAGE("...Answer received, result:<%d>", cmd.result);
						// OK reply received
						sendResult = static_cast<AES_CDH_ResultCode>(cmd.result);
					}
			 	 }
			 	 break;

			 	 case ServR::CMD_TRANSACTIONEND:
				 {
					// wait on Transaction end reply from CDH
					// wait for a max of WAIT_TIME_SEC time
					// will return 0 on Success -1 on failure or timeout
					retValue = m_TransEnd.acquire(timeout);

					{
						// Make sure that we are alone
						ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
						m_CmdReply = NULL;
					}

					// Check condition
					if( 0 == retValue)
					{
						AES_CDH_TRACE_MESSAGE("...Answer received, result:<%d>", cmd.result);
						// OK reply received
						sendResult = static_cast<AES_CDH_ResultCode>(cmd.result);
					}
				 }
				 break;

			 	 case ServR::CMD_TRANSACTIONCOMMIT:
				 {
					// wait on Transaction commit reply from CDH
					// wait for a max of WAIT_TIME_SEC time
					// will return 0 on Success -1 on failure or timeout
					transactionCommit = false;							////HV50261
					AES_CDH_TRACE_MESSAGE(" timeout value(B) %lu",timeout.msec());
					retValue = m_TransCommit.acquire(timeout);
					AES_CDH_TRACE_MESSAGE("timeout value(A) %lu",timeout.msec());

                                        if( retValue == -1)
                                        {
						AES_CDH_TRACE_MESSAGE("Transaction commit semaphore timeout");
                                        }
                                        else 
                                        {
					    while((transactionCommit == false) && (ACE_OS::gettimeofday() < timeout) )
					    {
					        AES_CDH_TRACE_MESSAGE("Problem in aquiring semaphore Transaction commit");
				                retValue = m_TransCommit.acquire(timeout);
				                if (transactionCommit == false)
					        {
                                                     continue;
					        }
					        else
					        {
						     retValue=0;
                                                     break;
					        }
					   }
					}

					{
						// Make sure that we are alone
						ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
						m_CmdReply = NULL;
					}									//HV50261

					// Check condition
					if( 0 == retValue)
					{
						AES_CDH_TRACE_MESSAGE("...Answer received, result:<%d>", cmd.result);
						// OK reply received
						sendResult = static_cast<AES_CDH_ResultCode>(cmd.result);
					}
					else
					{
						AES_CDH_TRACE_MESSAGE("retvalue %d",retValue);
					}
				 }
				 break;

			 	 case ServR::CMD_GETLASTCOMMITTEDBLOCK:
				 {
					// wait on get last reply from CDH
					// wait for a max of WAIT_TIME_SEC time
					// will return 0 on Success -1 on failure or timeout
					retValue = m_GetLastCommittedBlock.acquire(timeout);
					{
						// Make sure that we are alone
						ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
						m_CmdReply = NULL;
					}

					// Check condition
					if( 0 == retValue)
					{
						AES_CDH_TRACE_MESSAGE("...Answer received, result:<%d>", cmd.result);
						// OK reply received
						sendResult = static_cast<AES_CDH_ResultCode>(cmd.result);
					}
				 }
				 break;

			 	 case ServR::CMD_TRANSACTIONTERMINATE:
				 {
					// wait on Transaction terminate from CDH
					// wait for a max of WAIT_TIME_SEC time
					// will return 0 on Success -1 on failure or timeout
					retValue = m_TransTerminate.acquire(timeout);

					{
						// Make sure that we are alone
						ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
						m_CmdReply = NULL;
					}

					// Check condition
					if( 0 == retValue)
					{
						AES_CDH_TRACE_MESSAGE("...Answer received, result:<%d>", cmd.result);
						// OK reply received
						sendResult = static_cast<AES_CDH_ResultCode>(cmd.result);
					}
				 }
				 break;

			 	 default:
			 	 {
			 		AES_CDH_TRACE_MESSAGE("Unknown command");
			 	 }
			 }
		 }
		 else
		 {
			 m_CmdReply = NULL;
			 AES_CDH_TRACE_MESSAGE("Send command to CDH Failed");
		 }
	 }

	 AES_CDH_TRACE_MESSAGE("...Leaving, result:<%d>", sendResult);
     return sendResult;
}


/*===================================================================
   ROUTINE: svc
=================================================================== */
int AES_CDH_AsynchReceiver::svc()
{
    AES_CDH_ResultCode result;
    std::string fName;
    std::string newfName;
    std::string newSfName;
    std::string destSetName;
    bool receiveLoopOn = true;
    pid_t threadId = syscall(SYS_gettid);

    AES_CDH_TRACE_MESSAGE("AsynchReceiver thread started, id:<%d>", threadId);

    // Initialize the pollfd structure
	const nfds_t nfds = 2;
	struct pollfd fds[nfds];
	ACE_OS::memset(fds, 0, sizeof(fds));

	// Stop signal
	fds[0].fd = m_StopEvent;
	fds[0].events = POLLIN | POLLRDHUP;

	// Connection handle
	fds[1].fd = client->getHandle();
	fds[1].events = POLLIN | POLLRDHUP;

	ACE_INT32 pollResult;
    do 
    {
    	// Wait for input ready or close signal
    	pollResult = ACE_OS::poll(fds, nfds);

    	// Error on Poll
		if( -1 == pollResult )
		{
			if(errno == EINTR)
			{
				// try again
				continue;
			}
			AES_CDH_TRACE_MESSAGE("Error on pool, id:<%d>", threadId);
			closeSessionAckIsReceived = true;
			closeSessionSem.release();
			stopTasksAckIsReceived = true;
			stopTasksSem.release();
			receiveLoopOn = false;
			sessionOpen = false; //HY45379
			break;
		}
		if( (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL | POLLRDHUP)) || (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL | POLLRDHUP)) )
		{
			AES_CDH_TRACE_MESSAGE("POLL ERROR");
			// exit from the while loop
			closeSessionAckIsReceived = true;
			closeSessionSem.release();
			stopTasksAckIsReceived = true;
			stopTasksSem.release();
			receiveLoopOn = false;
			sessionOpen = false; //HY45379
			break;
		}

		// check handle
		if(fds[0].revents & POLLIN)
		{
			// Received signal of termination
			AES_CDH_TRACE_MESSAGE("ShutDown connection received!, id:<%d>", threadId);
			break;
		}

		if(fds[1].revents & POLLIN)
		{
			ACS_APGCC_Command cmd;
			if( client->receive(cmd) != -1 )
			{
				switch (cmd.cmdCode)
				{
					case (ServR::CMD_SENDFILE + ServR::CMD_OK):
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_SENDFILE ack, id:<%d>", threadId);
						if(!destSetPtr)
						{
							AES_CDH_TRACE_MESSAGE("Error. It's not possible to handle request CMD:%d. Destination Set detroyed!",cmd.cmdCode);
							break;
						}
						result = (AES_CDH_ResultCode)cmd.result;
						destSetName    = (std::string)cmd.data[0];
						fName       = (std::string)cmd.data[1];
						newfName    = (std::string)cmd.data[2];
						newSfName   = (std::string)cmd.data[5];
						destSetPtr->sendFileEv(result, destSetName, fName, newfName, newSfName);
						break;
					}

					case (ServR::CMD_SENDRECORDFILE + ServR::CMD_OK):
					{
						if(!destSetPtr)
						{
							AES_CDH_TRACE_MESSAGE("Error. It's not possible to handle request CMD:%d. Destination Set detroyed!",cmd.cmdCode);
							break;
						}
						result = (AES_CDH_ResultCode)cmd.result;
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_SENDRECORDFILE event:<%d> , id:<%d>",result, threadId);
						destSetPtr->sendRecordFileEv(result);
						break;
					}

					case (ServR::CMD_DESTEVENT):
					{
						if(!destSetPtr)
						{
							AES_CDH_TRACE_MESSAGE("Error. It's not possible to handle request CMD:%d. Destination Set detroyed!",cmd.cmdCode);
							break;
						}
						destSetName = (std::string)cmd.data[0];
						result = (AES_CDH_ResultCode)cmd.result;

						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_DESTEVENT, event:<%d> on destination:<%s>, id:<%d>", result, destSetName.c_str(), threadId);

						if(result == AES_CDH_RC_OK)
						{
							destSetPtr->sendDestEv(AES_CDH_RC_CONNECTOK, destSetName);
						}
						else
						{
							destSetPtr->sendDestEv(result, destSetName);
						}

						break;
					}

					case(ServR::CMD_CLOSESESSION + ServR::CMD_OK):
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_CLOSESESSION ack, id:<%d>", threadId);
						closeSessionAckIsReceived = true;
						closeSessionSem.release();
						receiveLoopOn = false;
						sessionOpen = false; //HY45379
						break;
					}

					case(ServR::CMD_STOPTASKS + ServR::CMD_OK):
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_STOPTASKS ack, id:<%d>", threadId);
						stopTasksAckIsReceived = true;
						stopTasksSem.release();
						break;
					}

					case(ServR::CMD_SETEVENTHANDLER + ServR::CMD_OK):
					{
						if(!destSetPtr)
						{
							AES_CDH_TRACE_MESSAGE("Error. It's not possible to handle request CMD:%d. Destination Set detroyed!",cmd.cmdCode);
							break;
						}
						destSetName = (std::string)cmd.data[0];
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_SETEVENTHANDLER ack, destination:<%s>, id:<%d>", destSetName.c_str(), threadId);

						if((AES_CDH_ResultCode)cmd.result == AES_CDH_RC_OK)
						{
							destSetPtr->sendDestEv(AES_CDH_RC_EVENTHANDLERSET, destSetName);
						}
						else
						{
							destSetPtr->sendDestEv(AES_CDH_RC_EVENTHANDLERSETERR, destSetName);
						}
						break;
					}

					case(ServR::CMD_REMOVEEVENTHANDLER + ServR::CMD_OK):
					{
						if(!destSetPtr)
						{
							AES_CDH_TRACE_MESSAGE("Error. It's not possible to handle request CMD:%d. Destination Set detroyed!",cmd.cmdCode);
							break;
						}
						destSetName  = (std::string)cmd.data[0];

						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_REMOVEEVENTHANDLER ack, destination:<%s>, id:<%d>", destSetName.c_str(), threadId);

						if((AES_CDH_ResultCode)cmd.result == AES_CDH_RC_OK)
						{
							destSetPtr->sendDestEv(AES_CDH_RC_EVENTHANDLERREM, destSetName);
						}
						else
						{
							destSetPtr->sendDestEv(AES_CDH_RC_EVENTHANDLERREMERR, destSetName);
						}
						break;
					}

					case(ServR::CMD_OPENSESSION + ServR::CMD_OK):
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_OPENSESSION ack, id:<%d>", threadId);
						break;
					}

					case( ServR::CMD_CDH_STOPPING ):
					{
						if(!destSetPtr)
						{
							AES_CDH_TRACE_MESSAGE("Error. It's not possible to handle request CMD:%d. Destination Set detroyed!",cmd.cmdCode);
							break;
						}
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_CDH_STOPPING command, id:<%d>", threadId);
						destSetPtr->sendDestEv( AES_CDH_RC_CDHSTOPPING, destSetName);
						break;
					}

					case(ServR::CMD_CDH_STOPPED):
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_CDH_STOPPED command, id:<%d>", threadId);
						closeSessionAckIsReceived = true;
						closeSessionSem.release();
						stopTasksAckIsReceived = true;
						stopTasksSem.release();
						receiveLoopOn = false;
						sessionOpen = false; //HY45379
						break;
					}

					case ServR::CMD_TRANSACTIONBEGIN:
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_TRANSACTIONBEGIN command reply result:<%d>, id:<%d>", cmd.result, threadId);

						{
							// Make sure that we are alone
						    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
						    if(NULL != m_CmdReply)
						     	(*m_CmdReply) = cmd;
						}

						m_TransBegin.release();
					}
					break;

					case ServR::CMD_TRANSACTIONEND:
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_TRANSACTIONEND command reply result:<%d>, id:<%d>", cmd.result, threadId);

						{
							// Make sure that we are alone
							ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
							if(NULL != m_CmdReply)
								(*m_CmdReply) = cmd;
						}

						m_TransEnd.release();
					}
					break;

					case ServR::CMD_TRANSACTIONCOMMIT:
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_TRANSACTIONCOMMIT command reply result:<%d>, id:<%d>", cmd.result, threadId);

						{
							// Make sure that we are alone
							ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
							if(NULL != m_CmdReply)
								(*m_CmdReply) = cmd;
						}
					 	transactionCommit = true;                       //HV50261
						m_TransCommit.release();
					}
					break;

					case ServR::CMD_GETLASTCOMMITTEDBLOCK:
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_GETLASTCOMMITTEDBLOCK command reply result:<%d>, id:<%d>", cmd.result, threadId);

						{
							// Make sure that we are alone
							ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
							if(NULL != m_CmdReply)
								(*m_CmdReply) = cmd;
						}

						m_GetLastCommittedBlock.release();
					}
					break;

					case ServR::CMD_TRANSACTIONTERMINATE:
					{
						AES_CDH_TRACE_MESSAGE("Received an ServR::CMD_TRANSACTIONTERMINATE command reply result:<%d>, id:<%d>", cmd.result, threadId);

						{
							// Make sure that we are alone
							ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_CmdMutex);
							if(NULL != m_CmdReply)
								(*m_CmdReply) = cmd;
						}

						m_TransTerminate.release();
					}
					break;

					default:
					{
						AES_CDH_TRACE_MESSAGE("Received an Unknown command, id:<%d>", threadId);
					}
				}
		    }
			else
			{
				AES_CDH_TRACE_MESSAGE("Failed on receive, id:<%d>", threadId);
				// exit from the while loop
				closeSessionAckIsReceived = true;
				closeSessionSem.release();
				stopTasksAckIsReceived = true;
				stopTasksSem.release();
				receiveLoopOn = false;
				break;
			}
	    }

    }
    while(receiveLoopOn);

    AES_CDH_TRACE_MESSAGE("...Leaving");

    return 0;
}

/*===================================================================
   ROUTINE: close(u_long)
=================================================================== */
int AES_CDH_AsynchReceiver::close(u_long)
{
	AES_CDH_TRACE_MESSAGE("Entering...");
	if(destSetPtr)
	{
		std::string tmpDestSetName = destinationSetName;
		destSetPtr->sendDestEv(AES_CDH_RC_NOSERVER, tmpDestSetName);
	}
	AES_CDH_TRACE_MESSAGE("...Leaving");
	return 0;
}

/*===================================================================
   ROUTINE: sendCmd
=================================================================== */
AES_CDH_ResultCode AES_CDH_AsynchReceiver::sendCmd(ACS_APGCC_Command &cmd)
{
	AES_CDH_TRACE_MESSAGE("Entering..., command:<%d>", cmd.cmdCode);

	AES_CDH_ResultCode sendResult = AES_CDH_RC_NOSERVER;

	if(sessionOpen)
	{
		// send to server
        if(client->send(cmd) != -1)
        	sendResult = AES_CDH_RC_OK;
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, result<%d>", sendResult);
    return sendResult;
}

/*===================================================================
   ROUTINE: shutDown
=================================================================== */
bool AES_CDH_AsynchReceiver::shutDown()
{
	AES_CDH_TRACE_MESSAGE("Entering ..., stop Event:<%d>", m_StopEvent);

	eventfd_t stop=1U;

	bool result = (0 == eventfd_write(m_StopEvent, stop) );

	AES_CDH_TRACE_MESSAGE("...Leaving, result:<%s>, error:<%d>", (result ? "OK" : "NOT OK"), errno);

	return result;
}
