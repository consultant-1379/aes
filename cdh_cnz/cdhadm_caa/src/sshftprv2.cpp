/******************************************************************************/
/**
@file sshftprv2.cpp

Class method implementation for sshftprv2.h

DESCRIPTION
This class supports sending files with responding SFTP protocol GOH V2.

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
#include <sshftprv2.h>
#include <event.h>
#include <servr.h>
#include <cdhcriticalsection.h>
#include <time.h>
#include <fstream>
#include<fcntl.h>
#include <parameter.h>
#include <ace/Get_Opt.h>
#include <aes_gcc_util.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;

/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
static const string sendDirName    = "Send";
static const string deleteDirName  = "Delete";
static const string readyDirName   = "Ready";
static const string archiveDirName = "Archive";
//Start - HU18195
static const string stagedDirName = "Staged";
//End - HU18195
static const ACE_INT32 nrOfSubDirs       = 4;
static const mode_t dir_full_perms = 0777; 
static const mode_t temp_mask = 0000;
AES_CDH_TRACE_DEFINE(AES_CDH_SSHFTPRV2);

/*===================================================================
   ROUTINE:SSHFTPRV2
=================================================================== */
SSHFTPRV2::SSHFTPRV2(): vdDestPath(""),
		vdPath(""),
		eventType(""),
		respondTime(""),
		clusterIPAddress(""),
		notificationAddress(""),
		notificationPortNr(""),
		suppressDirCreate(""),
		removeTQFilePrefix(""),
		isFileNotification(false),
		badSSHFTPPermissions(false),
		keep(NULL),
		keepTime(-1),
		db(NULL)
{
}
/*===================================================================
   ROUTINE:~SSHFTPRV2
=================================================================== */
SSHFTPRV2::~SSHFTPRV2()
{
	if (keep)
	{
		delete keep;
	}
}

