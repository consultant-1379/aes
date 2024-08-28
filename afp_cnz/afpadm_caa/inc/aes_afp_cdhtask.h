/*=================================================================== */
/**
   @file   	aes_afp_cdhtask.h

   @brief 	Header file for aes_afp_cdhtask class
			which is responsible for the communication with CDH

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
#ifndef aes_afp_cdhtask_h
#define aes_afp_cdhtask_h 1

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <set>
#include <string>

#include <ace/ACE.h>
#include <ace/OS.h>
#include <ace/Time_Value.h>
#include <ace/Task.h>
#include <ace/Message_Queue.h>
#include <ace/Monotonic_Time_Policy.h>

#include "aes_afp_destination.h"
#include "aes_afp_events.h"
#include "aes_afp_msgblock.h"
#include "aes_afp_threadmap.h"
#include "aes_afp_criticalsection.h"
#include "aes_afp_dsdsessionmap.h"
#include "aes_afp_proctype.h"

#include <ACS_DSD_Session.h>
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_eventcodes.h>
#include <aes_gcc_event.h>
#include <aes_gcc_eventhandler.h>
#include <aes_gcc_log.h>

/*====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/*==================================================================== */
/** @brief 			MessageQueue_Cdh
					ACE_Message_Queue which is responsible for the
					message queue towards CDH
 */
/*==================================================================== */
typedef ACE_Message_Queue< ACE_MT_SYNCH , ACE_Monotonic_Time_Policy > MessageQueue_Cdh;
/*==================================================================== */
/** @brief 			MessageQueue_Cdh
					std::set responsible for the map of files that has
					been called to stop transferring.
 */
/*==================================================================== */
typedef std::set< std::string  > StopFileSet;

/*==================================================================== */
/** @brief 			AceTask_Cdh
					ACE_Task which is responsible for the thread towards CDH.
 */
/*==================================================================== */
typedef ACE_Task< ACE_MT_SYNCH ,ACE_Monotonic_Time_Policy > AceTask_Cdh;
/*====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class aes_afp_cdhtask : public AceTask_Cdh
{
	/*====================================================================
						PUBLIC DECLARATION SECTION
	==================================================================== */

