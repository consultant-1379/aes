#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "ohi_cute_directoryhandler.h"
#include <iostream>

using namespace std;


void AES_OHI_CUTE_DirectoryHandler::send()
{
        AES_OHI_DirectoryHandler* myDirSender = NULL;
        myDirSender = new AES_OHI_DirectoryHandler(
                        "mySystem","myApplname","/home/tcschem");
        unsigned int reply = 0;
        cout<<"Calling the send API of AES_OHI_BlockHandler2"<<endl;
        reply = myDirSender->attach();
	reply = myDirSender->send("myDir");
        if ( reply == 0)
        {
            cout<<"Result of send API of AES_OHI_DirectoryHandler2 is SUCCESS"<<endl;
        }
        ASSERTM("Failed to send the transaction",reply == 0);
	myDirSender->detach();
        delete myDirSender;
        myDirSender = NULL;
}

cute::suite AES_OHI_CUTE_DirectoryHandler::make_suite_aes_cute_directoryhandler()
{
	cute::suite s;
	s.push_back(CUTE(send));
	return s;
}
