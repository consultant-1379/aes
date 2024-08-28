#pragma warning(disable:4786)

#include "apitest.h"
#include <osf\OS.h>
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
apitest::lockFileTest()
{
	string tmpStr("");
	string tmpDest("");
	string tmpUser("");
	int result(0);

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");
	inputCmd(tmpUser, "User");

	result = lockFile(tmpStr, tmpDest, tmpUser);
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

	result = unlockFile(tmpStr, tmpDest, tmpUser);
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

	result = isFileLocked(tmpStr, tmpDest, locked, tmpUser);

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
	string tmpStr("");
	string tmpDest("");
	string tmpDir("");
	string dummy("");
	int result(0);

	inputCmd(tmpStr, "Filename");
	inputCmd(tmpDir, "File path");
	inputCmd(tmpDest, "Destination");

	result = sendFileManually(tmpStr, tmpDir, tmpDest, 0, 10);
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

	for(itr=strVector.begin();itr!=strVector.end();++itr)
		{
			cout << "Reporting file: " << (*itr) << "  ";
			result = sendFile((*itr), AES_BINARY);
			if (result != 0)
				{
					cout << endl;
					cout << "   FAILED" << endl;
					cout << "Send file returned: " << result << ", " << getErrCodeText(result) << endl;
				}
			else
				{
					cout << "   OK"<<endl;
				}
		}
}

void
apitest::createTestFiles(string dir, int nr)
{

	HANDLE searchHandle;
	WIN32_FIND_DATA fileData;

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
			sprintf(tmpFile, "%s%s%d%s\0", tmpStr.c_str(), "\\test", i, ".txt");
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

	result = getDestinationList(destList);
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

	result = removeFileDestination(tmpStr);
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

	result = createSubfile(tmpStr, tmpDest, AES_FSREADY);
	cmdResultTest(result);
}

void
apitest::fileExistTest()
{
	int result(0);
	AES_AFP_Proctype procType;
	bool manualInit(false);
	string tmpStr("");
	string tmpDest("");

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpDest, "Destination");

	if (tmpDest == "EMPTY")
		{
			tmpDest = "";
		}

	result = fileExist(tmpStr, tmpDest, procType, manualInit);
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
apitest::setTransferStateTest()
{
	int result(0);
	string tmpStr("");
	string tmpDest("");
	string tmpSub("");

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpSub, "File");
	inputCmd(tmpDest, "Destination");

	tmpStr += "-";
	tmpStr += tmpSub;

	result = setTransferState (tmpStr, tmpDest, AES_FSDELETE);
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
	string tmpSub("");
	strstream outStream;

	inputCmd(tmpStr, "Transfer queue");
	inputCmd(tmpSub, "File");

	tmpStr += "-";
	tmpStr += tmpSub;
	result = listSubfile (tmpStr, outStream);

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
	result = listAllSubfiles(tmpStr, outStream);

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
	result = listDests(tmpStr, outStream);

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

	result = setAttr(tmpStr, tmpDest, (AES_GCC_Filestates)tmpState, tmpRemD, tmpRemB, tmpRetries, tmpRetTime);
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

	result = getAttr(tmpStr, tmpDest, outStream);

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

	result = getDirectoryPath(tmpStr, tmpDirectory);

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

	result = setDirectoryPath(tmpStr, tmpDirectory);
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

	result = createFileDestination(tmpStr,
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
//			case 20: return "Create test files";
			case 21: return "SEND MANY FILES";
			case 22: return "SEND FILE MANUALLY";
			case 23: return "LOCK A TRANSFER QUEUE";
			case 24: return "UNLOCK A TRANSFER QUEUE";
			case 25: return "CHECK IF TRANSFER QUEUE IS LOCKED";
			case 26: return "CLOSE API";
			case 27: return "HELP";
			case 28: return "LIST DESTINATIONS";
			case 29: return "CREATE TRANSFER QUEUE";
			default: return "NOT IMPLEMENTED";
		}
}

void apitest::buildCmdMap()
{
	char tmp[8];

	for (int idx=1;idx<=29;idx++)
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
	cout << endl << "APITEST RUNNING" << endl << endl;
	string tmpCmd("");
	string action("");

	if (option == "generic")
		{
			genericOpen = true;
			open("AES","APITEST");
		}
	else
		{
			genericOpen = false;
			open(option);
		}

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
//					case 20: createTestFiles(); break;
					case 21: sendManyFiles(); break;
					case 22: sendManualTest(); break;
					case 23: lockFileTest(); break;
					case 24: unlockFileTest(); break;
					case 25: isFileLockedTest(); break;
					case 26: closeTest(); return 0;
					case 27: printCmds(); break;
					case 28: listDestTest(); break;
					case 29: createFileDestTest(); break;
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
	cout << "Exit code: "<< res << "    Text: " << getErrCodeText(res) << endl;;	
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