#include "aes_afp_datatransferm_handler.h"

#include <iostream>
#include <ace/Signal.h>
#include <saImmOi.h>
#include <aes_afp_services.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
#include "aes_afp_defines.h"

AES_AFP_TRACE_DEFINE(AES_AFP_DataTransferMCmdHandler);
using namespace std;
#define INTERNAL_ROOT "/data/opt/ap/internal_root"


namespace readyPollParams
{
        const char* attrNameMaxFileNumber = "readyPollMaxFileNumber";
        const char* attrNameOldestFileDate = "readyPollOldestFileDate";
        const char* attrNamePollFrequency = "readyPollingPeriod";
        const unsigned int maxFileNumberLimit = 32000;
        const unsigned int oldestFileLimit = 720; //Hours
        const unsigned int maxPollFrequency = 7200; //Minutes
}

bool deleteflag=0;
//! Constructor
AES_AFP_DataTransferMCmdHandler::AES_AFP_DataTransferMCmdHandler(string className , string szimpName ,aes_afp_datatask &dataThread, ACE_Thread_Manager* thrMgr)
:acs_apgcc_objectimplementereventhandler_V3( szimpName )
{
	m_className = className;
	m_isClassImplAdded = false;
	dataThrd = &dataThread;
	this->thr_mgr( thrMgr);
	AES_AFP_TRACE_MESSAGE("In constructor m_className = %s",m_className.c_str());
	m_tpReactorImpl = new ACE_TP_Reactor;
	if(m_tpReactorImpl != 0)
	{
		m_poReactor = new ACE_Reactor(m_tpReactorImpl);
	}
	m_oiHandler = new acs_apgcc_oihandler_V3() ;

}

AES_AFP_DataTransferMCmdHandler::AES_AFP_DataTransferMCmdHandler( string szObjName,
		string szImpName,
		ACS_APGCC_ScopeT enScope,aes_afp_datatask &dataThread , ACE_Thread_Manager* thrMgr)
:acs_apgcc_objectimplementereventhandler_V3( szObjName,szImpName,enScope )
{
	m_className = szObjName;
	m_isClassImplAdded = false;
	dataThrd = &dataThread;
	this->thr_mgr( thrMgr);
	m_tpReactorImpl = new ACE_TP_Reactor;
	if(m_tpReactorImpl != 0)
	{
		m_poReactor = new ACE_Reactor(m_tpReactorImpl);
	}
	m_oiHandler = new acs_apgcc_oihandler_V3() ;
}


//! Destructor
AES_AFP_DataTransferMCmdHandler::~AES_AFP_DataTransferMCmdHandler()
{
	this->wait();
	if(m_oiHandler!=0)
	{
		delete m_oiHandler;
		m_oiHandler=0;
	}

	if(m_poReactor != 0)
	{
		delete m_poReactor;
		m_poReactor=0;
	}
	if(m_tpReactorImpl != 0)
	{
		delete m_tpReactorImpl;
		m_tpReactorImpl=0;
	}
}
ACS_CC_ReturnType AES_AFP_DataTransferMCmdHandler::setImpl()
{
	for (int i=0; i < 10; ++i)
	{
		m_oiHandler->removeClassImpl(this, m_className.c_str());
		ACS_CC_ReturnType errorCode = m_oiHandler->addClassImpl(this,m_className.c_str());

		if ( errorCode == ACS_CC_FAILURE )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR,"Set implementer %s for the object %s failed",getImpName().c_str(), getObjName().c_str());
			AES_AFP_TRACE_MESSAGE("Set implementer %s for the object %s failed",getImpName().c_str(), getObjName().c_str());
			int intErr = getInternalLastError();	 
			for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
			if ( i >= 10)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed with ErrCode = %d",intErr);
				AES_AFP_TRACE_MESSAGE("Failed with ErrCode = %d",intErr);
				return ACS_CC_FAILURE;
			}
			else
				continue;
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Success: Set implementer %s for the object %s",getImpName().c_str(), getObjName().c_str());
			m_isClassImplAdded = true;
			return ACS_CC_SUCCESS;
		}
	}
	return ACS_CC_FAILURE;
}
ACE_INT32 AES_AFP_DataTransferMCmdHandler::svc()
{
	AES_AFP_TRACE_MESSAGE("Entering");

	if (omHandler.Init() == ACS_CC_FAILURE)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"!Failed :OmHandler initialize");
		AES_AFP_TRACE_MESSAGE("!Failed :OmHandler initialize");
	}
	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();
	AES_AFP_TRACE_MESSAGE("Leaving");
	return 0;
}


