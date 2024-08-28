/******************************************************************************/
/**
@file transdest.cpp

Class method implementation for transdest.h

DESCRIPTION
This class describes a destination in the server part of CDH.

ERROR HANDLING
General rule:

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <transdest.h>
#include <ftprv2.h>
#include <servr.h>
#include <aes_gcc_eventhandler.h>
#include <aes_gcc_event.h>
#include <event.h>
#include <ace/ACE.h>
#include <ace/Message_Block.h>
#include <ace/Get_Opt.h>
#include <aes_gcc_variable.h>
#include <parameter.h>
#include <cdhcriticalsection.h>
#include <destinationalarm.h>
#include <aes_gcc_util.h>
#include <aes_cdh_common.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;

/*===================================================================
                        DECLARATION SECTION
=================================================================== */
AES_CDH_TRACE_DEFINE(AES_CDH_TransDest);

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#ifdef _DEBUG
#define DEBUGTEST
#endif

/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
const string probableCause= "CONNECTION SUPERVISION, AP CDH, CONNECTION TO REMOTE SYSTEM LOST";

const ACE_INT32 eventCode=AES_CDH_ConnectFault;

ACE_Mutex TransDest::resNotifMX_;

ACE_Thread_Manager* TransDest::resendThreadManager_ = NULL;

ResendNotification* TransDest::presendnotif_ = NULL;

bool TransDest::resendNotificationRunning_ = false;

ACE_INT32 TransDest::numberOfNotificationDestinations_ = 0;

/*===================================================================
   ROUTINE:TransDest
=================================================================== */
TransDest::TransDest() : PollVdName_(""),
		dest_(""),
		destSetName_(""),
		ptrans_(NULL),
		ptransForCheckConnection_(NULL),
		tmpPtransForCheckConnection_(NULL),
		pnotif_(NULL),
		PollObj(NULL),
		isPollingTrue(false),
		notificationRunning(false),
		exitThread_(false)
{
	connectionResult_ = AES_CDH_RC_CONNECTOK;
	threadManager_ = new ACE_Thread_Manager;        // File Notification, 020212
	DestAvailResult_ = AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:~TransDest
=================================================================== */
TransDest::~TransDest()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	DestinationAlarm::instance()->destroyDestinationEntry(dest_);

	if (pnotif_ != NULL)
	{
		if (notificationRunning == true)
		{
			MessageBlock *mb = new MessageBlock(MT_HANGUP);
			(pnotif_->msg_queue())->enqueue_head(mb);
			threadManager_->wait();

			// Shut down resend thread if last notification thread has been stopped
			StopResendNotification();

		}

		delete pnotif_;
		AES_CDH_TRACE_MESSAGE("File Notification DELETED");
	}

	if ( GetPollingStatus() && PollObj != NULL)
	{
		PollObj->shutdown();
		delete PollObj;
		PollObj = 0;
	}

	if( threadManager_ != 0 )
	{
		delete threadManager_;
		threadManager_ = 0;
	}

	if( ptrans_  != 0 )
	{
		delete ptrans_;
		ptrans_ = 0;
	}

	if( ptransForCheckConnection_ != 0 )
	{
		delete ptransForCheckConnection_;
		ptransForCheckConnection_ = 0;
	}

	AES_CDH_TRACE_MESSAGE("Exiting");
}

/*===================================================================
   ROUTINE:open
=================================================================== */
ACE_INT32 TransDest::open(void)
{
	exitThread_ = false;
	return this->activate();
}

ACE_INT32 TransDest::close(ulong  flag)
{
	(void)flag;
	AES_CDH_TRACE_MESSAGE("Active job being closed down");
	return 0;
}

/*===================================================================
   ROUTINE:svc
=================================================================== */
ACE_INT32 TransDest::svc(void)
{
	ACE_Message_Block* mb;
	setEvMsg *data;

	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeOut( this->gettimeofday());

	//check connection before starting service loop
	/*! commented as deletion of destination taking more time */
	//	connectionResult_ = checkConnection();
	AES_CDH_TRACE_MESSAGE("THREAD started for dest = %s", dest_.c_str());

	do
	{
		// Ten minutes between checkconnection
		timeOut = this->gettimeofday();
		timeOut += ACE_Time_Value(60*10, 0);

		if (getq(mb,&timeOut) >= 0)
		{

			data = (setEvMsg*)mb->base();
			AES_CDH_TRACE_MESSAGE("TransDest::svc message %d ",data->cmdC);
			handleMessage(data);
			delete data;
			mb->release();
		}
		else
		{
			connectionResult_ = checkConnection();
		}
	} while (! exitThread_);

	AES_CDH_TRACE_MESSAGE("THREAD exited for dest = %s", dest_.c_str());

	return 0;
}

/*===================================================================
   ROUTINE:checkConnection
=================================================================== */
AES_CDH_ResultCode TransDest::checkConnection(bool doCheck)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	if((doCheck))
	{
		connectionResult_ = checkConnection(DISABLE);  // TR HX18084
	}
	return connectionResult_;
}

