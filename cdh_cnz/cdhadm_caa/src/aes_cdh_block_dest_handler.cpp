/*=================================================================== */
/**
   @file aes_cdh_block_dest_handler.cpp

   Class method implementationn for CDH module.

   This module contains the implementation of class declared in
   the aes_cdh_block_dest_handler.h file.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       04/12/2012   XGANGAB   Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_cdh_block_dest_handler.h>
#include <ace/Task.h>
#include <acs_apgcc_omhandler.h>
#include <aes_cdh_result_r1.h>
#include <ACS_APGCC_Util.H>
#include <servr.h>
#include <transdest.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
#include <aes_cdh_common.h>

AES_CDH_TRACE_DEFINE(AES_CDH_BlockDestCmdHandler);
ACE_Recursive_Thread_Mutex AES_CDH_BlockDestCmdHandler::ccbId_mtx;
CommitInfo AES_CDH_BlockDestCmdHandler::m_iCommit ;
/*===================================================================
   ROUTINE: AES_CDH_BlockDestCmdHandler
=================================================================== */
AES_CDH_BlockDestCmdHandler::AES_CDH_BlockDestCmdHandler(
		string aDestClassName,
		string aDestImplName,
		ACS_APGCC_ScopeT p_scope )
		: acs_apgcc_objectimplementereventhandler_V3(aDestClassName,
		aDestImplName,
		p_scope)
