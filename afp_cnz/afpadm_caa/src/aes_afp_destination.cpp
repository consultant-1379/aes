//	INCLUDE aes_afp_destination.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	 190 89-CAA 109 0503
//
//	AUTHOR
//	 2003-01-20 UAB/KB/AU DAPA
//
//	REVISION
//	 A 2003-01-01 DAPA
//	 B 2003-05-12 DAPA
//
//	LINKAGE
//
//	SEE ALSO

#include <aes_afp_destination.h>
#include <aes_afp_database_fd.h>
#include <stdlib.h>
#include <ACS_APGCC_Util.H>
#include <aes_afp_services.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(AES_AFP_destination);

aes_afp_database_fd *afpDatabase = NULL;


//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_destination::aes_afp_destination (ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* manageQueue, std::string destinationSet, aes_afp_transferqueue * pTq)
: AES_CDH_DestinationSet(destinationSet),
  numberOfSendRetries_(0),
  timeBetweenRetries_(0),
  sendRetryNo_(0),
  retryDelay_(0),
  manualOrder_(false),
  shutdownInProgress_(false),
  isADirectory_(false),
  restarted_(false),
  nameOfDatatask_("datatask"),
  currentNewFilename_(""),
  currentDestinationSet_(""),
  currentTransferQueue_(""),
  fileMask_(""),
  renameTemplate_(""),
  currentFilename_(""),
  fileToTransfer_(""),
  currentNewSubfile_(""),
  currentOrderType_(FT_NONE),
  renameFile_(AES_RTUNKNOWN),
  format_(AES_DEFAULT),
  messageQueue_(manageQueue),
  pTq_(pTq)
{
}


aes_afp_destination::~aes_afp_destination()
{
}



