/*=================================================================== */
/**
   @file aes_cdh_file_init_dest_handler.cpp

   Class method implementationn for CDH module.

   This module contains the implementation of class declared in
   the aes_cdh_file_init_dest_handler.h file.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/11/2011   XCHEMAD   Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_cdh_file_init_dest_handler.h>
#include <ace/Task.h>
#include <acs_apgcc_omhandler.h>
#include <aes_cdh_result_r1.h>
#include <ACS_APGCC_Util.H>
#include <aes_cdh_common.h>
#include <servr.h>
#include <transdest.h>
#include <aes_cdh_brfc_interface.h>
#include <aes_gcc_util.h>
#include <crypto_status.h>
#include <crypto_api.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_FileInitDestCmdHandler);
ACE_Recursive_Thread_Mutex AES_CDH_FileInitDestCmdHandler::ccbId_mtx;
CommitInfo AES_CDH_FileInitDestCmdHandler::m_iCommit ;
/*===================================================================
   ROUTINE: AES_CDH_FileInitDestCmdHandler
=================================================================== */
AES_CDH_FileInitDestCmdHandler::AES_CDH_FileInitDestCmdHandler(
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
	m_passDestInfoList.clear();
	m_modifyDestList.clear();
	m_deleteDestList.clear();
	/** Create Reactor to handle the events**/
	m_poTp_reactor = new(std::nothrow) ACE_TP_Reactor;
	ServR::pFileInitDestInfo=0;
	m_primDestCreateccbId = 0;
	m_secDestCreateccbId = 0;
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
ACS_CC_ReturnType AES_CDH_FileInitDestCmdHandler::setObjectImplementer()
{
	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;

	AES_CDH_TRACE_MESSAGE("Entering");

	for (int i = 0; i < 10; ++i)
	{
		errorCode = m_poOiHandler->addClassImpl(this, theClassName.c_str());
		if( errorCode == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Error occured while add class implmenter for Destination");
			int intErr = getInternalLastError();
			for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
			if ( i >= 10)
			{
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Error occured while add class implmenter for Destination, ErrorCode = %d", intErr);
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
ACS_CC_ReturnType AES_CDH_FileInitDestCmdHandler::create(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *className,
		const char* parentName,
		ACS_APGCC_AttrValues **attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"ObjectCreateCallback invoked");
	(void)oiHandle;

	int i = 0;
	if( ACE_OS::strcmp( className, AES_CDH_PRIMARYINITDEST_IMM_CLASS_NM) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Creating initiating destination");
		m_primDestCreateccbId = ccbId;
		string destinationRdn("");
		string destinationName("");
		string destinationSetName("");
		string ipAddress("");
		string password("");
		string userName("");
		string remoteFolderPath("");
		string transferProtocol("");
		string myDetailInfo("");
		ACE_INT32 portNumber = -1;
		PassDestInfo *pPassDestInfo = 0;

		extractDestName(parentName, "OBJNAME", destinationSetName);
		AES_CDH_TRACE_MESSAGE("Create callback %s",destinationSetName.c_str());
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

		int ctr = 0;
		while( attr[ctr] )
		{
			if ( ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_PRIMARYINITDEST_RDN) == 0 )
			{
				destinationRdn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE("Primary destination Rdn is %s", destinationRdn.c_str());
			}
			ctr++;
		}
		while( attr[i] )
		{
			if (ACE_OS::strcmp(attr[i]->attrName, AES_CDH_REMOTE_IP_ADDR) == 0)
			{
				if(attr[i]->attrValuesNum!=0)
				{
					char *mytmp = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					if( mytmp != 0 )
					{
						AES_CDH_TRACE_MESSAGE("Primary destination IP address  = %s", mytmp);
						ipAddress.assign(mytmp);

						int validationResult = AES_CDH_Common_Util::isIPAddressValid(ipAddress.c_str());	//IPv6_feature:: validate IPv4 and IPv6 formats
						if(validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
						{
							AES_CDH_LOG(LOG_LEVEL_ERROR, "Destination Address is not a valid IP Address");
							ccbId_mtx.acquire();
							if(ServR::pFileInitDestInfo != 0 )
							{
								for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
								{
									delete[] ServR::pFileInitDestInfo->argv[ctr];
									ServR::pFileInitDestInfo->argv[ctr] = 0;
								}
								delete ServR::pFileInitDestInfo;
								ServR::pFileInitDestInfo = 0;
							}
							ccbId_mtx.release();
							//set error code for invalid value
							string objNm = destinationRdn + "," + parentName;
							if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6 , objNm.c_str());
							else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE , objNm.c_str());
							else
								setErrorCode(AES_CDH_RC_UNREAS_INITIP , objNm.c_str());
							return ACS_CC_FAILURE;
						}
					}

				}
				else
				{
					//set error code for incorrect usage
					setErrorCode(AES_CDH_RC_INCUSAGE);
					ccbId_mtx.acquire();
					if(ServR::pFileInitDestInfo != 0 )
					{
						for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
						{
							delete[] ServR::pFileInitDestInfo->argv[ctr];
							ServR::pFileInitDestInfo->argv[ctr] = 0;
						}
						delete ServR::pFileInitDestInfo;
						ServR::pFileInitDestInfo = 0;
					}
					ccbId_mtx.release();
					return ACS_CC_FAILURE;
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_PASSWORD) == 0 )
			{
				if(attr[i]->attrValuesNum != 0 )
				{
					password = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					pPassDestInfo = new PassDestInfo();
					pPassDestInfo->passwdDn = password;
					AES_CDH_TRACE_MESSAGE("Primary destination Password DN = %s", password.c_str());

				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_DEST_PORTNUMBER) == 0 )
			{
				if(attr[i]->attrValuesNum!=0)
				{
					portNumber = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Primary Destination Port Number = %d", portNumber);
					if( portNumber < 0 || portNumber > 65535 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Port No is out of range");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_INITPORTNO , objNm.c_str());
						ccbId_mtx.acquire();
						if(ServR::pFileInitDestInfo != 0 )
						{
							for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
							{
								delete[] ServR::pFileInitDestInfo->argv[ctr];
								ServR::pFileInitDestInfo->argv[ctr] = 0;
							}
							delete ServR::pFileInitDestInfo;
							ServR::pFileInitDestInfo = 0;
						}
						ccbId_mtx.release();
						return ACS_CC_FAILURE;
					}
				}	
			}
                        else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_FILE_REMOTE_FOLDER_PATH) == 0 )
                        {
                                if(attr[i]->attrValuesNum!=0)
                                {
                                        remoteFolderPath = reinterpret_cast<char *>(attr[i]->attrValues[0]);
                                        if (remoteFolderPath.find('#') != std::string::npos)    //TR_HX21780_start
                                        {
                                                AES_CDH_LOG(LOG_LEVEL_ERROR, "File Remote folder Path is invalid");
                                                //set error code for invalid character
                                                setErrorCode(AES_CDH_RC_ILLEGALREMOTEFOLDERPATH);
                                                return ACS_CC_FAILURE;
                                        }
                                        else
                                        {
                                                AES_CDH_TRACE_MESSAGE("File Remote folder Path = %s", remoteFolderPath.c_str());
                                                //set error code for success scenario
                                                setErrorCode(AES_CDH_RC_OK);
                                        }    //TR_HX21780_end
                                }
                        }
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_FILE_USERNAME) == 0 )
			{
				if (attr[i]->attrValuesNum!=0)
				{
					userName = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Primary destination user name = %s", userName.c_str());
				}

			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_TRANSFER_PROTOCOL) == 0 )
			{
				if (attr[i]->attrValuesNum!=0)
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE("File Remote Transfer Type = %d", mytmp);
					if( mytmp < 0 || mytmp > 1 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Transfer Type is invalid");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_FILETRANSFERTYPE , objNm.c_str());
						ccbId_mtx.acquire();
						if(ServR::pFileInitDestInfo != 0 )
						{
							for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
							{
								delete[] ServR::pFileInitDestInfo->argv[ctr];
								ServR::pFileInitDestInfo->argv[ctr] = 0;
							}
							delete ServR::pFileInitDestInfo;
							ServR::pFileInitDestInfo = 0;
						}
						ccbId_mtx.release();
						return ACS_CC_FAILURE;
					}
					switch( mytmp )
					{
					case 0:
						transferProtocol = AES_CDH_SFTPV2_TRANSFER_TYPE;
						break;

					case 1:
						transferProtocol = AES_CDH_FTPV2_TRANSFER_TYPE;
						break;
					}
				}
				else
				{
					//set error code for incorrect usage
					setErrorCode(AES_CDH_RC_INCUSAGE);
					ccbId_mtx.acquire();
					if(ServR::pFileInitDestInfo != 0 )
					{
						for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
						{
							delete[] ServR::pFileInitDestInfo->argv[ctr];
							ServR::pFileInitDestInfo->argv[ctr] = 0;
						}
						delete ServR::pFileInitDestInfo;
						ServR::pFileInitDestInfo = 0;
					}
					ccbId_mtx.release();
					return ACS_CC_FAILURE;
				}
			}
			i++;
		}

		extractDestName( destinationRdn, "RDN", destinationName);
		if( pPassDestInfo != 0 )
		{
			pPassDestInfo->destinationName = destinationName;	
			m_passDestInfoList.push_back(pPassDestInfo);
		}

		if ( isDestNameValid(destinationName) == false)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving   since the destination name is invalid");
			//set error code for invalid value
			string objNm = destinationRdn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS_PRIINITDESTNAME , objNm.c_str());
			ccbId_mtx.acquire();
			if(ServR::pFileInitDestInfo != 0 )
			{
				for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileInitDestInfo->argv[ctr];
					ServR::pFileInitDestInfo->argv[ctr] = 0;
				}
				delete ServR::pFileInitDestInfo;
				ServR::pFileInitDestInfo = 0;
			}
			ccbId_mtx.release();
			return ACS_CC_FAILURE;
		}

		//get the destination set name.
		extractDestName(parentName, "OBJNAME", destinationSetName);

		if ( transferProtocol != "" && transferProtocol == AES_CDH_SFTPV2_TRANSFER_TYPE )
		{
			AES_CDH_TRACE_MESSAGE("remoteUserName is %s", userName.c_str());
			AES_CDH_TRACE_MESSAGE("password is %s", password.c_str());
			if ( userName.empty() || userName == "anonymous" )
			{
				AES_CDH_TRACE_MESSAGE("%s", "remoteUserName is mandatory for SFTPV2 protocol");
				//set error code for incorrect usage
				string objNm = destinationRdn + "," + parentName;
				setErrorCode(AES_CDH_RC_SFTP_UNAME,objNm.c_str());
				ccbId_mtx.acquire();
				if(ServR::pFileInitDestInfo != 0 )
				{
					for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
					{
						delete[] ServR::pFileInitDestInfo->argv[ctr];
						ServR::pFileInitDestInfo->argv[ctr] = 0;
					}
					delete ServR::pFileInitDestInfo;
					ServR::pFileInitDestInfo = 0;
				}
				ccbId_mtx.release();
				return ACS_CC_FAILURE;

			}
			if ( password.empty())
			{
				AES_CDH_TRACE_MESSAGE("remotePassword is mandatory for SFTPV2 protocol");
				//set error code for incorrect usage
				string objNm = destinationRdn + "," + parentName;
				setErrorCode(AES_CDH_RC_SFTP_PASSWD,objNm.c_str());
				ccbId_mtx.acquire();
				if(ServR::pFileInitDestInfo != 0 )
				{
					for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
					{
						delete[] ServR::pFileInitDestInfo->argv[ctr];
						ServR::pFileInitDestInfo->argv[ctr] = 0;
					}
					delete ServR::pFileInitDestInfo;
					ServR::pFileInitDestInfo = 0;
				}
				ccbId_mtx.release();
				return ACS_CC_FAILURE;

			}
		}
		else if ( transferProtocol != "" && transferProtocol == AES_CDH_FTPV2_TRANSFER_TYPE )
		{
			if ( !password.empty() && password[0] != '0' )
			{
				if (userName == "" || userName == "anonymous")
				{
					AES_CDH_TRACE_MESSAGE("remoteUserName is mandatory when remotePassword is present for FTPV2 protocol");
					//set error code for incorrect usage
					string objNm = destinationRdn + "," + parentName;
					setErrorCode(AES_CDH_RC_FTP_UNAME,objNm.c_str());
					ccbId_mtx.acquire();
					if(ServR::pFileInitDestInfo != 0 )
					{
						for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
						{
							delete[] ServR::pFileInitDestInfo->argv[ctr];
							ServR::pFileInitDestInfo->argv[ctr] = 0;
						}
						delete ServR::pFileInitDestInfo;
						ServR::pFileInitDestInfo = 0;
					}
					ccbId_mtx.release();
					return ACS_CC_FAILURE;

				}
			}
		}

		if(ServR::pFileInitDestInfo == 0)
		{
			ServR::pFileInitDestInfo = new(std::nothrow) destInfo;
			if(ServR::pFileInitDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pFileInitDestInfo->argc = 0;
			//Copy the name of the command.
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], AES_CDH_CDHDEFINE);
		}
		ServR::pFileInitDestInfo->ccbId = ccbId;
		ServR::pFileInitDestInfo->destinationName = destinationName;
		ServR::pFileInitDestInfo->isSecDest = false;
		ServR::pFileInitDestInfo->isDestSet = false;
		ServR::pFileInitDestInfo->destinationSetName = destinationSetName;
		ServR::pFileInitDestInfo->transferType = transferProtocol;
		//int argc = 0;
		//char *argv[32] = { 0 };

		// Copy connection type
		ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
		ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-c");
		ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[2];
		ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "i");

		if( !ipAddress.empty())
		{
			//Copy the destination address option.
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-a");

			//Copy the destination address.
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ipAddress.size() + 1];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], ipAddress.c_str());
		}

		if( !password.empty())
		{
			//Copy the password option
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-p");

			//Copy the pasword
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[password.size() + 1 ];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], password.c_str());
		}

		if ( !userName.empty())
		{
			//Copy the user name option
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-u");

			//Copy the user name
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[userName.size() + 1 ];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], userName.c_str());
		}

		if ( !remoteFolderPath.empty())
		{
			//Copy the remote folder path option
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-r");

			//Copy the remote folder path
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[remoteFolderPath.size() + 1];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], remoteFolderPath.c_str());
		}


		if ( portNumber != -1 )
		{
			//Copy the port number option
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-n");

			//Copy the port number
			char myPortNo[11] = { 0 };
			ACE_OS::sprintf( myPortNo, "%d", portNumber);
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ACE_OS::strlen(myPortNo)+1];
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], myPortNo);
		}
		ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = 0;
		//set error code for success scenario
		setErrorCode(AES_CDH_RC_OK);
		return ACS_CC_SUCCESS;
	}
	else if ( ACE_OS::strcmp( className, AES_CDH_SECONDARYINITDEST_IMM_CLASS_NM) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Creating initiating destination");
		m_secDestCreateccbId = ccbId;
		string destinationRdn("");
		string destinationName("");
		string destinationSetName("");
		string ipAddress("");
		string password("");
		string userName("");
		string remoteFolderPath("");
		string transferProtocol("");
		ACE_INT32 portNumber = -1;
		ACE_INT32 fileOverwrite = -1;
		ACE_INT32 sendRetries = -1;
		ACE_INT32 retryDelay = -1;
		ACE_INT32 supSubFolderCreation = -1;
		string myDetailInfo("");
		PassDestInfo *pPassDestInfo = 0;
		extractDestName(parentName, "OBJNAME", destinationSetName);
		AES_CDH_TRACE_MESSAGE("Create callback %s",destinationSetName.c_str());
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

		int ctr = 0;
		while(attr[ctr])
		{
			if (ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_SECONDARYINITDEST_RDN) == 0 )
			{
				destinationRdn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE( "Secondary destination Rdn is %s", destinationRdn.c_str());
			}
			ctr++;
		}

		while( attr[i] )
		{
			if (ACE_OS::strcmp(attr[i]->attrName, AES_CDH_REMOTE_IP_ADDR) == 0)
			{
				if(attr[i]->attrValuesNum!=0)
				{
					char *mytmp = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					if( mytmp != 0 )
					{
						AES_CDH_TRACE_MESSAGE("Secondary destination IP address  = %s", mytmp);
						ipAddress.assign(mytmp);
						int validationResult = AES_CDH_Common_Util::isIPAddressValid(ipAddress.c_str());	//IPv6_feature:: validate IPv4 and IPv6 formats
						if (validationResult != AES_CDH_Common_Util::VALID_ADDRESS)
						{
							AES_CDH_LOG(LOG_LEVEL_ERROR,  "Destination Address is not a valid IP Address");
							//set error code for invalid value
							string objNm = destinationRdn + "," + parentName;
							if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6 , objNm.c_str());
							else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
								setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE , objNm.c_str());
							else
								setErrorCode(AES_CDH_RC_UNREAS_INITIP , objNm.c_str());
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
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_PASSWORD) == 0 )
			{
				if(attr[i]->attrValuesNum!=0)
				{
					password = reinterpret_cast<char *>(attr[i]->attrValues[0]);

					AES_CDH_TRACE_MESSAGE( "Secondary destination Password = %s", password.c_str());

					pPassDestInfo = new PassDestInfo();
					pPassDestInfo->passwdDn = password;
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_DEST_PORTNUMBER) == 0 )
			{
				if(attr[i]->attrValuesNum!=0)
				{
					portNumber = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "Secondary Destination Port Number = %d", portNumber);
					if( portNumber < 0 || portNumber > 65535 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Port No is out of range");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_INITPORTNO , objNm.c_str());
						return ACS_CC_FAILURE;
					}
				}	
			}
                        else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_FILE_REMOTE_FOLDER_PATH) == 0 )
                        {
                                if(attr[i]->attrValuesNum!=0)
                                {
                                        remoteFolderPath = reinterpret_cast<char *>(attr[i]->attrValues[0]);
                                        if (remoteFolderPath.find('#') != std::string::npos)    //TR_HX21780_start
                                        {
                                                AES_CDH_LOG(LOG_LEVEL_ERROR, "File Remote folder Path is invalid");
                                                //set error code for invalid character
                                                setErrorCode(AES_CDH_RC_ILLEGALREMOTEFOLDERPATH);
                                                return ACS_CC_FAILURE;
                                        }
                                        else
                                        {
                                                AES_CDH_TRACE_MESSAGE("File Remote folder Path = %s", remoteFolderPath.c_str());
                                                //set error code for success scenario
                                                setErrorCode(AES_CDH_RC_OK);    //TR_HX21780_end
                                        }
                                        //TR HR50274
                                }
                        }
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_FILE_USERNAME) == 0 )
			{
				if (attr[i]->attrValuesNum!=0)
				{
					userName = reinterpret_cast<char *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "Primary destination user name = %s", userName.c_str());
				}

			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_TRANSFER_PROTOCOL) == 0 )
			{
				if (attr[i]->attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "File Remote Transfer Type = %d", mytmp);
					if( mytmp < 0 || mytmp > 1 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Transfer Type is invalid");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_FILETRANSFERTYPE , objNm.c_str());
						return ACS_CC_FAILURE;
					}
					switch( mytmp )
					{
					case 0:
						transferProtocol = AES_CDH_SFTPV2_TRANSFER_TYPE;
						break;

					case 1:
						transferProtocol = AES_CDH_FTPV2_TRANSFER_TYPE;
						break;
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

		extractDestName( destinationRdn, "RDN", destinationName);
		if( pPassDestInfo != 0 )
		{
			pPassDestInfo->destinationName = destinationName;	
			m_passDestInfoList.push_back(pPassDestInfo);
		}


		if ( isDestNameValid(destinationName) == false)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving since the destination name is invalid");
			//set error code for invalid value
			string objNm = destinationRdn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS_SECINITDESTNAME , objNm.c_str());
			return ACS_CC_FAILURE;
		}

		OmHandler omHandler;
		if( omHandler.Init() == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Initializing the Om handler for getting advanced initiating parameters failed");
			//set error code for internal error
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}

		// Get the destination set name from the parent name
		extractDestName(parentName, "OBJNAME", destinationSetName);

		std::vector<std::string>  destinationList;
		omHandler.getClassInstances(AES_CDH_SECONDARYINITDEST_IMM_CLASS_NM, destinationList);
		for( unsigned int cntr = 0 ; cntr < destinationList.size() ; ++cntr)
		{
			string tmpDestinationSetName("");
			size_t pos1 =  string(destinationList[cntr]).find_first_of(",");
			tmpDestinationSetName= destinationList[cntr].substr(pos1+1);
			size_t pos2 = tmpDestinationSetName.find_first_of("=");
			size_t pos3 = tmpDestinationSetName.find_first_of(",");

			tmpDestinationSetName = tmpDestinationSetName.substr(pos2+1, pos3-pos2-1);
			//TR HR49259 
			if( ACE_OS::strcmp(tmpDestinationSetName.c_str(), destinationSetName.c_str()) == 0 )
			{
				//Do not allow creation of one more instance of Secondary Initiating destination if already exists
				AES_CDH_LOG(LOG_LEVEL_INFO, "SecondaryInitiatingDestionation already exists for this destination set.");
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
			if( myClassName == AES_CDH_ADVINITPARAMS_IMM_RDN_CL_NM )
			{
				objName = p_rdnList[rdnCntr];
				break;
			}
		}
		if(objName.empty())
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Om handler could not fetch the advanced initiating params object ");
			//Error code if primary dest is not created
			setErrorCode(AES_CDH_RC_NOADVPARAM,parentName);
			omHandler.Finalize();
			return ACS_CC_FAILURE;
		}

		ACS_APGCC_ImmObject advInitParamsObj;
		advInitParamsObj.objName = objName;

		//Get the advanced initiating params object

		if( omHandler.getObject( &advInitParamsObj) == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR,  "Om handler could not fetch the advanced initiating params object ");
			//Error code if primary dest is not created
			setErrorCode(AES_CDH_RC_NOPRIDEST);
			omHandler.Finalize();
			return ACS_CC_FAILURE;
		}

		omHandler.Finalize();

		//Read the attributes of the advanced initiating object
		for ( unsigned int cntr = 0; cntr  < advInitParamsObj.attributes.size(); cntr++ )
		{
			// To avoid the issue of not displaying default values in cdhls printout during the creation of
			// Secondary Initiating Destination Set
			// fileOverwrite,retryDelay,sendRetries,supSubFolderCreation values are assigned directly from IMM
			// without checking whether it is default or not
			if( advInitParamsObj.attributes[cntr].attrName == AES_CDH_FILE_FILEOVERWRITE )
			{
				if (advInitParamsObj.attributes[cntr].attrValuesNum != 0)
				{
					fileOverwrite = *reinterpret_cast<ACE_INT32 *>(advInitParamsObj.attributes[cntr].attrValues[0]);
					AES_CDH_TRACE_MESSAGE( " File Overwrite Option is %d", fileOverwrite);
				}
			}
			else if(advInitParamsObj.attributes[cntr].attrName == AES_CDH_FILE_RETRYDELAY )
			{
				if (advInitParamsObj.attributes[cntr].attrValuesNum != 0)
				{
					retryDelay = *reinterpret_cast<ACE_INT32 *>(advInitParamsObj.attributes[cntr].attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "File retry delay value is %d", retryDelay);
				}
			}
			else if (advInitParamsObj.attributes[cntr].attrName == AES_CDH_FILE_SENDRETRY )
			{
				if (advInitParamsObj.attributes[cntr].attrValuesNum != 0)
				{
					sendRetries  = *reinterpret_cast<ACE_INT32 *>(advInitParamsObj.attributes[cntr].attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "Send retry value is %d", sendRetries);
				}
			}
			else if(advInitParamsObj.attributes[cntr].attrName == AES_CDH_SUPPRESS_SUB_FOLDER_CREATION )
			{
				if (advInitParamsObj.attributes[cntr].attrValuesNum != 0)
				{
					supSubFolderCreation = *reinterpret_cast<ACE_INT32 *>(advInitParamsObj.attributes[cntr].attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "Suppress sub folder creation value is %d", supSubFolderCreation);
				}
			}
		}

		if ( transferProtocol != "" && transferProtocol == AES_CDH_SFTPV2_TRANSFER_TYPE )
		{
			if (userName.empty() || userName == "anonymous")
			{
				AES_CDH_TRACE_MESSAGE("remoteUserName and remotePassword are mandatory for SFTPV2 protocol");
				//set error code for incorrect usage
				string objNm = destinationRdn + "," + parentName;
				setErrorCode(AES_CDH_RC_SFTP_UNAME,objNm.c_str());
				return ACS_CC_FAILURE;
			}

			if (password.empty())
			{
				AES_CDH_TRACE_MESSAGE("remoteUserName and remotePassword are mandatory for SFTPV2 protocol");
				//set error code for incorrect usage
				string objNm = destinationRdn + "," + parentName;
				setErrorCode(AES_CDH_RC_SFTP_PASSWD,objNm.c_str());
				return ACS_CC_FAILURE;
			}

		}
		else if ( transferProtocol != "" && transferProtocol == AES_CDH_FTPV2_TRANSFER_TYPE )
		{
			if ( !password.empty() && password[0] != '0' )
			{
				if (userName == "" || userName == "anonymous")
				{
					AES_CDH_TRACE_MESSAGE("remoteUserName is mandatory when remotePassword is present for FTPV2 protocol");
					//set error code for incorrect usage
					string objNm = destinationRdn + "," + parentName;
					setErrorCode(AES_CDH_RC_FTP_UNAME,objNm.c_str());
					return ACS_CC_FAILURE;

				}
			}
		}

		// Create the destset 
		int argc = 0;
		char *argv[32] = { 0 };

		//Copy the name of the command.
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

		// Copy connection type
		argv[argc] = new char[3];
		ACE_OS::strcpy(argv[argc++], "-c");
		argv[argc] = new char[2];
		ACE_OS::strcpy(argv[argc++], "i");

		if( !ipAddress.empty())
		{
			//Copy the destination address option.
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-a");

			//Copy the destination address.
			argv[argc] = new char[ipAddress.size() + 1];
			ACE_OS::strcpy(argv[argc++], ipAddress.c_str());
		}

		if( !password.empty())
		{
			//Copy the password option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-p");

			//Copy the pasword
			argv[argc] = new char[password.size() + 1];
			ACE_OS::strcpy(argv[argc++], password.c_str());
		}

		if ( !userName.empty())
		{
			//Copy the user name option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-u");

			//Copy the user name
			argv[argc] = new char[userName.size() + 1];
			ACE_OS::strcpy(argv[argc++], userName.c_str());
		}

		if ( !remoteFolderPath.empty())
		{
			//Copy the remote folder path option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-r");

			//Copy the remote folder path
			argv[argc] = new char[remoteFolderPath.size() + 1];
			ACE_OS::strcpy(argv[argc++], remoteFolderPath.c_str());
		}


		if ( portNumber != -1)
		{
			//Copy the port number option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-n");

			//Copy the port number
			char myPortNo[11] = { 0 };
			ACE_OS::sprintf( myPortNo, "%d", portNumber);
			argv[argc] = new char[ACE_OS::strlen(myPortNo)+1];
			ACE_OS::strcpy(argv[argc++], myPortNo);
		}

		if ( fileOverwrite != -1 )
		{
			// Copy the file overwrite option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-o");

			// Copy the file overwrite option
			switch( fileOverwrite )
			{
			case 0:
				argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
				ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
				break;

			case 1:
				argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
				ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
				break;
			}
		}

		if ( retryDelay != -1 )
		{
			// Copy the retryDelay option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-d");

			// Copy the retryDelay value
			char myRetryDelay[11] = { 0 };
			ACE_OS::sprintf( myRetryDelay, "%d", retryDelay);
			argv[argc] = new char[ACE_OS::strlen(myRetryDelay)+1];
			ACE_OS::strcpy(argv[argc++], myRetryDelay);

		}

		if ( sendRetries != -1 )
		{
			// Copy the sendRetries option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-s");

			// Copy the sendRetries value
			char mySendRetries[11] = { 0 };
			ACE_OS::sprintf( mySendRetries, "%d", sendRetries);
			argv[argc] = new char[ACE_OS::strlen(mySendRetries)+1];
			ACE_OS::strcpy(argv[argc++], mySendRetries);

		}

		if ( supSubFolderCreation != -1 )
		{
			// Copy the supSubFolderCreation option
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-g");

			// Copy the supSubFolderCreation value
			switch( supSubFolderCreation )
			{
			case 0:
				argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
				ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
				break;

			case 1:
				argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
				ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
				break;
			}
		}
		argv[argc] = 0;
		if(ServR::pFileInitDestInfo==0)
		{
			ServR::pFileInitDestInfo = new(std::nothrow) destInfo;
			if(ServR::pFileInitDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				for ( int ctr = 0; ctr < argc ; ctr++)
				{
					delete[] argv[ctr];
					argv[ctr] = 0;
				}
				return ACS_CC_FAILURE;
			}
			ServR::pFileInitDestInfo->argc = 0; //HY68026
		}
		ServR::pFileInitDestInfo->ccbId = ccbId;
		ServR::pFileInitDestInfo->destinationName = destinationName;
		ServR::pFileInitDestInfo->isPrimDest = false;
		ServR::pFileInitDestInfo->isSecDest = true;
		ServR::pFileInitDestInfo->isDestSet = false;
		ServR::pFileInitDestInfo->destinationSetName = destinationSetName;
		ServR::pFileInitDestInfo->transferType = transferProtocol;
		// Create the destination
		int result = ServR::define( destinationName, transferProtocol, argc, argv, "", AES_CDH_SECONDARYINITDEST_RDN,destinationSetName);
		if(result!= AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Error occured while defining destination");

			for ( int ctr = 0; ctr < argc ; ctr++)
			{
				delete[] argv[ctr];
				argv[ctr] = 0;
			}
			//set error code for the result of define
			string objNm = destinationRdn + "," + parentName;
			setErrorCode(result, objNm.c_str());
			ccbId_mtx.acquire();
			if(ServR::pFileInitDestInfo != 0 )
			{
				for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileInitDestInfo->argv[ctr];
					ServR::pFileInitDestInfo->argv[ctr] = 0;
				}
				delete ServR::pFileInitDestInfo;
				ServR::pFileInitDestInfo = 0;
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
		string primaryDest(""), backupDest(""), usrName("");
		string destSetName = destinationSetName;

		int result1= ServR::changeDestSet(destSetName, primaryDest, destinationName, backupDest, usrName);
		if(result1!= AES_CDH_RC_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while secondary ot destset");
			ServR::deleteDest(destinationName, myDetailInfo,  usrName );
			//set error code for the result of changeDestSet
			setErrorCode(result1);
			ccbId_mtx.acquire();
			if(ServR::pFileInitDestInfo != 0 )
			{
				for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileInitDestInfo->argv[ctr];
					ServR::pFileInitDestInfo->argv[ctr] = 0;
				}
				delete ServR::pFileInitDestInfo;
				ServR::pFileInitDestInfo = 0;
			}
			ccbId_mtx.release();
			return ACS_CC_FAILURE;
		}
		//set error code for success scenario
		setErrorCode(AES_CDH_RC_OK);
		return ACS_CC_SUCCESS;
	}
	else if( ACE_OS::strcmp( className, AES_CDH_ADVINITPARAMS_IMM_CLASS_NM) == 0 )
	{
		AES_CDH_TRACE_MESSAGE( "Creating AdvancedInitiatingParams ");
		string destinationSetName;
		extractDestName(parentName, "OBJNAME", destinationSetName);
		AES_CDH_TRACE_MESSAGE( "Create callback %s",destinationSetName.c_str());
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
		string destinationRdn("");
		if(ServR::pFileInitDestInfo == 0)
		{
			ServR::pFileInitDestInfo = new(std::nothrow) destInfo;
			if(ServR::pFileInitDestInfo == NULL)
			{
				setErrorCode(AES_CDH_RC_INTPROGERR);
				return ACS_CC_FAILURE;
			}
			ServR::pFileInitDestInfo->argc = 0;
			//Copy the name of the command.
			ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], AES_CDH_CDHDEFINE);
		}
		//Parse the arguments
		int ctr = 0;
		while(attr[ctr])
		{
			if (ACE_OS::strcmp(attr[ctr]->attrName, AES_CDH_ADVINITPARAMS_RDN) == 0 )
			{
				destinationRdn = (reinterpret_cast<char *>(attr[ctr]->attrValues[0]));
				AES_CDH_TRACE_MESSAGE( "Advanced initiating parameters Rdn is %s", destinationRdn.c_str());
			}
			ctr++;
		}

		while( attr[i] )
		{

			if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_FILE_FILEOVERWRITE) == 0 )
			{

				if (attr[i]->attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "File Overwrite Option is %d", mytmp);
					if((mytmp < 0 || mytmp > 1 ))
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "File Overwrite is invalid");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_FILEOVERWRITE , objNm.c_str());
						ccbId_mtx.acquire();
						if(ServR::pFileInitDestInfo != 0 )
						{
							for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
							{
								delete[] ServR::pFileInitDestInfo->argv[ctr];
								ServR::pFileInitDestInfo->argv[ctr] = 0;
							}
							delete ServR::pFileInitDestInfo;
							ServR::pFileInitDestInfo = 0;
						}
						ccbId_mtx.release();
						return ACS_CC_FAILURE;
					}
					ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
					ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-o");
					switch( mytmp )
					{
					case 0:
						ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
						ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++],AES_CDH_INIT_NO);
						break;

					case 1:
						ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
						ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++],AES_CDH_INIT_YES);
						break;
					}
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_FILE_RETRYDELAY) == 0 )
			{

				if (attr[i]->attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					if(mytmp < 1 || mytmp > 60 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "File retry delay is out of range");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_FILERETRYDELAY , objNm.c_str());
						ccbId_mtx.acquire();
						if(ServR::pFileInitDestInfo != 0 )
						{
							for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
							{
								delete[] ServR::pFileInitDestInfo->argv[ctr];
								ServR::pFileInitDestInfo->argv[ctr] = 0;
							}
							delete ServR::pFileInitDestInfo;
							ServR::pFileInitDestInfo = 0;
						}
						ccbId_mtx.release();
						return ACS_CC_FAILURE;
					}
					ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
					ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-d");
					char myFileRetryDelay[11] = { 0 };
					ACE_OS::sprintf( myFileRetryDelay, "%d", mytmp);
					ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ACE_OS::strlen(myFileRetryDelay)+1];
					ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], myFileRetryDelay);
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_FILE_SENDRETRY) == 0 )
			{
				if (attr[i]->attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					if(mytmp < 0 || mytmp > 100 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, " send retry is out of range");
						AES_CDH_TRACE_MESSAGE("send retry is out of range");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_FILESENDRETRY , objNm.c_str());
						ccbId_mtx.acquire();
						if(ServR::pFileInitDestInfo != 0 )
						{
							for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
							{
								delete[] ServR::pFileInitDestInfo->argv[ctr];
								ServR::pFileInitDestInfo->argv[ctr] = 0;
							}
							delete ServR::pFileInitDestInfo;
							ServR::pFileInitDestInfo = 0;
						}
						ccbId_mtx.release();
						return ACS_CC_FAILURE;
					}
					ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
					ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], "-s");
					char myRetries[10];
					ACE_OS::sprintf( myRetries, "%d", mytmp);
					ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ACE_OS::strlen(myRetries)+1];
					ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++], myRetries);
				}
			}
			else if ( ACE_OS::strcmp(attr[i]->attrName, AES_CDH_SUPPRESS_SUB_FOLDER_CREATION) == 0 )
			{
				if (attr[i]->attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attr[i]->attrValues[0]);
					AES_CDH_TRACE_MESSAGE("Suppress sub folder creation = %d", mytmp);
					if((mytmp < 0 || mytmp > 1 ))
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Suppress sub folder creation is out of range");
						//set error code for invalid value
						string objNm = destinationRdn + "," + parentName;
						setErrorCode(AES_CDH_RC_UNREAS_SUPPRESSSUBFOLDRCREATION , objNm.c_str());
						ccbId_mtx.acquire();
						if(ServR::pFileInitDestInfo != 0 )
						{
							for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
							{
								delete[] ServR::pFileInitDestInfo->argv[ctr];
								ServR::pFileInitDestInfo->argv[ctr] = 0;
							}
							delete ServR::pFileInitDestInfo;
							ServR::pFileInitDestInfo = 0;
						}
						ccbId_mtx.release();
						return ACS_CC_FAILURE;
					}
					ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[3];
					ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[(ServR::pFileInitDestInfo->argc)++], "-g");
					switch( mytmp )
					{
					case 0:
						ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
						ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++],AES_CDH_INIT_NO);
						break;

					case 1:
						ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
						ACE_OS::strcpy(ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc++],AES_CDH_INIT_YES);
						break;
					}
				}
			}
			i++;
		}
		ServR::pFileInitDestInfo->argv[ServR::pFileInitDestInfo->argc] = 0;
		string destinationName("");
		extractDestName(destinationRdn, "RDN", destinationName);

		if ( isDestNameValid(destinationName) == false)
		{
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving  since the Advanced destination parameters dn name is invalid");
			//set error code for invalid value
			ccbId_mtx.acquire();
			if(ServR::pFileInitDestInfo != 0 )
			{
				for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileInitDestInfo->argv[ctr];
					ServR::pFileInitDestInfo->argv[ctr] = 0;
				}
				delete ServR::pFileInitDestInfo;
				ServR::pFileInitDestInfo = 0;
			}
			ccbId_mtx.release();

			//set error code for invalid value
			string objNm = destinationRdn + "," + parentName;
			setErrorCode(AES_CDH_RC_UNREAS, objNm.c_str());
			return ACS_CC_FAILURE;
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving ");
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;

}//End of create

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileInitDestCmdHandler::deleted( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"ObjectDeleteCallback invoked for object : %s",objName);
	ACE_Time_Value tv(0,10);
	ACE_OS::sleep(tv);
	ServR::initdestsetMutex.acquire();

	if( ServR::isInitDestSetDeleteTrig == true )
	{
		if( ServR::isDelOfInitDestSetSuccess == true )
		{
			ServR::initdestsetMutex.release();
			//set error code for success
			setErrorCode(AES_CDH_RC_OK);
			return ACS_CC_SUCCESS ;
		}
		else
		{
			ServR::initdestsetMutex.release();
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
	string parentName;

	// Extract the class name from the object RDN
	extractClassNameFromRdn(objName, myClassName);

	size_t pos1 = string(objName).find_first_of(",");
	string destSetRDN("");
	if( pos1 != string::npos )
	{
		destSetRDN = string(objName).substr(pos1+1);
		parentName=destSetRDN;
		extractDestName(destSetRDN, "OBJNAME",  destSetName);
	}

	if(  myClassName.compare(AES_CDH_ADVINITPARAMS_IMM_RDN_CL_NM) == 0 )
	{
		//Check whether destset is removed or only Advanced is getting removed
		if (ServR::getDestSet(destSetName, pdestset) == AES_CDH_RC_NODESTSET)//destset removed
		{
			ServR::initdestsetMutex.release();
			//set error code for success
			setErrorCode(AES_CDH_RC_OK);
			return ACS_CC_SUCCESS;
		}
		else
		{
			ServR::initdestsetMutex.release();
			//set error code for internal error
			setErrorCode(AES_CDH_RC_INTPROGERR);
			return ACS_CC_FAILURE;
		}

	}
	else if( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_PRIMINITDEST_RDN_CL_NM) == 0
			|| ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_SECINITDEST_RDN_CL_NM) == 0 )
	{
		extractDestName(objName, "OBJNAME", myDestinationName);


		string detailInfo(""), userName("");

		AES_CDH_ResultCode resultCode = AES_CDH_RC_OK;
		//remove secondary from destset
		if ( ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_SECINITDEST_RDN_CL_NM) == 0 )
		{
			//Check whether destset is removed or only secondary is getting removed
			if (ServR::getDestSet(destSetName, pdestset) != AES_CDH_RC_NODESTSET)
			{
				//remove secondary from destset
				resultCode = ServR::deleteDestSet(destSetName, detailInfo, myDestinationName, userName);
				if ( resultCode != AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing dest %s from destset %s", myDestinationName.c_str(), destSetName.c_str());
					AES_CDH_LOG(LOG_LEVEL_TRACE,  "Leaving ... with error code %d", resultCode);
					ServR::initdestsetMutex.release();
					//set error code for deleteDestSet failure
					setErrorCode(resultCode,parentName.c_str());// an error has occured during dis-associtaion
					return ACS_CC_FAILURE;
				}
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
				AES_CDH_LOG(LOG_LEVEL_INFO, "Error occured while deleting destination");
				AES_CDH_LOG(LOG_LEVEL_TRACE,  "Leaving  with error code %d", resultCode);
				//add secondary dest to destset
				string primaryDest(""), backupDest("");
				ServR::changeDestSet(destSetName, primaryDest, myDestinationName, backupDest, myUserName);
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

				ServR::initdestsetMutex.release();
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
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while deleting primary destination");
				AES_CDH_LOG(LOG_LEVEL_TRACE,  "Leaving  with error code %d", resultCode);
				ServR::initdestsetMutex.release();
				//deleting the primary dest, with out deleting destset (error code to be defined)
				setErrorCode(AES_CDH_RC_PROTECTEDDEST);
				return ACS_CC_FAILURE;
			}

			resultCode = ServR::deleteDest(myDestinationName, myDetailInfo,  myUserName );

			if ( resultCode != AES_CDH_RC_OK )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while deleting destination");
				AES_CDH_LOG(LOG_LEVEL_TRACE,  "Leaving  with error code %d", resultCode);
				ServR::initdestsetMutex.release();
				//set error code for deleteDestSet failure
				setErrorCode(resultCode);
				return ACS_CC_FAILURE;
			}
		}
	}
	ServR::initdestsetMutex.release();
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}//End of deleted

