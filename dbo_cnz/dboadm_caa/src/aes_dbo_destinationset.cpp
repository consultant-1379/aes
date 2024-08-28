

//******************************************************************************
//
// NAME
//      aes_dbo_destinationset.cpp
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
//      This class handles communication towards CDH server, it inherits class
//      AES_CDH_aes_dbo_destinationset.
//      An instance of this class is created when transfer of block data is
//      initialised. The name of the used AES_GCC_Stream is send to CDH server
//      who sends events back to this class. The events may also be send back
//      to the client thru the AES_CDH_cpfcom that created this instance.
//      If transfer failed is received from CDH server an alarm is issued by
//      this class by calling AES_DFO_AlarmListHandler.


// DOCUMENT NO
//      19089-CAA 109 0423

// AUTHOR 
//      2002-12-05 by UAB/UKY/AU  Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include <aes_dbo_destinationset.h>
#include <aes_dbo_tqdatabase.h>
#include <aes_dbo_alarmdatabase.h>
#include <aes_dbo_tqsession.h>
#include <EventCode.h>
#include <aes_gcc_tracer.h>

// ACS Trace definition
GCC_TDEF(AES_DBO_Destinationset);
AES_GCC_TRACE_DEFINE(AES_DBO_Destinationset);


//******************************************************************************
//
aes_dbo_destinationset::aes_dbo_destinationset(const std::string &tq, const std::string &dest,
                               const std::string &streamId, const std::string &eventText,
                               aes_dbo_tqsession *tqSession):
AES_CDH_DestinationSet(dest),
tq_(tq),
dest_(dest),
tqSession_(tqSession),
streamId_(streamId),
eventText_(eventText),
destsetdown_(false)
{
    // Check if is mirrored
    mirrored_ = aes_dbo_tqdatabase::instance()->mirrored(tq_);
	AES_GCC_TRACE_MESSAGE("TQ:<%s>, Destination set:<%s>, Stream ID:<%s>", tq.c_str(), dest.c_str(), streamId.c_str());
}


//******************************************************************************
//
aes_dbo_destinationset::~aes_dbo_destinationset()
{
	AES_GCC_TRACE_MESSAGE("TQ:<%s>, Destination set:<%s>, Stream ID:<%s>", tq_.c_str(), dest_.c_str(), streamId_.c_str());
}


//******************************************************************************
//
int aes_dbo_destinationset::openTransfer()
{
	AES_GCC_TRACE_MESSAGE("Entering...,TQ:<%s>", tq_.c_str());

	AES_CDH_ResultCode ret = open();

	if( AES_CDH_RC_OK != ret )
	{
		// Get error text from CDH
		std::string errText;
		getErrorText(errText);
		GCC_TERROR((AES_DBO_Destinationset,"(%t) aes_dbo_destinationset::open() error result code received from CDH: %d, error text: %s\n", ret, errText.c_str()));
		return (int)ret;
	}

    setEventSubscription(tq_, eventText_);

    ret = sendRecordFile(tq_, streamId_);

    if( AES_CDH_RC_OK != ret )
    {
    	// Get error text from CDH
    	std::string errText;
    	getErrorText(errText);
    	GCC_TERROR((AES_DBO_Destinationset,"(%t) aes_dbo_destinationset::sendRecordFile() error result code received from CDH: %d, error text: %s\n", ret, errText.c_str()));
    }
    
    AES_GCC_TRACE_MESSAGE("...Leaving ,TQ:<%s>, result:<%d>", tq_.c_str(), ret);

    return (int)ret;
}


//******************************************************************************
//
int aes_dbo_destinationset::closeTransfer()
{
	AES_GCC_TRACE_MESSAGE("Entering...,TQ:<%s>", tq_.c_str());

	removeEventSubscription();
    close();

    AES_GCC_TRACE_MESSAGE("...Leaving ,TQ:<%s>", tq_.c_str());
    return 0;
}


//******************************************************************************
//
void aes_dbo_destinationset::eventSendRecordFile(AES_CDH_ResultCode ret)
{
    // There are two cases when CDH calls this callback-method:
    // 1) when a subfile has been sent
    // 2) when a transfer fault occurs
    
    AES_GCC_TRACE_MESSAGE("Entering...,TQ:<%s>, StreamID:<%s>, event:<%d>", tq_.c_str(), streamId_.c_str(), ret);
    switch (ret)
    {
		case AES_CDH_RC_OK:
			// Subfile has been sent via CDH to the remote side
			onEvOK();
			break;

		case AES_CDH_RC_SWITCH:
			onEvSwitch();
			break;

		case AES_CDH_RC_CONNECTERR:
		case AES_CDH_RC_SENDERR:
			onEvError();
			break;

		case AES_CDH_RC_FILEERR:
			onEvFileError();
			break;

		case AES_CDH_RC_TASKSTOPPED:
			break;

		case AES_CDH_RC_TASKEXISTS:
			break;

		default:
			// Unexpected fault detected...
			onEvDefault(ret);
			break;
    }

    AES_GCC_TRACE_MESSAGE("...Leaving,TQ:<%s>", tq_.c_str() );

}


