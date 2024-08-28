#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include "aes_ohi_directoryhandler.h"
//#include "aes_ohi_filehandler_r2.h"
//#include "aes_ohi_errorcodes.h"

using namespace std;

unsigned int reply;
int main()
{
	AES_OHI_Format transFormat = AES_OHI_BINARY;
	AES_OHI_DirectoryHandler* myDirSender = NULL;
	myDirSender = new AES_OHI_DirectoryHandler("mySystem","myApplname",
			"tqname","myEventText","/data/aes/data/GOHST");


	int pid = ACE_OS::getpid();
	char checkVm[1024]= { 0 } ;
	sprintf( checkVm, "%d", pid );
	string checkVm2(checkVm);
	string checkVm1 = "cat /proc/" + checkVm2 + "/status | grep VmSize";
	system(checkVm1.c_str());

	for ( int i = 0 ; i < 10 ; i++ )
	{
		reply = myDirSender->attach();
		cout<<"Reply of attach is "<<reply<<endl;

		
		system(checkVm1.c_str());

		if( reply != 0 )
		{
			cout << "attach failed " << endl;
			delete myDirSender;
			myDirSender = 0;
			return 0;
		}
		char dirName[256] = { 0 } ;
		sprintf(dirName, "%s%i", "mydir", i );

		string myDirName = dirName;
		cout << "myDirName = " << myDirName.c_str() << endl;

		reply = myDirSender->send(myDirName.c_str(), transFormat);
		cout<<"Reply of send is "<<reply<<endl;

		if( reply == 117 )
		{
			cout << "send failed " << endl;
			delete myDirSender;
			myDirSender = 0;
			return 0;
		}
		
		reply = myDirSender->detach();
		cout<<"Reply of detach is "<<reply<<endl;
		if( reply == 117 )
		{
			delete myDirSender;
			myDirSender = 0;
			cout << "detach failed " << endl;
			return 0;
		}
		system(checkVm1.c_str());

	}
	delete myDirSender;
	myDirSender = 0;
	return 0;
}
