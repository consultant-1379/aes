#pragma warning(disable:4786)

#include <osf\OS.h>

#include "apitest.h"
#include "AES_OHI_FileHandler.h"
#include "AES_GCC_Filestates.h"

#include <string>
#include <vector>
#include <fstream>

using namespace std;

apitest::apitest()
	:	commandNr(""),
		genericOpen(false)
{
	
}

apitest::~apitest()
{

}

void
apitest::sendDirectoryTest()
{
	if (genericOpen == true)
		{
			cout << "A specific transfer queue needs to be opened" << endl;
			return;
		}

	string tmpDirString("");
	string tmpDirname("");
	int result(0);
	int index(0);

	inputCmd(tmpDirString, "Which directory");

	cout << "Creating 10 test files in directory: " << tmpDirString << endl;
	createTestFiles(tmpDirString, 10);

	index = tmpDirString.find_last_of('\\');
	tmpDirname = tmpDirString.substr(index+1);

	result = sendFile(tmpDirname,	AES_BINARY, "*.*", true);

	cmdResultTest(result);
}

void
apitest::nonExistentStatusChange()
{
	string tmpTq("");
	string tmpDest("");
	string tmpFilesDir("");
	string tmpStr("");
	int numOfFiles(0);
	int newStatus(0); // FS_READY
	int result(0);
	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;
	vector<string> strVector;
	vector<string>::iterator itr;

	string tmpSearchOpt("*.*");
	string tmpPath("C:\\afpfiles");
	string tmpFindPath("");
	string tmpFileName("");
	string tmpFilePath("");

	inputCmd(tmpTq, "TQ");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpStr, "Change status for number of files");
	numOfFiles = atoi(tmpStr.c_str() );
	sendManyFiles();


	tmpFindPath = tmpPath + "\\" + tmpSearchOpt;

	searchHandle = FindFirstFile(tmpFindPath.c_str() , &fileData);
	if (searchHandle != INVALID_HANDLE_VALUE)
		{
		do
		  {
        if (fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
          {
						strVector.push_back(fileData.cFileName);
          }
      }while(FindNextFile(searchHandle, &fileData) != 0) ;

		}
	FindClose(searchHandle);

	// add dummy file
	strVector.push_back("nisse");


	for (itr=strVector.begin();itr!=strVector.end();itr++)
		{
			result = setTransferState((*itr), tmpDest, (AES_GCC_Filestates)newStatus);
		}
	cmdResultTest(result);
}

void
apitest::ohiAttach()
{
	string tmpStr("");
	string tmpTq("");
	string subsystem("");
	string block("");
	string tmpEventTxt("");
	string waitTime("");
	int timeDelay(0);
	int result(0);
	char answer[128];

	inputCmd(subsystem, "Subsystem");
	inputCmd(block, "block");
	inputCmd(tmpTq, "TQ");
	inputCmd(tmpStr, "User input for close");
	inputCmd(waitTime,"Delay detach");
	inputCmd(tmpEventTxt, "Event text");

	timeDelay = atoi(waitTime.c_str() );

  ohiApi gohFileHandler(subsystem, block, tmpTq, tmpEventTxt);

	result = gohFileHandler.attach();
	if (result != 0)
		{
			cmdResultTest(result);
			return;
		}

	cout << "Attach OK" << endl;
	if (tmpStr == "YES" || tmpStr == "yes" || tmpStr == "Y" || tmpStr == "y")
		{
			cout << "User input needed before closing TQ: ";
			cin.getline(answer, 32, '\n');
		}
	else
		{
			cout << "Wait for " << waitTime << " before detaching" << endl;
			Sleep(timeDelay * 1000);
		}

	result = gohFileHandler.detach();

	if (result != 0)
		{
			cmdResultTest(result);
			return;
		}

	cout << "Detach OK" << endl;
	cmdResultTest(result);
}

