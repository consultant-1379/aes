/*=================================================================== */
	/**
	@file   aes_ohi_filehandler_r2.h

	@brief
	Provides an interface to send files.
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
#ifndef AES_OHI_FILEHANDLER_R2_H
#define AES_OHI_FILEHANDLER_R2_H 

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
//#include "aes_ohi_filehandlerimplementation.h"
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_filestates.h"
#include "aes_ohi_format.h"
#include <ace/Semaphore.h>

/*====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class aes_ohi_fileHandlerImplementation;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class aes_ohi_fileHandler_r2
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

	  @brief           aes_ohi_fileHandler_r2

					   Creates an file handler object

	  @param           subSys
                       Name of the applications sub system. Used in the protection

      @param           appName
                       Name of the application that are attaching. Used in the protection

      @param           fileDest
                       Name of the file destination

      @param           eventText
                       Event text used in the event generated for an alarm "Link-down"
                       or "File transfere failure.

      @return          void

      @exception       none
	*/
/*=================================================================== */

   aes_ohi_fileHandler_r2(const char* subSys,
                          const char* appName,
                          const char* fileDest,
                          const char* eventText = "");
/*=================================================================== */
  /**

	 @brief           aes_ohi_fileHandler_r2

					  Creates an file handler object

	 @param           subSys
					  Name of the applications sub system. Used in the protection

	 @param           appName
					  Name of the application that are attaching. Used in the protection

	 @param           fileDest
					  Name of the file destination

	 @param           eventText
					  Event text used in the event generated for an alarm "Link-down"
					  or "File transfere failure.

	 @param           path
					  The physical path to the directory

	 @return          void

	 @exception       none
	 */

/*=================================================================== */

   aes_ohi_fileHandler_r2(const char* subSys,
                          const char* appName,
                          const char* fileDest,
                          const char* eventText,
                          const char* path);


/*===================================================================
					   CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
  /**

	 @brief           aes_ohi_fileHandler_r2 destructor

					  Deletes file handler object

	 @exception       none
  */
/*=================================================================== */
   ~aes_ohi_fileHandler_r2();

/*===================================================================
						  PUBLIC METHOD
=================================================================== */
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
   unsigned int getDirectoryPath(char* path);

/*=================================================================== */
 /**
 @brief       Sends a file to the attached destination.

 @param       fileName
			  The name of the file or the directory to be sent
 @param       sendFormat
			  When using a FTP destination the format can be one of the following
			  binary or ASCII or the template.
 @return      returns  unsigned int aeserror code

 @exception   none
  */
/*=================================================================== */
   unsigned int send(const char* fileName, AES_OHI_Format sendFormat = AES_OHI_BINARY);

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
	unsigned int setTransferState(const char* fileName,
                                 const char* destination,
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
   unsigned int getTransferState(const char* fileName, AES_OHI_Filestates& status);

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

	unsigned int getTransferStateEx(char* fileName,
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
   unsigned int getLastSentFile(char* fileName);

/*=================================================================== */
	/**
	@brief       Retrieves the name of a file that generated the failed transfer event.

	@param       fileName
				 The name of the file.

	@return      returns  unsigned int aeserror code

	@exception   none
	 */
/*=================================================================== */

   unsigned int getEventFileName(char * fileName);
/*=================================================================== */
	/**
	@brief       A call back to be implemented by the user

	@param       eventCode
				 The code of the signaled event.

	@return      returns  unsigned int aeserror code

	@exception   none
	 */
/*=================================================================== */

   unsigned int virtual handleEvent(AES_OHI_Eventcodes eventCode)
   {
	   eventCode = AES_OHI_EVELOSTSERVER;
	   return AES_OHI_EVENTNOTHANDLED;
   };
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

/*=====================================================================
				PROTECTED DECLARATION SECTION
==================================================================== */
protected:
/*=================================================================== */
/**
	  @brief  implementation

			   Reference of aes_ohi_fileHandlerImplementation
 */
/*=================================================================== */
   aes_ohi_fileHandlerImplementation* implementation;
};



class aes_ohi_fileHandler_r1
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

	  @brief           aes_ohi_fileHandler_r1

					   Creates an file handler object

	  @param           subSys
					   Name of the applications sub system. Used in the protection

	  @param           appName
					   Name of the application that are attaching. Used in the protection

	  @param           fileDest
					   Name of the file destination

	  @param           eventText
					   Event text used in the event generated for an alarm "Link-down"
					   or "File transfere failure.

	  @exception       none
 */
/*=================================================================== */
   aes_ohi_fileHandler_r1(const char* subSys,
                          const char* appName,
                          const char* fileDest,
                          const char* eventText = "");

/*=================================================================== */
	 /**

	 @brief           aes_ohi_fileHandler_r2

					  Creates an file handler object

	 @param           subSys
					  Name of the applications sub system. Used in the protection

	 @param           appName
					  Name of the application that are attaching. Used in the protection

	 @param           fileDest
					  Name of the file destination

	 @param           eventText
					  Event text used in the event generated for an alarm "Link-down"
					  or "File transfere failure.

	 @param           path
					  The physical path to the directory

	 @return          void

	 @exception       none
	 */

/*=================================================================== */
   aes_ohi_fileHandler_r1(const char* subSys,
                          const char* appName,
                          const char* fileDest,
                          const char* eventText,
                          const char* path);

/*===================================================================
				   CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
	 /**

	 @brief           aes_ohi_fileHandler_r1 destructor

					  Deletes file handler object

	 @exception       none
	 */
/*=================================================================== */
   ~aes_ohi_fileHandler_r1();
   /*===================================================================
   						  PUBLIC METHOD
=================================================================== */
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
   unsigned int getDirectoryPath(char* path);

/*=================================================================== */
	/**
	@brief       Sends a file to the attached destination.

	@param       fileName
			  The name of the file or the directory to be sent
	@param       sendFormat
			  When using a FTP destination the format can be one of the following
			  binary or ASCII or the template.
	@return      returns  unsigned int aeserror code

	@exception   none
	 */
/*=================================================================== */
   unsigned int send(const char* fileName, AES_OHI_Format sendFormat = AES_OHI_BINARY);

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
	unsigned int setTransferState(const char* fileName,
                                 const char* destination,
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
   unsigned int getTransferState(const char* fileName, AES_OHI_Filestates& status);

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
   unsigned int getLastSentFile(char* fileName);

/*=================================================================== */
	/**
	@brief       Retrieves the name of a file that generated the failed transfer event.

	@param       fileName
				 The name of the file.

	@return      returns  unsigned int aeserror code

	@exception   none
	 */
/*=================================================================== */
   unsigned int getEventFileName(char* fileName);


/*=================================================================== */
	/**
	@brief       A call back to be implemented by the user

	@param       eventCode
				 The code of the signaled event.

	@return      returns  unsigned int aeserror code

	@exception   none
	 */
/*=================================================================== */
   unsigned int virtual handleEvent(AES_OHI_Eventcodes eventCode)
   {
	   eventCode = AES_OHI_EVELOSTSERVER;
	   return AES_OHI_EVENTNOTHANDLED;
   };

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
/*===================================================================
						PROTECTED DECLARATION SECTION
=================================================================== */
protected:
/*=================================================================== */
	 /** @brief aes_ohi_fileHandlerImplementation
				Reference of the aes_ohi_fileHandlerImplementation class.
	 */
/*===================================================================*/
   aes_ohi_fileHandlerImplementation* implementation;
};

#endif
