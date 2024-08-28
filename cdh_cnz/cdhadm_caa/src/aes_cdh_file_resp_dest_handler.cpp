/*=================================================================== */
/**
   @file aes_cdh_file_resp_dest_handler.cpp

   Class method implementationn for CDH module.

   This module contains the implementation of class declared in
   the aes_cdh_file_resp_dest_handler.h file.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/11/2011   XTANAGG   Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_cdh_file_resp_dest_handler.h>
#include <ace/Task.h>
#include <acs_apgcc_omhandler.h>
#include <aes_cdh_result_r1.h>
#include <ACS_APGCC_Util.H>
#include <aes_gcc_util.h>
#include <servr.h>
#include <transdest.h>
#include <aes_cdh_result_r1.h>
#include <aes_cdh_brfc_interface.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
#include <aes_cdh_common.h>

AES_CDH_TRACE_DEFINE(AES_CDH_FileRespDestCmdHandler);
ACE_Recursive_Thread_Mutex AES_CDH_FileRespDestCmdHandler::ccbId_mtx;
CommitInfo AES_CDH_FileRespDestCmdHandler::m_iCommit ;
/*===================================================================
   ROUTINE: AES_CDH_FileRespDestCmdHandler
=================================================================== */
AES_CDH_FileRespDestCmdHandler::AES_CDH_FileRespDestCmdHandler(
		string aDestClassName,
		string aDestImplName,
		ACS_APGCC_ScopeT p_scope )
: acs_apgcc_objectimplementereventhandler_V3(aDestClassName,
		aDestImplName,
		p_scope)
