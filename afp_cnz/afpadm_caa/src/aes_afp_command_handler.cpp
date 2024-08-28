#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <list>
#include <ace/Signal.h>
//#include <saImmOi.h>
#include <aes_afp_command_handler.h>
#include <ACS_APGCC_CommonLib.h>
#include <aes_afp_defines.h>
#include <algorithm>
#include "aes_afp_datatransferm_handler.h"
#include "aes_afp_services.h"
#include "aes_ohi_errorcodes.h"
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"

AES_AFP_TRACE_DEFINE(AES_AFP_CommandHandler);
#define TQ_SYS_PATH "/data/aes/data/afp/"

using namespace std;
namespace tqdelim 
{
const char equal = '=';
const char comma = ',';
}
string AES_AFP_CommandHandler::m_rdnName ;


string AES_AFP_CommandHandler::m_parentName ;
bool AES_AFP_CommandHandler::isEmptyDestination = false;
bool AES_AFP_CommandHandler::m_commit=false;
string AES_AFP_CommandHandler::m_tqcommit;
extern bool deleteflag;


#define MAXTRANSFERQUEUE 70

CCBPrevState *prevState = 0;
ACE_Recursive_Thread_Mutex AES_AFP_CommandHandler::abortMX_;


//TODO: return list of unique TQ_Destset entries(excluding TQ_Destset_temp) for a particular TQ. To be used in modify TQ attributes(afpchd)
bool getTq_destset(string tqName)
{
	DIR *dip;
	struct dirent   *dit;
	dip = opendir(TQ_SYS_PATH);

	if( dip == NULL )
	{
		return false;
	}

	while((dit = readdir(dip)) != NULL )
	{
		if (!strcmp(dit->d_name,".") ||!strcmp(dit->d_name,".."))
			continue;
		if( strncasecmp(tqName.c_str(), dit->d_name, strlen(tqName.c_str())) == 0)
		{
			if(dip != NULL)
				closedir(dip);
			return true;
		}
	}  // while fileDirExist
	if(dip != NULL)
		closedir(dip);
	return false;
}

//! Constructor
AES_AFP_CommandHandler::AES_AFP_CommandHandler( string ClassName,
		string szImpName,
		ACS_APGCC_ScopeT enScope, aes_afp_datatask &dataThread, ACE_Thread_Manager* thrMgr)
:acs_apgcc_objectimplementereventhandler_V3( ClassName,szImpName,enScope ), theClassName(ClassName)
{
	this->thr_mgr(thrMgr);
	dataThrd = &dataThread;
	m_isClassImplAdded = false;
	m_tpReactorImpl = 0;
	m_poReactor = 0;
	m_tpReactorImpl = new ACE_TP_Reactor();
	if(m_tpReactorImpl != 0)
	{
		m_poReactor = new ACE_Reactor(m_tpReactorImpl);
	}
	m_createFileTQCcbId =  0;
	m_createFileTQParamCcbId = 0;
	dblock = 0;
	changeDefaultStatus = false;
	changeRenameTemplate = false;
	changeNameTag1 = false;
	changeNameTag2 = false;
	changeRemoveBefore = false;
	changeRemoveDelay = false;
	changeSendRetries = false;
	changeRetryInterval = false;
	changeStartupSequenceNumber = false; //HT50930
	aes_afp_services::m_db = 0;	
}


//! Destructor
AES_AFP_CommandHandler::~AES_AFP_CommandHandler()
{
	this->wait();
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

ACS_CC_ReturnType AES_AFP_CommandHandler::setImpl()
{
	for (int i=0; i < 10; ++i)
	{
		oiHandler.removeClassImpl(this, theClassName.c_str());
		ACS_CC_ReturnType errorCode = oiHandler.addClassImpl(this, theClassName.c_str());

		if ( errorCode == ACS_CC_FAILURE )
		{
			int intErr = getInternalLastError();				
			for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
			if (i >= 10)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Failed with ErrCode = %d", intErr);
				AES_AFP_TRACE_MESSAGE("Failed with ErrCode = %d", intErr);
				return ACS_CC_FAILURE;
			}
			else
				continue;
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("addClassImpl is success for class = %s", theClassName.c_str()); 
			m_isClassImplAdded = true;
			return ACS_CC_SUCCESS;
		}
	}
	return ACS_CC_FAILURE;
}

ACE_INT32 AES_AFP_CommandHandler::svc()
{
	if(omHandler.Init() == ACS_CC_FAILURE )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occurred while initializing OmHandler");
		AES_AFP_TRACE_MESSAGE("Error occurred while initializing OmHandler");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return -1;
	}
	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();
	return 0;
}



