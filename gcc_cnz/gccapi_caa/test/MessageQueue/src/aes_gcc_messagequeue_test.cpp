#include <iostream>
#include <string>
#include "aes_gcc_message_queue.h"
#include <iostream>

using namespace std;

int main()
{
	AES_GCC_MessageQueue<string> message;
	string messageStr1("AESStrindData1");
	string strData;
	cout<<"Adding string object in to the message queue, data :"<<messageStr1<<endl;
	bool status = message.putq(messageStr1);
	cout<<"Status of putq()"<<status<<endl;

	int size = message.size();
	cout<<"size of message queue after putq : "<<size<<endl;

	string messageStr2("AESStrindData2"),messageStr3("AESStrindData3");
	cout<<"Adding string data :"<< messageStr2<<","<<messageStr3<<endl;
	message.putq(messageStr2);
	message.putq(messageStr3);
	size = message.size();
	cout<<"size of message queue after two more putq operations : "<<size<<endl;

	cout<<"getq operation on message queue for object : "<<messageStr2<<endl;
	if(message.getq(strData) == 0)
	{
		cout<<"getq successful"<<endl;
	}
	cout<<"Content of the first element which has removed :"<<strData<<endl;
	cout<<"Size of the messageQueue after getq :"<<message.size()<<endl;


	strData = "AESStringData4";

	if(message.prioritize(strData))
	{
		cout<<"prioritize successful"<<endl;
	}
	cout<<"Size of the messageQueue after prioritize :"<<message.size()<<endl;
	message.close();
	return 0;
}

