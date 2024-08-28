/*=================================================================== */
   /**
   @file aes_ohi_extfilehandlerimplementation2.cpp

   Class method implementationn for OHI type module.

   This module contains the implementation of class declared in
   the aes_ohi_extfilehandlerimplementation2.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/06/2011    XCHEMAD   Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include <aes_ohi_eventcodes.h>
#include <aes_ohi_errorcodes.h>
#include <aes_afp_api.h>
#include <aes_gcc_log.h>
#include "aes_ohi_extfilehandlerimplementation2.h"	
#include "aes_ohi_extfilehandler2_r1.h"

using namespace std;

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief MAXCOUNT

          This is the maxiumum number of outstanding messages in the queue.
 */
/*=================================================================== */

#define MAXCOUNT 10000  

/*=================================================================== */
/**
   @brief aes_ohi_api_extfile2

          Declaration of trace point for ext file handler.
 */
/*=================================================================== */
GCC_TDEFL(aes_ohi_api_extfile2,"C400");

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: aes_ohi_extFileHandlerImplementation2
=================================================================== */
aes_ohi_extFileHandlerImplementation2::aes_ohi_extFileHandlerImplementation2(
	aes_ohi_extFileHandler2_r1 *userFileDest, 
	const char* subSys,
	const char* appName)
{
   errorInSemaphore = AES_OHI_NOERRORCODE;
   subSystem.assign(subSys);
   applicationName.assign(appName);
   eventHandle = NULL;
   connected = false;
   afpApi = new ohiExtAfpComm(this);
   userFile2_r1 = userFileDest;
}

/*===================================================================
   ROUTINE: ~aes_ohi_extFileHandlerImplementation2
=================================================================== */
aes_ohi_extFileHandlerImplementation2::~aes_ohi_extFileHandlerImplementation2()
{
   if (eventHandle != NULL)
   {
		eventHandle->remove();
		delete eventHandle;
		eventHandle = NULL;
   }

   if (connected )
   {
      connected = false; 
      afpApi->close();
   }
   delete afpApi; 
   afpApi = 0;
}

/*===================================================================
   ROUTINE: attach
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::attach()
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%s\n %s%s\n", "Entering aes_ohi_extFileHandlerImplementation2::attach", "subSystem = ", subSystem.c_str(), "applicationName = ", applicationName.c_str()));

   unsigned int returnSts;

   if ((returnSts = afpApi->open (subSystem,applicationName))==AES_OHI_NOERRORCODE)
   {
      connected = true;
   }

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s\n", "return code from afpApi->open(subSystem,applicationName) = ", returnSts, "Exiting aes_ohi_extFileHandlerImplementation2::attach"));

   return returnSts;
}

/*===================================================================
   ROUTINE: detach
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::detach()        //KATJ
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n", "Entering aes_ohi_extFileHandlerImplementation2::detach"));

   unsigned int returnSts = AES_OHI_NOERRORCODE;
   if (connected )
   {
      connected = false; 
      returnSts = afpApi->close();
   }

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s\n", "return code from afpApi->close() = ", returnSts, "Exiting aes_ohi_extFileHandlerImplementation2::detach"));

   return returnSts;
}

/*===================================================================
   ROUTINE: fileTransferQueueDefined
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::fileTransferQueueDefined( const std::string TQName)
{  
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%s\n", "Entering aes_ohi_extFileHandlerImplementation2::fileTransferQueueDefined", "TransferQueueName = ", TQName.c_str()));

   bool manual;
   std::string destination;
   std::string fName =TQName;
   std::string fileTranferQueueDn("");

   unsigned int result;
   result = afpApi->transferQueueDefined(fName, destination, manual, fileTranferQueueDn);

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s%s\n %s%s\n %s%d\n %s\n", "afpApi->transferQueueDefined(fName, destination, fileType, manual,fileTranferQueueDn) = ",result, "fName = ", fName.c_str(), "destination = ", destination.c_str(), "manual = ", manual, "Exiting aes_ohi_extFileHandlerImplementation2::fileTransferQueueDefined"));

   return result;
}

/*===================================================================
   ROUTINE: fileTransferQueueDefined
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::fileTransferQueueDefined( const std::string TQName,
									      std::string & fileTranferQueueDn)
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%s\n", "Entering aes_ohi_extFileHandlerImplementation2::fileTransferQueueDefined", "TransferQueueName = ", TQName.c_str()));

   bool manual;
   std::string destination;
   std::string fName =TQName;

   unsigned int result;
   result = afpApi->transferQueueDefined(fName, destination, manual, fileTranferQueueDn);

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s%s\n %s%s\n %s%d\n %s%s\n %s\n", "afpApi->transferQueueDefined(fName, destination, fileType, manual, fileTranferQueueDn) = ",result, "fName = ", fName.c_str(), "destination = ", destination.c_str(), "manual = ", manual, "File transfer queue dn = ", fileTranferQueueDn.c_str(), "Exiting aes_ohi_extFileHandlerImplementation2::fileTransferQueueDefined"));

   return result;
}

/*===================================================================
   ROUTINE: getFileTransferQueues
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::getFileTransferQueues(
	std::list<std::string>& fileTQList)
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n", "Entering aes_ohi_extFileHandlerImplementation2::getFileTransferQueues"));

   unsigned int result;
   result = afpApi->getDestinationSetList(fileTQList);

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s\n", "afpApi->getDestinationList(fileTQList) = ", result, "Exiting aes_ohi_extFileHandlerImplementation2::getFileTransferQueues"));

   return result;
}

/*===================================================================
   ROUTINE: getEventHandle
=================================================================== */
ACE_Semaphore * aes_ohi_extFileHandlerImplementation2::getEventHandle()
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n", "Entering aes_ohi_extFileHandlerImplementation2::getEventHandle"));

   if (eventHandle == NULL)
   {
	eventHandle = new ACE_Semaphore(1, USYNC_THREAD,0,0,MAXCOUNT); //(0, USYNC_THREAD, 0,0,MAXCOUNT);
   }
   if ( !eventHandle )
   {
      GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n", "Error while creating a semaphore."));
   }

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n", "Exiting aes_ohi_extFileHandlerImplementation2::getEventHandle"));

   return eventHandle;
}