ACS_CC_ReturnType AES_AFP_DataTransferMCmdHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr){

	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;
	AES_AFP_TRACE_MESSAGE("ObjectCreateCallback invoked");
	AES_AFP_LOG(LOG_LEVEL_INFO,"create callback received");
	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType AES_AFP_DataTransferMCmdHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName){

	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	AES_AFP_TRACE_MESSAGE("ObjectDeleteCallback invoked");
	AES_AFP_LOG(LOG_LEVEL_INFO,"deleted callback received");
	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType AES_AFP_DataTransferMCmdHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	AES_AFP_TRACE_MESSAGE("ObjectModifyCallback invoked");
	AES_AFP_LOG(LOG_LEVEL_INFO,"modify callback received");
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	(void)attrMods;
	ACS_CC_ReturnType modifyResult=ACS_CC_SUCCESS;
	ACS_APGCC_AttrValues modAttribute;
	unsigned int maxFileNum(0),oldestFileDate(0),pollFrequency(0);
	for(int idx = 0; (ACS_CC_FAILURE != modifyResult)&&(attrMods[idx] != NULL);idx++)
	{
		modAttribute = attrMods[idx]->modAttr;
		if(0 == ACE_OS::strcmp(readyPollParams::attrNameMaxFileNumber,modAttribute.attrName)) 
		{
			if(0 != modAttribute.attrValuesNum)
			{
				maxFileNum = (*reinterpret_cast<unsigned int*>(modAttribute.attrValues[0]));
				modifyResult=(maxFileNum > readyPollParams::maxFileNumberLimit)?ACS_CC_FAILURE:ACS_CC_SUCCESS;
			}
			else
			{
				//Deletion of attribute not possible
				modifyResult=ACS_CC_FAILURE;
				
			}
			continue;
		}
		if(0 == ACE_OS::strcmp(readyPollParams::attrNameOldestFileDate,modAttribute.attrName))
		{
			if(0 != modAttribute.attrValuesNum)
			{
				oldestFileDate = (*reinterpret_cast<unsigned int*>(modAttribute.attrValues[0]));
				modifyResult=(oldestFileDate > readyPollParams::oldestFileLimit)?ACS_CC_FAILURE:ACS_CC_SUCCESS;
			}
			else
			{
				modifyResult=ACS_CC_FAILURE;
			}
			continue;
		}
		if(0 == ACE_OS::strcmp(readyPollParams::attrNamePollFrequency,modAttribute.attrName))
		{
			if(0 != modAttribute.attrValuesNum)
			{
				pollFrequency = (*reinterpret_cast<unsigned int*>(modAttribute.attrValues[0]));
				modifyResult = (pollFrequency > readyPollParams::maxPollFrequency)?ACS_CC_FAILURE:ACS_CC_SUCCESS;
			}
			else
			{
				modifyResult=ACS_CC_FAILURE;
			}
			continue;
		}
	}
	return modifyResult;
}

ACS_CC_ReturnType AES_AFP_DataTransferMCmdHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	AES_AFP_TRACE_MESSAGE("CcbCompleteCallback invoked");
	AES_AFP_LOG(LOG_LEVEL_INFO,"modify complete received");
	return ACS_CC_SUCCESS;

}

void AES_AFP_DataTransferMCmdHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	AES_AFP_TRACE_MESSAGE("CcbAbortCallback invoked");
	AES_AFP_LOG(LOG_LEVEL_INFO,"abort complete received");
}

