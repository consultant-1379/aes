#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "gcc_cute_eventhandler.h"


void AES_GCC_CUTE_EventHandler::eventTest()
{
	AES_GCC_Event_R1 * pEventObj = new AES_GCC_Event_R1();

	pEventObj->setObjectOfReference("AES_GCC_CUTE_EventHandler");
	pEventObj->setProbableCause("Probable cause for dummy Event");
	pEventObj->setProblemData("Problem Data for the dummy Event ");
	pEventObj->setProblemText("Problem Text for the dummy Event");
	pEventObj->setProblemText("Problem Text for the dummy Event");
	pEventObj->setKind(AES_GCC_Event::EV_EVENT);
	pEventObj->setEventCode(1000);

	AES_GCC_EventHandler * pEvHandler;
	pEvHandler = AES_GCC_EventHandler::instance();
	pEvHandler->processName = "GCC_CUTE_EventTest";
	bool status = pEvHandler->event(*pEventObj,false);
	ASSERTM("Failed to Raise an dummy Event ",status==true);
}
void AES_GCC_CUTE_EventHandler::raiseAndCeaseAlarmTest()
{
	AES_GCC_Event_R1 * pEventObj = new AES_GCC_Event_R1();
	pEventObj->setObjectOfReference("AES_GCC_CUTE_EventHandler");
	pEventObj->setProbableCause("Probable cause for dummy Alarm");
	pEventObj->setProblemData("Problem Data for the dummy Alarm ");
	pEventObj->setProblemText("Problem Text for the dummy Alarm");
	pEventObj->setProblemText("Problem Text for the dummy Alarm");
	pEventObj->setKind(AES_GCC_Event::EV_ALARM);
	pEventObj->setEventCode(2000);
	AES_GCC_EventHandler * pEvHandler;
	pEvHandler = AES_GCC_EventHandler::instance();
	pEvHandler->processName = "AES_GCC_CUTE_Test";
	bool status = pEvHandler->alarm(*pEventObj,"A1");
	ASSERTM("Failed to Raise an dummy Alarm ",status==true);
	status = false;
	status = pEvHandler->cease(*pEventObj);
	ASSERTM("Failed to Cease an dummy Alarm ",status==true);
	delete pEventObj;
}

void AES_GCC_CUTE_EventHandler::reportTest()
{
	AES_GCC_Event_R1 * pEventObj = new AES_GCC_Event();
	pEventObj->setObjectOfReference("AES_GCC_CUTE_EventHandler");
	pEventObj->setProbableCause("Probable cause for dummy Event Report");
	pEventObj->setProblemData("Problem Data for the dummy Event Report ");
	pEventObj->setProblemText("Problem Text for the dummy Event Report");
	pEventObj->setProblemText("Problem Text for the dummy Event Report");
	pEventObj->setEventCode(3000);
	AES_GCC_EventHandler * pEvHandler;
	pEvHandler = AES_GCC_EventHandler::instance();
	pEvHandler->processName = "AES_GCC_CUTE_Test";
	bool status = pEvHandler->report((ACE_UINT32)22,
					pEventObj->eventCode(),
					ACE_TEXT("aes_cute_eventhandler"),
					(ACE_INT32)58,
					pEventObj->problemData(),
					pEventObj->problemText());

	ASSERTM("Failed to Raise an dummy Event Report ",status==true);
}
void AES_GCC_CUTE_EventHandler::successiveAlarmTest()
{
	AES_GCC_Event_R1 * pEventObj = new AES_GCC_Event_R1();
        pEventObj->setObjectOfReference("APZ");
        pEventObj->setProbableCause("Probable cause for dummy Alarm");
        pEventObj->setProblemData("Problem Data for the dummy Alarm ");
        pEventObj->setProblemText("Problem Text for the dummy Alarm");
        pEventObj->setProblemText("Problem Text for the dummy Alarm");
        pEventObj->setKind(AES_GCC_Event::EV_ALARM);
        pEventObj->setEventCode(4000);
        AES_GCC_EventHandler * pEvHandler;
        pEvHandler = AES_GCC_EventHandler::instance();
        pEvHandler->processName = "AES_GCC_CUTE_Test";
        bool status = pEvHandler->alarm(*pEventObj,"A2");
        ASSERTM("Failed to Raise an dummy Alarm ",status==true);
        status = false;
	status = pEvHandler->alarm(*pEventObj,"A2");
        ASSERTM("Alarm has been raised successively for the second time",status==false);
	status = pEvHandler->cease(*pEventObj);
	ASSERTM("Failed to Cease an dummy Alarm ",status==true);
        delete pEventObj;


}
void AES_GCC_CUTE_EventHandler::successiveEventTest()
{
	AES_GCC_CUTE_EventHandler::eventTest();
}
cute::suite AES_GCC_CUTE_EventHandler::make_suite_aes_cute_eventhandler()
{
	cute::suite s;
	s.push_back(CUTE(eventTest));
	s.push_back(CUTE(raiseAndCeaseAlarmTest));
	s.push_back(CUTE(reportTest));
	s.push_back(CUTE(successiveAlarmTest));
	s.push_back(CUTE(successiveEventTest));

	return s;
}
