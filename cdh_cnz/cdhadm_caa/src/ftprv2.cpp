/*=================================================================== */
/**
@file ftprv2.cpp

Class method implementation for ftprv2.h

DESCRIPTION
This class supports sending files with responding FTP protocol GOH V2.
This class handles sending files with ftp responding
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
#include <ftprv2.h>
#include <event.h>
#include <servr.h>
#include <cdhcriticalsection.h>
#include <time.h>
#include <fstream>
#include <getopt.h>
#include <sys/types.h>
#include <errno.h>
#include <aes_gcc_util.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;

extern ACE_TCHAR* optarg;
extern ACE_INT32  optind;
extern ACE_INT32  opterr;

/*===================================================================
                        DECLARATION SECTION
=================================================================== */
static const mode_t dir_full_perms = 0777;
static const mode_t temp_mask = 0000;

AES_CDH_TRACE_DEFINE(AES_CDH_FTPRV2);

/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
static const string sendDirName    = "Send";
static const string deleteDirName  = "Delete";
static const string readyDirName   = "Ready";
static const string archiveDirName = "Archive";
static const ACE_INT32 nrOfSubDirs       = 4;

/*===================================================================
   ROUTINE:FTPRV2
=================================================================== */

//##ModelId=3DE49B440234
FTPRV2::FTPRV2(): vdDestPath(""),
                  vdPath(""),
                  eventType(""),
                  respondTime(""),
                  clusterIPAddress(""),
                  notificationAddress(""),
                  notificationPortNr(""),
                  suppressDirCreate(""),
                  removeTQFilePrefix(""),
                  isFileNotification(false),
                  badFTPPermissions(false),
                  keep(NULL),
                  keepTime(-1),
                  fileMaxAge(""),
		  db(NULL)
{
}

/*===================================================================
   ROUTINE:~FTPRV2
=================================================================== */
//##ModelId=3DE49B44023D
FTPRV2::~FTPRV2()
{
   if (keep)
      delete keep;
}

/*===================================================================
   ROUTINE:define
=================================================================== */
//##ModelId=3DE49B440247
AES_CDH_ResultCode FTPRV2::define(ACE_INT32& argc,
                                  ACE_TCHAR* argv[],
                                  const bool define,
                                  const bool recovery)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	returnCode = parseFTP(argc, argv);

	AES_CDH_TRACE_MESSAGE("Return code from parse FTP = %d", returnCode);


	if (returnCode == AES_CDH_RC_OK)
	{
		returnCode = parseFTPR(argc, argv, define, recovery);
	}

	AES_CDH_TRACE_MESSAGE("Return code from parse FTPR = %d", returnCode);

	cleanUpArgv(argc, argv, 1);

	if (returnCode == AES_CDH_RC_OK && argc != 1)
	{
		AES_CDH_TRACE_MESSAGE("Return code cleanUpArgv = %d", returnCode);
		if (!recovery)
			returnCode = AES_CDH_RC_INCUSAGE;
	}

	// START HG12384 XTBFRRO
	// When -g "yes" is used in the current destination,
	// following 2 restictions are used:
	// There may not exist any other destinations with:
	// (A) In case of a current keep destination:
	//  * "-g yes" AND same VDir.
	// (B) In case of a current NON keep destination:
	//  * "-g yes" AND same VDir AND keep option set (keepTime > -1).
	if (suppressDirCreate == "yes")
	{
		if (ServR::checkOtherIncompatibleRespDestExists(destinationName, vdPath, keepTime))
			return AES_CDH_RC_DESTWITHSAMEOPTEXIST;

	}
	//END HG12384

	// INGO3 GOH qabhall 010815 TR: HC78109  section moved to last pos in this method
	if (returnCode == AES_CDH_RC_OK)
	{
		returnCode = createFtpDestDir();
	}
	AES_CDH_TRACE_MESSAGE(" Return code from createFtpDestDir = %d", returnCode);

	// If keepTime specified, use KEEP functionality. Create FTPRV2Keep object
	if (returnCode == AES_CDH_RC_OK && keepTime > -1)
	{
		if (keep)
			delete keep;

		keep = new FTPRV2Keep;
		if (!keep)
			return AES_CDH_RC_INTPROGERR;

		returnCode = keep->define(vdDestPath.c_str(), keepTime, recovery);

		// If FTPRV2Keep::define return an error, we must clean up before leaving
		if (returnCode != AES_CDH_RC_OK)
		{
			this->remove();
		}
	}

	AES_CDH_TRACE_MESSAGE("Leaving with returnCode = %d", returnCode);
	return returnCode;
}

