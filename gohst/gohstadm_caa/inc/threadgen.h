//	INCLUDE <file>
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//	  All rights reserved.
//
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
//
//	      General rule:
//	      The error handling is specified for each method.
//
//	      No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	      190 89-CAA 109 xxxx
//
//	AUTHOR
//	      1999-04-	UAB/I/LN	xxxx
//
//	REVISION
//	      PA1  2001-06-13  xxxx  Prel. version.
//
//	LINKAGE
//
//	SEE ALSO
#ifndef threadGenh_h
#define threadGenh_h 1

#include "aes_ohi_filehandler.h"
#include "aes_ohi_directoryhandler.h"
#include "aes_ohi_extfilehandler2.h"
#include "aes_ohi_blockhandler2.h"
#include <ace/ACE.h>
#include <ace/Thread_Manager.h>
#include <ace/OS_NS_sys_time.h>
#include <ace/Time_Value.h>
//------------------------------
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/OS_NS_dlfcn.h>
#include <ace/Reactor.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Signal.h>
#include <ace/Get_Opt.h>
#include <ace/streams.h>
#include <ace/OS_NS_errno.h>
#include <ace/SString.h>
#include <ace/OS_NS_sys_wait.h>
#include <ace/OS_NS_unistd.h>
#include <ace/OS_NS_sys_stat.h>
#include <ace/OS_NS_sys_resource.h>
#include <ace/OS_NS_fcntl.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
//----------------------------
#include <ctime>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void thredFunc(void* p_arg);

struct structInfoCPBlock
{
	std::string strIdName;
	aes_ohi_blockHandler2_r1* myBlockSender;
};

class threadGen : public AES_OHI_ExtFileHandler2
{
public:
	threadGen();
	
	threadGen (ACE_INT32 recLength, ACE_INT32 nrOfRec, ACE_INT32 transActLength, ACE_INT32 freq, const std::string evText, ACE_INT32 fDestNr, ACE_INT32 transtype, const std::string newpath, const std::string streamID);
	
	virtual ~threadGen();

	void fillPattern (ACE_TCHAR* writebuff, ACE_INT32 fileNr, ACE_INT32 recordLength);
	
	bool GenerateFile (const std::string path, const std::string FileName, ACE_INT32 nrOfrecords, ACE_INT32 recordLenght, bool& exist);
	
	bool threadStart ();
	
	void resume ();
	
	void suspend ();
	
	void terminate ();
	
	void logfile (ACE_TCHAR* logtext);
	
	std::string copy_file( std::string destination, std::string source );
	
	bool copy_dir_tree(std::string refpath, std::string s);
	
	bool showResult(std::string operation, ACE_UINT32 res);
	
	std::string createPath(std::string p);
	
	std::string checkPath(std::string p);
	
	std::string lastDir(std::string p);
	
	void setDebugMode(bool mode){debugEnabled_ = mode;}
	
	bool getDebugMode(){return debugEnabled_;}
	
	void addSentBytes(ACE_INT32 bytes){nrOfSentBytes_ += bytes;}
	
	void removeSentBytes(ACE_INT32 bytes){nrOfSentBytes_ -= bytes;}
	
	void clearSentBytes(){nrOfSentBytes_ = 0;}
	
	ACE_INT64 sentBytes(){return nrOfSentBytes_;}

	void addTransactionSentBytes(ACE_INT32 transbytes){nrOfTransactionSentBytes_ += transbytes;}
	
	void removeTransactionSentBytes(ACE_INT32 transbytes){nrOfTransactionSentBytes_ -= transbytes;}
	
	void clearTransactionSentBytes(){nrOfTransactionSentBytes_ = 0;}
	
	ACE_INT64 TransactionSentBytes(){return nrOfTransactionSentBytes_;}
	
	ACE_INT32 frequency_;
	ACE_INT32 recordLength_;
	ACE_INT32 nrOfRecords_;
	ACE_INT32 transActLength_;
	ACE_INT32 transType_;
	ACE_INT32 name_;
	ACE_INT32 ifoRpc_;
	ACE_INT32 partOfDir_;
	ACE_INT32 lastSentFileNr_;
	ACE_INT32 m_nNumCP_Block; //Amount of Simulate CP in the Block Sender case
	
	bool sendFile_;
	bool sendDir_;
	bool sendBlock_;
	bool suspendAtStart_;
	
	std::string logPath_;
	std::string eventText_;
	std::string streamID_;
	std::string path_;
	std::string refPath_;
	std::string sendMask_;
	
protected:

private: 
	bool terminated_;
	
	ACE_thread_t hThread_;
        ACE_Thread_Manager *ace_thr_mgr;
	
	ACE_UINT64 threadId_;
	
	ACE_thread_mutex_t MsgCriticalSection;
	
	ACE_INT32 fileDestNr_;
	
	bool debugEnabled_;
	
	ACE_INT64 nrOfSentBytes_;
	ACE_INT64 nrOfTransactionSentBytes_;
	ACE_Recursive_Thread_Mutex theRecursiveThreadMutex;
	ACE_Condition<ACE_Recursive_Thread_Mutex> theMutexCondition;
};
#endif
