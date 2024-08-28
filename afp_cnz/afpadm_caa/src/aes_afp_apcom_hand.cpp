/*=================================================================== */
/**
	@file   aes_afp_apcom_hand.cpp

	@brief

	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       08/09/2011     XNADNAR       Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "aes_afp_apcom_serv.h"
#include "aes_afp_apcom_hand.h"
#include <aes_ohi_errorcodes.h>

#include <ace/ACE.h>
#include <ace/Task.h>
#include <ace/Message_Queue.h>
#include <aes_afp_services.h>

#include <sstream>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"

#include <ACS_DSD_Session.h>
AES_AFP_TRACE_DEFINE(AES_AFP_apcom_hand);
/*===================================================================
                       DEFINE DECLARATION SECTION
=================================================================== */
#define DSD_MAX_BUFFER  64000

/*===================================================================
					   ROUTINE: CONSTRUCTOR
=================================================================== */
aes_afp_apcom_hand::aes_afp_apcom_hand()
: motherTask_(NULL),applAttached_(false),
  nameOfDatatask_("datatask"),
  myThreadName_(""),
  attachedFileDest_(""),
  EMPTYTEMPLATE_(""),convSession_(NULL)
{

}
/*===================================================================
					   ROUTINE: CONSTRUCTOR
=================================================================== */
aes_afp_apcom_hand::aes_afp_apcom_hand (aes_afp_apcom_serv *mother)
: applAttached_(false),
  nameOfDatatask_("datatask"),
  myThreadName_(""),
  attachedFileDest_(""),
  EMPTYTEMPLATE_(""),convSession_(NULL)
{
	motherTask_ = mother;
}

/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
aes_afp_apcom_hand::~aes_afp_apcom_hand()
{

}
/*===================================================================
                    ROUTINE: open
=================================================================== */
int aes_afp_apcom_hand::open (ACS_DSD_Session * sess, ACE_Thread_Manager* thrMgr)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	convSession_ = sess;
	myThreadName_ = "apcom_hand";
	this->thr_mgr( thrMgr );
	AES_AFP_TRACE_MESSAGE("Leaving");
	return this->activate(THR_NEW_LWP | THR_DETACHED);
}