/*===================================================================
   ROUTINE:checkConnectionManually
=================================================================== */
AES_CDH_ResultCode TransDest::checkConnectionManually()
{
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	// Do not attempt to check anything that does not exist
	if ( pnotif_ != NULL )
	{
		MessageBlock *mb = new MessageBlock(MT_CHECKCONNECTION_MANUALLY);
		pnotif_->putq(mb);

		ACE_Message_Block *reply;

		// Wait at most 8 seconds for a reply
		ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout(this->gettimeofday());
		timeout += ACE_Time_Value(8,0);

		if (notifQueue.dequeue_head(reply, &timeout) != -1)
		{
			MessageBlock *mbReply = (MessageBlock *)reply;
			switch (mbReply->msgType())
			{

			case MT_OK:
			{
				rCode = AES_CDH_RC_CONNECTOK;
				AES_CDH_LOG(LOG_LEVEL_INFO, "MT_OK");
				AES_CDH_TRACE_MESSAGE("case MT_OK");
				reply->release();
				break;
			}

			case MT_CONNECTOK:
			{
				rCode = AES_CDH_RC_CONNECTOK;
				AES_CDH_LOG(LOG_LEVEL_INFO, "MT_CONNECTOK");
				AES_CDH_TRACE_MESSAGE("case MT_CONNECTOK");
				reply->release();
				break;
			}

			case MT_CONNECTERR:
			{
				rCode = AES_CDH_RC_CONNECTERR;
				AES_CDH_LOG(LOG_LEVEL_ERROR, "MT_CONNECTERR");
				AES_CDH_TRACE_MESSAGE("case MT_CONNECTERR");
				reply->release();
				break;
			}

			case MT_ERROR:
			{
				rCode = AES_CDH_RC_CONNECTERR;
				AES_CDH_LOG(LOG_LEVEL_ERROR, "MT_ERROR");
				AES_CDH_TRACE_MESSAGE("case MT_ERROR");
				reply->release();
				break;
			}

			default:
			{
				rCode = AES_CDH_RC_INTPROGERR;
				AES_CDH_LOG(LOG_LEVEL_INFO, "default");
				AES_CDH_TRACE_MESSAGE("case default");
				reply->release();
				break;
			}
			}
		}
		else    // Time out expired
		{
			rCode = AES_CDH_RC_TIMEOUT;
			AES_CDH_LOG(LOG_LEVEL_INFO, "AES_CDH_RC_TIMEOUT");
			AES_CDH_TRACE_MESSAGE("AES_CDH_RC_TIMEOUT");
		}
	}
	else
	{
		rCode = AES_CDH_RC_NOTNOTIFDEST;
		AES_CDH_LOG(LOG_LEVEL_INFO, "AES_CDH_RC_NOTNOTIFDEST");
		AES_CDH_TRACE_MESSAGE("AES_CDH_RC_NOTNOTIFDEST");
	}
	AES_CDH_TRACE_MESSAGE("Returning rCode = %d",rCode);
	return rCode;
}

/*===================================================================
   ROUTINE:checkNotification
=================================================================== */
AES_CDH_ResultCode TransDest::checkNotification()
{
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	if ( pnotif_ != NULL )
	{
		MessageBlock *mb = new MessageBlock(MT_CHECKCONNECTION);
		pnotif_->putq(mb);

		ACE_Message_Block *reply;

		// Wait at most 8 seconds for a reply
		ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout(this->gettimeofday());
		timeout += ACE_Time_Value(8,0);

		if (notifQueue.dequeue_head(reply, &timeout) != -1)
		{
			MessageBlock *mbReply = (MessageBlock *)reply;
			switch (mbReply->msgType())
			{

			case MT_OK:
			{
				rCode = AES_CDH_RC_CONNECTOK;
				reply->release();
				break;
			}

			case MT_CONNECTOK:
			{
				rCode = AES_CDH_RC_CONNECTOK;
				reply->release();
				break;
			}

			case MT_CONNECTERR:
			{
				rCode = AES_CDH_RC_CONNECTERR;
				reply->release();
				break;
			}

			case MT_ERROR:
			{
				rCode = AES_CDH_RC_CONNECTERR;
				reply->release();
				break;
			}

			default:
			{
				rCode = AES_CDH_RC_INTPROGERR;
				reply->release();
				break;
			}
			}
		}
		else    // Time out expired
		{
			rCode = AES_CDH_RC_TIMEOUT;
		}

	}
	else    // Notification is not defined
	{
		rCode = AES_CDH_RC_NOTNOTIFDEST;
	}

	return rCode;
}

/*===================================================================
   ROUTINE:isNotificationDefined
=================================================================== */
bool TransDest::isNotificationDefined()
{
	if (pnotif_ == NULL)
	{
		return false;           // Not notification
	}
	return true;
}

/*===================================================================
   ROUTINE:GetPollObj
=================================================================== */
ReadyPoll* TransDest::GetPollObj()
{	
	if ( PollObj != NULL )
	{
		return PollObj;
	}
	return NULL;	
}
/*===================================================================
   ROUTINE:GetPollingStatus
=================================================================== */
bool TransDest::GetPollingStatus()
{
	return isPollingTrue;
}