void
apitest::ohiAttachDetach()
{
	string tmpStr("");
	string tmpTq("");
	string subsystem("");
	string block("");
	string tmpMult("");
	int nrOfRuns(0);
	int result(0);
	char answer[32];

	inputCmd(subsystem, "Subsystem");
	inputCmd(block, "block");
	inputCmd(tmpTq, "TQ");
	inputCmd(tmpStr, "Number of attach/detach");
	nrOfRuns = atoi(tmpStr.c_str() );
	inputCmd(tmpStr, "User input for close");
	inputCmd(tmpMult, "Attach more than once");

  AES_OHI_FileHandler gohFileHandler(subsystem, block, tmpTq);

	for (int i=0;i < nrOfRuns;i++)
		{
			result = gohFileHandler.attach();
			if (result != 0)
				{
					cmdResultTest(result);
					break;
				}

			cout << "Attach OK" << endl;
			if (tmpMult == "YES" || tmpMult == "yes" || tmpMult == "Y"|| tmpMult == "y")
				{
					cout << "Attaching a second time to the same TQ" << endl;
					cout << "User input needed before attaching: ";
					cin.getline(answer, 32, '\n');
					result = gohFileHandler.attach();
					if (result != 0)
						{
							cmdResultTest(result);
							break;
						}
				}

			if (tmpStr == "YES" || tmpStr == "yes" || tmpStr == "Y" || tmpStr == "y")
				{
					cout << "User input needed before closing TQ: ";
					cin.getline(answer, 32, '\n');
				}

			result = gohFileHandler.detach();

			if (result != 0)
				{
					cmdResultTest(result);
					break;
				}
			cout << "Detach OK" << endl;
		}	
}

void
apitest::attachDetach()
{
	string tmpStr("");
	string tmpTq("");
	int nrOfRuns(0);
	int result(0);
	char answer[32];

	inputCmd(tmpTq, "TQ name");
	inputCmd(tmpStr, "Number of attach/detach");
	nrOfRuns = atoi(tmpStr.c_str() );
	inputCmd(tmpStr, "User input for close");


	for (int i=0;i < nrOfRuns;i++)
		{
			result = open(tmpTq);
			if (result != 0)
				{
					cmdResultTest(result);
					break;
				}

			if (tmpStr == "YES" || tmpStr == "yes" || tmpStr == "y")
				{
					cin.getline(answer, 32, '\n');
				}

			result = close();

			if (result != 0)
				{
					cmdResultTest(result);
					break;
				}
		}
}

void
apitest::lockFileTest()
{
	string tmpStr("");
	string tmpDest("");
	string tmpUser("");
	int result(0);

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpUser, "User");

	result = lockTransferQueue(tmpStr, tmpDest, tmpUser);
	cmdResultTest(result);
}

void
apitest::unlockFileTest()
{
	string tmpStr("");
	string tmpDest("");
	string tmpUser("");
	int result(0);

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpUser, "User");

	result = unlockTransferQueue(tmpStr, tmpDest, tmpUser);
	cmdResultTest(result);
}

void
apitest::isFileLockedTest()
{
	string tmpStr("");
	string tmpDest("");
	string tmpUser("");
	bool locked(false);
	int result(0);
	
	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpUser, "User");

	result = isTransferQueueLocked(tmpStr, tmpDest, locked, tmpUser);

	if (locked == true)
		{
			cout << "Transfer queue is locked by " << tmpUser<<endl;
		}
	else
		{
			cout << "Transfer queue is not locked" << endl;
		}
	cmdResultTest(result);
}

void
apitest::sendManualTest()
{
	string tmpFile("");
	string tmpDest("");
	string tmpDir("");
	string tmpTq("");
	string dummy("");
	int result(0);

	inputCmd(tmpFile, "Filename");
	inputCmd(tmpTq, "Transfer queue");

	inputCmd(tmpDir, "File path");
	inputCmd(tmpDest, "Destination");

	result = sendFileManually(tmpTq, "", tmpFile, tmpDir, tmpDest);
	cmdResultTest(result);
}