ACS_CC_ReturnType AES_AFP_CommandHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	AES_AFP_TRACE_MESSAGE("--------------------------------------------------------");
	AES_AFP_TRACE_MESSAGE("      ObjectCreateCallback for <%s>", className);
	AES_AFP_TRACE_MESSAGE("--------------------------------------------------------");
	AES_AFP_LOG(LOG_LEVEL_INFO, "create callback received for <%s>", className);
	(void)oiHandle;
	(void)ccbId;

	if (!dataThrd->isDbInitialized())
	{
		AES_AFP_LOG(LOG_LEVEL_WARN, "create request rejected for <%s>: TQ database is not initialized yet", className);
		AES_AFP_TRACE_MESSAGE("WARNING: create request rejected for <%s>: TQ database is not initialized yet", className);

		setExitCode(actionResult::FUNCTIONBUSYERR, actionResult::FUNCTIONBUSYSTR);
		return ACS_CC_FAILURE;
	}

	m_parentName= parentName ;
	std::string transferQueue("");
	std::string srcfolder("");
	std::string nameTag("");
	std::string renameTemplate("");
	char tmpstr[50],tmpstr1[50];
	int removebefore=0,removeDelay=10,startupSequenceNumber=0,defaultStatus=0,renamestatus=2;  //HT50930
	int i=0;
	unsigned int result;
	unsigned int resultF;
	string errorMsg;
	//aes_afp_datablock db;
	std::string destinationSet_("");
	int retryTimeInterval = 10;
	int retryTimes = 0;
	std::string userGroup_("");
	std::string tqParamRdn(""),tqRdn("");
	string objName("");
	ACS_CC_ImmParameter paramToFind;
	std::string fileDestination[3];
	//std::string dest[333];
	std::string parent2;
	bool nodestination = false;
	vector<string> initDestsetList,respDestsetList;
	Dest_Set_Type dType = DUMMY;
	string  nameTag1(""), nameTag2("");


	if((AES_GCC_Util::fetchDnOfFileTransferObjFromIMM(omHandler))==-1)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Failed to fetch fileTransfer object dn name");
		AES_AFP_TRACE_MESSAGE("Failed to fetch fileTransfer object dn name");
	}
	std::string parent = AES_GCC_Util::dnOfFileTransferM;
	if( ACE_OS::strcmp( className, TQ_CL_FILETQPARAMNAME ) == 0 )
	{
		m_createFileTQParamCcbId = ccbId;
		if(aes_afp_services::m_db == 0)
		{
			aes_afp_services::m_db = new(std::nothrow)aes_afp_datablock;
			if(aes_afp_services::m_db == NULL)
			{
				setErrorText(AES_CATASTROPHIC);
				return ACS_CC_FAILURE;
			}
		}

		while( attr[i])
		{
			AES_AFP_TRACE_MESSAGE("AttrName %s , i = %d",reinterpret_cast<char *>(attr[i]->attrName), i);
			switch( attr[i]->attrType)
			{
			case ATTR_STRINGT:
				if(ACE_OS::strcmp(attr[i]->attrName, TQ_FILETQPARAMRDN) == 0)
				{
					ACE_OS::strcpy(tmpstr1, reinterpret_cast<char *>(attr[i]->attrValues[0]));
					tqParamRdn=string(tmpstr1);
					AES_AFP_TRACE_MESSAGE(" TQ PARAM RDN %s , i = %d",tqParamRdn.c_str(), i);
					objName=tqParamRdn;
					objName+=",";
					objName+=parent;
					extractNameFromRdn(tqParamRdn,transferQueue);
					result = api_.validateTqName(transferQueue);
					if(result != AES_NOERRORCODE)
					{
						resultF = AES_FILENAMEINVALID;
						setErrorText(resultF, objName.c_str());
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Advanced TransferQueue name is not valid %s", transferQueue.c_str());
						AES_AFP_TRACE_MESSAGE("Advanced TransferQueue name is not valid %s", transferQueue.c_str());
						return ACS_CC_FAILURE;
					}

				}
				else if(ACE_OS::strcmp(attr[i]->attrName, TQ_NAMETAG1) == 0)
				{
					if(attr[i]->attrValuesNum != 0)
					{
						ACE_OS::strcpy(tmpstr, reinterpret_cast<char *>(attr[i]->attrValues[0]));
						AES_AFP_TRACE_MESSAGE("tmpstr = nametags %s, , i = %d",tmpstr,i);
						nameTag1=string(tmpstr);
						changeNameTag1 = true;
						if(dblock == 0)
						{
							dblock = new aes_afp_datablock;
						}
						nameTag = dblock->getNameTag();

						if(ACE_OS::strcmp(nameTag.c_str(),"")==0)
						{
							dblock->setNameTag(nameTag1);
						}else
						{
							nameTag1 += "*";
							nameTag1 += nameTag;
							dblock->setNameTag(nameTag1);
						}

					}
				}
				else if(ACE_OS::strcmp(attr[i]->attrName, TQ_NAMETAG2) == 0)
				{
					if(attr[i]->attrValuesNum != 0)
					{
						ACE_OS::strcpy(tmpstr, reinterpret_cast<char *>(attr[i]->attrValues[0]));
						AES_AFP_TRACE_MESSAGE("tmpstr = nametags %s, , i = %d",tmpstr,i);
						nameTag2=string(tmpstr);
						changeNameTag2 = true;
						if(dblock == 0)
						{
							dblock = new aes_afp_datablock;
						}
						nameTag =  dblock->getNameTag();

						if(ACE_OS::strcmp(nameTag.c_str(), "") != 0)
						{
							nameTag += "*";
						}
						nameTag += nameTag2;
						dblock->setNameTag(nameTag);


					}

				}
				else if(ACE_OS::strcmp(attr[i]->attrName, TQ_RENAMETEMPLATE ) == 0)
				{
					if(attr[i]->attrValuesNum !=0)
					{
						ACE_OS::strcpy(tmpstr, reinterpret_cast<char *>(attr[i]->attrValues[0]));
						AES_AFP_TRACE_MESSAGE("tmpstr = renameTemplate %s, , i = %d",tmpstr,i);
						if(ACE_OS::strcmp(tmpstr,"null")==0)
						{	
							strcpy(tmpstr,"");
						}
						renameTemplate=string(tmpstr);
						AES_AFP_TRACE_MESSAGE("renameTemplate %s, , i = %d",renameTemplate.c_str(),i);
					}
				}
				break;
			case ATTR_INT32T:
				if(ACE_OS::strcmp(attr[i]->attrName, TQ_DEFSTAT ) == 0)
				{
					if(attr[i]->attrValuesNum !=0)
					{
						defaultStatus = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
						AES_AFP_TRACE_MESSAGE("defaultStatus %d,, i = %d",defaultStatus,i);
						if(defaultStatus != AES_FSDELETE)
						{
							if(isEmptyDestination == true)
							{
								setErrorText(AES_ILLEGALDEFSTATUS,parentName);
								return ACS_CC_FAILURE;
							}
						}
					}
				}
				else if(ACE_OS::strcmp(attr[i]->attrName, TQ_REMOVEDELAY ) == 0)
				{
					if(attr[i]->attrValuesNum !=0)
					{
						removeDelay = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
					}
				}
				else if (ACE_OS::strcmp(attr[i]->attrName,TQ_NUMOFRETRIES) == 0)
				{
					if(attr[i]->attrValuesNum !=0)
					{
						retryTimes = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
						AES_AFP_TRACE_MESSAGE("numberOfRetryOnFailure : %d",retryTimes);
					}
				}
				else if (ACE_OS::strcmp(attr[i]->attrName,TQ_RETRYINTERVAL) == 0)
				{
					if(attr[i]->attrValuesNum !=0)
					{
						retryTimeInterval = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
						AES_AFP_TRACE_MESSAGE("retryInterval : %d",retryTimeInterval);
					}
				}
				break;
				//Start - HT50930
			case ATTR_UINT32T:
				if (ACE_OS::strcmp(attr[i]->attrName,TQ_SEQNO) == 0)
				{
					if(attr[i]->attrValuesNum !=0)
					{
						startupSequenceNumber = *reinterpret_cast<int *>(attr[i]->attrValues[0]);
						AES_AFP_TRACE_MESSAGE("startupSequenceNumber : %d", startupSequenceNumber);
					}
				}
				break;
				//End - HT50930
			default:
				break;
			}

			i++;
		}


		extractNameFromRdn(tqParamRdn,transferQueue);

		AES_AFP_LOG(LOG_LEVEL_INFO,"transfer Queue: %s nameTag: %s renameTemplate: %s", transferQueue.c_str(), nameTag.c_str(), renameTemplate.c_str());
		AES_AFP_LOG(LOG_LEVEL_INFO,"srcfolder: %s defaultStatus: %d removebefore: %d", srcfolder.c_str(), defaultStatus, removebefore);
		AES_AFP_LOG(LOG_LEVEL_INFO,"removeDelay: %d renamestatus: %d startupSequenceNumber: %d", removeDelay, renamestatus, startupSequenceNumber);
		AES_AFP_LOG(LOG_LEVEL_INFO,"numberOfRetryOnFailure : %d retryInterval : %d", retryTimes, retryTimeInterval);
		AES_AFP_TRACE_MESSAGE ("transfer Queue: %s",transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE ("nameTag: %s",nameTag.c_str());
		AES_AFP_TRACE_MESSAGE ("renameTemplate: %s",renameTemplate.c_str());
		AES_AFP_TRACE_MESSAGE ("srcfolder: %s",srcfolder.c_str());
		AES_AFP_TRACE_MESSAGE ("defaultStatus: %d",defaultStatus);
		AES_AFP_TRACE_MESSAGE ("removebefore: %d",removebefore);
		AES_AFP_TRACE_MESSAGE ("removeDelay: %d",removeDelay);
		AES_AFP_TRACE_MESSAGE ("renamestatus: %d",renamestatus);
		AES_AFP_TRACE_MESSAGE ("startupSequenceNumber: %d",startupSequenceNumber);
		AES_AFP_TRACE_MESSAGE ("numberOfRetryOnFailure : %d",retryTimes);
		AES_AFP_TRACE_MESSAGE ("retryInterval : %d",retryTimeInterval);

		char strtmp[50];
		ACE_OS::sprintf(strtmp,"%s=%s",TQ_FILETQPARAMRDN,transferQueue.c_str());
		tqRdn = string(strtmp);
		objName=tqRdn;
		objName+=",";
		objName+=parentName;
		std::string nameTag_t("");
		if(dblock != 0)
			nameTag_t = dblock->getNameTag();
		std::string buffer1,buffer2;
		if ( changeNameTag1 == false )
		{
			if(changeNameTag2 == true)
			{
				AES_AFP_LOG (LOG_LEVEL_INFO,"NameTag1 is not given");
				AES_AFP_TRACE_MESSAGE("NameTag1 is not given");
				if(dblock != 0)
				{
					delete dblock;
					dblock = 0;
				}
				unsetflags();
				setErrorText(AES_NAMETAG1MANDATORY,objName.c_str());
				return ACS_CC_FAILURE;
			}
		}
		aes_afp_services::m_db->setNameTag(nameTag_t);
		result = api_.validateRenameTemplate(aes_afp_services::m_db->getNameTag(),renameTemplate);
		if(result != AES_NOERRORCODE)
		{
			setErrorText(result ,  objName.c_str());
			AES_AFP_LOG (LOG_LEVEL_INFO,"validateRenameTemplate FAILED");
			AES_AFP_TRACE_MESSAGE("validateRenameTemplate FAILED");
			if(dblock != 0)
			{
				delete dblock;
				dblock = 0;
			}
			unsetflags();
			return ACS_CC_FAILURE;	
		}
		result = api_.validateTransferQueueParam(
				nameTag_t,
				renameTemplate,
				(AES_GCC_Filestates)defaultStatus,
				removeDelay,
				retryTimeInterval,
				retryTimes,
				//Start - HT50930
				startupSequenceNumber
				//End - HT50930
		);
		if(result != AES_NOERRORCODE)
		{	

			setErrorText(result, objName.c_str());
			if(dblock != 0)
			{
				delete dblock;
				dblock = 0;
			}
			unsetflags();
			AES_AFP_LOG (LOG_LEVEL_INFO,"Validation FAILED");
			AES_AFP_TRACE_MESSAGE("Validation FAILED");
			return ACS_CC_FAILURE;
		}
		if(renameTemplate != "")
		{
			aes_afp_services::m_db->setFileRename(AES_LOCAL);
			aes_afp_services::m_db->setFileTemplate(renameTemplate);
		}
		//Start - HT50930
		aes_afp_services::m_db->setStartupSequenceNumber(startupSequenceNumber);
		//End - HT50930
		aes_afp_services::m_db->setRemoveDelay(removeDelay);
		aes_afp_services::m_db->setSendRetries(retryTimes);
		aes_afp_services::m_db->setRetriesDelay(retryTimeInterval);
		//TR HT10785
		aes_afp_services::m_db->setStatus((AES_GCC_Filestates)defaultStatus);
		if(dblock != 0)
		{
			delete dblock;
			dblock = 0;
		}
	}
	else if( ACE_OS::strcmp( className, TQ_CL_FILETQ ) == 0 )
	{
		m_createFileTQCcbId = ccbId;
		if(aes_afp_services::m_db == 0)
		{
			aes_afp_services::m_db = new(std::nothrow)aes_afp_datablock;
			if(aes_afp_services::m_db == NULL)
			{
				setErrorText(AES_CATASTROPHIC);
				return ACS_CC_FAILURE;
			}
		}
		int nCount = 0;
		string tempTQRDN = "";
		char tmpString[50];
		int counter = 0;
		std::vector<std::string>  transferQueueList;
		omHandler.getClassInstances(TQ_CL_FILETQ, transferQueueList);
		if (transferQueueList.size() >= MAXTRANSFERQUEUE)
		{
			setErrorText(AES_MAXTQ);
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Maximum TQ's that can be created is 70");
			AES_AFP_TRACE_MESSAGE("Maximum TQ's that can be created is 70");
			return ACS_CC_FAILURE;
		}
		while(attr[counter])
		{
			if(ACE_OS::strcmp(attr[counter]->attrName, TQ_FILETQRDN ) == 0)
			{
				ACE_OS::strcpy(tmpString, reinterpret_cast<char *>(attr[counter]->attrValues[0]));
				tempTQRDN=string(tmpString);

			}
			counter++;
		}
		abortMX_.acquire();
		if(!AES_AFP_CommandHandler::m_commit)
		{
			AES_AFP_CommandHandler::m_commit=true;
			extractNameFromRdn(tempTQRDN,transferQueue);
			AES_AFP_CommandHandler::m_tqcommit.assign(transferQueue);
			AES_AFP_TRACE_MESSAGE("CREATE  m_commit= %d, tq_commit =%s  ",AES_AFP_CommandHandler::m_commit,transferQueue.c_str());
		}
		else if(AES_AFP_CommandHandler::m_commit && ((AES_AFP_CommandHandler::m_tqcommit.compare(transferQueue) != 0)))
		{
			AES_AFP_CommandHandler::m_commit=false;
			AES_AFP_CommandHandler::m_tqcommit.clear();
			AES_AFP_LOG(LOG_LEVEL_TRACE," CREATE m_commit= %d, tq_commit =%s  ",AES_AFP_CommandHandler::m_commit,transferQueue.c_str());
			AES_AFP_TRACE_MESSAGE(" CREATE m_commit= %d, tq_commit =%s  ",AES_AFP_CommandHandler::m_commit,transferQueue.c_str());
			setErrorText(AES_COMMITERROR);
			abortMX_.release();
			return ACS_CC_FAILURE;
		}
		abortMX_.release();
		string objTQName = tempTQRDN;		
		objTQName += ",";
		objTQName += parentName;

		while(attr[i])
		{
			AES_AFP_TRACE_MESSAGE("AttrName %s , i = %d",reinterpret_cast<char *>(attr[i]->attrName), i);
			switch( attr[i]->attrType)
			{
			case ATTR_STRINGT:
				if(ACE_OS::strcmp(attr[i]->attrName, TQ_FILETQRDN ) == 0)
				{
					ACE_OS::strcpy(tmpstr1, reinterpret_cast<char *>(attr[i]->attrValues[0]));
					tqRdn=string(tmpstr1);
					objName=tqRdn;
					objName+=",";
					objName+=parent;
					AES_AFP_TRACE_MESSAGE("AttrName %s , i = %d",reinterpret_cast<char *>(attr[i]->attrName), i);
					extractNameFromRdn(tqRdn,transferQueue);
					result = api_.validateTqName(transferQueue);
					if(result != AES_NOERRORCODE)
					{
						resultF = AES_INVALIDFILETQ;
						setErrorText(resultF, objName.c_str());
						AES_AFP_LOG(LOG_LEVEL_ERROR,"TransferQueue is not valid %s", transferQueue.c_str());
						AES_AFP_TRACE_MESSAGE("AttrName %s , i = %d",reinterpret_cast<char *>(attr[i]->attrName), i);
						return ACS_CC_FAILURE;
					}

				}
				if(ACE_OS::strcmp(attr[i]->attrName, TQ_FILETQINITDEST) == 0)
				{
					dType = INIT;
					int num = attr[i]->attrValuesNum;
					if(nCount+num > 1)
					{
						/*! ToDo Eneter this error code in to main error code list */
						setErrorText(AES_MULTIDESTSET, objTQName.c_str());
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set cannot be set with multiple values at a time");
						AES_AFP_TRACE_MESSAGE("Destination set cannot be set with multiple values at a time");
						return ACS_CC_FAILURE;
					}
					if(num != 0)
					{
						paramToFind.attrName = const_cast<char*>(TQ_FILETQINITDESTID);
						for (int j = 0; j< num; j++)
						{
							char str1[50];
							ACE_OS::strcpy(str1, reinterpret_cast<char *>(attr[i]->attrValues[j]));
							if(ACE_OS::strcmp(str1,"-")==0)
							{
								nodestination = true;
								isEmptyDestination = true;
								aes_afp_services::m_db->dest[nCount] = string(str1);
								nCount++;
								continue;
							}
							aes_afp_services::m_db->dest[nCount]=string(str1);
							result = api_.validateDestinationSet(aes_afp_services::m_db->dest[nCount]);
							fileDestination[nCount] = TQ_FILETQINITDESTID;
							fileDestination[nCount] += "=";
							fileDestination[nCount] += aes_afp_services::m_db->dest[nCount];
							fileDestination[nCount] += ",";
							fileDestination[nCount] += parent;
							if(result != AES_NOERRORCODE)
							{
								if(result == AES_INVALIDDESTNAME)
								{
									resultF = AES_UNREAS_IDESTSETNAME;
									setErrorText(resultF, objTQName.c_str());
								}
								else
								{
									//setErrorText(result, fileDestination[nCount].c_str());
									setErrorText(result);
								}
								AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination validation failed %s ", aes_afp_services::m_db->dest[nCount].c_str());
								AES_AFP_TRACE_MESSAGE("Destination validation failed %s ", aes_afp_services::m_db->dest[nCount].c_str());
								return ACS_CC_FAILURE;
							}
							AES_AFP_TRACE_MESSAGE("Index = %d InitiatingDestinationSet = %s",j,aes_afp_services::m_db->dest[nCount].c_str());
							int result1 = omHandler.getAttribute(fileDestination[nCount].c_str(), &paramToFind );
							if ( result1 == ACS_CC_FAILURE)
							{
								/*! @ToDo add this error code into error code list */
								setErrorText(AES_NOIDESTINATION);
								//setErrorText(AES_NODESTINATION);
								//setErrorText(AES_NODESTINATION, fileDestination[nCount].c_str());
								AES_AFP_LOG(LOG_LEVEL_ERROR,"Initiating Destination Set = %s does not exist", str1);
								AES_AFP_TRACE_MESSAGE("Initiating Destination Set = %s does not exist", str1);
								return ACS_CC_FAILURE;
							}
							nCount++;

						}

						for(unsigned int k=0; k < attr[i]->attrValuesNum; ++k)
						{
							initDestsetList.push_back(reinterpret_cast<char *>(attr[i]->attrValues[k]));
						}

						AES_AFP_TRACE_MESSAGE("in init  Ncount = %d", nCount);
					}

				}
				if(ACE_OS::strcmp(attr[i]->attrName, TQ_FILETQRESPDEST) == 0)
				{

					dType = RESP;
					int num = attr[i]->attrValuesNum;
					if(nCount + num > 1)
					{
						setErrorText(AES_MULTIDESTSET, objTQName.c_str());
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set cannot be set with multiple values at a time");
						AES_AFP_TRACE_MESSAGE("Destination set cannot be set with multiple values at a time");
						return ACS_CC_FAILURE;
					}

					if(num != 0)
					{
						paramToFind.attrName = const_cast<char*>(TQ_FILETQRESPDESTID);

						for (int j = 0; j< num; j++)
						{
							char str2[50];
							ACE_OS::strcpy(str2, reinterpret_cast<char *>(attr[i]->attrValues[j]));
							if(ACE_OS::strcmp(str2,"-")==0)
							{
								nodestination = true;
								isEmptyDestination = true;
								aes_afp_services::m_db->dest[nCount] = string(str2);
								nCount++;
								continue;
							}

							aes_afp_services::m_db->dest[nCount]=string(str2);
							result = api_.validateDestinationSet(aes_afp_services::m_db->dest[nCount]);
							fileDestination[nCount] = TQ_FILETQRESPDESTID;
							fileDestination[nCount] += "=";
							fileDestination[nCount] += aes_afp_services::m_db->dest[nCount];
							fileDestination[nCount] += ",";
							fileDestination[nCount] += parent;
							if(result != AES_NOERRORCODE)
							{
								if(result == AES_INVALIDDESTNAME)
								{

									resultF = AES_UNREAS_RDESTSETNAME;

									setErrorText(resultF, objTQName.c_str());
								}
								else
								{
									//	setErrorText(result, fileDestination[nCount].c_str());
									setErrorText(result);
								}
								AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set validation failed %s ", aes_afp_services::m_db->dest[nCount].c_str());
								AES_AFP_TRACE_MESSAGE("Destination set validation failed %s ", aes_afp_services::m_db->dest[nCount].c_str());
								return ACS_CC_FAILURE;
							}
							AES_AFP_TRACE_MESSAGE( "Index = %d RespondingDestinationSet = %s",j,aes_afp_services::m_db->dest[nCount].c_str());
							int result1 = omHandler.getAttribute(fileDestination[nCount].c_str(), &paramToFind );
							if ( result1 == ACS_CC_FAILURE)
							{
								setErrorText(AES_NORDESTINATION);
								//setErrorText(AES_NODESTINATION, fileDestination[nCount].c_str());
								AES_AFP_LOG(LOG_LEVEL_ERROR,"Responding Destination Set = %s does not exist" ,str2);
								AES_AFP_TRACE_MESSAGE("Responding Destination Set = %s does not exist" ,str2);
								return ACS_CC_FAILURE;
							}
							nCount++;
						}
						//Update resp dest set ref attribute
						for(unsigned int k=0; k < attr[i]->attrValuesNum; ++k)
						{
							respDestsetList.push_back(reinterpret_cast<char *>(attr[i]->attrValues[k]));
						}
						AES_AFP_TRACE_MESSAGE("in resp Ncount = %d", nCount);

					}
				}
				break;

			default:
				break;

			}
			i++;

		}

		AES_AFP_TRACE_MESSAGE("Total destinations count = %d", nCount);	
		if(nCount > 1)
		{
			setErrorText(AES_MULTIDESTSET, objTQName.c_str());
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Destination set cannot be set with multiple values at a time");
			AES_AFP_TRACE_MESSAGE("Destination set cannot be set with multiple values at a time");
			return ACS_CC_FAILURE;
		}
		if(nCount == 0)
		{
			//setErrorText(AES_MULTIDESTSET);
			setErrorText(AES_NO_DESTSETNAME, objTQName.c_str());
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Atleast one Destination set has to be defined");
			AES_AFP_TRACE_MESSAGE("Atleast one Destination set has to be defined");
			return ACS_CC_FAILURE;
		}

		ACS_CC_ValuesDefinitionType *attrRdn = 0, *attrStatus = 0;
		vector<ACS_CC_ValuesDefinitionType> attrList;
		ACE_INT32  numAttr1 = 1;
		attrRdn = new ACS_CC_ValuesDefinitionType[numAttr1];
		if(NULL == attrRdn)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for attrRdn");
			AES_AFP_TRACE_MESSAGE("Memory allocation failed for attrRdn");
			return ACS_CC_FAILURE;
		}

		//File the RDN for AdvancedRespParameters.
		char attrdn[]= TQ_FILETQPARAMRDN ;
		attrRdn[0].attrName = attrdn;
		attrRdn[0].attrType = ATTR_STRINGT;
		attrRdn[0].attrValuesNum = 1;
		string rdnValue = attrdn ;
		rdnValue  += "=";
		rdnValue  += transferQueue;
		void *attrValue[1] = {reinterpret_cast<void*>(const_cast<char *>(rdnValue.c_str()))};
		attrRdn[0].attrValues = attrValue;
		attrList.push_back( attrRdn[0] );

		if((nCount == 1) && (nodestination == true))
		{
			ACE_INT32  numAttr2 = 1;
			attrStatus = new ACS_CC_ValuesDefinitionType[numAttr2];
			char attstatus[]= TQ_DEFSTAT ;
			attrStatus[0].attrName = attstatus;
			attrStatus[0].attrType = ATTR_INT32T;
			attrStatus[0].attrValuesNum = 1;
			defaultStatus = AES_FSDELETE;
			void *attrValue[1] = {reinterpret_cast<void*>(&defaultStatus)};
			attrStatus[0].attrValues = attrValue;
			attrList.push_back( attrStatus[0] );
		}
		AES_AFP_TRACE_MESSAGE("RDN Value = %s", rdnValue.c_str());

		parent2 = TQ_FILETQRDN;
		parent2 += "=";
		parent2 += transferQueue;
		parent2 += ",";
		parent2 += parent;

		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );
		if (retVal != ACS_CC_SUCCESS)
		{
			setErrorText(AES_CATASTROPHIC);
			AES_AFP_LOG (LOG_LEVEL_WARN, "getCcbAugmentationInitialize::ACS_CC_FAILURE");
			AES_AFP_TRACE_MESSAGE("getCcbAugmentationInitialize::ACS_CC_FAILURE");
			if( 0 != attrRdn )
			{
				delete[] attrRdn;
				attrRdn = 0;
			}
			if( 0 != attrStatus )
			{
				delete[] attrStatus;
				attrStatus = 0;
			}
			return ACS_CC_FAILURE;
		}

		AES_AFP_TRACE_MESSAGE("Parent = %s", parent2.c_str());
		string AdvFileTqParamsClassName = TQ_CL_FILETQPARAMNAME;

		//Update resp dest set ref attribute
		if(retVal==ACS_CC_SUCCESS)//respDestsetList.size()!=0
		{
			retVal=updateRefAttibutes(ccbHandleVal,adminOwnwrHandlerVal,objName,TQ_FILETQRESPDEST,respDestsetList);
		}
		if(retVal== ACS_CC_SUCCESS)//initDestsetList.size()!=0
		{
			retVal=updateRefAttibutes(ccbHandleVal,adminOwnwrHandlerVal,objName,TQ_FILETQINITDEST,initDestsetList);
		}
		if( retVal != ACS_CC_SUCCESS)
		{
			setErrorText(AES_CATASTROPHIC);
			AES_AFP_LOG (LOG_LEVEL_ERROR, "  updateRefAttibutes failed");
			AES_AFP_TRACE_MESSAGE("  updateRefAttibutes failed");
			if( 0 != attrRdn )
			{
				delete[] attrRdn;
				attrRdn = 0;
			}
			if( 0 != attrStatus )
			{
				delete[] attrStatus;
				attrStatus = 0;
			}
			return ACS_CC_FAILURE;
		}
		else
		{
			/*prepare to apply ccb*/
			retVal = applyAugmentCcb (ccbHandleVal);
			if (retVal != ACS_CC_SUCCESS)
			{
				setErrorText(AES_CATASTROPHIC);
				AES_AFP_LOG (LOG_LEVEL_ERROR, "  applyAugmentCcb::ACS_CC_FAILURE");
				AES_AFP_TRACE_MESSAGE("  applyAugmentCcb::ACS_CC_FAILURE");
				if( 0 != attrRdn )
				{
					delete[] attrRdn;
					attrRdn = 0;
				}
				if( 0 != attrStatus )
				{
					delete[] attrStatus;
					attrStatus = 0;
				}
				return ACS_CC_FAILURE;
			}
		}
		if( 0 != attrRdn )
		{
			delete[] attrRdn;
			attrRdn = 0;
		}
		if( 0 != attrStatus  )
		{
			delete[] attrStatus;
			attrStatus = 0;
		}

		aes_afp_services::m_db->setTransferQueue(transferQueue);
		aes_afp_services::m_db->setFileRename((AES_AFP_Renametypes)renamestatus);
		aes_afp_services::m_db->setFileTemplate(renameTemplate);
		aes_afp_services::m_db->setNameTag(nameTag);
		aes_afp_services::m_db->setSourceDirectory(srcfolder);
		aes_afp_services::m_db->setStartupSequenceNumber(startupSequenceNumber);
		aes_afp_services::m_db->setDestinationSet(destinationSet_);
		aes_afp_services::m_db->setRetriesDelay(retryTimeInterval);
		aes_afp_services::m_db->setSendRetries(retryTimes);
		aes_afp_services::m_db->m_nCount=nCount;
		aes_afp_services::m_db->m_dType = dType;
		if(removebefore)
		{
			aes_afp_services::m_db->setRemoveBefore();
		}
		else
		{
			aes_afp_services::m_db->unsetRemoveBefore();
		}
		aes_afp_services::m_db->setRemoveDelay(removeDelay);
		aes_afp_services::m_db->setStatus((AES_GCC_Filestates)defaultStatus);

		AES_AFP_TRACE_MESSAGE("Total destinations associated to TQ = %d", aes_afp_services::m_db->m_nCount);

	}

	AES_AFP_TRACE_MESSAGE("SUCCESS");	
	setErrorText(AES_NOERRORCODE);
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_AFP_CommandHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;

	AES_AFP_TRACE_FUNCTION;
	AES_AFP_TRACE_MESSAGE("objName: <%s>", objName);
	AES_AFP_LOG(LOG_LEVEL_INFO, "deleted callback received <%s>", objName);

	if (!dataThrd->isDbInitialized())
	{
		AES_AFP_LOG(LOG_LEVEL_WARN, "delete request rejected for <%s>: TQ database is not initialized yet", objName);
		AES_AFP_TRACE_MESSAGE("WARNING: delete request rejected for <%s>: TQ database is not initialized yet", objName);

		setExitCode(actionResult::FUNCTIONBUSYERR, actionResult::FUNCTIONBUSYSTR);
		return ACS_CC_FAILURE;
	}

	std::string transferQueue;
	aes_afp_datablock db;
	AES_GCC_Errorcodes errorCode = AES_NOERRORCODE;
	std::string byWho("");
	std::stringstream destStream;
	std::string tmpString("");
	std::string myClassName("");
	extractClassNameFromRdn(objName, myClassName);
	AES_AFP_TRACE_MESSAGE("Object of class %s will be deleted ", myClassName.c_str());
	AES_AFP_LOG(LOG_LEVEL_INFO,"Object of class %s will be deleted ", myClassName.c_str());
	if (ACE_OS::strcasecmp(myClassName.c_str(),TQ_RDN_CL_FILETQPARAMNAME ) == 0 )
	{
		std::string myParentName(""); 
		extractParentNameFromRdn(objName, myParentName);
		unsigned int result;
		if(deleteflag==1)
		{
			AES_AFP_TRACE_MESSAGE("ADMIN operation called");
			setErrorText(AES_NOERRORCODE);
			return ACS_CC_SUCCESS;
		}
		std::string rdn = string(objName);
		//extractNameFromDn(rdn,transferQueue);
		//extractNameFromRdn(rdn,transferQueue);

		extractFileTQNameFromDn(rdn, transferQueue);
		//remove a dummy TQ from IMM
		string dummyTQ = transferQueue;

		if( getTq_destset(dummyTQ)== false )
		{
			AES_AFP_LOG (LOG_LEVEL_INFO,"ADMIN operation called: for dummy TQ");
			AES_AFP_TRACE_MESSAGE("ADMIN operation called: for dummy TQ");
			setErrorText(AES_NOERRORCODE);
			return  ACS_CC_SUCCESS;
		}
		result = api_.removeTransferQueue(transferQueue);
		/*! @toDo Protection error handle not implemented */
		if (result == AES_FILEISPROTECTED)
		{
			bool isLocked;
			result = api_.listDestinationSets(transferQueue, destStream);
			destStream >> tmpString;
			destStream >> tmpString;
			while (atoi(tmpString.c_str() ) != ENDOFSTREAM)
			{

				result = api_.isTransferQueueLocked(transferQueue, tmpString, isLocked, byWho);
				if (isLocked == true)
				{
				}
				destStream >> tmpString;
			}
			return ACS_CC_FAILURE;
		}
		if(result != AES_NOERRORCODE)
		{
			setErrorText(result, myParentName.c_str());
			AES_AFP_LOG (LOG_LEVEL_INFO,"Validation FAILED");
			AES_AFP_TRACE_MESSAGE("Validation FAILED");
			return ACS_CC_FAILURE;
		}
		db.setTransferQueue(transferQueue);

		//prev state info generation for rollback
		aes_afp_datablock *db1 = 0;
		AES_GCC_Errorcodes errorCode1;
		db1 = dataThrd->handle_getTransferQueueAttributesToModify(&db,errorCode1);
		AES_AFP_TRACE_MESSAGE("deleted:: getTransferQueueattributesToModify///errorCode1 = %d ",errorCode1);
		if(errorCode1 != AES_NOERRORCODE)
		{
			setErrorText(errorCode1, myParentName.c_str());
			AES_AFP_LOG(LOG_LEVEL_ERROR,"handle_getTransferQueueAttributes FAILED, errorCode : %d",errorCode);
			AES_AFP_TRACE_MESSAGE("handle_getTransferQueueAttributes FAILED, errorCode : %d",errorCode);
			//deletion of db1 is not necessary as it won't be allocated for error cases by database_fd; kept for future.
			if (db1 != 0)
			{
				delete db1;
				db1 = 0;
			}
			return ACS_CC_FAILURE;
		}

		if( NULL == db1)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR,"deleted :: Creation of object db1 FAILED");
			AES_AFP_TRACE_MESSAGE("deleted :: Creation of object db1 FAILED");
			return ACS_CC_FAILURE;
		}
		AES_AFP_TRACE_MESSAGE("Create:: TQ = %s\nDestSet = %s\nStatus = %d\nRemoveDelay = %d\nRenameTemplate = %s\nSourceDirectory = %s\nNameTag = %s\nStartupSeqNo = %ld", db1->getTransferQueue().c_str(), db1->getDestinationSet().c_str(), db1->getStatus(), db1->getRemoveDelay(), db1->getFileTemplate().c_str(), db1->getSourceDirectory().c_str(), db1->getNameTag().c_str(), db1->getStartupSequenceNumber());	
		vector<TqInfo> dsetList;
		dsetList = dataThrd->handle_infoAllDestset(transferQueue);
		if (!dsetList.empty())
		{
			abortMX_.acquire();
			vector<TqInfo>::iterator itr;
			prevState = new (std::nothrow) CCBPrevState;
			if (prevState == 0)
			{
				if (db1 != 0)
				{
					delete db1;
					db1 = 0;
				}
				setErrorText(AES_CATASTROPHIC, myParentName.c_str());
				AES_AFP_LOG (LOG_LEVEL_FATAL,"mem allocation failed for prevState");
				AES_AFP_TRACE_MESSAGE("mem allocation failed for prevState");
				abortMX_.release();
				return ACS_CC_FAILURE;
			}
			else
			{    
				int k;
				for(itr = dsetList.begin(), k = 0; itr != dsetList.end(); ++itr, ++k)
				{
					prevState->prev_db[k] = *db1;
					string tmpDset = (*itr).destsetName;
					Dest_Set_Type tmpDestType = (*itr).dsetType;
					prevState->prev_db[k].setDestinationSet(tmpDset);
					prevState->dType[k] = tmpDestType;
				}
				prevState->undoAction = IMMC_CREATE_TQ_REC;
				prevState->size = k;
			}
			abortMX_.release();
		}
		if (db1 != 0)
		{
			delete db1;
			db1 = 0;
		}
		if(!dataThrd->handle_removeTransferQueue((&db),errorCode))
		{
			//do not delete the prevState here, as it will be needed to TQ multidest deletion error reversal.
			//if(errorCode == AES_FILETQREFERRED)
			if((errorCode == AES_STSTQREFERRED) || (errorCode == AES_CPFTQREFERRED) || (errorCode == AES_ALOGTQREFERRED) || (errorCode == AES_PDSTQREFERRED))
				setErrorText(errorCode);
			else
				setErrorText(errorCode, myParentName.c_str());
			AES_AFP_LOG (LOG_LEVEL_ERROR,"FAILED");
			AES_AFP_TRACE_MESSAGE("FAILED");
			return ACS_CC_FAILURE;
		}
		AES_AFP_TRACE_MESSAGE("SUCCESS");
		setErrorText(AES_NOERRORCODE);
		AES_AFP_TRACE_MESSAGE("Advanced object deletion SUCCESS");
		return ACS_CC_SUCCESS;
	}
	setErrorText(AES_NOERRORCODE);
	return ACS_CC_SUCCESS;
}


