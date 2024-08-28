/*=================================================================== */
/**
@file cdhcriticalsection.cpp

Class method implementation for cdhcriticalsection.h

DESCRIPTION
The services provided by CDHCriticalSection facilitates the maintainance of
criticalsection.

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

N/A       21/07/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <cdhcriticalsection.h>
#include <ace/Mutex.h>


// Initialize the static pointer
std::auto_ptr<CDHCriticalSection> CDHCriticalSection::instancePtr;
ACE_Mutex CDHCriticalSection::m_mutex;


/*===================================================================
   ROUTINE:CDHCriticalSection
=================================================================== */
CDHCriticalSection::CDHCriticalSection()
{
    ACE_OS::thread_mutex_init (  &section);
}


/*===================================================================
   ROUTINE:~CDHCriticalSection
=================================================================== */
CDHCriticalSection::~CDHCriticalSection()
{
	ACE_OS::thread_mutex_destroy ( &section);
}


/*===================================================================
   ROUTINE:enter
=================================================================== */
void CDHCriticalSection::enter ()
{
	ACE_OS::thread_mutex_lock(  &section);
}


/*===================================================================
   ROUTINE:leave
=================================================================== */
void CDHCriticalSection::leave ()
{
	ACE_OS::thread_mutex_unlock(  &section);
}


/*===================================================================
   ROUTINE:instance
=================================================================== */
CDHCriticalSection& CDHCriticalSection::instance ()
{
	if(!instancePtr.get())
	{
		m_mutex.acquire();
		if(!instancePtr.get())
		{
			std::auto_ptr<CDHCriticalSection> temp = std::auto_ptr<CDHCriticalSection>(new CDHCriticalSection);
			instancePtr = temp;
		}
		m_mutex.release();
	}
	return *instancePtr;

}
