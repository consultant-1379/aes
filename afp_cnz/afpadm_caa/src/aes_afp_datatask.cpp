#include <aes_afp_datatask.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(AES_AFP_datatask);

aes_afp_datatask::aes_afp_datatask()
: savedCallerId_(NULL),savedCdhId_(NULL),messageCounter_(0),
  stop(false),
  apComObj_(NULL),
  nameOfMyThread_("datatask"),
  m_initialized(false)
{
	nameOftheClient ="";
}


aes_afp_datatask::~aes_afp_datatask()
{
	this->wait();
}


int aes_afp_datatask::open (void* args, ACE_Thread_Manager* thrMgr)
{
	(void)args;
	AES_AFP_TRACE_FUNCTION;
	this->thr_mgr(thrMgr);
	return this->activate(THR_NEW_LWP | THR_JOINABLE);
}

int aes_afp_datatask::close (unsigned long flags)
{
	AES_AFP_TRACE_MESSAGE(" flags %ld", flags);
	return 0;
}

bool isTQReferred(string tqName, string className)
{
	OmHandler omHandlerObj;
	std::vector<std::string> stsDnList;
	std::vector<std::string>::iterator it_dnList;

	if( omHandlerObj.Init() == ACS_CC_FAILURE )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occurred while initializing OmHandler");
		AES_AFP_TRACE_MESSAGE("Error occurred while initializing OmHandler");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return false;
	}

	omHandlerObj.getClassInstances(className.c_str(), stsDnList);
	AES_AFP_TRACE_MESSAGE("Instances found for %s = %d", className.c_str(), (int)stsDnList.size());
	ACS_CC_ImmParameter paramToFind;
	paramToFind.attrName = new char [50];
	ACE_OS::strcpy(paramToFind.attrName,"transferQueue");
	string tmpString;
	for(it_dnList=stsDnList.begin(); it_dnList != stsDnList.end(); ++it_dnList)
	{
		if(omHandlerObj.getAttribute((*it_dnList).c_str(),&paramToFind) == ACS_CC_SUCCESS)
		{
			AES_AFP_TRACE_MESSAGE("getAttribute() for %s, attrValuesNum = %d",(*it_dnList).c_str(),paramToFind.attrValuesNum);
			//if thr is a clinet job with empty tq
			if ( paramToFind.attrValuesNum > 0 && ACE_OS::strcmp(reinterpret_cast<char *>(paramToFind.attrValues[0]), "") != 0)
			{
				if (ACE_OS::strcmp(reinterpret_cast<char *>(paramToFind.attrValues[0]), tqName.c_str()) == 0)
				{
					delete[] paramToFind.attrName;
					omHandlerObj.Finalize();
					return true;
				}
			}
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Error occurred while getAttribute() for %s",(*it_dnList).c_str());
			AES_AFP_TRACE_MESSAGE("Error occurred while getAttribute() for %s",(*it_dnList).c_str());
		}
	}
	delete[] paramToFind.attrName;
	omHandlerObj.Finalize();
	return false;
}


bool aes_afp_datatask::loadTQsDataBase()
{
	AES_AFP_TRACE_FUNCTION;
	AES_GCC_Errorcodes code = AES_NOERRORCODE;
	bool result = dBase_.init(code);

	if (result)
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "########## TQ database loaded :-)");
		AES_AFP_TRACE_MESSAGE("########## TQ database loaded :-)");
		m_initialized = true;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to initiate database");
		AES_AFP_TRACE_MESSAGE("Unable to initiate database");
	}

	return result;
}