ACS_CC_ReturnType AES_AFP_CommandHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{

	(void)oiHandle;
	(void)ccbId;
	AES_AFP_TRACE_MESSAGE("-------------------------------------------------------");
	AES_AFP_TRACE_MESSAGE("------ Modify CALL BACK <%s>", objName);
	AES_AFP_TRACE_MESSAGE("-------------------------------------------------------");
	AES_AFP_LOG(LOG_LEVEL_INFO, "modify callback received <%s>", objName);

	if (!dataThrd->isDbInitialized())
	{
		AES_AFP_LOG(LOG_LEVEL_WARN, "modify request rejected for <%s>: TQ database is not initialized yet", objName);
		AES_AFP_TRACE_MESSAGE("WARNING: modify request rejected for <%s>: TQ database is not initialized yet", objName);

		setExitCode(actionResult::FUNCTIONBUSYERR, actionResult::FUNCTIONBUSYSTR);
		return ACS_CC_FAILURE;
	}

	m_rdnName = objName;
	ACE_UINT8 i = 0;
	std::string myClassName, renameTemplate, nameTag1, nameTag2, nameTag;
	extractClassNameFromRdn(objName, myClassName);
	string destList, deststr, tqDn_ ,transferQueue;
	std::stringstream str;
	aes_afp_datablock db, dbblock;
	char tmpstr[100];
	Dest_Set_Type dType = DUMMY;

	tqDn_ = string(objName);
	extractNameFromDn(tqDn_,transferQueue);

	AES_AFP_TRACE_MESSAGE("ClassName =  %s", myClassName.c_str());
	if( ACE_OS::strcasecmp(myClassName.c_str(),TQ_RDN_CL_FILETQPARAMNAME ) == 0 )
	{
		extractFileTQNameFromDn(tqDn_,transferQueue);	
		//int pos=tqDn_.find("fileTransferQueueId=");
		//string temp=tqDn_.substr(pos);
		//int pos1=temp.find(",");
		//int pos2=temp.find("=");
		//string tqTemp=temp.substr(pos2+1,(pos1-pos2-1));
		size_t pos1(0),pos2(0);
		for(int loopStr=0;loopStr<2;loopStr++)
		{
			pos1 = tqDn_.find_first_of(tqdelim::comma,pos1+1);
			pos2 = tqDn_.find_first_of(tqdelim::equal,pos2+1);
		}
		string tqTemp = tqDn_.substr(pos2+1,pos1-pos2-1);
		AES_AFP_TRACE_MESSAGE("MODIFY tqTemp =%s ",tqTemp.c_str());
		abortMX_.acquire();
		if(!AES_AFP_CommandHandler::m_commit)
		{
			AES_AFP_CommandHandler::m_commit=true;
			AES_AFP_CommandHandler::m_tqcommit.assign(tqTemp);
			AES_AFP_TRACE_MESSAGE("MODIFY m_commit= %d, tq_commit =%s  ",AES_AFP_CommandHandler::m_commit,transferQueue.c_str());
		}
		else if(AES_AFP_CommandHandler::m_commit && (AES_AFP_CommandHandler::m_tqcommit.compare(tqTemp) != 0))
		{
			AES_AFP_CommandHandler::m_commit=false;
			AES_AFP_CommandHandler::m_tqcommit.clear();
			AES_AFP_TRACE_MESSAGE("MODIFY m_commit= %d, tq_commit =%s  ",AES_AFP_CommandHandler::m_commit,transferQueue.c_str());
			setErrorText(AES_COMMITERROR);
			abortMX_.release();
			return ACS_CC_FAILURE;
		}
		abortMX_.release();
		AES_AFP_TRACE_MESSAGE(" Modifying %s",myClassName.c_str());

		while( attrMods[i] )
		{
			if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_REMOVEDELAY) == 0)
			{
				if( attrMods[i]->modAttr.attrValuesNum != 0 )
				{
					int removeDelay;
					removeDelay = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
					if(dblock == 0)
					{
						dblock = new aes_afp_datablock;
					}
					AES_AFP_TRACE_MESSAGE("Modifying %s to %d ",TQ_REMOVEDELAY,removeDelay);
					dblock->setRemoveDelay(removeDelay);
					changeRemoveDelay = true;

				}

			}
			else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_DEFSTAT) == 0)
			{
				if( attrMods[i]->modAttr.attrValuesNum != 0 )
				{
					int defaultStatus;
					defaultStatus = *reinterpret_cast<int *>(attrMods[i]->modAttr.attrValues[0]);
					if(dblock == 0)
					{
						dblock = new aes_afp_datablock;
					}
					AES_AFP_TRACE_MESSAGE("Modifying %s to %d ",TQ_DEFSTAT,defaultStatus);
					dblock->setStatus((AES_GCC_Filestates)defaultStatus);
					changeDefaultStatus = true;

				}
			}
			else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName,TQ_RENAMETEMPLATE) == 0)
			{

				if( attrMods[i]->modAttr.attrValuesNum != 0 )
				{
					ACE_OS::strcpy(tmpstr,reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					renameTemplate=string(tmpstr);
				}
				else
				{
					AES_AFP_TRACE_MESSAGE( "Modification, NO %s ",TQ_RENAMETEMPLATE);
				}
				if(dblock == 0)
				{
					dblock = new aes_afp_datablock;
				}
				AES_AFP_TRACE_MESSAGE("Modifying %s to %s ",TQ_RENAMETEMPLATE,renameTemplate.c_str());
				dblock->setFileTemplate(renameTemplate);
				dblock->setFileRename(AES_LOCAL);
				changeRenameTemplate = true;

			}
			else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_NAMETAG1) == 0)
			{
				if( attrMods[i]->modAttr.attrValuesNum != 0 )
				{
					ACE_OS::strcpy(tmpstr , reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					nameTag1=string(tmpstr);
				}
				else
				{
					AES_AFP_TRACE_MESSAGE("Modification, NO %s ",TQ_NAMETAG1);
				}
				AES_AFP_TRACE_MESSAGE("Modifying %s to %s ",TQ_NAMETAG1,nameTag1.c_str());
				changeNameTag1 = true;
				//******************
				if(dblock == 0)
				{
					dblock = new aes_afp_datablock;
				}
				nameTag = dblock->getNameTag();

				if(nameTag.empty())
				{
					dblock->setNameTag(nameTag1);
				}
				else
				{
					nameTag1 += "*";
					nameTag1 += nameTag;
					dblock->setNameTag(nameTag1);
				}

			}
			else if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_NAMETAG2) == 0)
			{
				if( attrMods[i]->modAttr.attrValuesNum != 0 )
				{
					ACE_OS::strcpy(tmpstr , reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]));
					nameTag2=string(tmpstr);
				}
				else
				{
					AES_AFP_TRACE_MESSAGE("Modification, NO %s ",TQ_NAMETAG2);
				}
				changeNameTag2 = true;
				AES_AFP_TRACE_MESSAGE("Modifying %s to %s ",TQ_NAMETAG2,nameTag2.c_str());
				//*******************
				if(dblock == 0)
				{
					dblock = new aes_afp_datablock;
				}
				nameTag =  dblock->getNameTag();

				if(!nameTag.empty())
				{
					nameTag += "*";
				}
				nameTag += nameTag2;
				dblock->setNameTag(nameTag);

			}
			else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName,TQ_NUMOFRETRIES) == 0)
			{
				if(attrMods[i]->modAttr.attrValuesNum !=0)
				{
					int retryTimes = *reinterpret_cast<int*>(attrMods[i]->modAttr.attrValues[0]);
					AES_AFP_TRACE_MESSAGE("Modifying numberOfRetryOnFailure to %d ",retryTimes);
					if(dblock == 0)
					{
						dblock = new aes_afp_datablock;
					}
					dblock->setSendRetries(retryTimes);
					changeSendRetries = true;
				}
			}
			else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName,TQ_RETRYINTERVAL) == 0)
			{
				if(attrMods[i]->modAttr.attrValuesNum !=0)
				{
					int retryInterval = *reinterpret_cast<int*>(attrMods[i]->modAttr.attrValues[0]);
					AES_AFP_TRACE_MESSAGE("Modifying retryInterval to %d ",retryInterval);
					if(dblock == 0)
					{
						dblock = new aes_afp_datablock;
					}
					dblock->setRetriesDelay(retryInterval);
					changeRetryInterval = true;
				}
			}
			//Start - HT50930
			else if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName,TQ_SEQNO) == 0)
			{
				if(attrMods[i]->modAttr.attrValuesNum !=0)
				{
					int startupSequenceNumber = *reinterpret_cast<int*>(attrMods[i]->modAttr.attrValues[0]);
					AES_AFP_TRACE_MESSAGE("Modifying startupSequenceNumber to %d ",startupSequenceNumber);
					if(dblock == 0)
					{
						dblock = new aes_afp_datablock;
					}
					dblock->setStartupSequenceNumber(startupSequenceNumber);
					changeStartupSequenceNumber = true;
				}
			}
			//End - HT50930
			++i;

		}

		if(dblock != 0)
		{
			dblock->setTransferQueue(transferQueue);
		}
		setErrorText(AES_NOERRORCODE);
		return ACS_CC_SUCCESS;
	}
	else if (ACE_OS::strcasecmp(myClassName.c_str(),TQ_CL_NAME ) == 0 )
	{
		AES_AFP_TRACE_MESSAGE("Modifying attributes of class %s ",TQ_CL_NAME);
		abortMX_.acquire();
		if(!AES_AFP_CommandHandler::m_commit)
		{
			AES_AFP_CommandHandler::m_commit=true;
			AES_AFP_CommandHandler::m_tqcommit.assign(transferQueue);
			AES_AFP_TRACE_MESSAGE("MODIFY m_commit= %d, tq_commit =%s  ",AES_AFP_CommandHandler::m_commit,transferQueue.c_str());
		}
		else if(AES_AFP_CommandHandler::m_commit && AES_AFP_CommandHandler::m_tqcommit.compare(transferQueue))
		{
			AES_AFP_CommandHandler::m_commit=false;
			AES_AFP_CommandHandler::m_tqcommit.clear();
			AES_AFP_TRACE_MESSAGE(" MODIFY m_commit= %d, tq_commit =%s  ",AES_AFP_CommandHandler::m_commit,transferQueue.c_str());
			setErrorText(AES_COMMITERROR);
			abortMX_.release();
			return ACS_CC_FAILURE;
		}
		abortMX_.release();
		ACS_APGCC_CcbHandle ccbHandleVal;
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal;

		ACE_UINT8 p = 0;

		unsigned int result;
		std::vector<std::string> currentDestsetList;
		std::vector<std::string> modifiedDestsetList;
		ACE_INT8 numDestsetModified(0),noOfInitDestSet(0),noOfRespDestSet(0);
		bool isDestSetAdded=false;
		std::string fileDest("");
		std::string Destst("");
		std::string parent = AES_GCC_Util::dnOfFileTransferM;

		ACS_CC_ImmParameter paramToFind;
		paramToFind.attrName = new char [30];

		unsigned int q = 	(attrMods[p]->modAttr.attrValuesNum);

		if(ACE_OS::strcmp(attrMods[p]->modAttr.attrName, TQ_FILETQINITDEST) == 0)
		{
			if(attrMods[p]->modAttr.attrValuesNum != 0)
			{				
				char str1[50];
				ACE_OS::strcpy(paramToFind.attrName, TQ_FILETQINITDESTID);

				ACE_OS::strcpy(str1, reinterpret_cast<char *>(attrMods[p]->modAttr.attrValues[q - 1]));
				Destst = str1;

				result = api_.validateDestinationSet(Destst);

				if(result != AES_NOERRORCODE)
				{
					if(result == AES_INVALIDDESTNAME)
					{
						result = AES_UNREAS_IDESTSETNAME;
						setErrorText(result, objName);
					}
					else
					{
						setErrorText(result);
					}
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination validation failed %s ", str1);
					AES_AFP_TRACE_MESSAGE("Destination validation failed %s ", str1);
					delete[] paramToFind.attrName;
					paramToFind.attrName = 0;
					return ACS_CC_FAILURE;
				}

				fileDest = TQ_FILETQINITDESTID;
				fileDest += "=";
				fileDest += str1;
				fileDest += ",";
				fileDest += parent;


				int result1 = omHandler.getAttribute(fileDest.c_str(), &paramToFind );
				if ( result1 == ACS_CC_FAILURE)
				{
					/*! @ToDo add this error code into error code list */
					setErrorText(AES_NOIDESTINATION);
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Initiating Destination Set = %s does not exist", str1);
					AES_AFP_TRACE_MESSAGE("Initiating Destination Set = %s does not exist", str1);
					delete[] paramToFind.attrName;
					paramToFind.attrName = 0;
					return ACS_CC_FAILURE;
				}
			}

		}
		else if( ACE_OS::strcmp(attrMods[p]->modAttr.attrName, TQ_FILETQRESPDEST) == 0)
		{
			if(attrMods[p]->modAttr.attrValuesNum != 0)
			{				
				char str1[50];
				ACE_OS::strcpy(str1, reinterpret_cast<char *>(attrMods[p]->modAttr.attrValues[q -1]));

				Destst = str1;


				ACE_OS::strcpy(paramToFind.attrName, TQ_FILETQRESPDESTID);
				result = api_.validateDestinationSet(Destst);

				if(result != AES_NOERRORCODE)
				{
					if(result == AES_INVALIDDESTNAME)
					{
						result = AES_UNREAS_RDESTSETNAME;
						setErrorText(result, objName);
					}
					else
					{
						setErrorText(result);
					}
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination validation failed %s ", str1);
					AES_AFP_TRACE_MESSAGE("Destination validation failed %s ", str1);
					delete[] paramToFind.attrName;
					paramToFind.attrName = 0;
					return ACS_CC_FAILURE;
				}
				fileDest = TQ_FILETQRESPDESTID;
				fileDest += "=";
				fileDest += str1;
				fileDest += ",";
				fileDest += parent;

				int result1 = omHandler.getAttribute(fileDest.c_str(), &paramToFind );
				if ( result1 == ACS_CC_FAILURE)
				{
					/*! @ToDo add this error code into error code list */
					setErrorText(AES_NORDESTINATION);
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Responding Destination Set = %s does not exist", str1);
					AES_AFP_TRACE_MESSAGE("Responding Destination Set = %s does not exist", str1);
					delete[] paramToFind.attrName;
					paramToFind.attrName = 0;
					return ACS_CC_FAILURE;
				}
			}

		}

		ACS_CC_ImmParameter paramToFind1;
		paramToFind1.attrName = new char [30];
		ACE_OS::strcpy(paramToFind1.attrName, TQ_FILETQINITDEST);
		if(omHandler.getAttribute(objName, &paramToFind1) == ACS_CC_SUCCESS)
		{
			noOfInitDestSet= paramToFind1.attrValuesNum;
			AES_AFP_TRACE_MESSAGE("Number of initiating destsets in current TQ : %d",noOfInitDestSet);
			if ((noOfInitDestSet == 1) && (ACE_OS::strcmp(reinterpret_cast<char *>(paramToFind1.attrValues[0]), "-") == 0))
			{
				AES_AFP_TRACE_MESSAGE("number of attrValues = %d",attrMods[i]->modAttr.attrValuesNum);
				if(attrMods[i]->modAttr.attrValuesNum == 0)
				{
					if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQINITDEST) == 0)
						setErrorText(AES_ILLEGALREMIDESTSET, objName);
					else
						setErrorText(AES_NORESPDESTSETEXIST, objName);

					//setErrorText(AES_DETACHFAILED, objName);
					AES_AFP_TRACE_MESSAGE("Removal of association between Transfer queue and last destination set is denied");	
				}
				if(attrMods[i]->modAttr.attrValuesNum != 0)
				{
					if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQINITDEST) == 0)
						setErrorText(AES_ILLITQEMPTYDESTSET);
					else	
						setErrorText(AES_ILLRTQEMPTYDESTSET);
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Illegal association of Destination set to the TransferQueue associated with Empty destination");
					AES_AFP_TRACE_MESSAGE("Illegal association of Destination set to the TransferQueue associated with Empty destination");	
				}
				delete[] paramToFind1.attrName;
				paramToFind1.attrName = 0;
				return ACS_CC_FAILURE;
			}
			paramToFind1.attrValuesNum=0;
		}

		ACE_OS::strcpy(paramToFind1.attrName, TQ_FILETQRESPDEST);
		if(omHandler.getAttribute(objName, &paramToFind1) == ACS_CC_SUCCESS)
		{
			noOfRespDestSet= paramToFind1.attrValuesNum;
			AES_AFP_TRACE_MESSAGE("Number of responding destsets in current TQ : %d",noOfRespDestSet);
			if ((noOfRespDestSet == 1) && (ACE_OS::strcmp(reinterpret_cast<char *>(paramToFind1.attrValues[0]), "-") == 0))
			{
				AES_AFP_TRACE_MESSAGE("number of attrValues = %d",attrMods[i]->modAttr.attrValuesNum);
				if(attrMods[i]->modAttr.attrValuesNum == 0)
				{

					if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQINITDEST) == 0)
						setErrorText(AES_NOINITDESTSETEXIST, objName);
					else
						setErrorText(AES_ILLEGALREMRDESTSET, objName);

					//setErrorText(AES_DETACHFAILED , objName);
					AES_AFP_TRACE_MESSAGE("Removal of association between Transfer queue and last destination set is denied");
				}
				if(attrMods[i]->modAttr.attrValuesNum != 0)
				{
					if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQINITDEST) == 0)
						setErrorText(AES_ILLITQEMPTYDESTSET);
					else	
						setErrorText(AES_ILLRTQEMPTYDESTSET);

					AES_AFP_LOG(LOG_LEVEL_ERROR,"Illegal association of Destination set to the TransferQueue associated with Empty destination");
					AES_AFP_TRACE_MESSAGE("Illegal association of Destination set to the TransferQueue associated with Empty destination");
				}
				delete[] paramToFind1.attrName;
				paramToFind1.attrName = 0;
				return ACS_CC_FAILURE;
			}
			paramToFind1.attrValuesNum=0;
		}
		delete[] paramToFind1.attrName;
		paramToFind1.attrName=0;
		ACS_CC_ReturnType retVal = getCcbAugmentationInitialize(oiHandle, ccbId, &ccbHandleVal, &adminOwnwrHandlerVal );

		if (retVal != ACS_CC_SUCCESS)
		{
			setErrorText(AES_CATASTROPHIC);
			AES_AFP_LOG (LOG_LEVEL_ERROR, "  getCcbAugmentationInitialize::ACS_CC_FAILURE in modification");
			AES_AFP_TRACE_MESSAGE("  getCcbAugmentationInitialize::ACS_CC_FAILURE in modification");
			return ACS_CC_FAILURE;

		}


		while( attrMods[i] )
		{
			if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQINITDEST) == 0 || ACE_OS::strcmp(attrMods[i]->modAttr.attrName,TQ_FILETQRESPDEST) == 0)
			{
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQINITDEST) == 0)
				{
					dType = INIT;
				}
				if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQRESPDEST) == 0)
				{
					dType = RESP;
				}

				/*get the value of "fileDestinationGroup" from IMM for this object*/
				ACS_CC_ImmParameter paramToFind;
				paramToFind.attrName = new char [30];
				ACE_OS::strcpy(paramToFind.attrName, attrMods[i]->modAttr.attrName);
				AES_AFP_TRACE_MESSAGE("Mofify attribute %s ",paramToFind.attrName);
				if(isDestSetAdded)
				{
					setErrorText(AES_MULTIDESTSET, objName);
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set cannot be set with multiple values at a time");
					AES_AFP_TRACE_MESSAGE("Destination set cannot be set with multiple values at a time");
					delete[] paramToFind.attrName;
					paramToFind.attrName = 0;
					return ACS_CC_FAILURE;
				}

				if(omHandler.getAttribute(objName, &paramToFind) == ACS_CC_SUCCESS)
				{
					numDestsetModified = paramToFind.attrValuesNum - attrMods[i]->modAttr.attrValuesNum;
					AES_AFP_TRACE_MESSAGE("!!Number of attr values in current TQ(from IMM) %d ",paramToFind.attrValuesNum);
					AES_AFP_TRACE_MESSAGE("!!Number of attr values in modified TQ %d ",attrMods[i]->modAttr.attrValuesNum);
					AES_AFP_TRACE_MESSAGE("!!Number of destsets modified %d ",numDestsetModified);
					if( abs(numDestsetModified) > 1) /*more than one destset added/deleted to TQ*/
					{
						setErrorText(AES_MULTIDESTSET, objName);
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set cannot be set with multiple values at a time");
						AES_AFP_TRACE_MESSAGE("Destination set cannot be set with multiple values at a time");
						delete[] paramToFind.attrName;
						return ACS_CC_FAILURE;
					}
					if((numDestsetModified == 0) && (paramToFind.attrValuesNum == 0) && (attrMods[i]->modAttr.attrValuesNum == 0))
					{
						if(ACE_OS::strcmp(attrMods[i]->modAttr.attrName, TQ_FILETQINITDEST) == 0)
							setErrorText(AES_NOINITDESTSETEXIST, objName);
						else
							setErrorText(AES_NORESPDESTSETEXIST, objName);
						delete[] paramToFind.attrName;
						paramToFind.attrName = 0;
						return ACS_CC_FAILURE;
					}
					if (numDestsetModified == 0) /*one or more destset replaced(overwritten) with a new value (no destset addede/deleted) */
					{
						setErrorText(AES_REPLACEDESTSET, objName);
						delete[] paramToFind.attrName;
						paramToFind.attrName = 0;
						return ACS_CC_FAILURE;
					}


					//get the list of associated destination sets, currently in IMM, in a vector
					currentDestsetList.clear();
					for(unsigned int k=0; k < paramToFind.attrValuesNum; ++k)
					{
						currentDestsetList.push_back(reinterpret_cast<char *>(paramToFind.attrValues[k]));
					}
					sort(currentDestsetList.begin(), currentDestsetList.end());

					//get the list of modified destset in a vector
					modifiedDestsetList.clear();
					for(unsigned int k=0; k < attrMods[i]->modAttr.attrValuesNum; ++k)
					{
						modifiedDestsetList.push_back(reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[k]));
					}
					sort(modifiedDestsetList.begin(), modifiedDestsetList.end());
					isDestSetAdded=true;
					delete[] paramToFind.attrName;//delete memory in case of success
				}
				else
				{
					setErrorText(AES_CATASTROPHIC); //object not exists in the imm
					AES_AFP_LOG(LOG_LEVEL_ERROR,"get Attribute failed for %s ",objName);
					AES_AFP_TRACE_MESSAGE("get Attribute failed for %s ",objName);
					delete[] paramToFind.attrName;
					paramToFind.attrName = 0;
					return ACS_CC_FAILURE;
				}
				/*Updating RO reference attributes using augmented modify */
				if(retVal == ACS_CC_SUCCESS)
				{
					retVal=updateRefAttibutes(ccbHandleVal,adminOwnwrHandlerVal,string(objName),string(attrMods[i]->modAttr.attrName), modifiedDestsetList);
				}
				if( retVal != ACS_CC_SUCCESS)
				{
					setErrorText(AES_CATASTROPHIC);
					AES_AFP_LOG (LOG_LEVEL_ERROR, "  updateRefAttibutes failed-MODIFY ");
					AES_AFP_TRACE_MESSAGE("  updateRefAttibutes failed-MODIFY ");
					return ACS_CC_FAILURE;
				}
				else
				{
					retVal = applyAugmentCcb (ccbHandleVal);
					if(retVal !=ACS_CC_SUCCESS)
					{
						setErrorText(AES_CATASTROPHIC);
						AES_AFP_LOG (LOG_LEVEL_INFO, "  applyAugmentCcb failed-MODIFY ");
						AES_AFP_TRACE_MESSAGE("  applyAugmentCcb failed-MODIFY ");
						return ACS_CC_FAILURE;
					}
				}
			}
			i++;
		}

		string destsetModified;
		destsetModified.clear();
		//delete one destset from a TQ
		if( numDestsetModified == 1) /*delete one destsets from TQ*/
		{
			if(noOfInitDestSet+noOfRespDestSet == 1)
			{
				AES_AFP_TRACE_MESSAGE("TQ associated with only one destination set");
				if(noOfInitDestSet == 0)
				{
					setErrorText(AES_ILLEGALREMRDESTSET , objName);
				}
				else if(noOfRespDestSet == 0)	
				{
					setErrorText(AES_ILLEGALREMIDESTSET , objName);
				}
				//	setErrorText(AES_DETACHFAILED, objName);
				return ACS_CC_FAILURE;
			}
			/*verify no destset of original list got replaced with a new value*/
			unsigned int currCnt, modCnt;
			bool mismatch = false;
			for(currCnt=0, modCnt=0; modCnt < modifiedDestsetList.size(); )
			{
				if (currentDestsetList[currCnt] != modifiedDestsetList[modCnt])
				{
					if (mismatch == true) /*more than one mismatch*/
					{
						setErrorText(AES_MULTIDESTSET, objName);
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set cannot be set with multiple values at a time");
						AES_AFP_TRACE_MESSAGE("Destination set cannot be set with multiple values at a time");
						return ACS_CC_FAILURE;
					}
					destsetModified = currentDestsetList[currCnt];
					++currCnt;
					mismatch = true;
				}
				else
				{
					++modCnt; ++currCnt;
				}

			}
			if (destsetModified.empty()) /*last destset of currentDestsetList is deleted*/
			{
				destsetModified = currentDestsetList[currCnt];
			}
			AES_AFP_TRACE_MESSAGE("DELETE: modified destset = %s ",destsetModified.c_str());
			if(ACE_OS::strcmp(destsetModified.c_str(),"-") == 0)
			{
				if(dType == INIT)
					setErrorText(AES_IEMPTYDESTSET, objName);
				else if(dType == RESP)
					setErrorText(AES_REMPTYDESTSET, objName);
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set(-) cannot be added/replaced/Deleted");
				AES_AFP_TRACE_MESSAGE("Destination set(-) cannot be added/replaced/Deleted");
				return ACS_CC_FAILURE;
			}
			/*call business logic of delete one destset from TQ*/
			string destsetName_(""), tqName_(""), tqDn_("");
			tqDn_ = string(objName);
			extractNameFromDn(tqDn_,tqName_);
			destsetName_=destsetModified;
			api_.removeDestinationSet(tqName_, destsetName_);
			aes_afp_datablock db;
			db.setTransferQueue(tqName_);
			db.setDestinationSet(destsetName_);

			//get all attributes of TQ for rollback before deletion
			aes_afp_datablock *db2 = 0;
			AES_GCC_Errorcodes errorCode2;
			db2 = dataThrd->handle_getTransferQueueAttributesToModify(&db,errorCode2);
			AES_AFP_TRACE_MESSAGE("getTransferQueueattributesToModify///errorCode2 = %d ",errorCode2);
			if(errorCode2 != AES_NOERRORCODE)
			{
				setErrorText(errorCode2, objName);
				if (db2 != 0)
				{
					delete db2;
					db2 = 0;
				}
				return ACS_CC_FAILURE;
			}

			if( NULL == db2 )
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Creation of object db2 FAILED");
				AES_AFP_TRACE_MESSAGE("Creation of object db2 FAILED");
				return ACS_CC_FAILURE;
			}
			AES_AFP_TRACE_MESSAGE("TQ = %s\nDestSet = %s\nStatus = %d\nRemoveDelay = %d\nRenameTemplate = %s\nSourceDirectory = %s\nNameTag = %s\nStartupSeqNo = %ld", db2->getTransferQueue().c_str(), db2->getDestinationSet().c_str(), db2->getStatus(), db2->getRemoveDelay(), db2->getFileTemplate().c_str(), db2->getSourceDirectory().c_str(), db2->getNameTag().c_str(), db2->getStartupSequenceNumber());
			AES_GCC_Errorcodes ret;
			if(!dataThrd->handle_removeDestinationSet_OI(&db, ret) )
			{
				setErrorText(ret, objName);
				AES_AFP_LOG (LOG_LEVEL_ERROR,"Remove TQ DestSet association FAILED");
				AES_AFP_TRACE_MESSAGE("Remove TQ DestSet association FAILED");
				if ( 0 != db2 )
				{
					delete db2;
					db2 = 0;
				}
				return ACS_CC_FAILURE;
			}
			else
			{
				abortMX_.acquire();
				prevState = new (std::nothrow) CCBPrevState;
				if (prevState == 0)
				{
					AES_AFP_LOG (LOG_LEVEL_FATAL,"Modify: mem allocation failed for prevState");
					AES_AFP_TRACE_MESSAGE("Modify: mem allocation failed for prevState");
					abortMX_.release();
					setErrorText(AES_CATASTROPHIC);
					if ( 0 != db2 )
					{
						delete db2;
						db2 = 0;
					}
					return ACS_CC_FAILURE;
				}
				else
				{	
					prevState->prev_db[0] = *db2;
					prevState->dType[0] = dType;
					prevState->undoAction = IMMC_CREATE_TQ;
				}
				abortMX_.release();
			}
			if (0 != db2)
			{
				delete db2;
				db2 = 0;
			}
			AES_AFP_TRACE_MESSAGE("Remove TQ DestSet association SUCCESS");
			setErrorText(AES_NOERRORCODE);
			return ACS_CC_SUCCESS;
		}
		else if( numDestsetModified == -1) /*add one destsets to TQ*/
		{

			AES_AFP_TRACE_MESSAGE("### Adding new dest set to the current Transfer queue");
			/*verify no destset of original list got replaced with a new value*/
			bool mismatch = false;
			int numOfExistingDestSet=noOfInitDestSet+noOfRespDestSet;
			if(numOfExistingDestSet < 3)
			{ 

				unsigned int currCnt, modCnt;
				for(currCnt=0, modCnt=0; currCnt < currentDestsetList.size(); )
				{
					if (currentDestsetList[currCnt] != modifiedDestsetList[modCnt])
					{
						if (mismatch == true) /*more than one mismatch*/
						{
							setErrorText(AES_MULTIDESTSET, objName);
							AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set cannot be set with multiple values at a time");
							AES_AFP_TRACE_MESSAGE("Destination set cannot be set with multiple values at a time");
							return ACS_CC_FAILURE;
						}
						destsetModified = modifiedDestsetList[modCnt];
						++modCnt;
						mismatch = true;
					}
					else
					{
						++modCnt; ++currCnt;
					}

				}
				if (destsetModified.empty()) /*last destset of modifiedDestsetList is added*/
				{
					destsetModified = modifiedDestsetList[modCnt];
				}
				AES_AFP_TRACE_MESSAGE("ADD: modified destset = %s ",destsetModified.c_str());
				if(ACE_OS::strcmp(destsetModified.c_str(),"-") == 0)
				{
					if(dType == INIT)
						setErrorText(AES_IEMPTYDESTSET, objName);
					else if(dType == RESP)
						setErrorText(AES_REMPTYDESTSET, objName);
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Destination set(-) cannot be Added/Replaced/Deleted");
					AES_AFP_TRACE_MESSAGE("Destination set(-) cannot be Added/Replaced/Deleted");
					return ACS_CC_FAILURE;
				}
				/*call business logic to add one destset to a TQ*/
				string destsetName_(""), tqName_(""), tqDn_("");
				tqDn_ = string(objName);
				extractNameFromDn(tqDn_,tqName_);
				destsetName_=destsetModified;
				aes_afp_datablock db;
				aes_afp_datablock *db1 = 0;
				db.setTransferQueue(tqName_);
				AES_GCC_Errorcodes errorCode  = AES_NOERRORCODE;
				AES_GCC_Errorcodes errorCodeF = AES_NOERRORCODE;

				db1 = dataThrd->handle_getTransferQueueAttributesToModify(&db,errorCode);
				AES_AFP_TRACE_MESSAGE("getTransferQueueattributes///errorCode = %d ",errorCode);
				if(errorCode!=AES_NOERRORCODE)
				{

					setErrorText(errorCode, objName);
					AES_AFP_TRACE_MESSAGE("handle_getTransferQueueAttributes FAILED, errorCode : %d",errorCode);
					if (db1 != 0)
					{
						delete db1;
						db1 = 0;
					}
					return ACS_CC_FAILURE;
				}
				if( db1 != 0 )
				{
					db1->setDestinationSet(destsetName_);
					if(!dataThrd->handle_createTransferQueue(db1,errorCode,dType))
					{
						if(errorCode == AES_NODESTINATION) // errorcode = 22
						{
							if(dType == INIT)
								errorCodeF = AES_UNREAS_IDESTSETNAME;
							else if(dType == RESP)
								errorCodeF = AES_UNREAS_RDESTSETNAME;
							setErrorText(errorCodeF, objName);
						}
						if(errorCode == AES_PROCORDEREXIST) // errorCode = 10
						{
							if(dType == INIT)
								errorCodeF = AES_IDESTSETALREADYATT;
							else if(dType == RESP)
								errorCodeF = AES_RDESTSETALREADYATT;
							setErrorText(errorCodeF);
						}
						else	
							setErrorText(errorCode, objName);

						AES_AFP_TRACE_MESSAGE("Create FAILED");
						if (db1 != 0)
						{
							delete db1;
							db1 = 0;
						}
						return ACS_CC_FAILURE;
					}
					else   //saving prev state for intermediate abort rollback
					{
						abortMX_.acquire();
						if (prevState == 0)
						{
							prevState = new (std::nothrow) CCBPrevState;
						}
						prevState->prev_db[0] = *db1;
						prevState->undoAction = IMMC_REMOVE_DESTSET;
						abortMX_.release();
					}

				}
				if( db1 != 0 )
				{
					delete db1;
					db1 = 0;
				}
			}
			else
			{
				setErrorText(AES_MAXDESTSETLIMIT);
				return ACS_CC_FAILURE;
			}
			AES_AFP_TRACE_MESSAGE("Adding DestSet to a TQ SUCCESS");
			return ACS_CC_SUCCESS;
		}
	}
	setErrorText(1);
	AES_AFP_TRACE_MESSAGE("No modification done. Exiting with failure ");

	return ACS_CC_FAILURE;
}