/*===================================================================
	ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileInitDestCmdHandler::modify(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName,
		ACS_APGCC_AttrModification **attrMods)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbModifyCallback invoked");
	(void) oiHandle;
	(void) ccbId;

	TransDest* pdest=0;
	string destName="";
	AES_CDH_Destination::destAttributes tempattr;
	extractDestName(objName, "OBJNAME", destName);
	string transferProtocol("");
	if(ServR::getDest(destName,pdest) ==AES_CDH_RC_OK)
	{
		if(pdest->getAttr(tempattr)== AES_CDH_RC_OK)
		{
			transferProtocol=tempattr.transferType;
			AES_CDH_TRACE_MESSAGE("Transfer protocol = %s ",transferProtocol.c_str());
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Not able to fetch the existing attribute values ");
		}
	}

	string myClassName("");
	string myDestinationName("");

	// Extract the class name from the object RDN
	extractClassNameFromRdn(objName, myClassName);

	AES_CDH_TRACE_MESSAGE( "Class name is %s", myClassName.c_str());

	if( myClassName.compare(AES_CDH_ADVINITPARAMS_IMM_RDN_CL_NM) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Modifying advanced initiating parameters");

		int argc = 0, i = 0;
		char *argv[32] = { 0 };

		//Copy the name of the command.
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

		//Parse the arguments
		while( attrMods[i] )
		{
			if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_FILE_FILEOVERWRITE) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE( "File OverWrite Option = %d", mytmp);
				if((mytmp < 0 || mytmp > 1 ))
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "File Overwrite is invalid");
					AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving ");
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
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
					break;
				}
			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_FILE_RETRYDELAY) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE( "File Retry Delay = %d", mytmp);
				if( mytmp < 1 || mytmp > 60 )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "File retry delay is out of range");
					AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving");
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_UNREAS_FILERETRYDELAY , objName);
					return ACS_CC_FAILURE;
				}
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-d");
				char myFileRetryDelay[11] = { 0 };
				ACE_OS::sprintf( myFileRetryDelay, "%d", mytmp);
				argv[argc] = new char[ACE_OS::strlen(myFileRetryDelay)+1];
				ACE_OS::strcpy(argv[argc++], myFileRetryDelay);

			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_FILE_SENDRETRY) == 0 )
			{
				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE( "File Retries = %d", mytmp);
				if(mytmp < 0 || mytmp > 100 )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Retry count is out of range");
					AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving");
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_UNREAS_FILESENDRETRY , objName);
					return ACS_CC_FAILURE;
				}
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-s");
				char myRetries[10];
				ACE_OS::sprintf( myRetries, "%d", mytmp);
				argv[argc] = new char[ACE_OS::strlen(myRetries)+1];
				ACE_OS::strcpy(argv[argc++], myRetries);
			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_SUPPRESS_SUB_FOLDER_CREATION) == 0 )
			{

				ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE( "Suppress sub folder creation  Option = %d", mytmp);
				if((mytmp < 0 || mytmp > 1 ))
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Suppress sub folder creation value is invalid");
					AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving");
					for ( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
					//set error code for invalid value
					setErrorCode(AES_CDH_RC_UNREAS_SUPPRESSSUBFOLDRCREATION , objName);
					return ACS_CC_FAILURE;
				}
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-g");
				switch( mytmp )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
					break;
				}

			}
			i++;
		}

		OmHandler omHandler;
		if ( omHandler.Init() == ACS_CC_FAILURE)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to initialize the omhandler");
			AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving");
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
		omHandler.getClassInstances(AES_CDH_PRIMARYINITDEST_IMM_CLASS_NM, primDestList);

		//Get the instances of the SecondaryInitiatingDestination.
		omHandler.getClassInstances( AES_CDH_SECONDARYINITDEST_IMM_CLASS_NM,secDestList);

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

				AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving  with error code %d", resultCode);
				//set error code for changeAttr failure
				setErrorCode(resultCode, objName);
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

	else if( myClassName.compare(AES_CDH_PRIMINITDEST_RDN_CL_NM) == 0 || myClassName.compare(AES_CDH_SECINITDEST_RDN_CL_NM) == 0 )
	{
		AES_CDH_TRACE_MESSAGE("Modifying primary initiating destination");

		int argc = 0, i = 0;
		char *argv[32] = { 0 };
		PassDestInfo *pPassDestInfo = 0;

		//Copy the name of the command.
		argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
		ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

		//Parse the arguments
		while( attrMods[i] )
		{
			if (ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_REMOTE_IP_ADDR) == 0)
			{
				char *mytmp = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
				AES_CDH_TRACE_MESSAGE( "File Remote Destination  = %s", mytmp);
				if( mytmp != 0 )
				{
					int validationResult = AES_CDH_Common_Util::isIPAddressValid(mytmp);	//IPv6_feature:: validate IPv4 and IPv6 formats
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
						AES_CDH_LOG(LOG_LEVEL_INFO, "Destination Address is not a valid IP Address");
						for ( int ctr = 0; ctr < argc ; ctr++)
						{
							delete[] argv[ctr];
							argv[ctr] = 0;
						}
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving");
						//set error code for invalid value
						if(validationResult == AES_CDH_Common_Util::INVALID_IPV4_MAPPED_IPV6_ADDRESS)
							setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6 , objName);
						else if(validationResult == AES_CDH_Common_Util::INVALID_IPV6_NATIVE)
							setErrorCode(AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE , objName);
						else
							setErrorCode(AES_CDH_RC_UNREAS_INITIP , objName);
						return ACS_CC_FAILURE;
					}

				}
			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_DEST_PORTNUMBER) == 0 )
			{
				if ( attrMods[i]->modAttr.attrValuesNum != 0 )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>(attrMods[i]->modAttr.attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "File Remote Destination Port = %d", mytmp);
					if(mytmp < 0 || mytmp > 65535 )
					{
						AES_CDH_LOG(LOG_LEVEL_INFO, "Port No is out of range");
						for ( int ctr = 0; ctr < argc ; ctr++)
						{
							delete[] argv[ctr];
							argv[ctr] = 0;
						}
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving");
						//set error code for invalid value
						setErrorCode(AES_CDH_RC_UNREAS_INITPORTNO , objName);
						return ACS_CC_FAILURE;
					}
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-n");
					char myPortNo[11] = { 0 };
					ACE_OS::sprintf( myPortNo, "%d", mytmp);
					argv[argc] = new char[ACE_OS::strlen(myPortNo)+1];
					ACE_OS::strcpy(argv[argc++], myPortNo);
				}
				else
				{
					argv[argc] = new char[3];
					ACE_OS::strcpy(argv[argc++], "-n");
					char myPortNo[11] = { 0 };
					argv[argc] = new char[ACE_OS::strlen(myPortNo)+1];
					ACE_OS::strcpy(argv[argc++], myPortNo);
				}

			}
                        else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_FILE_REMOTE_FOLDER_PATH) == 0 )
                        {
                                if ( attrMods[i]->modAttr.attrValuesNum != 0 )
                                {
                                        char *mytmp = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
                                        string myRemoteFolderPath = mytmp;
                                        if (myRemoteFolderPath.find('#') != std::string::npos)    //TR_HX21780_start
                                        {
                                                AES_CDH_LOG(LOG_LEVEL_ERROR, "File Remote folder Path is invalid");
                                                //set error code for invalid character
                                                setErrorCode(AES_CDH_RC_ILLEGALREMOTEFOLDERPATH);
                                                return ACS_CC_FAILURE;
                                        }
                                        else
                                        {
                                                AES_CDH_TRACE_MESSAGE("File Remote folder Path = %s", myRemoteFolderPath.c_str());
                                                //set error code for success scenario
                                                setErrorCode(AES_CDH_RC_OK);    //TR_HX21780_end
                                        }
                                        if(myRemoteFolderPath != "null" )
                                        {
                                                argv[argc] = new char[3];
                                                ACE_OS::strcpy(argv[argc++], "-r");
                                                argv[argc] = new char[ACE_OS::strlen(mytmp)+1];
                                                ACE_OS::strcpy(argv[argc++], mytmp);
                                        }
                                }
                                else
                                {
                                        string defaultValue("");
                                        argv[argc] = new char[3];
                                        ACE_OS::strcpy(argv[argc++], "-r");
                                        argv[argc] = new char[defaultValue.size()+1];
                                        ACE_OS::strcpy(argv[argc++], defaultValue.c_str());
                                }
                        }
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_PASSWORD) == 0 )
			{
				if ( attrMods[i]->modAttr.attrValuesNum != 0 )
				{
					char *mytmp = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
					string mypassword = mytmp;
					pPassDestInfo = new PassDestInfo();
					pPassDestInfo->passwdDn = mypassword;
					AES_CDH_TRACE_MESSAGE( "File Remote Destination Password iDN = %s", mypassword.c_str());

					if(mypassword.empty() && transferProtocol == AES_CDH_SFTPV2_TRANSFER_TYPE)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Password cannot be empty for SFTPV2 protocol");
						AES_CDH_TRACE_MESSAGE("Password cannot be empty for SFTPV2 protocol");
						delete pPassDestInfo;
						pPassDestInfo=0;
						setErrorCode(AES_CDH_RC_SFTP_PASSWD,objName);
						return ACS_CC_FAILURE;
					}
					if( mypassword != "null" )
					{
						argv[argc] = new char[3];
						ACE_OS::strcpy(argv[argc++], "-p");
						argv[argc] = new char[ACE_OS::strlen(mytmp)+1];
						ACE_OS::strcpy(argv[argc++], mytmp);
					}

				}
				else
				{
					if(transferProtocol == AES_CDH_SFTPV2_TRANSFER_TYPE)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Password cannot be empty for SFTPV2 protocol");
						AES_CDH_TRACE_MESSAGE("Password cannot be empty for SFTPV2 protocol");
						setErrorCode(AES_CDH_RC_SFTP_PASSWD,objName);
						return ACS_CC_FAILURE;
					}
				}
			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_FILE_USERNAME) == 0 )
			{
				if (attrMods[i]->modAttr.attrValuesNum!=0)
				{
					char *mytmp = reinterpret_cast<char *>(attrMods[i]->modAttr.attrValues[0]);
					AES_CDH_TRACE_MESSAGE( "File Remote Destination User Name = %s", mytmp);
					if( mytmp != 0 )
					{
						if(strcmp(mytmp,"anonymous") == 0 && transferProtocol == AES_CDH_SFTPV2_TRANSFER_TYPE)
						{
							AES_CDH_LOG(LOG_LEVEL_ERROR, "User name cannot be empty");
							AES_CDH_TRACE_MESSAGE("User name cannot be empty");
							setErrorCode(AES_CDH_RC_SFTP_UNAME,objName);
							return ACS_CC_FAILURE;
						}
						argv[argc] = new char[3];
						ACE_OS::strcpy(argv[argc++], "-u");
						argv[argc] = new char[ACE_OS::strlen(mytmp)+1];
						ACE_OS::strcpy(argv[argc++], mytmp);
					}
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "User name cannot be empty");
					AES_CDH_TRACE_MESSAGE("User name cannot be empty");
					setErrorCode(AES_CDH_RC_SFTP_UNAME,objName);
					return ACS_CC_FAILURE;
				}
			}
			else if ( ACE_OS::strcmp(attrMods[i]->modAttr.attrName, AES_CDH_TRANSFER_PROTOCOL) == 0 )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Incorrect Usage. Modification of Transfer Type is not allowed.");
				AES_CDH_TRACE_MESSAGE("Incorrect Usage. Modification of Transfer Type is not allowed.");
				for ( int ctr = 0; ctr < argc ; ctr++)
				{
					delete[] argv[ctr];
					argv[ctr] = 0;
				}
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving");
				//set error code for incorrect usage
				setErrorCode(AES_CDH_RC_MODIFY_TRANSFERTYPE,objName);
				return ACS_CC_FAILURE;

			}
			i++;
		}
		//Calling the Business logic
		AES_CDH_ResultCode resultCode  = AES_CDH_RC_OK;
		string strUserGroup("");
		argv[argc] = 0;

		//Extract the destination name from the RDN.
		extractDestName( objName, "OBJNAME", myDestinationName);
		if( pPassDestInfo != 0 )
		{
			pPassDestInfo->destinationName = myDestinationName;	
			m_passDestInfoList.push_back(pPassDestInfo);
		}

		AES_CDH_TRACE_MESSAGE("Destination Name = %s", myDestinationName.c_str());

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
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination");
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
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying destination");
			AES_CDH_TRACE_MESSAGE("Error occured while modifying destination");

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

			AES_CDH_LOG(LOG_LEVEL_TRACE,  "Leaving with error code %d", resultCode);
			//set error code for changeAttr failure
			setErrorCode(resultCode, objName);
			return ACS_CC_FAILURE;
		}
		for ( int ctr = 0; ctr < argc ; ctr++)
		{
			delete[] argv[ctr];
			argv[ctr] = 0;
		}
		AES_CDH_LOG(LOG_LEVEL_TRACE,  "Leaving with error code %d", resultCode);

	}
	//set error code for success
	setErrorCode(AES_CDH_RC_OK);
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileInitDestCmdHandler::complete( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbCompleteCallback invoked");
	(void)oiHandle;
	(void)ccbId;
	m_iCommit.commitFlag=false;
	m_iCommit.destCommit.clear();
	if(theClassName.compare(AES_CDH_PRIMARYINITDEST_IMM_CLASS_NM)== 0 ) 
	{

		if( m_primDestCreateccbId == ccbId)
		{
			ccbId_mtx.acquire();
			if(ServR::pFileInitDestInfo != 0)
			{
				string myDetailInfo("");
				string parentName = string(AES_CDH_INIT_DEST_SET_ID) + "=" + ServR::pFileInitDestInfo->destinationSetName + "," + AES_GCC_Util::dnOfFileTransferM;
				string objNm = string(AES_CDH_PRIMARYINITDEST_RDN) + "=" + ServR::pFileInitDestInfo->destinationName + "," + parentName;
				ServR::pFileInitDestInfo->isPrimDest=true;
				// Create the destination
				int result = ServR::define( ServR::pFileInitDestInfo->destinationName, ServR::pFileInitDestInfo->transferType , ServR::pFileInitDestInfo->argc, ServR::pFileInitDestInfo->argv, "", AES_CDH_PRIMARYINITDEST_RDN, ServR::pFileInitDestInfo->destinationSetName);
				if (result!= AES_CDH_RC_OK )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining destination");
					AES_CDH_TRACE_MESSAGE("Error occured while defining destination");
					for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
					{
						delete[] ServR::pFileInitDestInfo->argv[ctr];
						ServR::pFileInitDestInfo->argv[ctr] = 0;
					}
					ServR::pFileInitDestInfo->argc = 0;
					delete ServR::pFileInitDestInfo;
					ServR::pFileInitDestInfo=0;
					ccbId_mtx.release();
					//set error code for which is the result of define
					setErrorCode(result,objNm.c_str());
					return ACS_CC_FAILURE;
				}
				//define destset with this dest as primary
				string secondaryDestName(""), backupDestName(""), userGroup(""), usrName("");
				int result1 = ServR::defineDestSet(ServR::pFileInitDestInfo->destinationSetName, ServR::pFileInitDestInfo->destinationName,
						secondaryDestName,
						backupDestName,userGroup, AES_CDH_INIT_DEST_SET_ID);
				if (result1 != AES_CDH_RC_OK)
				{

					AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining destination set");
					AES_CDH_TRACE_MESSAGE("Error occured while defining destination set");
					//If destination set creation fails, then remove the primary destination too.
					ServR::deleteDest(ServR::pFileInitDestInfo->destinationName, myDetailInfo,  usrName );
					for(int i = 0; i < ServR::pFileInitDestInfo->argc ; i++)
					{
						delete[] ServR::pFileInitDestInfo->argv[i];
						ServR::pFileInitDestInfo->argv[i] = 0;
					}
					ServR::pFileInitDestInfo->argc = 0;
					delete ServR::pFileInitDestInfo;
					ServR::pFileInitDestInfo = 0;
					ccbId_mtx.release();

					//set error code for which is the result of defineDestSet
					setErrorCode(result1,parentName.c_str());
					return ACS_CC_FAILURE;
				}
				//HY68026
				/*for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
				{
					delete[] ServR::pFileInitDestInfo->argv[ctr];
					ServR::pFileInitDestInfo->argv[ctr] = 0;
				}
				delete ServR::pFileInitDestInfo;
				ServR::pFileInitDestInfo = 0;*/
			}
			ccbId_mtx.release();
			//m_primDestCreateccbId = 0; //HY68026
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;

} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void AES_CDH_FileInitDestCmdHandler::abort(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbAbortCallback invoked");
	(void)oiHandle;
	(void)ccbId;
	m_primDestCreateccbId = 0;
	m_secDestCreateccbId = 0;
	m_iCommit.commitFlag=false;
	m_iCommit.destCommit.clear();
	std::list <PassDestInfo *>::iterator itr = m_passDestInfoList.begin();

	//Delete the m_passDestInfoList
	while( itr != m_passDestInfoList.end())
	{
		PassDestInfo *ptr = (*itr);
		m_passDestInfoList.erase(itr++);
		delete ptr;
		ptr=0;
	}
	ccbId_mtx.acquire();
	if( ServR::pFileInitDestInfo != 0 )
	{
		if( ServR::pFileInitDestInfo->ccbId == ccbId )
		{
			//abort has been recived for destination creation.

			if(ServR::pFileInitDestInfo->isPrimDest  == true )
			{
				//Delete the destination set.
				string myDetailInfo(""), myDestinationName(""), myUserName("");

				ServR::deleteDestSet(ServR::pFileInitDestInfo->destinationSetName, myDetailInfo, myDestinationName, myUserName);

				//Delete the destination.
				ServR::deleteDest(ServR::pFileInitDestInfo->destinationName, myDetailInfo,  myUserName );
			}
			else if( ServR::pFileInitDestInfo->isSecDest == true )
			{
				//Delete the destination set association for secondary destination.
				string myDetailInfo(""), myDestinationName(""), myUserName("");

				ServR::deleteDestSet(ServR::pFileInitDestInfo->destinationSetName, myDetailInfo, ServR::pFileInitDestInfo->destinationName, myUserName);

				//Delete the secondary destination.
				ServR::deleteDest(ServR::pFileInitDestInfo->destinationName, myDetailInfo,  myUserName );
			}
		}

                for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)  //HY68026 
                {
                        delete[] ServR::pFileInitDestInfo->argv[ctr];
                        ServR::pFileInitDestInfo->argv[ctr] = 0;
                }
		delete ServR::pFileInitDestInfo;
		ServR::pFileInitDestInfo = 0;
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
				ServR::define( m_deleteDestList[ctr].destinationName, m_deleteDestList[ctr].transferType, m_deleteDestList[ctr].argc, m_deleteDestList[ctr].argv, strUserGroup, AES_CDH_SECONDARYINITDEST_RDN,m_deleteDestList[ctr].destinationSetName);
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

	AES_CDH_TRACE_MESSAGE( "Leaving");
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileInitDestCmdHandler::updateRuntime( const char* objName,
		const char** attrName)
{
	(void)objName;
	(void)attrName;
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void AES_CDH_FileInitDestCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
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

	std::vector<ACS_APGCC_AdminOperationParamType> vectorOut;
	ACS_APGCC_AdminOperationParamType firstElem;

	ACS_APGCC_AdminOperationParamType secondElem;

	if(ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_PRIMINITDEST_RDN_CL_NM) == 0
			|| ACE_OS::strcasecmp(myClassName.c_str(), AES_CDH_SECINITDEST_RDN_CL_NM) == 0 )
	{
		bool  myOIValidationError=true;
		int result;
		switch(operationId)
		{
		case 5://connectionDestinationVerify
		{
			AES_CDH_TRACE_MESSAGE("In Admin call back ,  connectionDestinationVerify, p_objName :%s ",p_objName);
			tempDest=string(p_objName);
			extractDestName(tempDest,"OBJNAME",destName);
			AES_CDH_TRACE_MESSAGE("Trying to check connection for destination : %s",destName.c_str());
			vector<ServR::checkConnAttributes> cAttrs;
			bool force   = true; //hard code to true
			result = ServR::checkConnection(destName, force, cAttrs);
			AES_CDH_TRACE_MESSAGE( "ServR::checkConnection ,result : %d",result);
			//set error code for result of checkConnection
			setErrorCode(result);
			if(result == AES_CDH_RC_CONNECTOK)
			{
				myOIValidationError=true;

			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "In Admin call back , connection verification failed ");
				AES_CDH_TRACE_MESSAGE("In Admin call back , connection verification failed ");
				myOIValidationError=false;
				/*create first Elemenet of param list*/
				char attrName1[] = "errorCode";
				//  errorCode = AES_CDH_RC_CONNECTERR;
				firstElem.attrName = attrName1;
				firstElem.attrType = ATTR_INT32T;
				int intValue1 = AES_CDH_RC_CONNECTERR ;				

				firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

				/*create second Elemenet of param list*/

				AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(result,""); // TR HS55206

				string ertxt="@ComNbi@";
				ertxt += resobj->errorText();
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "value of ertxt passed into COM-CLI =%s",ertxt.c_str());
				AES_CDH_TRACE_MESSAGE("value of ertxt passed into COM-CLI =%s",ertxt.c_str());

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
		break;

		default:
			AES_CDH_TRACE_MESSAGE("In Admin call back , default case");
			break;
		}
		ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, myOIValidationError, vectorOut);
		AES_CDH_TRACE_MESSAGE("result admin op :%d",rc);
		AES_CDH_TRACE_MESSAGE( "myOIValidationError: %d",myOIValidationError);
		if(rc != ACS_CC_SUCCESS)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Admin op failed , error : %d ",myOIValidationError);
			AES_CDH_TRACE_MESSAGE("Admin op failed , error : %d ",myOIValidationError);
		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "%s","Not yet Implemented");
		AES_CDH_TRACE_MESSAGE("Not yet Implemented");
		ACS_CC_ReturnType rc = adminOperationResult(oiHandle, invocation, true, vectorOut);
		AES_CDH_LOG(LOG_LEVEL_INFO, "result admin op :%d",rc);
	}

	AES_CDH_TRACE_MESSAGE( "Leaving");
}//End of adminOperationCallback

