/*=================================================================== */
	/**
	@file   aes_ohi_filehandler_r2.cpp

	@brief
	Provides an wrapper for aes afp api to send files.
	General rule: The methods returns an unsigned int as error code
	to indicate any error. See AES_OHI_Errorcodes
	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       29/08/2011     XNADNAR       Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_filehandlerimplementation.h"	

using namespace std;

/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
aes_ohi_fileHandler_r2::aes_ohi_fileHandler_r2(const char* subSys,
                                               const char* appName,
                                               const char* fileDest,
                                               const char* eventText)
{
   implementation = new aes_ohi_fileHandlerImplementation(this,
                                                          subSys,
                                                          appName,
                                                          fileDest,
                                                          eventText,
                                                          "");
}
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
aes_ohi_fileHandler_r2::aes_ohi_fileHandler_r2(const char* subSys,
                                               const char* appName,
                                               const char* fileDest,
                                               const char* eventText,
                                               const char* path)
{
   implementation = new aes_ohi_fileHandlerImplementation(this,
                                                          subSys,
                                                          appName,
                                                          fileDest,
                                                          eventText,
                                                          path);
}

/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
aes_ohi_fileHandler_r2::~aes_ohi_fileHandler_r2()
{
   delete implementation; 
}

/*===================================================================
                    ROUTINE: attach
=================================================================== */
//description: attach the destination -> protect and connect 
unsigned int aes_ohi_fileHandler_r2::attach()
{
	return implementation->attach();
}

/*===================================================================
                    ROUTINE: detach
=================================================================== */
//description: reverse the detach -> unprotect and disconnect 
unsigned int aes_ohi_fileHandler_r2::detach()
{
   return implementation->detach();	  
}

/*===================================================================
                    ROUTINE: send
=================================================================== */
//description: reverse the send -> unprotect and disconnect 
unsigned int aes_ohi_fileHandler_r2::send(const char* fileName,
                                          AES_OHI_Format sendFormat)
{
   return implementation->send(fileName, sendFormat);	  
}

/*===================================================================
                    ROUTINE: getEventHandle
=================================================================== */
//description: create an semaphore ->user dosen't use callback
ACE_Semaphore * aes_ohi_fileHandler_r2::getEventHandle()
{      
	return implementation->getEventHandle();
}

/*===================================================================
                    ROUTINE: getEvent
=================================================================== */
//description: handle event from AFP set semaphore or call call-back
unsigned int aes_ohi_fileHandler_r2::getEvent(AES_OHI_Eventcodes& eventCode)
{
	return implementation->getEvent(eventCode);
}

/*===================================================================
                    ROUTINE: getDirectoryPath
=================================================================== */
//description: retrive active path from  AFP
unsigned int aes_ohi_fileHandler_r2::getDirectoryPath(char* path)
{
   string strPath = "";
   unsigned int rcode = implementation->getDirectoryPath(strPath);
   if (strPath.length() > 0)
      strcpy(path, strPath.c_str());
   else
      *path = '\0';

   return rcode;
}

/*===================================================================
                    ROUTINE: getLastSentFile
=================================================================== */
//description: retrive listYoungestSubfile from  AFP
unsigned int aes_ohi_fileHandler_r2::getLastSentFile(char* fileName)
{
   string strFileName = "";
   unsigned int rcode = implementation->getLastSentFile(strFileName);
   if (strFileName.length() > 0)
      strcpy(fileName, strFileName.c_str());
   else
      *fileName = '\0';

   return rcode;
}

/*===================================================================
                    ROUTINE: getEventFileName
=================================================================== */
//description: get filename from API
unsigned int aes_ohi_fileHandler_r2::getEventFileName(char* fileName)
{
   string strFileName = "";
   unsigned int rcode = implementation->getEventFileName(strFileName);
   if (strFileName.length() > 0)
      strcpy(fileName, strFileName.c_str());
   else
      *fileName = '\0';

   return rcode;
}

/*===================================================================
                    ROUTINE: setTransferState
=================================================================== */
// description: set the transfer state for a file that was sent to 
// the filedestination
unsigned int aes_ohi_fileHandler_r2::setTransferState(const char* fileName,
                                                      const char* destination,
                                                      AES_OHI_Filestates status)
{
   string strFileName = fileName;
   string strDestination = destination;
	return implementation->setTransferState(strFileName, strDestination, status);
}

/*===================================================================
                    ROUTINE: getTransferState
=================================================================== */
//description: retrive transfer state from AFP
unsigned int aes_ohi_fileHandler_r2::getTransferState(const char* fileName,
                                                      AES_OHI_Filestates& status)
{
   string strFileName = fileName;
	return implementation->getTransferState(strFileName, status);
}

/*===================================================================
                    ROUTINE: getTransferStateEx
=================================================================== */
//description: retrieve status and other data from AFP
unsigned int aes_ohi_fileHandler_r2::getTransferStateEx(char* fileName,
	                                                     AES_OHI_Filestates& status,
                                                        char* destination,
                                                        int listOrder,
                                                        char* reportDate,
                                                        char* deleteDate,
                                                        char* manualStart,
                                                        char* manualStop,
                                                        char* failDate,
                                                        char* archiveDate,
                                                        int& reasonForFailed,
                                                        bool& isDirectory)
{
   string strFileName = fileName;
   string strDestination = destination;
   string strReportDate = "";
   string strDeleteDay = "";
   string strManualStart = "";
   string strManualStop = "";
   string strFailDate = "";
   string strArchiveDate = "";

	unsigned int rcode = implementation->getTransferStateEx(strFileName,
															      status,
                                                   strDestination,
                                                   listOrder,
                                                   strReportDate,
                                                   strDeleteDay,
                                                   strManualStart,
                                                   strManualStop,
                                                   strFailDate,
                                                   strArchiveDate,
                                                   reasonForFailed,
                                                   isDirectory);

   strcpy(fileName, strFileName.c_str());
   strcpy(destination, strDestination.c_str());
   strcpy(reportDate, strReportDate.c_str());
   strcpy(deleteDate, strDeleteDay.c_str());
   strcpy(manualStart, strManualStart.c_str());
   strcpy(manualStop, strManualStop.c_str());
   strcpy(failDate, strFailDate.c_str());
   strcpy(archiveDate, strArchiveDate.c_str());

   return rcode;
}

/*===================================================================
                    ROUTINE: getErrCodeText
=================================================================== */
const char* aes_ohi_fileHandler_r2::getErrCodeText (unsigned int retCode)
{
   return implementation->getErrCodeText(retCode);
}

/*===================================================================
                    ROUTINE: isConnected
=================================================================== */
bool aes_ohi_fileHandler_r2::isConnected()
{
	return implementation->isConnected();
}
