#include <iostream>
#include <string>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>

#include "aes_cdh_Linftpiv2.h"

std::string destinationName("MYTESTDEST");
std::string overWrite("yes");

void sendAllFolderFiles(const std::string& folderPath, LinFTPIV2* ftpObject);

std::string remoteSubFolder;
std::string remoteFileName;

/* TEST PROGRAM OF CDH FTP CLASS
 *
 * This program takes two arguments:
 * 	1) Mandatory, it must be a local path of a folder
 *  2) Optional, it must be a remote folder
 *
 * All files inside the local folder will be sent to the remote ftp server two times:
 * 	1) With overwrite mode to false
 *  2) With overwrite mode to true
 *
 * after that the local folder with its contents (files and inner folder) will be sent to the remote ftp server.
 *
 * The address of ftp server, username, password and remote folder path are hard-coded into the base class constructor:
 * 			FTPIV2()
			: userName("ftpuser01"),
			  passWord("ftpuser01"),
			  remoteDir("MYTEST/"),
			  hostAddress("10.35.15.85")
			{

			};
 *
 */

int main( int argc , char* argv[])
{
	cout << "**********************************\n";
	cout << "\n\n\t\t CDH FTP TEST 1.0 \n\n";
	cout << "**********************************\n";
	std::string localFolder;
	std::string fileMask;

	if(argc >= 2)
	{
		localFolder.assign(argv[1]);
		if( argc == 2) remoteSubFolder.assign(argv[2]);
	}
	else
	{
		printf ("usage: %s localFolder [remoteFolderName]\n", argv[0]);
		return 1;
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

	LinFTPIV2* myftp = new LinFTPIV2();

	cout << "\n Check FTP connection\n";
	cout << "Result : " << myftp->ftpVerifyConnection() << endl;

	cout << "\n*********\n SEND with overwrite ON\n*********\n";

	sendAllFolderFiles(localFolder, myftp);

	cout << "\n*********\n RE-SEND with overwrite OFF\n*********\n";
	overWrite.assign("no");

	sendAllFolderFiles(localFolder, myftp);

	cout << "\n*********\n SEND FOLDER\n*********\n";
	std::cout << "\nTransferring the folder: "<< localFolder << " .... ";

	if( myftp->sendDirectoryInitiating(localFolder, remoteSubFolder, remoteFileName, AES_CDH_DestinationSet::TR_BINARY, fileMask) == AES_CDH_RC_OK)
	{
		cout << " Done!\n";
	}
	else
		cout << " Failed!\n";

	delete myftp;
	curl_global_cleanup();

	return 0;
}

void sendAllFolderFiles(const std::string& folderPath, LinFTPIV2* ftpObject)
{
	DIR* localFolderHandler = opendir(folderPath.c_str());

	if(NULL != localFolderHandler)
	{
		struct dirent* finddata;
		while (((finddata = readdir(localFolderHandler)) != NULL) )
		{
			// Skip the "." and ".." folders
			if (strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, "..") )
			{
				// Check if a folder or a file
				if(DT_DIR != finddata->d_type)
				{
					std::stringstream file;
					file <<folderPath<< finddata->d_name;

					cout << "\n Transfer:"<<  file.str() << "  ..... ";
					if (ftpObject->sendFileInitiating(file.str(), remoteSubFolder, remoteFileName, AES_CDH_DestinationSet::TR_BINARY) == AES_CDH_RC_OK)
					{
						cout << " Done!\n";
					}
					else
						cout << " Failed!\n";
				}
			}
		}
		// Close directory handle
		closedir(localFolderHandler);
	}
}
