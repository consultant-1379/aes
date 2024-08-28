/*=================================================================== */
/**
@file ftpv2agent.cpp

Class method implementation for ftpv2agent.h

DESCRIPTION
Handles FTP parts common for FTP Version 2.
The source code is adapted for WinNT only.

ERROR HANDLING
General rule:

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ftpv2agent.h>
#include <aes_cdh_Linftpiv2.h>
#include <sshftpiv2.h>
#include <sshftprv2.h>
//#include <ftprv2backup.h>
//#include <sshftprv2backup.h>
#include <ace/Get_Opt.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"


AES_CDH_TRACE_DEFINE(AES_CDH_FTPV2Agent);

using namespace std;

/*===================================================================
   ROUTINE:ftpAgent
=================================================================== */
FTPV2Agent::FTPV2Agent() : ftpAgent(NULL)
{
    cdhchSemaphore= new ACE_Semaphore(1,USYNC_THREAD,(ACE_TCHAR*)0, 0, 0x7fffffff);
    isTimeStamp = false;
    directoryLevel=0;

}

/*===================================================================
   ROUTINE:~ftpAgent
=================================================================== */
FTPV2Agent::~FTPV2Agent()
{
	delete cdhchSemaphore;
	cdhchSemaphore = 0;
}

/*===================================================================
   ROUTINE:create
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::create(const string destName,
                                      const string transferType,
                                      ACE_INT32& argc,
                                      ACE_TCHAR* argv[],
                                      TransferAgent*& trans)
{
#ifdef DEBUGTEST
	cout<<"Entering FTPV2Agent::create()"<<endl;
#endif  

   (void) destName;
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
   ACE_INT32 cFlag = 0;
   string cType = "r";
   FTPV2Agent* transTemp;    

   for(int i = 0 ;  i < argc ; i++)
   {
	if( ACE_OS::strcmp(argv[i] , "-c" ) == 0 )
	{
		cType = argv[i+1];
		upperToLower(cType);
		if (cType != "i" && cType != "r")
		{
			return AES_CDH_RC_UNREAS;
		}

		
		cFlag++;
		break;
	}
	
		
   }
   
   if (cFlag > 1)
      returnCode = AES_CDH_RC_INCUSAGE;


   if (returnCode == AES_CDH_RC_OK)
   {

      if (transferType == FTPTypeV2 && cType == "i")
      {
         transTemp = new LinFTPIV2;
         transTemp->transType = transferType;
         trans = transTemp;
      }
      else if (transferType == SFTPTypeV2 && cType == "i")
      {
         transTemp = new SSHFTPIV2();
         transTemp->transType = transferType;
         trans = transTemp;
      }
	  // Provided functionality for SFTP Responding type.
      else if (transferType == SFTPTypeV2 && cType == "r")
      {
		    
		  transTemp = new SSHFTPRV2;
		  transTemp->transType = transferType;
		  trans = transTemp;
      }
      else
      {
         returnCode = AES_CDH_RC_INTPROGERR;
      }
   }
#ifdef DEBUGTEST
   cout<<"Exiting FTPV2Agent::create(), return code is "<<returnCode<<endl;
#endif
   return returnCode;
}

/*===================================================================
   ROUTINE:sendRecordFile
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::sendRecordFile(const string& streamName,
                                              const string& streamId,
                                              AES_DBO_DataBlock*& block,
                                              CmdHandler *cmdHdlr,
                                              const ACE_UINT64 ticks)
{
	(void) streamName;
	(void) streamId;
	(void) block;
	(void) cmdHdlr;
	(void) ticks;
    // The method sendRecordFile do not exist for FTP
    AES_CDH_ResultCode returnCode = AES_CDH_RC_INCUSAGE;
    return returnCode;
}
/*===================================================================
   ROUTINE:transactionBegin
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::transactionBegin()
{
    AES_CDH_LOG(LOG_LEVEL_TRACE, "%s", "FTPV2Agent::transactionBegin()");

    return AES_CDH_RC_INCUSAGE;
}

/*===================================================================
   ROUTINE:transactionEnd
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::transactionEnd(ACE_UINT32 &translatedBlockNr)
{
	(void)translatedBlockNr;
	AES_CDH_TRACE_MESSAGE("Entering");

    return AES_CDH_RC_INCUSAGE;
}

/*===================================================================
   ROUTINE:transactionCommit
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::transactionCommit(ACE_UINT32 &applBlockNr)
{
	(void)applBlockNr;
	AES_CDH_TRACE_MESSAGE("Entering");
    
    return AES_CDH_RC_INCUSAGE;
}

/*===================================================================
   ROUTINE:getLastCommittedBlock
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::getLastCommittedBlock(ACE_UINT32 &applBlockNr)
{
	(void)applBlockNr;
	AES_CDH_TRACE_MESSAGE("Entering");

    return AES_CDH_RC_INCUSAGE;
}

/*===================================================================
   ROUTINE:transactionTerminate
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::transactionTerminate()
{
	AES_CDH_TRACE_MESSAGE("Entering");

    return AES_CDH_RC_INCUSAGE;
}

/*===================================================================
   ROUTINE:setAPIClosed
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::setAPIClosed()
{
    return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:parseFTP
=================================================================== */
AES_CDH_ResultCode FTPV2Agent::parseFTP(ACE_INT32& argc, ACE_TCHAR* argv[])
{
   ACE_INT32 opt;
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
   ACE_INT32 cFlag = 0;
   ACE_INT32 oFlag = 0; // INGO3 GOH Drop1 010606 qabulfg

   // INGO3 GOH Drop1 010606 qabulfg   OSF_Get_Opt getopt(argc, argv, "c:");
	ACE_Get_Opt getopt(argc, argv, "c:o:"); // INGO3 GOH Drop1 010606 qabulfg

   while ((opt = getopt()) != EOF)   
   {
      switch (opt) 
      {
      case 'c':
         {
            if (argv[getopt.optind-1][0] == '-')
               return AES_CDH_RC_INCUSAGE;

				connectType = getopt.optarg;
            upperToLower(connectType);
            if (connectType != "i" && connectType != "r")
            {
               return AES_CDH_RC_UNREAS;
            }

            delete [] argv[getopt.optind-2];
            argv[getopt.optind-2] = NULL;
            delete [] argv[getopt.optind-1];
            argv[getopt.optind-1] = NULL;
            cFlag++;
            break;
         }

      // INGO3 GOH Drop1 qabulfg start
      case 'o':
         {
            // overwrite file at destination
            if (argv[getopt.optind-1][0] == '-')
               return AES_CDH_RC_INCUSAGE;

            overWrite = getopt.optarg;
            upperToLower(overWrite);

            if (overWrite != "yes" && overWrite != "no")
            {
               return AES_CDH_RC_INCUSAGE;
            }

            delete [] argv[getopt.optind-2];
            argv[getopt.optind-2] = NULL;
            delete [] argv[getopt.optind-1];
            argv[getopt.optind-1] = NULL;

            oFlag++;
            break;
         }
      // INGO3 GOH Drop1 qabulfg end 

      case '?':
         {
            if ((getopt.optind + 1) < argc)
            {
               if (argv[getopt.optind][0] != '-' && argv[getopt.optind + 1][0] == '-')
                  getopt.optind++;
            }
            break;
         }

      default:
         {
            return AES_CDH_RC_INTPROGERR;
         }
      }
   }

    if (cFlag > 1 || oFlag > 1)
       returnCode = AES_CDH_RC_INCUSAGE;

    cleanUpArgv(argc, argv, getopt.optind);

    return returnCode;
}  

