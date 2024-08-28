
//******************************************************************************
//
// NAME
//      aes_dbo_opentqsessions.cpp
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
//      19089-CAA 109 0423

// AUTHOR 
//      1999-11-18 by UAB/I/LN  Urban Söderberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010530  qabhall Changed due to new requirements INGO3 GOH.
//			011204	qabulfg	Function terminateAll changed. HC90320.

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include "aes_dbo_opentqsessions.h"

#include "aes_dbo_tqsession.h"

#include <aes_gcc_errorcodes.h>
#include <aes_gcc_tracer.h>


AES_GCC_TRACE_DEFINE(AES_DBO_Opentqsessions);


//******************************************************************************
//
aes_dbo_opentqsessions_impl::aes_dbo_opentqsessions_impl()
{
	AES_GCC_TRACE_MESSAGE("Constructor");
	for (int w = 0; w < listSize_; ++w)
	{
		sessionList[w] = 0;
	}
}


//******************************************************************************
//
aes_dbo_opentqsessions_impl::~aes_dbo_opentqsessions_impl()
{
	AES_GCC_TRACE_MESSAGE("Destructor");
}


//******************************************************************************
//
bool aes_dbo_opentqsessions_impl::insert(const string &tq)
{
    // Create an TQSession object to
    // handle communications towards CPF and RTR
	AES_GCC_TRACE_MESSAGE("In");

    aes_dbo_tqsession* tqSession = new aes_dbo_tqsession(tq);

    // Start session
    bool insertFlag = false;
    for (int w = 0; w < listSize_; w++)
    {
        if (sessionList[w] == 0)
        {
            sessionList[w] = tqSession;
            AES_GCC_TRACE_MESSAGE("TQSession object is inserted");
            AES_GCC_TRACE_MESSAGE("Address: %d", sessionList[w]);
            insertFlag = true;
            break;
        }
    }
    AES_GCC_TRACE_MESSAGE("Out: insertFlag = %s", insertFlag ? "TRUE" : "FALSE");
    return insertFlag;
}


//******************************************************************************
//
aes_dbo_tqsession* aes_dbo_opentqsessions_impl::exists(const string &tq)
{
	AES_GCC_TRACE_MESSAGE("In");

    aes_dbo_tqsession *ptr = NULL;
    for (int w = 0; w < listSize_; w++)
    {
        if (sessionList[w] != 0)
        {
            if (tq == sessionList[w]->tq_)
            {
                ptr = sessionList[w];
                AES_GCC_TRACE_MESSAGE("tq <%s> exists", sessionList[w]->tq_.c_str());
                break;
            }
        }
    }
    AES_GCC_TRACE_MESSAGE("Out");
    return ptr;
}


//******************************************************************************
//
unsigned int aes_dbo_opentqsessions_impl::terminate(const string &tq)
{
    AES_GCC_TRACE_MESSAGE("In: tq = %s, listSize = %d", tq.c_str(), listSize_);
    unsigned int rCode;
    for (int w = 0; w < listSize_; w++)
    {
        if (sessionList[w] != 0)
        {
            if (tq == sessionList[w]->tq_)
            {
                // Check the data area and remove it if it went OK
                rCode = sessionList[w]->checkForRemoval();
                if (rCode == AES_NOERRORCODE)
                {
                    // Close the session
                    sessionList[w]->close();
                    
                    AES_GCC_TRACE_MESSAGE("TQSession closed: %s", sessionList[w]->tq_.c_str());
                    AES_GCC_TRACE_MESSAGE("Address: %d", sessionList[w]);

                    delete sessionList[w];
                    sessionList[w] = 0;      
                    
                    AES_GCC_TRACE_MESSAGE("TQSession object is deleted!");
                }
                break;
            }
        }
    }
    AES_GCC_TRACE_MESSAGE("Out: rCode = %d", rCode);
    return rCode;
}


//******************************************************************************
//
void aes_dbo_opentqsessions_impl::terminateAll()
{
	AES_GCC_TRACE_MESSAGE("In");

    for (int w = 0; w < listSize_; w++)
    {
        if (sessionList[w] != 0)
        {
            sessionList[w]->close();
            delete sessionList[w];
            sessionList[w] = 0;
        }
    }
    AES_GCC_TRACE_MESSAGE("Out");
}