,theClassName(aDestClassName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	m_isClassImplAdded = false;
	m_DestOIThreadId = 0;
	ServR::pBlockDestInfo = 0;
	m_primDestCreateccbId = 0;
	m_secDestCreateccbId = 0;

	/** Create Reactor to handle the events**/
	m_poTp_reactor = new(std::nothrow) ACE_TP_Reactor;
	if( m_poTp_reactor == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Memory Allocation failed for ACE TP Reactor");
		AES_CDH_TRACE_MESSAGE("Memory Allocation failed for ACE TP Reactor");
	}	
	/** Create Reactor**/
	m_poReactor = new(std::nothrow) ACE_Reactor(m_poTp_reactor);
	if( m_poReactor == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Memory Allocation failed for ACE Reactor");
		AES_CDH_TRACE_MESSAGE("Memory Allocation failed for ACE Reactor");
	}

	// Create OI Handler 
	m_poOiHandler = new(std::nothrow) acs_apgcc_oihandler_V3() ;
	if( m_poOiHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Memory Allocation failed for OI Handler");
		AES_CDH_TRACE_MESSAGE("Memory Allocation failed for OI Handler");
	}

	AES_CDH_TRACE_MESSAGE("Leaving");

}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType AES_CDH_BlockDestCmdHandler::setObjectImplementer()
{
	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;

	AES_CDH_TRACE_MESSAGE("Entering");

	for (int i=0; i < 10; ++i)
	{
		errorCode = m_poOiHandler->addClassImpl(this, theClassName.c_str());
		if( errorCode == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while add class implmenter for Destination");
			AES_CDH_TRACE_MESSAGE("Error occured while add class implmenter for Destination");
			int intErr = getInternalLastError();
			if (intErr == -6)
			{
				for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
				if ( i >= 10)
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while add class implmenter for Destination, ErrorCode = %d", intErr);
					break;
				}
				else
					continue;
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "Error occured while add class implmenter for Destination, ErrorCode = %d", intErr);
				AES_CDH_TRACE_MESSAGE("Error occured while add class implmenter for Destination, ErrorCode = %d", intErr);
				break;
			}
		}
		else
		{
			m_isClassImplAdded = true;
			break;
		}
	}
	if(m_isClassImplAdded)
		AES_CDH_LOG(LOG_LEVEL_INFO, "%s", "Added class implementer successfully !!");
	AES_CDH_TRACE_MESSAGE("Leaving");
	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType AES_CDH_BlockDestCmdHandler::create(ACS_APGCC_OiHandle oiHandle,
							 ACS_APGCC_CcbId ccbId,
							 const char *className,
							 const char* parentName,
							 ACS_APGCC_AttrValues **attr)
{
	int i = 0;
	AES_CDH_LOG(LOG_LEVEL_INFO, "create callback received.");
	if( ACE_OS::strcmp( className, AES_CDH_PRIMARY_BLOCK_DEST_IMM_CLASS_NM ) == 0 )
	{
		m_blkPrimOiHandle = oiHandle;
		m_primDestCreateccbId = ccbId;
		string blockDestRdn("");
		string destinationName("");
		string blockDestSetName("");
		string ipAddress("");
		string userName("");
		string transferProtocol("");
		int ctr = 0;
		extractDestName(parentName, "OBJNAME", blockDestSetName);
		AES_CDH_TRACE_MESSAGE("Create callback %s", blockDestSetName.c_str());
		if(!m_iCommit.commitFlag)
		{
			m_iCommit.commitFlag=true;
			m_iCommit.destCommit.assign(blockDestSetName);
		}
		else if( m_iCommit.commitFlag && (m_iCommit.destCommit.compare(blockDestSetName) != 0))
		{
			m_iCommit.commitFlag=false;
			m_iCommit.destCommit.clear();
			setErrorCode(AES_CDH_RC_COMMITERROR);
			return ACS_CC_FAILURE;
		}
		while( attr[ctr] )
		{
			if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_PRIMARY_BLOCK_DEST_RDN) == 0 )
			{
				blockDestRdn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE("Primary destination Rdn is %s", blockDestRdn.c_str());
			}
			ctr++;
		}

		while( attr[i] )
		{
			if (ACE_OS::strcmp(attr[i]->attrName, AES_CDH_BLOCK_DEST_IP_ADDR) == 0)
			{
				if(attr[i]->attrValuesNum != 0 )
				{
					char *mytmp = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					if( mytmp != 0 )
					{
						AES_CDH_TRACE_MESSAGE("Primary destination IP address  = %s", mytmp);
						AES_CDH_LOG(LOG_LEVEL_INFO,"Primary destination IP address  = %s", mytmp);
						ipAddress.assign(mytmp);

						int validationResult = AES_CDH_Common_Util::isIPAddressValid(ipAddress.c_str());	//IPv6_feature:: validate IPv4 and IPv6 formats
						if (validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
						{
							AES_CDH_LOG(LOG_LEVEL_ERROR, "Destination Address is not a valid IP Address");
							AES_CDH_TRACE_MESSAGE("Destination Address is not a valid IP Address");
							//set error code for invalid value
							string objNm = blockDestRdn + "," + parentName;
							if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6 , objNm.c_str());
							else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE , objNm.c_str());
							else
								setErrorCode(AES_CDH_RC_UNREAS_BLOCKIP,objNm.c_str());
							return ACS_CC_FAILURE;
						}
					}

				}
				else
				{
					//set error code for incorrect usage
					setErrorCode(AES_CDH_RC_INCUSAGE);
					return ACS_CC_FAILURE;
				}
			}
			i++;
		}

		extractDestName( blockDestRdn, "RDN", destinationName );

		if ( isDestNameValid(destinationName) == false)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving since the destination name is invalid");
			AES_CDH_TRACE_MESSAGE("Leaving since the destination name is invalid");
			//set error code for invalid value
			string objNm = blockDestRdn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS_PRIBLOCKDESTNAME, objNm.c_str() );
			return ACS_CC_FAILURE;
		}
		AES_CDH_LOG(LOG_LEVEL_INFO, "DestinationName is valid");
		//get the destination set name.
		extractDestName(parentName, "OBJNAME", blockDestSetName);
		// Create the destination.
		if(ServR::pBlockDestInfo == 0)
		{
			ServR::pBlockDestInfo = new(std::nothrow) destInfo;
			if(ServR::pBlockDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pBlockDestInfo->argc = 0;
			//Copy the name of the command.
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc++], AES_CDH_CDHDEFINE);
		}							

		if( !ipAddress.empty())
		{
			//Copy the destination address option.
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[(ServR::pBlockDestInfo->argc)++], "-a");


			//Copy the destination address.
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char[ipAddress.size() + 1];
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[(ServR::pBlockDestInfo->argc)++], ipAddress.c_str());


		}
		ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = 0;

		ServR::pBlockDestInfo->ccbId = ccbId;
		ServR::pBlockDestInfo->destinationName = destinationName;
		ServR::pBlockDestInfo->isPrimDest = true;
		ServR::pBlockDestInfo->isSecDest = false;
		ServR::pBlockDestInfo->isDestSet = false;
		ServR::pBlockDestInfo->destinationSetName = blockDestSetName;
		ServR::pBlockDestInfo->transferType = AES_CDH_BGWRPC_TRANSFER_TYPE;


		//set error code for success scenario
		setErrorCode(AES_CDH_RC_OK);
		return ACS_CC_SUCCESS;
	}
	else if ( ACE_OS::strcmp( className, AES_CDH_SECONDARY_BLOCK_DEST_IMM_CLASS_NM) == 0 )
	{
		m_secDestCreateccbId = ccbId;
		string blockDestRdn("");
		string destinationName("");
		string blockDestSetName("");
		string ipAddress("");
		string transferProtocol("");
		ACE_INT32 blockSendRetries = -1;
		ACE_INT32 blockRetryDelay = -1;
		string myDetailInfo("");

		int ctr = 0;
		extractDestName(parentName, "OBJNAME", blockDestSetName);
		AES_CDH_TRACE_MESSAGE("Secondary Block Dest - Create callback %s",blockDestSetName.c_str());
		AES_CDH_LOG(LOG_LEVEL_INFO, "Secondary Block Dest - Create callback %s",blockDestSetName.c_str());
		if(!m_iCommit.commitFlag)
		{
			m_iCommit.commitFlag=true;
			m_iCommit.destCommit.assign(blockDestSetName);
		}
		else if( m_iCommit.commitFlag && (m_iCommit.destCommit.compare(blockDestSetName) != 0))
		{
			m_iCommit.commitFlag=false;
			m_iCommit.destCommit.clear();
			setErrorCode(AES_CDH_RC_COMMITERROR);
			return ACS_CC_FAILURE;
		}
		while( attr[ctr] )
		{
			if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_SECONDARY_BLOCK_DEST_RDN) == 0 )
			{
				blockDestRdn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE("Secondary destination Rdn is %s", blockDestRdn.c_str());
			}
			ctr++;
		}

		while( attr[i] )
		{
			if (ACE_OS::strcmp(attr[i]->attrName, AES_CDH_BLOCK_DEST_IP_ADDR) == 0)
			{
				if(attr[i]->attrValuesNum!=0)
				{
					char *mytmp = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					if( mytmp != 0 )
					{
						ipAddress.assign(mytmp);

						int validationResult = AES_CDH_Common_Util::isIPAddressValid(ipAddress.c_str());	//IPv6_feature:: validate IPv4 and IPv6 formats
						if (validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
						{
							AES_CDH_LOG(LOG_LEVEL_ERROR, "Destination Address is not a valid IP Address");
							AES_CDH_TRACE_MESSAGE("Destination Address is not a valid IP Address");

							//set error code for invalid value
							string objNm =blockDestRdn  + "," + parentName;
							if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6 , objNm.c_str());
							else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE , objNm.c_str());
							else
								setErrorCode(AES_CDH_RC_UNREAS_BLOCKIP , objNm.c_str());
							return ACS_CC_FAILURE;
						}

					}
				}
				else
				{
					//set error code for incorrect usage
					setErrorCode(AES_CDH_RC_INCUSAGE);
					return ACS_CC_FAILURE;
				}
			}
			i++;
		}

		extractDestName( blockDestRdn, "RDN", destinationName);

		if ( isDestNameValid(destinationName) == false)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving since the destination name is invalid");
			AES_CDH_TRACE_MESSAGE("Leaving since the destination name is invalid");

			//set error code for invalid value
			string objNm = blockDestRdn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS_SECBLOCKDESTNAME, objNm.c_str() );
			return ACS_CC_FAILURE;
		}

		OmHandler omHandler;
		if( omHandler.Init() == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving initializing the Om handler for getting advanced block parameters failed");
			AES_CDH_TRACE_MESSAGE("Leaving initializing the Om handler for getting advanced block parameters failed");
			//set error code for internal error
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}

		// Get the destination set name from the parent name
		extractDestName(parentName, "OBJNAME", blockDestSetName);

		std::vector<std::string>  destinationList;
		omHandler.getClassInstances(AES_CDH_SECONDARY_BLOCK_DEST_IMM_CLASS_NM, destinationList);
		for( unsigned int cntr = 0 ; cntr < destinationList.size() ; ++cntr)
		{
			string tmpDestinationSetName("");
			size_t pos1 =  string(destinationList[cntr]).find_first_of(",");
			tmpDestinationSetName= destinationList[cntr].substr(pos1+1);
			size_t pos2 = tmpDestinationSetName.find_first_of("=");
			size_t pos3 = tmpDestinationSetName.find_first_of(",");

			tmpDestinationSetName = tmpDestinationSetName.substr(pos2+1, pos3-pos2-1);

			if( ACE_OS::strcmp(tmpDestinationSetName.c_str(), blockDestSetName.c_str()) == 0 )
			{
				//Do not allow creation of one more instance of Secondary Block destination if already exists
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving secondaryBlockDestination already exists for this destination set.");
				AES_CDH_TRACE_MESSAGE("Leaving initializing the Om handler for getting advanced block parameters failed");

				//Error code for secondary dest already exists
				setErrorCode(AES_CDH_RC_SECDESTEXIST);
				omHandler.Finalize();
				return ACS_CC_FAILURE;
			}
		}
		std::vector<std::string>  p_rdnList;
		omHandler.getChildren(parentName, ACS_APGCC_SUBLEVEL,&p_rdnList);
		string objName("");
		for(unsigned int rdnCntr = 0; rdnCntr < p_rdnList.size(); rdnCntr ++)
		{
			string myClassName("");
			extractClassNameFromRdn(p_rdnList[rdnCntr], myClassName);
			if(myClassName == AES_CDH_BLOCK_ADV_PARAMS_IMM_RDN_CL_NM)
			{
				objName = p_rdnList[rdnCntr];
				break;
			}
		}
		if(objName.empty())
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving Om handler could not fetch the advanced block params object");
			AES_CDH_TRACE_MESSAGE("Leaving Om handler could not fetch the advanced block params object");

			//Error code if primary dest is not created
			setErrorCode(AES_CDH_RC_NOPRIDEST);
			omHandler.Finalize();
			return ACS_CC_FAILURE;
		}


		ACS_APGCC_ImmObject blockadvParamsObj;
		blockadvParamsObj.objName = objName;

		//Get the advanced block params object

		if( omHandler.getObject( &blockadvParamsObj) == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving Om handler could not fetch the advanced block params object");
			AES_CDH_TRACE_MESSAGE("Leaving Om handler could not fetch the advanced block params object");

			//Error code if primary dest is not created
			setErrorCode(AES_CDH_RC_NOPRIDEST);
			omHandler.Finalize();
			return ACS_CC_FAILURE;
		}

		omHandler.Finalize();

		//Read the attributes of the advanced block object
		for ( unsigned int cntr = 0; cntr  < blockadvParamsObj.attributes.size(); cntr++ )
		{
			if(blockadvParamsObj.attributes[cntr].attrName == AES_CDH_BLOCK_RETRYDELAY )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(blockadvParamsObj.attributes[cntr].attrValues[0]);
				blockRetryDelay = mytmp;
				AES_CDH_TRACE_MESSAGE("Block retry delay value is %d", blockRetryDelay);
			}
			else if (blockadvParamsObj.attributes[cntr].attrName == AES_CDH_BLOCK_SENDRETRY )
			{
				ACE_INT32 mytmp  = *reinterpret_cast<ACE_INT32 *>(blockadvParamsObj.attributes[cntr].attrValues[0]);
				blockSendRetries = mytmp;
				AES_CDH_TRACE_MESSAGE("Send retry value is %d", blockSendRetries);
			}
		}

		// Create the destset 
		int argc = 0;
		char *argv[32] = { 0 };

		//Copy the name of the command.
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

		if( !ipAddress.empty())
		{
			//Copy the destination address option.
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-a");

			//Copy the destination address.
			argv[argc] = new char[ipAddress.size() + 1];
			ACE_OS::strcpy(argv[argc++], ipAddress.c_str());
		}

		if ( blockRetryDelay != -1 )
		{

			// Copy the blockRetryDelay option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-m");

			// Copy the blockRetryDelay value
			char myRetryDelay[11] = { 0 };
			ACE_OS::sprintf( myRetryDelay, "%d", blockRetryDelay);
			argv[argc] = new char[ACE_OS::strlen(myRetryDelay)+1];
			ACE_OS::strcpy(argv[argc++], myRetryDelay);

		}

		if ( blockSendRetries != -1 )
		{
			// Copy the blockSendRetries option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-s");

			// Copy the blockSendRetries value
			char mySendRetries[11] = { 0 };
			ACE_OS::sprintf( mySendRetries, "%d", blockSendRetries);
			argv[argc] = new char[ACE_OS::strlen(mySendRetries)+1];
			ACE_OS::strcpy(argv[argc++], mySendRetries);

		}
		argv[argc] = 0;

		if(ServR::pBlockDestInfo == 0)
		{
			ServR::pBlockDestInfo = new(std::nothrow) destInfo;
			if(ServR::pBlockDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pBlockDestInfo->argc = 0;
		}							
		ServR::pBlockDestInfo->ccbId = ccbId;
		ServR::pBlockDestInfo->destinationName = destinationName;
		ServR::pBlockDestInfo->isPrimDest = false;
		ServR::pBlockDestInfo->isSecDest = true;
		ServR::pBlockDestInfo->isDestSet = false;
		ServR::pBlockDestInfo->destinationSetName = blockDestSetName;
		ServR::pBlockDestInfo->transferType = AES_CDH_BGWRPC_TRANSFER_TYPE;


		// Create the destination
		int result = ServR::define( destinationName, AES_CDH_BGWRPC_TRANSFER_TYPE, argc, argv, "",AES_CDH_SECONDARY_BLOCK_DEST_RDN );
		if(result!= AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining secondary destination");
			AES_CDH_TRACE_MESSAGE("Error occured while defining secondary destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			ccbId_mtx.acquire();
			if(ServR::pBlockDestInfo != 0)
			{
				delete ServR::pBlockDestInfo;
				ServR::pBlockDestInfo = 0;
			}
			ccbId_mtx.release();
			//set error code for the result of define
			setErrorCode(result);
			return ACS_CC_FAILURE;
		}
		for ( int ctr = 0; ctr < argc ; ctr++)
		{
			delete[] argv[ctr];
			argv[ctr] = 0;
		}
		//add secondary dest to destset
		string primaryDest(""), backupDest(""), usrName("");
		string destSetName = blockDestSetName;

		int result1= ServR::changeDestSet(destSetName, primaryDest, destinationName, backupDest, usrName);
		if(result1!= AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while updating the destset with secondary destination.");
			AES_CDH_TRACE_MESSAGE("Error occured while updating the destset with secondary destination.");
			ccbId_mtx.acquire();
			if(ServR::pBlockDestInfo != 0)
			{
				delete ServR::pBlockDestInfo;
				ServR::pBlockDestInfo = 0;
			}
			ccbId_mtx.release();	
			ServR::deleteDest(destinationName, myDetailInfo,  usrName );
			//set error code for the result of changeDestSet
			setErrorCode(result1);
			return ACS_CC_FAILURE;
		}
		//set error code for success scenario
		setErrorCode(AES_CDH_RC_OK);
		return ACS_CC_SUCCESS;
	}
	else if( ACE_OS::strcmp( className, AES_CDH_BLOCK_ADV_PARAMS_IMM_CLASS_NM) == 0 )
	{
		int blkretryDelay = -1;
		int blksendRetries = -1;
		string blockDestRdn("");
		string destinationName("");
		string destinationSetName;
		extractDestName(parentName, "OBJNAME", destinationSetName);
		AES_CDH_TRACE_MESSAGE("AdvancedInitiatingParams Create callback %s",destinationSetName.c_str());
		AES_CDH_LOG(LOG_LEVEL_INFO, "AdvancedInitiatingParams Create callback %s", destinationSetName.c_str());
		if(!m_iCommit.commitFlag)
		{
			m_iCommit.commitFlag=true;
			m_iCommit.destCommit.assign(destinationSetName);
		}
		else if( m_iCommit.commitFlag && (m_iCommit.destCommit.compare(destinationSetName) != 0))
		{
			m_iCommit.commitFlag=false;
			m_iCommit.destCommit.clear();
			setErrorCode(AES_CDH_RC_COMMITERROR);
			return ACS_CC_FAILURE;
		}

		if(ServR::pBlockDestInfo == 0)
		{
			ServR::pBlockDestInfo = new(std::nothrow) destInfo;
			if(ServR::pBlockDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pBlockDestInfo->argc = 0;
			//Copy the name of the command.
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc++], AES_CDH_CDHDEFINE);
		}							

		//Parse the arguments
		int ctr= 0;
		while( attr[ctr] )
		{
			if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_BLOCK_ADV_PARAMS_RDN) == 0 )
			{
				blockDestRdn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE("Advanced block destination Rdn is %s", blockDestRdn.c_str());
			}
			ctr++;
		}

		while( attr[i] )
		{

			if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_BLOCK_RETRYDELAY) == 0 )
			{

				if (attr[i]->attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					if(mytmp < 200 || mytmp > 10000 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Block retry delay is out of range");
						AES_CDH_TRACE_MESSAGE("Block retry delay is out of range");
						//set error code for invalid value
						string objNm = blockDestRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_BLOCKRETRYDELAY,objNm.c_str() );
						return ACS_CC_FAILURE;
					}
					blkretryDelay = mytmp;
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_BLOCK_SENDRETRY) == 0 )
			{
				if (attr[i]->attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					if(mytmp < 0 || mytmp > 100 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "send retry is out of range");
						AES_CDH_TRACE_MESSAGE("send retry is out of range");
						//set error code for invalid value
						string objNm = blockDestRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_BLOCKRETRIES,objNm.c_str() );
						return ACS_CC_FAILURE;
					}
					blksendRetries = mytmp;
				}
			}
			i++;
		}
		if ( blkretryDelay != -1 )
		{
			// Copy the blockRetryDelay option
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[(ServR::pBlockDestInfo->argc)++], "-m");

			// Copy the blockRetryDelay value
			char myRetryDelay[11] = { 0 };
			ACE_OS::sprintf( myRetryDelay, "%d", blkretryDelay);
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char[ACE_OS::strlen(myRetryDelay)+1];
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[(ServR::pBlockDestInfo->argc)++], myRetryDelay);

		}

		if ( blksendRetries != -1 )
		{
			// Copy the blockSendRetries option
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[(ServR::pBlockDestInfo->argc)++], "-s");

			// Copy the blockSendRetries value
			char mySendRetries[11] = { 0 };
			ACE_OS::sprintf( mySendRetries, "%d", blksendRetries);
			ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = new char[ACE_OS::strlen(mySendRetries)+1];
			ACE_OS::strcpy(ServR::pBlockDestInfo->argv[(ServR::pBlockDestInfo->argc)++], mySendRetries);

		}
		ServR::pBlockDestInfo->argv[ServR::pBlockDestInfo->argc] = 0;

		extractDestName( blockDestRdn, "RDN", destinationName);
		ServR::pBlockDestInfo->advDestinationName = destinationName;

		if ( isDestNameValid(destinationName) == false)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving  since the destination name is invalid");
			AES_CDH_TRACE_MESSAGE("Leaving  since the destination name is invalid");

			//set error code for invalid value
			ccbId_mtx.acquire();
			if(ServR::pBlockDestInfo != 0)
			{
				delete ServR::pBlockDestInfo;
				ServR::pBlockDestInfo = 0;
			}
			ccbId_mtx.release();
			string objNm = blockDestRdn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS_ADVBLOCKDESTNAME, objNm.c_str() );
			return ACS_CC_FAILURE;
		}

	}
	AES_CDH_TRACE_MESSAGE("Leaving");

	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType AES_CDH_BlockDestCmdHandler::deleted( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "delete callback received");
	ACE_Time_Value tv(0,10000);
	ACE_OS::sleep(tv);
	ServR::blockdestsetMutex.acquire();

	if( ServR::isBlockDestSetDeleteTrig == true )
	{
		AES_CDH_TRACE_MESSAGE("ServR::isBlockDestSetDeleteTrig is true");
		if( ServR::isDelOfBlockDestSetSuccess == true )
		{
			AES_CDH_TRACE_MESSAGE("ServR::isDelOfBlockDestSetSuccess is true");
			ServR::blockdestsetMutex.release();
			//set error code for success
			setErrorCode(AES_CDH_RC_OK);
			return ACS_CC_SUCCESS ;
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "ServR::isDelOfBlockDestSetSuccess is false");
			AES_CDH_TRACE_MESSAGE(" ServR::isDelOfBlockDestSetSuccess is false");
			ServR::blockdestsetMutex.release();
			//set error code for internal error
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}
	}
	AES_CDH_TRACE_MESSAGE("ServR::isBlockDestSetDeleteTrig is false");
	AES_CDH_LOG(LOG_LEVEL_INFO, "isBlockDestSetDeleteTrig is false");
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
	if(  myClassName.compare(AES_CDH_BLOCK_ADV_PARAMS_IMM_RDN_CL_NM) == 0 )
	{
		//Check whether destset is removed or only Advanced is getting removed
		if (ServR::getDestSet(destSetName, pdestset) == AES_CDH_RC_NODESTSET)//destset removed
		{
			ServR::blockdestsetMutex.release();
			//set error code for success
			setErrorCode(AES_CDH_RC_OK);
			return ACS_CC_SUCCESS;
		}
		else
		{
			ServR::blockdestsetMutex.release();
			//set error code for internal error
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}

	}
	else if( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_PRIMARY_BLOCK_DEST_RDN_CL_NM) == 0
	|| ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_SECONDARY_BLOCK_DEST_RDN_CL_NM) == 0 )
	{
		extractDestName(objName, "OBJNAME", myDestinationName);

		string detailInfo(""), userName("");

		AES_CDH_ResultCode resultCode = AES_CDH_RC_OK;
		//remove secondary from destset
		if ( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_SECONDARY_BLOCK_DEST_RDN_CL_NM) == 0 )
		{
			//Check whether destset is removed or only secondary is getting removed
			if (ServR::getDestSet(destSetName, pdestset) != AES_CDH_RC_NODESTSET)
			{
				//remove secondary from destset
				resultCode = ServR::deleteDestSet(destSetName, detailInfo, myDestinationName, userName);
				if ( resultCode != AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing dest %s from destset %s", myDestinationName.c_str(), destSetName.c_str());
					AES_CDH_TRACE_MESSAGE("Error occured while removing dest %s from destset %s", myDestinationName.c_str(), destSetName.c_str());
					ServR::blockdestsetMutex.release();
					//set error code for deleteDestSet failure
					setErrorCode(resultCode);// an error has occured during dis-associtaion
					return ACS_CC_FAILURE;
				}
			}
			vector<AES_CDH_Destination::destAttributes> secDestAttrs;
			resultCode = ServR::listDest( myDestinationName, secDestAttrs );
			if(  resultCode == AES_CDH_RC_OK )
			{
				destInfo deleteDestNode;
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
				AES_CDH_TRACE_MESSAGE("Error occured while deleting destination");
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

					vector <destInfo>::iterator itr = m_deleteDestList.begin();
					while( itr != m_deleteDestList.end())
					{
						m_deleteDestList.erase(itr);
						itr = m_deleteDestList.begin();
					}
				}

				//add secondary dest to destset
				string primaryDest(""), backupDest("");
				ServR::changeDestSet(destSetName, primaryDest, myDestinationName, backupDest, myUserName);
				ServR::blockdestsetMutex.release();
				//set error code for deleteDest failure
				setErrorCode(resultCode);
				return ACS_CC_FAILURE;
			}
		}
		else
		{
			resultCode = ServR::getDestSet(destSetName, pdestset);
			if(resultCode != AES_CDH_RC_NODESTSET)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while deleting primary destination. resultCode = %d",resultCode);
				AES_CDH_TRACE_MESSAGE("Error occured while deleting primary destination. resultCode = %d",resultCode);
				ServR::blockdestsetMutex.release();
				//deleting the primary dest, with out deleting destset (error code to be defined)
				setErrorCode(AES_CDH_RC_PROTECTEDDEST);
				return ACS_CC_FAILURE;
			}

			resultCode = ServR::deleteDest(myDestinationName, myDetailInfo,  myUserName );

			if ( resultCode != AES_CDH_RC_OK )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while deleting destination. resultCode = %d",resultCode);
				AES_CDH_TRACE_MESSAGE("Error occured while deleting destination. resultCode = %d",resultCode);
				ServR::blockdestsetMutex.release();
				//set error code for deleteDestSet failure
				setErrorCode(resultCode);
				return ACS_CC_FAILURE;
			}
		}
		AES_CDH_TRACE_MESSAGE("Destination %s removed",myDestinationName.c_str());
		AES_CDH_LOG(LOG_LEVEL_INFO, "Destination %s removed",myDestinationName.c_str());
	}
	ServR::blockdestsetMutex.release();
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
	ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType AES_CDH_BlockDestCmdHandler::modify(ACS_APGCC_OiHandle oiHandle,
							ACS_APGCC_CcbId ccbId,
							const char *objName,
							ACS_APGCC_AttrModification **attrMods)
{
	(void) oiHandle;
	(void) ccbId;

	string myClassName("");
	string myDestinationName("");

	// Extract the class name from the object RDN
	extractClassNameFromRdn(objName, myClassName);

	AES_CDH_TRACE_MESSAGE("Class name is %s",myClassName.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO, "Class name is %s",myClassName.c_str());

	if( myClassName.compare(AES_CDH_BLOCK_ADV_PARAMS_IMM_RDN_CL_NM) == 0)
	{
		AES_CDH_TRACE_MESSAGE("Modifying advanced block dest set parameters");

		int argc = 0, i = 0;
		char *argv[32] = { 0 };

		//Copy the name of the command.
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

		//Parse the arguments
		while( attrMods[i] )
		{
			if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_BLOCK_RETRYDELAY) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_LOG(LOG_LEVEL_INFO, "Block Retry Delay = %d", mytmp);
				AES_CDH_TRACE_MESSAGE("Block Retry Delay = %d", mytmp);
				if( mytmp < 200 || mytmp > 10000 )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Block retry delay is out of range");
					AES_CDH_TRACE_MESSAGE("Block retry delay is out of range");
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					//set error code for invalid value

					setErrorCode(AES_CDH_RC_UNREAS_BLOCKRETRYDELAY,objName);

					return ACS_CC_FAILURE;
				}
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-m");
				char myBlockRetryDelay[11] = { 0 };
				ACE_OS::sprintf( myBlockRetryDelay, "%d", mytmp);
				argv[argc] = new char[ACE_OS::strlen(myBlockRetryDelay)+1];
				ACE_OS::strcpy(argv[argc++], myBlockRetryDelay);

			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_BLOCK_SENDRETRY) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE("Block Retries  = %d", mytmp);
				AES_CDH_LOG(LOG_LEVEL_INFO, "Block Retries  = %d", mytmp);
				if(mytmp < 0 || mytmp > 100 )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Retry count is out of range");
					AES_CDH_TRACE_MESSAGE("Retry count is out of range");
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					//set error code for invalid value
					 
					setErrorCode(AES_CDH_RC_UNREAS_BLOCKRETRIES,objName );
					return ACS_CC_FAILURE;
				}
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-s");
				char myRetries[10];
				ACE_OS::sprintf( myRetries, "%d", mytmp);
				argv[argc] = new char[ACE_OS::strlen(myRetries)+1];
				ACE_OS::strcpy(argv[argc++], myRetries);
			}
			i++;
		}

		OmHandler omHandler;
		if ( omHandler.Init() == ACS_CC_FAILURE)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving - Unable to initialize the omhandler");
			AES_CDH_TRACE_MESSAGE("Leaving - Unable to initialize the omhandler");
			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			//set error code for invternal error
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}
		string destinationSetRdn = objName;
		size_t pos = destinationSetRdn.find_first_of(",");
		if( pos != string::npos )
		{
			destinationSetRdn = destinationSetRdn.substr(pos+1);
		}
		std::vector<std::string>  destinationList;
		std::vector<std::string>  primDestList;
		std::vector<std::string>  secDestList;

		//Get the instances of the PrimaryInitiatingDestination.
		omHandler.getClassInstances(AES_CDH_PRIMARY_BLOCK_DEST_IMM_CLASS_NM, primDestList);

		//Get the instances of the SecondaryInitiatingDestination.
		omHandler.getClassInstances( AES_CDH_SECONDARY_BLOCK_DEST_IMM_CLASS_NM,secDestList);

		for( unsigned int pCtr = 0 ; pCtr < primDestList.size(); ++pCtr )
		{
			size_t pos1 = string(primDestList[pCtr]).find_first_of(",");
			if( primDestList[pCtr].substr(pos1+1) == destinationSetRdn )
			{
				destinationList.push_back( primDestList[pCtr]);
			}
		}

		for( unsigned int sCtr = 0 ; sCtr < secDestList.size(); ++sCtr )
		{
			size_t pos1 = string(secDestList[sCtr]).find_first_of(",");
			if( secDestList[sCtr].substr(pos1+1) == destinationSetRdn )
			{
				destinationList.push_back( secDestList[sCtr]);
			}
		}
		omHandler.Finalize();
		for ( unsigned int cntr = 0 ; cntr < destinationList.size() ; ++cntr)
		{
			int tmpArgc= 0;
			char *tmpArgv[32] = { 0 };

			//Copy into temporary argc and argv
			tmpArgc = argc;

			for ( int tmpCntr=0; tmpCntr < argc ; tmpCntr++)
			{
				tmpArgv[tmpCntr] = new char[strlen(argv[tmpCntr] )];
				strcpy( tmpArgv[tmpCntr], argv[tmpCntr] );
			}

			//Call the business logic here
			AES_CDH_ResultCode resultCode  = AES_CDH_RC_OK;
			string strUserGroup("");
			argv[argc] = 0;

			//Fetch the destination name from the Rdn.
			extractDestName(destinationList[cntr].c_str(), "OBJNAME", myDestinationName);

			//Save the data to abort the modification in case of abort() callback.

			destInfo destModStruct;

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
				AES_CDH_TRACE_MESSAGE("Error occured while modifying destination");

				for( int ctr = 0; ctr < tmpArgc ; ctr++)
				{
					delete[] tmpArgv[ctr];
					tmpArgv[ctr] = 0;
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
			for( int i = 1; i < tmpArgc ;  i=i+2)
			{
				for ( int j = 0; j < tmpArgc1 ; j=j+2)
				{
					if( strcmp(tmpArgv[i], tmpArgv1[j]) == 0 )
					{
						destModStruct.argv[destModStruct.argc] = new char[strlen(tmpArgv[i]) + 1];
						strcpy( destModStruct.argv[destModStruct.argc++] , tmpArgv[i] );

						destModStruct.argv[destModStruct.argc] = new char[strlen(tmpArgv1[j+1]) + 1];
						strcpy( destModStruct.argv[destModStruct.argc++], tmpArgv1[j+1]);
					}
				}
			}

			destModStruct.argv[destModStruct.argc] = 0;

			m_modifyDestList.push_back(destModStruct);
			for( int ctr = 0; ctr < tmpArgc1 ; ctr++)
			{
				delete[] tmpArgv1[ctr];
				tmpArgv1[ctr] = 0;
			}

			//Invoke modify here.
			argv[argc] = 0;


			//Now modify the destination.
			resultCode = ServR::changeAttr( myDestinationName, tmpArgc, tmpArgv, strUserGroup);

			if( resultCode != AES_CDH_RC_OK )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination");
				AES_CDH_TRACE_MESSAGE("Error occured while modifying destination");
				// Delete the temporary variables
				for( int ctr = 0; ctr < tmpArgc ; ctr++)
				{
					delete[] tmpArgv[ctr];
					tmpArgv[ctr] = 0;
				}
				// Delete the main argc and argv variables
				for ( int ctr = 0; ctr < argc ; ctr++)
				{
					delete[] argv[ctr];
					argv[ctr] = 0;
				}
				// Delete the main argc and argv variables
				vector <destInfo>::iterator itr;
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

				AES_CDH_TRACE_MESSAGE("Leaving AES_CDH_BlockDestCmdHandler::modify with error code %d", resultCode);
				//set error code for changeAttr failure
				setErrorCode(resultCode);
				return ACS_CC_FAILURE;
			}
			for( int ctr = 0; ctr < tmpArgc ; ctr++)
			{
				delete[] tmpArgv[ctr];
				tmpArgv[ctr] = 0;
			}
		}

		for ( int ctr = 0; ctr < argc ; ctr++)
		{
			delete[] argv[ctr];
			argv[ctr] = 0;
		}
	}
	else if( myClassName.compare(AES_CDH_PRIMARY_BLOCK_DEST_RDN_CL_NM) == 0 || myClassName.compare(AES_CDH_SECONDARY_BLOCK_DEST_RDN_CL_NM) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Modifying block destination");

		int argc = 0, i = 0;
		char *argv[32] = { 0 };

		//Copy the name of the command.
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

		//Parse the arguments
		while( attrMods[i] )
		{
			if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_BLOCK_DEST_IP_ADDR) == 0)
			{
				char *mytmp = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE("Block Remote Destination  = %s", mytmp);
				if( mytmp != 0 )
				{
					int validationResult = AES_CDH_Common_Util::isIPAddressValid(mytmp); //IPv6_feature:: validate IPv4 and IPv6 formats
					if (validationResult == AES_CDH_Common_Util::VALID_ADDRESS)
					{
						//Copy the destination address option.
						argv[argc] = new char[3];
						ACE_OS::strcpy(argv[argc++], "-a");
						//Copy the destination address.
						argv[argc] = new char[ACE_OS::strlen(mytmp)+1];
						ACE_OS::strcpy(argv[argc++], mytmp);
					}
					else
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Destination Address is not a valid IP Address");
						AES_CDH_TRACE_MESSAGE("Destination Address is not a valid IP Address");

						for ( int ctr = 0; ctr < argc ; ctr++)
						{
							delete[] argv[ctr];
							argv[ctr] = 0;
						}
						//set error code for invalid value
						if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
							setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6 , objName);
						else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
							setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE , objName);
						else
							setErrorCode(AES_CDH_RC_UNREAS_BLOCKIP , objName);

						return ACS_CC_FAILURE;
					}

				}
			}
			i++;
		}
		//Calling the Business logic
		AES_CDH_ResultCode resultCode  = AES_CDH_RC_OK;
		string strUserGroup("");
		argv[argc] = 0;

		//Extract the destination name from the RDN.
		extractDestName( objName, "OBJNAME", myDestinationName);
		AES_CDH_TRACE_MESSAGE("Destination Name = %s", myDestinationName.c_str());

		//Save the data to abort the modification in case of abort() callback.

		destInfo destModStruct;

		destModStruct.ccbId  = ccbId;
		destModStruct.destinationName = myDestinationName;
		destModStruct.argc = 0;
		destModStruct.argv[destModStruct.argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( destModStruct.argv[destModStruct.argc++], AES_CDH_CDHCHANGE);

		char *tmpArgv[32] = {0};
		int tmpArgc = 0;

		if( getExistingDestObjFromIMM( objName, tmpArgc, tmpArgv ) == false )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination" );
			AES_CDH_TRACE_MESSAGE("Error occured while modifying destination");

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

		//Now modify the destination.
		resultCode = ServR::changeAttr( myDestinationName, argc, argv, strUserGroup);

		if( resultCode != AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination" );
			AES_CDH_TRACE_MESSAGE("Error occured while modifying destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			vector <destInfo>::iterator itr;
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

			AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);
			//set error code for changeAttr failure
			setErrorCode(resultCode);
			return ACS_CC_FAILURE;
		}
		for ( int ctr = 0; ctr < argc ; ctr++)
		{
			delete[] argv[ctr];
			argv[ctr] = 0;
		}
		AES_CDH_LOG(LOG_LEVEL_ERROR,"Leaving  with error code %d", resultCode);
		AES_CDH_TRACE_MESSAGE("Leaving  with error code %d", resultCode);

	}
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType AES_CDH_BlockDestCmdHandler::complete( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "complete callback triggered");
	(void)oiHandle;
	(void)ccbId;
	m_iCommit.commitFlag=false;
	m_iCommit.destCommit.clear();
	if(ServR::pBlockDestInfo != 0 )
	{
		if( ServR::pBlockDestInfo->ccbId == ccbId )
		{

			if(m_blkPrimOiHandle == oiHandle)
			{
				string myDetailInfo("");
				// Create the destination
				int result = ServR::define( ServR::pBlockDestInfo->destinationName, AES_CDH_BGWRPC_TRANSFER_TYPE, ServR::pBlockDestInfo->argc, ServR::pBlockDestInfo->argv, "", AES_CDH_PRIMARY_BLOCK_DEST_RDN);
				if (result!= AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining destination");
					AES_CDH_TRACE_MESSAGE("Error occured while defining destination");
					//set error code for which is the result of define
					ccbId_mtx.acquire();
					if( ServR::pBlockDestInfo != 0)
					{
						for(int i = 0; i < ServR::pBlockDestInfo->argc ; i++)
						{
							delete[] ServR::pBlockDestInfo->argv[i];
							ServR::pBlockDestInfo->argv[i] = 0;
						}
						ServR::pBlockDestInfo->argc = 0;
						delete ServR::pBlockDestInfo;
						ServR::pBlockDestInfo = 0;
					}
					ccbId_mtx.release();
					setErrorCode(result);
					return ACS_CC_FAILURE;
				}
				//define destset with this dest as primary
				string secondaryDestName(""), backupDestName(""), userGroup(""), usrName("");

				int result1 = ServR::defineDestSet(ServR::pBlockDestInfo->destinationSetName, ServR::pBlockDestInfo->destinationName, secondaryDestName,
						backupDestName,userGroup, AES_CDH_BLOCK_DEST_SET_ID);
				if (result1 != AES_CDH_RC_OK)
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining destinationset");
					AES_CDH_TRACE_MESSAGE("Error occured while defining destinationset");

					ccbId_mtx.acquire();
					if(ServR::pBlockDestInfo != 0)
					{
						for(int i = 0; i < ServR::pBlockDestInfo->argc ; i++)
						{
							delete[] ServR::pBlockDestInfo->argv[i];
							ServR::pBlockDestInfo->argv[i] = 0;
						}
						ServR::pBlockDestInfo->argc = 0;
						delete ServR::pBlockDestInfo;
						ServR::pBlockDestInfo = 0;
					}
					ccbId_mtx.release();
					//If destination set creation fails, then remove the primary destination too.
					ServR::deleteDest(ServR::pBlockDestInfo->destinationName, myDetailInfo,  usrName );
					//set error code for which is the result of defineDestSet
					setErrorCode(result1);
					return ACS_CC_FAILURE;
				}
				m_primDestCreateccbId = 0;
			}
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void AES_CDH_BlockDestCmdHandler::abort(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "abort callback triggered");
	(void)oiHandle;
	(void)ccbId;
	m_primDestCreateccbId = 0;
	m_secDestCreateccbId = 0;
	m_iCommit.commitFlag=false;
	m_iCommit.destCommit.clear();
	ccbId_mtx.acquire();
	if( ServR::pBlockDestInfo != 0 )
	{
		if(ServR::pBlockDestInfo->ccbId == ccbId )
		{
			//abort has been recived for destination creation.

			if( ServR::pBlockDestInfo->isPrimDest  == true )
			{
				//Delete the destination set.
				string myDetailInfo(""), myDestinationName(""), myUserName("");

				ServR::deleteDestSet(ServR::pBlockDestInfo->destinationSetName, myDetailInfo, myDestinationName, myUserName);

				//Delete the destination.
				ServR::deleteDest(ServR::pBlockDestInfo->destinationName, myDetailInfo,  myUserName );
			}
			else if( ServR::pBlockDestInfo->isSecDest == true )
			{
				//Delete the destination set association for secondary destination.
				string myDetailInfo(""), myDestinationName(""), myUserName("");

				ServR::deleteDestSet(ServR::pBlockDestInfo->destinationSetName, myDetailInfo, ServR::pBlockDestInfo->destinationName, myUserName);

				//Delete the secondary destination.
				ServR::deleteDest(ServR::pBlockDestInfo->destinationName, myDetailInfo,  myUserName );
			}
		}
		for(int i = 0; i < ServR::pBlockDestInfo->argc ; i++)
		{
			delete[] ServR::pBlockDestInfo->argv[i];
			ServR::pBlockDestInfo->argv[i] = 0;
		}
		delete ServR::pBlockDestInfo;
		ServR::pBlockDestInfo = 0;
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

		vector <destInfo>::iterator itr = m_modifyDestList.begin();
		while( itr != m_modifyDestList.end())
		{
			m_modifyDestList.erase(itr);
			itr = m_modifyDestList.begin();
		}
	}

	ServR::blockdestsetMutex.acquire();
	if(  m_deleteDestList.size() > 0 )
	{
		for ( unsigned ctr = 0 ; ctr < m_deleteDestList.size() ; ctr++)
		{
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

		vector <destInfo>::iterator itr = m_deleteDestList.begin();
		while( itr != m_deleteDestList.end())
		{
			m_deleteDestList.erase(itr);
			itr = m_deleteDestList.begin();
		}
	}
	ServR::blockdestsetMutex.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType AES_CDH_BlockDestCmdHandler::updateRuntime( const char* objName,
		const char** attrName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)objName;
	(void)attrName;
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void AES_CDH_BlockDestCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "admin callback triggered");
	(void)paramList;
	string myClassName("");
	string objName = string(p_objName);
	extractClassNameFromRdn(objName, myClassName);
	string destName;
	string tempDest;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;

	if(ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_PRIMARY_BLOCK_DEST_RDN_CL_NM ) == 0
		|| ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_SECONDARY_BLOCK_DEST_RDN_CL_NM ) == 0 )
	{
		bool  myOIValidationError=true;
		int result;
		AES_CDH_TRACE_MESSAGE("objName :%s", p_objName);
		switch(operationId)
		{
		case 5://connectionDestinationVerify
		{
			tempDest=string(p_objName);
			extractDestName(tempDest,"OBJNAME",destName);
			AES_CDH_TRACE_MESSAGE("Trying to check connection for destination : %s",destName.c_str());
			AES_CDH_LOG(LOG_LEVEL_INFO, "Trying to check connection for destination : %s",destName.c_str());
			vector<ServR::checkConnAttributes> cAttrs;
			bool force   = true; //hard code to true
			result = ServR::checkConnection(destName, force, cAttrs);
			//set error code for result of checkConnection
			setErrorCode(result);
			if(result == AES_CDH_RC_CONNECTOK)
			{
				myOIValidationError=true;

			}
			else
			{
				myOIValidationError=false;
				char attrName1[] = "errorCode";
				firstElem.attrName = attrName1;
				firstElem.attrType = ATTR_INT32T;
				int intValue1 = AES_CDH_RC_CONNECTERR ;
				firstElem.attrValues=reinterpret_cast<void*>(&intValue1);
				AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(AES_CDH_RC_CONNECTERR,"");
				string ertxt="@ComNbi@";
				ertxt += resobj->errorText();
				AES_CDH_LOG(LOG_LEVEL_ERROR, "connection verification failed. error txt = %s",ertxt.c_str());
				AES_CDH_TRACE_MESSAGE("connection verification failed. error txt = %s",ertxt.c_str());
				char attrName2[]= "errorText";

				secondElem.attrName = attrName2;
				secondElem.attrType = ATTR_STRINGT;
				char* stringValue = const_cast<char*>(ertxt.c_str());
				secondElem.attrValues=reinterpret_cast<void*>(stringValue);
				vectorOut.push_back(secondElem);
				delete resobj;
				resobj = NULL;

			}
		}
		break;

		default:
			AES_CDH_TRACE_MESSAGE("Default Case");
			AES_CDH_LOG(LOG_LEVEL_INFO, "%s", "Default Case");
			break;
		}
		ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError,vectorOut);
		AES_CDH_TRACE_MESSAGE("result admin op :%d, myOIValidationError: %d",rc,myOIValidationError);
		if(rc != ACS_CC_SUCCESS)
		{
			AES_CDH_TRACE_MESSAGE("Admin op failed , error : %d ",myOIValidationError);
		}
	}
	else
	{
		ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, true,vectorOut);
		AES_CDH_TRACE_MESSAGE("result admin op :%d",rc);
	}

	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of adminOperationCallback

