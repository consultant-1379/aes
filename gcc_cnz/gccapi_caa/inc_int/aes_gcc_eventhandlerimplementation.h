/*=================================================================== */
/**
	@file   aes_gcc_eventhandlerimplementation.h

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
#ifndef aes_gcc_eventhandlerimplementation_h
#define aes_gcc_eventhandlerimplementation_h 1

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Task.h>
#include <map>
#include <utility>
#include <string>
#include "acs_aeh_evreport.h"
#include "acs_aeh_types.h"
#include "aes_gcc_event.h"
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
		@brief AES_GCC_EventHandlerImplementation
		Implementation for,
		filtering the events and alarms
		sending alarm , cease or event to AEH.
 */
/*=================================================================== */

class AES_GCC_EventHandlerImplementation 
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

	      @brief          default constructor
						  Creates an empty event handler implementer object.

	      @return          void

	      @exception       none
	 */
	/*=================================================================== */

	AES_GCC_EventHandlerImplementation();


	virtual ~AES_GCC_EventHandlerImplementation();
	/*=================================================================== */
	/**

  	      @brief 			filter
  							Applies filter on event.

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

      						Calls filter() and creates an event report from an event.

      		@param           ex
      						 Event to be reported

      		@param           forced
      						 Event will not be filtered out

      		@return		 	 true                    if event is reported
      						 false                   if event is filtered out
      		@exception       none
	 */
	/*=================================================================== */


	bool event (AES_GCC_Event &ex, bool forced = false);


	/*=================================================================== */
	/**

      	    @brief        	alarm

      						Calls filter() and creates an alarm report from an event

      		@param           ex
      						 Alarm to be reported

      		@param           severity
      						 Level of severity (A1, A2, A3, O1, O2)

      		@return		 	 true                    if alarm was effective
      						 false                   if alarm was ineffective
      		@exception       none
	 */
	/*=================================================================== */
	bool alarm (AES_GCC_Event &ex, const ACE_TCHAR *severity);
	/*=================================================================== */
	/**

	    @brief        	cease

						ceases an alarm report from an event

		@param           ex
						 Alarm to be ceased

		@return		 	 true                   if cease was effective
						 false                  if cease was ineffective
		@exception       none
	 */
	/*=================================================================== */

	bool cease (AES_GCC_Event &ex);
	/*=================================================================== */
	/**

	    @brief        	report

						Creates a simple event report.

		@param           anerror
						 event number 0-99
		 @param          eventnumber
						 acs_aeh_specificProblem event code
		@param           afile
						 a file name (use __FILE__ )
						 source file where problem appeared
		@param           aline
						 a line number (use __LINE__ )
						 code line where fault appeared
		@param           problemdata
						 a free comment
		 @param          problemtext
						 formatted problem text
		@return		 	 true                    if event was reported
						 false                   if event was not reported
		@exception       none
	 */
	/*=================================================================== */
	bool report (ACE_INT32 anerror, acs_aeh_specificProblem eventnumber,
			const ACE_TCHAR *afile,
			ACE_INT32 aline,
			const ACE_TCHAR *problemdata,
			const ACE_TCHAR *problemtext
	);

	bool cease (AES_GCC_Event &ex, bool startUpFlg);
	/*=====================================================================
		                      PROTECTED DECLARATION SECTION
	==================================================================== */
protected:
	/*====================================================================
	                        PROTECTED ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief  ehMutex_

	        Object of ACE_Recursive_Thread_Mutex
	 */
	/*=================================================================== */

	//     AES_GCC_Mutex ehMutex_;
	ACE_Recursive_Thread_Mutex ehMutex_;

	/*=====================================================================
		                      PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*====================================================================
	                        PRIVATE ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief  evreport

	        Pointer to the acs_aeh_evreport object
	 */
	/*=================================================================== */
	acs_aeh_evreport *evreport;
	/*=================================================================== */
	/**
	      @brief  eventMap

	        Map containing <string,AES_GCC_Event>
	 */
	/*=================================================================== */
	std::multimap< std::string,AES_GCC_Event* > eventMap;

	/*=================================================================== */
	/**
	      @brief  eventPair

	        Pair of <string,AES_GCC_Event *>
	 */
	/*=================================================================== */
	std::pair< std::string,AES_GCC_Event* > eventPair;
	std::string m_processName;
	pid_t m_processId;
};
#endif
