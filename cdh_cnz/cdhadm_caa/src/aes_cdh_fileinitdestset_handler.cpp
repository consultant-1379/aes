#include "aes_cdh_fileinitdestset_handler.h"

#include <iostream>
#include <ace/Signal.h>
#include <saImmOi.h>
#include <servr.h>
#include <ACS_APGCC_Util.H>
#include <aes_cdh_brfc_interface.h>
#include <aes_gcc_util.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_FileInitDestSetCmdHandler);

using namespace std;

//! Constructor
AES_CDH_FileInitDestSetCmdHandler::AES_CDH_FileInitDestSetCmdHandler(string className , string szimpName,ACE_Thread_Manager* threadManager_)
:acs_apgcc_objectimplementereventhandler_V3( szimpName )
{
	this->thr_mgr(threadManager_);
	m_className = className;
	m_isClassImplAdded = false;
	m_tpReactorImpl = new(std::nothrow) ACE_TP_Reactor;
	m_deleteCcbId=1;//for coverity fix
	if(m_tpReactorImpl != 0)
	{
		m_poReactor = new(std::nothrow) ACE_Reactor(m_tpReactorImpl);
	}

	m_oiHandler = new(std::nothrow) acs_apgcc_oihandler_V3() ;
	if(m_oiHandler == 0)
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Failed to allocate memory for m_oiHandler pointer");
		AES_CDH_TRACE_MESSAGE("Failed to allocate memory for m_oiHandler pointer");
	}
}

AES_CDH_FileInitDestSetCmdHandler::~AES_CDH_FileInitDestSetCmdHandler()
{
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

ACS_CC_ReturnType AES_CDH_FileInitDestSetCmdHandler::setObjImpl()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	for (int i=0; i < 10; ++i)
	{ 
		ACS_CC_ReturnType errorCode = m_oiHandler->addClassImpl(this,m_className.c_str());

		if ( errorCode == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Set implementer for the class %s : failed",m_className.c_str());
			AES_CDH_TRACE_MESSAGE("Set implementer for the class %s : failed",m_className.c_str());
			int intErr = getInternalLastError();
			for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
			if ( i >= 10 )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Set implementer for the class %s : failed, Errcode = %d",m_className.c_str(), intErr);
				return ACS_CC_FAILURE;
			}
			else
				continue;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Success: Set implementer for the class %s",m_className.c_str());
			AES_CDH_LOG(LOG_LEVEL_INFO,"Success: Set implementer for the class %s",m_className.c_str());
			m_isClassImplAdded = true;
			return ACS_CC_SUCCESS;
		}
	}  
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_FAILURE;
}  

ACE_INT32 AES_CDH_FileInitDestSetCmdHandler::svc()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();
	AES_CDH_TRACE_MESSAGE("Exiting");
	return 0;
}


