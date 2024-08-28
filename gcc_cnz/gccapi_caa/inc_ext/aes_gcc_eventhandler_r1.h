
/*=================================================================== */
/**
	@file   aes_gcc_eventhandler_r1.h

	@brief
	The services provided by AES_GCC_EventHandler facilitates the
	reporting of event to AEH.

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
   N/A       22/06/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef aes_gcc_eventhandler_r1_h
#define aes_gcc_eventhandler_r1_h 1

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aeh_types.h"
#include "acs_aeh_evreport.h"
#include <string>
#include "aes_gcc_event.h"
#include <ace/ACE.h>
#include <ace/Mutex.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <memory>

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class AES_GCC_EventHandlerImplementation;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
		@brief AES_GCC_EventHandler_R1
	The services provided by AES_GCC_EventHandler facilitates the
	reporting of event to AEH.
 */
/*=================================================================== */

class AES_GCC_EventHandler_R1
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*====================================================================
	                        PUBLIC ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief  instance

	        Pointer to the single AES_GCC_EventHandler object
	 */
	/*=================================================================== */

	/*=================================================================== */
	/**

	      @brief          destructor
						  Deletes the event handler object
	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
	virtual ~AES_GCC_EventHandler_R1();

	/*=================================================================== */
	/**

	      @brief 			filter
							Applies filter on event.
							If the return value is 1 the event 'pex' should
							be reported to AP Event Handler (AEH). Otherwise
							it should not be reported.
							The event will be filtered out if there has been
							two events with the same event number and object of
							reference not older than 5 minutes.
							The second similar event will have a warning added
							to problem text. When a previous similar event happened
							between 5 and 20 minutes ago, a text is added to problem text.
		  @param           	ex
							Event to be filtered
		  @param           	pex
							Event to be reported

	      @return          Event is EV_CEASING:
							0                       Cease not successful
							1                       Cease successful
							Other event:
							0                       Event filtered out
							1                       Event not filtered out

	      @exception       none
	 */
	/*=================================================================== */

	ACE_INT32 filter (AES_GCC_Event &ex, AES_GCC_Event *&pex);
	/*=================================================================== */
	/**

	    @brief        	event

						Calls filter() and creates an event report from an event
						and sends it to AEH depending on the result from filter().
						The process name in the event report is "<name>:<processid>"
						where name is 'processName' (see below). The object of
						reference in the event report is "<name>/<processid>" if
						object of reference from the event is an empty string.

		@param           ex
						 Event to be reported

		@param           forced
						 Event will not be filtered out

		@return		 	 true                    Event has been reported
						 false                   Event has been filtered out
		@exception       none
	 */
	/*=================================================================== */

	bool event (AES_GCC_Event &ex, bool forced = false);
	/*=================================================================== */
	/**

	    @brief        	alarm

						Calls filter() and creates an alarm report from an event
						and sends it to AEH depending on the result from filter().
						The process name in the alarm report is "<name>:<processid>"
						where name is 'processName' (see below). The object of
						reference in the alarm report is "<name>/<processid>" if
						object of reference from the event is an empty string.

		@param           ex
						 Alarm to be reported

		@param           severity
						 Level of severity (A1, A2, A3, O1, O2)

		@return		 	 true                    Alarm has been reported
						 false                   Alarm has been filtered out
		@exception		 none
	 */
	/*=================================================================== */

	bool alarm (AES_GCC_Event &ex, const ACE_TCHAR *severity);
	/*=================================================================== */
	/**

	    @brief        	alarm

						Calls filter() and ceases an alarm report from an event
						in AEH depending on the result from filter().
						The process name in the alarm report is "<name>:<processid>"
						where name is 'processName' (see below). The object of
						reference in the alarm report is "<name>/<processid>" if
						object of reference from the event is an empty string.

		@param           ex
						 Alarm to be ceased

		@return		 	 true                    Alarm has been ceased
						 false                   Alarm has not been found
		@exception		 none
	 */
	/*=================================================================== */

	bool cease (AES_GCC_Event &ex);
	bool cease (AES_GCC_Event &ex, bool startUpFlag);
	/*=================================================================== */
	/**

	    @brief        	 report
						 Creates a simple event report and sends it to AEH.
						 The process name in the event report is "<name>:<processid>"
						 where name is 'processName' (see below). The object of
						 reference in the event report is "<name>/<processid>".
						 The problem data in the event report is
						 "<afile>:<aline> <problemdata>".
						 Probable cause in the event report is 'defaultProbableCause'
						 from the class AES_GCC_Event.
		@param           anerror
						 error code
		@param           eventnumber
						 Event number 23000 - 23999
		@param           afile
						 Source file where problem appeared
		@param           aline
						 Code line where fault appeared
		@param           problemdata
						 problemdata
		@param           problemtext
						 problemtext

		@return		 	 true                    Event reported
						 false                   Alarm not reported
		@exception       none
	 */
	/*=================================================================== */

	bool report (ACE_INT32 anerror, acs_aeh_specificProblem eventnumber,
			const ACE_TCHAR *afile,
			ACE_INT32 aline,
			const ACE_TCHAR *problemdata,
			const ACE_TCHAR *problemtext);
	/*=================================================================== */
	/**

	    @brief        	 instance
						 Singleton instanciator. Creates a new AES_GCC_EventHandler object
						 if it does not exist otherwise returns previously created object.

		@return		 	 AES_GCC_EventHandler_R1 object
		@exception       none
	 */
	/*=================================================================== */

	static AES_GCC_EventHandler_R1 &instance ();
	/*=====================================================================
		                      PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

	      @brief          default constructor
						  Creates an empty event handler object.
						  NOTE! Use the same handler object with instance() (see below)
						  when events are to be compared with each other.
	      @return          void

	      @exception       none
	 */
	/*=================================================================== */

	AES_GCC_EventHandler_R1();
	/*====================================================================
	                        PRIVATE ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief  implementation

	        Pointer to the AES_GCC_EventHandlerImplementation object
	 */
	/*=================================================================== */
	AES_GCC_EventHandlerImplementation* implementation;
	/*=================================================================== */
	/**

	      @brief          COPY constructor
						  Creates copy of an AES_GCC_EventHandler_R1 object

		  @param 		  ex
						  Reference of existing AES_GCC_EventHandler_R1 object

	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventHandler_R1(const AES_GCC_EventHandler_R1 &ex);

	/*=================================================================== */
	/**

	      @brief          operator overloading for = operator
						  Assigns the reference of an existing AES_GCC_EventHandler_R1 object to new AES_GCC_EventHandler_R1 object.
		  @param 		   ex
						   Reference of AES_GCC_EventHandler_R1 object
	      @return          Reference of AES_GCC_EventHandler_R1 object

	      @exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventHandler_R1& operator=(const AES_GCC_EventHandler_R1 &ex);
	/*=================================================================== */

	/*=================================================================== */
	static std::auto_ptr<AES_GCC_EventHandler_R1>  pInstance;
	/*=================================================================== */
	static std::string processName;
	static ACE_Mutex m_mutex;
};

#define AES_GCC_EVENT(ERROR,COMMENT) \
		AES_GCC_EventHandler_R1::instance->report(ERROR,EVENT0,__FILE__,__LINE__,COMMENT,"")


#endif
