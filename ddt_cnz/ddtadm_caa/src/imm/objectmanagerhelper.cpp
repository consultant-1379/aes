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

#include "imm/objectmanagerhelper.h"
#include "imm/imm.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/utility.h"
	#include <ACS_APGCC_Util.H>

	#include <sec/crypto_status.h>
	#include <sec/crypto_api.h>
#endif

#include "acs_apgcc_omhandler.h"

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_ObjectManagerHelper)

namespace imm
{
	namespace ManagedElement
	{
		const std::string attrName = "networkManagedElementId";
		const std::string DN = "managedElementId=1";
	}

	namespace ACAMessageStoreMOC
	{
		const std::string ACA_MESSAGESTORE = "AxeDataRecordMessageStore";
		const std::string ACA_MS_ATTR_MSNAME = "messageStoreName";
	}

	ObjectManagerHelper::ObjectManagerHelper()
	: m_objectManager(new OmHandler()),
	  m_ObjManagerInitialized(false),
	  m_mutex()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ObjectManagerHelper::~ObjectManagerHelper()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	bool ObjectManagerHelper::changePeerStatusAttribute(const std::string& peerDN, int32_t newStatus)
	{
		AES_DDT_TRACE_MESSAGE("Change Status to :<%d> of Peer:<%s>", newStatus, peerDN.c_str());

		bool result = false;
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if(initObjManager())
		{
			//Prepare Status Attribute
			ACS_CC_ImmParameter statusAttribute;
			char tmpAttrName[64] = {0};
			imm::peer_attribute::STATUS.copy(tmpAttrName, sizeof(tmpAttrName) - 1);
			statusAttribute.attrName = tmpAttrName;
			statusAttribute.attrType = ATTR_INT32T;
			statusAttribute.attrValuesNum = 1U;
			void* statusValue[1]={ reinterpret_cast<void*>(&newStatus)};
			statusAttribute.attrValues = statusValue;

			result = ( ACS_CC_SUCCESS == m_objectManager->modifyAttribute(peerDN.c_str(), &statusAttribute) );

			if(!result)
			{
				// Failed to update attribute
				AES_DDT_LOG(LOG_LEVEL_ERROR, "IMM modify Status Attribute(<%s>) failed! errorCode:<%d> errorMsg:<%s>",
						peerDN.c_str(), m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
				AES_DDT_TRACE_MESSAGE("IMM modify Status Attribute(<%s>) failed! errorCode:<%d> errorMsg:<%s>",
						peerDN.c_str(), m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
			}
		}

		AES_DDT_TRACE_MESSAGE("Change Status result:<%s>", common::utility::boolToString(result));
		return result;
	}

	void ObjectManagerHelper::initProgressReportForSwitchActivePeer(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			updateActionName(dataSinkDN, imm::actionprogress::SWITCHACTIVEPEER_NAME);
			updateActionId(dataSinkDN, imm::actionprogress::SWITCHACTIVEPEER_ID);
			updateActionProgressPercentage(dataSinkDN, imm::actionprogress::percentage::ZERO);
			updateActionState(dataSinkDN, imm::actionprogress::RUNNING );
			updateActionResult(dataSinkDN, imm::actionprogress::NOT_AVAILABLE);
			resetTimeActionCompleted(dataSinkDN);
			updateTimeActionStarted(dataSinkDN);
			updateTimeOfLastStatusUpdate(dataSinkDN);
		}
	}

	void ObjectManagerHelper::finalizeProgressReportForSwitchActivePeer(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			updateActionState(dataSinkDN, imm::actionprogress::FINISHED);
			updateActionProgressPercentage(dataSinkDN, imm::actionprogress::percentage::COMPLETE);
			updateTimeActionCompleted(dataSinkDN);
			updateTimeOfLastStatusUpdate(dataSinkDN);
		}
	}


