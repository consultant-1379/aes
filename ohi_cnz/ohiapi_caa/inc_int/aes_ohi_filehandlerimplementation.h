/*=================================================================== */
	/**
	@file   aes_ohi_filehandlerimplementation.h

	@brief
	A wrapper to aes_afp_api to send files and directories.
	General rule: The methods returns an unsigned int as error code
	to indicate any error. See AES_OHI_Errorcodes
	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       26/08/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_OHI_FILEHANDLERIMPLEMENTATION_H
#define AES_OHI_FILEHANDLERIMPLEMENTATION_H 
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ACE.h>
#include <ace/Semaphore.h>
#include "aes_ohi_filehandler_r2.h"
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_filestates.h"
#include "aes_ohi_format.h"
#include "ohiafpcomm.h"

/*====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class aes_ohi_fileHandler_r1;
class aes_ohi_fileHandler_r2;
//class ohiAfpComm;

/*=====================================================================
						CLASS DECLARATION SECTION
==================================================================== */
class aes_ohi_fileHandlerImplementation 
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

		  @brief           aes_ohi_fileHandlerImplementation
						   Creates an file handler object

		  @param           userFileDest
						   Reference of aes_ohi_fileHandler_r2 object.

		  @param           subSys
						   Name of the applications sub system. Used in the protection

		  @param           appName
						   Name of the application that are attaching. Used in the protection

		  @param           fileDest
						   Name of the file destination

		  @param           eventText
						   Event text used in the event generated for an alarm "Link-down"
						   or "File transfer failure".

		  @param           path
						   The physical path to the directory
		  @return          void

		  @exception       none
	 */

/*=================================================================== */
   aes_ohi_fileHandlerImplementation(aes_ohi_fileHandler_r2 *userFileDest,
                                     const char* subSys,
                                     const char* appName,
                                     const char* fileDest,
                                     const char* eventText,
                                     const char* path);

/*=================================================================== */
   	   /**

   		  @brief           aes_ohi_fileHandlerImplementation
   						   Creates an file handler object

   		  @param           userFileDest
						   Reference of aes_ohi_fileHandler_r1 object.

   		  @param           subSys
   						   Name of the applications sub system. Used in the protection

   		  @param           appName
   						   Name of the application that are attaching. Used in the protection

   		  @param           fileDest
   						   Name of the file destination

   		  @param           eventText
   						   Event text used in the event generated for an alarm "Link-down"
   						   or "File transfer failure".

   		  @param           path
   						   The physical path to the directory
   		  @return          void

   		  @exception       none
   	 */

/*=================================================================== */
   aes_ohi_fileHandlerImplementation(aes_ohi_fileHandler_r1 *userFileDest,
                                     const char* subSys,
                                     const char* appName,
                                     const char* fileDest,
                                     const char* eventText,
                                     const char* path);

/*===================================================================
					   CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   	 /**

   	 @brief           aes_ohi_fileHandlerImplementation destructor
   					  Deletes file handler object

   	 @exception       none
   	 */
/*=================================================================== */

   ~aes_ohi_fileHandlerImplementation();

/*=================================================================== */
   	 /**
   	 @brief       Attach to a destination and start the session by establishing
   				  an connection to GOH. The file destination is protected.

   	 @post        none

   	 @return      returns aes error code

   	 @exception   none
   	  */
/*=================================================================== */
   unsigned int attach();
/*=================================================================== */
   	 /**
   	 @brief       Gets the directory path where the files should be stored

   	 @param       path
   				  The physical path to the directory where the files should be stored
   	 @return      returns  unsigned int aeserror code

   	 @exception   none
   	  */
/*=================================================================== */

   unsigned int getDirectoryPath(std::string& path);
