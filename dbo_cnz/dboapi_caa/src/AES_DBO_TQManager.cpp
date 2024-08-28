
//*******************************************************************
// 
// .NAME
//  	AES_DBO_TQManager - Manages all of the transfer queues
// .LIBRARY 3C++
// .PAGENAME AES_DBO_TQManager
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_TQManager.h

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
// 	This class is used by all clients which uses the transfer queues.
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
// 	2002-05-02 by UAB/KB/AU Mats Nilsson

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

#include "AES_DBO_TQManager.h"
#include "AES_DBO_AsynchComm.h"
#include "AES_DBO_EventTarget.h"
#include "CommandCode.h"
#include "ParameterHandler.h"

#include <aes_gcc_errorcodes.h>
#include <aes_gcc_tracer.h>

// ACS Trace definition
AES_GCC_TRACE_DEFINE(AES_DBO_TQManager_Imp);

//*******************************************************************
//
AES_DBO_TQManager_Imp::AES_DBO_TQManager_Imp()
:m_Comunicator(new (std::nothrow) AES_DBO_AsynchComm(NULL)),
 m_mutex(),
 m_isConnectionUp(false)
{

}


//*******************************************************************
//
AES_DBO_TQManager_Imp::~AES_DBO_TQManager_Imp()
{
    if(NULL != m_Comunicator)
    {
    	m_Comunicator->destroy();
    	delete m_Comunicator;
    	m_Comunicator = NULL;
    }
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::createTQ(const std::string &tqName,
                                         const std::string &destName,
                                         unsigned int mirror, 
                                         unsigned int removeDelay,
                                         const std::string &userGroup)
{
	AES_GCC_TRACE_MESSAGE("Entering...\n TQ:<%s>, Destination set:<%s>, mirror:<%s>\n remove Delay:<%d>, UserG:<%s>",
			tqName.c_str(), destName.c_str(), (mirror ? "yes" : "no"), removeDelay, userGroup.c_str());

    ACS_APGCC_Command cmd;

    cmd.cmdCode = CMD_DEFINE;
	cmd.data[0] = tqName;
	cmd.data[1] = destName;
	cmd.data[2] = mirror;
	cmd.data[3] = removeDelay;
	cmd.data[4] = userGroup;

	// send the command to server
	sendRequest(cmd);

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);
    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::changeTQDest(const std::string &user,
                                             const std::string &tq,
                                             const std::string &dest)
{
	AES_GCC_TRACE_MESSAGE("Entering...\n user:<%s>, TQ:<%s>, Destination set:<%s>",
						user.c_str(), tq.c_str(), dest.c_str());

    ACS_APGCC_Command cmd;
	cmd.data[0] = user;
	cmd.data[1] = tq;
	cmd.data[2] = dest;
    cmd.cmdCode = CMD_C_DEST;

	// send the command to server
    sendRequest(cmd);

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::changeTQMirror(const std::string &user,
                                               const std::string &tq,
                                               unsigned int mirror,
                                               unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("Entering...\n user:<%s>, TQ:<%s>, mirror:<%s>, Remove delay:<%d>",
						user.c_str(), tq.c_str(), (mirror ? "yes" : "no"), delay);

    ACS_APGCC_Command cmd;
	cmd.data[0] = user;
	cmd.data[1] = tq;
	cmd.data[2] = mirror;
    cmd.data[3] = delay;
    cmd.cmdCode = CMD_C_MIRROR;

	// send the command to server
    sendRequest(cmd);

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::changeTQ(const std::string &user,
                                         const std::string &tq,
                                         const std::string &dest,
                                         unsigned int mirror,
                                         unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("Entering...\n user:<%s>, TQ:<%s>, \n Destination:<%s>, mirror:<%s>, Remove delay:<%d>",
				user.c_str(), tq.c_str(), dest.c_str(), (mirror ? "yes" : "no"), delay);

    ACS_APGCC_Command cmd;
	cmd.data[0] = user;
	cmd.data[1] = tq;
	cmd.data[2] = dest;
    cmd.data[3] = mirror;
    cmd.data[4] = delay;
    cmd.cmdCode = CMD_C_ALL;

	// send the command to server
    sendRequest(cmd);

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::removeTQ(const std::string &user,
                                         const std::string &tqName)
{
	AES_GCC_TRACE_MESSAGE("Entering..., User:<%s>, TQ:<%s>", user.c_str(), tqName.c_str() );

    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_DELETE;
	cmd.data[0] = user;
	cmd.data[1] = tqName;

	// send the command to server
	sendRequest(cmd);

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::tqExists(const std::string &tqName)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tqName.c_str() );

    ACS_APGCC_Command cmd;
	cmd.data[0] = tqName;
    cmd.cmdCode = CMD_EXISTS;

	// send the command to server
    sendRequest(cmd);
    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);
    return cmd.result;
}

unsigned int AES_DBO_TQManager_Imp::tqExists(const std::string &tqName,
						 std::string &tqDN)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tqName.c_str() );

    ACS_APGCC_Command cmd;
	cmd.data[0] = tqName;
    cmd.cmdCode = CMD_EXISTS_DN;

	// send the command to server
    sendRequest(cmd);

    if( cmd.result == AES_NOERRORCODE )
	{
		tqDN = cmd.data[1].c_str();
		AES_GCC_TRACE_MESSAGE("TQ DN:<%s>", tqDN.c_str() );
	}

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);

    return cmd.result;
}