int aes_afp_datatask::svc (void )
{
	ACE_Message_Block* aceMessage;
	aes_afp_msgblock* afpMessage;
	aes_afp_datablock* db;
	AES_GCC_Errorcodes code;

	aes_afp_threadmap::Instance().insert(nameOfMyThread_, this);
	if(stop)
	{
		handle_shutdown();
		aes_afp_threadmap::Instance().remove(nameOfMyThread_);
		return 0;
	}

	for (;;)
	{
		this->getq(aceMessage);
		afpMessage = (aes_afp_msgblock*)aceMessage;
		AES_AFP_TRACE_MESSAGE("Recieved message = %d", afpMessage->getMsgType() );

		if(stop)
		{
			handle_shutdown();
			aceMessage->release();
			aes_afp_threadmap::Instance().remove(nameOfMyThread_);
			return 0;
		}

		switch (afpMessage->getMsgType())
		{
		case HANGUP:
		{
			handle_shutdown();
			aceMessage->release();
			aes_afp_threadmap::Instance().remove(nameOfMyThread_);
			AES_AFP_LOG(LOG_LEVEL_INFO, "aes_afp_datatask thread shutdown, Done!! " );
			return 0;
		}
		case REMOVETRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_removeTransferQueue(db,code);

			aceMessage->release();

			break;
		}
		case FINDTRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_findTransferQueue(db);

			aceMessage->release();
			break;
		}
		case INFOONETRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_infoOneTransferQueue(db);

			aceMessage->release();
			break;
		}
		case INFOALLTRANSFERQUEUES:
		{
			int size;
			size= msg_queue()->message_count();
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_infoAllTransferQueues(db);

			aceMessage->release();
			break;
		}
		case INFOALLTRANSFERQUEUESSHORT:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_infoAllTransferQueuesShort(db);

			aceMessage->release();
			break;
		}

		case REMOVEFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_removeFile(db,code);

			aceMessage->release();
			break;
		}
		case REMOVEFILEALLDESTINATIONSETS:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_removeFile(db,code);

			aceMessage->release();
			break;
		}
		case REMOVEEXACTFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_removeFile(db,code);

			aceMessage->release();
			break;
		}
		case CREATEFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_createFile(db);

			aceMessage->release();
			break;
		}
		case SETFILEARCHIVE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_statusarchive(db);

			aceMessage->release();
			break;
		}
		case GETSTATUSFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_getStatusFile(db);

			aceMessage->release();
			break;
		}
		case GETSTATUSOLDFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData(); 
			handle_getStatusOldFile(db);

			aceMessage->release();
			break;
		}
		case GETSTATUSNEWFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_getStatusNewFile(db);

			aceMessage->release();
			break;
		}
		case GETSTATUSNEXTFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_getStatusNextFile(db);

			aceMessage->release();
			break;
		}
		case SETSTATUSFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_setStatusFile(db);

			aceMessage->release();
			break;
		}
		case CDHSETSTATUSFILE:
		{
			db = afpMessage->getData();
			handle_cdhSetStatusFile(db);

			aceMessage->release();
			break;
		}
		case GETTRANSFERQUEUEATTRIBUTES:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_getTransferQueueAttributes(db);

			aceMessage->release();
			break;
		}
		case SETTRANSFERQUEUEATTRIBUTES:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			AES_AFP_TRACE_MESSAGE("From dataTask RenameTemplate before calling set= %s, NameTag = %s", (db->getFileTemplate()).c_str(),(db->getNameTag()).c_str());
			handle_setTransferQueueAttributes(db,code);
			AES_AFP_TRACE_MESSAGE("From dataTask RenameTemplate after calling set= %s, NameTag = %s", (db->getFileTemplate()).c_str(),(db->getNameTag()).c_str());

			aceMessage->release();
			break;
		}
		case FINDNEWESTFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_findNewFile(db);

			aceMessage->release();
			break;
		}
		case FINDOLDESTFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_findOldFile(db);

			aceMessage->release();
			break;
		}
		case ADDDESTINATIONSET:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_addDestinationSet(db);

			aceMessage->release();
			break;
		}
		case REMOVEDESTINATIONSET:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_removeDestinationSet(db);

			aceMessage->release();
			break;
		}
		case INFODESTINATIONSET:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_infoDestinationSets(db);

			aceMessage->release();
			break;
		}
		case OK:
		{
			aceMessage->release();
			break;
		}
		case NOTOK:
		{
			aceMessage->release();
			break;
		}
		case CDHSTOPSENDFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_stopSendFile(db,code);

			aceMessage->release();
			break;
		}
		case CDHSTOPSENDALL:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_stopSendAllFiles(db);

			aceMessage->release();
			break;
		}
		case TRANSFERQUEUEDEFINED:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_transferQueueDefined(db);

			aceMessage->release();
			break;
		}
		case TRANSFERQUEUEDESTINATIONSETDEFINED:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_transferQueueDestinationSetDefined(db);

			aceMessage->release();
			break;
		}
		case LOCKTRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_lockTransferQueue(db);

			aceMessage->release();
			break;
		}
		case UNLOCKTRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_unlockTransferQueue(db);

			aceMessage->release();
			break;
		}
		case ISTRANSFERQUEUELOCKED:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_isTransferQueueLocked(db);

			aceMessage->release();
			break;
		}
		case RESENDONEFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_resendOneFile(db);

			aceMessage->release();
			break;
		}
		case RESENDALLFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_resendAllFiles(db,code);

			aceMessage->release();
			break;
		}
		case CREATETRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_createTransferQueue(db,code);

			aceMessage->release();
			break;
		}
		case SENDOHIFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_sendFile(db);

			aceMessage->release();
			break;
		}
		case GETSOURCEDIRECTORYPATH:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_getSourceDirectoryPath(db);

			aceMessage->release();
			break;
		}
		case SETSOURCEDIRECTORYPATH:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_setSourceDirectoryPath(db);

			aceMessage->release();
			break;
		}
		case OPENTRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_openTransferQueue(db);

			aceMessage->release();
			break;
		}
		case CLOSETRANSFERQUEUE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_closeTransferQueue(db);

			aceMessage->release();
			break;
		}
		case GETLASTREPORTEDFILE:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_lastReportedFile(db);

			aceMessage->release();
			break;
		}
		case GETDESTINATIONSETLIST:
		{
			savedCallerId_ = afpMessage->getCallerId();
			handle_getDestinationSetList();

			aceMessage->release();
			break;
		}
		case GETFILESTATUS:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_getFileStatus(db);

			aceMessage->release();
			break;
		}
		case OPENGENERIC:
		{
			savedCallerId_ = afpMessage->getCallerId();
			handle_openGenericConnection();

			aceMessage->release();
			break;
		}
		case REMOVESOURCEDIRECTORY:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_removeSourceDirectoryFiles(db,code);

			aceMessage->release();
			break;
		}
		case REMOVEFILESOURCEDIRECTORY:
		{
			savedCallerId_ = afpMessage->getCallerId();
			db = afpMessage->getData();
			handle_removeFileFromSourceDirectory(db,code);

			aceMessage->release();
			break;
		}
		default:
		{
			AES_AFP_TRACE_MESSAGE("svc() Unknown message received");
			aceMessage->release();
			break;
		}
		}
	}
	return 0;
}

//## Operation: handle_removeTransferQueue%3754ECE603A3; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_removeTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_removeTransferQueue (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code)
{
	AES_AFP_TRACE_MESSAGE("Entering" );
	//## begin aes_afp_datatask::handle_removeTransferQueue%3754ECE603A3.body preserve=yes
	bool result(false);
	//	AES_GCC_Errorcodes code;

	if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_STS_CLIENT_CLASS1 ) == true)
	{
		//code = AES_FILETQREFERRED;
		code = AES_STSTQREFERRED;
		nameOftheClient = "STS";
		return false; 
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_STS_CLIENT_CLASS2 ) == true)
	{
		//code = AES_FILETQREFERRED;
		code = AES_STSTQREFERRED;
		nameOftheClient = "STS";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_CPF_CLIENT_CLASS1) == true)
	{
		//code = AES_FILETQREFERRED;
		code = AES_CPFTQREFERRED;
		nameOftheClient = "CPF";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_ALOG_CLIENT_CLASS1) == true )
	{
		//code = AES_FILETQREFERRED;
		code = AES_ALOGTQREFERRED;
		nameOftheClient = "ALOG";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_RTR_CLIENT_CLASS) == true )
	{
		code = AES_FILETQREFERRED;
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_PDS_CLIENT_CLASS1) == true )
	{
		//code = AES_FILETQREFERRED;
		code = AES_PDSTQREFERRED;
		nameOftheClient = "PDS";
		return false;
	}

	result = dBase_.removeTransferQueue(dbBlock->getTransferQueue(), dbBlock->getUsername(), code );

	if (result == true)
	{
		//sendOk(savedCallerId_);
		return true;
	}
	else
	{
		//sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_removeTransferQueue%3754ECE603A3.body
}

