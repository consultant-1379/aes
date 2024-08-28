/*================================================================== */
/**
   @file   aes_ohi_blockhandlerimplementation.h

   @brief Header file for Block Handler functionality in OHI module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/06/2011   XCHEMAD     Initial Release
==================================================================== */


/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_AES_OHI_BLOCKHANDLER_IMPLEMENTATION_H_)
#define _AES_OHI_BLOCKHANDLER_IMPLEMENTATION_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <AES_DBO_DataBlock.h>
#include <AES_DBO_EventTarget.h>
#include <AES_DBO_TQManager.h>
#include <AES_DBO_TQSender.h>

#include <ace/Semaphore.h>

#include <list>
#include <string>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief MAXCOUNT

	Defines the maximum count of the number of outstanding messages
	in the queue.         
 */
/*=================================================================== */
#define MAXCOUNT 10000

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_ohi_blockHandlerImplementation

                 This class handles the block transfer functionality
*/
/*=================================================================== */

class aes_ohi_blockHandlerImplementation  : public AES_DBO_EventTarget 
{
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Constructor

      @pre             none

      @post            none

      @param           userBlockDest
                                Pointer to a aes_ohi_blockHandler2_r1 object.

      @param           subSys
                                Name of the sub system.

      @param           appName
                                Name of the application block.

      @param           blockTQ
                                Name of the transfer queue

      @param           eventText
                                Event text used in the event generated for an alarm.

      @param           streamId
                                Name of the stream. The rpcServer on the remote side
                                will use the streamId when creating the file where
                                the blocks shall be stored.


      @return          void

      @exception       none
   */
/*=================================================================== */

   aes_ohi_blockHandlerImplementation(aes_ohi_blockHandler2_r1 *userBlockDest,
	                                   const std::string subSys,
                                      const std::string appName,
                                      const std::string blockTQ,
                                      const std::string eventText,
                                      const std::string streamId);

/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Destructor

      @pre             none

      @post            none

      @return          none

      @exception       none
   */
/*=================================================================== */

   ~aes_ohi_blockHandlerImplementation();

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Attach to a destination and start the session by establishing
                   connection to GOH. The block destination is protected.

      @pre         none

      @post        none

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int attach();

/*=================================================================== */
   /**
      @brief       Begin a transaction

      @pre         none

      @post        none

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int transactionBegin();

/*=================================================================== */
   /**
      @brief       Terminate a transaction

      @pre         none

      @post        none

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int transactionTerminate();

/*=================================================================== */
   /**
      @brief       End a transaction

      @pre         none

      @post        none

      @param       blockNo
                        Replies the last sent block

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int transactionEnd(unsigned int& blockNo);

/*=================================================================== */
   /**
      @brief       Commit a transaction

      @pre         none

      @post        none

      @param       blockNo
                        Replies the last sent block

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int transactionCommit(unsigned int& blockNo);

/*=================================================================== */
   /**
      @brief       Reverse the attach. Unreserve the block destination
                   and close the connection to GOH

      @pre         none

      @post        none

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int detach();

/*=================================================================== */
   /**
      @brief       Send a block to the attached destination.

      @pre         none

      @post        none

      @param       buff
                        Block data to be sent

      @param       noOfBytes
                        The number of bytes in the block.

      @param       blockNo
                        Returns the block number set by GOH.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int send(const char* buff, int noOfBytes, unsigned int& blockNo);


/*=================================================================== */
   /**
      @brief       Retrieves the block number of the last sent and commited
                   block. Used for synchronization after restart.

      @pre         none

      @post        none

      @param       blockNo
                        Replies the last sent block

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getLastCommitedBlockNo(unsigned int& blockNo);

/*=================================================================== */
   /**
      @brief       Retrieves the block number for which the event was
                   generated.

      @pre         none

      @post        none

      @param       blockNo
                        Replies the last sent block

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getEventBlockNo(unsigned int& blockNo);

/*=================================================================== */
   /**
      @brief       Gets the event handle. The handle is used to inform
                   application of events. The GetEventHandle is used to
                   GetEvent to replace callback HandleEvent.

      @pre         none

      @post        none

      @return      Handle
                        Returns a handle to the event that can be used to
                        wait on the handle. The GetEventHandle is used with
                        GetEvent to replace callback HandleEvent.

      @exception   none
    */
/*=================================================================== */

