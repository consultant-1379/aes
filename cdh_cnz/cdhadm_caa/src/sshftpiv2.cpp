/*=====================================================================*/
/**
@file sshftpiv2.cpp

Class method implementation for sshftpiv2.h

DESCRIPTION
Handles FTP parts specific to SSH.
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
#include <sshftpiv2.h>
#include <event.h>
#include <servr.h>
#include <sstream>
#include <ace/ACE.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define SEND_BUFFER_SIZE (ACE_UINT64)8192

AES_CDH_TRACE_DEFINE(AES_CDH_SSHFTPIV2);

using namespace std;

// Mutex to stop different threads using different instances
// SftpApi at the same time. SftpApi is not "thread-safe", it
// uses global variables for its internal state.
ACE_Thread_Mutex SSHFTPIV2::sftpApiMutex;

//HINSTANCE SSHFTPIV2::hinstSftpLib = NULL;

/*===================================================================
   ROUTINE:sftpApiInUse
=================================================================== */
SSHFTPIV2::SSHFTPIV2() : sftpApiInUse(false)
{
	//CR 85 (SFTP API Replacement)
#ifdef DEBUGTEST
	std::cout << "SSHFTPIV2() new sftpApiObj try to create\n";
#endif

	sftpApiObj = new SftpAdapterClass();
	listTime = 0;
}

/*===================================================================
   ROUTINE:~sftpApiInUse
=================================================================== */
SSHFTPIV2::~SSHFTPIV2()
{
	if( sftpApiObj != 0 )
	{
		sftpApiObj->sftpFinalize();
		delete sftpApiObj;
		sftpApiObj = 0;
	}
}

