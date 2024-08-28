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
#include "imm/base_oi.h"

#include "imm/imm.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "operation/creator.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/macros.h"

	#include <ace/Future.h>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_Base_OI)


namespace imm
{

	Base_OI::Base_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName) :
			ACE_Event_Handler(),
			acs_apgcc_objectimplementerinterface_V3(objectImplementerName),
			m_managedObjectClassName(managedObjectClassName),
			m_objectImplementerName(objectImplementerName),
			m_immRegistered(false),
			m_reactorRegistered(false),
			m_currentOperationId(operation::NOOP)
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_INFO, "CREATED IMPLEMENTER. ECIM CLASS: <%s>, OI NAME: <%s>", m_managedObjectClassName.c_str(), m_objectImplementerName.c_str());
	}

	Base_OI::~Base_OI()
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_INFO, "DESTROYED IMPLEMENTER <%s>", m_managedObjectClassName.c_str());
	}

	int Base_OI::handle_input (ACE_HANDLE /*fd*/)
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "IMM Handle has been signaled for <%s>", m_managedObjectClassName.c_str());

		ACS_CC_ReturnType result = dispatch(ACS_APGCC_DISPATCH_ONE);
		if(ACS_CC_SUCCESS != result)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "<%s> dispatch failed!! result: <%d>", m_managedObjectClassName.c_str(), result);
			AES_DDT_TRACE_MESSAGE("ERROR: <%s> dispatch failed!! result: <%d>", m_managedObjectClassName.c_str(), result);
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "dispatch succeeded for <%s>", m_managedObjectClassName.c_str());
			AES_DDT_TRACE_MESSAGE("dispatch succeeded for <%s>", m_managedObjectClassName.c_str());
		}

		//handle_input always returns 0, otherwise the reactore removes Base_OI from its handlers
		return common::errorCode::ERR_NO_ERRORS;
	}

	int Base_OI::handle_close (ACE_HANDLE /*handle*/, ACE_Reactor_Mask /*close_mask*/)
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Ignore Handle Close for <%s>", m_managedObjectClassName.c_str());

		return common::errorCode::ERR_NO_ERRORS;
	}


	ACS_CC_ReturnType Base_OI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");
		AES_DDT_TRACE_MESSAGE("**** CREATE OBJ OF CLASS: <%s> ****", className);

		UNUSED(oiHandle);
		UNUSED(ccbId);
		UNUSED(className);
		UNUSED(parentname);
		UNUSED(attr);

		return ACS_CC_SUCCESS;
	}

	ACS_CC_ReturnType Base_OI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");
		AES_DDT_TRACE_MESSAGE("**** DELETE OBJ: <%s> ****", objName);

		UNUSED(oiHandle);
		UNUSED(ccbId);
		UNUSED(objName);

		return ACS_CC_SUCCESS;
	}

	ACS_CC_ReturnType Base_OI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		AES_DDT_TRACE_MESSAGE("**** MODIFY OBJ: <%s> ****", objName);

		UNUSED(oiHandle);
		UNUSED(ccbId);
		UNUSED(attrMods);

		return ACS_CC_SUCCESS;
	}

	ACS_CC_ReturnType Base_OI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");
		AES_DDT_TRACE_MESSAGE("**** COMPLETE CALLBACK ****");

		UNUSED(oiHandle);
		UNUSED(ccbId);

		return ACS_CC_SUCCESS;
	}

	void Base_OI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");
		AES_DDT_TRACE_MESSAGE("**** ABORT CALLBACK ****");

		UNUSED(oiHandle);
		UNUSED(ccbId);

		return;
	}

	void Base_OI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");
		AES_DDT_TRACE_MESSAGE("**** APPLY CALLBACK ****");

		UNUSED(oiHandle);
		UNUSED(ccbId);

		return;
	}

	ACS_CC_ReturnType Base_OI::updateRuntime(const char* p_objName, const char** p_attrName)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "In");
		AES_DDT_TRACE_MESSAGE("**** UPDATE OBJ: <%s> ****", p_objName);

		UNUSED(p_objName);
		UNUSED(p_attrName);

		return ACS_CC_SUCCESS;
	}

	void Base_OI::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList)
	{
		AES_DDT_LOG(LOG_LEVEL_INFO, "IMM Action requested: oiHandle == %llu, invocation == %llu, p_objName == '%s' , operationId == %llu", oiHandle, invocation, p_objName, operationId);

		UNUSED(paramList);

		//  invoke the correct action handler
		int action_result = SA_AIS_OK;

		//switch(operationId) and set action_result

		// set operation result
		ACS_CC_ReturnType imm_res = adminOperationResult(oiHandle, invocation, action_result);
		if(imm_res != ACS_CC_SUCCESS)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Call 'ACS_APGCC_RuntimeOwner_V3::adminOperationResult(..)' failed ! call_result == %d, imm last error == %d, imm last error text == '%s'",
				imm_res, getInternalLastError(), getInternalLastErrorText());
		}

		AES_DDT_LOG(LOG_LEVEL_INFO, "IMM Action executed. Result == %d. ", action_result);
	}

	bool Base_OI::sendRequestAndWaitResult(const operation::identifier_t& id, const void* op_info, bool reportErrorToImm )
	{
		AES_DDT_TRACE_FUNCTION;

		operation::result requestResult;
		ACE_Future<operation::result> waitResult;

		operation::Creator factoryCreator;
		factoryCreator.schedule(id, &waitResult, op_info);

		AES_DDT_TRACE_MESSAGE("Wait operation request:<%d> result...", id);
		waitResult.get(requestResult);

		if( requestResult.fail() )
		{
			AES_DDT_TRACE_MESSAGE("... operation request<%d> failed,"
					" errorCode:<%d>, errorMsg:<%s>", id, requestResult.getErrorCode(), requestResult.getErrorMessage() );

			if(reportErrorToImm)
			{
				// set the exit code to the caller
				setExitCode(requestResult.getErrorCode(), requestResult.getErrorMessage());
			}
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("... operation request<%d> succeeded.", id);
		}

		return requestResult.good();
	}

} /* namespace imm */
