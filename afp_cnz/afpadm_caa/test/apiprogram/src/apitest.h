#ifndef APITEST_H
#define APITEST_H

#include "AES_AFP_Api.h"
#include "AES_OHI_Filehandler.h"
#include "AES_GCC_Eventcodes.h"
#include "AES_OHI_Eventcodes.h"
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
		void sendDirectoryTest();
		void createSubfileTest();
		void fileExistTest();
		void getTransferStateTest();
		void getTransferStateExTest();
		void getOHITransferStateExTest();
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
		void createTestFiles();
		void createTestFiles(string dir, int nr);
		void sendManyFiles();
		void sendManualTest();
		void lockFileTest();
		void unlockFileTest();
		void isFileLockedTest();
		string getCmdName(int value);
		void attachDetach();
		void ohiAttachDetach();
		void ohiAttach();
		void nonExistentStatusChange();




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


class ohiApi : public AES_OHI_FileHandler
{
	public:
	ohiApi(string subsystem, string block, string tmpTq, string evTxt);
	unsigned int handleEvent(AES_OHI_Eventcodes eventCode);	

};

#endif