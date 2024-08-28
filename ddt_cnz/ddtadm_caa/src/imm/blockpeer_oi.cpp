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
#include "imm/blockpeer_oi.h"

#include "imm/imm.h"

#ifndef CUTE_TEST
	#include "imm/configurationhelper.h"
	#include "common/macros.h"
	#include "common/utility.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "engine/workingset.h"
	#include "engine/context.h"

	#include <ACS_APGCC_Util.H>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_BlockPeer_OI)

namespace imm
{

	BlockPeer_OI::BlockPeer_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName) :
			Base_OI(managedObjectClassName, objectImplementerName)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	BlockPeer_OI::~BlockPeer_OI()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ACS_CC_ReturnType BlockPeer_OI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
	{
		AES_DDT_TRACE_MESSAGE("class:<%s>, parent:<%s>", className,  parentname);
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		blockPeer newBlockPeer;
		newBlockPeer.action = Create;
		m_currentOperationId = operation::BLOCKPEER_ADD;

		// extract the attributes
		for(size_t idx = 0U; (NULL != attr[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			// RDN
			if( 0 == imm::blockpeer_attribute::RDN.compare(attr[idx]->attrName) )
			{
				std::string dataSinkDn(parentname);

				// get the datasource name from DN
				if( !common::utility::getRDNValue(dataSinkDn, imm::datasource_attribute::RDN, newBlockPeer.info.dataSourceName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				ACS_APGCC::toUpper(newBlockPeer.info.dataSourceName);
				AES_DDT_TRACE_MESSAGE("DataSource name:<%s>", newBlockPeer.getDataSourceName());

				// get the datasink name from DN
				if( !common::utility::getRDNValue(dataSinkDn, imm::datasink_attribute::RDN, newBlockPeer.info.dataSinkName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("DataSink name:<%s>", newBlockPeer.getDataSinkName());

				// blockPeer RDN e.g: "blockPeerId=SERVER1"
				std::string blockPeerRDN = reinterpret_cast<char*>(attr[idx]->attrValues[0]);

				// get the BlockPeer name from RDN
				if( !common::utility::getRDNValue(blockPeerRDN, imm::blockpeer_attribute::RDN, newBlockPeer.info.name) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("BlockPeer name:<%s>", newBlockPeer.getBlockPeerName());

				//build something like: blockPeerId=SERVER1,dataSinkId=REMOTE,dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferId=1
				newBlockPeer.info.moDN.assign(blockPeerRDN);
				newBlockPeer.info.moDN.push_back(imm::parserTag::comma);
				newBlockPeer.info.moDN += dataSinkDn;

				continue;
			}

			// IPADDRESS
			if( 0 == imm::blockpeer_attribute::IPADDRESS.compare(attr[idx]->attrName) )
			{
				std::string strIP = reinterpret_cast<char*>(attr[idx]->attrValues[0]);
				if(common::utility::isIPAddressValid(strIP.c_str()) != 0)
				{
					result = ACS_CC_FAILURE;
					break;
				}
				newBlockPeer.info.ipAddress.assign(reinterpret_cast<char*>(attr[idx]->attrValues[0]));
				AES_DDT_TRACE_MESSAGE("IP Address: <%s>", newBlockPeer.info.ipAddress.c_str());
				continue;
			}
		}

		if( ACS_CC_SUCCESS == result )
		{
			// BlockPeer parameters are valid
			operationTable_t::iterator element = m_blockPeerOperationTable.insert(std::make_pair(ccbId, newBlockPeer));

			configurationHelper_t::instance()->scheduleCreateOperation(newBlockPeer.info.dataSourceName, ccbId,
								ConfigurationHelper::BlockPeer, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on BlockPeer parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}


	ACS_CC_ReturnType BlockPeer_OI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		blockPeer deleteBlockPeer;
		deleteBlockPeer.action = Delete;
		deleteBlockPeer.info.moDN.assign(objName);

		m_currentOperationId = operation::BLOCKPEER_DEL;

		// get the datasource name from DN
		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, deleteBlockPeer.info.dataSourceName)
			&& common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, deleteBlockPeer.info.dataSinkName)
			&& common::utility::getRDNValue(objName, imm::blockpeer_attribute::RDN, deleteBlockPeer.info.name))
		{
			ACS_APGCC::toUpper(deleteBlockPeer.info.dataSourceName);
			result =  ACS_CC_SUCCESS;
			AES_DDT_TRACE_MESSAGE("Delete BlockPeer:<%s> in DataSink:<%s> of DS:<%s>",
					deleteBlockPeer.getBlockPeerName(), deleteBlockPeer.getDataSinkName(), deleteBlockPeer.getDataSourceName());

			operationTable_t::iterator element = m_blockPeerOperationTable.insert(std::pair<ACS_APGCC_CcbId, blockPeer>(ccbId, deleteBlockPeer));

			configurationHelper_t::instance()->scheduleDeleteOperation(deleteBlockPeer.info.dataSourceName, ccbId,
					ConfigurationHelper::BlockPeer, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType BlockPeer_OI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		AES_DDT_TRACE_MESSAGE("**** MODIFY OBJ: <%s> TODO - INCOMPLETE ****", objName);

		UNUSED(ccbId);
		UNUSED(oiHandle);

		ACS_CC_ReturnType result = ACS_CC_FAILURE;

		blockPeer blockPeerInfo;
		blockPeerInfo.action = Modify;
		blockPeerInfo.info.moDN.assign(objName);

		m_currentOperationId = operation::BLOCKPEER_MOD;

		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, blockPeerInfo.info.dataSourceName)
				&& common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, blockPeerInfo.info.dataSinkName)
				&& common::utility::getRDNValue(objName, imm::blockpeer_attribute::RDN, blockPeerInfo.info.name))
		{
			ACS_APGCC::toUpper(blockPeerInfo.info.dataSourceName);
			// extract the attributes to modify
			for(size_t idx = 0U; NULL != attrMods[idx]; idx++)
			{
				ACS_APGCC_AttrValues modAttribute = attrMods[idx]->modAttr;

				// check if recordSize attribute
				if( 0 == imm::blockpeer_attribute::STATUS.compare(modAttribute.attrName) )
				{
					AES_DDT_TRACE_MESSAGE("Modifying block peer status");
					result = ACS_CC_SUCCESS;
				}

				// check if recordSize attribute
				if( 0 == imm::blockpeer_attribute::IPADDRESS.compare(modAttribute.attrName) )
				{
					blockPeerInfo.info.ipAddress.assign( reinterpret_cast<char *>(modAttribute.attrValues[0]) );

					//result = ACS_CC_SUCCESS; - NOT IMPLEMENTED YET
					setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::NOT_IMPLEMENTED_YET);

					AES_DDT_TRACE_MESSAGE("Modify Block Peer address:<%s>",  blockPeerInfo.info.ipAddress.c_str());
					blockPeerInfo.info.changeMask |= operation::changeMask::IPADDRESS_CHANGE;
				}
			}
		}
		else
		{
			// Error
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		AES_DDT_TRACE_MESSAGE("MODIFY result <%d>", result);
		return result;
	}

	ACS_CC_ReturnType BlockPeer_OI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_blockPeerOperationTable.equal_range(ccbId);

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

				AES_DDT_TRACE_MESSAGE("BlockPeer operation failed");
				result = ACS_CC_FAILURE;
			}
		}

		AES_DDT_TRACE_MESSAGE(" result:<%d>", result);

		return result;
	}

	void  BlockPeer_OI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_blockPeerOperationTable.equal_range(ccbId);

		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Execute and Clean the configuration from the configuration helper
			configurationHelper_t::instance()->executeMoOperation(element->second.info.dataSourceName, ccbId );
		}

		// Erase all elements from the table of the operations
		m_blockPeerOperationTable.erase(operationRange.first, operationRange.second);

		AES_DDT_TRACE_MESSAGE(" IMM request on BlockPeer MO applied");
	}

	void BlockPeer_OI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_blockPeerOperationTable.equal_range(ccbId);

		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Clean the configuration from the configuration helper
			configurationHelper_t::instance()->removeConfiguration(element->second.info.dataSourceName, ccbId);
		}

		// Erase all elements from the table of the operations
		m_blockPeerOperationTable.erase(operationRange.first, operationRange.second);
	}

	void BlockPeer_OI::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(paramList);
		int operationResult = SA_AIS_ERR_BAD_OPERATION;

		bool connectionStatus = false;

		if( imm::action::VERIFY_BLOCK_PEER_CONNECTION_OP_ID == operationId )
		{
			operationResult = SA_AIS_ERR_FAILED_OPERATION;

			std::string dataSourceName;
			std::string peerName;
			common::utility::getRDNValue(p_objName, imm::datasource_attribute::RDN, dataSourceName);
			common::utility::getRDNValue(p_objName, imm::blockpeer_attribute::RDN, peerName);

			ACS_APGCC::toUpper(dataSourceName);

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Checking connection with peer: <%s>", peerName.c_str());
			AES_DDT_TRACE_MESSAGE("Checking connection with peer: <%s>", peerName.c_str());

			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);
			if( context )
			{
				boost::shared_ptr<engine::Peer> blockPeer = context->getPeerByName(peerName);

				if( blockPeer )
				{
					operationResult = SA_AIS_OK;
					connectionStatus = blockPeer->isReadyForConnection();
				}
			}
		}

		std::vector<ACS_APGCC_AdminOperationParamType> resultOut;

		if( SA_AIS_OK == operationResult )
		{
			ACS_APGCC_AdminOperationParamType operationReturnValue;

			const unsigned int bufferSize= 256U;
			char nameValue[bufferSize] = {0};
			imm::action::RESULT_ATTR_ID.copy(nameValue, bufferSize - 1);

			int status = (int) connectionStatus;
			operationReturnValue.attrName = nameValue;
			operationReturnValue.attrType = ATTR_INT32T;
			operationReturnValue.attrValues = reinterpret_cast<void*> (&status);

			resultOut.push_back(operationReturnValue);
		}
		else
		{
			std::string operationMessage;
			std::string errText = imm::action::ERROR_TEXT_PREFIX + imm::comcli_errormessage::VERIFY_CONNECTION_FAILED;

			ACS_APGCC_AdminOperationParamType errorTextElem;

			const unsigned int bufferSize= 256U;

			char nameValue[bufferSize] = {0};
			imm::action::ERROR_TEXT_ATTR_ID.copy(nameValue, bufferSize - 1);

			char errValue[bufferSize] = {0};
			errText.copy(errValue, bufferSize - 1);

			errorTextElem.attrName = nameValue;
			errorTextElem.attrType = ATTR_STRINGT;
			errorTextElem.attrValues = reinterpret_cast<void*> (errValue);

			resultOut.push_back(errorTextElem);

		}

		if(ACS_CC_SUCCESS != adminOperationResult(oiHandle, invocation, operationResult, resultOut))
		{
			AES_DDT_TRACE_MESSAGE("ERROR: adminOperationResult() Failed! DN:<%s> OperationId:<%d>. ErrorCode:<%d> ErrorMsg:<%s>",
					p_objName, (int) operationId, getInternalLastError(), getInternalLastErrorText());

			AES_DDT_LOG(LOG_LEVEL_ERROR, "adminOperationResult() Failed! DN:<%s> OperationId:<%d>. ErrorCode:<%d> ErrorMsg:<%s>",
					p_objName, (int) operationId, getInternalLastError(), getInternalLastErrorText());
		}
	}


} /* namespace ddt_imm */
