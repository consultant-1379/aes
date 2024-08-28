
//******************************************************************************
// 
// .NAME
//      CDHRPC - Common RPC API
// .LIBRARY 3C++
// .PAGENAME CDHRPC
// .HEADER  AES
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CDHRPC.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//  A common API for different RPC types.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  190 89-CAA 109 0507

// AUTHOR 
//  2002-12-13 by UAB/UKY/AU Hans-Erik Nilsson

// .LINKAGE
//  -

// .SEE ALSO 
//  TransferAgent

//******************************************************************************
#ifdef WIN32
#pragma warning(disable:4231)
#endif

#ifndef CDHRPC_H 
#define CDHRPC_H

//#include <osf/Synch.h>
#include <cstdlib>
#include <cstdio>
#include "aes_cdh_rpc.h"  
#include <transferagent.h>
#include <aes_cdh_resultcode.h>
#include <aes_gcc_stream.h>
#include <aes_cdh_destination.h>
#include <aes_cdh_destinationset.h>
#include <string>
#include <map>
#include <ace/Event_Handler.h>
#include <ace/Thread_Manager.h>
#include "rpc_definitions.h"


#include <AES_DBO_TQManager.h>		 // INGO4 GOH
#include <AES_DBO_TQReceiver.h>          // INGO4 GOH


using namespace std;

//##ModelId=3DE49B4801CA
class  CDHRPC : public TransferAgent
{

 public:

	// INGO4 GOH start
	//##ModelId=3DE49B48029E
	TransferAgent::TransferAgentAttributes  getAgentType();


	//##ModelId=3DE49B4801FD
  CDHRPC();

	//##ModelId=3DE49B480210
  virtual ~CDHRPC();

#if 0  // Not used any where in the code.
  static int GetHWVersion();
#endif

	//##ModelId=3DE49B48021A
  AES_CDH_ResultCode define(int& argc,
                            char* argv[],
                            const bool define,
                            const bool recovery);

	//##ModelId=3DE49B480225
  AES_CDH_ResultCode remove(void);

	//##ModelId=3DE49B480239
  AES_CDH_ResultCode sendFile(const string& fileName,
                              const string& remoteSubDirName="",
                              const string& newFileName="",
                              AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY); // INGO3 GOH qabhall
	// Return value: 
	//    AES_CDH_RC_OK
	//    AES_CDH_RC_FILEERR
	//    AES_CDH_RC_FILENAMETOOLONG
	//    AES_CDH_RC_TASKSTOPPED
	//    AES_CDH_RC_INTPROGERR
	//    AES_CDH_RC_CONNECTERR
	//    AES_CDH_RC_SENDERR

	//##ModelId=3DE49B48024D
  AES_CDH_ResultCode sendRecordFile(const string& streamName,
                                    const string& streamId,
                                    AES_DBO_DataBlock*& block,
                                    CmdHandler *cmdHdlr,
                                    const ACE_UINT64 ticks);
  // Return value: 
  //    AES_CDH_RC_OK
  //    AES_CDH_RC_FILEERR
  //    AES_CDH_RC_FILENAMETOOLONG
  //    AES_CDH_RC_CONNECTERR
  //    AES_CDH_RC_CONNECTWARN
  //    AES_CDH_RC_SENDERR
  //    AES_CDH_RC_SENDWARN
  //    AES_CDH_RC_INTPROGERR
  //    AES_CDH_RC_ENDOFSTREAM

    AES_CDH_ResultCode intSendRecordFile(const string& streamName,
                                    const string& streamId,
                                    AES_DBO_DataBlock*& block,
                                    CmdHandler *cmdHdlr,
                                    const ACE_UINT32 ticks);
  // Return value: 
  //    AES_CDH_RC_OK
  //    AES_CDH_RC_FILEERR
  //    AES_CDH_RC_FILENAMETOOLONG
  //    AES_CDH_RC_CONNECTERR
  //    AES_CDH_RC_CONNECTWARN
  //    AES_CDH_RC_SENDERR
  //    AES_CDH_RC_SENDWARN
  //    AES_CDH_RC_INTPROGERR
  //    AES_CDH_RC_ENDOFSTREAM

	//##ModelId=3DE49B48026B
	virtual AES_CDH_ResultCode getAttributes(string& transferType, vector<string>& attr) = 0;

   virtual AES_CDH_ResultCode getSingleAttr(const char* opt, void* value) = 0;

	//##ModelId=3DE49B48026B
	// Return value: 
	//    AES_CDH_RC_OK
	//    AES_CDH_RC_FILEERR
	//    AES_CDH_RC_FILENAMETOOLONG
	//    AES_CDH_RC_CONNECTERR
	//    AES_CDH_RC_CONNECTWARN
	//    AES_CDH_RC_SENDERR
	//    AES_CDH_RC_SENDWARN
	//    AES_CDH_RC_INTPROGERR
	//    AES_CDH_RC_ENDOFSTREAM

