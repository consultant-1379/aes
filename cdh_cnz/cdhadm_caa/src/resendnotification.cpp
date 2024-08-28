/*=================================================================== */
/**
@file resendnotification.cpp

Class method implementation for resendnotification.h

DESCRIPTION
This class will check that all files that are reported to
a remote host with file notification also is fetched. If the
file is not fetched in a certatin number of days, a new file
notification is sent to remind the remote host to fetch the file.

This class supervise that files ackknowledged by file
notification also is fetched at the FTP area

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
#include <resendnotification.h>
#include <cdhcriticalsection.h>
#include <notificationrecord.h>
#include <messageblock.h>
#include <transdest.h>
#include <servr.h>
#include <event.h>
#include <list>
#include <sys/types.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
/*===================================================================
                        DECLARATION SECTION
=================================================================== */
// ACS Trace definition
AES_CDH_TRACE_DEFINE(AES_CDH_ResendNotification);

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define WORK_INTERVAL (3600)
#define WAIT_TIME_BETWEEN_DESTINATIONS (2)

/*===================================================================
   ROUTINE:ResendNotification
=================================================================== */
ResendNotification::ResendNotification():
			nextScanTime_(0),
			working_(false),
			workingState_(WORKING_OFF)
{	
}

/*===================================================================
   ROUTINE:~ResendNotification
=================================================================== */
ResendNotification::~ResendNotification()
{
}

/*===================================================================
   ROUTINE:open
=================================================================== */
ACE_INT32 ResendNotification::open(ACE_Thread_Manager *thrmgr)
{
	AES_CDH_LOG(LOG_LEVEL_INFO,"ResendNotification activating thread");
	this->thr_mgr(thrmgr);

	ACE_INT32 result = this->activate(THR_NEW_LWP|THR_DETACHED);

	nextScanTime_ = time(NULL) + WORK_INTERVAL;

	if (result == -1)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR activating resendnotification thread, result %d", result);
		AES_CDH_TRACE_MESSAGE("ERROR activating resendnotification thread, result %d", result);
		return result;
	}
	AES_CDH_LOG(LOG_LEVEL_INFO,"ResendNotification thread spawned successfully");
	AES_CDH_TRACE_MESSAGE("ResendNotification thread spawned successfully");
	return result;
}

/*===================================================================
   ROUTINE:close
=================================================================== */
ACE_INT32 ResendNotification::close(ACE_UINT64 flags)
{
	(void)flags;
	AES_CDH_TRACE_MESSAGE( "Entering ");
	AES_CDH_TRACE_MESSAGE( "Leaving ");
	return 0;
}


/*===================================================================
   ROUTINE:svc
=================================================================== */
ACE_INT32 ResendNotification::svc(void)
{
	ACE_Message_Block *recvNotifMess; // receive
	MessageBlock *recvNotifMsg;
	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeOut(msg_queue_->gettimeofday());
	timeOut += ACE_Time_Value(WAIT_TIME_BETWEEN_DESTINATIONS,0);

	ACE_INT32 queue_result(0);

	AES_CDH_TRACE_MESSAGE("Entering");

	for(;;) // ever...
	{
		timeOut = (msg_queue_->gettimeofday());
		timeOut += ACE_Time_Value(WAIT_TIME_BETWEEN_DESTINATIONS,0);
		queue_result = getq(recvNotifMess,&timeOut);

		if (queue_result != -1)
		{
			recvNotifMsg = (MessageBlock*) recvNotifMess;

			// terminate
			if (recvNotifMsg->msgType() == MT_HANGUP)
			{
				recvNotifMess->release();
				return 0;
			}
		}

		Work();
	} // for
	return 0;
}


/*===================================================================
   ROUTINE:define
=================================================================== */
AES_CDH_ResultCode ResendNotification::define(void)
{
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	return rCode;
}

/*===================================================================
   ROUTINE:sendToDataBase
=================================================================== */
bool ResendNotification::sendToDataBase(const string &fileName, const string &destName, const string &fullPath, const string &storePath, const string &hostAddress, const ACE_UINT32 maxAge)
{
	//AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

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

	ACE_INT32 errors = 0;
	std::string fileSizePath = fullPath;
	ACE_UINT64 fSize = getFileSize(fileSizePath, dbFileName, errors);

	if (errors != 0)
	{
		// Error(s) occurred
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Errors occurred while calculating file size");
		AES_CDH_TRACE_MESSAGE("Errors occurred while calculating file size");
	}

	char fileSize[20];
	ACE_OS::sprintf(fileSize, "%lu", fSize);

	// Create the record to be stored
	NotificationRecord *rec = new NotificationRecord;
	if(rec != NULL)
	{
		rec->setStoreFileName(dbFileName);
		rec->setFileSize(fileSize);
		rec->setMessageNumber(1);
	}
	else
	{
		// Not able to allocate memory for "rec"
		Event::report(AES_CDH_memoryFault, "MEMORY ALLOCATION PROBLEM",
				"Storage space for NotificationRecord could not be allocated",
				"-");

		return AES_CDH_RC_ERROR;
	}

	CDHCriticalSection::instance().enter();
	DataBase *db = DataBase::instance();
	bool bSuccess = db->add(destName,
			dbFileName,
			fileSize,
			rec,
			hostAddress,
			storePath,
			fullPath,
			true,
			maxAge);

	CDHCriticalSection::instance().leave();

	return bSuccess;
}

