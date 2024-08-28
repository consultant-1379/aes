/*=================================================================== */
/**
	@file   aes_afp_cdhtask.cpp

	@brief

	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       08/09/2011     XNADNAR       Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_afp_cdhtask.h>
#include <aes_afp_services.h>
#include <ACS_DSD_Client.h>
#include <aes_afp_database_fd.h>
#include <aes_afp_apcom_serv.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"

AES_AFP_TRACE_DEFINE(AES_AFP_cdhtask);
extern aes_afp_database_fd *afpDatabase;

/*===================================================================
					   ROUTINE: CONSTRUCTOR
=================================================================== */
aes_afp_cdhtask::aes_afp_cdhtask()
: sendRetries_(0),
  retryDelay_(0),
  currentSendRetryNo_(0),
  manualOrder_(false),
  fileQueueEmpty_(false),
  stopLoop_(false),
  fileInTransfer_(false),
  resendInProgress_(false),
  shutdownInProgress_(false),
  firstConnectToCdh_(true),
  applIsAttached_(false),
  remove_tq(false),
  nameOfAttachedAppl_(""),
  currentNewFilename_(""),
  currentFileToTransfer_(""),
  nameOfMyThread_(""),
  currentTransferQueue_(""),
  currentFilename_(""),
  nameOfDatatask_("datatask"),
  destinationSet_(""),
  isCDHStopped(false),
  currentNewSubfileName_(""),
  alarmTextApplication_(""),fileDestination_(NULL),currentOrderType_(FT_NONE),
  applicationChannel_(NULL),
  cdhTaskMsgQueue_(NULL),
  pTq_(NULL)
{
	retryTime_ = manageQueue_.gettimeofday(); //HY97331
}

/*===================================================================
					   ROUTINE: DESTRUCTOR
=================================================================== */
aes_afp_cdhtask::~aes_afp_cdhtask()
{
	AES_AFP_LOG(LOG_LEVEL_INFO,"Destroying aes_afp_destination :<%s>",destinationSet_.c_str());
	if( NULL != fileDestination_ )
	{
		delete fileDestination_;
		fileDestination_ = NULL;
	}
}
/*===================================================================
					   ROUTINE: open
=================================================================== */
int aes_afp_cdhtask::open (void* args)//, ACE_Thread_Manager* thrMgr, aes_afp_transferqueue * pTq)
{
	char newDest[100];
	int ret = AES_NOERRORCODE;
	AES_CDH_ResultCode cdhResult;
	int result(0);
	pTq_ = reinterpret_cast<aes_afp_transferqueue*>(args);
	strcpy(newDest,(pTq_->getDestinationSet()).c_str());
	//newDest = (char*)args;
	destinationSet_ = newDest;

	fileDestination_ = new aes_afp_destination(&manageQueue_, destinationSet_,pTq_);
	if((cdhResult = fileDestination_->open()) != AES_CDH_RC_OK)
	{	
		ret = AES_NOCDHSERVER;
		AES_AFP_LOG(LOG_LEVEL_ERROR,"fileDestination_->open() failed!!!");
		AES_AFP_TRACE_MESSAGE("fileDestination_->open() failed!!!");
		delete fileDestination_;
		fileDestination_ = 0;
		return ret;
	}

	AES_AFP_TRACE_MESSAGE("Open thread for destination set %s, TQ %s",
			destinationSet_.c_str(), nameOfAttachedAppl_.c_str());

	this->thr_mgr(pTq_->threadManager_);
	result = this->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED);
	if (result == -1)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Thread for destination set %s could not be opened",
				destinationSet_.c_str());
		AES_AFP_TRACE_MESSAGE("Thread for destination set %s could not be opened",
				destinationSet_.c_str());
		if( fileDestination_ != 0 )
		{
			delete fileDestination_;
			fileDestination_ = 0;
		}

		return AES_CATASTROPHIC;
	}

	AES_AFP_TRACE_MESSAGE("Thread opened OK for destination set %s",
			destinationSet_.c_str());

	return ret;
}