,theClassName(aDestClassName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	m_pcreateDestNode = 0;
	m_isClassImplAdded = false;
	m_DestOIThreadId = 0;
	m_primDestCreateccbId = 0;
	m_secDestCreateccbId = 0;
	ServR::pFileRespDestInfo = 0;
	m_modifyDestList.clear();
	m_deleteDestList.clear();

	/** Create Reactor to handle the events**/
	m_poTp_reactor = new(std::nothrow) ACE_TP_Reactor;
	if( m_poTp_reactor == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Memory Allocation failed for ACE TP Reactor");
	}	
	/** Create Reactor**/
	m_poReactor = new(std::nothrow) ACE_Reactor(m_poTp_reactor);
	if( m_poReactor == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Memory Allocation failed for ACE Reactor");
	}

	// Create OI Handler 
	m_poOiHandler = new(std::nothrow) acs_apgcc_oihandler_V3() ;
	if( m_poOiHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Memory Allocation failed for OI Handler");
	}

	AES_CDH_TRACE_MESSAGE("Leaving");

}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileRespDestCmdHandler::setObjectImplementer()
{
	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;

	AES_CDH_TRACE_MESSAGE("Entering");

	for(int i=0; i < 10; i++)
	{
		errorCode = m_poOiHandler->addClassImpl(this, theClassName.c_str());
		if( errorCode == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, " Error occured while add class implmenter for Destination");
			AES_CDH_TRACE_MESSAGE(" Error occured while add class implmenter for Destination");
			int intErr = getInternalLastError();
			for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
			if ( i >= 10 )
			{
				AES_CDH_LOG(LOG_LEVEL_INFO,  "Error occured while add class implmenter for Destination, ErrCode = %d", intErr);
				AES_CDH_TRACE_MESSAGE("Error occured while add class implmenter for Destination, ErrCode = %d", intErr);
				break;
			}
			else
				continue;
		}
		else
		{
			m_isClassImplAdded = true;
			AES_CDH_LOG(LOG_LEVEL_INFO,"Success: Set implementer for the class %s",theClassName.c_str());
			break;
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileRespDestCmdHandler::create(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *className,
		const char* parentName,
		ACS_APGCC_AttrValues **attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"ObjectCreateCallback invoked");
	(void)oiHandle;
	bool portnumf(false);
	int i = 0;
	ACE_UINT32 notificationPortNo = 0;
	string myDestinationName(""), myNotificationAddr("");
	string myRespDestDn(""), mySecDestDn("") , myAdvRespParamsDn("");
	string myDestSetName("");

	//Parse the arguments
	if( ACE_OS::strcmp( className, AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM ) == 0  )
	{
		AES_CDH_TRACE_MESSAGE("Creating primary responding destination");
		m_primDestCreateccbId = ccbId;
		int ctr = 0;
		extractDestName(parentName, "OBJNAME", myDestinationName);
		AES_CDH_TRACE_MESSAGE(" Create callback %s",myDestinationName.c_str());
		if(!m_iCommit.commitFlag)
		{
			m_iCommit.commitFlag=true;
			m_iCommit.destCommit.assign(myDestinationName);
		}
		else if( m_iCommit.commitFlag && (m_iCommit.destCommit.compare(myDestinationName) != 0))
		{
			m_iCommit.commitFlag=false;
			m_iCommit.destCommit.clear();
			setErrorCode(AES_CDH_RC_COMMITERROR);
			return ACS_CC_FAILURE;
		}

		while( attr[ctr] )
		{
			if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_PRIM_RESP_DESTINATION_ID) == 0 )
			{
				myRespDestDn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE("RDN = %s", myRespDestDn.c_str());
			}
			ctr++;
		}

		while( attr[i] )
		{

			if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_RESP_NOTIFICATION_ADDR) == 0 )
			{
				if( attr[i]->attrValuesNum != 0 )
				{
					char *mytmp = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					if( mytmp != 0 )
					{
						AES_CDH_TRACE_MESSAGE(" Local Notification Addr = %s", mytmp);
						myNotificationAddr.assign(mytmp);

						int validationResult = AES_CDH_Common_Util::isIPAddressValid(myNotificationAddr.c_str());	//IPv6_feature:: validate IPv4 and IPv6 formats
						if (validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
						{
							AES_CDH_LOG(LOG_LEVEL_ERROR, " Notification Address is not a valid IP Address");
							AES_CDH_TRACE_MESSAGE("Leaving ... ");
							//set error code for invalid value
							string objNm = myRespDestDn + "," + parentName;
							ccbId_mtx.acquire();
							if(ServR::pFileRespDestInfo != 0)
							{
								for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
								{
									delete[] ServR::pFileRespDestInfo->argv[ctr];
									ServR::pFileRespDestInfo->argv[ctr] = 0;
								}
								ServR::pFileRespDestInfo->argc=0;
								delete ServR::pFileRespDestInfo;
								ServR::pFileRespDestInfo = 0;
							}
							ccbId_mtx.release();
							if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
								setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6 , objNm.c_str());
							else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
								setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE , objNm.c_str());
							else
								setErrorCode(AES_CDH_RC_UNREAS_RESPIP , objNm.c_str());
							return ACS_CC_FAILURE;
						}
					}
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_RESP_NOTIFICATION_PORT_NO) == 0 )
			{
				if(attr[i]->attrValuesNum !=0 )
				{
					portnumf=true;
					ACE_UINT32 mytmp = *reinterpret_cast<ACE_UINT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Notification Port No = %d", mytmp);
					if(mytmp > 65535 )
					{
						AES_CDH_LOG(LOG_LEVEL_INFO, "Notification Port No is out of range");
						AES_CDH_TRACE_MESSAGE("Leaving");
						//set error code for invalid value
						string objNm = myRespDestDn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_RESPPORTNO , objNm.c_str());
						ccbId_mtx.acquire();
						if(ServR::pFileRespDestInfo != 0)
						{
							for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
							{
								delete[] ServR::pFileRespDestInfo->argv[ctr];
								ServR::pFileRespDestInfo->argv[ctr] = 0;
							}
							ServR::pFileRespDestInfo->argc=0;
							delete ServR::pFileRespDestInfo;
							ServR::pFileRespDestInfo = 0;
						}
						ccbId_mtx.release();
						return ACS_CC_FAILURE;
					}
					notificationPortNo = mytmp;

				}
			}
			i++;
		}

		//Extract the destination name from the RDN.
		extractDestName( myRespDestDn,"RDN",myDestinationName);

		// change the destination name from lower case to upper case

		//Check if Destination Name is valid or not.
		if( isRespDestNameValid( myDestinationName ) == false )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Destination Name contains invalid characters ");
			AES_CDH_TRACE_MESSAGE("Leaving");
			//set error code for invalid value
			string objNm = myRespDestDn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS_PRIRESPDESTNAME , objNm.c_str());
			ccbId_mtx.acquire();
			if(ServR::pFileRespDestInfo != 0)
			{
				for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileRespDestInfo->argv[ctr];
					ServR::pFileRespDestInfo->argv[ctr] = 0;
				}
				ServR::pFileRespDestInfo->argc=0;
				delete ServR::pFileRespDestInfo;
				ServR::pFileRespDestInfo = 0;
			}
			ccbId_mtx.release();
			return ACS_CC_FAILURE;
		}
		//get the destination set name.
		extractDestName(parentName, "OBJNAME", myDestSetName);

		if(ServR::pFileRespDestInfo == 0)
		{
			ServR::pFileRespDestInfo = new(std::nothrow) destInfo;
			if(ServR::pFileRespDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pFileRespDestInfo->argc = 0;
			//Copy the name of the command.
			ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], AES_CDH_CDHDEFINE);
		}
		ServR::pFileRespDestInfo->ccbId = ccbId;
		ServR::pFileRespDestInfo->destinationName = myDestinationName;
		ServR::pFileRespDestInfo->isSecDest = false;
		ServR::pFileRespDestInfo->isDestSet = false;
		ServR::pFileRespDestInfo->destinationSetName =myDestSetName; 
		ServR::pFileRespDestInfo->transferType = AES_CDH_SFTPV2_TRANSFER_TYPE;

		// Copy connection type
		ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[3];
		ACE_OS::strcpy( ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], "-c");
		ServR::pFileRespDestInfo->argv[ ServR::pFileRespDestInfo->argc] = new char[3];
		ACE_OS::strcpy( ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], "r");

		// Copy the notification address.
		if( !myNotificationAddr.empty())
		{
			ServR::pFileRespDestInfo->argv[ ServR::pFileRespDestInfo->argc] = new char[3];
			ACE_OS::strcpy( ServR::pFileRespDestInfo->argv[ ServR::pFileRespDestInfo->argc++], "-f");
			ServR::pFileRespDestInfo->argv[ ServR::pFileRespDestInfo->argc] = new char[myNotificationAddr.length() + 1];		// +1 including null-terminating character
			ACE_OS::strcpy( ServR::pFileRespDestInfo->argv[ ServR::pFileRespDestInfo->argc++], myNotificationAddr.c_str() );
		}
		if( portnumf == true )
		{
			ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], "-x");
			ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[strlen( ACS_APGCC::itoa( notificationPortNo)) + 1]; // +1 including null-terminating character
			ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], ACS_APGCC::itoa( notificationPortNo));
		}

		ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = 0;
		//set error code for success
		setErrorCode(AES_CDH_RC_OK);
		return ACS_CC_SUCCESS;
	}
	else if( ACE_OS::strcmp( className, AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM ) == 0  )
	{
		//Now get the advanced resp parameters from the advancedrespparameters object.
		OmHandler myOmHandler;
		m_secDestCreateccbId = ccbId;

		//Get the AdvancedRespParameters instance and retrieve the values.

		if( myOmHandler.Init() == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, " OmHandler Init failed in create ");
			AES_CDH_TRACE_MESSAGE("OmHandler Init failed in create ");
			AES_CDH_TRACE_MESSAGE(" Leaving");
			//set error code for internal error
			setErrorCode(AES_CDH_RC_COMMITERROR);
			return ACS_CC_FAILURE;
		}
		AES_CDH_TRACE_MESSAGE("Creating secondary responding destination");
		int ctr = 0;
		string myDestinationName;
		extractDestName(parentName, "OBJNAME", myDestinationName);
		AES_CDH_TRACE_MESSAGE("Create callback %s",myDestinationName.c_str());
		if(!m_iCommit.commitFlag)
		{
			m_iCommit.commitFlag=true;
			m_iCommit.destCommit.assign(myDestinationName);
		}
		else if( m_iCommit.commitFlag && (m_iCommit.destCommit.compare(myDestinationName) != 0))
		{
			m_iCommit.commitFlag=false;
			m_iCommit.destCommit.clear();
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}

		while( attr[ctr] )
		{
			if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_SEC_RESP_DESTINATION_ID) == 0 )
			{
				mySecDestDn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0])); 	
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "RDN = %s", mySecDestDn.c_str());
				AES_CDH_TRACE_MESSAGE("RDN = %s", mySecDestDn.c_str());
				extractDestName( mySecDestDn, "RDN", myDestinationName);

				//Check if Destination Name is valid or not.
				if( isRespDestNameValid( myDestinationName ) == false )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, " Destination Name contains invalid characters ");
					AES_CDH_TRACE_MESSAGE("Destination Name contains invalid characters ");
					AES_CDH_TRACE_MESSAGE("Leaving");
					//set error code for invalid value
					string objNm = mySecDestDn + "," + parentName;
					setErrorCode(AES_CDH_RC_UNREAS_SECRESPDESTNAME , objNm.c_str());
					myOmHandler.Finalize();
					return ACS_CC_FAILURE;
				}

			}
			ctr++;
		}

		while( attr[i] )
		{

			if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_RESP_NOTIFICATION_ADDR) == 0 )
			{
				if( attr[i]->attrValuesNum != 0 )
				{
					char *mytmp = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Local Notification Addr = %s", mytmp);
					myNotificationAddr.assign(mytmp);

					int validationResult = AES_CDH_Common_Util::isIPAddressValid(myNotificationAddr.c_str());	//IPv6_feature:: validate IPv4 and IPv6 formats
					if (validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
					{
						myOmHandler.Finalize();
						AES_CDH_LOG(LOG_LEVEL_ERROR, " Notification Address is not a valid IP Address");
						AES_CDH_TRACE_MESSAGE("Notification Address is not a valid IP Address");
						AES_CDH_TRACE_MESSAGE("Leaving");
						//set error code for invalid value
						string objNm =  mySecDestDn + "," + parentName;
						if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
							setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6 , objNm.c_str());
						else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
							setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE , objNm.c_str());
						else
							setErrorCode(AES_CDH_RC_UNREAS_RESPIP , objNm.c_str());

						return ACS_CC_FAILURE;
					}
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_RESP_NOTIFICATION_PORT_NO) == 0 )
			{
				if(attr[i]->attrValuesNum!=0)
				{
					portnumf=true;
					ACE_UINT32 mytmp = *reinterpret_cast<ACE_UINT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Notification Port No = %d", mytmp);
					if(mytmp > 65535 )
					{
						AES_CDH_LOG(LOG_LEVEL_INFO,  " Notification Port No is out of range");
						AES_CDH_LOG(LOG_LEVEL_TRACE, " Leaving");
						//set error code for invalid value
						string objNm =  mySecDestDn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_RESPPORTNO , objNm.c_str());
						myOmHandler.Finalize();
						return ACS_CC_FAILURE;
					}
					notificationPortNo = mytmp;

				}
			}
			i++;
		}



		//get the destination set name.
		extractDestName(parentName, "OBJNAME", myDestSetName);

		std::vector<std::string>  p_destList;
		std::vector<std::string>  primdestList;
		std::vector<std::string>  secdestList;

		//Get the instances of the PrimaryRespondingDestination.
		myOmHandler.getClassInstances(AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM, primdestList);

		//Get the instances of the SecondaryRespondingDestination.
		myOmHandler.getClassInstances( AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM,
				secdestList);

		for( unsigned int pdestCtr = 0 ; pdestCtr < primdestList.size(); pdestCtr++ )
		{
			size_t pos1 = string(primdestList[pdestCtr]).find_first_of(",");
			if( primdestList[pdestCtr].substr(pos1+1) == parentName )
			{
				p_destList.push_back( primdestList[pdestCtr]);
			}
		}

		for( unsigned int sdestCtr = 0 ; sdestCtr < secdestList.size(); sdestCtr++ )
		{
			size_t pos1 = string(secdestList[sdestCtr]).find_first_of(",");
			if( secdestList[sdestCtr].substr(pos1+1) == parentName )
			{
				p_destList.push_back( secdestList[sdestCtr]);
			}
		}

		for( unsigned int destCtr= 0 ; destCtr < p_destList.size() ; destCtr++)
		{
			string myClassName("");
			// Extract the class name from the object RDN
			extractClassNameFromRdn(p_destList[destCtr], myClassName);
			if( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_SEC_RDN_CL_NM ) == 0 )
			{
				//Reject the creation as secondaryRespondingDestination instance already exists for this destination set.
				AES_CDH_LOG(LOG_LEVEL_INFO, " SecondaryRespondingDestionation already exists for this destination set.");
				AES_CDH_TRACE_MESSAGE("Leaving");				//set error code for sec dest already exists
				setErrorCode(AES_CDH_RC_SECDESTEXIST,parentName);
				myOmHandler.Finalize();
				return ACS_CC_FAILURE;
			}
		}
		std::vector<std::string>  p_rdnList;
		myOmHandler.getChildren(parentName, ACS_APGCC_SUBLEVEL,&p_rdnList);
		string objName("");
		for(unsigned int rdnCntr = 0; rdnCntr < p_rdnList.size(); rdnCntr ++)
		{
			string myClassName("");
			extractClassNameFromRdn(p_rdnList[rdnCntr], myClassName);
			if( myClassName == AES_CDH_ADV_RESP_PARAMS_RDN_CL_NM )
			{
				objName = p_rdnList[rdnCntr];
				break;
			}
		}
		if(objName.empty())
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Om handler could not fetch the advanced resp params object ");
			//Error code if advanced param is not created
			setErrorCode(AES_CDH_RC_NOADVPARAM,parentName);
			myOmHandler.Finalize();
			return ACS_CC_FAILURE;
		}

		ACS_APGCC_ImmObject myAdvRespParamsObj;
		myAdvRespParamsObj.objName = objName;
		//Invoke the getObject method on OmHandler.

		if( myOmHandler.getObject( &myAdvRespParamsObj ) == ACS_CC_FAILURE )
		{

			AES_CDH_LOG(LOG_LEVEL_ERROR, " Unable to get the AdvancedRespParamObj ");
			myOmHandler.Finalize();
			AES_CDH_TRACE_MESSAGE("Leaving");
			//set error code for primary dest doesnt exists
			setErrorCode(AES_CDH_RC_NOPRIDEST);
			myOmHandler.Finalize();
			return ACS_CC_FAILURE;
		}

		//Finalize the OmHandler.
		myOmHandler.Finalize();

		ACE_INT32 myFileOverWrite=2;
		ACE_INT32 myInhibitTQNamePrfx = 2;

		//Proceed if getObject was successful.
		for ( unsigned int ctr = 0; ctr  < myAdvRespParamsObj.attributes.size(); ctr++ )
		{
			if( myAdvRespParamsObj.attributes[ctr].attrName == AES_CDH_RESP_FILE_OVERWRITE )
			{
				myFileOverWrite = *reinterpret_cast<ACE_INT32 *>(myAdvRespParamsObj.attributes[ctr].attrValues[0]);
				AES_CDH_TRACE_MESSAGE("File Overwrite Option is %d", myFileOverWrite);
			}
			if( myAdvRespParamsObj.attributes[ctr].attrName == AES_CDH_REMOVE_TQ_PREFIX )
			{
				myInhibitTQNamePrfx = *reinterpret_cast<ACE_INT32 *>(myAdvRespParamsObj.attributes[ctr].attrValues[0]);
				AES_CDH_TRACE_MESSAGE("Inhibit TQ name prefix Option is %d", myInhibitTQNamePrfx );
			}


		}
		int argc = 0;
		char *argv[32] = { 0 };

		//Copy the name of the command.
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

		// Copy connection type
		argv[argc] = new char[3];
		ACE_OS::strcpy(argv[argc++], "-c");
		argv[argc] = new char[3];
		ACE_OS::strcpy(argv[argc++], "r");

		// Copy the notification address.
		if( !myNotificationAddr.empty())
		{
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-f");
			argv[argc] = new char[strlen( myNotificationAddr.c_str()) + 1];
			ACE_OS::strcpy(argv[argc++], myNotificationAddr.c_str());
		}
		if( portnumf == true )
		{
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-x");
			argv[argc] = new char[strlen( ACS_APGCC::itoa( notificationPortNo)) + 1];
			ACE_OS::strcpy(argv[argc++], ACS_APGCC::itoa( notificationPortNo));
		}
		//Updating fileOverwrite
		argv[argc] = new char[3];
		ACE_OS::strcpy(argv[argc++], "-o");
		if( myFileOverWrite == 1 )
		{
			argv[argc] = new char[strlen(AES_YES)];
			string yes = AES_YES;
			ACE_OS::strcpy( argv[argc++] , yes.c_str() );
		}
		else
		{
			argv[argc] = new char[strlen(AES_NO)];
			string no = AES_NO;
			ACE_OS::strcpy( argv[argc++] , no.c_str() );
		}

		// SupressSubFolderCreate
		//Enabling suppressSubFolderCreation in responding traffic might lead to problems in the traffic mix.
		//So, the attribute made as hidden and the value is set to false always.
		argv[argc] = new char[3];
		ACE_OS::strcpy(argv[argc++], "-g");
		argv[argc] = new char[strlen(AES_NO)+1];
		ACE_OS::strcpy( argv[argc++] , AES_NO );

		//inhibitTQNamePrefix
		argv[argc] = new char[3];
		ACE_OS::strcpy(argv[argc++], "-k");
		if(myInhibitTQNamePrfx == 1 )
		{
			argv[argc] = new char[strlen(AES_YES)];
			string yes = AES_YES;
			ACE_OS::strcpy( argv[argc++] , yes.c_str() );
		}
		else
		{
			argv[argc] = new char[strlen(AES_NO)];
			string no = AES_NO;
			ACE_OS::strcpy( argv[argc++] , no.c_str() );
		}

		argv[argc] = 0;
		//Now create the destination.
		int coded = ServR::define( myDestinationName, AES_CDH_SFTPV2_TRANSFER_TYPE, argc, argv, "", AES_CDH_SEC_RESP_DESTINATION_ID,myDestSetName);
		if(coded!= AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining destination");
			AES_CDH_TRACE_MESSAGE("Error occured while defining destination");
			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			//set error code for failure of define
			string objNm =  mySecDestDn + "," + parentName;
			setErrorCode(coded, objNm.c_str());
			ccbId_mtx.acquire();
			if(ServR::pFileRespDestInfo != 0)
			{
				for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileRespDestInfo->argv[ctr];
					ServR::pFileRespDestInfo->argv[ctr] = 0;
				}
				ServR::pFileRespDestInfo->argc=0;
				delete ServR::pFileRespDestInfo;
				ServR::pFileRespDestInfo = 0;
			}
			ccbId_mtx.release();
			return ACS_CC_FAILURE;
		}
		for ( int ctr = 0; ctr < argc ; ctr++)
		{
			delete[] argv[ctr];
			argv[ctr] = 0;
		}
		//add secondary dest to destset
		string primaryDest(""), backupDest(""), userName(""), myDetailInfo("");
		string destSetName = myDestSetName;

		int codeds= ServR::changeDestSet(destSetName, primaryDest, myDestinationName, backupDest, userName);
		if(codeds!= AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Error occured while secondary ot destset");
			AES_CDH_TRACE_MESSAGE("Error occured while secondary ot destset");
			//set error code for failure of changeDestSet
			setErrorCode(codeds, parentName);
			ServR::deleteDest(myDestinationName, myDetailInfo,  userName );
			ccbId_mtx.acquire();
			if(ServR::pFileRespDestInfo != 0)
			{
				for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileRespDestInfo->argv[ctr];
					ServR::pFileRespDestInfo->argv[ctr] = 0;
				}
				ServR::pFileRespDestInfo->argc=0;
				delete ServR::pFileRespDestInfo;
				ServR::pFileRespDestInfo = 0;
			}
			ccbId_mtx.release();
			return ACS_CC_FAILURE;
		}
		if(ServR::pFileRespDestInfo==0)
		{
			ServR::pFileRespDestInfo = new(std::nothrow) destInfo;
			if(ServR::pFileRespDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pFileRespDestInfo->argc = 0; //HY73279
		}

		ServR::pFileRespDestInfo->ccbId = ccbId;
		ServR::pFileRespDestInfo->destinationName = myDestinationName;
		ServR::pFileRespDestInfo->isPrimDest = false;
		ServR::pFileRespDestInfo->isSecDest = true;
		ServR::pFileRespDestInfo->isDestSet = false;
		ServR::pFileRespDestInfo->destinationSetName = destSetName;
		ServR::pFileRespDestInfo->transferType = AES_CDH_SFTPV2_TRANSFER_TYPE;
	}
	else if( ACE_OS::strcmp( className, AES_CDH_ADV_RESP_PARAMS_CLASS_NM ) == 0  )
	{
		AES_CDH_TRACE_MESSAGE("Creating advanced responding parameters object");
		int ctr = 0;
		string myDestinationName;
		extractDestName(parentName, "OBJNAME", myDestinationName);
		AES_CDH_TRACE_MESSAGE("Create callback %s",myDestinationName.c_str());
		if(!m_iCommit.commitFlag)
		{
			m_iCommit.commitFlag=true;
			m_iCommit.destCommit.assign(myDestinationName);
		}
		else if( m_iCommit.commitFlag && (m_iCommit.destCommit.compare(myDestinationName) != 0))
		{
			m_iCommit.commitFlag=false;
			m_iCommit.destCommit.clear();
			setErrorCode(AES_CDH_RC_COMMITERROR);
			return ACS_CC_FAILURE;
		}

		if(ServR::pFileRespDestInfo == 0)
		{
			ServR::pFileRespDestInfo = new(std::nothrow) destInfo;
			if(ServR::pFileRespDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pFileRespDestInfo->argc = 0;
			//Copy the name of the command.
			ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], AES_CDH_CDHDEFINE);

		}
		//Enabling suppressSubFolderCreation in responding traffic might lead to problems in the traffic mix.
		//So, the attribute made as hidden and the value is set to false always.
		ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[3];
		ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], "-g");
		ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[ACE_OS::strlen(AES_NO)+1];
		ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++],AES_NO);
		while( attr[ctr] )
		{
			if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_ADV_RESP_PARAM_ID) == 0 )
			{
				myAdvRespParamsDn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE( " RDN = %s", myAdvRespParamsDn.c_str());
			}
			ctr++;
		}
		while( attr[i] )
		{
			if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_RESP_FILE_OVERWRITE) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
				AES_CDH_TRACE_MESSAGE("File Overwrite = %d", mytmp);
				if((mytmp < 0 || mytmp > 1 ))
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "File Overwrite is invalid");
					AES_CDH_TRACE_MESSAGE("File Overwrite is invalid");
					AES_CDH_TRACE_MESSAGE("Leaving");
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_UNREAS_FILEOVERWRITE , objName.c_str());
					ccbId_mtx.acquire();
					if(ServR::pFileRespDestInfo != 0)
					{
						for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
						{
							delete[] ServR::pFileRespDestInfo->argv[ctr];
							ServR::pFileRespDestInfo->argv[ctr] = 0;
						}
						delete ServR::pFileRespDestInfo;
						ServR::pFileRespDestInfo = 0;
					}
					ccbId_mtx.release();
					return ACS_CC_FAILURE;
				}
				ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[3];
				ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], "-o");
				switch( mytmp )
				{
				case 0:
					ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++],AES_NO);
					break;

				case 1:
					ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++],AES_YES);
					break;
				}


			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_REMOVE_TQ_PREFIX) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
				AES_CDH_TRACE_MESSAGE("Remove file prefix = %d", mytmp );
				if((mytmp < 0 || mytmp > 1 ))
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, " Remove file prefix is invalid");
					AES_CDH_TRACE_MESSAGE("Leaving");
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_UNREAS_INHIBITTQNAMEPRFX , objName.c_str());
					ccbId_mtx.acquire();
					if(ServR::pFileRespDestInfo != 0)
					{
						for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
						{
							delete[] ServR::pFileRespDestInfo->argv[ctr];
							ServR::pFileRespDestInfo->argv[ctr] = 0;
						}
						delete ServR::pFileRespDestInfo;
						ServR::pFileRespDestInfo = 0;
					}
					ccbId_mtx.release();
					return ACS_CC_FAILURE;
				}
				ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[3];
				ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++], "-k");
				switch( mytmp )
				{
				case 0:
					ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++],AES_NO);
					break;

				case 1:
					ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(ServR::pFileRespDestInfo->argv[ServR::pFileRespDestInfo->argc++],AES_YES);
					break;
				}

			}
			i++;
		}
		string destinationName("");
		extractDestName(myAdvRespParamsDn, "RDN", destinationName);

		if ( isRespDestNameValid(destinationName) == false)
		{
			AES_CDH_TRACE_MESSAGE("Leaving  AES_CDH_FileRespDestCmdHandler::create since the Advanced destination parameters dn name is invalid");
			//set error code for invalid value
			ccbId_mtx.acquire();
			if( ServR::pFileRespDestInfo != 0 )
			{
				for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileRespDestInfo->argv[ctr];
					ServR::pFileRespDestInfo->argv[ctr] = 0;
				}
				delete ServR::pFileRespDestInfo;
				ServR::pFileRespDestInfo = 0;
			}
			ccbId_mtx.release();

			//set error code for invalid value
			string objNm = myAdvRespParamsDn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS, objNm.c_str());
			return ACS_CC_FAILURE;
		}
	}
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;

}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileRespDestCmdHandler::deleted( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName)
{
	ACE_Time_Value tv(0,10);
	ACE_OS::sleep(tv);
	AES_CDH_TRACE_MESSAGE(" Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"ObjectDeleteCallback invoked for object : %s",objName);

	ServR::respdestsetMutex.acquire();

	if( ServR::isRespDestSetDeleteTrig == true )
	{
		if( ServR::isDelOfRespDestSetSuccess == true )
		{
			ServR::respdestsetMutex.release();
			//set error code for success
			setErrorCode(AES_CDH_RC_OK);
			return ACS_CC_SUCCESS ;
		}
		else
		{
			ServR::respdestsetMutex.release();
			//set error code for internal error
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}
	}

	string myDestinationName(""), myUserName(""), myDetailInfo("");
	string myClassName("");
	string destSetName("");
	TransDestSet* pdestset=0;
	(void) oiHandle;
	(void) ccbId;

	// Extract the class name from the object RDN
	extractClassNameFromRdn(objName, myClassName);


	size_t pos1 = string(objName).find_first_of(",");
	string destSetRDN("");
	if( pos1 != string::npos )
	{
		destSetRDN = string(objName).substr(pos1+1);
		extractDestName(destSetRDN, "OBJNAME",  destSetName);
	}

	if( ACE_OS::strcasecmp( myClassName.c_str(), AES_CDH_ADV_RESP_PARAMS_RDN_CL_NM ) == 0  )
	{
		//Check whether destset is removed or only Advanced is getting removed
		int code = ServR::getDestSet(destSetName, pdestset);
		if (code== AES_CDH_RC_NODESTSET)//destset removed
		{
			ServR::respdestsetMutex.release();
			//set error code for success
			setErrorCode(AES_CDH_RC_OK);
			return ACS_CC_SUCCESS;
		}
		else
		{
			ServR::respdestsetMutex.release();
			//set error code for invlid deletion
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}

	}
	else if( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_PRIM_RDN_CL_NM ) == 0 
			|| ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_SEC_RDN_CL_NM ) == 0 )
	{
		extractDestName(objName, "OBJNAME", myDestinationName);

		string detailInfo(""), userName("");

		AES_CDH_ResultCode resultCode = AES_CDH_RC_OK;

		//remove secondary from destset
		if ( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_SEC_RDN_CL_NM ) == 0 )
		{
			//Check whether destset is removed or only secondary is getting removed
			int code1 = ServR::getDestSet(destSetName, pdestset);
			if(code1!= AES_CDH_RC_NODESTSET)
			{
				//remove secondary from destset
				resultCode = ServR::deleteDestSet(destSetName, detailInfo, myDestinationName, userName);                 
				if ( resultCode != AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, " Error occured while removing dest %s from destset %s", myDestinationName.c_str(), destSetName.c_str());
					AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
					ServR::respdestsetMutex.release();
					//set error code for failure of deleteDestSet
					setErrorCode(resultCode);
					return ACS_CC_FAILURE;
				}

				vector<AES_CDH_Destination::destAttributes> secDestAttrs;
				resultCode = ServR::listDest( myDestinationName, secDestAttrs );
				if(  resultCode == AES_CDH_RC_OK )
				{
					destNode deleteDestNode;
					deleteDestNode.argc = 0;
					deleteDestNode.destinationName = myDestinationName;
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

				resultCode = ServR::deleteDest(myDestinationName, myDetailInfo,  myUserName );

				if ( resultCode != AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while deleting destination");
					AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
					//add secondary dest to destset
					string primaryDest(""), backupDest("");
					ServR::changeDestSet(destSetName, primaryDest, myDestinationName, backupDest, userName);

					//Delete the m_deleteDestList.
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

					ServR::respdestsetMutex.release();
					//set error code for failure of deleteDest
					setErrorCode(resultCode);
					return ACS_CC_FAILURE;
				}
			}
		}
		else
		{
			resultCode = ServR::getDestSet(destSetName, pdestset);
			if(resultCode!= AES_CDH_RC_NODESTSET)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, " Error occured while deleting primary destination");
				AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
				ServR::respdestsetMutex.release();
				//set error code for invalid deletion
				setErrorCode(AES_CDH_RC_PROTECTEDDEST);
				return ACS_CC_FAILURE;
			}
			resultCode = ServR::deleteDest(myDestinationName, myDetailInfo,  myUserName );

			if ( resultCode != AES_CDH_RC_OK )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while deleting destination");
				AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
				ServR::respdestsetMutex.release();
				//set error code for failure of deleteDest
				setErrorCode(resultCode);
				return ACS_CC_FAILURE;
			}
			AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode );
		}
	}
	ServR::respdestsetMutex.release();
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileRespDestCmdHandler::modify(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName,
		ACS_APGCC_AttrModification **attrMods)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbModifyCallback invoked");

	(void)oiHandle;
	(void)ccbId;

	string myDestinationName(""), myNotificationAddr(""), myClassName("");

	// Extract the class name from the object RDN
	extractClassNameFromRdn(objName, myClassName);

	if( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_PRIM_RDN_CL_NM ) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Modifying Primary Responding Destination.");

		//Extract the destination name from the RDN.
		extractDestName( objName, "OBJNAME", myDestinationName);

		//Get the current values for the primary responding destination.


		int argc = 0, i = 0;
		char *argv[32] = { 0 };

		//Copy the name of the command.

		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

		//Parse the arguments

		while( attrMods[i] )
		{
			if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_RESP_NOTIFICATION_ADDR) == 0 )
			{
				if(attrMods[i]->modAttr.attrValuesNum !=0 )
				{
					char *mytmp = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Local Notification Addr = %s", mytmp);
					myNotificationAddr.assign(mytmp);

					int validationResult = AES_CDH_Common_Util::isIPAddressValid(myNotificationAddr.c_str());	//IPv6_feature:: validate IPv4 and IPv6 formats
					if (validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
					{
						for ( int ctr = 0; ctr < argc ; ctr++)
						{
							delete[] argv[ctr];
							argv[ctr] = 0;
						}

						AES_CDH_LOG(LOG_LEVEL_ERROR, " Notification Address is not a valid IP Address");
						AES_CDH_TRACE_MESSAGE("Leaving");
						//set error code for invalid value
						if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
							setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6 , objName);
						else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
							setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE , objName);
						else
							setErrorCode(AES_CDH_RC_UNREAS_RESPIP , objName);
						return ACS_CC_FAILURE;
					}
					//Create the command.
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-f");
					argv[argc] = new char[ACE_OS::strlen(myNotificationAddr.c_str())+1];
					ACE_OS::strcpy(argv[argc++], myNotificationAddr.c_str());
				}
				else
				{

#if 0
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-f");
					argv[argc] = new char[ACE_OS::strlen(myNotificationAddr.c_str())+1];
					ACE_OS::strcpy(argv[argc++], myNotificationAddr.c_str());
#endif
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					AES_CDH_LOG(LOG_LEVEL_ERROR, " Notification Address should not be deleted once set");
					AES_CDH_TRACE_MESSAGE("Leaving");
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_REMOVE_IPNOTIF, objName);
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_RESP_NOTIFICATION_PORT_NO) == 0 )
			{
				if(attrMods[i]->modAttr.attrValuesNum !=0 )
				{
					ACE_UINT32 mytmp = *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Notification Port No = %d", mytmp);
					if(mytmp > 65535 )
					{
						for ( int ctr = 0; ctr < argc ; ctr++)
						{
							delete[] argv[ctr];
							argv[ctr] = 0;
						}

						AES_CDH_LOG(LOG_LEVEL_ERROR,  " Notification Port No is out of range");
						AES_CDH_TRACE_MESSAGE("Leaving");
						//set error code for invalid value
						setErrorCode(AES_CDH_RC_UNREAS_RESPPORTNO , objName);
						return ACS_CC_FAILURE;
					}
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-x");
					argv[argc] = new char[ACE_OS::strlen(ACS_APGCC::itoa(mytmp))];
					ACE_OS::strcpy(argv[argc++], ACS_APGCC::itoa(mytmp));

				}
				else
				{
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					AES_CDH_LOG(LOG_LEVEL_ERROR,  "port should not be deleted once set");
					AES_CDH_TRACE_MESSAGE("Leaving");
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_REMOVE_PORT, objName);
					return ACS_CC_FAILURE;
				}
			}
			i++;
		}
		AES_CDH_ResultCode resultCode  = AES_CDH_RC_OK;

		//Save the old values before modify.

		vector<AES_CDH_Destination::destAttributes> attrs;
		if( (resultCode = ServR::listDest(myDestinationName, attrs ) ) != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifcation of destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
			//set error code for failure of changeAttr
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;
		}


		//Save the data to abort the modification in case of abort() callback.

		destNode destModStruct;

		destModStruct.ccbId  = ccbId;
		destModStruct.destinationName = myDestinationName;

		destModStruct.argc = 0;

		destModStruct.argv[destModStruct.argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( destModStruct.argv[destModStruct.argc++], AES_CDH_CDHCHANGE);

		char *tmpArgv[32] = {0};
		int tmpArgc = 0;

		if( getExistingDestObjFromIMM( objName, tmpArgc, tmpArgv ) == false )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, " Error occured while modifying destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			for( int ctr = 0; ctr < tmpArgc ; ctr++)
			{
				delete[] tmpArgv[ctr];
				tmpArgv[ctr] = 0;
			}
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;

		}
		for( int i = 1; i < argc ;  i=i+2)
		{
			for ( int j = 0; j < tmpArgc ; j=j+2)
			{
				if( strcmp(argv[i], tmpArgv[j]) == 0 )
				{
					destModStruct.argv[destModStruct.argc] = new char[strlen(argv[i]) + 1];
					strcpy( destModStruct.argv[destModStruct.argc++] , argv[i] );

					destModStruct.argv[destModStruct.argc] = new char[strlen(tmpArgv[j+1]) + 1];
					strcpy( destModStruct.argv[destModStruct.argc++], tmpArgv[j+1]);
				}
			}
		}
		destModStruct.argv[destModStruct.argc] = 0;

		m_modifyDestList.push_back(destModStruct);

		for( int ctr = 0; ctr < tmpArgc ; ctr++)
		{
			delete[] tmpArgv[ctr];
			tmpArgv[ctr] = 0;
		}

		//Invoke modify here.
		string strUserGroup("");
		argv[argc] = 0;


		// Change the destination name from lower to upper case

		AES_CDH_TRACE_MESSAGE(" Destination Name = %s", myDestinationName.c_str());

		//Now modify the destination.
		resultCode = ServR::changeAttr( myDestinationName, argc, argv, strUserGroup);

		if( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while validating the modifcation of destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			vector <destNode>::iterator itr;
			itr = m_modifyDestList.begin();
			while( itr  !=  m_modifyDestList.end() )
			{
				if( (*itr).destinationName == myDestinationName )
				{
					for( int ctr = 0; ctr < (*itr).argc ; ctr++)
					{
						delete[] (*itr).argv[ctr];
						(*itr).argv[ctr] = 0;
					}
					m_modifyDestList.erase(itr);
					itr = m_modifyDestList.begin();
				}
				else
				{
					++itr;
				}

			}

			AES_CDH_TRACE_MESSAGE("Leaving with error code %d", resultCode);
			//set error code for failure of changeAttr
			setErrorCode(resultCode, objName);
			return ACS_CC_FAILURE;
		}

		//Copy the contents in a structure and modify in apply.

		for ( int ctr = 0; ctr < argc ; ctr++)
		{
			delete[] argv[ctr];
			argv[ctr] = 0;
		}


	}
	else if( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_SEC_RDN_CL_NM ) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Modifying secondaryRespondingDestination ");
		int argc = 0, i = 0;
		char *argv[32] = { 0 };

		//Parse the arguments
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

		while( attrMods[i] )
		{
			if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_RESP_NOTIFICATION_ADDR) == 0 )
			{
				if(attrMods[i]->modAttr.attrValuesNum !=0 )
				{
					char *mytmp = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Local Notification Addr = %s", mytmp);
					myNotificationAddr.assign(mytmp);

					int validationResult = AES_CDH_Common_Util::isIPAddressValid(myNotificationAddr.c_str()); //IPv6_feature:: validate IPv4 and IPv6 formats
					if (validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
					{
						for ( int ctr = 0; ctr < argc ; ctr++)
						{
							delete[] argv[ctr];
							argv[ctr] = 0;
						}
						AES_CDH_LOG(LOG_LEVEL_ERROR,  " Notification Address is not a valid IP Address");
						AES_CDH_TRACE_MESSAGE("Leaving");
						//set error code for invalid value
						if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
							setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6 , objName);
						else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
							setErrorCode(AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE , objName);
						else
							setErrorCode(AES_CDH_RC_UNREAS_RESPIP , objName);
						return ACS_CC_FAILURE;
					}
					//Create the command.
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-f");
					argv[argc] = new char[ACE_OS::strlen(mytmp)+1];
					ACE_OS::strcpy(argv[argc++], mytmp);
				}
				else
				{
#if 0
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-f");
					argv[argc] = new char[ACE_OS::strlen(myNotificationAddr.c_str())+1];
					ACE_OS::strcpy(argv[argc++], myNotificationAddr.c_str());
#endif
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					AES_CDH_LOG(LOG_LEVEL_ERROR, " Notification Address should not be deleted once set");
					AES_CDH_TRACE_MESSAGE("Leaving");
					//set error code for invalid value

					setErrorCode(AES_CDH_RC_REMOVE_IPNOTIF, objName);
					return ACS_CC_FAILURE;
				}

			}
			else if( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_RESP_NOTIFICATION_PORT_NO) == 0 )
			{
				if(attrMods[i]->modAttr.attrValuesNum !=0 )
				{
					ACE_UINT32 mytmp = *reinterpret_cast<ACE_UINT32 *>(attrMods[i]->modAttr.attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Notification Port No = %d", mytmp);
					if(mytmp > 65535 )
					{
						for ( int ctr = 0; ctr < argc ; ctr++)
						{
							delete[] argv[ctr];
							argv[ctr] = 0;
						}
						AES_CDH_LOG(LOG_LEVEL_ERROR, " Notification Port No is out of range");
						AES_CDH_TRACE_MESSAGE("Leaving");
						//set error code for invalid value
						setErrorCode(AES_CDH_RC_UNREAS_RESPPORTNO , objName);
						return ACS_CC_FAILURE;
					}
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-x");
					argv[argc] = new char[ACE_OS::strlen(ACS_APGCC::itoa(mytmp))];
					ACE_OS::strcpy(argv[argc++], ACS_APGCC::itoa(mytmp));
				}
				else
				{
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Sec Resp port should not be deleted once set");
					AES_CDH_TRACE_MESSAGE("Leaving");
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_REMOVE_PORT, objName);
					return ACS_CC_FAILURE;
				}
			}
			i++;
		}
		//Invoke modify here.
		AES_CDH_ResultCode resultCode  = AES_CDH_RC_OK;
		string strUserGroup("");
		argv[argc] = 0;

		//Extract the destination name from the RDN.
		extractDestName( objName, "OBJNAME", myDestinationName);

		// Change the destination name from lower to upper case

		AES_CDH_TRACE_MESSAGE("Destination Name = %s", myDestinationName.c_str());

		//Save the data to abort the modification in case of abort() callback.

		vector<AES_CDH_Destination::destAttributes> attrs;
		if( (resultCode = ServR::listDest(myDestinationName, attrs ) ) != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, " Error occured while modifcation of destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
			//set error code for failure of 
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;
		}


		destNode secModStruct;

		secModStruct.ccbId  = ccbId;
		secModStruct.destinationName = myDestinationName;

		secModStruct.argc = 0;

		secModStruct.argv[secModStruct.argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( secModStruct.argv[secModStruct.argc++], AES_CDH_CDHCHANGE);

		int tmpArgc = 0;
		char *tmpArgv[32] = { 0 };

		if( getExistingDestObjFromIMM( objName, tmpArgc, tmpArgv ) == false )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR,  "Error occured while modifying destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			for( int ctr = 0; ctr < tmpArgc ; ctr++)
			{
				delete[] tmpArgv[ctr];
				tmpArgv[ctr] = 0;
			}
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;

		}

		for( int i = 1; i < argc ;  i=i+2)
		{
			for ( int j = 0; j < tmpArgc ; j=j+2)
			{
				if( strcmp(argv[i], tmpArgv[j]) == 0 )
				{
					secModStruct.argv[secModStruct.argc] = new char[strlen(argv[i]) + 1];
					strcpy( secModStruct.argv[secModStruct.argc++] , argv[i] );

					secModStruct.argv[secModStruct.argc] = new char[strlen(tmpArgv[j+1]) + 1];
					strcpy( secModStruct.argv[secModStruct.argc++], tmpArgv[j+1]);
				}
			}
		}
		secModStruct.argv[secModStruct.argc] = 0;

		m_modifyDestList.push_back(secModStruct);

		for( int ctr = 0; ctr < tmpArgc ; ctr++)
		{
			delete[] tmpArgv[ctr];
			tmpArgv[ctr] = 0;
		}


		//Now modify the destination.
		resultCode = ServR::changeAttr( myDestinationName, argc, argv, strUserGroup);

		if( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			vector <destNode>::iterator itr = m_modifyDestList.begin();
			while( itr !=  m_modifyDestList.end() )
			{
				if( (*itr).destinationName == myDestinationName )
				{
					for( int ctr = 0; ctr < (*itr).argc ; ctr++)
					{
						delete[] (*itr).argv[ctr];
						(*itr).argv[ctr] = 0;
					}
					m_modifyDestList.erase(itr);
					itr = m_modifyDestList.begin();
				}
				else
				{
					++itr;
				}
			}

			AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
			//set error code for failure of changeAttr
			setErrorCode(resultCode, objName);
			return ACS_CC_FAILURE;
		}

		for ( int ctr = 0; ctr < argc ; ctr++)
		{
			delete[] argv[ctr];
			argv[ctr] = 0;
		}
	}
	else if( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_ADV_RESP_PARAMS_RDN_CL_NM ) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Modifying advancedRespParameters ");
		int argc = 0, i = 0;
		char *argv[32] = { 0 };

		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

		//Parse the arguments

		while( attrMods[i] )
		{
			if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_RESP_SUPPRESS_CREATE_SUB_FOLDER) == 0 )
			{
				//Enabling suppressSubFolderCreation in responding traffic might lead to problems in the traffic mix.
				//So, the attribute made as hidden and the value is set to false always.
				AES_CDH_LOG(LOG_LEVEL_INFO, "Suppress sub folder creation is hidden  and set to false always");
			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_RESP_FILE_OVERWRITE) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE("File Overwrite = %d", mytmp);
				if((mytmp < 0 || mytmp > 1 ))
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR,  " File Overwrite is invalid");
					AES_CDH_TRACE_MESSAGE("Leaving");
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_UNREAS_FILEOVERWRITE , objName);
					return ACS_CC_FAILURE;
				}
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-o");
				switch( mytmp )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_YES);
					break;
				}


			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_REMOVE_TQ_PREFIX) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE( " Remove file prefix = %d", mytmp);
				if((mytmp < 0 || mytmp > 1 ))
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, " Remove file prefix is invalid");
					AES_CDH_TRACE_MESSAGE("Leaving");
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_UNREAS_INHIBITTQNAMEPRFX , objName);
					return ACS_CC_FAILURE;
				}
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-k");
				switch( mytmp )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_YES);
					break;
				}

			}
			i++;
		}
		if( argc > 0 )
		{
			// Now add the business logic to update both the primary and secondary destinations 		    // with the updated values.
			OmHandler myOmHandler;

			//Initialize the OmHandler.
			if( myOmHandler.Init() == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, " Error occured while initializing OmHandler ");
				AES_CDH_TRACE_MESSAGE("Leaving");
				//set error code for internal error
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}

			size_t pos1 = string(objName).find_first_of(",");
			string destSetRDN("");
			if( pos1 != string::npos )
			{
				destSetRDN = string(objName).substr(pos1+1);
			}
			std::vector<std::string>  p_destList;
			std::vector<std::string>  primdestList;
			std::vector<std::string>  secdestList;

			//Get the instances of the PrimaryRespondingDestination.
			myOmHandler.getClassInstances(AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM, primdestList);

			//Get the instances of the SecondaryRespondingDestination.
			myOmHandler.getClassInstances( AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM,
					secdestList);

			for( unsigned int pdestCtr = 0 ; pdestCtr < primdestList.size(); pdestCtr++ )
			{
				size_t pos1 = string(primdestList[pdestCtr]).find_first_of(",");
				if( primdestList[pdestCtr].substr(pos1+1) == destSetRDN )
				{
					p_destList.push_back( primdestList[pdestCtr]);
				}
			}

			for( unsigned int sdestCtr = 0 ; sdestCtr < secdestList.size(); sdestCtr++ )
			{
				size_t pos1 = string(secdestList[sdestCtr]).find_first_of(",");
				if( secdestList[sdestCtr].substr(pos1+1) == destSetRDN )
				{
					p_destList.push_back( secdestList[sdestCtr]);
				}
			}
			myOmHandler.Finalize();

			for( unsigned int destCtr = 0 ; destCtr < p_destList.size() ; destCtr++ )
			{
				int tempArgc= 0;
				char *tempArgv[32] = { 0 };

				//Copy argc into tempArgc

				tempArgc = argc;

				for ( int tempCtr=0; tempCtr < argc ; tempCtr++)
				{
					tempArgv[tempCtr] = new char[strlen(argv[tempCtr]) +1 ];
					strcpy( tempArgv[tempCtr], argv[tempCtr] );
				}
				tempArgv[tempArgc] = 0;

				//Invoke modify here.
				AES_CDH_ResultCode resultCode  = AES_CDH_RC_OK;
				string strUserGroup("");
				argv[argc] = 0;

				//Extract the destination name from the RDN.
				extractDestName( p_destList[destCtr].c_str(), "OBJNAME", myDestinationName);

				AES_CDH_TRACE_MESSAGE("Destination Name = %s", myDestinationName.c_str());


				AES_CDH_TRACE_MESSAGE("argc = %d", argc);
				for( int j  =0 ; j < tempArgc; j++)
				{
					AES_CDH_TRACE_MESSAGE("argv = %s", tempArgv[j] );
				}
				//Save the data to abort the modification in case of abort() callback.

				destNode destModStruct;

				destModStruct.ccbId  = ccbId;
				destModStruct.destinationName = myDestinationName;

				destModStruct.argc = 0;

				destModStruct.argv[destModStruct.argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
				ACE_OS::strcpy( destModStruct.argv[destModStruct.argc++], AES_CDH_CDHCHANGE);

				char *tmpArgv1[32] = {0};
				int tmpArgc1 = 0;

				if( getExistingAdvDestObjFromIMM( objName, tmpArgc1, tmpArgv1 ) == false )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination");
					for( int ctr = 0; ctr < tempArgc ; ctr++)
					{
						delete[] tempArgv[ctr];
						tempArgv[ctr] = 0;
					}
					for( int ctr = 0; ctr < tmpArgc1 ; ctr++)
					{
						delete[] tmpArgv1[ctr];
						tmpArgv1[ctr] = 0;
					}
					// Delete the main argc and argv variables
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					setErrorCode(AES_CDH_RC_INTPROGERR);
					return ACS_CC_FAILURE;
				}

				for( int i = 1; i < tempArgc ;  i=i+2)
				{
					for ( int j = 0; j < tmpArgc1 ; j=j+2)
					{
						if( strcmp(tempArgv[i], tmpArgv1[j]) == 0 )
						{
							destModStruct.argv[destModStruct.argc] = new char[strlen(tempArgv[i]) + 1];
							strcpy( destModStruct.argv[destModStruct.argc++] , tempArgv[i] );

							destModStruct.argv[destModStruct.argc] = new char[strlen(tmpArgv1[j+1]) + 1];
							strcpy( destModStruct.argv[destModStruct.argc++], tmpArgv1[j+1]);
						}
					}
				}
				for( int ctr = 0; ctr < tmpArgc1 ; ctr++)
				{
					delete[] tmpArgv1[ctr];
					tmpArgv1[ctr] = 0;
				}

				destModStruct.argv[destModStruct.argc] = 0;

				m_modifyDestList.push_back(destModStruct);

				//Now modify the destination.
				resultCode = ServR::changeAttr( myDestinationName, tempArgc, tempArgv, strUserGroup);

				if( resultCode != AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination");
					for( int ctr = 0; ctr < tempArgc ; ctr++)
					{
						if( tempArgv[ctr] != 0 )
						{
							delete[] tempArgv[ctr];
						}
						tempArgv[ctr] = 0;
					}
					vector <destNode>::iterator itr = m_modifyDestList.begin();
					while( itr !=  m_modifyDestList.end())
					{
						if( (*itr).destinationName == myDestinationName )
						{
							for( int ctr = 0; ctr < (*itr).argc ; ctr++)
							{
								delete[] (*itr).argv[ctr];
								(*itr).argv[ctr] = 0;
							}
							m_modifyDestList.erase(itr);
							itr = m_modifyDestList.begin();
						}
						else
						{
							++itr;
						}
					}

					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
					//set error code for failure of changeAttr
					setErrorCode(resultCode, objName);
					return ACS_CC_FAILURE;
				}
				for( int ctr = 0; ctr < tempArgc ; ctr++)
				{
					if( tempArgv[ctr] != 0 )
					{
						delete[] tempArgv[ctr];
					}
					tempArgv[ctr] = 0;
				}
			}

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileRespDestCmdHandler::complete( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbCompleteCallback invoked");
	(void)oiHandle;
	(void)ccbId;
	m_iCommit.commitFlag=false;
	m_iCommit.destCommit.clear();
	if(theClassName.compare(AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM)==0)
	{
		if(m_primDestCreateccbId == ccbId)
		{
			ccbId_mtx.acquire();
			if( ServR::pFileRespDestInfo != 0 )
			{
				string parentName = string(AES_CDH_RESP_DEST_SET_ID) + "=" + ServR::pFileRespDestInfo->destinationSetName + "," + AES_GCC_Util::dnOfFileTransferM;
				string objNm =string(AES_CDH_PRIM_RESP_DESTINATION_ID) + "=" + ServR::pFileRespDestInfo->destinationName + "," + parentName;
				ServR::pFileRespDestInfo->isPrimDest = true;  //HY73279

				//Now create the primary destination.
				int code=ServR::define( ServR::pFileRespDestInfo->destinationName , AES_CDH_SFTPV2_TRANSFER_TYPE, ServR::pFileRespDestInfo->argc, ServR::pFileRespDestInfo->argv, "", AES_CDH_PRIM_RESP_DESTINATION_ID,ServR::pFileRespDestInfo->destinationSetName);
				if(code!= AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining destination");
					AES_CDH_TRACE_MESSAGE("Error occured while defining destination");
					for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
					{
						delete[] ServR::pFileRespDestInfo->argv[ctr];
						ServR::pFileRespDestInfo->argv[ctr] = 0;
					}
					ServR::pFileRespDestInfo->argc = 0;
					delete ServR::pFileRespDestInfo;
					ServR::pFileRespDestInfo=0;
					//set error code for failure of define
					setErrorCode(code,objNm.c_str());
					ccbId_mtx.release();
					return ACS_CC_FAILURE;
				}
				//define destset with this dest as primary
				string secondaryDestName(""), backupDestName(""), userGroup("");
				string myDetailInfo(""), myUserName("");

				int result = ServR::defineDestSet(ServR::pFileRespDestInfo->destinationSetName, ServR::pFileRespDestInfo->destinationName, secondaryDestName,
						backupDestName,userGroup, AES_CDH_RESP_DEST_SET_ID);
				if (result != AES_CDH_RC_OK)
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining responding destinationset ");
					//If destination set creation fails, then remove the primary destination too.
					ServR::deleteDest(ServR::pFileRespDestInfo->destinationName, myDetailInfo,  myUserName );
					if(ServR::pFileRespDestInfo != 0)
					{
						for(int i = 0; i < ServR::pFileRespDestInfo->argc ; i++)
						{
							delete[] ServR::pFileRespDestInfo->argv[i];
							ServR::pFileRespDestInfo->argv[i] = 0;
						}
						ServR::pFileRespDestInfo->argc = 0;
						delete ServR::pFileRespDestInfo;
						ServR::pFileRespDestInfo=0;
					}
					//set error code for failure of defineDestSet
					setErrorCode(result,parentName.c_str());
					ccbId_mtx.release();
					return ACS_CC_FAILURE;
				}
			}
			ccbId_mtx.release();
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void AES_CDH_FileRespDestCmdHandler::abort(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbAbortCallback invoked");
	(void)oiHandle;
	m_primDestCreateccbId = 0;
	m_secDestCreateccbId = 0;
	m_iCommit.commitFlag=false;
	m_iCommit.destCommit.clear();
	//The following code will delete the created destination/destination sets in case create callback
	//is successful and then abort is recieved.
	ccbId_mtx.acquire();
	if( ServR::pFileRespDestInfo != 0 )
	{
		if( ServR::pFileRespDestInfo->ccbId == ccbId )
		{
			//abort has been recived for destination creation.

			if( ServR::pFileRespDestInfo->isPrimDest  == true )
			{
				//Delete the destination set.
				string myDetailInfo(""), myDestinationName(""), myUserName("");

				ServR::deleteDestSet(ServR::pFileRespDestInfo->destinationSetName, myDetailInfo, myDestinationName, myUserName);

				//Delete the destination.
				ServR::deleteDest(ServR::pFileRespDestInfo->destinationName, myDetailInfo,  myUserName );
			}
			else if( ServR::pFileRespDestInfo->isSecDest == true )
			{
				//Delete the destination set association for secondary destination.
				string myDetailInfo(""), myDestinationName(""), myUserName("");

				ServR::deleteDestSet(ServR::pFileRespDestInfo->destinationSetName, myDetailInfo, ServR::pFileRespDestInfo->destinationName, myUserName);

				//Delete the secondary destination.
				ServR::deleteDest(ServR::pFileRespDestInfo->destinationName, myDetailInfo,  myUserName );
			}
		}

		for ( int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++ ) //HY73279
		{
			delete[] ServR::pFileRespDestInfo->argv[ctr];
			ServR::pFileRespDestInfo->argv[ctr] = 0;
		}
		delete ServR::pFileRespDestInfo;
		ServR::pFileRespDestInfo = 0;
	}
	ccbId_mtx.release();
	if( m_modifyDestList.size() > 0 )
	{
		for ( unsigned ctr = 0 ; ctr < m_modifyDestList.size() ; ctr++)
		{
			string strUserGroup("");
			ServR::changeAttr( m_modifyDestList[ctr].destinationName, m_modifyDestList[ctr].argc, m_modifyDestList[ctr].argv, strUserGroup);
			for( int i = 0; i < m_modifyDestList[ctr].argc ; i++)
			{
				delete[] m_modifyDestList[ctr].argv[i];
				m_modifyDestList[ctr].argv[i] = 0;
			}
		}
		//Delete the entire list.

		vector <destNode>::iterator itr = m_modifyDestList.begin();
		while( itr != m_modifyDestList.end())
		{
			m_modifyDestList.erase(itr);
			itr = m_modifyDestList.begin();
		}
	}

	if(  m_deleteDestList.size() > 0 )
	{
		for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
		{
			if( m_deleteDestList[ctr].isSecDest == true )
			{
				//Create the seconadry destination.
				string strUserGroup(""), primDest(""), backupDest(""), userName("");
				ServR::define( m_deleteDestList[ctr].destinationName, m_deleteDestList[ctr].transferType, m_deleteDestList[ctr].argc, m_deleteDestList[ctr].argv, strUserGroup, AES_CDH_SEC_RESP_DESTINATION_ID,m_deleteDestList[ctr].destinationSetName);
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

	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileRespDestCmdHandler::updateRuntime( const char* objName,
		const char** attrName)
{
	(void)objName;
	(void)attrName;
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void AES_CDH_FileRespDestCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{        
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"adminOperationCallback invoked");
	(void)paramList;
	string myClassName("");
	string objName = string(p_objName);
	extractClassNameFromRdn(objName, myClassName);
	string destName;
	string tempDest;
	AES_CDH_TRACE_MESSAGE("Object Name %s", p_objName);

	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	ACS_APGCC_AdminOperationParamType firstElem;

	ACS_APGCC_AdminOperationParamType secondElem;

	if(ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_PRIM_RDN_CL_NM) == 0
			|| ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_FILE_RESP_DEST_SEC_RDN_CL_NM) == 0 )
	{
		bool  myOIValidationError=true;
		AES_CDH_ResultCode result = AES_CDH_RC_OK;
		switch(operationId)
		{
		case 7://manualNotify
		{
			AES_CDH_TRACE_MESSAGE( "In Admin call back , manualNotify, p_objName :%s ",p_objName);
			tempDest=string(p_objName);
			extractDestName(tempDest,"OBJNAME",destName);
			AES_CDH_TRACE_MESSAGE( "Trying for manual notify for destination : %s",destName.c_str());
			vector<ServR::checkConnAttributes> cAttrs;
			bool force   = true; //hard code to true
			result = ServR::checkConnectionManually(destName, force, cAttrs);
			AES_CDH_TRACE_MESSAGE("adminOp manualNotify check connection ,result : %d",result);
			//set error code for result of checkConnection
			//setErrorCode(result);
			if(result == AES_CDH_RC_CONNECTOK)
			{
				myOIValidationError=true;
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "In Admin call back , manualNotification failed ");
				myOIValidationError=false;
				/*create first Elemenet of param list*/
				char attrName1[] = "errorCode";
				//  errorCode = AES_CDH_RC_CONNECTERR;
				firstElem.attrName = attrName1;
				firstElem.attrType = ATTR_INT32T;
				int intValue1 = result;
				//int intValue1 = AES_CDH_RC_CONNECTERR ;
				firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

				/*create second Elemenet of param list*/

				AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(result,"");
				//AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(AES_CDH_RC_CONNECTERR,"");
				string ertxt = "@ComNbi@";
				ertxt += resobj->errorText();

				char attrName2[]= "errorText";

				secondElem.attrName = attrName2;

				secondElem.attrType = ATTR_STRINGT;

				char* stringValue = const_cast<char*>(ertxt.c_str());
				secondElem.attrValues=reinterpret_cast<void*>(stringValue);

				AES_CDH_LOG(LOG_LEVEL_INFO,"In Admin call back , manualNotification failed stringValue = %s", stringValue);
				/*insert parameter into the vector*/
				//vectorOut.push_back(firstElem);
				vectorOut.push_back(secondElem);
				delete resobj;
				resobj = NULL;
			}
		}
		break;
		case 6://check connection
		{
			AES_CDH_TRACE_MESSAGE("In Admin call back , check connection, p_objName :%s ",p_objName);
			tempDest=string(p_objName);
			extractDestName(tempDest,"OBJNAME",destName);
			AES_CDH_TRACE_MESSAGE("Trying to check connection for destination : %s",destName.c_str());
			vector<ServR::checkConnAttributes> cAttrs;
			bool force   = true; //hard code to true
			result = ServR::checkConnection(destName, force, cAttrs);
			AES_CDH_TRACE_MESSAGE("ServR::checkConnection ,result : %d",result);
			//set error code for result of checkConnection
			//setErrorCode(result);
			if(result == AES_CDH_RC_CONNECTOK)
			{
				myOIValidationError=true;

			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "In Admin call back , connectionVerification failed ");
				myOIValidationError=false;
				/*create first Elemenet of param list*/
				char attrName1[] = "errorCode";
				//  errorCode = AES_CDH_RC_CONNECTERR;
				firstElem.attrName = attrName1;
				firstElem.attrType = ATTR_INT32T;
				int intValue1 = AES_CDH_RC_CONNECTERR ;
				firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

				/*create second Elemenet of param list*/

				AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(AES_CDH_RC_CONNECTERR,"");
				string ertxt = "@ComNbi@";
				ertxt += resobj->errorText();

				char attrName2[]= "errorText";

				secondElem.attrName = attrName2;

				secondElem.attrType = ATTR_STRINGT;

				char* stringValue = const_cast<char*>(ertxt.c_str());
				secondElem.attrValues=reinterpret_cast<void*>(stringValue);

				AES_CDH_LOG(LOG_LEVEL_ERROR,"In Admin call back , connectionVerification failed stringValue = %s", stringValue);
				/*insert parameter into the vector*/
				//vectorOut.push_back(firstElem);
				vectorOut.push_back(secondElem);
				delete resobj;

			}
		}
		break;

		default:
			AES_CDH_LOG(LOG_LEVEL_INFO,"In Admin call back , Invalide operation Id, default case");
			break;
		}
		ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError, vectorOut);
		AES_CDH_TRACE_MESSAGE("result admin op :%d",rc);
		AES_CDH_TRACE_MESSAGE("myOIValidationError: %d",myOIValidationError);
		if(rc != ACS_CC_SUCCESS)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO,"Admin op failed , error : %d ",myOIValidationError);
		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Not yet Implemented");
		ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, true, vectorOut);
		AES_CDH_LOG(LOG_LEVEL_INFO,"result admin op :%d",rc);
	}

	AES_CDH_TRACE_MESSAGE("Leaving");


}//End of adminOperationCallback

