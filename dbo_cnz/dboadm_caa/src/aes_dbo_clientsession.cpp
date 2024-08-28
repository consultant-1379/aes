
//******************************************************************************
//
// NAME
//      aes_dbo_clientsession.cpp
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
//      This class handles communication towards clients. It inherits class 
//      OSF_Task.
//      An instance of this class is created for each attached client, that 
//      is commands and applications sending blocks.
//      If an block destination is opened the corresponding AES_DFO_cpfcom
//      objects thread is initialised and started. A reference to this object
//      is passed to the AES_DFO_cpfcom object, so that it can send messages
//      back to the application.

// DOCUMENT NO
//      19089-CAA 109 0423

// AUTHOR 
//      1999-11-18 by UAB/I/LN  Urban S�derberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010530  qabhall Changed due to new requirements INGO3 GOH.
//          010831  qablake INGO3 Drop 1 I5, avoid exception in sendEventToUser
//			011129	qabulfg HC83269 Sleep added in svc loop case CMD_CLOSE.

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include "aes_dbo_clientsession.h"
#include "aes_dbo_tqsession.h"
#include "aes_dbo_opentqsessions.h"

#include "aes_dbo_server.h"
#include "aes_dbo_macros.h"
#include "CommandCode.h"

#include <aes_gcc_errorcodes.h>
#include "aes_gcc_tracer.h"

AES_GCC_TRACE_DEFINE(AES_DBO_Clientsession)
GCC_TDEF(AES_DBO_Clientsession);

//******************************************************************************
//
aes_dbo_clientsession::aes_dbo_clientsession(int& stopEvent):
hSendMX(),
tqSession_(NULL),
tqOwner_(false),
destName(""),
fRec(),
cmd(),
stopEvent_(stopEvent)
{
	AES_GCC_TRACE_MESSAGE("Constructor");
}


//******************************************************************************
//
aes_dbo_clientsession::~aes_dbo_clientsession() 
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(hSendMX);
	AES_GCC_TRACE_MESSAGE("Destructor");
}


/*============================================================================
        ROUTINE: open
 ============================================================================ */
int aes_dbo_clientsession::open(void *args)
{
        // To avoid warning about unused parameter
        UNUSED(args);
        int result;

        // start event loop by svc thread
        result = activate( (THR_NEW_LWP| THR_DETACHED | THR_INHERIT_SCHED));

        // Check if the svc thread is started
        if(0 != result)
        {
	        AES_GCC_TRACE_MESSAGE("open(), error on start svc thread");
        }

	AES_GCC_TRACE_MESSAGE("Leaving open()");

        return result;
}

//******************************************************************************
//
int aes_dbo_clientsession::sendReply(ACS_APGCC_Command& cmd) 
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(hSendMX);
	AES_GCC_TRACE_MESSAGE("Sending replay to client");
	ACE_INT32 rcode = 0;
	rcode = cmd.send(m_cmdStreamIO);
	AES_GCC_TRACE_MESSAGE("Sent command with CMD_CODE:%d, CMD_RESULT:%d - result=%d", cmd.cmdCode, cmd.result, rcode);
	return rcode;
}


