//	COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson Utvecklings AB, Sweden.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson Utvecklings AB or
//	in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	DESCRIPTION:
//
//	ERROR HANDLING
//
//	      General rule:
//	      The error handling is specified for each method.
//
//	      No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	      190 89-CAA 109 xxxx
//
//	AUTHOR
//	      1999-04-	UAB/I/LN           UABxxxx
//
//	 REVISION
//	       PA1  2001-06-13  UABxxxx  Prel version.
//         PA2  2001-08-08  QABLAKE EM fix pattern for 4096 1..FF,0..FF -> 1..FF,1..FF
//                            Turn around after 9999 ->0000
//         RxA  2001-       QABMSOL
//
//	LINKAGE
//
//	SEE ALSO

#include <string>
#include <iostream>
#include <ace/ACE.h>
#include "ace/Basic_Types.h"
#include <ace/Thread_Manager.h>
#include <stdlib.h>
#include <stdio.h>
#include <fnmatch.h>

#include "test.h"
#include "threadgen.h"

#define INFINITE 0xFFFFFFFF

using namespace std;

void start(void *dummy);

// Add from qmicsal 21/02/2007
struct structInfoRecFile 
{
	ACE_TCHAR strCPName [8];				// CP Name
	ACE_TCHAR strRescueFilePath[60];		// Rescue file path
};

bool checkGohstState();
bool GetInfoRecFilePath(ACE_INT32 * nNumCp, struct structInfoRecFile ArrayInfoRecFile []);
static ACE_TCHAR * s_pCP_Name = NULL;	// CP Name

ACE_Semaphore* hSemaphore;

ACE_INT32 main(ACE_INT32 argc, ACE_TCHAR * argv [])
{
	if ((argc < 3) || (strcasecmp(argv[1], "-cp") != 0) || (argv[2][0] == '-')) 
	{
		cout << "____________________________________________________________________\n\n"; 
		cout << " No CP Name is passed in the line command. \n" << endl;

		s_pCP_Name = (char *)"";

		if (!checkGohstState())
			exit (0);
	}
	else
	{
		s_pCP_Name = argv[2];
	}

	cout << "____________________________________________________________________\n\n"; 
	cout << " Product name:\t" << PRODUCTNAME << endl;
	cout << " Product nr:\t" << PRODUCTNUMBER << endl;
	cout << " Version:\t" << GOHSTVERSION << endl;
	cout << " Updated:\t" << UPDATED << endl;
	cout << " CP Name:\t" << s_pCP_Name << endl;
	cout << "____________________________________________________________________\n\n\n";

        ACE_Thread_Manager* ace_thr_mgr = ACE_Thread_Manager::instance();
        ACE_thread_t ID_; 
        ace_thr_mgr->spawn((ACE_THR_FUNC)start,
                        NULL ,
                        THR_NEW_LWP | THR_DETACHED,
                        &ID_,
                        0,
                        ACE_DEFAULT_THREAD_PRIORITY,
                        -1,
                        0,
                        ACE_DEFAULT_THREAD_STACKSIZE);
	
        ace_thr_mgr->wait();
	exit(1);

	return 0;
}


void start(__attribute__((unused))void *dummmy)
{
	bool run = true;

	Test *t = new Test(s_pCP_Name);
	
	while(run)
	{
		if(run)
			cout << "GOHST> ";
		
		run = t->getCommand();
	}
	
	if (t)
	{
		delete t;
		t = 0;
	}

}

//////////////////////////////////////////////////////////////////
// Check the state of GOHST Destinations						//
//////////////////////////////////////////////////////////////////
bool checkGohstState()
{
	struct structInfoRecFile ArrayInfoFile[100] ;
	ACE_INT32 nNumCP = 0;
	
        GetInfoRecFilePath(&nNumCP, ArrayInfoFile);
	
	cout << "____________________________________________________________________\n\n"; 
	cout << " Rescue files already created:\n\n" << endl;
	cout << " CP Name\t\tGOHST Rescue Path\n" << endl;

	for (ACE_INT32 i = 0; i < nNumCP; i++)
	{
		cout << " " << ArrayInfoFile[i].strCPName << " \t\t" << ArrayInfoFile[i].strRescueFilePath << endl;
	}
		
	cout << "\n____________________________________________________________________\n\n";
	ACE_TCHAR command_[12];
	cout << " Please, write the CP Name and click on the return key to insert the" << endl;
	cout << " desired CP. If you don't want insert the CP, click on the return " << endl;
	cout << " key to continue with the default empty CP: --> ";

	ACE_OS::memset (command_, 0, 12);
	cin.getline(command_, 10);
	cout << "\n____________________________________________________________________\n\n";

	if (ACE_OS::strcmp(command_, "") == 0)
	{
		s_pCP_Name = (char *)"";
	}
	else
	{
		s_pCP_Name = command_;
	}
	return true;
}

//////////////////////////////////////////////////////////////////
// Return the path info of Resue files							//
//////////////////////////////////////////////////////////////////
bool GetInfoRecFilePath(ACE_INT32 * nNumCp, struct structInfoRecFile ArrayInfoRecFile [])
{
	ACE_TCHAR szCurDir[100] = { 0 }; 
	struct dirent* dit = 0;
	DIR* dip  = 0;
	string tempPath("");
	ACE_INT32 i = 0;

	ACE_OS::strcpy(szCurDir,"/opt/ap/aes/bin/");
        ACE_OS::strcat(szCurDir, "gohst_rescue");
	
	string currentPath = szCurDir; 
	string currentFile;
		
         dip = opendir(currentPath.c_str());
         if( dip == NULL )
         {
                perror("Dir not opened");
                return false;
         }

        while((dit = readdir(dip)) != NULL )
	{
                if (fnmatch("*.rec", dit->d_name, FNM_CASEFOLD) != 0)
                     continue;
	
		tempPath = currentPath + "/" + string(dit->d_name);
		currentFile = string(dit->d_name);

		ACE_INT32 npos = currentFile.find(".rec");

		if (npos >= 0)
				currentFile = currentFile.substr(0, npos);
			else
				currentFile = "";

			npos = currentFile.find("gohst_");

			if (npos >= 0)
				currentFile = currentFile.substr(6, currentFile.length() - 6);
			else
				currentFile = "______";

			strcpy (ArrayInfoRecFile[i].strCPName, currentFile.c_str());
			strcpy (ArrayInfoRecFile[i].strRescueFilePath, tempPath.c_str());
		
			i++;

	}  // while fileDirExist

	closedir(dip);
	*nNumCp = i;

	return true;
}
// End Add from qmicsal
