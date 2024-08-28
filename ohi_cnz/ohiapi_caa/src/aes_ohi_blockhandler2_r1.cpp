/*=================================================================== */
   /**
   @file aes_ohi_blockhandler2_r1.cpp

   Class method implementationn for OHI type module.

   This module contains the implementation of class declared in
   the aes_ohi_blockhandler2_r1.h module

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

#include <string>
#include <list>
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_blockhandler2_r1.h"	
#include "aes_ohi_blockhandlerimplementation.h"	

using namespace std;

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: aes_ohi_blockHandler2_r1
=================================================================== */
aes_ohi_blockHandler2_r1::aes_ohi_blockHandler2_r1(const char* subSys,
                                                   const char* appName, 
                                                   const char* blockTQ, 
                                                   const char* eventText, 
                                                   const char* streamId)
{
   implementation = new aes_ohi_blockHandlerImplementation(this,
                                                           subSys,
                                                           appName,
                                                           blockTQ,
                                                           eventText,
                                                           streamId);
}

/*===================================================================
   ROUTINE: ~aes_ohi_blockHandler2_r1
=================================================================== */
aes_ohi_blockHandler2_r1::~aes_ohi_blockHandler2_r1()
{
   delete implementation; 
}

/*===================================================================
   ROUTINE: attach
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::attach()
{
	return implementation->attach();
}

/*===================================================================
   ROUTINE: transactionBegin
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::transactionBegin()
{
	return implementation->transactionBegin();
}

/*===================================================================
   ROUTINE: transactionTerminate
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::transactionTerminate()
{
	return implementation->transactionTerminate();
}

/*===================================================================
   ROUTINE: transactionEnd
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::transactionEnd(unsigned int& blockNo)
{
	return implementation->transactionEnd(blockNo);
}

/*===================================================================
   ROUTINE: transactionCommit
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::transactionCommit(unsigned int& blockNo)
{
	return implementation->transactionCommit(blockNo);
}

/*===================================================================
   ROUTINE: detach
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::detach()
{
    return implementation->detach();	  
}

/*===================================================================
   ROUTINE: getEventHandle
=================================================================== */
ACE_Semaphore * aes_ohi_blockHandler2_r1::getEventHandle()
{      
	return implementation->getEventHandle();
}

/*===================================================================
   ROUTINE: send
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::send(const char* buff,
                                            int noOfBytes,
                                            unsigned int& blockNo)
{
	return implementation->send(buff, noOfBytes, blockNo);
}

/*===================================================================
   ROUTINE: getLastCommitedBlockNo
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::getLastCommitedBlockNo(unsigned int& blockNo)
{
   return implementation->getLastCommitedBlockNo(blockNo);
}

/*===================================================================
   ROUTINE: getEvent
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::getEvent(AES_OHI_Eventcodes& eventCode)
{
	return implementation->getEvent(eventCode);
}

/*===================================================================
   ROUTINE: getEventBlockNo
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::getEventBlockNo(unsigned int & blockNo)
{
   return implementation->getEventBlockNo(blockNo);
}

/*===================================================================
   ROUTINE: isConnected
=================================================================== */
bool aes_ohi_blockHandler2_r1::isConnected()
{
	return implementation->isConnected();
}

/*===================================================================
   ROUTINE: getErrCodeText
=================================================================== */
const char* aes_ohi_blockHandler2_r1::getErrCodeText(unsigned int retCode)
{
	return implementation->getErrCodeText(retCode);
}

/*===================================================================
   ROUTINE: blockTransferQueueDefined
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::blockTransferQueueDefined(const char* blockTransferQueue)
{  
	return aes_ohi_blockHandlerImplementation::blockTransferQueueDefined((string)blockTransferQueue);
}


/*===================================================================
   ROUTINE: blockTransferQueueDefined
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::blockTransferQueueDefined(const char* blockTransferQueue, std::string & blockTranferQueueDn)
{
	return aes_ohi_blockHandlerImplementation::blockTransferQueueDefined((string)blockTransferQueue, blockTranferQueueDn);
}


/*===================================================================
   ROUTINE: getBlockTransferQueues
=================================================================== */
unsigned int aes_ohi_blockHandler2_r1::getBlockTransferQueues(int& noOfItems,
                                                              char** transferQueueList[])
{
   // Initialize return values
   noOfItems = 0;
   *transferQueueList = NULL;

   std::list<string> TQlist;
   unsigned int rcode = aes_ohi_blockHandlerImplementation::getBlockTransferQueues(TQlist);
   if (TQlist.size() > 0)
   {
      // Allocate a vector with n number of pointers
      *transferQueueList = new char* [sizeof(char*) * TQlist.size()];
      if (*transferQueueList)
      {
         std::list<string>::iterator iter = TQlist.begin();
         while (iter != TQlist.end())
         {
            // Allocate each string and copy transfer queue name to it
            (*transferQueueList)[noOfItems] = new char[(*iter).length()+1];
            if ((*transferQueueList)[noOfItems])
            {
               strcpy((*transferQueueList)[noOfItems++], (*iter).c_str());
            }

            iter++;
         }
      }

      TQlist.clear();
   }

   return rcode;
}
