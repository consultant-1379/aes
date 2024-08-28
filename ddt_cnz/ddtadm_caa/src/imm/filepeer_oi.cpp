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
#include "imm/filepeer_oi.h"

#include "imm/imm.h"

#ifndef CUTE_TEST
	#include "imm/configurationhelper.h"
	#include "common/macros.h"
	#include "common/utility.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "engine/workingset.h"
	#include "engine/context.h"

	#include <acs_apgcc_omhandler.h>
	#include <ACS_APGCC_Util.H>
#endif

namespace path_checkers
{
	const std::string INVALID_SEQUENCE("//");
	const std::string VALID_CHARS("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_/");
}

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_FilePeer_OI)

namespace imm
{
	FilePeer_OI::FilePeer_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName) :
					Base_OI(managedObjectClassName, objectImplementerName)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	FilePeer_OI::~FilePeer_OI()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ACS_CC_ReturnType FilePeer_OI::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr)
	{
		AES_DDT_TRACE_MESSAGE("class:<%s>, parent:<%s>", className,  parentname);
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		filePeer newFilePeer;
		newFilePeer.action = Create;
		m_currentOperationId = operation::FILEPEER_ADD;

		// extract the attributes
		for(size_t idx = 0U; (NULL != attr[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			// RDN
			if( 0 == imm::filepeer_attribute::RDN.compare(attr[idx]->attrName) )
			{
				std::string dataSinkDn(parentname);

				// get the datasource name from DN
				if( !common::utility::getRDNValue(dataSinkDn, imm::datasource_attribute::RDN, newFilePeer.info.dataSourceName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				ACS_APGCC::toUpper(newFilePeer.info.dataSourceName);

				AES_DDT_TRACE_MESSAGE("DataSource name:<%s>", newFilePeer.getDataSourceName());

				// get the datasink name from DN
				if( !common::utility::getRDNValue(dataSinkDn, imm::datasink_attribute::RDN, newFilePeer.info.dataSinkName) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("DataSink name:<%s>", newFilePeer.getDataSinkName());

				// datasink RDN e.g: "filePeerId=SERVER1"
				std::string filePeerRDN = reinterpret_cast<char*>(attr[idx]->attrValues[0]);

				// get the filepeer name from DN
				if( !common::utility::getRDNValue(filePeerRDN, imm::filepeer_attribute::RDN, newFilePeer.info.name) )
				{
					result = ACS_CC_FAILURE;
					break;
				}

				AES_DDT_TRACE_MESSAGE("FilePeer name:<%s>", newFilePeer.getFilePeerName());

				//build something like: filePeerId=SERVER1,dataSinkId=REMOTE,dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferId=1
				newFilePeer.info.moDN.assign(filePeerRDN);
				newFilePeer.info.moDN.push_back(imm::parserTag::comma);
				newFilePeer.info.moDN += dataSinkDn;

				continue;
			}

			// USERNAME
			if( 0 == imm::filepeer_attribute::USERNAME.compare(attr[idx]->attrName))
			{
				newFilePeer.info.username.assign(reinterpret_cast<char*>(attr[idx]->attrValues[0]));
				AES_DDT_TRACE_MESSAGE("USERNAME: <%s>", newFilePeer.info.username.c_str());
				continue;
			}

			// PASSWORD
			if( 0 == imm::filepeer_attribute::PASSWORD.compare(attr[idx]->attrName) )
			{
				newFilePeer.info.passwordDN.assign(reinterpret_cast<char*>(attr[idx]->attrValues[0]));
				AES_DDT_TRACE_MESSAGE("password DN: <%s>", newFilePeer.info.passwordDN.c_str());
				continue;
			}

			// IPADDRESS
			if( 0 == imm::filepeer_attribute::IPADDRESS.compare(attr[idx]->attrName) )
			{
				std::string strIP = reinterpret_cast<char*>(attr[idx]->attrValues[0]);
				if(common::utility::isIPAddressValid(strIP.c_str()) != 0)
				{
					result = ACS_CC_FAILURE;
					break;
				}
				newFilePeer.info.ipAddress.assign(reinterpret_cast<char*>(attr[idx]->attrValues[0]));
				AES_DDT_TRACE_MESSAGE("IP Address: <%s>", newFilePeer.info.ipAddress.c_str());
				continue;
			}

			// PORTNUMBER
			if( 0 == imm::filepeer_attribute::PORTNUMBER.compare(attr[idx]->attrName)  && (attr[idx]->attrValuesNum > 0))
			{
				newFilePeer.info.portNumber = *reinterpret_cast<uint32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("portNumber: <%u>", newFilePeer.info.portNumber);
				continue;
			}

			// REMOTEFOLDERPATH
			if(( 0 == imm::filepeer_attribute::REMOTEFOLDERPATH.compare(attr[idx]->attrName) ) && (attr[idx]->attrValuesNum > 0))
			{
				newFilePeer.info.remoteFolderPath.assign(reinterpret_cast<char*>(attr[idx]->attrValues[0]));
				adjustPathSlashes(newFilePeer.info.remoteFolderPath);
				AES_DDT_TRACE_MESSAGE( "REMORE FOLDER PATH: <%s> is %s", newFilePeer.info.remoteFolderPath.c_str(), (result != ACS_CC_FAILURE ? "VALID" : "NOT VALID") );
				continue;
			}

			// TRANSFERPROTOCOL
			if( 0 == imm::filepeer_attribute::TRANSFERPROTOCOL.compare(attr[idx]->attrName) )
			{
				newFilePeer.info.transferProtocol = *reinterpret_cast<int32_t*>(attr[idx]->attrValues[0]);
				AES_DDT_TRACE_MESSAGE("TRANSFER PROTOCOL: <%s>", (newFilePeer.info.transferProtocol == 0 ? "SFTP" : "FTP"));
				continue;
			}
		}

		if( (ACS_CC_SUCCESS == result) && (ACS_CC_SUCCESS == portNumberHandling(newFilePeer,oiHandle, ccbId)) )
		{
			operationTable_t::iterator element = m_filePeerOperationTable.insert(std::make_pair(ccbId, newFilePeer));

			configurationHelper_t::instance()->scheduleCreateOperation(newFilePeer.info.dataSourceName, ccbId,
					ConfigurationHelper::FilePeer, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error on FilePeer parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC);
		}

		return result;
	}

	ACS_CC_ReturnType FilePeer_OI::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		filePeer deleteFilePeer;
		deleteFilePeer.action = Delete;
		deleteFilePeer.info.moDN.assign(objName);
		m_currentOperationId = operation::FILEPEER_DEL;

		// get the datasource name from DN
		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, deleteFilePeer.info.dataSourceName) &&
			common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, deleteFilePeer.info.dataSinkName) &&
			common::utility::getRDNValue(objName, imm::filepeer_attribute::RDN, deleteFilePeer.info.name))
		{
			ACS_APGCC::toUpper(deleteFilePeer.info.dataSourceName);
			result =  ACS_CC_SUCCESS;
			AES_DDT_TRACE_MESSAGE("Delete FilePeer:<%s> in DataSink:<%s> of DS:<%s>",
					deleteFilePeer.getFilePeerName(), deleteFilePeer.getDataSinkName(), deleteFilePeer.getDataSourceName());

			operationTable_t::iterator element = m_filePeerOperationTable.insert(std::pair<ACS_APGCC_CcbId, filePeer>(ccbId, deleteFilePeer));

			configurationHelper_t::instance()->scheduleDeleteOperation(deleteFilePeer.info.dataSourceName, ccbId,
								ConfigurationHelper::FilePeer, m_currentOperationId, &(element->second.info));
		}
		else
		{
			// Error
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType FilePeer_OI::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
	{
		AES_DDT_TRACE_MESSAGE("objName:<%s>", objName);
		ACS_CC_ReturnType result = ACS_CC_FAILURE;
		UNUSED(oiHandle);

		filePeer modifyFilePeer;
		modifyFilePeer.action = Modify;
		modifyFilePeer.info.moDN.assign(objName);

		m_currentOperationId = operation::FILEPEER_MOD;

		if( common::utility::getRDNValue(objName, imm::datasource_attribute::RDN, modifyFilePeer.info.dataSourceName) &&
				common::utility::getRDNValue(objName, imm::datasink_attribute::RDN, modifyFilePeer.info.dataSinkName) &&
				common::utility::getRDNValue(objName, imm::filepeer_attribute::RDN, modifyFilePeer.info.name))
		{
			ACS_APGCC::toUpper(modifyFilePeer.info.dataSourceName);
			result = ACS_CC_SUCCESS;
		}

		// extract the attributes
		for(size_t idx = 0U; (NULL != attrMods[idx]) && (ACS_CC_FAILURE != result); ++idx)
		{
			// USERNAME
			if( 0 == imm::filepeer_attribute::USERNAME.compare(attrMods[idx]->modAttr.attrName) )
			{
				if( (attrMods[idx]->modAttr.attrValuesNum > 0) )
				{
					modifyFilePeer.info.username.assign(reinterpret_cast<char*>(attrMods[idx]->modAttr.attrValues[0]));
					modifyFilePeer.info.changeMask |= operation::changeMask::USERNAME_CHANGE;

					AES_DDT_TRACE_MESSAGE("USERNAME: <%s>", modifyFilePeer.info.username.c_str());
					continue;
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "Attribute <%s> of <%s> MO is not nillable", attrMods[idx]->modAttr.attrName, objName);
					// Not allowed to remove username attribute
					result = ACS_CC_FAILURE;
					break;
				}
			}

			// PASSWORD
			if( 0 == imm::filepeer_attribute::PASSWORD.compare(attrMods[idx]->modAttr.attrName) )
			{
				if( (attrMods[idx]->modAttr.attrValuesNum > 0) )
				{
					modifyFilePeer.info.passwordDN.assign(reinterpret_cast<char*>(attrMods[idx]->modAttr.attrValues[0]));
					modifyFilePeer.info.changeMask |= operation::changeMask::PASSWORD_CHANGE;

					AES_DDT_TRACE_MESSAGE("password DN: <%s>", modifyFilePeer.info.passwordDN.c_str());
					continue;
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "Attribute <%s> of <%s> MO is not nillable", attrMods[idx]->modAttr.attrName, objName);
					// Not allowed to remove password attribute
					result = ACS_CC_FAILURE;
					break;
				}
			}

			// PORTNUMBER
			if( 0 == imm::filepeer_attribute::PORTNUMBER.compare(attrMods[idx]->modAttr.attrName))
			{
				if( (attrMods[idx]->modAttr.attrValuesNum > 0) )
				{
					modifyFilePeer.info.portNumber = *reinterpret_cast<uint32_t*>(attrMods[idx]->modAttr.attrValues[0]);
					modifyFilePeer.info.changeMask |= operation::changeMask::PORT_NUMBER_CHANGE;

					AES_DDT_TRACE_MESSAGE("portNumber: <%u>", modifyFilePeer.info.portNumber);
					continue;
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "Attribute <%s> of <%s> MO is not nillable", attrMods[idx]->modAttr.attrName, objName);
					// Not allowed to remove port number attribute
					result = ACS_CC_FAILURE;
					break;
				}
			}

			// REMOTEFOLDERPATH
			if( 0 == imm::filepeer_attribute::REMOTEFOLDERPATH.compare(attrMods[idx]->modAttr.attrName) )
			{
				modifyFilePeer.info.changeMask |= operation::changeMask::FOLDER_PATH_CHANGE;

				// check if not empty
				if( (attrMods[idx]->modAttr.attrValuesNum > 0) )
				{
					modifyFilePeer.info.remoteFolderPath.assign(reinterpret_cast<char*>(attrMods[idx]->modAttr.attrValues[0]));
					adjustPathSlashes(modifyFilePeer.info.remoteFolderPath);
				}

				continue;
			}

			// IPADDRESS
			if( 0 == imm::filepeer_attribute::IPADDRESS.compare(attrMods[idx]->modAttr.attrName) )
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "Attribute <%s> of <%s> MO is not changeable", attrMods[idx]->modAttr.attrName, objName);
				// Not allowed to change ip address attribute
				result = ACS_CC_FAILURE;
				break;
			}

		}

		if(ACS_CC_SUCCESS == result)
		{
			//Schedule operation only if something was actually modified
			if (modifyFilePeer.info.changeMask)
			{
				// FilePeer parameters are valid
				operationTable_t::iterator element = m_filePeerOperationTable.insert(std::make_pair(ccbId, modifyFilePeer));

				configurationHelper_t::instance()->scheduleModifyOperation(modifyFilePeer.info.dataSourceName, ccbId,
						ConfigurationHelper::FilePeer, m_currentOperationId, &(element->second.info));
			}
		}
		else
		{
			// Error on FilePeer parameters
			setExitCode(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC );
		}

		return result;
	}

	ACS_CC_ReturnType FilePeer_OI::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_filePeerOperationTable.equal_range(ccbId);

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

				AES_DDT_TRACE_MESSAGE("File Peer operation failed");
				result = ACS_CC_FAILURE;
			}

		}

		AES_DDT_TRACE_MESSAGE(" result:<%d>", result);

		return result;
	}

	void  FilePeer_OI::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_filePeerOperationTable.equal_range(ccbId);

		for(operationTable_t::iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Execute and Clean the configuration from the configuration helper
			configurationHelper_t::instance()->executeMoOperation(element->second.info.dataSourceName, ccbId);
		}

		// Erase all elements from the table of the operations
		m_filePeerOperationTable.erase(operationRange.first, operationRange.second);

		AES_DDT_TRACE_MESSAGE(" IMM request on DataSink MO applied");
	}

	void FilePeer_OI::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(oiHandle);

		// find all operations related to the same ccbid
		std::pair<operationTable_t::iterator, operationTable_t::iterator> operationRange;
		operationRange = m_filePeerOperationTable.equal_range(ccbId);

		//for each operation found
		for(operationTable_t::const_iterator element = operationRange.first; (operationRange.second != element); ++element)
		{
			// Clean the configuration from the configuration helper
			configurationHelper_t::instance()->removeConfiguration(element->second.info.dataSourceName, ccbId);
		}

		// Erase all elements from the table of the operations
		m_filePeerOperationTable.erase(operationRange.first, operationRange.second);
	}

	void FilePeer_OI::adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(paramList);
		int operationResult = SA_AIS_ERR_BAD_OPERATION;

		bool connectionStatus = false;

		if( imm::action::VERIFY_FILE_PEER_CONNECTION_OP_ID == operationId )
		{
			operationResult = SA_AIS_ERR_FAILED_OPERATION;

			std::string dataSourceName;
			std::string peerName;
			common::utility::getRDNValue(p_objName, imm::datasource_attribute::RDN, dataSourceName);
			common::utility::getRDNValue(p_objName, imm::filepeer_attribute::RDN, peerName);

			ACS_APGCC::toUpper(dataSourceName);

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Checking connection for peer: <%s>", peerName.c_str());
			AES_DDT_TRACE_MESSAGE("Checking connection for peer: <%s>", peerName.c_str());

			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);
			if( context )
			{
				boost::shared_ptr<engine::Peer> peer = context->getPeerByName(peerName);

				if( peer )
				{
					operationResult = SA_AIS_OK;
					connectionStatus = peer->isReadyForConnection();
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

	ACS_CC_ReturnType FilePeer_OI::portNumberHandling(filePeer& filePeerData, ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
	{
		ACS_CC_ReturnType result = ACS_CC_SUCCESS;

		if(0 == filePeerData.info.portNumber)
		{
			filePeerData.info.portNumber =
					common::PROTOCOL_FTP == filePeerData.info.transferProtocol ? common::FTP_DEFAULT_PORT : common::SFTP_DEFAULT_PORT;

			AES_DDT_TRACE_MESSAGE("[File Peer: %s] DEFAULT PORT NUMBER: <%d>", filePeerData.getFilePeerName(), filePeerData.info.portNumber);

			ACS_APGCC_CcbHandle ccbHandle;
			ACS_APGCC_AdminOwnerHandle adminOwnerHandler;

			ACS_CC_ReturnType result = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandle, &adminOwnerHandler);

			if (ACS_CC_SUCCESS == result)
			{
				// Set the attribute port of dataSink MOC
				ACS_CC_ImmParameter portNumberAttribute;

				const unsigned int bufferSize = 256U;
				char tmpPortAttr[bufferSize] = {0};
				imm::filepeer_attribute::PORTNUMBER.copy(tmpPortAttr, sizeof(tmpPortAttr) - 1);

				portNumberAttribute.attrName = tmpPortAttr;
				portNumberAttribute.attrType = ATTR_UINT32T;
				portNumberAttribute.attrValuesNum = 1;

				void* tmpValue[1] = {reinterpret_cast<void*>(&(filePeerData.info.portNumber))};
				portNumberAttribute.attrValues = tmpValue;

				result = modifyObjectAugmentCcb(ccbHandle, adminOwnerHandler, filePeerData.getFilePeerDN(), &portNumberAttribute);

				if( ACS_CC_SUCCESS == result)
				{
					// apply transaction
					result =  applyAugmentCcb(ccbHandle);


					if(ACS_CC_SUCCESS != result)
					{
						AES_DDT_TRACE_MESSAGE("ERROR: applyAugmentCcb() Failed! DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
								filePeerData.getFilePeerDN(), getInternalLastError(), getInternalLastErrorText());

						AES_DDT_LOG(LOG_LEVEL_ERROR, "applyAugmentCcb() Failed! DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
								filePeerData.getFilePeerDN(), getInternalLastError(), getInternalLastErrorText());
					}
				}
				else
				{
					AES_DDT_TRACE_MESSAGE("ERROR: modifyObjectAugmentCcb() Failed! Attribute:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
							tmpPortAttr, filePeerData.getFilePeerDN(), getInternalLastError(), getInternalLastErrorText());

					AES_DDT_LOG(LOG_LEVEL_ERROR, "modifyObjectAugmentCcb() Failed! Attribute:<%s> DN:<%s>. ErrorCode:<%d> ErrorMsg:<%s>",
							tmpPortAttr, filePeerData.getFilePeerDN(), getInternalLastError(), getInternalLastErrorText());
				}
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("ERROR: getCcbAugmentationInitialize() Failed!. ErrorCode:<%d> ErrorMsg:<%s>", getInternalLastError(), getInternalLastErrorText());
				AES_DDT_LOG(LOG_LEVEL_ERROR, "getCcbAugmentationInitialize() Failed!. ErrorCode:<%d> ErrorMsg:<%s>", getInternalLastError(), getInternalLastErrorText());
			}

		}

		return result;
	}

	void FilePeer_OI::adjustPathSlashes(std::string& path)
	{
		if(!path.empty())
		{
			//fix the path
			if(path[0] == '/') path = path.substr(1);
			if(path[path.size() - 1] != '/') path.push_back('/');

			AES_DDT_TRACE_MESSAGE("path being used is <%s>", path.c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "path being used is <%s>", path.c_str());
		}
	}

} /* namespace ddt_imm */
