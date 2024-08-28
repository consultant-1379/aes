/*=================================================================== */
	/**
	@file   aes_ohi_filehandlerimplementation.cpp

	@brief
	A wrapper to aes_afp_api to send files and directories.
	General rule: The methods returns an unsigned int as error code
	to indicate any error. See AES_OHI_Errorcodes
	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       26/08/2011     XNADNAR       Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_afp_api.h"
//#include "aes_gcc_filedestprotection.h"
#include "aes_ohi_filehandlerimplementation.h"	
#include "aes_ohi_filehandler_r2.h"	
#include "ohiafpcomm.h"	
#include "aes_gcc_log.h"
#include <iostream>
#define STS_APPLICATION 1
using namespace std;

/*===================================================================
                        DEFINE DECLARATION SECTION
=================================================================== */
#define MAXCOUNT 10000  // number of outstanding messages in the que

// Declaration of trace points
GCC_TDEFL(aes_ohi_api_file, "C400");
GCC_TDEFL(aes_ohi_api_file_send,"C400");
GCC_TDEFL(aes_ohi_api_file_gettransferstateex,"C800");

/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
aes_ohi_fileHandlerImplementation::aes_ohi_fileHandlerImplementation(
		aes_ohi_fileHandler_r2 *userFileDest,
		const char* subSys,
		const char* appName,
		const char* fileDest,
		const char* eventText,
		const char* path) : userFile(0), user2File(0)
{
	errorInSemaphore = AES_OHI_NOERRORCODE;
	subSystem = subSys;
	applicationName = appName;
	fileDestination = fileDest;
	eventTextStr = eventText;
	destpath = path;

	eventHandle = NULL;
	connected = false;
	afpApi = new ohiAfpComm(this);
	user2File = userFileDest;
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
aes_ohi_fileHandlerImplementation::aes_ohi_fileHandlerImplementation(
		aes_ohi_fileHandler_r1 *userFileDest,
		const char* subSys,
		const char* appName,
		const char* fileDest,
		const char* eventText,
		const char* path) : userFile(0), user2File(0)
{
	errorInSemaphore = AES_OHI_NOERRORCODE;
	subSystem = subSys;
	applicationName = appName;
	fileDestination = fileDest;
	eventTextStr = eventText;
	destpath = path;

	eventHandle = NULL;
	connected = false;
	afpApi = new ohiAfpComm(this);
	userFile = userFileDest;
}

/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
aes_ohi_fileHandlerImplementation::~aes_ohi_fileHandlerImplementation()
{
	if (connected)
	{
		connected = false;
		if (afpApi)
			afpApi->close();
	}

	if (afpApi)
	{
		delete afpApi;
		afpApi = NULL;
	}

	if (eventHandle)
	{
		delete eventHandle;
		eventHandle = NULL;
	}
}

/*===================================================================
                    ROUTINE: attach
=================================================================== */
//description: attach the destination -> protect and connect 
unsigned int aes_ohi_fileHandlerImplementation::attach()
{
	GCC_TRACE((aes_ohi_api_file,"Entering aes_ohi_fileHandlerImplementation::attach ,fileDestination = %s", fileDestination.c_str()));
	//AES_GCC_FileDestProtection* protector;
	//protector = new AES_GCC_FileDestProtection(subSystem, applicationName);
	unsigned int returnSts;

	//if (protector->applicationProtect(fileDestination.c_str(), eventTextStr) == 0)
	//{
        if (strcasecmp(applicationName.c_str(),"sts") == 0)
	{
	    GCC_TRACE((aes_ohi_api_file,"calling opentq for sts"));
            returnSts = afpApi->open(fileDestination, STS_APPLICATION);
	}
        else
        {
             returnSts = afpApi->open(fileDestination);
        }
	if (returnSts == AES_OHI_NOERRORCODE)
	{
		connected = true;
		if (destpath != "")
		{
			returnSts = afpApi->setSourceDirectoryPath(fileDestination, destpath);
			// do a disconnect if this fails
		}
	}
	else
	{
		//protector->applicationUnprotect(fileDestination.c_str());
	}
	//}
	//else
	//{
	//returnSts = AES_OHI_FILEISPROTECTED;
	//}

	//delete protector;
	GCC_TRACE((aes_ohi_api_file,"returnSts = %d , Exiting aes_ohi_fileHandlerImplementation::attach", returnSts));
	return returnSts;
}

/*===================================================================
                    ROUTINE: detach
=================================================================== */
//description: reverse the detach -> unprotect and disconnect 
unsigned int aes_ohi_fileHandlerImplementation::detach()
{
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::detach"));

	unsigned int returnSts = AES_OHI_NOERRORCODE;

	//AES_GCC_FileDestProtection* protector;
	//protector = new AES_GCC_FileDestProtection(subSystem, applicationName);
	//protector->applicationUnprotect(fileDestination.c_str());
	//delete protector;

	if (connected)
	{
		connected = false;
		returnSts = afpApi->close();
	}
	GCC_TRACE((aes_ohi_api_file,"Exiting aes_ohi_fileHandlerImplementation::detach, returnSts = %d",returnSts));

	return  returnSts;
}

/*===================================================================
                    ROUTINE: getEventHandle
=================================================================== */
//description: create an semaphore ->user dosen't use callback
ACE_Semaphore * aes_ohi_fileHandlerImplementation::getEventHandle()
{
	GCC_TRACE((aes_ohi_api_file,"%s","In aes_ohi_fileHandlerImplementation::getEventHandle"));

	if (!eventHandle)//create eventHandle as already acquired, it will be released when event is signalled
	{
		eventHandle = new ACE_Semaphore(0, USYNC_THREAD,0,0,MAXCOUNT); //(0, USYNC_THREAD, 0,0,MAXCOUNT);
	}
	if (!eventHandle)
	{
		GCC_TRACE((aes_ohi_api_file,"Error while creating a semaphore. Error number is : %d", ACE_OS::last_error()));
	}

	GCC_TRACE((aes_ohi_api_file,"%s","Exiting aes_ohi_filehandlerimplementation::getEventHandle"));
	return eventHandle;
}

/*===================================================================
                    ROUTINE: event
=================================================================== */
//description: handle event from AFP set semaphore or call call-back
unsigned int aes_ohi_fileHandlerImplementation::event(AES_OHI_Eventcodes eventCode)
{
	GCC_TRACE((aes_ohi_api_file,"Entering aes_ohi_fileHandlerImplementation::event ,eventCode = %d", eventCode));

	if (eventHandle)
	{
		int result = eventHandle->release();

		if ( result == -1)
		{
			errorInSemaphore = AES_SEMAPHOREERR;
		}
	}
	else
	{
		eventFileName = "";
		afpApi->getEvent((AES_GCC_Eventcodes&)eventCode, eventFileName);
		if (userFile)
		{
			if (userFile->handleEvent(eventCode) == AES_OHI_EVENTNOTHANDLED)
			{
				// Do something
			}
		}

		if (user2File)
		{
			if (user2File->handleEvent(eventCode) == AES_OHI_EVENTNOTHANDLED)
			{
				// Do something
			}
		}
	}
	GCC_TRACE((aes_ohi_api_file,"eventFileName = %s , Exiting aes_ohi_fileHandlerImplementation::event",eventFileName.c_str()));
	return AES_OHI_NOERRORCODE;
}
/*===================================================================
                    ROUTINE: getEvent
=================================================================== */
//description: handle event from AFP set semaphore or call call-back
unsigned int aes_ohi_fileHandlerImplementation::getEvent(AES_OHI_Eventcodes& eventCode)
{
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::getEvent"));

	unsigned int returnSts = afpApi->getEvent((AES_GCC_Eventcodes&)eventCode,
			eventFileName);

	if (returnSts == AES_OHI_NOERRORCODE)
		returnSts = errorInSemaphore;

	GCC_TRACE((aes_ohi_api_file,"%s","Exiting aes_ohi_fileHandlerImplementation::getEvent , eventFileName = %s", eventFileName.c_str()));

	return returnSts;
}

/*===================================================================
                    ROUTINE: getDirectoryPath
=================================================================== */
//description: retrive active path from  AFP
unsigned int aes_ohi_fileHandlerImplementation::getDirectoryPath(std::string& path)
{  
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::getDirectoryPath"));

	unsigned int result = afpApi->getSourceDirectoryPath(fileDestination, path);

	GCC_TRACE((aes_ohi_api_file,"fileDestination = %s", fileDestination.c_str(),
			"status of afpApi->getDirectoryPath(fileDestination, path) = %d", result,
			"path = %s", path.c_str(),
			"%s","Exiting aes_ohi_fileHandlerImplementation::getDirectoryPath"));
	return result;
}

/*===================================================================
                    ROUTINE: getLastSentFile
=================================================================== */
//description: retrive last Reported (sent) File from  AFP
unsigned int aes_ohi_fileHandlerImplementation::getLastSentFile(std::string& fileName)
{
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::getLastSentFile"));
	std::string generatedFileName;

	unsigned int result = afpApi->lastReportedFile(fileDestination,
			fileName,
			generatedFileName);
	GCC_TRACE((aes_ohi_api_file,"fileDestination = %s", fileDestination.c_str(),
			"status of afpApi->lastReportedFile(fileDestination,fileName,",
			"generatedFileName) = %d", result,
			"fileName = %s", fileName.c_str(),
			"generatedFileName = %s", generatedFileName.c_str(),
			"%s","Exiting aes_ohi_fileHandlerImplementation::getLastSentFile"));
	return result;
}

/*===================================================================
                    ROUTINE: getEventFileName
=================================================================== */
//description: get filename from API
unsigned int aes_ohi_fileHandlerImplementation::getEventFileName(std::string& fileName)
{
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::getEventFileName"));
	fileName = eventFileName;
	GCC_TRACE((aes_ohi_api_file,"%s","Exiting aes_ohi_fileHandlerImplementation::getEventFileName,fileName = %s",fileName.c_str()));
	return 0;
}

/*===================================================================
                    ROUTINE: setTransferState
=================================================================== */
// description: set the transfer state for a file that was sent to 
// the filedestination
unsigned int aes_ohi_fileHandlerImplementation::setTransferState(
		std::string fileName,
		std::string destination,
		AES_OHI_Filestates status)
{
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::setTransferState",
			"fileName = %s", fileName.c_str(),
			"destination = %s", destination.c_str(),
			"status = %d", status));

	std::string tmpfileName = fileName;
	unsigned int result = afpApi->setTransferState(tmpfileName,
			destination,
			(AES_GCC_Filestates)status);
	GCC_TRACE((aes_ohi_api_file,"%s","afpApi->setTransferState(tmpfileName,destination,",
			"(AES_GCC_Filestates)status) = %d", result,
			"%s","Exiting aes_ohi_fileHandlerImplementation::setTransferState"));
	return result;
}