/*===================================================================
   ROUTINE: apply
=================================================================== */
void AES_CDH_FileInitDestCmdHandler::apply(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	AES_CDH_TRACE_MESSAGE( "Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"CcbApplyCallback invoked");
	(void)oiHandle;
	(void)ccbId;

	if((m_primDestCreateccbId == ccbId) || (m_secDestCreateccbId == ccbId))  //HY68026
        {
                ccbId_mtx.acquire();
                if(ServR::pFileInitDestInfo != 0)
                {
                        for ( int ctr = 0; ctr < ServR::pFileInitDestInfo->argc ; ctr++)
                        {
                                delete[] ServR::pFileInitDestInfo->argv[ctr];
                                ServR::pFileInitDestInfo->argv[ctr] = 0;
                        }
                        delete ServR::pFileInitDestInfo;
                        ServR::pFileInitDestInfo = 0;
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

	//do nothing

	/* ECIM pwd changes -- Start*/
	//TO DO : use libcrypto for encryption/decryption of ECIM password
	if( m_passDestInfoList.size() > 0 )
	{
		OmHandler pwdOmHandler;
		if( pwdOmHandler.Init() == ACS_CC_SUCCESS )
		{
			for ( std::list<PassDestInfo*>::iterator itr=m_passDestInfoList.begin(); itr != m_passDestInfoList.end(); ++itr)
			{
				ACS_CC_ImmParameter attrPasswd;
				SecCryptoStatus decryptStatus;
				attrPasswd.attrName = (char *)AES_ECIM_PWD_ATTRIBUTE;

				//retrieving the password from ECIM password struct
				if ( pwdOmHandler.getAttribute( ((*itr)->passwdDn).c_str(), &attrPasswd) == ACS_CC_SUCCESS )
				{
					string password("");
					if(attrPasswd.attrValuesNum  != 0 )
					{
						password = reinterpret_cast<char*>(attrPasswd.attrValues[0]);
						if(!password.empty())
						{

							AES_CDH_TRACE_MESSAGE( "Decrypting the password - %s", password.c_str());
							char * plaintext = NULL; 
							string ciphertext = password;
							decryptStatus = sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str());
							if(decryptStatus != SEC_CRYPTO_OK )
							{
								AES_CDH_LOG(LOG_LEVEL_ERROR, "Password decryption failed");
								AES_CDH_TRACE_MESSAGE("Password decryption failed");
							}
							else
							{
								password = plaintext;
							}
							if(plaintext !=0 )
								free(plaintext);
						}
					}

					int argc= 0;
					char *argv[32] = { 0 };

					argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHCHANGE) + 1] ;
					ACE_OS::strcpy( argv[argc++], AES_CDH_CDHCHANGE);

					argv[argc] = new char[3];
					strcpy( argv[argc++] ,  "-p" );
					argv[argc] = new char[password.length()+1];
					strcpy(argv[argc++], password.c_str());

					argv[argc] = 0;

					//Call the business logic here
					AES_CDH_ResultCode resultCode  = AES_CDH_RC_OK;

					//Now modify the destination.
					resultCode = ServR::changeAttr( (*itr)->destinationName, argc, argv, "");

					if( resultCode != AES_CDH_RC_OK )
					{
						AES_CDH_LOG(LOG_LEVEL_DEBUG, "Error occured while modifying destination");
						AES_CDH_TRACE_MESSAGE("Error occured while modifying destination");
					}
					// Delete the temporary variables
					for( int ctr = 0; ctr < argc ; ctr++)
					{
						delete[] argv[ctr];
						argv[ctr] = 0;
					}
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "Error in getting password attribute for DN = %s", ((*itr)->passwdDn).c_str());
					AES_CDH_TRACE_MESSAGE("Error in getting password attribute for DN = %s", ((*itr)->passwdDn).c_str());
				}
			}
		}
		pwdOmHandler.Finalize();
		std::list <PassDestInfo *>::iterator itr = m_passDestInfoList.begin();
		while( itr != m_passDestInfoList.end())
		{
			PassDestInfo *ptr = (*itr);
			m_passDestInfoList.erase(itr++);
			delete ptr;
			ptr=0;
		}
		m_passDestInfoList.clear();

	}
	/* ECIM pwd changes -- End*/
	AES_CDH_TRACE_MESSAGE( "Leaving");
}//End of apply

