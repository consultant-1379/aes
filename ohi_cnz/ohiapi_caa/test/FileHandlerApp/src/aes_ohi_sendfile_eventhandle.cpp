#include <string>
#include <iostream>

#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include <ace/ACE.h>
/*
Example: Usage of event handle
 */
using namespace std;
struct filePattData
{
	unsigned char data;
	int nr1;
	int nr2;
};
const int MaxFiles = 1;
const int recordLength = 2048;
const int nrOfrecords = 1000;
filePattData fp[MaxFiles];

bool createFile(std::string path,std::string fullName)
{
	std::string fileName = path+fullName;

	FILE * fp = ACE_OS::fopen(fileName.c_str(),"a+" ); //666
	if(fp == NULL)
	{
		std::cerr<<"Error in creating the file , errno :"<<errno<<std::endl;
		return false;
	}
	else
	{
		std::cout<<"File created successfully"<<std::endl;
	}

	//write
	char * writeBuff;
	int noOfRecords = 50;
	writeBuff = new char[40];
	ACE_OS::sprintf(writeBuff,ACE_TEXT("Testing file Transfer"));
	for(int i=0; i<noOfRecords;i++)
	{
		ACE_OS::fwrite(writeBuff,strlen(writeBuff),1,fp);
	}
	ACE_OS::fclose(fp);
	return true;

}

//Example: Usage of event handle
int main()
{
	AES_OHI_FileHandler* myFileSender;
	ACE_Semaphore * eventHandle;
	AES_OHI_Eventcodes eventCode = AES_OHI_EVELOSTSERVER;
	char subname[10];
	string path = "/opt/filehandler";
	string subname_tmp;
	int fileNo = 1;
	ACE_Time_Value tv(500);
	unsigned int gohError;
	bool running = true;
	myFileSender = new AES_OHI_FileHandler("TST","TESTAPL","TESTTRANSFERQUEUE","Used for testing");
	eventHandle = myFileSender->getEventHandle();

	while ((gohError = myFileSender->attach()) ==
			AES_OHI_NOSERVERACCESS)
	{
		cerr << "Server not responding, waiting...: " << endl;
		sleep(20000);
	}
	if (gohError == AES_OHI_NOERRORCODE)
	{
		/*! toDo@ restore the comment after lastReportedFile api ready
		 *
		 */
#if 0
		myFileSender->getLastSentFile((ACE_TCHAR*)subname_tmp.c_str());
#endif
		subname_tmp = "0001";
		strcpy(subname,subname_tmp.c_str());
		fileNo = atoi(subname); // resync after restart
		fileNo = fileNo + 1; //You want the next file
		while (running)
		{
			sprintf(subname,"%04u",fileNo++);
			createFile(path,subname);
			int status;
			if(eventHandle!=NULL){
			status = eventHandle->acquire(tv);
			cout<<"status of acquire : "<<status<<endl;
			}
			if(status == AES_OHI_NOERRORCODE)
			{
#if 0
				gohError = myFileSender->getEvent(eventCode);
#endif

				if (gohError != AES_OHI_NOERRORCODE)
				{
					cerr<<"getEvent status is not AES_OHI_NOERRORCODE "<<endl;
					running = false;
					switch (eventCode)
					{
					case AES_OHI_EVELINKDOWN:
					{
						//Alarm("my link is down ",a1);
						cerr<<"AES_OHI_EVELINKDOWN"<<endl;
						break;
					}
					case AES_OHI_EVELINKDOWNCEASE:
					{
						//Alarmcease("my link is down ");
						cerr<<"AES_OHI_EVELINKDOWNCEASE"<<endl;
						break;
					}
					default:
						cerr<<"default"<<endl;
						break;
					}
				}

				else if(myFileSender->send(subname,AES_OHI_DEFAULT)== AES_OHI_NOERRORCODE)
				{
					cout<<"stauts of send : AES_OHI_NOERRORCODE "<<endl;
					running = false;
				}
			}
			else
			{
				cerr<<"sema acquire failed"<<endl;
				if (gohError == AES_OHI_NOSERVERACCESS)
				{
					running = false;
				}
			}
		}
	}
	myFileSender->detach();
	delete myFileSender;
}