ACS_CC_ReturnType AES_CDH_FileInitDestSetCmdHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	AES_CDH_TRACE_MESSAGE("            ObjectCreateCallback invoked           ");
	AES_CDH_LOG(LOG_LEVEL_INFO,"ObjectCreateCallback invoked");
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	string destSetName("");
	int i = 0;
	while(attr[i])
	{
		string tmpString;
		if(ACE_OS::strcmp(attr[i]->attrName, "initiatingDestinationSetId")==0)
		{
			tmpString = string((reinterpret_cast<char *>(attr[i]->attrValues[0])));
			extractNameFromRdn(tmpString,destSetName);
			if (isValidName(destSetName) == false)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving  with FAILURE");
				//set the error code for invalid value
				string objName = tmpString + "," + parentName ;
				setErrorCode(AES_CDH_RC_UNREAS, objName.c_str());
				return ACS_CC_FAILURE;
			}
		}
		i++;
	}
	AES_CDH_TRACE_MESSAGE("Leaving with SUCCESS");
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_CDH_FileInitDestSetCmdHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	//Acquire mutex.
	ServR::initdestsetMutex.acquire();
	m_deleteCcbId = ccbId;

	ServR::isInitDestSetDeleteTrig = true;

	ServR::isDelOfInitDestSetSuccess = false;

	AES_CDH_TRACE_MESSAGE("ObjectDeleteCallback invoked");
	AES_CDH_LOG(LOG_LEVEL_INFO,"ObjectDeleteCallback invoked for object : %s",objName);

	(void)oiHandle;

	string destSetName(""), detailInfo(""), destName(""), userName("");
	string tmpString(objName);
	AES_CDH_ResultCode resultCode=AES_CDH_RC_ERROR;

	//Extract the destination set name from the objname.
	extractNameFromDn(tmpString,destSetName);

	//Get the destination set and destination attributes.
	vector<AES_CDH_DestinationSet::destSetAttributes> destSetAttrs;
	vector<AES_CDH_Destination::destAttributes> primDestAttrs;
	vector<AES_CDH_Destination::destAttributes> secDestAttrs;

	resultCode= ServR::listDestSet( destSetName, destSetAttrs);
	if(resultCode!= AES_CDH_RC_OK)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Destset %s not found", destSetName.c_str());
		AES_CDH_TRACE_MESSAGE("Destset %s not found", destSetName.c_str());
		ServR::initdestsetMutex.release();
		setErrorCode(resultCode);
		return ACS_CC_FAILURE;
	}
	//check if the dest set any destination associated or not
	if (destSetAttrs.size() == 0)
	{
		setErrorCode(AES_CDH_RC_OK);
		ServR::initdestsetMutex.release();
		return ACS_CC_SUCCESS;
	}
	//Now parse the destSetAttrs and get the destinations.

	if( !destSetAttrs[0].primDest.empty() )
	{
		ServR::listDest( destSetAttrs[0].primDest, primDestAttrs );

		destNode deleteDestNode;
		deleteDestNode.argc = 0;
		deleteDestNode.destinationName = destSetAttrs[0].primDest;
		deleteDestNode.argv[deleteDestNode.argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
		ACE_OS::strcpy( deleteDestNode.argv[deleteDestNode.argc++], AES_CDH_CDHDEFINE);

		for( unsigned int ctr = 1 ; ctr <= primDestAttrs[0].otherAttrs.size(); ctr++, deleteDestNode.argc++)
		{
			deleteDestNode.argv[ctr] = new char[primDestAttrs[0].otherAttrs[ctr-1].size()+1];
			strcpy( deleteDestNode.argv[ctr], primDestAttrs[0].otherAttrs[ctr-1].c_str());
		}
		deleteDestNode.destinationSetName = destSetName;
		deleteDestNode.isPrimDest = true;
		deleteDestNode.isSecDest = false;
		deleteDestNode.isDestSet = false;
		deleteDestNode.transferType = primDestAttrs[0].transferType;

		//Insert the destination node in the deleteDestList.
		m_deleteDestList.push_back(deleteDestNode);
		AES_CDH_TRACE_MESSAGE( "push back value %s",deleteDestNode.destinationSetName.c_str());
		for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
		{
			AES_CDH_TRACE_MESSAGE("after push back");
			AES_CDH_TRACE_MESSAGE( "%s",m_deleteDestList[ctr].destinationSetName.c_str());
		}

	}
	if( !destSetAttrs[0].secDest.empty())
	{
		ServR::listDest( destSetAttrs[0].secDest, secDestAttrs );

		destNode deleteDestNode;
		deleteDestNode.argc = 0;
		deleteDestNode.destinationName = destSetAttrs[0].secDest;
		deleteDestNode.argv[deleteDestNode.argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
		ACE_OS::strcpy( deleteDestNode.argv[deleteDestNode.argc++], AES_CDH_CDHDEFINE);

		for( unsigned int ctr = 1 ; ctr <= secDestAttrs[0].otherAttrs.size(); ctr++, deleteDestNode.argc++)
		{
			deleteDestNode.argv[ctr] = new char[secDestAttrs[0].otherAttrs[ctr-1].size()+1];
			strcpy( deleteDestNode.argv[ctr], secDestAttrs[0].otherAttrs[ctr-1].c_str());
		}
		deleteDestNode.destinationSetName = destSetName;
		deleteDestNode.isPrimDest = false;
		deleteDestNode.isSecDest = true;
		deleteDestNode.isDestSet = false;
		deleteDestNode.transferType = secDestAttrs[0].transferType;

		//Insert the destination node in the deleteDestList.
		m_deleteDestList.push_back(deleteDestNode);
		AES_CDH_TRACE_MESSAGE( "push back value %s",deleteDestNode.destinationSetName.c_str());
		for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
		{
			AES_CDH_TRACE_MESSAGE("after push back");
			AES_CDH_TRACE_MESSAGE("%s", m_deleteDestList[ctr].destinationSetName.c_str());
		}

	}

	if( !destSetAttrs[0].secDest.empty() )
	{
		//Delete the association between destination set and secondary destination.
		AES_CDH_LOG(LOG_LEVEL_INFO,"Delete the association between destination set and secondary destination.");
		resultCode = ServR::deleteDestSet(destSetName, detailInfo, destSetAttrs[0].secDest, userName);
		if ( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Error occurred while removing dest %s from destset %s", destSetAttrs[0].secDest.c_str(), destSetName.c_str());
			AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
			ServR::initdestsetMutex.release();
			//set the error code for failure of deleteDestSet
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;
		}
		resultCode = ServR::deleteDest(destSetAttrs[0].secDest, detailInfo,  userName );
		if ( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Error occurred while removing secondary dest %s from destset %s", destName.c_str(), destSetName.c_str());
			AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
			//Re-create the association
			string primDest(""), backupDest("");
			ServR::changeDestSet(destSetName, primDest, destSetAttrs[0].secDest, backupDest, userName);
			ServR::initdestsetMutex.release();
			//set the error code for failure of deleteDest
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;
		}
	}

	//Now delete the destination set.
	resultCode = ServR::deleteDestSet(destSetName, detailInfo, destName, userName);
	AES_CDH_TRACE_MESSAGE("ServR::deleteDestSet , resultCode = %d",resultCode);
	string secInitDestRdn = AES_CDH_SECONDARYINITDEST_RDN;
	if (resultCode != AES_CDH_RC_OK)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ServR::deleteDestSet , failed");
		ServR::initdestsetMutex.release();
		//set the error code for failure of deleteDestSet
		setErrorCode(resultCode);
		return ACS_CC_FAILURE;
	}
	if( !destSetAttrs[0].primDest.empty() )
	{
		//remove primary from destset
		resultCode = ServR::deleteDest(destSetAttrs[0].primDest, detailInfo,  userName );
		if ( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR,  " Error occured while removing primary dest %s from destset %s", destName.c_str(), destSetName.c_str());
			AES_CDH_TRACE_MESSAGE("Leaving with error code %d", resultCode);
			string userGroup("");
			ServR::initdestsetMutex.release();
			//set the error code for failure of deleteDest
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;

		}

	}
	AES_CDH_TRACE_MESSAGE("Destination-%s and Destination set-%s got removed", destName.c_str(),destSetName.c_str());
	ServR::isDelOfInitDestSetSuccess = true;
	ServR::initdestsetMutex.release();
	//set the error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_CDH_FileInitDestSetCmdHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)attrMods;
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	ACS_CC_ReturnType enResult=ACS_CC_FAILURE;
	AES_CDH_TRACE_MESSAGE("ObjectModifyCallback invoked");
	return enResult;
}

