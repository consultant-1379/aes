//******************************************************************************
/*=================================================================== */
/**
	@file   event.h

	@brief
	This class is a common interface within GCC for using
	events in AEH.

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
#ifndef EVENT_H 
#define EVENT_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include "acs_aeh_evreport.h"
#include "acs_aeh_types.h"
#include <ace/ACE.h>

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */

const ACE_INT32 Event_maxNumOfEvents = 100;
const ACE_TCHAR Event_processName[] = "AES_GCC";
const ACE_TCHAR Event_objClassAPZ[] = "APZ";
/*====================================================================
                        VARIABLE DECLARATION SECTION
==================================================================== */
acs_aeh_specificProblem Event_specificProblem = 29300;
acs_aeh_specificProblem Event_shareInfoFault       = Event_specificProblem + 0;
acs_aeh_specificProblem Event_mapViewFault         = Event_specificProblem + 1;
acs_aeh_specificProblem Event_createFileMapFault   = Event_specificProblem + 2;
acs_aeh_specificProblem Event_createSemaphoreFault = Event_specificProblem + 3;
acs_aeh_specificProblem Event_variableFault        = Event_specificProblem + 4;
acs_aeh_specificProblem Event_aesInfoFault         = Event_specificProblem + 5;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
		@brief Event
				Class holding method to report the event data in to AEH
 */
/*=================================================================== */
class  Event
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */

public:
	/*=================================================================== */
	/**

	@brief				report

						Creates an AES_GCC_Event object and raises this event using
						AES_GCC_Eventhandler instance.
	@param				specificProblem
						Event number

	@param				probableCause

	@param				problemData

	@param				problemText

	@return          	void

	@exception       	none
	 */
	/*=================================================================== */

	static void report(acs_aeh_specificProblem specificProblem,
			const std::string& probableCause,
			const std::string& problemData,
			const std::string& problemText);

};
#endif
