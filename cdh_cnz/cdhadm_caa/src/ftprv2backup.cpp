/*=================================================================== */
/**
@file ftprv2backup.cpp

Class method implementation for ftprv2backup.h

DESCRIPTION
The services provided by FTPRV2Backup facilitates the backup of data in responding mode.

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
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <sstream>
#include <vector>
#include <transdest.h>
#include <ftprv2backup.h>
#include <backupwriter.h>
#include <servr.h>
#include <event.h>
#include <blockpairmapper.h>
#include <blockpairmappermanager.h>
#include <destinationalarm.h>

#include <AES_DBO_DataBlock.h>
#include <AES_DBO_TQManager.h>

#include <aes_gcc_errorcodes.h>
#include <aes_gcc_log.h>

/*===================================================================
                        DECLARATION SECTION
=================================================================== */

// ACS Trace definition
GCC_TDEF(AES_CDH_FTPRV2Backup);

/*===================================================================
   ROUTINE:FTPRV2Backup
=================================================================== */
//##ModelId=3DF4880F02B4
FTPRV2Backup::FTPRV2Backup() : backupWriter(0), 
    apiClosed(false), oldRecordNumber(APPLID_MAX+1), switchState(AGENT_OK),
    isOpen(false), badFTPArea(false), bCease(true), TQrecv(0), bpMapper(0)
{}

/*===================================================================
   ROUTINE:~FTPRV2Backup
=================================================================== */
//##ModelId=3DF4886201CD
FTPRV2Backup::~FTPRV2Backup()
{}

/*===================================================================
   ROUTINE:open
=================================================================== */
//##ModelId=3DF488C90207
AES_CDH_ResultCode FTPRV2Backup::open(const string &streamName,
                                      const string &streamId,
                                      string &sourceId)
{
    GCC_TRACE((AES_CDH_FTPRV2Backup,
        "(%t) FTPRV2Backup::open()\n"));

    transMutex.acquire();
    bpMapper = BlockPairMapperManager::instance()->getMapper(destinationPtr->getDestSetName());

	streamId_ = streamId;

    AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

    if (streamId == "")
    {
        if (this->getClusterName(sourceId) == false)
        {
            sourceId = "UNKNOWN";
        }

        sourceId += "_" + streamName + "_" + destinationPtr->getDestSetName();
    }
    else
    {
        sourceId = streamId;
    }


    if (backupWriter->open(destinationPtr->getDestSetName(), sourceId, this))
    {
        string RPCName = this->destinationName;

        // The destination name shall be used as RPCName
        // for backup-destinations
        if (! bpMapper->open(sourceId, RPCName))
        {
            backupWriter->close();

            GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                "(%t) FTPRV2Backup::open()\nCouldn't open bpMapper with name: %s\n", sourceId.c_str()));
            
            rCode = AES_CDH_RC_PHYSFILEERR;
        }

        if (rCode == AES_CDH_RC_OK)
        {
            ACE_UINT32 CDRBid = 0;
            backupWriter->lastTransaction(CDRBid);
            
            if (! bpMapper->validFile())
            {
                string RPCName = this->destinationName;

                // The destination name shall be used as RPCName
                // for backup-destinations
                ACE_UINT32 status = bpMapper->getFileStatus(RPCName);
                if (status == 0)
                {
                    bpMapper->setCDRBid(CDRBid, RPCName);
                }
                else if (status == 2)
                {
                    bpMapper->setCDRBidOnly(CDRBid, RPCName);
                }
                GCC_TRACE((AES_CDH_FTPRV2Backup,
                    "(%t) FTPRV2Backup::open()\nFile status: %u\n", status));
            }
        }

        if (oldRecordNumber == APPLID_MAX + 1)
        {
            oldRecordNumber = bpMapper->getApplBlockId();

            GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                "(%t) FTPRV2Backup::open(), oldRecordNumber set to %u\n", oldRecordNumber));
        }
        else
        {
            GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                "(%t) FTPRV2Backup::open(), oldRecordNumber == %u\n", oldRecordNumber));
        }

    }
    else
    {
        // Error opening backup-writer
        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            "Error opening backup writer", "-");
        rCode = AES_CDH_RC_SENDERR;
    }

    if (rCode == AES_CDH_RC_OK)
    {
        this->isOpen = true;
    }

    transMutex.release();

    GCC_TRACE((AES_CDH_FTPRV2Backup,
        "(%t) FTPRV2Backup::open()\nExiting with exitcode: %d\n", rCode));
    
    return rCode;
}