/*===================================================================
   ROUTINE: apply
=================================================================== */
void AES_CDH_FileRespDestCmdHandler::apply(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbApplyCallback invoked");
	(void)oiHandle;
	(void)ccbId;
	//Delete the creation struct.
	if((m_primDestCreateccbId == ccbId) || (m_secDestCreateccbId == ccbId))  //HY73279
	{
		ccbId_mtx.acquire();
		if(ServR::pFileRespDestInfo != 0)
		{
			for (int ctr = 0; ctr < ServR::pFileRespDestInfo->argc ; ctr++)
        		{
                		delete[] ServR::pFileRespDestInfo->argv[ctr];
                		ServR::pFileRespDestInfo->argv[ctr] = 0;
        		}
			delete ServR::pFileRespDestInfo;
			ServR::pFileRespDestInfo = 0;
		}
		ccbId_mtx.release();
		(m_primDestCreateccbId == ccbId)? (m_primDestCreateccbId = 0):(m_secDestCreateccbId = 0);
	}
	//Delete the modification struct.
	if( m_modifyDestList.size() > 0 )
	{
		for ( unsigned ctr = 0 ; ctr < m_modifyDestList.size() ; ctr++)
		{
			for( int i = 0; i < m_modifyDestList[ctr].argc ; i++)
			{
				delete[] m_modifyDestList[ctr].argv[i];
				m_modifyDestList[ctr].argv[i] = 0;
			}
		}
		//Delete the entire list.

		vector <destNode>::iterator itr = m_modifyDestList.begin();
		while( itr != m_modifyDestList.end())
		{
			m_modifyDestList.erase(itr);
			itr = m_modifyDestList.begin();
		}
	}

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

	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of apply

/*===================================================================
   ROUTINE: DestOIThread
=================================================================== */
ACE_THR_FUNC_RETURN AES_CDH_FileRespDestCmdHandler::DestOIThread(void* aCDHDestPtr)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_FileRespDestCmdHandler * myDestImplementer = 0;
	myDestImplementer = reinterpret_cast<AES_CDH_FileRespDestCmdHandler*>(aCDHDestPtr);

	if(myDestImplementer != 0)
	{
		myDestImplementer->m_poReactor->open(1);

		myDestImplementer->dispatch(myDestImplementer->m_poReactor,
				ACS_APGCC_DISPATCH_ALL) ;

		myDestImplementer->m_poReactor->run_reactor_event_loop();
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "myDestImplementer is NULL");
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return 0;
}//End of DestOIThread

