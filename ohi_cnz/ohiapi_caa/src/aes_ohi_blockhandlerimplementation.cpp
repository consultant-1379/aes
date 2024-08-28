/*=================================================================== */
   /**
   @file aes_ohi_blockhandlerimplementation.cpp

   Class method implementation for OHI type module.

   This module contains the implementation of class declared in
   the aes_ohi_blockhandlerimplementation.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/06/2011    XCHEMAD   Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "aes_ohi_blockhandler2_r1.h"
#include "aes_ohi_blockhandlerimplementation.h"
#include <aes_ohi_errorcodes.h>
#include <aes_ohi_eventcodes.h>

#include "aes_gcc_errorcodes.h"
#include "aes_gcc_eventcodes.h"
#include "aes_gcc_log.h"

#include <iostream>

GCC_TDEF(aes_ohi_api_block);
GCC_TDEF(aes_ohi_api_block_send);

void toUpper(std::string &);
/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: aes_ohi_blockHandlerImplementation
=================================================================== */
aes_ohi_blockHandlerImplementation::aes_ohi_blockHandlerImplementation(
   aes_ohi_blockHandler2_r1* userBlockTQ,
   const std::string subSys,
   const std::string appName, 
   const std::string blockTQ,
   const std::string eventText,
   const std::string stId ):
subSystem(subSys),
applicationName(appName),
eventTextStr(eventText),
streamId(stId),
transferQueue(blockTQ),
eventBlockNo(-1),
eventHandle(NULL),
connected(false),
transActive(false),       // active after transactionBegin
firstSendInTransaction(true),
transPerformed(false),
lastCommitedBlockNrIsFresh(false),
getLastCommitedBlockNoActive(false),
errorInSemaphore(AES_OHI_NOERRORCODE),
sentBlockNo(0),
commitedBlockNo(0),
userBlock(userBlockTQ),
tq(NULL)
{
}

/*===================================================================
   ROUTINE: ~aes_ohi_blockHandlerImplementation
=================================================================== */
aes_ohi_blockHandlerImplementation::~aes_ohi_blockHandlerImplementation()
{
   if (eventHandle != NULL)
   { 
      delete eventHandle;
      eventHandle = NULL;
   }

   if (connected)
   {
      connected = false; 
   }

   if(tq != NULL)
   {
      tq->closeTransfer();
      tq->destroy();
      tq = NULL;
   }

}