/*===================================================================
   ROUTINE:close
=================================================================== */
//##ModelId=3DF488D0006D
AES_CDH_ResultCode FTPRV2Backup::close()
{
    GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::close()\n"));

    transMutex.acquire();

    if (backupWriter->getFileSize() != 0)
    {
        this->publishBackupFile();
    }

    this->isOpen = false;
    backupWriter->close();
    bpMapper->close();  
    transMutex.release();
    return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:writeToFile
=================================================================== */
//##ModelId=3E82FE4901B7
AES_CDH_ResultCode FTPRV2Backup::writeToFile(AES_DBO_DataBlock *block)
{
    bool skipBlock = false;
    AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
    
    GCC_DEBUG((AES_CDH_FTPRV2Backup,"(%t) FTPRV2Backup::writeToFile() block %u received from the stream\n", block->blockNr_));
    
    if (block->blockNr_ != oldRecordNumber + 1)
    {
        GCC_TDEBUG((AES_CDH_FTPRV2Backup, "\nrecordNumber != oldRecordNumber + 1, recordNumber = %u, oldRecordNumber + 1 = %u", block->blockNr_, (oldRecordNumber + 1)));

        if (block->blockNr_ <= bpMapper->getApplBlockId())
        {
            ACE_UINT32 CDRBid = 0;
            backupWriter->lastTransaction(CDRBid);
            //ACE_UINT32 prevCDRBid = CDRBid == 0 ? 4294967295 : CDRBid - 1;
            bpMapper->setNextApplBlockId(block->blockNr_, CDRBid);
            
            GCC_DEBUG((AES_CDH_FTPRV2Backup,"(%t) FTPRV2Backup::sendRecordFile() calling setNextApplBlockId(..), CDRBid=%u\n", CDRBid));
        }
        else
        {
            // This block is not "wanted", just throw away block and get next block
            GCC_TDEBUG((AES_CDH_FTPRV2Backup, "\nblock skipped!, block number = %u ", block->blockNr_));
            skipBlock = true;
        }
    }
    
    if (skipBlock)
    {
        return AES_CDH_RC_SKIPPED;
    }
    else
    {
        bool b = backupWriter->write(block);
        
        if (b != true)
        {
            GCC_TDEBUG((AES_CDH_FTPRV2Backup, "\nSend Error, block number =%u ", block->blockNr_));
            returnCode = AES_CDH_RC_SENDERR;
        }
        else
        {
            GCC_TDEBUG((AES_CDH_FTPRV2Backup, "\nSent block number = %u", block->blockNr_));
            oldRecordNumber = block->blockNr_;
        }
    }

    return returnCode;
}

/*===================================================================
   ROUTINE:sendRecordFile
=================================================================== */
//##ModelId=3DF485DC01BA
AES_CDH_ResultCode FTPRV2Backup::sendRecordFile(const string& streamName,
                                                const string& streamId,
                                                AES_DBO_DataBlock*& block,
                                                CmdHandler *cmdHdlr,
                                                const ACE_UINT64 ticks)
{
   GCC_TRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::sendRecordFile() - entering\n"));

   if (TQrecv == NULL)
	   AES_DBO_TQManager::instance()->openRx(streamName, TQrecv);

   ACE_UINT32 GCC_rc = TQrecv->openTransfer();
   if (GCC_rc == AES_BUFFERERROR)
   {
	   TQrecv->closeTransfer();
	   TQrecv->destroy();
       TQrecv = NULL;

      GCC_TRACE((AES_CDH_FTPRV2Backup,
                 "(%t) FTPRV2Backup::sendRecordFile()\nError opening stream: %u\n"
                 "Returning AES_CDH_RC_STREAMERROR",
                 GCC_rc));

      return AES_CDH_RC_STREAMERROR;
   }


   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

   bool exitLoop = false;
   string sourceId = "";

   transMutex.acquire();
   returnCode = this->open(streamName, streamId, sourceId);

   if (returnCode == AES_CDH_RC_OK)
   {
      while (!apiClosed)
      { 
			// TR HI58671: Missing Alarm "TRANSFER FAULT" in DBO
			 GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                     "(%t) FTPRV2Backup::sendRecordFile() before check getVDDestPath \n"));
				
			if (::access(this->getVDDestPath().c_str(), 0) != 0)
			{
				 GCC_TINFO((AES_CDH_FTPRV2Backup,
                     "(%t) FTPRV2Backup::sendRecordFile() getVDDestPath access problem\n"));
				 returnCode = AES_CDH_RC_SENDERR;
			}
			GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                           "(%t) FTPRV2Backup::sendRecordFile() The return code: %u\n",
                           returnCode));
			if (returnCode == AES_CDH_RC_SENDERR)
               	{
				  GCC_TINFO((AES_CDH_FTPRV2Backup,
					  "(%t) FTPRV2Backup::sendRecordFile()->For closing the transfer:The return code: %u\n",
                           returnCode));
                  	this->close();
                  	TQrecv->closeTransfer();
                  	transMutex.release();
					ACE_Time_Value tv(0, 500*1000);
					ACE_OS::sleep(tv);
                  	return returnCode;
               	}
			// TR HI58671

         if (switchState == AGENT_STOP)
         {
            GCC_TRACE((AES_CDH_FTPRV2Backup,
                       "(%t) FTPRV2Backup::sendRecordFile()\n"
                       "switchState == AGENT_STOP\n"));

            bCease = true;
            switchState = AGENT_OK;
            returnCode = AES_CDH_RC_SWITCH;
            break;
         }

         if (block == NULL)
         {
            GCC_rc = TQrecv->recv(block, 1000);
         }
         else
         {
            GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                        "(%t) FTPRV2Backup::sendRecordFile(), block != NULL!\n"));

            GCC_rc = AES_NOERRORCODE;
         }

         if (GCC_rc == AES_NOERRORCODE)
         {
            returnCode = this->writeToFile(block);

            if (bCease == true)
            {
               if (returnCode != AES_CDH_RC_OK && returnCode != AES_CDH_RC_SKIPPED)
               {
                  this->close();
                  TQrecv->closeTransfer();
                  transMutex.release();
				  ACE_Time_Value tv(0, 500*1000);
				  ACE_OS::sleep(tv);
                  return returnCode;
               }
               else if (returnCode == AES_CDH_RC_OK)
               {
                  // We managed to send the block
                  //this->close();
                  //TQrecv->closeTransfer();
                  //delete block;
                  //block = 0;
                  //transMutex.release();
                  //return AES_CDH_RC_OK;
                  bCease = false;
                  this->sendEvent(cmdHdlr, ticks);
               }
            }
            else
            {
               if (returnCode != AES_CDH_RC_OK && returnCode != AES_CDH_RC_SKIPPED)
               {
				  ACE_Time_Value tv(0, 500*1000);
				  ACE_OS::sleep(tv);
                  break;
               }
            }

            delete block;
            block = NULL;
         }
         else
         {
            switch (GCC_rc)
            {
            case AES_NOSERVERACCESS:
               GCC_TINFO((AES_CDH_FTPRV2Backup,
                           "(%t) FTPRV2Backup::sendRecordFile() stream returned AES_NOSERVERACCESS\n"));
               break;

            case AES_BUFFERERROR:
               GCC_TINFO((AES_CDH_FTPRV2Backup,
                           "(%t) FTPRV2Backup::sendRecordFile() stream returned AES_BUFFERERROR\n"));
               returnCode = AES_CDH_RC_STREAMERROR;
               break;

            case AES_TIMEOUT:
               GCC_TINFO((AES_CDH_FTPRV2Backup,
                           "(%t) FTPRV2Backup::sendRecordFile() stream returned AES_TIMEOUT\n"));

               if (apiClosed == true)
               {
                  exitLoop = true;
                  returnCode = AES_CDH_RC_OK;
               }
               break;

            case AES_BUFFEREMPTY:
               GCC_TINFO((AES_CDH_FTPRV2Backup,
                           "(%t) FTPRV2Backup::sendRecordFile() stream returned AES_BUFFEREMPTY\n"));
               break;

            default:
               GCC_TINFO((AES_CDH_FTPRV2Backup,
                           "(%t) FTPRV2Backup::sendRecordFile() Unknown return code: %u\n",
                           GCC_rc));
               break;
            }

            if (exitLoop)
               break;
         }

         if (badFTPArea)
         {
            returnCode = AES_CDH_RC_SENDERR;
            break;
         }

         transMutex.release();
         transMutex.acquire();
      } // while
   }
   else
   {
      GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                  "(%t) FTPRV2Backup::sendRecordFile() open() error, returning AES_CDH_RC_SENDERR !!!\n"));
      returnCode = AES_CDH_RC_SENDERR;
   }

   this->close();
   TQrecv->closeTransfer();
   TQrecv->destroy();
   TQrecv = 0;

   if (block)
   {
      delete block;
      block = NULL;
   }

   if (apiClosed == true)
   {
      apiClosed = false;
      returnCode = AES_CDH_RC_TASKSTOPPED;
   }

   if (badFTPArea)
      returnCode = AES_CDH_RC_SENDERR;

   bCease = true;
   badFTPArea = false;

   GCC_TTRACE((AES_CDH_FTPRV2Backup,
               "(%t) FTPRV2Backup::sendRecordFile()\nExiting with exitcode: %d\n",
               returnCode));

   transMutex.release();

   return returnCode;
}

