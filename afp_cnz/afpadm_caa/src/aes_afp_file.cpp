#include <sstream>
#include <string>
#include <stdio.h>
#include <aes_afp_objectfactory.h>
#include <aes_afp_file.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(AES_AFP_file);

extern int errno_;

// Class aes_afp_file 

aes_afp_file::aes_afp_file()
: absoluteCreationTime_(0),
  absoluteDeletionTime_(0),
  isDirectory_(false),
  transferMode_(AES_BINARY),
  status_(AES_FSREADY),
  removeBefore_(false),
  sequenceCounter_(1),
  sourceDir_("")
{
	ACE_OS::memset(creationDate_, '\0', sizeof(creationDate_));
	ACE_OS::memset(readyDate_, '\0', sizeof(readyDate_));
	ACE_OS::memset(sendDate_, '\0', sizeof(sendDate_));
	ACE_OS::memset(failedDate_, '\0', sizeof(failedDate_));
	ACE_OS::memset(deleteDate_, '\0', sizeof(deleteDate_));
	ACE_OS::memset(noneDate_, '\0', sizeof(noneDate_));
	ACE_OS::memset(archiveDate_, '\0', sizeof(archiveDate_));
	ACE_OS::memset(stoppedDate_, '\0', sizeof(stoppedDate_));
	transferMask_ = "";
	originalFileName_ = ""; //HU96961
}


aes_afp_file::~aes_afp_file()
{
}

//	---------------------------------------------------------
//	       init()
//	---------------------------------------------------------
bool aes_afp_file::init (std::string tempDiscName, std::string afpDiscName, AES_GCC_Filestates status, AES_GCC_Errorcodes& error,std::string sourceDirPath,bool removeBefore, bool isDirectory,std::string originalFilename, std::string transferMask, AES_GCC_Format transferMode) 
{
	//HI70260 - REMOVEDELAY attribute has been moved to destinationset level, 
	//and aes_afp_file dont have removedelay attribute. Hence removeDelay has been removed from argument list
	//## begin aes_afp_file::init%371478C80309.body preserve=yes
	AES_AFP_TRACE_MESSAGE( "Status %d, isDirectory %s",status, isDirectory?"true":"false");
	bool result(false);

	//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	//and it will used for further calculations
	//	removeDelay_ = removeDelay;
	removeBefore_ = removeBefore;
	isDirectory_ = isDirectory;
	//	strcpy(transferMask_, transferMask.c_str() ); jost
	transferMask_ = transferMask;
	transferMode_ = transferMode;
	sourceDir_ = sourceDirPath;
	originalFileName_ = originalFilename;  //HU96961
	ACE_OS::strcpy(creationDate_, aes_afp_timestamp::Instance().getTimeChar() );

	absoluteCreationTime_ = aes_afp_timestamp::Instance().getAbsTime();

	// File is dumped to disc in setStatus
	result = setStatus(tempDiscName, afpDiscName, status, error);

	return result;
}
//	---------------------------------------------------------
//	       initFromFile()
//	---------------------------------------------------------
bool aes_afp_file::initFromFile (std::string path, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE( "File path %s", path.c_str());

	bool result(false);
	aes_afp_protocolfile protocol;

	protocol.clearValues();
	result = protocol.readFromFile(path, error);
	if (result == false)
	{
		return false;
	}

	status_ = (AES_GCC_Filestates)protocol.getIntValue("STATUS");
	//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	//and it will used for further calculations
	//	removeDelay_ = protocol.getIntValue("REMOVEDELAY");
	removeBefore_ = protocol.getBoolValue("REMOVEBEFORE");
	isDirectory_ = protocol.getBoolValue("ISDIRECTORY");
	//	strcpy(transferMask_, protocol.getCharStringValue("TRANSFERMASK") ); jost
	transferMask_ = protocol.getStringValue("TRANSFERMASK");
	transferMode_ = (AES_GCC_Format)protocol.getIntValue("TRANSFERMODE");
	ACE_OS::strcpy(creationDate_, protocol.getCharStringValue("CREATIONDATE") );
	ACE_OS::strcpy(readyDate_, protocol.getCharStringValue("READYDATE") );
	ACE_OS::strcpy(sendDate_, protocol.getCharStringValue("SENDDATE") );
	ACE_OS::strcpy(failedDate_, protocol.getCharStringValue("FAILEDDATE") );
	ACE_OS::strcpy(deleteDate_, protocol.getCharStringValue("DELETEDATE") );
	ACE_OS::strcpy(noneDate_, protocol.getCharStringValue("NONEDATE") );
	ACE_OS::strcpy(archiveDate_, protocol.getCharStringValue("ARCHIVEDATE") );
	originalFileName_ = protocol.getStringValue("ORIGFILENAME");                              //HU96961
	absoluteCreationTime_ = protocol.getLongValue("ABSOLUTECREATIONTIME");
	absoluteDeletionTime_ = protocol.getLongValue("ABSOLUTEDELETIONTIME");
	sourceDir_ = protocol.getStringValue("SOURCEDIRECTORY");

	return result;	
}

