
//******************************************************************************
// 
// .NAME
//  	AES_DBO_EventMsg - Encasulates event messages
// .LIBRARY 3C++
// .PAGENAME AES_DBO_EventMsg
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_EventMsg.h

// .COPYRIGHT
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
// 	This class encapsulates the event messages stored in the event queue.
//  

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 xxxx

// AUTHOR 
// 	2002-05-09 by UAB/KB/AU Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R2A_DMDN6.lib

// .SEE ALSO 
//  

//******************************************************************************

#ifndef AES_DBO_EVENTMSG_H
#define AES_DBO_EVENTMSG_H

class AES_DBO_EventMsg
{
public:
    /**
         * @brief
         * constructor with three parameters 
         */
    AES_DBO_EventMsg(unsigned int eventCode, unsigned int blockNr):
      eventCode_(eventCode), blockNr_(blockNr) {}
      
    const unsigned int eventCode_;
    const unsigned int blockNr_;
};

#endif
