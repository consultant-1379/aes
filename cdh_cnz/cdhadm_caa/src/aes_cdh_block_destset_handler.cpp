
#include <aes_cdh_block_destset_handler.h>
#include <iostream>
#include <ace/Signal.h>
#include <saImmOi.h>
#include <servr.h>
#include <ACS_APGCC_Util.H>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_BlockDestSetCmdHandler);

using namespace std;

//! Constructor
AES_CDH_BlockDestSetCmdHandler::AES_CDH_BlockDestSetCmdHandler(string className , string szimpName,ACE_Thread_Manager* threadManager_)
:acs_apgcc_objectimplementereventhandler_V3( szimpName )
{

	AES_CDH_TRACE_MESSAGE("Entering");
	this->thr_mgr(threadManager_);
	m_className = className;
	m_isClassImplAdded = false;
	m_tpReactorImpl = new ACE_TP_Reactor;
	m_deleteCcbId=1;//for coverity fix
	if(m_tpReactorImpl != 0)
	{
		m_poReactor = new(std::nothrow) ACE_Reactor(m_tpReactorImpl);
	}

	m_oiHandler = new(std::nothrow) acs_apgcc_oihandler_V3() ;
	if(m_oiHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Failed to allocate memory for m_oiHandler pointer");
		AES_CDH_TRACE_MESSAGE("Failed to allocate memory for m_oiHandler pointer");
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

AES_CDH_BlockDestSetCmdHandler::~AES_CDH_BlockDestSetCmdHandler()
{
	AES_CDH_TRACE_MESSAGE("Entering");
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

ACS_CC_ReturnType AES_CDH_BlockDestSetCmdHandler::setObjImpl()
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
			if (intErr == -6)
			{
				for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
				if ( i >= 10 )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while add class implmenter, ErrorCode = %d", intErr);
					return ACS_CC_FAILURE;
				}
				else
					continue;
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while add class implmenter, ErrorCode = %d", intErr);
				return ACS_CC_FAILURE;
			}
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Success: Set implementer for the class %s",m_className.c_str());
			AES_CDH_LOG(LOG_LEVEL_INFO, "Success: Set implementer for the class %s",m_className.c_str());
			m_isClassImplAdded = true;
			return ACS_CC_SUCCESS;
		}

	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_FAILURE;

}

ACE_INT32 AES_CDH_BlockDestSetCmdHandler::svc()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	m_poReactor->open(1);
	dispatch(m_poReactor, ACS_APGCC_DISPATCH_ALL);
	m_poReactor->run_reactor_event_loop();
	AES_CDH_TRACE_MESSAGE("Exiting");
	return 0;
}


