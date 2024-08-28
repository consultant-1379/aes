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
#include "imm/fileformat_oi.h"
#include "imm/imm.h"

#ifndef CUTE_TEST
	#include "imm/configurationhelper.h"
	#include "common/macros.h"
	#include "common/utility.h"
	#include "common/tracer.h"
	#include "common/logger.h"

	#include <ACS_APGCC_Util.H>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_FileFormat_OI)

namespace imm
{

	FileFormat_OI::FileFormat_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName) :
		Base_OI(managedObjectClassName, objectImplementerName)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	FileFormat_OI::~FileFormat_OI()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ACS_CC_ReturnType FileFormat_OI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
	{
		AES_DDT_TRACE_MESSAGE("class:<%s>, parent:<%s>", className,  parentname);
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		fileFormat newFileFormat;
		newFileFormat.action = Create;
		m_currentOperationId = operation::FILEFORMAT_ADD;


		// extract the attributes
		for(size_t idx = 0U; (NULL != attr[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			// RDN
			if( 0 == imm::fileformat_attribute::RDN.compare(attr[idx]->attrName) )
			{
				std::string outputFormatDn(parentname);

				// get the datasource name from DN
				if( !common::utility::getRDNValue(outputFormatDn, imm::datasource_attribute::RDN, newFileFormat.info.dataSourceName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				ACS_APGCC::toUpper(newFileFormat.info.dataSourceName);
				AES_DDT_TRACE_MESSAGE("DataSource name:<%s>", newFileFormat.getDataSourceName());

				// get the datasink name from DN
				if( !common::utility::getRDNValue(outputFormatDn, imm::datasink_attribute::RDN, newFileFormat.info.dataSinkName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("DataSink name:<%s>", newFileFormat.getDataSinkName());


				// get the OutputFormat name from RDN
				if( !common::utility::getRDNValue(outputFormatDn, imm::outputformat_attribute::RDN, newFileFormat.info.outputFormatName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("OutputFormat name:<%s>", newFileFormat.getOutputFormatName());


				//FileFormat RDN e.g: "fileFormatId=FF1"
				std::string fileFormatRDN = reinterpret_cast<char*>(attr[idx]->attrValues[0]);

				// get the FileFormat name from RDN
				if( !common::utility::getRDNValue(fileFormatRDN, imm::fileformat_attribute::RDN, newFileFormat.info.name) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("FileFormat name:<%s>", newFileFormat.getFileFormatName());

				//build something like: fileFormatId=FF1,outputFormatId=FORMAT1,dataSinkId=REMOTE,dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferId=1
				newFileFormat.info.moDN.assign(fileFormatRDN);
				newFileFormat.info.moDN.push_back(imm::parserTag::comma);
				newFileFormat.info.moDN += outputFormatDn;

				continue;
			}

			// FILESIZE
			if( 0 == imm::fileformat_attribute::FILESIZE.compare(attr[idx]->attrName) )
			{
				newFileFormat.info.fileSize = *reinterpret_cast<uint32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("FILESIZE: <%u>", newFileFormat.info.fileSize);
				continue;
			}

			// STARTUP_SEQ_NUM
			if( 0 == imm::fileformat_attribute::STARTUP_SEQ_NUM.compare(attr[idx]->attrName) )
			{
				newFileFormat.info.startupSequenceNumber = *reinterpret_cast<uint32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("STARTUP_SEQ_NUM: <%u>", newFileFormat.info.startupSequenceNumber);
				continue;
			}

			// TEMPLATE
			if( 0 == imm::fileformat_attribute::TEMPLATE.compare(attr[idx]->attrName) )
			{
				newFileFormat.info.templateValue.assign(reinterpret_cast<char*>(attr[idx]->attrValues[0]));
				AES_DDT_TRACE_MESSAGE("TEMPLATE: <%s>", newFileFormat.info.templateValue.c_str());
				continue;
			}
		}

		if( ACS_CC_SUCCESS == result )
		{
			// FileFormat parameters are valid
			operationTable_t::iterator element = m_operationTable.insert(std::make_pair(ccbId, newFileFormat));

			configurationHelper_t::instance()->scheduleCreateOperation(newFileFormat.info.dataSourceName, ccbId,
								ConfigurationHelper::FileFormat, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on FileFormat parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType FileFormat_OI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		fileFormat deleteFileFormat;
		deleteFileFormat.action = Delete;
		deleteFileFormat.info.moDN.assign(objName);

		m_currentOperationId = operation::FILEFORMAT_DEL;

		// get the datasource name from DN
		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, deleteFileFormat.info.dataSourceName) &&
			common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, deleteFileFormat.info.dataSinkName) &&
			common::utility::getRDNValue(objName, imm::outputformat_attribute::RDN, deleteFileFormat.info.outputFormatName) &&
			common::utility::getRDNValue(objName, imm::fileformat_attribute::RDN, deleteFileFormat.info.name) )
		{
			ACS_APGCC::toUpper(deleteFileFormat.info.dataSourceName);
			result =  ACS_CC_SUCCESS;
			AES_DDT_TRACE_MESSAGE("Delete FileFormat:<%s> in OutputFormat:<%s> [Data Source:<%s>/Sink:<%s>]",
					deleteFileFormat.getFileFormatName(), deleteFileFormat.getOutputFormatName(), deleteFileFormat.getDataSourceName(), deleteFileFormat.getDataSinkName());

			operationTable_t::iterator element = m_operationTable.insert(std::pair<ACS_APGCC_CcbId, fileFormat>(ccbId, deleteFileFormat));

			configurationHelper_t::instance()->scheduleDeleteOperation(deleteFileFormat.info.dataSourceName, ccbId,
					ConfigurationHelper::FileFormat, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType FileFormat_OI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		fileFormat modifyFileFormat;
		modifyFileFormat.action = Modify;
		modifyFileFormat.info.moDN.assign(objName);

		m_currentOperationId = operation::FILEFORMAT_MOD;

		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, modifyFileFormat.info.dataSourceName) &&
				common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, modifyFileFormat.info.dataSinkName) &&
				common::utility::getRDNValue(objName, imm::outputformat_attribute::RDN, modifyFileFormat.info.outputFormatName) &&
				common::utility::getRDNValue(objName, imm::fileformat_attribute::RDN, modifyFileFormat.info.name) )
		{
			ACS_APGCC::toUpper(modifyFileFormat.info.dataSourceName);
			result = ACS_CC_SUCCESS;
		}

		// extract the attributes
		for(size_t idx = 0U; (NULL != attrMods[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			if( (attrMods[idx]->modType == ACS_APGCC_ATTR_VALUES_REPLACE) && (attrMods[idx]->modAttr.attrValuesNum > 0) )
			{
				// FILESIZE
				if( 0 == imm::fileformat_attribute::FILESIZE.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyFileFormat.info.fileSize = *reinterpret_cast<uint32_t*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyFileFormat.info.changeMask |= operation::changeMask::FILESIZE_CHANGE;

					AES_DDT_TRACE_MESSAGE("FILESIZE: <%u>", modifyFileFormat.info.fileSize);
					continue;
				}

				// STARTUP_SEQ_NUM
				if( 0 == imm::fileformat_attribute::STARTUP_SEQ_NUM.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyFileFormat.info.startupSequenceNumber = *reinterpret_cast<uint32_t*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyFileFormat.info.changeMask |= operation::changeMask::FILE_SEQ_NUMBER_CHANGE;

					AES_DDT_TRACE_MESSAGE("STARTUP_SEQ_NUM: <%u>", modifyFileFormat.info.startupSequenceNumber);
					continue;
				}

				// TEMPLATE
				if( 0 == imm::fileformat_attribute::TEMPLATE.compare(attrMods[idx]->modAttr.attrName) )
				{
					modifyFileFormat.info.templateValue.assign(reinterpret_cast<char*>(attrMods[idx]->modAttr.attrValues[0]));
					modifyFileFormat.info.changeMask |= operation::changeMask::FILE_TEMPLATE_CHANGE;

					AES_DDT_TRACE_MESSAGE("TEMPLATE: <%s>", modifyFileFormat.info.templateValue.c_str());
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
			// FileFormat parameters are valid
			operationTable_t::iterator element = m_operationTable.insert(std::make_pair(ccbId, modifyFileFormat));

			configurationHelper_t::instance()->scheduleModifyOperation(modifyFileFormat.info.dataSourceName, ccbId,
					ConfigurationHelper::FileFormat, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on FileFormat parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType FileFormat_OI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_operationTable.equal_range(ccbId);

		//for each operation found
		operationTable_t::iterator element;
		for(element = operationRange.first; (element != operationRange.second) && (ACS_CC_SUCCESS == result); ++element)
		{
			if( (element->second.completed = configurationHelper_t::instance()->verifyConfiguration(element->second.info.dataSourceName, ccbId,m_currentOperationId)) )
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

	void FileFormat_OI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_operationTable.equal_range(ccbId);

		for(operationTable_t::iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Execute and Clean the configuration from the configuration helper
			configurationHelper_t::instance()->executeMoOperation(element->second.info.dataSourceName, ccbId );
		}

		// Erase all elements from the table of the operations
		m_operationTable.erase(operationRange.first, operationRange.second);

		AES_DDT_TRACE_MESSAGE(" IMM request on FileFormat MO applied");
	}

	void FileFormat_OI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_operationTable.equal_range(ccbId);

		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Clean the configuration from the configuration helper
			configurationHelper_t::instance()->removeConfiguration(element->second.info.dataSourceName, ccbId);
		}

		// Erase all elements from the table of the operations
		m_operationTable.erase(operationRange.first, operationRange.second);
	}

} /* namespace ddt_imm */
