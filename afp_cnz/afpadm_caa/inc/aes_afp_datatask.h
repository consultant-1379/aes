/*=================================================================== */
/**
   @file   	aes_afp_datatask.h

   @brief 	Header file for aes_afp_datatask class
			The thread responsible for communication with the database.
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
#ifndef aes_afp_datatask_h
#define aes_afp_datatask_h 1

/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <iostream>
#include <string>

#include "aes_afp_datablock.h"
#include "aes_afp_apcom_serv.h"
#include "aes_afp_msgblock.h"
#include "aes_afp_threadmap.h"
#include "aes_afp_database_fd.h"
#include <aes_gcc_log.h>
#include <aes_gcc_util.h>

#include <ace/Time_Value.h>
#include <ace/Task.h>

#define AES_AFP_STS_CLIENT_CLASS1 "FileOutputJob"
#define AES_AFP_STS_CLIENT_CLASS2 "AxeStatisticalCounterMeasurementProgram"
#define AES_AFP_CPF_CLIENT_CLASS1 "AxeCpFileSystemInfiniteFile"
#define AES_AFP_ALOG_CLIENT_CLASS1 "AxeAuditLoggingAuditLoggingM"
#define AES_AFP_PDS_CLIENT_CLASS1 "AxeAlphanumericDevicePdsSubFile"
#define AES_AFP_RTR_CLIENT_CLASS   "AxeDataRecordFileBasedJob"

/*=====================================================================
                          TYPE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  AceTask_Datatask
           This class is responsible for the thread towards the database.
 */
/*=================================================================== */
typedef ACE_Task<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy> AceTask_Datatask;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_afp_datatask

                 The thread responsible for communication with the database.
 */
/*=================================================================== */

class aes_afp_datatask : public AceTask_Datatask
{
public:

	/// Constructor
	aes_afp_datatask();

	/// Destructor
	virtual ~aes_afp_datatask();

	inline bool isDbInitialized() const { return m_initialized; }

	/**
	 * @brief loadTQsDataBase
	 * 			Load the datatabase for all TQs by reading status files and IMM related information.
	 *
	 * @return bool

	 */
	bool loadTQsDataBase();

	/*=================================================================== */
	   /**

		  @brief           open

						   Opens a Task.

		  @param           args

		  @param           thrMgr
						   Instance of Thread Manager
		  @return          ACE_INT32

		  @exception       none
	   */
	/*=================================================================== */
	ACE_INT32 open (void* args = 0, ACE_Thread_Manager* thrMgr = 0);

	/*=================================================================== */
	   /**

		  @brief          	close

							Called from svc when task is ended.

		  @param          	flags
							Update the flags description
		  @return          	ACE_INT32

		  @exception       	none
	   */
	/*=================================================================== */
	ACE_INT32 close (unsigned long flags = 0);

	/*=================================================================== */
	   /**

		  @brief          	svc
							Run by a daemon thread to handle deferred processing.

		  @return          	ACE_INT32

		  @exception       	none
	   */
	/*=================================================================== */

	ACE_INT32 svc (void );
	/*=================================================================== */
	/**

			@brief          	set_stop
								sets the stop flag
			@param				stop
								"true" to set the stop flag else "false"
			@return          	void

			@exception       	none
	 */
	/*=================================================================== */
	void set_stop(bool stop);
	bool handle_createTransferQueue (aes_afp_datablock* dbBlock, AES_GCC_Errorcodes& code, Dest_Set_Type dType=DUMMY);
	bool handle_removeFile (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code);
	bool handle_removeFileFromSourceDirectory (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code);	//for APZ21230/5-758
	/*=================================================================== */
	/**
				@brief          	handle_resendAllFiles
									Resends all files in status FS_FAILED
									for one transfer queue.

				@param				dbBlock
									Pointer to aes_afp_datablock
				@return          	true on "success" else "false"

				@exception       	none
	 */
	/*=================================================================== */