/*===================================================================
   ROUTINE:getFileSize
=================================================================== */
ACE_UINT64
ResendNotification::getFileSize(const string &path, const string &fName, ACE_INT32 &errors)
{
	//
	// path:   this is a path to a file or a directory
	//
	// errors: should be zero, if not the value indicates the number
	//         of errors that occurred while processing
	//

	ACE_HANDLE fh;
	string fileName("");
	string workPath("");
	string searchPattern = path;

	if (!fName.empty())
	{
		searchPattern += "/" + fName;
	}

	ACE_UINT64 totSize = 0;
	ACE_UINT64 recFileSize = 0;

	DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if ( pDir == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "%s  in open directory failed! with error %d",searchPattern.c_str(), errno);
		AES_CDH_TRACE_MESSAGE("%s  in open directory failed! with error %d",searchPattern.c_str(), errno);
		errors++;
	}
	else
	{
		bool bIsDir;
		struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);

		if (pEntry==NULL)
			errors++;

		while (pEntry!=NULL)
		{
			fileName.assign(path).append("/").append(pEntry->d_name);
			struct stat buf;
			stat(fileName.c_str(), &buf);
			bIsDir = S_ISDIR(buf.st_mode);
			if (!bIsDir)
			{
				if ( (fh = ACE_OS::open(fileName.c_str(), O_RDONLY, S_IREAD)) != -1)
				{
					totSize += (ACE_UINT64)ACE_OS::filesize(fh);
					int r = ACE_OS::close(fh);
					if( r != 0)
					{
						AES_CDH_LOG(LOG_LEVEL_INFO, "%s  in ACE_OS::close failed! %d",fileName.c_str(), errno);
						AES_CDH_TRACE_MESSAGE("%s  in ACE_OS::close failed! %d",fileName.c_str(), errno);
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
				if (! ((ACE_OS::strcmp(pEntry->d_name,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name,"..") == 0)))
				{
					// A sub directory found. Make a recursive call to getFileSize
					recFileSize += getFileSize(fileName.append("/"), "", errors);
				}
			}

			pEntry = ACE_OS::readdir(pDir);
		}
		closedir(pDir);
	}
	return recFileSize + totSize;
}


/*===================================================================
   ROUTINE:Work
=================================================================== */
void ResendNotification::Work(void)
{
	if(working_)
	{
		switch(workingState_)
		{
		case WORKING_START:
		{
			AES_CDH_TRACE_MESSAGE("RESENDNOTIFICATION::Work: START");

			lDestNames_.clear();

			const std::list<TransDest *> *destlist = ServR::getDestList();
			if(destlist != NULL)
			{
				for(std::list<TransDest *>::const_iterator li = destlist->begin(); li != destlist->end(); ++li)
				{
					if((*li)->isNotificationDefined())
					{
						string destName = (*li)->getDestinationName();
						lDestNames_.push_back(destName);
					}
				}
			}

			lDestNamesIt_ = lDestNames_.begin();
			workingState_ = WORKING_RUNNING;
			break;
		}
		case WORKING_RUNNING:
		{
			AES_CDH_TRACE_MESSAGE("RESENDNOTIFICATION::Work: RUNNING");

			if(lDestNamesIt_ != lDestNames_.end())
			{
				string destName = *lDestNamesIt_;
				string fullPath;
				string storePath;
				string hostAddress;
				ACE_UINT32 maxAge;

				CDHCriticalSection::instance().enter();
				DataBase::instance()->getFileFullPath(destName, fullPath);
				DataBase::instance()->getFileStorePath(destName, storePath);
				DataBase::instance()->getHostName(destName, hostAddress);
				DataBase::instance()->getFileMaxAge(destName, maxAge);
				CDHCriticalSection::instance().leave();

				AES_CDH_TRACE_MESSAGE("destName: %s fullPath: %s storePath: %s hostAdress: %s maxAge: %d ", destName.c_str(),fullPath.c_str(),storePath.c_str(),hostAddress.c_str(),maxAge);
				if(maxAge > 0) // maxAge == 0  means feature is disabled
				{
					// Check "Ready" dir
					CheckDir(destName, fullPath, storePath, hostAddress, maxAge, false);

					if(ChangePath(fullPath))
					{
						// Check "Send" dir
						CheckDir(destName, fullPath, storePath, hostAddress, maxAge, true);
					}
				}

				++lDestNamesIt_;
			}
			else
			{
				working_ = false;
				nextScanTime_ = time(NULL) + WORK_INTERVAL;
				workingState_ = WORKING_OFF;
			}
			break;
		}
		default:
		{
			working_ = false;
			nextScanTime_ = time(NULL) + WORK_INTERVAL;
			workingState_ = WORKING_OFF;
			break;
		}
		}
	}
	else
	{
		if(time(NULL)>nextScanTime_)
		{
			working_ = true;
			workingState_ = WORKING_START;
		}
	}
}

