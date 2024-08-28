//*******************************************************************
//
// NAME
//      DBOrm.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// DOCUMENT NO
//      19089-CAA 109 1299

// AUTHOR 
//      2002-06-19 UAB/UKB/AU qabulfg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//  PA1     020619  qabulfg First issue.

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//*******************************************************************

#include <iostream>
#include <iomanip>
#include <ACS_PRC_Process.H>
#include <ACS_ExceptionHandler.H>
#include <AES_GCC_Errorcodes.h>
#include <AES_GCC_GetOpt.h>
#include <AES_GCC_User.h>
#include "AES_DBO_TQManager.h"
#include "DBOrm.h"
#include "command.h"

using namespace std;

//*******************************************************************
//
int DBOrm::exec(int argc, char **argv)
{
    AP_InitProcess("AES_DBO_dborm", AP_COMMAND);
    
#ifndef _DEBUG
    // To provide handling of unhandled exceptions
    AP_SetCleanupAndCrashRoutine("AES_DBO_dborm", NULL);
#endif
    
    int t_flag = 0;
    int d_flag = 0;
    
    string transferqueue;
    string destination;
    
    string myTransferqueue;
    string dest;
    
    int opt;
    AES_GCC_GetOpt getopt(argc,argv, "");
    while ((opt = getopt()) != -1)
    {
        switch(opt)
        {
        case '?':
            printUsage();
            return 2;
            break;
        }
    }
    
    if(argc != 2)
    {
        printUsage();
        return 2;
    }
    
    transferqueue = argv[1];
    t_flag = 1;
    
    // Check DBO transfer queue name
    if (t_flag == 1)
    {
		if (!validTQName(transferqueue))
        {
			cerr << "Unreasonable value: " << transferqueue.c_str() << endl;
            return 3;
        }
		toUpper(transferqueue);
		myTransferqueue = transferqueue;
    }
    else
    {
        printUsage();
        return 2;
    }
    
    AES_GCC_User user;
    unsigned int rCode;
    rCode = AES_DBO_TQManager::instance()->
        removeTQ(user.getName(), myTransferqueue);
    if(rCode != AES_NOERRORCODE)
    {
        string error = AES_DBO_TQManager::instance()->getErrorText(rCode);
        cerr << error << endl;
        if(rCode == AES_NOSERVERACCESS)
           return 117;
    }
    return rCode;
}


//*******************************************************************
//
void DBOrm::printUsage()
{
   cerr << "Incorrect usage" << endl;
   cerr << "Usage:" << endl;
   cerr << "dborm transferqueue" << endl;
}


//*******************************************************************
//
int main(int argc, char* argv[])
{
    DBOrm dborm;
    return dborm.exec(argc, argv);
}
