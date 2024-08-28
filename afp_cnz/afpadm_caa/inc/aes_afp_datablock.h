/*=================================================================== */
/**
   @file   	aes_afp_datablock.h

   @brief 	Header file for aes_afp_datablock class
			This class holds the data that is exchanged between the
			different threads.

   @version 1.0.0
 */
/*
   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A      22/09/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef aes_afp_datablock_h
#define aes_afp_datablock_h 1
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_DSD_Session.h>
#include "aes_afp_msgtypes.h"
#include "aes_afp_renametypes.h"
#include <string>
#include <ace/ACE.h>
#include <ace/OS.h>
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_filestates.h>
#include <aes_gcc_format.h>
#include <aes_gcc_util.h>

/*====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_afp_datablock

                 This class holds the data that is exchanged between the
                 different threads.
 */
/*=================================================================== */
class aes_afp_datablock 
{
	/*====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */

public:
	/*=====================================================================
							CLASS CONSTRUCTORS
	======================================================================= */
	/*=================================================================== */
	/**
			  @brief           Default constructor
								Creates a default object
			  @return          void

			  @exception       none
	 */
	/*=================================================================== */
	aes_afp_datablock();
	/*=====================================================================
						CLASS DESTRUCTOR
	======================================================================= */
	/*=================================================================== */
	/**

		  @brief           Destructor

		  @exception       none
	 */
	/*=================================================================== */
	virtual ~aes_afp_datablock();
	/*=================================================================== */
	/**

	@brief           	setTransferQueue
						Set the transfer queue name.
	@param				transferQueue
						TranferQueue name
	@return          	void

	@exception       	none
	 */
	/*=================================================================== */
	void setTransferQueue (std::string transferQueue);
	/*=================================================================== */
	/**

	@brief           	getTransferQueue
						Retrieves the transfer queue name.
	@return          	transfer queue name as a string

	@exception       	none
	 */
	/*=================================================================== */
	std::string getTransferQueue ();

	/*=================================================================== */
	/**

	@brief           	setFilename
						Set the filename.
	@param				filename
						file name
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setFilename (std::string filename);
	/*=================================================================== */
	/**

	@brief           	setFilename
					          setr  the original filename.
	@return        		void
	@exception       	none
	 */
	/*=================================================================== */
	 void setOriginalFileName(std::string orgFileName);					//HU96961
        /*=================================================================== */
        /**

        @brief                  getFilename
                                                Retrieves the filename.
        @return                 file name as a string
        @exception              none
         */
        /*=================================================================== */

	std::string getFilename ();
	/*=================================================================== */
	/**

	@brief           	setGeneration
						Sets the generation for a send item.
	@param				generation
						Generation as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setGeneration (std::string generation);

	/*=================================================================== */
	/**
	@brief           	getGeneration
						Retrieves the generation for the send item.
	@return          	Generation as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getGeneration ();
	std::string dest[3];
	
	/*=================================================================== */
	/**

	@brief           	setDestinationSet
						Sets the destinationset name.
	@param				destinationSet
						destinationSet as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setDestinationSet (std::string destinationSet);

	/*=================================================================== */
	/**

	@brief           	getDestinationSet
						Retrieves the destination.
	@return          	DestinationSet as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getDestinationSet ();

	/*=================================================================== */
	/**

	@brief           	setStatus
						Set the status in a datablock.
	@param				status
						AES_GCC_Filestates
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setStatus (AES_GCC_Filestates status);

	/*=================================================================== */
	/**

	@brief           	getStatus
						Returns the status field from a datablock.
	@return          	AES_GCC_Filestates
	@exception       	none
	 */
	/*=================================================================== */
	AES_GCC_Filestates getStatus () const;

	/*=================================================================== */
	/**

	@brief           	setRemoveDelay
						Sets the remove delay.
	@param				removeDelay
						Delay value
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */

	void setRemoveDelay (ACE_INT32 removeDelay);
	/*=================================================================== */
	/**

	@brief           	getRemoveDelay
						Returns the remove delay.
	@return          	delay value
	@exception       	none
	 */
	/*=================================================================== */
	ACE_INT32 getRemoveDelay () const;