/*===================================================================
   ROUTINE: apply
=================================================================== */
void AES_CDH_BlockDestCmdHandler::apply(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "apply callback triggered");
	(void)oiHandle;
	(void)ccbId;
	if( m_secDestCreateccbId == ccbId)
	{
		ccbId_mtx.acquire();
		if(ServR::pBlockDestInfo  != 0)
		{
			for(int i = 0; i < ServR::pBlockDestInfo->argc ; i++)
			{
				delete[] ServR::pBlockDestInfo->argv[i];
				ServR::pBlockDestInfo->argv[i] = 0;
			}
			ServR::pBlockDestInfo->argc = 0;
			delete ServR::pBlockDestInfo;
			ServR::pBlockDestInfo = 0;
		}
		ccbId_mtx.release();
		m_secDestCreateccbId=0;
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

		vector <destInfo>::iterator itr = m_modifyDestList.begin();
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

		vector <destInfo>::iterator itr = m_deleteDestList.begin();
		while( itr != m_deleteDestList.end())
		{
			m_deleteDestList.erase(itr);
			itr = m_deleteDestList.begin();
		}
	}

	//do nothing
	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of apply

/*===================================================================
   ROUTINE: DestOIThread
=================================================================== */
ACE_THR_FUNC_RETURN AES_CDH_BlockDestCmdHandler::DestOIThread(void* aCDHDestPtr)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_BlockDestCmdHandler * myDestImplementer = 0;
	myDestImplementer = reinterpret_cast<AES_CDH_BlockDestCmdHandler*>(aCDHDestPtr);

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
		AES_CDH_TRACE_MESSAGE("myDestImplementer is NULL");
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return 0;
}//End of DestOIThread

