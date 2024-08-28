/*=================================================================== */
/**
   @file aes_afp_transfer_entity.cpp

   This module contains the implementation for ObjectImplementer for TranferEntityInfo operations.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       18/11/2011     XTANAGG       Initial Release
*/

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

#if 0   //transfer entity removel from model

#include <aes_afp_transfer_entity.h>
#include <aes_afp_datatask.h>
#include <acs_apgcc_omhandler.h>
#include <aes_afp_command_handler.h>
#include <aes_afp_services.h>
#include <string>
#include <vector>
#include <iostream>
using namespace std;

GCC_TDEF(aes_afp_transferEntity);

aes_afp_transfer_entity::aes_afp_transfer_entity(aes_afp_datatask* dataThread)
{
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering aes_afp_transfer_entity"));
	dataThrd = dataThread;
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving aes_afp_transfer_entity"));
}

aes_afp_transfer_entity::~aes_afp_transfer_entity()
{
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering ~aes_afp_transfer_entity"));

	transferEntityMapType::iterator p;
	for( p = transferEntityMap.begin() ; p != transferEntityMap.end(); )
	{
		transferEntityMap.erase(p++);
	}
	transferEntityMap.clear();
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving ~aes_afp_transfer_entity"));
}

bool aes_afp_transfer_entity::initTransferEntityRunTimeOwner()
{
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering initTransferEntityRunTimeOwner"));

	if( init(AES_AFP_TRANSFERENTITYIMPLEMENTER) != ACS_CC_SUCCESS)
	{
		GCC_TERROR((aes_afp_transferEntity, "%s", "init of TransferEntityRunTimeOwner failed"));
		return false;
	}
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving initTransferEntityRunTimeOwner"));
	return true;
}

bool aes_afp_transfer_entity::finalizeTransferEntityRunTimeOwner()
{
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering finalizeTransferEntityRunTimeOwner"));
	if( finalize() != ACS_CC_SUCCESS )
	{
		GCC_TERROR((aes_afp_transferEntity, "%s", "finalizeTransferEntityRunTimeOwner failed."));
		return false;
	}
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving finalizeTransferEntityRunTimeOwner"));
	return true;
}

ACS_CC_ReturnType aes_afp_transfer_entity::updateCallback(const char* aOBJName, const char** aAttrName)
{
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering updateCallback"));

	(void) aAttrName;

	string transferQueue("");
	string destinationSet("");
	string fileName("");


	//Get the name of the Destination Set.
	if( getTQDestSetFileInfo( aOBJName ,
				  transferQueue,
			          destinationSet ,
				  fileName) == ACS_CC_FAILURE )
        {
		GCC_TERROR((aes_afp_transferEntity, "%s", "Unable to get getTQDestSetFileInfo" ));                 
		return ACS_CC_FAILURE ;
	
        }
	aes_afp_datablock dbBlock;

	//Set the transfer queue.
	dbBlock.setTransferQueue( transferQueue );

	//Set the destination set.
	dbBlock.setDestinationSet( destinationSet );

	//Set the file name
	dbBlock.setFilename ( fileName );

	if( dataThrd->handle_getFileInfo( &dbBlock ) == false )
	{
		GCC_TERROR((aes_afp_transferEntity, "%s", "Unable to get the file information from the transfer queue and destination set"));
		return ACS_CC_FAILURE;
	}
	
	//Got the file information successfully.
	//Print the values on the console.

	ACS_CC_ImmParameter parToModify;

	//Updating the File Status
	int myFileStatus = dbBlock.getStatus();
	parToModify.attrName = (ACE_TCHAR *)AES_AFP_FILE_TRANSMISSION_STATUS;
	parToModify.attrType = ATTR_INT32T;
	parToModify.attrValuesNum = 1;
	parToModify.attrValues=new void*[1];
	void *fileStatusValue = reinterpret_cast<void*>(&myFileStatus);
	parToModify.attrValues[0] = fileStatusValue;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		GCC_TERROR((aes_afp_transferEntity, "%s", "Modification of AES_AFP_FILE_STATUS Failed "));
		return ACS_CC_FAILURE;
	}

	//Updating the ISDIR Flag
	ACE_INT32 myIsDirFlag = (ACE_INT32)dbBlock.getDirectoryFlag();
	parToModify.attrName = (ACE_TCHAR *)AES_AFP_IS_DIR_FLAG;
	parToModify.attrType = ATTR_INT32T;
	parToModify.attrValuesNum = 1;
	parToModify.attrValues=new void*[1];
	void *isDirFlagValue = reinterpret_cast<void*>(&myIsDirFlag);
	parToModify.attrValues[0] = isDirFlagValue;

	if(this->modifyRuntimeObj(aOBJName, &parToModify) == ACS_CC_FAILURE)
	{
		GCC_TERROR((aes_afp_transferEntity, "%s", "Modification of AES_AFP_IS_DIR_FLAG Failed "));
		return ACS_CC_FAILURE;
	}

	GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving updateCallback"));
	return ACS_CC_SUCCESS;
}