	/*=================================================================== */
	/**

	@brief           	setReadyDate
						Sets the ready date.
	@param				date
						date as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setReadyDate (std::string date);

	/*=================================================================== */
	/**

	@brief           	getReadyDate
						Returns the date when send item was set to ready.
	@return          	date as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string getReadyDate ()const;

	/*=================================================================== */
	/**

	@brief           	setSendDate
						Sets the date for send.
	@param				date
						date as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setSendDate (std::string date);

	/*=================================================================== */
	/**

	@brief           	getSendDate
						Returns the date for status send.
	@return          	date as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string getSendDate ()const;

	/*=================================================================== */
	/**

	@brief           	setFailedDate
						Sets status failed date.
	@param				date
						date as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setFailedDate (std::string date);

	/*=================================================================== */
	/**

	@brief           	getFailedDate
						Returns the date when send item was in status failed.
	@return          	fail date a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string getFailedDate ()const;

	/*=================================================================== */
	/**

	@brief           	setDeleteDate
						Sets date when send item was in status delete.
	@param				date
						date as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setDeleteDate (std::string date);

	/*=================================================================== */
	/**

	@brief           	getDeleteDate
						Returns the date when send item was in status delete.
	@return          	Delete date as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string getDeleteDate ()const;

	/*=================================================================== */
	/**

	@brief           	setNoneDate
						Sets the date for status none.
	@param				date
						date as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setNoneDate (std::string date);

	/*=================================================================== */
	/**

	@brief           	getNoneDate
						Returns the date for status none.
	@return          	date as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string getNoneDate ()const;

	/*=================================================================== */
	/**

	@brief           	setArchiveDate
						Sets the archive date.
	@param				date
						ArchiveDate as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setArchiveDate (std::string date);

	/*=================================================================== */
	/**

	@brief           	getArchiveDate
						Returns the date when state was 'archive'
	@return          	gets ArchiveDate as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string getArchiveDate ()const;

	/*=================================================================== */
	/**

	@brief           	setStoppedDate
						Sets date when file was stopped.
	@param				date
						stopped date as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setStoppedDate (std::string date);

	/*=================================================================== */
	/**

	@brief           	getStoppedDate
						Retrieves the date when the file was stopped.
	@return          	gets the StoppedDate as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string getStoppedDate ()const;

	/*=================================================================== */
	/**

	@brief           	getStrBuf
						Returns a referencer to string object.
	@return          	string buffer
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getStrBuf ();

	/*=================================================================== */
	/**

	@brief           	setSendRetries
						Sets the send retries.
	@param				retries
						send retries number ACE_INT32
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setSendRetries (ACE_INT32 retries);

	/*=================================================================== */
	/**

	@brief           	getSendRetries
						Retrieves the value for send retries.
	@return          	ACE_INT32
	@exception       	none
	 */
	/*=================================================================== */
	ACE_INT32 getSendRetries ()const;

	/*=================================================================== */
	/**

	@brief           	setMsgType
						Set the message type in a datablock.
	@param				type
						aes_afp_msgtypes
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setMsgType (aes_afp_msgtypes type);

	/*=================================================================== */
	/**
	@brief           	getMsgType
						Retrieves the message type from a datablock.
	@return          	aes_afp_msgtypes
	@exception       	none
	 */
	/*=================================================================== */
	aes_afp_msgtypes getMsgType ()const;

	/*=================================================================== */
	/**
	@brief           	setErrorCode
						Sets the error code in a datablock.
	@param				code
						AES_GCC_Errorcodes
	@return          	file name as a string
	@exception       	none
	 */
	/*=================================================================== */
	void setErrorCode (AES_GCC_Errorcodes code);

	/*=================================================================== */
	/**

	@brief           	getErrorCode
						Retrieves the error code from a datablock.
	@return          	AES_GCC_Errorcodes
	@exception       	none
	 */
	/*=================================================================== */
	AES_GCC_Errorcodes getErrorCode ()const;