/*===================================================================
   ROUTINE: attach
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::attach()
{
   unsigned int maxSizeOfStreamId = 100;
   GCC_TRACE((aes_ohi_api_block,"\n%s\n %s%s\n","Entering aes_ohi_blockHandlerImplementation::attach",  "transferQueue = ", transferQueue.c_str()));

   if(connected)  // already connected
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "The application is already attached to the destination ! ", "Exiting aes_ohi_blockHandlerImplementation::attach"));

      return AES_OHI_INCORRECTCOMMAND;
   }

   // Illegal size of streamId
   if (streamId.length() > maxSizeOfStreamId)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "Application try to use a streamId with size > maxSizeOfStreamId! ", "Exiting aes_ohi_blockHandlerImplementation::attach"));
      return AES_OHI_INCORRECTCOMMAND;
   }
   
   unsigned int result = 0;
   getLastCommitedBlockNoActive = false;


   result = AES_DBO_TQManager::instance()->
      openTx(this, transferQueue, streamId, eventTextStr, tq);

   GCC_TRACE((aes_ohi_api_block, "\n %s\n %s%u\n", "AES_DBO_TQManager::instance()->openTx(this,", "transferQueue, streamId, eventTextStr, tq) = ", result));

   if (result == AES_NOERRORCODE)
   {
	   result = tq->openTransfer();

	   GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s\n", "return code from tq->openTransfer() = ", result, "Exiting aes_ohi_blockHandlerImplementation::attach"));

	   if(result != AES_NOERRORCODE)
       {
	   tq->closeTransfer();                  //CNI_33_6_707 //HW65595 
    	   tq->destroy();
           tq = NULL;
       }

   }

   switch (result)
   {
   case AES_BUFFERERROR:
      return AES_OHI_BUFFERERROR;

   case AES_EXECUTEERROR:
      return AES_OHI_EXECUTEERROR;

   case AES_NOSERVERACCESS:
      return AES_OHI_NOSERVERACCESS;

   case AES_NOPROCORDER:
      return AES_OHI_NOPROCORDER;

   case AES_NODESTINATION:
      return AES_OHI_NODESTINATION;

   case AES_NOCDHSERVER:
      return AES_OHI_NOCDHSERVER;

   case AES_TQISOPEN:
      return AES_OHI_INCORRECTCOMMAND;

   case AES_NOERRORCODE:
      connected = true;
      return AES_OHI_NOERRORCODE;

   default:
      return AES_OHI_ERRORUNKNOWN;
   }
}

/*===================================================================
   ROUTINE: transactionBegin
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::transactionBegin()
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::transactionBegin"));

   if(!connected)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "OHI is not connected to the destination !", "Exiting aes_ohi_blockHandlerImplementation::transactionBegin"));
      return AES_OHI_INCORRECTCOMMAND;
   }

   if(!lastCommitedBlockNrIsFresh)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "transactionCommit or getLastCommitedBlockNo has not been performed !", "Exiting aes_ohi_blockHandlerImplementation::transactionBegin"));
      return AES_OHI_COMMITNOTPERFORMED;
   }
   else
   {
      lastCommitedBlockNrIsFresh = false;
   }

   if (transActive)
   {
      return AES_OHI_TRANSACTIONACTIVE;
   }


   transActive = true;
   firstSendInTransaction = true;
   transPerformed = false; //I2

   sentBlockNo = commitedBlockNo;

   unsigned int result = AES_OHI_ERRORUNKNOWN;
   if(tq != NULL) //HW59563
   	result = tq->transactionBegin();

   GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s\n", "tq->transactionBegin() = ", result, "Exiting aes_ohi_blockHandlerImplementation::transactionBegin"));

   return result;
}

/*===================================================================
   ROUTINE: transactionTerminate
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::transactionTerminate()
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::transactionTerminate"));
   if (transActive)
   {
      transActive = false;
      getLastCommitedBlockNoActive = false;

      unsigned int result = AES_OHI_ERRORUNKNOWN;

      if(tq != NULL) 	//HW59563
	result = tq->transactionTerminate();

      GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s\n", "tq->transactionTerminate() = ", result, "Exiting aes_ohi_blockHandlerImplementation::transactionTerminate"));

      return result;
   }
   else
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "Transaction is not active !", "Exiting aes_ohi_blockHandlerImplementation::transactionTerminate"));
      return AES_OHI_TRANSACTIONNOTACTIVE;
   }
}

/*===================================================================
   ROUTINE: transactionEnd
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::transactionEnd(unsigned int& blockNo)
{
   GCC_TRACE((aes_ohi_api_block,  "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::transactionEnd"));
   if (transActive)
   {
      transPerformed = true;

      unsigned int result = AES_OHI_ERRORUNKNOWN;
      if(tq != NULL)  	//HW59563
      	result = tq->transactionEnd(blockNo);

      GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s%u\n %s\n", "tq->transactionEnd(blockNo) = ", result, "blockNo = ", blockNo, "Exiting aes_ohi_blockHandlerImplementation::transactionEnd"));

      return result;
   }
   else
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "No transaction is active ! ", "Exiting aes_ohi_blockHandlerImplementation::transactionEnd"));
      return AES_OHI_TRANSACTIONNOTACTIVE;
   }
}

/*===================================================================
   ROUTINE: transactionCommit
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::transactionCommit(unsigned int& blockNo)
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n",  "Entering aes_ohi_blockHandlerImplementation::transactionCommit"));

   if (!transPerformed)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "No transaction performed !", "Exiting aes_ohi_blockHandlerImplementation::transactionCommit"));
      return AES_OHI_NOTRANSACTIONPERFORMED;
   }
   else
   {
      lastCommitedBlockNrIsFresh = true;
      transActive = false;
      unsigned int result = AES_OHI_ERRORUNKNOWN;
      if(tq != NULL)	//HW59563
     	result = tq->transactionCommit(blockNo);

      if (result == AES_NOERRORCODE)
      {
         sentBlockNo = blockNo;
         commitedBlockNo = blockNo;
      }

      GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s%u\n %s\n", "tq->transactionCommit(blockNo) = ",result, "blockNo = ", blockNo, "Exiting aes_ohi_blockHandlerImplementation::transactionCommit"));

      return result;
   }
}

/*===================================================================
   ROUTINE: detach
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::detach()
{
   GCC_TRACE((aes_ohi_api_block,"\n%s\n %s%s\n","Entering aes_ohi_blockHandlerImplementation::detach",  "transferQueue = ", transferQueue.c_str()));

   if(!connected)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s%s\n %s\n", "OHI is not attached to the transfer queue = !", transferQueue.c_str(), "Exiting aes_ohi_blockHandlerImplementation::detach"));
      return AES_OHI_INCORRECTCOMMAND;
   }

   if (transActive)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "Transaction is active !", "Exiting aes_ohi_blockHandlerImplementation::detach"));
      return AES_OHI_TRANSACTIONACTIVE;
   }

   unsigned int result = AES_OHI_ERRORUNKNOWN;

   if(tq != NULL)
   {
      result = tq->closeTransfer();
      tq->destroy();
      tq = NULL;
      lastCommitedBlockNrIsFresh = false;
   }

   getLastCommitedBlockNoActive = false;

   GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s\n", "return code from tq->closeTransfer() = ", result, "Exiting aes_ohi_blockHandlerImplementation::detach"));

   if( eventHandle != NULL)
   {
      while ((eventHandle->acquire())  > 0 )
      {

      }
   }

   connected = false;

   switch (result)
   {
   	   case AES_NOSERVERACCESS:
   		   return AES_OHI_NOSERVERACCESS;

   	   case AES_NOERRORCODE:
   		   return AES_OHI_NOERRORCODE;

   	   case AES_NODESTINATION:
   		   return AES_OHI_NOPROCORDER;

   	   default:
   		   return AES_OHI_ERRORUNKNOWN;
   }
}

/*===================================================================
   ROUTINE: getEventHandle
=================================================================== */
ACE_Semaphore * aes_ohi_blockHandlerImplementation::getEventHandle()
{   
   ACE_INT32 result = 0;
   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::getEventHandle"));

   if (!eventHandle)
   {
	eventHandle = new(std::nothrow) ACE_Semaphore(0); 
   } 
   if ( eventHandle == NULL )
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s%u\n", "Error while creating a semaphore. Error number is ", result));
   }

   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Exiting aes_ohi_blockHandlerImplementation::getEventHandle"));

   return eventHandle;
}

