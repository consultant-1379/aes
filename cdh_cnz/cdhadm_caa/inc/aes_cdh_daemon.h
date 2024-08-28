/*=================================================================== */
/**
   @file   aes_cdh_daemon.h

   @brief Header file for CDH module.

          This module contains all the declarations useful to
          specify the class aes_cdh_daemon.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/04/2012   XCHEMAD   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_DAEMON_H_
#define AES_CDH_DAEMON_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_APGCC_Daemon.H>
#include <ace/Reactor.h>
#include <ace/Thread_Mutex.h>
#include <ace/Condition_T.h>
#include <ace/ACE.h>
#include <aes_gcc_log.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

/*===================================================================*/
/**
        @brief		This class inherits ACS_APGCC_Daemon and overrides some
				    methods of its base class to handle extra checks on the
					member variables.
 */

/*===================================================================*/
class aes_cdh_daemon : public ACS_APGCC_Daemon
{
public:
	/**
	 * @brief Constructor
	 * @param name
	 * @return none
	 */
   aes_cdh_daemon(const char* name);
   /**
    * @brief Constructor
    * @return none
    */
   aes_cdh_daemon(){};
   /**
    * @brief Virtual Destructor
    * @return none
    */
   virtual ~aes_cdh_daemon();
   /**
    * @brief method
    * @return int
    */
   virtual int close() = 0;
   /**
    * @brief method
    * @param argc
    * @param argv
    * @return int
    */
   int  StartService();
   /**
    * brief method
    * @return int
    */
   int  StopService();
};

#endif /* AES_CDH_DAEMON_H_ */
