//******************************************************************************
//
// NAME
//      aes_dbo_tqsession.cpp
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
//      This class handles block destinations. It inherits class OSF_Task. 
//      An instance of this class exists for every defined block
//      destination.
//      When a block destination is opened by a client, the thread in this class 
//      is started by the calling object, i.e. AES_DFO_SessionHandler, and an
//      instance of AES_DFO_Transfer is created.
//      Messages from an attached client is received thru AES_DFO_SessionHandler.
//      Messages can also be sent back to the client thru AES_DFO_SessionHandler.
//      Method init has to be called prior starting the thread in order to set
//      needed variables and the pointer to AES_DFO_SessionHandler. 

// DOCUMENT NO
//      19089-CAA 109 0423

// AUTHOR 
//      1999-12-01 by UAB/I/LN  Urban S�derberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010530  qabhall Changed due to new requirements INGO3 GOH.
//          010831  qablake INGO3 Drop 1 I5, avoid exception in sendEventToUser
//          010904  qabhall Debug print when using mutex are put within #ifdef
//			011119  qabulfg Event number changed 233xx -> 292xx
//			011129	qabulfg	Change in open() and close() to solve HC83269
//			011130  qabulfg Change in Handle_Failed. 
//			011203	qabulfg	Try and catch used in function sendEventToUser

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include "aes_dbo_tqsession.h"

#include "aes_dbo_clientsession.h"
#include "aes_dbo_macros.h"
#include "aes_dbo_server.h"
#include "aes_dbo_tqdatabase.h"

#include "CommandCode.h"
#include "EventCode.h"

#include <aes_gcc_errorcodes.h>
#include <aes_gcc_tracer.h>


GCC_TDEF(AES_DBO_TQsession);
AES_GCC_TRACE_DEFINE(AES_DBO_TQsession);


//******************************************************************************
//
aes_dbo_tqsession::aes_dbo_tqsession(const std::string &tq):
tq_(tq),
dest_(NULL),
client_(NULL),
cdhConnectionLost_(),
tqOpen_(false),
mirrored_(false),
mirror_(tq),
tqMutex()
{
	AES_GCC_TRACE_MESSAGE("Creating new TQ: %s", tq.c_str());

    // Add a mirror maintainer if needed
    unsigned int mirrored = aes_dbo_tqdatabase::instance()->mirrored(tq_);
    if (mirrored)
    {
        int delay;
        aes_dbo_tqdatabase::instance()->getRemoveDelay(tq_, delay);
        mirror_.create(delay);

        // Set mirrored flag to yes
        mirrored_ = true;
    }
}


//******************************************************************************
//
aes_dbo_tqsession::~aes_dbo_tqsession() 
{
	AES_GCC_TRACE_MESSAGE("Destroying tq:<%s>", tq_.c_str());

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	if(mirror_.running())
	{
		mirror_.remove();

		if(!aes_dbo_server::isStopEventSignalled)
		{
			mirror_.abort();
		}
		else
		{
			mirror_.forceExit();
		}

		mirror_.wait();
	}
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::open(aes_dbo_clientsession *client, const string &sourceId,const string &eventText)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	AES_GCC_TRACE_MESSAGE("Opening TQ:<%s>, mirrored = %s", tq_.c_str(), (mirrored_ ? "TRUE" : "FALSE"));

    // Check if the file is already opened
    if (aes_dbo_tqdatabase::instance()->isOpened(tq_))
    {
        GCC_TERROR((AES_DBO_TQsession, "(%t) aes_dbo_tqsession::open() Error: more than one user for TQ: %s\n", tq_.c_str()));
        return AES_TQISOPEN;
    }

    // Get the destination that is tied to the TQ
    std::string destination;
    if (aes_dbo_tqdatabase::instance()->getDestinationSet(tq_, destination) == AES_DESTINATIONSETEXIST)
    {
        // Create destination set object
        dest_ = new (std::nothrow) aes_dbo_destinationset(tq_, destination, sourceId, eventText, this);

        if(NULL == dest_)
        {
        	AES_GCC_TRACE_MESSAGE("Failed to allocate dest_ for TQ:<%s>", tq_.c_str());
            return AES_ERRORUNKNOWN;
        }

        if(dest_->openTransfer() != 0 )
        {
        	AES_GCC_TRACE_MESSAGE("TQ: %s couldn't be opened...", tq_.c_str());
            delete dest_;
            dest_ = NULL;
            cdhConnectionLost_.reset();
            return AES_ERRORUNKNOWN;
        }
    }
    else
    {
    	AES_GCC_TRACE_MESSAGE("Error. Destination set not found...");
        return AES_NODESTINATION;
    }

    // Save the client
    client_ = client;

    // Mark TQ open
    tqOpen_ = true;

    // Mark file as opened
    aes_dbo_tqdatabase::instance()->open(tq_);

    AES_GCC_TRACE_MESSAGE("TQ:<%s> is opened!", tq_.c_str());
    return AES_NOERRORCODE;
}


