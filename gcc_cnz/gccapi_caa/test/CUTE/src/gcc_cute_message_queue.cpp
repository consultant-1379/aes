#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "gcc_cute_message_queue.h"

AES_GCC_MessageQueue<std::string> GCC_Cute_MessageQueue::message;
std::string message1("AES message 1");
std::string message2("AES message 2");
std::string message3("AES message 3");
void GCC_Cute_MessageQueue::vPutq()
{
	bool status = message.putq(message1);
	if(status)
	{
		status = message.putq(message2);
	}
	ASSERTM("Failed to putq the data in to MessageQueue", status=true);
}
void GCC_Cute_MessageQueue::vGetq()
{
	bool status = message.getq(message2);
	ASSERTM("Failed to getQ the data from the messageQueue",status == AES_NOERRORCODE);
}
void GCC_Cute_MessageQueue::vPrioritize()
{

	bool status = message.prioritize(message3);
	ASSERTM("Failed to prioritize the data of messageQueue",status == true);
}
void GCC_Cute_MessageQueue::vGetSize()
{
	int size = message.size();
	ASSERTM("Failed to get the size of messageQueue",size == 2);
}

cute::suite GCC_Cute_MessageQueue::make_suite_gcc_cute_message_queue()
{
	cute::suite s;
	s.push_back(CUTE(vPutq));
	s.push_back(CUTE(vGetq));
	s.push_back(CUTE(vPrioritize));
	s.push_back(CUTE(vGetSize));
	return s;
}



