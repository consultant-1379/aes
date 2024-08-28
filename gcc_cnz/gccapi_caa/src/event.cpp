/* =================================================================== */
	/**
	@file event.cpp

	Class method implementation for event.h

	DESCRIPTION

	ERROR HANDLING

	General rule:
	The error handling is specified for each method.

	No methods initiate or send error reports unless specified.

	@version 1.1.1

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       22/06/2011    xnadnar      Initial Release
	*/
/* =================================================================== */

/*===================================================================
					INCLUDE DECLARATION SECTION
=================================================================== */
#include "event.h"
#include "ACS_TRA_trace.h"
#include "aes_gcc_eventhandler.h"

using namespace std;
/*===================================================================
                    ROUTINE: report
=================================================================== */
void Event::report(acs_aeh_specificProblem specificProblem,
                   const std::string& probableCause, 
                   const std::string& problemData, 
                   const std::string& problemText)
{
	AES_GCC_Event event(problemData.c_str());
	event.setProblemText(problemText.c_str());
	event.setEventCode(specificProblem);
	event.setProbableCause(probableCause.c_str());
	AES_GCC_EventHandler::instance().event(event, false);
}  












