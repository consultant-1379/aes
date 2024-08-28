
//*******************************************************************
// 
// .NAME
//  	AES_DBO_TQSender
// .LIBRARY 3C++
// .PAGENAME AES_DBO_TQSender
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_TQSender.h

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
// 	This class is used by sending clients which uses the transfer queues.
//  The class makes it possible to send data through a queue.
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
// 	2002-05-27 by UAB/KB/AU Mats Nilsson

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


#include "AES_DBO_TQSender.h"
#include "ParameterHandler.h"

#include "AES_DBO_EventTarget.h"
#include "DataMirror.h"
#include "CommandCode.h"

#include <aes_gcc_tracer.h>
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_eventcodes.h>

#include <fstream>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <ACS_APGCC_Util.H>


// ACS Trace definition
AES_GCC_TRACE_DEFINE(AES_DBO_TQSender);


//*******************************************************************
//
AES_DBO_TQSender::AES_DBO_TQSender(AES_DBO_EventTarget *target,
                                   unsigned int streamSize,
                                   const std::string &tqName):
AES_DBO_AsynchComm(target, tqName),
mirrorMutex_(),
streamSize_(streamSize),
name_(tqName),
sourceId_(""),
eventText_(""),
mirror_(NULL),
m_DataStream()
{

}


//*******************************************************************
//
AES_DBO_TQSender::AES_DBO_TQSender(AES_DBO_EventTarget *target,
                                   unsigned int streamSize,
                                   const std::string &tqName,
                                   const std::string &sourceId,
                                   const std::string &eventText):
AES_DBO_AsynchComm(target,tqName),
mirrorMutex_(),
streamSize_(streamSize),
name_(tqName),
sourceId_(sourceId),
eventText_(eventText),
mirror_(NULL),
m_DataStream()
{

}


//*******************************************************************
//
AES_DBO_TQSender::~AES_DBO_TQSender()
{
	// Cleanup
    if(NULL != mirror_)
    {
    	 AES_GCC_TRACE_MESSAGE("Remove mirror, TQ<%s>", name_.c_str() );
         delete mirror_;
    }
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::openTransfer()
{
    AES_GCC_TRACE_MESSAGE("Entering..., stream Size:<%d>, sourceID:<%s>, TQ<%s>", streamSize_, sourceId_.c_str(), name_.c_str() );

    std::string tqPath(ParameterHandler::instance()->getDBODataPath());
    tqPath.append(name_);
    AES_GCC_TRACE_MESSAGE("TQ path:<%s>", tqPath.c_str());

    // Create the stream and start the thread
    if (!m_DataStream.create(name_, tqPath, streamSize_))
    {
    	AES_GCC_TRACE_MESSAGE("...Leaving, failed to create shared memory object for TQ<%s>", name_.c_str() );
        return AES_BUFFERERROR;
    }

	// send the command to server
    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_OPEN;
	cmd.data[0] = name_;
    cmd.data[1] = sourceId_;
    cmd.data[2] = eventText_;

	// send the command to server
    sendCmd(cmd);

    // Check if the data should be mirrored
    // data[0] - mirroring on / off
    unsigned int mirror = cmd.data[0];

	if (cmd.result == AES_NOERRORCODE && mirror)
    {
		// Make sure that we are alone
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mirrorMutex_);
		AES_GCC_TRACE_MESSAGE("Create mirroring object for TQ:<%s>", name_.c_str() );
        mirror_ = new (std::nothrow) DataMirror(this, name_, sourceId_);
    }

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>, TQ:<%s>", cmd.result, name_.c_str() );
    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::closeTransfer()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ<%s>", name_.c_str() );

    // send the command to server
    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_CLOSE;

	// send the command to server
    sendCmd(cmd);

    // Remove the mirror
    if(NULL != mirror_)
    {
    	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mirrorMutex_);
    	AES_GCC_TRACE_MESSAGE("Delete mirroring object for TQ:<%s>", name_.c_str() );
        delete mirror_;
        mirror_ = NULL;
    }

    m_DataStream.close();

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>, TQ:<%s>", cmd.result, name_.c_str() );
    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::transactionBegin()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ<%s>", name_.c_str() );

    // send the command to server
    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_T_BEGIN;

	// send the command to server
    sendCmd(cmd);

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>, TQ:<%s>", cmd.result, name_.c_str() );

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::transactionEnd(unsigned int &blockNr)
{
    AES_GCC_TRACE_MESSAGE("Entering..., TQ<%s>", name_.c_str() );

    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_T_END;

    blockNr = 0;

	// send the command to server
    sendCmd(cmd);

    // If all went OK, read the answer from the server
    if (cmd.result == AES_NOERRORCODE)
		blockNr = cmd.data[0];

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d> block number:<%d>, TQ:<%s>", cmd.result, blockNr, name_.c_str() );
    return cmd.result;
}

