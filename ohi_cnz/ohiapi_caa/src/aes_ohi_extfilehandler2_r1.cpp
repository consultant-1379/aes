/*=================================================================== */
   /**
   @file aes_ohi_extfilehandler2_r1.cpp

   Class method implementationn for OHI type module.

   This module contains the implementation of class declared in
   the aes_ohi_extfilehandler2_r1.h module

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
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_extfilehandler2_r1.h"
#include "aes_ohi_extfilehandlerimplementation2.h"
//#include "aes_gcc_nameprotection.h"

using namespace std;

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: aes_ohi_extFileHandler2_r1
=================================================================== */

aes_ohi_extFileHandler2_r1::aes_ohi_extFileHandler2_r1(const char* subSys,
                                                       const char* appName)                         
{
   implementation = new aes_ohi_extFileHandlerImplementation2(this,
                                                              subSys,
                                                              appName);
}

/*===================================================================
   ROUTINE: ~aes_ohi_extFileHandler2_r1
=================================================================== */
aes_ohi_extFileHandler2_r1::~aes_ohi_extFileHandler2_r1()
{
   delete implementation;
}

/*===================================================================
   ROUTINE: attach
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::attach()
{	
	return implementation->attach();
}

/*===================================================================
   ROUTINE: detach
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::detach()
{
	return implementation->detach();
}

/*===================================================================
   ROUTINE: fileTransferQueueDefined
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::fileTransferQueueDefined(const char* fileTransferQueue)
{
   return implementation->fileTransferQueueDefined(fileTransferQueue);
}

/*===================================================================
   ROUTINE: fileTransferQueueDefined
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::fileTransferQueueDefined(const char* fileTransferQueue,
								  std::string & fileTransferQueueDn)
{
   return implementation->fileTransferQueueDefined(fileTransferQueue, fileTransferQueueDn);
}

/*===================================================================
   ROUTINE: getFileTransferQueues
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::getFileTransferQueues(int& noOfItems,
                                                               char** transferQueueList[])
{
   // Initialize return values
   noOfItems = 0;
   *transferQueueList = NULL;

   std::list<string> TQlist;
   ACE_UINT32 rcode = implementation->getFileTransferQueues(TQlist);
   if (TQlist.size() > 0)
   {
      // Allocate a vector with n number of pointers
      *transferQueueList = new char* [sizeof(char*) * TQlist.size()];
      if (*transferQueueList)
      {
         std::list<string>::iterator iter = TQlist.begin();
         while (iter != TQlist.end())
         {
            // Allocate each string and copy transfer queue name to it
            (*transferQueueList)[noOfItems] = new char[(*iter).length()+1];
            if ((*transferQueueList)[noOfItems])
            {
               strcpy((*transferQueueList)[noOfItems++], (*iter).c_str());
            }

            iter++;
         }
      }

      TQlist.clear();
   }

   return rcode;
}

/*===================================================================
   ROUTINE: getDestDirectoryPath
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::getDestDirectoryPath(const char* fileDest, char * path)
{
    	string strPath = "";
        unsigned int rcode = 0;
	rcode = implementation->getDestDirectoryPath(fileDest, strPath);
        if (strPath.length() > 0)
      	    strcpy(path, strPath.c_str());
   	else
      	    *path = '\0';
	return rcode;

}


/*===================================================================
   ROUTINE: setDestDirectoryPath
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::setDestDirectoryPath(const char* fileDest,
                                                              const char* path)
{
	return implementation->setDestDirectoryPath(fileDest, path);
}


/*===================================================================
   ROUTINE: sendFileManually
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::sendFileManually(const char* filename,
                                                          const char* path,
                                                          const char* destination,
                                                          int retryTimes,
                                                          int retryTimeInterval)
{
   (void) filename;
   (void) path;
   (void) destination;
   (void) retryTimes;
   (void) retryTimeInterval;
   return AES_OHI_NOSERVERACCESS;
}

/*===================================================================
   ROUTINE: removeFile
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::removeFile(const char* fileDest,
                                                    const char* fileName)
{
	(void) fileDest;
	(void) fileName;
	return AES_OHI_NOSERVERACCESS;
	//return implementation->removeFile(fileDest, fileName); // This functionality is not supported in APG43L
}

/*===================================================================
   ROUTINE: getErrCodeText
=================================================================== */
const char* aes_ohi_extFileHandler2_r1::getErrCodeText(unsigned int retCode)
{
   return implementation->getErrCodeText(retCode);
}

/*===================================================================
   ROUTINE: isConnected
=================================================================== */
bool aes_ohi_extFileHandler2_r1::isConnected()
{
	return implementation->isConnected();
}

/*===================================================================
   ROUTINE: getEventHandle
=================================================================== */
ACE_Semaphore * aes_ohi_extFileHandler2_r1::getEventHandle()
{      
	return implementation->getEventHandle();
}

/*===================================================================
   ROUTINE: getEvent
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::getEvent(AES_OHI_Eventcodes& eventCode)
{
	return implementation->getEvent(eventCode);
}

/*===================================================================
   ROUTINE: getEventFileName
=================================================================== */
unsigned int aes_ohi_extFileHandler2_r1::getEventFileName(char* fileName)
{
   string strFilename = "";
   ACE_UINT32 rcode = implementation->getEventFileName(strFilename);
   if (strFilename.length() > 0)
   {
      strcpy(fileName, strFilename.c_str());
   }
   else
   {
      *fileName = '\0';
   }

   return rcode;
}