/*===================================================================
   ROUTINE: setupDestOIThread
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileRespDestCmdHandler::setupDestOIThread(AES_CDH_FileRespDestCmdHandler * aDestImplementer, 
		ACE_Thread_Manager* threadManager_)
{
	AES_CDH_TRACE_MESSAGE(" Entering");
	int myGroupId = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	if( aDestImplementer->setObjectImplementer() != ACS_CC_SUCCESS)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "setObjectImplementer failed");
		AES_CDH_TRACE_MESSAGE("Leaving ");
		return ACS_CC_FAILURE;
	}

	AES_CDH_TRACE_MESSAGE("Spawning the  Dest OI Thread");

	myGroupId = threadManager_->spawn(&DestOIThread,
			(void*)aDestImplementer ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&m_DestOIThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	if(myGroupId == -1 )
	{
		AES_CDH_LOG(LOG_LEVEL_INFO,  " Spawning of DestOIThread failed");
		rc = ACS_CC_FAILURE;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rc;

}//End of setupDestThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void AES_CDH_FileRespDestCmdHandler::shutdown()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	m_poReactor->end_reactor_event_loop();
	if( m_isClassImplAdded == true )
	{

		for (int i=0; i < 3; ++i)
		{
			if( m_poOiHandler->removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing class implementer");
				int intErr = getInternalLastError();
				if ( intErr == -6)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if ( i >= 10)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing class implementer, ErrCode = %d", intErr);
						break;
					}
					else
						continue;
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing class implementer, ErrCode = %d", intErr);
					break;
				}
			}
			else
			{
				m_isClassImplAdded = false;
				break;
			}
		}
	}
	if(!m_isClassImplAdded)
		AES_CDH_LOG(LOG_LEVEL_INFO,"Removed class implementer for class %s successfully !!",theClassName.c_str());
	if( m_DestOIThreadId != 0 )
	{
		ACE_Thread_Manager::instance()->join(m_DestOIThreadId);

	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of shutdown

/*===================================================================
	ROUTINE: ~AES_CDH_FileRespDestCmdHandler
=================================================================== */
AES_CDH_FileRespDestCmdHandler::~AES_CDH_FileRespDestCmdHandler()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if( m_poOiHandler != 0 )
	{
		delete m_poOiHandler;
		m_poOiHandler = 0;
	}

	if( m_poReactor != 0)
	{
		delete m_poReactor;
		m_poReactor = 0;
	}

	if( m_poTp_reactor != 0 )
	{
		delete m_poTp_reactor;
		m_poTp_reactor = 0;
	}

	AES_CDH_TRACE_MESSAGE("Leaving");

}//End of Destructor