public:
	/*=====================================================================
					CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**
	  @brief           Default constructor
						<Need to be update>
	  @return          void

	  @exception       none
	 */
	/*=================================================================== */
	aes_afp_cdhtask();
	/*=====================================================================
					CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

	  @brief           Destructor

	  @exception       none
	 */
	/*=================================================================== */
	virtual ~aes_afp_cdhtask();
	/*=================================================================== */
	/**

	@brief           	open
						Opens a task.

	@param          	args

	@param  		   	thrMgr
						Thread manager instance
	@return          	ACE_INT32

	@exception       	none
	 */
	/*=================================================================== */
	int open (void* args = 0);

	/*=================================================================== */
	/**

	@brief           	svc
						The tasks working method.

	@param          	args

	@param  		   	thrMgr
						Thread manager instance
	@return          	ACE_INT32

	@exception       	none
	 */
	/*=================================================================== */
	int svc (void );
	/*=================================================================== */
	/**

	@brief           	close
						This method is called from svc upon close of the task.

	@param          	flags

	@return          	ACE_INT32

	@exception       	none
	 */
	/*=================================================================== */
	int close (unsigned long flags = 0);
	/*=================================================================== */
	/**

	@brief           	changeStatusOfSendItem
						Change status of send item

	@param  		   	newStatus
						new status of the send item
	@return          	void

	@exception       	none
	 */
	/*=================================================================== */
	void changeStatusOfSendItem (AES_GCC_Filestates newStatus);

	/*=================================================================== */
	/**

	@brief           	copyFromDatablock
						Copy some data from the datablock.

	@param  		   	dataBlock
						Pointer to the aes_afp_datablock object
	@return          	void

	@exception       	none
	 */
	/*=================================================================== */
	void copyFromDatablock (aes_afp_datablock* dataBlock);

	/*=================================================================== */
	/**

	@brief           	askForManageQueue
						Retrieve the manage queue in cdh task.

	@return          	Pointer of ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy> type.

	@exception       	none
	 */
	/*=================================================================== */
	ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* askForManageQueue ();

	/*=================================================================== */
	/**

	@brief           	attachApplication
						Attaches an application to the even handling mechanism.

	@param  		   	connectedFileDest
						The current connected file destination name.
	@param  		   	applAlarmText
						Application's alarm text
	@param  		   	applDsdSession
						DSD session object for the application
	@return          	AES_GCC_Errorcodes
	@exception       	none
	 */
	/*=================================================================== */
	AES_GCC_Errorcodes attachApplication (std::string connectedFileDest = "", std::string applAlarmText = "", ACS_DSD_Session* applDsdSession = 0);

	/*=================================================================== */
	/**

	@brief           	detachApplication
						Detach an application from cdh.

	@return          	void
	@exception       	none
	 */
	/*=================================================================== */

	void detachApplication ();
	/*=================================================================== */
	/**

	@brief           	set_remove_tq
						To set or unset remove_tq flag
	@param				value
						"true" to set remove_tq else "false"

	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void set_remove_tq(bool value);
	/*====================================================================
						PRIVATE DECLARATION SECTION
==================================================================== */
private:
	/*=================================================================== */
	/**

	@brief           	handle_manageQueue
						Handles the connection manageQueue
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void handle_manageQueue ();
	/*=================================================================== */
	/**
	@brief           	handle_fileQueue
						Handle the connection queue for files.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void handle_fileQueue ();
	/*=================================================================== */
	/**
	@brief           	stopAllFileTransfer
						Stops all transfer of files.
	@return          	success or failure
	@exception       	none
	 */
	/*=================================================================== */
	bool stopAllFileTransfer ();
	/*=================================================================== */
	/**
	@brief           	setQueuedFilesToStopped
						Sets all files in queue to status stopped.
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void setQueuedFilesToStopped ();
	/*=================================================================== */
	/**
	@brief           	sendEventToAppl
						Forwards an event to an attached application.
	@param				eventCode
						Event code
	@param				dataString
						problem data
	@param				errorCode
						Error code
	@return          	void
	@exception       	none
	 */
	/*=================================================================== */
	void sendEventToAppl (AES_GCC_Eventcodes eventCode, std::string dataString, AES_GCC_Errorcodes errorCode);