/*===================================================================
   ROUTINE: setupDestOIThread
=================================================================== */
ACS_CC_ReturnType AES_CDH_BlockDestCmdHandler::setupDestOIThread(AES_CDH_BlockDestCmdHandler * aDestImplementer,
                               ACE_Thread_Manager* threadManager_)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	int myGroupId = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	if( aDestImplementer->setObjectImplementer() != ACS_CC_SUCCESS)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "setObjectImplementer failed");
		AES_CDH_TRACE_MESSAGE("setObjectImplementer failed");
		return ACS_CC_FAILURE;
	}

	myGroupId = threadManager_->spawn(&DestOIThread,
			(void*)aDestImplementer ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&m_DestOIThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE);

	AES_CDH_TRACE_MESSAGE("Spawning the  Dest OI Thread");
	AES_CDH_LOG(LOG_LEVEL_INFO, "Spawning the  Dest OI Thread");
	if(myGroupId == -1 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Spawning of DestOIThread failed");
		AES_CDH_TRACE_MESSAGE("Spawning of DestOIThread failed");
		rc = ACS_CC_FAILURE;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rc;

}//End of setupDestThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void AES_CDH_BlockDestCmdHandler::shutdown()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "shutdown started");
	m_poReactor->end_reactor_event_loop();

	if( m_isClassImplAdded == true )
	{
		for (int i=0; i < 10; ++i)
		{
			if( m_poOiHandler->removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE )
			{
				int intErr = getInternalLastError();
				if ( intErr == -6)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if ( i >= 10 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing class implementer, ErrCode = %d", intErr);
						AES_CDH_TRACE_MESSAGE("Error occured while removing class implementer, ErrCode = %d", intErr);
						break;
					}
					else
						continue;
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing class implementer, ErrCode = %d", intErr);
					AES_CDH_TRACE_MESSAGE("Error occured while removing class implementer, ErrCode = %d", intErr);
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
	if( m_DestOIThreadId != 0 )
	{
		ACE_Thread_Manager::instance()->join(m_DestOIThreadId);
	}

	if(!m_isClassImplAdded)
		AES_CDH_LOG(LOG_LEVEL_INFO,"Removed class implementer for class %s successfully !!",theClassName.c_str());

	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of shutdown

/*===================================================================
	ROUTINE: ~AES_CDH_BlockDestCmdHandler
=================================================================== */
AES_CDH_BlockDestCmdHandler::~AES_CDH_BlockDestCmdHandler()
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
void AES_CDH_BlockDestCmdHandler::extractDestName(const string attrName,
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
		pos2 = attrName.find_first_of(",");

		if(( pos1 != string::npos) && ( pos2 != string::npos ))
		{
			destinationName = attrName.substr(pos1+1, pos2-pos1-1);
		}

	}
	AES_CDH_TRACE_MESSAGE("Leaving extractDestName = %s", destinationName.c_str());

}

/*===================================================================
        ROUTINE: extractClassNameFromRdn
=================================================================== */
void AES_CDH_BlockDestCmdHandler::extractClassNameFromRdn( const string myObjRdn,
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
bool AES_CDH_BlockDestCmdHandler::isIPAddrValid( const char * aIPAddr )
{
	AES_CDH_TRACE_MESSAGE("Entering");

	if ( aIPAddr == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving with false");
		AES_CDH_TRACE_MESSAGE("Leaving  with false");
		return false;
	}
	unsigned int b1 = 0, b2 =0, b3 = 0, b4 = 0;
	unsigned char c = 0;
	if( sscanf(aIPAddr, "%3u.%3u.%3u.%3u%c", &b1, &b2, &b3, &b4, &c ) != 4 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving  with false");
		AES_CDH_TRACE_MESSAGE("Leaving with false");
		return false;
	}

	if(( b1 | b2 | b3 | b4 ) > 255 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving with false");
		AES_CDH_TRACE_MESSAGE("Leaving with false");
		return false;
	}


	if( strspn( aIPAddr, "0123456789.") < ACE_OS::strlen( aIPAddr ))
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving with false");
		AES_CDH_TRACE_MESSAGE("Leaving with false");
		return false;
	}

	AES_CDH_TRACE_MESSAGE("Leaving with true");
	return true;
}

/*===================================================================
        ROUTINE: isDestNameValid
=================================================================== */
bool AES_CDH_BlockDestCmdHandler::isDestNameValid( const string& destName )
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if( destName.length() < 1  || destName.length() > 32 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving . Destination name is invalid since the length of the destination name exceeds limit");
		AES_CDH_TRACE_MESSAGE("Leaving . Destination name is invalid since the length of the destination name exceeds limit");
		return false;
	}
	const string legalChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	const string legalChars2("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (destName.find_first_not_of(legalChars) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving . Destination name is invalid since the destination name contains invalid characters");
		AES_CDH_TRACE_MESSAGE("Leaving . Destination name is invalid since the destination name contains invalid characters");
		return false;
	}
	if (destName.substr(0,1).find_first_not_of(legalChars2) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving . Destination name is invalid since the destination name starts with number");
		AES_CDH_TRACE_MESSAGE("Leaving . Destination name is invalid since the destination name starts with number");
		return false;
	}
	AES_CDH_TRACE_MESSAGE("Leaving . Destination name is valid");
	return true;
}
/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void AES_CDH_BlockDestCmdHandler::setErrorCode(unsigned int code,const char* objName)
{
	AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(code,"");
	string ertxt=resobj->errorText();
	AES_CDH_LOG(LOG_LEVEL_INFO, "setExitCode : exitCode = %d , errorText = %s",code,ertxt.c_str());
	AES_CDH_TRACE_MESSAGE("setExitCode : exitCode = %d , errorText = %s",code,ertxt.c_str());
	if( code == AES_CDH_RC_UNREAS || code == AES_CDH_RC_UNREAS_BLOCKIP || code == AES_CDH_RC_UNREAS_BLOCKRETRYDELAY
			|| code == AES_CDH_RC_UNREAS_BLOCKRETRIES || code == AES_CDH_RC_UNREAS_PRIBLOCKDESTNAME
			|| code == AES_CDH_RC_UNREAS_SECBLOCKDESTNAME || code == AES_CDH_RC_UNREAS_ADVBLOCKDESTNAME
			|| code == AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6 || code == AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE)
	{
		if( objName != 0 )
		{
			ertxt = ertxt + " in " + objName;
		}
	}

	setExitCode(code,ertxt );
	delete resobj;
}