void AES_AFP_DataTransferMCmdHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	AES_AFP_TRACE_MESSAGE("CcbApplyCallback invoked");
	AES_AFP_LOG(LOG_LEVEL_INFO,"apply complete received");
}

ACS_CC_ReturnType AES_AFP_DataTransferMCmdHandler::updateRuntime(const char *objName, const char **attrName)
{
	(void)objName;
	(void)attrName;
	AES_AFP_TRACE_MESSAGE("UpdateRuntimeCallback invoked");

	return ACS_CC_SUCCESS;
}

void AES_AFP_DataTransferMCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList)
{
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	(void)paramList;
	if (!dataThrd->isDbInitialized())
	{
		AES_AFP_LOG(LOG_LEVEL_WARN, "adminOperationCallback ignored: TQ database is not initialized yet");
		AES_AFP_TRACE_MESSAGE("WARNING: adminOperationCallback ignored: TQ database is not initialized yet");

		char errMsg[512] = {0};
		snprintf(errMsg, sizeof(errMsg),"%s%s", actionResult::NBI_PREFIX, actionResult::FUNCTIONBUSYSTR.c_str() );

		ACS_APGCC_AdminOperationParamType errorMessageParameter;
		errorMessageParameter.attrName = actionResult::ErrorMessageAttribute;
		errorMessageParameter.attrType = ATTR_STRINGT;
		errorMessageParameter.attrValues = reinterpret_cast<void*>(errMsg);
		std::vector<ACS_APGCC_AdminOperationParamType> outParameteres;
		outParameteres.push_back(errorMessageParameter);

		ACS_CC_ReturnType result = adminOperationResult(oiHandle, invocation, actionResult::FAILED, outParameteres);
		if(ACS_CC_SUCCESS != result)
		{
			AES_AFP_TRACE_MESSAGE("error on action result reply: FUNCTION BUSY");
			AES_AFP_LOG(LOG_LEVEL_ERROR, "error on action result reply: FUNCTION BUSY");
		}


		return;
	}


	std::string transferQ;
	std::string dntransferQ;
	int myOIValidationError=1,result;
	ACS_CC_ImmParameter paramToFind;
	aes_afp_datablock db;
	std::string byWho;
	int i=0;
	std::string myDNNameOfTQ(TQ_FILETQRDN);
	string myClassName("");
	string objName = string(p_objName);
	extractClassNameFromRdn(objName, myClassName);
	if(myClassName.compare(AES_AFP_FILETRANSFERM_RDN_CL_NAME) != 0)
	{
		return;
	}
	AES_AFP_TRACE_MESSAGE("AdminOpCallback invoked");
	AES_AFP_LOG(LOG_LEVEL_INFO,"admin operation is invoked");
	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;

	aes_afp_datablock *db1 = new aes_afp_datablock;
	string transferQueue;
	string filename, sourceDirectory, destinationSet;
	string userGroup("");
	int retryTimes = 0;
	int retryTimeInterval = 0;
	bool isDirectory;
	string tmpFilePath;
	size_t delimPos = std::string::npos;
	string tmpFilePath1;
	AES_GCC_Errorcodes errorCode;

	switch(operationId)
	{
	case 1://deleteFileStreamPolicy
		AES_AFP_TRACE_MESSAGE("In Admin call back deleteFileStreamPolicy");
		AES_AFP_LOG(LOG_LEVEL_INFO,"deleteFileTQAndSourceFiles is called");
		while(paramList[i] !=0)
		{
			std::string myParamName = paramList[i]->attrName;
			if(myParamName.compare(TQ_NAME) == 0)
			{
				dntransferQ = string (reinterpret_cast<char *>(paramList[i]->attrValues) );
				AES_AFP_TRACE_MESSAGE("dntransferQ = %s ",dntransferQ.c_str());
			}
			else
			{
				AES_AFP_TRACE_MESSAGE("myParamName = %s ",myParamName.c_str());
			}
			i++;
		}
		transferQ=dntransferQ;
		AES_AFP_TRACE_MESSAGE("dntransferQ %s == transferQ %s",dntransferQ.c_str(), transferQ.c_str() );
		result = api_.removeSourceDirFiles(transferQ);
		if (result == AES_FILEISPROTECTED)
		{
			std::stringstream destStream;
			std::string tmpString("");
			result = api_.listDestinationSets(transferQ, destStream);
			destStream >> tmpString; // skip procorderstart
			destStream >> tmpString;
			bool isLocked(false);
			while (atoi(tmpString.c_str() ) != ENDOFSTREAM)
			{

				result = api_.isTransferQueueLocked(transferQ, tmpString, isLocked, byWho);
				if (isLocked == true)
				{
				}
				destStream >> tmpString;
			}
		}
		if (result)
		{
			myOIValidationError = result;
			AES_AFP_LOG (LOG_LEVEL_ERROR,"Validation failed %d", myOIValidationError);
			AES_AFP_TRACE_MESSAGE("Validation failed %d", myOIValidationError);
			if(result  == AES_FILE_NAME_INVALID)
			{
				/*create first Elemenet of param list*/
				char attName1[]= "errorCode";

				firstElem.attrName = attName1;
				firstElem.attrType = ATTR_INT32T;
				int intValue1 = result;
				firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

				/*create second Elemenet of param list*/
				const char * ertxt1 = api_.getErrorCodeText(result);
				char ertxt[500];
				ACE_OS::memset(ertxt, 0, sizeof(char[500]) );
				strcpy(ertxt,"@ComNbi@");
				strcat(ertxt,ertxt1);
				char attName2[]= "errorText";
				secondElem.attrName = attName2;
				secondElem.attrType = ATTR_STRINGT;
				char* stringValue = const_cast<char*>(ertxt);
				secondElem.attrValues = reinterpret_cast<void*>(stringValue);

				/*insert parameter into the vector*/
				vectorOut.push_back(secondElem);
			}
			break;
		}
		db.setTransferQueue(transferQ);
		myOIValidationError = (int)(dataThrd->handle_removeSourceDirectoryFiles((&db),errorCode));
		AES_AFP_TRACE_MESSAGE("Admin handle_removeSourceDirectoryFiles %d", myOIValidationError);
		if (!myOIValidationError)
		{

			/*create first Elemenet of param list*/
			char attName1[]= "errorCode";

			firstElem.attrName = attName1;
			firstElem.attrType = ATTR_INT32T;
			int intValue1 = errorCode;
			firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

			/*create second Elemenet of param list*/
			const char * ertxt1 = api_.getErrorCodeText(errorCode);
			char ertxt[500];
			ACE_OS::memset(ertxt, 0, sizeof(char[500]) );
			strcpy(ertxt,"@ComNbi@");
			strcat(ertxt,ertxt1);
			char attName2[]= "errorText";
			secondElem.attrName = attName2;
			secondElem.attrType = ATTR_STRINGT;
			char* stringValue = const_cast<char*>(ertxt);
			secondElem.attrValues = reinterpret_cast<void*>(stringValue);

			/*insert parameter into the vector*/
			vectorOut.push_back(secondElem);

			//  setErrorText(errorCode);
			AES_AFP_LOG (LOG_LEVEL_ERROR,"Execution failed %d", myOIValidationError);
			AES_AFP_TRACE_MESSAGE("Execution failed %d", myOIValidationError);
			break;
		}

		deleteflag = 1;
		myDNNameOfTQ += "=";
		myDNNameOfTQ += transferQ;
		myDNNameOfTQ += ",";
		myDNNameOfTQ += p_objName;
		AES_AFP_TRACE_MESSAGE("myDNNameOfTQ == %s",myDNNameOfTQ.c_str());
		paramToFind.attrName = const_cast<char*>(TQ_FILETQRDN);
		if( omHandler.getAttribute(myDNNameOfTQ.c_str(), &paramToFind )==ACS_CC_FAILURE)
		{
			//given TQ not exist in IMM
			AES_AFP_LOG(LOG_LEVEL_ERROR, " Get attribute of TransferQueue failed due to %s ",omHandler.getInternalLastErrorText());
			AES_AFP_TRACE_MESSAGE(" Get attribute of TransferQueue failed due to %s ",omHandler.getInternalLastErrorText());	
			//Free the memory allocated for attribute names.
			myOIValidationError = false;
		}
		else
		{
			AES_AFP_TRACE_MESSAGE(" Success : Get attribute of TransferQueue %s ",myDNNameOfTQ.c_str());
			std::vector<std::string>  p_rdnList;
			omHandler.getChildren(myDNNameOfTQ.c_str(), ACS_APGCC_SUBLEVEL,&p_rdnList);
			AES_AFP_TRACE_MESSAGE("Number of adv param objs under %s group is : %d ",myDNNameOfTQ.c_str(),(int)p_rdnList.size());
			if(p_rdnList.size()==0)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR,"There are no adv param objects");
				AES_AFP_TRACE_MESSAGE("There are no adv param objects");
				myOIValidationError = false;
			}
			else if (p_rdnList.size()==1)
			{
				AES_AFP_TRACE_MESSAGE("Size of Advanced parameters objects vector %d ",(int)p_rdnList.size());
				ACE_INT32 returnCode;
				string advParamDn("");

				advParamDn = p_rdnList[0].c_str();
				AES_AFP_TRACE_MESSAGE("Adv param dn =  %s ",advParamDn.c_str());
				returnCode = omHandler.deleteObject(const_cast<char*>(advParamDn.c_str()));
				if( returnCode != ACS_CC_SUCCESS )
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Deletion of advance param Object failed %s",omHandler.getInternalLastErrorText());
					AES_AFP_TRACE_MESSAGE("Deletion of advance param Object failed %s",omHandler.getInternalLastErrorText());
					myOIValidationError = false;
				}
				else
				{
					advParamDn.clear();
					AES_AFP_TRACE_MESSAGE("!Success : Deletion of advance param Object");
					returnCode = omHandler.deleteObject(const_cast<char*>(myDNNameOfTQ.c_str()));
					if( returnCode != ACS_CC_SUCCESS )
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR, "Deletion of File transfer queue Object failed %s",omHandler.getInternalLastErrorText());
						AES_AFP_TRACE_MESSAGE("!Success : Deletion of advance param Object");
						myOIValidationError = false;
					}

					else
					{
						AES_AFP_TRACE_MESSAGE("!Success: Deletion of File transfer queue Object ");
						myOIValidationError = true;
					}
				}
			}
			else
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "The TQ contains more than one advance param object");
				AES_AFP_TRACE_MESSAGE("The TQ contains more than one advance param object");
				myOIValidationError = false;
			}
		}

		break;

	 case 2: //manualTransferOfEntity
	 {
		 AES_AFP_LOG(LOG_LEVEL_INFO,"manualTransferOfEntity is called");
		 while(paramList[i] !=0)
		 {
			 std::string myParamName = paramList[i]->attrName;
			 if(myParamName.compare(FILE_PATH) == 0)
			 {
				 tmpFilePath = string( reinterpret_cast<char *>(paramList[i]->attrValues) );
				 AES_AFP_TRACE_MESSAGE("tmpFilePath = %s ",tmpFilePath.c_str());
			 }
			 else if(myParamName.compare(TQ_NAME) == 0)
			 {
				 transferQueue = string( reinterpret_cast<char *>(paramList[i]->attrValues) );
				 AES_AFP_TRACE_MESSAGE("transferQueue = %s ",transferQueue.c_str());
			 }
			 else if(myParamName.compare(DESTSET) == 0)
			 {
				 destinationSet = string( reinterpret_cast<char *>(paramList[i]->attrValues) );
				 AES_AFP_TRACE_MESSAGE("destinationSet = %s ",destinationSet.c_str());
			 }
			 i++;
		 }
		 delimPos = tmpFilePath.find_last_of('/');

		if (std::string::npos != delimPos)
		{
			//HS99735 Corrected the source directory path 
			sourceDirectory.assign(INTERNAL_ROOT);
			sourceDirectory += tmpFilePath.substr(0, delimPos+1);
			AES_AFP_TRACE_MESSAGE("sourceDirectory is = %s ",sourceDirectory.c_str());
			filename = tmpFilePath.substr(delimPos+1);
			AES_AFP_TRACE_MESSAGE("filename is = %s ",filename.c_str());
		}
		tmpFilePath = INTERNAL_ROOT + tmpFilePath;
		AES_AFP_TRACE_MESSAGE("tmpFilePath is = %s ",tmpFilePath.c_str());

		ACE_stat f_stat;
		int result;
		result = ACE_OS::stat(tmpFilePath.c_str(),&f_stat);
		if(result == 0)
		{
			if ( S_ISDIR(f_stat.st_mode) )
			{
				isDirectory=true;

			}
			else
			{
				isDirectory=false;
			}
			AES_AFP_TRACE_MESSAGE("isDirectory = %d ",isDirectory);
			db1->setTransferQueue(transferQueue);
			db1->setDestinationSet(destinationSet);		
			db1->setFilename(filename);

			result=dataThrd->handle_transferQueueDefined(db1);
			AES_AFP_TRACE_MESSAGE("result of handle_getTransferQueueAttributes %d",result);
			if(result == true)
			{
				result = dataThrd->handle_transferQueueDestinationSetDefined(db1);
				if(result == true)
				{
					db1->setTransferQueue(transferQueue);
					db1->setUserGroup(userGroup);
					db1->setFilename(filename);
					db1->setSourceDirectory(sourceDirectory);
					db1->setDestinationSet(destinationSet);
					db1->setSendRetries(retryTimes);
					db1->setRetriesDelay(retryTimeInterval);
					if(isDirectory)
						db1->setDirectoryFlag();
					result = dataThrd->handle_sendManually(db1,errorCode);
					if (result == false)
					{
						if (errorCode == AES_INCONSISTENTDIR)
						{
							AES_AFP_LOG(LOG_LEVEL_ERROR,"Admin Operation : sendManually() not successful" );
							AES_AFP_TRACE_MESSAGE("Admin Operation : sendManually() not successful" );
							errorCode = AES_INCONSISTENTDIR;
							myOIValidationError = false;
						}
						else if (errorCode == AES_SENDITEMEXIST)
						{
							myOIValidationError = false;
							errorCode = AES_SENDITEMEXIST;
							AES_AFP_LOG(LOG_LEVEL_ERROR,"Admin Operation : sendManually() not successful" );
							AES_AFP_TRACE_MESSAGE("Admin Operation : sendManually() not successful" );
						}
						else if(errorCode == AES_CATASTROPHIC)
						{
							myOIValidationError = false; 
							errorCode = AES_CATASTROPHIC;
							AES_AFP_LOG(LOG_LEVEL_ERROR,"Admin Operation : sendManually() not successful" );
							AES_AFP_TRACE_MESSAGE("Admin Operation : sendManually() not successful" );		
						}
					}
					else 
					{
						myOIValidationError = true;
						AES_AFP_TRACE_MESSAGE("Admin Operation : sendManually() successful" );
					}
				}
				else
				{
					myOIValidationError = false;
					errorCode = AES_TQDESTSETCOMBINATION;
                        		AES_AFP_LOG(LOG_LEVEL_ERROR,"Admin Operation : Destset name is invalid" );
					AES_AFP_TRACE_MESSAGE("Admin Operation : Destset name is invalid" );

				}
 
			}
			else 
			{
				myOIValidationError = false;
				errorCode = AES_NOPROCORDER;;
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Admin Operation : TQ name is invalid" );
				AES_AFP_TRACE_MESSAGE("Admin Operation : TQ name is invalid" );

			}
			
		}
		else
		{	
			errorCode = AES_NOSUCHITEM;
			myOIValidationError = false;
              		AES_AFP_LOG(LOG_LEVEL_ERROR,"Admin Operation : filename invalid" );
			AES_AFP_TRACE_MESSAGE("Admin Operation : filename invalid" );
		}
		if(myOIValidationError == false)
		{
              		const char * ertxt1 = api_.getErrorCodeText(errorCode);
              		char ertxt[500];
               		strcpy(ertxt,"@ComNbi@");
              		strcat(ertxt,ertxt1);
               		char attName2[]= "errorText";
              		secondElem.attrName = attName2;
               		secondElem.attrType = ATTR_STRINGT;
               		char* stringValue = const_cast<char*>(ertxt);
               		secondElem.attrValues = reinterpret_cast<void*>(stringValue);
               		vectorOut.push_back(secondElem);
		}
	}
		break;
	 default:

		 AES_AFP_LOG(LOG_LEVEL_ERROR, "No Admin call back with this Id %d : ",(int)operationId);
		 AES_AFP_TRACE_MESSAGE("No Admin call back with this Id %d : ",(int)operationId);
		 break;
	}
	ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError,vectorOut);
	if(rc != ACS_CC_SUCCESS)
	{
		AES_AFP_LOG (LOG_LEVEL_ERROR,"Admin op failed %d", myOIValidationError);
		AES_AFP_TRACE_MESSAGE("Admin op failed %d", myOIValidationError);
	}
	if (db1)
		delete db1;
	db1 = 0;
	AES_AFP_TRACE_MESSAGE(" Admin op Success %d", myOIValidationError);	
	deleteflag=0;
}

