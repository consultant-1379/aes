//******************************************************************************
//
// NAME
//      EventHandler.cpp
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
//      <General description of the class>

// DOCUMENT NO
//      19089-CAA 109 0319

// AUTHOR 
//      1999-11-18 by UAB/I/LN  Urban Sderberg

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include <stdio.h>

#include <aes_gcc_log.h>
#include "EventHandler.h"

EventHandler *EventHandler::instance_ = 0;

// ACS Trace definition
GCC_TDEF(aes_dbo_eventhandler);

//******************************************************************************
//
EventHandler::EventHandler():
evHeader_("DBO FAULT"),
alHeader_("DATA OUTPUT, AP TRANSMISSION FAULT")
{
    GCC_TRACE((aes_dbo_eventhandler,
        "(%t) EventHandler::EventHandler()\n"));

	errorString = 0;
	ACE_OS::memset( procName, 0, sizeof(procName));	
	ACE_OS::memset( refName, 0, sizeof(refName));			
	ACE_OS::memset( consolDText, 0, sizeof( consolDText ));
	ACE_OS::memset( consolSText, 0, sizeof( consolSText ));
    aehPtr = new(std::nothrow) acs_aeh_evreport;
}


//******************************************************************************
//
EventHandler::~EventHandler()
{
    GCC_TRACE((aes_dbo_eventhandler,
        "(%t) EventHandler::~EventHandler()\n"));

	if( aehPtr != 0 )
	{
		delete aehPtr;
		aehPtr = 0;
	}
}


//******************************************************************************
//
void EventHandler::initAlarm(const long &alNr, const char *alType,
                             const char *alProblemtext, const char *alText,
                             const char *uData)
{
    GCC_TRACE((aes_dbo_eventhandler,
        "(%t) EventHandler::initAlarm()\n"));

    ACE_OS::sprintf(procName,"%s:%d","AES_DBO_server", ACE_OS::getpid());
    ACE_OS::strcpy(refName, uData);
    
    retCodeAEH = aehPtr->sendEventMessage(procName,
        alNr,     
        alType,
        alHeader_,
        "APZ",
        refName,
        alProblemtext,
        alText);
    
    if (retCodeAEH == ACS_AEH_error)
    {    
    	ACE_OS::sprintf(consolDText,
            "%s:\n%s\n%s: %s",
            "Delivery Failure at initiating DBO alarm",
            alHeader_,
            alProblemtext,
            alText);
        
    	ACE_OS::sprintf(consolSText,
            "%s:\n%s\n%s: %s",
            "Syntax Error at initiating DBO alarm",
            alHeader_,
            alProblemtext,
            alText);
        
        errorString = const_cast<char*>(aehPtr->getErrorText());
        FILE* fp = fopen("/dev/console","w");
        
        switch (aehPtr->getError())
        {    
        case ACS_AEH_eventDeliveryFailure:
            if (fp != 0)
            {
                fprintf(fp,"\n%s\n%s\n", consolDText, errorString);
                fclose(fp);
            }
            break;
            
        case ACS_AEH_syntaxError:
            if (fp != 0)
            {
                fprintf(fp,"\n%s\n%s\n", consolSText, errorString);
                fclose(fp);
            }
            break;   
	default:
	    //Coverity fix	
	    if (fp != 0)
	    {
		fclose(fp);
	    }		
	    break;
	
        }   
    }
}


//******************************************************************************
//
void EventHandler::ceaseAlarm(const long &alNr, const char *alProblemtext,
                              const char *alText, const char *uData)
{
    GCC_TRACE((aes_dbo_eventhandler,
        "(%t) EventHandler::ceaseAlarm()\n"));

    ACE_OS::sprintf(procName,"%s:%d","AES_DBO_server", ACE_OS::getpid());
    ACE_OS::strcpy(refName, uData);
    
    retCodeAEH = aehPtr->sendEventMessage(procName,
        alNr,   
        "CEASING",
        alHeader_,
        "APZ",
        refName,
        alProblemtext,
        alText);
    
    if (retCodeAEH == ACS_AEH_error)
    {    
        ACE_OS::sprintf(consolDText,
            "%s:\n%s\n%s: %s",
            "Delivery Failure at ceasing DBO alarm",
            alHeader_,
            alProblemtext,
            alText);
        
        ACE_OS::sprintf(consolSText,
            "%s:\n%s\n%s: %s",
            "Syntax Error at ceasing DBO alarm",
            alHeader_,
            alProblemtext,
            alText);
        
        errorString = const_cast<char*>(aehPtr->getErrorText());
        FILE *fp = fopen("/dev/console","w");
        
        switch (aehPtr->getError())
        {    
        case ACS_AEH_eventDeliveryFailure:
            if (fp != 0)
            {
                fprintf(fp,"\n%s\n%s\n", consolDText, errorString);
                fclose(fp);
            }
            break;
            
        case ACS_AEH_syntaxError:
            if (fp != 0)
            {
                fprintf(fp,"\n%s\n%s\n", consolSText, errorString);
                fclose(fp);
            }
            break;       
	default:
             //Coverity fix
            if (fp != 0)
            {
                fclose(fp);
            }
    	    break;
        }
    }
}


//******************************************************************************
//
void EventHandler::sendEvent(const long &evNr, const char *evProblemData,
                             const char *evProblemText)
{
    GCC_TRACE((aes_dbo_eventhandler,
        "(%t) EventHandler::sendEvent()\n"));

    sprintf(procName,"%s", "AES_DBO_server");     
    strcpy(refName, "AES_DBO_server/Event");
    event.setProbableCause(evHeader_);
    event.setProblemText(evProblemText);
    event.setProblemData(evProblemData);
    event.setEventCode(evNr);
    event.setObjectOfReference(refName);
    event.setKind(AES_GCC_Event::EV_EVENT);
    AES_GCC_EventHandler::instance().event(event);
}

//******************************************************************************
//
EventHandler *EventHandler::instance()
{
    GCC_TRACE((aes_dbo_eventhandler,
        "(%t) EventHandler::instance()\n"));

    if (!instance_)
        return instance_ = new EventHandler();
    else
        return instance_;
}