/*===================================================================
	ROUTINE: extractDestName
=================================================================== */
void AES_CDH_FileRespDestCmdHandler::extractDestName(const string attrName,
		const string attrType,
		string &destinationName)
{

	AES_CDH_TRACE_MESSAGE("Entering");

	destinationName = "";

	if ( attrType == "RDN" )
	{
		string::size_type pos1 = string::npos;

		unsigned int pos2 = attrName.length();

		pos1 = attrName.find_first_of("=");

		if(( pos1 != string::npos) && pos1 < pos2 )
		{
			destinationName = attrName.substr(pos1+1, pos2-pos1-1);
		}
	}
	else if ( attrType == "OBJNAME" )
	{

		string::size_type pos1 = string::npos;
		string::size_type pos2 = string::npos;

		pos1 = attrName.find_first_of("=");
		pos2 = attrName.find_first_of(",");

		if(( pos1 != string::npos) && ( pos2 != string::npos ))
		{
			destinationName = attrName.substr(pos1+1, pos2-pos1-1);
		}
	}
	else if ( attrType == "PARENT" )
	{
		string::size_type pos1 = string::npos;
		string::size_type pos2 = string::npos;
		string::size_type pos3 = string::npos;
		pos3 = attrName.find_first_of("=");
		pos1 = attrName.find_first_of("=", pos3+1);
		pos2 = attrName.find_first_of(",", pos1);

		if(( pos1 != string::npos) && ( pos2 != string::npos ))
		{
			destinationName = attrName.substr(pos1+1, pos2-pos1-1);
		}

	}
	AES_CDH_TRACE_MESSAGE( "Leaving extractDestName = %s", destinationName.c_str());

}