/*===================================================================
   ROUTINE:transactionBegin
=================================================================== */
//##ModelId=3DF4A6E60144
AES_CDH_ResultCode FTPRV2Backup::transactionBegin()
{

    GCC_TTRACE((AES_CDH_FTPRV2Backup,
        "(%t) FTPRV2Backup::transactionBegin()\n"));

    return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:transactionEnd
=================================================================== */
//##ModelId=3DF4A6F1008C

AES_CDH_ResultCode FTPRV2Backup::transactionEnd(ACE_UINT32 &translatedBlockNr)
{
	(void)translatedBlockNr;
    AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
    transMutex.acquire();

    if (! this->isOpen)
    {
        GCC_TDEBUG((AES_CDH_FTPRV2Backup,
            "(%t) FTPRV2Backup::transactionEnd()\n"
            "Destination is not open!\n"
            "blocknumber == %u\n", translatedBlockNr));

        transMutex.release();
        return AES_CDH_RC_CONNECTERR;
    }
    
    ACE_UINT32 CDRBid = 0;
    rCode = backupWriter->lastTransaction(CDRBid);
    ACE_UINT32 tempCDRBid = CDRBid == 0 ? 0 : CDRBid - 1;
    translatedBlockNr = bpMapper->getApplBlockId(tempCDRBid); 

    transMutex.release();

    
    GCC_TTRACE((AES_CDH_FTPRV2Backup,
        "(%t) FTPRV2Backup::transactionEnd()\n"
        "blocknumber == %u\n"
        "Exiting with exitcode: %d\n",translatedBlockNr, rCode));
    return rCode;
}

/*===================================================================
   ROUTINE:transactionCommit
=================================================================== */
//##ModelId=3DF4A734018C
AES_CDH_ResultCode FTPRV2Backup::transactionCommit(ACE_UINT32 &applBlockNr)
{
    (void)applBlockNr;
    AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
    ACE_UINT32 CDRBid;
    transMutex.acquire();
    
    if (! this->isOpen)
    {
        GCC_TDEBUG((AES_CDH_FTPRV2Backup,
            "(%t) FTPRV2Backup::transactionCommit()\n"
            "Destination is not open!\n"
            "blocknumber == %u\n", applBlockNr));

        transMutex.release();
        return AES_CDH_RC_CONNECTERR;
    }

    rCode = backupWriter->lastTransaction(CDRBid);

    if (rCode == AES_CDH_RC_OK)
    {
        BlockPairMapper::PairId id;
        id.applid_ = bpMapper->getApplBlockId(CDRBid); 

        GCC_DEBUG(("(%t) FTPRV2Backup::transactionCommit(), fetched id.applid = %u\n", id.applid_));

       if (id.applid_ > APPLID_MAX)
        {
            GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::transactionCommit()\n"
                "id.applid = %u\n"
                "CDRBid from lastTransaction = %u\n"
                "Returning AES_CDH_RC_NOBLOCKNOAVAILABLE\n", id.applid_, CDRBid));

            transMutex.release();
            return AES_CDH_RC_NOBLOCKNOAVAILABLE;
        }

        id.cdrbid_ = CDRBid;
        id.nextApplid_ = id.applid_ + 1;
        bpMapper->commit(id);
        applBlockNr = bpMapper->getApplBlockId();
        GCC_DEBUG(("(%t) FTPRV2Backup::transactionCommit(), CDRBid=%u\n", CDRBid));
        GCC_DEBUG(("(%t) FTPRV2Backup::transactionCommit(), applBlockNr=%u\n", applBlockNr));

		oldRecordNumber = bpMapper->getApplBlockId(CDRBid);   // For HI13164
		if (backupWriter->getFileSize() >= 3072000)
        {
            AES_CDH_ResultCode rc = this->publishBackupFile();

            GCC_TDEBUG((AES_CDH_FTPRV2Backup,
                "(%t) FTPRV2Backup::transactionCommit()\n"
                "Data file is larger than 3 Mb\n"
                "publishBackupFile() returned %d\n", rc));

        }
    }
    else
    {
        GCC_TINFO((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::transactionCommit(), lastTransaction failed\n"));
    }

    if (switchState == AGENT_SWITCH)
    {
        switchState = AGENT_STOP;

        GCC_TDEBUG((AES_CDH_FTPRV2Backup,
            "(%t) FTPRV2Backup::transactionCommit()\n"
            "switchState changed from AGENT_SWITCH to AGENT_STOP\n"));
    }

    transMutex.release();

    GCC_TTRACE((AES_CDH_FTPRV2Backup,
        "(%t) FTPRV2Backup::transactionCommit()\n"
        "blocknumber == %u\n"
        "Exiting with exitcode: %d\n",applBlockNr, rCode));
    return rCode;
}

/*===================================================================
   ROUTINE:sendEvent
=================================================================== */
void FTPRV2Backup::sendEvent(CmdHandler *cmdHdlr, const ACE_UINT64 ticks)
{
	ServR::cmdHandlerListMX_.acquire();
	if (ServR::validCmdHandler(cmdHdlr, ticks))
	{
		cmdHdlr->sendRecordFileReply(AES_CDH_RC_OK, ""/*fileName*/, 0/*recordsSent*/);
	}
	ServR::cmdHandlerListMX_.release();

	GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::sendEvent()"
			"\nCeasing destination alarm.\n"));

	//removed to solve TR HI51176   XTBOLAR
	/*DestinationAlarm::instance()->cease(this->destinationName,
                                       this->destinationPtr->getDestSetName(),
                                       DestinationAlarm::AlarmRecord::ok);*/
}

