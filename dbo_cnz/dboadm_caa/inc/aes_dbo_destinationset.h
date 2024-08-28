//******************************************************************************
// 
// .NAME
//      DestinationSet - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME DestinationSet
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

#ifndef AES_DBO_DESTINATIONSET_H
#define AES_DBO_DESTINATIONSET_H 

#include <aes_gcc_eventhandler.h>
#include <aes_gcc_eventcodes.h>
#include <aes_gcc_event.h>
#include <aes_gcc_log.h>

#include <aes_cdh_resultcode.h>
#include <aes_cdh_result.h>

#include <aes_cdh_destination.h>
#include <aes_cdh_destinationset.h>
#include <aes_cdh_destination_r1.h>
#include <aes_cdh_resultcode_r1.h>
#include <aes_cdh_result_r1.h>

#include <ACS_APGCC_Command.H>

#include <ace/Event_Handler.h>

#include <string>

class aes_dbo_tqsession;

class destRec;

class aes_dbo_destinationset: public AES_CDH_DestinationSet 
{
private:
    std::string tq_;
    std::string dest_;
    aes_dbo_tqsession* tqSession_;
    std::string streamId_;
    std::string eventText_;
    bool mirrored_, destsetdown_;
    
public:
    /**
         * @brief
         * constructor with five parameters
         */
    aes_dbo_destinationset (const std::string &tq, const std::string &dest,
        const std::string &streamId, const std::string &eventText,
        aes_dbo_tqsession *tqSession);
    /**
         * @brief
         * destructor 
         */
    virtual ~aes_dbo_destinationset();
    /**
         *  openTransfer  method: This method is used to open a transfer.
         */
    int openTransfer();
    /**
         *  closeTransfer  method: This method is used to close a transfer.
         */
    int closeTransfer();
    /**
         *  eventSendRecordFile method: This method is invoked when a record file is sent.
         *  @param  ret         : AES_CDH_ResultCode
         */
    virtual void eventSendRecordFile(AES_CDH_ResultCode ret);
    /**
         *  eventDestinationSet  method: This method is invoked when a event on destination set is raised.
         *  @param  ret         : AES_CDH_ResultCode
         *  @param  dest         : const string
         */
    virtual void eventDestinationSet(AES_CDH_ResultCode ret,
        const std::string &dest);
    /**
         *  onEvOK  method: This method is invoked on event of ok.
         */
    void onEvOK();
    /**
         *  onEvSwitch  method: This method is invoked on event of switch.
         */
    void onEvSwitch();
    /**
         *  onEvError  method: This method is invoked on event of error .
         */
    void onEvError();
    /**
         *  onEvFileError  method: This method is invoked on event of file error .
         */
    void onEvFileError();
    /**
         *  onEvDefault  method: This method is invoked on default event.
         *  @param  ret          : AES_CDH_ResultCode
         */
    void onEvDefault(AES_CDH_ResultCode ret);
    /**
         *  insertAlarm  method: This method is used to insert an alarm.
         *  @param  tq          : const string
         *  @param  dest         : const string
         */
    void insertAlarm(const std::string &tq, const std::string &dest);
    /**
         *  removeAlarm  method: This method is used to remove an alarm.
         *  @param  tq          : const string
         *  @param  dest         : const string
         */
    bool removeAlarm(const std::string &tq, const std::string &dest);
};

#endif
