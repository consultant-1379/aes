/*================================================================== */
/**
   @file   aes_ohi_extfilehandlerimplementation2.h

   @brief Header file for aes_ohi_extFileHandlerImplementation2 in OHI module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/06/2011   XCHEMAD     Initial Release
==================================================================== */


/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_AES_OHI_EXTFILEHANDLER_IMPLEMENTATION2_H_)
#define _AES_OHI_EXTFILEHANDLER_IMPLEMENTATION2_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <list>
#include <ace/ACE.h>
#include <ace/Semaphore.h>
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_filestates.h"
#include "aes_ohi_format.h"
#include "ohiextafpcomm.h"

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class aes_ohi_extFileHandler2_r1;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_ohi_extFileHandlerImplementation2

                 This class handles the ext file handler functionality
*/
/*=================================================================== */

class aes_ohi_extFileHandlerImplementation2 
{
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Constructor

      @pre             none

      @post            none

      @param           subSys
                                Name of the sub system.

      @param           appName
                                Name of the application block.

      @param           userFileDest
				Pointer to aes_ohi_extFileHandler2_r1

      @return          none

      @exception       none
   */
/*=================================================================== */

   aes_ohi_extFileHandlerImplementation2(aes_ohi_extFileHandler2_r1 *userFileDest,
                                         const char* subSys,
                                         const char* appName);

/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Destructor

      @pre             none

      @post            none

      @return          none

      @exception       none
   */
/*=================================================================== */

   ~aes_ohi_extFileHandlerImplementation2();

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Attach to a GOH and start the session by establishing an
		   connection to GOH.

      @pre         none

      @post        none

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int attach();

/*=================================================================== */
   /**
      @brief       Check if the filedest is defined

      @pre         none

      @post        none

      @param       fileName
                        Name of the file.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int fileTransferQueueDefined(const std::string fileName);
/*=================================================================== */
   /**
      @brief       Check if the filedest is defined

      @pre         none

      @post        none

      @param       fileName
                        Name of the file.

      @param       fileTransferQueueDn 
                        Dn of the file transfer queue.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int fileTransferQueueDefined(const std::string fileName,
					 std::string & fileTransferQueueDn);

/*=================================================================== */
   /**
      @brief       Retrive a list of all defined file destinations in GOH.

      @pre         none

      @post        none

      @param       fileTQList
                        List of file destination names

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getFileTransferQueues(std::list<std::string>& fileTQList);

/*=================================================================== */
   /**
      @brief       Retrives the name of a file that generated the failed transfer event.

      @pre         none

      @post        none

      @param       filName
                        The name of the file.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getEventFileName(std::string& fileName);

/*=================================================================== */
   /**
      @brief       Get event handle. The handle is used to inform application of events.
		   The getEventHandle is used with getEvent to replace callback handleEvent.

      @pre         none

      @post        none

      @return      ACE_Semaphore*
			Returns the semaphore handle
			NULL indicates failure to create the event

      @exception   none
    */
/*=================================================================== */

   ACE_Semaphore * getEventHandle();

/*=================================================================== */
   /**
      @brief       Get the event code associated with the event signaled through
		   the event handle and removes it from the queue.
		   Not to be used if callback is implemented.

      @pre         none

      @post        none

      @param       eventCode
			The code of the signaled event.

      @return      Return code

      @exception   none
    */
/*=================================================================== */

   unsigned int getEvent(AES_OHI_Eventcodes& eventCode);

/*=================================================================== */
   /**
      @brief       Reverse the attach unreserve the FileDestination, and close the
		   connection to GOH.

      @pre         none

      @post        none

      @return      Return code

      @exception   none
    */
/*=================================================================== */

   unsigned int detach();

/*=================================================================== */
   /**
      @brief       Retreives an error code text.

      @pre         none

      @post        none

      @param       retCode
			The numeral code to retrieve the text string for.

      @return      Text
			The error code text.

      @exception   none
    */
/*=================================================================== */
   const char* getErrCodeText(unsigned int retCode);

/*=================================================================== */
   /**
      @brief       Returns the state if the application is connected to the server

      @pre         none

      @post        none

      @return      Connection status

      @exception   none
    */
/*=================================================================== */

   bool isConnected();

/*=================================================================== */
   /**
      @brief       Sets the directory path to the specified path

      @pre         none

      @post        none

      @param	   file
			Name of the file

      @param       filePath
			Physical path of the file

      @return      Return code

      @exception   none
    */
/*=================================================================== */

   unsigned int setDirectoryPath(const std::string file, const std::string filePath );

/*=================================================================== */
   /**
      @brief       Gets the event for the given event code.

      @pre         none

      @post        none

      @param       eventCode
                        The event code number

      @return      Return code

      @exception   none
    */
/*=================================================================== */

   unsigned int event(AES_OHI_Eventcodes eventCode);

/*=================================================================== */
   /**
      @brief       Gets the destination directory path

      @pre         none

      @post        none

      @param       fileDest
                        Name of the file destination

      @param       path
                        Path of the file

      @return      Return code

      @exception   none
    */
/*=================================================================== */

   unsigned int getDestDirectoryPath(const std::string fileDest, std::string& path);

/*=================================================================== */
   /**
      @brief       Sets the destination directory path

      @pre         none

      @post        none

      @param       fileDest
                        Name of the file destination

      @param       filePath
                        Path of the file

      @return      Return code

      @exception   none
    */
/*=================================================================== */
   unsigned int setDestDirectoryPath(const std::string fileDest, const std::string filePath);

/*=================================================================== */
   /**
      @brief       Removes the file

      @pre         none

      @post        none

      @param       fileDest
                        Name of the file destination

      @param       fileName
                        Name of the file

      @return      Return code

      @exception   none
    */
/*=================================================================== */

   unsigned int removeFile(const std::string fileDest, const std::string fileName );

/*===================================================================
                        PROTECTED DECLARATION SECTION
=================================================================== */
protected:
/*===================================================================
                        PROTECTED ATTRIBUTE
=================================================================== */
/*=================================================================== */
   /**
      @brief  subsystem

              This attribute holds the sub system name.
   */
/*=================================================================== */
   std::string subSystem;            

/*=================================================================== */
   /**
      @brief  applicationName

              This attribute holds the name of the application block.
   */
/*=================================================================== */
   std::string applicationName;      

/*=================================================================== */
   /**
      @brief  eventFileName

              This attribute holds the file name associated with event
   */
/*=================================================================== */
   std::string eventFileName;        

/*=================================================================== */
   /**
      @brief  eventHandle

              This attribute holds the semaphore handle
   */
/*=================================================================== */
   ACE_Semaphore * eventHandle;

/*=================================================================== */
   /**
      @brief  connected

              This attribute holds the connection status
   */
/*=================================================================== */
   bool connected;

/*=================================================================== */
   /**
      @brief  errorInSemaphore

              This attribute holds the error status of the semaphore handle.
   */
/*=================================================================== */
   unsigned int errorInSemaphore;

/*=================================================================== */
   /**
      @brief  afpApi

              This attribute holds the link forward to AFP
   */
/*=================================================================== */
   ohiExtAfpComm* afpApi;               

/*=================================================================== */
   /**
      @brief  userFile2_r1

              This attribute holds the link back to user
   */
/*=================================================================== */
   aes_ohi_extFileHandler2_r1* userFile2_r1;    
};

#endif