//	---------------------------------------------------------
//	       eventSendFile()
//	---------------------------------------------------------
void aes_afp_destination::eventSendFile (AES_CDH_ResultCode ret,
		const std::string &destinationSet,
		const std::string &filename,
		const std::string &transferQueue,
		const std::string &newSubFileName)
{
	(void)(newSubFileName);
	AES_AFP_TRACE_MESSAGE("Event code = %d Destination set = %s transfer queue = %s new filename = %s New subfile name = %s", ret, destinationSet.c_str(), filename.c_str(), transferQueue.c_str(), newSubFileName.c_str());

	switch (ret)
	{
	case AES_CDH_RC_OK:
	{
		AES_AFP_TRACE_MESSAGE("CDH event OK");
		changeStatusOfSendItem(AES_FSDELETE);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		afpDatabase->sendApplEvent(currentTransferQueue_, AES_EVEDESTSETDOWNCEASE, AES_NOERRORCODE);
		break;
	}
	case AES_CDH_RC_ERROR:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "CDH event ERROR");
		AES_AFP_TRACE_MESSAGE("CDH event ERROR");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_INCUSAGE:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "CDH event INCUSAGE");
		AES_AFP_TRACE_MESSAGE("CDH event INCUSAGE");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_UNKNOWNRC:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "CDH event UNKNOWNRC");
		AES_AFP_TRACE_MESSAGE("CDH event UNKNOWNRC");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_DESTDEF:
	{
		AES_AFP_TRACE_MESSAGE("CDH event DESTDEF");
		break;
	}
	case AES_CDH_RC_UNREAS:
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "CDH event UNREAS");
		AES_AFP_TRACE_MESSAGE("CDH event UNREAS");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_PHYSFILEERR:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "CDH event PHYSFILEERR");
		AES_AFP_TRACE_MESSAGE("CDH event PHYSFILEERR");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_NODEST:
	{
		AES_AFP_LOG(LOG_LEVEL_DEBUG, "CDH event NODEST");
		AES_AFP_TRACE_MESSAGE("CDH event NODEST");
		if (numberOfSendRetries_ == -1)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
		}
		else if (sendRetryNo_ < numberOfSendRetries_)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
			sendRetryNo_++;
		}
		else
		{
			changeStatusOfSendItem(AES_FSFAILED);
			sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		}
		break;
	}
	case AES_CDH_RC_PROTECTEDDEST:
	{
		AES_AFP_LOG(LOG_LEVEL_DEBUG, "CDH event PROTECTEDDEST");
		AES_AFP_TRACE_MESSAGE("CDH event PROTECTEDDEST");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		break;
	}
	case AES_CDH_RC_INTPROGERR:
	{
		AES_AFP_LOG(LOG_LEVEL_DEBUG, "CDH event INTPROGERR");
		AES_AFP_TRACE_MESSAGE("CDH event INTPROGERR");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_NOTRANS:
	{
		AES_AFP_LOG(LOG_LEVEL_DEBUG, "CDH event NOTRANS");
		AES_AFP_TRACE_MESSAGE("CDH event NOTRANS");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_NOSERVER:
	{
		AES_AFP_TRACE_MESSAGE("CDH event NOSERVER");
		changeStatusOfSendItem(AES_FSFAILED);
		AES_AFP_TRACE_MESSAGE("after CDH event NOSERVER");
		sendMessageToCdhTask(CDHFINISHED, AES_NOCDHSERVER);
		AES_AFP_TRACE_MESSAGE("after send CDHFINISHED");
		sendMessageToCdhTask(CDHREOPEN, AES_NOCDHSERVER);
		AES_AFP_TRACE_MESSAGE("after send CDHREOPEN");

		break;
	}
	case AES_CDH_RC_FILEERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event FILEERR");
		if (numberOfSendRetries_ == -1)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
		}
		else if (sendRetryNo_ < numberOfSendRetries_)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
			sendRetryNo_++;
		}
		else
		{
			changeStatusOfSendItem(AES_FSFAILED);
			sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		}
		break;
	}

	case AES_CDH_RC_FILENAMETOOLONG:
	{
		AES_AFP_TRACE_MESSAGE("CDH event FILENAMETOOLONG");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}

	case AES_CDH_RC_CONNECTERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event CONNECTERR");
		if (numberOfSendRetries_ == -1)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
		}
		else if (sendRetryNo_ < numberOfSendRetries_)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
			sendRetryNo_++;
		}
		else
		{
			changeStatusOfSendItem(AES_FSFAILED);
			sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
			afpDatabase->sendApplEvent(currentTransferQueue_, AES_EVEDESTSETDOWN, AES_NOSERVERACCESS);
		}
		break;
	}
	case AES_CDH_RC_SENDERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event SENDERR");
		if (numberOfSendRetries_ == -1)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
		}
		else if (sendRetryNo_ < numberOfSendRetries_)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
			sendRetryNo_++;
		}
		else
		{
			changeStatusOfSendItem(AES_FSFAILED);
			sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		}
		break;
	}
	case AES_CDH_RC_ENDOFSTREAM:
	{
		AES_AFP_TRACE_MESSAGE("CDH event ENDOFSTREAM");
		break;
	}
	case AES_CDH_RC_CONNECTWARN:
	{
		AES_AFP_TRACE_MESSAGE("CDH event CONNECTWARN");
		if (numberOfSendRetries_ == -1)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
		}
		else if (sendRetryNo_ < numberOfSendRetries_)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
			sendRetryNo_++;
		}
		else
		{
			changeStatusOfSendItem(AES_FSFAILED);
			sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		}
		break;
	}
	case AES_CDH_RC_SENDWARN:
	{
		AES_AFP_TRACE_MESSAGE("CDH event SENDWARN");
		if (numberOfSendRetries_ == -1)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
		}
		else if (sendRetryNo_ < numberOfSendRetries_)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
			sendRetryNo_++;
		}
		else
		{
			changeStatusOfSendItem(AES_FSFAILED);
			sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		}
		break;
	}
	case AES_CDH_RC_DATAAREAERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event DATAAREAERR");
		if (numberOfSendRetries_ == -1)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
		}
		else if (sendRetryNo_ < numberOfSendRetries_)
		{
			sendMessageToCdhTask(CDHSENDRETRY, AES_NOERRORCODE);
			sendRetryNo_++;
		}
		else
		{
			changeStatusOfSendItem(AES_FSFAILED);
			sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		}
		break;
	}

	case AES_CDH_RC_TASKSTOPPED:
	{
		AES_AFP_TRACE_MESSAGE("CDH event TASKSTOPPED");
		sendMessageToCdhTask(CDHSTOPPED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_TASKEXISTS:
	{
		AES_AFP_TRACE_MESSAGE("CDH event TASKEXIST");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_NOTASK:
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "CDH event NOTASK");
		AES_AFP_TRACE_MESSAGE("CDH event NOTASK");
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_FILEALREADYSENT:
	{
		AES_AFP_TRACE_MESSAGE("CDH event FILEALREADYSENT");
		changeStatusOfSendItem(AES_FSDELETE);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	case AES_CDH_RC_EVENTHANDLERSET:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERSET");
		break;
	}
	case AES_CDH_RC_EVENTHANDLERSETERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERSETERR");
		break;
	}
	case AES_CDH_RC_EVENTHANDLERREM:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERREM");
		break;
	}
	case AES_CDH_RC_EVENTHANDLERREMERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERREMERR");
		break;
	}
	case AES_CDH_RC_CONNECTOK:
	{
		AES_AFP_TRACE_MESSAGE("CDH event CONNECTOK");
		break;
	}
	default:
	{
		AES_AFP_TRACE_MESSAGE("CDH event default reached");
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);

		break;
	}
	}

	if (ret != AES_CDH_RC_OK)
	{
		std::string problemText;
		std::string problemData;
		std::string newdr;
		if (AES_GCC_Util::datapath_trn(filename,AES_DATA_PATH,newdr)==false)
		{
			AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
			newdr = "Not Available";
		}
		problemText =  AES_CDH_Result(ret).errorText();
		problemData = "Event send file error FILE: ";
		problemData += newdr;
		problemData += " TRANSFERQUEUE:";
		problemData += transferQueue;
		problemData += "_";
		problemData += destinationSet;


		Event::report(EVENT_TRANSERROR, "Transfer error", problemData, problemText);
	}

	//## end aes_afp_destination::eventSendFile%380196F1027B.body
}

