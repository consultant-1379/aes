#include "cute_suite.h"
#include "aes_cdh_filedestset_handler.h"
#include "aes_cdh_destination_handler.h"
#include "aes_cdh_file_resp_dest_handler.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;
class CuteFileDestSet
{
public:

	static bool vCreate(std::string data);
	static void vDeleted(std::string data,std::string status);
	static void vModify(std::string data,std::string status);
	static void vAdminOperationCallback(std::string data,std::string status);
	static bool fetchData(std::string fileName);
	static void fileDestSetCmdexecute();
	static cute::suite make_suite_cute_cdh_filedestset();
	static vector<string> cmdStringList;
	static vector<string> dataList;
	static vector<string> cmdResultList;
	static AES_CDH_FileDestSetCmdHandler * fileDestSetHandlerPtr[2];
	static AES_CDH_DestCmdHandler * localDestHandlerPtr;
	static AES_CDH_FileRespDestCmdHandler * respondingdestHandlerPtr[3];
};
