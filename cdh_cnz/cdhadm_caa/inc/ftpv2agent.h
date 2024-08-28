/*=================================================================== */
/**
	@file   ftpv2agent.h

	@brief
	Handles FTP parts common for FTP Version 2.
	The source code is adapted for WinNT only.

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
#ifndef FTPV2AGENT_H
#define FTPV2AGENT_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <transferagent.h>
#include <aes_cdh_destination.h>
#include <aes_cdh_resultcode.h>
#include <ace/Semaphore.h>
#include <string>
#include <ace/ACE.h>
#include <AES_DBO_DataBlock.h>

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  FTPTypeV2

           -
 */
/*=================================================================== */
const string FTPTypeV2 = "FTPV2";
/*=================================================================== */
/**
   @brief  SFTPTypeV2

           -
 */
/*=================================================================== */
const string SFTPTypeV2 = "SFTPV2";
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     FTPV2Agent

                 Handles FTP parts common for FTP Version 2.
 */
/*=================================================================== */
//##ModelId=3DE49B4203DF
class  FTPV2Agent : public TransferAgent
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
		     	                        CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

		   	      @brief          FTPV2Agent
		   						  FTPV2Agent constructor.

		   		  @return          void

		   	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B430034
	FTPV2Agent();
	/*=====================================================================
  	 	     	                        CLASS DESTRUCTOR
  	 ==================================================================== */
	/*=================================================================== */
	/**

  	 	   	      @brief          ~FTPV2Agent
  	 	   						  FTPV2Agent destructor.

  	 	   		  @return          void

  	 	   	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B43003D
	virtual ~FTPV2Agent();
	/*===================================================================
  	 	                               ROUTINE DECLARATION SECTION
  	 =================================================================== */
	/*=================================================================== */
	/**

  					    	      @brief          create
  					    						  -

  					    		  @param          destName
  												  destination Name
								  @param          transferType
  												  transfer Type
								  @param          argc
  												  -
								  @param          argv[]
  												  -
								  @param          trans
  												  -

  					    	      @return         AES_CDH_ResultCode		gives the status of create

  					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B430047
	static AES_CDH_ResultCode create(const string destName,
			const string transferType,
			ACE_INT32& argc,
			ACE_TCHAR* argv[],
			TransferAgent*& trans);
	/*=================================================================== */
	/**

    					    	      @brief          sendRecordFile
    					    						  return codes from sendRecordFile

    					    		  @param          streamName
													  stream Name
									  @param          streamId
													  stream Id
									  @param          block
													  block
									  @param          cmdHdlr
													  command handler
									  @param          ticks
													  ticks

    					    	      @return         AES_CDH_ResultCode		gives the status of sendRecordFile

    					    	      @exception      none
	 */
	/*=================================================================== */
	// AES_CDH_RC_INCUSAGE (this method do not exist for ftp)
	//##ModelId=3DE49B430070
	AES_CDH_ResultCode sendRecordFile(const string& streamName,
                                    const string& streamId,
                                    AES_DBO_DataBlock*& block,
                                    CmdHandler *cmdHdlr,
                                    const ACE_UINT64 ticks);
	/*=================================================================== */
	/**

    					    	      @brief          transactionBegin
    					    						  -

    					    	      @return         AES_CDH_ResultCode		Gives the status of transactionBegin

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B43008D
	AES_CDH_ResultCode transactionBegin();
	/*=================================================================== */
	/**

    					    	      @brief          transactionEnd
    					    						  -

    					    		  @param          translatedBlockNr
    												  translated Block Number

    					    	      @return         AES_CDH_ResultCode		Gives the status of transactionEnd

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B43008E
	AES_CDH_ResultCode transactionEnd(ACE_UINT32 &translatedBlockNr);
	/*=================================================================== */
	/**

    					    	      @brief          transactionCommit
    					    						  -

    					    		  @param          applBlockNr
    												  -

    					    	      @return         AES_CDH_ResultCode		Gives the status of transactionCommit

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B430098
	AES_CDH_ResultCode transactionCommit(ACE_UINT32 &applBlockNr);
	/*=================================================================== */
	/**

    					    	      @brief          getLastCommittedBlock
    					    						  -

    					    		  @param          applBlockNr
    												  -

    					    	      @return         AES_CDH_ResultCode		Gives the status of getLastCommittedBlock

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4300A2
	AES_CDH_ResultCode getLastCommittedBlock(ACE_UINT32 &applBlockNr);
	/*=================================================================== */
	/**

    					    	      @brief          transactionTerminate
    					    						  -

    					    	      @return         AES_CDH_ResultCode		Gives the status of transactionTerminate

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4300AC
	AES_CDH_ResultCode transactionTerminate();
	/*=================================================================== */
	/**

    					    	      @brief          setAPIClosed
    					    						  -

    					    	      @return         AES_CDH_ResultCode		Gives the status of setAPIClosed

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4300B5
	AES_CDH_ResultCode setAPIClosed();
	/*=================================================================== */
	/**

    					    	      @brief          getAgentType
    					    						  -

    					    		  @param          name
    												  -

    					    	      @return         TransferAgentAttributes	get the attributes

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B430065
	TransferAgent::TransferAgentAttributes  getAgentType();

	/*=================================================================== */
	/**

	  					    	      @brief          switchAgent
	  					    						  -

	  					    		  @param          stopTransfer
	  												  -

	  					    	      @return         void

	  					    	      @exception      none
	 */
	/*=================================================================== *///##ModelId=3E2F9B4E03C5
	void switchAgent(bool stopTransfer);
	/*=================================================================== */
	/**

	  					    	      @brief          dataTransfer
	  					    						  -

	  					    	      @return         true		on success
	  												  false     on unsuccessful

	  					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3E7998AB01BD
	bool dataTransfer(void);

	/*=====================================================================
	                        PROTECTED DECLARATION SECTION
	==================================================================== */
