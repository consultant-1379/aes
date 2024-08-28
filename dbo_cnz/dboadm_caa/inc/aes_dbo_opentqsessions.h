//******************************************************************************
// 
// .NAME
//      OpenTQSessions - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME OpenTQSessions
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
//      1999-11-10 by UAB/I/LN  Urban Söderberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010530  qabhall Changed due to new requirements INGO3 GOH.

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef AES_DBO_OPENTQSESSIONS_H
#define AES_DBO_OPENTQSESSIONS_H 

#include<ace/Singleton.h>

#include <string>

class aes_dbo_tqsession;

class aes_dbo_opentqsessions_impl
{

	friend class ACE_Singleton<aes_dbo_opentqsessions_impl, ACE_Recursive_Thread_Mutex>;

private:
    enum { listSize_ = 1000 };
    aes_dbo_tqsession *sessionList[listSize_];
    /**
         * @brief
         * constructor 
         */
    aes_dbo_opentqsessions_impl();
    /**
     * @brief
     * destructor
     */
    ~aes_dbo_opentqsessions_impl();
    
public:
    /**
         *  insert method: This method is used to insert a TQ seesion.
         *  @param  tq          : const string
         */
    bool insert(const std::string &tq);
    /**
         *  exists method: This method is used to check if a TQ seesion exists.
         *  @param  tq          : const string
         */
    aes_dbo_tqsession *exists(const std::string &tq);
    /**
         *  terminate method: This method is used to terminate a TQ seesion.
         *  @param  tq          : const string
         */
    unsigned int terminate(const std::string &tq);
    /**
         *  terminateAll method: This method is used to terminate all TQ seesions.
         */
    void terminateAll();
};

typedef ACE_Singleton<aes_dbo_opentqsessions_impl, ACE_Recursive_Thread_Mutex> aes_dbo_opentqsessions;

#endif