//******************************************************************************
//
int aes_dbo_clientsession::svc()
{
	AES_GCC_TRACE_MESSAGE("Entering in svc: thread activated!");

	acs_dsd::HANDLE dsdHandle;
	int dsdHandleCount = 1;

	const nfds_t nfds = 2;
	struct pollfd fds[nfds];
	ACE_INT32 ret;

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	m_cmdStreamIO.get_handles(&dsdHandle, dsdHandleCount);

	fds[0].fd = stopEvent_;
	fds[0].events = POLLIN | POLLRDHUP;

	fds[1].fd = dsdHandle;
	fds[1].events = POLLIN | POLLRDHUP;

	bool exit_ = false;
	while (!exit_)
	{
		ret = ACE_OS::poll(fds, nfds);

		if( -1 == ret )
		{
			if(errno == EINTR)
			{
				continue;
			}
			else
			{
				GCC_TERROR((AES_DBO_Clientsession,"(%t) aes_dbo_clientsession::svc() poll error\n"));

				if( tqSession_ && (!aes_dbo_server::isStopEventSignalled) )
				{
					tqSession_->close();
				}
				// exit from the while loop
				break;
			}
		}

		if( (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL | POLLRDHUP)) || (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL | POLLRDHUP)) )
		{
			GCC_TERROR((AES_DBO_Clientsession,"(%t) aes_dbo_clientsession::svc() poll error\n"));

			if( tqSession_ && (!aes_dbo_server::isStopEventSignalled) )
			{
				tqSession_->close();
			}
			// exit from the while loop
			break;
		}

		// Server shutdown request
		if(fds[0].revents & POLLIN)
		{
			AES_GCC_TRACE_MESSAGE("Received a stop request from server");
			// The server will take care to close open tqsessions
			// exit from the while loop
			break;
		}

		// Data ready to read
		if(fds[1].revents & POLLIN)
		{
			if( cmd.recv(m_cmdStreamIO) != 0)
			{
				// Client is lost
				if ( tqSession_ && tqOwner_ && (!aes_dbo_server::isStopEventSignalled) )
				{
					tqSession_->close(); // Close tq if this session owns the tqSession
				}
				// exit from the while loop
				break;
			}

			switch (cmd.cmdCode)
			{
			case CMD_OPENSESSION:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_OPENSESSION command");
				cmd.result = AES_NOERRORCODE;
				cmd.cmdCode = CMD_OPENSESSION + CMD_OK;
				sendReply(cmd);
				break;

			case CMD_CLOSESESSION:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_CLOSESESSION command");
				cmd.result = AES_NOERRORCODE;
				cmd.cmdCode = CMD_CLOSESESSION + CMD_OK;
				if( tqSession_ && (!aes_dbo_server::isStopEventSignalled) )
				{
					AES_GCC_TRACE_MESSAGE("CMD_CLOSE not received forcing to close Tqsession.");
					tqSession_->close();
					tqSession_ = NULL;
					tqOwner_ = false;
				}
				sendReply(cmd);
				exit_ = true;
				break;

			case CMD_OPEN:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_OPEN command");
				onOpen(cmd);
				break;

			case CMD_CLOSE:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_CLOSE command");
				onClose(cmd);
				break;

			case CMD_EXISTS:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_EXISTS command");
				onExists(cmd);
				break;

			case CMD_EXISTS_DN:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_EXISTS_DN command");
				onExistsDN(cmd);
				break;

			case CMD_DEFINE:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_DEFINE command");
				onDefine(cmd);
				break;

			case CMD_LIST:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_LIST command");
				onList(cmd);
				break;

			case CMD_DELETE:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_DELETE command");
				onDelete(cmd);
				break;

			case CMD_C_DEST:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_C_DEST command");
				onChangeDest(cmd);
				break;

			case CMD_C_MIRROR:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_C_MIRROR command");
				onChangeMirror(cmd);
				break;

			case CMD_C_ALL:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_C_ALL command");
				onChangeAll(cmd);
				break;

			case CMD_T_BEGIN:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_T_BEGIN command");
				onTBegin(cmd);
				break;

			case CMD_T_END:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_T_END command");
				onTEnd(cmd);
				break;

			case CMD_T_TERMINATE:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_T_TERMINATE command");
				onTTerminate(cmd);
				break;

			case CMD_T_COMMIT:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_T_COMMIT command");
				onTCommit(cmd);
				break;

			case CMD_GETLASTCOMMITED:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_GETLASTCOMMITED command");
				onGetLast(cmd);
				break;

			case CMD_MIRRORFILEBEG:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_MIRRORFILEBEG command");
				onMirrorFileBeg(cmd);
				break;

			case CMD_MIRRORFILEEND:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_MIRRORFILEEND command");
				onMirrorFileEnd(cmd);
				break;

			case CMD_REMOVEMIRROR:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_REMOVEMIRROR command");
				onRemoveMirror(cmd);
				break;
			case CMD_TQ_ATTACH:
				AES_GCC_TRACE_MESSAGE("Handling of CMD_TQ_ATTACH command");
				onIsTQAttached(cmd);
				break;

			default:
				// Unknown command code
				AES_GCC_TRACE_MESSAGE("Unknown command code!");
				cmd.result = AES_CDH_RC_UNREAS;
				sendReply(cmd);
			}
		}
	}// Exit thread
	// call 'close(u_long)' at exit of 'svc'
	return 0;
}

