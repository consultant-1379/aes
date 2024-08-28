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
#include "imm/transferprogress_oi.h"
#include "imm/imm.h"
#include "common/utility.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/utility.h"
	#include "imm/imm.h"
	#include "engine/workingset.h"

	#include <ACS_APGCC_Util.H>
#else
	#include "stubs/operation_stub.h"
#endif


AES_DDT_TRACE_DEFINE(AES_DDT_IMM_TransferProgress_OI)

namespace imm
{

	TransferProgress_OI::TransferProgress_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName) :
			Base_OI(managedObjectClassName, objectImplementerName)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	TransferProgress_OI::~TransferProgress_OI()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ACS_CC_ReturnType TransferProgress_OI::updateRuntime(const char* p_objName, const char** p_attrName)
	{
		AES_DDT_TRACE_MESSAGE("MO:<%s>", p_objName);

		ACS_CC_ReturnType result = ACS_CC_SUCCESS;

		// Object DN
		std::string objectDn(p_objName);

		std::string producerName;
		std::string dataSourceName;

		// get the datasource and cp name from DN
		if(common::utility::getRDNValue(objectDn, imm::datasource_attribute::RDN, dataSourceName) &&
			common::utility::getRDNValue(objectDn, imm::transferprogress_attribute::RDN, producerName ))
		{
			ACS_APGCC::toUpper(dataSourceName);

			AES_DDT_TRACE_MESSAGE("DataSource:<%s> and Producer:<%s>", dataSourceName.c_str(), producerName.c_str());

			for(int attrIndex = 0; NULL != p_attrName[attrIndex]; ++attrIndex)
			{
				AES_DDT_TRACE_MESSAGE("DataSource:<%s> and Producer:<%s> update attribute:<%s>", dataSourceName.c_str(), producerName.c_str(), p_attrName[attrIndex]);

				if(0 == imm::transferprogress_attribute::LASTRECEIVEDMESSAGE.compare(p_attrName[attrIndex]))
				{
					uint64_t lastReceivedMsg = 0U;

					if ( common::errorCode::ERR_NO_ERRORS != engine::workingSet_t::instance()->getStoringManager().getLastReceivedMessage(dataSourceName, producerName, lastReceivedMsg))
					{
						AES_DDT_TRACE_MESSAGE("[%s@%s] ERROR: Not able obtain lastReceivedMessage", dataSourceName.c_str(), producerName.c_str());
						AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] Not able obtain lastReceivedMessage", dataSourceName.c_str(), producerName.c_str());
					}

					result = modifyRuntimeAttribute(lastReceivedMsg, p_objName, p_attrName[attrIndex]);
					continue;
				}

				if(0 == imm::transferprogress_attribute::LASTSENTMESSAGE.compare(p_attrName[attrIndex]))
				{

					uint64_t lastSentMsg = 0U;
					if( common::errorCode::ERR_NO_ERRORS != engine::workingSet_t::instance()->getStoringManager().getLastSentMessage(dataSourceName, producerName, lastSentMsg))
					{
						AES_DDT_TRACE_MESSAGE("[%s@%s] ERROR: Not able obtain lastSentMessage", dataSourceName.c_str(), producerName.c_str());
						AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] Not able obtain lastSentMessage", dataSourceName.c_str(), producerName.c_str());
					}

					result = modifyRuntimeAttribute(lastSentMsg, p_objName, p_attrName[attrIndex]);
					continue;
				}
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%d>", result);
		return result;
	}

	ACS_CC_ReturnType TransferProgress_OI::modifyRuntimeAttribute(uint64_t& attrvalue, const char* objName, const char* attrName)
	{
		AES_DDT_TRACE_FUNCTION;
		ACS_CC_ImmParameter runtimeAttr;
		char tmpBuffer[64] = {0};
		strcpy(tmpBuffer , attrName);

		runtimeAttr.attrName = tmpBuffer;
		runtimeAttr.attrType = ATTR_UINT64T;
		runtimeAttr.attrValuesNum = 1U;

		void* value[1] = {reinterpret_cast<void*>(&attrvalue)};
		runtimeAttr.attrValues = value;

		ACS_CC_ReturnType result = modifyRuntimeObj(objName, &runtimeAttr );

		if(ACS_CC_SUCCESS != result)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "modifyRuntimeObj(%s) failed for MO<%s>, errorCode:<%d> errorMsg:<%s>",
					attrName, objName, getInternalLastError(), getInternalLastErrorText());
			AES_DDT_TRACE_MESSAGE("ERROR: OmHandler modifyAttribute failed for <%s>, errorCode:<%d> errorMsg:<%s>", objName, getInternalLastError(), getInternalLastErrorText());
		}

		return result;
	}

} /* namespace imm */
