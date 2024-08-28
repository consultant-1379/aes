/*=================================================================== */
/**
	@file   event.h

	@brief
	This class is an common interface within CDH for using events in AEH.
	events in AEH and creating trace points in TRA.This class is an common interface within CDH for using events in AEH.


	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
   N/A       27/09/2011     xbhadur       Initial Release
============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef EVENT_H
#define EVENT_H
/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <acs_aeh_evreport.h>
#include <acs_aeh_types.h>
#include <ACS_TRA_trace.h>
#include <ace/ACE.h>

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief AES_CDH_maxNumOfEvents

          -
 */
/*=================================================================== */
const ACE_INT32 AES_CDH_maxNumOfEvents = 100;
/*=================================================================== */
/**
   @brief AES_CDH_processName

          -
 */
/*=================================================================== */
const ACE_TCHAR AES_CDH_processName[] = "AES_CDH_Server";
/*=================================================================== */
/**
   @brief AES_CDH_objClassAPZ

          -
 */
/*=================================================================== */
const ACE_TCHAR AES_CDH_objClassAPZ[] = "APZ";
/*====================================================================
                        VARIABLE DECLARATION SECTION
==================================================================== */
acs_aeh_specificProblem AES_CDH_specificProblem = 29100;//INGO3 GOH qabulfg

acs_aeh_specificProblem AES_CDH_parameterFault   = AES_CDH_specificProblem + 0;
acs_aeh_specificProblem AES_CDH_variableFault    = AES_CDH_specificProblem + 1;
acs_aeh_specificProblem AES_CDH_physFileFault    = AES_CDH_specificProblem + 2;
acs_aeh_specificProblem AES_CDH_memoryFault      = AES_CDH_specificProblem + 3;
acs_aeh_specificProblem AES_CDH_recoveryFault    = AES_CDH_specificProblem + 4;
acs_aeh_specificProblem AES_CDH_destSaveFault    = AES_CDH_specificProblem + 5;
acs_aeh_specificProblem AES_CDH_fileCopyFault    = AES_CDH_specificProblem + 6;
acs_aeh_specificProblem AES_CDH_fileRemoveFault  = AES_CDH_specificProblem + 7;
acs_aeh_specificProblem AES_CDH_dataAreaFault    = AES_CDH_specificProblem + 8;
acs_aeh_specificProblem AES_CDH_hardLinkFault    = AES_CDH_specificProblem + 9;
acs_aeh_specificProblem AES_CDH_remoteFileFault  = AES_CDH_specificProblem + 10;
acs_aeh_specificProblem AES_CDH_ftpLogFileFault  = AES_CDH_specificProblem + 11;
acs_aeh_specificProblem AES_CDH_unknownCmdFault  = AES_CDH_specificProblem + 12;
acs_aeh_specificProblem AES_CDH_fileErrFault     = AES_CDH_specificProblem + 13;
acs_aeh_specificProblem AES_CDH_streamErrFault   = AES_CDH_specificProblem + 14;
acs_aeh_specificProblem AES_CDH_fileNameFault    = AES_CDH_specificProblem + 15;
acs_aeh_specificProblem AES_CDH_intProgFault     = AES_CDH_specificProblem + 16;
acs_aeh_specificProblem AES_CDH_rpcConnectFault  = AES_CDH_specificProblem + 17;
acs_aeh_specificProblem AES_CDH_rpcSendFault     = AES_CDH_specificProblem + 18;
acs_aeh_specificProblem AES_CDH_ftpConnectFault  = AES_CDH_specificProblem + 19;
acs_aeh_specificProblem AES_CDH_clientReplyFault = AES_CDH_specificProblem + 20;
acs_aeh_specificProblem AES_CDH_ConnectFault     = AES_CDH_specificProblem + 21;
acs_aeh_specificProblem AES_CDH_NotifyFault      = AES_CDH_specificProblem + 22;
acs_aeh_specificProblem AES_CDH_fileMoveFault    = AES_CDH_specificProblem + 23;
acs_aeh_specificProblem AES_CDH_ReadyDirUsageLimExced	= AES_CDH_specificProblem + 24;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     Event

                 This class is an common interface within CDH for using events in AEH.
*/
/*=================================================================== */
class  Event
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
 public:
	/*====================================================================
							PUBLIC  METHODS
	==================================================================== */
	/*=================================================================== */
	    	/**

	    	      @brief          report
	    						  -

	    		  @param           	specificProblem
	      							specificProblem
				  @param           	probableCause
	      							probableCause
	      		  @param           	problemData
	      							-
	      		  @param           	problemText
	      							-

	    	      @return           void

	    	      @exception        none
	    	 */
	   /*=================================================================== */
  static void report(acs_aeh_specificProblem specificProblem, 
                     const std::string& probableCause, 
                     const std::string& problemData, 
                     const std::string& problemText);

};