/*===================================================================
   ROUTINE:define
=================================================================== */
AES_CDH_ResultCode TransDest::define(const string destName,
		const string transType,
		ACE_INT32 &argc,
		ACE_TCHAR* argv[],
		const string& userGroup,
		const bool define,
		const bool recovery,
		const string &destRdn,
		const string destSetName)
{
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	m_destRdn = destRdn;
	TransferAgent* transPtr = 0;
	AES_CDH_LOG(LOG_LEVEL_INFO, "Define destName = %s",destName.c_str());
	AES_CDH_TRACE_MESSAGE("destName = %s",destName.c_str());

	// File Notif, test start
	std::string connectType("");
	std::string notifAddress("");
	std::string notifPort("");
	std::string responseTime("");
	std::string eventType("");
	std::string maxAge("");
	//bool vdExistFlg = false;

	ACE_INT32 parseNotifParamsReply = this->parseNotifParams(argc,
			argv,
			connectType,
			notifAddress,
			notifPort,
			responseTime,
			eventType);
	if(parseNotifParamsReply == -1)
	{
		return AES_CDH_RC_INCUSAGE;
	}
	ACE_INT32 tempArgc = argc;
	ACE_TCHAR *tempArgv[32] = { 0 };

	for (ACE_INT32 i = 0; i < argc; i++)
	{
		tempArgv[i] = new ACE_TCHAR[ACE_OS::strlen(argv[i])+1];
		(void)ACE_OS::strcpy(tempArgv[i], argv[i]);
	}
	dest_ = destName;
	PollDest_ = destName;

#ifdef DEBUGTEST
	std::cout << "1.TransDest::define() try to call TransferAgent::create transType: "<< + transType.c_str();
	std::cout << "\n";
#endif
	// Define TransferAgent
	rcode = TransferAgent::create(this, transType, argc, argv, define, recovery, transPtr);

#ifdef DEBUGTEST
	std::cout << "1.TransDest::define() called TransferAgent::create()\n";
#endif

	if ( rcode == AES_CDH_RC_OK )
	{
		ptrans_ = transPtr;
		usergrp_ = userGroup;

		//Changed for SFTP Responding IP, inserted a check for sftpv2 along with ftpv2
		if (((::strcasecmp(transType.c_str(), "ftpv2")) == 0)||((::strcasecmp(transType.c_str(),"sftpv2"))==0))
		{
			if  ((::strcasecmp(connectType.c_str(), "r")) == 0)
			{
				//For CDH Ready Directory Polling Functionality.

				PollObj = new(std::nothrow) ReadyPoll(PollDest_);

				if (PollObj == NULL)
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "PollObj not Created");
					rcode = AES_CDH_RC_INTPROGERR;
				}
				else
				{
					PollObj->setCDHReadyPollParams(ptrans_);
					// Validate and start thread	
					if (PollObj->validatePollParams()) 
					{
						ACE_INT32 PollObj_error = PollObj->open();
						if (PollObj_error == -1)
						{
							// The Ready directory Polling thread didn't start.
							AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to start Ready directory Polling thread");
							AES_CDH_TRACE_MESSAGE("Not able to start Ready directory Polling thread");
							rcode = AES_CDH_RC_INTPROGERR;
							delete PollObj;
						}
						isPollingTrue = true;
					}
					else
					{
						delete PollObj;
					}
				}
			}
		}
		// File Notification, 020212, start
		if (notificationRunning == false && parseNotifParamsReply > 0)
		{
			//Changed for SFTP Responding IP, inserted a check for sftpv2 along with ftpv2
			if ( ((::strcasecmp(transType.c_str(), "ftpv2")) == 0) ||((::strcasecmp(transType.c_str(),"sftpv2"))==0))
			{
				if  ((::strcasecmp(connectType.c_str(), "r")) == 0)
				{
					pnotif_ = new Notification;
					if (pnotif_ == NULL)
					{
						rcode = AES_CDH_RC_INTPROGERR;
					}
					else
					{
						rcode = pnotif_->define(dest_,
								notifAddress,
								notifPort,
								responseTime,
								eventType,
								destSetName);

						if (rcode == AES_CDH_RC_OK)
						{
							ACE_INT32 err = pnotif_->open(&notifQueue, threadManager_);
							if (err == -1)
							{
								// The notification thread did not start
								rcode = AES_CDH_RC_INTPROGERR;
								delete pnotif_;
							}
							else
							{
								// Resend File Notification
								if(!resendNotificationRunning_)
								{
									rcode = StartResendNotification();
									if(rcode != AES_CDH_RC_OK)
									{
										delete pnotif_;
									}
								}
								if(resendNotificationRunning_)
								{
									notificationRunning = true;
									resNotifMX_.acquire();
									numberOfNotificationDestinations_++;
									resNotifMX_.release();
									string aesDataDirectory("");
									string cdhRootDirectory("");
									string newDirectory("");

									if (this->readParameters(aesDataDirectory, cdhRootDirectory))
									{
										newDirectory.assign(aesDataDirectory);
										newDirectory.append("/");
										newDirectory.append(cdhRootDirectory);
										newDirectory.append("/");
										newDirectory.append(ServR::getDataBaseDirectory());
										newDirectory.append("/");
										newDirectory.append(destName);

										if (ACE_OS::mkdir(newDirectory.c_str(), NULL) == -1 && ACE_OS::last_error() != EEXIST)
										{
											string newdr("");
											if (AES_GCC_Util::datapath_trn(newDirectory,AES_DATA_PATH,newdr)==false)
											{
												AES_CDH_LOG(LOG_LEVEL_INFO, "Directory name sent for event %s",newdr.c_str());
												AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
											}
											AES_CDH_TRACE_MESSAGE("Event raised on directory: %s",newdr.c_str());
											Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM", ServR::NTErrorText(), "Database destination directory could not be created: " + newdr);
										}
									}
								}
							}
						}
					} // else
				} // if
			}
		}

		// File Notification, 020212, end
		if (rcode == AES_CDH_RC_OK)
		{
#ifdef DEBUGTEST
			std::cout << "2.TransDest::define() try to call TransferAgent::create transType: "<< + transType.c_str();
			std::cout << "\n";
#endif
			rcode = TransferAgent::create(this,
					transType,
					tempArgc,
					tempArgv,
					define,
					recovery,
					ptransForCheckConnection_);
#ifdef DEBUGTEST
			std::cout << "2.TransDest::define() TransferAgent::create() called \n";
#endif
		}
	}

	if ( rcode == AES_CDH_RC_OK )
	{
		open();
	}

	// Release tempArgv
	for (ACE_INT32 j = 0; j < tempArgc; j++)
	{
		delete[] tempArgv[j];
	}

	return rcode;
}
/*===================================================================
   ROUTINE:changeAttr
=================================================================== */
AES_CDH_ResultCode TransDest::changeAttr(const string transType, 
		ACE_INT32 &argc, ACE_TCHAR* argv[])
{
	AES_CDH_ResultCode rcode;
	ACE_INT32 changeNotification = 0;

	// File Notif, test start
	std::string connectType("");
	std::string notifAddress("");
	std::string notifPort("");
	std::string responseTime("");
	std::string eventType;

	this->parseNotifParams(argc,
			argv,
			connectType,
			notifAddress,
			notifPort,
			responseTime,
			eventType);

	changeNotification = this->checkNotifChangeAttr(argc,
			argv,
			connectType,
			notifAddress,
			notifPort,
			responseTime,
			eventType);

	TransferAgent* newTransPtr;
	TransferAgent* oldTransPtr = ptrans_;       // Save old TransferAgent;

	ACE_INT32 tempArgc = argc;
	ACE_TCHAR *tempArgv[32];

	for (ACE_INT32 i = 0; i < argc; i++)
	{
		tempArgv[i] = new ACE_TCHAR[::strlen(argv[i])+1];
		(void)::strcpy(tempArgv[i], argv[i]);
	}

	// Define TransferAgent
	rcode = TransferAgent::create(this,
			transType,
			argc,
			argv,
			false,
			false,
			newTransPtr);

	if ( rcode == AES_CDH_RC_OK )
	{
		oldTransPtr = ptrans_;
		ptrans_ = newTransPtr;                  // Insert new TransferAgent
		//rcode = ServR::saveDestinations();      // Save destinations to file
		ptrans_ = oldTransPtr;
		if (rcode == AES_CDH_RC_OK)             // Destinations successfully saved
		{
			if ((rcode = ptrans_->changeAttr( newTransPtr )) == AES_CDH_RC_OK)
			{
				// pnotif_ is non-NULL if notification is defined
				if (pnotif_ != NULL && changeNotification > 0)
				{
					pnotif_->changeAttr(notifAddress, notifPort, responseTime, eventType);
					AES_CDH_TRACE_MESSAGE("Putting msg MT_CHANGEATTR in notif queue");
					pnotif_->putq(new MessageBlock(MT_CHANGEATTR));
				}
			}
			else
			{
				ptrans_ = oldTransPtr;          // Restore old TransferAgent
				delete newTransPtr;             // Delete new TransferAgent
				//(void) ServR::saveDestinations();   // Save to file
			}
		}
#if 0
		//commented for coverity fix
		else                                    // Destinations could not be saved
		{
			ptrans_ = oldTransPtr;              // Restore old TransferAgent
			delete newTransPtr;                 // Delete new TransferAgent
			//(void) ServR::saveDestinations();
		}
#endif
		if (rcode == AES_CDH_RC_OK)
		{
			// if there is file traffic and cdhch is executed, the file traffic will continue
			// to go to the previous version of the destination. Check connection must be
			// connected to the old version of the destination until file traffic is commited.
			if ((ptrans_->getAgentType()) == TransferAgent::BLOCK)
			{
				// cdhch is made earlier while block traffic is running
				// check that blocktraffic is still running, if not delete old pointer
				if (tmpPtransForCheckConnection_ != NULL && !ptrans_->dataTransfer())
				{
					//delete tmpPtransForCheckConnection_;
					tmpPtransForCheckConnection_ = NULL;
				}

				// cdhch is made, check if block traffic is running, in that case save
				// old pointer to check connection questions
				else if (tmpPtransForCheckConnection_ == NULL)
				{
					if (oldTransPtr->dataTransfer())
					{
						tmpPtransForCheckConnection_ = oldTransPtr;
					}
				}

				delete ptransForCheckConnection_;
				rcode = TransferAgent::create( this, transType, tempArgc, tempArgv, false, false, ptransForCheckConnection_ );
			}
			else
			{
				checkConnMutex.acquire(); //No connection verification is taking place.
				delete ptransForCheckConnection_;
				rcode = TransferAgent::create( this, transType, tempArgc, tempArgv, false, false, ptransForCheckConnection_ );
				checkConnMutex.release();

			}
		}
	}

	for (ACE_INT32 j = 0; j < tempArgc; j++)
	{
		delete[] tempArgv[j];
	}
	return rcode;
}