  // INGO3 GOH qabhall start
	//##ModelId=3DE49B48027F
  AES_CDH_ResultCode sendDirectory(const string& dirName,
                                   const string& remoteSubDirName="",
                                   const string& newDirName="", 
                                   AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY,
                                   const string& fileMask="");

	//##ModelId=3DE49B480293
  virtual AES_CDH_ResultCode checkConnection(void) = 0;

// INGO3 GOH qabhall end

	//##ModelId=3DE49B4802A8
    AES_CDH_ResultCode transactionBegin();

	//##ModelId=3DE49B4802B1
    AES_CDH_ResultCode transactionEnd(unsigned int &translatedBlockNr);

	//##ModelId=3DE49B4802C5
    AES_CDH_ResultCode transactionCommit(unsigned int &applBlockNr);

	//##ModelId=3DE49B4802CF
    AES_CDH_ResultCode getLastCommittedBlock(unsigned int &applBlockNr);

	//##ModelId=3DE49B4802D9
    AES_CDH_ResultCode transactionTerminate();

	//##ModelId=3DE49B4802DA
    AES_CDH_ResultCode setAPIClosed();

	//##ModelId=3DE49B4802E3
    AES_CDH_ResultCode changeAttr(TransferAgent *agent);

    // INGO4 GOH end

	//##ModelId=3E3E765A0096
	std::string getVDDestPath();

	AES_CDH_ResultCode readVDPath(std::string& path);

protected:

	//##ModelId=3E2E905802C0
	enum AgentState 
	{
	
		//##ModelId=3E2F99A102BA
		AGENT_OK,

		//##ModelId=3E2F99A80288
		AGENT_SWITCH,

		//##ModelId=3E2F99AD0330
		AGENT_STOP
	};

	//##ModelId=3DE49B4802EE
  bool paramError;
	//##ModelId=3DE49B48030C
  string hostAddress;
  string blockSendRetries;
  string blockRetryDelay;

  //##ModelId=3E2E93350378
  AgentState switchState;

//  virtual AES_CDH_ResultCode open(int& argc, char* argv[]) = 0;
	//##ModelId=3DE49B48033D
  virtual AES_CDH_ResultCode open(int& argc, char* argv[], unsigned int &blockFromOpen) = 0;

	//##ModelId=3DE49B48035B
  virtual void close(void) = 0;
  //virtual void close_internal(void) = 0;

	//##ModelId=3DE49B480366
  virtual AES_CDH_ResultCode sendRecord(void* buf,
                                        unsigned long bufSize,
                                        unsigned long recordNumber) = 0;

	//##ModelId=3DE49B48038D
  virtual void getType(string& type) = 0;

  //##ModelId=3E79904D02D5
  virtual void changeAfterTerminate() = 0;


	//##ModelId=3DE49B4803A2
  AES_DBO_TQReceiver* TQrecv;      // INGO4 GOH
	//##ModelId=3DE49B4803E7
  bool bCease;


	//##ModelId=3DE49B49001D
  bool bTerminate;          // true if transactionTerminate()
	//##ModelId=3DE49B49003B
  bool bCommit;             // true if transactionCommit();
	//##ModelId=3DE49B490059
  bool apiClosed;           // true if the api has been closed

  ACE_Manual_Event* hEvAbortRead;		// handle to event for aborting read operation

  ACE_Manual_Event* hEvInstanceClosed;

  //HMODULE hRpcLib;

  friend class AES_CDH_Contactor;

private:

   //##ModelId=3DE49B4900D3
   static multimap< string, CDHRPC* > masterCopyMap;
   //##ModelId=3DE49B490105
   static ACE_Thread_Mutex masterCopyMutex;

   //  AES_GCC_Stream stream;
   //##ModelId=3DE49B49012C
   bool streamOpened;
   //##ModelId=3DE49B49014A
   bool retrySendRecord;
   //##ModelId=3DE49B490168
   int bytesRead;
#if 0              // Not Used Anywhere
   //##ModelId=3DE49B490186
   char buffer[AES_IFO_BLOCK_SIZE];
   //##ModelId=3DE49B4901A4
   unsigned int currentRecordNumber;
#endif
   //##ModelId=3DE49B4901D9
   CDHRPC* parentCopy;

   bool continueRetries;
   int blockSendRetries_int;
   int blockSendRetries_int_done;
   int blockSendRetries_int_todo;

   //##ModelId=3DE49B490280
   AES_CDH_ResultCode parseRPC(int& argc, char* argv[]);

   void sendEvent(CmdHandler *cmdHdlr, const ACE_UINT32 ticks);
};

#endif // CDHRPC_H