/*===================================================================
                    ROUTINE: getTransferState
=================================================================== */
//description: retrieve transfer state from AFP
unsigned int aes_ohi_fileHandlerImplementation::getTransferState(
		const std::string fileName,
		AES_OHI_Filestates& status)
{
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::getTransferState",
			"fileName = %s", fileName.c_str()));

	std::string tmpfileName = fileName;

	unsigned int result = afpApi->getFileStatus(tmpfileName,
			(AES_GCC_Filestates&)status);
	GCC_TRACE((aes_ohi_api_file,"afpApi->getFileStatus (tmpfileName,(AES_GCC_Filestates&)status) = %d",result,
			"status = %d", status,
			"%s","Exiting aes_ohi_fileHandlerImplementation::getTransferState"));
	return result;
}

/*===================================================================
                    ROUTINE: getTransferStateEx
=================================================================== */
//description: retrieve transfer state from AFP
unsigned int aes_ohi_fileHandlerImplementation::getTransferStateEx(
		std::string& fileName,
		AES_OHI_Filestates& status,
		std::string& destination,
		int listOrder,
		std::string& reportDate,
		std::string& deleteDate,
		std::string& manualStart,
		std::string& manualStop,
		std::string& failDate,
		std::string& archiveDate,
		int& reasonForFailed,
		bool& isDirectory)
{
	GCC_TRACE((aes_ohi_api_file_gettransferstateex,"%s","Entering aes_ohi_fileHandlerImplementation::gettransferstateex",
			"fileName %s = ", fileName.c_str(),
			"AES_OHI_Filestates = %d", status,
			"destination = %s", destination.c_str(),
			"listOrder = %d", listOrder,
			"reportDate = %s", reportDate.c_str(),
			"deleteDate = %s", deleteDate.c_str(),
			"manualStart = %s", manualStart.c_str(),
			"manualStop = %s", manualStop.c_str(),
			"failDate = %s", failDate.c_str(),
			"archiveDate %s= ", archiveDate.c_str(),
			"reasonForFailed = %d", reasonForFailed,
			"isDirectory = %d", isDirectory));

	unsigned int result = afpApi->getTransferStateEx(fileName,
			(AES_GCC_Filestates&)status,
			destination,
			listOrder,
			reportDate,
			deleteDate,
			manualStart,
			manualStop,
			failDate,
			archiveDate,
			reasonForFailed,
			isDirectory);
	GCC_TRACE((aes_ohi_api_file_gettransferstateex,"afpApi->getTransferStateEx(....) = %d",result,"%s",
			"Exiting aes_ohi_fileHandlerImplementation::getTransferStateEx"));
	return result;
}

