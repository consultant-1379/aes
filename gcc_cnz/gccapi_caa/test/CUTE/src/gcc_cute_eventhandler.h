#include "cute_suite.h"
#include "aes_gcc_event.h"
#include "aes_gcc_eventhandler.h"

class AES_GCC_Event_R1;
class AES_GCC_EventHandler_R1;
class AES_GCC_CUTE_EventHandler
{
public:
	static cute::suite make_suite_aes_cute_eventhandler();
	static void eventTest();
	static void raiseAndCeaseAlarmTest();
	static void reportTest();
	static void successiveAlarmTest();
	static void successiveEventTest();
};


