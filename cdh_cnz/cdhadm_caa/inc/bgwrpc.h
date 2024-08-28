
//******************************************************************************
// 
// .NAME
//      BGWRPC - BGw RPC Protocol Handler
// .LIBRARY 3C++
// .PAGENAME BGWRPC
// .HEADER  AES
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE bgwrpc.h

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
//  Handles RPC parts specific to the Billing Gateway (BGw)
//  implementation.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  19089-CAA 109 1302

// AUTHOR 
//  1999-07-05 by I/LD Gunnar Andersson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010530  qabhall Method checkConnection added INGO3 GOH. 

// .LINKAGE
//  -

// .SEE ALSO 
//  CDHRPC

//******************************************************************************
#ifndef BGWRPC_H 
#define BGWRPC_H

#include <rpc_definitions.h>  
#include <cdhrpc.h>
#include <aes_cdh_resultcode.h>
#include <blockpairmapper.h>            // INGO4 GOH
#include <transactionrules.h>           // INGO4 GOH
#include <string>
#include <aes_gcc_util.h>
#include <acs_apgcc_omhandler.h>

#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <aes_cdh_rpc.h>


using namespace std;

const string BGWRPCType = "BGWRPC";

#define AES_CDH_BGW_RPC_ProgramNumber 770703857
#define AES_CDH_BGW_RPC_Timeout       20 

/*====================================================================*/
/**
 *      @brief READY_MAX_FILENUM
 */
/*==================================================================== */
#define BGWRPC_STREAM_SIZE	"rpcStreamSize"

#define BGWRPC_PGM_NUMBER	"rpcProgramNumber"

#define BGWRPC_TIMEOUT		"rpcTimeout"




class  BGWRPC : public CDHRPC
{
public:
	
	//##ModelId=3DE49B4C02CB
    BGWRPC();
    
	//##ModelId=3DE49B4C02D5
    virtual ~BGWRPC();
    
	//##ModelId=3DE49B4C02E0
    TransferAgent* clone(void);
    
    // INGO4 GOH begin
	//##ModelId=3DE49B4C02F3
    AES_CDH_ResultCode transactionBegin();

	//##ModelId=3DE49B4C02FD
    AES_CDH_ResultCode transactionEnd(unsigned int &translatedBlockNr);

	//##ModelId=3DE49B4C0308
    AES_CDH_ResultCode transactionCommit(unsigned int &applBlockNr);

	//##ModelId=3DE49B4C0312
    AES_CDH_ResultCode getLastCommittedBlock(unsigned int &applBlockNr);

	//##ModelId=3DE49B4C031C
    AES_CDH_ResultCode transactionTerminate();


     OmHandler omHandlerObj;
     OmHandler myomHandler;

	//##ModelId=3DE49B4C0325
    AES_CDH_ResultCode define(int &argc,
                              char *argv[],
                              const bool define,
                              const bool recovery);

	//##ModelId=3DE49B4C0330
    AES_CDH_ResultCode remove();

	//##ModelId=3DE49B4C0339
    AES_CDH_ResultCode changeAttr(TransferAgent *agent);

    //##ModelId=3E2E902E00CB
	void switchAgent(bool stopTransfer);

	//##ModelId=3E799AC70354
	bool dataTransfer();

	ACE_UINT32 rpcPgmNumber;
	ACE_UINT32 rpcTimeout;
	ACE_UINT32 rpcStreamSize;

private:

    BGWRPC* bgwrpc;
    //HCLIENT* client;  //llv5
    void* client;    //llv5
    BlockInfo cdhOutVar;
    string mainFileName;
    string subFileName;
    char fileName[AES_IFO_FILE_NAME_SIZE + 1];
    unsigned long int bgwProgramNumber;
    unsigned long int bgwTimeout;

    BlockPairMapper* bpMapper;
    TransactionRules transRules;
    ACE_Recursive_Thread_Mutex* transMutex;
    unsigned long oldRecordNumber;
    
    string clusterName;
    ACE_Manual_Event* BGWRPCopen;
    bool inBGWRPCopen;

    AES_CDH_ResultCode checkConnection(void); // INGO3 GOH qabhall
    
    AES_CDH_ResultCode openForCheckConnection(); // INGO3 GOH qabulfg
    
    AES_CDH_ResultCode open(int& argc, char* argv[], unsigned int &blockFromOpen);
    AES_CDH_ResultCode openAfterChange();
    
    void close(void);
    void close_internal(void);

    AES_CDH_ResultCode sendRecord(void* buf, unsigned long bufSize, 
                                  unsigned long recordNumber);

    void getType(string& type);

    ACE_INT32 readCDHBgwrpcParams();
    
    AES_CDH_ResultCode updateBlockID(void);

    AES_CDH_ResultCode lastTransaction(unsigned int &CDRBid);

	void changeAfterTerminate();

	AES_CDH_ResultCode getAttributes(string& transferType, vector<string>& attr);

   AES_CDH_ResultCode getSingleAttr(const char* opt, void* value);

};

#endif // BGWRPC_H