ACS_CC_ReturnType AES_CDH_FileInitDestSetCmdHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	AES_CDH_TRACE_MESSAGE("CcbCompleteCallback invoked");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbCompleteCallback invoked");
	return ACS_CC_SUCCESS;
}

void AES_CDH_FileInitDestSetCmdHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("CcbAbortCallback invoked");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbAbortCallback invoked");
    (void)oiHandle;
	if( ccbId == m_deleteCcbId )
	{
		ServR::initdestsetMutex.acquire();
		if(  m_deleteDestList.size() > 0 )
		{
			for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
			{
				if( m_deleteDestList[ctr].isPrimDest == true )
				{
					//Create the seconadry destination.
					string strUserGroup(""), secDest(""), backupDest(""), userName("");
					ServR::define( m_deleteDestList[ctr].destinationName, m_deleteDestList[ctr].transferType, m_deleteDestList[ctr].argc, m_deleteDestList[ctr].argv, strUserGroup, AES_CDH_PRIMARYINITDEST_RDN, m_deleteDestList[ctr].destinationSetName);
					ServR::defineDestSet(m_deleteDestList[ctr].destinationSetName, m_deleteDestList[ctr].destinationName, secDest, backupDest,strUserGroup, AES_CDH_INIT_DEST_SET_ID);

				}
				if( m_deleteDestList[ctr].isSecDest == true )
				{
					//Create the seconadry destination.
					string strUserGroup(""), primDest(""), backupDest(""), userName("");
					ServR::define( m_deleteDestList[ctr].destinationName, m_deleteDestList[ctr].transferType, m_deleteDestList[ctr].argc, m_deleteDestList[ctr].argv, strUserGroup, AES_CDH_SECONDARYINITDEST_RDN, m_deleteDestList[ctr].destinationSetName);
					ServR::changeDestSet(m_deleteDestList[ctr].destinationSetName, primDest, m_deleteDestList[ctr].destinationName, backupDest, userName);
				}

			}
			for ( unsigned ctr = 0; ctr < m_deleteDestList.size() ; ctr++)
			{
				for( int i = 0; i < m_deleteDestList[ctr].argc ; i++)
				{
					delete[] m_deleteDestList[ctr].argv[i];
					m_deleteDestList[ctr].argv[i] = 0;
				}
			}
			//Delete the entire list.
			for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
			{
				AES_CDH_TRACE_MESSAGE( "before delete");
				AES_CDH_TRACE_MESSAGE( "%s",m_deleteDestList[ctr].destinationSetName.c_str());
			}
			vector <destNode>::iterator itr = m_deleteDestList.begin();
			while( itr != m_deleteDestList.end())
			{
				m_deleteDestList.erase(itr);
				itr = m_deleteDestList.begin();
			}
			for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
			{
				AES_CDH_TRACE_MESSAGE("after delete");
				AES_CDH_TRACE_MESSAGE("%s", m_deleteDestList[ctr].destinationSetName.c_str());
			}
		}

		ServR::isInitDestSetDeleteTrig = false;
		ServR::initdestsetMutex.release();
	}
}

