#include "aes_gcc_variable.h"
#include <iostream>
#include <string.h>
int main()
{
	std::cout<<"Checking whether aesapdata path exists or not"<<std::endl;
	AES_GCC_Variable aesData("aesapdata");
	bool result = aesData.exists();
	std::cout<<"Status of exists() method :"<<result<<std::endl;
	if(result)
	{
		std::cout<<"AES Data path exists"<<std::endl;
	}
	std::cout<<"Fetching the data path for aesapdata"<<std::endl;
	std::string path = aesData.getStr();
	std::cout<<"aes data path obtained as :"<<path<<std::endl;

	std::cout<<"Checking whether aesaplogs path exists or not"<<std::endl;
	AES_GCC_Variable aesLogs("aesaplogs");
	result = aesLogs.exists();
	if(result)
	{
		std::cout<<"aes logs path exists"<<std::endl;
	}
	std::cout<<"Fetching the aesaplogs path "<<std::endl;
	path = aesLogs.getStr();
	std::cout<<"aes logs path obtained as : "<<path<<std::endl;
}