/*===================================================================
        ROUTINE: extractClassNameFromRdn
=================================================================== */
void AES_CDH_FileRespDestCmdHandler::extractClassNameFromRdn( const string myObjRdn,
		string &myClassName)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	myClassName = "";
	string::size_type pos1 = string::npos;

	pos1 = myObjRdn.find_first_of("=");
	if ( pos1 != string::npos )
	{
		myClassName = myObjRdn.substr(0, pos1-2);
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
        ROUTINE: isIPAddrValid
=================================================================== */
bool AES_CDH_FileRespDestCmdHandler::isIPAddrValid( const char * aIPAddr )
{
	AES_CDH_TRACE_MESSAGE("Entering ");

	if ( aIPAddr == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR,"Leaving  with false");
		return false;
	}
	unsigned int b1 = 0, b2 =0, b3 = 0, b4 = 0;
	unsigned char c = 0;
	if( sscanf(aIPAddr, "%3u.%3u.%3u.%3u%c", &b1, &b2, &b3, &b4, &c ) != 4 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving  with false");
		return false;
	}

	if(( b1 | b2 | b3 | b4 ) > 255 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, " Leaving  with false");
		return false;
	}


	if( strspn( aIPAddr, "0123456789.") < ACE_OS::strlen( aIPAddr ))
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving  with false");
		return false;
	}

	AES_CDH_TRACE_MESSAGE("Leaving  with true");
	return true;
}