//******************************************************************************
//
void aes_dbo_tqsession::close()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	AES_GCC_TRACE_MESSAGE("Closing TQ:<%s>", tq_.c_str());

	if(tqOpen_)
	{
		tqOpen_ = false; // Set this flag to false to avoid double close during shutdown.

		dest_->closeTransfer();
		// Waiting lost connection signal
		cdhConnectionLost_.wait();
		AES_GCC_TRACE_MESSAGE("aes_cdh_asynchreceiver thread successfully exited for TQ:<%s>", tq_.c_str());
		delete dest_;
		dest_ = NULL;

		client_ = NULL;  // Set the client pointer to NULL but only after asynchreceiver's exit

		// Mark transfer queue as closed
		aes_dbo_tqdatabase::instance()->close(tq_);

		AES_GCC_TRACE_MESSAGE("Closed TQ:<%s>", tq_.c_str());
	}
	else
	{
		AES_GCC_TRACE_MESSAGE("TQ:<%s> already closed", tq_.c_str());
	}
}

//******************************************************************************
//
void aes_dbo_tqsession::lostCdhConnection()
{
	AES_GCC_TRACE_MESSAGE("Signaling lost of cdh connection, for TQ:<%s>", tq_.c_str());
	cdhConnectionLost_.signal();
}

//******************************************************************************
//
void aes_dbo_tqsession::sendEventToUser(unsigned int evCode, unsigned int blockNr)
{
	AES_GCC_TRACE_MESSAGE("Sending Event:<%d>, Block nr:<%d>", evCode, blockNr);

	if (NULL != client_)
	{
		ACS_APGCC_Command cmd;
		cmd.cmdCode = CMD_EVENT;
		cmd.result = AES_NOERRORCODE;
		cmd.data[0] = evCode;
		cmd.data[1] = blockNr;
		if (NULL != client_)
		{
			client_->sendReply(cmd);
		}
	}
}

//******************************************************************************
//
void aes_dbo_tqsession::changeMirror(unsigned int shouldMirror)
{
	AES_GCC_TRACE_MESSAGE("Should be mirrored: %s", shouldMirror ? "yes" : "no");

    // Add or remove the mirror maintainer
    if (!mirrored_ && shouldMirror)
    {
        // Create a maintainer
        int delay;
        aes_dbo_tqdatabase::instance()->getRemoveDelay(tq_, delay);
        mirror_.create(delay);

        // Turn mirroring on in the API
        sendEventToUser(AES_EVMIRRORON | AES_APIEVENT, 0);
        mirrored_ = true;
    }
    else if (mirrored_ && !shouldMirror)
    {
        // Turn mirroring off in the API
        sendEventToUser(AES_EVMIRROROFF | AES_APIEVENT, 0);

        // If there is no client, remove the maintainer directly
        if (!client_)
        {
            // Deactivate the maintainer
        	AES_GCC_TRACE_MESSAGE("Error. No client removing maintainer");
            mirror_.remove();
            mirrored_ = false;
        }
    }
}