	void ObjectManagerHelper::updateActionName(const std::string& dataSinkDN, const std::string& name)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
		if( initObjManager() )
		{
			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::NAME.copy(attributeNameAsChar, bufferSize-1);

			updateReportAttribute(dataSinkDN, attributeNameAsChar, name);
		}
	}

	void ObjectManagerHelper::updateActionId(const std::string& dataSinkDN, uint32_t id)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::ACTION_ID.copy(attributeNameAsChar, bufferSize-1);
			updateReportAttribute(dataSinkDN, attributeNameAsChar, id);
		}
	}

	void ObjectManagerHelper::updateActionResult(const std::string& dataSinkDN, actionprogress::ResultType result)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			int32_t resultValue = static_cast<int32_t>(result);
			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::RESULT.copy(attributeNameAsChar, bufferSize-1);
			updateReportAttribute(dataSinkDN, attributeNameAsChar, resultValue);
		}
	}

	void ObjectManagerHelper::updateActionResultInfo(const std::string& dataSinkDN, const std::string& info)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
		if( initObjManager() )
		{
			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::RESULT_INFO.copy(attributeNameAsChar, bufferSize-1);

			updateReportAttribute(dataSinkDN, attributeNameAsChar, info);
		}
	}

	void ObjectManagerHelper::updateActionState(const std::string& dataSinkDN, actionprogress::StateType state)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
		if( initObjManager() )
		{
			int32_t stateValue = static_cast<int32_t>(state);
			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::STATE.copy(attributeNameAsChar, bufferSize-1);
			updateReportAttribute(dataSinkDN, attributeNameAsChar, stateValue);
		}
	}

	void ObjectManagerHelper::updateActionProgressPercentage(const std::string& dataSinkDN, uint32_t progressPercentage)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::PROGRESS_PERCENTAGE.copy(attributeNameAsChar, bufferSize-1);
			updateReportAttribute(dataSinkDN, attributeNameAsChar, progressPercentage);
		}
	}

	void ObjectManagerHelper::updateTimeActionStarted(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			std::string currentDateTime;
			getDateAndTime(currentDateTime);

			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::TIME_STARTED.copy(attributeNameAsChar, bufferSize-1);

			updateReportAttribute(dataSinkDN, attributeNameAsChar, currentDateTime);
		}
	}

	void ObjectManagerHelper::updateTimeActionCompleted(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			std::string currentDateTime;
			getDateAndTime(currentDateTime);

			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::TIME_COMPLETED.copy(attributeNameAsChar, bufferSize-1);

			updateReportAttribute(dataSinkDN, attributeNameAsChar, currentDateTime);
		}
	}

	void ObjectManagerHelper::resetTimeActionCompleted(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::TIME_COMPLETED.copy(attributeNameAsChar, bufferSize-1);

			// Empty string
			std::string currentDateTime;

			updateReportAttribute(dataSinkDN, attributeNameAsChar, currentDateTime);
		}
	}

	void ObjectManagerHelper::updateTimeOfLastStatusUpdate(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("DataSink:<%s>", dataSinkDN.c_str());
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			std::string currentDateTime;
			getDateAndTime(currentDateTime);

			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			actionprogress_attribute::TIME_OF_LAST_UPDATE.copy(attributeNameAsChar, bufferSize-1);

			updateReportAttribute(dataSinkDN, attributeNameAsChar, currentDateTime);
		}
	}

	bool ObjectManagerHelper::initObjManager()
	{
		if(!m_ObjManagerInitialized)
		{
			m_ObjManagerInitialized = (ACS_CC_SUCCESS == m_objectManager->Init());

			if(!m_ObjManagerInitialized)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "*** CANNOT INITIALIZE IMM OBJECT MANAGER! errorCode:<%d> errorMsg:<%s> ***",
						m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
			}
		}

		return m_ObjManagerInitialized;
	}

	void ObjectManagerHelper::updateReportAttribute(const std::string& dataSinkDN, char* attributeName, const std::string& attributeValue)
	{
		AES_DDT_TRACE_MESSAGE("Attribute:<%s> value:<%s>", attributeName, attributeValue.c_str() );

		ACS_CC_ImmParameter progressReportAttribute;

		progressReportAttribute.attrName = attributeName;
		progressReportAttribute.attrType = ATTR_STRINGT;
		progressReportAttribute.attrValuesNum = 1;

		const unsigned int bufferSize= 256U;
		char tmpValue[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(tmpValue, bufferSize, "%s", attributeValue.c_str() );

		void* tmpValueArray[1] = { reinterpret_cast<void*>(tmpValue) };
		progressReportAttribute.attrValues = tmpValueArray;

		char progressReportDN[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(progressReportDN, bufferSize, "%s=%s,%s", actionprogress_attribute::RDN.c_str(), actionprogress::SWITCHACTIVEPEER_RDN.c_str(), dataSinkDN.c_str() );

		ACS_CC_ReturnType updateResult = m_objectManager->modifyAttribute(progressReportDN, &progressReportAttribute);

		if( ACS_CC_SUCCESS != updateResult)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "IMM modify Attribute(<%s>) of <%s> failed! errorCode:<%d> errorMsg:<%s>",
					attributeName, progressReportDN, m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
			AES_DDT_TRACE_MESSAGE("IMM modify Attribute(<%s>) of <%s> failed! errorCode:<%d> errorMsg:<%s>",
					attributeName, progressReportDN, m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
		}
	}

	void ObjectManagerHelper::updateReportAttribute(const std::string& dataSinkDN, char* attributeName, int32_t attributeValue)
	{
		AES_DDT_TRACE_MESSAGE("Attribute:<%s> value:<%d>", attributeName, attributeValue );
		ACS_CC_ImmParameter progressReportAttribute;

		progressReportAttribute.attrName = attributeName;
		progressReportAttribute.attrType = ATTR_INT32T;
		progressReportAttribute.attrValuesNum = 1;

		void* tmpValueArray[1] = { reinterpret_cast<void*>(&attributeValue) };
		progressReportAttribute.attrValues = tmpValueArray;

		const unsigned int bufferSize= 256U;
		char progressReportDN[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(progressReportDN, bufferSize, "%s=%s,%s", actionprogress_attribute::RDN.c_str(), actionprogress::SWITCHACTIVEPEER_RDN.c_str(), dataSinkDN.c_str() );

		ACS_CC_ReturnType updateResult = m_objectManager->modifyAttribute(progressReportDN, &progressReportAttribute);

		if( ACS_CC_SUCCESS != updateResult)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "IMM modify Attribute(<%s>) of <%s> failed! errorCode:<%d> errorMsg:<%s>",
					attributeName, progressReportDN, m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
			AES_DDT_TRACE_MESSAGE("IMM modify Attribute(<%s>) of <%s> failed! errorCode:<%d> errorMsg:<%s>",
					attributeName, progressReportDN, m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
		}
	}

	void ObjectManagerHelper::updateReportAttribute(const std::string& dataSinkDN, char* attributeName, uint32_t attributeValue)
	{
		AES_DDT_TRACE_MESSAGE("Attribute:<%s> value:<%u>", attributeName, attributeValue );
		ACS_CC_ImmParameter progressReportAttribute;

		progressReportAttribute.attrName = attributeName;
		progressReportAttribute.attrType = ATTR_UINT32T;
		progressReportAttribute.attrValuesNum = 1;

		void* tmpValueArray[1] = { reinterpret_cast<void*>(&attributeValue) };
		progressReportAttribute.attrValues = tmpValueArray;

		const unsigned int bufferSize= 256U;
		char progressReportDN[bufferSize] = {0};
		// Assemble the RDN value
		ACE_OS::snprintf(progressReportDN, bufferSize, "%s=%s,%s", actionprogress_attribute::RDN.c_str(), actionprogress::SWITCHACTIVEPEER_RDN.c_str(), dataSinkDN.c_str() );

		ACS_CC_ReturnType updateResult = m_objectManager->modifyAttribute(progressReportDN, &progressReportAttribute);

		if( ACS_CC_SUCCESS != updateResult)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "IMM modify Attribute(<%s>) of <%s> failed! errorCode:<%d> errorMsg:<%s>",
					attributeName, progressReportDN, m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
			AES_DDT_TRACE_MESSAGE("IMM modify Attribute(<%s>) of <%s> failed! errorCode:<%d> errorMsg:<%s>",
					attributeName, progressReportDN, m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText() );
		}
	}

	void ObjectManagerHelper::getDateAndTime(std::string& currentDateAndTime)
	{
		currentDateAndTime.clear();

		const unsigned int bufferSize= 256U;
		char tmpBuffer[bufferSize] = {0};

		time_t currentTime = time(NULL);
		struct tm *tmpTimeStruct = localtime(&currentTime);
		if( (NULL != tmpTimeStruct )
				&& ( strftime(tmpBuffer, sizeof(tmpBuffer), "%Y-%m-%dT%X", tmpTimeStruct) != 0 ))
		{
			currentDateAndTime.assign(tmpBuffer);
		}
	}

	std::string ObjectManagerHelper::getManagedElementId()
	{

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);
		std::string managedElemId;

		if( initObjManager() )
		{
			ACS_CC_ImmParameter paramToFind;

			const unsigned int bufferSize= 256U;
			char attributeNameAsChar[bufferSize] = {0};
			ManagedElement::attrName.copy(attributeNameAsChar, bufferSize-1);

			paramToFind.attrName = attributeNameAsChar;
			paramToFind.attrType = ATTR_STRINGT;


			if( ACS_CC_SUCCESS == m_objectManager->getAttribute(ManagedElement::DN.c_str(), &paramToFind))
			{
				if(0 != paramToFind.attrValuesNum)
				{
					managedElemId.assign(reinterpret_cast<char*>(paramToFind.attrValues[0]));
					AES_DDT_TRACE_MESSAGE("Managed element ID : <%s>",managedElemId.c_str());
					AES_DDT_LOG(LOG_LEVEL_INFO,"Managed element ID : <%s>", managedElemId.c_str());
				}

			}

		}

		return managedElemId;
	}

	common::errorCode::ErrorConstants ObjectManagerHelper::getACAMessageStoreNames(std::vector<std::string> & msNames)
	{
		AES_DDT_TRACE_FUNCTION;

		common::errorCode::ErrorConstants result = common::errorCode::ERR_GENERIC;
		std::vector<std::string> msList;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_mutex);

		if( initObjManager() )
		{
			if (ACS_CC_SUCCESS == m_objectManager->getClassInstances(ACAMessageStoreMOC::ACA_MESSAGESTORE.c_str(), msList))
			{
				ACS_CC_ImmParameter messageStoreName;

				const unsigned int bufferSize= 256U;
				char attributeMSNameAsChar[bufferSize] = {0};
				ACAMessageStoreMOC::ACA_MS_ATTR_MSNAME.copy(attributeMSNameAsChar, bufferSize-1);

				messageStoreName.attrName = attributeMSNameAsChar;

				for(std::vector<std::string>::const_iterator it = msList.begin(); it != msList.end(); ++it)
				{
					if( (ACS_CC_SUCCESS == m_objectManager->getAttribute((*it).c_str(),&messageStoreName)) && (messageStoreName.attrValuesNum>0))
					{
						std::string msName(reinterpret_cast<char *>(messageStoreName.attrValues[0]));
						ACS_APGCC::toUpper(msName);
						msNames.push_back(msName);
						AES_DDT_LOG(LOG_LEVEL_INFO, "Message StoreNames  = %s ", msName.c_str());
						AES_DDT_TRACE_MESSAGE("ACA Message StoreNames  = %s ", msName.c_str());
						result = common::errorCode::ERR_NO_ERRORS;
					}
					else
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "Call 'getAttribute' for object '%s' failed, error = '%s', "
								"continuing with next message store", (*it).c_str(), m_objectManager->getInternalLastErrorText());
						AES_DDT_TRACE_MESSAGE( "ERROR Call 'getAttribute' for object '%s' failed, error = '%s', "
								"continuing with next message store", (*it).c_str(), m_objectManager->getInternalLastErrorText());
						result = common::errorCode::ERR_GENERIC;
						break;
					}

				} //for loop
			}
			else
			{
				//ERROR
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Call 'getClassInstances' failed, error = '%s', returning -1",
						m_objectManager->getInternalLastErrorText());
				AES_DDT_TRACE_MESSAGE ("ERROR Call 'getClassInstances' failed, error = '%s', returning -1",
						m_objectManager->getInternalLastErrorText());
				result = common::errorCode::ERR_GENERIC;
			}
		}

		return result;
	}

	common::errorCode::ErrorConstants ObjectManagerHelper::getPassword(const std::string& passwordDN, std::string& password)
	{
		AES_DDT_TRACE_FUNCTION;
		common::errorCode::ErrorConstants result = common::errorCode::ERR_GENERIC;

		password.clear();

		// Init OM resource
		if( initObjManager() )
		{
			char ecimPwdAttributeName[256] = {0};
			imm::ecimpassword_attribute::PASSWORD.copy(ecimPwdAttributeName, imm::ecimpassword_attribute::PASSWORD.length());
			ACS_CC_ImmParameter passwordAttribute;

			passwordAttribute.attrName = ecimPwdAttributeName;

			//retrieving the password from ECIM password structure
			if ( (m_objectManager->getAttribute( passwordDN.c_str(), &passwordAttribute) == ACS_CC_SUCCESS) &&
					( 0U != passwordAttribute.attrValuesNum) )
			{
				std::string ciphertext = reinterpret_cast<char*>(passwordAttribute.attrValues[0]);

				AES_DDT_TRACE_MESSAGE("ciphertext:<%s>", ciphertext.c_str());

				if(!ciphertext.empty())
				{
					// password decrypting
					char* plaintext = NULL;

					if(sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str()) == SEC_CRYPTO_OK )
					{
						password.assign(plaintext);
						result = common::errorCode::ERR_NO_ERRORS;
					}
					else
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "PASSWORD DECRYPTING FAILED! ciphertext:<%s>", ciphertext.c_str() );
						AES_DDT_TRACE_MESSAGE("ERROR: PASSWORD DECRYPTING FAILED! ciphertext:<%s>", ciphertext.c_str() );
					}

					free(plaintext);
				}
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
							passwordDN.c_str(), m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText());
				AES_DDT_TRACE_MESSAGE("ERROR: OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
									  passwordDN.c_str(), m_objectManager->getInternalLastError(), m_objectManager->getInternalLastErrorText());
			}

		}

		AES_DDT_TRACE_MESSAGE("result:<%s> *** ONLY FOR TEST PWD:<%s> ***", common::utility::boolToString(result), password.c_str() );
		return result;
	}

} /* namespace imm */