void
apitest::sendManyFiles()
{
	if (genericOpen == true)
		{
			cout << "A specific transfer queue needs to be opened" << endl;
			return;
		}

	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;

	int result(0);
	int tmpInt(0);
	string tmpStrInt("");
	string tmpStr("");
	string tmpSigns("\\*.*");
	vector<string> strVector;
	vector<string>::iterator itr;

	inputCmd(tmpStr, "Which directory contains your files");
	inputCmd(tmpStrInt, "Number of files");

	tmpInt = atoi(tmpStrInt.c_str() );

	cout << "Creating test files in directory: " << tmpStr << endl;

	createTestFiles(tmpStr, tmpInt);

	tmpStr += tmpSigns;

	searchHandle = FindFirstFile(tmpStr.c_str() , &fileData);
	if (searchHandle != INVALID_HANDLE_VALUE)
		{
		do
		  {
        if (fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
          {
						strVector.push_back(fileData.cFileName);
          }
      }while(FindNextFile(searchHandle, &fileData) != 0) ;

		}
	FindClose(searchHandle);

	cout << "Reporting files, please be patient... " << endl << endl;
	int idx(0);
	int charSwitch(0);
	int outNr(10);
	for(itr=strVector.begin();itr!=strVector.end();++itr)
		{
			result = sendFile((*itr), AES_BINARY);
			if (result != 0)
				{
					cout << endl;
					cout << "   FAILED" << endl;
					cout << "Send file returned: " << result << ", " << getErrorCodeText(result) << endl;
				}
			idx++;
			if (idx%5 == 0)
				{
					charSwitch++;
					switch (charSwitch)
						{
							case 1: cout << "          |     "; break;
							case 2: cout << "          /     ";break;
							case 3: cout << "          -     ";break;
							case 4: cout << "          \\     ";charSwitch = 0;break;
						}
					cout << outNr << " files reported" << "\r";
				}
			if (idx%10 == 0)
				{
					outNr += 10;
				}
		}
	cout <<"          " << idx << " files reported                           " << endl;
}

void
apitest::createTestFiles()
{
	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;

	string tmpStr("");
	string tmpRemFile("");
	int nr(0);

	inputCmd(tmpStr, "Directory for files? ");
	tmpRemFile = tmpStr;
	string nrOfFiles("");
	inputCmd(nrOfFiles, "How many files? ");
	nr = atoi(nrOfFiles.c_str() );

	char tmpFile[128];
	fstream fout;

	tmpRemFile += "\\*.*";
	searchHandle = FindFirstFile(tmpRemFile.c_str() , &fileData);
	if (searchHandle != INVALID_HANDLE_VALUE)
		{
		do
		  {
        if (fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
          {
						tmpRemFile = tmpStr;
						tmpRemFile +="\\";
						tmpRemFile += fileData.cFileName;
						::remove(tmpRemFile.c_str() );
          }
      }while(FindNextFile(searchHandle, &fileData) != 0) ;

		}
	FindClose(searchHandle);

	for (int i=0;i<nr;i++)
		{
			sprintf(tmpFile, "%s%s%02d%s\0", tmpStr.c_str(), "\\TEST", i, ".TXT");
			fout.open(tmpFile, ios_base::out | ios_base::trunc);
			fout<<"Hej "<<i;
			fout.close();
		}

}

void
apitest::createTestFiles(string dir, int nr)
{

	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;


	if (dir == "")
		{
			inputCmd(dir, "Directory for files? ");
			string nrOfFiles("");
			inputCmd(nrOfFiles, "How many files? ");
			nr = atoi(nrOfFiles.c_str() );
		}
	
	string tmpStr(dir);
	string tmpRemFile(dir);

	char tmpFile[128];
	fstream fout;

	tmpRemFile += "\\*.*";
	searchHandle = FindFirstFile(tmpRemFile.c_str() , &fileData);
	if (searchHandle != INVALID_HANDLE_VALUE)
		{
		do
		  {
        if (fileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY)
          {
						tmpRemFile = dir;
						tmpRemFile +="\\";
						tmpRemFile += fileData.cFileName;
						::remove(tmpRemFile.c_str() );
          }
      }while(FindNextFile(searchHandle, &fileData) != 0) ;

		}
	FindClose(searchHandle);

	for (int i=0;i<nr;i++)
		{
			sprintf(tmpFile, "%s%s%02d%s\0", tmpStr.c_str(), "\\test", i, ".txt");
			fout.open(tmpFile, ios_base::out | ios_base::trunc);
			fout<<"Hej "<<i;
			fout.close();
		}
}

void
apitest::closeTest()
{
	int result(0);
	string tmpStr("");
	result = close();
	cmdResultTest(result);
}

void
apitest::removeFileAllDestTest()
{
	int result(0);
	string tmpStr("");
	string tmpFile("");

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpFile, "Filename");

	result = removeFile(tmpStr, tmpFile);
	cmdResultTest(result);
}

void
apitest::removeFileExactTest()
{
	int result(0);
	string tmpStr("");
	string tmpDest("");
	string tmpFile("");

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpFile, "Filename");

	result = removeFile(tmpStr, tmpDest, tmpFile);
	cmdResultTest(result);
}

void
apitest::getFileStatusTest()
{
	int result(0);
	string tmpStr("");
	AES_GCC_Filestates status;

	inputCmd(tmpStr, "Filename");

	result = getFileStatus(tmpStr, status);
	cout << "Value: " << status; 
	switch (status)
		{
			case 0: cout << "  READY" << endl; break;
			case 1: cout << "  SEND" << endl; break;
			case 2: cout << "  ARCHIVE" << endl; break;
			case 3: cout << "  DELETE" << endl; break;
			case 4: cout << "  FAILED" << endl; break;
			case 5: cout << "  USE DEFAULT" << endl; break;
			case 6: cout << "  REMOVE" << endl; break;
			case 7: cout << "  STOPPED" << endl; break;
			case 8: cout << "  PENDING" << endl; break;
			case 9: cout << "  ON FTP AREA" << endl; break;
			default: cout << "NONE" << endl; break;
		}

	cmdResultTest(result);
}

