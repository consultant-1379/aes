
//*******************************************************************
// 
// .NAME
//  	AES_DBO_AsynchReceiver - Manages the connection to the server
// .LIBRARY 3C++
// .PAGENAME AES_DBO_AsynchReceiver
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_AsynchReceiver.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	This class is used by the other API classes to be able to
//  communicate with the server.
//  

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 1300

// AUTHOR 
// 	2002-05-30 by UAB/KB/AU Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R1A_DMDN6.lib

// .SEE ALSO 
//  

/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_DBO_ASYNCHRECEIVER_H
#define AES_DBO_ASYNCHRECEIVER_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ACS_APGCC_Command.H>

#include <ace/Task_T.h>
#include <ace/Thread_Mutex.h>


#include <string>
#include <aes_gcc_message_queue.h>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
class CmdClient;
class AES_DBO_EventTarget;
class ACE_Thread_Semaphore;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class AES_DBO_AsynchComm: public ACE_Task<ACE_MT_SYNCH>
{
	protected:

		struct eventMsg
		{
			unsigned int event;
			unsigned int blockEventNumber;
		};

    	//const unsigned int cmdTimeout_;
    	//const unsigned int loopTimeout_;
    	std::string tqName_;
    	ACE_Thread_Mutex sendMutex_;

    	AES_DBO_EventTarget *target_;
    	CmdClient *client_;
    	bool sessionOpen_;

    	ACE_Thread_Mutex m_EventMutex;
    	std::list<eventMsg> m_EventList;

    	bool connect();

	public:

    	/**
		 * @brief
		 * constructor with one parameter
		 */
		AES_DBO_AsynchComm(AES_DBO_EventTarget *target, std::string tqName = "NONE");

		/**
			 * @brief
			 * destructor
			 */
		virtual ~AES_DBO_AsynchComm();

		/**
			 *  sendCmd method: This method is used to send commands to the server.
			 *  @param  cmd          : ACS_APGCC_Command
			 */
		unsigned int sendCmd(ACS_APGCC_Command &cmd);

		/**
			 *  destroy method: This method destroys the AsychComm.
			 */
		virtual void destroy();

		/**
			 *  apiEvent method: This method is responsible for api events.
			 */
		virtual void apiEvent(unsigned int evCode);

		/**
		 * 	@brief  Run by a daemon thread to execute the asynchronous communication.
		*/
		virtual int svc(void);

		/**
		 * 	@brief	This method initializes a task and prepare it for execution
		*/
		virtual int open(void *args = 0);

		/**
		 * 	@brief	This method signal to internal thread to terminate
		*/
		int stopSvcThread();

		/**
		 * 	@brief	This method close the connection towards DBO server,
		 *  only used from TQ manager
		*/
		void close();

	private:

		/**
		 * 	@brief	Semaphore to wait answer on a command
		*/
		ACE_Thread_Semaphore* m_CmdReplyReady;

		/**
		 * 	@brief	Answer on a command
		*/
		ACS_APGCC_Command* m_CmdReply;

		/**
		 * 	@brief	Internal thread running state
		*/
		int m_svcRun;
};

#endif
