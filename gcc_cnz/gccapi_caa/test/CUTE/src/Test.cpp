#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "Test.h"
#include "gcc_cute_eventhandler.h"
#include "gcc_cute_variableimplementation.h"
#include "gcc_cute_threadImpl.h"
#include "gcc_cute_message_queue.h"
#include "gcc_cute_stream.h"


void runSuite()
{
	cute::suite s;
	cute::ide_listener lis;
	cute::makeRunner(lis)(s, "The Suite");
	cute::makeRunner(lis)(AES_GCC_CUTE_EventHandler::make_suite_aes_cute_eventhandler(),"Event Handler Test Suite\n");
	cute::makeRunner(lis)(GCC_Cute_Variable::make_suite_gcc_variableimplementation(),"Variable implementation Test Suite \n");
	cute::makeRunner(lis)(GCC_Cute_MessageQueue::make_suite_gcc_cute_message_queue(),"Message queue Test Suite \n");
	cute::makeRunner(lis)(GCC_Cute_Thread_Test::make_suite_gcc_cute_threadImpl(),"Thread implementation Test Suite \n");
	cute::makeRunner(lis)(AES_GCC_CUTE_Stream::make_suite_gcc_cute_stream(),"Stream Test Suite \n");

}

int main()
{
	runSuite();
	return 0;
}


