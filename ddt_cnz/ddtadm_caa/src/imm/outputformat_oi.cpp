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
#include "imm/outputformat_oi.h"

#include "imm/imm.h"

#ifndef CUTE_TEST
	#include "imm/configurationhelper.h"
	#include "common/macros.h"
	#include "common/utility.h"
	#include "common/tracer.h"
	#include "common/logger.h"

	#include <ACS_APGCC_Util.H>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_OutputFormat_OI)

namespace imm
{

	OutputFormat_OI::OutputFormat_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName) :
		Base_OI(managedObjectClassName, objectImplementerName)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	OutputFormat_OI::~OutputFormat_OI()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ACS_CC_ReturnType OutputFormat_OI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
	{
		AES_DDT_TRACE_MESSAGE("class:<%s>, parent:<%s>", className,  parentname);
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		outputFormat newOutputFormat;
		newOutputFormat.action = Create;
		m_currentOperationId = operation::OUTPUTFORMAT_ADD;

		// extract the attributes
		for(size_t idx = 0U; (NULL != attr[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			// RDN
			if( 0 == imm::outputformat_attribute::RDN.compare(attr[idx]->attrName) )
			{
				std::string dataSinkDn(parentname);

				// get the datasource name from DN
				if( !common::utility::getRDNValue(dataSinkDn, imm::datasource_attribute::RDN, newOutputFormat.info.dataSourceName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				ACS_APGCC::toUpper(newOutputFormat.info.dataSourceName);
				AES_DDT_TRACE_MESSAGE("DataSource name:<%s>", newOutputFormat.getDataSourceName());

				// get the datasink name from DN
				if( !common::utility::getRDNValue(dataSinkDn, imm::datasink_attribute::RDN, newOutputFormat.info.dataSinkName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("DataSink name:<%s>", newOutputFormat.getDataSinkName());

				//outputFormat RDN e.g: "outputFormatId=FORMAT1"
				std::string outputFormatRDN = reinterpret_cast<char*>(attr[idx]->attrValues[0]);

				// get the OutputFormat name from RDN
				if( !common::utility::getRDNValue(outputFormatRDN, imm::outputformat_attribute::RDN, newOutputFormat.info.name) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("OutputFormat name:<%s>", newOutputFormat.getOutputFormatName());

				//build something like: outputFormatId=FORMAT1,dataSinkId=REMOTE,dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferId=1
				newOutputFormat.info.moDN.assign(outputFormatRDN);
				newOutputFormat.info.moDN.push_back(imm::parserTag::comma);
				newOutputFormat.info.moDN += dataSinkDn;

				continue;
			}

			// HOLDTIME
			if( 0 == imm::outputformat_attribute::HOLDTIME.compare(attr[idx]->attrName) )
			{
				newOutputFormat.info.holdTime = *reinterpret_cast<uint32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("HOLDTIME: <%u>", newOutputFormat.info.holdTime);
				continue;
			}

			// DATACHUNKSIZE
			if( 0 == imm::outputformat_attribute::DATACHUNKSIZE.compare(attr[idx]->attrName) )
			{
				newOutputFormat.info.dataChunkSize = *reinterpret_cast<uint32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("DATACHUNKSIZE: <%u>", newOutputFormat.info.dataChunkSize);
				continue;
			}

			// DATALENGTHTYPE
			if( 0 == imm::outputformat_attribute::DATALENGTHTYPE.compare(attr[idx]->attrName) )
			{
				newOutputFormat.info.dataLengthType = *reinterpret_cast<int32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("DATALENGTHTYPE: <%d>", newOutputFormat.info.dataLengthType);
				continue;
			}

			// PADDINGCHAR
			if( 0 == imm::outputformat_attribute::PADDINGCHAR.compare(attr[idx]->attrName) )
			{
				newOutputFormat.info.paddingChar = *reinterpret_cast<uint32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("PADDINGCHAR: <%u>", newOutputFormat.info.paddingChar);
				continue;
			}
		}

		if( ACS_CC_SUCCESS == result )
		{
			// OutputFormat parameters are valid
			operationTable_t::iterator element = m_outputFormatOperationTable.insert(std::make_pair(ccbId, newOutputFormat));

			configurationHelper_t::instance()->scheduleCreateOperation(newOutputFormat.info.dataSourceName, ccbId,
					ConfigurationHelper::OutputFormat, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on OutputFormat parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType OutputFormat_OI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		outputFormat modifyOutputFormat;
		modifyOutputFormat.action = Modify;
		modifyOutputFormat.info.moDN.assign(objName);

		m_currentOperationId = operation::OUTPUTFORMAT_MOD;

		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, modifyOutputFormat.info.dataSourceName) &&
				common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, modifyOutputFormat.info.dataSinkName) &&
				common::utility::getRDNValue(objName, imm::outputformat_attribute::RDN, modifyOutputFormat.info.name))
		{
			ACS_APGCC::toUpper(modifyOutputFormat.info.dataSourceName);
			result = ACS_CC_SUCCESS;
		}

		// extract the attributes
		for(size_t idx = 0U; (NULL != attrMods[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			if( (attrMods[idx]->modType == ACS_APGCC_ATTR_VALUES_REPLACE) && (attrMods[idx]->modAttr.attrValuesNum > 0) )
			{

				// HOLDTIME
				if( 0 == imm::outputformat_attribute::HOLDTIME.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyOutputFormat.info.holdTime = *reinterpret_cast<uint32_t*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyOutputFormat.info.changeMask |= operation::changeMask::HOLDTIME_CHANGE;

					AES_DDT_TRACE_MESSAGE("HOLDTIME: <%u>", modifyOutputFormat.info.holdTime);
					continue;
				}

				// DATACHUNKSIZE
				if( 0 == imm::outputformat_attribute::DATACHUNKSIZE.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyOutputFormat.info.dataChunkSize = *reinterpret_cast<uint32_t*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyOutputFormat.info.changeMask |= operation::changeMask::DATACHUNKSIZE_CHANGE;

					AES_DDT_TRACE_MESSAGE("DATACHUNKSIZE: <%u>", modifyOutputFormat.info.dataChunkSize);
					continue;
				}

				// DATALENGTHTYPE
				if( 0 == imm::outputformat_attribute::DATALENGTHTYPE.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyOutputFormat.info.dataLengthType = *reinterpret_cast<int32_t*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyOutputFormat.info.changeMask |= operation::changeMask::DATALENGHTTYPE_CHANGE;

					AES_DDT_TRACE_MESSAGE("DATALENGTHTYPE: <%d>", modifyOutputFormat.info.dataLengthType);
					continue;
				}

				// PADDINGCHAR
				if( 0 == imm::outputformat_attribute::PADDINGCHAR.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyOutputFormat.info.paddingChar = *reinterpret_cast<uint32_t*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyOutputFormat.info.changeMask |= operation::changeMask::PADDINGCHAR_CHANGE;

					AES_DDT_TRACE_MESSAGE("PADDINGCHAR: <%u>", modifyOutputFormat.info.paddingChar);
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
			operationTable_t::iterator element = m_outputFormatOperationTable.insert(std::make_pair(ccbId, modifyOutputFormat));

			configurationHelper_t::instance()->scheduleModifyOperation(modifyOutputFormat.info.dataSourceName, ccbId,
					ConfigurationHelper::OutputFormat, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on DataSink parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}


	ACS_CC_ReturnType OutputFormat_OI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		outputFormat deleteOutputFormat;
		deleteOutputFormat.action = Delete;
		deleteOutputFormat.info.moDN.assign(objName);

		m_currentOperationId = operation::OUTPUTFORMAT_DEL;

		// get the datasource name from DN
		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, deleteOutputFormat.info.dataSourceName) &&
			common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, deleteOutputFormat.info.dataSinkName) &&
			common::utility::getRDNValue(objName, imm::outputformat_attribute::RDN, deleteOutputFormat.info.name))
		{
			ACS_APGCC::toUpper(deleteOutputFormat.info.dataSourceName);
			result =  ACS_CC_SUCCESS;
			AES_DDT_TRACE_MESSAGE("Delete OutputFormat:<%s> in DataSink:<%s> of DS:<%s>",
					deleteOutputFormat.getOutputFormatName(), deleteOutputFormat.getDataSinkName(), deleteOutputFormat.getDataSourceName());

			operationTable_t::iterator element = m_outputFormatOperationTable.insert(std::pair<ACS_APGCC_CcbId, outputFormat>(ccbId, deleteOutputFormat));

			configurationHelper_t::instance()->scheduleDeleteOperation(deleteOutputFormat.info.dataSourceName, ccbId,
					ConfigurationHelper::OutputFormat, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType OutputFormat_OI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_outputFormatOperationTable.equal_range(ccbId);

		//for each operation found
		operationTable_t::iterator element;
		for(element = operationRange.first; (element != operationRange.second) && (ACS_CC_SUCCESS == result); ++element)
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

				AES_DDT_TRACE_MESSAGE("OutputFormat operation failed");
				result = ACS_CC_FAILURE;
			}
		}

		AES_DDT_TRACE_MESSAGE(" result:<%d>", result);

		return result;
	}

	void OutputFormat_OI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_outputFormatOperationTable.equal_range(ccbId);

		for(operationTable_t::iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Execute and Clean the configuration from the configuration helper
			configurationHelper_t::instance()->executeMoOperation(element->second.info.dataSourceName, ccbId );
		}

		// Erase all elements from the table of the operations
		m_outputFormatOperationTable.erase(operationRange.first, operationRange.second);

		AES_DDT_TRACE_MESSAGE(" IMM request on OutputFormat MO applied");
	}

	void OutputFormat_OI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(oiHandle);

		// TODO HANDLING FILEPEER ABORT
		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_outputFormatOperationTable.equal_range(ccbId);

		//for each operation found
		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Clean the configuration from the configuration helper
			configurationHelper_t::instance()->removeConfiguration(element->second.info.dataSourceName, ccbId);
		}

		// Erase all elements from the table of the operations
		m_outputFormatOperationTable.erase(operationRange.first, operationRange.second);
	}

	bool OutputFormat_OI::createFileFormat(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char* parentDN)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = false;

		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

		ACS_CC_ReturnType immResult = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal);

		if(ACS_CC_SUCCESS == immResult)
		{
			char rdnTmpName[64]= {0};
			imm::fileformat_attribute::RDN.copy(rdnTmpName, imm::fileformat_attribute::RDN.length());

			char tmpRDN[256] = {0};
			snprintf(tmpRDN, sizeof(tmpRDN)-1, "%s=1", imm::fileformat_attribute::RDN.c_str());
			void* tmpValueRDN[1] = { reinterpret_cast<void*>(tmpRDN) };

			AES_DDT_TRACE_MESSAGE("FileFormat RDN:<%s>", tmpRDN);

			ACS_CC_ValuesDefinitionType attributeRDN;
			attributeRDN.attrName = rdnTmpName;
			attributeRDN.attrType = ATTR_NAMET;
			attributeRDN.attrValuesNum = 1U;
			attributeRDN.attrValues = tmpValueRDN;

			// The vector of attributes
			std::vector<ACS_CC_ValuesDefinitionType> objAttrList;

			objAttrList.push_back(attributeRDN);

			immResult = createObjectAugmentCcb(ccbHandleVal, adminOwnwrHandlerVal, imm::moc_name::FILE_FORMAT.c_str(), parentDN, objAttrList);

			if(ACS_CC_SUCCESS == immResult)
			{
				immResult = applyAugmentCcb(ccbHandleVal);
				result = (ACS_CC_SUCCESS == immResult);
				if(!result)
				{
					AES_DDT_TRACE_MESSAGE("ERROR: applyAugmentCcb() Failed! RDN:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
										  tmpRDN, parentDN, getInternalLastError(), getInternalLastErrorText());

					AES_DDT_LOG(LOG_LEVEL_ERROR, "applyAugmentCcb() Failed! RDN:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
								tmpRDN, parentDN, getInternalLastError(), getInternalLastErrorText());
				}
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("ERROR: createObjectAugmentCcb() Failed! RDN:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
							tmpRDN, parentDN, getInternalLastError(), getInternalLastErrorText());

				AES_DDT_LOG(LOG_LEVEL_ERROR, "createObjectAugmentCcb() Failed! RDN:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
							tmpRDN, parentDN, getInternalLastError(), getInternalLastErrorText());
			}
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("ERROR: getCcbAugmentationInitialize() Failed!. ErrorCode:<%d> ErrorMsg:<%s>", getInternalLastError(), getInternalLastErrorText());
			AES_DDT_LOG(LOG_LEVEL_ERROR, "getCcbAugmentationInitialize() Failed!. ErrorCode:<%d> ErrorMsg:<%s>", getInternalLastError(), getInternalLastErrorText());
		}
		return result;
	}

} /* namespace imm */
