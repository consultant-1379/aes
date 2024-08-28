
//******************************************************************************
//
// NAME
//      aes_dbo_alarmdatabase.cpp
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
//          010530  qabhall Alarm printout changed. Application supplied 
//                          alarm text implemented.  INGO3 GOH.
//          010815  qabhall TR: HC77861 Alarm text corrected.
//			011119  qabulfg Event & Alarm number changed 233xx -> 292xx
//			011123	qabulfg Cease basic dfo alarms when dfoch or dform
//			020108	qabulfg	Function insert changed. For Basic DFO issue
//							alarm always, even if Alarm in Alarmlist.
//			020402	qabulfg	New function, ceaseAll, implemented.

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include "aes_dbo_alarmdatabase.h"

#include "EventCode.h"
#include "EventHandler.h"
#include "ParameterHandler.h"

#include <aes_gcc_tracer.h>
#include <aes_gcc_errorcodes_r1.h>

#include <string>

AES_GCC_TRACE_DEFINE(AES_DBO_alarmdatabase);


//******************************************************************************
//
aes_dbo_alarmdatabase_impl::aes_dbo_alarmdatabase_impl():
mtx_()
{
	AES_GCC_TRACE_MESSAGE("Costructor");
}


//******************************************************************************
//
aes_dbo_alarmdatabase_impl::~aes_dbo_alarmdatabase_impl()
{
    AES_GCC_TRACE_MESSAGE("Destructor");
}

//******************************************************************************
//
void aes_dbo_alarmdatabase_impl::insert(AlarmRecord *a)
{
	string alText;
	string alCause;
	string uData;
	string tqrdnstr;
	string dsrdnstr;
	string na;

	alText="\nCAUSE\n";
	alCause="DATA TRANSFER, BLOCK TRANSFER";
	alText=alText+alCause;
	alText+="\n\nFILE NAME\n";
	na="-";
	alText=alText+na;
	alText += "\nTRANSFER QUEUE\n";
	tqrdnstr="blockTransferQueueId=";
	alText = alText+tqrdnstr+a->file;
	alText += "\n\nDESTINATION SET\n";
	dsrdnstr="blockDestinationSetId=";
	alText = alText+dsrdnstr+a->destination;
	alText +="\n\nDESTINATION\n";
	alText=alText+na;
    
	if(strlen(a->alText) == 0)
	{
		strcpy(a->alText, "Transfer error");
	}

    // Insert alarm into alarm database and send it to ALH
    ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    if (!isDefined(a)) // File does not exist in alarm list
    {
        // Update alarm list and alarm system file
        list_.push_back(a);
        
        // issue alarm
        stringstream s;
        s << a->file << "_" << a->destination;
        uData = s.str();
        EventHandler::instance()->initAlarm(AL_DBOFAULT, "A2", a->alText, alText.c_str(), uData.c_str());
        AES_GCC_TRACE_MESSAGE("Alarm inserted and issued: File %s", a->file);
    }
    else 
    {
        AES_GCC_TRACE_MESSAGE("Alarm NOT inserted because already defined in alarm list: File %s", a->file);
        delete a;
    }
}

//******************************************************************************
//
bool aes_dbo_alarmdatabase_impl::remove(AlarmRecord *a)
{
    // Remove alarm item from list and cease corresponding alarm
	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    bool found = false;
    if (list_.size() > 0)
    {
    	AlarmRecord *aRec;
    	std::list<AlarmRecord *>::iterator it;
    	it = list_.begin();
    	while (it != list_.end())
        {
            aRec = *it;
            if (!strcmp(aRec->file, a->file))
            {
                // File exists in alarm list, cease alarm
                // Update alarm list
                found = true;
                list_.erase(it);

                string alText;
                string uData;
                string na;
                string alCause;       
	
                alText="\nCAUSE\n";
                alCause="DATA TRANSFER, BLOCK TRANSFER";
                alText=alText+alCause;
                alText+="\n\nFILE NAME\n";
                na="-";
                alText=alText+na;             
                alText += "\nTRANSFER QUEUE\n";
                string tqrdnstr="blockTransferQueueId=";
                alText = alText+tqrdnstr+aRec->file;
                alText += "\n\nDESTINATION SET\n";
                string dsrdnstr="blockDestinationSetId=";
                alText = alText+dsrdnstr+aRec->destination;
                alText +="\n\nDESTINATION\n";
                alText=alText+na;
                stringstream s;
                s << aRec->file << "_" << aRec->destination;
                uData = s.str();
                
                // cease the alarm
                EventHandler::instance()->ceaseAlarm(AL_DBOFAULT, aRec->alText, alText.c_str(), uData.c_str());
                AES_GCC_TRACE_MESSAGE("Alarm ceased, file: %s", aRec->file);
                delete aRec;
                
                break; // Done, bail out
            }
            ++it;
        }
    }
    return found;
}


//******************************************************************************
//
int aes_dbo_alarmdatabase_impl::ceaseAll()
{
	// cease all DBO alarms
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
	if (list_.size() > 0)
	{
		AlarmRecord *aRec;
		std::list<AlarmRecord *>::iterator it;
		it = list_.begin();
		while (it != list_.end())
		{
			aRec = *it;
			string alText;
			string uData;
			string na;
			string alCause;

			alText="\nCAUSE\n";
			alCause="DATA TRANSFER, BLOCK TRANSFER";
			alText=alText+alCause;
			alText+="\n\nFILE NAME\n";
			na="-";
			alText=alText+na;
			alText += "\nTRANSFER QUEUE\n";
			string tqrdnstr="blockTransferQueueId=";
			alText = alText+tqrdnstr+aRec->file;
			alText += "\n\nDESTINATION SET\n";
			string dsrdnstr="blockDestinationSetId=";
			alText = alText+dsrdnstr+aRec->destination;
			alText +="\n\nDESTINATION\n";
			alText=alText+na;
			stringstream s;
			s << aRec->file << "_" << aRec->destination;
			uData = s.str();

			// cease the DFO alarm
			EventHandler::instance()->ceaseAlarm(AL_DBOFAULT, aRec->alText, alText.c_str(), uData.c_str());
			AES_GCC_TRACE_MESSAGE("Alarm ceased, file: %s", aRec->file);
			++it;
		}
	}
	return 0;
}


//******************************************************************************
//
bool aes_dbo_alarmdatabase_impl::isDefined(const AlarmRecord *a)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
	if (list_.size() > 0)
	{
		AlarmRecord *aRec;
		std::list<AlarmRecord *>::iterator it;
		it = list_.begin();
		while (it != list_.end())
		{
			aRec = *it;
			if (!strcmp(aRec->file, a->file))
			{
				return true; // Entry exists
			}
			++it;
		}
	}
	return false;      // Entry does not exists
}

//******************************************************************************
//
void aes_dbo_alarmdatabase_impl::clearAlarmListAtShutdown()
{
	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
	std::list<AlarmRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		AlarmRecord *aRec = *it;
		list_.erase(it);
		delete aRec;
		it = list_.begin();
	}
}

