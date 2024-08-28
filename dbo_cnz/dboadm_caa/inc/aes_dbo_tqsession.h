//******************************************************************************
// 
// .NAME
//      AES_DFO_CpfCom - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME AES_DFO_CpfCom
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
//      1999-11-30 by UAB/I/LN  Urban Söderberg

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************


#ifndef AES_DBO_TQSESSION_H
#define AES_DBO_TQSESSION_H

#include "aes_dbo_destinationset.h"
#include "aes_dbo_mirrormaintainer.h"
#include <aes_cdh_resultcode.h>
#include <ace/Auto_Event.h>
#include <string>

class aes_dbo_clientsession;

class aes_dbo_tqsession 
{
public:
    // Transfer Queue name
    const std::string tq_;

private:
    aes_dbo_destinationset* dest_;          // Pointer to the destination set object
    aes_dbo_clientsession* client_;
    ACE_Auto_Event cdhConnectionLost_;
    bool tqOpen_;
    bool mirrored_;
    aes_dbo_mirrormaintainer mirror_;
    ACE_Recursive_Thread_Mutex tqMutex;
public:
    /**
         * @brief
         * constructor 
         */ 
    aes_dbo_tqsession(const std::string &tq);
    /**
         * @brief
         * destructor
         */
    ~aes_dbo_tqsession();
    /**
         *  open  method: This method is used to open a client session.
         *  @param  client          :  aes_dbo_clientsession pointer
         *  @param  sourceId         : const string
         *  @param  eventText         : const string
         */
    unsigned int open(aes_dbo_clientsession *client, const std::string &sourceId = "", const std::string &eventText = "");
    /**
         *  close  method: This method is used to close a client session.
         */
    void close();
    /**
         *  sendEventToUser  method: This method is used to send event to user.
         *  @param  evCode          : unsigned int
         *  @param  blockNr         : unsigned int
         */
    void sendEventToUser(unsigned int evCode, unsigned int blockNr);
    /**
         *  lostCdhConnection method: This method is invoked when connection to CDH is lost.
         */
    void lostCdhConnection();
    /**
         *  changeMirror  method: This method is used to change mirror.
         *  @param  shouldMirror          : unsigned int
         */
    void changeMirror(unsigned int shouldMirror);
    /**
         *  changeDelay method: This method is used to change delay.
         *  @param  delay          : unsigned int
         */
    void changeDelay(unsigned int delay);
    /**
         *  transactionBegin  method: This method is begins a transaction.
         */
    unsigned int transactionBegin();
    /**
         *  transactionEnd  method: This method is used to end a transaction.
         *  @param  blockNr          : unsigned int
         */
    unsigned int transactionEnd(unsigned int &blockNr);
    /**
         *  transactionTerminate method: This method  terminates a transaction.
         */
    unsigned int transactionTerminate();
    /**
         *  transactionCommit method: This method commits a transaction.
         *  @param  blockNr          : unsigned int
         */
    unsigned int transactionCommit(unsigned int &blockNr);
    /**
         *  getLastCommittedBlock  method: This method is used to get the last commited block number.
         *  @param  blockNr          : unsigned int
         */
    unsigned int getLastCommittedBlock(unsigned int &blockNr);
    /**
         *  newMirrorFile  method: This method is used for a new mirror file.
         *  @param  fName          : const string
         */
    unsigned int newMirrorFile(const std::string &fName);
    /**
         *  closeMirrorFile  method: This method is  used to close a mirror file.
         *  @param  fName          : const string
         */
    unsigned int closeMirrorFile(const std::string &fName);
    /**
         * removeMirror  method: This method is used to remove mirror.
         */
    unsigned int removeMirror();
    /**
         * checkForRemoval method: This method is invoked when mirror is to be removed.
         */
    unsigned int checkForRemoval();
    /**
      	 * resetClient method: This method is invoked in order to set client_ to NULL.
      	 */
    void resetClient();
};

#endif