ACS_CC_ReturnType AES_CDH_BlockDestSetCmdHandler::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "create callback received.");
	string destSetName("");
	int i = 0;
	while(attr[i])
	{
		string tmpString;
		if(ACE_OS::strcmp(attr[i]->attrName, AES_CDH_BLOCK_DESTSET_RDN) == 0)
		{
			tmpString = string((reinterpret_cast<char *>(attr[i]->attrValues[0])));
			extractNameFromRdn(tmpString,destSetName);
			if (isValidName(destSetName) == false)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving  with FAILURE");
				AES_CDH_TRACE_MESSAGE("Leaving  with FAILURE");
				//set the error code for invalid value
				string objName = tmpString + "," + parentName;
				setErrorCode(AES_CDH_RC_UNREAS_BLOCKDESTSETNAME, objName.c_str() );
				return ACS_CC_FAILURE;
			}
		}
		i++;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_CDH_BlockDestSetCmdHandler::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	//Acquire mutex.
	ServR::blockdestsetMutex.acquire();
	AES_CDH_TRACE_MESSAGE(" ServR::blockdestsetMutex acquired");
	m_deleteCcbId = ccbId;

	ServR::isBlockDestSetDeleteTrig = true;

	ServR::isDelOfBlockDestSetSuccess = false;

	AES_CDH_TRACE_MESSAGE("Entering into deleted callback objName = %s",objName);
	AES_CDH_LOG(LOG_LEVEL_INFO, "deleted callback received.");
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
		ServR::blockdestsetMutex.release();
		setErrorCode(resultCode);
		return ACS_CC_FAILURE;
	}
	//check if the dest set any destination associated or not
	if (destSetAttrs.size() == 0)
	{
		setErrorCode(AES_CDH_RC_OK);
		ServR::blockdestsetMutex.release();
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

	}

	if( !destSetAttrs[0].secDest.empty() )
	{
		//Delete the association between destination set and secondary destination.
		resultCode = ServR::deleteDestSet(destSetName, detailInfo, destSetAttrs[0].secDest, userName);
		if ( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while removing dest %s from destset %s error code %d", destSetAttrs[0].secDest.c_str(), destSetName.c_str(), resultCode);
			AES_CDH_TRACE_MESSAGE("Error occurred while removing dest %s from destset %s error code %d", destSetAttrs[0].secDest.c_str(), destSetName.c_str(), resultCode);
			if( m_deleteDestList.size() > 0 )
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

			ServR::blockdestsetMutex.release();
			//set the error code for failure of deleteDestSet
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;
		}
		AES_CDH_TRACE_MESSAGE("Association removed %s-%s",destSetAttrs[0].secDest.c_str(), destSetName.c_str());
		AES_CDH_LOG(LOG_LEVEL_INFO, "Association removed %s-%s",destSetAttrs[0].secDest.c_str(), destSetName.c_str());
		resultCode = ServR::deleteDest(destSetAttrs[0].secDest, detailInfo,  userName );
		if ( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while removing secondary dest %s from destset %s error code %d", destName.c_str(), destSetName.c_str(), resultCode);
			AES_CDH_TRACE_MESSAGE("Error occurred while removing secondary dest %s from destset %s error code %d", destName.c_str(), destSetName.c_str(), resultCode);
			if( m_deleteDestList.size() > 0 )
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
					itr =  m_deleteDestList.begin();
				}
			}

			//Re-create the association
			string primDest(""), backupDest("");
			AES_CDH_TRACE_MESSAGE(" Association created %s-%s",destSetAttrs[0].secDest.c_str(), destSetName.c_str());
			ServR::changeDestSet(destSetName, primDest, destSetAttrs[0].secDest, backupDest, userName);
			ServR::blockdestsetMutex.release();
			//set the error code for failure of deleteDest
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;
		}
	}

	//Now delete the destination set.
	resultCode = ServR::deleteDestSet(destSetName, detailInfo, destName, userName);
	AES_CDH_TRACE_MESSAGE("ServR::deleteDestSet , resultCode = %d", resultCode);
	AES_CDH_LOG(LOG_LEVEL_INFO, "deleteDestSet resultCode = %d", resultCode);
	string secBlockDestRdn = AES_CDH_SECONDARY_BLOCK_DEST_RDN;
	if (resultCode != AES_CDH_RC_OK)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ServR::deleteDestSet %s failed",destSetName.c_str());
		AES_CDH_TRACE_MESSAGE("ServR::deleteDestSet %s failed",destSetName.c_str());
		if( !destSetAttrs[0].secDest.empty() )
		{
			char *argv[32] = { 0 };
			int argc = 0;
			argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

			//Create secondary and its assoc with dest set.
			for( unsigned int ctr = 1 ; ctr <= secDestAttrs[0].otherAttrs.size(); ctr++, argc++)
			{
				argv[ctr] = new char[secDestAttrs[0].otherAttrs[ctr-1].size()+1];
				strcpy( argv[ctr], secDestAttrs[0].otherAttrs[ctr-1].c_str());
			}
			argv[argc] = 0;
			string primDest(""), backupDest(""), userGroup("");
			ServR::define( secDestAttrs[0].destName, secDestAttrs[0].transferType, argc, argv, userGroup, secBlockDestRdn);
			for( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			ServR::changeDestSet(destSetName, primDest, secDestAttrs[0].destName, backupDest,userGroup);
			AES_CDH_TRACE_MESSAGE("Association created again for %s-%s",destSetAttrs[0].secDest.c_str(), destSetName.c_str());
		}
		//Delete the m_deleteDestList

		if( m_deleteDestList.size() > 0 )
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


		ServR::blockdestsetMutex.release();
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
			AES_CDH_LOG(LOG_LEVEL_ERROR,  "Error occured while removing primary dest %s from destset %s, error cdoe %d", destName.c_str(), destSetName.c_str(), resultCode);
			AES_CDH_TRACE_MESSAGE("Error occured while removing primary dest %s from destset %s, error cdoe %d", destName.c_str(), destSetName.c_str(), resultCode);
			string userGroup("");
			if( !destSetAttrs[0].secDest.empty())
			{
				char *argv[32] = { 0 };
				int argc = 0;

				argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
				ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

				//Create secondary dest.
				for( unsigned int ctr = 1 ; ctr <= secDestAttrs[0].otherAttrs.size(); ctr++, argc++)
				{
					argv[ctr] = new char[secDestAttrs[0].otherAttrs[ctr-1].size()+1];
					strcpy( argv[ctr], secDestAttrs[0].otherAttrs[ctr-1].c_str());
				}
				argv[argc] = 0;
				if( argc !=0 )
				{
					ServR::define( secDestAttrs[0].destName, secDestAttrs[0].transferType, argc, argv, userGroup,secBlockDestRdn );
					AES_CDH_LOG(LOG_LEVEL_INFO,  " recreating destination %s",destSetAttrs[0].secDest.c_str());
				}
				for( int ctr = 0; ctr < argc ; ctr++)
				{
					delete[] argv[ctr];
					argv[ctr] = 0;
				}
			}
			string  backupDest("");
			AES_CDH_LOG(LOG_LEVEL_INFO,  " recreating destination set %s with %s and %s",destSetName.c_str(),destSetAttrs[0].primDest.c_str(),destSetAttrs[0].secDest.c_str());
			ServR::defineDestSet(destSetName, destSetAttrs[0].primDest, destSetAttrs[0].secDest, backupDest,userGroup, AES_CDH_BLOCK_DEST_SET_ID);

			if( m_deleteDestList.size() > 0 )
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

			ServR::blockdestsetMutex.release();
			//set the error code for failure of deleteDest
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;

		}

	}
	//Delete block pair mapper files and pointer
	AES_CDH_TRACE_MESSAGE("Removing block pair files and instance for Destination set %s ", destSetName.c_str());
	BlockPairMapperManager::instance()->remove(destSetName);
	AES_CDH_TRACE_MESSAGE("Destination-%s and Destination set-%s got removed", destName.c_str(),destSetName.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO, "Destination-%s and Destination set-%s got removed", destName.c_str(),destSetName.c_str());
	ServR::isDelOfBlockDestSetSuccess = true;
	AES_CDH_TRACE_MESSAGE("ServR::isDelOfBlockDestSetSuccess set to true");
	ServR::blockdestsetMutex.release();
	//set the error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_CDH_BlockDestSetCmdHandler::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)attrMods;
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	ACS_CC_ReturnType enResult=ACS_CC_FAILURE;
	AES_CDH_TRACE_MESSAGE("            ObjectModifyCallback invoked           ");
	AES_CDH_LOG(LOG_LEVEL_INFO, "modify callback invoked");
	return enResult;
}

