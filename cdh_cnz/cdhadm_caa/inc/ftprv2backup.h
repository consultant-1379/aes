/*=================================================================== */
/**
	@file   ftprv2backup.h

	@brief
	The services provided by FTPRV2Backup facilitates the backup of data in responding mode.

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
   N/A       27/09/2011     xbhadur       Initial Release
============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef FTPRV2BACKUP_H
#define FTPRV2BACKUP_H
/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <aes_cdh_resultcode.h>
#include <ftprv2.h>
#include <ace/ACE.h>
#include <AES_DBO_TQReceiver.h>

//const ACE_UINT32 APPLID_MAX = 4000000000;

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class BackupWriter;
class BlockPairMapper;

using namespace std;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     FTPRV2Backup

                 The services provided by FTPRV2Backup facilitates the backup of data in responding mode.
 */
/*=================================================================== */
//##ModelId=3DF4840A0106
class FTPRV2Backup : public FTPRV2
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
		                           PUBLIC ATTRIBUTES
	==================================================================== */
	/*=====================================================================
		                        ENUMERATED DECLARATION SECTION
	==================================================================== */
	/*=================================================================== */
	/**
		   @brief  AgentState

		           -
	 */
	/*=================================================================== */
	//##ModelId=3E34E4C7019B
	enum AgentState
	{
		//##ModelId=3E34E4DB01E0
		AGENT_OK,
		//##ModelId=3E34E4E200AA
		AGENT_SWITCH,
		//##ModelId=3E34E4E80062
		AGENT_STOP
	};
	/*=====================================================================
   	     	                        CLASS CONSTRUCTOR
   	==================================================================== */
	/*=================================================================== */
	/**

   	   	      @brief          FTPRV2Backup
   	   						  FTPRV2Backup constructor.

   	   		  @return          void

   	   	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DF4880F02B4
	FTPRV2Backup();
	/*=====================================================================
   	     	                        CLASS DESTRUCTOR
   	==================================================================== */
	/*=================================================================== */
	/**

   	   	      @brief          FTPRV2Backup
   	   						  FTPRV2Backup destructor.

   	   		  @return          void

   	   	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DF4886201CD
	virtual ~FTPRV2Backup();
	/*===================================================================
       	                               ROUTINE DECLARATION SECTION
     =================================================================== */
	/*=================================================================== */
	/**

   					    	      @brief          switchAgent
   					    						  -

   					    		  @param          stopTransfer
   												  -

   					    	      @return         void

   								  @exception      throw()
	 */
	/*=================================================================== */
	//##ModelId=3E34DBA40274
	void switchAgent(bool stopTransfer);


	//##ModelId=3DF485DC01BA
	AES_CDH_ResultCode sendRecordFile(const string& streamName,
                                     const string& streamId,
                                     AES_DBO_DataBlock*& block,
                                     CmdHandler *cmdHdlr,
                                     const ACE_UINT64 ticks);
	/*=================================================================== */
	/**

   					    	      @brief          transactionBegin
   					    						  -

   					    	      @return         AES_CDH_ResultCode	status

   								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF4A6E60144
	AES_CDH_ResultCode transactionBegin();
	/*=================================================================== */
	/**

   					    	      @brief          transactionEnd
   					    						  -

   					    		  @param          translatedBlockNr

   					    	      @return         AES_CDH_ResultCode	status

   								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF4A6F1008C
	AES_CDH_ResultCode transactionEnd(ACE_UINT32 &translatedBlockNr);
	/*=================================================================== */
	/**

   					    	      @brief          transactionCommit
   					    						  -

   					    		  @param          applBlockNr
   												  -

   					    	      @return         AES_CDH_ResultCode	status

   								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF4A734018C
	AES_CDH_ResultCode transactionCommit(ACE_UINT32 &applBlockNr);
	/*=================================================================== */
	/**

   					    	      @brief          getLastCommittedBlock
   					    						  -

   					    		  @param          applBlockNr
												  -

   					    	      @return         AES_CDH_ResultCode	status

   								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF4A7C502C1
	AES_CDH_ResultCode getLastCommittedBlock(ACE_UINT32 &applBlockNr);
	/*=================================================================== */
	/**

    					    	      @brief          transactionTerminate
    					    						  -

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF4A7460319
	AES_CDH_ResultCode transactionTerminate();
	/*=================================================================== */
	/**

    					    	      @brief          getAgentType
    					    						  -

    					    	      @return         TransferAgentAttributes

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF8AA4602A2
	TransferAgent::TransferAgentAttributes getAgentType();
	/*=================================================================== */
	/**

    					    	      @brief          define
    					    						  -

    					    		  @param          argc
    												  -
    					      		  @param          argv
    												  -
    					    	      @param          define
    												  -
    					      		  @param          recovery
    												  -

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3E2C09D902E1
	AES_CDH_ResultCode define(ACE_INT32 &argc,
			ACE_TCHAR *argv[],
			const bool define,
			const bool recovery);
	/*=================================================================== */
	/**

    					    	      @brief          setAPIClosed
    					    						  -

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3E2FEB020194
	AES_CDH_ResultCode setAPIClosed();

	/*=====================================================================
                           PRIVATE DECLARATION SECTION
   ==================================================================== */