/*===================================================================
   ROUTINE:CheckDir
=================================================================== */
//
//    checkSend - If true => checks "Send" dir instead of "Ready" dir
//  ---------------------------------------------
void ResendNotification::CheckDir(const string& destName, const string& fullPath, const string& storePath, const string& hostAddress, const ACE_UINT32 maxAge, const bool checkSend)
{
	string searchPath = fullPath;
	DIR* pDir = opendir ( (ACE_TCHAR*)searchPath.c_str());
	if ( pDir == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, " %s  in open directory failed with error %d!",searchPath.c_str(), errno);
		AES_CDH_TRACE_MESSAGE("%s  in open directory failed with error %d!",searchPath.c_str(), errno);
		return;
	}
	struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);

	if(pEntry == NULL)
	{
		if(!checkSend)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "No files in Ready dir");
			AES_CDH_TRACE_MESSAGE("No files in Ready dir");
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "No files in Send dir");
			AES_CDH_TRACE_MESSAGE("No files in Send dir");
		}
		if(pDir != NULL)
			closedir(pDir);
		return;
	}

	while(pEntry != NULL)
	{
		// Process file/dir
		string fileName = pEntry->d_name;

		if (! ((ACE_OS::strcmp(pEntry->d_name,".") == 0)  || (ACE_OS::strcmp(pEntry->d_name,"..") == 0)))
		{
			ACE_INT32 errors = 0;
			ACE_UINT32 filesize = getFileSize(fullPath, fileName.c_str(), errors);
			/* Current system date time    */
			time_t rawtime(NULL);
			time(&rawtime);
			struct tm *currentTime = gmtime(&rawtime);
			ACE_stat stp;
			string filePath = fullPath + "/" + string(pEntry->d_name);
			if(0 == ACE_OS::stat(filePath.c_str(), &stp))
			{
				ACE_UINT32 seconds = CalcRelativeTime(stp.st_mtime,currentTime);

				AES_CDH_TRACE_MESSAGE("FileName: %s Size: %u Seconds: %u ", fileName.c_str(),filesize,seconds);

				// Convert from hours to seconds
				ACE_UINT32 maxAgeInSec = maxAge * 3600; // Use this line

				if(!checkSend)
				{
					// Should we resend the file notification
					//           if(seconds > maxAgeInSec)
					if((seconds > maxAgeInSec) && ((seconds%maxAgeInSec)<3600))
					{
						sendToDataBase(fileName, destName, fullPath, storePath, hostAddress, maxAge);

					}
				}
				else
				{
					// Create "trace" if file/dir is left in Send directory
					if(seconds > maxAgeInSec)
					{
						AES_CDH_TRACE_MESSAGE("%s  in Send directory is to old!",fileName.c_str());
					}
				}
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "stat failed for %s",filePath.c_str());
			}

		}
		// Get next file/dir
		pEntry = ACE_OS::readdir(pDir);
	}
	if(pDir != NULL)
		closedir(pDir);
}

/*===================================================================
   ROUTINE:CalcRelativeTime
=================================================================== */
//
//    Calculate the differance (in seconds) between the currentTime and the creationTime
//  ---------------------------------------------
ACE_UINT32 ResendNotification::CalcRelativeTime(const time_t creationTime,struct tm *currentTime)
{
	time_t sec =mktime(currentTime);
	ACE_UINT32 seconds = static_cast<ACE_INT32>(sec-creationTime);
	return seconds;
}
/*===================================================================
   ROUTINE:ChangePath
=================================================================== */
//
//    Change the last part of the path from "Ready" to "Send"
//  ---------------------------------------------
bool ResendNotification::ChangePath(string &path)
{
	string strSend("Send");

	ACE_INT32 index = path.rfind("/Ready");

	if(-1 == index)
	{
		return false;
	}
	path.replace(index+1, 5, strSend);
	return true;
}