/*===================================================================
   ROUTINE:getAttr
=================================================================== */
AES_CDH_ResultCode TransDest::getAttr( AES_CDH_Destination::destAttributes &attr )
{
	// Get destination name and destination set name from TransDest
	attr.destName = dest_;
	attr.destSetName = destSetName_;

	AES_CDH_TRACE_MESSAGE("Entering");

	// Get other attributes from TransferAgent
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	rCode = ptrans_->getAttributes(attr.transferType, attr.otherAttrs);

	// Get physical path to destination
	attr.destPath = ptrans_->getVDDestPath();

	/*if (rCode == AES_CDH_RC_OK && usergrp_ != "")
   	{
      		attr.otherAttrs.push_back("-v");
      		attr.otherAttrs.push_back(usergrp_);
   	}*/
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rCode;
}

/*===================================================================
   ROUTINE:getSingleAttr
=================================================================== */
AES_CDH_ResultCode TransDest::getSingleAttr(const ACE_TCHAR* opt, void* value)
{
	return ptrans_->getSingleAttr(opt, value);
}

/*===================================================================
   ROUTINE:sendFile
=================================================================== */
AES_CDH_ResultCode TransDest::sendFile(const string& fileName, 
		const string& remoteSubDirName,
		const string& newFileName,
		const AES_CDH_DestinationSet::transferMode trMode,
		const string& fileMask,
		const bool isDir)
{
	AES_CDH_ResultCode rCode;

	if(! isDir)
	{
		rCode = ptrans_->sendFile(fileName, remoteSubDirName, newFileName, trMode);
	}
	else
	{
		rCode = ptrans_->sendDirectory(fileName, remoteSubDirName, newFileName, trMode, fileMask);
	}

	return rCode;
}