/*===================================================================
					   ROUTINE: svc
=================================================================== */
int aes_afp_cdhtask::svc (void )
{
	AES_AFP_TRACE_MESSAGE("Entering aes_afp_cdhtask::svc");

	cdhTaskMsgQueue_ = msg_queue();

	if (!destinationSet_.c_str())
	{
		AES_AFP_TRACE_MESSAGE("Starting execution of thread for destination set %s", destinationSet_.c_str());
	}

	for(;;)
	{
		handle_manageQueue();

		if (stopLoop_ == true)
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "Stopping message loop for destination set %s", destinationSet_.c_str());
			AES_AFP_TRACE_MESSAGE("Stopping message loop for destination set %s", destinationSet_.c_str());
			break;
		}
		if (resendInProgress_ == true || fileInTransfer_ == true)
		{
			AES_AFP_TRACE_MESSAGE("fileInTransfer_");
			continue;
		}

		handle_fileQueue();
	}

	if (currentTransferQueue_.empty() == false && remove_tq == true )
	{
		std::string tmpUser("");
		tmpUser = currentTransferQueue_;
		tmpUser += "_" + destinationSet_;
	}

	if(fileDestination_) // HW57148
		fileDestination_->close();

	AES_AFP_TRACE_MESSAGE("Thread execution ending for destination set %s", destinationSet_.c_str());
	return 0;
}
/*===================================================================
					   ROUTINE: close
=================================================================== */
int aes_afp_cdhtask::close (unsigned long flags)
{
	//(void)(flags);	
	AES_AFP_TRACE_MESSAGE( "flags : %ld",flags);
	return 0;
}
/*===================================================================
					   ROUTINE: changeStatusOfSendItem
=================================================================== */
void aes_afp_cdhtask::changeStatusOfSendItem (AES_GCC_Filestates newStatus)
{
	AES_AFP_TRACE_MESSAGE("Changing status to %d for file %s in TQ %s",
			newStatus, currentFilename_.c_str(), currentTransferQueue_.c_str());
	AES_GCC_Errorcodes error;
	if( pTq_->changeFileStatus(newStatus,currentFilename_,error) )
	{
		AES_AFP_TRACE_MESSAGE("OK");
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Changing status to %d for file %s in TQ %s has failed with error %d",newStatus, currentFilename_.c_str(), currentTransferQueue_.c_str(),error);
		AES_AFP_TRACE_MESSAGE("Changing status to %d for file %s in TQ %s has failed with error %d",newStatus, currentFilename_.c_str(), currentTransferQueue_.c_str(),error);
	}

}