void
apitest::getDestListTest()
{
	int result(0);
	list<string> destList;
	list<string>::iterator itr;

	result = getDestinationSetList(destList);
	for (itr=destList.begin();itr!= destList.end();++itr)
		{
			cout<<*itr<<" ";
		}
	cout << endl;

	cmdResultTest(result);
}

void
apitest::getLastRepTest()
{
	int result(0);
	string tmpStr("");
	string tmpOriginal("");
	string tmpGenerated("");

	inputCmd(tmpStr, "Transfer queue");

	result = lastReportedFile(tmpStr, tmpOriginal, tmpGenerated);
	cout << "Last reported original: " << tmpOriginal << " Last reported generated: " << tmpGenerated << endl;
	cmdResultTest(result);
}

void
apitest::removeFileDestinationTest()
{
	int result(0);
	string tmpStr("");

	inputCmd(tmpStr, "Transfer queue");

	result = removeTransferQueue(tmpStr);
	cmdResultTest(result);
}

void
apitest::createSubfileTest()
{
int result(0);
	string tmpStr("");
	string tmpSub("");
	string tmpDest("");

	inputCmd(tmpStr, "Transfer queue");
	tmpStr += "-";
	inputCmd(tmpSub, "Subfile");
	inputCmd(tmpDest, "Destination");
	
	if (tmpDest == "EMPTY")
		{
			tmpDest = "";
		}
	tmpStr += tmpSub;

//	result = createFile(tmpStr, tmpDest, AES_FSREADY);
//	cmdResultTest(result);
}

void
apitest::fileExistTest()
{
	int result(0);
	bool manualInit(false);
	string tmpStr("");
	string tmpDest("");

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");

	if (tmpDest == "EMPTY")
		{
			tmpDest = "";
		}

	result = transferQueueDefined(tmpStr, tmpDest, manualInit);
	cmdResultTest(result);
}

void
apitest::getTransferStateTest()
{
int result(0);
	string tmpStr("");
	string tmpSub("");
	string tmpDest("");
	AES_GCC_Filestates transState;

	inputCmd(tmpStr, "Transfer queue");
	tmpStr += "-";
	inputCmd(tmpSub, "File");
	tmpStr += tmpSub;
	inputCmd(tmpDest, "Destination");

	result = getTransferState (tmpStr, tmpDest, transState);

	cout << "Value: " << transState;
	switch (transState)
		{
			case 0: cout << "  READY" << endl; break;
			case 1: cout << "  SEND" << endl; break;
			case 2: cout << "  ARCHIVE" << endl; break;
			case 3: cout << "  DELETE" << endl; break;
			case 4: cout << "  FAILED" << endl; break;
			case 5: cout << "  USE DEFAULT" << endl; break;
			case 6: cout << "  REMOVE" << endl; break;
			case 7: cout << "  STOPPED" << endl; break;
			case 8: cout << "  PENDING" << endl; break;
			case 9: cout << "  ON FTP AREA" << endl; break;
			default: cout << "NONE" << endl; break;
		}

	cmdResultTest(result);
}

void
apitest::getTransferStateExTest()
{
	int result(0);
	string tmpStr("");
	string tmpSub("");
	string tmpDest("");
	string repDate("");
	string delDate("");
	string manStart("");
	string manStop("");
	string fDate("");
	string archDate("");
	string tmpFileOrder("");
	int reasonFail(0);
	int fileOrder(0);
	bool dir(false);

	AES_GCC_Filestates transState;

	inputCmd(tmpSub, "File");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpFileOrder, "Order of file (0=oldest, 1=newest, 2=all, 3=omitted, 4=next): ");
	fileOrder = atoi(tmpFileOrder.c_str() );

	result = getTransferStateEx(tmpSub,
															transState,
															tmpDest,
															fileOrder,
															repDate,
															delDate,
															manStart,
															manStop,
															fDate,
															archDate,
															reasonFail,
															dir);

	cout << "Transfer state: ";
	switch (transState)
		{
			case 0: cout << "  READY" << endl; break;
			case 1: cout << "  SEND" << endl; break;
			case 2: cout << "  ARCHIVE" << endl; break;
			case 3: cout << "  DELETE" << endl; break;
			case 4: cout << "  FAILED" << endl; break;
			case 5: cout << "  USE DEFAULT" << endl; break;
			case 6: cout << "  REMOVE" << endl; break;
			case 7: cout << "  STOPPED" << endl; break;
			case 8: cout << "  PENDING" << endl; break;
			case 9: cout << "  ON FTP AREA" << endl; break;
			default: cout << "NONE" << endl; break;
		}

	cout << "File name: " << tmpSub << endl;
	cout << "Destination: " << tmpDest << endl;
	cout << "List order: ";
	switch (fileOrder)
		{
			case 0: cout << "Oldest file" << endl; break;
			case 1: cout << "Newest file" << endl; break;
			case 3: cout << "Omitted" << endl; break;
			case 4: cout << "Next file" << endl; break;
			default: break;
		}

	cout << "Reported date: " << repDate << endl;
	cout << "Delete date: " << delDate << endl;
	cout << "Manual start date: " << manStart << endl;
	cout << "Manual stop date: " << manStop << endl;
	cout << "Failed date: " << fDate << endl;
	cout << "Archive date: " << archDate << endl;
	cout << "Reason for failed: " << reasonFail << endl;
	cout << "Is a directory: " << boolalpha << dir << noboolalpha << endl;
	cmdResultTest(result);
}

