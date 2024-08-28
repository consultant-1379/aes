
#include <aes_afp_events.h>

void Event::report(aes_afp_events specificProblem, 
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