/*===================================================================
   ROUTINE:ftpConnect
=================================================================== */
void SSHFTPIV2::ftpConnect(const string& hostAddress,
                        const string& portNr, 
                        const string& userName, 
                        const string& passWord,
                        bool ascii) throw (AES_CDH_ResultCode){
	AES_CDH_TRACE_MESSAGE("Establishing a FTP Connection ");
	ACE_INT32 portNr_int;
	sftpApiInUse = true;

	if (portNr == "")
	{
		portNr_int = 22;
	}
	else
	{
		portNr_int = atoi(portNr.c_str());
	}

	if ( userName != "" )
	{

		AES_CDH_ResultCode result = sftpApiObj->sftpConnect(this, hostAddress.c_str(),portNr_int,userName.c_str(), passWord.c_str(),ascii);
		if (result == AES_CDH_RC_LOGONFAILURE)
		{
			Event::report(AES_CDH_ftpConnectFault,
					"SFTP Logon failure:",
					sftpErrorText(),
					"Destination: "
					+ destinationName);
			throw result;
		}
		else if ( result == AES_CDH_RC_TASKSTOPPED)
		{
			Event::report(AES_CDH_ftpConnectFault,
					"SFTP CONNECT ERROR:",
					sftpErrorText(),
					"when connecting to "
					+ hostAddress + " with username " + userName);

			throw result;
		}
		else if ( result == AES_CDH_RC_CONNECTERR)
		{
			Event::report(AES_CDH_ftpConnectFault,
					"SFTP CONNECT ERROR:",
					sftpErrorText(),
					"when connecting to "
					+ hostAddress + " with username " + userName);

			throw result;

		}

	}
	else
	{
		Event::report(AES_CDH_ftpConnectFault,
				"SFTP CONNECT ERROR",
				"Incorrect usage of username and password", "-");

		throw AES_CDH_RC_CONNECTERR;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
   ROUTINE:ftpDisconnect
=================================================================== */
void SSHFTPIV2::ftpDisconnect(void) throw() {
	AES_CDH_TRACE_MESSAGE("Entering");

	//CR 85 (SFTP API Replacement)
	sftpApiObj->sftpDisconnect();  

	AES_CDH_TRACE_MESSAGE("Exiting");

}

void SSHFTPIV2::ftpDisconnect(bool closeconnection) throw() {  //HY73707
        AES_CDH_TRACE_MESSAGE("Entering");

        //CR 85 (SFTP API Replacement)
        sftpApiObj->sftpDisconnect(closeconnection);  

        AES_CDH_TRACE_MESSAGE("Exiting");

}

AES_CDH_ResultCode SSHFTPIV2::verifyConnection()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	try
	{
		ftpConnect(hostAddress, portNr, userName, passWord);
	}
	catch(const AES_CDH_ResultCode & rc)
	{
		rCode = rc;
	}

	ftpDisconnect(true); //HY73707
	AES_CDH_TRACE_MESSAGE("Exiting");
	return rCode;
}

/*===================================================================
   ROUTINE:ftpCreateDirectory
=================================================================== */
void SSHFTPIV2::ftpCreateDirectory(const std::string& name) throw (AES_CDH_ResultCode) {
	AES_CDH_TRACE_MESSAGE("Entering");

	string fullPath = addNameToPath(remoteFullPath, name);

	//CR 85 (SFTP API Replacement)
	bool result = sftpApiObj->sftpCreateDirectories(fullPath.c_str());

	if (result != SFTPAPI_API_SUCCESS)
	{
#if 0
		//CR 85 (SFTP API Replacement)
		rcode = sftpApiObj->getSftpOperationStaus();

		if (rcode != (SFTPAPI_DIRECTORY_ALREADY_EXISTS))
		{
			if (!silent)
			{
				Event::report(AES_CDH_remoteFileFault, "SFTP ERROR",
						sftpErrorText(),
						"Unable to create directory : " + fullPath);
				throw AES_CDH_RC_SENDERR;
			}
		}
#endif
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to create directory");
		AES_CDH_TRACE_MESSAGE("Unable to create directory");
	}
	AES_CDH_TRACE_MESSAGE("Exiting");

}

/*===================================================================
   ROUTINE:ftpDeleteFile
=================================================================== */
bool SSHFTPIV2::ftpDeleteFile(const string& fileName, string& errorText) throw() {
	(void)errorText;

	AES_CDH_TRACE_MESSAGE("Entering");

	//CR 85 (SFTP API Replacement)
	bool result = sftpApiObj->sftpDeleteFile(fileName.c_str());
	if (result != SFTPAPI_API_SUCCESS)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to delete file");
		AES_CDH_TRACE_MESSAGE("Not able to delete file");
		return false;
#if 0
		ACE_INT32 errCode = sftpApiObj->getSftpOperationStaus();
		if (errCode != SFTPAPI_NO_SUCH_FILE)
		{
			errorText = sftpErrorText();
			return false;
		}
#endif
	}

	AES_CDH_TRACE_MESSAGE("Exiting");

	return true;
}

/*===================================================================
   ROUTINE:ftpSendFile
=================================================================== */
AES_CDH_ResultCode SSHFTPIV2::ftpSendFile(const string& fileName,
                                          const string& remoteFileName,
                                          AES_CDH_DestinationSet::transferMode trMode)throw (AES_CDH_ResultCode)
{
	(void)fileName;
	(void)remoteFileName;
	(void)trMode;

	return AES_CDH_RC_CONNECTOK;
}

