//******************************************************************************
// 
// .NAME
//  	SSHFTPRV2Backup
// .LIBRARY 3C++
// .PAGENAME SSHFTPRV2Backup
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE sshftprv2backup.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
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
// 	<General description of the class>

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  

// AUTHOR 
//  2003-01-14 by UAB/UKY/AU Hans-Erik Nilsson
//	2007-12-14 by XCSAPVA/Aparna Vadala	for SFTP Responding IP

// .LINKAGE
//	<link information: libraries needed and order>

// .SEE ALSO
// 	<Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef SSHFTPRV2BACKUP_H
#define SSHFTPRV2BACKUP_H

#include <string>
#include <aes_cdh_resultcode.h>
#include <sshftprv2.h>
#include <ace/ACE.h>

#include <AES_DBO_TQManager.h>
#include <AES_DBO_TQReceiver.h>

class BackupWriter;
class BlockPairMapper;

using namespace std;


class SSHFTPRV2Backup : public SSHFTPRV2
{
public:

   enum AgentState 
   {

      AGENT_OK,

      AGENT_SWITCH,

      AGENT_STOP
   };


   void switchAgent(bool stopTransfer);


   SSHFTPRV2Backup();


   virtual ~SSHFTPRV2Backup();

  AES_CDH_ResultCode sendRecordFile(const string& streamName,
                                     const string& streamId,
                                     AES_DBO_DataBlock*& block,
                                     CmdHandler *cmdHdlr,
                                     const ACE_UINT64 ticks);

   AES_CDH_ResultCode transactionBegin();


   AES_CDH_ResultCode transactionEnd(ACE_UINT32 &translatedBlockNr);


   AES_CDH_ResultCode transactionCommit(ACE_UINT32 &applBlockNr);


   AES_CDH_ResultCode getLastCommittedBlock(ACE_UINT32 &applBlockNr);


   AES_CDH_ResultCode transactionTerminate();


   
   TransferAgent::TransferAgentAttributes getAgentType();

   
   AES_CDH_ResultCode define(ACE_INT32 &argc,
                             ACE_TCHAR *argv[],
                             const bool define,
                             const bool recovery);

   
   AES_CDH_ResultCode setAPIClosed();

private:
   BackupWriter* backupWriter;
   
   bool apiClosed;
   
   ACE_UINT64 oldRecordNumber;
   
   AgentState switchState;
   
   bool isOpen;
   
   bool badFTPArea;

   
   bool bCease;


   AES_CDH_ResultCode publishBackupFile();
   AES_CDH_ResultCode writeToFile(AES_DBO_DataBlock *block);

   AES_CDH_ResultCode open(const string &streamName, const string &streamId, string &sourceId);

   
   AES_CDH_ResultCode close();

   
   AES_DBO_TQReceiver *TQrecv; 

   
   ACE_Mutex transMutex;

   
   BlockPairMapper *bpMapper; 

   

   
   void sendEvent(CmdHandler *cmdHdlr, const ACE_UINT64 ticks);

public:
   
   const string getDestSetName();

   
   AES_CDH_ResultCode remove();
};

#endif