/*===================================================================
        ROUTINE: getExistingDestObjFromIMM
=================================================================== */
bool AES_CDH_BlockDestCmdHandler::getExistingDestObjFromIMM( const char *objName, int &tmpArgc, char** tmpArgv )
{
	AES_CDH_TRACE_MESSAGE("Entering");

	OmHandler myOmhandler;

	//Initialize OmHandler
	if( myOmhandler.Init() == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "OmHandler initialization failed.");
		AES_CDH_TRACE_MESSAGE("OmHandler initialization failed.");
		return false;
	}

	//Get the data from IMM.

	ACS_APGCC_ImmObject destObj;
	destObj.objName = objName;

	if( myOmhandler.getObject( &destObj) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting object from IMM");
		AES_CDH_TRACE_MESSAGE("Error occured while getting object from IMM");
		myOmhandler.Finalize();
		return false;

	}

	//Read the attributes of the primary and secondary block objects
	for ( unsigned int cntr = 0; cntr  < destObj.attributes.size(); cntr++ )
	{
		if (destObj.attributes[cntr].attrName ==  AES_CDH_BLOCK_DEST_IP_ADDR)
		{
			string ipAddress("");
			tmpArgv[tmpArgc] = new char [3] ;
			ACE_OS::strcpy( tmpArgv[tmpArgc++], "-a");

			if(destObj.attributes[cntr].attrValuesNum !=0 )
			{
				char *mytmp = reinterpret_cast<char *>(destObj.attributes[cntr].attrValues[0]);
				if( mytmp != 0 )
				{
					ipAddress.assign(mytmp);
				}
			}
			tmpArgv[tmpArgc] = new char [strlen(ipAddress.c_str())+1 ] ;
			ACE_OS::strcpy( tmpArgv[tmpArgc++], ipAddress.c_str());
		}
	}
	myOmhandler.Finalize();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;
}