/*===================================================================
   ROUTINE:getLastCommittedBlock
=================================================================== */
//##ModelId=3DF4A7C502C1

AES_CDH_ResultCode FTPRV2Backup::getLastCommittedBlock(ACE_UINT32 &applBlockNr)
{
	(void)applBlockNr;
    ACE_UINT32 CDRBid = 0;
    AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

    ACE_INT32 count = 0;
    do{ 
		ACE_Time_Value tv(0, 100*1000);
		ACE_OS::sleep(tv); 
	}
	while ( this->isOpen == false && count++ < 20);

    transMutex.acquire();

    if (! this->isOpen)
    {
        GCC_TDEBUG((AES_CDH_FTPRV2Backup,
            "(%t) FTPRV2Backup::getLastCommittedBlock()\n"
            "Destination is not open!\n"));

        transMutex.release();
        return AES_CDH_RC_CONNECTERR;
    }

    rCode = backupWriter->lastTransaction(CDRBid);

    if (rCode != AES_CDH_RC_OK)
    {
        transMutex.release();

        GCC_TTRACE((AES_CDH_FTPRV2Backup,
            "(%t) FTPRV2Backup::getLastCommittedBlock()\n"
            "Unable to fetch CDRBid!\n",
            "Error code: %d", rCode));

        return rCode;
    }

   if (bpMapper->isOpen())
    {
        if (! bpMapper->validFile())
        {
            transMutex.release();

            GCC_TTRACE((AES_CDH_FTPRV2Backup,
                "(%t) FTPRV2Backup::getLastCommittedBlock()\n"
                "Mapper file is invalid!\n"
                "Returning %d", AES_CDH_RC_NOBLOCKNOAVAILABLE));

            return AES_CDH_RC_NOBLOCKNOAVAILABLE;
        }
    }
    if (rCode == AES_CDH_RC_OK)
    {
        applBlockNr = bpMapper->getApplBlockId(CDRBid); 

        if (applBlockNr > APPLID_MAX)
        {
            transMutex.release();
            
            GCC_TTRACE((AES_CDH_FTPRV2Backup,
                "(%t) FTPRV2Backup::getLastCommittedBlock()\n"
                "blocknumber > %u\n"
                "Returning %d", APPLID_MAX, AES_CDH_RC_NOBLOCKNOAVAILABLE));

            return AES_CDH_RC_NOBLOCKNOAVAILABLE;
        }

		//solution to TR HI45644
		oldRecordNumber = applBlockNr;
    }

    transMutex.release();

    GCC_TTRACE((AES_CDH_FTPRV2Backup,
        "(%t) FTPRV2Backup::getLastCommittedBlock()\n"
        "blocknumber == %u\n"
        "Exiting with exitcode: %d\n",applBlockNr, rCode));
    return rCode;
}