	/*=================================================================== */
	/**

	@brief           	setFullPath
						Set full physical path.
	@param				path
						path as a string variable
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setFullPath (std::string path);

	/*=================================================================== */
	/**

	@brief           	getFullPath
						Retrieves the full path
	@return          	Full path as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getFullPath ();

	/*=================================================================== */
	/**

	@brief           	setRetriesDelay
						Sets delay time between send retries.
	@param				retriesDelay
						Retries delay
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setRetriesDelay (ACE_INT32 retriesDelay = 0);

	/*=================================================================== */
	/**

	@brief           	getRetriesDelay
						Retrieves delay time between send retries.
	@return          	Delay value
	@exception       	none
	 */
	/*=================================================================== */
	ACE_INT32 getRetriesDelay ()const;

	/*=================================================================== */
	/**
	@brief           	setRemoveBefore
						Sets the removebefore_ flag to true
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setRemoveBefore ();

	/*=================================================================== */
	/**

	@brief           	unsetRemoveBefore
						Sets the removebefore_ flag to false
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void unsetRemoveBefore ();

	/*=================================================================== */
	/**
	@brief           	getRemoveBefore
						Returns true if the send item remove counter
						starts when the send item is reported.
	@return          	" true " or "false"
	@exception       	none
	 */
	/*=================================================================== */
	bool getRemoveBefore ()const;
	/*=================================================================== */
	/**

	@brief           	setManual
						Sets manual transfer mode.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setManual ();

	/*=================================================================== */
	/**

	@brief           	getManual
						Retrieves manual mode.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	bool getManual ()const;

	/*=================================================================== */
	/**

	@brief           	setLock
						Set lock to true.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setLock ();

	/*=================================================================== */
	/**

	@brief           	unsetLock
						Set lock to false.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void unsetLock ();

	/*=================================================================== */
	/**

	@brief           	getLock
						Checks if lock is set.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	bool getLock ()const;

	/*=================================================================== */
	/**

	@brief           	setRangeBegin
						Set a the first file in a range of files.
	@param				rangeBegin
						first file in a range as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setRangeBegin (std::string rangeBegin);

	/*=================================================================== */
	/**

	@brief           	getRangeBegin
						Retrieve the first subfile in range.
	@return          	First file in a range
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getRangeBegin ();

	/*=================================================================== */
	/**

	@brief                 getOriginalFilename
					             Retrieve the Original Filename.
	@param				Filename
							@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	 std::string& getOriginalFileName();                                                //HU96961

        /*=================================================================== */
        /**

        @brief                  setRangeEnd
                                                Set a the last subfile in a range of subfiles.
        @param                          rangeEnd
                                                Last file in a range of subfiles as a string
        @return                 void
        @exception              none
         */
        /*=================================================================== */

	void setRangeEnd (std::string rangeEnd);

	/*=================================================================== */
	/**

	@brief           	getRangeEnd
						Retrieve the last file in a range.
	@return          	Last file name in range as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getRangeEnd ();

	/*=================================================================== */
	/**

	@brief           	setNewDestinationSet
						Set name of new destination set.
	@param				newDestinationSet
						New destination set as a string
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setNewDestinationSet (std::string newDestinationSet);

	/*=================================================================== */
	/**

	@brief           	getNewDestinationSet
						Retrieve the new destinationset name.
	@return          	new destination set as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getNewDestinationSet ();

	/*=================================================================== */
	/**

	@brief           	setFileRename
						Set file renaming according to aes_afp_filerenametypes.
	@param				rename
						AES_AFP_Renametypes
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setFileRename (AES_AFP_Renametypes rename);

	/*=================================================================== */
	/**
	@brief           	getFileRename
						Retrieves the sort of file renaming specified.
	@return          	AES_AFP_Renametypes
	@exception       	none
	 */
	/*=================================================================== */
	AES_AFP_Renametypes getFileRename ()const;
	/*=================================================================== */
	/**
	@brief           	setFileTemplate
						Sets the file template to use.
	@param				renameTemplate
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setFileTemplate (std::string renameTemplate);

	/*=================================================================== */
	/**
	@brief           	getFileTemplate
						Retrieves the file template to use.
	@return          	File Template
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getFileTemplate ();

	/*=================================================================== */
	/**

	@brief           	setSourceDirectory
						Sets the directory where an
						application should store the files.
	@param				sourceDirectory
						sourceDirectory
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setSourceDirectory (std::string sourceDirectory);

	/*=================================================================== */
	/**

	@brief           	getSourceDirectory
						Retrieves the application directory.
	@return          	Source directory name
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getSourceDirectory ();
	/*=================================================================== */
	/**

	@brief           	setFormat
						Sets the transfer format for a file.
	@param				format
						AES_GCC_Format
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setFormat (AES_GCC_Format format);

	/*=================================================================== */
	/**

	@brief           	getFormat
						Retrieves the send format for a file.
	@return          	AES_GCC_Format
	@exception       	none
	 */
	/*=================================================================== */
	AES_GCC_Format& getFormat ();