/*===================================================================
   ROUTINE:sendRecordFile
=================================================================== */


AES_CDH_ResultCode TransDest::sendRecordFile(const string& streamName,
		const string& streamId,
		AES_DBO_DataBlock*& block,
		CmdHandler *cmdHdlr,
		const ACE_UINT64 ticks)
{
	return ptrans_->sendRecordFile(streamName, streamId, block, cmdHdlr, ticks);
}


/*===================================================================
   ROUTINE:remove
=================================================================== */
AES_CDH_ResultCode TransDest::remove(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	if (destSetName_ != "")
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Destination is part of a destination set");
		return AES_CDH_RC_PROTECTEDDEST;
	}
	//cease destination alarm
	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	alarmCause = DestinationAlarm::AlarmRecord::ok;
	DestinationAlarm::instance()->cease(dest_, destSetName_, alarmCause);
	AES_CDH_LOG(LOG_LEVEL_INFO, "ceasing alarms for destination %s",dest_.c_str());
	shutDown();
	AES_CDH_TRACE_MESSAGE("Waiting for all threads to close");
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	if (ptrans_ != NULL)        // If there is a TransferAgent
	{
		// ***** File Notification, start *****
		rCode = ptrans_->remove();
		AES_CDH_TRACE_MESSAGE(" rCode = %d",rCode);
		// If notification is not defined, we are finished here
		if (rCode == AES_CDH_RC_OK && !notificationRunning)
		{
			delete ptransForCheckConnection_;
			ptransForCheckConnection_ = NULL;
			AES_CDH_TRACE_MESSAGE("Exited remove with rcode = %d ",rCode);
			return rCode;
		}

		if (rCode == AES_CDH_RC_OK && pnotif_ != NULL)
		{
			MessageBlock *mb = new MessageBlock(MT_HANGUP);
			(pnotif_->msg_queue())->enqueue_head(mb);
			this->notificationRunning = false;

			AES_CDH_TRACE_MESSAGE("Calling Thread Manager wait in TransDest::remove()");
			threadManager_->wait();
			string aesDataDirectory("");
			string cdhRootDirectory("");
			if (this->readParameters(aesDataDirectory, cdhRootDirectory))
			{
				string dbPath("");
				this->makeDataBaseDestPath(dbPath, aesDataDirectory, cdhRootDirectory, dest_);
				AES_CDH_TRACE_MESSAGE("Calling removeDataBaseDestDir");
				this->removeDataBaseDestDir(dbPath);
				delete ptransForCheckConnection_;
				ptransForCheckConnection_ = NULL;
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error removing database destination directory ");
				AES_CDH_TRACE_MESSAGE("Error removing database destination directory ");
				// Error removing database destination directory
			}

			// Remove resend file notification thread if this was the last notification thread
			StopResendNotification();
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Called open again");
			open();
		}
		// ***** File Notification, end *****
	}
	else
	{
		rCode = AES_CDH_RC_OK;
	}

	AES_CDH_TRACE_MESSAGE("Return code from TranDest remove = %d ",rCode);
	return rCode;
}