//	---------------------------------------------------------
//	       setStatus()
//	---------------------------------------------------------
bool aes_afp_file::setStatus (std::string& tempDiscName, std::string& afpDiscName, AES_GCC_Filestates status, AES_GCC_Errorcodes& error)
{
	bool result(false);

	switch (status)
	{
	case AES_FSREADY:
	{
		status_ = status;
		strcpy(readyDate_, aes_afp_timestamp::Instance().getTimeChar() );
		break;
	}
	case AES_FSSEND:
	{
		status_ = status;
		strcpy(sendDate_, aes_afp_timestamp::Instance().getTimeChar() );
		break;
	}
	case AES_FSFAILED:
	{
		status_ = status;
		strcpy(failedDate_, aes_afp_timestamp::Instance().getTimeChar() );
		break;
	}
	case AES_FSDELETE:
	{
		status_ = status;
		strcpy(deleteDate_, aes_afp_timestamp::Instance().getTimeChar() );
		absoluteDeletionTime_ = aes_afp_timestamp::Instance().getAbsTime();
		break;
	}
	case AES_FSNONE:
	{
		status_ = status;
		strcpy(noneDate_, aes_afp_timestamp::Instance().getTimeChar() );
		break;
	}
	case AES_FSARCHIVE:
	{
		status_ = status;
		strcpy(archiveDate_, aes_afp_timestamp::Instance().getTimeChar() );
		break;
	}
	case AES_FSSTOPPED:
	{
		status_ = status;
		strcpy(stoppedDate_, aes_afp_timestamp::Instance().getTimeChar() );
		break;
	}
	case AES_FSPENDING:
	{
		status_ = status;
		break;
	}
	case AES_FSUSEDEFAULT:
	{
		break;
	}
	default:
	{
		status_ = status;
		strcpy(noneDate_, aes_afp_timestamp::Instance().getTimeChar() );
		break;
	}
	}

	result = dumpToFile_new(afpDiscName, error);
	//If dumpToFile fail, set status to FAILED.
	//Note, this can result in duplicated files if AFP is restarted/(afpfti -f).
	if (!result)
	{
		status_ = AES_FSFAILED;

		AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to save data on disk() Status %d afpDiscName %s",
				status, afpDiscName.c_str());
		AES_AFP_TRACE_MESSAGE("Failed to save data on disk() Status %d tempDiscName %s afpDiscName %s",
				status, tempDiscName.c_str(), afpDiscName.c_str());
		Event::report(	EVENT_NOACCESSAFP,
				"FAILED TO UPDATE AFP SYSTEM FILE",
				"tempDiscName: " + tempDiscName + " AFPDiscName: " + afpDiscName,
				"Write access to senditem file failed.");
	}	
	return result;
}

