/*===================================================================*/
/**
	@file aes_gcc_util.cpp

	Class method implementation for aes_gcc_util.h

	@version 1.1.1

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       26/09/2011    XCHEMAD      Initial Release
 */
/* =================================================================== */
/*===================================================================
				INCLUDE DECLARATION SECTION
=================================================================== */
#include "aes_gcc_util.h"
#include <ace/OS_NS_fcntl.h>
#include <string>
#include <iostream>
#include "aes_gcc_log.h"
using namespace std;
std::string AES_GCC_Util::dnOfFileTransferM="";
std::string AES_GCC_Util::dnOfBlockTransferM="";
GCC_TDEF(aes_gcc_util);
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Util::AES_GCC_Util()
{
}
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_Util::~AES_GCC_Util()
{
}
/*===================================================================
                    ROUTINE: copyFile
=================================================================== */
ACE_INT32 AES_GCC_Util::copyFile (string sourceFile, string destinationFile)
{
	char  buffer[BUFFER_SIZE]= { 0 };  /* buffer for data */
	ACE_HANDLE   in_file = ACE_INVALID_HANDLE;              /* input file descriptor */
	ACE_HANDLE   out_file = ACE_INVALID_HANDLE;             /* output file descriptor */
	ssize_t   read_size = 0;            /* number of bytes on last read */
	struct stat stat_buf;       /* hold information about input file */
	int result = 0;                 /* holds return value */
	mode_t temp_mask=0000;

	in_file = ACE_OS::open(sourceFile.c_str(), O_RDONLY|O_BINARY);
	if (in_file  == ACE_INVALID_HANDLE)
	{
		return -1;
	}
	result = ACE_OS::fstat(in_file, &stat_buf);
	mode_t mask = ACE_OS::umask(temp_mask);
	if ( result < 0 )
	{
		out_file = ACE_OS::open(destinationFile.c_str(), O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0766);
	}
	else
	{
		//out_file = ACE_OS::open(destinationFile.c_str(), O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, stat_buf.st_mode);
		out_file = ACE_OS::open(destinationFile.c_str(), O_WRONLY|O_TRUNC|O_CREAT|O_BINARY, 0766);
	}
	ACE_OS::umask(mask);
	if (out_file < 0)
	{
		close(in_file);
		return -1;
	}
	while (1)
	{
		memset( buffer, 0, sizeof(buffer));
		read_size = ACE_OS::read(in_file, buffer, sizeof(char)*BUFFER_SIZE);


		if (read_size == 0)
			break;              /* end of file */

		if (read_size < 0)
		{
			ACE_OS::close(in_file);
			ACE_OS::close(out_file);
			return -1;
		}
		ssize_t noOfBytesWriiten = ACE_OS::write(out_file, buffer, read_size);
		if( noOfBytesWriiten < 0 )
		{
			ACE_OS::close(in_file);
			ACE_OS::close(out_file);
			return -1;
		}

	}
	ACE_OS::close(in_file);
	ACE_OS::close(out_file);
	return 0;
}

ACE_INT32 AES_GCC_Util::fetchDnOfFileTransferObjFromIMM (OmHandler& omHandler)
{
	static int infoAlreadyLoad = 0;
	static char dnOfRootClass[512] = {0};

	ACE_Recursive_Thread_Mutex loadingSyncObject;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(loadingSyncObject, true);

	if (infoAlreadyLoad)
	{
		dnOfFileTransferM = std::string(dnOfRootClass);
		return 0;
	}

	if (!guard.locked())
	{
		return -1;
	}

	std::vector<std::string> dnList;
	int returnCode = 0;

	if (omHandler.getClassInstances(AES_GCC_FILETRANSFERM_CLASSNAME, dnList) != ACS_CC_SUCCESS)
	{
		//ERROR
		returnCode = -1;
	}
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() ^ 1)
		{
			//WARNING: 0 (zero) or more than one node found
			returnCode = -1;
		}
		else
		{
			//OK: Only one root node
			strncpy(dnOfRootClass, dnList[0].c_str(), AES_ARRAY_SIZE(dnOfRootClass));
			dnOfRootClass[AES_ARRAY_SIZE(dnOfRootClass) - 1] = 0;
			infoAlreadyLoad = 1;
			dnOfFileTransferM = std::string(dnOfRootClass);
		}
	}

	return returnCode;
}

ACE_INT32 AES_GCC_Util::fetchDnOfBlockTransferObjFromIMM (OmHandler& omHandler)
{
	static int infoAlreadyLoad = 0;
	static char dnOfRootClass[512] = {0};

	ACE_Recursive_Thread_Mutex loadingSyncObject;

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(loadingSyncObject, true);

	if (infoAlreadyLoad)
	{
		dnOfBlockTransferM = std::string(dnOfRootClass);
		return 0;
	}

	if (!guard.locked())
	{
		return -1;
	}

	std::vector<std::string> dnList;
	int returnCode = 0;

	if (omHandler.getClassInstances(AES_GCC_BLOCKTRANSFERM_CLASSNAME, dnList) != ACS_CC_SUCCESS)
	{
		//ERROR
		returnCode = -1;
	}
	else
	{
		//OK: Checking how much dn items was found in IMM
		if (dnList.size() ^ 1)
		{
			//WARNING: 0 (zero) or more than one node found
			returnCode = -1;
		}
		else
		{
			//OK: Only one root node
			strncpy(dnOfRootClass, dnList[0].c_str(), AES_ARRAY_SIZE(dnOfRootClass));
			dnOfRootClass[AES_ARRAY_SIZE(dnOfRootClass) - 1] = 0;
			infoAlreadyLoad = 1;
			dnOfBlockTransferM = std::string(dnOfRootClass);
		}
	}

	return returnCode;
}



/*===================================================================
                    ROUTINE: datapath_trn
=================================================================== */
bool AES_GCC_Util::datapath_trn(std::string inVar1,std::string inVar2, std::string& outVar)
{
	if(inVar1.length()>0)
	{
		size_t pos = inVar1.find(inVar2);
		if((pos!=string::npos))
		{
			outVar = inVar1.substr(pos,strlen(inVar1.c_str()));
			return true;
		}
		else
		{
			outVar=inVar1;
			return false;
		}
	}
	outVar=inVar1;
	return false;
}


bool AES_GCC_Util::getPersistentDir(string& path)
{

     FILE *fp = 0;
     string myfile="/usr/share/pso/storage-paths/config";     
     //Read the address from /usr/share/pso/storage/config using popen.

     fp = fopen(myfile.c_str(),"r");
     if ( fp == NULL)
	 {
		 return false;
	 }
     char Buf[200] = { 0};
     fread(&Buf, 1 , 170, fp);
     if (ferror(fp) != 0)
	 {
		 fclose(fp);
		 return false;
	 }
	 char *ptr = strrchr(Buf, '\n');
	 *ptr = '\0';
     path = Buf;

	 fclose(fp);
     return true;
}

void AES_GCC_Util::extractComDnfromDn(const char* inDN, std::string& outRDN)
{
	size_t index = 0;

        std::string str(inDN);
        size_t idx = str.find("fileTransferManagerId=1");
        if(idx !=string::npos)
        {
                str = str.substr(0,idx);
        }
        while((index = str.find(",")) != string::npos)
        {
                std::string temp = "";
                temp = str.substr(0,index);
                str = str.substr(index+1,str.length());
                outRDN = temp+","+outRDN;
        }
        outRDN = outRDN.substr(0,outRDN.length()-1);
}

