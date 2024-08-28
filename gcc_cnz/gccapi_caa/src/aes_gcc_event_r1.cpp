/*=================================================================== */
	/**
	@file aes_gcc_event_r1.cpp

	Class method implementation for aes_gcc_event_r1.h
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
#include "acs_aeh_evreport.h"
#include "acs_aeh_types.h"
#include <iostream>
#include <string>
#include "aes_gcc_event_r1.h"
#include "aes_gcc_eventimplementation.h"

/*===================================================================
                        DECLARATION SECTION
=================================================================== */

std::string AES_GCC_Event_R1::defaultProbableCause = "";
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Event_R1::AES_GCC_Event_R1()

{
    implementation = new AES_GCC_EventImplementation;
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Event_R1::AES_GCC_Event_R1 (ACE_INT32 anerror)

{
  implementation = new AES_GCC_EventImplementation(anerror);

}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Event_R1::AES_GCC_Event_R1 (ACE_INT32 anerror, const ACE_TCHAR *mycomment)
{
    implementation = new AES_GCC_EventImplementation(anerror, mycomment);
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Event_R1::AES_GCC_Event_R1 (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, const ACE_TCHAR *mycomment)
{
    implementation = new AES_GCC_EventImplementation(anerror, oserror, afile, aline, mycomment);
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Event_R1::AES_GCC_Event_R1 (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, std::istream &istr)
{
    implementation = new AES_GCC_EventImplementation(anerror, oserror, afile, aline, istr);
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Event_R1::AES_GCC_Event_R1 (const AES_GCC_Event_R1& ex)
{
    implementation = new AES_GCC_EventImplementation(*(ex.implementation));
}
/*===================================================================
                    ROUTINE: operator=
=================================================================== */

AES_GCC_Event_R1& AES_GCC_Event_R1::operator=(const AES_GCC_Event_R1& ex)
{
	if( implementation != 0 )
	{
		delete implementation;
		implementation = 0;
	}
    implementation = new(std::nothrow) AES_GCC_EventImplementation(*(ex.implementation));
    return *this;
}    
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Event_R1::AES_GCC_Event_R1 (const ACE_TCHAR *problemdata)
{
    implementation = new AES_GCC_EventImplementation(problemdata);
}
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_Event_R1::~AES_GCC_Event_R1()
{
	delete implementation;
}
/*===================================================================
                    ROUTINE: errorCode
=================================================================== */
ACE_INT32 AES_GCC_Event_R1::errorCode () const
{
 	return implementation->errorCode();
 }
/*===================================================================
                    ROUTINE: setErrorCode
=================================================================== */
void AES_GCC_Event_R1::setErrorCode (ACE_INT32 err)
{
	implementation->setErrorCode(err);
}
/*===================================================================
                    ROUTINE: osError
=================================================================== */
ACE_INT32 AES_GCC_Event_R1::osError ()
{
	return implementation->osError();
}
/*===================================================================
                    ROUTINE: setOsError
=================================================================== */
void AES_GCC_Event_R1::setOsError (ACE_INT32 err)
{
	implementation->setOsError(err);
}
/*===================================================================
                    ROUTINE: eventCode
=================================================================== */
ACE_INT64 AES_GCC_Event_R1::eventCode ()
{
	return implementation->eventCode();
}
/*===================================================================
                    ROUTINE: setEventCode
=================================================================== */
void AES_GCC_Event_R1::setEventCode (acs_aeh_specificProblem evnum)
{
    implementation->setEventCode(evnum);
}
/*===================================================================
                    ROUTINE: problemData
=================================================================== */
const ACE_TCHAR * AES_GCC_Event_R1::problemData () const
{
	return implementation->problemData();
}
/*===================================================================
                    ROUTINE: setProblemData
=================================================================== */
void AES_GCC_Event_R1::setProblemData (const ACE_TCHAR *problemdata)
{
    implementation->setProblemData(problemdata);
}
/*===================================================================
                    ROUTINE: problemText
=================================================================== */
const ACE_TCHAR * AES_GCC_Event_R1::problemText () const
{
	return implementation->problemText();
}
/*===================================================================
                    ROUTINE: setProblemText
=================================================================== */
void AES_GCC_Event_R1::setProblemText (const ACE_TCHAR *ptext)
{
    implementation->setProblemText(ptext);
}
/*===================================================================
                    ROUTINE: kind
=================================================================== */
AES_GCC_Event_R1::EventType AES_GCC_Event_R1::kind ()
{
	return implementation->kind();
}
/*===================================================================
                    ROUTINE: setKind
=================================================================== */
void AES_GCC_Event_R1::setKind (AES_GCC_Event_R1::EventType kind)
{
    implementation->setKind(kind);
}
/*===================================================================
                    ROUTINE: counter
=================================================================== */
ACE_INT32 AES_GCC_Event_R1::counter ()
{
	return implementation->counter();
}
/*===================================================================
                    ROUTINE: setCounter
=================================================================== */
void AES_GCC_Event_R1::setCounter (ACE_INT32 count)
{
    implementation->setCounter(count);
}
/*===================================================================
                    ROUTINE: incCounter
=================================================================== */
void AES_GCC_Event_R1::incCounter ()
{
    implementation->incCounter();
}
/*===================================================================
                    ROUTINE: age
=================================================================== */
ACE_Time_Value AES_GCC_Event_R1::age ()
{
	return implementation->age();
}
/*===================================================================
                    ROUTINE: resetAge
=================================================================== */
void AES_GCC_Event_R1::resetAge ()
{
    implementation->resetAge();
}
/*===================================================================
                    ROUTINE: operator <<(const ACE_TCHAR*)
=================================================================== */
AES_GCC_Event_R1 & AES_GCC_Event_R1::operator << (const ACE_TCHAR *mycomment)
{
	*implementation << mycomment;
    return *this;
}
/*===================================================================
                    ROUTINE: errorCode(ACE_INT64)
=================================================================== */
AES_GCC_Event_R1 & AES_GCC_Event_R1::operator << (ACE_INT64 detail)
{
	*implementation << detail;
    return *this;
}
/*===================================================================
                    ROUTINE: operator <<
=================================================================== */
AES_GCC_Event_R1 & AES_GCC_Event_R1::operator << (std::istream &is)
{
	*implementation << is;
    return *this;
}
/*===================================================================
                    ROUTINE: str
=================================================================== */
std::string AES_GCC_Event_R1::str ()
{
	return implementation->str();
}
/*===================================================================
                    ROUTINE: probableCause
=================================================================== */
const ACE_TCHAR * AES_GCC_Event_R1::probableCause () const
{
	return implementation->probableCause();
}
/*===================================================================
                    ROUTINE: setProbableCause
=================================================================== */
void AES_GCC_Event_R1::setProbableCause (const ACE_TCHAR *cause)
{
    implementation->setProbableCause(cause);
}
/*===================================================================
                    ROUTINE: manualCease
=================================================================== */
bool AES_GCC_Event_R1::manualCease ()
{
    return implementation->manualCease();
}
/*===================================================================
                    ROUTINE: setManualCease
=================================================================== */
void AES_GCC_Event_R1::setManualCease (bool mCease)
{
    implementation->setManualCease(mCease);
}
/*===================================================================
                    ROUTINE: objectOfReference
=================================================================== */
const ACE_TCHAR* AES_GCC_Event_R1::objectOfReference() const
{
	return implementation->objectOfReference();
}
/*===================================================================
                    ROUTINE: setObjectOfReference
=================================================================== */
void AES_GCC_Event_R1::setObjectOfReference(const ACE_TCHAR* objRef)
{
    implementation->setObjectOfReference(objRef);
}
