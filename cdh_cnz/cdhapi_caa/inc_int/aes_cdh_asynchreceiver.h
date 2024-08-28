/*================================================================== */
/**
   @file   aes_cdh_asynchreceiver.h

   @brief Header file for AES_CDH_AsynchReceiver type module.
	
	  Creates a thread that handles receving of replies from CDH server during
	  a send file task or a send record file task.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD    Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_ASYNCHRECEIVER_H 
#define AES_CDH_ASYNCHRECEIVER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <servr.h>
#include <aes_cdh_destinationset.h>


#include <ace/Task.h>
#include <ace/Synch.h>
#include <ace/Auto_Event.h>


#include <string>

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class AES_CDH_DestinationSetImplementation;
class AES_CDH_DestinationImplementation;
class AES_CDH_CmdClient;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_AsynchReceiver

                 This class handles the asynchronous calls
*/
/*=================================================================== */
class  AES_CDH_AsynchReceiver : public ACE_Task_Base
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

      @brief           AES_CDH_AsynchReceiver

                       Class constructor

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_AsynchReceiver();
    
/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Class destructor

      @return          none

      @exception       none
   */
/*=================================================================== */
    virtual ~AES_CDH_AsynchReceiver();
    
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       This method opens session for data transfer

      @param	   destSet
			Destination set implementation object

      @param	   destSetName
			Destination set name

      @return      Result code 

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode openSession(AES_CDH_DestinationSetImplementation& destSet, const string destSetName); 
    
/*=================================================================== */
   /**
      @brief       This method closes session for data transfer

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode closeSession(void);
    
/*=================================================================== */
   /**
      @brief       This method gets executed once activate is called.

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    virtual int svc(void);
    
/*=================================================================== */
   /**
      @brief       This method is called at 'svc' thread exit, due to parent implementation.

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    virtual int close(u_long);

/*=================================================================== */
   /**
      @brief       This method sends the command to server.

      @param	   cmd
			Command object

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode sendCmd(ACS_APGCC_Command &cmd); 
    
/*=================================================================== */
   /**
      @brief       This method closes the session if acknowledgement received

      @param       set
                        status of acknowledgement

      @return      Status

      @exception   none
    */
/*=================================================================== */
    bool closeSessionAckReceived(bool set = false);  
    
/*=================================================================== */
   /**
      @brief       This method stops the tasks if acknowledgement received

      @param       set
                        status of acknowledgement

      @return      Status

      @exception   none
    */
/*=================================================================== */
    bool stopTasksAckReceived(bool set = false); 

/*=================================================================== */
   /**
	  @brief       This method sends the command to server.

	  @param	   cmd
			Command object

	  @return      Return value

	  @exception   none
	*/
/*=================================================================== */
	AES_CDH_ResultCode sendCmdAndWait(ACS_APGCC_Command &cmd);
    
/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:

    /**
         @brief       This method force the internal thread to stop

         @return      Status

         @exception   none
       */
    bool shutDown();

/*===================================================================
                       PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   client

               Command client object
   */
/*=================================================================== */
    AES_CDH_CmdClient *client;
    
/*=================================================================== */
   /**
      @brief   sessionOpen

               Flag to enable if session is open
   */
/*=================================================================== */
    bool sessionOpen;
    
/*=================================================================== */
   /**
      @brief   transactionCommit

               Flag to enable if acknowlegment is recieved for CDH Server for transactionCommit
   */
/*=================================================================== */
    bool transactionCommit;					//HV50261
/*=================================================================== */
   /**
      @brief   stopTasksAckIsReceived

               Flag to enable if acknowlegment is recieved for stop tasks
   */
/*=================================================================== */

    bool closeSessionAckIsReceived;
    
/*=================================================================== */
   /**
      @brief   stopTasksAckIsReceived

               Flag to enable if acknowlegment is recieved for stop tasks
   */
/*=================================================================== */
    bool stopTasksAckIsReceived;
    
/*=================================================================== */
   /**
      @brief   closeSessionSem

               Semaphore object for close session
   */
/*=================================================================== */
    ACE_Thread_Semaphore closeSessionSem;
    
/*=================================================================== */
   /**
      @brief   stopTasksSem

               Semaphore object for stop tasks
   */
/*=================================================================== */
    ACE_Thread_Semaphore  stopTasksSem;
    

/*=================================================================== */
   /**
      @brief   destSetPtr

               Destination set implementation object
   */
/*=================================================================== */
    AES_CDH_DestinationSetImplementation* destSetPtr;


/*=================================================================== */
   /**
      @brief   destinationSetName

               Name of the destination set
   */
/*=================================================================== */
    std::string destinationSetName;

    /**
        @brief	m_StopEvent : signal to internal thread to exit
    */
    int m_StopEvent;


    /**
       @brief   m_TransBegin

    	   Semaphore object for transaction begin reply
    */
    ACE_Thread_Semaphore  m_TransBegin;

   /**
	  @brief   m_TransEnd

			   Semaphore object for transaction end reply
   */
	ACE_Thread_Semaphore  m_TransEnd;

   /**
	  @brief   m_TransTerminate

			   Semaphore object for transaction commit reply
   */
	ACE_Thread_Semaphore  m_TransTerminate;


   /**
	  @brief   m_TransCommit

			   Semaphore object for transaction commit reply
   */
	ACE_Thread_Semaphore  m_TransCommit;


   /**
	  @brief   m_GetLastCommittedBlock

			   Semaphore object for get last commited block reply
   */
	ACE_Thread_Semaphore  m_GetLastCommittedBlock;

	/**
	 * 	@brief	Answer on a command
	*/
	ACS_APGCC_Command* m_CmdReply;

	/**
	 * 	@brief	Mutex on cmd reply pointer
	*/
	ACE_Recursive_Thread_Mutex m_CmdMutex;
};

#endif // AES_CDH_ASYNCHRECEIVER_H