//	---------------------------------------------------------
//	       getStatus()
//	---------------------------------------------------------
AES_GCC_Filestates aes_afp_file::getStatus ()const
{
	return status_;
}
//      ---------------------------------------------------------
//             getOriginalFileName()
//      ---------------------------------------------------------

std::string aes_afp_file::getOriginalFileName()
{
	return originalFileName_;			//HU96961
}
//	---------------------------------------------------------
//	       remove()
//	---------------------------------------------------------
bool aes_afp_file::removeFile (std::string& afpDiscName, AES_GCC_Errorcodes& error)
{

	AES_AFP_TRACE_MESSAGE("Entering");

	int result(0);
	bool resultRemoveDir = false;
	//Code change start for TR HK91965
	//Code change start for TR HK91965

	ACE_stat fstat;
	if(ACE_OS::stat(afpDiscName.c_str(),&fstat) == 0)
	{
		if(fstat.st_mode & S_IFDIR)
		{
			result = doDirDelete(afpDiscName.c_str());                         //TR HK91965 for the removal of the directories.
			if (result)
			{
				if(ACE_OS::stat(afpDiscName.c_str(),&fstat) == 0) //HV56130 start 
                                {
                                        resultRemoveDir = ACE_OS::rmdir(afpDiscName.c_str());

                                }
                                else
                                {
                                        AES_AFP_LOG(LOG_LEVEL_WARN,"Dir %s  not exists",afpDiscName.c_str()); //HV56130 end
                                }

			}
			if (resultRemoveDir != 0)
			{
				result = false;
				Event::report(EVENT_FILENOTDEL, "REMOVE FAULT", afpDiscName, "Not able to delete file or directory: " + afpDiscName);
			}
		}
		else
		{
			if( ACE_OS::unlink(afpDiscName.c_str()) == 0 )
			{
				result = true;
			}
		}
	}

	if (result == true)
	{
		error = AES_NOERRORCODE;
		AES_AFP_TRACE_MESSAGE("Leaving");
		return true;
	}

	std::ostringstream err_back;
	err_back << " errno : " << errno;
	Event::report(EVENT_FILENOTDEL, "REMOVE FAULT", afpDiscName, "Not able to delete file or directory: " + afpDiscName + err_back.str());

	//if (errno == EACCES)
	//{
	//	error = AES_NOACCESS;
	//	GCC_TERROR((aes_afp_File,"Leaving removeFile"));
	//	return false;
	//}

	if (errno == ENOENT)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"File does not exists, no need to remove ");
		AES_AFP_TRACE_MESSAGE("File does not exists, no need to remove ");
		error = AES_NOERRORCODE;
		// If file not found there is no meaning with returning false,
		// since the files is to be removed.
		AES_AFP_TRACE_MESSAGE("Leaving");
		return true;
	}

	else 
	{
		error= AES_NOACCESS;
		AES_AFP_LOG(LOG_LEVEL_DEBUG,"Leaving with errono %d",errno);
		AES_AFP_TRACE_MESSAGE("Leaving with errono %d",errno);
		return false;
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
	return false;
}

//	---------------------------------------------------------
//	       fillFileInfo()
//	---------------------------------------------------------
//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
//and //and it will used for further calculations
void aes_afp_file::fillFileInfo (std::string& listBuffer, int removeDelay)
{
	long tmpRemDelay(0);
	//long tmp(0);
	char tmpStr[32];
	char tmpChar[5];  //Increased the size to prevent crash while giving afpls...TR HK47895
	std::string tmpValue("");

	ACE_OS::sprintf(tmpChar, "%d\n", isDirectory_);
	listBuffer += tmpChar;

	ACE_OS::sprintf(tmpChar, "%d\n", status_);
	listBuffer += tmpChar;

	//HI70260
	if ( removeDelay != 0 )
		tmpRemDelay = calcRemDelay(removeDelay);
	else
		tmpRemDelay = 0;

	if (tmpRemDelay == -1)
	{
		ACE_OS::sprintf(tmpStr, "%ld\n", tmpRemDelay);
	}
	else
	{
		long tmp = tmpRemDelay / 60;
		if (tmpRemDelay % 60 > 51)
		{
			tmp++;
		}
		ACE_OS::sprintf(tmpStr, "%lu\n", tmp);
	}

	listBuffer += tmpStr;
}