/*===================================================================
                    ROUTINE: close
=================================================================== */
int aes_afp_apcom_hand::close (unsigned long flags)
{
	(void)(flags);
	AES_AFP_TRACE_MESSAGE("Entering, flags = %lu ",flags);

	if (convSession_ != 0)
	{
		AES_AFP_TRACE_MESSAGE("Before acquire");
		// close conversation with client
		aes_afp_apcom_serv::afpMutex.acquire();
		AES_AFP_TRACE_MESSAGE("After acquire");
		aes_afp_dsdsessionmap::Instance().remove(attachedFileDest_);
		convSession_->close();
		delete convSession_;
		convSession_ = 0;
		AES_AFP_TRACE_MESSAGE("convSession_ closed");
		try
		{
			// try to erase me from parents map
			std::map<std::string, aes_afp_apcom_hand*>::iterator itr;
			char stname[32];
			//ACE_OS::sprintf(stname, "HAND%X", (ACE_UINT32*)this);
			ACE_OS::sprintf(stname, "HAND%p", this);

			itr = motherTask_->handMap_.find(stname);
			if (itr == motherTask_->handMap_.end() )
			{
				// did not find myself in map!?!?
				AES_AFP_LOG(LOG_LEVEL_INFO, "Could not find myself");
				AES_AFP_TRACE_MESSAGE("Could not find myself");
			}
			else
			{
				motherTask_->handMap_.erase(itr);		// erase me from map
			}
		}catch (...)
		{}
		aes_afp_apcom_serv::afpMutex.release();
		AES_AFP_TRACE_MESSAGE("mutex released");
	}
	else
	{

		AES_AFP_TRACE_MESSAGE("Before acquire , remove");
		aes_afp_apcom_serv::afpMutex.acquire();
		AES_AFP_TRACE_MESSAGE("After acquire , remove");
		aes_afp_dsdsessionmap::Instance().remove(attachedFileDest_);
		aes_afp_apcom_serv::afpMutex.release();

		//convSession_ = 0;
	}

	delete this;	// must be last action!
	AES_AFP_TRACE_MESSAGE("Leaving ");

	return 0;
}
/*===================================================================
                    ROUTINE: svc
=================================================================== */
int aes_afp_apcom_hand::svc (void )
{
	AES_AFP_TRACE_MESSAGE("Entering");
	char buf[DSD_MAX_BUFFER +1];
	size_t msglen(DSD_MAX_BUFFER);
	aes_afp_msgblock* mb;
	aes_afp_msgblock* answerMsg;
	ACE_Message_Block* aceMessage;
	aes_afp_datablock* db;
	aes_afp_datablock* answerData = 0;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	aes_afp_msgtypes typeOfMsg;
	AES_GCC_Errorcodes errCode;

	int msgtype(0);
	int cmdType(0);
	int data(0);
	int dataContent(0);
	int errorCode(0);
	void* dataTaskId;

	std::string tmpStr("");
	std::string dbBuff("");
	unsigned short startPos(0);
	int search(0);
	int result(0);
	bool hasdata(false);
	bool infoList(false);

	// get datatask identity. this is where we will be sending data
	nameOfDatatask_ = "datatask";
	dataTaskId = aes_afp_threadmap::Instance().searchId(nameOfDatatask_);
	AES_AFP_TRACE_MESSAGE("Name of datatask = %s, id = %ld", nameOfDatatask_.c_str(), (long int)dataTaskId);
	if(dataTaskId == 0)
		return -1;

	while (1)
	{
		// receive message block from client
		char recvBuf[DSD_MAX_BUFFER];
		ACE_OS::memset(recvBuf, 0, sizeof(char)*DSD_MAX_BUFFER );
		msglen = DSD_MAX_BUFFER;
		result = convSession_->recv(&recvBuf, msglen, 2000);
		if (result <= 0) 
		{
			if( -8 == result )
			{
				if( aes_afp_services::afpStopEvt == true )
				{
					return 0;
				}
				continue;
			}
			else if (-43 == result)
			{
				AES_AFP_TRACE_MESSAGE("Received %d bytes from DSD, result = %d", result, convSession_->last_error());
				AES_AFP_TRACE_MESSAGE("entering for peer closed ");
				// TR HT21805 - To avoid clearing of mb and db incase of dataTaskId = 0
				dataTaskId = aes_afp_threadmap::Instance().searchId(nameOfDatatask_);
				if( 0 == dataTaskId )
				{
					AES_AFP_LOG(LOG_LEVEL_INFO,"There is no datatask to send and closing.");
					AES_AFP_TRACE_MESSAGE("There is no datatask to send and closing."); 
					return -1;
				}

				db = new(std::nothrow) aes_afp_datablock;
				if( 0 == db )
				{
					AES_AFP_LOG(LOG_LEVEL_FATAL,"Memory allocation failed for db ");
					AES_AFP_TRACE_MESSAGE("Memory allocation failed for db ");
					return -1;
				}
				db->setMsgType((aes_afp_msgtypes)CLOSETRANSFERQUEUE);
				db->setTransferQueue(attachedFileDest_);
				db->setErrorCode((AES_GCC_Errorcodes)errorCode);
				mb = new(std::nothrow) aes_afp_msgblock(sizeof(*db), db);
				if(0 == mb)
				{
					AES_AFP_LOG(LOG_LEVEL_FATAL,"Memory allocation failed for mb ");
					AES_AFP_TRACE_MESSAGE("Memory allocation failed for mb ");
					delete db;
					db = 0;
					return -1;
				}
				mb->setCallerId(this);
				mb->setCallerName(myThreadName_);
				mb->setMsgType((aes_afp_msgtypes)CLOSETRANSFERQUEUE);
				sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)dataTaskId;
				AES_AFP_TRACE_MESSAGE("Before putq");
				sendQueue->putq(mb);
				AES_AFP_TRACE_MESSAGE("Before getq");
				this->getq(aceMessage);
				applAttached_=false;
				aceMessage->release();	
				return 0;
			}
			return -1;
		}

		db = new(std::nothrow) aes_afp_datablock;
		if(0 == db )
		{
			AES_AFP_LOG(LOG_LEVEL_FATAL,"Memory allocation failed for db ");
			AES_AFP_TRACE_MESSAGE("Memory allocation failed for db ");
			return -1;
		}


		tmpStr = &recvBuf[4]; // skip first 4 bytes (header)
		AES_AFP_TRACE_MESSAGE("Msglen %d", result);
		startPos = 0;

		//---------------------------------------------------------
		// END FLAG, should always be one at this time
		//---------------------------------------------------------
		search = tmpStr.find_first_of("\n");
		if (search > 0)
		{
			tmpStr[search] = ';';
		}
		// get END FLAG. ascii string
		dbBuff = tmpStr.substr(startPos, search-startPos);
		ACE_OS::atoi(dbBuff.c_str());
		startPos = search+1;

		//---------------------------------------------------------
		// MESSAGE KIND
		//---------------------------------------------------------
		search = tmpStr.find_first_of("\n");
		if (search > 0)
		{
			tmpStr[search] = ';';
		}
		dbBuff = tmpStr.substr(startPos, search-startPos);
		cmdType = ACE_OS::atoi(dbBuff.c_str());
		if (cmdType != COMMAND)
		{
			// Not a command, something is wrong
		}

		startPos = search+1;

		//---------------------------------------------------------
		// MESSAGE TYPE
		//---------------------------------------------------------
		search = tmpStr.find_first_of("\n");
		if (search>0)
		{
			tmpStr[search] = ';';
		}
		dbBuff = tmpStr.substr(startPos, search-startPos);
		msgtype = ACE_OS::atoi(dbBuff.c_str());
		if (msgtype == CLOSETRANSFERQUEUE && applAttached_ == false)
		{
//			AES_AFP_LOG(LOG_LEVEL_INFO, "CLOSETRANSFERQUEUE && attached FALSE");
			AES_AFP_TRACE_MESSAGE("CLOSETRANSFERQUEUE && attached FALSE");
			char tmpBuf[DSD_MAX_BUFFER];
			delete db;
			ACE_OS::sprintf(tmpBuf, "%s%d\n%d\n%d\n%d\n%d\n","rpch",1,COMMAND,OK,AES_NOERRORCODE_DETACH,0);
			msglen = ACE_OS::strlen(tmpBuf);

			aes_afp_apcom_serv::afpMutex.acquire();
			unsigned int tv = 10000;
			result = convSession_->send((void*)tmpBuf, msglen , tv);
			aes_afp_apcom_serv::afpMutex.release();
			return 0;
		}
		db->setMsgType((aes_afp_msgtypes)msgtype);
		startPos = search+1;

		//---------------------------------------------------------
		// ERROR CODE
		//---------------------------------------------------------
		search = tmpStr.find_first_of("\n");
		if (search > 0)
		{
			tmpStr[search] = ';';
		}
		dbBuff = tmpStr.substr(startPos, search-startPos);
		errorCode = ACE_OS::atoi(dbBuff.c_str());
		db->setErrorCode((AES_GCC_Errorcodes)errorCode);
		startPos = search+1;

		//---------------------------------------------------------
		// DATA
		//---------------------------------------------------------
		search = tmpStr.find_first_of("\n");
		if (search>0)
		{
			tmpStr[search] = ';';
		}
		dbBuff = tmpStr.substr(startPos, search-startPos);
		dataContent = ACE_OS::atoi(dbBuff.c_str());
		startPos = search+1;

		if (dataContent == 1)
		{
			std::string str = tmpStr.substr(startPos);
			decodeDataPart(str, startPos, db);
			mb = new aes_afp_msgblock(sizeof(db), db);
		}
		else
		{
			mb = new aes_afp_msgblock(0, 0);
		}

		// compose message block
		mb->setCallerId(this);
		mb->setCallerName(myThreadName_);
		mb->setMsgType((aes_afp_msgtypes)msgtype);

		dataTaskId = aes_afp_threadmap::Instance().searchId(nameOfDatatask_);
		if(dataTaskId == 0)
		{
			if (db)
			{
				delete db;
				db = 0;
			}
			return -1;
		}
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)dataTaskId;
		AES_AFP_TRACE_MESSAGE("before putq");
		sendQueue->putq(mb);

		AES_AFP_TRACE_MESSAGE("before getq");
		this->getq(aceMessage);

		AES_AFP_TRACE_MESSAGE("after getq");
		// convert and decode message
		std::stringstream str;
		std::string tmpListString("");
		answerMsg = (aes_afp_msgblock*)aceMessage;
		AES_AFP_TRACE_MESSAGE("before answerMsg->getMsgType");
		typeOfMsg = answerMsg->getMsgType();

		AES_AFP_TRACE_MESSAGE("after answerMsg->getMsgType");
		if (typeOfMsg == HANGUP)
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "Msg HANGUP received");
			AES_AFP_TRACE_MESSAGE("Msg HANGUP received");
			if (dataContent != 1)
			{
				delete db;
				db = 0;
			}
			aceMessage->release();
			return 0;
		}

		errCode = answerMsg->getErrorCode();
		hasdata = answerMsg->hasData();
		AES_AFP_TRACE_MESSAGE("Type of msg = %d, error code = %d, data content = %d ", typeOfMsg, errCode, hasdata);
		AES_AFP_TRACE_MESSAGE("Msg HANGUP received");
		// compose answer to caller client
		if (hasdata == true)
		{
			answerData = answerMsg->getData();
		}

		switch (typeOfMsg)
		{
		case OK:
		{
			data = 0;
			break;
		}
		case NOTOK:
		{
			data = 0;
			break;
		}

                // Get status for a send item
                case GETSTATUSFILE:
                case GETSTATUSOLDFILE:
                case GETSTATUSNEWFILE:
                case GETSTATUSNEXTFILE:
                {
                        data = 1;
                        str << answerData->getStatus() << std::endl;
                        str << answerData->getOriginalFileName()<< std::endl;             //HU96961
			str << answerData->getDestinationSet() << std::endl;
			str << answerData->getReadyDate() << std::endl;
			str << answerData->getDeleteDate() << std::endl;
			str << answerData->getSendDate() << std::endl;
			str << answerData->getStoppedDate() << std::endl;
			str << answerData->getFailedDate() << std::endl;
			str << answerData->getArchiveDate() << std::endl;

			break;
		}

		// Get attributes for a process order
		case GETTRANSFERQUEUEATTRIBUTES:
		{
			data = 1;
			str << answerData->getTransferQueue()			<< std::endl;
			str << answerData->getUserGroup() << std::endl;
			str << answerData->getDestinationSet()		<< std::endl;
			str << answerData->getStatus()				<< std::endl;
			str << (answerData->getRemoveBefore() ? "1" : "0") << std::endl;
			str << answerData->getRemoveDelay()		<< std::endl;
			str << answerData->getSendRetries()		<< std::endl;
			str << answerData->getRetriesDelay()	<< std::endl;
			str << (answerData->getManual() ? "1" : "0") << std::endl;

			if (answerData->getFileRename() == AES_LOCAL)
			{
				str << "0" << std::endl;
			}
			if (answerData->getFileRename() == AES_REMOTE)
			{
				str << "1" << std::endl;
			}
			if (answerData->getFileRename() == AES_NONE)
			{
				str << "2" << std::endl;
			}

			str << answerData->getSourceDirectory() << std::endl;
			str << answerData->getFileTemplate() << std::endl;
			str << answerData->getNameTag() << std::endl;
			str << answerData->getStartupSequenceNumber() << std::endl;
			break;
		}

		// Set attributes for a process order
		case SETTRANSFERQUEUEATTRIBUTES:
		{
			data = 0;
			break;
		}

		case ISTRANSFERQUEUELOCKED:
		{
			data = 1;
			if (answerData->getLock() == true)
			{
				str << "1" << std::endl;
			}
			else
			{
				str << "0" << std::endl;
			}
			str << answerData->getUsername();
			break;
		}
		case GETSOURCEDIRECTORYPATH:
		{
			data = 1;
			str << answerData->getSourceDirectory() <<std::endl;
			break;
		}
		case GETLASTREPORTEDFILE:
		{
			data = 1;
			str << answerData->getStrBuf();
			break;
		}
		case GETDESTINATIONSETLIST:
		{
			data = 1;
			str << answerData->getStrBuf();
			break;
		}
		case GETFILESTATUS:
		{
			data = 1;
			str << answerData->getStatus() << std::endl;
			break;
		}

		case INFODESTINATIONSET:
		{
			data = 1;
			str << answerData->getStrBuf();
			break;
		}
		// It must be info/listing then...
		default:
		{
			infoList = true;
			data = 1;
			tmpListString = answerData->getListString();
			break;
		}
		} // end switch

		int len(0);
		int endFlag(0);
		char slaskbuf[2048];
		char tmpbuff[DSD_MAX_BUFFER];
		int tmplen(0);
		int totlen(0);
		bool firstBuffer(true);

		// make a test header to see what size is will be...
		ACE_OS::sprintf(slaskbuf,
				"%s%d\n%d\n%d\n%d\n%d\n",
				"rpch", endFlag, COMMAND, typeOfMsg, errCode, data);

		int slasklen = ACE_OS::strlen(slaskbuf);
		if (infoList == true)
		{
			totlen = tmpListString.length();
		}
		else
		{
			totlen = sizeof(str.str());
		}

		AES_AFP_TRACE_MESSAGE("sending  stream data in 64K chunks");
		// Send stream data in 64K chunks
		while (endFlag == 0)
		{
			if (totlen > DSD_MAX_BUFFER-slasklen)
			{
				endFlag = 0;
			}
			else
			{
				endFlag = 1;
			}

			if (firstBuffer == true)
			{
				// RPC header 0=more buffers ahead, 1=last buffer, message is a command, message type, error code, 0 if no data, 1 if data
				ACE_OS::sprintf(buf,
						"%s%d\n%d\n%d\n%d\n%d\n",
						"rpch", endFlag,	COMMAND, typeOfMsg,	errCode, data);
				firstBuffer = false;
			}
			else
			{
				ACE_OS::sprintf(buf, "%s%d\n",
						"rpch", endFlag);
			}

			len = ACE_OS::strlen(buf);

			if (infoList == true)
			{
				std::string tmpStr("");
				tmpStr = tmpListString.substr(0, DSD_MAX_BUFFER-len);
				ACE_OS::strcpy(tmpbuff, tmpStr.c_str() );
				tmpListString.erase(0, DSD_MAX_BUFFER-len);

			}
			else
			{
				str.get(tmpbuff, DSD_MAX_BUFFER-len, '\0');
			}

			tmplen = ACE_OS::strlen(tmpbuff);

			totlen = totlen - tmplen;

			// if buf is set to exactly the size of DSD_MAX_BUFFER, strcat will cause
			// overwriting of the dsd session object.
			ACE_OS::strcat(buf, tmpbuff);

			len = ACE_OS::strlen(buf);

			// check length of buffer
			msglen = ACE_OS::strlen(buf);

			// send buffer to client
			AES_AFP_TRACE_MESSAGE("sendMsg  %d %d %d %d %d", buf[0],buf[1],buf[2],buf[3],buf[4]);

			aes_afp_apcom_serv::afpMutex.acquire();
			unsigned int  tv = 10000;
			result = convSession_->send((void*)buf, msglen, tv);
			aes_afp_apcom_serv::afpMutex.release();

			if (result <= 0)
			{
				AES_AFP_LOG(LOG_LEVEL_INFO, "Could not send msg to client, error = %d, error text = %s",
						convSession_->last_error(), convSession_->last_error_text());
				AES_AFP_TRACE_MESSAGE("Could not send msg to client, error = %d, error text = %s",
						convSession_->last_error(), convSession_->last_error_text());

				aceMessage->release();
				if (dataContent != 1)
					delete db;
				return -1;
			}
			else
			{
				if (endFlag == 0)
				{
					//usleep(100000);	// Small delay so we not choke dsd...
					struct timespec wait;
					wait.tv_sec=100000/(1000*1000);
					wait.tv_nsec=(100000%(1000*1000))*1000;
					nanosleep(&wait, NULL);
				}
			}
		} // end while


		infoList = false;
		// dispose of ace messageclass
		firstBuffer = true;
		aceMessage->release();
	} // while(1)

	AES_AFP_TRACE_MESSAGE("Leaving");
	return 0;

}