/*===================================================================
   ROUTINE:renameRemoteFile
=================================================================== */
//  WARNING: This method overwrites already existing files
//  SEE ALSO: isRemoteFile()
//******************************************************************************
bool SSHFTPIV2::renameRemoteFile(const string& currentName,
                                 string& newName,
                                 string& errorText) throw()
{

	AES_CDH_TRACE_MESSAGE("Entering");
	string fullCurrentName = addNameToPath(remoteFullPath, currentName);
	string fullNewName = addNameToPath(remoteFullPath, newName);

	if (overWrite == "yes")
	{

		//CR 85 (SFTP API Replacement)
		if (sftpApiObj->renameRemoteFile(fullCurrentName.c_str(),
				fullNewName.c_str()) == (SFTPAPI_API_SUCCESS))
		{
			return true;
		}


		//  errorText = sftpErrorText();
		return false;
	}

	bool found = checkRemoteFile(remoteFileList, newName);

	if (!found)
	{

		if (sftpApiObj->renameRemoteFile(fullCurrentName.c_str(),
				fullNewName.c_str()) == (SFTPAPI_API_SUCCESS))
		{
			return true;
		}

		errorText = sftpErrorText();
		return false;
	}

	// overwrite is not specified, and the filename is not
	// available at the remote system. Find a unique filename

	bool done = false;
	ACE_INT32 maxRetries = 0;
	while (maxRetries <= 10 && !done)
	{
		if (stepFileName(newName, maxRetries))
		{
			found = checkRemoteFile(remoteFileList, newName);
			if (!found)
				done = true;
		}

		maxRetries++;
	}

	if (!done)
	{
		errorText = "stepFileName() failed";
		return false;
	}

	fullNewName = addNameToPath(remoteFullPath, newName);

	if (sftpApiObj->renameRemoteFile(fullCurrentName.c_str(),
			fullNewName.c_str()) == (SFTPAPI_API_SUCCESS))
	{
		return true;
	}

	errorText = sftpErrorText();

	AES_CDH_TRACE_MESSAGE("Exiting");
	return false;
}

/*===================================================================
   ROUTINE:renameRemoteDir
=================================================================== */
bool SSHFTPIV2::renameRemoteDir(const string& currentName,
                                string& newName,
                                string& errorText)throw()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	string fullCurrentName = addNameToPath(remoteFullPath, currentName);
	string fullNewName = addNameToPath(remoteFullPath, newName);

	bool found = checkRemoteFile(remoteFileList, newName);

	if (!found)
	{

		if (sftpApiObj->renameRemoteDir(fullCurrentName.c_str(),
				fullNewName.c_str()) == (SFTPAPI_API_SUCCESS))
		{
			return true;
		}

		errorText = sftpErrorText();
		return false;
	}

	// overwrite is not specified, and the filename is not
	// available at the remote system. Find a unique filename

	bool done = false;
	ACE_INT32 maxRetries = 0;
	while (maxRetries <= 10 && !done)
	{
		if (stepFileName(newName, maxRetries))
		{
			found = checkRemoteFile(remoteFileList, newName);
			if (!found)
				done = true;
		}

		maxRetries++;
	}

	if (!done)
	{
		errorText = "stepFileName() failed";
		return false;
	}

	fullNewName = addNameToPath(remoteFullPath, newName);

	if (sftpApiObj->renameRemoteDir(fullCurrentName.c_str(),
			fullNewName.c_str()) == (SFTPAPI_API_SUCCESS))
	{
		return true;
	}


	errorText = sftpErrorText();

	AES_CDH_TRACE_MESSAGE("Exiting");

	return false;
}

/*===================================================================
   ROUTINE:isRemoteFile
=================================================================== */
//  Returns true if fileName exists at the remote location
//******************************************************************************
bool SSHFTPIV2::isRemoteFile(const string& fileName) const throw()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	bool result = sftpApiObj->isRemoteFile(fileName.c_str());
	AES_CDH_TRACE_MESSAGE("Exiting");

	return result;
}

/*===================================================================
   ROUTINE:isRemoteDir
=================================================================== */
//  Returns true if fileName exists at the remote location
//******************************************************************************
bool SSHFTPIV2::isRemoteDir(const string& dirName) const throw()
{
	return sftpApiObj->isRemoteDir(dirName.c_str());

}

/*===================================================================
   ROUTINE:checkRemoteFile
=================================================================== */
//  Check if a file exist in the file list
//******************************************************************************
bool SSHFTPIV2::checkRemoteFile(std::list<string>& fileList,
                                const string& fileName) throw()
{
	(void) fileList;

	AES_CDH_TRACE_MESSAGE("Entering");
	// Filelist is empty
	if (remoteFileList.empty())
		return false;

	size_t nPos = fileName.find_last_of("//:");
	string fileNameOnly = fileName.substr(nPos+1);

	size_t nFileLen = fileNameOnly.length();

	// Traverse the list to check if any filename match

	bool bFound = false;
	if (nFileLen>0)
	{
		remoteFileListIter = remoteFileList.begin();
		while (!bFound && remoteFileListIter != remoteFileList.end())
		{

			if (ACE_OS::strcmp((*remoteFileListIter).c_str(), fileNameOnly.c_str()) == 0)
				bFound = true;
			else if (ACE_OS::strcasecmp((*remoteFileListIter).c_str(), fileNameOnly.c_str()/*, nFileLen*/) < 0)
				break;
			else
				remoteFileListIter++;
		}
	}


	// If the file was not found, move to the first file in the list
	if (!bFound)
		remoteFileListIter = remoteFileList.begin();

	return bFound;

}