bool AES_CDH_FileRespDestCmdHandler::isRespDestNameValid( const string& aDestinationName )
{

	AES_CDH_TRACE_MESSAGE("Entering");

	if( aDestinationName.length() < 1  || aDestinationName.length() > 32 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "The length of the destination name exceeds limit");
		AES_CDH_TRACE_MESSAGE("Leaving ");
		return false;
	}
	const string legalChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	const string legalChars2("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (aDestinationName.find_first_not_of(legalChars) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, " Destination name contains invalid characters");
		AES_CDH_TRACE_MESSAGE("Leaving");
		return false;
	}
	if (aDestinationName.substr(0,1).find_first_not_of(legalChars2) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, " Destination name starts with number");
		AES_CDH_TRACE_MESSAGE("Leaving");
		return false;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");

	return true;
}

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void AES_CDH_FileRespDestCmdHandler::setErrorCode(unsigned int code, const char* objName)
{
	AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(code,"");
	string ertxt=resobj->errorText();
	if( objName != 0 )
	{
		if( code  == AES_CDH_RC_UNREAS || code  == AES_CDH_RC_UNREAS_RESPIP || code  == AES_CDH_RC_UNREAS_RESPPORTNO || code  == AES_CDH_RC_UNREAS_FILETRANSFERTYPE
				|| code  == AES_CDH_RC_UNREAS_PRIRESPDESTNAME || code  == AES_CDH_RC_UNREAS_SECRESPDESTNAME || code == AES_CDH_RC_UNREAS_FILEOVERWRITE
				|| code == AES_CDH_RC_UNREAS_SUPPRESSSUBFOLDRCREATION || code == AES_CDH_RC_UNREAS_INHIBITTQNAMEPRFX
				|| code == AES_CDH_RC_REMOVE_PORT || code == AES_CDH_RC_REMOVE_IPNOTIF || code == AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6 || code == AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE)
		{
			string comDn("");
			AES_GCC_Util::extractComDnfromDn(objName,comDn);
			ertxt = ertxt + " in " + comDn;
		}
	}
	AES_CDH_TRACE_MESSAGE("setExitCode : exitCode = %d , errorText = %s",code,ertxt.c_str());
	setExitCode(code,ertxt );
	delete resobj;
}