/*===================================================================
   ROUTINE:getDestinationName
=================================================================== */
string TransDest::getDestinationName(void)
{
	return dest_;
}

/*===================================================================
   ROUTINE:transactionBegin
=================================================================== */
AES_CDH_ResultCode TransDest::transactionBegin()
{
	AES_CDH_ResultCode rc = ptrans_->transactionBegin();
	return rc;
}

/*===================================================================
   ROUTINE:transactionEnd
=================================================================== */
AES_CDH_ResultCode TransDest::transactionEnd(ACE_UINT32 &applBlockNr)
{
	return ptrans_->transactionEnd(applBlockNr);
}

/*===================================================================
   ROUTINE:transactionCommit
=================================================================== */
AES_CDH_ResultCode TransDest::transactionCommit(ACE_UINT32 &applBlockNr)
{
	return ptrans_->transactionCommit(applBlockNr);
}

/*===================================================================
   ROUTINE:getLastCommittedBlock
=================================================================== */
AES_CDH_ResultCode TransDest::getLastCommittedBlock(ACE_UINT32 &applBlockNr)
{
	return ptrans_->getLastCommittedBlock(applBlockNr);
}

/*===================================================================
   ROUTINE:transactionTerminate
=================================================================== */
AES_CDH_ResultCode TransDest::transactionTerminate()
{
	return ptrans_->transactionTerminate();
}

/*===================================================================
   ROUTINE:setAPIClosed
=================================================================== */
AES_CDH_ResultCode TransDest::setAPIClosed()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	return ptrans_->setAPIClosed();
}

/*===================================================================
   ROUTINE:getAgentType
=================================================================== */
TransferAgent::TransferAgentAttributes TransDest::getAgentType(void )
{
	return ptrans_->getAgentType();
}

/*===================================================================
   ROUTINE:setDestSetName
=================================================================== */
void TransDest::setDestSetName(const string& destSetName)
{
	destSetName_ = destSetName;
}

/*===================================================================
   ROUTINE:getDestSetName
=================================================================== */
string TransDest::getDestSetName(void )
{
	return destSetName_;
}

/*===================================================================
   ROUTINE:parseNotifParams
=================================================================== */
ACE_UINT32 TransDest::parseNotifParams(ACE_INT32 &argc, ACE_TCHAR *argv[],
		std::string &connectType,
		std::string &notifAddress,
		std::string &notifPort,
		std::string &responseTime,
		std::string &eventType)
{
	ACE_INT32 retValue = 0;
	ACE_INT32 ix = 0;
	while (ix < argc)
	{
		if ( (strcmp(argv[ix], "-c")) == 0 )
		{
			if(ix != argc-1)
			{
				connectType.assign(argv[ix + 1]);
			}
			else
			{
				return -1;
			}
		}

		if ( (strcmp(argv[ix], "-e")) == 0 )
		{
			if (ix != argc-1)
			{
				eventType.assign(argv[ix + 1]);
			}
			else
			{
				return -1;
			}
		}
		if ( (strcmp(argv[ix], "-f")) == 0 ) 
		{
			if(ix != argc-1)
			{
				notifAddress.assign(argv[ix + 1]);
				retValue++;
			}
			else
			{
				return -1;
			}
		}

		if ((strcmp(argv[ix], "-x")) == 0 )
		{
			if (ix != argc-1)
			{
				notifPort.assign(argv[ix + 1]);
				retValue++;
			}
			else
			{
				return -1;
			}
		}

		if ( ( (strcmp(argv[ix], "-y")) == 0 ) && (ix != argc-1))
		{
			if (ix != argc-1)
			{
				responseTime.assign(argv[ix + 1]);
			}
			else
			{
				return -1;
			}
		}

		ix++;
	}

	return retValue;
}
/*===================================================================
   ROUTINE:checkNotifChangeAttr
=================================================================== */
ACE_UINT32 TransDest::checkNotifChangeAttr(ACE_INT32 &argc, ACE_TCHAR *argv[],
		std::string &connectType,
		std::string &notifAddress,
		std::string &notifPort,
		std::string &responseTime,
		std::string &eventType)
{
	(void) connectType;
	ACE_INT32 ix = 0;
	ACE_INT32 changed = 0;
	while (ix < argc)
	{
		if ( (strcmp(argv[ix], "-e")) == 0 )
		{
			eventType.assign(argv[ix + 1]);
			changed++;
		}

		if ( (strcmp(argv[ix], "-f")) == 0 )
		{
			notifAddress.assign(argv[ix + 1]);
			changed++;
		}

		if ( (strcmp(argv[ix], "-x")) == 0 )
		{
			notifPort.assign(argv[ix + 1]);
			changed++;
		}

		if ( (strcmp(argv[ix], "-y")) == 0 )
		{
			responseTime.assign(argv[ix + 1]);
			changed++;
		}

		ix++;
	}
	return changed;     // >0 means that there is a change
}