/*===================================================================
   ROUTINE: event
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::event(unsigned int eventCode)
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::event"));
   if (eventHandle)
   {
      ACE_INT32 result = eventHandle->release();

      if ( result == -1)
      {
         errorInSemaphore = AES_OHI_SEMAPHOREERR;
      }
   }
   else
   {
      eventBlockNo = -1;


      AES_OHI_Eventcodes ohiEventCode;

      // translate gcc event code to ohi event code
      ohiEventCode = (AES_OHI_Eventcodes)eventCode;

      if( userBlock->handleEvent(ohiEventCode) == AES_OHI_EVENTNOTHANDLED)
      {
         // failed to transfer event to application
      }
   }

   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Exiting aes_ohi_blockHandlerImplementation::event"));

   return AES_OHI_NOERRORCODE; 
}

/*===================================================================
   ROUTINE: send
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::send(const char* buff,
                                                      int noOfBytes,
                                                      unsigned int& blockNo)
{	
   const ACE_INT32 minBlockSize = 1;  
   const ACE_INT32 maxBlockSize = 65536; 

   GCC_TRACE((aes_ohi_api_block_send, "\n %s\n %s%u\n %s%u\n", "Entering aes_ohi_blockHandlerImplementation::send", "noOfBytes = ", noOfBytes, "blockNo = ", blockNo));

   if(!connected)
   {
      GCC_TRACE((aes_ohi_api_block_send, "\n %s\n %s\n", "Send operation can not be performed because not attached !", "Exiting aes_ohi_blockHandlerImplementation::send"));

      return AES_OHI_INCORRECTCOMMAND;
   }

   if(!transActive)
   {
      GCC_TRACE((aes_ohi_api_block_send, "\n %s\n %s\n", "Send operation can not be performed because no active transaction !", "Exiting aes_ohi_blockHandlerImplementation::send"));

      return AES_OHI_TRANSACTIONNOTACTIVE;
   }

   if ((!firstSendInTransaction) && (blockNo != sentBlockNo + 1))
   {
      GCC_TRACE((aes_ohi_api_block_send, "\n %s\n %s%d\n %s%u\n %s%u\n %s\n", "Application and GOH is not in synch !", "firstSendInTransaction = ", firstSendInTransaction, "blockNo = ", blockNo, "sentBlockNo = ", sentBlockNo, "Exiting aes_ohi_blockHandlerImplementation::send"));
      return AES_OHI_NOTINSYNCH;
   }

   if ((firstSendInTransaction) &&
      (blockNo != sentBlockNo + 1) &&
      (blockNo != 0) &&
      (blockNo != 1))
   {
      GCC_TRACE((aes_ohi_api_block_send, "\n %s\n %s%d\n %s%u\n %s%u\n %s\n", "Application and GOH is not in synch !", "firstSendInTransaction = ", firstSendInTransaction, "blockNo = ", blockNo, "sentBlockNo = ", sentBlockNo, "Exiting aes_ohi_blockHandlerImplementation::send"));
      return AES_OHI_NOTINSYNCH;
   }

   if (blockNo > 4000000000)
   {
      GCC_TRACE((aes_ohi_api_block_send, "\n %s\n %s\n", "Block number > 4000000000 !", "Exiting aes_ohi_blockHandlerImplementation::send"));
      return AES_OHI_INVALIDBLOCKNR;
   }

   if ((noOfBytes < minBlockSize) || (noOfBytes > maxBlockSize))
   {
      return AES_OHI_INCORRECTBLOCKSIZE;
   }

   AES_DBO_DataBlock *myBlock = 
      AES_DBO_DataBlock::create(blockNo, buff, noOfBytes);

   if( myBlock == NULL )
   {
         return AES_OHI_ERRORUNKNOWN;
   }

   unsigned int result = AES_OHI_ERRORUNKNOWN; 
   if(tq != NULL)	//HW59563
   	result = tq->send(myBlock);

   GCC_TRACE((aes_ohi_api_block_send, "\n %s%u\n %s%d\n %s%u\n %s\n", "tq->send(myBlock) = ", result, "noOfBytes = ", noOfBytes, "blockNo = ", blockNo, "Exiting aes_ohi_blockHandlerImplementation::send"));

   delete myBlock;

   switch (result)
   {
   case AES_NOSERVERACCESS:
      return AES_OHI_NOSERVERACCESS;

   case AES_BUFFERFULL:
      return AES_OHI_BUFFERFULL;

   case AES_NOTINSYNCH:
      return AES_OHI_NOTINSYNCH;

   case AES_INCORRECTBLOCKSIZE:
      return AES_OHI_INCORRECTBLOCKSIZE;

   case AES_CONNECTERROR:
      return AES_OHI_CONNECTERROR;

   case AES_NOCDHSERVER:
      return AES_OHI_NOCDHSERVER;

   case AES_NOERRORCODE:
      {
         firstSendInTransaction = false;
         sentBlockNo = blockNo;
         return AES_OHI_NOERRORCODE;
      }

   default:
      {
         return AES_OHI_ERRORUNKNOWN;
      }
   }
}

/*===================================================================
   ROUTINE: getLastCommitedBlockNo
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::getLastCommitedBlockNo(unsigned int& blockNo)
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::getLastCommitedBlockNo"));

   if(getLastCommitedBlockNoActive)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "Illegal call for getLastCommitedBlockNo !", "Exiting aes_ohi_blockHandlerImplementation::getLastCommitedBlockNo"));

      return AES_OHI_INCORRECTCOMMAND;
   }


   if(!connected)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "OHI is not connected to the destination !", "Exiting aes_ohi_blockHandlerImplementation::getLastCommitedBlockNo"));

      return AES_OHI_INCORRECTCOMMAND;
   }

   if(transActive)
   {
      GCC_TRACE((aes_ohi_api_block, "\n %s\n %s\n", "OHI is in state active transaction !", "Exiting aes_ohi_blockHandlerImplementation::getLastCommitedBlockNo"));

      return AES_OHI_TRANSACTIONACTIVE;
   }

   unsigned int result = AES_OHI_ERRORUNKNOWN;
   if(tq != NULL)	//HW59563
      result = tq->getLastCommittedBlockNr(blockNo);

   if ((result == AES_NOERRORCODE) || 
      (result == AES_BLOCKNRFROMGOHLIST))
   {
      sentBlockNo = blockNo;
      commitedBlockNo = blockNo;
   }
   else
   {
      sentBlockNo = commitedBlockNo;
   }

   lastCommitedBlockNrIsFresh = true;
   getLastCommitedBlockNoActive = true;

   GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s%u\n %s\n", "result = tq->getLastCommittedBlockNr(blockNo) = ", result, "blockNo = ", blockNo, "Exiting aes_ohi_blockHandlerImplementation::getLastCommitedBlockNo"));

   return result;
}

/*===================================================================
   ROUTINE: getEventBlockNo
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::getEventBlockNo(unsigned int& blockNo)
{
   blockNo = eventBlockNo;

   GCC_TRACE((aes_ohi_api_block, "\n %s\n %s%u\n", "aes_ohi_blockHandlerImplementation::getEventBlockNo", "blockNo = ",blockNo));

   return 0;
}

/*===================================================================
   ROUTINE: getEvent
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::getEvent(AES_OHI_Eventcodes& eventCode)
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::getEvent"));

   unsigned int result = AES_OHI_ERRORUNKNOWN;
   if(tq != NULL) //HW59563
   	result = tq->getEvent((unsigned int&)eventCode, eventBlockNo);

   GCC_TRACE((aes_ohi_api_block, "\n %s%d\n %s%u\n %s%u\n %s\n ", "eventCode = ", eventCode, "eventBlockNo = ", eventBlockNo, "tq->getEvent((unsigned int&)eventCode, eventBlockNo)= ", result, "Exiting aes_ohi_blockHandlerImplementation::getEvent"));

   if (result == AES_NOERRORCODE)
   {
      result = errorInSemaphore;
   }

   return result;
}

/*===================================================================
   ROUTINE: isConnected
=================================================================== */
bool aes_ohi_blockHandlerImplementation::isConnected()
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n %s%d\n", "aes_ohi_blockHandlerImplementation::isConnected", "connected = ", connected));
   return connected;
};