//*******************************************************************
//
unsigned int AES_DBO_TQSender::transactionCommit(unsigned int &blockNr)
{
    AES_GCC_TRACE_MESSAGE("Entering..., TQ<%s>", name_.c_str() );

    // Clear the stream
    m_DataStream.clear();

    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_T_COMMIT;

    // send the command to server
    sendCmd(cmd);

    // If all went OK, read the answer from the server
    if (cmd.result == AES_NOERRORCODE)
    {
        // Save the block number for the client
		blockNr = cmd.data[0];

        // Tell the mirror if any
        if(NULL != mirror_)
        {
        	// Acquire lock
        	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mirrorMutex_);
        	// double check after lock
        	if(NULL != mirror_)
        	{

        		AES_GCC_TRACE_MESSAGE("Mirror is on, commit mirroring data, TQ:<%s>", name_.c_str());
        		mirror_->commitIsDone();
        	}
        }
    }

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d> block number:<%d>, TQ:<%s>", cmd.result, blockNr, name_.c_str() );

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::transactionTerminate()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ<%s>", name_.c_str() );

    // Clear the stream
    m_DataStream.clear();

    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_T_TERMINATE;

	// send the command to server
    sendCmd(cmd);

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>, TQ:<%s>", cmd.result, name_.c_str() );

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::send(const AES_DBO_DataBlock *block)
{
	AES_GCC_TRACE_MESSAGE("Entering..., block number:<%d>, TQ:<%s>", block->blockNr_, name_.c_str() );

    // If no session open, reply with no server access
    if (!sessionOpen_)
    {
    	AES_GCC_TRACE_MESSAGE("...Leaving, session is not open towards DBO, TQ:<%s>", name_.c_str() );
        return AES_NOSERVERACCESS;
    }

    // Write the block to the stream
    char *ptr = (char *)block;
    unsigned int rCode = m_DataStream.write(ptr, block->length_ + block->hLength_);

    // Write the block to the mirror if any
	//HJ88370
	if((rCode == AES_NOERRORCODE) && mirror_)
	{
		// Acquire lock
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mirrorMutex_);
		// double check after lock
		if(mirror_)
		{
			rCode = mirror_->write(block->blockNr_, ptr + block->hLength_, block->length_);
			AES_GCC_TRACE_MESSAGE("Report data to mirror, result:<%d>, TQ:<%s>", rCode, name_.c_str() );
		}
	}//HJ88370 End

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>, TQ:<%s>", rCode, name_.c_str() );

    return rCode;
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::getLastCommittedBlockNr(unsigned int &blockNr)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", name_.c_str() );

    ACS_APGCC_Command cmd;
    cmd.cmdCode = CMD_GETLASTCOMMITED;

	// send the command to server
    sendCmd(cmd);

    // If all went OK, read the answer from the server
    if (cmd.result == AES_NOERRORCODE)
    {
        // Save the blocknumber for the client
		blockNr = cmd.data[0];

        // Tell the mirror if any
		if(mirror_)
		{
			// Acquire lock
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mirrorMutex_);
			// double check after lock
			if(mirror_)
			{
				AES_GCC_TRACE_MESSAGE("Mirror is on, commit mirroring data, TQ:<%s>", name_.c_str());
				mirror_->commitIsDone();
			}
		}
    }

    AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d> block number:<%d>, TQ:<%s>", cmd.result, blockNr, name_.c_str() );

    return cmd.result;
}


//*******************************************************************
//
unsigned int AES_DBO_TQSender::getEvent(unsigned int &event, 
                                        unsigned int &blockNr)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", name_.c_str() );

    unsigned int rCode = AES_EXECUTEERROR;

    // Make sure that we are alone
    ACE_Guard<ACE_Thread_Mutex> guard(m_EventMutex);
    if(!m_EventList.empty())
    {
    	std::list<eventMsg>::iterator element = m_EventList.begin();
    	event = element->event;
    	blockNr = element->blockEventNumber;
    	m_EventList.erase(element);
    	rCode = AES_NOERRORCODE;
    	AES_GCC_TRACE_MESSAGE("found event:<%d> block event:<%d>, TQ:<%s>", event, blockNr, name_.c_str() );
    }

    AES_GCC_TRACE_MESSAGE("...Leaving, TQ:<%s>", name_.c_str() );
    return rCode;
}


//*******************************************************************
//
void AES_DBO_TQSender::destroy()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", name_.c_str() );

    // call base destroy method
    AES_DBO_AsynchComm::destroy();

    AES_GCC_TRACE_MESSAGE("...Leaving, TQ:<%s>", name_.c_str() );
    // Remove the instance
    // TODO Change this way of working!
    delete this;
}


//*******************************************************************
//
void AES_DBO_TQSender::apiEvent(unsigned int evCode)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", name_.c_str() );


   // Check type of event
   // Check if there is a mirror, if so remove it.
   // Acquire lock
   ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mirrorMutex_);

    switch (evCode)
    {
		case AES_EVMIRROROFF:
			AES_GCC_TRACE_MESSAGE("Mirror OFF event, TQ:<%s>", name_.c_str() );

			if (mirror_)
			{
				AES_GCC_TRACE_MESSAGE("Removing mirror, TQ:<%s>", name_.c_str() );

				mirror_->clear();

				delete mirror_;
				mirror_ = NULL;

				// Tell the server to remove maintainer
				ACS_APGCC_Command cmd;
				cmd.cmdCode = CMD_REMOVEMIRROR;

				AES_GCC_TRACE_MESSAGE("Send command CMD_REMOVEMIRROR to DBO, TQ:<%s>", name_.c_str() );

				// send the command to server
				sendCmd(cmd);
			}
			break;

		case AES_EVMIRRORON:

			AES_GCC_TRACE_MESSAGE("Mirror ON event, TQ:<%s>", name_.c_str() );

			// Check if there is a mirror, otherwise create it.
			if (NULL == mirror_)
			{
				AES_GCC_TRACE_MESSAGE("Creating mirror object, TQ:<%s>", name_.c_str() );
				mirror_ = new (std::nothrow) DataMirror(this, name_, sourceId_);
			}
			else
			{
				AES_GCC_TRACE_MESSAGE("Mirror object already present, TQ:<%s>", name_.c_str() );
			}
			break;

		default:
			AES_GCC_TRACE_MESSAGE("unknown event code<%d>, TQ:<%s>", evCode, name_.c_str() );
    }

    AES_GCC_TRACE_MESSAGE("...Leaving, TQ:<%s>", name_.c_str() );
}