	bool handle_resendAllFiles (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code);
	/*=================================================================== */
	/**
					@brief          	handle_stopSendFile
										Stop sending of one file.
					@param				dbBlock
										Pointer to aes_afp_datablock
					@return          	true on "success" else "false"

					@exception       	none
	 */
	/*=================================================================== */
	bool handle_stopSendFile (aes_afp_datablock* dbBlock, AES_GCC_Errorcodes& code);

        /*=================================================================== */
        /**
                        @brief                  handle_getTransferQueueAttributes
                                                                Retrieves the attributes for a transfer queue.
                        @param                          dbBlock
                                                                Pointer to aes_afp_datablock
                        @return                 true on "success" else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_getTransferQueueAttributes (aes_afp_datablock* dbBlock);
        aes_afp_datablock * handle_getTransferQueueAttributesToModify(aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code);

        /*=================================================================== */
        /**

                        @brief                  handle_removeDestinationSet
                                                                Removes a destination set from database.
                        @param                          dbBlock
                                                                Pointer aes_afp_datablock object
                        @return                 "true" on successful deletion of a particular
                                                                 destination set else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_removeDestinationSet (aes_afp_datablock* dbBlock);

        /*=================================================================== */
        /**

                        @brief                  handle_removeDestinationSet_OI
                                                Removes a destination set from database for IMM OM/OI architecture.
                        @param                          dbBlock
                                                                Pointer aes_afp_datablock object
                        @return                 "true" on successful deletion of a particular
                                                                 destination set else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_removeDestinationSet_OI (aes_afp_datablock* dbBlock, AES_GCC_Errorcodes& ret);

        /*=================================================================== */
        /**
                        @brief                  handle_removeSourceDirectoryFiles
                                                                Removes a Files reported in Source Directory
                        @param                          dbBlock
                                                                Pointer to aes_afp_datablock
                        @return                 true on "success" else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_removeSourceDirectoryFiles (aes_afp_datablock* dbBlock , AES_GCC_Errorcodes& code);

	/*=================================================================== */
		/**

				@brief          	handle_removeTransferQueue
									Removes a transfer queue from database.
				@param				dbBlock
									Pointer aes_afp_datablock object
				@return          	"true" if transfer queue removed else "false"

				@exception       	none
		 */
		/*=================================================================== */
	bool handle_removeTransferQueue (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code);
		/*=================================================================== */
	/*=================================================================== */
	/**
			@brief          	handle_getFileStatus
								Retrieves the status for a file.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_getFileStatus (aes_afp_datablock* dbBlock);

	bool handle_getFileInfo (aes_afp_datablock* dbBlock);
        bool handle_infoDestinationSetsForModify(aes_afp_datablock* dbBlock, string& destList,AES_GCC_Errorcodes& code);

         /*=================================================================== */
        /**
                        @brief                  handle_setTransferQueueAttributes
                                                                Sets transfer queue attributes.
                        @param                          dbBlock
                                                                Pointer to aes_afp_datablock
                        @return                 true on "success" else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_setTransferQueueAttributes (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code, bool changeRenameTemplate = false);

        /*=================================================================== */
        /**
                        @brief                  handle_sendManually
                                                                Sends a file manually.
                        @param                          dbBlock
                                                                Pointer to aes_afp_datablock
                        @return                 true on "success" else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_sendManually (aes_afp_datablock* dbBlock, AES_GCC_Errorcodes& errCode);


	/*=================================================================== */

        vector<TqInfo> handle_infoAllDestset(std::string);
	void setServer(aes_afp_apcom_serv* &obj);
	bool apcomThreadExists();

	std::string nameOftheClient;
	