//## Operation: handle_addDestinationSet%3754ED9302F8; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_addDestinationSet()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_addDestinationSet (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_addDestinationSet%3754ED9302F8.body preserve=yes
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.addDestinationSet(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getUsername(),
			code );
	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_addDestinationSet%3754ED9302F8.body
}

//## Operation: handle_removeDestinationSet%3754EE0400BF; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_removeDestinationSet()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_removeDestinationSet (aes_afp_datablock* dbBlock)
{
	AES_AFP_TRACE_MESSAGE("Entering" );
	bool result(false);
	AES_GCC_Errorcodes code;

	if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_STS_CLIENT_CLASS1 ) == true)
	{
		code = AES_CATASTROPHIC;
		nameOftheClient = "STS";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_STS_CLIENT_CLASS2 ) == true)
	{
		code = AES_CATASTROPHIC;
		nameOftheClient = "STS";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_CPF_CLIENT_CLASS1) == true)
	{
		code = AES_CATASTROPHIC;
		nameOftheClient = "CPF";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_ALOG_CLIENT_CLASS1) == true)
	{
		code = AES_CATASTROPHIC;
		nameOftheClient = "ALOG";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(),AES_AFP_RTR_CLIENT_CLASS) == true)
	{
		code = AES_CATASTROPHIC;
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_PDS_CLIENT_CLASS1) == true )
	{
		code = AES_CATASTROPHIC;
		nameOftheClient = "PDS";
		return false;
	}

	result = dBase_.removeDestinationSet(dbBlock->getTransferQueue(), 
			dbBlock->getDestinationSet(),
			dbBlock->getUsername(),
			code );
	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
}


bool aes_afp_datatask::handle_removeDestinationSet_OI (aes_afp_datablock* dbBlock, AES_GCC_Errorcodes& code )
{
	AES_AFP_TRACE_MESSAGE("Entering" );
	bool result(false);
	code = AES_NOERRORCODE;

	result = dBase_.checkFileStatus(dbBlock->getTransferQueue(), dbBlock->getDestinationSet(), code); // TR HW88888

        if (result == true)
        {
                AES_AFP_TRACE_MESSAGE("No File is in READY/SEND state" );
        }
        else
        {
                AES_AFP_TRACE_MESSAGE("File is in either READY/SEND state or destinationSet not found");
		if(code == AES_NOERRORCODE)
                	code = AES_TQISOPEN;

                return false;
        }

	result = dBase_.removeDestinationSet(dbBlock->getTransferQueue(), dbBlock->getDestinationSet(), dbBlock->getUsername(), code );
	if (result == true)
	{
		AES_AFP_TRACE_MESSAGE("Destination deassociation success " );
		return true;
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("In aes_afp_datatask::handle_removeDestinationSet_OI Destination deassociation success " );
		// ret=code;
		return false;
	}
	//## end aes_afp_datatask::handle_removeDestinationSet%3754EE0400BF.body
}


//## Operation: handle_createFile%3754F4CC02A6; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_createFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_createFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_createFile%3754F4CC02A6.body preserve=yes
	bool result(false);
	AES_GCC_Errorcodes code;

	if (dbBlock->getDestinationSet() == "")
	{
		result = dBase_.createFile(dbBlock->getTransferQueue(),
				dbBlock->getFilename(),
				dbBlock->getGeneration(),
				dbBlock->getStatus(),
				code,
				"", // caller
				dbBlock->getUsername());
	}
	else
	{
		result = dBase_.createExactFile(dbBlock->getTransferQueue(),
				dbBlock->getDestinationSet(),
				dbBlock->getFilename(),
				dbBlock->getGeneration(),
				dbBlock->getStatus(),
				code,
				"", // caller
				dbBlock->getUsername());
	}
	if (result ==  true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_createFile%3754F4CC02A6.body
}

//## Operation: sendOk%375509D401D6; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendOk()
//	---------------------------------------------------------
void aes_afp_datatask::sendOk (void* to)
{
	//## begin aes_afp_datatask::sendOk%375509D401D6.body preserve=yes

	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	aes_afp_msgblock* answerMsg;

	answerMsg = new (std::nothrow) aes_afp_msgblock(0, 0);
	if(answerMsg != 0)
	{
		answerMsg->setMsgType(OK);
		answerMsg->setErrorCode(AES_NOERRORCODE);
		answerMsg->setCallerName("datatask");
		answerMsg->setCallerId(this);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)to;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		else
		{
			delete answerMsg;
			answerMsg = 0;
		}
	}
	//## end aes_afp_datatask::sendOk%375509D401D6.body
}

//## Operation: sendNotOk%37550A570333; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendNotOk()
//	---------------------------------------------------------
void aes_afp_datatask::sendNotOk (void* to, AES_GCC_Errorcodes code)
{
	//## begin aes_afp_datatask::sendNotOk%37550A570333.body preserve=yes
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	aes_afp_msgblock* answerMsg;

	answerMsg = new (std::nothrow) aes_afp_msgblock(0, 0);
	if (answerMsg != 0)
	{
		answerMsg->setMsgType(NOTOK);
		answerMsg->setErrorCode(code);
		answerMsg->setCallerName("datatask");
		answerMsg->setCallerId(this);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)to;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		else
		{
			delete answerMsg;
			answerMsg = 0;
		}
	}
	//## end aes_afp_datatask::sendNotOk%37550A570333.body
}

//## Operation: handle_infoOneTransferQueue%37563EF900A6; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_infoOneTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_infoOneTransferQueue (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_infoOneTransferQueue%37563EF900A6.body preserve=yes
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	aes_afp_msgblock* answerMsg;
	aes_afp_datablock* answerData;
	AES_GCC_Errorcodes code;

	answerData = new aes_afp_datablock;

	result = dBase_.getOneInformation(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			answerData->getListString(),
			code);
	if (result == false)
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return true;
	}

	answerData->setMsgType(INFOONETRANSFERQUEUE);
	answerData->setErrorCode(AES_NOERRORCODE);
	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	answerMsg->setMsgType(INFOONETRANSFERQUEUE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);
	return true;
	//## end aes_afp_datatask::handle_infoOneTransferQueue%37563EF900A6.body
}