unsigned int AES_DBO_TQManager_Imp::isDestSetAttached(const std::string &destSetName )
{
	AES_GCC_TRACE_MESSAGE("Entering..., destSetName:<%s>", destSetName.c_str() );
	ACS_APGCC_Command cmd;
	cmd.data[0] = destSetName;
	cmd.cmdCode = CMD_TQ_ATTACH;

	// send the command to server
	sendRequest(cmd);

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", cmd.result);

	return cmd.result;
}
		
//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::tqList(std::list<std::string> &qList,
                                       const std::string &tqName)
{
	AES_GCC_TRACE_MESSAGE("Entering...");

    ACS_APGCC_Command cmd;
    cmd.data[0] = tqName;
    cmd.cmdCode = CMD_LIST;

    AES_GCC_TRACE_MESSAGE("TQ:<%s>", tqName.c_str() );
	// send the command to server
    sendRequest(cmd);
    
    // If all went OK, fill the list of strings
    if (cmd.result == AES_NOERRORCODE)
    {
    	AES_GCC_TRACE_MESSAGE("TQ:<%s>", tqName.c_str() );

    	for(int i = 0; i < cmd.numOfData(); ++i)
        {
            qList.push_back(std::string(cmd.data[i]));
        }
    }

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>, TQ List size:<%zu>", cmd.result, qList.size() );
    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::openRx(const std::string &tqName,
                                       AES_DBO_TQReceiver *&tq)
{
    AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tqName.c_str());

    unsigned int result = AES_NOERRORCODE;
    // Return a created transfer queue
	tq = new (std::nothrow) AES_DBO_TQReceiver(tqName);

	if( NULL == tq)
		result = AES_EXECUTEERROR;

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", result);

	return result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::openTx(AES_DBO_EventTarget *evTarget,
                                          const std::string &tqName,
                                          AES_DBO_TQSender *&tq)
{

    AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tqName.c_str());

    unsigned int openResult = AES_NOSERVERACCESS;

    unsigned int streamSize =  ParameterHandler::instance()->getStreamSize();

    AES_GCC_TRACE_MESSAGE("create a TQ Sender, stream size:<%d>", streamSize);

    // Create a transfer queue sender object
    tq = new ( std::nothrow) AES_DBO_TQSender(evTarget, streamSize, tqName );

    if((NULL != tq) )
	{
    	AES_GCC_TRACE_MESSAGE("open TQ sender");
		// open internal communication thread
    	openResult = tq->open();

		if( AES_NOERRORCODE != openResult)
		{
			AES_GCC_TRACE_MESSAGE("failed to start TQ sender thread");
			delete tq;
			tq = NULL;
		}
	}

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", openResult);

    return openResult;
}