ACS_CC_ReturnType AES_AFP_CommandHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	AES_AFP_TRACE_MESSAGE("-------------------------------------------------");
	AES_AFP_TRACE_MESSAGE("-----            complete call back         -----");
	AES_AFP_TRACE_MESSAGE("-------------------------------------------------");
	AES_AFP_LOG(LOG_LEVEL_INFO, "complete callback received");
	AES_GCC_Errorcodes code;
	int k =0;
	int a =0;
	abortMX_.acquire();
	AES_AFP_CommandHandler::m_commit=false;
	AES_AFP_CommandHandler::m_tqcommit.clear();

	if(theClassName == TQ_CL_FILETQ && aes_afp_services::m_db != 0 )
	{
		if(m_createFileTQCcbId == ccbId)
		{
			AES_AFP_TRACE_MESSAGE(" In complete total destinations associated to TQ = %d", aes_afp_services::m_db->m_nCount);

			for(k = 0 ; k < aes_afp_services::m_db->m_nCount ; k++)
			{
				aes_afp_services::m_db->setDestinationSet(aes_afp_services::m_db->dest[k]);
				if(!dataThrd->handle_createTransferQueue((aes_afp_services::m_db),code,aes_afp_services::m_db->m_dType))
				{
					setErrorText(code );
					AES_AFP_TRACE_MESSAGE("Create FAILED");
					for(a = 0;a < k-1; a++)
					{
						aes_afp_services::m_db->setDestinationSet(aes_afp_services::m_db->dest[a]);
						if(!dataThrd->handle_removeSourceDirectoryFiles((aes_afp_services::m_db),code))
						{
							setErrorText(code);
							AES_AFP_LOG(LOG_LEVEL_ERROR, "Rollback failed for destination %s", aes_afp_services::m_db->dest[a].c_str());
							AES_AFP_TRACE_MESSAGE("Rollback failed for destination %s", aes_afp_services::m_db->dest[a].c_str());
						}
					}
					abortMX_.release();
					unsetflags();
					return ACS_CC_FAILURE;
				}
				else   //saving prev state for intermediate abort rollback
				{
					if (prevState == 0)
					{
						prevState = new (std::nothrow) CCBPrevState;
					}
					prevState->prev_db[k] = *aes_afp_services::m_db;
				}
				AES_AFP_TRACE_MESSAGE("CreateTransferQueue Success for %s ", aes_afp_services::m_db->dest[k].c_str());
			}
			if (prevState != 0)
			{
				prevState->size = aes_afp_services::m_db->m_nCount;
				prevState->undoAction = IMMC_REMOVE_TQ;
			}
			if(aes_afp_services::m_db != 0)
			{
				delete aes_afp_services::m_db;
				aes_afp_services::m_db = 0;
			}
		}
		if(isEmptyDestination == true)
		{
			isEmptyDestination = false;
		}
	}
	if(theClassName == TQ_CL_FILETQPARAMNAME && dblock != 0)
	{
		std::stringstream str;
		std::string destList, deststr;
		aes_afp_datablock *dbtmp = 0;
		AES_GCC_Errorcodes errorCode;
		int numberOfRetries,retryInterval;
		AES_GCC_Filestates oldDefaultStatus;
		dbtmp = dataThrd->handle_getTransferQueueAttributesToModify(dblock,errorCode);
		if(errorCode!=AES_NOERRORCODE)
		{
			setErrorText(errorCode, m_rdnName.c_str());
			AES_AFP_TRACE_MESSAGE("handle_getTransferQueueAttributes FAILED, errorCode : %d",errorCode);
			delete dblock;
			dblock = 0;
			if( dbtmp != 0 )
			{
				delete dbtmp;
				dbtmp = 0;
			}
			abortMX_.release();
			unsetflags();
			return ACS_CC_FAILURE;
		}
		else //saving prev state info for rollback
		{
			if ( prevState == 0 )
			{
				prevState = new (std::nothrow) CCBPrevState;
			}
			if (prevState == 0 || dbtmp == 0)
			{
				AES_AFP_TRACE_MESSAGE("Create: mem allocation failed for prevState");
			}
			else
			{
				prevState->prev_db[0] = *dbtmp;
				prevState->undoAction = IMMC_MODIFY_TQ;
			}
		}

		if( NULL == dbtmp )
		{
			abortMX_.release();
			unsetflags();
			AES_AFP_TRACE_MESSAGE("mem allocation failed for dbtmp");
			return ACS_CC_FAILURE;
		}

		AES_AFP_TRACE_MESSAGE( "Parameters before modification");
		AES_AFP_TRACE_MESSAGE ("transfer Queue: %s",dbtmp->getTransferQueue().c_str());
		AES_AFP_TRACE_MESSAGE ("nameTag: %s",(dbtmp->getNameTag()).c_str());
		AES_AFP_TRACE_MESSAGE ("renameTemplate: %s",(dbtmp->getFileTemplate()).c_str());
		AES_AFP_TRACE_MESSAGE ("removeDelay : %d",dbtmp->getRemoveDelay());
		AES_AFP_TRACE_MESSAGE ("numberOfRetryOnFailure : %d",dbtmp->getSendRetries());
		AES_AFP_TRACE_MESSAGE ("retryInterval : %d",dbtmp->getRetriesDelay());
		//Start - HT50930
		AES_AFP_TRACE_MESSAGE ("startupSequenceNumber : %ld",dbtmp->getStartupSequenceNumber());
		//End - HT50930
		oldDefaultStatus = dbtmp->getStatus();
		AES_AFP_TRACE_MESSAGE ("olddefaultStatus: %d",oldDefaultStatus);

		if (changeRenameTemplate == false )
		{
			dblock->setFileTemplate(dbtmp->getFileTemplate());
		}
		if ( changeNameTag1 == false )
		{
			if(changeNameTag2 == false)
			{
				dblock->setNameTag(dbtmp->getNameTag());
			}
			else
			{
				std::string buffer1,buffer2;
				std::string nameTag_temp = dbtmp->getNameTag();
				if(nameTag_temp != "")
				{
					int indexy = nameTag_temp.find_last_of('*');
					if (indexy != (ACE_INT32)nameTag_temp.npos)
					{
						buffer2 = nameTag_temp.substr(indexy+1,(strlen(nameTag_temp.c_str()))-1);
						buffer1 = nameTag_temp.substr(0, indexy);
					}
					else
					{
						buffer1 = nameTag_temp;
					}
					std::string buffer = buffer1 ;
					if(ACE_OS::strcmp(dblock->getNameTag().c_str(),"")!=0)
					{
						buffer += "*";
						buffer += dblock->getNameTag();
					}
					dblock->setNameTag(buffer);
				}
				else
				{
					AES_AFP_TRACE_MESSAGE("NameTag1 is not given");
					delete dblock;
					dblock = 0;
					if( dbtmp != 0 )
					{
						delete dbtmp;
						dbtmp = 0;
					}
					unsetflags();
					if (prevState != 0)
					{
						delete prevState;
						prevState = 0;
					}
					abortMX_.release();
					setErrorText(AES_NAMETAG1MANDATORY, m_rdnName.c_str());
					return ACS_CC_FAILURE;
				}
			}
		}
		else
		{
			if(changeNameTag2 == false)
			{
				std::string buffer1,buffer2;

				std::string nameTag_temp = dbtmp->getNameTag();
				if(nameTag_temp != "")
				{
					int indexy = nameTag_temp.find_last_of('*');
					if (indexy != (ACE_INT32)nameTag_temp.npos)
					{
						buffer2 = nameTag_temp.substr(indexy+1,(strlen(nameTag_temp.c_str()))-1);
						buffer1 = nameTag_temp.substr(0, indexy);
					}
					else
					{
						buffer1 = nameTag_temp;
					}
					std::string buffer = dblock->getNameTag() ;
					if(ACE_OS::strcmp(buffer2.c_str(),"")!=0)
					{
						buffer += "*";
						buffer += buffer2;
					}
					dblock->setNameTag(buffer);
				}
			}
		}

		if (changeRemoveBefore == false)
		{
			if(dbtmp->getRemoveBefore())
			{
				dblock->setRemoveBefore();
			}
			else
			{
				dblock->unsetRemoveBefore();
			}
		}
		if(changeRemoveDelay == false)
		{
			dblock->setRemoveDelay(dbtmp->getRemoveDelay());
		}
		if(changeDefaultStatus == false)
		{
			dblock->setStatus(oldDefaultStatus);
		}
		if(changeSendRetries == false)
		{
			numberOfRetries = dbtmp->getSendRetries();
			dblock->setSendRetries(numberOfRetries);
		}
		if(changeRetryInterval == false)
		{
			retryInterval = dbtmp->getRetriesDelay();
			dblock->setRetriesDelay(retryInterval);
		}
		//Start - HT50930
		if(changeStartupSequenceNumber == false)
		{
			dblock->setStartupSequenceNumber(dbtmp->getStartupSequenceNumber());
		}
		//End - HT50930

		AES_AFP_LOG(LOG_LEVEL_DEBUG, "Parameters after modification");
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"transfer Queue: %s",dblock->getTransferQueue().c_str());
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"nameTag: %s",(dblock->getNameTag()).c_str());
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"renameTemplate: %s",(dblock->getFileTemplate()).c_str());
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"defaultStatus: %d",dblock->getStatus());
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"removeDelay : %d",dblock->getRemoveDelay());
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"numberOfRetryOnFailure : %d",dblock->getSendRetries());
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"retryInterval : %d",dblock->getRetriesDelay());
		//Start - HT50930
		AES_AFP_LOG (LOG_LEVEL_DEBUG,"startupSequenceNumber : %ld",dblock->getStartupSequenceNumber());
		//End - HT50930
		int result = api_.validateRenameTemplate(dblock->getNameTag(),dblock->getFileTemplate());
		if(result != AES_NOERRORCODE)
		{
			setErrorText(result,  m_rdnName.c_str());
			AES_AFP_TRACE_MESSAGE("Validation FAILED");
			delete dblock;
			dblock = 0;
			if( dbtmp != 0 )
			{
				delete dbtmp;
				dbtmp = 0;
			}
			unsetflags();
			if (prevState != 0)
			{
				delete prevState;
				prevState = 0;
			}
			abortMX_.release();
			return ACS_CC_FAILURE;	
		}

		result = api_.validateTransferQueueParam(
				dblock->getNameTag(),
				dblock->getFileTemplate(),
				dblock->getStatus(),
				dblock->getRemoveDelay(),
				dblock->getRetriesDelay(),
				dblock->getSendRetries(),
				//Start - HT50930
				dblock->getStartupSequenceNumber()
				//End - HT50930
		);

		if(result != AES_NOERRORCODE)
		{
			setErrorText(result, m_rdnName.c_str());
			AES_AFP_TRACE_MESSAGE("Validation FAILED");
			if (dblock != 0)
			{
				delete dblock;
				dblock = 0;
			}
			if( dbtmp != 0 )
			{
				delete dbtmp;
				dbtmp = 0;
			}
			unsetflags();
			if (prevState != 0)
			{
				delete prevState;
				prevState = 0;
			}
			abortMX_.release();
			return ACS_CC_FAILURE;
		}
		if( dbtmp != 0 )
		{
			delete dbtmp;
			dbtmp = 0;
		}

		bool flag = dataThrd->handle_infoDestinationSetsForModify(dblock, destList,errorCode);
		if(!flag)
		{
			setErrorText(result, m_rdnName.c_str());
			AES_AFP_TRACE_MESSAGE("complete FAILED");
			if (dblock != 0)
			{
				delete dblock;
				dblock = 0;
			}
			unsetflags();
			if (prevState != 0)
			{
				delete prevState;
				prevState = 0;
			}
			abortMX_.release();
			return ACS_CC_FAILURE;
		}
		str << destList;
		str >> deststr;
		str >> deststr;
		int index = 0;
		while((atoi(deststr.c_str() ) != ENDOFSTREAM))
		{
			int result = api_.checkDefaultStatus(dblock->getStatus(),deststr);
			if (result != AES_NOERRORCODE)
			{
				setErrorText(result, m_rdnName.c_str());
				AES_AFP_TRACE_MESSAGE("Validation FAILED");
				if (dblock != 0)
				{
					delete dblock;
					dblock = 0;
				}
				abortMX_.release();
				unsetflags();
				return ACS_CC_FAILURE;
			}	
			dblock->setDestinationSet(deststr);
			bool templateChange = changeRenameTemplate||changeNameTag2||changeNameTag1||changeStartupSequenceNumber; //HT50930
			if(!dataThrd->handle_setTransferQueueAttributes((dblock),errorCode,templateChange))
			{
				setErrorText(errorCode, m_rdnName.c_str());
				AES_AFP_TRACE_MESSAGE("FAILED");
				delete dblock;
				dblock = 0;
				unsetflags();
				if (prevState != 0)
				{
					delete prevState;
					prevState = 0;
				}
				abortMX_.release();
				return ACS_CC_FAILURE;
			}
			else
			{
				prevState->prev_db[index] = prevState->prev_db[0];
				prevState->prev_db[index].setDestinationSet(deststr);
				++index; ++prevState->size;
			}
			AES_AFP_TRACE_MESSAGE( "destination af integer %d, ENDOFSTREAM = %d", atoi(deststr.c_str()),ENDOFSTREAM);
			AES_AFP_TRACE_MESSAGE("Destination: %s",deststr.c_str());
			str >> deststr;
		}
		if (dblock != 0)
		{
			delete dblock;
			dblock = 0;
		}
		abortMX_.release();
		unsetflags();
		setErrorText(AES_NOERRORCODE);
		return ACS_CC_SUCCESS;
	}
	unsetflags();
	abortMX_.release();
	setErrorText(AES_NOERRORCODE);
	return ACS_CC_SUCCESS;
}