//## Operation: handle_findTransferQueue%375BA98300D7; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_findTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_findTransferQueue (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_findTransferQueue%375BA98300D7.body preserve=yes
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	aes_afp_msgblock* answerMsg;
	aes_afp_datablock* answerData;
	AES_GCC_Errorcodes code;

	answerData = new aes_afp_datablock;

	result = dBase_.findTransferQueue(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			answerData->getListString(),
			code);

	if (result == false)
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return true;
	}

	answerData->setMsgType(FINDTRANSFERQUEUE);
	answerData->setErrorCode(AES_NOERRORCODE);

	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	answerMsg->setMsgType(FINDTRANSFERQUEUE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);
	return true;
	//## end aes_afp_datatask::handle_findTransferQueue%375BA98300D7.body
}

//get all assoaciated destset info; used in CCB rollback
vector<TqInfo> aes_afp_datatask::handle_infoAllDestset(std::string TqName)
{
	return  dBase_.infoAllDestset(TqName);
}


//## Operation: handle_infoAllTransferQueues%375E04130318; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_infoAllTransferQueues()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_infoAllTransferQueues (aes_afp_datablock* dbBlock)
{
	(void)(dbBlock);		
	//## begin aes_afp_datatask::handle_infoAllTransferQueues%375E04130318.body preserve=yes
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	aes_afp_msgblock* answerMsg;
	aes_afp_datablock* answerData;
	AES_GCC_Errorcodes code;
	bool result(false);
	answerData = new aes_afp_datablock;
	result = dBase_.getAllInformation(answerData->getListString(), code );

	if (result == false)
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return true;
	}

	answerData->setMsgType(INFOALLTRANSFERQUEUES);
	answerData->setErrorCode(AES_NOERRORCODE);
	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setMsgType(INFOALLTRANSFERQUEUES);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	std::string answerList = answerData->getListString();

	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);
	return true;
	//## end aes_afp_datatask::handle_infoAllTransferQueues%375E04130318.body
}

bool aes_afp_datatask::handle_infoAllTransferQueuesShort (aes_afp_datablock* dbBlock)
{
	(void)(dbBlock);	
	//## begin aes_afp_datatask::handle_infoAllTransferQueues%375E04130318.body preserve=yes
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	aes_afp_msgblock* answerMsg;
	aes_afp_datablock* answerData;
	AES_GCC_Errorcodes code;
	bool result(false);
	answerData = new aes_afp_datablock;
	result = dBase_.getAllInformationShort(answerData->getListString(), code );

	if (result == false)
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return true;
	}

	answerData->setMsgType(INFOALLTRANSFERQUEUESSHORT);
	answerData->setErrorCode(AES_NOERRORCODE);
	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setMsgType(INFOALLTRANSFERQUEUESSHORT);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	std::string answerList = answerData->getListString();

	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);
	return true;
	//## end aes_afp_datatask::handle_infoAllTransferQueues%375E04130318.body
}


//## Operation: handle_findNewFile%375E494B0037; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_findNewFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_findNewFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_findNewFile%375E494B0037.body preserve=yes
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	aes_afp_msgblock* answerMsg;
	aes_afp_datablock* answerData;
	AES_GCC_Errorcodes code;

	answerData = new aes_afp_datablock;

	result = dBase_.infoNewestFile(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			answerData->getListString(),
			code);

	if (result == false)
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}

	answerData->setMsgType(FINDNEWESTFILE);
	answerData->setErrorCode(AES_NOERRORCODE);
	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setMsgType(FINDNEWESTFILE);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);

	return true;
	//## end aes_afp_datatask::handle_findNewFile%375E494B0037.body
}

//## Operation: handle_findOldFile%375E49FE0283; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_findOldFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_findOldFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_findOldFile%375E49FE0283.body preserve=yes
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	aes_afp_msgblock* answerMsg;
	aes_afp_datablock* answerData;
	AES_GCC_Errorcodes code;

	answerData = new aes_afp_datablock;

	result = dBase_.infoOldestFile(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			answerData->getListString(),
			code);

	if (result == false)
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}

	answerData->setErrorCode(AES_NOERRORCODE);
	answerData->setErrorCode(code);
	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setMsgType(FINDOLDESTFILE);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);

	return true;
	//## end aes_afp_datatask::handle_findOldFile%375E49FE0283.body
}

//## Operation: handle_setStatusFile%375F99860161; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_setStatusFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_setStatusFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_setStatusFile%375F99860161.body preserve=yes
	AES_GCC_Errorcodes code;
	bool result(false);

	result = dBase_.changeFileStatus(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			dbBlock->getStatus(),
			code);

	if (result == false)
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	else
	{
		sendOk(savedCallerId_);
		return true;
	}
	//## end aes_afp_datatask::handle_setStatusFile%375F99860161.body
}

//## Operation: handle_deleteFile%3764CAF60395; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_deleteFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_deleteFile ()
{
	//## begin aes_afp_datatask::handle_deleteFile%3764CAF60395.body preserve=yes
	dBase_.deleteFiles();

	return true;
	//## end aes_afp_datatask::handle_deleteFile%3764CAF60395.body
}

//## Operation: handle_getStatusFile%3764FB0F0282; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_getStatusFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_getStatusFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_getStatusFile%3764FB0F0282.body preserve=yes
	bool result(false);
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	AES_GCC_Errorcodes code;

	result = dBase_.getFileStatus(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			answerData,
			code);
	if (result == true)
	{
		answerData->setMsgType(GETSTATUSFILE);
		answerData->setErrorCode(AES_NOERRORCODE);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETSTATUSFILE);
		answerMsg->setErrorCode(AES_NOERRORCODE);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_getStatusFile%3764FB0F0282.body
}