ACS_CC_ReturnType AES_CDH_BlockDestSetCmdHandler::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	AES_CDH_TRACE_MESSAGE("            CcbCompleteCallback invoked           ");
	AES_CDH_LOG(LOG_LEVEL_INFO, "complete callback invoked");
	return ACS_CC_SUCCESS;
}

void AES_CDH_BlockDestSetCmdHandler::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	AES_CDH_TRACE_MESSAGE("%s", "Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "abort callback invoked");
	if( ccbId == m_deleteCcbId )
	{
		ServR::blockdestsetMutex.acquire();
		if(  m_deleteDestList.size() > 0 )
		{
			for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
			{
				if( m_deleteDestList[ctr].isPrimDest == true )
				{
					//Create the seconadry destination.
					string strUserGroup(""), secDest(""), backupDest(""), userName("");
					ServR::define( m_deleteDestList[ctr].destinationName, m_deleteDestList[ctr].transferType, m_deleteDestList[ctr].argc, m_deleteDestList[ctr].argv, strUserGroup, AES_CDH_PRIMARY_BLOCK_DEST_RDN);
					ServR::defineDestSet(m_deleteDestList[ctr].destinationSetName, m_deleteDestList[ctr].destinationName, secDest, backupDest,strUserGroup, AES_CDH_BLOCK_DEST_SET_ID);
				}
				if( m_deleteDestList[ctr].isSecDest == true )
				{
					//Create the seconadry destination.
					string strUserGroup(""), primDest(""), backupDest(""), userName("");
					ServR::define( m_deleteDestList[ctr].destinationName, m_deleteDestList[ctr].transferType, m_deleteDestList[ctr].argc, m_deleteDestList[ctr].argv, strUserGroup, AES_CDH_SECONDARY_BLOCK_DEST_RDN);
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

			vector <destNode>::iterator itr = m_deleteDestList.begin();
			while( itr != m_deleteDestList.end())
			{
				m_deleteDestList.erase(itr);
				itr = m_deleteDestList.begin();
			}
		}

		ServR::isBlockDestSetDeleteTrig = false;
		ServR::blockdestsetMutex.release();
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

void AES_CDH_BlockDestSetCmdHandler::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "apply callback invoked");
	(void)oiHandle;
	if( ccbId == m_deleteCcbId )
	{
		ServR::blockdestsetMutex.acquire();
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

		ServR::isBlockDestSetDeleteTrig = false;
		ServR::blockdestsetMutex.release();
	}
	AES_CDH_TRACE_MESSAGE("            CcbApplyCallback invocated              ");
}

ACS_CC_ReturnType AES_CDH_BlockDestSetCmdHandler::updateRuntime(const char *objName, const char **attrName)
{
	AES_CDH_TRACE_MESSAGE("           CcbUpdateRuntimeCallback invocated             ");
	AES_CDH_TRACE_MESSAGE("Object Name %s  Attr Name %s ", objName , attrName[0]);
	AES_CDH_LOG(LOG_LEVEL_INFO, "Object Name %s  Attr Name %s ", objName , attrName[0]);

	//Extract the destination set name from the object Name.

	std::string destSetName("");
	std::string activeDestName("");
	//Create an object of ACS_CC_ImmParameter.
	ACS_CC_ImmParameter parToModify;
	parToModify.attrName = (char *) AES_CDH_BLOCK_ACTIVE_DEST_DESTSET;
	parToModify.attrType = ATTR_STRINGT;
	parToModify.attrValuesNum = 1;
	parToModify.attrValues = new void*[parToModify.attrValuesNum];

	extractNameFromDn( objName, destSetName );

	//Update the value of active destination in the destination set object.
	if( ServR::getActiveDestForDestSet( destSetName, activeDestName ) == true )
	{
		parToModify.attrValues[0] = reinterpret_cast<void*>(const_cast<char *>( activeDestName.c_str()));

		//Modify the attribute.
		if( modifyRuntimeObj( objName, &parToModify) != ACS_CC_SUCCESS )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying activeDestination in updateRuntime ");
			AES_CDH_TRACE_MESSAGE("Error occured while modifying activeDestination in updateRuntime ");
			delete[] parToModify.attrValues;
			parToModify.attrValues = 0;
			return ACS_CC_SUCCESS; //Return success here otherwise user wont be able to see other attributes as well.
		}
	}
	if(parToModify.attrValues[0] != NULL)
	{
		delete[] parToModify.attrValues;
		parToModify.attrValues = 0;
	}
	return ACS_CC_SUCCESS;
}

void AES_CDH_BlockDestSetCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
							       ACS_APGCC_InvocationType invocation, 
							       const char* p_objName, 
							       ACS_APGCC_AdminOperationIdType operationId,  
							       ACS_APGCC_AdminOperationParamType**paramList)
{
	(void)paramList;
	AES_CDH_TRACE_MESSAGE("           adminOperationCallback invocated             ");
	AES_CDH_LOG(LOG_LEVEL_INFO, "admin operation invoked");
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
			AES_CDH_TRACE_MESSAGE("In Admin call back , stringValue = %s",stringValue);
			/*insert parameter into the vector*/
			//vectorOut.push_back(firstElem);
			vectorOut.push_back(secondElem);
			delete resobj;
			resobj = NULL;

		}
	}
	}
	ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError,vectorOut);
	AES_CDH_TRACE_MESSAGE("result admin op :%d",rc);
}