void
apitest::getOHITransferStateExTest()
{
  AES_OHI_Filestates transState;

	int result(0);
	string subSystem("");
	string blockName("");
	string TQ("");

	string tmpStr("");
	string tmpSub("");
	string tmpDest("");
	string repDate("");
	string delDate("");
	string manStart("");
	string manStop("");
	string fDate("");
	string archDate("");
	string tmpFileOrder("");
	int reasonFail(0);
	int fileOrder(0);
	bool dir(false);

	inputCmd(subSystem, "Sub system");
	inputCmd(blockName, "Block name");
	inputCmd(TQ, "Transfer queue");

	inputCmd(tmpSub, "File");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpFileOrder, "Order of file (0=oldest, 1=newest, 2=all, 3=omitted, 4=next): ");
	fileOrder = atoi(tmpFileOrder.c_str() );

  AES_OHI_FileHandler gohFileHandler(subSystem, blockName, TQ);
	result = gohFileHandler.attach();
	if (result == AES_OHI_NOERRORCODE)
		{
			result = gohFileHandler.getTransferStateEx(tmpSub,
																							 transState,
																							 tmpDest,
																							 fileOrder,
																								repDate,
																								delDate,
																								manStart,
																								manStop,
																								fDate,
																								archDate,
																								reasonFail,
																								dir);
			gohFileHandler.detach();
		}

	cout << "Transfer state: ";
	switch (transState)
		{
			case 0: cout << "  READY" << endl; break;
			case 1: cout << "  SEND" << endl; break;
			case 2: cout << "  ARCHIVE" << endl; break;
			case 3: cout << "  DELETE" << endl; break;
			case 4: cout << "  FAILED" << endl; break;
			case 5: cout << "  USE DEFAULT" << endl; break;
			case 6: cout << "  REMOVE" << endl; break;
			case 7: cout << "  STOPPED" << endl; break;
			case 8: cout << "  PENDING" << endl; break;
			case 9: cout << "  ON FTP AREA" << endl; break;
			default: cout << "NONE" << endl; break;
		}

	cout << "File name: " << tmpSub << endl;
	cout << "Destination: " << tmpDest << endl;
	cout << "List order: ";
	switch (fileOrder)
		{
			case 0: cout << "Oldest file" << endl; break;
			case 1: cout << "Newest file" << endl; break;
			case 3: cout << "Omitted" << endl; break;
			case 4: cout << "Next file" << endl; break;
			default: break;
		}

	cout << "Reported date: " << repDate << endl;
	cout << "Delete date: " << delDate << endl;
	cout << "Manual start date: " << manStart << endl;
	cout << "Manual stop date: " << manStop << endl;
	cout << "Failed date: " << fDate << endl;
	cout << "Archive date: " << archDate << endl;
	cout << "Reason for failed: " << reasonFail << endl;
	cout << "Is a directory: " << boolalpha << dir << noboolalpha << endl;

	cmdResultTest(result);
}

void
apitest::setTransferStateTest()
{
	int result(0);
	string tmpStr("");
	string tmpDest("");
	string tmpSub("");
	string tmpState("");
	int status(0);

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpSub, "File");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpState, "Transfer state");
	status = atoi(tmpState.c_str() );


	tmpStr = tmpSub;

	result = setTransferState (tmpStr, tmpDest, (AES_GCC_Filestates)status);
	cmdResultTest(result);
}

