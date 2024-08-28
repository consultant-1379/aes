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

#include "operation/createtransferprogress.h"
#include "common/programconstants.h"
#include "imm/imm.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "acs_apgcc_omhandler.h"
	#include "ACS_APGCC_Util.H"
	#include "acs_apgcc_error.h"
	#include "common/utility.h"
#else
	#include "stubs/imm_stubs.h"
#endif

#include <vector>

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_CreateTransferProgress)

namespace operation
{

	CreateTransferProgress::CreateTransferProgress()
	: OperationBase(TRANSFERPROGRESS_ADD)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	CreateTransferProgress::~CreateTransferProgress()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void CreateTransferProgress::setOperationDetails(const void* op_details)
	{
		AES_DDT_TRACE_FUNCTION;
		const operation::transferProgressInfo* pTransferProgressInfo = reinterpret_cast<const operation::transferProgressInfo*>(op_details);

		m_info.dataSourceName = pTransferProgressInfo->dataSourceName;
		m_info.producerName = pTransferProgressInfo->producerName;
		m_info.dataSourceDN = pTransferProgressInfo->dataSourceDN;

	}

	int CreateTransferProgress::call()
	{

		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("Create a new Tranfser Progress instance for :<%s>, producer name:<%s>", m_info.getDataSourceName(), m_info.getProducerName());

		(void)createTransferProgressObject();

		return common::errorCode::ERR_NO_ERRORS;
	}

	ACS_CC_ReturnType CreateTransferProgress::createTransferProgressObject()
	{

		AES_DDT_TRACE_FUNCTION;

		OmHandler objManager;
		// Init OM resource
		ACS_CC_ReturnType result = objManager.Init();

		if(ACS_CC_SUCCESS == result )
		{
			std::string parentName = m_info.getDataSourceDN();

			//The vector of attributes
			std::vector<ACS_CC_ValuesDefinitionType> AttrList;

			//the attributes
			ACS_CC_ValuesDefinitionType attrRDN;

			//Fill the rdn attribute name
			char tmpAttrName[64] = {0};
			imm::transferprogress_attribute::RDN.copy(tmpAttrName,sizeof(tmpAttrName));
			attrRDN.attrName = tmpAttrName;

			//RDN value
			char tmpRdnValue[256] = {0};
			snprintf(tmpRdnValue, sizeof(tmpRdnValue) - 1, "%s=%s", imm::transferprogress_attribute::RDN.c_str(), m_info.getProducerName());
			AES_DDT_TRACE_MESSAGE("RDN Value = %s", tmpRdnValue);
			void* valueRDN[1]={reinterpret_cast<void*>(tmpRdnValue)};

			attrRDN.attrType = ATTR_STRINGT;
			attrRDN.attrValuesNum = 1;
			attrRDN.attrValues = valueRDN;

			AttrList.push_back(attrRDN);

			AES_DDT_LOG(LOG_LEVEL_INFO,"Class Name : <%s>, Parent DN: <%s>", imm::moc_name::TRANSFER_PROGRESS.c_str(), parentName.c_str());
			AES_DDT_TRACE_MESSAGE("Class Name : <%s>, Parent DN: <%s>", imm::moc_name::TRANSFER_PROGRESS.c_str(), parentName.c_str());

			//Create the TransferProgress MO.
			result = objManager.createObject(imm::moc_name::TRANSFER_PROGRESS.c_str(), parentName.c_str(), AttrList);

			if(ACS_CC_SUCCESS != result )
			{

				if (objManager.getInternalLastError() != imm::error_code::ERR_IMM_ALREADY_EXIST)
				{
					// createObject failed, check for ERR_ELEMENT_ALREADY_EXIST
					AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler createObject(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
							m_info.getProducerName(), objManager.getInternalLastError(), objManager.getInternalLastErrorText() );

					AES_DDT_TRACE_MESSAGE("ERROR: OmHandler createObject(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
							m_info.getProducerName(), objManager.getInternalLastError(), objManager.getInternalLastErrorText() );
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_INFO, "TransferProgress MO already exists for <%s> and <%s> ",m_info.getDataSourceName(),
							m_info.getProducerName());
					AES_DDT_TRACE_MESSAGE("TransferProgress MO already exists for <%s> and <%s> ",m_info.getDataSourceName(),m_info.getProducerName());
					result = ACS_CC_SUCCESS;
				}

			}

			// Deallocate OM resource
			objManager.Finalize();
		}
		return result;
	}

} /* namespace operation */