//******************************************************************************
//
void aes_dbo_destinationset::eventDestinationSet(AES_CDH_ResultCode ret, const std::string &dest)
{
    AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>, dest:<%s>, event:<%d>", tq_.c_str(), dest.c_str(), ret);

    switch (ret)
    {
		case AES_CDH_RC_CONNECTERR:
			tqSession_->sendEventToUser(AES_EVELINKDOWN, 0);

			// Insert data into alarm list handler => an alarm will be issued
			insertAlarm(tq_, dest_);
			break;

		case AES_CDH_RC_CONNECTOK:
			tqSession_->sendEventToUser(AES_EVELINKDOWNCEASE, 0);

			// Check if there exists an alarm for this TQ,
			// if so, cease the corresponding alarm.
			removeAlarm(tq_, dest_);
			break;

		case AES_CDH_RC_DESTSETDOWN:
			if (destsetdown_ == false) {
			   tqSession_->sendEventToUser(AES_EVEDESTSETDOWN, 0);
			   destsetdown_ = true;
			}
			break;

		case AES_CDH_RC_DESTSETDOWNCEASE:
			if (destsetdown_ == true) {
			   tqSession_->sendEventToUser(AES_EVEDESTSETDOWNCEASE, 0);
			   destsetdown_ = false;
			}
			break;

		case AES_CDH_RC_NOSERVER:
			tqSession_->lostCdhConnection();
			break;
		default :
			break;
    }

    AES_GCC_TRACE_MESSAGE("...Leaving,TQ:<%s>", tq_.c_str() );
}


//******************************************************************************
//
void aes_dbo_destinationset::onEvOK()
{
    AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tq_.c_str() );

    if(destsetdown_ == true)
    {
    	tqSession_->sendEventToUser(AES_EVEDESTSETDOWNCEASE, 0);
        destsetdown_ = false;
    }

    // Check if there exists an alarm for this TQ,
    // if so, cease the corresponding alarm.
    if( removeAlarm(tq_, dest_) )
        tqSession_->sendEventToUser(AES_EVEFILETRANSFERFAILEDCEASE, 0);

    AES_GCC_TRACE_MESSAGE("...Leaving,TQ:<%s>", tq_.c_str() );
}


//******************************************************************************
//
void aes_dbo_destinationset::onEvSwitch()
{
	AES_GCC_TRACE_MESSAGE("TQ:<%s>", tq_.c_str() );

    tqSession_->sendEventToUser(AES_EVEBLOCKTRANSFERFAILED, 0);
}


//******************************************************************************
//
void aes_dbo_destinationset::onEvError()
{
    AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tq_.c_str() );

    tqSession_->sendEventToUser(AES_EVEBLOCKTRANSFERFAILED, 0);
    if (destsetdown_ == false) {
       tqSession_->sendEventToUser(AES_EVEDESTSETDOWN, 0);
       destsetdown_ = true;
    }
    
    // Insert data into alarm list handler => an alarm will be issued
    insertAlarm(tq_, dest_);

    AES_GCC_TRACE_MESSAGE("...Leaving,TQ:<%s>", tq_.c_str() );
}


//******************************************************************************
//
void aes_dbo_destinationset::onEvFileError()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tq_.c_str() );

    tqSession_->sendEventToUser(AES_EVEBLOCKTRANSFERFAILED, 0);
    
    // Insert data into alarm list handler => an alarm will be issued
    insertAlarm(tq_, dest_);

    AES_GCC_TRACE_MESSAGE("...Leaving,TQ:<%s>", tq_.c_str() );
}


//******************************************************************************
//
void aes_dbo_destinationset::onEvDefault(AES_CDH_ResultCode ret)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", tq_.c_str() );

    tqSession_->sendEventToUser(AES_EVEBLOCKTRANSFERFAILED, 0);

    // Insert data into alarm list handler => an alarm will be issued
    insertAlarm(tq_, dest_);

    AES_GCC_TRACE_MESSAGE("...Leaving,TQ:<%s>", tq_.c_str() );
}


//******************************************************************************
//
void aes_dbo_destinationset::insertAlarm(const std::string &tq, const std::string &dest)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>, Dest:<%s>", tq.c_str(), dest.c_str() );

    AlarmRecord* a = new (std::nothrow) AlarmRecord();
    if( NULL != a)
    {
    	strcpy(a->file, tq.c_str());
    	strcpy(a->destination, dest.c_str());

    	a->write = 0;

    	if(mirrored_)
    		a->write = 1;

    	if(!eventText_.empty())
    		strcpy(a->alText, eventText_.c_str());
    	else
    		a->alText[0] = 0;

    	aes_dbo_alarmdatabase::instance()->insert(a);
    }
    
    AES_GCC_TRACE_MESSAGE("...Leaving,TQ:<%s>", tq.c_str() );
}


//******************************************************************************
//
bool aes_dbo_destinationset::removeAlarm(const std::string &tq, const std::string &dest)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>, Dest:<%s>", tq.c_str(), dest.c_str() );

    bool found = false;

    AlarmRecord *a = new (std::nothrow) AlarmRecord();
    if(NULL != a)
    {
    	strcpy(a->file, tq.c_str());
    	strcpy(a->destination, dest.c_str());
    	found = aes_dbo_alarmdatabase::instance()->remove(a);
    	delete a;
    }
    return found;
}
