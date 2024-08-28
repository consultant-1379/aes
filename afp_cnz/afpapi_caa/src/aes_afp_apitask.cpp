//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3AFB82A10276.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3AFB82A10276.cm

//## begin module%3AFB82A10276.cp preserve=no
//	INCLUDE aes_afp_apitask.h
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
//	 190 89-CAA 109 0505
//
//	AUTHOR
//	 2003-04-30 DAPA
//
//	REVISION
//	 A 2003-04-30 DAPA
//	 B 2003-04-30 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3AFB82A10276.cp

//## Module: aes_afp_apitask%3AFB82A10276; Package body
//## Subsystem: AFP::afpapi_caa::src%37DF37FE018C
//## Source file: Z:\ntaes\afp\afpapi_caa\src\aes_afp_apitask.cpp

//## begin module%3AFB82A10276.additionalIncludes preserve=no
//## end module%3AFB82A10276.additionalIncludes

//## begin module%3AFB82A10276.includes preserve=yes
//## end module%3AFB82A10276.includes

#include "aes_afp_apitask.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(AES_AFP_apitask);

void* svcThread(void* apiObj);

aes_afp_apitask::aes_afp_apitask(ACS_DSD_Session* conversionSession,
                                 aes_afp_apiinternal* apiinternal,
                                 AES_AFP_Api* apiPointer)
      : stackSize(1000),runUntilDoomsDay_(true),
        convSession_(conversionSession),
        internalPtr_(apiinternal),
        apiPtr_(0),eventQueue_(0),cmdQueue_(0)
{
	apiPtr_ = apiPointer;
	ACE_OS::event_init(&treadTerminated_);
	ACE_OS::thread_mutex_init(&lockCondition);
	_threadHandle = 0;
	_threadId = 0;
	AES_AFP_TRACE_MESSAGE("Constructor()");
}

aes_afp_apitask::~aes_afp_apitask()
{
  ACE_OS::event_destroy(&treadTerminated_);
  ACE_OS::thread_mutex_destroy(&lockCondition);
}

int aes_afp_apitask::open (void* args, ACE_Thread_Manager* thrMgr)
{
	(void)(args);
	(void)(thrMgr);
	AES_AFP_TRACE_MESSAGE("Entering");
	eventQueue_ = internalPtr_->getEventQueue();
	cmdQueue_ = internalPtr_->getCmdQueue();
	int resultThreadCreation = pthread_create(&_threadId, NULL, svcThread, this);
	if (resultThreadCreation != 0)
	{
		AES_AFP_TRACE_MESSAGE("Thread spawn failed, errno = %d", errno);
		char buffer[256];
		AES_AFP_TRACE_MESSAGE("error message = %s", strerror_r(errno, buffer, 256));
	}
	AES_AFP_TRACE_MESSAGE("Thread spawn result = %d", resultThreadCreation);

	return resultThreadCreation;
}

int aes_afp_apitask::close(bool dsdSessionClose)
{
	AES_AFP_TRACE_MESSAGE( "Svc is finished, closing thread");
	if(dsdSessionClose)
		this->convSession_->close(); 

	ACE_OS::event_signal(&(this->treadTerminated_));
	return 0;
}

bool  aes_afp_apitask::getStopCondition()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	bool condition_state;

	ACE_OS::thread_mutex_lock(&lockCondition);

	condition_state = runUntilDoomsDay_;

	ACE_OS::thread_mutex_unlock(&lockCondition);
	AES_AFP_TRACE_MESSAGE( "Leaving with %d", condition_state);
	return condition_state;
}

//	---------------------------------------------------------
//	       receiveFromDsd()
//	---------------------------------------------------------
int aes_afp_apitask::receiveFromDsd (std::string& dsdBuffer)
{
	int result(0);
	unsigned long totdatasize(0);
	char endFlag('0');
	char tmpBuf[AES_AFP_MAXBUFFER];

	dsdBuffer = "";
	while (endFlag != '1')
	{
		char recvBuf[AES_AFP_MAXBUFFER] = {0};
		unsigned int msgLen = AES_AFP_MAXBUFFER;
		AES_AFP_TRACE_MESSAGE("Calling DSD");
		// Added the timeout for detach hang problem in APG43L
		result = convSession_->recv((char*)recvBuf, msgLen, 100);

		if (result <= 0)
		{
			if (runUntilDoomsDay_ == false) 
			{
				AES_AFP_TRACE_MESSAGE("runUtilDoomsDay_ is false.");
				AES_AFP_TRACE_MESSAGE("Returning from recieveDsd");
				return -1;
			} 
			AES_AFP_TRACE_MESSAGE("result = %d", result );
			if( (result == -43) || (result == 0 ))
			{

				AES_AFP_TRACE_MESSAGE("Returning from recieveDsd");
				return AES_NOSERVERACCESS;
			}
			continue;

		}
		else
		{
			AES_AFP_TRACE_MESSAGE("Received %d bytes from DSD, result = %d", result, convSession_->last_error());
			totdatasize += (result-1);

			endFlag = recvBuf[4]; // index 4 is flag for more data (more buffers).

			strcpy(tmpBuf, &recvBuf[6]);
			dsdBuffer += tmpBuf;
		}
	}
	AES_AFP_TRACE_MESSAGE("Received totally %ld bytes from DSD", totdatasize);
	return 0;

}