/*===================================================================
   ROUTINE:addNameToPath
=================================================================== */
//  Adds name to path and returns result
//******************************************************************************
string SSHFTPIV2::addNameToPath(const string& path, const string& name)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if(path.length() == 0)
	{
		AES_CDH_TRACE_MESSAGE("path length = 0 and Exiting");
		return name;
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Exiting");
		return path + "/" + name;
	}
}

/*===================================================================
   ROUTINE:sftpErrorText
=================================================================== */
//  Returns an error text for the last previous call of SftpApi
//******************************************************************************
std::string SSHFTPIV2::sftpErrorText(void) const
{
	string errorMsg = sftpApiObj->sftpErrorText();
	return errorMsg;
#if 0
	string error = "error";
	return error;
#endif
}

/*===================================================================
   ROUTINE:sendFileInitiating
=================================================================== */
AES_CDH_ResultCode SSHFTPIV2::sendFileInitiating(const string& fileName,
		const string& remoteSubDirName,
		const string& newFileName,
		AES_CDH_DestinationSet::transferMode trMode)
{
	// HM66365:Begin
	bool ascii= false;
	if(trMode == AES_CDH_DestinationSet::TR_ASCII)
	{
		ascii = true;
	}
	else
	{
		ascii = false;
	}

	AES_CDH_TRACE_MESSAGE("Entering , Transfer mode = %d",ascii);

	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	try
	{

		if (sftpApiObj->isTransmissionInterrupted(this))
		{
			AES_CDH_TRACE_MESSAGE("isTransmissionInterrupted is true");
			returnCode = AES_CDH_RC_TASKSTOPPED;
			Event::report(AES_CDH_remoteFileFault,
					"sftpSendFile() failed in SFTP:",sftpErrorText(),
					" on file: " + newFileName);

		}
		else
		{
			//CR 85 (SFTP API Replacement)
			ftpConnect(hostAddress, portNr, userName, passWord,ascii); // HM66365


			bool overwrite = false;
			if (overWrite == "yes")
			{
				overwrite = true;
			}

			AES_CDH_TRACE_MESSAGE("sending file %s ...", fileName.c_str());
			bool result = sftpApiObj->sftpSendFile(this, fileName.c_str(),newFileName.c_str(),
					remoteDir.c_str(),remoteSubDirName.c_str(),overwrite,ascii); // HM66365
			AES_CDH_TRACE_MESSAGE("sftpSendFile , file : %s , status :%d",fileName.c_str(),result);
			// if sendFile is failed
			if (!result)
			{
				// check if transmission is interrupted by afpfte command
				bool isInterrupted = sftpApiObj->isTransmissionInterrupted(this);
				// if transmission is interrupted return AES_CDH_TASKSTOPPED code
				if (isInterrupted)
				{
					Event::report(AES_CDH_remoteFileFault,
							"sftpSendFile() failed in SFTP:",sftpErrorText(),
							" on file: " + newFileName);
					returnCode = AES_CDH_RC_TASKSTOPPED;
				}

				else

				{
#if 0
					check the error code value and Exception Type
					ACE_INT32 sftpExceptionType = sftpApiObj->getSftpExceptionType();
					ACE_INT32 sftpErrorCode = sftpApiObj->getSftpOperationErrorCode();

					if (sftpErrorCode != SFTPAPI_OK)
					{
						// get error Message
						const ACE_TCHAR* errorMsg = sftpErrorText().c_str();

						// if the failed transmission depends on local system
						if (sftpExceptionType == LocalSystemException)
						{
							Event::report(AES_CDH_remoteFileFault,
									"LOCAL FILE FAULT",
									errorMsg,
									"when calling sftpSendFile() on file: " + newFileName);
						}
						else
						{
							// it means that a ssh/sftp exception occours
							Event::report(AES_CDH_remoteFileFault,
									"REMOTE FILE FAULT",
									errorMsg,
									"when calling sftpSendFile() on file: " + newFileName);
						}

					}
#endif
					Event::report(AES_CDH_remoteFileFault,
							"sftpSendFile() failed in SFTP:",sftpErrorText(),
							" on file: " + newFileName);
					returnCode =  AES_CDH_RC_SENDERR;
				}
			}
		}

	}  // end try catch begin
	catch(AES_CDH_ResultCode rc)
	{
		returnCode = rc;  // check this
	}

	ftpDisconnect();

	AES_CDH_TRACE_MESSAGE("Exiting");

	return returnCode;
}