protected:
	/*====================================================================
		                       PROTECTED ATTRIBUTES
		==================================================================== */

	/*=================================================================== */
	/**
			      @brief   isTimeStamp
						   True if a timestamp has been added to the filename
						   Set to false in sendFileInititing and to when a timestamp has been added
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4300BF
	bool isTimeStamp;
	/*=================================================================== */
	/**
  			      @brief   connectType
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4300E8
	string connectType;
	/*=================================================================== */
	/**
  			      @brief   overWrite
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B43011A
	string overWrite;
	/*=================================================================== */
	/**
  			      @brief   transType
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B430142
	string transType;
	/*=================================================================== */
	/**
  			      @brief   remoteFullPath
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B43017D
	string remoteFullPath;
	/*=================================================================== */
	/**
  			      @brief   aRemoteDirName
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4301B0
	string aRemoteDirName;
	/*=================================================================== */
	/**
  			      @brief   directoryLevel
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4301D7
	ACE_INT32 directoryLevel;
	/*=================================================================== */
	/**

    					    	      @brief          parseFTP
    					    						  -

    					    		  @param          argc
    												  -
    								  @param          argv[]
    												  -

    					    	      @return         AES_CDH_ResultCode		Gives the status of

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4301F5
	AES_CDH_ResultCode parseFTP(ACE_INT32& argc, ACE_TCHAR* argv[]);
	/*=================================================================== */
	/**

    					    	      @brief          stepFileName
    					    						  -

    					    		  @param          fileName
    												  -
									  @param          count
    												  -

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B430213
	bool stepFileName(std::string &fileName, ACE_INT32 count);
	/*=================================================================== */
	/**
  			      @brief   ftpAgent
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B43023C
	TransferAgent *ftpAgent; // cdhch
	/*=================================================================== */
	/**
  			      @brief   cdhchSemaphore
  						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B43026E
	ACE_Semaphore *cdhchSemaphore;

};

#endif