//## Operation: putOnEventQueue%3B0B99990133; C++
//## Semantics:
//	---------------------------------------------------------
//	       putOnEventQueue()
//	---------------------------------------------------------
int aes_afp_apitask::putOnEventQueue (std::string& dataString, unsigned short message)
{
	//## begin aes_afp_apitask::putOnEventQueue%3B0B99990133.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Text %s event %d", dataString.c_str(), message);

	aes_afp_apimsg* msg = new (std::nothrow)aes_afp_apimsg;
	dataString += '\0';
	if (msg != NULL)
	{
		msg->setMsgString(dataString);
		msg->setEventCode((AES_GCC_Eventcodes)message);
		ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeout(eventQueue_->gettimeofday());
		timeout += ACE_Time_Value(3, 0);
		if(eventQueue_->state() == ACE_Message_Queue_Base::ACTIVATED)
		{
			eventQueue_->enqueue_head(msg,&timeout);
		}
	}
	AES_GCC_Eventcodes errorcode = (AES_GCC_Eventcodes)message;

	// notify appl. about this event
	if (apiPtr_ != 0)
	{
		//apiPtr_->event( (AES_GCC_Eventcodes&)message);
		apiPtr_->event(errorcode);
	}

	return 0;
	//## end aes_afp_apitask::putOnEventQueue%3B0B99990133.body
}

//## Operation: putOnCmdQueue%3B0BB71901FB; C++
//## Semantics:
//	---------------------------------------------------------
//	       putOnCmdQueue()
//	---------------------------------------------------------
int aes_afp_apitask::putOnCmdQueue (std::string& dataString, unsigned short message, unsigned short errorCode)
{
	//## begin aes_afp_apitask::putOnCmdQueue%3B0BB71901FB.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Message = %d, error code = %d", message, errorCode);

	aes_afp_apimsg* msg = new aes_afp_apimsg;

	msg->setMsgString(dataString);
	msg->setMessage((aes_afp_msgtypes)message);
	msg->setErrorCode((AES_GCC_Errorcodes)errorCode);

	cmdQueue_->enqueue_head(msg);
	AES_AFP_TRACE_MESSAGE("Message = %d, dataString.length = %d", (int)message, (int)dataString.length());
	return 0;
	//## end aes_afp_apitask::putOnCmdQueue%3B0BB71901FB.body
}

//## Operation: endSvcLoop%3B11E7910025; C++
//## Semantics:
//	---------------------------------------------------------
//	       endSvcLoop()
//	---------------------------------------------------------
void aes_afp_apitask::endSvcLoop ()
{
	//## begin aes_afp_apitask::endSvcLoop%3B11E7910025.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Begin.. Set loop flag to false");
	//WriteIntoAFPApi("endSvcLoop() Begin.. Set loop flag to false");
	//HK67222:: Guarding the runUntilDoomsDay_ to synchronize the threads.
	ACE_OS::thread_mutex_lock(&lockCondition);

	runUntilDoomsDay_ = false;

	ACE_OS::thread_mutex_unlock(&lockCondition);
	//WriteIntoAFPApi("endSvcLoop() After.. Set loop flag to false");

	AES_AFP_TRACE_MESSAGE("After.. Set loop flag to false");
	//## end aes_afp_apitask::endSvcLoop%3B11E7910025.body
}

