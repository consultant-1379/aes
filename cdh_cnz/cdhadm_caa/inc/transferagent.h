/*=================================================================== */
/**
   @file   transferagent.h

   @brief Header file for CDH module.

          This module contains all the declarations useful to
          specify the TransferAgent class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       13/04/2012   XCHEMAD   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef TRANSFERAGENT_H 
#define TRANSFERAGENT_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>
#include <aes_cdh_resultcode.h>
#include <aes_cdh_destinationset.h>
#include <AES_DBO_DataBlock.h>
#include <cmdhandler.h>
#include <ace/ACE.h>
using namespace std;

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class TransDest;

class  TransferAgent
{

public:
	enum TransferAgentAttributes 
	{
        FILE       = 0x1,
        BLOCK      = 0x2,
        BACKUP     = 0x4,
        FILEBACKUP = 0x5
	};


	virtual TransferAgentAttributes getAgentType() = 0;

    virtual ~TransferAgent() {}

	virtual bool isInstanceClosed() {return false; };
    static AES_CDH_ResultCode create(TransDest* destPtr, 
                                     const string transferType, 
                                     ACE_INT32& argc,
                                     ACE_TCHAR* argv[],
                                     const bool define,
                                     const bool recovery,
                                     TransferAgent*& trans);

    virtual AES_CDH_ResultCode remove(void) = 0;

    virtual AES_CDH_ResultCode sendFile(const string& fileName,
                                        const string& remoteSubDirName="",
                                        const string& newFileName="",
                                        AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY) = 0;// INGO4 GOH qabhefe
    virtual AES_CDH_ResultCode sendDirectory(const string& dirName,
                                             const string& remoteSubDirName="",
                                             const string& newDirName="",
                                             AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY,
                                             const string& fileMask="") = 0;// INGO4 GOH qabhefe& 

    virtual AES_CDH_ResultCode checkConnection(void) = 0;
  
    virtual AES_CDH_ResultCode sendRecordFile(const string& streamName,
                                              const string& streamId,
                                              AES_DBO_DataBlock*& block,
                                              CmdHandler *cmdHdlr,
                                              const ACE_UINT64 ticks) = 0;


    virtual AES_CDH_ResultCode getAttributes(string& transferType, 
                                             vector<string>& attr) = 0;

    virtual AES_CDH_ResultCode getSingleAttr(const ACE_TCHAR* opt, void* value) = 0;


    virtual AES_CDH_ResultCode transactionBegin() = 0;

    virtual AES_CDH_ResultCode transactionEnd(ACE_UINT32 &translatedBlockNr) = 0;

    virtual AES_CDH_ResultCode transactionCommit(ACE_UINT32 &applBlockNr) = 0;

    virtual AES_CDH_ResultCode getLastCommittedBlock(ACE_UINT32 &applBlockNr) = 0;

    virtual AES_CDH_ResultCode transactionTerminate() = 0;

    virtual AES_CDH_ResultCode setAPIClosed() = 0;

    virtual AES_CDH_ResultCode changeAttr(TransferAgent *agent) = 0;

	virtual void switchAgent(bool stopTransfer) = 0;

	virtual std::string getVDDestPath() = 0;

	virtual bool dataTransfer(void) = 0;

	virtual AES_CDH_ResultCode readVDPath( std::string& path) = 0;

	TransDest* destinationPtr;

protected:

  string destinationName;


  virtual AES_CDH_ResultCode define(ACE_INT32& argc,
                                    ACE_TCHAR* argv[],
                                    const bool define,
                                    const bool recovery) = 0;

  static void lowerToUpper(string& str);

  static void upperToLower(string& str);

  static void cleanUpArgv(ACE_INT32& argc, ACE_TCHAR* argv[], ACE_INT32 firstOper);

  bool getClusterName(string& name);

};

 
#endif // TRANSFERAGENT_H