void
apitest::listFileTest()
{
	int result(0);
	string tmpStr("");
	string tmpDest("");
	strstream outStream;

	cout << endl;
	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");

	result = listFile(tmpStr, tmpDest, outStream);
	while (outStream.good() )
		{
			outStream >> tmpStr;
			cout << tmpStr << " ";
		}

	cout << endl;
	cmdResultTest(result);
}

void
apitest::listSubfileTest()
{
	int result(0);
	string tmpStr("");
	string tmpDestSet("");
	strstream outStream;

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDestSet, "Destinationset");

	result = listFile (tmpStr, tmpDestSet, outStream);

	while (outStream.good() )
		{
			outStream >> tmpStr;
			cout << tmpStr << " ";
		}

	cout << endl;
	cmdResultTest(result);
}

void
apitest::listAllSubfilesTest()
{
	int result(0);
	string tmpStr("");
	strstream outStream;

	inputCmd(tmpStr, "Transfer queue");
	result = listAllFiles(tmpStr, outStream);

	while (outStream.good() )
		{
			outStream >> tmpStr;
			cout << tmpStr << " ";
		}

	cout << endl;
	cmdResultTest(result);
}

void
apitest::listDestTest()
{
	int result(0);
	string tmpStr("");
	strstream outStream;

	inputCmd(tmpStr, "Transfer queue");
	result = listDestinationSets(tmpStr, outStream);

	while (outStream.good() )
		{
			outStream >> tmpStr;
			cout << tmpStr << " ";
		}

	cout << endl;
	cmdResultTest(result);
}

void
apitest::setAttrTest()
{
	int result(0);
	string tmpStr("");
	string tmpDest("");
	string tmpStateString("");
	string tmpRemString("");
	int tmpState(0);

	int tmpRemD(0);
	int tmpRemB(0);
	int tmpRetries(0);
	int tmpRetTime(0);
	
	cout<<endl;
	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");

	inputCmd(tmpStateString, "Default state? (READY = 0, SEND = 1, ARCHIVE = 2, DELETE = 3, FAILED = 4\nUSEDEFAULT = 5, REMOVE = 6, STOPPED = 7, PENDING = 8)");
	tmpState = atoi(tmpStateString.c_str() );

	inputCmd(tmpRemString, "Remove delay");
	tmpRemD = atoi(tmpRemString.c_str() );

	inputCmd(tmpRemString, "Remove before (No = 0, Yes = 1)");
	tmpRemB = atoi(tmpRemString.c_str() );
	inputCmd(tmpRemString, "Number of retries");
	tmpRetries = atoi(tmpRemString.c_str() );
	inputCmd(tmpRemString, "Retry time");
	tmpRetTime = atoi(tmpRemString.c_str() );

	result = setAttributes(tmpStr, tmpDest, (AES_GCC_Filestates)tmpState, tmpRemD, tmpRemB, tmpRetries, tmpRetTime);
	cmdResultTest(result);
}

void
apitest::getAttrTest()
{
	int result(0);
	string tmpStr("");
	string tmpDest("");
	strstream outStream;

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");

	result = getAttributes(tmpStr, tmpDest, outStream);

	while (outStream.good() )
		{
			outStream >> tmpStr;
			cout << tmpStr << " ";
		}
	cout << endl;
	cmdResultTest(result);
}

void
apitest::getDirTest()
{
int result(0);
	string tmpStr("");
	string tmpDirectory("");

	inputCmd(tmpStr, "Transfer queue");

	result = getSourceDirectoryPath(tmpStr, tmpDirectory);

	cout << "Directory: " << tmpDirectory << endl;
	cmdResultTest(result);
}

void
apitest::setDirTest()
{
int result(0);
	string tmpStr("");
	string tmpDirectory("");

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDirectory, "New directory");

	result = setSourceDirectoryPath(tmpStr, tmpDirectory);
	cmdResultTest(result);
}

void
apitest::createFileDestTest()
{
int result(0);
	string tmpStr("");
	string tmpDest("");
	string tmpDir("");
	string tmpFileTmp("");
	string tmpRen("");
	string slask("");
	AES_AFP_Renametypes ren(AES_NONE);

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpDir, "Directory");
	if (tmpDir == "EMPTY" || tmpDir.empty() )
		{
			tmpDir = "";
		}
	inputCmd(tmpFileTmp, "Template");
	inputCmd(tmpRen, "Rename file (local, remote)");
	if (tmpRen == "local")
		{
			ren = AES_LOCAL;
		}
	if (tmpRen == "remote")
		{
			ren = AES_REMOTE;
		}

	result = createTransferQueue(tmpStr,
																			tmpDest,
																			tmpDir,
																			AES_FSREADY,
																			10,
																			false,
																			10,
																			10,
																			ren,
																			tmpFileTmp);
	cmdResultTest(result);
}