//******************************************************************************
//
int aes_dbo_clientsession::close(u_long)
{
	AES_GCC_TRACE_MESSAGE("Closing svc thread, deleting this");
	if(NULL != tqSession_)
	{
		tqSession_->resetClient();
	}
	delete this;
	return 0;
}


//******************************************************************************
//
void aes_dbo_clientsession::onOpen(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Opening Tq");
    string tq = (std::string)cmd.data[0];

    if(!aes_dbo_server::isStopEventSignalled)
    {
    	// Check if transfer queue is open in opentqsessions
    	tqSession_ = aes_dbo_opentqsessions::instance()->exists(tq.c_str());
    }
    else
    {
    	tqSession_ = NULL;
    }

    if(tqSession_ == NULL)
    {
    	AES_GCC_TRACE_MESSAGE("Error. TQ requested not found");
    	cmd.result = AES_NODESTINATION;
    }
    else
    {
        if (cmd.numOfData() == 1)
        {
          cmd.result = tqSession_->open(this);
          AES_GCC_TRACE_MESSAGE("Opened TQ without any parameters");
        }

        else
        {
            string sourceId = (std::string)cmd.data[1];
            string eventText = (std::string)cmd.data[2];
            cmd.result = tqSession_->open(this, sourceId, eventText);
            cmd.data[0] = aes_dbo_tqdatabase::instance()->mirrored(tq);
            AES_GCC_TRACE_MESSAGE("Opened TQ with source id '%s' and eventText '%s'",sourceId.c_str(),eventText.c_str());
        }
    }

    // Check if the TQSession is OK to use
    if (cmd.result != AES_NOERRORCODE)
        tqSession_ = NULL;
    else
        tqOwner_ = true;

    cmd.cmdCode = CMD_OPEN + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onClose(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Closing TQ");

	if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
		cmd.result = AES_NODESTINATION;
    else
    {
        tqSession_->close();
        tqSession_ = NULL;
        cmd.result = AES_NOERRORCODE;
        tqOwner_ = false;
    }
    cmd.cmdCode = CMD_CLOSE + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);
}


//******************************************************************************
//
void aes_dbo_clientsession::onExists(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("verifying if exists");

    string tq = (std::string)cmd.data[0];
    cmd.clear();
    cmd.result = AES_TQNOTFOUND;
    cmd.cmdCode = CMD_EXISTS + CMD_OK;

    // Check if it is defined in the database
    if (aes_dbo_tqdatabase::instance()->isDefined(tq))
    {
        cmd.result = AES_NOERRORCODE;
        cmd.data[0] = aes_dbo_tqdatabase::instance()->mirrored(tq);
    }

    // Send the reply
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}

void aes_dbo_clientsession::onIsTQAttached(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("verifying if TQ is attached");
	string destSet = (std::string)cmd.data[0];
	cmd.clear();
	cmd.result = AES_NOERRORCODE;
	cmd.cmdCode = CMD_TQ_ATTACH + CMD_OK;
	if(aes_dbo_tqdatabase::instance()->isTQAttached(destSet))
	{
		cmd.result = AES_TQISPROTECTED;
	}
	AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
	sendReply(cmd);
}