/*===================================================================
                    ROUTINE: send
=================================================================== */
unsigned int aes_ohi_fileHandlerImplementation::send(const std::string fileName,
		AES_OHI_Format sendFormat,
		const std::string fileMask ,
		bool isDirectory )
{
	GCC_TRACE((aes_ohi_api_file_send,"Entering aes_ohi_fileHandlerImplementation::send",
			"fileName = %s", fileName.c_str(),
			"sendFormat = %s", sendFormat,
			"fileMask = %s", fileMask.c_str(),
			"isDirectory = %d", isDirectory));

	unsigned int result;

	// check if directory and empty string
	if (isDirectory && (fileMask == ""))
	{
		GCC_TRACE((aes_ohi_api_file_send,"%s","File mask for directory is changed to *"));

		result = afpApi->sendFile(fileName, (AES_GCC_Format)sendFormat,"*",isDirectory);
	}
	else
	{
		result = afpApi->sendFile(fileName,
				(AES_GCC_Format)sendFormat,
				fileMask,
				isDirectory);
	}
	GCC_TRACE((aes_ohi_api_file_send,"afpApi->sendFile(fileName, (AES_GCC_Format)sendFormat, fileMask,isDirectory),	      result  = %d",result,
	"%s","Exiting aes_ohi_fileHandlerImplementation::send"));
	return result;
}

