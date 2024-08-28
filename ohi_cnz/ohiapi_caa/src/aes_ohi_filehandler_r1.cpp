/*=================================================================== */
	/**
	@file   aes_ohi_filehandler_r1.cpp

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

aes_ohi_fileHandler_r1::aes_ohi_fileHandler_r1(const char* subSys,
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
aes_ohi_fileHandler_r1::aes_ohi_fileHandler_r1(const char* subSys,
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
aes_ohi_fileHandler_r1::~aes_ohi_fileHandler_r1()
{
   delete implementation; 
}

/*===================================================================
                    ROUTINE: attach
=================================================================== */
unsigned int aes_ohi_fileHandler_r1::attach()
{
   return implementation->attach();
}
/*===================================================================
                    ROUTINE: detach
=================================================================== */
unsigned int aes_ohi_fileHandler_r1::detach()
{
   return implementation->detach();	  
}
/*===================================================================
                    ROUTINE: send
=================================================================== */
unsigned int aes_ohi_fileHandler_r1::send(const char* fileName,
                                          AES_OHI_Format sendFormat)
{
	//description: reverse the send -> unprotect and disconnect
   return implementation->send(fileName, sendFormat);	  
}

/*===================================================================
                    ROUTINE: getEventHandle
=================================================================== */
ACE_Semaphore * aes_ohi_fileHandler_r1::getEventHandle()
{      
	//description: create an semaphore ->user dosen't use callback
	return implementation->getEventHandle();
}
/*===================================================================
                    ROUTINE: getEvent
=================================================================== */
unsigned int aes_ohi_fileHandler_r1::getEvent(AES_OHI_Eventcodes& eventCode)
{
//	handle event from AFP set semaphore or call call-back
	return implementation->getEvent(eventCode);
}

/*===================================================================
                    ROUTINE: getDirectoryPath
=================================================================== */
unsigned int aes_ohi_fileHandler_r1::getDirectoryPath(char* path)
{
	//description: retrieve active path from  AFP
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
//description: retrieve listYoungestSubfile from  AFP
unsigned int aes_ohi_fileHandler_r1::getLastSentFile(char* fileName)
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
unsigned int aes_ohi_fileHandler_r1::getEventFileName(char* fileName)
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
// description: set the transfer state for a file that was sent to the filedestination
unsigned int aes_ohi_fileHandler_r1::setTransferState(const char* fileName,
                                                      const char* destination,
                                                      AES_OHI_Filestates status)
{
   string strFileName = fileName;
   string strDestination = destination;
	return implementation->setTransferState(fileName, destination, status);
}

/*===================================================================
                    ROUTINE: getTransferState
=================================================================== */
//description: retrive transfer state from AFP
unsigned int aes_ohi_fileHandler_r1::getTransferState(const char* fileName,
                                                      AES_OHI_Filestates& status)
{
   string strFileName = fileName;
	return implementation->getTransferState(fileName, status);
}

/*===================================================================
                    ROUTINE: getErrCodeText
=================================================================== */
const char* aes_ohi_fileHandler_r1::getErrCodeText(unsigned int retCode)
{
   return implementation->getErrCodeText(retCode);
}

/*===================================================================
                    ROUTINE: isConnected
=================================================================== */
bool aes_ohi_fileHandler_r1::isConnected()
{
	return implementation->isConnected();
}