void AES_AFP_CommandHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	AES_AFP_TRACE_FUNCTION;
	AES_AFP_LOG(LOG_LEVEL_INFO,"abort callback received");
	(void)oiHandle;
	(void)ccbId;

	if (!dataThrd->isDbInitialized())
	{
		AES_AFP_LOG(LOG_LEVEL_WARN, "abort request ignored: TQ database is not initialized yet");
		AES_AFP_TRACE_MESSAGE("WARNING: abort request ignored: TQ database is not initialized yet");
		return;
	}



	abortMX_.acquire();
	AES_AFP_CommandHandler::m_commit=false;
	AES_AFP_CommandHandler::m_tqcommit.clear();

	if (prevState != 0)
	{

		AES_GCC_Errorcodes errorCode;
		switch(prevState->undoAction)
		{
		case IMMC_REMOVE_TQ:
			for (int k = 0; k < prevState->size; ++k)
			{
				dataThrd->handle_removeTransferQueue(&(prevState->prev_db[k]),errorCode);
				AES_AFP_TRACE_MESSAGE("IMMC_REMOVE_TQ: errorcode = %d ", errorCode);
			}
			break;

		case IMMC_REMOVE_DESTSET:
			dataThrd->handle_removeDestinationSet_OI(&(prevState->prev_db[0]), errorCode);
			AES_AFP_TRACE_MESSAGE("IMMC_REMOVE_DESTSET: errorcode = %d ", errorCode);
			break;

		case IMMC_CREATE_TQ_REC:
			for (int k = 0; k < prevState->size; ++k)
			{
				dataThrd->handle_createTransferQueue(&(prevState->prev_db[k]), errorCode, prevState->dType[k]);
				AES_AFP_TRACE_MESSAGE("IMMC_REMOVE_TQ: errorcode = %d ", errorCode);
			}
			break;

		case IMMC_CREATE_TQ:
			dataThrd->handle_createTransferQueue(&(prevState->prev_db[0]),errorCode,prevState->dType[0]);
			AES_AFP_TRACE_MESSAGE("IMMC_CREATE_TQ: errorcode = %d ", errorCode);
			break;

		case IMMC_MODIFY_TQ:
			for (int k = 0; k < prevState->size; ++k)
			{
				dataThrd->handle_setTransferQueueAttributes(&(prevState->prev_db[k]),errorCode);
				AES_AFP_TRACE_MESSAGE("IMMC_MODIFY_TQ: errorcode = %d ", errorCode);
			}
			break;
		default:
			AES_AFP_TRACE_MESSAGE("default case");
		}
		AES_AFP_TRACE_MESSAGE("deleting prevState");
		delete prevState;
		prevState = 0;
	}
	abortMX_.release();
	if( (m_createFileTQCcbId == ccbId) || (m_createFileTQParamCcbId == ccbId))
	{
		if(aes_afp_services::m_db != 0)
		{
			delete aes_afp_services::m_db;
			aes_afp_services::m_db = 0;
		}
		if(isEmptyDestination == true)
		{
			isEmptyDestination = false;
		}

	}
	AES_AFP_TRACE_MESSAGE("Leaving");
}

