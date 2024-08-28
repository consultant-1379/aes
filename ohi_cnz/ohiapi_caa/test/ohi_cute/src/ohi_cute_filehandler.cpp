#include "cute.h"
#include <string.h>
#include "ide_listener.h"
#include "cute_runner.h"
#include "ohi_cute_filehandler.h"
#include "aes_ohi_filehandler.h"
#include "aes_ohi_format.h"
#include <ace/Semaphore.h>
unsigned int retval = 1;
AES_OHI_FileHandler * AES_OHI_CUTE_FileHandler::pFileHandler = NULL;
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

void AES_OHI_CUTE_FileHandler::vAttach()
{
	retval =  pFileHandler->attach();
	bool bstatus = 0;
	bstatus = pFileHandler->isConnected();
	ASSERTM("Failure occurred during attach ",((retval == 0) && (bstatus == 1)));
}

void AES_OHI_CUTE_FileHandler::vGetDirectoryPath()
{
	char * path = new char[30];;
	retval = pFileHandler->getDirectoryPath(path);
	ASSERTM("Failure occurred while retrieving DirectoryPath ",retval==0);
}
void AES_OHI_CUTE_FileHandler::vSend()
{
	std::string path= "/opt";
	std::string fileName = "/file";
	std::string subName = "0001";
	std::string fullName = fileName+subName;
	if(createFile(path,fullName))
	{
		retval = pFileHandler->send(subName.c_str(),AES_OHI_DEFAULT);
	}

	ASSERTM("Failed to send the file ",retval==0);
}
void AES_OHI_CUTE_FileHandler::vSetTransferState()
{
	std::string fileName = "/opt/file0001";
	std::string destination = "/temp";

	retval = pFileHandler->setTransferState(fileName.c_str(),destination.c_str(),AES_OHI_FSREADY);
	ASSERTM("Failed to setTransferState ",retval==0);
}

void AES_OHI_CUTE_FileHandler::vGetTransferState()
{
	std::string fileName = "/opt/file0001";
	AES_OHI_Filestates status;
	retval = pFileHandler->getTransferState(fileName.c_str(),status);
	ASSERTM("Failed to getTransferState ",retval==0);
}
void AES_OHI_CUTE_FileHandler::vGetTransferStateEx()
{
	char * fileName;
	fileName = new char[30];
	strcpy(fileName,"/opt/file0001");
	AES_OHI_Filestates status;
	char* destination = new char[30];
	int listOrder;
	char* reportDate =  new char[30];
	char* deleteDate = new char[30];
	char* manualStart = new char[30];
	char* manualStop = new char[30];
	char* failDate = new char[30];
	char* archiveDate = new char[30];
	int reasonForFailed;
	bool isDirectory;
	retval = pFileHandler->getTransferStateEx(fileName,
			status,
			destination,
			listOrder,
			reportDate,
			deleteDate,
			manualStart,
			manualStop,
			failDate,
			archiveDate,
			reasonForFailed,
			isDirectory);

	ASSERTM("Failed to getTransferStateEx ",retval==0);
}
void AES_OHI_CUTE_FileHandler::vGetLastSentFile()
{
	char * fileName;
	fileName = new char[20];
	pFileHandler->getLastSentFile(fileName);
	ASSERTM("Failed to get LastSentFile ",retval==0);
}
void AES_OHI_CUTE_FileHandler::vGetEventFileName()
{
	char * filename;
	filename =  new char[20];
	pFileHandler->getEventFileName(filename);
	ASSERTM("Failed to get EventFileName ",retval==0);
}
void AES_OHI_CUTE_FileHandler::vHandleEvent()
{
	AES_OHI_Eventcodes eventCode = AES_OHI_EVELINKDOWNCEASE;
	pFileHandler->handleEvent(eventCode);
	ASSERTM("Event handled by application ",retval==AES_OHI_EVENTNOTHANDLED);
}

void AES_OHI_CUTE_FileHandler::vGetEventHandle()
{
	ACE_Semaphore * semaHandle = NULL;
	semaHandle = pFileHandler->getEventHandle();
	ASSERTM("Failed to get semaphore event handle ",semaHandle != NULL);
}
void AES_OHI_CUTE_FileHandler::vGetEvent()
{
	AES_OHI_Eventcodes eventCode;
	retval = pFileHandler->getEvent(eventCode);
	ASSERTM("Failed to get get Event code ",retval==AES_OHI_NOERRORCODE);
}
void AES_OHI_CUTE_FileHandler::vDetach()
{
	retval = pFileHandler->detach();
	bool bstatus = pFileHandler->isConnected();
	ASSERTM("Failed to detach",(retval==AES_OHI_NOERRORCODE)&&(bstatus == 0));
}
void AES_OHI_CUTE_FileHandler::vGetErrCodeText()
{
	unsigned int errCode;
	const char * errorText = " ";
	errorText=pFileHandler->getErrCodeText(errCode);
	ASSERTM("Failed to get the Error Text",strlen(errorText)>0);
}

void AES_OHI_CUTE_FileHandler::vAttachingTwice()
{
	retval =  pFileHandler->attach();
	bool bstatus = 0;
	bstatus = pFileHandler->isConnected();
	ASSERTM("Failed to attach without detach ",((retval == 0) && (bstatus == 1)));
}
void AES_OHI_CUTE_FileHandler::vDetachWithoutAttach()
{
	bool bstatus = pFileHandler->detach();
	ASSERTM("Detach called without attach",bstatus);
}

cute::suite AES_OHI_CUTE_FileHandler::make_suite_aes_cute_filehandler()
{
	pFileHandler= new AES_OHI_FileHandler("CUTE","CUTETESTAPL","TESTTRANSFERQUEUE","Used for testing");
	cute::suite s;
	s.push_back(CUTE(vDetachWithoutAttach));
	s.push_back(CUTE(vAttach));
	s.push_back(CUTE(vAttachingTwice));
	s.push_back(CUTE(vGetDirectoryPath));
	s.push_back(CUTE(vSetTransferState));
	s.push_back(CUTE(vGetTransferState));
	s.push_back(CUTE(vGetTransferStateEx));
	s.push_back(CUTE(vGetLastSentFile));
	s.push_back(CUTE(vGetEventFileName));
	s.push_back(CUTE(vSend));
	//	s.push_back(CUTE(vHandleEvent));
	s.push_back(CUTE(vGetEventHandle));
	s.push_back(CUTE(vGetEvent));
	s.push_back(CUTE(vGetErrCodeText));
	s.push_back(CUTE(vDetach));

	return s;
}

