#ifndef AES_AFP_APITASK_H
#define AES_AFP_APITASK_H 1

#include "aes_afp_api.h"
#include "aes_afp_apiinternal.h"
#include "aes_afp_apimsg.h"
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_eventcodes.h>
#define AES_AFP_MAXBUFFER 65000

class aes_afp_apitask 
{
public:
   aes_afp_apitask(ACS_DSD_Session* conversionSession,
                   aes_afp_apiinternal* apiinternal,
                   AES_AFP_Api* apiPointer);

   virtual ~aes_afp_apitask();

   int open(void* args = 0, ACE_Thread_Manager* thrMgr = 0);

   int close(bool dsdSessionClose = false);

   void endSvcLoop();

   void getHandle(ACE_event_t& th)const { th=treadTerminated_;};

   int receiveFromDsd(std::string& dsdBuffer);

   int putOnEventQueue(std::string& dataString, unsigned short message);

   int putOnCmdQueue(std::string& dataString, unsigned short message, unsigned short errorCode);

   bool getStopCondition();

   aes_afp_apiinternal* getInternalAPiPrt() {return internalPtr_;};

   //ACE_thread_t getThreadId() const;
   pthread_t getThreadId() const;

private: 
	
   ACE_thread_mutex_t lockCondition;

   ACE_event_t treadTerminated_;

   size_t stackSize;

   ACE_hthread_t _threadHandle;

   //ACE_thread_t _threadId;
   pthread_t _threadId;

   bool runUntilDoomsDay_;

   ACS_DSD_Session *convSession_;

   aes_afp_apiinternal *internalPtr_;

   AES_AFP_Api *apiPtr_;

   Ace_MessageQueue *eventQueue_;

   Ace_MessageQueue *cmdQueue_;

};

#endif
