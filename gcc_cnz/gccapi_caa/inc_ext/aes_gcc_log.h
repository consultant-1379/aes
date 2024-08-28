/*=================================================================== */
/**
   @file   aes_gcc_log.h

   @brief This module contains all the declarations useful for implementing TRA traces.

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/07/2011   XNADNAR     APG43 on Linux.

==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef ACS_GCC_TRA_H_
#define ACS_GCC_TRA_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <iostream>
using namespace std;
#include "ace/ACE.h"
#include "ace/Log_Msg.h"
#include "ACS_TRA_trace.h"
#include "ACS_TRA_Logging.h"
#include <ace/Mutex.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  DEBUG_PREFIX

 */
/*=================================================================== */
#define DEBUG_PREFIX "\n DEBUG%I"
#define AES_AFP_PROC_NAME       "aes_afpd"
#define AES_CDH_PROC_NAME       "aes_cdhd"
#define AES_DBO_PROC_NAME       "aes_dbod"


/*=================================================================== */
/**
   @brief  INFO_PREFIX

 */
/*=================================================================== */
#define INFO_PREFIX "\n INFO%I"
/*=================================================================== */

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class AES_GCC_Log
{
/*=====================================================================
						PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=====================================================================
						PRIVATE  ATTRIBUTE
==================================================================== */
/*=================================================================== */
   /**
	  @brief   instance_

				Singleton instance of AES_GCC_Log
   */
/*=================================================================== */
    static AES_GCC_Log *instance_;
/*=================================================================== */
   /**
	  @brief   logMutex_

				Instance of ACE_Mutex used while logging
   */
/*=================================================================== */
    ACE_Mutex logMutex_;                   // Cannot be AES_GCC_Mutex
/*=================================================================== */
   /**
	  @brief   traceMutex_

				Instance of ACE_Mutex used while tracing
   */
/*=================================================================== */
    ACE_Mutex traceMutex_;                 // Cannot be AES_GCC_Mutex

/*=================================================================== */
   /**
	  @brief   AES_GCC_Logging

				Instance of ACS_TRA_Logging used for logging traces in to a log file.
   */
/*=================================================================== */
    static ACS_TRA_Logging AES_GCC_Logging;
/*=====================================================================
						CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

	  @brief           Creates default log object

	  @return          void

	  @exception       none
   */
/*=================================================================== */
    AES_GCC_Log();
/*=================================================================== */
   /**

	  @brief           getString
					  Creates a string buffer of the trace arguments passed.

	  @param           format
					   message format
	  @param           argp
					   va_list( variable argument list of the message format)
	  @return          void

	  @exception       none
   */
/*=================================================================== */
    std::string getString(const char *format, va_list argp);

/*=====================================================================
						PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
						CLASS DESTRUCTOR
==================================================================== */
    /*=================================================================== */
       /**

          @brief           Deletes the instance of AES_GCC_Log

          @return          void

          @exception       none
       */
    /*=================================================================== */
    ~AES_GCC_Log();
/*=================================================================== */
   /**

	  @brief           log
					   Used to log the traces in to a specified log file.

	  @param           format
					   message format

	  @return          void

	  @exception       none
   */
/*=================================================================== */
    void logDebug(const char *format, ...);
     void log(const char *format, ...);
/*=================================================================== */
   /**

	  @brief           log
					   Used to log the traces in to a specified log file.

	  @param           tp
					   ACS_TRA_trace object used for tracing.
	  @param           format
					   message format
	  @return          void

	  @exception       none
   */

/*=================================================================== */
    void logDebug(ACS_TRA_trace &tp , const char *format, ...);
	void log(ACS_TRA_trace &tp , const char *format, ...);
/*=================================================================== */
   /**

	  @brief           trace
					   Used for tracing the debug messages.

	  @param           tp
					   ACS_TRA_trace object used for tracing.
	  @param           format
					   message format
	  @return          void

	  @exception       none
   */

void logTrace(const char *format, ...);
void logInfo(const char *format, ...);
void logWarn(const char *format, ...);
void logError(const char *format, ...);
void logFatal(const char *format, ...);
void logTrace(ACS_TRA_trace& , const char *format, ...);
void logInfo(ACS_TRA_trace& , const char *format, ...);
void logWarn(ACS_TRA_trace& , const char *format, ...);
void logError(ACS_TRA_trace& , const char *format, ...);
void logFatal(ACS_TRA_trace& , const char *format, ...);
/*=================================================================== */
    void trace(ACS_TRA_trace &tp, const char *format, ...);
/*=================================================================== */
   /**

	  @brief          instance
					  Gives an singleton instance of AES_GCC_Log class.

	  @return         Object of class AES_GCC_Log

	  @exception      none
   */
/*=================================================================== */
    static AES_GCC_Log *instance();
};

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  GCC_TDEF
		   Used to create TRA trace object of trace length 512 bytes

 */
/*=================================================================== */
#define GCC_TDEF(X) \
    ACS_TRA_trace  X(#X, "C512")
/*=================================================================== */
/**
   @brief  GCC_TDEFL
		   Used to create TRA trace object.

 */
/*=================================================================== */
#define GCC_TDEFL(X, Y) \
    ACS_TRA_trace X(#X, #Y)

#ifdef ACEDEBUG

#define GCC_DEBUG (X)\
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX X ))

#define GCC_TRACE (X) \
ACE_DEBUG(( LM_INFO, \
INFO_PREFIX X))

#define GCC_TDEBUG (X) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX X ))

#define GCC_TTRACE (X) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX X ))

#define GCC_TINFO (X) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX X ))

#define GCC_TWARN (X) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX X ))

#define GCC_TERROR (X) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX X ))

#define GCC_TFATAL (X) \
ACE_DEBUG(( LM_DEBUG, \
DEBUG_PREFIX X ))
#else //not ACEDEBUG
/*=================================================================== */
/**
   @brief  GCC_TRACE
		   This will call the trace method

 */
/*=================================================================== */
#define GCC_TRACE(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->trace X; \
    } while(0);
/*=================================================================== */
/**
   @brief  GCC_DEBUG
		   This will call the log method

 */
/*=================================================================== */
#define GCC_DEBUG(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->log X; \
    } while(0);
/*=================================================================== */
/**
   @brief  GCC_DEBUG
		   This will call both trace and log methods

 */
/*=================================================================== */
#define GCC_TDEBUG(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->trace X; \
    log__->logDebug X; \
    } while(0);

#define GCC_TTRACE(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->trace X; \
    log__->logTrace X; \
    } while(0);

#define GCC_TINFO(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->trace X; \
    log__->logInfo X; \
    } while(0);

#define GCC_TWARN(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->trace X; \
    log__->logWarn X; \
    } while(0);

#define GCC_TERROR(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->trace X; \
    log__->logError X; \
    } while(0);

#define GCC_TFATAL(X) \
    do \
    { \
    AES_GCC_Log *log__ = AES_GCC_Log::instance(); \
    log__->trace X; \
    log__->logFatal X; \
    } while(0);


#endif

#endif /* ACS_GCC_LOG_H_ */
