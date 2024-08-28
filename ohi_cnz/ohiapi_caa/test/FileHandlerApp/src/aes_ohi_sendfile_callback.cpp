#include <string>
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_eventcodes.h"
#include <ace/ACE.h>
#include <iostream>

bool createFile(std::string path,std::string fullName)
{
	std::string fileName = path+fullName;

	FILE * fp = ACE_OS::fopen(fileName.c_str(),"a+" ); //666
	if(fp == NULL)
	{
		std::cout<<"Error in creating the file , errno :"<<errno<<std::endl;
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

class FileTransfer: public AES_OHI_FileHandler
{
public:
	FileTransfer(std::string subSys, const std::string appName,std::string transferQueue, const std::string eventText);
	virtual int handleEvent(int eventCode);
	bool isRunning;
};

FileTransfer::FileTransfer(std::string subSys, const std::string appName,std::string transferQueue, const std::string eventText)
:AES_OHI_FileHandler(subSys.c_str(),appName.c_str(),transferQueue.c_str(),eventText.c_str()," ")
{
	std::cout<<"In FileTransfer constructor , Creating File handler object"<<std::endl;

}

int	 FileTransfer::handleEvent(int eventCode)
{
	std::string fileName;
	switch(eventCode)
	{

	case AES_OHI_EVELINKDOWN:
	{
		std::cerr<<"My link is down "<<std::endl;
		break;
	}

	case AES_OHI_EVELINKDOWNCEASE:
	{
		std::cerr<<"My link is up "<<std::endl;
		break;
	}

	case AES_OHI_EVEFILETRANSFERFAILED:
	{
		getEventFileName((ACE_TCHAR*)fileName.c_str());
		std::cerr<<"File Transfer failed "<<std::endl;
		break;
	}
	}
	return AES_OHI_NOERRORCODE;
}




int main()
{
	FileTransfer * myFileSender;
	std::string fileName = "/file_transfer";
	std::string fullName;
	std::string path = "/opt";
	char subname[50];
	int fileNo = 1;

	myFileSender = new FileTransfer("TST","TESTAPP","TESTTRANSFERQUEUE","Used for testing file transfer");
	myFileSender->isRunning = true;
	if(myFileSender->attach() == AES_OHI_NOERRORCODE)
	{
		//myFileSender->getDirectoryPath((ACE_TCHAR*)path.c_str());
//		while( myFileSender->isRunning)
//		{
		std::cout<<"path = "<<path.c_str()<<std::endl;
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			std::cout<<"full name = "<<fullName.c_str()<<std::endl;
			createFile(path,fullName);

			if(myFileSender->send(subname)!= AES_OHI_NOERRORCODE)
			{
				std::cerr<<"send status != AES_OHI_NOERRORCODE "<<std::endl;
				myFileSender->isRunning = false;
			}
			sleep(1);
//		}
		myFileSender->detach();

	}
	delete myFileSender;
}