private:
	/*=================================================================== */
	/**
	@brief           	sendRetries_
						Number of send retries for this send item.
	 */
	/*=================================================================== */
	int sendRetries_;

	/*=================================================================== */
	/**
	@brief           	sendRetries_
						Delay time between send retries.
	 */
	/*=================================================================== */
	int retryDelay_;
	/*=================================================================== */
	/**
	@brief           	currentSendRetryNo_
						The number of the current send retries.
	 */
	/*=================================================================== */
	int currentSendRetryNo_;
	/*=================================================================== */
	/**
	@brief           	manualOrder_
						True if the order is manual initiated.
	 */
	/*=================================================================== */
	bool manualOrder_;
	/*=================================================================== */
	/**
	@brief           	fileQueueEmpty_
						True if there is no files waiting to be transfered.
	 */
	/*=================================================================== */
	bool fileQueueEmpty_;
	/*=================================================================== */
	/**
	@brief           	stopLoop_
						True if the send file loop is to be ended.
	 */
	/*=================================================================== */
	bool stopLoop_;
	/*=================================================================== */
	/**
	@brief           	fileInTransfer_
						True if a file is in transfer.
	 */
	/*=================================================================== */
	bool fileInTransfer_;
	/*=================================================================== */
	/**
	@brief           	resendInProgress_
						True if resend of a file is on going.
	 */
	/*=================================================================== */
	bool resendInProgress_;
	/*=================================================================== */
	/**
	@brief           	shutdownInProgress_
						True if this task is to be ended.
	 */
	/*=================================================================== */
	bool shutdownInProgress_;
	/*=================================================================== */
	/**
	@brief           	firstConnectToCdh_
						True if this is the first connection with CDH.
	 */
	/*=================================================================== */
	bool firstConnectToCdh_;
	/*=================================================================== */
	/**
	@brief           	applIsAttached_
						True if an application is attached.
	 */
	/*=================================================================== */
	bool applIsAttached_;
	/*=================================================================== */
	/**
	@brief           	remove_tq
						Flag specify whether a TQ can be deleted or not.
	 */
	/*=================================================================== */
	bool remove_tq;
	/*=================================================================== */
	/**
	@brief           	nameOfAttachedAppl_
						Name of attached application.
	 */
	/*=================================================================== */
	std::string nameOfAttachedAppl_;
	/*=================================================================== */
	/**
	@brief           	currentNewFilename_
						Name of new filename.
	 */
	/*=================================================================== */
	std::string currentNewFilename_;
	/*=================================================================== */
	/**
	@brief           	currentFileToTransfer_
						Current file to transfer.
	 */
	/*=================================================================== */
	std::string currentFileToTransfer_;
	/*=================================================================== */
	/**
	@brief           	nameOfMyThread_
						Name of this thread.
	 */
	/*=================================================================== */
	std::string nameOfMyThread_;
	/*=================================================================== */
	/**
	@brief           	currentTransferQueue_
						Current transfer queue.
	 */
	/*=================================================================== */
	std::string currentTransferQueue_;
	/*=================================================================== */
	/**
	@brief           	currentFilename_
						Current file to transfer.
	 */
	/*=================================================================== */
	std::string currentFilename_;
	/*=================================================================== */
	/**
	@brief           	nameOfDatatask_
						Name of datatask thread.
	 */
	/*=================================================================== */
	const std::string nameOfDatatask_;
	/*=================================================================== */
	/**
	@brief           	destinationSet_
						Name of destination set.
	 */
	/*=================================================================== */
	std::string destinationSet_;
	/*=================================================================== */
	/**
	@brief           	isCDHStopped
						Flag for CDHSTOPPED
	 */
	/*=================================================================== */
	bool isCDHStopped;
	/*=================================================================== */
	/**
	@brief           	currentNewSubfileName_
						New sub filename.
	 */
	/*=================================================================== */
	std::string currentNewSubfileName_;
	/*=================================================================== */
	/**
	@brief           	alarmTextApplication_
						Applications provided alarm text.
	 */
	/*=================================================================== */
	std::string alarmTextApplication_;

	/*=================================================================== */
	/**
	@brief           	fileDestination_
						Pointer to the file destination object
	 */
	/*=================================================================== */
	aes_afp_destination *fileDestination_;
	/*=================================================================== */
	/**
	@brief           	currentOrderType_
						Current order type
	 */
	/*=================================================================== */
	AES_AFP_Proctype currentOrderType_;
	/*=================================================================== */
	/**
	@brief           	applicationChannel_
						DSD session object for the application
	 */
	/*=================================================================== */
	ACS_DSD_Session *applicationChannel_;
	/*=================================================================== */
	/**
	@brief           	manageQueue_
						ACE_Message_Queue responsible for the managing queue
	 */
	/*=================================================================== */
	MessageQueue_Cdh manageQueue_;
	/*=================================================================== */
	/**
	@brief           	cdhTaskMsgQueue_
						ACE_Message_Queue responsible for message
	 */
	/*=================================================================== */
	MessageQueue_Cdh *cdhTaskMsgQueue_;
	/*=================================================================== */
	/**
	@brief           	stopFileSet_
						Responsible for the map of files that has
						been called to stop transferring.
	 */
	/*=================================================================== */
	StopFileSet stopFileSet_;
	aes_afp_transferqueue *pTq_;
	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> retryTime_; //HY97331
};
#endif