void
apitest::sendFileTest()
{
	string tmpStr("");
	string tmpTrans("");
	string tmpSub("");
	string tmpMask("");
	string tmpDirFlag("");
	string tmpState("");
	bool dirFlag(false);
	AES_GCC_Format format;
	AES_GCC_Filestates status;
	int result(0);

	inputCmd(tmpSub, "File name");
	inputCmd(tmpTrans, "Transfer mode (ASCII, binary)");

	format = AES_BINARY;
	if (tmpTrans == "ASCII")
		{
			format = AES_ASCII;
		}

	inputCmd(tmpMask, "Mask");
	inputCmd(tmpDirFlag, "A directory (yes, no)");
	if (tmpDirFlag == "yes")
		{
			dirFlag = true;
		}

	inputCmd(tmpState, "File state (ready, failed)");

	if (tmpState == "ready")
		{
			status = AES_FSREADY;
		}
	if (tmpState == "failed")
		{
			status = AES_FSFAILED;
		}

	result = sendFile(tmpSub,	format, tmpMask, dirFlag, status);
	cmdResultTest(result);
}

string
apitest::getCmdName(int index)
{
	switch (index)
		{
			case 1: return "REMOVE FILE DESTINATION";
			case 2: return "CREATE SUBFILE";
			case 3: return "TRANSFER QUEUE EXIST";
			case 4: return "GET TRANSFER STATE";
			case 5: return "SET TRANSFER STATE";
			case 6: return "LIST FILE";
			case 7: return "LIST SUBFILE";
			case 8: return "LIST ALL SUBFILES";
			case 9: return "SET ATTRIBUTES";
			case 10: return "GET ATTRIBUTES";
			case 11: return "GET DIRECTORY PATH";
			case 12: return "SET DIRECTORY PATH";
			case 13: return "SEND FILE";
			case 14: return "GET LAST REPORTED FILE";
			case 15: return "GET DESTINATION LIST";
			case 16: return "GET FILE STATUS";
			case 17: return "REMOVE FILE FOR ALL DESTINATIONS";
			case 18: return "REMOVE EXACT FILE";
			case 19: return "END";
			case 20: return "CREATE TEST FILES";
			case 21: return "SEND MANY FILES";
			case 22: return "SEND FILE MANUALLY";
			case 23: return "LOCK A TRANSFER QUEUE";
			case 24: return "UNLOCK A TRANSFER QUEUE";
			case 25: return "CHECK IF TRANSFER QUEUE IS LOCKED";
			case 26: return "CLOSE API";
			case 27: return "HELP";
			case 28: return "LIST DESTINATIONS";
			case 29: return "CREATE TRANSFER QUEUE";
			case 30: return "GET TRANSFER STATE EX";
			case 31: return "GET OHI TRANSFER STATE EX";
			case 32: return "ATTACH AND DETACH";
			case 33: return "ATTACH AND DETACH THRU OHI";
			case 34: return "NON EXSISTENT STATUS CHANGE";
			case 35: return "OHI ATTACH";
			case 36: return "SEND DIRECTORY";
			default: return "NOT IMPLEMENTED";
		}
}

void apitest::buildCmdMap()
{
	char tmp[8];

	for (int idx=1;idx<=36;idx++)
		{
			if (idx < 10)
				{
					sprintf(tmp, "%d%d", 0, idx);
				}
			else
				{
					sprintf(tmp, "%d", idx);
				}
			cmdpair.first = tmp;
			cmdpair.second = getCmdName(idx);
			cmdmap.insert(cmdpair);
		}
}

void apitest::printCmds()
{
	map<string, string, less<string> >::iterator itr;

	cout << "Available commands:" << endl << endl;
	for (itr=cmdmap.begin();itr != cmdmap.end();itr++)
		{
			cout << "            " << (*itr).first << "   " << (*itr).second << endl;
		}
}

void apitest::printCmd(string& cmdnr)
{
	map<string, string, less<string> >::iterator itr;

	itr = cmdmap.find(cmdnr);
	if (itr != cmdmap.end() )
		{
			cout << endl << "Executing command: " << (*itr).second << endl;
		}
}

string apitest::getCmdString(string& cmd)
{
	map<string, string, less<string> >::iterator itr;

	itr = cmdmap.find(cmd);
	if (itr != cmdmap.end() )
		{
			return (*itr).second;
		}
	else
		{
			return "Command not found";
		}
}

