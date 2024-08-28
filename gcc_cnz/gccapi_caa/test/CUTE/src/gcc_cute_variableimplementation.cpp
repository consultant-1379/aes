#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "gcc_cute_variableimplementation.h"
#include <iostream>
using namespace std;

// static AES_GCC_Variable GCC_Cute_Varaible::pVarObj = 0;
void GCC_Cute_Variable::vPathExists()
{
	AES_GCC_Variable * pVarObj = new AES_GCC_Variable("aesapdata");
	bool status = pVarObj->exists();
	cout<<"Data path result "<<status<<endl;
	ASSERTM("data path not exist ", status==true);
	delete pVarObj;

}

void GCC_Cute_Variable::vGetPathStr()
{
	AES_GCC_Variable * pVarObj = new AES_GCC_Variable("aesaplogs");
	std::string result = pVarObj->getStr();
	cout<<"Data path result "<<result.c_str()<<endl;
	ASSERTM("Failed to obtain the data path", (result.compare("/data/aes/logs"))== 0);
	delete pVarObj;
}

cute::suite GCC_Cute_Variable::make_suite_gcc_variableimplementation()
{

	cute::suite s;
	s.push_back(CUTE(vPathExists));
	s.push_back(CUTE(vGetPathStr));
	return s;
}