void AES_AFP_CommandHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	AES_AFP_LOG(LOG_LEVEL_INFO,"apply callback received");
	if(m_createFileTQParamCcbId == ccbId)
	{
		if(aes_afp_services::m_db != 0)
		{
			delete aes_afp_services::m_db;
			aes_afp_services::m_db = 0;
		}
		if(isEmptyDestination == true)
		{
			isEmptyDestination = false;
		}
	}

	AES_AFP_TRACE_MESSAGE("Entering");
	(void)oiHandle;
	(void)ccbId;

	abortMX_.acquire();
	if (prevState != 0)
	{
		delete prevState;
		prevState = 0;
	}
	abortMX_.release();
}

ACS_CC_ReturnType AES_AFP_CommandHandler::updateRuntime(const char *objName, const char **attrName)
{
	(void)objName;
	(void)attrName;
	return ACS_CC_SUCCESS;
}


void AES_AFP_CommandHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{

	AES_AFP_TRACE_FUNCTION;
	AES_AFP_TRACE_MESSAGE("The admin op invoked on the dn is:%s",p_objName);
	AES_AFP_LOG(LOG_LEVEL_INFO,"admin callback received on the dn is:%s",p_objName);
	(void) paramList;

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

	bool myOIValidationError=true;
	ACE_UINT32  result;
	aes_afp_datablock db;
	ACS_CC_ImmParameter paramToFind;
	std::string entity, transferqueue,destinationSetName;
	std::string destination_(""), byWho;
	std::string rdn;
	AES_GCC_Errorcodes errorCode;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	ACS_APGCC_AdminOperationParamType secondElem;
	switch(operationId)
	{
	case 2: // manualTransferFailedEntitiesToAll
		AES_AFP_LOG(LOG_LEVEL_INFO,"manualTransferOfFailedEntities is called");
		AES_AFP_TRACE_MESSAGE("In Admin call back manualTransferFailedEntitiesToAll");
		rdn = string(p_objName);
		extractNameFromDn(rdn,transferqueue);
		AES_AFP_TRACE_MESSAGE("TransferQ name %s", transferqueue.c_str());
		result = api_.resendAllFailed(transferqueue);
		if (result)
		{
			//setErrorText(result);
			myOIValidationError = false;
			const char * ertxt1 = api_.getErrorCodeText(errorCode);
			char ertxt[500];
			strcpy(ertxt,"@ComNbi@");
			strcat(ertxt,ertxt1);
			char attName2[]= "errorText";
			secondElem.attrName = attName2;
			secondElem.attrType = ATTR_STRINGT;
			char* stringValue = const_cast<char*>(ertxt);
			secondElem.attrValues = reinterpret_cast<void*>(stringValue);

			// vectorOut.push_back(firstElem);
			vectorOut.push_back(secondElem);

			AES_AFP_TRACE_MESSAGE("Validation failed %d", myOIValidationError);

		}
		else
		{
			myOIValidationError = true;
			db.setTransferQueue(transferqueue);
			dataThrd->handle_resendAllFiles((&db),errorCode);
			if ( errorCode != AES_NOERRORCODE ) {
				myOIValidationError = false;
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
				AES_AFP_TRACE_MESSAGE("handle_resendAllFiles failed %s", ertxt1);
			}

		}
		break;
	default:
		AES_AFP_TRACE_MESSAGE( "In Admin call back invalid opId %d",(int)operationId);
		break;
	}

	ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError,vectorOut);
	if(rc != ACS_CC_SUCCESS)
	{
		AES_AFP_TRACE_MESSAGE("Admin op failed %u", myOIValidationError);
	}
	AES_AFP_TRACE_MESSAGE(" Admin op Success %u", myOIValidationError);

	/* Error Handling for APG/L */
	string errorText = api_.getErrorCodeText(myOIValidationError);
	AES_AFP_TRACE_MESSAGE(" Admin op ErrorCode %u", myOIValidationError);
	AES_AFP_TRACE_MESSAGE(" Admin op ErrorText %s", errorText.c_str());
	setExitCode(myOIValidationError, errorText); 
	/* End -Error Handling for APG/L */
}