/*===================================================================
   ROUTINE: event
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::event(AES_OHI_Eventcodes eventCode)
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%d\n", "Entering aes_ohi_extFileHandlerImplementation2::event", "eventCode = ", eventCode));

   if (eventHandle != NULL)
   {
      ACE_INT32 result = eventHandle->release();
      if ( result == -1)
      {
         errorInSemaphore = AES_OHI_SEMAPHOREERR;
      }
   }
   else
   {
      eventFileName = "";
      afpApi->getEvent((AES_GCC_Eventcodes&)eventCode,eventFileName);

      if (userFile2_r1 != 0)
      {
         if ( userFile2_r1->handleEvent(eventCode) == AES_OHI_EVENTNOTHANDLED)
         {
            //Nothing
         }
      }
   }

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n", "Exiting aes_ohi_extFileHandlerImplementation2::event"));
   return AES_OHI_NOERRORCODE; 
}

/*===================================================================
   ROUTINE: getEvent
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::getEvent(AES_OHI_Eventcodes& eventCode)
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%d\n", "Entering aes_ohi_extFileHandlerImplementation2::getEvent", "eventCode = ", eventCode));

   unsigned int returnSts=afpApi->getEvent(
      (AES_GCC_Eventcodes&)eventCode,eventFileName);

   if ( returnSts==AES_OHI_NOERRORCODE)
   {
      returnSts = errorInSemaphore;
   }

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s\n", "afpApi->getEvent((AES_GCC_Eventcodes&)eventCode,eventFileName = ",returnSts, "Exiting aes_ohi_extFileHandlerImplementation2::getEvent"));

   return returnSts;
}

/*===================================================================
   ROUTINE: getEventFileName
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::getEventFileName(
	std::string& fileName)
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%s\n", "Entering aes_ohi_extFileHandlerImplementation2::getEventFileName", "fileName =", fileName.c_str()));

   fileName = eventFileName;

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%s\n %s\n", "eventFileName =", fileName.c_str(), "Exiting aes_ohi_extFileHandlerImplementation2::getEventFileName"));
   return 0;
}

/*===================================================================
   ROUTINE: isConnected
=================================================================== */
bool aes_ohi_extFileHandlerImplementation2::isConnected()
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%d\n", "aes_ohi_extFileHandlerImplementation2::isConnected", "connected = ", connected));

   return connected;
}

/*===================================================================
   ROUTINE: getErrCodeText
=================================================================== */
const char* aes_ohi_extFileHandlerImplementation2::getErrCodeText(unsigned int retCode)
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%u\n", "aes_ohi_extFileHandlerImplementation2::getErrCodeText", "retCode = ", retCode));

   return afpApi->getErrorCodeText(retCode);
}

/*===================================================================
   ROUTINE: getDestDirectoryPath
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::getDestDirectoryPath(
	const std::string fileDest,
	std::string& path)
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%s\n", "Entering aes_ohi_extFileHandlerImplementation2::getDestDirectoryPath", "fileDest = ", fileDest.c_str()));
   std::string destPathtmp = fileDest;

   unsigned int result;
   result = afpApi->getSourceDirectoryPath(destPathtmp, path);

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s%s\n %s\n", "afpApi->getDirectoryPath(destPathtmp, path) = ", result, "path = ", path.c_str(), "Exiting aes_ohi_extFileHandlerImplementation2::getDestDirectoryPath"));

   return result;
}

/*===================================================================
   ROUTINE: setDestDirectoryPath
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::setDestDirectoryPath(
	const std::string fileDest,
	const std::string filePath )
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%s\n %s%s\n", "Entering aes_ohi_extFileHandlerImplementation2::setDestDirectoryPath", "fileDest = ", fileDest.c_str(), "filePath = ", filePath.c_str()));

   std::string destPathtmp = fileDest;
   std::string destPath = filePath;

   unsigned int result;
   result = afpApi->setSourceDirectoryPath(destPathtmp,destPath );

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s\n", "afpApi->setDirectoryPath(destPathtmp,destPath ) = ", result, "Exiting aes_ohi_extFileHandlerImplementation2::setDestDirectoryPath"));

   return result;
}

/*===================================================================
   ROUTINE: removeFile
=================================================================== */
unsigned int aes_ohi_extFileHandlerImplementation2::removeFile(
	const std::string fileDest,
	const std::string fileName )
{
   GCC_TRACE((aes_ohi_api_extfile2, "\n %s\n %s%s\n %s%s\n", "Entering aes_ohi_extFileHandlerImplementation2::removeFile", "fileDest = ", fileDest.c_str(), "fileName = ", fileName.c_str()));

   unsigned int result;
   result = afpApi->removeFile(fileDest,fileName );

   GCC_TRACE((aes_ohi_api_extfile2, "\n %s%u\n %s\n", "afpApi->removeFile(fileDest,fileName ) = ", result, "Exiting aes_ohi_extFileHandlerImplementation2::removeFile"));

   return result;
}