/*=====================================================================
                        MACRO DECLARATION SECTION
==================================================================== */

/*=================================================================== */
/**
   @brief           CDH_TDEFL

                    Declare a ACS trace control block

   @param           name,
                    -

   @param           format,
                    -

*/
/*=================================================================== */
#define CDH_TDEFL(name, format) \
    static ACS_TRA_trace name = ACS_TRA_DEF(#name, format)

/*=================================================================== */
/**
   @brief           CDH_LOCALTIME

                    Print current local time on standard error output

*/
/*=================================================================== */
#define CDH_LOCALTIME \
    do \
    { \
        SYSTEMTIME systemTime__; \
        GetLocalTime(&systemTime__); \
        cerr << systemTime__.wHour << "." << systemTime__.wMinute \
             << "." << systemTime__.wSecond << "." << systemTime__.wMilliseconds; \
    } while(0);

/*=================================================================== */
/**
   @brief           CDH_TDEBUG

                    -

   @param           W,
                    -

   @param           X,
                    -

   @param           Y,
                    -

   @param           Z,
                    -
*/
/*=================================================================== */
#ifndef AES_DEBUG
#define CDH_TDEBUG(W, X, Y, Z) \
    if (ACS_TRA_ON(W)) \
    { \
        ACS_TRA_event X; \
    }
#else
#define CDH_TDEBUG(W, X, Y, Z) \
    do \
    { \
    if (ACS_TRA_ON(W)) \
    { \
        ACS_TRA_event X; \
    } \
    CDH_LOCALTIME; \
    cerr << " (" << GetCurrentThreadId() << ") " << Y \
         << Z << endl; \
    } while(0);
#endif
/*=================================================================== */
/**
   @brief           CDH_TDEBUG1

                    Send trace (and debug) data with one parameter

   @param           name,
                    -

   @param           X1,
                    -
*/
/*=================================================================== */
#define CDH_TDEBUG1(name, X1) \
    CDH_TDEBUG(name, (&name, X1), #name, X1)
/*=================================================================== */
/**
   @brief           CDH_TDEBUG1T

                    Send trace (and debug) data with one parameter and thread id

	@param          name,
                    -

   @param           X1,
                    -
*/
/*=================================================================== */
#define CDH_TDEBUG1T(name, X1) \
    CDH_TDEBUG(name, (&name, GetCurrentThreadId(), X1), #name, X1)
/*=================================================================== */
/**
   @brief           CDH_TDEBUG2

                    Send trace (and debug) data with two parameters

   @param           name,
                    -

   @param           X1,
                    -

   @param           X2,
                    -
*/
/*=================================================================== */
#define CDH_TDEBUG2(name, X1, X2) \
    CDH_TDEBUG(name, (&name, X1, X2), \
               #name, X1 << X2)
/*=================================================================== */
/**
   @brief           Brief Description

                    Detailed description

   @param           name,
                    -

   @param           X1,
                    -

   @param           X2,
                    -
*/
/*=================================================================== */
// Send trace (and debug) data with two parameters and thread id
#define CDH_TDEBUG2T(name, X1, X2) \
    CDH_TDEBUG(name, (&name, GetCurrentThreadId(), X1, X2), \
               #name, X1 << X2)
/*=================================================================== */
/**
   @brief           CDH_TDEBUG4

                    Send trace (and debug) data with four parameters

   @param           name,
                    -

   @param           X1,
                    -

   @param           X2,
                    -

   @param           X3,
                    -

   @param           X4,
                    -
*/
/*=================================================================== */
#define CDH_TDEBUG4(name, X1, X2, X3, X4) \
    CDH_TDEBUG(name, (&name, X1, X2, X3, X4), \
               #name, X1 << X2 << X3 << X4)
/*=================================================================== */
/**
   @brief           CDH_TDEBUG4T

                    Send trace (and debug) data with four parameters and thread id

   @param           name,
                    -

   @param           X1,
                    -

   @param           X2,
                    -

   @param           X3,
                    -

   @param           X4,
                    -

*/
/*=================================================================== */
#define CDH_TDEBUG4T(name, X1, X2, X3, X4) \
    CDH_TDEBUG(name, (&name, GetCurrentThreadId(), X1, X2, X3, X4), \
               #name, X1 << X2 << X3 << X4)

#endif