void AES_CDH_FileInitDestSetCmdHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("CcbApplyCallback invocated");
	AES_CDH_LOG(LOG_LEVEL_INFO, "CcbApplyCallback invoked");
	(void)oiHandle;
	if( ccbId == m_deleteCcbId )
	{
		ServR::initdestsetMutex.acquire();
		if(  m_deleteDestList.size() > 0 )
		{
			for ( unsigned ctr = 0; ctr < m_deleteDestList.size() ; ctr++)
			{
				for( int i = 0; i < m_deleteDestList[ctr].argc ; i++)
				{
					delete[] m_deleteDestList[ctr].argv[i];
					m_deleteDestList[ctr].argv[i] = 0;
				}
			}
			//Delete the entire list.

			vector <destNode>::iterator itr = m_deleteDestList.begin();
			while( itr != m_deleteDestList.end())
			{
				m_deleteDestList.erase(itr);
				itr = m_deleteDestList.begin();
			}
		}
		ServR::isInitDestSetDeleteTrig = false;
		ServR::initdestsetMutex.release();
	}
}

ACS_CC_ReturnType AES_CDH_FileInitDestSetCmdHandler::updateRuntime(const char *objName, const char **attrName)
{
	AES_CDH_TRACE_MESSAGE("CcbUpdateRuntimeCallback invocated");
	AES_CDH_LOG(LOG_LEVEL_INFO, "CcbUpdateRuntimeCallback invoked");
	(void)attrName;
	//Extract the destination set name from the object Name.

	std::string destSetName("");
	std::string activeDestName("");
	//Create an object of ACS_CC_ImmParameter.
	ACS_CC_ImmParameter parToModify;
	parToModify.attrName = (char *) AES_CDH_INIT_ACTIVE_DEST_DESTSET;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	parToModify.attrValues=new void*[parToModify.attrValuesNum];

	extractNameFromDn( objName, destSetName );

	//Update the value of active destination in the destination set object.
	if( ServR::getActiveDestForDestSet( destSetName, activeDestName ) == true )
	{
		parToModify.attrValues[0] = reinterpret_cast<void*>(const_cast<char *>( activeDestName.c_str()));

		//Modify the attribute.
		if( modifyRuntimeObj( objName, &parToModify) != ACS_CC_SUCCESS )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, " Error occured while modifying activeDestination in updateRuntime ");
			AES_CDH_TRACE_MESSAGE("Leaving");
			delete[] parToModify.attrValues;
			parToModify.attrValues = 0;
			return ACS_CC_SUCCESS; //Return success here otherwise user wont be able to see other attributes as well.
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving  with activeDest as %s", activeDestName.c_str());
	if (parToModify.attrValues[0] != NULL)
	{
		delete[] parToModify.attrValues;
		parToModify.attrValues = 0;
	}
	return ACS_CC_SUCCESS;
}