//## Operation: handle_getStatusOldFile%3765155401EC; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_getStatusOldFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_getStatusOldFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_getStatusOldFile%3765155401EC.body preserve=yes
	bool result(false);
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	AES_GCC_Errorcodes code;

	result = dBase_.getStatusOldFile(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			answerData,
			code);

	if (result == true)
	{
		answerData->setMsgType(GETSTATUSOLDFILE);
		answerData->setErrorCode(AES_NOERRORCODE);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETSTATUSOLDFILE);
		answerMsg->setErrorCode(AES_NOERRORCODE);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;

	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_getStatusOldFile%3765155401EC.body
}

//## Operation: handle_getStatusNewFile%3765156A007B; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_getStatusNewFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_getStatusNewFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_getStatusNewFile%3765156A007B.body preserve=yes
	bool result(false);
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	AES_GCC_Errorcodes code;

	result = dBase_.getStatusNewFile(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			answerData,
			code);

	if (result == true)
	{
		answerData->setMsgType(GETSTATUSNEWFILE);
		answerData->setErrorCode(AES_NOERRORCODE);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETSTATUSNEWFILE);
		answerMsg->setErrorCode(AES_NOERRORCODE);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_getStatusNewFile%3765156A007B.body
}

void aes_afp_datatask::handle_shutdown ()
{
	AES_GCC_Errorcodes code;
	dBase_.destroy(code);
}

bool aes_afp_datatask::handle_removeFile (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code)
{
	bool result(false);
	AES_AFP_TRACE_MESSAGE( "Entering" );
	//  AES_GCC_Errorcodes code;

	result = dBase_.removeFile(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			dbBlock->getUsername(),
			code);

	if (result == true)
	{
		//Commented for APG43L.
#if 0
		sendOk(savedCallerId_);
#endif
		return true;
	}
	else
	{
		//Commented for APG43L.
#if 0
		sendNotOk(savedCallerId_, code);
#endif
		return false;
	}

	//## end aes_afp_datatask::handle_removeFile%376A2E900399.body
}

//## Operation: handle_statusarchive%376A46E80200; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_statusarchive()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_statusarchive (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_statusarchive%376A46E80200.body preserve=yes
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.changeFileStatus(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			AES_FSARCHIVE,
			code);

	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_statusarchive%376A46E80200.body
}

//## Operation: handle_getTransferQueueAttributes%37E9CAA300C6; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_getTransferQueueAttributes()
//	---------------------------------------------------------
aes_afp_datablock * aes_afp_datatask::handle_getTransferQueueAttributesToModify (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code  )
{
	bool result(false);
	aes_afp_datablock *  answerData = new (std::nothrow) aes_afp_datablock;
	result = dBase_.getTransferQueueAttributes(dbBlock->getTransferQueue(),dbBlock->getDestinationSet(),answerData,code);
	AES_AFP_TRACE_MESSAGE("Transfer Queue get attributes, errorCode : %d, result = %d ",code, result);
	return answerData;
}
bool aes_afp_datatask::handle_getTransferQueueAttributes (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_getTransferQueueAttributes%37E9CAA300C6.body preserve=yes
	bool result(false);

	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	AES_GCC_Errorcodes code;

	result = dBase_.getTransferQueueAttributes(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			answerData,
			code);

	if (result == true)
	{
		answerData->setMsgType(GETTRANSFERQUEUEATTRIBUTES);
		answerData->setErrorCode(code);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETTRANSFERQUEUEATTRIBUTES);
		answerMsg->setErrorCode(code);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_getTransferQueueAttributes%37E9CAA300C6.body
}

//## Operation: handle_setTransferQueueAttributes%37E9CB7D03AE; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_setTransferQueueAttributes()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_setTransferQueueAttributes (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code, bool changeRenameTemplate)
{
	//## begin aes_afp_datatask::handle_setTransferQueueAttributes%37E9CB7D03AE.body preserve=yes
	bool result(false);
	//  AES_GCC_Errorcodes code;

	AES_AFP_TRACE_MESSAGE("From dataTask second RenameTemplate = %s, NameTag = %s", (dbBlock->getFileTemplate()).c_str(),(dbBlock->getNameTag()).c_str());

	result = dBase_.setTransferQueueAttributes(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getUsername(),
			code,
			dbBlock->getSendRetries(),
			dbBlock->getRemoveDelay(),
			dbBlock->getStatus(),
			dbBlock->getRetriesDelay(),
			dbBlock->getFileTemplate(),
			dbBlock->getNameTag(),
			dbBlock->getStartupSequenceNumber(), //HT50930
			dbBlock->getRemoveBefore(),
			changeRenameTemplate);
	if (result == true)
	{
		// sendOk(savedCallerId_);
		return true;
	}
	else
	{
		// sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_setTransferQueueAttributes%37E9CB7D03AE.body
}

//## Operation: handle_cdhSetStatusFile%3804359600EE; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_cdhSetStatusFile()
//	---------------------------------------------------------
void aes_afp_datatask::handle_cdhSetStatusFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_cdhSetStatusFile%3804359600EE.body preserve=yes
	AES_GCC_Errorcodes code;

	dBase_.cdhChangeFileStatus(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			dbBlock->getStatus(),
			code);
	//## end aes_afp_datatask::handle_cdhSetStatusFile%3804359600EE.body
}

//## Operation: handle_stopSendFile%380870A00040; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_stopSendFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_stopSendFile (aes_afp_datablock* dbBlock, AES_GCC_Errorcodes& code)
{
	//## begin aes_afp_datatask::handle_stopSendFile%380870A00040.body preserve=yes
	bool result(false);
	//  AES_GCC_Errorcodes code;

	result = dBase_.stopSendFile(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			dbBlock->getUsername(),
			code);
	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_stopSendFile%380870A00040.body
}

//## Operation: handle_stopSendAllFiles%380870B00057; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_stopSendAllFiles()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_stopSendAllFiles (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_stopSendAllFiles%380870B00057.body preserve=yes
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.stopSendAll(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getUsername(),
			code);
	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_stopSendAllFiles%380870B00057.body
}

