/* =================================================================== */
/**
	@file aes_gcc_eventimplementation.cpp

	Class method implementation for aes_gcc_eventimplementation

	DESCRIPTION

	ERROR HANDLING

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.1.1

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       22/06/2011    xnadnar      Initial Release
 */
/* =================================================================== */

/*===================================================================
					INCLUDE DECLARATION SECTION
=================================================================== */
#include <iostream>
#include <sstream>
#include <string>
#include "aes_gcc_eventimplementation.h"
#include "acs_aeh_evreport.h"
#include "aes_gcc_log.h"
#include <ace/System_Time.h>
GCC_TDEF(aes_gcc_eventimlpementation);
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::AES_GCC_EventImplementation()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X: Default constructor ", this));

	//evTime = new ACE_Time_Value();
	ACE_System_Time::get_local_system_time(evTime);
	error_				= 0;	// default internal error
	oserror_			= 0;
	file_					= "";
	line_					= 0;
	problemdata_	= "";
	problemtext_	= "";
	event_				= 0;
	counter_			= 0;
	kind_					= AES_GCC_Event::EV_GENERAL;
	cause_          = AES_GCC_Event::defaultProbableCause;
	objRef_         = "";
	manualCease_    = false;

}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::AES_GCC_EventImplementation (ACE_INT32 anerror)
{
	//evTime				= new ACE_Time_Value();
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:constructor with anerror arg ", this));

	ACE_System_Time::get_local_system_time(evTime);
	error_				= anerror;
	oserror_			= 0;
	file_					= "";
	line_					= 0;
	problemdata_	= "";
	problemtext_	= "";
	event_				= 0;
	counter_			= 0;
	kind_					= AES_GCC_Event::EV_GENERAL;
	cause_          = AES_GCC_Event::defaultProbableCause;
	objRef_         = "";
	manualCease_    = false;
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::AES_GCC_EventImplementation (ACE_INT32 anerror, const ACE_TCHAR *mycomment)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:constructor anerror,char args ", this));
	//evTime				= new ACE_Time_Value();
	ACE_System_Time::get_local_system_time(evTime);
	error_				= anerror;
	oserror_			= 0;
	file_					= "";
	line_					= 0;
	problemdata_	= mycomment;
	problemtext_	= "";
	event_				= 0;
	counter_			= 0;
	kind_					= AES_GCC_Event::EV_GENERAL;
	cause_          = AES_GCC_Event::defaultProbableCause;
	objRef_         = "";
	manualCease_    = false;
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::AES_GCC_EventImplementation (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, const ACE_TCHAR *mycomment)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:constructor an,os,af,an,char args ", this));
	//evTime				= new ACE_Time_Value();
	ACE_System_Time::get_local_system_time(evTime);
	error_				= anerror;
	oserror_			= oserror;
	file_					= afile;
	line_					= aline;
	problemdata_	= mycomment;
	problemtext_	= "";
	event_				= 0;
	counter_			= 0;
	kind_					= AES_GCC_Event::EV_GENERAL;
	cause_          = AES_GCC_Event::defaultProbableCause;
	objRef_         = "";
	manualCease_    = false;
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::AES_GCC_EventImplementation (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, std::istream &istr)
{

	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:constructor an,os,af,an,istr args ", this));

	//evTime				= new ACE_Time_Value();
	ACE_System_Time::get_local_system_time(evTime);
	error_				= anerror;
	oserror_			= oserror;
	file_					= afile;
	line_					= aline;
	event_				= 0;
	counter_			= 0;
	kind_					= AES_GCC_Event::EV_GENERAL;
	std::string str;
	istr >> str;
	problemdata_ += str;
	problemtext_	= "";
	cause_          = AES_GCC_Event::defaultProbableCause;
	objRef_         = "";
	manualCease_    = false;
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::AES_GCC_EventImplementation (const AES_GCC_EventImplementation& ex)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:constructor ex ", this));

	evTime				= ex.evTime;
	error_				= ex.error_;
	oserror_			= ex.oserror_;
	file_					= ex.file_;
	line_					= ex.line_;
	problemdata_	= ex.problemdata_;
	problemtext_	= ex.problemtext_;
	event_				= ex.event_;
	counter_			= ex.counter_;
	kind_					= ex.kind_;
	cause_          = ex.cause_;
	objRef_         = ex.objRef_;
	manualCease_    = ex.manualCease_;
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::AES_GCC_EventImplementation (const ACE_TCHAR *problemdata)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:constructor with problem data as argument", this));

	//evTime				= new ACE_Time_Value();
	ACE_System_Time::get_local_system_time(evTime);
	error_				= 0;
	oserror_			= 0;
	file_					= "";
	line_					= 0;
	problemdata_	= problemdata;
	problemtext_	= "";
	event_				= 0;
	counter_			= 0;
	kind_					= AES_GCC_Event::EV_GENERAL;
	cause_          = AES_GCC_Event::defaultProbableCause;
	objRef_         = "";
	manualCease_    = false;
}
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_EventImplementation::~AES_GCC_EventImplementation()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:destructor ", this));
	//delete evTime;
}
/*===================================================================
                    ROUTINE: errorCode
=================================================================== */
ACE_INT32 AES_GCC_EventImplementation::errorCode () const
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:errCode %d\n", this, error_ ));
	return error_;
}
/*===================================================================
                    ROUTINE: setErrorCode
=================================================================== */
void AES_GCC_EventImplementation::setErrorCode (ACE_INT32 err)
{
	error_ = err;

	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setErrorCode %d\n", this, err ));
}
/*===================================================================
                    ROUTINE: osError
=================================================================== */
ACE_INT32 AES_GCC_EventImplementation::osError ()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:osError %d\n", this, oserror_));
	return oserror_;
}
/*===================================================================
                    ROUTINE: setOsError
=================================================================== */
void AES_GCC_EventImplementation::setOsError (ACE_INT32 err)
{
	oserror_ = err;
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setOsError %d\n", this, err));
}
/*===================================================================
                    ROUTINE: eventCode
=================================================================== */
ACE_INT64 AES_GCC_EventImplementation::eventCode ()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:evNumber %d\n", this, event_));
	return event_;
}
/*===================================================================
                    ROUTINE: setEventCode
=================================================================== */
void AES_GCC_EventImplementation::setEventCode (acs_aeh_specificProblem evnum)
{
	event_ = evnum;
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setEvNumber %d\n", this, evnum));
}
/*===================================================================
                    ROUTINE: problemData
=================================================================== */
const ACE_TCHAR * AES_GCC_EventImplementation::problemData () const
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:problemData\n", this));
	return problemdata_.c_str();
}
/*===================================================================
                    ROUTINE: setProblemData
=================================================================== */
void AES_GCC_EventImplementation::setProblemData (const ACE_TCHAR *problemdata)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setProblemData\n", this));
	problemdata_ = problemdata;
}
/*===================================================================
                    ROUTINE: problemText
=================================================================== */
const ACE_TCHAR * AES_GCC_EventImplementation::problemText () const
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:problemText\n", this));
	return problemtext_.c_str();
}
/*===================================================================
                    ROUTINE: setProblemText
=================================================================== */
void AES_GCC_EventImplementation::setProblemText (const ACE_TCHAR *ptext)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setProblemText\n", this));
	problemtext_ = ptext;
}
/*===================================================================
                    ROUTINE: kind
=================================================================== */
AES_GCC_Event::EventType AES_GCC_EventImplementation::kind ()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:kind %d\n", this, kind_));
	return kind_;
}
/*===================================================================
                    ROUTINE: setKind
=================================================================== */
void AES_GCC_EventImplementation::setKind (AES_GCC_Event::EventType kind)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setKind %d\n", this, kind));
	kind_ = kind;
}
/*===================================================================
                    ROUTINE: counter
=================================================================== */
ACE_INT32 AES_GCC_EventImplementation::counter ()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:counter %d\n", this, counter_));
	return counter_;
}
/*===================================================================
                    ROUTINE: setCounter
=================================================================== */
void AES_GCC_EventImplementation::setCounter (ACE_INT32 count)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setCounter %d\n", this, count));
	counter_ = count;
}
/*===================================================================
                    ROUTINE: inCounter
=================================================================== */
void AES_GCC_EventImplementation::incCounter ()
{
	counter_ ++;
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:incCounter %d\n", this, counter_));
}
/*===================================================================
                    ROUTINE: age
=================================================================== */
ACE_Time_Value AES_GCC_EventImplementation::age ()
{
	ACE_Time_Value now;
	ACE_Time_Value tv;

	if( ACE_System_Time::get_local_system_time(now)== 0)
	{
		ACE_INT32 seconds = now.sec() - evTime.sec();
		GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:age %d\n", this, seconds));
		tv.sec(seconds);

	}
	else
	{
		GCC_TRACE((aes_gcc_eventimlpementation,"%s","AES_GCC_EventImplementation::age: Failed to get the local time"));
		tv.sec(0);
	}
	return tv;
}
/*===================================================================
                    ROUTINE: resetAge
=================================================================== */
void AES_GCC_EventImplementation::resetAge ()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:resetAge", this));
	time_t now;
	if(ACE_System_Time::get_local_system_time(now) == 0)
	{
		evTime.set(now);
	}
}
/*===================================================================
                    ROUTINE: operator<<(const ACE_TCHAR *)
=================================================================== */
AES_GCC_EventImplementation & AES_GCC_EventImplementation::operator << (const ACE_TCHAR *mycomment)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:<< ", this));
	problemdata_ += mycomment;
	return *this;
}
/*===================================================================
                    ROUTINE: operator<<(ACE_INT64)
=================================================================== */
AES_GCC_EventImplementation & AES_GCC_EventImplementation::operator << (ACE_INT64 detail)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:<< ", this));
	ACE_TCHAR buff[30];
	ACE_OS::sprintf(buff, "%ld", detail);
	problemdata_ += buff;
	return *this;
}
/*===================================================================
                    ROUTINE: operator<<(std::istream)
=================================================================== */
AES_GCC_EventImplementation & AES_GCC_EventImplementation::operator << (std::istream &is)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:<< ", this));
	std::string str;
	is >> str;
	problemdata_ += str;
	return *this;
}
/*===================================================================
                    ROUTINE: str
=================================================================== */
std::string AES_GCC_EventImplementation::str ()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:str ", this));
	std::stringstream ost;
	ost.clear();
	ost << "File: "	<< file_ << " " << std::endl;
	ost << "Line: " << line_ << " " << std::endl;
	ost << "Error: (" << error_ << ") " << std::endl;
	//ost << "ErrorMessage: " << errorMessages[error_].errorText << " " << std::endl;
	ost << "Counter: " << counter_ << " " << std::endl;
	ost << "Event nr: " << event_ << " " << std::endl;
	ost << "Kind: " << kind_ << " " << std::endl;
	ost << "Age: " << this->age().sec() << " " << std::endl;
	ost << "Cause: " << cause_ << " " << std::endl;
	ost << "Object of reference: " << objRef_ << " " << std::endl;
	ost << "Problem data: " << problemdata_ << " " << std::endl;
	ost << "Problem text: " << problemtext_ << " " << std::endl;
	ost << "OS error: (" << oserror_ << ") " << strerror(oserror_) << " " << std::endl;
	ost << '\0';
	return ost.str();
}
/*===================================================================
                    ROUTINE: probableCause
=================================================================== */
const ACE_TCHAR * AES_GCC_EventImplementation::probableCause () const
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:probableCause\n", this));
	return cause_.c_str();
}
/*===================================================================
                    ROUTINE: setProbableCause
=================================================================== */
void AES_GCC_EventImplementation::setProbableCause (const ACE_TCHAR *cause)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setProbableCause %s\n", this, cause));
	cause_ = cause;
}
/*===================================================================
                    ROUTINE: manualCease
=================================================================== */
bool AES_GCC_EventImplementation::manualCease ()
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:manualCease\n", this));
	return manualCease_;
}
/*===================================================================
                    ROUTINE: setManualCease
=================================================================== */
void AES_GCC_EventImplementation::setManualCease (bool mCease)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setManualCease %s\n", this, mCease ? "true" : "false"));
	manualCease_ = mCease;
}
/*===================================================================
                    ROUTINE: objectOfReference
=================================================================== */
const ACE_TCHAR* AES_GCC_EventImplementation::objectOfReference() const
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:objectOfReference\n", this));
	return objRef_.c_str();
}
/*===================================================================
                    ROUTINE: setObjectOfReference
=================================================================== */
void AES_GCC_EventImplementation::setObjectOfReference(const ACE_TCHAR* objRef)
{
	GCC_TRACE((aes_gcc_eventimlpementation,"EXCEPTION %X:setObjectOfReference %s\n", this, objRef));
	objRef_ = objRef;
}
