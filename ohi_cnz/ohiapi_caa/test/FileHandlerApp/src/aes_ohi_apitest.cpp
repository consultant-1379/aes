#include <string>
#include "aes_ohi_filehandler.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_eventcodes.h"
#include <ace/ACE.h>
#include <iostream>

using namespace std;

bool createFile(std::string path,std::string fullName)
{
	cout<<"In createFile. Path is "<<path<<" fullname is "<<fullName<<endl; 
	std::string fileName = path+'/';
	fileName = fileName+fullName;
	cout<<"File name is "<<fileName.c_str()<<endl;

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
	cout<<"Created file successfully"<<endl;
	cout<<"Out createFile"<<endl;
	return true;

}

class FileTransfer: public AES_OHI_FileHandler
{
public:
	FileTransfer(std::string subSys, const std::string appName,std::string transferQueue, const std::string eventText, const std::string path);
	//virtual int handleEvent(int eventCode);
	unsigned int virtual handleEvent(AES_OHI_Eventcodes eventCode);
	bool isRunning;
};

FileTransfer::FileTransfer(std::string subSys, const std::string appName,std::string transferQueue, const std::string eventText, const std::string path)
:AES_OHI_FileHandler(subSys.c_str(),appName.c_str(),transferQueue.c_str(),eventText.c_str(),path.c_str())
{
	std::cout<<"In FileTransfer constructor , Creating File handler object"<<std::endl;

}

unsigned int FileTransfer::handleEvent(AES_OHI_Eventcodes eventCode)
{
	cout<<"In handleEvent"<<endl;
	cout<<"Event code received is "<<eventCode<<endl;
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
	cout<<"Out handleEnent"<<endl;
	return AES_OHI_NOERRORCODE;
}