	/*=================================================================== */
	/**

	@brief           	setMask
						Sets the transfer mask for a file.
	@param				mask
						mask
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setMask (std::string mask);

	/*=================================================================== */
	/**
	@brief           	getMask
						Retrieves the transfer mask for a file.
	@return          	Transfer Mask as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getMask ();

	/*=================================================================== */
	/**

	@brief           	setDirectoryFlag
						Sets the flag for indication of a directory.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setDirectoryFlag ();

	/*=================================================================== */
	/**

	@brief           	getDirectoryFlag
						Retrieves the indication of a directory.
	@return          	"true" if directory else "false"
	@exception       	none
	 */
	/*=================================================================== */
	bool getDirectoryFlag ()const;

	/*=================================================================== */
	/**

	@brief           	setDsdChannel
						Set the applications DSD channel.
	@param				channel
						DSD session object
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setDsdChannel (ACS_DSD_Session* channel);

	/*=================================================================== */
	/**

	@brief           	getDsdChannel
						Retrieves the DSD channel associated with
						the user application.
	@return          	DSD session object
	@exception       	none
	 */
	/*=================================================================== */
	ACS_DSD_Session* getDsdChannel ();

	/*=================================================================== */
	/**

	@brief           	setAfpRestarted
						Set a flag to true if AFP is restarted.
	@param				restarted
						"true" or "false"
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setAfpRestarted (bool restarted = false);

	/*=================================================================== */
	/**

	@brief           	getAfpRestarted
						Retrieves a flag that checks if AFP is restarted.
	@return          	"true" or "false"
	@exception       	none
	 */
	/*=================================================================== */
	bool getAfpRestarted ()const;

	/*=================================================================== */
	/**

	@brief           	setListString
						Sets the string used by list operations.
	@param				listString
						string name
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setListString (std::string listString = "");

	/*=================================================================== */
	/**

	@brief           	getListString
						Retrieves the string used in list operations.
	@return          	string name
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getListString ();

	/*=================================================================== */
	/**

	@brief           	setUsername
						Sets the user name.
	@param				username
						User name
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setUsername (std::string username);

	/*=================================================================== */
	/**

	@brief           	getUsername
						Retrieves the user name.
	@return          	User name as a string
	@exception       	none
	 */
	/*=================================================================== */

	std::string& getUsername ();
	/*=================================================================== */
	/**

	@brief           	setUserGroup
						Sets the user group.
	@param				userGroup
						User group name
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setUserGroup (std::string userGroup);

	/*=================================================================== */
	/**

	@brief           	getUserGroup
						Retrieves the user group.
	@return          	User group name as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getUserGroup ();

	/*=================================================================== */
	/**

	@brief           	setApplication
						Sets the application name.
	@param				application
						Application  name
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setApplication (std::string application);

	/*=================================================================== */
	/**

	@brief           	getApplication
						Retrieves the application name.
	@return          	Application name as a string
	@exception       	none
	 */
	/*=================================================================== */