//******************************************************************************
//
void aes_dbo_clientsession::onExistsDN(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("verifying if exists DN");

    string tq = (std::string)cmd.data[0];
    cmd.clear();
    cmd.result = AES_TQNOTFOUND;
    cmd.cmdCode = CMD_EXISTS_DN + CMD_OK;

    string tqDN;

    // Check if it is defined in the database
    if (aes_dbo_tqdatabase::instance()->isDefined(tq, tqDN))
    {
    	cmd.result = AES_NOERRORCODE;
    	cmd.data[0] = aes_dbo_tqdatabase::instance()->mirrored(tq);
    	cmd.data[1] = tqDN;
    }

    // Send the reply
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onDefine(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Define new TQ");

    string tq = (std::string)cmd.data[0];
    string dest = (std::string)cmd.data[1];
    unsigned int mirror = cmd.data[2];
    unsigned int delay = cmd.data[3];
    string userGroup = (std::string)cmd.data[4];

    // Check if the destination is OK in CDH
    AES_CDH_DestinationSet d(dest);
    unsigned int rCode = d.exists(AES_CDH_DestinationSet::BLOCK);
    if (rCode != AES_CDH_RC_OK)
    {
        switch (rCode)
        {
        case AES_CDH_RC_NODESTSET:
            cmd.result = AES_NODESTINATIONSET;
            break;

        case AES_CDH_RC_NOSERVER:
            cmd.result = AES_NOCDHSERVER;
            break;

        case AES_CDH_RC_NOTBLOCKDESTSET:
            cmd.result = AES_INVALIDTRANSTYPE;
            break;
        }
        cmd.cmdCode = CMD_DEFINE + CMD_OK;
        sendReply(cmd);
        return;
    }

    cmd.result = aes_dbo_tqdatabase::instance()->defineTQ(tq, dest, mirror, delay);
    cmd.cmdCode = CMD_DEFINE + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);

    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onList(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Verify in List");
    
    string tqName = (std::string)cmd.data[0];
    cmd.clear();
    cmd.result = AES_NOERRORCODE;
    cmd.cmdCode = CMD_LIST + CMD_OK;
    aes_dbo_tqdatabase::instance()->tqList(cmd, tqName);

    //TODO - VERIFY IF NEED CHECK RETRUN CODE
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onDelete(ACS_APGCC_Command &cmd)
{
    string user = (std::string)cmd.data[0];
    string tq = (std::string)cmd.data[1];
    AES_GCC_TRACE_MESSAGE("Deleting TQ %s requested", tq.c_str());

    cmd.result = aes_dbo_tqdatabase::instance()->deleteTQ(tq);
    cmd.cmdCode = CMD_DELETE + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onChangeDest(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Changing destination set");

    string user = (std::string)cmd.data[0];
    string tq = (std::string)cmd.data[1];
    string dest = (std::string)cmd.data[2];

    // Check if the destination is OK in CDH
    AES_CDH_DestinationSet d(dest);
    unsigned int rCode = d.exists(AES_CDH_DestinationSet::BLOCK);
    if (rCode != AES_CDH_RC_OK)
    {
        switch (rCode)
        {
        case AES_CDH_RC_NODESTSET:
            cmd.result = AES_NODESTINATIONSET;
            break;

        case AES_CDH_RC_NOSERVER:
            cmd.result = AES_NOCDHSERVER;
            break;

        case AES_CDH_RC_NOTBLOCKDESTSET:
            cmd.result = AES_INVALIDTRANSTYPE;
            break;
        }
        cmd.cmdCode = CMD_C_DEST + CMD_OK;
        AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
        sendReply(cmd);
        return;
    }

    cmd.result = aes_dbo_tqdatabase::instance()->changeTQDest(tq, dest);
    cmd.cmdCode = CMD_C_DEST + CMD_OK;
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onChangeMirror(ACS_APGCC_Command &cmd)
{

    AES_GCC_TRACE_MESSAGE("Changing mirroring");

    string user = (std::string)cmd.data[0];
    string tq = (std::string)cmd.data[1];
    unsigned int mirror = cmd.data[2];
    unsigned int delay = cmd.data[3];

    // Change only the necessary
    cmd.result = aes_dbo_tqdatabase::instance()->changeTQMirror(tq, mirror);
    if (cmd.result == AES_NOERRORCODE)
    {
        if (mirror == 1)
            cmd.result = aes_dbo_tqdatabase::instance()->changeTQDelay(tq, delay);
        
        // Get the active tq-session
        tqSession_ = aes_dbo_opentqsessions::instance()->exists(tq.c_str());
        if (tqSession_)
        {
            // Change the necessary parameters
            tqSession_->changeMirror(mirror);
            if (mirror == 1)
                tqSession_->changeDelay(delay);
        }
    }
    cmd.cmdCode = CMD_C_MIRROR + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onChangeAll(ACS_APGCC_Command &cmd)
{
		AES_GCC_TRACE_MESSAGE("Changing all");

    string user = (std::string)cmd.data[0];
    string tq = (std::string)cmd.data[1];
    string dest = (std::string)cmd.data[2];
    unsigned int mirror = cmd.data[3];
    unsigned int delay = cmd.data[4];

    // Check if the destination is OK in CDH
    AES_CDH_DestinationSet d(dest);
    unsigned int rCode = d.exists(AES_CDH_DestinationSet::BLOCK);
    if (rCode != AES_CDH_RC_OK)
    {
        switch (rCode)
        {
        case AES_CDH_RC_NODESTSET:
            cmd.result = AES_NODESTINATIONSET;
            break;

        case AES_CDH_RC_NOSERVER:
            cmd.result = AES_NOCDHSERVER;
            break;

        case AES_CDH_RC_NOTBLOCKDESTSET:
            cmd.result = AES_INVALIDTRANSTYPE;
            break;
        }
        cmd.cmdCode = CMD_C_ALL + CMD_OK;
        AES_GCC_TRACE_MESSAGE("Sending replay...command code = %d", cmd.cmdCode);
        sendReply(cmd);
        return;
    }

    cmd.result = aes_dbo_tqdatabase::instance()->changeTQ(tq, dest, mirror, delay);
    if (cmd.result == AES_NOERRORCODE)
    {
        // Get the active tq-session
        tqSession_ =aes_dbo_opentqsessions::instance()->exists(tq.c_str());
        if (tqSession_)
        {
            // Change the parameters
            tqSession_->changeDelay(delay);
            tqSession_->changeMirror(mirror);
        }
    }
    cmd.cmdCode = CMD_C_ALL + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onTBegin(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Transaction BEGIN");

    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATION;
    else
        cmd.result = tqSession_->transactionBegin();
    cmd.cmdCode = CMD_T_BEGIN + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onTEnd(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Transaction END");

    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATION;
    else
    {
        unsigned int blockNr = 0;
        cmd.result = tqSession_->transactionEnd(blockNr);
        cmd.data[0] = blockNr;
    }
    cmd.cmdCode = CMD_T_END + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);
}


//******************************************************************************
//
void aes_dbo_clientsession::onTTerminate(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Transaction TERMINATE");

    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATION;
    else
        cmd.result = tqSession_->transactionTerminate();
    cmd.cmdCode = CMD_T_TERMINATE + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onTCommit(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Transaction COMMIT");

    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATION;
    else
    {
        unsigned int blockNr = 0;
        cmd.result = tqSession_->transactionCommit(blockNr);
        cmd.data[0] = blockNr;
    }
    cmd.cmdCode = CMD_T_COMMIT + CMD_OK;
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onGetLast(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Get last committed block info");

    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATION;
    else
    {
        unsigned int blockNr = 0;
        cmd.result = tqSession_->getLastCommittedBlock(blockNr);
        cmd.data[0] = blockNr;
    }
    cmd.cmdCode = CMD_GETLASTCOMMITED + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onMirrorFileBeg(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("New Mirror File Beg");

    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATIONSET;
    else
    {
        // Report the new mirrorfile to the TQ session
        string fName = (std::string)cmd.data[0];
        cmd.result = tqSession_->newMirrorFile(fName);
    }
    cmd.cmdCode = CMD_MIRRORFILEBEG + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);   
}


//******************************************************************************
//
void aes_dbo_clientsession::onMirrorFileEnd(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Closing Mirror file");
    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATION;
    else
    {
        // Report the new mirrorfile to the TQ session
        string fName = (std::string)cmd.data[0];
        cmd.result = tqSession_->closeMirrorFile(fName);
    }
    cmd.cmdCode = CMD_MIRRORFILEEND + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);
}


//******************************************************************************
//
void aes_dbo_clientsession::onRemoveMirror(ACS_APGCC_Command &cmd)
{
	AES_GCC_TRACE_MESSAGE("Removing Mirror");

    if( (tqSession_ == NULL) || (aes_dbo_server::isStopEventSignalled) )
        cmd.result = AES_NODESTINATION;
    else
    {
        // Tell the TQ session to remove the maintainer
        cmd.result = tqSession_->removeMirror();
    }
    cmd.cmdCode = CMD_REMOVEMIRROR + CMD_OK;
    AES_GCC_TRACE_MESSAGE("Operation completed. Sending replay...command code = %d", cmd.cmdCode);
    sendReply(cmd);
}
