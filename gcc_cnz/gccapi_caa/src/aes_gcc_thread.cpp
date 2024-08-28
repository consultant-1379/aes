/* =================================================================== */
	/**
	@file aes_gcc_thread.cpp

	Class method implementation for aes_gcc_thread.h

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------

	N/A       01/08/2011    xnadnar      Initial Release
	*/
/* =================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "aes_gcc_thread.h"
#include "aes_gcc_log.h"
#include <ace/ACE.h>
#include <iostream>
using namespace std;

GCC_TDEF(aes_gcc_thread);
//*****************************************************************************
//
ACE_UINT64 AES_GCC_Thread::entry(void *obj)
{
	GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread::entry method"));
	// Cast the object pointer
	AES_GCC_Thread * thread = (AES_GCC_Thread *)obj;

	// Run the methods and save the return value from exec()
	thread->init();
	thread->rValue_ = thread->exec();
	thread->finish();

	// Check if the thread should be removed
	if (thread->autoClean_)
	{
		//cout<<"autoclean = "<<thread->autoClean_<<endl;
	        GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread::entry autoclean"));
		delete thread;
	}
	GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread::entry method exit"));
	return 0;
}


/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Thread::AES_GCC_Thread(bool autoClean):
								autoClean_(autoClean),
								thStatus_(-1),
								exit_(false)
{
	//ace_thr_mgr = ACE_Thread_Manager::instance();
	ID_ = 0;
	rValue_ = -1;
	ace_thr_mgr = NULL;
	ace_thr_mgr = new(std::nothrow) ACE_Thread_Manager();
	GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread constructor "));
}


/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_Thread::~AES_GCC_Thread()
{
	//	delete ace_thr_mgr;
	#if 0
	ace_thr_mgr->close_singleton();
	#endif
	delete ace_thr_mgr;
	ace_thr_mgr = NULL;
	GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread destructor "));
}

/*===================================================================
                    ROUTINE: activate
=================================================================== */
bool AES_GCC_Thread::activate()
{
	GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread::activate method "));
	// Clear the exit flag
	exit_ = false;
	// Check if the thread is already activated
	
	GCC_TRACE((aes_gcc_thread,"%s", "the value of theStatus_ =%u\n ", thStatus_));
	if (thStatus_ != -1)
		return false;

	// Create the thread
	const ACE_TCHAR * pthreadName = "AES_GCC_Thread";
        if (autoClean_)
        {
             thStatus_ = ace_thr_mgr->spawn((ACE_THR_FUNC)AES_GCC_Thread::entry,
                         (void*)this ,
                         THR_NEW_LWP | THR_DETACHED | THR_INHERIT_SCHED,
                         &ID_,
                         0,
                         ACE_DEFAULT_THREAD_PRIORITY,
                         -1,
                         0,
                         ACE_DEFAULT_THREAD_STACKSIZE,
                         &pthreadName);

        }        
        else
        {
	    thStatus_ = ace_thr_mgr->spawn((ACE_THR_FUNC)AES_GCC_Thread::entry,
			(void*)this ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&ID_,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&pthreadName);
        }
	// Check if all is OK
	if (thStatus_ == -1)
	{
		GCC_TRACE((aes_gcc_thread,"%s","Failed to spawn thread , returning false "));
		return false;
	}
	GCC_TRACE((aes_gcc_thread,"%s","Exit from AES_GCC_Thread::activate"));
	return true;                // Everything is OK
}

/*===================================================================
                    ROUTINE: deActivate
=================================================================== */
void AES_GCC_Thread::deActivate()
{
	// Set the exit_ boolean to true
	exit_ = true;
}
/*===================================================================
                    ROUTINE: wait
=================================================================== */
bool AES_GCC_Thread::wait(int timeout) //timeout is in  milli seconds
{
	GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread::wait method"));
	ACE_Time_Value tv(0,1000*timeout);
	// Wait for the thread
	int r = 0;
	if( timeout == 0 )
	{
		r = ace_thr_mgr->wait(0,1,0);
	}
	else
	{

		r = ace_thr_mgr->wait(&tv,1,0);
	}
	GCC_TRACE((aes_gcc_thread,"Thread wait status : %d",r));
	bool result = false;

	// Check if we got a signal
	if (r == 0)
	{
		GCC_TRACE((aes_gcc_thread,"Thread wait signaled : %d",r));
		ace_thr_mgr->join(ID_);
		result = true;
	}
	return result;
}

/*===================================================================
                    ROUTINE: wait
=================================================================== */
bool AES_GCC_Thread::wait(int &rv, int timeout) //timeout is in milli seconds
{
	GCC_TRACE((aes_gcc_thread,"%s","In AES_GCC_Thread::wait method"));
	// Wait for the thread
	ACE_Time_Value tv(0,1000*timeout);
	int r;
	if( timeout == 0 )
	{
		r = ace_thr_mgr->wait();
	}
	else
	{
		r = ace_thr_mgr->wait(&tv,0,0);
	}
		
	GCC_TRACE((aes_gcc_thread,"Thread wait status :%d",r));
	bool result = false;
	// Check if we got a signal
	if (r == 0)
	{
		rv = rValue_;
		ace_thr_mgr->join(ID_);
		GCC_TRACE((aes_gcc_thread,"Thread wait signaled :%d",r));
		result = true;
	}
	return result;
}
/*===================================================================
                    ROUTINE: init
=================================================================== */
void AES_GCC_Thread::init()
{
	// This is intentionally left empty
	// and could be overridden by a inherited class
}

/*===================================================================
                    ROUTINE: finish
=================================================================== */
void AES_GCC_Thread::finish()
{
	// This is intentionally left empty
	// and could be overridden by a inherited class
}