	std::string& getApplication ();
	/*=================================================================== */
	/**

	@brief           	setNewFilename
						Sets a new filename.
	@param				newFilename
						New filename
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setNewFilename (std::string newFilename);

	/*=================================================================== */
	/**

	@brief           	getNewFilename
						Retrieves the new filename.
	@return          	New file name as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getNewFilename ();

	/*=================================================================== */
	/**

	@brief           	setNameTag
						Sets the name tag used for file renaming.
	@param				nameTag
						name tag to be set
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setNameTag (std::string nameTag);

	/*=================================================================== */
	/**

	@brief           	getNameTag
						Retrieves name tag for file renaming.
	@return          	name tag  as a string
	@exception       	none
	 */
	/*=================================================================== */
	std::string& getNameTag ();

	/*=================================================================== */
	/**

	@brief           	setStartupSequenceNumber
						Sets the startup sequence nr and rollover
						value used for file renaming.
	@param 				startupSequenceNumber
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setStartupSequenceNumber (ACE_INT64 startupSequenceNumber);

	/*=================================================================== */
	/**
	@brief           	getStartupSequenceNumber
						Retrieves name tag for file renaming.
	@return          	ACE_INT64 integer startup sequence number
	@exception       	none
	 */
	/*=================================================================== */
	ACE_INT64 getStartupSequenceNumber ()const;

	Dest_Set_Type m_dType;

	int m_nCount;
			
	/*====================================================================
						PRIVATE DECLARATION SECTION
	==================================================================== */
private:

	/*=================================================================== */
	/**
	@brief           	removeDelay_
						Delay time before a send item is removed.
	 */
	/*=================================================================== */
	ACE_UINT32 removeDelay_;
	/*=================================================================== */
	/**
			@brief           	sendRetries_
								Number of retries to resend a send item.
	 */
	/*=================================================================== */
	ACE_INT32 sendRetries_;
	/*=================================================================== */
	/**
			@brief           	retriesDelay_
								Delay time between send retries.
	 */
	/*=================================================================== */
	ACE_INT32 retriesDelay_;

	/*=================================================================== */
	/**
			@brief           	removeBefore_
								Set to true if the remove delay counter
								should start when the  send item is reported.
	 */
	/*=================================================================== */
	bool removeBefore_;

	/*=================================================================== */
	/**
			@brief           	manual_
								True if the order is manually initiated.
	 */
	/*=================================================================== */
	bool manual_;

	/*=================================================================== */
	/**
			@brief           	lock_
								True if the order is locked
								from removal by the user.
	 */
	/*=================================================================== */
	bool lock_;

	/*=================================================================== */
	/**
			@brief           	isDirectory_
								True if filename is a directory.
	 */
	/*=================================================================== */
	bool isDirectory_;

	/*=================================================================== */
	/**
			@brief           	afpRestarted_
								True if afp has been restarted.
	 */
	/*=================================================================== */
	bool afpRestarted_;

	/*=================================================================== */
	/**
			@brief           	fileGeneration_
								generation of FMS subfile - not used.
	 */
	/*=================================================================== */
	std::string fileGeneration_;

	/*=================================================================== */
	/**
			@brief           	transferMask_
								Mask for transfer of files.
	 */
	/*=================================================================== */
	std::string transferMask_;

	/*=================================================================== */
	/**
			@brief           	originalfilename_
								Mask of original filename	.
	 */
	/*=================================================================== */
	std::string originalfilename_;							//HU96961

        /*=================================================================== */
        /**
                        @brief                  rangeEnd_
                                                                End of a file range.
         */
        /*=================================================================== */

	std::string rangeEnd_;

	/*=================================================================== */
	/**
			@brief           	listOperations_
								Operation on destination set list.
	 */
	/*=================================================================== */
	std::string listOperations_;

	/*=================================================================== */
	/**
			@brief           	rangeBegin_
								Start of range.
	 */
	/*=================================================================== */
	std::string rangeBegin_;

	/*=================================================================== */
	/**
			@brief           	transferQueue_
								Name of transfer queue.
	 */
	/*=================================================================== */
	std::string transferQueue_;

	/*=================================================================== */
	/**
			@brief           	filename_
								Name of file.
	 */
	/*=================================================================== */
	std::string filename_;

	/*=================================================================== */
	/**
			@brief           	path_
								path
	 */
	/*=================================================================== */
	std::string path_;