/*===================================================================
   ROUTINE: DestOIThread
=================================================================== */
ACE_THR_FUNC_RETURN AES_CDH_FileInitDestCmdHandler::DestOIThread(void* aCDHDestPtr)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_FileInitDestCmdHandler * myDestImplementer = 0;
	myDestImplementer = reinterpret_cast<AES_CDH_FileInitDestCmdHandler*>(aCDHDestPtr);

	if(myDestImplementer != 0)
	{
		myDestImplementer->m_poReactor->open(1);
		myDestImplementer->dispatch(myDestImplementer->m_poReactor,	ACS_APGCC_DISPATCH_ALL) ;
		myDestImplementer->m_poReactor->run_reactor_event_loop();
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "myDestImplementer is NULL");
		AES_CDH_TRACE_MESSAGE("myDestImplementer is NULL");
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return 0;
}//End of DestOIThread

/*===================================================================
   ROUTINE: setupDestOIThread
=================================================================== */
ACS_CC_ReturnType AES_CDH_FileInitDestCmdHandler::setupDestOIThread(AES_CDH_FileInitDestCmdHandler * aDestImplementer,
		ACE_Thread_Manager* threadManager_)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	int myGroupId = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	if( aDestImplementer->setObjectImplementer() != ACS_CC_SUCCESS)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "setObjectImplementer failed");
		AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving");
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
void AES_CDH_FileInitDestCmdHandler::shutdown()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	m_poReactor->end_reactor_event_loop();

	if( m_isClassImplAdded == true )
	{
		for (int i=0; i < 10; ++i)
		{
			if( m_poOiHandler->removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing class implementer");
				AES_CDH_TRACE_MESSAGE("Error occured while removing class implementer");
				int intErr = getInternalLastError();
				if ( intErr == -6)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if ( i >= 10 )
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR,  "Error occured while removing class implementer, ErrCode = %d", intErr);
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

	if(! m_isClassImplAdded)
		AES_CDH_LOG(LOG_LEVEL_INFO,"Removed class implementer for class %s successfully !!",theClassName.c_str());
	AES_CDH_TRACE_MESSAGE("Leaving");
}//End of shutdown

/*===================================================================
	ROUTINE: ~AES_CDH_FileInitDestCmdHandler
=================================================================== */
AES_CDH_FileInitDestCmdHandler::~AES_CDH_FileInitDestCmdHandler()
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
void AES_CDH_FileInitDestCmdHandler::extractDestName(const string attrName,
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
void AES_CDH_FileInitDestCmdHandler::extractClassNameFromRdn( const string myObjRdn,
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
bool AES_CDH_FileInitDestCmdHandler::isIPAddrValid( const char * aIPAddr )
{
	AES_CDH_TRACE_MESSAGE("Entering");

	if ( aIPAddr == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving with false");
		return false;
	}
	unsigned int b1 = 0, b2 =0, b3 = 0, b4 = 0;
	unsigned char c = 0;
	if( sscanf(aIPAddr, "%3u.%3u.%3u.%3u%c", &b1, &b2, &b3, &b4, &c ) != 4 )
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Leaving with false");
		return false;
	}

	if(( b1 | b2 | b3 | b4 ) > 255 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving with false");
		return false;
	}


	if( strspn( aIPAddr, "0123456789.") < ACE_OS::strlen( aIPAddr ))
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "%s", "Leaving with false");
		return false;
	}

	AES_CDH_TRACE_MESSAGE("Leaving with true");
	return true;
}

