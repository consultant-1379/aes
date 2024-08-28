/*=================================================================== */
/**
   @file   aes_ohi_extfilehandler2_r1.h

   @brief Header file for aes_ohi_extFileHandler2_r1 in OHI module.

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
#if !defined(_AES_OHI_EXTFILEHANDLER2_R1_H_)
#define _AES_OHI_EXTFILEHANDLER2_R1_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Semaphore.h>
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_filestates.h"
#include "aes_ohi_format.h"

/*====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class aes_ohi_extFileHandlerImplementation2;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_ohi_extFileHandler2_r1

                 This class handles the ext file handling functionality
*/
/*=================================================================== */

class aes_ohi_extFileHandler2_r1 
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
                                Name of the Sub system.

      @param           appName
                                Name of the application block.

      @return          none

      @exception       none
   */
/*=================================================================== */

   aes_ohi_extFileHandler2_r1(const char* subSys, const char* appName);  

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

   ~aes_ohi_extFileHandler2_r1();
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Attach without protection.

      @pre         none

      @post        none

      @return      Error code

      @exception   none
    */
/*=================================================================== */
   unsigned int attach();

/*=================================================================== */
   /**
      @brief       Detach without protection.

      @pre         none

      @post        none

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int detach();

/*=================================================================== */
   /**
      @brief       Ask GOH if file transfer queue is defined in GOH.

      @pre         none

      @post        none

      @param	   fileTransferQueue
			Name of the file transfer queue.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int fileTransferQueueDefined(const char* fileTransferQueue);

/*=================================================================== */
   /**
      @brief       Ask GOH if file transfer queue is defined in GOH.

      @pre         none

      @post        none

      @param       fileTransferQueue
                        Name of the file transfer queue.

      @param       fileTransferQueueDn
                        Dn of the file transfer queue.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int fileTransferQueueDefined(const char* fileTransferQueue, std::string & fileTransferQueueDn);

/*=================================================================== */
   /**
      @brief       Retrieves a list of all defined file transfer Queues in GOH.

      @pre         none

      @post        none

      @param       noOfItems
                        Number of transfer queue strings received

      @param       transferQueueList
                        A string vector with the transfer queue names

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getFileTransferQueues(int& noOfItems, char** transferQueueList[]);

/*=================================================================== */
   /**
      @brief       Retrieves the name of a file that generated the event.

      @pre         none

      @post        none

      @param       fileName
                        The name of the file.

      @return      Error code

      @exception   none
    */
/*=================================================================== */
   unsigned int getEventFileName(char* fileName);

/*=================================================================== */
   /**
      @brief       A call back to be implemented by the user

      @pre         none

      @post        none

      @param       eventCode
                        The code of the signalled event

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   virtual unsigned int handleEvent(AES_OHI_Eventcodes eventCode)
   {
      (void) eventCode;
      return AES_OHI_EVENTNOTHANDLED;
   };

/*=================================================================== */
   /**
      @brief       Get event handle. The handle is used to inform application of
		   events. The getEventHandle is used with getEvent to replace
		   callback handleEvent.

      @pre         none

      @post        none

      @return      Returns the semaphore handle.
		   NULL indicates failure to create the event.

      @exception   none
    */
/*=================================================================== */

   ACE_Semaphore * getEventHandle();

/*=================================================================== */
   /**
      @brief       Get the event code associated with the event signalled through
                   the event handle and removes it from the queue.
                   Not to be used if callback is implemented.

      @pre         none

      @post        none

      @param	   eventCode
			The code of the signalled event.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getEvent(AES_OHI_Eventcodes& eventCode);

/*=================================================================== */
   /**
      @brief       Retrieves an error code text.

      @pre         none

      @post        none

      @param       retCode
                        The numeral code to retrieve the text string for.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   const char* getErrCodeText(unsigned int retCode);

/*=================================================================== */
   /**
      @brief       Returns the state if the application is connected to the server

      @pre         none

      @post        none

      @param       retCode
                        The numeral code to retrieve the text string for.

      @return      Return status

      @exception   none
    */
/*=================================================================== */

   bool isConnected();

/*=================================================================== */
   /**
      @brief       Get the directory path where the application stores the files

      @pre         none

      @post        none

      @param       fileDest
                        The file transfer queue name

      @param       path
                        The physical path to the directory where the files should
			be stored.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getDestDirectoryPath(const char* fileDest, char* path);

/*=================================================================== */
   /**
      @brief       Set the directory path where the files should be stored for
		   a given dest.

      @pre         none

      @post        none

      @param       fileDest
                        The file destination name

      @param       path
                        The physical path to the directory where the files should
                        be stored.

      @return      Error code

      @exception   none
    */
/*=================================================================== */
   unsigned int setDestDirectoryPath(const char* fileDest, const char* filePath);

/*=================================================================== */
   /**
      @brief       removeFile a file from a given file destination.

      @pre         none

      @post        none

      @param       fileDest
                        The name of the file transfer queue

      @param       fileName
                        The file to be removed.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int removeFile(const char* fileDest, const char* fileName);

/*=================================================================== */
   /**
      @brief       sendFileManually to a specific destination without using
		   a file transfer queue.
		   
      @pre         none

      @post        none

      @param       filename
                        The name of the file to send. 

      @param       path 
                        The path to the directory where the file is located.

      @param       destination
                        The destination to where the file has to be sent.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int sendFileManually(const char* filename,
                                 const char* path,
                                 const char* destination,
                                 int retryTimes,
                                 int retryTimeInterval);

/*===================================================================
                        PROTECTED DECLARATION SECTION
=================================================================== */
protected:
/*===================================================================
                        PROTECTED ATTRIBUTE
=================================================================== */
/*=================================================================== */
   /**
      @brief  implementation

              A pointer to the aes_ohi_extFileHandlerImplementation2 class.
   */
/*=================================================================== */
   aes_ohi_extFileHandlerImplementation2* implementation;
};

#endif
