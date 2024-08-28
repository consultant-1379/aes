//'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE hardlink.h

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
//      

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//    CAA 109 0420  

// AUTHOR 
//      1999-09-29	UAB/I/LD	Ulf Gustafsson

// REVISION
//			PA1		1999-09-29	ULFG		Ported to CDH

// .LINKAGE
//      

// .SEE ALSO 
//      

//*******************************************************************

#ifndef HARDLINK
#define HARDLINK

// contains os-specific stuff

#ifdef WIN32

#pragma warning( disable : 4786 )

#include <io.h>  //need
#include <windows.h> //need

int link(const char* orgFile, const char* linkFile);
// end of WIN32

#else	// not WIN32
// nothing
#endif // what os

#endif // 

// EOF hardlink.h