void AES_CDH_BlockDestSetCmdHandler::shutdown()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "shutdown started");
	m_poReactor->end_reactor_event_loop();

	if( m_isClassImplAdded == true )
	{
		for (int i =0; i < 10; ++i)
		{
			if( m_oiHandler->removeClassImpl(this, m_className.c_str()) == ACS_CC_FAILURE )
			{
				int intErr = getInternalLastError();
				if( intErr == -6)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if ( i >= 10)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while removing class implementer after waiting, ErrCode = %d", intErr);
						break;
					}
					else
						continue;
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while removing class implementer, ErrCode = %d", intErr);
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
	if(!m_isClassImplAdded)
			AES_CDH_LOG(LOG_LEVEL_INFO,"Removed class implementer for class %s successfully !!",m_className.c_str());
	this->wait();
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
	ROUTINE: extractDestNmFromRdn
=================================================================== */
void AES_CDH_BlockDestSetCmdHandler::extractNameFromRdn( const string rdnName,
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

void AES_CDH_BlockDestSetCmdHandler::extractNameFromDn (const string dnName, string& name)
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


bool AES_CDH_BlockDestSetCmdHandler::isValidName(string name)
{
	bool isNameValid = true;
	const string legalChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	const string legalChars2("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (name.empty())
	{
		isNameValid = false;
		AES_CDH_LOG(LOG_LEVEL_ERROR, "The %s is Empty",name.c_str());
		return isNameValid;
	}
	if( name.length() < 1  || name.length() > 32 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "The length of the %s exceeds limit",name.c_str());
		isNameValid = false;
		return isNameValid;
	}
	if (name.find_first_not_of(legalChars) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "%s contains invalid characters",name.c_str());
		isNameValid = false;
		return isNameValid;
	}
	if (name.substr(0,1).find_first_not_of(legalChars2) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "%s starts with number",name.c_str());
		isNameValid = false;
		return isNameValid;
	}
	return isNameValid;
}
/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void AES_CDH_BlockDestSetCmdHandler::setErrorCode(unsigned int code,const char* objName)
{
	AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(code,"");
	string ertxt=resobj->errorText();
	AES_CDH_TRACE_MESSAGE("setExitCode : exitCode = %d , errorText = %s",code,ertxt.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO, "setExitCode : exitCode = %d , errorText = %s",code,ertxt.c_str());
	if( code == AES_CDH_RC_UNREAS || code == AES_CDH_RC_UNREAS_BLOCKDESTSETNAME )
	{
		if( objName != 0 )
		{
			ertxt = ertxt  + " in " + objName;
		}
	}

	setExitCode(code,ertxt );
	delete resobj;
}