/*===================================================================
   ROUTINE:transactionTerminate
=================================================================== */
//##ModelId=3DF4A7460319
AES_CDH_ResultCode FTPRV2Backup::transactionTerminate()
{
    GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::transactionTerminate()\n"));
    return AES_CDH_RC_OK;
}


/*===================================================================
   ROUTINE:FTPRV2Backup
=================================================================== */
//##ModelId=3DF8AA4602A2
TransferAgent::TransferAgentAttributes FTPRV2Backup::getAgentType()
{
   return TransferAgent::FILEBACKUP;
}

/*===================================================================
   ROUTINE:define
=================================================================== */
//##ModelId=3E2C09D902E1
AES_CDH_ResultCode FTPRV2Backup::define(ACE_INT32 &argc,
                                        ACE_TCHAR *argv[],
                                        const bool define,
                                        const bool recovery)
{
    if ((backupWriter = new BackupWriter()) == 0)
    {
        // No memory...
        Event::report(AES_CDH_memoryFault, "MEMORY ALLOCATION PROBLEM",
            "Storage space for BackupWriter could not be allocated", "-");

        GCC_TFATAL((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::define()\n",
            "Error allocating BackupWriter!\n"));

        return AES_CDH_RC_INTPROGERR;
    }

    return FTPRV2::define(argc, argv, define, recovery);
}

