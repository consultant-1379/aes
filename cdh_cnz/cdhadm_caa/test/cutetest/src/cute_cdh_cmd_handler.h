#include "cute_suite.h"
#include <ace/ACE.h>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;
class Cute_Command_Handler
{
public:
	static cute::suite make_suite_cute_command_handler();
	static bool createCommandPairLists(string fileName);
	static bool immExecuteCommand(string cmd);
	static bool executeTest(string fileName);
	static void FileDestSetTestCases();
	static void respDestTestCases();
	static void fileRemoteTestCases();
	static void getMacros(string fileName);
	static void replaceMacros(string fileName);
	
	static bool executeTest(string fileName);
	static void fileRemoteTestCases();
	static void localDestTestCases();
	static void FileDestSetTestCases();
	static list<string> commandStringList;
	static list<string> commandResultList;

};

