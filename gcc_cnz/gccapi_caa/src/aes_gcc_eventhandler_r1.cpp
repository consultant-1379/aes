/*===================================================================*/
	/**
	@file aes_gcc_eventhandler_r1.cpp

	Class method implementation for aes_gcc_eventhandler_r1.h.

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
#include "acs_aeh_evreport.h"
#include "acs_aeh_types.h"
#include "aes_gcc_event.h"
#include "aes_gcc_eventhandler_r1.h"
#include "aes_gcc_eventhandlerimplementation.h"

std::auto_ptr<AES_GCC_EventHandler_R1> AES_GCC_EventHandler_R1::pInstance;
ACE_Mutex AES_GCC_EventHandler_R1::m_mutex;
std::string AES_GCC_EventHandler_R1::processName = "";
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventHandler_R1::AES_GCC_EventHandler_R1():implementation(0)
{
    implementation = new AES_GCC_EventHandlerImplementation;
}
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_EventHandler_R1::~AES_GCC_EventHandler_R1()
{
	if( implementation != 0 )
	{
		delete implementation;
		implementation = 0;
	}
}
/*===================================================================
                    ROUTINE: filter
=================================================================== */
ACE_INT32 AES_GCC_EventHandler_R1::filter (AES_GCC_Event &ex, AES_GCC_Event *&pex)
{
	return implementation->filter(ex, pex);
}
/*===================================================================
                    ROUTINE: event
=================================================================== */
bool AES_GCC_EventHandler_R1::event (AES_GCC_Event &ex, bool forced)
{
	return implementation->event(ex, forced);
}
/*===================================================================
                    ROUTINE: alarm
=================================================================== */
bool AES_GCC_EventHandler_R1::alarm (AES_GCC_Event &ex, const ACE_TCHAR *severity)
{
	return implementation->alarm(ex, severity);
}
/*===================================================================
                    ROUTINE: cease
=================================================================== */
bool AES_GCC_EventHandler_R1::cease (AES_GCC_Event &ex )
{
	return implementation->cease(ex);
}
/*===================================================================
                    ROUTINE: report
=================================================================== */
bool AES_GCC_EventHandler_R1::report (ACE_INT32 anerror, acs_aeh_specificProblem eventnumber, const ACE_TCHAR *afile, ACE_INT32 aline, const ACE_TCHAR *problemdata, const ACE_TCHAR *problemtext)
{
	return implementation->report(anerror, eventnumber, afile, aline, problemdata, problemtext);
}
/*===================================================================
                    ROUTINE: instance
=================================================================== */
AES_GCC_EventHandler_R1& AES_GCC_EventHandler_R1::instance ()
{
	if(!pInstance.get())
	{
		m_mutex.acquire();
		if( !pInstance.get())
		{
			std::auto_ptr<AES_GCC_EventHandler_R1> temp = std::auto_ptr<AES_GCC_EventHandler_R1>(new AES_GCC_EventHandler_R1);
			pInstance = temp;
		}
		m_mutex.release();
	}
	return *pInstance;
}
bool AES_GCC_EventHandler_R1::cease (AES_GCC_Event &ex, bool startupFlag)
{
	return implementation->cease(ex, startupFlag);
}