string apitest::getCmdNr(string& cmd)
{
	map<string, string, less<string> >::iterator itr;

	int isChar = isalpha(cmd[0]);
	if (isChar == 0)
		{
			int tmpInt = atoi(cmd.c_str() );
			if (tmpInt < 9)
				{
					cmd.insert(0, "0");
				}
		}

	itr = cmdmap.find(cmd);
	if (itr != cmdmap.end() )
		{
			return (*itr).first;
		}

	for (itr=cmdmap.begin();itr != cmdmap.end();itr++)
		{
			if ( (*itr).second == cmd)
				{
					return (*itr).first;
				}
		}
	return "Command not found";
}

apitest::run(string option)
{
	string tmpCmd("");
	string action("");
	string tmpName("");

	if (option == "generic")
		{
			genericOpen = true;
			open("AES","APITEST");
		}
	else if (option == "OHI" || option == "NONE")
		{
			// Do nothing
		}
	else
		{
			genericOpen = false;
			open(option);
		}

	tmpName = "APITEST 2.0.0.1 - open " + option;
	SetConsoleTitle(tmpName.c_str() );

	buildCmdMap();
	printCmds();
	cout << endl << "Enter command name or number (Help for available commands, End for exit)? ";

	inputCmd(tmpCmd, "");
	cout << endl;

	while (1)
		{
			action = getCmdNr(tmpCmd);
			printCmd(action);

			commandNr = action.c_str();
			
			switch (atoi(commandNr.c_str() ) )
				{
					case 1: removeFileDestinationTest(); break;
					case 2: createSubfileTest(); break;
					case 3: fileExistTest(); break;
					case 4: getTransferStateTest(); break;
					case 5: setTransferStateTest(); break;
					case 6: listFileTest(); break;
					case 7: listSubfileTest(); break;
					case 8: listAllSubfilesTest(); break;
					case 9: setAttrTest(); break;
					case 10: getAttrTest(); break;
					case 11: getDirTest(); break;
					case 12: setDirTest(); break;
					case 13: sendFileTest(); break;
					case 14: getLastRepTest(); break;
					case 15: getDestListTest(); break;
					case 16: getFileStatusTest(); break;
					case 17: removeFileAllDestTest(); break;
					case 18: removeFileExactTest(); break;
					case 19: return 0;
					case 20: createTestFiles(); break;
					case 21: sendManyFiles(); break;
					case 22: sendManualTest(); break;
					case 23: lockFileTest(); break;
					case 24: unlockFileTest(); break;
					case 25: isFileLockedTest(); break;
					case 26: closeTest(); return 0;
					case 27: printCmds(); break;
					case 28: listDestTest(); break;
					case 29: createFileDestTest(); break;
					case 30: getTransferStateExTest(); break;
					case 31: getOHITransferStateExTest(); break;
					case 32: attachDetach(); break;
					case 33: ohiAttachDetach(); break;
					case 34: nonExistentStatusChange(); break;
					case 35: ohiAttach(); break;
					case 36: sendDirectoryTest(); break;
					default: cout << "          ERROR" << endl; break;
				}

			cout << endl;
			cout << "Enter command or number (End for exit)? ";
			inputCmd(tmpCmd, "");
		}
	close();
	return 0;
}

void
apitest::cmdResultTest(int res)
{
	cout << "Result from: " << getCmdString(commandNr) << "  ";	
	cout << "Exit code: "<< res << "    Text: " << getErrorCodeText(res) << endl;;	
}

void
apitest::inputCmd(string& newCmd, string question)
{
	char command[32];

	if (!question.empty() )
		{
			cout << question <<"? ";
		}
	cin.getline(command, 32, '\n');

	if (question == "Template")
		{
			newCmd = command;
		}
	else
		{
			newCmd = toUpper(command);
		}
}
unsigned int
apitest::event(AES_GCC_Eventcodes& event)
{
	cout << "Event received: " << event << endl;
	AES_GCC_Eventcodes newEvent;
	string evText("");
	getEvent(newEvent, evText);
	cout << endl << "Nev event: " << newEvent << " Text: " << evText << endl << endl;
	return 0;
}

char*
apitest::toUpper (char* str)
{
  int i = 0;

  while (str[i]) 
    {
      str[i] = toupper(str[i]);
      i++;
    }

  return str;
}

unsigned int
ohiApi::handleEvent(AES_OHI_Eventcodes eventCode)
{
	cout << "Handle event called, event is " << eventCode << endl;

	return 0;
}

ohiApi::ohiApi(string subsystem, string block, string tmpTq, string evTxt)
:AES_OHI_FileHandler(subsystem, block,tmpTq, evTxt)
{

}