/*===================================================================
   ROUTINE: getErrCodeText
=================================================================== */
const char* aes_ohi_blockHandlerImplementation::getErrCodeText(unsigned int retCode)
{
   GCC_TRACE((aes_ohi_api_block, "\n %s\n %s%u\n", "aes_ohi_blockHandlerImplementation::getErrCodeText", "retCode = ", retCode));
   const char* res = 0;

    res = AES_DBO_TQManager::instance()->getErrorText(retCode).c_str();

	
   GCC_TRACE((aes_ohi_api_block, "\n %s%s\n %s\n", "AES_DBO_TQManager::instance()->tqExists(blockTransferQueue) = ", res, "Exiting aes_ohi_blockHandlerImplementation::getErrCodeText"));
   return res;

};

/*===================================================================
   ROUTINE: blockTransferQueueDefined
=================================================================== */
unsigned int  aes_ohi_blockHandlerImplementation::blockTransferQueueDefined(
	const std::string& blockTransferQueue)
{  
   GCC_TRACE((aes_ohi_api_block, "\n %s\n %s%s\n", "Entering aes_ohi_blockHandlerImplementation::blockTransferQueueDefined", "blockTransferQueue = ", blockTransferQueue.c_str()));

   unsigned int result = 0;
   std::string temp;
   temp = blockTransferQueue;
   result = AES_DBO_TQManager::instance()->tqExists(temp);

   GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s\n", "AES_DBO_TQManager::instance()->tqExists(blockTransferQueue) = ", result, "Exiting aes_ohi_blockHandlerImplementation::blockTransferQueueDefined"));

   switch (result)
   {
   case AES_TQNOTFOUND:
      return AES_OHI_TQNOTFOUND;

   case AES_TQNAMEINVALID:
      return AES_OHI_TQNAMEINVALID;

   case AES_NOERRORCODE:
      return AES_OHI_NOERRORCODE;

   case AES_NODESTINATION:
      return AES_OHI_NODESTINATION;

   case AES_INVALIDDESTNAME:
      return AES_OHI_INVALIDDESTNAME;

   case AES_SENDITEMNAMEINVAL:
      return AES_OHI_SENDITEMNAMEINVAL;

   case AES_NOPROCORDER:
      return AES_OHI_NOPROCORDER;

   case AES_NOSERVERACCESS:
      return AES_OHI_NOSERVERACCESS;

   default:
      return AES_OHI_ERRORUNKNOWN;
   }
}

