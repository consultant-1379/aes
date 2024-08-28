#include <string>
#include <list>
#include <iostream>
#include "aes_ohi_extfilehandler2.h"
#include "aes_ohi_errorcodes.h"

using namespace std;

int main()
{
    unsigned int reply = 0;
    bool status;
    //define
    AES_OHI_ExtFileHandler2*  extFileObj =  NULL;
    extFileObj = new AES_OHI_ExtFileHandler2("mySystem", "myApplname");

    //attach 
    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the attach api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    reply = extFileObj->attach();
    if ( reply != AES_OHI_NOERRORCODE )
    {
	cout<<"Attach failed due to the error :: "<<reply<<" . Error text is "<<extFileObj->getErrCodeText(reply)<<endl;
	cout<<"Leaving the application"<<endl;
	delete extFileObj;
	return 0;
    }
    else
    {
	cout<<"Attached succesfully"<<endl;
    }
	
   
    
    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the fileTransferQueueDefined api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    string tqDn("");
    reply = extFileObj->fileTransferQueueDefined("GOHSTDEST1", tqDn);
    if ( reply != AES_OHI_NOERRORCODE )
    {
	cout<<"Transfer queue does not exist in GOH"<<endl;
	cout<<"Error is "<<reply<<" . Error text is "<< extFileObj->getErrCodeText(reply)<<endl;
    }
    else
    {
	cout<<"Transfer queue exists in GOH"<<endl;
	cout<<"TQ dn is "<<tqDn<<endl;
    }

    
    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the isConnected api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    status = extFileObj->isConnected();
    if ( status )
    {
	cout<<"Application is connected"<<endl;
    }
    else
    {
	cout<<"Application is not connected"<<endl;
    }

    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the getFileTransferQueues api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    int noOfTQs = 0;
    char ** tqList[20] = { 0 } ;
    reply = extFileObj->getFileTransferQueues(noOfTQs,tqList);
    if ( reply != AES_OHI_NOERRORCODE )
    {
	cout<<"Could not fetch the transfer queue list due to the error :: "<<reply<<" . Error text is "<<extFileObj->getErrCodeText(reply)<<endl;
    }
    else
    {
    	cout<<"No of TQs are "<<noOfTQs<<endl;
    }

    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the getDestDirectoryPath api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    char dirPath[1024] = { 0 } ;
    reply = extFileObj->getDestDirectoryPath("GOHSTDEST1", dirPath);
    if ( reply != AES_OHI_NOERRORCODE )
    {
        cout<<"Could not fetch the directory path due to the error :: "<<reply<<" . Error text is "<<extFileObj->getErrCodeText(reply)<<endl;
    }
    else
    {
	cout<<"Directory path is "<<dirPath<<endl;
    }

    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the setDestDirectoryPath api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    char dirPath1[1024] = "/opt/ap/aes/bin";
    reply = extFileObj->setDestDirectoryPath("GOHSTDEST1", dirPath1);
    if ( reply != AES_OHI_NOERRORCODE )
    {
        cout<<"Could not fetch the directory path due to the error :: "<<reply<<" . Error text is "<<extFileObj->getErrCodeText(reply)<<endl;
    }
    else
    {
	cout<<"Directory path is set successfully"<<endl;
	reply = extFileObj->getDestDirectoryPath("GOHSTDEST1", dirPath);
	cout<<"Directory path is changed to "<<dirPath<<endl;
    }

    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the removeFile api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    reply = extFileObj->removeFile("GOHSTDEST1", "file0001");
    if ( reply != AES_OHI_NOERRORCODE )
    {
        cout<<"Could not remove the file due to the error :: "<<reply<<" . Error text is "<<extFileObj->getErrCodeText(reply)<<endl;
    }
    else
    {
	cout<<"Removed the file successfully"<<endl;
    }

    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the getEventFileName of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    char eventFileName[1024] = { 0 } ;
    reply = extFileObj->getEventFileName(eventFileName);
    if ( reply != AES_OHI_NOERRORCODE )
    {
        cout<<"Could not fetch the event file name due to the error :: "<<reply<<" . Error text is "<<extFileObj->getErrCodeText(reply)<<endl;
    }
    else
    {
	cout<<"Event file name is "<<eventFileName<<endl;
    }

    cout<<"-----------------------------------------------------------------------"<<endl;
    cout<<"Testing the detach api of the AES_OHI_ExtFileHandler2 class"<<endl;
    cout<<"-----------------------------------------------------------------------"<<endl;
    reply = extFileObj->detach();
    if ( reply != AES_OHI_NOERRORCODE )
    {
        cout<<"Could not detach from the application due to the error :: "<<reply<<" . Error text is "<<extFileObj->getErrCodeText(reply)<<endl;
    }
    else
    {
        cout<<"Detached successfully"<<endl;
    }

    //delete object
    if(extFileObj != NULL)
    {
        delete extFileObj;
	extFileObj = NULL;
    }

    return 0;
}
