
//*******************************************************************
// 
// .NAME
//  	AES_DBO_TQManager - Manages all of the transfer queues
// .LIBRARY 3C++
// .PAGENAME AES_DBO_TQManager
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_TQManager.h

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
// 	This class is used by all clients which uses the transfer queues.
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
// 	2002-05-02 by UAB/KB/AU Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R1A_DMDN6.lib

// .SEE ALSO 
//  

//*******************************************************************
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_DBO_TQMANAGER_H
#define AES_DBO_TQMANAGER_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "AES_DBO_EventTarget.h"
#include "AES_DBO_TQSender.h"
#include "AES_DBO_TQReceiver.h"

#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
class AES_DBO_AsynchComm;
class AES_DBO_EventTarget;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */

class AES_DBO_TQManager_Imp
{
	public:

			friend class ACE_Singleton<AES_DBO_TQManager_Imp, ACE_Recursive_Thread_Mutex>;

			/**
			 *  createTQ method: This method is used to create TQ.
			 *  @param  tqName          : const string
			 *  @param  destName         : const string
			 */
			unsigned int createTQ(const std::string &tqName,
									const std::string &destName,
									unsigned int mirror = 0,
									unsigned int removeDelay = 1440,
									const std::string &userGroup = "\\");

			/**
			 *  changeTQDest method: This method is used to change TQ destination.
			 *  @param  user          : const string
			 *  @param  tq         : const string
			 *  @param  dest         : const string
			 */
			unsigned int changeTQDest(const std::string &user,
										const std::string &tq,
										const std::string &dest);

			/**
			 *  changeTQMirror method: This method is used to change TQ mirror.
			 *  @param  user          : const string
			 *  @param  tq         : const string
			 *  @param  mirror         : unsigned int
			 *  @param  delay         :  unsigned int
			 */
			unsigned int changeTQMirror(const std::string &user,
										const std::string &tq,
										unsigned int mirror,
										unsigned int delay = 1440);

			/**
			 *  changeTQ method: This method is used to change TQ.
			 *  @param  user          : const string
			 *  @param  tq         : const string
			 *  @param  dest         : const string
			 *  @param  mirror         : unsigned int
			 *  @param  delay         :  unsigned int
			 */
			unsigned int changeTQ(const std::string &user,
									const std::string &tq,
									const std::string &dest,
									unsigned int mirror,
									unsigned int delay = 1440);

			/**
			 *  removeTQ method: This method is used to remove TQ.
			 *  @param  user          : const string
			 *  @param  tqName         : const string
			 */
			unsigned int removeTQ(const std::string &user, const std::string &tqName);

			/**
			 *  isDestSetAttached method: This method is used to check if a destination set is attached.
			 *  @param destSetName  : const string
			 */
			unsigned int isDestSetAttached(const std::string &destSetName);

			/**
			 *  tqExists method: This method is used to check if a tq exists or not..
			 *  @param tqName  : const string
			 */
			unsigned int tqExists(const std::string &tqName);

			/**
			 *  tqExists method: This method is used to check if a tq exists or not.
			 *  @param  tqName         : const string
			 *  @param  tqDN         :  string
			 */
			unsigned int tqExists(const std::string &tqName, std::string &tqDN);

			/**
			 *  tqList method: This method is used to list the TQa.
			 *  @param  qList         : list string
			 *  @param  tqName         :const string
			 */
			unsigned int tqList(std::list<std::string> &qList, const std::string &tqName = "");

			/**
			 *  openRx  method: This method is used to create a TQ receiver.
			 *  @param  tqName         :const string
			 *  @param  tq         : AES_DBO_TQReceiver pointer
			 */
			unsigned int openRx(const std::string &tqName, AES_DBO_TQReceiver *&tq);

			/**
			 *  openTx  method: This method is used to create a TQ sender.
			 *  @param  evTarget         : AES_DBO_EventTarget pointer
			 *  @param  tqName         :const string
			 *  @param  tq         : AES_DBO_TQSender pointer
			 */
			unsigned int openTx(AES_DBO_EventTarget *evTarget,
								const std::string &tqName,
								AES_DBO_TQSender *&tq);

			/**
			 *  openTx  method: This method is used to create a TQ sender.
			 *  @param  evTarget         : AES_DBO_EventTarget pointer
			 *  @param  tqName         :const string
			 *  @param  sourceId         :const string
			 *  @param  eventText         :const string
			 *  @param  tq         : AES_DBO_TQSender pointer
			 */
			unsigned int openTx(AES_DBO_EventTarget *evTarget,
								const std::string &tqName,
								const std::string &sourceId,
								const std::string &eventText,
								AES_DBO_TQSender *&tq);

			/**
			 *  getErrorText method: This method is used to get the error text.
			 *  @param  code         :int
			 */
		   std::string getErrorText(unsigned int code);

		   /**
			 *  close  method: This method is used to close the session.
			 */
		   void close();

		   /**
			 *  destroy method: This method is used to destroy the session.
			 */
		   virtual void destroy();

	private:

		   /**
			* @brief
			* constructor
		   */
		   AES_DBO_TQManager_Imp();

		   /**
		    * @brief
		    * destructor
		   */
		   virtual ~AES_DBO_TQManager_Imp();

		   /**
		    * @brief This method sends a request to DBO
		   */
		   void sendRequest(ACS_APGCC_Command& cmd);

		   /**
		    * @brief This method opens a connection towards DBO
		   */
		   bool initComunication();

		   AES_DBO_AsynchComm* m_Comunicator;

		   // Mutex used to serialize requests
		   ACE_Recursive_Thread_Mutex m_mutex;

		   // Flag to indicate connection state
		   bool m_isConnectionUp;
};

typedef ACE_Singleton<AES_DBO_TQManager_Imp, ACE_Recursive_Thread_Mutex> AES_DBO_TQManager;

#endif