//## Semantics:
//	---------------------------------------------------------
//	       calcRemDelay()
//	---------------------------------------------------------
//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
//and it will used for further calculations to delete the files
long aes_afp_file::calcRemDelay (ACE_INT32 removeDelay)
{
	ACE_UINT64 deleteSeconds(0);
	ACE_UINT64 todaySeconds(0);
	ACE_INT64 calcTime(0);
	//	ACE_UINT64 tmpAbsoluteCreationTime(0);

	if (status_ != AES_FSDELETE && removeBefore_ == false)
	{
		return (removeDelay * 60);
	}

	if (removeDelay == -1)
	{
		return -1;
	}

	todaySeconds = aes_afp_timestamp::Instance().getAbsTime();

	if (removeBefore_ == true)
	{
		deleteSeconds = absoluteCreationTime_;
	}
	else
	{
		deleteSeconds = absoluteDeletionTime_;
	}

	deleteSeconds = (removeDelay * 60) + deleteSeconds;
	calcTime = (deleteSeconds - todaySeconds);
	if (calcTime < 0)
	{
		calcTime = 0;
	}
	return calcTime;
}

//## Semantics:
//	---------------------------------------------------------
//	       readyForDelete()
//	---------------------------------------------------------
//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
//and it will used for further calculations to delete the file
bool aes_afp_file::readyForDelete (AES_GCC_Errorcodes& error, int removeDelay)
{
	bool result(false);

	if ( (status_ ==  AES_FSDELETE || status_ == AES_FSREMOVE) && (removeDelay == 0 || calcRemDelay(removeDelay) == 0) ) //HI70260
	{
		// Status file removal is moved to separate thread.
		result =true;
	}
	error = AES_NOERRORCODE;
	return result;
}

//## Operation: getFile%37650211032C; C++
//## Semantics:
//	---------------------------------------------------------
//	       getFile()
//	---------------------------------------------------------
void aes_afp_file::getFile (aes_afp_datablock* dBlock)
{
	dBlock->setArchiveDate(aes_afp_timestamp::Instance().removeSequenceNumber(archiveDate_) );
	dBlock->setDeleteDate(aes_afp_timestamp::Instance().removeSequenceNumber(deleteDate_) );
	dBlock->setFailedDate(aes_afp_timestamp::Instance().removeSequenceNumber(failedDate_) );
	dBlock->setNoneDate(aes_afp_timestamp::Instance().removeSequenceNumber(noneDate_) );
	dBlock->setReadyDate(aes_afp_timestamp::Instance().removeSequenceNumber(readyDate_) );
	dBlock->setSendDate(aes_afp_timestamp::Instance().removeSequenceNumber(sendDate_) );
	dBlock->setStoppedDate(aes_afp_timestamp::Instance().removeSequenceNumber(stoppedDate_) );
	dBlock->setStatus(status_);
	//Set the directory flag 
	if( isDirectory_ == true )
	{
		dBlock->setDirectoryFlag();
	}
}

//## Operation: getCreationDate%37E48F3202B0; C++
//## Semantics:
//	---------------------------------------------------------
//	       getCreationDate()
//	---------------------------------------------------------
std::string aes_afp_file::getCreationDate ()const
{
	return creationDate_;
}