//## Operation: handle_infoDestinationSets%3833ADA501ED; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_infoDestinationSets()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_infoDestinationSets (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_infoDestinationSets%3833ADA501ED.body preserve=yes
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.getInformationDestinationSets(dbBlock->getTransferQueue(),
			answerData->getStrBuf(),
			code);

	if (result == true)
	{
		answerData->setMsgType(INFODESTINATIONSET);
		answerData->setErrorCode(code);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(INFODESTINATIONSET);
		answerMsg->setErrorCode(code);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_infoDestinationSets%3833ADA501ED.body
}

bool aes_afp_datatask::handle_infoDestinationSetsForModify(aes_afp_datablock* dbBlock , string& destList,AES_GCC_Errorcodes& code)
{
	aes_afp_datablock* answerData = new aes_afp_datablock;
	bool result(false);

	result = dBase_.getInformationDestinationSets(dbBlock->getTransferQueue(),
			answerData->getStrBuf(),
			code);

	if (result == true)
	{
		destList = answerData->getStrBuf();
	}
	delete answerData;
	answerData = 0;
	return result;	
}

//## Operation: handle_transferQueueDefined%388C20D10289; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_transferQueueDefined()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_transferQueueDefined (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_transferQueueDefined%388C20D10289.body preserve=yes
	AES_GCC_Errorcodes code;
	bool result(false);
	result = dBase_.transferQueueDefined(dbBlock->getTransferQueue(), code);

	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_transferQueueDefined%388C20D10289.body
}

//## Operation: handle_transferQueueDestinationSetDefined%388C20F101A8; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_transferQueueDestinationSetDefined()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_transferQueueDestinationSetDefined (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_transferQueueDestinationSetDefined%388C20F101A8.body preserve=yes
	AES_GCC_Errorcodes code;
	bool result(false);
	result = dBase_.transferQueueDestinationSetDefined(dbBlock->getTransferQueue(), dbBlock->getDestinationSet(), code);

	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_transferQueueDestinationSetDefined%388C20F101A8.body
}

//## Operation: handle_lockTransferQueue%38B25FBA01D2; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_lockTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_lockTransferQueue (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_lockTransferQueue%38B25FBA01D2.body preserve=yes
	AES_GCC_Errorcodes code;
	bool result(false);

	result = dBase_.lockTransferQueue(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getUsername(),
			code);

	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_lockTransferQueue%38B25FBA01D2.body
}

//## Operation: handle_unlockTransferQueue%38B25FD003C8; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_unlockTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_unlockTransferQueue (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_unlockTransferQueue%38B25FD003C8.body preserve=yes
	AES_GCC_Errorcodes code;
	bool result(false);

	result = dBase_.unlockTransferQueue(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getUsername(),
			code);

	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_unlockTransferQueue%38B25FD003C8.body
}

//## Operation: handle_isTransferQueueLocked%38B38E5002BB; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_isTransferQueueLocked()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_isTransferQueueLocked (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_isTransferQueueLocked%38B38E5002BB.body preserve=yes
	AES_GCC_Errorcodes code;
	bool result(false);
	std::string user("");
	aes_afp_datablock* answerData;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;

	result = dBase_.isTransferQueueLocked(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			user,
			code);

	if (result == false)
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}

	answerData = new aes_afp_datablock;

	answerData->setMsgType(ISTRANSFERQUEUELOCKED);
	answerData->setErrorCode(AES_NOERRORCODE);
	answerData->setApplication(user);
	answerData->setLock();
	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setMsgType(ISTRANSFERQUEUELOCKED);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);

	return true;
	//## end aes_afp_datatask::handle_isTransferQueueLocked%38B38E5002BB.body
}

//## Operation: handle_resendOneFile%3A6EC56E0277; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_resendOneFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_resendOneFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_resendOneFile%3A6EC56E0277.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Enteirng");
	AES_GCC_Errorcodes code = AES_NOERRORCODE;
	bool result(false);
	std::string user("");

	if (dbBlock->getFilename() == "")
	{
		result = dBase_.resendOne(dbBlock->getTransferQueue(),
				dbBlock->getDestinationSet(),
				dbBlock->getUsername(),
				code,
				dbBlock->getNewDestinationSet() );
	}
	else
	{
		result = dBase_.resendOneFile(dbBlock->getTransferQueue(),
				dbBlock->getFilename(),
				dbBlock->getDestinationSet(),
				dbBlock->getUsername(),
				code,
				dbBlock->getNewDestinationSet() );
	}

	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}

	//## end aes_afp_datatask::handle_resendOneFile%3A6EC56E0277.body
}

//	---------------------------------------------------------
//	       handle_resendAllFiles()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_resendAllFiles (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code)
{
	AES_AFP_TRACE_MESSAGE("Entering" );
	//AES_GCC_Errorcodes code;
	bool result(false);

	result = dBase_.resendAll(dbBlock->getTransferQueue(),dbBlock->getUsername(),code);

	if (result == true)
	{
		//sendOk(savedCallerId_);
		return true;
	}
	else
	{
		//sendNotOk(savedCallerId_, code);
		return false;
	}
}

//## Operation: handle_createTransferQueue%3B25B6660084; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_createTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_createTransferQueue (aes_afp_datablock* dbBlock, AES_GCC_Errorcodes& code, Dest_Set_Type dType)
{
	//## begin aes_afp_datatask::handle_createTransferQueue%3B25B6660084.body preserve=yes
	bool result(false);
	//  AES_GCC_Errorcodes code;

	result = dBase_.createTransferQueue(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			code,
			dbBlock->getSourceDirectory(),
			dbBlock->getStatus(),
			dbBlock->getRemoveDelay(),
			dbBlock->getRemoveBefore(),
			dbBlock->getRetriesDelay(),
			dbBlock->getSendRetries(),
			dbBlock->getFileRename(),
			dbBlock->getFileTemplate(),
			dbBlock->getUserGroup(),
			dbBlock->getNameTag(),
			dbBlock->getStartupSequenceNumber(), dType);
	if (result == true)
	{
#if 0
		sendOk(savedCallerId_); //Commented for APG43L
#endif
		return true;
	}
#if 0	//Commented for APG43L
	sendNotOk(savedCallerId_, code);
#endif
	AES_AFP_LOG(LOG_LEVEL_ERROR,"Transfer Queue creation failed , errorCode : %d ",code);
	AES_AFP_TRACE_MESSAGE("Transfer Queue creation failed , errorCode : %d ",code);
	return false;
	//## end aes_afp_datatask::handle_createTransferQueue%3B25B6660084.body
}

