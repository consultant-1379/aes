
//*******************************************************************
// 
// .NAME
//      EventCode.h - Command codes used between server and API
// .LIBRARY 3C++
// .PAGENAME EventCode
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE EventCode.h

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

#ifndef EVENTCODE_H
#define EVENTCODE_H 

typedef enum
{
    AL_DBOFAULT		    = 29200,
    EV_THREADERROR      = 29210,
    EV_MUTEXERROR       = 29211,
    EV_FILECREATERROR   = 29215,
    EV_FILEOPENERROR    = 29216,
    EV_FILEUPDATEERROR  = 29217,
    EV_DESTNOTFOUND     = 29218,
    EV_DESTOPENERROR    = 29219,
    EV_TQOPENERROR      = 29220,
    EV_OHIERROR         = 29221,
    EV_AFPSTATUSERROR   = 29224,
    EV_MAINFILENOTINAFP = 29225,
    EV_SYNCHERROR       = 29226,
    EV_CONVERTERROR     = 29227,
    EV_CORRUPTFILE      = 29228,
    EV_FILELOCKEDINAFP  = 29230,
    EV_FILUNLOCKEDINAFP = 29231,
    EV_FILENOTINAFP     = 29232,
    EV_FILELOCKERROR    = 29233,
    EV_FILEREMOVEERROR  = 29234,
    EV_PARAMNOTFOUND    = 29250
} EventCode;

#endif