/*===================================================================
   ROUTINE:readParameters
=================================================================== */
bool TransDest::readParameters(string &path1, string &path2)
{
	// Get parameters from PHA
	if (AES_CDH_Paths::instance()->getCDHDataPath( path1 ) )
	{
		path2 = AES_CDH_RootDirectory;
		return true;
	}
	else
	{
		// Event report, could not get parameters from PHA
		Event::report(AES_CDH_variableFault, "VARIABLE PROBLEM",
				"Retrieval of variable (" + string(AES_CDH_AesDataDirectory) +
				") from GCC failed.", "-");
	}

	return false;
}

/*===================================================================
   ROUTINE:makeDataBaseDestPath
=================================================================== */
void TransDest::makeDataBaseDestPath(string &dbPath, const string &aesDataDir,
		const string &cdhRootDir, const string &dest)
{
	dbPath.assign(aesDataDir);
	dbPath.append("/");
	dbPath.append(cdhRootDir);
	dbPath.append("/");
	dbPath.append(ServR::getDataBaseDirectory());
	dbPath.append("/");
	dbPath.append(dest);
	AES_CDH_LOG(LOG_LEVEL_INFO, "DataBaseDest path for dest %s is %s",dest.c_str(),dbPath.c_str());
	AES_CDH_TRACE_MESSAGE("dbPath = %s", dbPath.c_str());
}


/*===================================================================
   ROUTINE:removeDataBaseDestDir
=================================================================== */
void TransDest::removeDataBaseDestDir(const string &dbPath)
{
	AES_CDH_LOG(LOG_LEVEL_INFO, "Remove databse path %s",dbPath.c_str());
	CDHCriticalSection::instance().enter();	
	DataBase::instance()->destroyDestinationEntry(dest_);
	CDHCriticalSection::instance().leave();

	if (ACE_OS::rmdir(dbPath.c_str()) == 0)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(dbPath,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Directory name sent for event: %s",newdr.c_str());
		}
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				ServR::NTErrorText(), "Failed to remove database directory: "
				+ newdr);
	}
}

/*===================================================================
   ROUTINE:handleMessage
=================================================================== */
void TransDest::handleMessage(setEvMsg *data)
{
	// bool doCheckConnection = false;
	//AES_Command sendMsg;
	ACS_APGCC_Command sendMsg;

	switch(data->cmdC)
	{
	case TRDMSG_DELETE:
	{
		// Destination deleted. Remove any existing alarms
		AES_CDH_LOG(LOG_LEVEL_INFO, "TRDMSG_DELETE");
		exitThread_ = true;
	}
	break;

	case TRDMSG_CHECKCON:
	{
		// Check connection
		connectionResult_ = checkConnection();
	}
	break;
	//HI58671
	case TRDMSG_CHECKDESTAVAIL:
	{
		// Check Availabillity of Destinations..
		DestAvailResult_ = checkDestinations();
	}
	break;
	//HI58671
	default:
		break;

	} // switch
}
/*===================================================================
   ROUTINE:checkDestinations
=================================================================== */
AES_CDH_ResultCode TransDest::checkDestinations()
{
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	if (tmpPtransForCheckConnection_ != NULL)
	{
		// check that traffic is still running
		if (tmpPtransForCheckConnection_->dataTransfer())
		{
			rCode = tmpPtransForCheckConnection_->checkConnection();
		}
		else
		{
			//delete tmpPtransForCheckConnection_;
			tmpPtransForCheckConnection_ = NULL;
			rCode = ptransForCheckConnection_->checkConnection();
		}
	}
	else
	{
		rCode = ptransForCheckConnection_->checkConnection();
	}
	return rCode;
}

/*===================================================================
   ROUTINE:GetDestAvailable
=================================================================== */
AES_CDH_ResultCode TransDest::GetDestAvailable()
{
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	rCode = DestAvailResult_;
	//AES_CDH_LOG(LOG_LEVEL_DEBUG,"(%t) AES_CDH_transdest::GetDestAvailable() The return code: %u\n", rCode));
	return rCode;
}