   ACE_Semaphore * getEventHandle();

/*=================================================================== */
   /**
      @brief       Gets the event code associated with the event signalled
                   through the event handle.

      @pre         none

      @post        none

      @param       eventCode
                        The code of the signalled event.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int getEvent(AES_OHI_Eventcodes& eventCode);

/*=================================================================== */
   /**
      @brief       Ask GOH if block transfer queue is defined in GOH.

      @pre         none

      @post        none

      @param       blockTransferQueue
                        The name of the transfer queue.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   static unsigned int blockTransferQueueDefined(const std::string& blockTransferQueue);

/*=================================================================== */
/*=================================================================== */
   /**
      @brief       Ask GOH if block transfer queue is defined in GOH.If yes, return
	  			   the DN of the block transfer queue MO.

      @pre         none

      @post        none

      @param       blockTransferQueue
                        The name of the transfer queue.

	  @param	   blockTransferQueueDN

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   static unsigned int blockTransferQueueDefined(const std::string& blockTransferQueue, std::string & blockTranferQueueDn);

/*=================================================================== */
   /**
      @brief       Gets a list with all block transfer queues.

      @pre         none

      @post        none

      @param       nofOfItems
                        Number of transfer queue strings received.

      @param       transferQueueList
                        A string vector with the transfer queue names.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   static unsigned int getBlockTransferQueues(std::list<std::string>& transferQueueList);

/*=================================================================== */
   /**
      @brief       Returns the connection state true when connected to server

      @pre         none

      @post        none

      @return      Connection Status
                        true    Connected
                        false   Not Connected

      @exception   none
    */
/*=================================================================== */

   bool isConnected(); 

/*=================================================================== */
   /**
      @brief       Gets the error code text.

      @pre         none

      @post        none

      @param       retCode
                        The error code number

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   const char* getErrCodeText(unsigned int retCode);

/*=================================================================== */
   /**
      @brief       Gets the event for the given event code.

      @pre         none

      @post        none

      @param       retCode
                        The event code number

      @return      Error code

      @exception   none
    */
/*=================================================================== */


   virtual unsigned int event(unsigned int eventCode);

/*===================================================================
                        PROTECTED DECLARATION SECTION
=================================================================== */
protected:
/*===================================================================
                        PROTECTED ATTRIBUTE
=================================================================== */
/*=================================================================== */
   /**
      @brief  subsystem

              This attribute holds the sub system name.
   */
/*=================================================================== */
   std::string subSystem;            

/*=================================================================== */
   /**
      @brief  applicationName

              This attribute holds the application blockname.
   */
/*=================================================================== */
   std::string applicationName;      

/*=================================================================== */
   /**
      @brief  blockDestination	

              This attribute holds the block destination name
   */
/*=================================================================== */
   std::string blockDestination;      

/*=================================================================== */
   /**
      @brief  eventTextStr

              This attribute holds the event Text.
   */
/*=================================================================== */
   std::string eventTextStr;

/*=================================================================== */
   /**
      @brief  streamId

              This attribute holds the Stream Identifier.
   */
/*=================================================================== */
   std::string streamId;

/*=================================================================== */
   /**
      @brief  transferQueue

              This attribute holds the name of the transfer queue.
   */
/*=================================================================== */
   std::string transferQueue;

/*=================================================================== */
   /**
      @brief  eventBlockNo

              This attribute stores the file name associated with event
   */
/*=================================================================== */
   unsigned int eventBlockNo; 

/*=================================================================== */
   /**
      @brief  eventHandle

              This attribute holds the event handle.
   */
/*=================================================================== */
   ACE_Semaphore *eventHandle;

/*=================================================================== */
   /**
      @brief  connected.

              This attribute holds the status of the connection.
   */
/*=================================================================== */
   bool connected;

/*=================================================================== */
   /**
      @brief  transActive

              This attribute stores the active status of transfer
   */
/*=================================================================== */
   bool transActive;

/*=================================================================== */
   /**
      @brief  firstSendInTransaction

              This attribute holds the first send in transaction.
   */
/*=================================================================== */
   bool firstSendInTransaction;

/*=================================================================== */
   /**
      @brief  transPerformed

              This attribute holds the status of transfer performed.
   */
/*=================================================================== */
   bool transPerformed;

/*=================================================================== */
   /**
      @brief  lastCommitedBlockNrIsFresh

              This attribute holds the last commited block number.
   */
/*=================================================================== */
   bool lastCommitedBlockNrIsFresh;

/*=================================================================== */
   /**
      @brief  getLastCommitedBlockNoActive

              This attribute holds status of the last commited block number active.
   */
/*=================================================================== */
   bool getLastCommitedBlockNoActive;

/*=================================================================== */
   /**
      @brief  errorInSemaphore

              This attribute holds the error in the semaphore.
   */
/*=================================================================== */
   unsigned int errorInSemaphore;

/*=================================================================== */
   /**
      @brief  sentBlockNo

              This attribute holds the send block number.
   */
/*=================================================================== */
   unsigned int sentBlockNo;

/*=================================================================== */
   /**
      @brief  commitedBlockNo

              This attribute holds the commited block number.
   */
/*=================================================================== */
   unsigned int commitedBlockNo;

/*=================================================================== */
   /**
      @brief  userBlock

              This attribute holds a pointer to aes_ohi_blockHandler2_r1 object
   */
/*=================================================================== */
   aes_ohi_blockHandler2_r1* userBlock;

/*=================================================================== */
   /**
      @brief  tq

              This attribute holds a pointer to AES_DBO_TQSender object.
   */
/*=================================================================== */
   AES_DBO_TQSender *tq;
};

#endif
