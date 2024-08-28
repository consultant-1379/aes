/*=================================================================== */
/**
   @file aes_dbo_ha_service.cpp

   Class method implementation for DBO module.

   This module contains the implementation of class declared in
   the AES_DBO_HA_Service.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       12/10/2011   XBHADUR   Initial Release
 */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_dbo_ha_service.h>
#include <aes_gcc_tracer.h>

#include <sys/capability.h>

/*===================================================================
			GLOBAL VARIABLE
 =================================================================== */
AES_GCC_TRACE_DEFINE(AES_DBO_HAService)

/*===================================================================
   ROUTINE: AES_DBO_HA_Service
=================================================================== */
AES_DBO_HA_Service::AES_DBO_HA_Service(const char* daemon_name, const char* username):
ACS_APGCC_ApplicationManager(daemon_name, username),
m_applicationThreadId(0),
theServR(0)
{
	AES_GCC_TRACE_MESSAGE("In constructor");

	// Clear CAP_SYS_RESOURCE bit thus root user cannot override disk quota limits
	cap_t cap = cap_get_proc();

	if(NULL != cap)
	{
		cap_value_t cap_list[1];
		cap_list[0] = CAP_SYS_RESOURCE;

		// Clear capability CAP_SYS_RESOURCE
		if(cap_set_flag(cap, CAP_EFFECTIVE, 1, cap_list, CAP_CLEAR) == -1)
		{
			// handle error
			AES_GCC_TRACE_MESSAGE("cap_set_flag() failed, error = %s", strerror(errno));
		}
		else
		{
			// Change process capability
			if (cap_set_proc(cap) == -1)
			{
				// handle error
				AES_GCC_TRACE_MESSAGE("cap_set_proc() failed, error = %s", strerror(errno));
			}
		}

		if(cap_free(cap) == -1)
		{
			// handle error
			AES_GCC_TRACE_MESSAGE("cap_free() failed, error = %s", strerror(errno));
		}
	}
	else
	{
		// handle error
		AES_GCC_TRACE_MESSAGE("cap_get_proc() failed, error = %s", strerror(errno));
	}
}//End of Constructor

/*===================================================================
   ROUTINE: ~AES_DBO_HA_Service
=================================================================== */
AES_DBO_HA_Service::~AES_DBO_HA_Service()
{
	AES_GCC_TRACE_MESSAGE("In destructor");
}

/*===================================================================
   ROUTINE: DBO_serviceMain
=================================================================== */
ACE_THR_FUNC_RETURN DBO_serviceMain(void *ptr )
{
	AES_GCC_TRACE_MESSAGE("In");
	aes_dbo_server * servR = (aes_dbo_server*)ptr;
	servR->run_reactor_service();
	AES_GCC_TRACE_MESSAGE("Out");
	return 0;
}

/*===================================================================
   ROUTINE: performStateTransitionToActiveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_DBO_HA_Service::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_GCC_TRACE_MESSAGE("Entering");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->goActive();
	AES_GCC_TRACE_MESSAGE("Leaving ");
	return ret;
}//End of performStateTransitionToActiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToPassiveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_DBO_HA_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_GCC_TRACE_MESSAGE("Entering");
	(void) previousHAState;
	ACS_APGCC_ReturnType ret = this->goPassive();
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ret;
}//End of performStateTransitionToPassiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToQueisingJobs
=================================================================== */
ACS_APGCC_ReturnType AES_DBO_HA_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_GCC_TRACE_MESSAGE("Entering");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ret;
}//End of performStateTransitionToQueisingJobs

/*===================================================================
   ROUTINE: performStateTransitionToQuiescedJobs
=================================================================== */
ACS_APGCC_ReturnType AES_DBO_HA_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_GCC_TRACE_MESSAGE("Entering");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ret;
}

/*===================================================================
   ROUTINE: performComponentHealthCheck
=================================================================== */
ACS_APGCC_ReturnType AES_DBO_HA_Service::performComponentHealthCheck(void)
{
	return ACS_APGCC_SUCCESS;
}//End of performComponentHealthCheck

/*===================================================================
   ROUTINE: performComponentTerminateJobs
=================================================================== */
ACS_APGCC_ReturnType AES_DBO_HA_Service::performComponentTerminateJobs(void)
{
	AES_GCC_TRACE_MESSAGE("Entering");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ret;
}//End of performComponentTerminateJobs

/*===================================================================
   ROUTINE: performComponentRemoveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_DBO_HA_Service::performComponentRemoveJobs(void)
{
	AES_GCC_TRACE_MESSAGE("Entering");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ret;
}//End of performComponentRemoveJobs

/*========================================================================
	ROUTINE: performApplicationShutdownJobs
========================================================================*/
ACS_APGCC_ReturnType AES_DBO_HA_Service::performApplicationShutdownJobs()
{
	AES_GCC_TRACE_MESSAGE("Entering");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ret;
}

/*========================================================================
	ROUTINE: goActive
========================================================================*/
ACS_APGCC_ReturnType AES_DBO_HA_Service::goActive()
{
	AES_GCC_TRACE_MESSAGE("Entering");

	if( theServR == 0 )
	{
		theServR =  new (std::nothrow) aes_dbo_server();
		if(theServR == 0)
		{
			AES_GCC_TRACE_MESSAGE("Creation of ServR object failed");
			return ACS_APGCC_FAILURE;
		}
		//reset signal value
		aes_dbo_server::isStopEventSignalled = false;

		AES_GCC_TRACE_MESSAGE("Starting the main functionality of DBO in a separate thread");
		int threadGroupId =
			ACE_Thread_Manager::instance()->spawn(&DBO_serviceMain,
								(void *)theServR ,
								THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
								&m_applicationThreadId,
								0,
								ACE_DEFAULT_THREAD_PRIORITY,
								-1,
								0,
								ACE_DEFAULT_THREAD_STACKSIZE);
		if (threadGroupId == -1)
		{
			AES_GCC_TRACE_MESSAGE("Error occurred while creating the DBO application thread");
			return ACS_APGCC_FAILURE;
		}
		theServR->open();;
		theServR->start_service();
	}
	else
	{
		AES_GCC_TRACE_MESSAGE("aes_dbod is already active!");
	}
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: goPassive
========================================================================*/
ACS_APGCC_ReturnType AES_DBO_HA_Service::goPassive()
{
	AES_GCC_TRACE_MESSAGE("Entering");
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: shutdown
========================================================================*/
ACS_APGCC_ReturnType AES_DBO_HA_Service::shutdown()
{
	AES_GCC_TRACE_MESSAGE("Entering");
	if( theServR != 0 )
	{
			aes_dbo_server::StopEvent->signal();
			aes_dbo_server::isStopEventSignalled = true;
			theServR->close();
			if( m_applicationThreadId != 0 )
			{
				ACE_Thread_Manager::instance()->join( m_applicationThreadId);
			}
			delete theServR;
			theServR = 0;
	}
	AES_GCC_TRACE_MESSAGE("Leaving");
	return ACS_APGCC_SUCCESS;
}