/*===================================================================
        ROUTINE: getExistingDestObjFromIMM
=================================================================== */
bool AES_CDH_FileRespDestCmdHandler::getExistingDestObjFromIMM( const char *objName, int &tmpArgc, char** tmpArgv )
{
	AES_CDH_TRACE_MESSAGE("Entering");

	OmHandler myOmhandler;

	//Initialize OmHandler
	if( myOmhandler.Init() == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "OmHandler initialization failed.");
		return false;
	}

	//Get the data from IMM.

	ACS_APGCC_ImmObject destObj;
	destObj.objName = objName;

	if( myOmhandler.getObject( &destObj) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting object from IMM");
		myOmhandler.Finalize();
		return false;

	}
	myOmhandler.Finalize();

	//Read the attributes of the advanced initiating object
	for ( unsigned int cntr = 0; cntr  < destObj.attributes.size(); cntr++ )
	{
		if (destObj.attributes[cntr].attrName ==  AES_CDH_RESP_NOTIFICATION_ADDR)
		{
			if(destObj.attributes[cntr].attrValuesNum !=0 )
			{
				string ipNotifAddress("");
				char *mytmp = reinterpret_cast<char *>(destObj.attributes[cntr].attrValues[0]);
				if( mytmp != 0 )
				{
					tmpArgv[tmpArgc] = new char [3] ;
					ACE_OS::strcpy( tmpArgv[tmpArgc++], "-f");

					ipNotifAddress.assign(mytmp);
					tmpArgv[tmpArgc] = new char [strlen(ipNotifAddress.c_str())+1 ] ;
					ACE_OS::strcpy( tmpArgv[tmpArgc++], ipNotifAddress.c_str());
				}
			}
		}
		else if ( destObj.attributes[cntr].attrName == AES_CDH_RESP_NOTIFICATION_PORT_NO)
		{

			if( destObj.attributes[cntr].attrValuesNum != 0)
			{
				unsigned int notificationPortNo = 0;
				notificationPortNo = *reinterpret_cast<unsigned int *>(destObj.attributes[cntr].attrValues[0]);

				tmpArgv[tmpArgc] = new char[3];
				ACE_OS::strcpy( tmpArgv[tmpArgc++] , "-x");

				tmpArgv[tmpArgc] = new char [strlen(ACS_APGCC::itoa( notificationPortNo)) + 1] ;
				ACE_OS::strcpy( tmpArgv[tmpArgc++], ACS_APGCC::itoa( notificationPortNo));
			}
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;
}

/*===================================================================
        ROUTINE: getExistingAdvDestObjFromIMM
=================================================================== */
bool AES_CDH_FileRespDestCmdHandler::getExistingAdvDestObjFromIMM( const char *objName, int &tmpArgc, char** tmpArgv )
{
	AES_CDH_TRACE_MESSAGE("Entering");

	OmHandler myOmhandler;

	//Initialize OmHandler
	if( myOmhandler.Init() == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "OmHandler initialization failed.");
		return false;
	}

	//Get the data from IMM.

	ACS_APGCC_ImmObject destObj;
	destObj.objName = objName;

	if( myOmhandler.getObject( &destObj) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting Adv object from IMM");
		myOmhandler.Finalize();
		return false;

	}
	myOmhandler.Finalize();

	//Read the attributes of the advanced initiating object
	for ( unsigned int cntr = 0; cntr  < destObj.attributes.size(); cntr++ )
	{

		if ( destObj.attributes[cntr].attrName == AES_CDH_RESP_FILE_OVERWRITE)
		{
			int fileOverwrite = 0;
			if (destObj.attributes[cntr].attrValuesNum != 0 )
			{
				fileOverwrite = *reinterpret_cast<ACE_INT32 *>(destObj.attributes[cntr].attrValues[0]);
			}
			// Copy the file overwrite option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-o");

			// Copy the file overwrite option
			switch( fileOverwrite )
			{
			case 0:
				tmpArgv[tmpArgc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
				ACE_OS::strcpy(tmpArgv[tmpArgc++],AES_CDH_INIT_NO);
				break;

			case 1:
				tmpArgv[tmpArgc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
				ACE_OS::strcpy(tmpArgv[tmpArgc++],AES_CDH_INIT_YES);
				break;
			}
		}
		else if ( destObj.attributes[cntr].attrName == AES_CDH_REMOVE_TQ_PREFIX)
		{
			int removeTQPrefix = 0;
			if (destObj.attributes[cntr].attrValuesNum != 0 )
			{
				removeTQPrefix = *reinterpret_cast<ACE_INT32 *>(destObj.attributes[cntr].attrValues[0]);
			}
			// Copy the file removeTQPrefix option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-k");

			// Copy the file overwrite option
			switch( removeTQPrefix )
			{
			case 0:
				tmpArgv[tmpArgc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
				ACE_OS::strcpy(tmpArgv[tmpArgc++],AES_CDH_INIT_NO);
				break;

			case 1:
				tmpArgv[tmpArgc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
				ACE_OS::strcpy(tmpArgv[tmpArgc++],AES_CDH_INIT_YES);
				break;
			}

		}
		else if ( destObj.attributes[cntr].attrName ==  AES_CDH_RESP_SUPPRESS_CREATE_SUB_FOLDER )
		{
			//Enabling suppressSubFolderCreation in responding traffic might lead to problems in the traffic mix.
			//So, the attribute made as hidden and the value set to false.

			// Copy the supSubFolderCreation option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-g");

			// Copy the supSubFolderCreation value
			tmpArgv[tmpArgc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
			ACE_OS::strcpy(tmpArgv[tmpArgc++],AES_CDH_INIT_NO);
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;
}