/*===================================================================
                    ROUTINE: isConnected
=================================================================== */
bool aes_ohi_fileHandlerImplementation::isConnected()
{
	GCC_TRACE((aes_ohi_api_file,"%s","aes_ohi_fileHandlerImplementation::isConnected",
			"connected = %d", connected));
	return connected;
}

/*===================================================================
                    ROUTINE: setDirectoryPath
=================================================================== */
unsigned int aes_ohi_fileHandlerImplementation::setDirectoryPath(
		const std::string file,
		const std::string filePath )
{
	GCC_TRACE((aes_ohi_api_file,"%s","Entering aes_ohi_fileHandlerImplementation::setDirectoryPath",
			"file = %s", file.c_str(),
			"filePath = %s", filePath.c_str()));

	// kolla med danne
	std::string destPath = filePath;

	unsigned int result = afpApi->setSourceDirectoryPath(fileDestination,
			destPath);
	GCC_TRACE((aes_ohi_api_file,"afpApi->setDirectoryPath(fileDestination,destPath ),result = %d", result,
			"%s","Exiting aes_ohi_fileHandlerImplementation::setDirectoryPath"));

	return result;
}

/*===================================================================
                    ROUTINE: getErrCodeText
=================================================================== */
const char* aes_ohi_fileHandlerImplementation::getErrCodeText(unsigned int retCode)
{
	GCC_TRACE((aes_ohi_api_file,"%s","aes_ohi_fileHandlerImplementation::getErrCodeText"));
	return afpApi->getErrorCodeText(retCode);
}

