//*******************************************************************
//
// NAME
//      DBOch.cpp
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

// .DESCRIPTION 
//      <General description of the class>

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
#include "DBOch.h"
#include "command.h"

//*******************************************************************
//
int DBOch::exec(int argc, char **argv)
{
    AP_InitProcess("AES_DBO_dboch", AP_COMMAND);
    
#ifndef _DEBUG
    // To provide handling of unhandled exceptions
    AP_SetCleanupAndCrashRoutine("AES_DBO_dboch", NULL);
#endif
  
    int t_flag = 0;
    int d_flag = 0;

    int m_flag = 0;
    int r_flag = 0;

    int length = 0;
	int r_value = 1440; // default value
    
    string transferqueue;
    string destination;
	string m_value;
	string temp_r_value;
    unsigned int mirroring = 0;
    
    int opt;
	AES_GCC_GetOpt getopt(argc, argv, "m:r:");
    while ((opt = getopt()) != -1)
    {
        switch(opt)
        {
        case 'm':
            m_flag++;
            m_value = getopt.optarg;
            break;
		case 'r':
            r_flag++;
            temp_r_value = getopt.optarg;
            break;
        case '?':
            printUsage();
            return 2;
            break;
        }
    }
    
	// parse transfer queue name
    if (argc > getopt.optind)
    { 
		transferqueue = argv[getopt.optind];
		t_flag++;
		getopt.optind++;
	}
	else
	{
        printUsage();
        return 2;
	}

	// parse destination name
    if (argc > getopt.optind)
    { 
		destination = argv[getopt.optind];
		d_flag++;
		getopt.optind++;
	}

	// No more names 
	if (argc != getopt.optind)
    {
        printUsage();
        return 2;
    }
      
    if ((t_flag != 1) || (d_flag > 1) || (m_flag > 1) || (r_flag > 1))
    {
        printUsage();
        return 2;
    }
    
    // Check DBO transfer queue name
    if (t_flag == 1)
    {
		if (!validTQName(transferqueue))
        {
			cerr << "Unreasonable value: " << transferqueue.c_str() << endl;
            return 3;
        }
		toUpper(transferqueue);
    }
    else
    {
        printUsage();
        return 2;
    }
    
    // Check destination
    if (d_flag == 1)
    {
		if (!validDestName(destination))
        {
			cerr << "Unreasonable value: " << destination.c_str() << endl;
            return 3;
        }
		toUpper(destination);
    }
    
    // Check mirror flag
    if (m_flag == 1)
    {
        toUpper(m_value);
        if (m_value.length() > 3)
        {
            cerr << "Unreasonable value: " << m_value << endl;
            return 3;
        }

        // Mirroring
        if (m_value == "YES")
            mirroring = 1;

        // Not mirroring
        else if(m_value == "NO")
            mirroring = 0;

        // Unreasonable value
        else
        {
            cerr << "Unreasonable value: " << m_value << endl;
            return 3;
        }
    }

	// check combination mirroring remove_delay
    if ((r_flag == 1 ) && (!mirroring))
    {
        printUsage();
        return 2;
    }

    // Check remove delay value
    if (r_flag == 1)
	{

        if (temp_r_value.find_first_not_of("0123456789") != string::npos)
        {
			cerr << "Unreasonable value: " << temp_r_value.c_str() << endl;
            return 3;
        }

		r_value = atoi(temp_r_value.c_str());
		{
			if ((r_value > 10080) || (r_value < 1))
			{
				cerr << "Unreasonable value: " << r_value << endl;
				return 3;
			}
		}
	}

    AES_GCC_User user;
	unsigned int rCode = AES_NOERRORCODE;
	if (d_flag && !m_flag && !r_flag)
		rCode = AES_DBO_TQManager::instance()->
        changeTQDest(user.getName(), transferqueue, destination);
	else if (!d_flag && m_flag && !r_flag)
		rCode = AES_DBO_TQManager::instance()->
        changeTQMirror(user.getName(), transferqueue, mirroring);
	else if (!d_flag && m_flag && r_flag)
		rCode = AES_DBO_TQManager::instance()->
        changeTQMirror(user.getName(), transferqueue, mirroring, r_value);
	else if (d_flag && m_flag && !r_flag)
		rCode = AES_DBO_TQManager::instance()->
        changeTQ(user.getName(), transferqueue, destination, mirroring);
	else if (d_flag && m_flag && r_flag)
		rCode = AES_DBO_TQManager::instance()->
        changeTQ(user.getName(), transferqueue, destination, mirroring, r_value);
    
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
void DBOch::printUsage()
{
   cerr << "Incorrect usage" << endl;
   cerr << "Usage:" << endl;
   cerr << "dboch -m yes [-r remove_delay] transferqueue [destinationset]" << endl;
   cerr << "dboch [-m no] transferqueue [destinationset]" << endl;
}

//*******************************************************************
//
int main(int argc, char **argv)
{
    DBOch dboch;
    return dboch.exec(argc, argv);
}