/*===================================================================
        ROUTINE: isDestNameValid
=================================================================== */
bool AES_CDH_FileInitDestCmdHandler::isDestNameValid( const string& destName )
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if( destName.length() < 1  || destName.length() > 32 )
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving . Destination name is invalid since the length of the destination name exceeds limit");
		return false;
	}
	const string legalChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	const string legalChars2("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (destName.find_first_not_of(legalChars) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving . Destination name is invalid since the destination name contains invalid characters");
		return false;
	}
	if (destName.substr(0,1).find_first_not_of(legalChars2) != string::npos)
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving . Destination name is invalid since the destination name starts with number");
		return false;
	}
	AES_CDH_TRACE_MESSAGE("Leaving AES_CDH_FileInitDestCmdHandler::isDestNameValid. Destination name is valid");
	return true;
}
/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void AES_CDH_FileInitDestCmdHandler::setErrorCode(unsigned int code, const char *objName)
{
	AES_CDH_Result_R1 *resobj= new AES_CDH_Result_R1(code,"");
	string ertxt=resobj->errorText();
	AES_CDH_TRACE_MESSAGE("setExitCode : exitCode = %d , errorText = %s",code,ertxt.c_str());
	if(objName != 0 && code != AES_CDH_FILEDESTSETLIMIT)
	{
		string comDn("");
		AES_GCC_Util::extractComDnfromDn(objName,comDn);
		ertxt = ertxt + " in " + comDn;
	}
	setExitCode(code,ertxt );
	delete resobj;
}