/*===================================================================
   ROUTINE:define
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::define(ACE_INT32& argc,ACE_TCHAR* argv[],const bool define,const bool recovery)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	returnCode = parseFTP(argc, argv);

	if (returnCode == AES_CDH_RC_OK) 
	{
		returnCode = parseSSHFTPR(argc, argv, define, recovery);
	}

	cleanUpArgv(argc, argv, 1);

	if (returnCode == AES_CDH_RC_OK && argc != 1)
	{
		if (!recovery)
		{
			returnCode = AES_CDH_RC_INCUSAGE;
		}
	}
	else if (returnCode == AES_CDH_RC_PHYSFILEERR)		//For SSHconfig file
	{
		return returnCode;
	}

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
		{
			return AES_CDH_RC_DESTWITHSAMEOPTEXIST;	
		}
	}

	if (returnCode == AES_CDH_RC_OK)
	{
		returnCode = createSSHFtpDestDir();
	}
	AES_CDH_LOG(LOG_LEVEL_INFO, "vdDest path in define is %s", vdDestPath.c_str());
	AES_CDH_TRACE_MESSAGE("vdDest path in define is %s", vdDestPath.c_str());

	if (returnCode == AES_CDH_RC_OK && keepTime > -1)
	{
		if (keep)
		{
			delete keep;
		}

		keep = new FTPRV2Keep;
		if (!keep)
		{
			return AES_CDH_RC_INTPROGERR;
		}

		returnCode = keep->define(vdDestPath.c_str(), keepTime, recovery);


		if (returnCode != AES_CDH_RC_OK)
		{
			this->remove();
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}

/*===================================================================
   ROUTINE:remove
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::remove(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	bool dirsAreEmpty = true;
	bool bOtherDestExists = false;
	bool bSuppressDirCreate = false;

	if (!vdDestPath.empty())
	{
		if (suppressDirCreate == "yes")
		{
			bSuppressDirCreate = true;
		}

		cleanUpSSHFtpDeleteDir();


		// Check if another destination uses the same virtual directory and
		// when the -g option set to "yes"
		if (bSuppressDirCreate)
		{
			bOtherDestExists = ServR::checkOtherRespDestExists(this->destinationName,
					vdPath);
		}

		if (!bOtherDestExists)
		{
			// Check if all directories are empty before deleting any.
			//Start - HU18195
                        //const ACE_INT32 nNumOfDirs = (bSuppressDirCreate ? nrOfSubDirs : nrOfSubDirs+1);
                        ACE_INT32 nNumOfDirs = (bSuppressDirCreate ? nrOfSubDirs : nrOfSubDirs+1);
                        nNumOfDirs = (isFileNotification ? nNumOfDirs : nNumOfDirs+1);
                        //End - HU18195
			ACE_INT32 i = 0, saveIdx;
			//Start - HU18195

                        //string dirNames[nrOfSubDirs+1];
                        string dirNames[nrOfSubDirs+2];
                        //End - HU18195

			dirNames[i++] = vdDestPath + "/" + sendDirName;
			dirNames[i++] = vdDestPath + "/" + deleteDirName;
			dirNames[i++] = vdDestPath + "/" + readyDirName;
			dirNames[i++] = vdDestPath + "/" + archiveDirName;
			//Start - HU18195
			if(!isFileNotification)
                        {
                                dirNames[i++] = vdDestPath + "/" + stagedDirName;
                        }
                        //End - HU18195
			// If we are using suppress subdirectory creation, we can ignore the "root"
			// directory
			if (!bSuppressDirCreate)
			{
				dirNames[i++] = vdDestPath;
			}

			for (i = 0; i < nrOfSubDirs; i++)
			{
				string searchPattern = dirNames[i] ;
				//hFind = FindFirstFile(searchPattern.c_str(), &finddata);
				DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
				if( pDir == NULL )
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to open %s directory",searchPattern.c_str());
					AES_CDH_TRACE_MESSAGE("Not able to open %s directory",searchPattern.c_str());
				}
				else
				{
					struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
					while (pEntry)
					{
						if (! ((ACE_OS::strcmp(pEntry->d_name,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name,"..") == 0)))
						{
							dirsAreEmpty = false;
							saveIdx = i;
							break;
						}
						pEntry = ACE_OS::readdir(pDir);
					}
					if(pDir != NULL)
						closedir(pDir);


				}
			}

			// Check if a user is standing in the path with some tool. If
			// the "root" can not be renamed, he may do just that. Either way,
			// removing the ftp-area is generally not possible if the "root"
			// can not be renamed.
			for (i = 0; i < nNumOfDirs; i++)
			{
				if ( (returnCode = IsDirectoryLocked(dirNames[i])) != AES_CDH_RC_OK)
				{
					return returnCode;
				}
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
							{
								returnCode = keepResultCode;
							}
						}
					}

					if (ACE_OS::rmdir(dirNames[i].c_str()) == -1)
					{
						string newdr("");
						if (AES_GCC_Util::datapath_trn(dirNames[i],AES_DATA_PATH,newdr)==false)
						{
							AES_CDH_LOG(LOG_LEVEL_INFO, "Directory name sent for event %s",newdr.c_str());
							AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
							newdr = "Not Available";
						}
						Event::report(AES_CDH_dataAreaFault,
								"DATA AREA ERROR",
								ServR::NTErrorText(),
								"Directory could not be removed: " + newdr);

						returnCode = AES_CDH_RC_DATAAREAERR;
					}
				}

				if (keep)
				{
					if (nNumOfDirs == nrOfSubDirs && returnCode == AES_CDH_RC_OK)
					{
						if ((keepResultCode = keep->remove()) != AES_CDH_RC_OK)
						{
							returnCode = keepResultCode;
						}
					}
				}
			}
			else
			{
				string newdr("");
				if (AES_GCC_Util::datapath_trn( dirNames[saveIdx],AES_DATA_PATH,newdr)==false)
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "Directory name sent for event %s",newdr.c_str());
					AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
					newdr = "Not Available";
				}
				Event::report(AES_CDH_dataAreaFault,
						"DATA AREA ERROR",
						"Subdirectories not empty",
						"Directory could not be removed: " +newdr);

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
	AES_CDH_TRACE_MESSAGE("Leaving");

	return returnCode;
}


/*===================================================================
   ROUTINE:getAttributes
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::getAttributes(string& transferType,
		vector<string>& attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	transferType = transType;
	m_ftpAgentMx.acquire();
	if(NULL != ftpAgent)
	{
		SSHFTPRV2 * tempAgent = dynamic_cast<SSHFTPRV2 *>(ftpAgent);
		if (NULL != tempAgent)
			fetchAttrValues(tempAgent, attr);
	}
	else
	{
		fetchAttrValues(this,attr);
	}
	m_ftpAgentMx.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}	

void SSHFTPRV2::fetchAttrValues(SSHFTPRV2 * psshftprv2, vector<string>& attr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if (!((psshftprv2->connectType).empty()))
	{
		attr.push_back("-c");
		attr.push_back(psshftprv2->connectType);
	}

	if (!((psshftprv2->overWrite).empty()))
	{
		attr.push_back("-o");
		attr.push_back(psshftprv2->overWrite);
	}

	if (!((psshftprv2->eventType).empty()))
	{
		attr.push_back("-e");
		attr.push_back(psshftprv2->eventType);
	}

	if (!((psshftprv2->notificationAddress).empty()))
	{
		attr.push_back("-f");
		attr.push_back(psshftprv2->notificationAddress);
	}

	if (!((psshftprv2->notificationPortNr).empty()))
	{
		attr.push_back("-x");
		attr.push_back(psshftprv2->notificationPortNr);
	}

	if (!((psshftprv2->respondTime).empty()))
	{
		attr.push_back((std::string)"-y");
		attr.push_back(psshftprv2->respondTime);
	}

	if (!((psshftprv2->suppressDirCreate).empty()))
	{
		attr.push_back("-g");
		attr.push_back(psshftprv2->suppressDirCreate);
	}

	if (!((psshftprv2->removeTQFilePrefix).empty()))
	{
		attr.push_back("-k");
		attr.push_back(psshftprv2->removeTQFilePrefix);
	}
	// Get the attributes for the KEEP functionality
	if (psshftprv2->keep)
	{
		(psshftprv2->keep)->getAttributes(attr);
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}
	
/*===================================================================
   ROUTINE:getSingleAttr
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::getSingleAttr(const ACE_TCHAR* opt, void* value)
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
AES_CDH_ResultCode SSHFTPRV2::sendFile(const string& fileName,
		const string& remoteSubDirName,
		const string& newFileName,
		AES_CDH_DestinationSet::transferMode trMode)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	(void)trMode;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	cdhchSemaphore->acquire();

	aFileName = fileName;
	aNewMainFileName = remoteSubDirName;
	aNewSubFileName = newFileName;

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
	cdhchSemaphore->release(1);

	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}  


/*===================================================================
   ROUTINE:changeParameters
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::changeParameters()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	SSHFTPRV2* p = dynamic_cast<SSHFTPRV2 *>(ftpAgent);
	if (NULL != p)
	{
		this->notificationAddress = p->notificationAddress;
		this->notificationPortNr  = p->notificationPortNr;
		this->respondTime         = p->respondTime;
		this->eventType           = p->eventType;
		this->suppressDirCreate   = p->suppressDirCreate;
		this->removeTQFilePrefix  = p->removeTQFilePrefix;

		// Shared attributes from FTPV2Agent
		this->connectType  = p->connectType;
		this->overWrite    = p->overWrite;
	}
	AES_CDH_ResultCode  returnCode = readVDPath(vdPath);

	if (returnCode != AES_CDH_RC_OK)
		return returnCode;

	AES_CDH_TRACE_MESSAGE("vdDestPath before is %s",vdDestPath.c_str());
	if (suppressDirCreate == "yes")
	{
		this->vdDestPath = vdPath;
	}
	else
	{
		this->vdDestPath.assign(vdPath).append("/").append(this->destinationName);
	}
	AES_CDH_TRACE_MESSAGE("vdDestPath after is %s",vdDestPath.c_str());
	// Attributes for KEEP
	if (keep && p->keep)
	{
		keep->changeParameters(p->keep);
	}

	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:sendDirectory
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::sendDirectory(const string& dirName,
		const string& remoteSubDirName,
		const string& newDirName,
		AES_CDH_DestinationSet::transferMode trMode,
		const string& fileMask)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)trMode;
	(void)fileMask;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	cdhchSemaphore->acquire();

	aFileName = dirName;
	aNewMainFileName = remoteSubDirName;
	aNewSubFileName = newDirName;

	if (removeTQFilePrefix == "yes")
	{
		returnCode = sendDirectoryResponding(dirName, "", newDirName);
	}
	else
	{
		returnCode = sendDirectoryResponding(dirName, remoteSubDirName, newDirName);
	}
	m_ftpAgentMx.acquire();
	if (NULL != ftpAgent)
	{
		// Change of parameters using cdhch requested
		this->changeParameters();
		delete ftpAgent;
		ftpAgent = NULL;
	}
	m_ftpAgentMx.release();
	cdhchSemaphore->release(1);
	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}

/*===================================================================
   ROUTINE:checkConnection
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::checkConnection(void)     
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	if (keep)
		returnCode = keep->checkConnection();
	else
		cleanUpSSHFtpDeleteDir();

	return returnCode;
}

/*===================================================================
   ROUTINE:changeAttr
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::changeAttr(TransferAgent *agent)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	if (cdhchSemaphore->tryacquire() < 0)
	{
		AES_CDH_TRACE_MESSAGE("CANNOT ACQUIRE SEMAPHORE BECAUSE TRAFFIC ONGOING");
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
		//LOCK ACQUIRED
		SSHFTPRV2* p = dynamic_cast<SSHFTPRV2*>(agent);
		if (NULL != p)
		{
			this->notificationAddress = p->notificationAddress;
			this->notificationPortNr  = p->notificationPortNr;
			this->respondTime         = p->respondTime;
			this->eventType           = p->eventType;
			this->suppressDirCreate   = p->suppressDirCreate;
			this->removeTQFilePrefix  = p->removeTQFilePrefix;

			// Shared attributes from FTPV2Agent
			this->connectType  = p->connectType;
			this->overWrite    = p->overWrite;
		}
		clusterIPAddress.assign(ServR::getClusterIP(notificationAddress));
		AES_CDH_TRACE_MESSAGE("clusterIPAddress is %s",clusterIPAddress.c_str());
		AES_CDH_TRACE_MESSAGE("vdDestPath before is %s", vdDestPath.c_str());
		if (suppressDirCreate == "yes" )
		{
			this->vdDestPath = vdPath;
		}
		else
		{
			this->vdDestPath.assign(vdPath).append("/").append(this->destinationName);
		}
		AES_CDH_TRACE_MESSAGE("vdDestPath after is %s", vdDestPath.c_str());

		// Change attributes for KEEP
		if (NULL != p)
		{
			if (keep && p->keep)
			{
				keep->changeParameters(p->keep);
			}
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
AES_CDH_ResultCode SSHFTPRV2::IsDirectoryLocked(const string &path)
{
	if (ACE_OS::rename(path.c_str(), (path + ".tmp").c_str()) == -1)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn( path,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
			newdr = "Not Available";
		}
		// Error renaming directory! Stop here, and report error to caller
		Event::report(AES_CDH_dataAreaFault,
				"DATA AREA ERROR",
				"FTP directory busy?",
				"Directory could not be removed: " + newdr);

		return AES_CDH_RC_DATAAREAERR;
	}

	// The directory has been renamed. Undo now
	ACE_OS::rename((path + ".tmp").c_str(), path.c_str());

	return AES_CDH_RC_OK;
}
/*===================================================================
   ROUTINE:parseSSHFTPR
=================================================================== */

