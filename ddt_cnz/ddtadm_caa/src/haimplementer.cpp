//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************
#include "haimplementer.h"

#include "common/programconstants.h"
#include "common/tracer.h"
#include "common/logger.h"


AES_DDT_TRACE_DEFINE(AES_DDT_HaImplementer)


ACS_APGCC_ReturnType HaImplementer::performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previous_state)
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "STATE TRANSISTION TO ACTIVE prev = <%d>", previous_state);

	// If we are in the same previous state then nothing will be done
	if (ACS_APGCC_AMF_HA_ACTIVE == previous_state) return ACS_APGCC_SUCCESS;

	schedule(operation::START);

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType HaImplementer::performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previous_state)
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "STATE TRANSISTION TO PASSIVE prev = <%d>", previous_state);

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_STANDBY) return ACS_APGCC_SUCCESS;

	schedule(operation::STOP);

	return ACS_APGCC_SUCCESS;
}


ACS_APGCC_ReturnType HaImplementer::performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previous_state)
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "STATE TRANSISTION TO QUEISING prev = <%d>", previous_state);

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_QUIESCING) return ACS_APGCC_SUCCESS;

	schedule(operation::STOP);

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HaImplementer::performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previous_state)
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "STATE TRANSISTION TO QUIESCED prev = <%d>", previous_state);

	// If we are in the same previous state then nothing will be done
	if (previous_state == ACS_APGCC_AMF_HA_QUIESCED) return ACS_APGCC_SUCCESS;

	schedule(operation::STOP);

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HaImplementer::performComponentHealthCheck()
{
	// TODO add server check!
	// Nothing to do for now

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HaImplementer::performComponentTerminateJobs()
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "STATE TRANSISTION TO TERNMINATE");

	schedule(operation::STOP);

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HaImplementer::performComponentRemoveJobs()
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "STATE TRANSISTION TO REMOVE");

	schedule(operation::STOP);

	return ACS_APGCC_SUCCESS;
}

ACS_APGCC_ReturnType HaImplementer::performApplicationShutdownJobs()
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG, "STATE TRANSISTION TO SHUTDOWN");

	schedule(operation::STOP);

	return ACS_APGCC_SUCCESS;
}

void HaImplementer::schedule(operation::identifier_t id)
{
	AES_DDT_TRACE_FUNCTION;
	operation::Creator operationFactoryCreator;
	operationFactoryCreator.schedule(id);
}