//******************************************************************************
//
void aes_dbo_tqsession::changeDelay(unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("Changing Delay: %d", delay);

    // Change the delay in the maintainer
    if (mirrored_)
        mirror_.changeDelay(delay);
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::transactionBegin()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	AES_GCC_TRACE_MESSAGE("Transaction BEGIN");

	unsigned int rCode = AES_ERRORUNKNOWN;

	if( NULL != dest_ )
	{
		rCode = dest_->transactionBegin();

		switch (rCode)
		{
		case AES_CDH_RC_OK:
			rCode = AES_NOERRORCODE;
			break;

		case AES_CDH_RC_NOSERVER:
			rCode = AES_NOCDHSERVER;
			break;

		case AES_CDH_RC_INCUSAGE:
			rCode = AES_INCUSAGE;
			break;

		case AES_CDH_RC_CONNECTERR:
			rCode = AES_CONNECTERROR;
			break;

		default:
			rCode = AES_ERRORUNKNOWN;
		}
	}

    AES_GCC_TRACE_MESSAGE("Transaction Begin Completed Result : %d", rCode);

    return rCode;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::transactionEnd(unsigned int &blockNr)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	AES_GCC_TRACE_MESSAGE("Transaction END");

	unsigned int rCode = AES_ERRORUNKNOWN;

	if( NULL != dest_ )
	{
		rCode = dest_->transactionEnd(blockNr);

		switch (rCode)
		{
		case AES_CDH_RC_OK:
			rCode = AES_NOERRORCODE;
			break;

		case AES_CDH_RC_NOSERVER:
			rCode = AES_NOCDHSERVER;
			break;

		case AES_CDH_RC_INCUSAGE:
			rCode = AES_INCUSAGE;
			break;

		case AES_CDH_RC_CONNECTERR:
			rCode = AES_CONNECTERROR;
			break;

		default:
			rCode = AES_ERRORUNKNOWN;
		}
	}

    AES_GCC_TRACE_MESSAGE("Transaction End Completed Result : %d", rCode);

    return rCode;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::transactionTerminate()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	AES_GCC_TRACE_MESSAGE("Transaction TERMINATE");

	unsigned int rCode = AES_ERRORUNKNOWN;

	if( NULL != dest_ )
	{
		rCode = dest_->transactionTerminate();

		switch (rCode)
		{
		case AES_CDH_RC_OK:
			rCode = AES_NOERRORCODE;
			break;

		case AES_CDH_RC_NOSERVER:
			rCode = AES_NOCDHSERVER;
			break;

		case AES_CDH_RC_INCUSAGE:
			rCode = AES_INCUSAGE;
			break;

		case AES_CDH_RC_CONNECTERR:
			rCode = AES_CONNECTERROR;
			break;

		default:
			rCode = AES_ERRORUNKNOWN;
		}
	}

    AES_GCC_TRACE_MESSAGE("Transaction End Completed Result : %d", rCode);

    return rCode;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::transactionCommit(unsigned int &blockNr)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	AES_GCC_TRACE_MESSAGE("Transaction COMMIT");

	unsigned int rCode = AES_ERRORUNKNOWN;

	if( NULL != dest_ )
	{
		rCode = dest_->transactionCommit(blockNr);

		switch (rCode)
		{
		case AES_CDH_RC_OK:
			rCode = AES_NOERRORCODE;
			break;

		case AES_CDH_RC_NOSERVER:
			rCode = AES_NOCDHSERVER;
			break;

		case AES_CDH_RC_INCUSAGE:
			rCode = AES_INCUSAGE;
			break;

		case AES_CDH_RC_CONNECTERR:
			rCode = AES_CONNECTERROR;
			break;

		case AES_CDH_RC_NOBLOCKNOAVAILABLE:
			rCode = AES_BLOCKNRNOTAVAILABLE;
			break;

		default:
			rCode = AES_ERRORUNKNOWN;
		}
	}

	AES_GCC_TRACE_MESSAGE("Transaction Commit Completed Result : %d", rCode);

	return rCode;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::getLastCommittedBlock(unsigned int &blockNr)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(tqMutex);

	AES_GCC_TRACE_MESSAGE("Get last committed block info");

	unsigned int rCode = AES_ERRORUNKNOWN;

	if( NULL != dest_ )
	{
		rCode = dest_->getLastCommittedBlock(blockNr);

		switch (rCode)
		{
		case AES_CDH_RC_OK:
			rCode = AES_NOERRORCODE;
			break;

		case AES_CDH_RC_NOSERVER:
			rCode = AES_NOCDHSERVER;
			break;

		case AES_CDH_RC_INCUSAGE:
			rCode = AES_INCUSAGE;
			break;

		case AES_CDH_RC_CONNECTERR:
			rCode = AES_CONNECTERROR;
			break;

		case AES_CDH_RC_NOBLOCKNOAVAILABLE:
			rCode = AES_BLOCKNRNOTAVAILABLE;
			break;

		default:
			rCode = AES_ERRORUNKNOWN;
		}
	}

    AES_GCC_TRACE_MESSAGE("Get Last committed Block Completed Block Number: %u Result : %d", blockNr, rCode);

    return rCode;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::newMirrorFile(const string &fName)
{
	AES_GCC_TRACE_MESSAGE("New Mirror File");

    // Report the mirror file if a mirror exists
    unsigned int rCode;
    if (mirrored_)
        rCode = mirror_.newFile(fName);
    else
        rCode = AES_INCUSAGE;

    AES_GCC_TRACE_MESSAGE("Operation completed. New Mirror File Result: %d\n", rCode);
    return rCode;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::closeMirrorFile(const string &fName)
{
	AES_GCC_TRACE_MESSAGE("Closing Mirror file: %s", fName.c_str());

    // Report the mirror file if a mirror exists
    unsigned int rCode;
    if (mirrored_)
        rCode = mirror_.closeFile(fName);
    else
        rCode = AES_INCUSAGE;

    AES_GCC_TRACE_MESSAGE("Operation completed. Close Mirror File Result: %d\n", rCode);
    return rCode;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::removeMirror()
{
	AES_GCC_TRACE_MESSAGE("Removing Mirror");

    // Deactivate the maintainer
    mirror_.remove();
    mirrored_ = false;
    return AES_NOERRORCODE;
}


//******************************************************************************
//
unsigned int aes_dbo_tqsession::checkForRemoval()
{
	AES_GCC_TRACE_MESSAGE("Checking for removal");

    unsigned int rCode = AES_NOERRORCODE;
    AES_GCC_TRACE_MESSAGE("Mirror %s running",(mirror_.running() ? "" : "not"));
    if (mirror_.running())
    {
        // Deactivate the maintainer
        mirror_.abort();
        mirror_.wait();

        // Check the data area in the mirror for removal
        rCode = mirror_.checkDataArea();

        // If it failed start the mirror again
        if (rCode != AES_NOERRORCODE)
        {
        	AES_GCC_TRACE_MESSAGE("Check for data area failed and starting the mirror again");
            mirror_.create();
            return rCode;
        }
    }
    else
    {
    	AES_GCC_TRACE_MESSAGE("check for data area");
        // Check the data area in the mirror for removal
        rCode = mirror_.checkDataArea();
    }

    AES_GCC_TRACE_MESSAGE("check completed, result: %d", rCode);
    return rCode;
}


//******************************************************************************
//
void aes_dbo_tqsession::resetClient()
{
	AES_GCC_TRACE_MESSAGE("Setting client_ to NULL");
	client_ = NULL;
}