/*===================================================================
        ROUTINE: getExistingAdvDestObjFromIMM
=================================================================== */
bool AES_CDH_BlockDestCmdHandler::getExistingAdvDestObjFromIMM( const char *objName, int &tmpArgc, char** tmpArgv )
{
	AES_CDH_TRACE_MESSAGE("Entering");

	OmHandler myOmhandler;

	//Initialize OmHandler
	if( myOmhandler.Init() == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "OmHandler initialization failed.");
		AES_CDH_TRACE_MESSAGE("OmHandler initialization failed.");
		return false;
	}

	//Get the data from IMM.

	ACS_APGCC_ImmObject destObj;
	destObj.objName = objName;

	if( myOmhandler.getObject( &destObj) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting object from IMM");
		AES_CDH_TRACE_MESSAGE("Error occured while getting object from IMM");
		myOmhandler.Finalize();
		return false;
	}
	myOmhandler.Finalize();

	//Read the attributes of the advanced initiating object
	for ( unsigned int cntr = 0; cntr  < destObj.attributes.size(); cntr++ )
	{

		if ( destObj.attributes[cntr].attrName == AES_CDH_BLOCK_RETRYDELAY)
		{
			int retryDelay =-1;
			if (destObj.attributes[cntr].attrValuesNum != 0 )
			{
				retryDelay = *reinterpret_cast<ACE_INT32 *>(destObj.attributes[cntr].attrValues[0]);
			}
			// Copy the retryDelay option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-m");

			// Copy the retryDelay value
			char myRetryDelay[11] = { 0 };
			ACE_OS::sprintf( myRetryDelay, "%d", retryDelay);
			tmpArgv[tmpArgc] = new char[ACE_OS::strlen(myRetryDelay)+1];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], myRetryDelay);

		}
		else if ( destObj.attributes[cntr].attrName == AES_CDH_BLOCK_SENDRETRY)
		{
			int sendRetries = -1 ;
			if (destObj.attributes[cntr].attrValuesNum != 0 )
			{
				sendRetries = *reinterpret_cast<ACE_INT32 *>(destObj.attributes[cntr].attrValues[0]);
			}
			// Copy the sendRetries option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-s");

			// Copy the sendRetries value
			char mySendRetries[11] = { 0 };
			ACE_OS::sprintf( mySendRetries, "%d", sendRetries);
			tmpArgv[tmpArgc] = new char[ACE_OS::strlen(mySendRetries)+1];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], mySendRetries);

		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;
}