/*=================================================================== */
   	/**
   	@brief       Sends a file to the attached destination.

   	@param       fileName
				 The name of the file or the directory to be sent
   	@param       sendFormat
				 When using a FTP destination the format can be one of the following
				 binary or ASCII or the template.
	@param       fileMask
				 The fileMask of the file or the directory to be sent
	@param       isDirectory
				 Flag to specify the file or directory to be sent

   	@return      returns  unsigned int aeserror code

   	@exception   none
   	 */
/*=================================================================== */

   unsigned int send(const std::string fileName,
                     AES_OHI_Format sendFormat,
                     const std::string fileMask = "",
                     bool isDirectory = false);

/*=================================================================== */
   	/**
   	@brief       Sets the transfer state for a file that was sent to the file destination

   	@param       fileName
				 The name of the file which status is to be set.
   	@param       destination
				 The name of the destination.
   	@param       status
				 AES_OHI_Filestates : The transfer state to set.

   	@return      returns  unsigned int aeserror code

   	@exception   none
   	 */
/*=================================================================== */

   unsigned int setTransferState(std::string fileName,
                                 std::string destination,
                                 AES_OHI_Filestates status);

/*=================================================================== */
   	/**
   	@brief       Gets the transfer state of a file that was sent to the file destination

   	@param       fileName
   				 The name of the file which status is required.

   	@param       status
   				 The transfer state of the file.
   	@return      returns  unsigned int aeserror code

   	@exception   none
   	 */
/*=================================================================== */
   unsigned int getTransferState(const std::string fileName, AES_OHI_Filestates& status);

/*=================================================================== */
   	/**
   	@brief       Get the transfer state and other data of a file that
   				 was sent to the file destination.

   	@param       fileName
   				 The name of the file for which status and data is required.

   	@param       status
   				 The transfer state of the file.

   	@param       destination
   				 The destination name associated with a transfer queue.

   	@param       listOrder
   				 Specifies which file or directory is listed if parameter file name is empty.
   				   0 = Oldest file
   				   1 = Youngest file
   				   2 = All
   				   3 = Omitted
   				   4 = Next file

   	@param       reportDate
   				 Date when file or directory was reported.

   	@param       deleteDate
   				 Date when file or directory was set to status delete.

   	@param       manualStart
   				 Date when file or directory was set to status send.

   	@param       manualStop
   				 Date when file or directory was set to status stopped.

   	@param       failDate
             	     Date when file or directory was set to status failed.

   	@param       archiveDate
              	     Date when file or directory was set to archive.

   	@param       reasonForFailed
   				 Reason for transfer failure.
   					   0 = OK
   					   10 = Transfer failure

   	@param       isDirectory
   				 Is true if file name is a directory, false if file name is a file.

   	@return      returns  unsigned int aeserror code

   	@exception   none
   	 */
/*=================================================================== */
   unsigned int getTransferStateEx(std::string& fileName,
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
                                   bool& isDirectory);

/*=================================================================== */
   	/**
   	@brief       Retrieves the name of the last file that the application has sent to GOH
   				 for this filedest. The Intended use is to be able to synchronize after restart.

   	@param       fileName
   				 The name of the file which status is required.

   	@return      returns  unsigned int aeserror code

   	@exception   none
   	 */
/*=================================================================== */
   unsigned int getLastSentFile(std::string& fileName);

/*=================================================================== */
   	/**
   	@brief       Retrieves the name of a file that generated the failed transfer event.

   	@param       fileName
   				 The name of the file.

   	@return      returns  unsigned int aeserror code

   	@exception   none
   	 */
/*=================================================================== */
   unsigned int getEventFileName(std::string& fileName);

/*=================================================================== */
   	/**
   	@brief       Get event handle. The handle is used to inform application of events.
   				 The getEventHandle is used with getEvent to replace callback handleEvent.

   	@return      Returns an semaphore handle to the event that can be used in wait.
   				 NULL indicates failure to create the event handle.

   	@exception   none
   	 */
/*=================================================================== */
   ACE_Semaphore * getEventHandle();