/*===================================================================
   ROUTINE: blockTransferQueueDefined
=================================================================== */
unsigned int  aes_ohi_blockHandlerImplementation::blockTransferQueueDefined(
	const std::string& blockTransferQueue, std::string& blockTransferQueueDN)
{  
   GCC_TRACE((aes_ohi_api_block, "\n %s\n %s%s\n", "Entering aes_ohi_blockHandlerImplementation::blockTransferQueueDefined", "blockTransferQueue = ", blockTransferQueue.c_str()));

   unsigned int result = 0;
   std::string temp;
   temp = blockTransferQueue;
   result = AES_DBO_TQManager::instance()->tqExists(temp, blockTransferQueueDN);

   GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s\n", "AES_DBO_TQManager::instance()->tqExists(blockTransferQueue) = ", result, "Exiting aes_ohi_blockHandlerImplementation::blockTransferQueueDefined"));

   switch (result)
   {
   case AES_TQNOTFOUND:
      return AES_OHI_TQNOTFOUND;

   case AES_TQNAMEINVALID:
      return AES_OHI_TQNAMEINVALID;

   case AES_NOERRORCODE:
      return AES_OHI_NOERRORCODE;

   case AES_NODESTINATION:
      return AES_OHI_NODESTINATION;

   case AES_INVALIDDESTNAME:
      return AES_OHI_INVALIDDESTNAME;

   case AES_SENDITEMNAMEINVAL:
      return AES_OHI_SENDITEMNAMEINVAL;

   case AES_NOPROCORDER:
      return AES_OHI_NOPROCORDER;

   case AES_NOSERVERACCESS:
      return AES_OHI_NOSERVERACCESS;

   default:
      return AES_OHI_ERRORUNKNOWN;
   }
}