/*===================================================================
   ROUTINE:setAPIClosed
=================================================================== */
//##ModelId=3E2FEB020194
AES_CDH_ResultCode FTPRV2Backup::setAPIClosed()
{
    if (this->isOpen)
        apiClosed = true;

	while (this->isOpen == true) {
		ACE_Time_Value tv(0,10*1000);
		ACE_OS::sleep(tv);
	}
    GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::setAPIClosed()\n"));
    GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::setAPIClosed()\n"));
    return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:switchAgent
=================================================================== */
//##ModelId=3E34DBA40274
void FTPRV2Backup::switchAgent(bool stopTransfer)
{
    transMutex.acquire();
    // Transfer is in progress, order a stop after commit
    if (stopTransfer)
    {
        GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::switchAgent()"
            "stopTransfer is true\n"
            "switchState set to AGENT_SWITCH"));

        switchState = AGENT_SWITCH;
    }
    else
    {
        // A switch to a different transfer agent has been performed

        GCC_DEBUG(("(%t) FTPRV2Backup::switchAgent(), stopTransfer is false\n"));
        bpMapper = BlockPairMapperManager::instance()->getMapper(destinationPtr->getDestSetName());
        if (bpMapper != NULL)
        {
            // Reset old block number
            oldRecordNumber = APPLID_MAX + 1;
            bpMapper->setAllChanged();

            GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::switchAgent()"
                "oldRecordNumber set to %u + 1\n", APPLID_MAX));
        }
        else
        {
            Event::report(AES_CDH_memoryFault, "MEMORY ALLOCATION PROBLEM",
                "Storage space for block mapping could not be allocated", "-");

            GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::switchAgent()"
                "Storage space for block mapping could not be allocated\n"));
        }
    }
    transMutex.release();
}