int main()
{
	cout<<"Testing File transfer using callback approach"<<endl;
	FileTransfer * myFileSender;
	AES_OHI_Errorcodes result;
	std::string fileName = "file";
	std::string fullName;
	std::string outputPath="/data/aes/data/GOHST";
	char tmppath[1024] = { 0 };
	char subname[50];
	int fileNo = 1;
	AES_OHI_Filestates fileState;

	myFileSender = new FileTransfer("TST","TESTAPP","GOHSTDEST1","Used for testing file transfer",outputPath);
	cout<<"Created an object of FileTransfer type"<<endl;
	myFileSender->isRunning = true;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 1) Testing the attach and detach api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
	result = (AES_OHI_Errorcodes) myFileSender->attach();
	if ( result != AES_OHI_NOERRORCODE )
	{
		cout<<"Attach failed due to the error :: "<<result<<endl;
		cout<<"Attach failed due to the error text:: "<<myFileSender->getErrCodeText(result)<<endl;
		delete myFileSender;
		return -1;
	}
	else
	{
		cout<<"Attached successfully "<<endl;
		cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
		result = (AES_OHI_Errorcodes) myFileSender->detach();
		if ( result != AES_OHI_NOERRORCODE )
		{
			cout<<"Detach failed due to the error :: "<<result<<"Error text is "<<myFileSender->getErrCodeText(result)<<endl;
			cout<<"Leaving the application"<<endl;
			delete myFileSender;
			return -1;
		}
		else
		{
			cout<<"Detached successfully"<<endl;
		}
	}

	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 2) Testing the getDirectoryPath api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<". Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		cout<<"Trying to get the directory path"<<endl;
		char path[1024] = { 0 };
		result = (AES_OHI_Errorcodes) myFileSender->getDirectoryPath(path);
		if ( result != AES_OHI_NOERRORCODE )
		{
			cout<<"Fetching the directory path failed due to the error :: "<<result <<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
			cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->detach();
			if ( result != AES_OHI_NOERRORCODE )
			{
				cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Leaving the application"<<endl;
				delete myFileSender;
				return -1;
			}
			else
			{
				cout<<"Detached successfully"<<endl;
			}
		}
		else
		{
			cout<<"Fetched the directory path successfully"<<endl;
			cout<<"Directory path is "<<path<<endl;
			cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->detach();
			if ( result != AES_OHI_NOERRORCODE )
			{
				cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Leaving the application"<<endl;
				delete myFileSender;
				return -1;
			}
			else
			{
				cout<<"Detached successfully"<<endl;
			}
		}
        }

	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 3) Testing the send api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		char path[1024] = { 0 };
                result = (AES_OHI_Errorcodes) myFileSender->getDirectoryPath(path);
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Fetching the directory path failed due to the error :: "<<result <<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                        if ( result != AES_OHI_NOERRORCODE )
                        {
                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                cout<<"Leaving the application"<<endl;
                                delete myFileSender;
                                return -1;
                        }
                        else
                        {
                                cout<<"Detached successfully"<<endl;
                        }
                }
		else
		{
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			createFile(path,fullName);
			cout<<"Trying to send the file "<<fullName<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->send(fullName.c_str());
			if(result != AES_OHI_NOERRORCODE)
			{
				cout<<"Send failed for the file "<<fullName.c_str() <<" due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->detach();
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
					cout<<"Leaving the application"<<endl;
					delete myFileSender;
					return -1;
				}
				else
				{
					cout<<"Detached successfully"<<endl;
				}
			}
			else
			{
				cout<<"Sent the file "<<fullName.c_str()<<" successfully"<<endl;
				cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                                result = (AES_OHI_Errorcodes) myFileSender->detach();
                                if ( result != AES_OHI_NOERRORCODE )
                                {
                                        cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                        cout<<"Leaving the application"<<endl;
                                        delete myFileSender;
                                        return -1;
                                }
                                else
                                {
                                        cout<<"Detached successfully"<<endl;
                                }
			}
		}
        }

	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 4) Testing the getLastSentFile api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		char path[1024] = { 0 };
                result = (AES_OHI_Errorcodes) myFileSender->getDirectoryPath(path);
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Fetching the directory path failed due to the error :: "<<result <<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                        if ( result != AES_OHI_NOERRORCODE )
                        {
                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                cout<<"Leaving the application"<<endl;
                                delete myFileSender;
                                return -1;
                        }
                        else
                        {
                                cout<<"Detached successfully"<<endl;
                        }
                }
		else
		{
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			createFile(path,fullName);
			cout<<"Trying to send the file "<<fullName<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->send(fullName.c_str());
			if(result != AES_OHI_NOERRORCODE)
			{
				cout<<"Send failed for the file "<<fullName.c_str() <<" due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->detach();
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
					cout<<"Leaving the application"<<endl;
					delete myFileSender;
					return -1;
				}
				else
				{
					cout<<"Detached successfully"<<endl;
				}
			}
			else
			{
				cout<<"Sent the file "<<fullName.c_str()<<" successfully"<<endl;
				cout<<"Trying to get the last sent file "<<endl;
				char lastfile[1024] = { 0 };
				result = (AES_OHI_Errorcodes) myFileSender->getLastSentFile(lastfile);
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Could not fetch the last sent file due to the error :: "<<result<<endl;
					cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
					result = (AES_OHI_Errorcodes) myFileSender->detach();
					if ( result != AES_OHI_NOERRORCODE )
					{
						cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
						cout<<"Leaving the application"<<endl;
						delete myFileSender;
						return -1;
					}
					else
					{
						cout<<"Detached successfully"<<endl;
					}
				}
				else
				{
					cout<<"Fetched the last sent file successfully"<<endl;
					cout<<"LAST SENT FILE IS :: "<<lastfile<<endl;	
				        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                                        if ( result != AES_OHI_NOERRORCODE )
                                        {
                                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                                cout<<"Leaving the application"<<endl;
                                                delete myFileSender;
                                                return -1;
                                        }
                                        else
                                        {
                                                cout<<"Detached successfully"<<endl;
                                        }	
				}
			}
		}
        }

	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 5) Testing the getEventFileName api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		char path[1024] = { 0 };
                result = (AES_OHI_Errorcodes) myFileSender->getDirectoryPath(path);
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Fetching the directory path failed due to the error :: "<<result <<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                        if ( result != AES_OHI_NOERRORCODE )
                        {
                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                cout<<"Leaving the application"<<endl;
                                delete myFileSender;
                                return -1;
                        }
                        else
                        {
                                cout<<"Detached successfully"<<endl;
                        }
                }
		else
		{
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			createFile(path,fullName);
			cout<<"Trying to send the file "<<fullName<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->send(fullName.c_str());
			if(result != AES_OHI_NOERRORCODE)
			{
				cout<<"Send failed for the file "<<fullName.c_str() <<" due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->detach();
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
					cout<<"Leaving the application"<<endl;
					delete myFileSender;
					return -1;
				}
				else
				{
					cout<<"Detached successfully"<<endl;
				}
			}
			else
			{
				cout<<"Sent the file "<<fullName.c_str()<<" successfully"<<endl;
				cout<<"Trying to get the last sent file "<<endl;
				char eventfilename[1024] = { 0 };
				result = (AES_OHI_Errorcodes) myFileSender->getEventFileName(eventfilename);
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Could not fetch the event file name due to the error :: "<<result<<endl;
					cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
					result = (AES_OHI_Errorcodes) myFileSender->detach();
					if ( result != AES_OHI_NOERRORCODE )
					{
						cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
						cout<<"Leaving the application"<<endl;
						delete myFileSender;
						return -1;
					}
					else
					{
						cout<<"Detached successfully"<<endl;
					}
				}
				else
				{
					cout<<"Fetched the event file name successfully"<<endl;
					cout<<"EVENT FILE NAME IS :: "<<eventfilename<<endl;	
				        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                                        if ( result != AES_OHI_NOERRORCODE )
                                        {
                                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                                cout<<"Leaving the application"<<endl;
                                                delete myFileSender;
                                                return -1;
                                        }
                                        else
                                        {
                                                cout<<"Detached successfully"<<endl;
                                        }	
				}
			}
		}
        }

	cout<<"----------------------------------------------------------------------------"<<endl;
        cout<<"\n 6) Testing the isConnected api of AES_OHI_FileHandler class \n"<<endl;
        cout<<"----------------------------------------------------------------------------"<<endl;
        cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		cout<<"Trying to check if connection is there"<<endl;
		bool status =  myFileSender->isConnected();
		cout<<"Status of connection is "<<status<<endl;
                cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                result = (AES_OHI_Errorcodes) myFileSender->detach();
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Leaving the application"<<endl;
                        delete myFileSender;
                        return -1;
                }
                else
                {
                        cout<<"Detached successfully"<<endl;
                }
        }

	cout<<"----------------------------------------------------------------------------"<<endl;
        cout<<"\n 7) Testing the getErrCodeText api of AES_OHI_FileHandler class \n"<<endl;
        cout<<"----------------------------------------------------------------------------"<<endl;
        cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		cout<<"Trying to get the error code text of the error::"<<result<<endl;
		cout<<"Error code text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                result = (AES_OHI_Errorcodes) myFileSender->detach();
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Leaving the application"<<endl;
                        delete myFileSender;
                        return -1;
                }
                else
                {
                        cout<<"Detached successfully"<<endl;
                }
        }

	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 8) Testing the getTransferState api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		char path[1024] = { 0 };
                result = (AES_OHI_Errorcodes) myFileSender->getDirectoryPath(path);
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Fetching the directory path failed due to the error :: "<<result <<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                        if ( result != AES_OHI_NOERRORCODE )
                        {
                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                cout<<"Leaving the application"<<endl;
                                delete myFileSender;
                                return -1;
                        }
                        else
                        {
                                cout<<"Detached successfully"<<endl;
                        }
                }
		else
		{
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			createFile(path,fullName);
			cout<<"Trying to send the file "<<fullName<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->send(fullName.c_str());
			if(result != AES_OHI_NOERRORCODE)
			{
				cout<<"Send failed for the file "<<fullName.c_str() <<" due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->detach();
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
					cout<<"Leaving the application"<<endl;
					delete myFileSender;
					return -1;
				}
				else
				{
					cout<<"Detached successfully"<<endl;
				}
			}
			else
			{
				cout<<"Sent the file "<<fullName.c_str()<<" successfully"<<endl;
				cout<<"Trying to get the transfer status of the file "<<fullName.c_str()<<endl;
				AES_OHI_Filestates state;
				result = (AES_OHI_Errorcodes) myFileSender->getTransferState(fullName.c_str(), state);
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"result of fetching the transfer state is  "<<myFileSender->getErrCodeText(result)<<endl;
				}
				else
				{
					cout<<"Transfer state of the file is "<<state<<endl;
				}
				sleep(5);
				cout<<"Checking the status of the file after 5 seconds"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->getTransferState(fullName.c_str(), state);
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"result of fetching the transfer state is  "<<myFileSender->getErrCodeText(result)<<endl;
				}
				else
				{
					cout<<"Transfer state of the file is "<<state<<endl;
				}
				cout<<"Trying to detach the transfer queue::GOHSTDEST1"<<endl;
                                result = (AES_OHI_Errorcodes) myFileSender->detach();
                                if ( result != AES_OHI_NOERRORCODE )
                                {
                                        cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                        cout<<"Leaving the application"<<endl;
                                        delete myFileSender;
                                        return -1;
                                }
                                else
                                {
                                        cout<<"Detached successfully"<<endl;
                                }
			}
		}
        }

	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 9) Testing the getTransferState api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		char path[1024] = { 0 };
                result = (AES_OHI_Errorcodes) myFileSender->getDirectoryPath(path);
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Fetching the directory path failed due to the error :: "<<result <<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                        if ( result != AES_OHI_NOERRORCODE )
                        {
                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                cout<<"Leaving the application"<<endl;
                                delete myFileSender;
                                return -1;
                        }
                        else
                        {
                                cout<<"Detached successfully"<<endl;
                        }
                }
		else
		{
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			createFile(path,fullName);
			cout<<"Trying to send the file "<<fullName<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->send(fullName.c_str());
			if(result != AES_OHI_NOERRORCODE)
			{
				cout<<"Send failed for the file "<<fullName.c_str() <<" due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->detach();
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
					cout<<"Leaving the application"<<endl;
					delete myFileSender;
					return -1;
				}
				else
				{
					cout<<"Detached successfully"<<endl;
				}
			}
			else
			{
				cout<<"Sent the file "<<fullName.c_str()<<" successfully"<<endl;
				cout<<"Trying to get the transfer status of the file "<<fullName.c_str()<<endl;
				char name[1024] = {0};
				AES_OHI_Filestates state;
				char destset[1024] = { 0 } ;
				int listOrder = 0;
				char reportDate[1024] = { 0 };
				char deleteDate[1024] = { 0 };
				char manualStart[1024] = { 0 };
				char manualStop[1024] = { 0 };
				char failDate[1024] = { 0 };
				char archiveDate[1024] = { 0 };
				int reasonForFailed = 0;
				bool isDirectory("false");
				result = (AES_OHI_Errorcodes) myFileSender->getTransferStateEx(name, 
												state, 
												destset,
												listOrder,
												reportDate,
												deleteDate,
												manualStart,
												manualStop,
												failDate,
												archiveDate,
												reasonForFailed,
												isDirectory );
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"result of fetching the transfer state is  "<<myFileSender->getErrCodeText(result)<<endl;
				}
				else
				{
					cout<<"File name is "<<name<<endl;
					cout<<"Transfer state of the file is "<<state<<endl;
					cout<<"Dest set is "<<destset<<endl;
					cout<<"List order is "<<listOrder<<endl;
					cout<<"reportDate is "<<reportDate<<endl;
					cout<<"deleteDate is "<<deleteDate<<endl;	
					cout<<"manualStart is "<<manualStart<<endl;
					cout<<"manualStop  is "<<manualStop<<endl;
					cout<<"failDate is "<<failDate<<endl;
					cout<<"archiveDate is "<<archiveDate<<endl;
					cout<<"reasonForFailed is "<<reasonForFailed<<endl;
					cout<<"isDirectory is "<<isDirectory<<endl;
				}
				sleep(5);
				cout<<"Checking the status of the file after 5 seconds"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->getTransferStateEx(name,
												state, 
												destset,
												listOrder,
												reportDate,
												deleteDate,
												manualStart,
												manualStop,
												failDate,
												archiveDate,
												reasonForFailed,
												isDirectory );
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"result of fetching the transfer state is  "<<myFileSender->getErrCodeText(result)<<endl;
				}
				else
				{
					cout<<"File name is "<<name<<endl;
					cout<<"Transfer state of the file is "<<state<<endl;
					cout<<"Dest set is "<<destset<<endl;
					cout<<"List order is "<<listOrder<<endl;
					cout<<"reportDate is "<<reportDate<<endl;
					cout<<"deleteDate is "<<deleteDate<<endl;	
					cout<<"manualStart is "<<manualStart<<endl;
					cout<<"manualStop  is "<<manualStop<<endl;
					cout<<"failDate is "<<failDate<<endl;
					cout<<"archiveDate is "<<archiveDate<<endl;
					cout<<"reasonForFailed is "<<reasonForFailed<<endl;
					cout<<"isDirectory is "<<isDirectory<<endl;
					cout<<"Value of true is"<<true<<endl;
					cout<<"Value of false is "<<false<<endl;
				}
				cout<<"Trying to detach the transfer queue::GOHSTDEST1"<<endl;
                                result = (AES_OHI_Errorcodes) myFileSender->detach();
                                if ( result != AES_OHI_NOERRORCODE )
                                {
                                        cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                        cout<<"Leaving the application"<<endl;
                                        delete myFileSender;
                                        return -1;
                                }
                                else
                                {
                                        cout<<"Detached successfully"<<endl;
                                }
			}
		}
        }