	/*=================================================================== */
	/**
			@brief           	strBuf_
								Buffer for transporting data.
	 */
	/*=================================================================== */
	std::string strBuf_;

	/*=================================================================== */
	/**
			@brief           	username_
								Name of command user.
	 */
	/*=================================================================== */
	std::string username_;

	/*=================================================================== */
	/**
			@brief           	destinationSet_
								Name of destination set.
	 */
	/*=================================================================== */
	std::string destinationSet_;

	/*=================================================================== */
	/**
			@brief           	sourceDirectory_
								Name of applications source directory.
	 */
	/*=================================================================== */
	std::string sourceDirectory_;

	/*=================================================================== */
	/**
			@brief           	renameTemplate_
								Template for renaming files.
	 */
	/*=================================================================== */
	std::string renameTemplate_;

	/*=================================================================== */
	/**
			@brief           	newDestinationSet_
								Name of new destination set.
	 */
	/*=================================================================== */
	std::string newDestinationSet_;

	/*=================================================================== */
	/**
			@brief           	userGroup_
								Name of user group.
	 */
	/*=================================================================== */
	std::string userGroup_;

	/*=================================================================== */
	/**
			@brief           	generation_
								Generation for FMS subfile - not used.
	 */
	/*=================================================================== */
	std::string generation_;

	/*=================================================================== */
	/**
			@brief           	application_
								Name of application.
	 */
	/*=================================================================== */
	std::string application_;

	/*=================================================================== */
	/**
			@brief           	newFilename_
								New filename.
	 */
	/*=================================================================== */
	std::string newFilename_;

	/*=================================================================== */
	/**
			@brief           	sendDate_
								Date for status delete.
	 */
	/*=================================================================== */
	std::string sendDate_;

	/*=================================================================== */
	/**
			@brief           	readyDate_
								Date for status ready.
	 */
	/*=================================================================== */
	std::string readyDate_;

	/*=================================================================== */
	/**
			@brief           	deleteDate_
								Date for status delete.
	 */
	/*=================================================================== */
	std::string deleteDate_;

	/*=================================================================== */
	/**
			@brief           	noneDate_
								Date for status none.
	 */
	/*=================================================================== */
	std::string noneDate_;

	/*=================================================================== */
	/**
			@brief           	archiveDate_
								Date for status archive.
	 */
	/*=================================================================== */
	std::string archiveDate_;

	/*=================================================================== */
	/**
			@brief           	stopDate_
								Date for status stopped.
	 */
	/*=================================================================== */
	std::string stopDate_;

	/*=================================================================== */
	/**
			@brief           	failedDate_
								Date for status failed.
	 */
	/*=================================================================== */
	std::string failedDate_;

	/*=================================================================== */
	/**
			@brief           	nameTag_
								Name tag for file renaming.
	 */
	/*=================================================================== */
	std::string nameTag_;

	/*=================================================================== */
	/**
			@brief           	startupSequenceNumber_
								Startup sequence number / rollover value
								for file renaming.
	 */
	/*=================================================================== */
	ACE_INT64 startupSequenceNumber_;

	/*=================================================================== */
	/**
			@brief           	status_
								AES_GCC_Filestates
	 */
	/*=================================================================== */
	AES_GCC_Filestates status_;

	/*=================================================================== */
	/**
			@brief           	messageType_
								aes_afp_msgtypes
	 */
	/*=================================================================== */
	aes_afp_msgtypes messageType_;

	/*=================================================================== */
	/**
			@brief           	errorCode_
								AES_GCC_Errorcodes
	 */
	/*=================================================================== */
	AES_GCC_Errorcodes errorCode_;

	/*=================================================================== */
	/**
			@brief           	transferFormat_
								Transfer Format
	 */
	/*=================================================================== */
	AES_GCC_Format transferFormat_;

	/*=================================================================== */
	/**
			@brief           	rename_
								AES_AFP_Renametypes
	 */
	/*=================================================================== */
	AES_AFP_Renametypes rename_;

	/*=================================================================== */
	/**
			@brief           	dsdChannel_
								dsd session object
	 */
	/*=================================================================== */
	ACS_DSD_Session *dsdChannel_;
};

#endif
