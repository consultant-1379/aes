/*=================================================================== */
/**
   @file aes_afp_ha_service.cpp

   Class method implementation for AES module.

   This module contains the implementation of class declared in
   the aes_afp_ha_service.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       14/09/2011   XTANAGG   Initial Release
 */
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_afp_ha_service.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"

AES_AFP_TRACE_DEFINE(AES_AFP_HA_Service);
/*===================================================================
   ROUTINE: AES_AFP_HA_Service
=================================================================== */
AES_AFP_HA_Service::AES_AFP_HA_Service(const char* daemon_name, const char* username):
ACS_APGCC_ApplicationManager(daemon_name, username)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	afpAdmPtr = 0;
	AES_AFP_TRACE_MESSAGE("Leaving");
}//End of Constructor

/*===================================================================
   ROUTINE: ~AES_AFP_HA_Service
=================================================================== */
AES_AFP_HA_Service::~AES_AFP_HA_Service()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_TRACE_MESSAGE("Leaving");
}//End of Destructor


/*===================================================================
   ROUTINE: performStateTransitionToActiveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_AFP_HA_Service::performStateTransitionToActiveJobs(
		ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->goActive();
	AES_AFP_TRACE_MESSAGE("Leaving with return type : %d", ret);
	return ret;

}//End of performStateTransitionToActiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToPassiveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_AFP_HA_Service::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	(void)previousHAState;
	ACS_APGCC_ReturnType ret = this->goPassive();
	AES_AFP_TRACE_MESSAGE("Leaving with return type : %d", ret);
	return ret;

}//End of performStateTransitionToPassiveJobs

/*===================================================================
   ROUTINE: performStateTransitionToQueisingJobs
=================================================================== */
ACS_APGCC_ReturnType AES_AFP_HA_Service::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	(void)previousHAState;
	AES_AFP_LOG(LOG_LEVEL_WARN,"AFP received performStateTransitionToQueisingJobs");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_AFP_TRACE_MESSAGE("Leaving with return type : %d", ret);
	return ret;

}//End of performStateTransitionToQueisingJobs

/*===================================================================
   ROUTINE: performStateTransitionToQuiescedJobs
=================================================================== */
ACS_APGCC_ReturnType AES_AFP_HA_Service::performStateTransitionToQuiescedJobs(
		ACS_APGCC_AMF_HA_StateT previousHAState)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	(void)previousHAState;
	AES_AFP_LOG(LOG_LEVEL_WARN,"AFP received performStateTransitionToQuiescedJobs");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_AFP_TRACE_MESSAGE("Leaving with return type : %d", ret);
	return ret;
}//End of performStateTransitionToQuiescedJobs

/*===================================================================
   ROUTINE: performComponentHealthCheck
=================================================================== */
ACS_APGCC_ReturnType AES_AFP_HA_Service::performComponentHealthCheck(void)
{
	//TODO, Implement Health Check for Application.
	return ACS_APGCC_SUCCESS;

}//End of performComponentHealthCheck

/*===================================================================
   ROUTINE: performComponentTerminateJobs
=================================================================== */
ACS_APGCC_ReturnType AES_AFP_HA_Service::performComponentTerminateJobs(void)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_LOG(LOG_LEVEL_WARN,"AFP received performComponentTerminateJobs");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_AFP_TRACE_MESSAGE("Leaving with return type : %d", ret);
	return ret;
}//End of performComponentTerminateJobs

/*===================================================================
   ROUTINE: performComponentRemoveJobs
=================================================================== */
ACS_APGCC_ReturnType AES_AFP_HA_Service::performComponentRemoveJobs(void)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_LOG(LOG_LEVEL_INFO,"AFP received performComponentRemoveJobs");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_AFP_TRACE_MESSAGE("Leaving with return type : %d", ret);
	return ret;
}//End of performComponentRemoveJobs

/*========================================================================
	ROUTINE: performApplicationShutdownJobs
========================================================================*/
ACS_APGCC_ReturnType AES_AFP_HA_Service::performApplicationShutdownJobs()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_LOG(LOG_LEVEL_WARN,"AFP received performApplicationShutdownJobs");
	ACS_APGCC_ReturnType ret = this->shutdown();
	AES_AFP_TRACE_MESSAGE("Leaving with return type : %d", ret);
	return ret;
}//End of performApplicationShutdownJobs


/*========================================================================
	ROUTINE: goActive
========================================================================*/
ACS_APGCC_ReturnType AES_AFP_HA_Service::goActive()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	if(afpAdmPtr == 0)
	{
		afpAdmPtr = new (std::nothrow) aes_afp_services();

		if( afpAdmPtr != 0 )
		{
			AES_AFP_LOG(LOG_LEVEL_WARN,"AFP service is running as ACTIVE!! and Starting business Thread");
			AES_AFP_TRACE_MESSAGE("Starting business Thread");
			ACS_CC_ReturnType myReturnErrorCode = afpAdmPtr->setupAFPThread(afpAdmPtr);
			if(myReturnErrorCode == ACS_CC_FAILURE)
			{
				delete afpAdmPtr;
				afpAdmPtr = 0;
				AES_AFP_TRACE_MESSAGE("Leaving");
				return ACS_APGCC_FAILURE;
			}
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR,"Memory allocation failed for afpAdmPtr");
			AES_AFP_TRACE_MESSAGE("Memory allocation failed for afpAdmPtr");
			AES_AFP_LOG(LOG_LEVEL_WARN,"Leaving");
			AES_AFP_TRACE_MESSAGE("Leaving");
			return ACS_APGCC_FAILURE;
		}
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_WARN,"aes_afpd is already active.");
		AES_AFP_TRACE_MESSAGE("aes_afpd is already active.");
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: goPassive
========================================================================*/
ACS_APGCC_ReturnType AES_AFP_HA_Service::goPassive()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_LOG(LOG_LEVEL_WARN,"AFP service is running as PASSIVE!!");
	//If the service needs to perform any functional activities on the passive node 
	//in future, please add the neccessary code below.
	AES_AFP_TRACE_MESSAGE("Leaving");
	return ACS_APGCC_SUCCESS;
}

/*========================================================================
	ROUTINE: shutdown
========================================================================*/
ACS_APGCC_ReturnType AES_AFP_HA_Service::shutdown()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_LOG(LOG_LEVEL_WARN,"AFP service shutdown started");
	if(afpAdmPtr != 0)
	{
		if( afpAdmPtr->StopEvent != 0)
		{
			afpAdmPtr->StopEvent->signal();
			aes_afp_services::afpStopEvt = true;
			if( afpAdmPtr->m_afpMainThreadId != 0 )
			{
				ACE_Thread_Manager::instance()->join(afpAdmPtr->m_afpMainThreadId);
			}
		}
		delete afpAdmPtr;
		afpAdmPtr = 0;

	}
	AES_AFP_TRACE_MESSAGE("Leaving");
	AES_AFP_LOG(LOG_LEVEL_WARN,"AFP service shutdown is completed");
	return ACS_APGCC_SUCCESS;
}