	 /*=================================================================== */
        /**
                        @brief                  handle_transferQueueDefined
                                                                Indicates if an transfer queue  is defined in database.
                        @param                          dbBlock
                                                                Pointer to aes_afp_datablock
                        @return                 true on "success" else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_transferQueueDefined (aes_afp_datablock* dbBlock);

        /*=================================================================== */
        /**
                        @brief                  handle_transferQueueDestinationSetDefined
                                                                Indicates if an transfer queue is defined
                                                                to a specific destination set.
                        @param                          dbBlock
                                                                Pointer to aes_afp_datablock
                        @return                 true on "success" else "false"

                        @exception              none
         */
        /*=================================================================== */
        bool handle_transferQueueDestinationSetDefined (aes_afp_datablock* dbBlock);

	
private:
	/*=================================================================== */
	/**

			@brief          	handle_removeTransferQueue
								Removes a transfer queue from database.
			@param				dbBlock
								Pointer aes_afp_datablock object
			@return          	"true" if transfer queue removed else "false"

			@exception       	none
	 */
	/*=================================================================== */
	//bool handle_removeTransferQueue (aes_afp_datablock* dbBlock);
	/*=================================================================== */
	/**

			@brief          	handle_addDestinationSet
								Adds a destination set i database.
			@param				dbBlock
								Pointer aes_afp_datablock object
			@return          	"true" on successful destination set creation else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_addDestinationSet (aes_afp_datablock* dbBlock);
	/*=================================================================== */
	/**

			@brief          	handle_removeDestinationSet
								Removes a destination set from database.
			@param				dbBlock
								Pointer aes_afp_datablock object
			@return          	"true" on successful deletion of a particular
								 destination set else "false"

			@exception       	none
	 */
	/*=================================================================== */
	//bool handle_removeDestinationSet (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**

			@brief          	handle_createFile
								Creates a file in database.
			@param				dbBlock
								Pointer aes_afp_datablock object
			@return          	"true" on successful creation of a file
								 in database else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_createFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**

			@brief          	sendOk
								Sends an OK message to specified thread.
			@param				to
								void pointer to a thread
			@return          	void

			@exception       	none
	 */
	/*=================================================================== */
	void sendOk (void* to);
	/*=================================================================== */
	/**

			@brief          	sendNotOk
								Send NOT OK to a thread.
			@param				to
								void pointer to a thread
			@param				code
								AES_GCC_Errorcodes
			@return          	void

			@exception       	none
	 */
	/*=================================================================== */
	void sendNotOk (void* to, AES_GCC_Errorcodes code);

	/*=================================================================== */
	/**
			@brief          	handle_infoOneTransferQueue
								RetriEves information about one transfer queue
								and sends it to the caller.
			@param				dbBlock
								aes_afp_datablock
			@return          	void

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_infoOneTransferQueue (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_findTransferQueue
								Searches for a transfer queue.

			@param				dbBlock
								aes_afp_datablock
			@return          	true on finding the transfer queue else false

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_findTransferQueue (aes_afp_datablock* dbBlock);


	/*=================================================================== */
	/**
			@brief          	handle_infoAllTransferQueues
								Retrieves all information from database.

			@param				dbBlock
								aes_afp_datablock
			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */

	bool handle_infoAllTransferQueues (aes_afp_datablock* dbBlock);
	bool handle_infoAllTransferQueuesShort (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_findNewFile
								Finds the newest reported file.

			@param				dbBlock
								aes_afp_datablock
			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_findNewFile (aes_afp_datablock* dbBlock);
	/*=================================================================== */
	/**
			@brief          	handle_findOldFile
								Finds the oldest reported file.

			@param				dbBlock
								aes_afp_datablock
			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_findOldFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_setStatusFile
								Sets the status of a file.

			@param				dbBlock
								aes_afp_datablock
			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_setStatusFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_deleteFile
								Removes a file that is in status 'delete' and whose
								remove delay has expired.

			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_deleteFile ();

	/*=================================================================== */
	/**
			@brief          	handle_getStatusFile
								Retrieves the status for a specific file.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_getStatusFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_getStatusOldFile
								Retrieves status about the oldest reported file.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */

	bool handle_getStatusOldFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_getStatusNewFile
								Retrieves status about the youngest reported file..
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on success else false

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_getStatusNewFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_shutdown
								Shutdown database.

			@return          	void

			@exception       	none
	 */
	/*=================================================================== */
	void handle_shutdown ();

	/*=================================================================== */
	/**
			@brief          	handle_removeFile
								Removes a file from database.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	//bool handle_removeFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_statusarchive
								Set status for a file to archive.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_statusarchive (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_getTransferQueueAttributes
								Retrieves the attributes for a transfer queue.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	//bool handle_getTransferQueueAttributes (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_cdhSetStatusFile
								Used when CDH change status of a file.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */

	void handle_cdhSetStatusFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_stopSendAllFiles
								Stop sending of all files for a transfer queue.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_stopSendAllFiles (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_infoDestinationSets
								Retrieves information about all destination sets
								for a transfer queue.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_infoDestinationSets (aes_afp_datablock* dbBlock);


	/*=================================================================== */
	/**
			@brief          	handle_lockTransferQueue
								Locks an entry in the database.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */

	bool handle_lockTransferQueue (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_unlockTransferQueue
								Unlocks an entry in database
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_unlockTransferQueue (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_isTransferQueueLocked
								Check if a transfer queue is locked.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_isTransferQueueLocked (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_resendOneFile
								Resend files that are in status FS_FAILED
								from one destination set.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_resendOneFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_createTransferQueue
								Creates a transfer queue in the database.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	//bool handle_createTransferQueue (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_sendFile
								Inserts a file in a transfer queue.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_sendFile (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_getSourceDirectoryPath
								Retrieves the source directory path for a transfer queue.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_getSourceDirectoryPath (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_setSourceDirectoryPath
								Sets the applications source directory path.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_setSourceDirectoryPath (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_openTransferQueue
								Opens a transfer queue and
								sets the event subscription in CDH.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_openTransferQueue (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_closeTransferQueue
								Closes the transfer queue and
								ends the event subscription in CDH.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_closeTransferQueue (aes_afp_datablock* dbBlock);

	/*=================================================================== */
	/**
			@brief          	handle_lastReportedFile
								Retrieves the last reported file.
			@param				dbBlock
								Pointer to aes_afp_datablock
			@return          	true on "success" else "false"

			@exception       	none
	 */
	/*=================================================================== */
	bool handle_lastReportedFile (aes_afp_datablock* dbBlock);

	/**
	 * @brief	Retrieves a list of destination sets.
	 * @param	Pointer to aes_afp_datablock
	 * @return	true on "success" else "false"
	 * @exception       	none
	 */
	bool handle_getDestinationSetList ();

	/**
	 * @brief	Open a generic connection to AFP.
	 * @return	true on "success" else "false"
	 * @exception       	none
	 */
	bool handle_openGenericConnection ();

	/**
	 * @brief   Get status for next file in tq's file map.
	 * @param	dbBlock - Pointer to aes_afp_datablock
	 * @return	true on "success" else "false"
	 */
	bool handle_getStatusNextFile (aes_afp_datablock* dbBlock);

private:
	/**
	 * @brief	The identification number of the caller.
	 */
	void* savedCallerId_;

	/**
	 * @brief	Identification of CDH task connected to a order.
	 */
	void* savedCdhId_;

	/**
	 * @brief	Number of messages received by datatask.
	 */
	ACE_INT32 messageCounter_;

	/**
	 * @brief	stop flag for file transfer
	 */
	bool stop;

	/**
	 * @brief	pointer to the AP COM server for the communication with clients (RTR, CPF, ALOG, STS, MML, afpls...)
	 */
	aes_afp_apcom_serv *apComObj_;


	/**
	 * @brief	name of my thread
	 */
	const std::string nameOfMyThread_;


	/**
	 * @brief	instance of aes_afp_database_fd
	 */
	aes_afp_database_fd dBase_;

	/**
	 * @brief Indicates that the TQ data base has been initialized
	 */
	bool m_initialized;

};
#endif