void aes_afp_transfer_entity::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
						ACS_APGCC_InvocationType invocation,
						const char* p_objName,
						ACS_APGCC_AdminOperationIdType operationId,
						ACS_APGCC_AdminOperationParamType**paramList)
{
	(void) oiHandle;
	(void) invocation;
	(void) p_objName;
	(void) operationId;
	(void) paramList;

}

ACS_CC_ReturnType aes_afp_transfer_entity::createTransferEntityInfo(std::string & destinationSetName,
								    std::string & transferQueueName,
								    std::string & fileName)
{

	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering createTransferEntityInfo"));

	GCC_TDEBUG((aes_afp_transferEntity, "destinationSetName = %s,\n transferQueueName=%s\n, fileName=%s", destinationSetName.c_str(), transferQueueName.c_str(), fileName.c_str()));

	//The list of attributes
	vector<ACS_CC_ValuesDefinitionType> myAttrList;

	//the attributes
	ACS_CC_ValuesDefinitionType myAttrRDN;
	ACS_CC_ValuesDefinitionType myAttrFileTransStatus;
	ACS_CC_ValuesDefinitionType myAttrIsDir;

	//Fill the rdn Attribute
	string myStrRDN =  AES_AFP_TRANSFER_ENTITY_ID ;
	myStrRDN += "=" ;
	myStrRDN += transferQueueName;
	myStrRDN += "_";
	myStrRDN += fileName;

	char attrRDN[]  = AES_AFP_TRANSFER_ENTITY_ID;
	myAttrRDN.attrName  = attrRDN;
	myAttrRDN.attrType = ATTR_STRINGT;
	myAttrRDN.attrValuesNum = 1;
	void* valueRDN[1]={reinterpret_cast<void*>(const_cast<char *>(myStrRDN.c_str()))};
	myAttrRDN.attrValues = valueRDN;

	char AttrFileTransStatus[] = AES_AFP_FILE_TRANSMISSION_STATUS;
	myAttrFileTransStatus.attrName = AttrFileTransStatus;
	myAttrFileTransStatus.attrType = ATTR_INT32T;
	myAttrFileTransStatus.attrValuesNum = 0;
	myAttrFileTransStatus.attrValues = 0;

	char AttrIsDir[] = AES_AFP_IS_DIR_FLAG;
	myAttrIsDir.attrName = AttrIsDir;
	myAttrIsDir.attrType = ATTR_INT32T;
	myAttrIsDir.attrValuesNum = 0;
	myAttrIsDir.attrValues = 0;

	myAttrList.push_back(myAttrRDN);
	myAttrList.push_back(myAttrFileTransStatus);
	myAttrList.push_back(myAttrIsDir);

	//Determine whether its a responding dest set or initiating dest set.
	
	OmHandler myOmHandler;

	if( myOmHandler.Init() == ACS_CC_FAILURE )
	{
		GCC_TERROR((aes_afp_transferEntity, "%s", "Error occured while initializing OmHandler."));
		GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving createTransferEntityInfo()"));
		return ACS_CC_FAILURE;
	}
	std::string myParentRDN(""); 
	
	string tempDestSetRDN = AES_CDH_RESPONDING_DESTSET_RDN;
	tempDestSetRDN += "=";
	tempDestSetRDN += destinationSetName;
	tempDestSetRDN += ",";
	tempDestSetRDN += AES_GCC_Util::dnOfFileTransferM; 

	ACS_APGCC_ImmObject myDestSetObj; 
	myDestSetObj.objName = tempDestSetRDN;

	if(  myOmHandler.getObject(&myDestSetObj) == ACS_CC_FAILURE )
	{
		//The destination set is not a responding destination set.We will check for inititaing destination set now.
		tempDestSetRDN = AES_CDH_INITIATING_DESTSET_RDN;
		tempDestSetRDN += "=";
		tempDestSetRDN += destinationSetName;
		tempDestSetRDN += ",";
		tempDestSetRDN += AES_GCC_Util::dnOfFileTransferM; 

		myDestSetObj.objName = tempDestSetRDN;
		if(  myOmHandler.getObject(&myDestSetObj) == ACS_CC_FAILURE )
		{
			GCC_TERROR((aes_afp_transferEntity, "%s", "Given destination set is neither a responding nor an initiating destination set."));
			GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving createTransferEntityInfo()"));
			myOmHandler.Finalize();
			return ACS_CC_FAILURE;
		}
		else
		{
			myParentRDN = tempDestSetRDN;
		}
	}
	else
	{
		myParentRDN = tempDestSetRDN;
	}

	string myDN = myStrRDN;
	myDN += ",";
    myDN += myParentRDN;	


	if( createRuntimeObj(AES_AFP_TRANSFER_ENTITY_CLASS_NM, myParentRDN.c_str(), myAttrList) == ACS_CC_FAILURE )
	{
			if( this->getInternalLastError() != -14) //14,ERROR_ALREADY_EXISTS. 
			{
				GCC_TERROR(( aes_afp_transferEntity, "%s", "Creation of RunTime object failed."));
				GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving createTransferEntity"));
				myOmHandler.Finalize();
				return ACS_CC_FAILURE;
			}
			GCC_TDEBUG((aes_afp_transferEntity, "%s", "TE object already exists"));
	}
	GCC_TDEBUG((aes_afp_transferEntity, "%s", "Creation of RunTime Object is success."));
	
	//Insert the RDN  into a map.
	string strTqDestSetFile = transferQueueName;
	strTqDestSetFile += "_";
	strTqDestSetFile += destinationSetName;
	strTqDestSetFile += "_";
	strTqDestSetFile += fileName;
	transferEntityMap.insert( transferEntityPair( strTqDestSetFile, myDN ));


	GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving createTransferEntity"));
	myOmHandler.Finalize();
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType aes_afp_transfer_entity::deleteTransferEntity(const std::string& aFileName,
								const std::string& aDestinationSetName,
								const std::string& aTransferQueueName)
{
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering deleteTransferEntity"));

	//transferEntityInfoId=TQ_FILENAME,respondingDestinationSetId=DESTSET,fileTransferMId=1,dataTransferMId=1
	//transferEntityId=FILENAME,initiatingDestinationSetId=DESTSET,fileTransferMId=1,dataTransferMId=1
	
	string RDNName("");
	string strTQDestSetFileName = aTransferQueueName;
	strTQDestSetFileName += "_";
	strTQDestSetFileName += aDestinationSetName;
	strTQDestSetFileName += "_";
	strTQDestSetFileName += aFileName;

	transferEntityMapType::iterator p;
	p = transferEntityMap.find( strTQDestSetFileName );
	if( p != transferEntityMap.end() )
	{
			RDNName = (*p).second;

			if( deleteRuntimeObj(RDNName.c_str()) == ACS_CC_FAILURE )
			{
				GCC_TERROR(( aes_afp_transferEntity, "Deletion of RunTime object %s failed.", RDNName.c_str()));
				GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving deleteTransferEntity"));
				return ACS_CC_FAILURE;
			}
			else
			{
					GCC_TDEBUG(( aes_afp_transferEntity, "Deletion of RunTime object %s is success.", RDNName.c_str()));
					transferEntityMap.erase(p);
			}
	}
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving deleteTransferEntity"));
	return ACS_CC_SUCCESS;

}

// START OF aes_afp_transferEntity_runtimeHandlerThread

aes_afp_transferEntity_runtimeHandlerThread::aes_afp_transferEntity_runtimeHandlerThread(
		ACE_Thread_Manager* threadManager_)
{
	this->thr_mgr(threadManager_);
	theIsStop=false;
	theTransferEntityRuntimeOwnerImplementer=0;

}


aes_afp_transferEntity_runtimeHandlerThread::~aes_afp_transferEntity_runtimeHandlerThread()
{


}

void aes_afp_transferEntity_runtimeHandlerThread::setImpl(aes_afp_transfer_entity *pImpl)
{
	theTransferEntityRuntimeOwnerImplementer = pImpl;
	theIsStop = false;

}

void aes_afp_transferEntity_runtimeHandlerThread::stop()
{
	theIsStop=true;

}

//svc method
int aes_afp_transferEntity_runtimeHandlerThread::svc(void)
{
	int ret;
	struct pollfd fds[1];
	fds[0].fd = theTransferEntityRuntimeOwnerImplementer->getSelObj();
	fds[0].events = POLLIN;
	while(!theIsStop)
	{
		ret = poll(fds, 1, 1);
		if (ret == 0)
		{


		}
		else if (ret == -1)
		{

		}
		else
		{
			theTransferEntityRuntimeOwnerImplementer->dispatch(ACS_APGCC_DISPATCH_ALL);

		}
	}

	return 0;
}

ACS_CC_ReturnType aes_afp_transfer_entity::getTQDestSetFileInfo( const char *aOBJName,
				                                 std::string &transferQueue,
								 std::string &destinationSet,
							         std::string &fileName) 
{
	GCC_TTRACE((aes_afp_transferEntity, "%s", "Entering getTQDestSetFileInfo" ));

	//transferEntityId=FILENAME,respondingDestinationSetId=DESTSET,fileTransferMId=1,dataTransferMId=1
	//transferEntityId=FILENAME,initiatingDestinationSetId=DESTSET,fileTransferMId=1,dataTransferMId=1

	string myObjName = aOBJName;

	string::size_type pos1 = string::npos;
    string::size_type pos2 = string::npos;
    string::size_type pos3 = string::npos;
    string::size_type pos4 = string::npos;

	pos1 = myObjName.find_first_of("_");
	pos2 = myObjName.find_first_of(",");

	if( pos1 != string::npos && pos2 != string::npos )
	{
		fileName = myObjName.substr(pos1+1, pos2-pos1-1);
	}
	else
	{
		//Unable to get the file name.
		GCC_TDEBUG((aes_afp_transferEntity, "%s", "Unable to get the file name."));
		GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving getTQDestSetFileInfo" ));
		return ACS_CC_FAILURE;
	}

	pos3 = myObjName.find_first_of("=", pos2);
	pos4 = myObjName.find_first_of(",", pos3);

	if( pos3 != string::npos && pos4 != string::npos)
	{
		destinationSet = myObjName.substr(pos3+1, pos4-pos3-1);	
			
	}
	else
	{
		//Unable to get the destination set/file name.
		GCC_TDEBUG((aes_afp_transferEntity, "%s", "Unable to get the destination set name."));
		GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving getTQDestSetFileInfo" ));
		return ACS_CC_FAILURE;
	}

	pos1 = myObjName.find_first_of("=");
	pos2 = myObjName.find_first_of("_");

	if( pos1 != string::npos )
	{
		transferQueue = myObjName.substr( pos1+1, pos2-pos1-1 );	//TQ
	}
	else
	{
		//Unable to get the transfer queue name.
		GCC_TDEBUG((aes_afp_transferEntity, "%s", "Unable to get the transfer queue name."));
		GCC_TTRACE((aes_afp_transferEntity, "%s", "Leaving getTQDestSetFileInfo" ));
		return ACS_CC_FAILURE;
	}

	GCC_TTRACE((aes_afp_transferEntity, "Leaving getTQDestSetFileInfo destinationSet = %s\n fileName=%s\nTQ=%s\n", destinationSet.c_str(), fileName.c_str(), transferQueue.c_str() ));

	return ACS_CC_SUCCESS; 

}

#endif