private:
	/*====================================================================
   		                       PRIVATE ATTRIBUTES
   	==================================================================== */

	/*=================================================================== */
	/**
   			      @brief   backupWriter
   						   -
	 */
	/*=================================================================== */
	//##ModelId=3E4396EA0111
	BackupWriter* backupWriter;
	/*=================================================================== */
	/**
      			      @brief   apiClosed
      						   -
	 */
	/*=================================================================== */
	//##ModelId=3E2FEC2C0156
	bool apiClosed;
	/*=================================================================== */
	/**
        			      @brief   oldRecordNumber
        						   -
	 */
	/*=================================================================== */
	//##ModelId=3E30345703AB
	ACE_UINT64  oldRecordNumber;
	/*=================================================================== */
	/**
      			      @brief   switchState
      						   -
	 */
	/*=================================================================== */
	//##ModelId=3E34E5C10385
	AgentState switchState;
	/*=================================================================== */
	/**
      			      @brief   isOpen
      						   -
	 */
	/*=================================================================== */
	//##ModelId=3E82FE34022F
	bool isOpen;
	/*=================================================================== */
	/**
      			      @brief   badFTPArea
      						   -
	 */
	/*=================================================================== */
	//##ModelId=3E82EF1103DC
	bool badFTPArea;
	/*=================================================================== */
	/**
      			      @brief   bCease
      						   -
	 */
	/*=================================================================== */
	//##ModelId=3E82FE290374
	bool bCease;

	//##ModelId=3DF4B74A0089
	 AES_DBO_TQReceiver *TQrecv;
	/*=================================================================== */
	/**
      			      @brief   transMutex
      						   -
	 */
	/*=================================================================== */
	//##ModelId=3DF5F81B001C
	ACE_Mutex transMutex;

	//##ModelId=3DF495FD00B3
	 BlockPairMapper *bpMapper;
	/*===================================================================
       	                               ROUTINE DECLARATION SECTION
       =================================================================== */
	/*=================================================================== */
	/**

    					    	      @brief          publishBackupFile
													  -

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3E4A635B008E
	AES_CDH_ResultCode publishBackupFile();
	/*=================================================================== */
	/**

    					    	      @brief          writeToFile
    					    						  -

    					    		  @param          block
    												  -

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3E82FE4901B7
	 AES_CDH_ResultCode writeToFile(AES_DBO_DataBlock *block);
	/*=================================================================== */
	/**

    					    	      @brief          open
    					    						  -

    					    		  @param          streamName
    												  -
    					      		  @param          sourceId
    												  -

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF488C90207
	AES_CDH_ResultCode open(const string &streamName, const string &streamId, string &sourceId);
	/*=================================================================== */
	/**

    					    	      @brief          close

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DF488D0006D
	AES_CDH_ResultCode close();

	/*=================================================================== */
	/**

    					    	      @brief          sendEvent
    					    						  -

    					    		  @param          cmdHdlr
    												  -
    					      		  @param          ticks
    												  -

    					    	      @return         void

    								  @exception      none
	 */
	/*=================================================================== */
	// UABCHSN: Added for trouble report HD83326
	void sendEvent(CmdHandler *cmdHdlr, const ACE_UINT64 ticks);

	/*=====================================================================
                           PUBLIC DECLARATION SECTION
   ==================================================================== */
public:
	/*===================================================================
       	                               ROUTINE DECLARATION SECTION
     =================================================================== */
	/*=================================================================== */
	/**

    					    	      @brief          getDestSetName
    					    						  -

    					    	      @return         string		-

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3E43A076031E
	const string getDestSetName();
	/*=================================================================== */
	/**

    					    	      @brief          remove

    					    	      @return         AES_CDH_ResultCode	status

    								  @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3E34F19B0193
	AES_CDH_ResultCode remove();
};

#endif