//unsigned svcThread(void* apiObj)
void* svcThread(void* apiObj)
{
	//WriteIntoAFPApi("ApiTask Inside svcThread");
	std::string tmpStr("");
	std::string tmpbuf("");
	int result(0);
	int search(0);
	int startPos(0);
	unsigned short codeMsg(0);
	unsigned short messageKind(0);
	unsigned short errorCode(0);
	//unsigned short dataPart(0);
	std::string errEvent("");
	std::string errCmd("");

	aes_afp_apitask *this_ =  reinterpret_cast<aes_afp_apitask*> (apiObj);
	aes_afp_apiinternal* intApiPrt = this_->getInternalAPiPrt();

	AES_AFP_TRACE_MESSAGE("strarted....");
	while (this_->getStopCondition())
	{
		result = this_->receiveFromDsd(tmpStr);
		if (result == -1)
		{			
			bool runFlag = this_->getStopCondition();
			{
			}
			AES_AFP_TRACE_MESSAGE("Receive from DSD failed, result = %d, run loop is %s", result, runFlag?"true":"false");

			this_->close(true); 
			//return AES_NOERRORCODE;
			return NULL;
		}
		if (result == AES_NOSERVERACCESS) //HL17927: To handle when the server is not accessible.
		{
			if( intApiPrt != 0 )
			{
				errEvent = intApiPrt->getErrorCodeText(AES_NOSERVERACCESS);

				this_->putOnEventQueue(errEvent, AES_EVELOSTSERVER);
				errCmd   = intApiPrt->getErrorCodeText(AES_NOSERVERACCESS);
				this_->putOnCmdQueue(errCmd,AES_EVELOSTSERVER,AES_NOSERVERACCESS);
			}
			else
			{
				errEvent = "Unable to connect to server";
				errCmd = "Unable to connect to server";
				this_->putOnEventQueue(errEvent, AES_EVELOSTSERVER);
				this_->putOnCmdQueue( errCmd, AES_EVELOSTSERVER,AES_NOSERVERACCESS);
			}

			AES_AFP_TRACE_MESSAGE("Finish, for AES_NOSERVERACCESS");

			this_->close(true); 
			//return AES_NOSERVERACCESS;
			return NULL;
		}

		// Get kind of message
		startPos = 0;
		search = tmpStr.find_first_of('\n');
		if (search>0)
			tmpStr[search] = ';';
		tmpbuf = tmpStr.substr(startPos, search);
		messageKind = atoi((const char*)tmpbuf.c_str());

		AES_AFP_TRACE_MESSAGE("Received message kind %d from server", messageKind);
		startPos = search +1;

		// Get message or event

		search = tmpStr.find_first_of('\n');
		if (search>0)
			tmpStr[search] = ';';
		tmpbuf = tmpStr.substr(startPos, search-startPos);
		codeMsg = atoi((const char*)tmpbuf.c_str());

		AES_AFP_TRACE_MESSAGE("Received message type %d from server", codeMsg);

		// Get the error code
		startPos = search +1;
		search = tmpStr.find_first_of('\n');
		if (search>0)
			tmpStr[search] = ';';
		tmpbuf = tmpStr.substr(startPos, search-startPos);
		errorCode = atoi((const char*)tmpbuf.c_str());

		// Get the data part
		startPos = search +1;
		search = tmpStr.find_first_of('\n');
		if (search>0)
			tmpStr[search] = ';';
		tmpbuf = tmpStr.substr(startPos, search-startPos);
		/*dataPart = */atoi((const char*)tmpbuf.c_str());

		if (messageKind == COMMAND)
		{
			AES_AFP_TRACE_MESSAGE("Received a command answer");
			std::string str1 = tmpStr.substr(search+1);
			//this_->putOnCmdQueue(tmpStr.substr(search +1), codeMsg, errorCode);
#if 1
			if (errorCode == AES_NOERRORCODE_DETACH)//TR HP85896
				this_->putOnCmdQueue(str1, codeMsg, AES_NOERRORCODE);
			else
#endif
				this_->putOnCmdQueue(str1, codeMsg, errorCode);
		}
		if (messageKind == EVENT)
		{
			AES_AFP_TRACE_MESSAGE("Received an event");
			std::string str2 = tmpStr.substr(search +1);
			this_->putOnEventQueue(str2, codeMsg);

		}
#if 1
		if (errorCode == AES_NOERRORCODE_DETACH) //TR HP85896
		{
			//break;
			ACE_Time_Value tv(0,50000);
			ACE_OS::sleep(tv);
		}
#endif
	}
	AES_AFP_TRACE_MESSAGE("Finish");
	//WriteIntoAFPApi("ApiTask svcThread() :...Finish");
	this_->close(); 
	//return 0;
	return NULL;
}

pthread_t aes_afp_apitask::getThreadId() const
{
	return _threadId;
}

