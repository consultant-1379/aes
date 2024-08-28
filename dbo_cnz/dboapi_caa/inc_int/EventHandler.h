//******************************************************************************
// 
// .NAME
//      EventHandler - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME EventHandler
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

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
//      <General description of the class>

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      19089-CAA 109 0319

// AUTHOR 
//      1999-11-10 by UAB/I/LN  Urban Sderberg

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <acs_aeh_evreport.h>
#include <aes_gcc_eventhandler.h>
#include <aes_gcc_event.h>


class EventHandler
{
private:
    static EventHandler *instance_;
    const char *evHeader_;
    const char *alHeader_;

    ACS_AEH_ReturnType retCodeAEH;
    acs_aeh_evreport* aehPtr;

    char* errorString;
    char procName[256];
    char refName[256];
    char consolDText[1024];
    char consolSText[1024];

    AES_GCC_Event event;
    //AES_GCC_EventHandler evHandler;
    
public:
    EventHandler();
    ~EventHandler();

    void initAlarm(const long&  alNr,
        const char *alType,
        const char *alProblemtext,
        const char *alText,
        const char *uData);
    void ceaseAlarm(const long&  alNr,
        const char *alProblemtext,
        const char *alText,
        const char *uData);
    void sendEvent(const long&  evNr,
        const char *evProblemData,
        const char *evProblemText);
    
    static EventHandler *instance();
};

#endif