/*===================================================================
					   ROUTINE: copyFromDatablock
=================================================================== */
void aes_afp_cdhtask::copyFromDatablock (aes_afp_datablock* dataBlock)
{
	currentFileToTransfer_ = dataBlock->getFullPath();
	currentTransferQueue_ = dataBlock->getTransferQueue();
	currentFilename_ = dataBlock->getFilename();
	sendRetries_ = dataBlock->getSendRetries();
	retryDelay_ = dataBlock->getRetriesDelay();
	manualOrder_ = dataBlock->getManual();
}
/*===================================================================
					   ROUTINE: handle_manageQueue
=================================================================== */
void aes_afp_cdhtask::handle_manageQueue ()
{
	int queueResult = 0;
	ACE_Message_Block* aceMessage = 0;
	aes_afp_msgblock* manageMessage = 0;
	aes_afp_datablock* manageDataBlock = 0;
	bool result = false;
	AES_GCC_Errorcodes errorCode;
	AES_CDH_ResultCode cdhResult;
	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeOutTime;

	if (fileInTransfer_ == true)
	{
		if (resendInProgress_ == true)
		{
			timeOutTime = manageQueue_.gettimeofday();  //HY97331
			int elpasedTime = (timeOutTime-retryTime_).sec();
			AES_AFP_TRACE_MESSAGE("retryTime_ = %ld, currentTime = %ld, elpasedTime = %d", retryTime_.sec(), timeOutTime.sec(), elpasedTime);
			int diffTime = retryDelay_-elpasedTime;
			timeOutTime += ACE_Time_Value((diffTime <= 0)?0:diffTime);
			queueResult = manageQueue_.dequeue_head(aceMessage, &timeOutTime);
			AES_AFP_TRACE_MESSAGE("queueResult = %d", queueResult);
		}
		else
		{
			queueResult = manageQueue_.dequeue_head(aceMessage);
			AES_AFP_TRACE_MESSAGE("File is in transfer, no resend in progress, received a message");
		}
	}
	else
	{
		timeOutTime = manageQueue_.gettimeofday();
		queueResult = manageQueue_.dequeue_head(aceMessage, &timeOutTime);
	}

	if (queueResult == -1)
	{
		if (fileInTransfer_ == true && resendInProgress_ == true)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "File is in transfer, resend in progress, retrying file sending");
			AES_AFP_TRACE_MESSAGE("File is in transfer, resend in progress, retrying file sending");
			fileDestination_->sendFileToCdh();
			resendInProgress_ = false;
		}
		return;
	}
	manageMessage = (aes_afp_msgblock*)aceMessage;
	switch (manageMessage->getMsgType() )
	{
	case CDHFINISHED:
	{
		fileInTransfer_ = false;
		resendInProgress_ = false;

		AES_AFP_TRACE_MESSAGE("Message CDHFINISHED received, shutdown is %s",(shutdownInProgress_?"TRUE":"FALSE"));
		if (shutdownInProgress_)
		{
			stopLoop_ = true;
		}
		else
		{
			if (manageMessage->getErrorCode() != AES_NOERRORCODE)
			{
				AES_AFP_TRACE_MESSAGE("Sending event link down");
				sendEventToAppl(AES_EVELINKDOWN, currentFileToTransfer_, manageMessage->getErrorCode());
			}
		}

		aceMessage->release();
		break;
	}
	case CDHRESEND:
	{
		AES_AFP_TRACE_MESSAGE("Message CDHRESEND received");
		aes_afp_datablock* cdhDb = 0;
		cdhDb = new(std::nothrow) aes_afp_datablock();
		if ( cdhDb !=0 )
		{
			fileDestination_->copyToDatablock( cdhDb );

			if( !(cdhDb->getFilename()).empty() )
			{

				aes_afp_datablock *pFileInfo = new(std::nothrow) aes_afp_datablock();
				AES_GCC_Errorcodes err;

				//Get the file state. 
				afpDatabase->getFileStatus(cdhDb->getTransferQueue(),
						cdhDb->getDestinationSet(),
						cdhDb->getFilename(),
						pFileInfo,
						err);
				if( pFileInfo != 0 )
				{
					if( pFileInfo->getStatus() == AES_FSSEND)
					{
						AES_AFP_TRACE_MESSAGE("Resending %s for transfer queue: %s, destination set: %s ", cdhDb->getFilename().c_str(), currentTransferQueue_.c_str(), cdhDb->getDestinationSet().c_str());
						aes_afp_msgblock* cdhMessage = 0;
						cdhMessage = new aes_afp_msgblock(sizeof(aes_afp_datablock), cdhDb);
						cdhMessage->setMsgType(CDHSENDFILE);
						cdhMessage->setCallerName(currentTransferQueue_ );
						cdhMessage->setCallerId(this);

						cdhTaskMsgQueue_->enqueue_head(cdhMessage);
					}
					else
					{
						delete cdhDb;
						cdhDb = 0;
					}
					delete pFileInfo;
					pFileInfo = 0;
				}
				else
				{
					delete cdhDb;
					cdhDb = 0;
				}
			}
			else
			{

				delete cdhDb;
				cdhDb = 0;
			}
		}
		aceMessage->release();
		break;
	}
	case CDHREOPEN:
	{
		AES_AFP_TRACE_MESSAGE("Message CDHREOPEN received");
		fileDestination_->close();
		if( aes_afp_services::afpStopEvt == true )
		{
			AES_AFP_LOG(LOG_LEVEL_DEBUG, "CDHREOPEN stopevent is true");
			AES_AFP_TRACE_MESSAGE("CDHREOPEN stopevent is true");
			aes_afp_msgblock* messageToCdhTask = 0;
			aes_afp_msgtypes message(HANGUP);
			AES_GCC_Errorcodes error(AES_NOCDHSERVER);

			messageToCdhTask = new aes_afp_msgblock(0, 0);
			messageToCdhTask->setMsgType(message);
			messageToCdhTask->setCallerId(this);
			messageToCdhTask->setErrorCode(error);

			manageQueue_.enqueue_head(messageToCdhTask);
			aceMessage->release();
			break;
		}
		cdhResult = fileDestination_->open();

		if (cdhResult != AES_CDH_RC_OK && cdhResult != AES_CDH_RC_INCUSAGE)
		{
			//AES_AFP_LOG(LOG_LEVEL_DEBUG, "(cdhResult != AES_CDH_RC_OK && cdhResult != AES_CDH_RC_INCUSAGE)");
			AES_AFP_TRACE_MESSAGE("(cdhResult != AES_CDH_RC_OK && cdhResult != AES_CDH_RC_INCUSAGE) - CDHREOPEN as AES_NOCDHSERVER");
			fileDestination_->sendMessageToCdhTask(CDHREOPEN, AES_NOCDHSERVER);
			sleep(1);
		}
		else
		{
			//AES_AFP_LOG(LOG_LEVEL_DEBUG, "sendMessageToCdhTask CDHREATTACH as AES_NOCDHSERVER");
			AES_AFP_TRACE_MESSAGE("sendMessageToCdhTask CDHREATTACH as AES_NOCDHSERVER");
			fileDestination_->sendMessageToCdhTask(CDHREATTACH, AES_NOCDHSERVER);
		}

		aceMessage->release();
		break;
	}
	case CDHREATTACH:
	{
		AES_AFP_TRACE_MESSAGE("Message CDHREATTACH received");
		errorCode = attachApplication(nameOfAttachedAppl_, alarmTextApplication_, applicationChannel_);
		if (errorCode != AES_NOERRORCODE && errorCode != AES_INCORRECTCOMMAND)
		{
			//AES_AFP_LOG(LOG_LEVEL_DEBUG, "sendMessageToCdhTask CDHREATTACH as AES_NOCDHSERVER");
			AES_AFP_TRACE_MESSAGE("sendMessageToCdhTask CDHREATTACH as AES_NOCDHSERVER");
			ACE_OS::sleep(1);
			fileDestination_->sendMessageToCdhTask(CDHREATTACH, AES_NOCDHSERVER);
		}
		else
		{
			//int msgQError = 0;
			AES_AFP_TRACE_MESSAGE("Send message CDHRESEND");
			if( fileDestination_->sendMessageToCdhTask(CDHRESEND, AES_NOERRORCODE ) == false )
			{
				changeStatusOfSendItem(AES_FSFAILED);
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occured while sending CDHRESEND");
				AES_AFP_TRACE_MESSAGE("Error occured while sending CDHRESEND");
			}
			isCDHStopped =  false;
		}

		aceMessage->release();
		break;
	}
	case CDHSTOPSENDFILE:
	{
		AES_AFP_LOG(LOG_LEVEL_DEBUG, "Message CDHSTOPSENDFILE received, file in transfer %s",(fileInTransfer_?"TRUE":"FALSE"));
		AES_AFP_TRACE_MESSAGE("Message CDHSTOPSENDFILE received, file in transfer %s",(fileInTransfer_?"TRUE":"FALSE"));
		manageDataBlock = manageMessage->getData();
		if (currentFileToTransfer_ == manageDataBlock->getFullPath() && fileInTransfer_ == true)
		{
			fileDestination_->stopFileTransfer();
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_DEBUG, "Inserting %s into stop file set",
					(manageDataBlock->getFullPath()).c_str() );
			AES_AFP_TRACE_MESSAGE("Inserting %s into stop file set",
					(manageDataBlock->getFullPath()).c_str() );
			stopFileSet_.insert(manageDataBlock->getFullPath() );
		}
		aceMessage->release();
		break;
	}
	case CDHSTOPSENDALL:
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "Message CDHSTOPSENDALL received");
		AES_AFP_TRACE_MESSAGE("Message CDHSTOPSENDALL received");
		aceMessage->release();
		if (fileInTransfer_ == true)
		{
			stopAllFileTransfer();
		}
		setQueuedFilesToStopped();
		break;
	}
	case HANGUP:
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "Message HANGUP received");
		AES_AFP_TRACE_MESSAGE("Message HANGUP received");
		shutdownInProgress_ = true;
		if (fileInTransfer_ == false)
		{
			AES_AFP_TRACE_MESSAGE("No file in transfer, stopping loop");
			cdhTaskMsgQueue_->close();
			stopLoop_ = true;
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("File in transfer, calling stopAllFileTransfer()");
			result = stopAllFileTransfer();
			if (result == false)
			{
				AES_AFP_TRACE_MESSAGE("stopAllFileTransfer() returned false, stopping svc loop");
				cdhTaskMsgQueue_->close();
				stopLoop_ = true;
			}
		}
		fileDestination_->setShutdownForDest();
		aceMessage->release();
		break;
	}
	case CDHSTOPPED:
	{
		isCDHStopped = true;

		AES_AFP_TRACE_MESSAGE("Message CDHSTOPPED received, shutdown is %s",(shutdownInProgress_?"TRUE":"FALSE"));

		if (shutdownInProgress_ == true)
		{
			cdhTaskMsgQueue_->close();
		}
		aceMessage->release();
		break;
	}
	case CDHSENDRETRY:
	{
		AES_AFP_TRACE_MESSAGE("Message CDHSENDRETRY received");
		resendInProgress_ = true;
		retryTime_ = manageQueue_.gettimeofday(); //HY97331
		aceMessage->release();
		break;
	}
	case CDHCONNECTED:
	{
//		AES_AFP_LOG(LOG_LEVEL_INFO, "Message CDHCONNECTED received, first connect %s",
//				firstConnectToCdh_?"true":"false");
		AES_AFP_TRACE_MESSAGE("Message CDHCONNECTED received, first connect %s",
				firstConnectToCdh_?"true":"false");
		if (firstConnectToCdh_ == false)
		{
			sendEventToAppl(AES_EVELINKDOWNCEASE, currentFileToTransfer_, manageMessage->getErrorCode() );
		}
		aceMessage->release();
		break;
	}
	case CDHCONNECTERROR:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Message CDHCONNECTERR received");
		AES_AFP_TRACE_MESSAGE("Message CDHCONNECTERR received");
		firstConnectToCdh_ = false;
		sendEventToAppl(AES_EVELINKDOWN, currentFileToTransfer_, manageMessage->getErrorCode());
		aceMessage->release();
		break;
	}
	case CDHEVENTHANDLERSET:
	{
//		AES_AFP_LOG(LOG_LEVEL_DEBUG, "Message CDHEVENTHANDLERSET received");
		AES_AFP_TRACE_MESSAGE("Message CDHEVENTHANDLERSET received");
		aceMessage->release();
		break;
	}
	case CDHEVENTHANDLERSETERR:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Message CDHEVENTHANDLERSETERR received");
		AES_AFP_TRACE_MESSAGE("Message CDHEVENTHANDLERSETERR received");
		aceMessage->release();
		break;
	}
	case CDHEVENTHANDLERREM:
	{
//		AES_AFP_LOG(LOG_LEVEL_DEBUG, "Message CDHEVENTHANDLERREM received");
		AES_AFP_TRACE_MESSAGE("Message CDHEVENTHANDLERREM received");
		aceMessage->release();
		break;
	}
	case CDHEVENTHANDLERREMERR:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Message CDHEVENTHANDLERREMERR received");
		AES_AFP_TRACE_MESSAGE("Message CDHEVENTHANDLERREMERR received");
		aceMessage->release();
		break;
	}
	case UNKNOWN:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Message CDHUNKNOWN received");
		AES_AFP_TRACE_MESSAGE("Message CDHEVENTHANDLERREMERR received");
		aceMessage->release();
		break;
	}
	default:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Default reached, unknown message received");
		AES_AFP_TRACE_MESSAGE("Default reached, unknown message received");
		aceMessage->release();
		break;
	}
	}
}
/*===================================================================
					   ROUTINE: handle_fileQueue
=================================================================== */
void aes_afp_cdhtask::handle_fileQueue ()
{
	ACE_Message_Block* aceMessage = 0;
	aes_afp_msgblock* afpMessage = 0;
	aes_afp_datablock* db = 0;
	int queueResult = 0;
	std::set<std::string>::iterator itr;

	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeOutTime = msg_queue()->gettimeofday();
	timeOutTime += ACE_Time_Value(0, 500*1000);

	queueResult = this->getq(aceMessage, &timeOutTime);
	if (queueResult == -1)
	{
		//AES_AFP_LOG((aes_afp_Cdhtask, "handle_fileQueue()\n Get queue returned -1\n"));
		return;
	}
	afpMessage = (aes_afp_msgblock*)aceMessage;

	//bool toDelete = false;
	switch (afpMessage->getMsgType())
	{
	case CDHSENDFILE:
	{
		AES_AFP_TRACE_MESSAGE("Message CDHSENDFILE received");
		db = afpMessage->getData();
		copyFromDatablock(db);
		if (stopFileSet_.empty() )
		{
			fileDestination_->copyFromDatablock(db);
			changeStatusOfSendItem(AES_FSSEND);
			AES_CDH_ResultCode cdhRes = fileDestination_->sendFileToCdh();
			AES_AFP_TRACE_MESSAGE("cdhRes = %d",cdhRes);
			fileInTransfer_ = true;
		}
		else
		{
			itr = stopFileSet_.find(currentFileToTransfer_);
			if (itr != stopFileSet_.end() )
			{
				AES_AFP_LOG(LOG_LEVEL_WARN,"File %s found in stopFileSet_, changing status to stopped", currentFileToTransfer_.c_str());
				AES_AFP_TRACE_MESSAGE("File %s found in stopFileSet_, changing status to stopped", currentFileToTransfer_.c_str());
				stopFileSet_.erase(itr);
			}
			else
			{
				AES_AFP_TRACE_MESSAGE("Sending file %s to CDH",currentFileToTransfer_.c_str());
				fileDestination_->copyFromDatablock(db);
				AES_CDH_ResultCode cdhRes = fileDestination_->sendFileToCdh();
				AES_AFP_TRACE_MESSAGE("cdhRes = %d",cdhRes);
				changeStatusOfSendItem(AES_FSSEND);
				fileInTransfer_ = true;
			}
		}
		aceMessage->release();
		break;
	}
	default:
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "Default reached");
		AES_AFP_TRACE_MESSAGE("Default reached in handle_fileQueue");
		if( aceMessage != 0 )
		{
			aceMessage->release();
		}
		break;
	}
	}
	AES_AFP_TRACE_MESSAGE("handle_fileQueue() filename = <%s>" , currentFilename_.c_str());
}
/*===================================================================
					   ROUTINE: askForManageQueue
=================================================================== */
ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* aes_afp_cdhtask::askForManageQueue ()
{
	return &manageQueue_;
}
/*===================================================================
					   ROUTINE: stopAllFileTransfer
=================================================================== */
bool aes_afp_cdhtask::stopAllFileTransfer ()
{
	AES_AFP_TRACE_MESSAGE( "Entering");
	int result(0);
	result = fileDestination_->stopFileTransfer();
	if (result < 0)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to stop file %s",currentFileToTransfer_.c_str());
		AES_AFP_TRACE_MESSAGE("Unable to stop file %s",currentFileToTransfer_.c_str());
		return false;
	}

	AES_AFP_TRACE_MESSAGE("All files stopped");
	return true;
}
/*===================================================================
					   ROUTINE: setQueuedFilesToStopped
=================================================================== */
void aes_afp_cdhtask::setQueuedFilesToStopped ()
{
	AES_AFP_LOG(LOG_LEVEL_INFO, "Setting %d queued files to FS_STOPPED", (int)cdhTaskMsgQueue_->message_count());
	AES_AFP_TRACE_MESSAGE("Setting %d queued files to FS_STOPPED", (int)cdhTaskMsgQueue_->message_count());
	ACE_Message_Block* aceMessage = 0;
	aes_afp_msgblock* afpMessage = 0;
	aes_afp_datablock* db;
	int queueResult(0);
	//	void* savedCallerId;

	while (cdhTaskMsgQueue_->message_count() > 0 )
	{
		queueResult = this->getq(aceMessage);
		if (queueResult == -1)
		{
			AES_AFP_TRACE_MESSAGE("No files in queue");
		}

		afpMessage = (aes_afp_msgblock*)aceMessage;
		//		savedCallerId = afpMessage->getCallerId();

		switch (afpMessage->getMsgType())
		{
		case CDHSENDFILE:
		{
			AES_AFP_TRACE_MESSAGE("Message CDHSENDFILE received");
			db = afpMessage->getData();
			copyFromDatablock(db);
			aceMessage->release();
			break;
		}
		default:
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "Default reached");
			AES_AFP_TRACE_MESSAGE("Default reached");
			if( aceMessage !=  0)
			{
				aceMessage->release();
			}
			break;
		}
		}
	}
}