//## Operation: handle_sendFile%3B2763F302DE; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_sendFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_sendFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_sendFile%3B2763F302DE.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Entering");
	bool result(false);
	AES_GCC_Errorcodes code = AES_NOERRORCODE;
	result = dBase_.sendFile(dbBlock->getTransferQueue(),
			dbBlock->getFilename(),
			code,
			dbBlock->getFormat(),
			dbBlock->getMask(),
			dbBlock->getDirectoryFlag(),
			dbBlock->getStatus() );

	if(result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	else if (result == false)
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}
	return false;	
}

//## Operation: handle_getSourceDirectoryPath%3B27A03F010E; C++
//## Semantics:
//	---------------------------------------------------------
//	       getSourceDirectoryPath()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_getSourceDirectoryPath (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_getSourceDirectoryPath%3B27A03F010E.body preserve=yes
	aes_afp_datablock* answerData;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	std::string tmpDir("");

	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.getSourceDirectory(dbBlock->getTransferQueue(), tmpDir, code);
	if (result == false)
	{
		sendNotOk(savedCallerId_, code);
		return false;
	}

	answerData = new aes_afp_datablock; 
	answerData->setMsgType(GETSOURCEDIRECTORYPATH);
	answerData->setErrorCode(AES_NOERRORCODE);
	answerData->setSourceDirectory(tmpDir);
	answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
	answerMsg->setMsgType(GETSOURCEDIRECTORYPATH);
	answerMsg->setErrorCode(AES_NOERRORCODE);
	sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
	if (apcomThreadExists() == true)
		sendQueue->putq(answerMsg);

	return true;
	//## end aes_afp_datatask::handle_getSourceDirectoryPath%3B27A03F010E.body
}

//## Operation: handle_setSourceDirectoryPath%3B27A89903E3; C++
//## Semantics:
//	---------------------------------------------------------
//	       setSourceDirectoryPath()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_setSourceDirectoryPath (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_setSourceDirectoryPath%3B27A89903E3.body preserve=yes
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.setSourceDirectory(dbBlock->getTransferQueue(), dbBlock->getSourceDirectory(), code );
	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	sendNotOk(savedCallerId_, code);
	return false;
	//## end aes_afp_datatask::handle_setSourceDirectoryPath%3B27A89903E3.body
}

//## Operation: handle_openTransferQueue%3B27AF9C02B8; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_openTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_openTransferQueue (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_openTransferQueue%3B27AF9C02B8.body preserve=yes
	bool result(false);
	AES_GCC_Errorcodes code;
	result = dBase_.openTransferQueue(dbBlock->getTransferQueue(), dbBlock->getDsdChannel(), code );
	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	sendNotOk(savedCallerId_, code);
	return false;
	//## end aes_afp_datatask::handle_openTransferQueue%3B27AF9C02B8.body
}

//## Operation: handle_closeTransferQueue%3B407A6D03B9; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_closeTransferQueue()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_closeTransferQueue (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_closeTransferQueue%3B407A6D03B9.body preserve=yes
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.closeTransferQueue(dbBlock->getTransferQueue(), code );
	if (result == true)
	{
		sendOk(savedCallerId_);
		return true;
	}
	sendNotOk(savedCallerId_, code);
	return false;
	//## end aes_afp_datatask::handle_closeTransferQueue%3B407A6D03B9.body
}

//## Operation: handle_lastReportedFile%3B3F308701B3; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_lastReportedFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_lastReportedFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_lastReportedFile%3B3F308701B3.body preserve=yes
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.getLastReported(dbBlock->getTransferQueue(),
			answerData->getStrBuf(),
			code);

	if (result == true)
	{
		answerData->setMsgType(GETLASTREPORTEDFILE);
		answerData->setErrorCode(code);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETLASTREPORTEDFILE);
		answerMsg->setErrorCode(code);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_lastReportedFile%3B3F308701B3.body
}

//## Operation: handle_getDestinationSetList%3B3F309F01C2; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_getDestinationSetList()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_getDestinationSetList ()
{
	//## begin aes_afp_datatask::handle_getDestinationSetList%3B3F309F01C2.body preserve=yes
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	AES_GCC_Errorcodes code;

	result = dBase_.getDestinationSetList(answerData->getStrBuf(),
			code);

	if (result == true)
	{
		answerData->setMsgType(GETDESTINATIONSETLIST);
		answerData->setErrorCode(code);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETDESTINATIONSETLIST);
		answerMsg->setErrorCode(code);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_getDestinationSetList%3B3F309F01C2.body
}

//## Operation: handle_getFileStatus%3B3F30BF01F0; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_getFileStatus()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_getFileStatus (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_getFileStatus%3B3F30BF01F0.body preserve=yes
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	bool result(false);
	AES_GCC_Errorcodes code;
	AES_GCC_Filestates state;

	result = dBase_.getFileStatus(dbBlock->getFilename(),
			state,
			code);

	if (result == true)
	{
		answerData->setMsgType(GETFILESTATUS);
		answerData->setErrorCode(code);
		answerData->setStatus(state);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETFILESTATUS);
		answerMsg->setErrorCode(code);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}

	//## end aes_afp_datatask::handle_getFileStatus%3B3F30BF01F0.body
}

//## Operation: handle_openGenericConnection%3B3F42B60398; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_openGenericConnection()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_openGenericConnection ()
{
	//## begin aes_afp_datatask::handle_openGenericConnection%3B3F42B60398.body preserve=yes
	sendOk(savedCallerId_);
	return true;
	//## end aes_afp_datatask::handle_openGenericConnection%3B3F42B60398.body
}

