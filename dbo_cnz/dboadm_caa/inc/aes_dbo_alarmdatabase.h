//******************************************************************************
// 
// .NAME
//      aes_dbo_alarmdatabase - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME aes_dbo_alarmdatabase
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
//      1999-10-20 by UAB/I/LN  Urban Söderberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//			020402	qabulfg	New function, ceaseAll, implemented.

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef AES_DBO_ALARMDATABASE_H
#define AES_DBO_ALARMDATABASE_H 

#include <ace/Singleton.h>

#include <list>

// The structure which the alarm list consists of
class AlarmRecord
{
public:
    char file[256];           // DFO file name
    char destination[256];    // Destination in CDH
    int write;                // 1 = Secured DFO, 0 = Basic DFO  
    char alText[2048];
};


class aes_dbo_alarmdatabase_impl
{
	friend class ACE_Singleton<aes_dbo_alarmdatabase_impl, ACE_Recursive_Thread_Mutex>;

private:
	std::list<AlarmRecord *> list_;
    ACE_Recursive_Thread_Mutex mtx_;
    /**
         * @brief
         * constructor 
    */
    aes_dbo_alarmdatabase_impl();
    /**
        * @brief
        * destructor
    */
    ~aes_dbo_alarmdatabase_impl();

public:
    /**
         *  insert method: This method is used to insert a alarmrecord.
         *  @param a : AlarmRecord pointer
    */
    void insert(AlarmRecord *a);
    /**
         *  remove method: This method is used to remove a alarmrecord.
         *  @param  a : AlarmRecord pointer
         *  @return bool : True/False
    */
    bool remove(AlarmRecord *a);
    /**
         *  ceaseAll method : This method is used to cease all  alarm.
         *  @return int : corresponding error code
    */
    int ceaseAll();
    /**
         *  iDefined method : This method is used to check if an  alarm is defined or not.
         *  @param    : const AlarmRecord
         *  @return bool : True/False
    */
    bool isDefined(const AlarmRecord *a);
    /**
         *  clearAlarmListAtShutdown method: This method is used to clear all alarmrecords at shutdown.
    */
    void clearAlarmListAtShutdown();

};

typedef ACE_Singleton<aes_dbo_alarmdatabase_impl, ACE_Recursive_Thread_Mutex> aes_dbo_alarmdatabase;

#endif