//## Operation: sendFileToCdh%3801EF7C0333; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendFileToCdh()
//	---------------------------------------------------------
AES_CDH_ResultCode aes_afp_destination::sendFileToCdh ()
{
	//## begin aes_afp_destination::sendFileToCdh%3801EF7C0333.body preserve=yes

	AES_CDH_ResultCode cdhResult;
	std::string tmpprocDest(currentTransferQueue_);
	tmpprocDest += "_" + currentDestinationSet_;
	AES_CDH_DestinationSet::transferMode mode = TR_BINARY;

	if (format_ == AES_BINARY)
	{
		mode = TR_BINARY;
	}
	if (format_ == AES_ASCII)
	{
		mode = TR_ASCII;
	}

	AES_AFP_TRACE_MESSAGE("File %s transfer queue %s",
			fileToTransfer_.c_str(), currentTransferQueue_.c_str());


	AES_AFP_TRACE_MESSAGE("New filename %s User unique %s Transfer mode %d File mask %s Directory = %s AFP restart = %s",currentNewFilename_.c_str(), tmpprocDest.c_str(), mode, fileMask_.c_str(), isADirectory_?"true":"false", restarted_?"true":"false");
	cdhResult = sendFile(fileToTransfer_,
			currentTransferQueue_,
			currentNewFilename_,
			tmpprocDest,
			mode,
			fileMask_,
			isADirectory_,
			restarted_);

	if (cdhResult != AES_CDH_RC_OK)
	{
		std::string problemText;
		std::string problemData;

		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to send file, CDH result code = %d", cdhResult);
		AES_AFP_TRACE_MESSAGE("Unable to send file, CDH result code = %d", cdhResult);
		changeStatusOfSendItem(AES_FSFAILED);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		std::string newdr;
		if (AES_GCC_Util::datapath_trn(fileToTransfer_,AES_DATA_PATH,newdr)==false)
		{
			AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
			newdr = "Not Available";
		}
		problemText =  AES_CDH_Result(cdhResult).errorText();
		problemData = "Send file error FILE: ";
		problemData += newdr;
		problemData += " TRANSFERQUEUE:";
		problemData += tmpprocDest;
		Event::report(EVENT_TRANSERROR, "Transfer error", problemData, problemText);
	}
	AES_AFP_TRACE_MESSAGE("file name = <%s> ",currentNewFilename_.c_str());
	return cdhResult;

	//## end aes_afp_destination::sendFileToCdh%3801EF7C0333.body
}

//## Operation: changeStatusOfSendItem%3805CE0F02E1; C++
//## Semantics:
//	---------------------------------------------------------
//	       changeStatusOfSendItem()
//	---------------------------------------------------------
void aes_afp_destination::changeStatusOfSendItem (AES_GCC_Filestates newStatus)
{
	//## begin aes_afp_destination::changeStatusOfSendItem%3805CE0F02E1.body preserve=yes
	AES_GCC_Errorcodes error;
        if( pTq_->changeFileStatus(newStatus,currentFilename_,error) )
        {
		AES_AFP_TRACE_MESSAGE("OK");
        }
        else
        {       
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Changing status to %d for file %s in TQ %s has failed with error %d",newStatus, currentFilename_.c_str(), currentTransferQueue_.c_str(),error);
		AES_AFP_TRACE_MESSAGE("Changing status to %d for file %s in TQ %s has failed with error %d",newStatus, currentFilename_.c_str(), currentTransferQueue_.c_str(),error);
        }
}

