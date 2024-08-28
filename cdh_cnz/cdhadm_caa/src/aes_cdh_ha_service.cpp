/*=================================================================== */
/**
   @file aes_cdh_ha_service.cpp

   Class method implementation for CDH module.

   This module contains the implementation of class declared in
   the AES_CDH_HA_Service.h module

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
#include <aes_cdh_ha_service.h>
#include <servr.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
using namespace std;
/*===================================================================
			GLOBAL VARIABLE
 =================================================================== */
AES_CDH_TRACE_DEFINE(AES_CDH_HA_Service);
/*===================================================================
   ROUTINE: AES_CDH_HA_Service
=================================================================== */
AES_CDH_HA_Service::AES_CDH_HA_Service(const char* daemon_name, const char* username):ACS_APGCC_ApplicationManager(daemon_name, username)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	m_applicationThreadId = 0;
	theServR = 0;
	AES_CDH_TRACE_MESSAGE("Leaving");

}//End of Constructor

/*===================================================================
   ROUTINE: ~AES_CDH_HA_Service
=================================================================== */
AES_CDH_HA_Service::~AES_CDH_HA_Service()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
   ROUTINE: CDH_serviceMain
=================================================================== */
ACE_THR_FUNC_RETURN CDH_serviceMain(void *ptr)
{
	AES_CDH_LOG(LOG_LEVEL_INFO, "Calling Servr open() for main functionality");
	ServR * servR = (ServR*)ptr;
	servR->open();
	AES_CDH_TRACE_MESSAGE("CDH_serviceMain::Starting run_service ");
	servR->StartService();

	return 0;
}

/*===================================================================
   ROUTINE: performStateTransitionToActiveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_CDH_HA_Service::performStateTransitionToActiveJobs(
		ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "performStateTransitionToActiveJobs invoked");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->goActive();
	AES_CDH_TRACE_MESSAGE("Leaving  with return type : %d", ret);
	return ret;
}//End of performStateTransitionToActiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToPassiveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_CDH_HA_Service::performStateTransitionToPassiveJobs(
					ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "performStateTransitionToPassiveJobs invoked");
	(void) previousHAState;
	ACS_APGCC_ReturnType ret = this->goPassive();
	AES_CDH_TRACE_MESSAGE("Leaving  with return type : %d", ret);
	return ret;
}//End of performStateTransitionToPassiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToQueisingJobs
=================================================================== */
ACS_APGCC_ReturnType AES_CDH_HA_Service::performStateTransitionToQueisingJobs(
					ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "performStateTransitionToQueisingJobs invoked");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_CDH_TRACE_MESSAGE("Leaving  with return type : %d", ret);
	return ret;
}//End of performStateTransitionToQueisingJobs

/*===================================================================
   ROUTINE: performStateTransitionToQuiescedJobs
=================================================================== */
ACS_APGCC_ReturnType AES_CDH_HA_Service::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "performStateTransitionToQuiescedJobs invoked");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_CDH_TRACE_MESSAGE("Leaving  with return type : %d", ret);
	return ret;
}

/*===================================================================
   ROUTINE: performComponentHealthCheck
=================================================================== */
ACS_APGCC_ReturnType AES_CDH_HA_Service::performComponentHealthCheck(void)
{
	return ACS_APGCC_SUCCESS;
}//End of performComponentHealthCheck

/*===================================================================
   ROUTINE: performComponentTerminateJobs
=================================================================== */
ACS_APGCC_ReturnType AES_CDH_HA_Service::performComponentTerminateJobs(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "performComponentTerminateJobs invoked");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_CDH_TRACE_MESSAGE("Leaving  with return type : %d", ret);
	return ret;
}//End of performComponentTerminateJobs

/*===================================================================
   ROUTINE: performComponentRemoveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_CDH_HA_Service::performComponentRemoveJobs(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "%s", "performComponentRemoveJobs invoked");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_CDH_TRACE_MESSAGE("Leaving  with return type : %d", ret);
	return ret;
}//End of performComponentRemoveJobs

/*========================================================================
	ROUTINE: performApplicationShutdownJobs
========================================================================*/
ACS_APGCC_ReturnType AES_CDH_HA_Service::performApplicationShutdownJobs()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "performApplicationShutdownJobs invoked");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_CDH_TRACE_MESSAGE("Leaving  with return type : %d", ret);
	return ret;
}

/*========================================================================
	ROUTINE: goActive
========================================================================*/
ACS_APGCC_ReturnType AES_CDH_HA_Service::goActive()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	if( theServR == 0 )
	{
		theServR =  new (std::nothrow) ServR();
		if(theServR == 0)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR,"Creation of ServR object failed.Error occurred while allocating memory for servR.");
			AES_CDH_TRACE_MESSAGE("Creation of ServR object failed.Error occurred while allocating memory for servR.");
			AES_CDH_TRACE_MESSAGE("Leaving");
			return ACS_APGCC_FAILURE;
		}

		AES_CDH_TRACE_MESSAGE("Starting the main functionality of CDH in a separate thread");
		AES_CDH_LOG(LOG_LEVEL_INFO, "Spawning thread for the main functionality");
		int threadGroupId =
			ACE_Thread_Manager::instance()->spawn(&CDH_serviceMain,
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
			AES_CDH_LOG(LOG_LEVEL_ERROR, " Error occurred while creating the CDH application thread");
			AES_CDH_TRACE_MESSAGE(" Error occurred while creating the CDH application thread");
			AES_CDH_TRACE_MESSAGE("Leaving");
			return ACS_APGCC_FAILURE;
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("aes_cdhd is already active.");
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: goPassive
========================================================================*/
ACS_APGCC_ReturnType AES_CDH_HA_Service::goPassive()
{
	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: shutdown
========================================================================*/
ACS_APGCC_ReturnType AES_CDH_HA_Service::shutdown()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "Initiated shutdown for CDH service");
	if( theServR != 0 )
	{
			theServR->StopEvent->signal();
			theServR->isStopEventSignalled = true;
			theServR->StopService();
			theServR->close();

			if( m_applicationThreadId != 0 )
			{
				ACE_Thread_Manager::instance()->join( m_applicationThreadId);
			}
			delete theServR;
			theServR = 0;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_APGCC_SUCCESS;
}