/*=================================================================== */
   	/**
   	@brief       Get the event code associated with the event signaled through
   				 the event handle and removes it from the queue.
   				 Not to be used if callback is implemented.

   	@param       eventCode
   				 The code of the signaled event.

   	@return      returns  unsigned int aeserror code

   	@exception   none
   	 */
/*=================================================================== */
   unsigned int getEvent(AES_OHI_Eventcodes& eventCode);

/*=================================================================== */
   	/**
   	@brief    	Reverses the attach unreserve the FileDestination, and close the
   				connection to GOH.

   	@return     returns  unsigned int aeserror code

   	@exception  none
   	 */
/*=================================================================== */
   unsigned int detach();

/*=================================================================== */
   	/**
   	@brief    	Retrieves an error code text.

   	@param      retCode
   				The numeral code to retrieve the text string for.

   	@return     returns  error code text.

   	@exception  none
   	 */
/*=================================================================== */
   const char* getErrCodeText(unsigned int retCode);

/*=================================================================== */
   	/**
   	@brief    	Returns the state if the application is connected to the server

   	@return     returns  true or false

   	@exception  none
   	 */
/*=================================================================== */
   bool isConnected();

/*=================================================================== */
      	/**
      	@brief    	Sets the directory path of the file

      	@param      file
					Name of the file that has to be tranferred.

		@param      filePath
					File path of the file.

      	@return     returns  unsigned int aeserror code
      	@exception  none
      	 */
/*=================================================================== */

   unsigned int setDirectoryPath(const std::string file, const std::string filePath);
/*=================================================================== */
		/**
		@brief    	handle event from AFP ( set semaphore or call call-back )

		@param      eventCode
					The code of the signaled event.
		@return     returns  unsigned int aeserror code
		@exception  none
		 */
/*=================================================================== */
   unsigned int event(AES_OHI_Eventcodes eventCode);
/*===================================================================
				   PUBLIC DECLARATION SECTION
=================================================================== */
protected:
/*=================================================================== */
   /**
    * @brief 	subSystem
    *			Users subsystem
    */
/*=================================================================== */
   std::string subSystem;

/*=================================================================== */
  /**
   * @brief 	applicationName
   *			Users application name
   */
/*=================================================================== */
   std::string applicationName;

/*=================================================================== */
	 /**
	  * @brief 	fileDestination
	  *			file destination path
	  */
/*=================================================================== */
   std::string fileDestination;

/*=================================================================== */
	/**
	 * @brief 	eventTextStr
	 *			eventTextStr
	 */
/*=================================================================== */
   std::string eventTextStr;

/*=================================================================== */
	/**
	 * @brief 	destpath
	 * 			destination path
	 */
/*=================================================================== */
   std::string destpath;
/*=================================================================== */
	/**
	 * @brief 	eventFileName
	 * 			Stores file name associated with event
	 */
/*=================================================================== */
   std::string eventFileName;

/*=================================================================== */
	/**
	 * @brief 	eventHandle
	 * 			Semaphore Handle used for event handling.
	 */
/*=================================================================== */
   ACE_Semaphore * eventHandle;

/*=================================================================== */
	/**
	 * @brief 	connected
	 * 			status of connection for application with server.
	 */
/*=================================================================== */
   bool connected;
/*=================================================================== */
	/**
	 * @brief 	errorInSemaphore
	 * 			Sets when semaphore error occurs
	 */
/*=================================================================== */
   unsigned int errorInSemaphore;
/*=================================================================== */
	/**
	 * @brief 	afpApi
	 * 			link forward to AFP
	 */
/*=================================================================== */
   ohiAfpComm* afpApi;

/*=================================================================== */
	/**
	 * @brief 	userFile
	 * 			link back to user
	 */
/*=================================================================== */
   aes_ohi_fileHandler_r1* userFile;
/*=================================================================== */
	/**
	 * @brief 	user2File
	 * 			link back to user
	 */
/*=================================================================== */
   aes_ohi_fileHandler_r2* user2File;        //link back to user	
};

#endif