/*===================================================================
   ROUTINE:remove
=================================================================== */
//##ModelId=3E34F19B0193
AES_CDH_ResultCode FTPRV2Backup::remove()
{
    GCC_DEBUG(("(%t) FTPRV2Backup::remove()\n"));
    return FTPRV2::remove();
}

/*===================================================================
   ROUTINE:getDestSetName
=================================================================== */
//##ModelId=3E43A076031E
const string FTPRV2Backup::getDestSetName()
{
    return destinationPtr->getDestSetName();
}

/*===================================================================
   ROUTINE:publishBackupFile
=================================================================== */
//##ModelId=3E4A635B008E
AES_CDH_ResultCode FTPRV2Backup::publishBackupFile()
{
    transMutex.acquire();
    ostringstream remoteFileName;
    remoteFileName << ::time(NULL);
	string transtype="ftprv2";
    
    AES_CDH_ResultCode rCode = AES_CDH_RC_SENDERR;
	GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
                "\nEntering function. Set rCode = %d\n", rCode));

    std::vector<std::string> vec;
    string path = backupWriter->getFilePath(vec,transtype);   // Changed For SFTP Responding IP, added second parameter.
    if (path != "")
    {
        rCode = AES_CDH_RC_OK;
	    GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
                "\nPath != ''. Set rCode = %d\n", rCode));

        ACE_UINT32 nFiles = vec.size();
        for (ACE_UINT32 i = 0; i < nFiles; i++)
        {
            rCode = this->sendFileResponding(path + "/" + vec[i], "", vec[i], true);

			GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
                "\nsendFileResponding called. Set rCode = %d\n", rCode));

            if (rCode != AES_CDH_RC_OK)
            {
                rCode = this->sendFileResponding(path + "/" + vec[i], "", vec[i], true);

				GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
                "\nsendFileResponding called. Set rCode = %d\n", rCode));

                if (rCode != AES_CDH_RC_OK)
                {
                    badFTPArea = true;                
                }
            }
            else
            {
                badFTPArea = false;
            }

            GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
                "\nReply from sendFileResponding: %d\n", rCode));
        }
    }
    else
    {
        // Empty path received from backupWriter
        GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
            "\nNo path received from backupWriter, file not put onto FTP-AREA\n"));
    }

	GCC_TDEBUG((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
                "\nLeaving function. rCode = %d\n", rCode));

    if (rCode == AES_CDH_RC_OK)
    {
		GCC_TTRACE((AES_CDH_FTPRV2Backup, "(%t) FTPRV2Backup::publishBackupFile()"
            "\nCeasing destination alarm.\n"));
        DestinationAlarm::instance()->cease(this->destinationName,
                                            destinationPtr->getDestSetName(),
                                            DestinationAlarm::AlarmRecord::ok);
    }

    transMutex.release();
    return rCode;
}