/*===================================================================
   ROUTINE:remove
=================================================================== */
//##ModelId=3DE49B440298
AES_CDH_ResultCode FTPRV2::remove(void)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	bool dirsAreEmpty = true;
	bool bOtherDestExists = false;
	bool bSuppressDirCreate = false;

	if (!vdDestPath.empty())
	{
		if (suppressDirCreate == "yes")
			bSuppressDirCreate = true;

		cleanUpFtpDeleteDir();

		// START HG12384 XTBFRRO
		// Check if another destination uses the same virtual directory and
		// when the -g option set to "yes"
		if (bSuppressDirCreate)
			bOtherDestExists = ServR::checkOtherRespDestExists(this->destinationName,
					vdPath);

		if (!bOtherDestExists)
		{
			// END HG12384 XTBFRRO

			// Check if all directories are empty before deleting any.
			const ACE_INT32 nNumOfDirs = (bSuppressDirCreate ? nrOfSubDirs : nrOfSubDirs+1);
			ACE_INT32 i = 0, saveIdx;

			string dirNames[nrOfSubDirs+1];
			dirNames[i++] = vdDestPath + "/" + sendDirName;
			dirNames[i++] = vdDestPath + "/" + deleteDirName;
			dirNames[i++] = vdDestPath + "/" + readyDirName;
			dirNames[i++] = vdDestPath + "/" + archiveDirName;

			// If we are using suppress subdirectory creation, we can ignore the "root"
			// directory
			if (!bSuppressDirCreate)
				dirNames[i++] = vdDestPath;

			for (i = 0; i < nrOfSubDirs; i++)
			{
				string searchPattern = dirNames[i] ;
				//hFind = FindFirstFile(searchPattern.c_str(), &finddata);
				DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
				if ( !pDir)
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "FTPRV2::remove(), opendir failed for path, errno = %d", errno);
					continue;
				}

				struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
				while(pEntry!=NULL)
				{
					if (! ((ACE_OS::strcmp(pEntry->d_name,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name,"..") == 0)))
					{
						dirsAreEmpty = false;
						saveIdx = i;
						break;
					}
					//bMore = FindNextFile(hFind, &finddata);
					pEntry = ACE_OS::readdir(pDir);
				}

				if (pDir != NULL)
					closedir(pDir);

			}

			// Check if a user is standing in the path with some tool. If
			// the "root" can not be renamed, he may do just that. Either way,
			// removing the ftp-area is generally not possible if the "root"
			// can not be renamed.
			for (i = 0; i < nNumOfDirs; i++)
			{
				if ( (returnCode = IsDirectoryLocked(dirNames[i])) != AES_CDH_RC_OK)
					return returnCode;
			}

			// Do not delete any directory if not all are empty.
			if (dirsAreEmpty)
			{
				AES_CDH_ResultCode keepResultCode;

				for (i = 0; i < nNumOfDirs; i++)
				{
					if (keep)
					{
						//if (nNumOfDirs != nrOfSubDirs && i == (nNumOfDirs - 1))
						if (i == nrOfSubDirs)
						{
							// If KEEP functionaliy is used, clean up in the KEEP dir
							if ((keepResultCode = keep->remove()) != AES_CDH_RC_OK)
								returnCode = keepResultCode;
						}
					}

					if (ACE_OS::rmdir(dirNames[i].c_str()) == -1)
					{
						Event::report(AES_CDH_dataAreaFault,
								"DATA AREA ERROR",
								ServR::NTErrorText(),
								"Directory could not be removed: " + dirNames[i]);

						returnCode = AES_CDH_RC_DATAAREAERR;
					}
				}

				if (keep)
				{
					if (nNumOfDirs == nrOfSubDirs && returnCode == AES_CDH_RC_OK)
					{
						if ((keepResultCode = keep->remove()) != AES_CDH_RC_OK)
							returnCode = keepResultCode;
					}
				}
			}
			else
			{
				Event::report(AES_CDH_dataAreaFault,
						"DATA AREA ERROR",
						"Subdirectories not empty",
						"Directory could not be removed: " + dirNames[saveIdx]);

				returnCode = AES_CDH_RC_DATAAREAERR;
			}
		}//End if (!bOtherDestExists)
	}

	// Finally, release the keep object if remove operation was successful
	if (keep && returnCode == AES_CDH_RC_OK)
	{
		delete keep;
		keep = NULL;
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:getAttributes
=================================================================== */
//##ModelId=3DE49B44025B
AES_CDH_ResultCode FTPRV2::getAttributes(string& transferType,
                                         vector<string>& attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	transferType = transType;
	m_ftpAgentMx.acquire();
	if(NULL != ftpAgent)
	{
		FTPRV2 * tempftprv2 = dynamic_cast<FTPRV2*>(ftpAgent);
		fetchAttrValues(tempftprv2,attr);
	}
	else
	{
		fetchAttrValues(this,attr);
	}
	m_ftpAgentMx.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}
void FTPRV2::fetchAttrValues(FTPRV2 * pftprv2, vector<string>& attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if (!((pftprv2->connectType).empty()))
	{
		attr.push_back("-c");
		attr.push_back(pftprv2->connectType);
	}

	if (!((pftprv2->overWrite)).empty())
	{
		attr.push_back("-o");
		attr.push_back(pftprv2->overWrite);
	}

	if (!((pftprv2->eventType).empty()))
	{
		attr.push_back("-e");
		attr.push_back(pftprv2->eventType);
	}

	if (!((pftprv2->notificationAddress).empty()))
	{
		attr.push_back("-f");
		attr.push_back(pftprv2->notificationAddress);
	}

	if (!((pftprv2->notificationPortNr).empty()))
	{
		attr.push_back("-x");
		attr.push_back(pftprv2->notificationPortNr);
	}

	if (!((pftprv2->respondTime).empty()))
	{
		attr.push_back((std::string)"-y");
		attr.push_back(pftprv2->respondTime);
	}

	// INGO4 maintenance uabchsn start
	if (!((pftprv2->suppressDirCreate).empty()))
	{
		attr.push_back("-g");
		attr.push_back(pftprv2->suppressDirCreate);
	}

	if (!((pftprv2->removeTQFilePrefix).empty()))
	{
		attr.push_back("-k");
		attr.push_back(pftprv2->removeTQFilePrefix);
	}
	// INGO4 maintenance uabchsn end

	// Get the attributes for the KEEP functionality
	if (pftprv2->keep)
	{
		(pftprv2->keep)->getAttributes(attr);
	}

	if (!((pftprv2->fileMaxAge).empty()))
	{
		attr.push_back("-z");
		attr.push_back(pftprv2->fileMaxAge);
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}
/*===================================================================
   ROUTINE:getSingleAttr
=================================================================== */
AES_CDH_ResultCode FTPRV2::getSingleAttr(const ACE_TCHAR* opt, void* value)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	switch (*(opt+1))
	{
	case 'b':
		if (keep)
			returnCode = keep->getSingleAttr(opt, value);
		else
			returnCode = AES_CDH_RC_INCUSAGE;
		break;

	default:
		returnCode = AES_CDH_RC_INCUSAGE;
	}

	return returnCode;
}
/*===================================================================
   ROUTINE:sendFile
=================================================================== */
//  sendFile()
//  sends a file with ftp initiating or responding.
//******************************************************************************
//##ModelId=3DE49B44026F
AES_CDH_ResultCode FTPRV2::sendFile(const string& fileName,
                                    const string& remoteSubDirName,
                                    const string& newFileName,
                                    AES_CDH_DestinationSet::transferMode trMode)
{
	(void) trMode;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	cdhchSemaphore->acquire();

	if (removeTQFilePrefix == "yes")
		returnCode = sendFileResponding(fileName, "", newFileName);
	else
		returnCode = sendFileResponding(fileName, remoteSubDirName, newFileName);

	m_ftpAgentMx.acquire();
	if (NULL != ftpAgent )
	{
		// Change of parameters using cdhch requested
		this->changeParameters();
		delete ftpAgent;
		ftpAgent = NULL;
	}
	m_ftpAgentMx.release();

	cdhchSemaphore->release();
	return returnCode;
}  

/*===================================================================
   ROUTINE:changeParameters
=================================================================== */
//##ModelId=3DE49B4502A2
AES_CDH_ResultCode FTPRV2::changeParameters()
{
	FTPRV2* p = dynamic_cast<FTPRV2*>(ftpAgent);

	this->notificationAddress = p->notificationAddress;
	this->notificationPortNr  = p->notificationPortNr;
	this->respondTime         = p->respondTime;
	this->eventType           = p->eventType;
	this->suppressDirCreate   = p->suppressDirCreate;
	this->removeTQFilePrefix  = p->removeTQFilePrefix;

	// Resend File Notification
	this->fileMaxAge          = p->fileMaxAge;

	// Shared attributes from FTPV2Agent
	this->connectType  = p->connectType;
	this->overWrite    = p->overWrite;

	// Attributes for KEEP
	if (keep && p->keep)
	{
		keep->changeParameters(p->keep);
	}
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:sendDirectory
=================================================================== */
//##ModelId=3DE49B440283
AES_CDH_ResultCode FTPRV2::sendDirectory(const string& dirName,
                                         const string& remoteSubDirName,
                                         const string& newDirName,
                                         AES_CDH_DestinationSet::transferMode trMode,
                                         const string& fileMask)
{
	(void) trMode;
	(void) fileMask;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	cdhchSemaphore->acquire();

	if (removeTQFilePrefix == "yes")
		returnCode = sendDirectoryResponding(dirName, "", newDirName);
	else
		returnCode = sendDirectoryResponding(dirName, remoteSubDirName, newDirName);
	m_ftpAgentMx.acquire();
	if (NULL != ftpAgent)
	{
		// Change of parameters using cdhch requested
		this->changeParameters();
		delete ftpAgent;
		ftpAgent = NULL;
	}
	m_ftpAgentMx.release();
	cdhchSemaphore->release();
	return returnCode;
}

/*===================================================================
   ROUTINE:checkConnection
=================================================================== */
//##ModelId=3DE49B4402A2
AES_CDH_ResultCode FTPRV2::checkConnection(void)     
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	AES_CDH_TRACE_MESSAGE("Entering");

	bool success = true;

	if (keep)
		returnCode = keep->checkConnection();
	else
		success = cleanUpFtpDeleteDir();

	if (!success)
		returnCode = AES_CDH_RC_CONNECTERR;

	return returnCode;
}

/*===================================================================
   ROUTINE:changeAttr
=================================================================== */
//##ModelId=3DE49B4402B5
AES_CDH_ResultCode FTPRV2::changeAttr(TransferAgent *agent)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	if (cdhchSemaphore->tryacquire() < 0)
	{
		m_ftpAgentMx.acquire();	
		AES_CDH_LOG(LOG_LEVEL_TRACE, "[FTPRV2::changeAttr] CANNOT ACQUIRE SEMAPHORE BECAUSE TRAFFIC ONGOING");
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
		//LOCK ACQUIRED
		FTPRV2* p = dynamic_cast<FTPRV2*>(agent);

		this->notificationAddress = p->notificationAddress;
		this->notificationPortNr  = p->notificationPortNr;
		this->respondTime         = p->respondTime;
		this->eventType           = p->eventType;

		// Resend File Notification
		this->fileMaxAge          = p->fileMaxAge;

		// Shared attributes from FTPV2Agent
		this->connectType  = p->connectType;
		this->overWrite    = p->overWrite;

		// Change attributes for KEEP
		if (keep && p->keep)
		{
			keep->changeParameters(p->keep);
		}

		delete agent;
		agent = NULL;

		cdhchSemaphore->release();
	}

	AES_CDH_TRACE_MESSAGE("Leaving");

	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:IsDirectoryLocked
=================================================================== */
AES_CDH_ResultCode FTPRV2::IsDirectoryLocked(const string &path)
{
	if (ACE_OS::rename(path.c_str(), (path + ".tmp").c_str()) == -1)
	{
		// Error renaming directory! Stop here, and report error to caller
		Event::report(AES_CDH_dataAreaFault,
				"DATA AREA ERROR",
				"FTP directory busy?",
				"Directory could not be removed: " + path);

		return AES_CDH_RC_DATAAREAERR;
	}

	// The directory has been renamed. Undo now
	ACE_OS::rename((path + ".tmp").c_str(), path.c_str());

	return AES_CDH_RC_OK;
}
#if 0
/*===================================================================
   ROUTINE:readPHA
=================================================================== */
//  We are reading from GCC only
//******************************************************************************
//##ModelId=3DE49B4500A4

AES_CDH_ResultCode FTPRV2::readPHA(void)
{
    AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
    string aesdir ; 

	// Create parameter object
	AES_GCC_Variable aesDataPath(AES_CDH_AesDataDirectory);

	// Get parameters from GCC
	if (aesDataPath.exists())
	{
		aesdir = aesDataPath.getStr();
	}
	else
	{
		Event::report(AES_CDH_variableFault, "VARIABLE PROBLEM",
            "Retrieval of variable (" + string(AES_CDH_AesDataDirectory) + 
            ") from GCC failed.", "-");
        returnCode = AES_CDH_RC_INTPROGERR;
        // read aesdir from GCC failed
	}

    if (returnCode == AES_CDH_RC_OK)
    {
        cdhFtpDirName = aesdir ;
		int pos = cdhFtpDirName.find_last_of("/");
		if ((pos+1) != cdhFtpDirName.length())
		{
			cdhFtpDirName += "/" ;
		}
        cdhFtpDirName += AES_CDH_RootDirectory ;
        cdhFtpDirName += "/" ;
        cdhFtpDirName += AES_CDH_FtpDirectory ;
    }
    
    return returnCode;
}
#endif

/*===================================================================
   ROUTINE:parseFTPR
=================================================================== */
//##ModelId=3DE49B4500B8
AES_CDH_ResultCode FTPRV2::parseFTPR(ACE_INT32& argc,
                                     ACE_TCHAR* argv[],
                                     const bool define,
                                     const bool recovery)
{
	ACE_TCHAR opt;
	ACE_INT32 bFlag = 0;
	ACE_INT32 eFlag = 0;
	ACE_INT32 fFlag = 0;
	ACE_INT32 xFlag = 0;
	ACE_INT32 yFlag = 0;
	ACE_INT32 gFlag = 0;
	ACE_INT32 kFlag = 0;
	ACE_INT32 zFlag = 0;

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string temp;
	optind = 1;
	while ((opt = getopt(argc, argv, "b:e:f:g:k:x:y:z:")) != -1)
	{

		switch (opt)
		{

		case 'b':
		{
			// KEEP functionality is enabled. Keep time option
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			temp = optarg;
			if (temp.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			keepTime = atol(optarg);
			if (keepTime < 0 || keepTime > 168)
				return AES_CDH_RC_UNREAS;

			// If option "-b" was not used during cdhdef, it will not be allowed
			// to change this option with cdhch
			if (!define && !recovery)
			{
				TransDest* pDest;
				if (ServR::getDest(destinationName, pDest) == AES_CDH_RC_OK)
				{
					// Check for option existense
					if (pDest->getSingleAttr("-b", NULL) != AES_CDH_RC_OK)
						return AES_CDH_RC_KEEPOPTNOTDEFINED;
				}
			}

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			bFlag++;
			break;
		}
		case 'e':
		{
			if (argv[optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			eventType = optarg;

			upperToLower(eventType);
			if (eventType != "alarm" && eventType !="event" && eventType != "")
			{
				return AES_CDH_RC_INCUSAGE;
			}

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			eFlag++;
			break;
		}

		case 'f':
		{
			if (argv[optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			notificationAddress = optarg;

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			fFlag++;
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

		case 'k':
		{
			// Remove file prefix with TQ name from the filenames
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			removeTQFilePrefix = optarg;
			upperToLower(removeTQFilePrefix);

			// "no" is default
			if (removeTQFilePrefix != "yes" && removeTQFilePrefix != "no")
			{
				return AES_CDH_RC_INCUSAGE;
			}

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;

			kFlag++;
			break;
		}

		case 'x':
		{
			if (argv[optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			notificationPortNr = optarg;
			isFileNotification = true;

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			xFlag++;
			break;
		}

		case 'y':
		{
			if (argv[optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			respondTime = optarg;

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			yFlag++;
			break;
		}
		case 'z':
		{

			// Resend File Notification time
			if (argv[optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			fileMaxAge = optarg;
			if (fileMaxAge.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			ACE_INT32 tmpAge = atoi(optarg);
			if (tmpAge < 0 || tmpAge > 168)
				return AES_CDH_RC_UNREAS;

			delete [] argv[optind-2];
			argv[optind-2] = NULL;
			delete [] argv[optind-1];
			argv[optind-1] = NULL;
			zFlag++;
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
			return AES_CDH_RC_INTPROGERR;
		}
		}
	}


	if ( ( (fFlag >= 1)  &&  (xFlag == 0) ) ||
			( (fFlag == 0)  &&  (xFlag >=1) ) ||
			(gFlag > 1) ||
			(kFlag > 1) ||
			(bFlag > 1) ||
			(zFlag > 1) ||
			((zFlag >= 1) && (fFlag == 0)) )
	{
		return AES_CDH_RC_INCUSAGE;
	}


	returnCode = readVDPath( vdPath);

	if (returnCode != AES_CDH_RC_OK)
		return returnCode;

	if ((fFlag == 1) && (xFlag == 1))
	{
		if (notificationPortNr.find_first_not_of("0123456789") != string::npos)
		{
			return AES_CDH_RC_UNREAS;
		}

		ACE_INT32 notificationPortNr_int = atoi(notificationPortNr.c_str());
		if ((notificationPortNr_int > 65535) || (notificationPortNr_int < 0))
		{
			return AES_CDH_RC_UNREAS;
		}


		if (eFlag > 1)
		{
			return AES_CDH_RC_INCUSAGE;
		}

		if (yFlag == 0)
		{
			// -y not specified, default value is 10
		}
		else if (yFlag == 1)
		{
			// -y specified, verify correct input
			if (respondTime.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			// Valid range for the port, 0 .. 65535
			ACE_INT32 respondTime_int = atoi(respondTime.c_str());
			if ((respondTime_int > 10000) || (respondTime_int < 10))
			{
				return AES_CDH_RC_UNREAS;
			}
		}

		else if (yFlag > 1)
		{
			return AES_CDH_RC_INCUSAGE;
		}

		AES_CDH_TRACE_MESSAGE("FTPRV2::calling getClusterIp with notificationAddress = %s\n",notificationAddress.c_str());

		clusterIPAddress.assign(ServR::getClusterIP(notificationAddress));

		// Create an entry in the database for this destination
		this->createDataBaseEntry();
	}
	else
	{
		if (yFlag > 0)                  // -y may only be specified once
		{
			return AES_CDH_RC_INCUSAGE;
		}

		if (eFlag > 0)
		{
			return AES_CDH_RC_INCUSAGE; // -e may only be specified once
		}
	}

	if ((fFlag > 1) || (xFlag > 1))
	{
		return AES_CDH_RC_INCUSAGE;
	}

	cleanUpArgv(argc, argv, optind);

	if (returnCode == AES_CDH_RC_OK)
	{
		if (fFlag == 1 && connectType != "r" )      // -f requires responding
		{
			returnCode = AES_CDH_RC_INCUSAGE;
		}
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:createDataBaseEntry
=================================================================== */
//##ModelId=3DE49B45013A
bool FTPRV2::createDataBaseEntry()
{
	string destn = this->destinationName;
	upperToLower(destn);
	std::string fullPath("");
	std::string storePath("");

	if (suppressDirCreate == "yes")
	{
		fullPath = vdPath + "/" + readyDirName;
		storePath = readyDirName;
	}
	else
	{
		fullPath = vdPath + "/" + destn + "/" + readyDirName;
		storePath = destn + "/" + readyDirName;
	}

	AES_CDH_TRACE_MESSAGE("fullPath = %s and storePath = %s",fullPath.c_str(),storePath.c_str());

	ACE_UINT32 tmpAge  = 0;
	if( !fileMaxAge.empty())
	{
		tmpAge  = atoi(fileMaxAge.c_str());

	}

	CDHCriticalSection::instance().enter();
	DataBase *db = DataBase::instance();
	db->createDestinationEntry(this->destinationName, storePath, fullPath, clusterIPAddress, tmpAge);
	CDHCriticalSection::instance().leave();

	return true;
}

/*===================================================================
   ROUTINE:doDelete
=================================================================== */
//##ModelId=3DE49B450176
bool FTPRV2::doDelete(string path)
{
	bool success = true;
	bool bIsDir;
	string searchPattern = path;
	string fileN;
	AES_CDH_TRACE_MESSAGE("Entering with path :%s ",searchPattern.c_str());

	DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if ( !pDir)
	{ 
		AES_CDH_LOG(LOG_LEVEL_ERROR, "FTPRV2::doDelete(), opendir failed, errno = %d", errno);
		return false;
	}
	struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);

	if (pEntry)
	{
		AES_CDH_TRACE_MESSAGE("pEntry is not NULL");
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("pEntry = NULL");
		success = false;
	}

	while (pEntry != NULL)
	{ 
		fileN = path + "/" + string(pEntry->d_name);
		struct stat buf;
		stat(fileN.c_str(), &buf);
		bIsDir = S_ISDIR(buf.st_mode);
		if (!bIsDir)
		{
			fileN = path + "/" + pEntry->d_name;

			AES_CDH_TRACE_MESSAGE("File to be deleted = %s",fileN.c_str());

			if (::remove(fileN.c_str()) != 0)
			{
				Event::report(AES_CDH_fileRemoveFault,
						"FILE REMOVE ERROR",
						ServR::NTErrorText(),
						"Failed to remove output file: " + fileN);
			}
		}
		else
		{
			if (! ((ACE_OS::strcmp(pEntry->d_name,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name,"..") == 0)))
			{
				// A sub directory found. 
				// Make a recursive call to ourself in order to delete
				// files or subdirectories in it
				fileN = path + "/" + pEntry->d_name;
				doDelete(fileN);

				if (ACE_OS::rmdir(fileN.c_str()) == -1)
				{
					Event::report(AES_CDH_fileRemoveFault,
							"FILE REMOVE ERROR",
							ServR::NTErrorText(),
							"Failed to remove output directory: " + fileN);
				}
			}
		}

		pEntry = ACE_OS::readdir(pDir);
	}
	if (pDir != NULL)
		closedir(pDir);
	return success;
}
/*===================================================================
	ROUTINE:cleanUpFtpDeleteDir
=================================================================== */

//  Delete all files in the ftp delete directory.
//******************************************************************************
//##ModelId=3DE49B450162
bool FTPRV2::cleanUpFtpDeleteDir(void)
{
	bool success = true;
	AES_CDH_TRACE_MESSAGE("Entering");

	if (connectType == "r" && vdDestPath != "")
	{
		string path = vdDestPath + "/" + deleteDirName;
		success = doDelete(path);
	}

	return success;
}
/*===================================================================
ROUTINE:createFtpDestDir
=================================================================== */
//  Create the directory where the ftp output files will be placed.
//******************************************************************************
//##ModelId=3DE49B450194
AES_CDH_ResultCode FTPRV2::createFtpDestDir(void)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	bool bSuppressDirCreate = false;

	if (suppressDirCreate == "yes")
		bSuppressDirCreate = true;

	// vdDestPath: <virtual directory path>\\destname
	string destn = destinationName;
	upperToLower(destn);

	if (bSuppressDirCreate)
		vdDestPath = vdPath;
	else
		vdDestPath.assign(vdPath).append("/").append(destn);

	const ACE_INT32 nSubDirs = (bSuppressDirCreate ? nrOfSubDirs-1 : nrOfSubDirs);
	string dirNames[nrOfSubDirs+1];
	ACE_INT32 i = 0;

	// If we are using suppress subdirectory creation, we can ignore the "root"
	// directory
	if (!bSuppressDirCreate)
	{
		dirNames[i++] = vdDestPath;
	}

	dirNames[i++] = vdDestPath + "/" + sendDirName;
	dirNames[i++] = vdDestPath + "/" + deleteDirName;
	dirNames[i++] = vdDestPath + "/" + readyDirName;
	dirNames[i++] = vdDestPath + "/" + archiveDirName;

	for (i = 0; i < (nSubDirs+1); i++)   //INGO3 GOH qabhall
	{
		mode_t mask = ACE_OS::umask(temp_mask);
		if (ACE_OS::mkdir(dirNames[i].c_str() , dir_full_perms) == -1)
		{
			// Unsuccessful CreateDirectory. Check error code
			if (ACE_OS::last_error() == EEXIST)
			{
				// <virtual directory>\<destName> directory already exist, do nothing
			}
			else if (ACE_OS::last_error() == ENOENT)
			{
				Event::report(AES_CDH_physFileFault,
						"PHYSICAL FILE PROBLEM",
						ServR::NTErrorText(),
						"Physical path to virtual directory does not exist: "
						+ vdPath);

				returnCode = AES_CDH_RC_NOVIRTUALPATH;
			}
			else
			{
				if (bSuppressDirCreate)
					Event::report(AES_CDH_dataAreaFault,
							"DATA AREA ERROR",
							ServR::NTErrorText(),
							"FTP Directory could not be created: "
							+ dirNames[i]);
				else
					Event::report(AES_CDH_dataAreaFault,
							"DATA AREA ERROR",
							ServR::NTErrorText(),
							"FTP//destname Directory could not be created: "
							+ dirNames[i]);

				returnCode = AES_CDH_RC_DATAAREAERR;
			}
		}

		else // Only to be done if CreateDirectory was successful. HE78917
			if (bSuppressDirCreate || i == 0) // HE78917
			{
				if (! ServR::clearAcl(dirNames[i]))
					returnCode = AES_CDH_RC_INTPROGERR;
#if 0
				if (! ServR::changePermissions(dirNames[i],
						"AESADMG",
						GENERIC_ALL,
						"set",
						SUB_CONTAINERS_AND_OBJECTS_INHERIT))
				{
					returnCode = AES_CDH_RC_INTPROGERR;
				}

				if (! ServR::changePermissions(dirNames[i],
						"CDHFTPUSRG",
						GENERIC_ALL,
						"set",
						SUB_CONTAINERS_AND_OBJECTS_INHERIT))
				{
					returnCode =  AES_CDH_RC_INTPROGERR;
				}
#endif
			}
		ACE_OS::umask(mask);
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:setFTPPermissions
=================================================================== */
//##ModelId=3E7EBE8D030A
AES_CDH_ResultCode FTPRV2::setFTPPermissions()
{
#if 0 // we are not going to set the file permissions
   string destPath = this->getVDDestPath();
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
   bool bSuppressDirCreate = false;

   if (suppressDirCreate == "yes")
	   bSuppressDirCreate = true;

   string dirNames[nrOfSubDirs + 1];
   const ACE_INT32 nSubDirs = (bSuppressDirCreate ? nrOfSubDirs-1 : nrOfSubDirs);
   ACE_INT32 i = 0;

   // If we are using suppress subdirectory creation, we can ignore the "root"
   // directory
   if (!bSuppressDirCreate)
   {
      dirNames[i++] = vdDestPath;
   }

   dirNames[i++] = vdDestPath + "/" + sendDirName;
   dirNames[i++] = vdDestPath + "/" + deleteDirName;
   dirNames[i++] = vdDestPath + "/" + readyDirName;
   dirNames[i++] = vdDestPath + "/" + archiveDirName;

   for (i = 0; i < (nSubDirs+1); i++)
   {
      if (::access(dirNames[i].c_str(), 0) == 0)
      {
         //if (::GetFileAttributes(dirNames[i].c_str()) & FILE_ATTRIBUTE_DIRECTORY)
    	  ACE_stat stp;
    	  ACE_OS::stat(dirNames[i].c_str()),&stp);//
    	  if (S_ISDIR(stp.st_mode))
         {
            if (!ServR::clearAcl(dirNames[i]))
               returnCode = AES_CDH_RC_INTPROGERR;

            if (!ServR::changePermissions(dirNames[i],
                                          "AESADMG",
                                          GENERIC_ALL,
                                          "set",
                                          SUB_CONTAINERS_AND_OBJECTS_INHERIT))
            {
               returnCode = AES_CDH_RC_INTPROGERR;
            }

            if (!ServR::changePermissions(dirNames[i],
                                          "CDHFTPUSRG",
                                          GENERIC_ALL,
                                          "set",
                                          SUB_CONTAINERS_AND_OBJECTS_INHERIT))
            {
               returnCode =  AES_CDH_RC_INTPROGERR;
            }
         }
         else
         {
            returnCode = AES_CDH_RC_INTPROGERR;
            break;
         }
      }
      else
      {
         returnCode = AES_CDH_RC_INTPROGERR;
         break;
      }
   }
#endif
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
   return returnCode;
}

/*===================================================================
   ROUTINE:sendFileResponding
=================================================================== */
//##ModelId=3DE49B4501B2
AES_CDH_ResultCode FTPRV2::sendFileResponding(const string& fileName,
                                              const string& remoteSubDirName,
                                              const string& newFileName,
                                              const bool moveFile)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string remoteFileName = "";
	string outputfilepath = "";
	string inputFile = fileName;

	string mov_f = "false";
	if (moveFile)
		mov_f = "true";

	AES_CDH_LOG(LOG_LEVEL_DEBUG, "FTPRV2::sendFileResponding() fileName         = %s",fileName.c_str());
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "FTPRV2::sendFileResponding() remoteSubDirName = %s",remoteSubDirName.c_str());
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "FTPRV2::sendFileResponding() newFileName      = %s",newFileName.c_str());
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "FTPRV2::sendFileResponding() moveFile         = %s",mov_f.c_str());
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "FTPRV2::sendFileResponding() streamId_        = %s",streamId_.c_str());

	if (keep)
		returnCode = keep->checkConnection();
	else
		cleanUpFtpDeleteDir();

	isTimeStamp = false;        // INGO3 GOH qabulfg 010925

	if (newFileName == "")
	{
		string::size_type pos = fileName.find_last_of("\\/:");

		// If '/' or '\' or ':' is not found then use whole filename
		if (pos == string::npos)
		{
			remoteFileName = fileName;
		}
		else
		{
			remoteFileName = fileName.substr(pos + 1);
		}
	}
	else
	{
		if (streamId_.length() > 0) {
			remoteFileName = streamId_ + "+";
		}

		remoteFileName += newFileName;
	}

	if (remoteSubDirName != "")
	{
		remoteFileName = remoteSubDirName + "-" + remoteFileName;
	}

	AES_CDH_TRACE_MESSAGE("remoteFileName = %s",remoteFileName.c_str());

	outputfilepath = vdDestPath;
	outputfilepath += "/" + readyDirName;

	// INGO3 GOH qabulfg start 010925
	// Time stamp is added to filename if filename is alerady used
	bool bOverWrite = false;
	if (overWrite == "yes")
	{
		bOverWrite = true;
	}

	// HD80596 start
	if (badFTPPermissions)
	{
		if (this->setFTPPermissions() == AES_CDH_RC_OK)
			badFTPPermissions = false;
	}
	// HD80596 end

	if (bOverWrite)
	{
		if (moveFile == true)
		{
			returnCode = rMoveFile(inputFile, outputfilepath, remoteFileName);
		}
		else
		{
			// Copy the file to the FTP-area for this destination
			returnCode = rCopyFile(inputFile, outputfilepath, remoteFileName, false);
		}

		// If the file should be notified, put info about that
		// file in the database before returning to the caller
		if (isFileNotification == true && returnCode == AES_CDH_RC_OK)
		{
			sendToDataBase(remoteFileName);
		}

		if (returnCode == AES_CDH_RC_OK)
			badFTPPermissions = false;

		return returnCode;
	}
	else
	{
		// Overwrite should not be used.
		// Check if the file already exists
		if (isLocalFile(outputfilepath + "/" + remoteFileName))
		{
			// File exists already, add timestamp
			bool done = false;
			ACE_INT32 count = 0;
			while (!done && (count <= 100))
			{
				stepFileName(remoteFileName, 0);
				if (!isLocalFile(outputfilepath + "/" + remoteFileName))
				{
					done = true;
				}
				count++;
			}
		}

		if (moveFile == true)
		{
			returnCode = rMoveFile(inputFile, outputfilepath, remoteFileName);
		}
		else
		{
			returnCode = rCopyFile(inputFile, outputfilepath, remoteFileName, bOverWrite);
			// INGO3 GOH qabulfg end
		}

		// If the file should be notified, put info about the file
		// in the database before returning
		if (isFileNotification == true && returnCode == AES_CDH_RC_OK)
		{
			sendToDataBase(remoteFileName);
		}

		if (returnCode == AES_CDH_RC_OK)
			badFTPPermissions = false;

		return returnCode;
	}

	return returnCode;
}
  
/*===================================================================
   ROUTINE:sendToDataBase
=================================================================== */
//##ModelId=3DE49B450126
bool FTPRV2::sendToDataBase(const std::string &fileName)
{
	//   AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	std::string dbFileName("");
	ACE_INT32 pos = fileName.find_last_of("/");
	if (pos != -1)
	{
		dbFileName.assign( fileName.substr(pos + 1) );
	}
	else
	{
		dbFileName.assign(fileName);
	}

	// Create paths for the database
	std::string fullPath  = vdDestPath + "/" + readyDirName;
	std::string storePath = this->destinationName + "/" + readyDirName;

	ACE_INT32 errors = 0;
	std::string fileSizePath = vdDestPath + "/" + readyDirName;
	ACE_UINT64 fSize = getFileSize(fileSizePath, dbFileName, errors);

	if (errors != 0)
	{
		// Error(s) occurred
	}

	ACE_TCHAR fileSize[20];
	ACE_OS::sprintf(fileSize, "%lu", fSize);

	// Create the record to be stored
	NotificationRecord *rec = new NotificationRecord;
	rec->setStoreFileName(dbFileName);
	rec->setFileSize(fileSize);
	rec->setMessageNumber(1);

	ACE_UINT32 tmpAge = atoi(fileMaxAge.c_str());

	CDHCriticalSection::instance().enter();
	db = DataBase::instance();
	bool bSuccess = db->add(destinationName,
			dbFileName,
			fileSize,
			rec,
			clusterIPAddress,
			storePath,
			fullPath,
			true,
			tmpAge);

	CDHCriticalSection::instance().leave();

	return bSuccess;
}

/*===================================================================
   ROUTINE:isLocalFile
=================================================================== */
//  Returns true if fileName exists
//******************************************************************************
//##ModelId=3DE49B450144
bool FTPRV2::isLocalFile(const string &fileName) const
{
	ACE_stat statBuffer;
	int ret = ACE_OS::stat( fileName.c_str() , &statBuffer );
	if( ret == 0 )
	{
		return true;
	}
	return false;

}

/*===================================================================
   ROUTINE:sendDirectoryResponding
=================================================================== */
//##ModelId=3DE49B4501E4
AES_CDH_ResultCode FTPRV2::sendDirectoryResponding(const string& dirName,
		const string& remoteSubDirName,
		const string& newDirName)
{

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	if (keep)
		returnCode = keep->checkConnection();
	else
		cleanUpFtpDeleteDir();

	isTimeStamp = false;        // INGO3 GOH qabulfg 010926

	try
	{
		if (newDirName == "")
		{
			string::size_type pos = dirName.find_last_of("\\/:");

			// If '/' or '\' or ':' is not found then copy whole filename
			if (pos == string::npos)
				aRemoteDirName = dirName;
			else
				aRemoteDirName = dirName.substr(pos + 1);
		}
		else
		{
			aRemoteDirName = newDirName;
		}

		if (remoteSubDirName != "")
		{
			aRemoteDirName = remoteSubDirName + "-" + aRemoteDirName;
		}

		directoryLevel = 0;

		string outputfilepath = vdDestPath;
		outputfilepath += "/" + readyDirName; // qabhall

		// Time stamp is added to filename if filename is alerady used
		// Overwrite should not be used. Check if the file already exists
		if (isLocalFile(outputfilepath + "/" + aRemoteDirName))
		{
			// File exists already, add timestamp
			bool done = false;
			ACE_INT32 count = 0;
			while (!done && (count <= 100))
			{
				stepFileName(aRemoteDirName, 0);
				if (!isLocalFile(outputfilepath + "/" + aRemoteDirName))
				{
					done = true;
				}
				count++;
			}
		}
		returnCode = doSendDirResponding(dirName, aRemoteDirName);

		// If the file should be notified, put info about that
		// file in the database before returning to the caller
		if (isFileNotification == true && returnCode == AES_CDH_RC_OK)
		{
			sendToDataBase(aRemoteDirName);
		}
	}
	catch (AES_CDH_ResultCode rc)
	{
		returnCode = rc;
	}

	/*
   catch(...)
   {
      Event::report(AES_CDH_intProgFault, "INTERNAL PROGRAM ERROR",
                    "Internal program error in CDHFTP::sendDirectoryResponding", "-");

      returnCode = AES_CDH_RC_INTPROGERR;
   }
	 */

	return returnCode;
}

/*===================================================================
   ROUTINE:getFileSize
=================================================================== */
//##ModelId=3DE49B4500EA
ACE_UINT64 FTPRV2::getFileSize(const string &path, const string &fName, ACE_INT32 &errors)
{
	//
	// path:   this is a path to a file or a directory
	//
	// errors: should be zero, if not the value indicates the number
	//         of errors that occurred while processing
	//

	ACE_HANDLE fh;
	// ACE_UINT64 fSize = 0;
	string fileName("");
	string workPath("");
	string searchPattern = path;

	if (fName == "")
	{
	}
	else
		searchPattern += "/" + fName;


	ACE_UINT64 totSize = 0;
	ACE_UINT64 recFileSize = 0;

	DIR* pDir = ACE_OS::opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if ( pDir == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "FTPRV2::getFileSize(), opendir failed, errno = %d", errno);
		errors++;
	}
	if(pDir != NULL)
	{
		struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
		bool bIsDir;

		if (!pEntry)
			errors++;

		while (pEntry!= NULL)
		{
			fileName.assign(path).append("/").append(pEntry->d_name);
			struct stat buf;
			stat(fileName.c_str(), &buf); // need to modify name??
			bool bIsDir;
			bIsDir= S_ISDIR(buf.st_mode);

			if (!bIsDir)
			{
				if ( (fh = ACE_OS::open(fileName.c_str(), O_RDONLY, S_IREAD)) != -1)
				{
					totSize += ACE_OS::filesize(fh);
					int t = ACE_OS::close(fh);
					if(t == -1)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "FTPRV2::getFileSize(), close failed");

						errors++;
					}
				}
				else
				{
					// What now ??  Spin another round and get a bad result ??
					// At least indicate by incrementing the error flag
					errors++;
				}
			}
			else
			{
				if (! ((ACE_OS::strcmp( pEntry->d_name,".") == 0)  || (ACE_OS::strcmp( pEntry->d_name,"..") == 0)))
				{
					// A sub directory found. Make a recursive call to getFileSize
					recFileSize += getFileSize(fileName.append("/"), "", errors);
				}
			}
			//bMore = FindNextFile(hFind, &finddata);
			pEntry = ACE_OS::readdir(pDir);
		}
		closedir(pDir);
	}

	return recFileSize + totSize;
}

/*===================================================================
   ROUTINE:rCreateDirectory
=================================================================== */
//##ModelId=3DE49B45028E
void FTPRV2::rCreateDirectory(string name)
{
	if (ACE_OS::mkdir(name.c_str(),NULL) != -1)
	{
		//    returnCode = AES_CDH_RC_OK;
	}
	else
	{
		Event::report(AES_CDH_remoteFileFault,
				"FTP RESPONING ERROR",           // AES_CDH_remoteFileFault, "FTP ERROR" ??
				"Unable to create directory : ",
				name.c_str());

		throw AES_CDH_RC_SENDERR;                                   // AES_CDH_RC_SENDERR ??
	}
}

/*===================================================================
   ROUTINE:rCopyFile
=================================================================== */
//##ModelId=3DE49B450248
AES_CDH_ResultCode FTPRV2::rCopyFile(const string inPath,
		const string outPath,
		const string fName,
		const bool noOwrWrite)
{
	(void) noOwrWrite;
	//bool stopFileTransfer = false;
	string tmpStr = outPath + "/" + fName;
	string outputfilepath = outPath;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	/* Replace code below with this block if using hard links */
	//  returnCode = linkFileToCdhFtp(inPath, fName, outputfilepath, ! noOwrWrite);
	//  return returnCode;
	/* */

	if ((AES_GCC_Util::copyFile(inPath.c_str(), tmpStr.c_str()))!=-1)
		// if (CopyFile(inPath.c_str(), tmpStr.c_str(), noOwrWrite) != 0)
	{
		returnCode = AES_CDH_RC_OK;
		//file copied from inputfile to <aesapdata>cdh\ftp\<destName>\Ready\outputfile
	}
	else
	{
		ACE_UINT64 error = ACE_OS::last_error();
		if (( error == EEXIST ) || ( error == ENOENT ))
		{
			Event::report(AES_CDH_fileErrFault,
					"FILE ERROR",
					"File not found : ",
					inPath);

			// input file not found
			returnCode = AES_CDH_RC_SENDERR;
		}
		else
		{
			Event::report(AES_CDH_fileCopyFault,
					"FILE COPY ERROR",
					ServR::NTErrorText(),
					"Failed to copy file to CDH FTP area: " + inPath + " to " + outPath);

			returnCode = AES_CDH_RC_SENDERR;
		}
		// Failed to copy the file!!
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:rMoveFile
=================================================================== */
//##ModelId=3E32904002CA
AES_CDH_ResultCode FTPRV2::rMoveFile(const string &inPath,
		const string &outPath,
		const string &remoteFileName)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	string targetPath = outPath + "/" + remoteFileName;

	if (ACE_OS::rename(inPath.c_str(), targetPath.c_str())!= -1)
	{
		returnCode = AES_CDH_RC_OK;
	}
	else
	{
		ACE_UINT64 error = ACE_OS::last_error();
		if (error == EEXIST || error == ENOENT)
		{
			Event::report(AES_CDH_fileErrFault,
					"FILE ERROR",
					"File not found : ",
					inPath);

			returnCode = AES_CDH_RC_SENDERR;
		}
		else
		{
			Event::report(AES_CDH_fileErrFault,
					"FILE ERROR",
					ServR::NTErrorText(),
					"Failed to move file to CDH FTP area: " + inPath + " to " + outPath);

			returnCode = AES_CDH_RC_SENDERR;
		}

		// HD80596 start
		if (error == EACCES)
			badFTPPermissions = true;
		// HD80596 end
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:doSendDirResponding
=================================================================== */
//##ModelId=3DE49B450220
AES_CDH_ResultCode FTPRV2::doSendDirResponding(string path, string outPath)
{
	bool bIsDir;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string searchPattern = path ;
	string intOutPath = outPath;
	DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if ( !pDir)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "FTPRV2::doSendDirResponding(), opendir failed for path, errno = %d", errno);
		returnCode = AES_CDH_RC_ERROR;
		return returnCode;
	}

	struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
	directoryLevel += 1;

	while (pEntry!=NULL)
	{
		if (directoryLevel == 1)
		{
			remoteFullPath = vdDestPath + "/" + readyDirName + "/" + aRemoteDirName;
			rCreateDirectory(remoteFullPath);
			intOutPath = remoteFullPath;
			directoryLevel += 1;
		}
		string filePath = path + "/" + string(pEntry->d_name);
		struct stat buf;
		stat(filePath.c_str(), &buf);
		bIsDir =S_ISDIR(buf.st_mode);
		if (!bIsDir)
		{
			returnCode = rCopyFile(path + "/" + pEntry->d_name,
					intOutPath , pEntry->d_name, true);
			if(returnCode != AES_CDH_RC_OK)
				break;
		}
		else
		{
			if (! ((ACE_OS::strcmp(pEntry->d_name,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name,"..") == 0)))
			{
				// A sub directory found. Create it and change to it on remote site.
				// Make a recursive call to sendDir in order to send files and/or create new sub directories.
				rCreateDirectory(intOutPath + "/" + pEntry->d_name);
				returnCode = doSendDirResponding(path + "/" + pEntry->d_name,
						intOutPath + "/" + pEntry->d_name);
				if(returnCode != AES_CDH_RC_OK)
					break;
			}
		}
		pEntry = ACE_OS::readdir(pDir);
	}
	if (pDir != NULL)
		closedir(pDir);

	if(returnCode == AES_CDH_RC_OK)
		directoryLevel -= 1;

	return returnCode;
}

/*===================================================================
   ROUTINE:getAgentType
=================================================================== */
//##ModelId=3DF9AF710319
TransferAgent::TransferAgentAttributes FTPRV2::getAgentType()
{
	return TransferAgent::FILEBACKUP;
}

/*===================================================================
   ROUTINE:
=================================================================== */
//##ModelId=3E3E75EE0217
std::string FTPRV2::getVDDestPath()
{
	return vdDestPath;
}

/*===================================================================
   ROUTINE:getVDPath
=================================================================== */
//##ModelId=3E6DF453026F
std::string FTPRV2::getVDPath()
{
	return vdPath;
}

/*===================================================================
   ROUTINE:readVDPath
=================================================================== */
//##ModelId=3E68A7A40126
AES_CDH_ResultCode FTPRV2::readVDPath(std::string& path)
{

	string dataTransferDir("");
	if( ServR::getDataTransferFolderPath(dataTransferDir))
	{
		path = dataTransferDir;
	}
	else
	{
		return AES_CDH_RC_NOALIAS;
	}

	return AES_CDH_RC_OK;
}
