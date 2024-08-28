#ifndef APITEST_H
#define APITEST_H

#include "AES_AFP_Api.h"
#include "AES_GCC_Eventcodes.h"
#include <string>
#include <map>

using namespace std;

class apitest : public AES_AFP_Api
{
	public:

		apitest();
		virtual ~apitest();
		int run(string option);
		unsigned int event (AES_GCC_Eventcodes& eventCode);
		void removeFileDestinationTest();
		void createSubfileTest();
		void fileExistTest();
		void getTransferStateTest();
		void setTransferStateTest();
		void listFileTest();
		void listSubfileTest();
		void listAllSubfilesTest();
		void listDestTest();
		void removeSubfileTest();
		void setAttrTest();
		void getAttrTest();
		void getDirTest();
		void setDirTest();
		void createFileDestTest();
		void cmdResultTest(int res);
		void sendFileTest();
		void getLastRepTest();
		void getDestListTest();
		void getFileStatusTest();
		void removeFileExactTest();
		void removeFileAllDestTest();
		void closeTest();
		void createTestFiles(string dir, int nr);
		void sendManyFiles();
		void sendManualTest();
		void lockFileTest();
		void unlockFileTest();
		void isFileLockedTest();
		string getCmdName(int value);

	private:
		void buildCmdMap();
		void printCmds();
		void printCmd(string& cmdnr);
		string getCmdNr(string& cmd);
		string getCmdString(string& cmd);
		void inputCmd(string& newCmd, string question);
		map<string, string, less<string> > cmdmap;
		pair<string, string> cmdpair;
		string commandNr;
		bool genericOpen;
		char* toUpper(char* str);

};

#endif