/*===================================================================
   ROUTINE:stepFileName
=================================================================== */
//*****************************************************************
//  stepFileName() - updates the series that is added to the
//                   filename. Example: file01 --> file01_001
//*****************************************************************
bool FTPV2Agent::stepFileName(std::string &fileName, ACE_INT32 count)
{
	(void)count;
    if (isTimeStamp)
    {
        ACE_INT32 nsPos = fileName.find_last_of('_');

        if (nsPos == (ACE_INT32)fileName.npos)
        {
            nsPos = fileName.length();
        }
        else
        {
            fileName = fileName.substr(0, nsPos);
        }
    }

    time_t ltime;
    time(&ltime);
    ACE_TCHAR stamp[15];
    ACE_OS::sprintf(stamp, "%ld", ltime);
    fileName.append("_");
    fileName.append(stamp);
    isTimeStamp = true;
    return true;
}

/*===================================================================
   ROUTINE:getAgentType
=================================================================== */
TransferAgent::TransferAgentAttributes  FTPV2Agent::getAgentType()
{
    return TransferAgent::FILE;
}

/*===================================================================
   ROUTINE:switchAgent
=================================================================== */
void FTPV2Agent::switchAgent(bool stopTransfer)
{
	(void)stopTransfer;
	// Nothing to do
}

/*===================================================================
   ROUTINE:dataTransfer
=================================================================== */
bool FTPV2Agent::dataTransfer(void)
{
	// method used only in bgwrpc
	return false;
}