//## Operation: nextSeqNrCreationDate; C++
//## Semantics:
//	---------------------------------------------------------
//	       nextSeqNrCreationDate()
//	---------------------------------------------------------
std::string aes_afp_file::nextSeqNrCreationDate ()
{
	ACE_OS::snprintf(&creationDate_[14],6,"%06d", sequenceCounter_++);

	if(sequenceCounter_ > 999999)
	{
		sequenceCounter_ = 0;
	}

	return creationDate_;
}

//## Semantics:
//	---------------------------------------------------------
//	       getDirectoryFlag()
//	---------------------------------------------------------
bool aes_afp_file::getDirectoryFlag ()const
{
	return isDirectory_;
}

//## Semantics:
//	---------------------------------------------------------
//	       getTransferMask()
//	---------------------------------------------------------
std::string aes_afp_file::getTransferMask ()const
{
	return transferMask_;
}

//## Semantics:
//	---------------------------------------------------------
//	       getTransferMode()
//	---------------------------------------------------------
AES_GCC_Format aes_afp_file::getTransferMode ()const
{
	return transferMode_;
}
std::string aes_afp_file::getSourceDirPath()const
{
	return sourceDir_;
}

//## Semantics:
//	---------------------------------------------------------
//	       dumpToFile()
//	---------------------------------------------------------
bool aes_afp_file::dumpToFile (std::string& tempPath, std::string& path, AES_GCC_Errorcodes& error)
{
	aes_afp_protocolfile protocol;
	bool result(false);

	protocol.clearValues();

	protocol.addStringValue("CREATIONDATE",  creationDate_);
	protocol.addStringValue("READYDATE",  readyDate_);
	protocol.addStringValue("SENDDATE",  sendDate_);
	protocol.addStringValue("FAILEDDATE",  failedDate_);
	protocol.addStringValue("DELETEDATE",  deleteDate_);
	protocol.addStringValue("NONEDATE",  noneDate_);
	protocol.addStringValue("ARCHIVEDATE",  archiveDate_);

	protocol.addLongValue("ABSOLUTECREATIONTIME", absoluteCreationTime_);
	protocol.addLongValue("ABSOLUTEDELETIONTIME", absoluteDeletionTime_);

	//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	//and aes_afp_file dont have removedelay attribute
	//	protocol.addIntValue("REMOVEDELAY", removeDelay_);
	protocol.addBoolValue("REMOVEBEFORE", removeBefore_);
	protocol.addBoolValue("ISDIRECTORY", isDirectory_);
	protocol.addStringValue("TRANSFERMASK", transferMask_);
	protocol.addIntValue("TRANSFERMODE", transferMode_);
	protocol.addIntValue("STATUS", status_);
	protocol.addStringValue("SOURCEDIRECTORY",  sourceDir_);
	protocol.addStringValue("ORIGFILENAME",originalFileName_);   //HU96961

	AES_AFP_TRACE_MESSAGE("dumpToFile() Dumping to path %s", path.c_str());
	result = protocol.writeToFile(tempPath, path, error);
	return result;
}

//## Semantics:
//	---------------------------------------------------------
//	       isYounger()
//	---------------------------------------------------------

bool aes_afp_file::isYounger(aes_afp_file & fileObj)
{
	char thisChar_, fileObjChar_;

	for(int i = 0;i < 18 ;i++)
	{
		thisChar_	 = this->creationDate_[i];
		fileObjChar_ = fileObj.creationDate_[i];

		if(thisChar_ == fileObjChar_)
		{
			continue;

		}else if(thisChar_ < fileObjChar_)
		{
			return false;

		}else if(thisChar_ > fileObjChar_)
		{
			return true;
		}
	}

	return false; //Impossible, this means that they are the same age and has the same seqnr.
}

//added by Deb