void AES_CDH_FileInitDestSetCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
							       ACS_APGCC_InvocationType invocation, 
							       const char* p_objName, 
							       ACS_APGCC_AdminOperationIdType operationId,  
							       ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)paramList;
	AES_CDH_TRACE_MESSAGE("adminOperationCallback invocated");
	AES_CDH_LOG(LOG_LEVEL_INFO, "adminOperationCallback invoked");

	bool myOIValidationError = true;
	string destSetName("");
	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;
	switch ( operationId )
	{
	case 8:
	{
		//Extract the destination set name from the objname.

		extractNameFromDn(p_objName,destSetName);
		if(ServR::isSecondaryDestExists(destSetName)== true)
		{
			AES_CDH_ResultCode result = ServR::switchActiveDest(destSetName);
			setErrorCode(result);
			if(result == AES_CDH_RC_OK)
			{
				myOIValidationError=true;
			}
		}
		else
		{
			myOIValidationError=false;
			AES_CDH_LOG(LOG_LEVEL_ERROR,  "In Admin call back , route traffic  failed ");
			char attrName1[] = "errorCode";
			firstElem.attrName = attrName1;
			firstElem.attrType = ATTR_INT32T;
			int intValue1 = AES_CDH_RC_ROUTEERR ;
			firstElem.attrValues=reinterpret_cast<void*>(&intValue1);
			AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(AES_CDH_RC_ROUTEERR,"");
			string ertxt = "@ComNbi@";
			ertxt += resobj->errorText();
			char attrName2[]= "errorText";
			secondElem.attrName = attrName2;
			secondElem.attrType = ATTR_STRINGT;
			char* stringValue = const_cast<char*>(ertxt.c_str());
			secondElem.attrValues=reinterpret_cast<void*>(stringValue);
			AES_CDH_TRACE_MESSAGE( "In Admin call back , stringValue = %s",stringValue);
			/*insert parameter into the vector*/
			//vectorOut.push_back(firstElem);
			vectorOut.push_back(secondElem);
			delete resobj;
			resobj = NULL;

		}
	}
	}
	ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError,vectorOut);
	AES_CDH_TRACE_MESSAGE(  "result admin op :%d",rc);
}