//## Operation: handle_sendManually%3B45E2A7001A; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_sendManually()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_sendManually (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& errCode)
{
	//## begin aes_afp_datatask::handle_sendManually%3B45E2A7001A.body preserve=yes
	bool result(false);

	result = dBase_.sendManually(dbBlock->getTransferQueue(),
			dbBlock->getFilename(),
			dbBlock->getSourceDirectory(),
			dbBlock->getDestinationSet(),
			dbBlock->getSendRetries(),
			dbBlock->getRetriesDelay(),
			dbBlock->getDirectoryFlag(),
			dbBlock->getUsername(),
			dbBlock->getUserGroup(),
			errCode);
	return result;
}

//## Operation: handle_getStatusNextFile%3E27FB01017A; C++
//## Semantics:
//	---------------------------------------------------------
//	       handle_getStatusNextFile()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_getStatusNextFile (aes_afp_datablock* dbBlock)
{
	//## begin aes_afp_datatask::handle_getStatusNextFile%3E27FB01017A.body preserve=yes
	bool result(false);
	aes_afp_datablock* answerData = new aes_afp_datablock;
	aes_afp_msgblock* answerMsg;
	ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* sendQueue;
	AES_GCC_Errorcodes code;

	std::string nextFileName("");

	result = dBase_.getNextFilename(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			nextFileName,
			code);
	if (result == false)
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}

	result = dBase_.getFileStatus(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			nextFileName,
			answerData,
			code);
	if (result == true)
	{
		answerData->setMsgType(GETSTATUSFILE);
		answerData->setErrorCode(AES_NOERRORCODE);
		answerMsg = new aes_afp_msgblock(sizeof(*answerData), answerData);
		answerMsg->setMsgType(GETSTATUSFILE);
		answerMsg->setErrorCode(AES_NOERRORCODE);
		sendQueue = (ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>*)savedCallerId_;
		if (apcomThreadExists() == true)
			sendQueue->putq(answerMsg);
		return true;
	}
	else
	{
		delete answerData;
		sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_getStatusNextFile%3E27FB01017A.body
}

void aes_afp_datatask::set_stop(bool shutdown)
{
	stop = shutdown;
	AES_AFP_TRACE_MESSAGE("shutdown flag set");

}

//	---------------------------------------------------------
//	       handle_removeSourceDirectoryFiles()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_removeSourceDirectoryFiles (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code)		//for APZ21230/4-699
{
	AES_AFP_TRACE_MESSAGE("Entering" );
	//## begin aes_afp_datatask::handle_removeSourceDirectoryFiles%3754ECE603A3.body preserve=yes
	bool result(false);
	//  AES_GCC_Errorcodes code;

	if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_STS_CLIENT_CLASS1 ) == true)
	{
		//code = AES_FILETQREFERRED;
		code = AES_STSTQREFERRED;
		nameOftheClient = "STS";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_STS_CLIENT_CLASS2 ) == true)
	{
		//code = AES_FILETQREFERRED;
		code = AES_STSTQREFERRED;
		nameOftheClient = "STS";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_CPF_CLIENT_CLASS1) == true)
	{
		//code = AES_FILETQREFERRED;
		code = AES_CPFTQREFERRED;
		nameOftheClient = "CPF";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_ALOG_CLIENT_CLASS1) == true)
	{
		//code = AES_FILETQREFERRED;
		code = AES_ALOGTQREFERRED;
		nameOftheClient = "ALOG";
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(),AES_AFP_RTR_CLIENT_CLASS) == true)
	{
		code = AES_FILETQREFERRED;
		return false;
	}
	else if ( isTQReferred(dbBlock->getTransferQueue(), AES_AFP_PDS_CLIENT_CLASS1) == true )
	{
		//code = AES_FILETQREFERRED;
		code = AES_PDSTQREFERRED;
		nameOftheClient = "PDS";
		return false;
	}

	result = dBase_.removeSourceDirectoryFiles(dbBlock->getTransferQueue(), dbBlock->getUsername(), code );

	if (result == true)
	{
		//sendOk(savedCallerId_);
		return true;
	}
	else
	{
		//sendNotOk(savedCallerId_, code);
		return false;
	}
	//## end aes_afp_datatask::handle_removeSourceDirectoryFiles%3754ECE603A3.body
}

//	---------------------------------------------------------
//	       handle_removeFileSourceDirectory()
//	---------------------------------------------------------
bool aes_afp_datatask::handle_removeFileFromSourceDirectory (aes_afp_datablock* dbBlock,AES_GCC_Errorcodes& code)		//for APZ21230/4-699
{
	//## begin aes_afp_datatask::handle_removeFileFromSourceDirectory%376A2E900399.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Entering" );
	bool result(false);
	//AES_GCC_Errorcodes code;

	result = dBase_.removeFileFromSourceDirectory(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			dbBlock->getUsername(),
			code);

	if (result == true)
	{
		//Commented for APG43L.
#if 0
		sendOk(savedCallerId_);
#endif
		return true;
	}
	else
	{
		//Commented for APG43L.
#if 0
		sendNotOk(savedCallerId_, code);
#endif
		return false;
	}

}


bool aes_afp_datatask::handle_getFileInfo (aes_afp_datablock* dbBlock)
{
	AES_AFP_TRACE_MESSAGE("Entering");

	AES_GCC_Errorcodes code;

	bool result = dBase_.getFileStatus(dbBlock->getTransferQueue(),
			dbBlock->getDestinationSet(),
			dbBlock->getFilename(),
			dbBlock,
			code);
	AES_AFP_TRACE_MESSAGE("Leaving with result = %d", result);
	return result;

}

void aes_afp_datatask::setServer(aes_afp_apcom_serv* &obj)
{
	apComObj_ = obj;
}

bool aes_afp_datatask::apcomThreadExists()
{
	aes_afp_apcom_serv::afpMutex.acquire();
	bool found = false;
	char stname[32];
	ACE_OS::sprintf(stname, "HAND%p", savedCallerId_);

	// try to check for apcomhandler existance
	std::map<std::string, aes_afp_apcom_hand*>::iterator itr;
	itr = apComObj_->handMap_.find(stname);
	if (itr != apComObj_->handMap_.end() )
	{
		found = true;
	}
	aes_afp_apcom_serv::afpMutex.release();
	return found;
}
