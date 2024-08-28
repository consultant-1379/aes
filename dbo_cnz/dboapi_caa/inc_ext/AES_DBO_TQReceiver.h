
//*******************************************************************
// 
// .NAME
//  	AES_DBO_TQReceiver
// .LIBRARY 3C++
// .PAGENAME AES_DBO_TQReceiver
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
// 	This class is used by receiving clients which uses the transfer queues.
//  The class makes it possible to receive data through a queue.
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


#ifndef AES_DBO_TQRECEIVER_H
#define AES_DBO_TQRECEIVER_H

#include <string>
#include <aes_gcc_stream.h>
#include "AES_DBO_DataBlock.h"

class AES_DBO_TQReceiver
{
private:

	friend class AES_DBO_TQManager_Imp;

	/**
    * @brief
    * constructor with one parameter
    */
	AES_DBO_TQReceiver(const std::string &tqName);

	/**
	 * @brief
	 * destructor
	*/
	virtual ~AES_DBO_TQReceiver();

	std::string name_;

	// Object to manage the shared memory
	AES_GCC_Stream m_DataStream;

public:

        /**
         *  openTransfer method: This method is used to open transfer.
         */
        unsigned int openTransfer();

        /**
         *  closeTransfer method: This method is used to close transfer.
         */ 
        unsigned int closeTransfer();

        /**
         *  recv method: This method is used to receive a data block.
         *  @param block           : AES_DBO_DataBlock pointer
         */
	    unsigned int recv(AES_DBO_DataBlock *&block,
	    					unsigned int timeout = 0,
	    					ACE_Event* hAbortRead = NULL);

	    /**
         *  destroy method: This method is destroy a session.
         */
        virtual void destroy();
};

#endif