/*===================================================================
                    ROUTINE: decodeDataPart
=================================================================== */
bool aes_afp_apcom_hand::decodeDataPart (std::string& dataBuffer, unsigned short& startPosition, aes_afp_datablock* dataBlock)
{
	AES_AFP_TRACE_MESSAGE("dataBuffer = %s, startPosition = %d " , dataBuffer.c_str(),startPosition);
	std::string dbBuff;
	std::string tmpToUpper;

	protocol_.clearValues();

	protocol_.setString(dataBuffer);

	AES_AFP_TRACE_MESSAGE("msgType = %d", dataBlock->getMsgType());
	if (applAttached_ == true)
	{
		dataBlock->setTransferQueue(attachedFileDest_);
		if (dataBlock->getMsgType() == CLOSETRANSFERQUEUE)
		{
//			AES_AFP_LOG(LOG_LEVEL_INFO, "CLOSETRANSFERQUEUE");
			AES_AFP_TRACE_MESSAGE("CLOSETRANSFERQUEUE");
			applAttached_ = false;
		}
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("Appl not attched");
		dataBlock->setTransferQueue(protocol_.getStringValue("TRANSFERQUEUE") );

		if (dataBlock->getMsgType() == OPENTRANSFERQUEUE)
		{
//			AES_AFP_LOG(LOG_LEVEL_INFO, "OPENTRANSFERQUEUE");
			AES_AFP_TRACE_MESSAGE("OPENTRANSFERQUEUE");
			applAttached_ = true;
			attachedFileDest_ = protocol_.getStringValue("TRANSFERQUEUE");

			aes_afp_apcom_serv::afpMutex.acquire();
			aes_afp_dsdsessionmap::Instance().insert(attachedFileDest_, convSession_);
			aes_afp_apcom_serv::afpMutex.release();
			dataBlock->setDsdChannel(convSession_);
		}
	}

	dataBlock->setUsername(protocol_.getStringValue("USERNAME") );
	dataBlock->setDestinationSet(protocol_.getStringValue("DESTINATIONSET") );
	dataBlock->setFilename(protocol_.getStringValue("FILENAME") );
	dataBlock->setSourceDirectory(protocol_.getStringValue("SOURCEDIRECTORY") );
	dataBlock->setUserGroup(protocol_.getStringValue("USERGROUP") );
	dataBlock->setStatus( (AES_GCC_Filestates)protocol_.getIntValue("STATUS") );
	dataBlock->setRemoveDelay(protocol_.getIntValue("REMOVEDELAY") );

	if (protocol_.getBoolValue("REMOVEBEFORE") == true)
	{
		dataBlock->setRemoveBefore();
	}
	else
	{
		dataBlock->unsetRemoveBefore();
	}

	dataBlock->setSendRetries(protocol_.getIntValue("SENDRETRIES") );
	dataBlock->setRetriesDelay(protocol_.getIntValue("SENDRETRIESDELAY") );

	if (protocol_.getBoolValue("MANUALINITIATED") == true)
	{
		dataBlock->setManual();
	}

	dataBlock->setRangeBegin(protocol_.getStringValue("RANGEBEGIN") );
	dataBlock->setRangeBegin(protocol_.getStringValue("RANGEEND") );
	dataBlock->setNewDestinationSet(protocol_.getStringValue("NEWDESTINATIONSET") );
	dataBlock->setFileRename((AES_AFP_Renametypes)protocol_.getIntValue("RENAMETYPE") );
	dataBlock->setFileTemplate(protocol_.getStringValue("RENAMETEMPLATE") );
	dataBlock->setFormat((AES_GCC_Format)protocol_.getIntValue("TRANSFERMODE") );
	dataBlock->setMask(protocol_.getStringValue("TRANSFERMASK") );

	if (protocol_.getBoolValue("ISDIRECTORY") == true)
	{
		dataBlock->setDirectoryFlag();
	}

	dataBlock->setApplication(protocol_.getStringValue("APPLICATION") );
	dataBlock->setNameTag(protocol_.getStringValue("NAMETAG") );
	// uabmha: CNI 1135, call needed for afpdef -k option
	dataBlock->setStartupSequenceNumber(protocol_.getIntValue("STARTUPSEQUENCENUMBER") );

	return 0;
}
/*===================================================================
                    ROUTINE: toUpper
=================================================================== */
char* aes_afp_apcom_hand::toUpper (char* str)
{
	int i = 0;

	while (str[i])
	{
		str[i] = toupper(str[i]);
		i++;
	}

	return str;
}

/*===================================================================
                    ROUTINE: toUpper
=================================================================== */
void aes_afp_apcom_hand::toUpper (std::string& str)
{
	std::string::iterator itr;

	for (itr = str.begin();itr != str.end(); ++itr/*++*/)
	{
		(*itr) = toupper((*itr) );
	}

}
