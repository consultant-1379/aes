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
#include "imm/datasource_oi.h"

#include "imm/imm.h"
#include "common/macros.h"
#include "common/programconstants.h"
#include "imm/configurationhelper.h"
#include "common/utility.h"

#ifndef CUTE_TEST
	#include "operation/creator.h"
	#include "operation/operation.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include <ACS_APGCC_Util.H>
#else
	#include "stubs/operation_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_DataSource_OI)

namespace imm
{
	DataSource_OI::DataSource_OI() : Base_OI(imm::moc_name::DATA_SOURCE, imm::oi_name::DATA_SOURCE)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	DataSource_OI::~DataSource_OI()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ACS_CC_ReturnType DataSource_OI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char *parentname, ACS_APGCC_AttrValues **attr)
	{
		AES_DDT_TRACE_MESSAGE("class:<%s>, parent:<%s>", className,  parentname);
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		dataSource newDataSource;
		newDataSource.action = Create;
		m_currentOperationId = operation::DATASOURCE_ADD;

		// extract the attributes
		for(size_t idx = 0U; (NULL != attr[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			// if this is the RDN attribute then get the DataSource Name, e.g: "dataSourceId=VCHS"
			if( 0 == imm::datasource_attribute::RDN.compare(attr[idx]->attrName) )
			{
				std::string dataSourceRDN = reinterpret_cast<char *>(attr[idx]->attrValues[0]);
				newDataSource.info.moDN.assign(dataSourceRDN);
				newDataSource.info.moDN.push_back(imm::parserTag::comma);
				newDataSource.info.moDN += std::string(parentname);
				//build something like: dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferId=1

				// get the datasource name from RDN
				if( !getDataSourceNameFromDN(dataSourceRDN, newDataSource.info.name) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE(" DataSource name:<%s>, DN:<%s>", newDataSource.getDataSourceName(), newDataSource.getDataSourceDN());
				continue;
			}

			// check if recordSize attribute
			if( 0 == imm::datasource_attribute::RECORDSIZE.compare(attr[idx]->attrName) )
			{
				newDataSource.info.recordSize = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE(" DataSource recordSize:<%d>", newDataSource.info.recordSize );
				continue;
			}

			// check if alarmLevel attribute
			if( 0 == imm::datasource_attribute::ALARMLEVEL.compare(attr[idx]->attrName) )
			{
				newDataSource.info.alarmLevel = *reinterpret_cast<unsigned int*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE(" DataSource alarm level:<%d>", newDataSource.info.alarmLevel );
				continue;
			}
		}

		if(ACS_CC_SUCCESS == result)
		{
			operationTable_t::iterator element = m_dataSourceOperationTable.insert(std::pair<ACS_APGCC_CcbId, dataSource>(ccbId, newDataSource));

			configurationHelper_t::instance()->scheduleCreateOperation(newDataSource.info.name, ccbId,
								ConfigurationHelper::DataSource, m_currentOperationId, &(element->second.info));

			AES_DDT_TRACE_MESSAGE("Create data source name:<%s>",  newDataSource.getDataSourceName());
		}
		else
		{
			// Error on DataSource parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC);
		}

		return result;
	}

	ACS_CC_ReturnType DataSource_OI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		dataSource deleteDataSource;
		deleteDataSource.action = Delete;
		deleteDataSource.info.moDN.assign(objName);

		m_currentOperationId = operation::DATASOURCE_DEL;

		// get the datasource name from DN
		if( getDataSourceNameFromDN(objName, deleteDataSource.info.name) )
		{
			AES_DDT_TRACE_MESSAGE("Delete data source name:<%s>",  deleteDataSource.getDataSourceName());
			result =  ACS_CC_SUCCESS;
			operationTable_t::iterator element = m_dataSourceOperationTable.insert(std::pair<ACS_APGCC_CcbId, dataSource>(ccbId, deleteDataSource));

			configurationHelper_t::instance()->scheduleDeleteOperation(deleteDataSource.info.name, ccbId,
											ConfigurationHelper::DataSource, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType DataSource_OI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		dataSource modifyDataSource;
		modifyDataSource.action = Modify;
		modifyDataSource.info.moDN.assign(objName);

		m_currentOperationId = operation::DATASOURCE_MOD;

		// get the datasource name from DN
		if( getDataSourceNameFromDN(objName, modifyDataSource.info.name) )
		{
			// extract the attributes to modify
			for(size_t idx = 0U; (NULL != attrMods[idx]) && (ACS_CC_SUCCESS == result); idx++)
			{
				ACS_APGCC_AttrValues modAttribute = attrMods[idx]->modAttr;

				// check if recordSize attribute
				if( 0 == imm::datasource_attribute::RECORDSIZE.compare(modAttribute.attrName) )
				{
					modifyDataSource.info.recordSize = *reinterpret_cast<unsigned int *>(modAttribute.attrValues[0]);
					AES_DDT_TRACE_MESSAGE("Modify data source name:<%s> with the new recordSize:<%d>",  modifyDataSource.getDataSourceName(), modifyDataSource.info.recordSize);

					modifyDataSource.info.changeMask |= operation::changeMask::RECORDSIZE_CHANGE;
				}

				// check if recordSize attribute
				if( 0 == imm::datasource_attribute::ALARMLEVEL.compare(modAttribute.attrName) )
				{
					modifyDataSource.info.alarmLevel = *reinterpret_cast<unsigned int *>(modAttribute.attrValues[0]);
					AES_DDT_TRACE_MESSAGE("Modify data source name:<%s> with the new alarmLevel:<%d>",  modifyDataSource.getDataSourceName(), modifyDataSource.info.alarmLevel);

					modifyDataSource.info.changeMask |= operation::changeMask::ALARMLEVEL_CHANGE;
				}
			}
		}

		if(ACS_CC_SUCCESS == result)
		{
			m_dataSourceOperationTable.insert(std::pair<ACS_APGCC_CcbId, dataSource>(ccbId, modifyDataSource));
		}

		return result;
	}

	ACS_CC_ReturnType DataSource_OI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_dataSourceOperationTable.equal_range(ccbId);

		//for each operation found
		operationTable_t::iterator element;
		for(element = operationRange.first; (element != operationRange.second) && (ACS_CC_SUCCESS == result); ++element)
		{
			if( (element->second.completed = configurationHelper_t::instance()->verifyConfiguration(element->second.info.name, ccbId, m_currentOperationId)) )
			{
				AES_DDT_TRACE_MESSAGE("VALID REQUEST");
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("INVALID REQUEST");
				setExitCode(configurationHelper_t::instance()->getErrorCode(element->second.info.name, ccbId),
						(configurationHelper_t::instance()->getErrorMessage(element->second.info.name, ccbId)).c_str());

				AES_DDT_TRACE_MESSAGE("Data Source operation failed");
				result = ACS_CC_FAILURE;
			}
		}

		AES_DDT_TRACE_MESSAGE(" result:<%d>", result);

		return result;
	}

	void  DataSource_OI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		UNUSED(oiHandle);
		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_dataSourceOperationTable.equal_range(ccbId);

		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			//Execute and Clean the configuration from the configuration helper
			configurationHelper_t::instance()->executeMoOperation(element->second.info.name, ccbId );
		}

		// Erase all elements from the table of the operations
		m_dataSourceOperationTable.erase(operationRange.first, operationRange.second);
		AES_DDT_TRACE_MESSAGE(" IMM request on DataSource MO applied");
	}

	void DataSource_OI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_dataSourceOperationTable.equal_range(ccbId);

		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Clean the configuration from the configuration helper
			configurationHelper_t::instance()->removeConfiguration(element->second.info.name, ccbId);
		}

		// Erase all elements from the table of the operations
		m_dataSourceOperationTable.erase(operationRange.first, operationRange.second);
	}

	bool DataSource_OI::getDataSourceNameFromDN(const std::string& objectDN, std::string& dataSourceName)
	{
		// get the datasource name from DN
		bool result = common::utility::getLastFieldValue(objectDN, dataSourceName);

		if(result)
		{
			// make the value in upper case
			ACS_APGCC::toUpper(dataSourceName);
			AES_DDT_TRACE_MESSAGE(" dataSourceName:<%s>", dataSourceName.c_str());
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, " Failed to get data source name from DN:<%s>",  objectDN.c_str() );
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		AES_DDT_TRACE_MESSAGE(" result:<%s>", common::utility::boolToString(result) );
		return result;
	}

} /* namespace ddt_imm */
