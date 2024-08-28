#include <iostream>
#include <list>
#include <fstream>
#include <string.h>
#include <utility>
#include <sys/types.h>
#include <dirent.h>
#include "aes_ohi_filehandler.h"
#include "aes_ohi_extfilehandler2.h"
using namespace std;

void testAttach();

int main()
{
	DIR  *handle = 0;
	struct dirent  *entry = 0;

	string  fName(""), fileToSend("");
	AES_OHI_FileHandler  *fileHandler = 0;
	int i = 1;
	char dirName[256] = { 0 };

	sprintf( dirName, "/data/aes/data/GOHST/GOHSTDEST%d", i );

	fileHandler = new AES_OHI_FileHandler ("TEST", "TESTATTACH", "GOHSTDEST1", "Transfer Queue handling", dirName);

	int errorCode;
	int cntr = 0;

	while( 1)
	{

		errorCode = fileHandler->attach();
		cout << "result of attach = " << errorCode << endl;

		testAttach();

		if (errorCode == AES_OHI_NOERRORCODE)
		{
			if ((handle = opendir (dirName)) == NULL)  return 0;

			if ((entry = readdir(handle)) != NULL)
			{
				fName = entry->d_name;
				if( strcmp( fName.c_str(), ".") !=0 && strcmp(fName.c_str(), "..") != 0 )
				{
					fileToSend = fName;
					cout << " fileToSend = " << fileToSend << endl;

					errorCode = fileHandler->send (fileToSend.c_str());
					cout << " error code for send = " << errorCode << endl;
				}
			}
			closedir(handle);
		
			errorCode = fileHandler->detach();
			cout << " error code for detach = " << errorCode << endl;

		}
		else
		{	
			cout << "Transfer Queue send to GOH :  attach error " <<  errorCode << endl;
			errorCode = fileHandler->detach();
		}

		cout << "Deleting the fileHandler object " << endl;
	}

	delete  fileHandler;
	fileHandler = 0;

}

void testAttach()
{
	int i = 1;
	unsigned int  errorCode;
	char dirName[256] = { 0 };

	sprintf( dirName, "/data/aes/data/GOHST/GOHSTDEST%d", i );

	AES_OHI_ExtFileHandler2  *extFileH;
	extFileH = new AES_OHI_ExtFileHandler2 ("TEST", "TESTATTACH");
	errorCode = extFileH->attach();
	if (errorCode == AES_OHI_NOERRORCODE)
	{
			errorCode = extFileH->fileTransferQueueDefined ("GOHSTDEST1");

	}
	extFileH->detach();
}



