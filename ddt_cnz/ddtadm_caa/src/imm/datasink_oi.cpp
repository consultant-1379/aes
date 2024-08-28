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
#include "imm/datasink_oi.h"
#include "imm/imm.h"

#include "imm/configurationhelper.h"
#include "common/macros.h"
#include "common/utility.h"

#ifndef CUTE_TEST
	#include "operation/operation.h"
	#include "operation/creator.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include <ACS_APGCC_Util.H>
#else
	#include "stubs/operation_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_DataSink_OI)

namespace imm
{
	DataSink_OI::DataSink_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName) :
			Base_OI(managedObjectClassName, objectImplementerName)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	DataSink_OI::~DataSink_OI()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ACS_CC_ReturnType DataSink_OI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
	{
		AES_DDT_TRACE_MESSAGE("class:<%s>, parent:<%s>", className,  parentname);
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		dataSink newDataSink;
		newDataSink.action = Create;
		m_currentOperationId = operation::DATASINK_ADD;

		// extract the attributes
		for(size_t idx = 0U; (NULL != attr[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			// if this is the RDN attribute then get the DatSink Name, e.g: "dataSinkId=REMOTE"
			if( 0 == imm::datasink_attribute::RDN.compare(attr[idx]->attrName) )
			{
				std::string dataSourceDn(parentname);

				// get the datasource name from DN
				if( !common::utility::getRDNValue(dataSourceDn, imm::datasource_attribute::RDN, newDataSink.info.dataSourceName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				// make the value in upper case
				ACS_APGCC::toUpper(newDataSink.info.dataSourceName);

				AES_DDT_TRACE_MESSAGE("DataSource name:<%s>", newDataSink.getDataSourceName());

				// datasink RDN e.g: dataSinkId=REMOTE
				std::string dataSinkRDN = reinterpret_cast<char *>(attr[idx]->attrValues[0]);

				// get the datasink name from RDN
				if( !common::utility::getRDNValue(dataSinkRDN, imm::datasink_attribute::RDN, newDataSink.info.name) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("DataSink name:<%s>", newDataSink.getDataSinkName());

				//build something like: dataSinkId=REMOTE,dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferId=1
				newDataSink.info.moDN.assign(dataSinkRDN);
				newDataSink.info.moDN.push_back(imm::parserTag::comma);
				newDataSink.info.moDN += dataSourceDn;

				continue;
			}

			// check if retryAttempts attribute
			if( 0 == imm::datasink_attribute::RETRYATTEMPTS.compare(attr[idx]->attrName) )
			{
				newDataSink.info.retryAttempts = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE(" DataSink retryAttempts:<%d>", newDataSink.info.retryAttempts );
				continue;
			}

			// check if retryDelay attribute
			if( 0 == imm::datasink_attribute::RETRYDELAY.compare(attr[idx]->attrName) )
			{
				newDataSink.info.retryDelay = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE(" DataSink retryDelay:<%d>", newDataSink.info.retryDelay );
				continue;
			}
		}

		if( (ACS_CC_SUCCESS == result) && progressReportMOHandling(oiHandle, ccbId, newDataSink.info.moDN.c_str()) )
		{
			// DataSink parameters are valid
			operationTable_t::iterator element = m_dataSinkOperationTable.insert(std::make_pair(ccbId, newDataSink));

			configurationHelper_t::instance()->scheduleCreateOperation(newDataSink.info.dataSourceName, ccbId,
					ConfigurationHelper::DataSink, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on DataSink parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
			result = ACS_CC_FAILURE;
		}

		return result;
	}

	ACS_CC_ReturnType DataSink_OI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		dataSink modifyDataSink;
		modifyDataSink.action = Modify;
		modifyDataSink.info.moDN.assign(objName);

		m_currentOperationId = operation::DATASINK_MOD;

		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, modifyDataSink.info.dataSourceName)
		&& common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, modifyDataSink.info.name))
		{
			ACS_APGCC::toUpper(modifyDataSink.info.dataSourceName);
			result = ACS_CC_SUCCESS;
		}

		// extract the attributes
		for(size_t idx = 0U; (NULL != attrMods[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			if( (attrMods[idx]->modType == ACS_APGCC_ATTR_VALUES_REPLACE) && (attrMods[idx]->modAttr.attrValuesNum > 0) )
			{

				// check if retryAttempts attribute
				if( 0 == imm::datasink_attribute::RETRYATTEMPTS.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyDataSink.info.retryAttempts = *reinterpret_cast<unsigned int*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyDataSink.info.changeMask |= operation::changeMask::RETRYATTEMPTS_CHANGE;

					AES_DDT_TRACE_MESSAGE(" DataSink retryAttempts:<%d>", modifyDataSink.info.retryAttempts );
					continue;
				}

				// check if retryDelay attribute
				if( 0 == imm::datasink_attribute::RETRYDELAY.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyDataSink.info.retryDelay = *reinterpret_cast<unsigned int*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyDataSink.info.changeMask |= operation::changeMask::RETRYDELAY_CHANGE;

					AES_DDT_TRACE_MESSAGE(" DataSink retryDelay:<%d>", modifyDataSink.info.retryDelay );
					continue;
				}
			}
			else
			{
				result = ACS_CC_FAILURE;
			}
		}

		if(ACS_CC_SUCCESS == result)
		{
			// DataSink parameters are valid
			operationTable_t::iterator element = m_dataSinkOperationTable.insert(std::make_pair(ccbId, modifyDataSink));

			configurationHelper_t::instance()->scheduleModifyOperation(modifyDataSink.info.dataSourceName, ccbId,
					ConfigurationHelper::DataSink, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on DataSink parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}


	ACS_CC_ReturnType DataSink_OI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		dataSink deleteDataSink;
		deleteDataSink.action = Delete;
		deleteDataSink.info.moDN.assign(objName);

		m_currentOperationId = operation::DATASINK_DEL;

		// get the datasource name from DN
		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, deleteDataSink.info.dataSourceName)
			&& common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, deleteDataSink.info.name))
		{
			ACS_APGCC::toUpper(deleteDataSink.info.dataSourceName);
			AES_DDT_TRACE_MESSAGE("Delete Data Sink name:<%s> of DS:<%s>", deleteDataSink.getDataSinkName(), deleteDataSink.getDataSourceName());
			result =  ACS_CC_SUCCESS;

			operationTable_t::iterator element = m_dataSinkOperationTable.insert(std::pair<ACS_APGCC_CcbId, dataSink>(ccbId, deleteDataSink));

			configurationHelper_t::instance()->scheduleDeleteOperation(deleteDataSink.info.dataSourceName, ccbId,
					ConfigurationHelper::DataSink, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType DataSink_OI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_dataSinkOperationTable.equal_range(ccbId);

		//for each operation found
		operationTable_t::iterator element;
		for(element = operationRange.first; (element != operationRange.second); ++element)
		{
			if( (element->second.completed = configurationHelper_t::instance()->verifyConfiguration(element->second.info.dataSourceName, ccbId, m_currentOperationId)) )
			{
				AES_DDT_TRACE_MESSAGE("VALID REQUEST");
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("INVALID REQUEST");
				setExitCode(configurationHelper_t::instance()->getErrorCode(element->second.info.dataSourceName, ccbId),
						(configurationHelper_t::instance()->getErrorMessage(element->second.info.dataSourceName, ccbId)).c_str());

				AES_DDT_TRACE_MESSAGE("Data Sink operation failed");
				result = ACS_CC_FAILURE;
				break;
			}
		}

		AES_DDT_TRACE_MESSAGE(" result:<%d>", result);

		return result;
	}

	void  DataSink_OI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_dataSinkOperationTable.equal_range(ccbId);

		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			//Execute and Clean the configuration from the configuration helper
			configurationHelper_t::instance()->executeMoOperation(element->second.info.dataSourceName, ccbId );
		}

		// Erase all elements from the table of the operations
		m_dataSinkOperationTable.erase(operationRange.first, operationRange.second);

		AES_DDT_TRACE_MESSAGE(" IMM request on DataSink MO applied");

	}

	void DataSink_OI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_dataSinkOperationTable.equal_range(ccbId);

		//for each operation found
		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Clean the configuration from the configuration helper
			configurationHelper_t::instance()->removeConfiguration(element->second.info.dataSourceName, ccbId);
		}

		// Erase all elements from the table of the operations
		m_dataSinkOperationTable.erase(operationRange.first, operationRange.second);
	}

	void DataSink_OI::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList)
	{
		UNUSED(paramList);
		int operationResult = SA_AIS_ERR_BAD_OPERATION;

		if( imm::action::SWITCH_ACTIVE_PEER_OP_ID == operationId )
		{
			operationResult = SA_AIS_ERR_BUSY;
			operation::switchActivePeerInfo actionInfo(p_objName);

			AES_DDT_TRACE_MESSAGE("Switch Active Peer Action on DataSink:<%s>", actionInfo.getDataSinkDN() );

			// get the data source name from DN
			if( common::utility::getRDNValue(actionInfo.dataSinkDN, imm::datasource_attribute::RDN, actionInfo.dataSourceName) )
			{
				ACS_APGCC::toUpper(actionInfo.dataSourceName);
				AES_DDT_TRACE_MESSAGE("DataSource:<%s>", actionInfo.getDataSourceName());

				// Schedule operation
				operationResult = SA_AIS_OK;
				operation::Creator factoryCreator;
				factoryCreator.schedule(operation::SWITCHACTIVEPEER_ACTION, &actionInfo);
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to get DataSource Name from DN:<%s>", actionInfo.getDataSinkDN() );
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to get DataSource Name from DN:<%s>", actionInfo.getDataSinkDN() );
			}
		}

		if(ACS_CC_SUCCESS != adminOperationResult(oiHandle, invocation, operationResult))
		{
			AES_DDT_TRACE_MESSAGE("ERROR: adminOperationResult() Failed! DN:<%s> OperationId:<%llu>. ErrorCode:<%d> ErrorMsg:<%s>",
					p_objName, operationId, getInternalLastError(), getInternalLastErrorText());

			AES_DDT_LOG(LOG_LEVEL_ERROR, "adminOperationResult() Failed! DN:<%s> OperationId:<%llu>. ErrorCode:<%d> ErrorMsg:<%s>",
					p_objName, operationId, getInternalLastError(), getInternalLastErrorText());
		}

	}

	bool DataSink_OI::progressReportMOHandling(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char* parentDN)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = false;

		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

		ACS_CC_ReturnType immResult = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);

		if(ACS_CC_SUCCESS == immResult)
		{
			// create progress structure and update dataSink MO
			if( createProgressReportMO(ccbHandleVal, adminOwnwrHandlerVal, parentDN) &&
					updateDataSinkProgressAttribute(ccbHandleVal, adminOwnwrHandlerVal, parentDN) )
			{
				// apply transaction
				immResult =  applyAugmentCcb(ccbHandleVal);

				result = (ACS_CC_SUCCESS == immResult);

				if(!result)
				{
					AES_DDT_TRACE_MESSAGE("ERROR: applyAugmentCcb() Failed! DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
							parentDN, getInternalLastError(), getInternalLastErrorText());

					AES_DDT_LOG(LOG_LEVEL_ERROR, "applyAugmentCcb() Failed! DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
							parentDN, getInternalLastError(), getInternalLastErrorText());
				}
			}
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("ERROR: getCcbAugmentationInitialize() Failed!. ErrorCode:<%d> ErrorMsg:<%s>", getInternalLastError(), getInternalLastErrorText());
			AES_DDT_LOG(LOG_LEVEL_ERROR, "getCcbAugmentationInitialize() Failed!. ErrorCode:<%d> ErrorMsg:<%s>", getInternalLastError(), getInternalLastErrorText());
		}

		return result;
	}

	bool DataSink_OI::createProgressReportMO(ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* parentDN)
	{
		AES_DDT_TRACE_FUNCTION;

		std::vector<ACS_CC_ValuesDefinitionType> objAttrList;

		const unsigned int bufferSize = 256U;
		char tmpRDN[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(tmpRDN, bufferSize, "%s=%s", actionprogress_attribute::RDN.c_str(), actionprogress::SWITCHACTIVEPEER_RDN.c_str());

		// Fill the RDN attribute fields
		ACS_CC_ValuesDefinitionType attributeRDN;
		char tmpRDNAttr[64] = {0};
		actionprogress_attribute::RDN.copy(tmpRDNAttr, sizeof(tmpRDNAttr) - 1);

		attributeRDN.attrName = tmpRDNAttr;
		attributeRDN.attrType = ATTR_STRINGT;
		attributeRDN.attrValuesNum = 1;
		void* tmpValueRDN[1] = { reinterpret_cast<void*>(tmpRDN) };
		attributeRDN.attrValues = tmpValueRDN;

		objAttrList.push_back(attributeRDN);

		ACS_CC_ReturnType createResult = createObjectAugmentCcb(ccbHandle, adminOwnerHandler, imm::moc_name::ACTION_PROGRESS.c_str(), parentDN, objAttrList);

		bool result = (ACS_CC_SUCCESS == createResult);

		if(!result)
		{
			AES_DDT_TRACE_MESSAGE("ERROR: createObjectAugmentCcb() Failed! RDN:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
					tmpRDN, parentDN, getInternalLastError(), getInternalLastErrorText());

			AES_DDT_LOG(LOG_LEVEL_ERROR, "createObjectAugmentCcb() Failed! RDN:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
					tmpRDN, parentDN, getInternalLastError(), getInternalLastErrorText());
		}

		AES_DDT_TRACE_MESSAGE("createProgressReportMO result:<%s>", common::utility::boolToString(result));
		return result;
	}

	bool DataSink_OI::updateDataSinkProgressAttribute(ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* dataSinkDN)
	{
		AES_DDT_TRACE_FUNCTION;

		// Set the attribute progressReport of dataSink MOC
		ACS_CC_ImmParameter progressReportAttribute;

		const unsigned int bufferSize = 256U;
		char tmpProgressAttr[bufferSize] = {0};
		imm::datasink_attribute::PROGRESSREPORT.copy(tmpProgressAttr, sizeof(tmpProgressAttr) - 1);

		progressReportAttribute.attrName = tmpProgressAttr;
		progressReportAttribute.attrType = ATTR_NAMET;
		progressReportAttribute.attrValuesNum = 1;

		char tmpValueDN[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(tmpValueDN, bufferSize, "%s=%s,%s", actionprogress_attribute::RDN.c_str(), actionprogress::SWITCHACTIVEPEER_RDN.c_str(), dataSinkDN);

		void* tmpValue[1] = {reinterpret_cast<void*>(tmpValueDN)};
		progressReportAttribute.attrValues = tmpValue;

		ACS_CC_ReturnType modifyResult = modifyObjectAugmentCcb(ccbHandle, adminOwnerHandler, dataSinkDN, &progressReportAttribute);

		bool result = (ACS_CC_SUCCESS == modifyResult);

		if(!result)
		{
			AES_DDT_TRACE_MESSAGE("ERROR: modifyObjectAugmentCcb() Failed! Attribute:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
					tmpValueDN, dataSinkDN, getInternalLastError(), getInternalLastErrorText());

			AES_DDT_LOG(LOG_LEVEL_ERROR, "modifyObjectAugmentCcb() Failed! Attribute:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
					tmpValueDN, dataSinkDN, getInternalLastError(), getInternalLastErrorText());
		}

		AES_DDT_TRACE_MESSAGE("updateDataSinkProgressAttribute result:<%s>", common::utility::boolToString(result));
		return result;
	}

} /* namespace ddt_imm */