void aes_afp_file::recover_data(){
	ACE_OS::strcpy(creationDate_, aes_afp_timestamp::Instance().getTimeChar() );
	absoluteCreationTime_ = aes_afp_timestamp::Instance().getAbsTime();
	ACE_OS::strcpy(readyDate_, aes_afp_timestamp::Instance().getTimeChar() );
	ACE_OS::strcpy(sendDate_, aes_afp_timestamp::Instance().getTimeChar() );
	if(status_ == AES_FSDELETE){
		absoluteDeletionTime_ = aes_afp_timestamp::Instance().getAbsTime();
		ACE_OS::strcpy(deleteDate_, aes_afp_timestamp::Instance().getTimeChar() );
		//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
		//and aes_afp_file dont have removedelay attribute
		//	   removeDelay_= 10;
	}
	else{
		absoluteDeletionTime_ = 0;
		ACE_OS::strcpy(failedDate_, aes_afp_timestamp::Instance().getTimeChar() );
		status_= AES_FSREADY; //HK47851 - Made the status of Corrupted file to READY in stead of DELETE to resend it again..

	}
}

bool aes_afp_file::dumpToFile_new (std::string& tempPath,AES_GCC_Errorcodes& error)
{
	aes_afp_protocolfile protocol;
	bool result(false);

	protocol.clearValues();

	protocol.addStringValue("CREATIONDATE",  creationDate_);
	protocol.addStringValue("READYDATE",  readyDate_);
	protocol.addStringValue("SENDDATE",  sendDate_);
	protocol.addStringValue("FAILEDDATE",  failedDate_);
	protocol.addStringValue("DELETEDATE",  deleteDate_);
	protocol.addStringValue("NONEDATE",  noneDate_);
	protocol.addStringValue("ARCHIVEDATE",  archiveDate_);
	protocol.addLongValue("ABSOLUTECREATIONTIME", absoluteCreationTime_);
	protocol.addLongValue("ABSOLUTEDELETIONTIME", absoluteDeletionTime_);

	//HI70260 - REMOVEDELAY attribute has been moved to destinationset level,
	//and aes_afp_file dont have removedelay attribute
	//	protocol.addIntValue("REMOVEDELAY", removeDelay_);
	protocol.addBoolValue("REMOVEBEFORE", removeBefore_);
	protocol.addBoolValue("ISDIRECTORY", isDirectory_);
	protocol.addStringValue("TRANSFERMASK", transferMask_);
	protocol.addIntValue("TRANSFERMODE", transferMode_);
	protocol.addIntValue("STATUS", status_);
	protocol.addStringValue("SOURCEDIRECTORY", sourceDir_);
	protocol.addStringValue("ORIGFILENAME",originalFileName_);                //HU96961

	AES_AFP_TRACE_MESSAGE("Dumping to path %s", tempPath.c_str());
	result = protocol.writeToFile_new(tempPath,error);
	return result;
} 

bool aes_afp_file::isFileStatusDataValid()
{
	if(ACE_OS::strcmp(deleteDate_,"") && ACE_OS::strcmp(sendDate_,"") && (status_!= AES_FSDELETE)){
		status_ = AES_FSDELETE;
		return false;
	}
	if(!ACE_OS::strcmp(creationDate_, "") || (absoluteCreationTime_ == 0))
		return false;
	if(status_ == AES_FSREADY && !ACE_OS::strcmp(readyDate_,""))
		return false;
	if(status_ == AES_FSSEND && (!ACE_OS::strcmp(readyDate_,"") || (!ACE_OS::strcmp(sendDate_,""))))
		return false;
	if(status_ == AES_FSFAILED && (!ACE_OS::strcmp(readyDate_,"") ||(!ACE_OS::strcmp(sendDate_,"")) || (!strcmp(failedDate_,""))))
		return false;
	//Fix for HH33276 - when creationDate is also empty then only we will consider the file as corruptedFile.
	if(status_ == AES_FSDELETE && !ACE_OS::strcmp(creationDate_, "") && (!ACE_OS::strcmp(readyDate_,"") || (!strcmp(sendDate_,"")) || (!strcmp(deleteDate_,"")) || (absoluteDeletionTime_ == 0)))
		return false;
	if((ACE_OS::strcmp(sendDate_,"") || ACE_OS::strcmp(failedDate_,"") || ACE_OS::strcmp(deleteDate_,"")) && (status_ == AES_FSREADY)){
		return false;
	}
	return true;
}
///	------------------------------------------------------------------------
//								doDirDelete()
//				For the removal of sub directories and files
//	------------------------------------------------------------------------
//  Returns true,if the function deletes all the files and sub directories.
//  Returns false if the function fails to delete any file or sub directory.