#if 0 
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"\n 10) Testing the setTransferState api of AES_OHI_FileHandler class \n"<<endl;
	cout<<"----------------------------------------------------------------------------"<<endl;
	cout<<"Trying to attach to the transfer queue::GOHSTDEST1"<<endl;
        result = (AES_OHI_Errorcodes) myFileSender->attach();
        if ( result != AES_OHI_NOERRORCODE )
        {
                cout<<"Attach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                cout<<"Leaving the application"<<endl;
                delete myFileSender;
                return -1;
        }
        else
        {
                cout<<"Attached successfully "<<endl;
		char path[1024] = { 0 };
                result = (AES_OHI_Errorcodes) myFileSender->getDirectoryPath(path);
                if ( result != AES_OHI_NOERRORCODE )
                {
                        cout<<"Fetching the directory path failed due to the error :: "<<result <<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                        cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
                        result = (AES_OHI_Errorcodes) myFileSender->detach();
                        if ( result != AES_OHI_NOERRORCODE )
                        {
                                cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                cout<<"Leaving the application"<<endl;
                                delete myFileSender;
                                return -1;
                        }
                        else
                        {
                                cout<<"Detached successfully"<<endl;
                        }
                }
		else
		{
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			createFile(path,fullName);
			cout<<"Trying to send the file "<<fullName<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->send(fullName.c_str());
			if(result != AES_OHI_NOERRORCODE)
			{
				cout<<"Send failed for the file "<<fullName.c_str() <<" due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
				cout<<"Try to detach the transfer queue::GOHSTDEST1"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->detach();
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
					cout<<"Leaving the application"<<endl;
					delete myFileSender;
					return -1;
				}
				else
				{
					cout<<"Detached successfully"<<endl;
				}
			}
			else
			{
				cout<<"Sent the file "<<fullName.c_str()<<" successfully"<<endl;
				cout<<"Trying to get the transfer status of the file "<<fullName.c_str()<<endl;
				AES_OHI_Filestates state;
				sleep(5);
				cout<<"Checking the status of the file after 5 seconds"<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->getTransferState(fullName.c_str(), state);
				if ( result != AES_OHI_NOERRORCODE )
				{
					 cout<<"result of fetching the transfer state is  "<<myFileSender->getErrCodeText(result)<<endl;
				}
				else
				{
					cout<<"Transfer state of the file is "<<state<<endl;
				}
				cout<<"Trying to set the state of the transfer file to FAILED "<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->setTransferState(fullName.c_str(), "initdestset", AES_OHI_FSFAILED);
				if ( result != AES_OHI_NOERRORCODE )
				{
					cout<<"Could not set the transfer state due to the error :: "<<result<<" . Error codes is "<<myFileSender->getErrCodeText(result)<<endl;

				}
				else
				{
					cout<<"Set the transfer state successfully"<<endl;
				}
				cout<<"Trying to detach the transfer queue::GOHSTDEST1"<<endl;
                                result = (AES_OHI_Errorcodes) myFileSender->detach();
                                if ( result != AES_OHI_NOERRORCODE )
                                {
                                        cout<<"Detach failed due to the error :: "<<result<<" . Error text is "<<myFileSender->getErrCodeText(result)<<endl;
                                        cout<<"Leaving the application"<<endl;
                                        delete myFileSender;
                                        return -1;
                                }
                                else
                                {
                                        cout<<"Detached successfully"<<endl;
                                }
			}
		}
        }

