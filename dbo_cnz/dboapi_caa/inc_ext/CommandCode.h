
//*******************************************************************
// 
// .NAME
//      CommandCode.h - Command codes used between server and API
// .LIBRARY 3C++
// .PAGENAME CommandCode
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CommandCode.h

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
//      Command codes used between server and API

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      19089-CAA 109

// AUTHOR 
//      2002-05-31 by UAB/KB/AU  Mats Nilsson

// .LINKAGE
//      libAES_DBO_R1_DMDN6.lib

// .SEE ALSO 
//      -

//*******************************************************************

#ifndef COMMANDCODE_H
#define COMMANDCODE_H 

// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

typedef enum
{
    CMD_OPENSESSION		= 1,
    CMD_CLOSESESSION	= 2,
    CMD_OPEN			= 3,
    CMD_CLOSE			= 4,
    CMD_DEFINE			= 5,
    CMD_LIST			= 6,
    CMD_DELETE			= 8,
    CMD_STREAMFAILED	= 9,
    CMD_EVENT			= 10,
    CMD_GETLASTBLOCK	= 11,
    CMD_EXISTS			= 12,
	CMD_T_BEGIN			= 13,
	CMD_T_END			= 14,
	CMD_T_TERMINATE		= 15,
	CMD_T_COMMIT		= 16,
	CMD_GETLASTCOMMITED = 17,
	CMD_C_DEST          = 18,
	CMD_C_MIRROR		= 19,
    CMD_C_ALL           = 20,
    CMD_MIRRORFILEBEG   = 21,
    CMD_MIRRORFILEEND   = 22,
    CMD_REMOVEMIRROR    = 23,
	CMD_EXISTS_DN		= 24,
	CMD_TQ_ATTACH   =   25,
    CMD_OK				= 128 
} CommandCode;

#endif