bool aes_afp_file::doDirDelete(std::string path)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	DIR *dir;
	struct dirent * finddata;
	std::string searchPattern = path;
	std::string fileN;
	dir = opendir(searchPattern.c_str());
	char * problemdata = new char[150];
	static int dirCounter = 0;
	AES_AFP_TRACE_MESSAGE("path = %s", path.c_str());
	if (dir == NULL)
	{
		ACE_OS::sprintf(problemdata,"Not able to open file or directory: %s" , searchPattern.c_str());
	}

	else
	{
		dirCounter++;
		while ((finddata = readdir(dir)) != NULL)
		{
			if (strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, ".."))

			{
				fileN = path +  aes_afp_parameter::delimiter() + finddata->d_name;
				struct stat stat_buff;
				stat(fileN.c_str(),&stat_buff);
				if(S_ISDIR(stat_buff.st_mode))
				{
					fileN = path +"/"+ finddata->d_name;
					doDirDelete(fileN);
					if(ACE_OS::rmdir(fileN.c_str())==0)
					{
						AES_AFP_TRACE_MESSAGE("Directory deleted , dir path : %s",fileN.c_str());

					}
					else
					{
						Event::report(EVENT_FILENOTDEL,"REMOVE FAULT",fileN, "Not able to delete file or directory : "+fileN);
						AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to delete directory , dir path : %s , errno : %d",fileN.c_str(),ACE_OS::last_error());
						AES_AFP_TRACE_MESSAGE("Failed to delete directory , dir path : %s , errno : %d",fileN.c_str(),ACE_OS::last_error());
						if(dir != NULL)
							closedir(dir);
						if(problemdata != 0)
						{
							delete[] problemdata;
							problemdata = 0;
						}
						return false;
					}
				}
				else
				{
					fileN = path +"/"+ finddata->d_name;
					if(fileN.c_str()!=NULL)
					{
						int status  = ACE_OS::unlink(fileN.c_str());
						if(status !=0)
						{
							AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to delete file , file path : %s , errno : %d",fileN.c_str(),ACE_OS::last_error());
							AES_AFP_TRACE_MESSAGE("Failed to delete file , file path : %s , errno : %d",fileN.c_str(),ACE_OS::last_error());
							Event::report(EVENT_FILENOTDEL,"REMOVE FAULT",fileN, "Not able to delete file or directory : "+fileN);
							if(dir != NULL)
								closedir(dir);
							if(problemdata != 0)
							{
								delete[] problemdata;
								problemdata = 0;
							}

							return false;
						}

						else
						{
							AES_AFP_TRACE_MESSAGE("file deleted , file path : %s",fileN.c_str());

						}


					}

				}
			}
		}
		dirCounter--;
		if(dir != NULL)
			closedir(dir);
	}
	if(problemdata != 0)
	{
		delete[] problemdata;
		problemdata = 0;
	}

	if( dirCounter == 0 )
	{
		if( !ACE_OS::rmdir( path.c_str() ) ) //Here, for windows a non-zero value means success; where as in linux 0 is returned for success
		{
			AES_AFP_TRACE_MESSAGE("Directory deleted , dir path : %s",path.c_str());
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to delete directory , dir path : %s , errno : %d",path.c_str(),ACE_OS::last_error());
			AES_AFP_TRACE_MESSAGE("Failed to delete directory , dir path : %s , errno : %d",path.c_str(),ACE_OS::last_error());
			return false;
		}
	}
	return true;
}
