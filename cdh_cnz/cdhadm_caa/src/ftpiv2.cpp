/*=================================================================== */
/**
@file ftpiv2.cpp

Class method implementation for ftpiv2.h

DESCRIPTION
This class handles sending files with ftp initiating.
The source code is adapted for WinNT only.

ERROR HANDLING
General rule:

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ftpiv2.h>
#include <event.h>
#include <servr.h>
#include <fstream>
#include <aes_gcc_errorcodes_r1.h>
#include <ace/ACE.h>
#include <getopt.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
#include <aes_cdh_common.h>

using namespace std;

extern ACE_TCHAR* optarg;
extern ACE_INT32  optind;
extern ACE_INT32  opterr;

/*===================================================================
                        DECLARATION SECTION
=================================================================== */
AES_CDH_TRACE_DEFINE(AES_CDH_FTPIV2);



/*===================================================================
   ROUTINE:FTPIV2
=================================================================== */
FTPIV2::FTPIV2()
{
	listTime = 0;
}

/*===================================================================
   ROUTINE:~FTPIV2
=================================================================== */
FTPIV2::~FTPIV2()
{
}

/*===================================================================
   ROUTINE:define
=================================================================== */
AES_CDH_ResultCode FTPIV2::define(ACE_INT32& argc,
		ACE_TCHAR* argv[],
		const bool define,
		const bool recovery)
{
	(void) define;
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	returnCode = parseFTP(argc, argv);
	if (returnCode == AES_CDH_RC_OK)
	{
		returnCode = parseFTPI(argc, argv);
	}
	cleanUpArgv(argc, argv, 1);
	if (returnCode == AES_CDH_RC_OK && argc != 1)
	{
		if (!recovery)
			returnCode = AES_CDH_RC_INCUSAGE;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}

/*===================================================================
   ROUTINE:remove
=================================================================== */
AES_CDH_ResultCode FTPIV2::remove(void)
{    
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:getAttributes
=================================================================== */
AES_CDH_ResultCode FTPIV2::getAttributes(string& transferType, vector<string>& attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	// INGO# GOH qabhall transferType = "ftp";
	transferType = transType;
	m_ftpAgentMx.acquire();
	if(NULL != ftpAgent)
	{ 
		FTPIV2 * tempAgent = dynamic_cast<FTPIV2 *>(ftpAgent);
		if (NULL != tempAgent)
			fetchAttrValues(tempAgent,attr);
	}
	else
	{
		fetchAttrValues(this,attr);
	}
	m_ftpAgentMx.release();
	AES_CDH_TRACE_MESSAGE( "Leaving");
	return AES_CDH_RC_OK;
}
void FTPIV2::fetchAttrValues(FTPIV2 * pftpiv2, vector<string>& attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if (!((pftpiv2->hostAddress).empty()))
	{
		attr.push_back("-a");
		attr.push_back(pftpiv2->hostAddress);
	}
	if (!((pftpiv2->connectType).empty()))
	{
		attr.push_back("-c");
		attr.push_back(pftpiv2->connectType);
	}

	if (!((pftpiv2->portNr).empty()))
	{
		attr.push_back("-n");
		attr.push_back(pftpiv2->portNr);
	}

	if (!((pftpiv2->passWord).empty()))
	{
		attr.push_back("-p");
		attr.push_back(pftpiv2->passWord);
	}

	if (!((pftpiv2->remoteDir).empty()))
	{
		attr.push_back("-r");
		attr.push_back(pftpiv2->remoteDir);
	}

	if (!((pftpiv2->userName).empty()))
	{
		attr.push_back("-u");
		attr.push_back(pftpiv2->userName);
	}

	if (!((pftpiv2->overWrite).empty()))
	{
		attr.push_back("-o");
		attr.push_back(pftpiv2->overWrite);
	}

	if (!((pftpiv2->suppressDirCreate).empty()))
	{
		attr.push_back("-g");
		attr.push_back(pftpiv2->suppressDirCreate);
	}

	if (!((pftpiv2->fileRetryDelay).empty()))
	{
		attr.push_back("-d");
		attr.push_back(pftpiv2->fileRetryDelay);
	}

	if (!((pftpiv2->fileSendRetries).empty()))
	{
		attr.push_back("-s");
		attr.push_back(pftpiv2->fileSendRetries);
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}	
/*===================================================================
   ROUTINE:getSingleAttr:
=================================================================== */
AES_CDH_ResultCode FTPIV2::getSingleAttr(const ACE_TCHAR* opt, void* value)
{
	(void) opt;
	(void) value;
	AES_CDH_TRACE_MESSAGE("Entering");
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:sendFile
=================================================================== */
AES_CDH_ResultCode FTPIV2::sendFile(const string& fileName,
		const string& remoteSubDirName,
		const string& newFileName,
		AES_CDH_DestinationSet::transferMode trMode)

{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	cdhchSemaphore->acquire();

	this->aFileName = fileName;
	this->aNewMainFileName = remoteSubDirName;
	this->aNewSubFileName = newFileName;

	// qabjsti Ingo4_PRA2 start
	ACE_INT32 fileSendRetries_int;
	ACE_INT32 fileRetryDelay_int;
	if (fileSendRetries == "")
	{
		fileSendRetries_int = 0;
		fileRetryDelay_int = 10;
		AES_CDH_TRACE_MESSAGE("Send Retries is not defined, hence taking the default values.");
		AES_CDH_TRACE_MESSAGE("Send Retries = %d, Retry Delay = %d", fileSendRetries_int, fileRetryDelay_int);
	}
	else
	{
		fileSendRetries_int = atoi(fileSendRetries.c_str()) ;
		AES_CDH_TRACE_MESSAGE("Send Retries = %d", fileSendRetries_int);

		if (fileRetryDelay == "")
		{
			fileRetryDelay_int = 10;
			AES_CDH_TRACE_MESSAGE("Retry Delay is not define, hence taking the default value.");
			AES_CDH_TRACE_MESSAGE("Retry Delay = %d",fileRetryDelay_int);
		}
		else
		{
			fileRetryDelay_int = atoi(fileRetryDelay.c_str());
			AES_CDH_TRACE_MESSAGE("Retry Delay = %d",fileRetryDelay_int);
		}
	}
	while (fileSendRetries_int  >= 0)
	{
		// New option suppressDirCreate (uabchsn 031013). if =yes we dont use
		// remoteSubDirName as sub directory to store files on the remote system
		AES_CDH_TRACE_MESSAGE("Trying to send the file. Counter is %d ", fileSendRetries_int);
		returnCode = sendFileInitiating(fileName,
				(suppressDirCreate == "yes" ? "" : remoteSubDirName),
				newFileName,
				trMode);
		if (returnCode == AES_CDH_RC_OK)
		{
			fileSendRetries_int = -1;
			AES_CDH_TRACE_MESSAGE("Send the file successfully. Changing the counter to %d", fileSendRetries_int);
		}
		else if (returnCode == AES_CDH_RC_CONNECTERR ||
				returnCode == AES_CDH_RC_LOGONFAILURE ||
				returnCode == AES_CDH_RC_SENDERR)
		{
			--fileSendRetries_int;
			ACE_stat fileStat;
			int res = ACE_OS::stat(fileName.c_str(), &fileStat );
			if(res != 0)
			{
				if(errno == ENOENT)
				{
					AES_CDH_LOG(LOG_LEVEL_WARN, "In FTPIV2::sendFile() %s , Unable to find origin file",fileName.c_str());
					returnCode = AES_CDH_RC_PHYSFILEERR;
					break;
				}
			}
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "Couldn't send file %s. Error is %d. Changing the counter to %d", fileName.c_str(),returnCode, fileSendRetries_int);
			if (fileSendRetries_int > -1)
			{
				AES_CDH_TRACE_MESSAGE("Sleeping for retry seconds %d", fileRetryDelay_int);
				ACE_Time_Value tv( fileRetryDelay_int, 0 );
				string destSetName = destinationPtr->getDestSetName();
				ACE_Event *stopFileTransferEvent = 0;
				if( ServR::getFileTransferStopEvent(destSetName, aFileName,aNewMainFileName,aNewSubFileName , &stopFileTransferEvent ) == true )
				{
					int ret = stopFileTransferEvent->wait(&tv, 0);
					if( ret == 0 )
					{
						AES_CDH_TRACE_MESSAGE("Stop transfer is requested, making retries as -1");
						//Stop Transfer Requested.
						fileSendRetries_int = -1;
					}
					AES_CDH_TRACE_MESSAGE("Retry Delay over");
				}
				else
				{
					fileSendRetries_int = -1;
				}

			}
		}
		else
		{	// no point in trying again
			fileSendRetries_int = -1;
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "Couldn't send file %s. Changing the counter to %d",fileName.c_str(),fileSendRetries_int);
		}
	} //end while
	m_ftpAgentMx.acquire();
	if (ftpAgent != NULL)
	{
		// Change of parameters using cdhch requested
		this->changeParameters();
		delete ftpAgent;
		ftpAgent = NULL;
	}
	m_ftpAgentMx.release();
	//disableBreakTransmFlag();
	cdhchSemaphore->release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}  

/*===================================================================
   ROUTINE:changeParameters
=================================================================== */
AES_CDH_ResultCode FTPIV2::changeParameters()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	FTPIV2 *p = dynamic_cast<FTPIV2 *>(ftpAgent);
	if (NULL != p)
	{
		this->userName     = p->userName;
		this->passWord     = p->passWord;
		this->portNr       = p->portNr;
		this->hostAddress  = p->hostAddress;
		this->remoteDir    = p->remoteDir;
		this->fileRetryDelay = p->fileRetryDelay;
		this->fileSendRetries = p->fileSendRetries;
		this->suppressDirCreate = p->suppressDirCreate;
		// Shared attributes from FTPV2Agent
		this->connectType  = p->connectType;
		this->overWrite    = p->overWrite;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:sendDirectory
=================================================================== */
AES_CDH_ResultCode FTPIV2::sendDirectory(const string& dirName,
		const string& remoteSubDirName,
		const string& newDirName,
		AES_CDH_DestinationSet::transferMode trMode,
		const string& fileMask)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	cdhchSemaphore->acquire();

	this->aFileName = dirName;
	this->aNewMainFileName = remoteSubDirName;
	this->aNewSubFileName = newDirName;

	// qabjsti Ingo4_PRA2 start
	ACE_INT32 fileSendRetries_int;
	ACE_INT32 fileRetryDelay_int;

	if (fileSendRetries == "")
	{
		fileSendRetries_int = 0;
		fileRetryDelay_int = 10;
	}
	else
	{
		fileSendRetries_int = atoi(fileSendRetries.c_str());

		if (fileRetryDelay == "")
		{
			fileRetryDelay_int = 10;
		}
		else
		{
			fileRetryDelay_int = atoi(fileRetryDelay.c_str());
		}
	}

	AES_CDH_TRACE_MESSAGE("startvalue fileSendRetries_int %d", fileSendRetries_int);
	AES_CDH_TRACE_MESSAGE("fileRetryDelay_int %d", fileRetryDelay_int);

	while (fileSendRetries_int >= 0)
	{
		// New option suppressDirCreate (uabchsn 031013). if =yes we dont use
		// remoteSubDirName as sub directory to store files on the remote system
		returnCode = sendDirectoryInitiating(dirName,
				(suppressDirCreate == "yes" ? "" : remoteSubDirName),
				newDirName,
				trMode,
				fileMask);

		if (returnCode == AES_CDH_RC_OK)
		{
			fileSendRetries_int = -1;
		}
		else if (returnCode == AES_CDH_RC_CONNECTERR ||
				returnCode == AES_CDH_RC_LOGONFAILURE ||
				returnCode == AES_CDH_RC_SENDERR)
		{
			--fileSendRetries_int;
			ACE_stat dirStat;
			int res = ACE_OS::stat(dirName.c_str(), &dirStat );
			if(res != 0)
			{
				if(errno == ENOENT)
				{
					AES_CDH_TRACE_MESSAGE("In FTPIV2::sendDirectory() %s , Unable to find origin directroy",dirName.c_str());
					returnCode = AES_CDH_RC_PHYSFILEERR;
					break;
				}
			}

			if (fileSendRetries_int > -1)
			{
				string destSetName = destinationPtr->getDestSetName();
				ACE_Event *stopFileTransferEvent = 0;
				if( ServR::getFileTransferStopEvent(destSetName, aFileName,aNewMainFileName,aNewSubFileName , &stopFileTransferEvent ) == true )
				{
					ACE_Time_Value tv( fileRetryDelay_int, 0 );
					int ret = stopFileTransferEvent->wait(&tv, 0);
					if( ret == 0 )
					{
						AES_CDH_TRACE_MESSAGE("Stop transfer is requested, making retries as -1");
						//Stop Transfer Requested.
						fileSendRetries_int = -1;
					}	
				}
				else
				{
					fileSendRetries_int = -1;
				}
			}
		}
		else
		{	// no point in trying again
			fileSendRetries_int = -1;
		}

		AES_CDH_TRACE_MESSAGE("whileloop fileSendRetries_int %d", fileSendRetries_int);
	} //end while
	// qabjsti Ingo4_PRA2 end

	m_ftpAgentMx.acquire();
	if (ftpAgent != NULL)
	{
		// Change of parameters using cdhch requested
		this->changeParameters();
		delete ftpAgent;
		ftpAgent = NULL;
	}
	m_ftpAgentMx.release();
	//disableBreakTransmFlag();
	cdhchSemaphore->release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}

/*===================================================================
   ROUTINE:checkConnection
=================================================================== */
AES_CDH_ResultCode FTPIV2::checkConnection(void)      // INGO3 GOH qabhall 010510
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	connectMutex.acquire();

	try
	{
		rCode = verifyConnection();
	}
	catch(const AES_CDH_ResultCode & rc)
	{
		rCode = rc;
	}

	connectMutex.release();
	AES_CDH_TRACE_MESSAGE("Leaving with result code is %d", rCode);
	return rCode;
}

/*===================================================================
   ROUTINE:changeAttr
=================================================================== */
AES_CDH_ResultCode FTPIV2::changeAttr(TransferAgent *agent)
{
	AES_CDH_TRACE_MESSAGE( "Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"changeAttr invoked");
	if (cdhchSemaphore->tryacquire() < 0)
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, "%s","[FTPIV2::changeAttr] CANNOT ACQUIRE SEMAPHORE BECAUSE TRAFFIC ONGOING");
		m_ftpAgentMx.acquire();
		if(NULL != ftpAgent)
		{
			delete ftpAgent;
			ftpAgent = NULL;
		}
		ftpAgent = agent;
		m_ftpAgentMx.release();
	}
	else
	{
		//Lock acquired

		FTPIV2 *p = dynamic_cast<FTPIV2 *>(agent);
		if (NULL != p)
		{
			this->userName     = p->userName;
			this->passWord     = p->passWord;
			this->portNr       = p->portNr;
			this->hostAddress  = p->hostAddress;
			this->remoteDir    = p->remoteDir;
			this->fileRetryDelay = p->fileRetryDelay;
			this->fileSendRetries = p->fileSendRetries;
			this->suppressDirCreate = p->suppressDirCreate;

			// Shared attributes from FTPV2Agent
			this->connectType  = p->connectType;
			this->overWrite    = p->overWrite;
		}
		delete agent;
		agent = NULL;

		cdhchSemaphore->release();
	}

	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:parseFTPI
=================================================================== */
AES_CDH_ResultCode FTPIV2::parseFTPI(ACE_INT32& argc, ACE_TCHAR* argv[])
{
	AES_CDH_TRACE_MESSAGE("Entering");
	ACE_INT32 opt;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	ACE_INT32 aFlag = 0;
	ACE_INT32 dFlag = 0;
	ACE_INT32 gFlag = 0; // INGO4 new option '-g' 031013 uabchsn
	ACE_INT32 nFlag = 0;
	ACE_INT32 pFlag = 0;
	ACE_INT32 rFlag = 0;
	ACE_INT32 sFlag = 0;
	ACE_INT32 uFlag = 0;
	optind = 1;

	while ((opt = getopt(argc, argv, "a:d:g:n:p:r:s:u:")) != -1)
	{
		switch (opt)
		{
		case 'a':
		{
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			hostAddress = optarg;
			if(AES_CDH_Common_Util::isIPv6Address(hostAddress))		//IPv6_feature:: IPv6 format length limits
			{
				if ((hostAddress.length() < 2) ||
						(hostAddress.length() > 39))
				{
					return AES_CDH_RC_UNREAS; // host_address name too long or not specified
				}
			}
			else
			{														//IPv6_feature:: IPv4 format length limits
				if ((hostAddress.length() < 1) ||
						(hostAddress.length() > 30))
				{
					return AES_CDH_RC_UNREAS; // host_address name too long or not specified
				}
			}


			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			aFlag++;
			break;
		}

		case 'd':
		{
			fileRetryDelay = optarg;

			if(fileRetryDelay.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			ACE_INT32 fileRetryDelay_int = atoi(fileRetryDelay.c_str());
			if((fileRetryDelay_int > 60) || (fileRetryDelay_int < 1))
				return AES_CDH_RC_UNREAS;

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			dFlag++;
			break;
		}

		case 'g':
		{
			// Suppress creation of directory with TQ name on remote system
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			suppressDirCreate = optarg;
			upperToLower(suppressDirCreate);

			// "no" is default
			if (suppressDirCreate != "yes" && suppressDirCreate != "no")
			{
				return AES_CDH_RC_INCUSAGE;
			}

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;

			gFlag++;
			break;
		}

		case 'n':
		{
			portNr = optarg;

			// TR: HC77290 start
			if(portNr.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			ACE_INT32 portNr_int = atoi(portNr.c_str());
			if((portNr_int > 65535) || (portNr_int < 0))
				return AES_CDH_RC_UNREAS;
			// TR: HC77290 end

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			nFlag++;
			break;
		}

		case 'p':
		{
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;
			passWord = optarg;
#if 0
			if ((passWord.length() < 1) || (passWord.length() > 128))
			{
				return AES_CDH_RC_UNREAS; // password too long or not specified
			}
#endif

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			pFlag++;
			break;
		}

		case 'r':
		{
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			// INGO3 GOH qabhall start
			// Do not allow backslash in remoteDir. Exchange backslash to slash.
			remoteDir = optarg;
			size_t found;


			found = remoteDir.find("\\");
			if ( found != string::npos)
			{
				return AES_CDH_RC_INCUSAGE;
			}


			if( remoteDir.length() != 0 )
			{

				// Check if remoteDir is terminated correct with a slash. Otherwise add it.
				string::size_type pos;
				pos = remoteDir.find_last_of("/");
				if ( (pos != remoteDir.length() - 1) && (remoteDir.length() > 0) )
				{
					remoteDir = remoteDir + "/";
				}
			}


			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			rFlag++;
			break;
		}

		case 's':
		{
			fileSendRetries = optarg;

			if(fileSendRetries.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			ACE_INT32 fileSendRetries_int = atoi(fileSendRetries.c_str());
			if((fileSendRetries_int > 100) || (fileSendRetries_int < 0))
				return AES_CDH_RC_UNREAS;

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			sFlag++;
			break;
		}

		case 'u':
		{
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;
			userName = optarg;

			if ((userName.length() < 1) || (userName.length() > 128))
			{
				return AES_CDH_RC_UNREAS; // username too long or not specified
			}

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			uFlag++;
			break;
		}

		case '?':
		{
			if ((optind + 1) < argc)
			{
				if (argv[optind][0] != '-' &&
						argv[optind + 1][0] == '-')
					optind++;
			}
			break;
		}

		default:
		{
			/*
            if (recovery)
            {
               // This is a option created in a future version of CDH which this
               // version does not recognise (for backwords compatibility).
               // It is a option read from the extended destination file during
               // startup. We just ignore this parameter.
               delete [] argv[optind-2];
               argv[optind-2] = NULL;
               delete [] argv[optind-1];
               argv[optind-1] = NULL;
            }
            else */
			return AES_CDH_RC_INTPROGERR;
		}
		}
	}

	if (aFlag > 1 || dFlag > 1 || nFlag > 1 || pFlag > 1 ||
			rFlag > 1 || sFlag > 1 || uFlag > 1) returnCode = AES_CDH_RC_INCUSAGE;

	if (aFlag == 0)
		returnCode = AES_CDH_RC_INCUSAGE;

	if (transType == SFTPTypeV2)
	{
		if (pFlag == 0 || uFlag == 0) returnCode = AES_CDH_RC_INCUSAGE;
	}
	else
	{
		// transfer type FTPV2
		if (pFlag == 1 && uFlag == 0) returnCode = AES_CDH_RC_INCUSAGE;
	}

	cleanUpArgv(argc, argv, optind);
	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}  

/*===================================================================
   ROUTINE:maskInFileName
=================================================================== */
bool FTPIV2::maskInFileName(string pattern, string fileName) 
{
	AES_CDH_TRACE_MESSAGE("Entering");
	string substr1;
	string substr2;

	if (pattern.find_first_of('*') == string::npos)
	{
		return (!pattern.compare(fileName));	// No '*'s, just test for equallity
	}

	if (pattern.find_first_of('*') != 0)
	{
		substr1 = pattern.substr(0, pattern.find_first_of('*'));
		substr2 = fileName.substr(0, substr1.length());

		if (substr1.compare(substr2))
		{
			return false;
		}

		pattern = pattern.substr(substr1.length());	// remove part of pattern
		fileName = fileName.substr(substr1.length());	// pattern before first '*'
	}

	if ((pattern.find_last_of('*')+1) != pattern.length())
	{
		substr1 = pattern.substr(pattern.find_last_of('*') + 1);

		// Check that mask is not longer than the filename
		ACE_INT32 nLen = (substr1.length() > fileName.length() ? 0 : fileName.length() - substr1.length());
		substr2 = fileName.substr(nLen);

		if (substr1.compare(substr2))
		{
			return false;
		}								// Remove part of pattern after last '*'

		pattern = pattern.substr(0, pattern.length() - substr1.length());
		fileName = fileName.substr(0, nLen);
	}

	//HI57432
	//This for loop will return false when the file name is not having any extension
	//Even the file is not having any extension, the transfer mode shoyld not be changed.
	/*

	for (;;)
   {
		if (!pattern.compare("*"))
      {
			return true;
		}

		substr1 = pattern.substr(1, pattern.find_first_of('*',1) - 1);
		if ((pos = fileName.find(substr1)) == string::npos)
      {
			return false;
		}

		fileName = fileName.substr(pos + substr1.length());	// Remove first part of
		pattern = pattern.substr(substr1.length() + 1);		// string and pattern
	}
	 */
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;
	//HI57432
}
/*===================================================================
   ROUTINE:getVDDestPath
=================================================================== */
std::string FTPIV2::getVDDestPath()
{
	return "";
}
/*===================================================================
   ROUTINE:readVDPath
=================================================================== */
AES_CDH_ResultCode FTPIV2::readVDPath( std::string& path)
{
	(void)path;
	return AES_CDH_RC_INCUSAGE;
}