void AES_AFP_CommandHandler::extractNameFromRdn( const string rdnName,
		string &name)
{
	name = "";

	string::size_type pos1 = string::npos;

	unsigned int pos2 = rdnName.length();

	pos1 = rdnName.find_first_of("=");

	if(( pos1 != string::npos) && pos1 < pos2 )
	{
		name = rdnName.substr(pos1+1, pos2-pos1-1);
	}

	AES_AFP_TRACE_MESSAGE( "Leaving");

}

void AES_AFP_CommandHandler::shutdown()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_LOG(LOG_LEVEL_INFO,"Shutdown is triggered for %s",theClassName.c_str());
	if (omHandler.Finalize() == ACS_CC_FAILURE)
	{
	}
	if( m_isClassImplAdded == true )
	{

		for (int i=0; i < 10; ++i)
		{ 
			if( oiHandler.removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE )
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Error occured while removing class implementer");
				AES_AFP_TRACE_MESSAGE( "Error occured while removing class implementer");
				//sleep for a 0.5 second   //HY75188
				ACE_Time_Value selectTime;
				selectTime.set(0,500000);
				while((ACE_OS::poll(0,0,selectTime) == -1) && errno == EINTR)
				{
					continue;
				}
			}
			else
			{
				m_isClassImplAdded = false;
				break;
			}
		}
	}
	m_poReactor->end_reactor_event_loop();

	AES_AFP_TRACE_MESSAGE( "Leaving");
}