#endif
#if 0 	
	if( result == AES_OHI_NOERRORCODE)
	{
		cout<<"Attached successfully"<<endl;
		myFileSender->getDirectoryPath(tmppath);
		string mypath = tmppath;
		while( myFileSender->isRunning  )
		{
			sprintf(subname,"%04u",fileNo++);
			fullName = fileName+subname;
			createFile(mypath.c_str(),fullName);

			cout<<"Trying to send the file "<<fullName<<endl;
			result = (AES_OHI_Errorcodes) myFileSender->send(fullName.c_str());
			if(result != AES_OHI_NOERRORCODE)
			{
				cout<<"Error occurred while sending the file "<<fullName.c_str()<<endl;
				cout<<"Error is "<<result<<endl;
				myFileSender->isRunning = false;
			}
			else
			{
				cout<<"Send the file "<<fullName.c_str()<<" succesfully "<<endl;
				result = (AES_OHI_Errorcodes) myFileSender->getTransferState(fullName.c_str(), fileState);
				cout<<"Status of setting the transfer state to the file is "<<result<<endl;
				cout<<"File state of the file is "<<fileState<<endl;
			}
			sleep(1);
				result = (AES_OHI_Errorcodes) myFileSender->getTransferState(fullName.c_str(), fileState);
				cout<<"Status of setting the transfer state to the file is "<<result<<endl;
				cout<<"File state of the file is "<<fileState<<endl;
		}
		myFileSender->detach();

	}
	else
	{
		cout<<"Could not attach to the transfer queue. Error code is "<<result<<endl;
	}
#endif

	delete myFileSender;
	return 0;
}




