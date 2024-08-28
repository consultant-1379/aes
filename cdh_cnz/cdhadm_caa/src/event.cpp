/*=================================================================== */
/**
@file event.cpp

Class method implementation for event.h

DESCRIPTION
This class is an common interface within CDH for using events in AEH.
events in AEH and creating trace points in TRA.This class is an common interface within CDH for using events in AEH.

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

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <event.h>
#include <aes_gcc_eventhandler.h>

using namespace std;

/*===================================================================
   ROUTINE:report
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


//******************************************************************************