void AES_CDH_FileInitDestSetCmdHandler::shutdown()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	m_poReactor->end_reactor_event_loop();

	if( m_isClassImplAdded == true )
	{
		for (int i =0; i < 10; ++i)
		{
			if( m_oiHandler->removeClassImpl(this, m_className.c_str()) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while removing class implementer");
				int intErr = getInternalLastError();
				if( intErr == -6)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if ( i >= 10)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while removing class implementer, ErrCode = %d", intErr);
						AES_CDH_TRACE_MESSAGE("Error occurred while removing class implementer, ErrCode = %d", intErr);
						break;
					}
					else
						continue;
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "Error occurred while removing class implementer, ErrCode = %d", intErr);
					break;
				}
			}
			else
			{
				AES_CDH_TRACE_MESSAGE("removing class implementer Success");
				m_isClassImplAdded = false;
				break;
			}
		}
	}
	this->wait();
	if(!m_isClassImplAdded)
			AES_CDH_LOG(LOG_LEVEL_INFO,"Removed class implementer for class %s successfully !!",m_className.c_str());
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
	ROUTINE: extractDestNmFromRdn
=================================================================== */
void AES_CDH_FileInitDestSetCmdHandler::extractNameFromRdn( const string rdnName,
		string &name)
{

	AES_CDH_TRACE_MESSAGE("Entering");

	name = "";

	string::size_type pos1 = string::npos;

	unsigned int pos2 = rdnName.length();

	pos1 = rdnName.find_first_of("=");

	if(( pos1 != string::npos) && pos1 < pos2 )
	{
		name = rdnName.substr(pos1+1, pos2-pos1-1);
	}

	AES_CDH_TRACE_MESSAGE("Leaving");

}
void AES_CDH_FileInitDestSetCmdHandler::extractNameFromDn (const string dnName, string& name)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	name = "";

	string::size_type pos1 = string::npos;

	string::size_type pos2 = string::npos;

	pos1 = dnName.find_first_of("=");
	pos2 = dnName.find_first_of(",");
	if(( pos1 != string::npos) && (pos2 != string::npos)&& pos1 < pos2 )
	{
		name = dnName.substr(pos1+1, pos2-pos1-1);
	}

	AES_CDH_TRACE_MESSAGE("Leaving");
}


bool AES_CDH_FileInitDestSetCmdHandler::isValidName(string name)
{
	bool isNameValid = true;
	const string legalChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	const string legalChars2("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (name.empty())
	{
		isNameValid = false;
		AES_CDH_TRACE_MESSAGE("The %s is Empty",name.c_str());
		return isNameValid;
	}
	if( name.length() < 1  || name.length() > 32 )
	{
		AES_CDH_TRACE_MESSAGE("The length of the %s exceeds limit",name.c_str());
		isNameValid = false;
		return isNameValid;
	}
	if (name.find_first_not_of(legalChars) != string::npos)
	{
		AES_CDH_TRACE_MESSAGE("%s contains invalid characters",name.c_str());
		isNameValid = false;
		return isNameValid;
	}
	if (name.substr(0,1).find_first_not_of(legalChars2) != string::npos)
	{
		AES_CDH_TRACE_MESSAGE("%s starts with number",name.c_str());
		isNameValid = false;
		return isNameValid;
	}
	return isNameValid;
}
/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void AES_CDH_FileInitDestSetCmdHandler::setErrorCode(unsigned int code, const char* objName)
{
	AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(code,"");
	string ertxt=resobj->errorText();
	if( code == AES_CDH_RC_UNREAS )
	{
		if( objName != 0 )
		{
			string comDn = "";
			AES_GCC_Util::extractComDnfromDn(objName,comDn);
			ertxt = ertxt + " for " + comDn;
		}
	}
	setExitCode(code,ertxt );
	delete resobj;
}