/*===================================================================
        ROUTINE: getExistingDestObjFromIMM
=================================================================== */
bool AES_CDH_FileInitDestCmdHandler::getExistingDestObjFromIMM( const char *objName, int &tmpArgc, char** tmpArgv )
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

	//Read the attributes of the advanced initiating object
	for ( unsigned int cntr = 0; cntr  < destObj.attributes.size(); cntr++ )
	{
		if (destObj.attributes[cntr].attrName ==  AES_CDH_REMOTE_IP_ADDR)
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
		else if ( destObj.attributes[cntr].attrName == AES_CDH_PASSWORD)
		{
			string password("");
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy( tmpArgv[tmpArgc++] , "-p");

			if( destObj.attributes[cntr].attrValuesNum != 0)
			{
				password = reinterpret_cast<char *>(destObj.attributes[cntr].attrValues[0]);

				ACS_CC_ImmParameter attrPasswd;
				attrPasswd.attrName = (char *)AES_ECIM_PWD_ATTRIBUTE;

				//retrieving the password from ECIM password struct
				if ( myOmhandler.getAttribute( password.c_str(), &attrPasswd) == ACS_CC_SUCCESS )
				{
					password.clear();
					password = reinterpret_cast<char*>(attrPasswd.attrValues[0]);
				}
			}
			tmpArgv[tmpArgc] = new char [strlen(password.c_str())+1 ] ;
			ACE_OS::strcpy( tmpArgv[tmpArgc++], password.c_str());
		}
		else if ( destObj.attributes[cntr].attrName ==  AES_CDH_DEST_PORTNUMBER)
		{
			int portNumber = -1;

			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy( tmpArgv[tmpArgc++] , "-n");
			if(destObj.attributes[cntr].attrValuesNum!=0)
			{
				portNumber = *reinterpret_cast<ACE_INT32 *>(destObj.attributes[cntr].attrValues[0]);
			}	
			char myPortNo[11] = { 0 };
			ACE_OS::sprintf( myPortNo, "%d", portNumber);
			tmpArgv[tmpArgc] = new char[ACE_OS::strlen(myPortNo)+1];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], myPortNo);
		}
		else if ( destObj.attributes[cntr].attrName ==  AES_CDH_FILE_REMOTE_FOLDER_PATH)
		{ 
			string remoteFolderPath("");
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-r");

			if(destObj.attributes[cntr].attrValuesNum!=0)
			{
				remoteFolderPath = reinterpret_cast<char *>(destObj.attributes[cntr].attrValues[0]);
			}
			tmpArgv[tmpArgc] = new char[remoteFolderPath.size() + 1];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], remoteFolderPath.c_str());
		}
		else if ( destObj.attributes[cntr].attrName  == AES_CDH_FILE_USERNAME) 
		{
			string userName("");
			//Copy the user name option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-u");

			//Copy the user name
			if (destObj.attributes[cntr].attrValuesNum!=0)
			{
				userName = reinterpret_cast<char *>(destObj.attributes[cntr].attrValues[0]);
			}
			tmpArgv[tmpArgc] = new char[userName.size() + 1 ];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], userName.c_str());
		}
	}
	myOmhandler.Finalize();
	AES_CDH_TRACE_MESSAGE( "Leaving");
	return true;
}