//*******************************************************************
//
unsigned int AES_DBO_TQManager_Imp::openTx(AES_DBO_EventTarget *evTarget,
                                          const std::string &tqName,
                                          const std::string &sourceId,
                                          const std::string &eventText,
                                          AES_DBO_TQSender *&tq)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>, Source ID:<%s>, event text:<%s>",
			tqName.c_str(), sourceId.c_str(), eventText.c_str() );

    unsigned int openResult = AES_NOSERVERACCESS;

    unsigned int streamSize =  ParameterHandler::instance()->getStreamSize();
    AES_GCC_TRACE_MESSAGE("create a TQ Sender, stream size:<%d>", streamSize);
    // Create a transfer queue sender object

    tq = new (std::nothrow) AES_DBO_TQSender(evTarget, streamSize, tqName, sourceId, eventText);

	if((NULL != tq) )
	{
		AES_GCC_TRACE_MESSAGE("open TQ sender");
		// open internal communication thread
		openResult = tq->open();

		if(AES_NOERRORCODE != openResult)
		{
			AES_GCC_TRACE_MESSAGE("failed to start TQ sender thread");
			delete tq;
			tq = NULL;
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>", openResult);

    return openResult;
}


//*******************************************************************
//
std::string AES_DBO_TQManager_Imp::getErrorText(unsigned int retCode)
{
	AES_GCC_TRACE_MESSAGE("Entering..., error code:<%d>", retCode);

	std::string errorText;

	switch(retCode)
    {
	    case AES_NOERRORCODE:
	    	break;
        
	    case AES_EXECUTEERROR:
	    	errorText = "Error when executing";
	    	break;
        
	    case AES_INCORRECTCOMMAND:
	    	errorText = "Error in command line";
	    	break;
        
	    case AES_TIMEOUT:
	    	errorText = "Timeout occurred";
	    	break;
        
	    case AES_INVALIDHANDLE:
	    	errorText = "The used handle is invalid";
	    	break;
        
	    case AES_BUFFERFULL:
	    	errorText = "The used buffer or stream is full";
	    	break;
        
	    case AES_BUFFERERROR:
	    	errorText = "Something has went wrong with the buffer";
	    	break;
        
	    case AES_DESTINATIONINUSE:
	    	errorText = "Destination already in use";
	    	break;

	    case AES_PROCORDEREXIST:
	    	errorText = "Transfer queue already defined";
	    	break;
        
	    case AES_NOPROCORDER:
	    	errorText = "Transfer queue not defined";
	    	break;
        
	    case AES_FILENOTFOUND:
	    	errorText = "File not found";
	    	break;

	    case AES_FILENAMEINVALID:
	    	errorText = "Invalid file name";
	    	break;
        
	    case AES_DESTINATIONEXIST:
	    	errorText = "Destination already exist";
	    	break;
        
	    case AES_NODESTINATION:
	    	errorText = "Destination does not exist";
	    	break;
        
	    case AES_INVALIDDESTNAME:
	    	errorText = "Invalid destination name";
	    	break;
        
	    case AES_NOACCESS:
	    	errorText = "Access error";
	    	break;
        
	    case AES_NOSERVERACCESS:
	    	errorText = "Unable to connect to server";
	    	break;

	    case AES_CATASTROPHIC:
	    	errorText = "Internal program error";
	    	break;

	    case AES_ILLEGALDELAYVALUE:
	    	errorText = "Illegal remove delay value";
	    	break;

	    case AES_ILLEGALRETRYVALUE:
	    	errorText = "Illegal retry value";
	    	break;

	    case AES_WRONGFILETYPE:
	    	errorText = "Wrong file type";
	    	break;

	    case AES_ILLEGALTIMEVALUE:
	    	errorText = "Illegal time value";
	    	break;

	    case AES_ILLEGALSTATUSVALUE:
	    	errorText = "Illegal status value";
	    	break;

	    case AES_FILEISPROTECTED:
	    	errorText = "File is protected by another user";
	    	break;

	    case AES_INVALIDTRANSMODE:
	    	errorText = "Transfer mode is invalid";
	    	break;

	    case AES_SEMAPHOREERR:
	    	errorText = "Semaphore error";
	    	break;

	    case AES_EVENTNOTHANDLED:
	    	errorText = "Event is not handled";
	    	break;

	    case AES_NOCDHSERVER:
	    	errorText = "No contact with CDH server";
	    	break;

		case AES_CONNECTERROR:
			errorText = "Connection error towards remote host";
			break;

		case AES_MAXTQ:
			errorText = "Too many transfer queues defined in GOH";
			break;

		case AES_DATAAREAERROR:
			errorText = "Data area error";
			break;

		case AES_INVALIDTRANSTYPE:
			errorText = "Destination set is not configured for block transfer";
			break;

		case AES_FILEISOPEN:
			errorText = "Transfer in progress";
			break;

		case AES_ERRORCHANGESTATUS:
			errorText = "Unabel to change status";
			break;

		case AES_TIMERNOTEXPERIED:
			errorText = "Timer not experied";
			break;

		case AES_UNABLETOLOCK:
			errorText = "Unable to lock file";
			break;

		case AES_UNABLETOUNLOCK:
			errorText = "Unable to unlock file";
			break;

		case AES_EVENTHANDLERSETERR:
			errorText = "Error when setting event handler in CDH";
			break;

		case AES_EVENTHANDLERREMERR:
			errorText = "Error when removing event handler in CDH";
			break;

		case AES_BUFFERISFULL:
			errorText = "The internal block buffer in GOH is full";
			break;

		case AES_TRANSACTIONNOTACTIVE:
			errorText = "No transaction is active";
			break;

		case AES_TRANSACTIONACTIVE:
			errorText = "A transaction is already active";
			break;

		case AES_NOTRANSACTIONPERFORMED:
			errorText = "No transaction has been performed";
			break;

		case AES_NOTINSYNCH:
			errorText = "GOH is not in synch with the remote rpc sever";
			break;

		case AES_INCORRECTBLOCKSIZE:
			errorText = "The specified block size is incorrect";
			break;

		case AES_TQNOTFOUND:
			errorText = "The transfer queue was not found";
			break;

		case AES_USERNOTDEFINED:
			errorText = "User not member of specified user group";
			break;

		case AES_NOTAUTHORIZED:
			errorText = "User not authorized to perform specified action";
			break;

		case AES_DESTINATIONSETEXIST:
			errorText = "Destination set already exist";
			break;

		case AES_NODESTINATIONSET:
			errorText = "Destination set does not exist";
			break;

		case AES_DESTINATIONSETINUSE:
			errorText = "Destination set already in use";
			break;

		case AES_ERRORUNKNOWN:
			errorText = "Unkown error code";
			break;

		case AES_UNREASVAL_BLOCKTQNAME:
			errorText = "Unreasonable value for blockTransferQueueId";
			break;

		case AES_UNREASVAL_BLOCKDESTSET:
			errorText = "Unreasonable value for blockDestinationSet";
			break;

		case AES_UNREASVAL_BLOCKMIRROR:
			errorText = "Unreasonable value for mirrored";
			break;

		case AES_UNREASVAL_BLOCKREMDELAY:
			errorText = "Unreasonable value for removeDelay";
			break;

		case AES_FILETQREFERRED:
			errorText = "BlocktransferQueue is protected";
			break;

		case AES_TQISPROTECTED:
			errorText = "BlocktransferQueue is protected";
			break;

		case AES_BACKUP_INPROGRESS:
			errorText = "Command not executed, AP backup in progress";
			break;

		default:
			errorText = "Unknown error code";
    }
	
	AES_GCC_TRACE_MESSAGE("...Leaving, error text:<%s>", errorText.c_str());

	return errorText;
}

void AES_DBO_TQManager_Imp::close()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

	// Make sure that we are alone
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(m_isConnectionUp)
	{
		AES_GCC_TRACE_MESSAGE("destroy opened session");
		m_Comunicator->destroy();
	}
	AES_GCC_TRACE_MESSAGE("Leaving...");
}
	