AES_CDH_ResultCode SSHFTPRV2::parseSSHFTPR(ACE_INT32& argc,
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

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string temp;
	optind = 1;

	ACE_Get_Opt getopt(argc, argv, "b:e:f:g:k:x:y:");
	while ((opt = getopt()) != EOF)
	{
		switch (opt)
		{
		case 'b':
		{
			// KEEP functionality is enabled. Keep time option
			if (argv[getopt.optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			temp = getopt.optarg;
			if (temp.find_first_not_of("0123456789") != string::npos)
				return AES_CDH_RC_UNREAS;

			keepTime = atol(getopt.optarg);
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

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			bFlag++;
			break;
		}
		case 'e':
		{
			if (argv[getopt.optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			eventType = getopt.optarg;

			upperToLower(eventType);
			if (eventType != "alarm" && eventType !="event" && eventType != "")
			{
				return AES_CDH_RC_INCUSAGE;
			}

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			eFlag++;
			break;
		}

		case 'f':
		{
			if (argv[getopt.optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			notificationAddress = getopt.optarg;

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			fFlag++;
			break;
		}

		case 'g':
		{
			// Suppress creation of directory with TQ name on remote system
			if (argv[getopt.optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			suppressDirCreate = getopt.optarg;
			upperToLower(suppressDirCreate);

			// "no" is default
			if (suppressDirCreate != "yes" && suppressDirCreate != "no")
			{
				return AES_CDH_RC_INCUSAGE;
			}

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;

			gFlag++;
			break;
		}
		case 'k':
		{
			// Remove file prefix with TQ name from the filenames
			if (argv[getopt.optind-1][0] == '-')
				return AES_CDH_RC_INCUSAGE;

			removeTQFilePrefix = getopt.optarg;
			upperToLower(removeTQFilePrefix);

			// "no" is default
			if (removeTQFilePrefix != "yes" && removeTQFilePrefix != "no")
			{
				return AES_CDH_RC_INCUSAGE;
			}

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;

			kFlag++;
			break;
		}

		case 'x':
		{
			if (argv[getopt.optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			notificationPortNr = getopt.optarg;
			isFileNotification = true;

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			xFlag++;
			break;
		}

		case 'y':
		{
			if (argv[getopt.optind-1][0] == '-')
			{
				return AES_CDH_RC_INCUSAGE;
			}

			respondTime = getopt.optarg;

			delete [] argv[getopt.optind-2];
			argv[getopt.optind-2] = NULL;
			delete [] argv[getopt.optind-1];
			argv[getopt.optind-1] = NULL;
			yFlag++;
			break;
		}
		case '?':
		{
			if ((getopt.optind + 1) < argc)
			{
				if (argv[getopt.optind][0] != '-' &&
						argv[getopt.optind + 1][0] == '-')
					getopt.optind++;
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
			(bFlag > 1) )
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

	cleanUpArgv(argc, argv, getopt.optind);

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
bool SSHFTPRV2::createDataBaseEntry()
{
	string destn = this->destinationName;
	//upperToLower(destn);
	std::string fullPath;
	std::string storePath;

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

	CDHCriticalSection::instance().enter();
	DataBase *db = DataBase::instance();
	if (db == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "db == NULL");
		AES_CDH_TRACE_MESSAGE("db == NULL");
		CDHCriticalSection::instance().leave();
		return false;
	}

	AES_CDH_TRACE_MESSAGE("destinationName = %s storePath = %s fullPath = %s clusterIPAddress = %s",this->destinationName.c_str(), storePath.c_str(), fullPath.c_str(), clusterIPAddress.c_str());

	bool ret = db->createDestinationEntry(this->destinationName, storePath, fullPath, clusterIPAddress);
	AES_CDH_TRACE_MESSAGE("db->createDestinationEntry return value = %d",ret);
	CDHCriticalSection::instance().leave();

	return true;
}

/*===================================================================
   ROUTINE:doDelete
=================================================================== */
void SSHFTPRV2::doDelete(string path)
{
	//bool bIsDir;
	string searchPattern = path;
	string fileN;
	DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if( pDir == NULL )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to open %s directory as %d",searchPattern.c_str (), errno);
		AES_CDH_TRACE_MESSAGE("Not able to open %s directory as %d",searchPattern.c_str (), errno);
		return;
	}
	struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
	while(pEntry!=NULL)
	{
		fileN = path + "/" + string(pEntry->d_name);
		struct stat buf;
		stat(fileN.c_str(), &buf);
		bool bIsDir = S_ISDIR(buf.st_mode);
		if (!bIsDir)
		{
			//fileN = path + "/" + pEntry->d_name;
			if (::remove(fileN.c_str()) != 0)
			{
				string newdr("");
				if (AES_GCC_Util::datapath_trn(fileN,AES_DATA_PATH,newdr)==false)
				{
					AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",fileN.c_str());
					newdr = "Not Available";
				}
				Event::report(AES_CDH_fileRemoveFault,
						"FILE REMOVE ERROR",
						ServR::NTErrorText(),
						"Failed to remove output file: " + newdr);
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
					string newdr("");
					if (AES_GCC_Util::datapath_trn(fileN,AES_DATA_PATH,newdr)==false)
					{
						AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",fileN.c_str());
						newdr = "Not Available";
					}
					Event::report(AES_CDH_fileRemoveFault,
							"FILE REMOVE ERROR",
							ServR::NTErrorText(),
							"Failed to remove output directory: " + newdr);
				}
			}
		}

		pEntry = ACE_OS::readdir(pDir);
	}

	if(pDir != NULL)
		closedir(pDir);

}

/*===================================================================
   ROUTINE:cleanUpSSHFtpDeleteDir
=================================================================== */
void SSHFTPRV2::cleanUpSSHFtpDeleteDir(void)
{
	if (connectType == "r" && vdDestPath != "")
	{
		string path = vdDestPath + "/" + deleteDirName;
		doDelete(path);
	}
}

/*===================================================================
   ROUTINE:createSSHFtpDestDir
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::createSSHFtpDestDir(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	bool bSuppressDirCreate = false;

	if (suppressDirCreate == "yes")
		bSuppressDirCreate = true;

	string destn = destinationName;

	if (bSuppressDirCreate)
		vdDestPath = vdPath;
	else
		vdDestPath.assign(vdPath).append("/").append(destn);

	//Start - HU18195
        //ACE_INT32 nSubDirs = (bSuppressDirCreate ? nrOfSubDirs-1 : nrOfSubDirs);
        ACE_INT32 nSubDirs = (bSuppressDirCreate ? nrOfSubDirs-1 : nrOfSubDirs);
        nSubDirs = (isFileNotification ? nSubDirs : nSubDirs+1);
        //string dirNames[nrOfSubDirs+1];
        string dirNames[nrOfSubDirs+2];
        //End - HU18195

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

	//Start - HU18195
        if(!isFileNotification)
        {
                dirNames[i++] = vdDestPath + "/" + stagedDirName;
        }
        //End - HU18195

	for (i = 0; i < (nSubDirs+1); i++)   //INGO3 GOH qabhall
	{	
		mode_t mask = ACE_OS::umask(temp_mask);
		if (ACE_OS::mkdir(dirNames[i].c_str(),dir_full_perms) == -1)
		{
			// Unsuccessful CreateDirectory. Check error code
			if (ACE_OS::last_error() == EEXIST)
			{
				// <virtual directory>\<destName> directory already exist, do nothing
			}
			else if (ACE_OS::last_error() == ENOENT)
			{
				string newdr("");
				if (AES_GCC_Util::datapath_trn(vdPath,AES_DATA_PATH,newdr)==false)
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Directory name sent for event %s",vdPath.c_str());
					AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",vdPath.c_str());
					newdr = "Not Available";
				}
				Event::report(AES_CDH_physFileFault, 
						"PHYSICAL FILE PROBLEM",
						ServR::NTErrorText(), 
						"Physical path to virtual directory does not exist: "
						+ newdr);

				returnCode = AES_CDH_RC_NOVIRTUALPATH;		
			}
			else
			{
				if (bSuppressDirCreate)
				{
					string newdr("");
					if (AES_GCC_Util::datapath_trn( dirNames[i],AES_DATA_PATH,newdr)==false)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Directory name sent for event %s",newdr.c_str());
						AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
						newdr = "Not Available";
					}

					Event::report(AES_CDH_dataAreaFault,
							"DATA AREA ERROR",
							ServR::NTErrorText(),
							"SSHFTP Directory could not be created: " 
							+ newdr);
				}
				else
				{
					string newdr("");
					if (AES_GCC_Util::datapath_trn( dirNames[i],AES_DATA_PATH,newdr)==false)
					{
						AES_CDH_LOG(LOG_LEVEL_INFO, "Directory name sent for event %s",newdr.c_str());
						AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
						newdr = "Not Available";
					}
					Event::report(AES_CDH_dataAreaFault,
							"DATA AREA ERROR",
							ServR::NTErrorText(),
							"SSHFTP/destname Directory could not be created: " 
							+ newdr);
				}

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


	AES_CDH_TRACE_MESSAGE("Out SSHFTPRV2::createSSHFtpDestDir");
	return returnCode;
}


/*===================================================================
   ROUTINE:setSSHFTPPermissions
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::setSSHFTPPermissions()
{
#if 0
	// change permissions are not allowed in linux
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
			if (::GetFileAttributes(dirNames[i].c_str()) & FILE_ATTRIBUTE_DIRECTORY)
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
AES_CDH_ResultCode SSHFTPRV2::sendFileResponding(const string& fileName,
		const string& remoteSubDirName,
		const string& newFileName,
		const bool moveFile)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	//Start - HU18195
        AES_CDH_LOG(LOG_LEVEL_INFO, "fileName = %s, remoteSubDirName = %s, newFileName = %s,moveFile = %d",fileName.c_str(),remoteSubDirName.c_str(),newFileName.c_str(),moveFile);
        //End - HU18195	

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string remoteFileName = "";
	string outputfilepath = "";
	string inputFile = fileName;
	
	//Start - HU18195
        string stagedFolderPath = "";
        string stagedFilePath = "";
        //End - HU18195 

	if (keep)
		returnCode = keep->checkConnection();
	else
		cleanUpSSHFtpDeleteDir();

	isTimeStamp = false;       

	if (newFileName == "")
	{
		string::size_type pos = fileName.find_last_of("\\/:");

		// If '/' or '\' or ':' is not found then use whole filename
		if (pos == string::npos)
		{
			remoteFileName = fileName;
			 //Start - HU18195
                        AES_CDH_LOG(LOG_LEVEL_INFO, "newFileName NULL - IF COND: remoteFileName = %s",remoteFileName.c_str());
                        //End - HU18195
		}
		else
		{
			remoteFileName = fileName.substr(pos + 1);
			//Start - HU18195
                        AES_CDH_LOG(LOG_LEVEL_INFO, "newFileName NULL - ELSE COND: remoteFileName = %s",remoteFileName.c_str());
                        //End - HU18195
		}
	}
	else
	{
		remoteFileName = newFileName;
		//Start - HU18195
                AES_CDH_LOG(LOG_LEVEL_INFO, "newFileName NOT NULL - ELSE COND: remoteFileName = %s",remoteFileName.c_str());
                //End - HU18195
	}

	if (remoteSubDirName != "")
	{
		remoteFileName = remoteSubDirName + "-" + remoteFileName;
		//Start - HU18195
                AES_CDH_LOG(LOG_LEVEL_INFO, "remoteSubDirName NOT NULL: remoteFileName = %s",remoteFileName.c_str());
                //End - HU18195
	}

	AES_CDH_TRACE_MESSAGE("vdDestPath is %s",vdDestPath.c_str());
	outputfilepath = this->vdDestPath;
	outputfilepath += "/" + readyDirName;
	AES_CDH_TRACE_MESSAGE("outputfilepath is %s",outputfilepath.c_str());

	//Start - HU18195
        if(!isFileNotification)
        {
                stagedFolderPath = this->vdDestPath + "/" + stagedDirName;
                //File copied to Staged with remoteFileName
                stagedFilePath = this->vdDestPath + "/" + stagedDirName + "/" + remoteFileName;
        }
        //End - HU18195

	// Time stamp is added to filename if filename is alerady used
	bool bOverWrite = false;
	if (overWrite == "yes")
	{
		bOverWrite = true;
		//Start - HU18195
                AES_CDH_TRACE_MESSAGE("overWrite == yes");
                //End - HU18195
	}


	if (badSSHFTPPermissions)
	{
		if (this->setSSHFTPPermissions() == AES_CDH_RC_OK)
			badSSHFTPPermissions = false;
	}


	if (bOverWrite)
	{
		if (moveFile == true)
		{
			returnCode = rMoveFile(inputFile, outputfilepath, remoteFileName);
			//Start - HU18195
                        AES_CDH_LOG(LOG_LEVEL_INFO, "rMoveFile - inputFile = %s, outputfilepath = %s, remoteFileName = %s,returnCode = %d",inputFile.c_str(),outputfilepath.c_str(),remoteFileName.c_str(),returnCode);
                        //End - HU18195
		}
		else
		{
			// Copy the file to the FTP-area for this destination
			//Start - HU18195
                        if(!isFileNotification)
                        {
                                returnCode = rCopyFile(inputFile, stagedFolderPath, remoteFileName, false);
                                AES_CDH_LOG(LOG_LEVEL_INFO,"rCopyFile - inputFile = %s, stagedFolderPath = %s, remoteFileName = %s,returnCode = %d",inputFile.c_str(),stagedFolderPath.c_str(),remoteFileName.c_str(),returnCode);
                                if(returnCode == AES_CDH_RC_OK)
                                {
                                        //Move the file from Staged to Ready
                                        returnCode = rMoveFile(stagedFilePath, outputfilepath, remoteFileName);
                                        AES_CDH_LOG(LOG_LEVEL_INFO,"rMoveFile - stagedFilePath = %s, outputfilepath = %s, remoteFileName = %s,returnCode = %d",stagedFilePath.c_str(),outputfilepath.c_str(),remoteFileName.c_str(),returnCode);
                                }
                        }
                        else
                        {
                                //Stag not required for Notification Case
                                returnCode = rCopyFile(inputFile, outputfilepath, remoteFileName, false);
                                AES_CDH_LOG(LOG_LEVEL_INFO,"rCopyFile - inputFile = %s, outputfilepath = %s, remoteFileName = %s,returnCode = %d",inputFile.c_str(),outputfilepath.c_str(),remoteFileName.c_str(),returnCode);
                        }
                        //End - HU18195
		}

		// If the file should be notified, put info about that
		// file in the database before returning to the caller
		if (isFileNotification == true && returnCode == AES_CDH_RC_OK)
		{
			AES_CDH_TRACE_MESSAGE("isFileNotification = %d, returnCode = %d",isFileNotification,returnCode);
			sendToDataBase(remoteFileName);
		}

		if (returnCode == AES_CDH_RC_OK)
			badSSHFTPPermissions = false;

		return returnCode;
	}
	else
	{
		if (moveFile == true)
		{

			// Overwrite should not be used.
      		          // Check if the file already exists

                	if (isLocalFile(outputfilepath + "/" + remoteFileName))
		        {
        	                AES_CDH_LOG(LOG_LEVEL_INFO, "inside if(isLocalFile)  File exists already, add timestamp");
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
                        	AES_CDH_LOG(LOG_LEVEL_INFO, "inside if(isLocalFile)  File exists already, add timestamp count=%d, outputfilepath = %s, remoteFileName=%s",count,outputfilepath.c_str(),remoteFileName.c_str());
	                }

			returnCode = rMoveFile(inputFile, outputfilepath, remoteFileName);
			//Start - HU18195
                        AES_CDH_LOG(LOG_LEVEL_INFO,"NO OVERWRITE rMoveFile - inputFile = %s, outputfilepath = %s, remoteFileName = %s,returnCode = %d",inputFile.c_str(),outputfilepath.c_str(),remoteFileName.c_str(),returnCode);
                        //End - HU18195

		}
		else
		{
			 //Start - HU18195
                        if(!isFileNotification)
                        {
                                returnCode = rCopyFile(inputFile, stagedFolderPath, remoteFileName, false);
                                AES_CDH_LOG(LOG_LEVEL_INFO,"OVERWRITE - rCopyFile - inputFile = %s, stagedFolderPath = %s, remoteFileName = %s,returnCode = %d",inputFile.c_str(),stagedFolderPath.c_str(),remoteFileName.c_str(),returnCode);
                                if(returnCode == AES_CDH_RC_OK)
                                {

					  // Overwrite should not be used.
        			        // Check if the file already exists
			                if (isLocalFile(outputfilepath + "/" + remoteFileName))
			                {
                        			AES_CDH_LOG(LOG_LEVEL_INFO, "inside if(isLocalFile)  File exists already, add timestamp");
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
			                        AES_CDH_LOG(LOG_LEVEL_INFO, "inside if(isLocalFile)  File exists already, add timestamp count=%d, outputfilepath = %s, remoteFileName=%s",count,outputfilepath.c_str(),remoteFileName.c_str());
			                }

                                        //Move the file from Staged to Ready
                                        returnCode = rMoveFile(stagedFilePath, outputfilepath, remoteFileName);
                                        AES_CDH_LOG(LOG_LEVEL_INFO,"NO OVERWRITE - rMoveFile - stagedFilePath = %s, outputfilepath = %s, remoteFileName = %s,returnCode = %d",stagedFilePath.c_str(),outputfilepath.c_str(),remoteFileName.c_str(),returnCode);
                                }
                        }
                        else
                        {

					  // Overwrite should not be used.
                                        // Check if the file already exists
                                        if (isLocalFile(outputfilepath + "/" + remoteFileName))
                                        {
                                                AES_CDH_LOG(LOG_LEVEL_INFO, "inside if(isLocalFile)  File exists already, add timestamp outputfilepath = %s, remoteFileName=%s",outputfilepath.c_str(),remoteFileName.c_str());
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
                                                AES_CDH_LOG(LOG_LEVEL_INFO, "inside if(isLocalFile)  File exists already, add timestamp count=%d, outputfilepath = %s, remoteFileName=%s",count,outputfilepath.c_str(),remoteFileName.c_str());
                                        }

                                returnCode = rCopyFile(inputFile, outputfilepath, remoteFileName, false);
                                AES_CDH_LOG(LOG_LEVEL_INFO,"NO OVERWRITE - rCopyFile - inputFile = %s, outputfilepath = %s, remoteFileName = %s,returnCode = %d",inputFile.c_str(),outputfilepath.c_str(),remoteFileName.c_str(),returnCode);
                        }
                        //End - HU18195

		}

		// If the file should be notified, put info about the file
		// in the database before returning
		if (isFileNotification == true && returnCode == AES_CDH_RC_OK)
		{
			AES_CDH_TRACE_MESSAGE("isFileNotification = %d, returnCode = %d",isFileNotification,returnCode);
			sendToDataBase(remoteFileName);
		}

		if (returnCode == AES_CDH_RC_OK)
			badSSHFTPPermissions = false;

		return returnCode;
	}

	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}


/*===================================================================
   ROUTINE:sendToDataBase
=================================================================== */
bool SSHFTPRV2::sendToDataBase(const std::string &fileName)
{

	//AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	AES_CDH_TRACE_MESSAGE("Entering SSHFTPRV2::sendToDataBase, fileName = %s",fileName.c_str());
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
	AES_CDH_TRACE_MESSAGE("dbFileName = %s", dbFileName.c_str());

	// Create paths for the database
	std::string fullPath  = vdDestPath + "/" + readyDirName;   /*+ "/" + dbFileName;*/
	AES_CDH_TRACE_MESSAGE("fullPath = %s", fullPath.c_str());
	std::string storePath = this->destinationName + "/" + readyDirName;
	AES_CDH_TRACE_MESSAGE("storePath = %s", storePath.c_str());

	ACE_INT32 errors = 0;
	std::string fileSizePath = vdDestPath + "/" + readyDirName;
	AES_CDH_TRACE_MESSAGE("fileSizePath = %s", fileSizePath.c_str());
	ACE_UINT64 fSize = getFileSize(fileSizePath, dbFileName, errors);

	if (errors != 0)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "getFileSize error = %d",errors);
		AES_CDH_TRACE_MESSAGE("getFileSize error = %d",errors);
		// Error(s) occurred
	}

	ACE_TCHAR fileSize[20];
	ACE_OS::sprintf(fileSize, "%lu", fSize);

	// Create the record to be stored
	NotificationRecord *rec = new NotificationRecord;
	rec->setStoreFileName(dbFileName);
	rec->setFileSize(fileSize);
	rec->setMessageNumber(1);

	CDHCriticalSection::instance().enter();
	db = DataBase::instance(); //TBD: make this function's new as no_throw and include error check here
	bool bSuccess = false;
	if (db == NULL)
		AES_CDH_LOG(LOG_LEVEL_INFO, "sendToDataBase db == NULL ");
	else
	{
		AES_CDH_TRACE_MESSAGE("destinationName = %s", destinationName.c_str());
		AES_CDH_TRACE_MESSAGE("dbFileName = %s", dbFileName.c_str());
		AES_CDH_TRACE_MESSAGE("fileSize = %s", fileSize);
		AES_CDH_TRACE_MESSAGE("clusterIPAddress = %s", clusterIPAddress.c_str());
		AES_CDH_TRACE_MESSAGE("storePath = %s", storePath.c_str());
		AES_CDH_TRACE_MESSAGE("fullPath = %s", fullPath.c_str());

		bSuccess = db->add(destinationName,			//TBD
				dbFileName,
				fileSize,
				rec,
				clusterIPAddress,
				storePath,
				fullPath);

		CDHCriticalSection::instance().leave();
		AES_CDH_TRACE_MESSAGE("return value = %d", bSuccess);
	}
	//delete rec;
	return bSuccess;
}

/*===================================================================
   ROUTINE:isLocalFile
=================================================================== */
bool SSHFTPRV2::isLocalFile(const string &fileName) const
{

	AES_CDH_LOG(LOG_LEVEL_INFO, "Entering");
	ACE_stat statBuffer;
	int ret = ACE_OS::stat( fileName.c_str() , &statBuffer );
	AES_CDH_LOG(LOG_LEVEL_INFO, "SSHFTPRV2::isLocalFile ret=%d",ret);
	if( ret == 0 ) 
	{
		return true;
	}
	return false;
}

/*===================================================================
   ROUTINE:sendDirectoryResponding
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::sendDirectoryResponding(const string& dirName,
		const string& remoteSubDirName,
		const string& newDirName)
{
	AES_CDH_LOG(LOG_LEVEL_INFO, "Entering");
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	if (keep)
		returnCode = keep->checkConnection();
	else
		cleanUpSSHFtpDeleteDir();

	isTimeStamp = false;        

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
		//Start - HU18195
                if(!isFileNotification)
                {
                        outputfilepath = vdDestPath + "/" + stagedDirName;
                }
                //End - HU18195
		AES_CDH_LOG(LOG_LEVEL_INFO, "outputfilepath is %s",outputfilepath.c_str());
/*
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
*/
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
ACE_UINT64 SSHFTPRV2::getFileSize(const string &path, const string &fName, ACE_INT32 &errors)
{
	//
	// path:   this is a path to a file or a directory
	//
	// errors: should be zero, if not the value indicates the number
	//         of errors that occurred while processing
	//

	ACE_HANDLE fh;
	//bool bIsDir;
	string fileName("");
	string workPath("");
	string searchPattern = path;
	ACE_UINT64 fileSize;

	if(!fName.empty())
		searchPattern += "/" + fName;

	ACE_stat statBuffer;

	if( ACE_OS::stat( searchPattern.c_str(), &statBuffer ) != 0 )
	{
		errors++;
		return 0;
	}
	else
	{
		ACE_UINT64  recFileSize=0;
		if( statBuffer.st_mode == S_IFDIR )
		{
			ACE_UINT64 totSize=0;
			DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
			if( pDir != 0 )
			{
				struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
				fileSize = totSize = recFileSize = 0;

				if (pEntry==NULL)
					errors++;

				while (pEntry)
				{ 
					fileName.assign(path).append("/").append(pEntry->d_name);
					struct stat buf;
					stat(fileName.c_str(), &buf);
					bool bIsDir=S_ISDIR(buf.st_mode);

					if (!bIsDir) 
					{
						if ( (fh = ACE_OS::open(fileName.c_str(), O_RDONLY, S_IREAD)) != ACE_INVALID_HANDLE)
						{
							totSize += (ACE_UINT64)ACE_OS::filesize(fh);
							int t = ACE_OS::close(fh);
							if(t == -1)
							{
								AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to close %s file as %d",fileName.c_str(), errno);
								AES_CDH_TRACE_MESSAGE("Not able to close %s file as %d",fileName.c_str(), errno);
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
						if (! ((ACE_OS::strcmp(pEntry->d_name,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name,"..") == 0)))
						{
							// A sub directory found. Make a recursive call to getFileSize
							recFileSize += getFileSize(fileName.append("/"), "", errors);
						}
					}

					pEntry = ACE_OS::readdir(pDir);
				}
				if(pDir != NULL)
					closedir(pDir);
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to open %s directory as %d",searchPattern.c_str(), errno);
				AES_CDH_TRACE_MESSAGE("Not able to open %s directory as %d",searchPattern.c_str(), errno);
			}

			return recFileSize + totSize;
		}
		else
		{
			recFileSize = statBuffer.st_size;
			return recFileSize;
		}
	}
}

/*===================================================================
ROUTINE:rCreateDirectory
=================================================================== */
void SSHFTPRV2::rCreateDirectory(string name)
{
	mode_t mask = ACE_OS::umask(temp_mask);
	if (ACE_OS::mkdir(name.c_str(),dir_full_perms) != -1)
	{
		//    	returnCode = AES_CDH_RC_OK;
		ACE_OS::umask(mask);

	}
	else 
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn( name,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Directory name sent for event %s",newdr.c_str());
			AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
			newdr = "Not Available";
		}
		Event::report(AES_CDH_remoteFileFault,
				"FTP RESPONING ERROR",           // AES_CDH_remoteFileFault, "FTP ERROR" ??
				"Unable to create directory : ",
				newdr.c_str());

		throw AES_CDH_RC_SENDERR;                                   // AES_CDH_RC_SENDERR ??
	}
}

/*===================================================================
   ROUTINE:rCopyFile
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::rCopyFile(const string inPath,
		const string outPath,
		const string fName,
		const bool noOwrWrite)
{
	(void)noOwrWrite;
	string tmpStr = outPath + "/" + fName;
	string outputfilepath = outPath;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	string destSetName = destinationPtr->getDestSetName();
	bool bStopFileTransfer = ServR::checkStopFlag(destSetName,
			aFileName,
			aNewMainFileName,
			aNewSubFileName);
	if (bStopFileTransfer)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "FileTransfer interrupted");
		AES_CDH_TRACE_MESSAGE("FileTransfer interrupted");
		return AES_CDH_RC_TASKSTOPPED;
	}


	int ret = copyFile(inPath.c_str(), tmpStr.c_str());
	if (ret != -1)
	{
		if( ret == 1 ) 
		{
			returnCode = AES_CDH_RC_TASKSTOPPED;
			AES_CDH_LOG(LOG_LEVEL_ERROR,"in SSHFTPRV2::rCopyFile if( ret == 1 ) returnCode = AES_CDH_RC_TASKSTOPPED");
		}
		else
		{
			returnCode = AES_CDH_RC_OK;
			AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::rCopyFile else returnCode = AES_CDH_RC_OK ret=%d",ret);
		}
		//file copied from inputfile to <aesapdata>cdh\ftp\<destName>\Ready\outputfile
	}
	else
	{
		int error = ACE_OS::last_error();
		AES_CDH_LOG(LOG_LEVEL_ERROR,"in SSHFTPRV2::rCopyFile---copyFile FAILED error=%d",error);
		if (( error == EEXIST ) || ( error == ENOENT))
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR,"in SSHFTPRV2::rCopyFile---copyFile FAILED in side if");
			string newdr("");
			if (AES_GCC_Util::datapath_trn( inPath,AES_DATA_PATH,newdr)==false)
			{
				AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
				AES_CDH_LOG(LOG_LEVEL_ERROR,"in SSHFTPRV2::rCopyFile---copyFile FAILED in if datapath_");
				newdr = "Not Available";
			}
			Event::report(AES_CDH_fileErrFault,
					"FILE ERROR",
					"File not found : ",
					newdr);

			// input file not found
			returnCode = AES_CDH_RC_SENDERR;
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR,"in SSHFTPRV2::rCopyFile---copyFile FAILED in side else");
			string newdr1(""),newdr2("");
			if (AES_GCC_Util::datapath_trn( inPath,AES_DATA_PATH,newdr1)==false)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR,"in SSHFTPRV2::rCopyFile---copyFile FAILED in side else in if datapath_");
				newdr1 = "Not Available";
			}

			if( AES_GCC_Util::datapath_trn( outPath,AES_DATA_PATH,newdr2)==false)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Directory name sent for event %s & %s ",newdr1.c_str(),newdr2.c_str());
				AES_CDH_TRACE_MESSAGE("Directory name sent for event %s & %s ",newdr1.c_str(),newdr2.c_str());
				newdr2 = "Not Available";
			}
			Event::report(AES_CDH_fileCopyFault,
					"FILE COPY ERROR",
					ServR::NTErrorText(),
					"Failed to copy file to CDH FTP area: " + newdr1 + " to " + newdr2);

			returnCode = AES_CDH_RC_SENDERR;
		}
		// Failed to copy the file!!
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:rMoveFile
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::rMoveFile(const string &inPath,
		const string &outPath,
		const string &remoteFileName)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	string targetPath = outPath + "/" + remoteFileName;
	AES_CDH_LOG(LOG_LEVEL_INFO, "in SSHFTPRV2::rMoveFile ");
	if (ACE_OS::rename(inPath.c_str(), targetPath.c_str())==0)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "in SSHFTPRV2::rMoveFile ACE_OS::rename SUCCESS");
		returnCode = AES_CDH_RC_OK;
	}
	else
	{
		int error = ACE_OS::last_error();
		AES_CDH_LOG(LOG_LEVEL_ERROR, "in SSHFTPRV2::rMoveFile ACE_OS::rename FAILED error=%d",error);
		if ((error == EEXIST ) || ( error == ENOENT))
		{
			string newdr("");

			if (AES_GCC_Util::datapath_trn( inPath,AES_DATA_PATH,newdr)==false)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Directory name sent for event %s",newdr.c_str());
				AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
				newdr = "Not Available";
			}
			Event::report(AES_CDH_fileErrFault,
					"FILE ERROR",
					"File not found : ",
					newdr);

			returnCode = AES_CDH_RC_SENDERR;
		}
		else
		{
			string newdr1(""),newdr2("");
			AES_CDH_LOG(LOG_LEVEL_ERROR, "in SSHFTPRV2::rMoveFile ACE_OS::rename FAILED in else");
			if (AES_GCC_Util::datapath_trn( inPath,AES_DATA_PATH,newdr1)==false)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Directory name sent for event %s & %s ",newdr1.c_str(),newdr2.c_str());
				AES_CDH_TRACE_MESSAGE("Directory name sent for event %s & %s ",newdr1.c_str(),newdr2.c_str());
				newdr1 = "Not Available";
			}
			if (AES_GCC_Util::datapath_trn( outPath,AES_DATA_PATH,newdr2)==false)
			{
				newdr2 = "Not Available";
			}
			AES_CDH_LOG(LOG_LEVEL_ERROR,"ACE::OS rename error=%d",error);
			Event::report(AES_CDH_fileErrFault,
					"FILE ERROR",
					ServR::NTErrorText(),
					"Failed to move file to CDH FTP area: " + newdr1 + " to " + newdr2);

			returnCode = AES_CDH_RC_SENDERR;
		}

		if (error == EACCES)
			{
			AES_CDH_LOG(LOG_LEVEL_INFO,"error = EACCESS");
			badSSHFTPPermissions = true;
			}
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:doSendDirResponding
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::doSendDirResponding(string path, string outPath)
{
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string searchPattern = path ;
	string intOutPath = outPath;
	DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if( pDir != 0 )
	{
		struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
		directoryLevel += 1;
		bool bIsDir;
		while(pEntry!=NULL)
		{
			if (directoryLevel == 1)
			{
				remoteFullPath = vdDestPath + "/" + readyDirName + "/" + aRemoteDirName;
				AES_CDH_LOG(LOG_LEVEL_INFO, "remoteFullPath (REady) is %s",remoteFullPath.c_str());
                                AES_CDH_TRACE_MESSAGE("Inside SSHFTPRV2::doSendDirResponding, remoteFullPath (REady) is %s", remoteFullPath.c_str());
				//Start - HU18195
                                if(!isFileNotification)
                                {

                                        remoteFullPath = vdDestPath + "/" + stagedDirName + "/" + aRemoteDirName;
                                        AES_CDH_LOG(LOG_LEVEL_INFO, "remoteFullPath (Staged) is %s", remoteFullPath.c_str());
                                        AES_CDH_TRACE_MESSAGE("Inside SSHFTPRV2::doSendDirResponding for WIthout Notification, remoteFullPath (Staged) is %s", remoteFullPath.c_str());
                                }
                                //End - HU18195
				try
				{
					rCreateDirectory(remoteFullPath);
				}
				// exception caught, close the dir and return the error
				catch( AES_CDH_ResultCode rc)
				{
					closedir(pDir);
					return rc;
				}
				intOutPath = remoteFullPath;
				directoryLevel += 1;
			}
			string filePath = path + "/" + string(pEntry->d_name);
			struct stat buf;
			stat(filePath.c_str(), &buf);
			bIsDir=S_ISDIR(buf.st_mode);
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
					try
					{
						rCreateDirectory(intOutPath + "/" + pEntry->d_name);
					}
					// exception caught, close the dir and return the error
					catch( AES_CDH_ResultCode rc)
					{
						closedir(pDir);
						return rc;
					}
					returnCode = doSendDirResponding(path + "/" + pEntry->d_name,
							intOutPath + "/" + pEntry->d_name);
					if(returnCode != AES_CDH_RC_OK)
						break;
				}
			}
			pEntry = ACE_OS::readdir(pDir);
		}
		if(pDir != NULL)
			closedir(pDir);

		if(returnCode == AES_CDH_RC_OK)
			directoryLevel -= 1;
	


	
		//Start - HU18195
                //to move Staged to Ready       
                if(!isFileNotification)
                {
                        string outPath="";
                        string inPath ="";
                        inPath = vdDestPath + "/" + stagedDirName + "/" + aRemoteDirName;
                        outPath=vdDestPath + "/" + readyDirName;


		 // Time stamp is added to filename if filename is alerady used
                // Overwrite should not be used. Check if the file already exists
                if (isLocalFile(outPath+ "/" + aRemoteDirName))
                {
                        // File exists already, add timestamp
                        bool done = false;
                        ACE_INT32 count = 0;
                        while (!done && (count <= 100))
                        {
                                stepFileName(aRemoteDirName, 0);
                                if (!isLocalFile(outPath + "/" + aRemoteDirName))
                                {
                                        done = true;
                                }
                                count++;
                        }
                }


                        returnCode = rMoveFile(inPath,outPath, aRemoteDirName);
                        AES_CDH_LOG(LOG_LEVEL_INFO,"rMoveFile - stagedFilePath = %s, outputfilepath = %s, remoteFileName = %s,returnCode = %d",inPath.c_str(),outPath.c_str(),aRemoteDirName.c_str(),returnCode);
                }
                // END - HU18195
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to open %s directory as %d",searchPattern.c_str(), errno);
		AES_CDH_TRACE_MESSAGE("Not able to open %s directory as %d",searchPattern.c_str(), errno);
		returnCode = AES_CDH_RC_SENDERR;
	}

	return returnCode;
}

/*===================================================================
   ROUTINE:getAgentType
=================================================================== */
TransferAgent::TransferAgentAttributes SSHFTPRV2::getAgentType()
{
	return TransferAgent::FILEBACKUP;
}

/*===================================================================
   ROUTINE:getVDDestPath
=================================================================== */
std::string SSHFTPRV2::getVDDestPath()
{
	return vdDestPath;
}
/*===================================================================
   ROUTINE:getVDPath
=================================================================== */
std::string SSHFTPRV2::getVDPath()
{
	return vdPath;
}

/*===================================================================
   ROUTINE:readVDPath
=================================================================== */
AES_CDH_ResultCode SSHFTPRV2::readVDPath(std::string& path)
{
	//This path will be retrieved later using APGCC API.
	string dataTransferDir("");
	if( ServR::getDataTransferFolderPath(dataTransferDir))
	{
		path = dataTransferDir ;
	}
	else
	{	
		return AES_CDH_RC_NOALIAS;
	}

	return AES_CDH_RC_OK;

}

/*===================================================================
 *                     ROUTINE: copyFile
 *=================================================================== */
ACE_INT32 SSHFTPRV2::copyFile (string sourceFile, string destinationFile)
{
	char  buffer[BUFFER_SIZE]= { 0 };  /* buffer for data */
	ACE_HANDLE   in_file = ACE_INVALID_HANDLE;              /* input file descriptor */
	ACE_HANDLE   out_file = ACE_INVALID_HANDLE;             /* output file descriptor */
	ssize_t   read_size = 0;            /* number of bytes on last read */
	struct stat stat_buf;       /* hold information about input file */
	int result = 0;                 /* holds return value */

	AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::copyFile");
	in_file = ACE_OS::open(sourceFile.c_str(), O_RDONLY|O_BINARY);
	if (in_file  == ACE_INVALID_HANDLE)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::copyFile ACE_OS::open FAILED");
		return -1;
	}
	result = ACE_OS::fstat(in_file, &stat_buf);
	AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::copyFile after ACE_OS::fstat result=%d",result);
	if ( result < 0 )
	{	
		mode_t exist_mask = ACE_OS::umask(temp_mask);
		out_file = ACE_OS::open(destinationFile.c_str(), O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
		ACE_OS::umask(exist_mask);
	}
	else
	{
		mode_t exist_mask = ACE_OS::umask(temp_mask);
		//out_file = ACE_OS::open(destinationFile.c_str(), O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, stat_buf.st_mode);
		out_file = ACE_OS::open(destinationFile.c_str(), O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, S_IRWXU|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
		ACE_OS::umask(exist_mask);
	}
	if (out_file < 0)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::copyFile ACE_OS::open FAILED if (out_file < 0)");
		close(in_file);
		return -1;
	}
	while (1)
	{
		memset( buffer, 0, sizeof(buffer));
		string destSetName = destinationPtr->getDestSetName();
		bool bStopFileTransfer = ServR::checkStopFlag(destSetName,
				aFileName,
				aNewMainFileName,
				aNewSubFileName);
		if (bStopFileTransfer)
		{
			ACE_OS::close(in_file);
			ACE_OS::close(out_file);
			AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::copyFile in while(1)");
			return 1; //TASK_STOPPED.
		}

		read_size = ACE_OS::read(in_file, buffer, sizeof(char)*BUFFER_SIZE);

		if (read_size == 0)
			break;              /* end of file */

		if (read_size < 0)
		{
			ACE_OS::close(in_file);
			ACE_OS::close(out_file);
			AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::copyFile in while(1)  if (read_size < 0)");
			return -1;
		}
		ssize_t noOfBytesWriiten = ACE_OS::write(out_file, buffer, read_size);
		if( noOfBytesWriiten < 0 )
		{
			ACE_OS::close(in_file);
			ACE_OS::close(out_file);
			AES_CDH_LOG(LOG_LEVEL_INFO,"in SSHFTPRV2::copyFile in while(1)   if( noOfBytesWriiten < 0 )");
			return -1;
		}

	}
	ACE_OS::close(in_file);
	ACE_OS::close(out_file);
	return 0;
}