/*===================================================================
        ROUTINE: getExistingAdvDestObjFromIMM
=================================================================== */
bool AES_CDH_FileInitDestCmdHandler::getExistingAdvDestObjFromIMM( const char *objName, int &tmpArgc, char** tmpArgv )
{
	AES_CDH_TRACE_MESSAGE( "Entering");

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
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting Adv object from IMM");
		AES_CDH_TRACE_MESSAGE("Error occured while getting Adv object from IMM");
		myOmhandler.Finalize();
		return false;

	}
	myOmhandler.Finalize();

	//Read the attributes of the advanced initiating object
	for ( unsigned int cntr = 0; cntr  < destObj.attributes.size(); cntr++ )
	{

		if ( destObj.attributes[cntr].attrName == AES_CDH_FILE_FILEOVERWRITE)
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
		else if ( destObj.attributes[cntr].attrName == AES_CDH_FILE_RETRYDELAY) 
		{
			int retryDelay =-1;
			if (destObj.attributes[cntr].attrValuesNum != 0 )
			{
				retryDelay = *reinterpret_cast<ACE_INT32 *>(destObj.attributes[cntr].attrValues[0]);
			}
			// Copy the retryDelay option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-d");

			// Copy the retryDelay value
			char myRetryDelay[11] = { 0 };
			ACE_OS::sprintf( myRetryDelay, "%d", retryDelay);
			tmpArgv[tmpArgc] = new char[ACE_OS::strlen(myRetryDelay)+1];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], myRetryDelay);

		}
		else if ( destObj.attributes[cntr].attrName == AES_CDH_FILE_SENDRETRY)
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
		else if ( destObj.attributes[cntr].attrName == AES_CDH_SUPPRESS_SUB_FOLDER_CREATION )
		{
			int supSubFolderCreation = 0;
			if (destObj.attributes[cntr].attrValuesNum != 0 )
			{
				supSubFolderCreation = *reinterpret_cast<ACE_INT32 *>(destObj.attributes[cntr].attrValues[0]);
			}
			// Copy the supSubFolderCreation option
			tmpArgv[tmpArgc] = new char[3];
			ACE_OS::strcpy(tmpArgv[tmpArgc++], "-g");

			// Copy the supSubFolderCreation value
			switch( supSubFolderCreation )
			{
			case 0:
				tmpArgv[tmpArgc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
				ACE_OS::strcpy(tmpArgv[tmpArgc++],AES_CDH_INIT_NO);
				break;

			case 1:
				tmpArgv[tmpArgc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
				ACE_OS::strcpy(tmpArgv[tmpArgc++],AES_CDH_INIT_YES);
			}
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;
}