/*===================================================================
   ROUTINE:sendDirectoryInitiating
=================================================================== */
//##ModelId=3DE49B4701DD
AES_CDH_ResultCode SSHFTPIV2::sendDirectoryInitiating(const string& dirName,
                                                   const string& remoteSubDirName,
                                                   const string& newDirName,
                                                   AES_CDH_DestinationSet::transferMode trMode, // QABULFG
                                                   const string& fileMask) // QABULFG)
{
	(void)trMode;
	(void) fileMask;
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string errorText;
	isTimeStamp = false;
	remoteFullPath = "";

	try
	{
		if (sftpApiObj->isTransmissionInterrupted(this))
		{
			returnCode = AES_CDH_RC_TASKSTOPPED;
			Event::report(AES_CDH_remoteFileFault,
					"sftpSendFile() failed in SFTP:",sftpErrorText(),
					" on file: " + newDirName);

		}
		else
		{
			ftpConnect(hostAddress, portNr, userName, passWord);

			// qabhall 010823        if (remoteSubDirName == "")
			if (newDirName == "") // qabhall 010823
			{
				string::size_type pos = dirName.find_last_of("//:");

				// If '/' or '\' or ':' is not found then copy whole filename
				if (pos == string::npos)
				{
					aRemoteDirName = dirName;
				}
				else
				{
					aRemoteDirName = dirName.substr(pos + 1);
				}
			}
			else
			{
				aRemoteDirName = newDirName;
			}
			directoryLevel = 0;


			bool result = sftpApiObj->sftpSendDir(this, dirName.c_str(),newDirName.c_str(),
					remoteDir.c_str(),remoteSubDirName.c_str());

			// if sendDir is failed
			if (!result)
			{
				// check if transmission is interrupted by afpfte command
				bool isInterrupted = sftpApiObj->isTransmissionInterrupted(this);
				// if transmission is interrupted return AES_CDH_TASKSTOPPED code
				if (isInterrupted)
				{
					returnCode = AES_CDH_RC_TASKSTOPPED;
				}

				else
				{
#if 0
					//check the error code value and Exception Type
					ACE_INT32 sftpExceptionType = sftpApiObj->getSftpExceptionType();
					ACE_INT32 sftpErrorCode = sftpApiObj->getSftpOperationErrorCode();

					if (sftpErrorCode != SFTPAPI_OK)
					{
						// get error Message
						const ACE_TCHAR* errorMsg = sftpErrorText().c_str();

						// if the failed transmission depends on local system
						if (sftpExceptionType == LocalSystemException)
						{
							Event::report(AES_CDH_remoteFileFault,
									"LOCAL FILE FAULT",
									errorMsg,
									"when calling sftpSendDir() on folder: " + newDirName);
						}
						else
						{
							// it means that a ssh/sftp exception occours
							Event::report(AES_CDH_remoteFileFault,
									"REMOTE FILE FAULT",
									errorMsg,
									"when calling sftpSendDir() on folder: " + newDirName);
						}

					}
#endif
					Event::report(AES_CDH_remoteFileFault,
							"sftpSendDir() failed in SFTP :",sftpErrorText(),
							" on folder: " + newDirName);
					returnCode =  AES_CDH_RC_SENDERR;
				}

			}
		}
	}
	catch(AES_CDH_ResultCode rc)
	{
		returnCode = rc;
	}
	ftpDisconnect();
	AES_CDH_TRACE_MESSAGE("Exiting");
	return returnCode;
}