void AES_AFP_DataTransferMCmdHandler::setErrorText(unsigned int code)
{
        string errorMsg = string(api_.getErrorCodeText(code));
	AES_AFP_TRACE_MESSAGE("setExitCode : ExitCode = %d, Error text = %s ",code,errorMsg.c_str());
        setExitCode(code,errorMsg);
}

void AES_AFP_DataTransferMCmdHandler::shutdown()
{
	if( m_isClassImplAdded == true )
	{
		for (int i=0; i < 10; ++i)
		{
			if( m_oiHandler->removeClassImpl(this, m_className.c_str()) == ACS_CC_FAILURE )
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occured while removing class implementer");
				AES_AFP_TRACE_MESSAGE("Error occured while removing class implementer");
				//sleep for a 0.5 second    //HY75188
				ACE_Time_Value selectTime;
				selectTime.set(0,500000);
				while((ACE_OS::poll(0,0,selectTime) == -1) && errno == EINTR)
				{
					continue;
				}
			}
			else
			{
				AES_AFP_TRACE_MESSAGE( "removing class implementer Success");
				m_isClassImplAdded = false;
				break;
			}
		}
	}
	omHandler.Finalize() ;
	m_poReactor->end_reactor_event_loop();

	AES_AFP_TRACE_MESSAGE("Leaving");
}

void AES_AFP_DataTransferMCmdHandler::extractNameFromDn (const string dnName, string& name)
{
	AES_AFP_TRACE_MESSAGE("Entering");

	name = "";

	string::size_type pos1 = string::npos;

	string::size_type pos2 = string::npos;

	pos1 = dnName.find_first_of("=");
	pos2 = dnName.find_first_of(",");
	if(( pos1 != string::npos) && (pos2 != string::npos)&& pos1 < pos2 )
	{
		name = dnName.substr(pos1+1, pos2-pos1-1);
	}

	AES_AFP_TRACE_MESSAGE("Leaving");
}


void AES_AFP_DataTransferMCmdHandler::extractClassNameFromRdn( const string myObjRdn,
		string &myClassName)
{
	AES_AFP_TRACE_MESSAGE( "Entering");

	myClassName = "";
	string::size_type pos1 = string::npos;

	pos1 = myObjRdn.find_first_of("=");
	if ( pos1 != string::npos )
	{
		myClassName = myObjRdn.substr(0, pos1-2);
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
}