//## Operation: sendMessageToCdhTask%3805CF5C02A4; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendMessageToCdhTask()
//	---------------------------------------------------------
bool aes_afp_destination::sendMessageToCdhTask (aes_afp_msgtypes message, AES_GCC_Errorcodes error)
{
	//## begin aes_afp_destination::sendMessageToCdhTask%3805CF5C02A4.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Entering ,Message %d", message);
	AES_AFP_LOG(LOG_LEVEL_WARN, "Entering ,Message %d", message);
	aes_afp_msgblock* messageToCdhTask = 0;

	messageToCdhTask = new aes_afp_msgblock(0, 0);
	messageToCdhTask->setMsgType(message);
	messageToCdhTask->setCallerId(this);
	messageToCdhTask->setErrorCode(error);

	if (message == CDHFINISHED)
	{
		sendRetryNo_ = 0;
	}

	int retry = 0;
	int ret = 0;
	do
	{
		if(shutdownInProgress_)
		{
			AES_AFP_TRACE_MESSAGE("eventDestinationSet(), CDH task thread exited, shutdownInProgress_ is true");
			break;
		}

		ret = messageQueue_->enqueue_tail(messageToCdhTask);
		int lasterrno = ACE_OS::last_error();
		if( ret >= 0 )
		{
			break;
		}
		else if( ret == -1 )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "enqueue_tail failed %d, retry = %d, errno = %d", ret, retry, lasterrno);
			AES_AFP_TRACE_MESSAGE("enqueue_tail failed %d, retry = %d, errno = %d", ret, retry, lasterrno);
			retry++;
		}
	}
	while((ret == -1) && (retry < 5 ));
	AES_AFP_TRACE_MESSAGE("Leaving , Message %d", message);
	if( ret == -1 )
	{
		return false;
	}
	else
	{
		return true;
	}
}

//## Operation: copyFromDatablock%3805E4860100; C++
//## Semantics:
//	---------------------------------------------------------
//	       copyFromDatablock()
//	---------------------------------------------------------
void aes_afp_destination::copyFromDatablock (aes_afp_datablock* dataBlock)
{
	//## begin aes_afp_destination::copyFromDatablock%3805E4860100.body preserve=yes
	fileToTransfer_ = dataBlock->getFullPath();
	currentTransferQueue_ = dataBlock->getTransferQueue();
	currentDestinationSet_ = dataBlock->getDestinationSet();
	currentFilename_ = dataBlock->getFilename();
	currentNewFilename_ = dataBlock->getNewFilename();
	numberOfSendRetries_ = dataBlock->getSendRetries();
	retryDelay_ = dataBlock->getRetriesDelay();
	manualOrder_ = dataBlock->getManual();
	renameFile_ = dataBlock->getFileRename();
	renameTemplate_ = dataBlock->getFileTemplate();
	isADirectory_ = dataBlock->getDirectoryFlag();
	fileMask_ = dataBlock->getMask();
	format_ = dataBlock->getFormat();
	restarted_ = dataBlock->getAfpRestarted();
	//## end aes_afp_destination::copyFromDatablock%3805E4860100.body
}

//## Operation: stopFileTransfer%38081B8402A3; C++
//## Semantics:
//	---------------------------------------------------------
//	       stopFileTransfer()
//	---------------------------------------------------------
int aes_afp_destination::stopFileTransfer ()
{
	//## begin aes_afp_destination::stopFileTransfer%38081B8402A3.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Stopping file transfer for %s in TQ %s",
			fileToTransfer_.c_str(), currentNewFilename_.c_str());
	AES_CDH_ResultCode cdhResult;

	cdhResult = stopSendFile(fileToTransfer_, currentTransferQueue_, currentNewFilename_);
	sendRetryNo_ = 0;

	switch (cdhResult)
	{
	case AES_CDH_RC_NOTASK:
	{
		AES_AFP_TRACE_MESSAGE("CDH reported no task");
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		AES_AFP_TRACE_MESSAGE("Returning -1");
		return -1;
	}
	case AES_CDH_RC_OK:
	{
		AES_AFP_TRACE_MESSAGE("CDH reported file stopped");
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		return 0;
	}
	case AES_CDH_RC_NOSERVER:
	{
		AES_AFP_TRACE_MESSAGE("CDH reported no server");
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		return -1;
	}
	default:
	{
		AES_AFP_TRACE_MESSAGE("Reached default, cdhResult = %d",cdhResult);
		sendMessageToCdhTask(CDHFINISHED, AES_NOERRORCODE);
		return -1;
	}
	}
	//## end aes_afp_destination::stopFileTransfer%38081B8402A3.body
}