/*===================================================================
   ROUTINE:doSendDirInitiating
=================================================================== */
//To be ported in future
AES_CDH_ResultCode SSHFTPIV2::doSendDirInitiating(string path,
		AES_CDH_DestinationSet::transferMode trMode, // qabulfg
		const string fileMask ) // qabulfg)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_DestinationSet::transferMode transferModeForSingleFile; // qabulfg
	bool bIsDir;
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
	string searchPattern = path ;
	DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if(pDir == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Exiting as opendir failed with error %d", errno);
		AES_CDH_TRACE_MESSAGE("Exiting as opendir failed with error %d", errno);
		return AES_CDH_RC_FILEERR;
	}
	struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
	directoryLevel += 1;

	try
	{  // qabsjen 010925 See catch below
		while (pEntry!=NULL)
		{
			if (directoryLevel == 1)
			{
				ftpCreateDirectory(aRemoteDirName);
#if 0
				if (!ftpChangeDirectoryDown(aRemoteDirName))

					throw AES_CDH_RC_SENDERR;
#endif
				directoryLevel += 1;
			}
			string filePath = path + "/" + string(pEntry->d_name);
			struct stat buf;
			stat(filePath.c_str(), &buf);
			bIsDir =S_ISDIR(buf.st_mode);
			if (!bIsDir)
			{
				// INGO3 GOH qabulfg start
				if (maskInFileName(fileMask, pEntry->d_name ))
				{
					transferModeForSingleFile = trMode;
				}
				else
				{
					if (trMode == AES_CDH_DestinationSet::TR_BINARY)
					{
						// set to opposite
						transferModeForSingleFile = AES_CDH_DestinationSet::TR_ASCII;
					}
					else
					{
						transferModeForSingleFile = AES_CDH_DestinationSet::TR_BINARY;
					}
				}

				returnCode = ftpSendFile(path + "/" + pEntry->d_name,pEntry->d_name,transferModeForSingleFile);

				// INGO3 GOH qabulfg end

				if(returnCode != AES_CDH_RC_OK)
					break;
			}
			else
			{
				if (! ((ACE_OS::strcmp(pEntry->d_name ,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name ,"..") == 0)))
				{

					// A sub directory found. Create it and change to it on remote site.
					// Make a recursive call to sendDir in order to send files and/or create new sub directories.
					ftpCreateDirectory(pEntry->d_name );
#if 0
					if (!ftpChangeDirectoryDown(pEntry->d_name ))
						throw AES_CDH_RC_SENDERR;
#endif
					returnCode = doSendDirInitiating(path + "/" + pEntry->d_name,trMode,fileMask); //qabulfg
					if (returnCode != AES_CDH_RC_OK)
						break;
				}
			}
			pEntry = ACE_OS::readdir(pDir);
		}
		if(pDir != NULL)
			closedir(pDir);
		if(returnCode == AES_CDH_RC_OK)
		{
			directoryLevel -= 1;
			if (directoryLevel >= 1)
			{
#if 0
				if (!ftpChangeDirectoryUp())
					throw AES_CDH_RC_SENDERR;
#endif
			}
		}

	}// try
	catch (AES_CDH_ResultCode rc)
	{
		// Added by qabsjen 010925 TR HC81779
		// Caught exception; Close handle and rethrow exception to let
		// sendDirectoryInitiating() handle it.
		// FindClose(hFind);
		if(pDir != NULL)
			closedir(pDir);

		throw rc;
	}

	AES_CDH_TRACE_MESSAGE("Exiting");
	return returnCode;
}