/*===================================================================
					   ROUTINE: sendEventToAppl
=================================================================== */
void aes_afp_cdhtask::sendEventToAppl (AES_GCC_Eventcodes eventCode, std::string dataString, AES_GCC_Errorcodes errorCode)
{
	AES_AFP_TRACE_MESSAGE("Event code %d", eventCode);

	char sendBuff[2048]= { 0 };
	int msgLen(0);

	ACE_OS::sprintf(sendBuff, "%s%d\n%d\n%d\n%d\n%d\n%s\n", "rpch", 1, EVENT, eventCode, errorCode, 1, dataString.c_str() );
	msgLen = ACE_OS::strlen(sendBuff);

	aes_afp_apcom_serv::afpMutex.acquire();
	applicationChannel_ = aes_afp_dsdsessionmap::Instance().searchSession(nameOfAttachedAppl_);
	if (applicationChannel_ != 0)
	{
		applicationChannel_->send((void*)sendBuff, msgLen);
	}
	else
	{
		detachApplication();
	}
	aes_afp_apcom_serv::afpMutex.release();
}

/*===================================================================
					   ROUTINE: attachApplication
=================================================================== */
AES_GCC_Errorcodes aes_afp_cdhtask::attachApplication (std::string connectedFileDest, std::string applAlarmText, ACS_DSD_Session* applDsdSession)
{
	AES_CDH_ResultCode cdhCode;

	nameOfAttachedAppl_ = connectedFileDest;
	applicationChannel_ = applDsdSession;
	alarmTextApplication_ = applAlarmText;

	AES_AFP_TRACE_MESSAGE("TQ %s Destination set %s Event text = %s",
			nameOfAttachedAppl_.c_str(), destinationSet_.c_str(), alarmTextApplication_.c_str() );

	cdhCode = fileDestination_->setEventSubscription(connectedFileDest, applAlarmText);

	switch (cdhCode)
	{
	case AES_CDH_RC_OK:
	{
		AES_AFP_TRACE_MESSAGE("Application attach OK");
		applIsAttached_ = true;
		return AES_NOERRORCODE;
	}
	case AES_CDH_RC_NODEST:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to attach, no destination set %s",destinationSet_.c_str());
		AES_AFP_TRACE_MESSAGE("Not able to attach, no destination set %s",destinationSet_.c_str());
		return AES_NODESTINATION;
	}
	case AES_CDH_RC_NOSERVER:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to attach, no CDH server");
		AES_AFP_TRACE_MESSAGE("Not able to attach, no CDH server");
		return AES_NOCDHSERVER;
	}
	case AES_CDH_RC_INCUSAGE:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to attach, CDH incorrect usage");
		AES_AFP_TRACE_MESSAGE("Not able to attach, CDH incorrect usage");
		applIsAttached_ = true;
		return AES_INCORRECTCOMMAND;
	}
	default: // Other errors
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to attach, default reached. CDH Error code: %d", cdhCode);
		AES_AFP_TRACE_MESSAGE("Not able to attach, default reached. CDH Error code: %d", cdhCode);
		return AES_ERRORUNKNOWN;
	}
	}
}

/*===================================================================
					   ROUTINE: detachApplication
=================================================================== */
void aes_afp_cdhtask::detachApplication ()
{
	AES_AFP_TRACE_MESSAGE( "Application detaching");

	AES_CDH_ResultCode cdhCode;
	cdhCode = fileDestination_->removeEventSubscription();
	applIsAttached_ = false;
}

void aes_afp_cdhtask::set_remove_tq(bool value)
{
	remove_tq = value;
}

