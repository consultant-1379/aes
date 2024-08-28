
//*******************************************************************
// 
// .NAME
//  	AES_DBO_TQSender
// .LIBRARY 3C++
// .PAGENAME AES_DBO_TQSender
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_TransferQueue.h

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
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_DBO_TQSENDER_H
#define AES_DBO_TQSENDER_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "AES_DBO_AsynchComm.h"
#include "AES_DBO_DataBlock.h"

#include <aes_gcc_stream.h>
#include <aes_gcc_eventcodes.h>

#include <ace/Thread_Mutex.h>
#include <string>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
class DataMirror;
class AES_DBO_EventTarget;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */

class AES_DBO_TQSender : public AES_DBO_AsynchComm
{
 private:

	friend class AES_DBO_TQManager_Imp;

	/**
	 * @brief
	 * constructor with three parameters
	 */
	AES_DBO_TQSender(AES_DBO_EventTarget *target,
        unsigned int streamSize,
        const std::string &tqName);

	/**
	 * @brief
	 * constructor with four parameters
	 */
	AES_DBO_TQSender(AES_DBO_EventTarget *target,
        unsigned int streamSize, const std::string &tqName,
        const std::string &sourceId,
        const std::string &eventText);

        ACE_Recursive_Thread_Mutex mirrorMutex_;
        unsigned int streamSize_;
        std::string name_;
        std::string sourceId_;
        std::string eventText_;

        DataMirror* mirror_;

        // Object to manage the shared memory
        AES_GCC_Stream m_DataStream;

public:
        /**
         * @brief
         * destructor 
         */
        virtual ~AES_DBO_TQSender();

        /**
         *  openTransfer  method: This method is used to open a transfer.
         */
        unsigned int openTransfer();

        /**
         *  closeTransfer  method: This method is used to close a transfer.
         */
        unsigned int closeTransfer();

        /**
         *  transactionBegin  method: This method is used to begin a transaction.
         */
	    unsigned int transactionBegin();

	    /**
         *  transactionEnd  method: This method is used to end a transaction.
         */   
	    unsigned int transactionEnd(unsigned int &blockNr);

	    /**
         *  transactionCommit  method: This method is used to commit a transaction.
         */
	    unsigned int transactionCommit(unsigned int &blockNr);

	    /**
         *  transactionTerminate  method: This method is used to terminate a transaction.
         */ 
	    unsigned int transactionTerminate();

	    /**
         *  send method: This method is used to send data blocks.
         *  @param  block      :   comst AES_DBO_DataBlock pointer
         */
	    unsigned int send(const AES_DBO_DataBlock *block);

	    /**
         *  getLastCommittedBlockNr method: This method is used to get the last commited block number.
         *  @param  blockNr      :   unsigned int
         */ 
	    unsigned int getLastCommittedBlockNr(unsigned int &blockNr);

	    /**
         *  getEvent method: This method is used to get events.
         *  @param  event      :   unsigned int
         *  @param  blockNr      :   unsigned int
         */
	    unsigned int getEvent(unsigned int &event, unsigned int &blockNr);

	    /**
         *  destroy  method: This method is used to destroy a session.
         */
        virtual void destroy();

        /**
         *  apiEvent  method: This method is responsible for api event.
         */
        virtual void apiEvent(unsigned int evCode);
};

#endif
