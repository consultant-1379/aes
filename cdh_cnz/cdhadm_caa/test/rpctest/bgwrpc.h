
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
#pragma warning(disable:4231)
#ifndef BGWRPC_H 
#define BGWRPC_H

#include "cdhrpc.h"
#include "aes_cdh_rpc.h"
#include "aes_cdh_resultcode.h"
#include "blockpairmapper.h"            // INGO4 GOH
#include "transactionrules.h"           // INGO4 GOH
#include <string>
#include "aes_gcc_mutex.h"

using namespace std;

//const string BGWRPCType = "bgwrpc";
const string BGWRPCType = "BGWRPC";

class  BGWRPC : public CDHRPC
{
public:
    
    BGWRPC();
    
    virtual ~BGWRPC();
    
    TransferAgent* clone(void);
    
    // INGO4 GOH begin
    AES_CDH_ResultCode transactionBegin();

    AES_CDH_ResultCode transactionEnd(unsigned int &translatedBlockNr);

    AES_CDH_ResultCode transactionCommit(unsigned int &applBlockNr);

    AES_CDH_ResultCode getLastCommittedBlock(unsigned int &applBlockNr);

    AES_CDH_ResultCode transactionTerminate();

    AES_CDH_ResultCode define(int &argc, char *argv[]);

    AES_CDH_ResultCode remove();

    // INGO4 GOH end

private:

    HCLIENT* client;
    BlockInfo cdhOutVar;
    string mainFileName;
    string subFileName;
    char fileName[AES_IFO_FILE_NAME_SIZE + 1];
    unsigned long int bgwProgramNumber;
    unsigned long int bgwTimeout;

    BlockPairMapper *bpMapper;
    TransactionRules transRules;
    AES_GCC_Mutex transMutex;
    unsigned long oldRecordNumber;
    
    AES_CDH_ResultCode checkConnection(void); // INGO3 GOH qabhall
    
    AES_CDH_ResultCode BGWRPC::openForCheckConnection(); // INGO3 GOH qabulfg
    
    AES_CDH_ResultCode open(int& argc, char* argv[]);
    
    void close(void);
    
    AES_CDH_ResultCode sendRecord(void* buf, unsigned long bufSize, 
                                  unsigned long recordNumber);

    void getType(string& type);
    
    bool getClusterName(string& name);
    
    AES_CDH_ResultCode updateBlockID(void);

    AES_CDH_ResultCode lastTransaction(unsigned int &CDRBid);

};

#endif // BGWRPC_H
