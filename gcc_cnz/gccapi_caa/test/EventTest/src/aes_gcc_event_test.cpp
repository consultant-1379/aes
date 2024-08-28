#include "aes_gcc_event.h"
#include "aes_gcc_eventhandler.h"
#include <ace/ACE.h>
#include <iostream>
#include <aes_gcc_tra.h>
int main()
{

	AES_GCC_Event myevent;
	AES_GCC_EventHandler * pev =AES_GCC_EventHandler::instance();
	myevent.setErrorCode(1234);
	myevent.setEventCode(1111);
	std::string problemData = "My event problem data";
	std::string problemText = "My event problem Text";
	myevent.setProblemData(problemData.c_str());
	myevent.setProblemText(problemText.c_str());
	pev->processName = "AES Event Test";
	pev->event(myevent);


	AES_GCC_Event myalarm;
	myalarm.setEventCode(2222);
	myalarm.setErrorCode(1235);
	problemData = "My alarm problem data";
	problemText = "My alarm problem Text";
	myalarm.setProblemData(problemData.c_str());
	myalarm.setProblemText(problemText.c_str());
	std::string severity = "A2";
	pev->processName = "AES Alarm Test";
	pev->alarm(myalarm,severity.c_str());

	pev->cease(myalarm);
	return 0;
}

