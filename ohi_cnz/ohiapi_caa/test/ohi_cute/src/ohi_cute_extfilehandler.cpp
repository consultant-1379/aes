#include <iostream>
#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "ohi_cute_extfilehandler.h"

using namespace std;

void AES_OHI_CUTE_ExtFileHandler::attach()
{
    unsigned int reply = 0;
    AES_OHI_ExtFileHandler2*  extFileObj =  NULL;
    extFileObj = new AES_OHI_ExtFileHandler2("mySystem", "myApplName");
    cout<<"Calling the attach API of AES_OHI_ExtFileHandler2 "<<endl;
    reply = extFileObj->attach();
    if ( reply == 0 )
    {
        cout<<"Result of attach API of AES_OHI_ExtFileHandler2 is SUCCESS"<<endl;
    }
    ASSERTM("Failed to attach" ,reply == 0);
    reply = extFileObj->detach();
    if(extFileObj != NULL)
    {
        delete extFileObj;
        extFileObj = NULL;
    }
}

void AES_OHI_CUTE_ExtFileHandler::detach()
{
    unsigned int reply = 0;
    AES_OHI_ExtFileHandler2*  extFileObj =  NULL;
    extFileObj = new AES_OHI_ExtFileHandler2("mySystem", "myApplName");
    reply = extFileObj->attach();
    cout<<"Calling the detach API of AES_OHI_ExtFileHandler2 "<<endl;
    reply = extFileObj->detach();
    if ( reply == 0 )
    {
        cout<<"Result of detach API of AES_OHI_ExtFileHandler2 is SUCCESS"<<endl;
    }
    ASSERTM("Failed to detach" ,reply == 0);
    if(extFileObj != NULL)
    {
        delete extFileObj;
        extFileObj = NULL;
    }
}

void AES_OHI_CUTE_ExtFileHandler::fileTransferQueueDefined()
{
    unsigned int reply = 0;
    AES_OHI_ExtFileHandler2*  extFileObj =  NULL;
    extFileObj = new AES_OHI_ExtFileHandler2("mySystem", "myApplName");
    reply = extFileObj->attach();
    cout<<"Calling the fileTransferQueueDefined API of AES_OHI_ExtFileHandler2 "<<endl;
    reply = extFileObj->fileTransferQueueDefined("myTQ");
    if ( reply == 0 )
    {
        cout<<"Result of fileTransferQueueDefined API of AES_OHI_ExtFileHandler2 is SUCCESS"<<endl;
    }
    ASSERTM("Failed to find if the file transfer queue is defined or not" ,reply == 0);
    reply = extFileObj->detach();
    if(extFileObj != NULL)
    {
        delete extFileObj;
        extFileObj = NULL;
    }
}

void AES_OHI_CUTE_ExtFileHandler::getEvent()
{
    unsigned int reply = 0;
    AES_OHI_Eventcodes eventCode;
    AES_OHI_ExtFileHandler2*  extFileObj =  NULL;
    extFileObj = new AES_OHI_ExtFileHandler2("mySystem", "myApplName");
    reply = extFileObj->attach();
    cout<<"Calling the getEvent API of AES_OHI_ExtFileHandler2 "<<endl;
    reply = extFileObj->getEvent(eventCode);
    if ( reply == 0 )
    {
        cout<<"Result of getEvent API of AES_OHI_ExtFileHandler2 is SUCCESS"<<endl;
    }
    ASSERTM("Failed to fetch the getEvent" ,reply == 0);
    reply = extFileObj->detach();
    if(extFileObj != NULL)
    {
        delete extFileObj;
        extFileObj = NULL;
    }
}

void AES_OHI_CUTE_ExtFileHandler::removeFile()
{
    unsigned int reply = 0;
    AES_OHI_ExtFileHandler2*  extFileObj =  NULL;
    extFileObj = new AES_OHI_ExtFileHandler2("mySystem", "myApplName");
    reply = extFileObj->attach();
    cout<<"Calling the removeFile API of AES_OHI_ExtFileHandler2 "<<endl;
    reply = extFileObj->removeFile("abc", "myfile");
    if ( reply == 0 )
    {
        cout<<"Result of removeFile API of AES_OHI_ExtFileHandler2 is SUCCESS"<<endl;
    }
    ASSERTM("Failed to remove the file" ,reply == 0);
    reply = extFileObj->detach();
    if(extFileObj != NULL)
    {
        delete extFileObj;
        extFileObj = NULL;
    }
}

cute::suite AES_OHI_CUTE_ExtFileHandler::make_suite_aes_cute_extfilehandler()
{
	cute::suite s;
	s.push_back(CUTE(attach));
	s.push_back(CUTE(detach));
	s.push_back(CUTE(fileTransferQueueDefined));
	s.push_back(CUTE(getEvent));
	s.push_back(CUTE(removeFile));
	return s;
}