//## Operation: eventDestinationSet%3B3B27E603AD; C++
//## Semantics:
//	---------------------------------------------------------
//	       eventDestinationSet()
//	---------------------------------------------------------
void aes_afp_destination::eventDestinationSet (AES_CDH_ResultCode ret,
		const std::string& destinationSet)
{
	//## begin aes_afp_destination::eventDestinationSet%3B3B27E603AD.body preserve=yes
	AES_AFP_TRACE_MESSAGE( "DestinationSet %s",destinationSet.c_str());
	switch (ret)
	{
	case AES_CDH_RC_CONNECTOK:
	{
		AES_AFP_TRACE_MESSAGE("CDH event CONNECTOK");
		sendMessageToCdhTask(CDHCONNECTED, AES_NOERRORCODE);
		break;
	}
	case AES_CDH_RC_CONNECTERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event CONNECTERR");
		sendMessageToCdhTask(CDHCONNECTERROR, AES_CONNECTERROR);
		break;
	}
	case AES_CDH_RC_EVENTHANDLERSET:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERSET");
		break;
	}
	case AES_CDH_RC_EVENTHANDLERSETERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERSETERR");
		sendMessageToCdhTask(CDHEVENTHANDLERSETERR, AES_EVENTHANDLERSETERR);
		break;
	}
	case AES_CDH_RC_EVENTHANDLERREM:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERREM");
		sendMessageToCdhTask(CDHEVENTHANDLERREM, AES_NOERRORCODE);
		break;
	}
	case AES_CDH_RC_EVENTHANDLERREMERR:
	{
		AES_AFP_TRACE_MESSAGE("CDH event EVENTHANDLERREMERR\n");
		sendMessageToCdhTask(CDHEVENTHANDLERREMERR, AES_EVENTHANDLERREMERR);
		break;
	}
	case AES_CDH_RC_NOSERVER:
	{
		AES_AFP_TRACE_MESSAGE("CDH event NOSERVER for %s",destinationSet.c_str());
		sendMessageToCdhTask(CDHFINISHED, AES_NOCDHSERVER);
		sendMessageToCdhTask(CDHREOPEN, AES_NOCDHSERVER);
		break;
	}
	case AES_CDH_RC_DESTSETDOWN:
	{
		afpDatabase->sendApplEvent(currentTransferQueue_, AES_EVEDESTSETDOWN, AES_NOSERVERACCESS);
		break;
	}
	case AES_CDH_RC_DESTSETDOWNCEASE:
	{
		afpDatabase->sendApplEvent(currentTransferQueue_, AES_EVEDESTSETDOWNCEASE, AES_NOERRORCODE);
		break;
	}
	case AES_CDH_RC_CDHSTOPPING:
	{
		AES_AFP_TRACE_MESSAGE("CDH CDHSTOPPING");
		sendMessageToCdhTask(CDHSTOPPED, AES_NOERRORCODE);
		break;
	}
	default:
	{
		AES_AFP_TRACE_MESSAGE("Default reached, CDH event UNKNOWN");
		sendMessageToCdhTask(UNKNOWN, AES_ERRORUNKNOWN);
		break;
	}
	}
	//## end aes_afp_destination::eventDestinationSet%3B3B27E603AD.body
}

//## Operation: copyToDatablock%3805E4860100; C++
//## Semantics:
//	---------------------------------------------------------
//	       copyFromDatablock()
//	---------------------------------------------------------
void aes_afp_destination::copyToDatablock (aes_afp_datablock* dataBlock)
{
	//## begin aes_afp_destination::copyFromDatablock%3805E4860100.body preserve=yes
	dataBlock->setFullPath(fileToTransfer_ );
	dataBlock->setTransferQueue(currentTransferQueue_ );
	dataBlock->setDestinationSet(currentDestinationSet_ );
	dataBlock->setFilename(currentFilename_ );
	dataBlock->setNewFilename(currentNewFilename_ );
	dataBlock->setSendRetries(numberOfSendRetries_ );
	dataBlock->setRetriesDelay(retryDelay_ );
	if( manualOrder_ )
		dataBlock->setManual();
	dataBlock->setFileRename(renameFile_ );
	dataBlock->setFileTemplate(renameTemplate_ );
	if( isADirectory_ )
		dataBlock->setDirectoryFlag();
	dataBlock->setMask(fileMask_ );
	dataBlock->setFormat(format_ );
	dataBlock->setAfpRestarted(restarted_ );
}

void aes_afp_destination::setShutdownForDest()
{
	shutdownInProgress_ = true;
}