//*******************************************************************
//
void AES_DBO_TQManager_Imp::destroy()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

    // Make sure that we are alone
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(m_isConnectionUp)
	{
		AES_GCC_TRACE_MESSAGE("destroy opened session");
		m_Comunicator->destroy();
	}
	AES_GCC_TRACE_MESSAGE("Leaving...");
}

void AES_DBO_TQManager_Imp::sendRequest(ACS_APGCC_Command& cmd)
{
	AES_GCC_TRACE_MESSAGE("Entering...");

    // Make sure that we are alone
   	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if( initComunication() )
	{
		AES_GCC_TRACE_MESSAGE("send command to DBO server");
		// send the command to server
		m_Comunicator->sendCmd(cmd);

		if( AES_NOSERVERACCESS == cmd.result)
		{
			AES_GCC_TRACE_MESSAGE("connection is down, close it");
			m_isConnectionUp = false;
			m_Comunicator->close();
			AES_GCC_TRACE_MESSAGE("connection closed");
		}
	}
	else
	{
		AES_GCC_TRACE_MESSAGE("fail to connect to DBO");
		cmd.result = AES_NOSERVERACCESS;
	}

	AES_GCC_TRACE_MESSAGE("...Leaving");
}

bool AES_DBO_TQManager_Imp::initComunication()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

	// Make sure that we are alone
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

	if(!m_isConnectionUp)
	{
		if(NULL == m_Comunicator)
			 m_Comunicator = new (std::nothrow) AES_DBO_AsynchComm(NULL);

		AES_GCC_TRACE_MESSAGE(" try to connect to DBO");

		// Open communication towards DBO server
		if( (NULL != m_Comunicator) && ( m_Comunicator->open() == AES_NOERRORCODE ) )
		{
			 m_isConnectionUp = true;
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, connection to DBO <%s>",
			 	 	 	 	 (m_isConnectionUp ? "UP": "DOWN") );

	return m_isConnectionUp;
}
