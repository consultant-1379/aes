/*=================================================================== */
/**
   @file   aes_ohi_blockhandler2_r1.h

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
#if !defined(_AES_OHI_BLOCKHANDLER2_R1_H_)
#define _AES_OHI_BLOCKHANDLER2_R1_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include "ace/ACE.h"
#include <ace/Semaphore.h>

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */

#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"

class aes_ohi_blockHandlerImplementation;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_ohi_blockHandler2_r1

                 This class handles the block transfer functionality
*/
/*=================================================================== */
class aes_ohi_blockHandler2_r1 
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

      @param 	       subSys
				Name of the Sub system.

      @param 	       appName
				Name of the application block.

      @param 	       blockTQ
				Name of the transfer queue

      @param 	       eventText
				Event text used in the event generated for an alarm.

      @param 	       streamId
				Name of the stream. The rpcServer on the remote side
                      		will use the streamId when creating the file where
                      		the blocks shall be stored.


      @return          void

      @exception       none
   */
/*=================================================================== */


   aes_ohi_blockHandler2_r1(const char* subSys,
	                         const char* appName, 
                            const char* blockTQ,
                            const char* eventText = "", 
                            const char* streamId = "");

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

   ~aes_ohi_blockHandler2_r1();

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

      @param	   blockNo
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
      @brief       Callback method to be implemented by the user.

      @pre         none

      @post        none

      @param       AES_OHI_EventCodes eventCode
                        The code of the signalled event.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int virtual handleEvent(AES_OHI_Eventcodes eventCode) 
   {
      (void) eventCode;
      return AES_OHI_EVENTNOTHANDLED;
   };

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
      @brief       Returns the connection state true when connected to server

      @pre         none

      @post        none

      @return      Connection Status
			true	Connected
			false	Not Connected

      @exception   none
    */
/*=================================================================== */

   bool isConnected();

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

   static unsigned int blockTransferQueueDefined(const char* blockTransferQueue);
/*=================================================================== */
   /**
      @brief       Ask GOH if block transfer queue is defined in GOH.If yes,
	  			   return the transfer queue DN.

      @pre         none

      @post        none

      @param       blockTransferQueue
                        The name of the transfer queue.

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   static unsigned int blockTransferQueueDefined(const char* blockTransferQueue, std::string & blockTranferQueueDn);

/*=================================================================== */

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

   static unsigned int getBlockTransferQueues(int& noOfItems, char** transferQueueList[]);

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

/*===================================================================
                        PROTECTED DECLARATION SECTION
=================================================================== */
protected:

/*===================================================================
                        PROTECTED ATTRIBUTE
=================================================================== */
/*=================================================================== */
   /**
      @brief  implementation

              Pointer to the aes_ohi_blockHandlerImplementation object.
   */
/*=================================================================== */

   aes_ohi_blockHandlerImplementation* implementation;
};

#endif
