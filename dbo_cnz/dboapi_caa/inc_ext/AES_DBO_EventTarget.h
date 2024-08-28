
//******************************************************************************
// 
// .NAME
//  	AES_DBO_EventTarget - A base for clients. To be used for eventreception.
// .LIBRARY 3C++
// .PAGENAME AES_DBO_EventTarget
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_EventTarget.h

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
// 	This class is used as a base for client classes and makes it possible
//  for the clients to receive events from DBO.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 xxxx

// AUTHOR 
// 	2001-05-02 by UAB/KB/AU Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R1A_DMDN6.lib

// .SEE ALSO 
//  

//******************************************************************************

#ifndef AES_DBO_EVENTTARGET_H
#define AES_DBO_EVENTTARGET_H

#include <aes_gcc_eventcodes.h>

class AES_DBO_EventTarget 
{
public:
	virtual unsigned int event(unsigned int eCode) = 0;
};

#endif