/*===================================================================
   ROUTINE:checkConnection
=================================================================== */
AES_CDH_ResultCode TransDest::checkConnection(char setAlarm)
{
	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	AES_CDH_TRACE_MESSAGE("destination: %s",dest_.c_str());
	checkConnMutex.acquire();

	if (tmpPtransForCheckConnection_ != NULL)
	{
		// check that traffic is still running
		if (tmpPtransForCheckConnection_->dataTransfer())
		{
			AES_CDH_TRACE_MESSAGE("Step 1");
			rCode = tmpPtransForCheckConnection_->checkConnection();
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Step 2");
			//delete tmpPtransForCheckConnection_;
			tmpPtransForCheckConnection_ = NULL;
			rCode = ptransForCheckConnection_->checkConnection();
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Step 3");
		rCode = ptransForCheckConnection_->checkConnection();
	}

	if (rCode == AES_CDH_RC_OK)
	{
		AES_CDH_TRACE_MESSAGE("Return code == AES_CDH_RC_OK");
		rCode = AES_CDH_RC_CONNECTOK;

		if(setAlarm == ENABLE)  // TR HX18084
		{
			// cease alarm EXCEPT with write error
			alarmCause = DestinationAlarm::AlarmRecord::write_error;
			DestinationAlarm::instance()->cease(dest_, destSetName_, alarmCause);
		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Return code is %u", rCode);
		AES_CDH_TRACE_MESSAGE("return code is %u", rCode);

		if(setAlarm == ENABLE)  // TR HX18084
		{
			// cease alarm EXCEPT with connect error 
			if(ServR::isStopEventSignalled == false)
			{
				alarmCause = DestinationAlarm::AlarmRecord::connect_error;
				DestinationAlarm::instance()->cease(dest_, destSetName_, alarmCause);
				// raise alarm with connect error
				DestinationAlarm::instance()->raise(dest_, destSetName_, alarmCause);
			}
		}
	}
	checkConnMutex.release();

	return rCode;
}

/*===================================================================
ROUTINE:shutDown
=================================================================== */
void TransDest::shutDown(void)
{
	setEvMsg *aMsg = new(setEvMsg);
	aMsg->cmdC = TRDMSG_DELETE;
	ACE_Message_Block* mb = new ACE_Message_Block((ACE_TCHAR*)aMsg,sizeof(*aMsg));
	putq(mb);
	this->wait();
}

/*===================================================================
   ROUTINE:checkCon
=================================================================== */
void TransDest::checkCon(void)
{
	setEvMsg *aMsg = new(setEvMsg);
	aMsg->cmdC = TRDMSG_CHECKCON;
	ACE_Message_Block* mb = new ACE_Message_Block((ACE_TCHAR*)aMsg,sizeof(*aMsg));
	putq(mb);
}

/*===================================================================
   ROUTINE:CheckDestAvailable
=================================================================== */
void TransDest::CheckDestAvailable(void)
{
	setEvMsg *aMsg = new(setEvMsg);
	aMsg->cmdC = TRDMSG_CHECKDESTAVAIL;
	ACE_Message_Block* mb = new ACE_Message_Block((ACE_TCHAR*)aMsg,sizeof(*aMsg));
	putq(mb);
}

/*===================================================================
   ROUTINE:getUserGroup
=================================================================== */
const string TransDest::getUserGroup() const
{
	return usergrp_;
}

/*===================================================================
   ROUTINE:switchAgent
=================================================================== */
void TransDest::switchAgent(bool stopTransfer)
{
	ptrans_->switchAgent(stopTransfer);
}


/*===================================================================
   ROUTINE:StartResendNotification
=================================================================== */
AES_CDH_ResultCode TransDest::StartResendNotification(void)
{
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;

	resNotifMX_.acquire();

	if((resendThreadManager_ != NULL) || (presendnotif_ != NULL))
	{
		rcode = AES_CDH_RC_INTPROGERR;
	}

	if(rcode == AES_CDH_RC_OK)
	{
		resendThreadManager_ = new ACE_Thread_Manager;
		presendnotif_        = new ResendNotification;

		if ((presendnotif_ == NULL) || (resendThreadManager_ == NULL))
		{
			Event::report(AES_CDH_memoryFault, "MEMORY ALLOCATION PROBLEM",
					"Storage space for the \"Resend File Notification Thread\" could not be allocated at server start-up",
					"-");
			rcode = AES_CDH_RC_INTPROGERR;
		}
		else
		{
			rcode = presendnotif_->define();

			if (rcode == AES_CDH_RC_OK)
			{
				ACE_INT32 err = presendnotif_->open(resendThreadManager_);
				if (err == -1)
				{
					// The notification thread did not start
					rcode = AES_CDH_RC_INTPROGERR;
					delete presendnotif_;
				}
				else
				{
					resendNotificationRunning_ = true;
				}
			}
		} // else
	}
	resNotifMX_.release();
	return rcode;
}

/*===================================================================
   ROUTINE:StopResendNotification
=================================================================== */
void TransDest::StopResendNotification(void)
{
	resNotifMX_.acquire();
	numberOfNotificationDestinations_--;

	if((numberOfNotificationDestinations_ < 1) && (resendNotificationRunning_ == true))
	{
		if (presendnotif_ != NULL)
		{
			MessageBlock *mb = new MessageBlock(MT_HANGUP);
			(presendnotif_->msg_queue())->enqueue_head(mb);
			resendThreadManager_->wait();
			resendNotificationRunning_ = false;
			delete presendnotif_;
			presendnotif_ = NULL;

			if(resendThreadManager_ != NULL)
			{
				delete resendThreadManager_;
				resendThreadManager_ = NULL;
			}
		}
	}
	resNotifMX_.release();
}

void TransDest::getDestinationRdn( string &destRdn )
{
	//Copy the destination rdn to destRdn.
	//It will be of type primaryInitiatingDestinationId..
	destRdn = m_destRdn;
}
