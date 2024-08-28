//******************************************************************************
// 
// .NAME
//      AES_DFO_server - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME AES_DFO_server
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
//      <General description of the class>

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      19089-CAA 109 0319

// AUTHOR 
//      2001-06-10 by UAB/I/LN  Bjrn Hall

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef AES_DBO_SERVER_H 
#define AES_DBO_SERVER_H


#include <aes_dbo_blocktransferm_handler.h>
#include <aes_dbo_blocktransfer_queue_handler.h>

#include <ACS_APGCC_DSD.H>
#include <acs_apgcc_omhandler.h>
#include <aes_gcc_log.h>

#include <ace/Event.h>
#include <ace/Event_Handler.h>

#include <sys/eventfd.h>


class AES_DBO_BlockTransferMCmdHandler;
class AES_DBO_BlockTransferQueueCmdHandler;

class aes_dbo_server: public ACE_Event_Handler
{
public:
	static ACE_Event *StopEvent;
	static bool isStopEventSignalled;
	/**
	 * @brief
	 * constructor
	 */
	aes_dbo_server();
	/**
	 * @brief
	 * destructor
	 */
	virtual ~aes_dbo_server();
	/**
	 *  open method: This method is open a session.
	 */
	virtual ACE_INT32 open(int argc = 0, char* argv[] = 0);
	/**
	 *  close method: This method is close a session.
	 */
	virtual ACE_INT32 close();
	/**
	 *  handle_session method: This method is used in session handling.
	 *  @param  session      :    SessionObj pointer
	 */
	virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);


	virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask close_mask);

	/** @brief initDSDServer method
	 *
	 *      This method initialize DSD server.
	 *
	 *      return true on success, false otherwise.
	 */
	bool initDSDServer();


	/** @brief stopDSDServer method
	 *
	 *      This method close DSD server.
	 *
	 */
	void stopDSDServer();

	bool registerDSDHandles();

	void removeDSDHandles();

	int run_reactor_service ();

	int stop_reactor_service();

	int start_service ();

	void stop_service ();

	ACS_CC_ReturnType StartOIThreads();
	/**
	 *  StopOIThreads  method: This method is used to  stop the OI threads.
	 *  @return  ACS_CC_ReturnType      :    Success/Failure
	 */
	ACS_CC_ReturnType StopOIThreads();
	/**
	 * recoverTQs method: This method is used to  recover the TQs.
	 */
	ACE_INT32 recoverTQs();

	OmHandler omHandler;

	//===========//
	// Operators //
	//===========//
private:
	aes_dbo_server & operator= (const aes_dbo_server & rhs);
	/**
	 * checkBlockTransferObjInIMM  method : This method is used to check for block transfer object in IMM.
	 */
	bool checkBlockTransferObjInIMM();

	AES_DBO_BlockTransferMCmdHandler *m_poBlockTransferMCmdHandler;

	AES_DBO_BlockTransferQueueCmdHandler *m_poBlockTransferQueueCmdHandler;

	ACE_Thread_Mutex task_done_mutex;
	/**
           @brief  cmdServerAcceptor : DSD server object
	 */
	ACS_APGCC_DSD_Acceptor m_CmdServerAcceptor;
	/**
           @brief  serverOnLine : indicates that DSD server is open
	 */
	bool m_serverOnLine;
	/**
           @brief  fdList : list of DSD handles
	 */
	std::vector<ACE_HANDLE> fdList;
	/**
           @brief  DSDInitiate
	 */
	bool m_DSDInitiate;

	ACE_Reactor* _reactor;

	int m_ShutDown;
};

#endif