void AES_AFP_CommandHandler::extractFileTQNameFromDn ( string dnName, string& name)
{
	AES_AFP_TRACE_MESSAGE( "Entering");

	name = "";

	unsigned int length = dnName.length();
	string::size_type pos1 = string::npos;

	string::size_type pos2 = string::npos;
	for(int g = 0 ; g < 2 ; g ++)
	{
		pos1 = dnName.find_first_of("=");
		pos2 = dnName.find_first_of(",");
		if(( pos1 != string::npos) && (pos2 != string::npos)&& pos1 < pos2 )
		{
			name = dnName.substr(pos1+1, pos2-pos1-1);
		}
		dnName = dnName.substr(pos2+1, length);
	}

	AES_AFP_TRACE_MESSAGE( "Leaving");
}
void AES_AFP_CommandHandler::extractNameFromDn (const string dnName, string& name)
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

	AES_AFP_TRACE_MESSAGE( "Leaving");
}


void AES_AFP_CommandHandler::extractClassNameFromRdn( const string myObjRdn,string &myClassName)
{
	AES_AFP_TRACE_MESSAGE("Entering");

	myClassName = "";
	string::size_type pos1 = string::npos;

	pos1 = myObjRdn.find_first_of("=");
	if ( pos1 != string::npos )
	{
		myClassName = myObjRdn.substr(0, pos1-2);
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
}



void AES_AFP_CommandHandler::extractParentNameFromRdn( const string myObjRdn,string &myParentName)
{
	AES_AFP_TRACE_MESSAGE("Entering");

	myParentName = "";
	string::size_type pos1 = string::npos;
	unsigned int length = myObjRdn.length();

	pos1 = myObjRdn.find_first_of(",");
	if ( pos1 != string::npos )
	{
		myParentName = myObjRdn.substr(pos1+1, length);
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
}

void AES_AFP_CommandHandler::unsetflags()
{
	changeDefaultStatus = false;
	changeRenameTemplate = false;
	changeNameTag1 = false;
	changeNameTag2 = false;
	changeRemoveBefore = false;
	changeRemoveDelay = false;
	changeSendRetries = false;
	changeRetryInterval = false;
	changeStartupSequenceNumber = false; //HT50930
}
void AES_AFP_CommandHandler::setErrorText(unsigned int code, const char *objName)
{
	std::string rdnName("");
	string errorMsg = string(api_.getErrorCodeText(code));
	AES_AFP_TRACE_MESSAGE ("setExitCode : ExitCode = %d, Error text = %s ",code,errorMsg.c_str());
	if(objName != 0)
	{
		AES_GCC_Util::extractComDnfromDn(objName ,rdnName);

		if( (code == AES_NAMETAGINVALID) || (code == AES_INVALIDFILETQ) || (code == AES_FILENAMEINVALID) || (code == AES_UNREAS_IDESTSETNAME) ||(code == AES_UNREAS_RDESTSETNAME) || (code == AES_NAMETAG1MANDATORY) || ( code == AES_TEMPLATEFAULT))
		{
			errorMsg = errorMsg + " in " + rdnName;
		}
		else if( (code == AES_NORESPDESTSETEXIST) || (code == AES_NOINITDESTSETEXIST) )
		{
			errorMsg = errorMsg + " to " + rdnName;
		}	
		else if(code == AES_ILLEGALDEFSTATUS)
		{
			errorMsg = errorMsg + " " + rdnName;
		}
		else
		{
			errorMsg = errorMsg + " for " + rdnName;
		}

	}

	setExitCode(code,errorMsg);
}
ACS_CC_ReturnType AES_AFP_CommandHandler::updateRefAttibutes(ACS_APGCC_CcbHandle ccbHandleVal,
		ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal,
		string objName,
		string attrName,
		vector<string> attrVal)

{

	ACS_CC_ReturnType retVal;
	ACS_CC_ImmParameter parToModify;
	int attrSize=attrVal.size();
	string parent = AES_GCC_Util::dnOfFileTransferM;
	AES_AFP_TRACE_MESSAGE ("Entering" );
	AES_AFP_TRACE_MESSAGE ("initDest Entry");
	string myDest;
	parToModify.attrValues=new void*[attrSize];
	parToModify.attrType = ATTR_NAMET;
	parToModify.attrValuesNum = attrSize;
	if(strcmp(attrName.c_str(),TQ_FILETQINITDEST)==0)
	{
		parToModify.attrName = (char *)TQ_FILETQINITDESTOBJ;
	}
	else
	{
		parToModify.attrName = (char *)TQ_FILETQRESPDESTOBJ;
	}
	for(int j=0; j<attrSize; ++j)
	{
		myDest.clear();
		AES_AFP_TRACE_MESSAGE ("initDest getAttribute");

		if(strcmp(attrName.c_str(),TQ_FILETQINITDEST)==0)
		{
			myDest = TQ_FILETQINITDESTID; 
		}
		else
		{
			myDest=TQ_FILETQRESPDESTID;
		}

		myDest += "=";
		myDest += (attrVal[j]).c_str();
		myDest += ",";
		myDest += parent;
		AES_AFP_TRACE_MESSAGE ("DestSet=%s , Attr =%s ,objName=%s ", myDest.c_str(),attrName.c_str(),objName.c_str());
		parToModify.attrValues[j] = new char[myDest.length()+1];
		strcpy( (char*)parToModify.attrValues[j], (char*)(myDest.c_str()) );
	}
	// Modify the object
	retVal = modifyObjectAugmentCcb(ccbHandleVal,
			adminOwnwrHandlerVal,
			objName.c_str(),
			&parToModify);

	if( retVal != ACS_CC_SUCCESS )
	{
		setErrorText(AES_CATASTROPHIC);
		AES_AFP_LOG (LOG_LEVEL_ERROR, " Error occured while modifying %s : %s", parToModify.attrName, getInternalLastErrorText());
		AES_AFP_TRACE_MESSAGE(" Error occured while modifying %s : %s", parToModify.attrName, getInternalLastErrorText());
		AES_AFP_TRACE_MESSAGE ("Leaving");
		/*delete allocated memory*/
		for(int j=0; j<attrSize; ++j)
		{
			delete[] (char*)parToModify.attrValues[j];
		}
		delete[] parToModify.attrValues;
		return ACS_CC_FAILURE;
	}
	/*delete allocated memory*/
	for(int j=0; j<attrSize; ++j)
	{
		delete[] (char*)parToModify.attrValues[j];
	}
	delete[] parToModify.attrValues;
	AES_AFP_TRACE_MESSAGE("initDest ref success");
	return ACS_CC_SUCCESS;
}