/*===================================================================
   ROUTINE: getBlockTransferQueues
=================================================================== */
unsigned int aes_ohi_blockHandlerImplementation::getBlockTransferQueues(
	std::list<std::string>& transferQueueList)
{  
   GCC_TRACE((aes_ohi_api_block, "\n %s\n", "Entering aes_ohi_blockHandlerImplementation::getBlockTransferQueues"));

   unsigned int result = 0;

   result = AES_DBO_TQManager::instance()->tqList(transferQueueList);

   GCC_TRACE((aes_ohi_api_block, "\n %s%u\n %s\n", "AES_DBO_TQManager::instance()->tqList(transferQueueList) = ", result, "Exiting aes_ohi_blockHandlerImplementation::getBlockTransferQueues"));

   switch (result)
   {
   case AES_NOERRORCODE:
      return AES_OHI_NOERRORCODE;

   case AES_NOSERVERACCESS:
      return AES_OHI_NOSERVERACCESS;

   case AES_NOPROCORDER:
      return AES_OHI_NOPROCORDER;

   default:
      return AES_OHI_ERRORUNKNOWN;
   }
}

/*===================================================================
   ROUTINE: toUpper
=================================================================== */
void toUpper(std::string &str)
{
    int len = str.length();
    
    for (int i = 0; i <= len - 1; i++)
        if (str[i] >= 97 && str[i] <= 122) 
            str[i] = str[i] - 32;
}
