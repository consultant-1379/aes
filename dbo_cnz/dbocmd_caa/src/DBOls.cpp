//*******************************************************************
//
// NAME
//      DBOls.cpp
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
#include <sstream>
#include <stdlib.h>
#include <aes_gcc_errorcodes.h>
#include <ACS_APGCC_GetOpt.H>
#include "AES_DBO_TQManager.h"
#include "DBOls.h"
#include "command.h"

using namespace std;

//*******************************************************************
//
int DBOls::exec(int argc, char **argv)
{
    int l_flag = 0;
    int t_flag = 0;
    string transferqueue;
    string destination;
    int opt;
    //cout<<"Executing dbols"<<endl;
    while ((opt = getopt(argc, argv, "l")) != -1)
    {
	    switch(opt) {
        case 'l':
		if(l_flag)
		{
			printUsage();
			return 2;
			break;
		}	
		l_flag++;
            break;
        default:
		    printUsage();
		    return 2;
            break;
        }
    }

    if (argc > 3 || ( !l_flag && argc ==3))
    {
        printUsage();
        return 2;
    }
    if((!l_flag && argc == 2) || (l_flag == 1 && argc == 3))
    {
        transferqueue = argv[argc - 1];
        t_flag++;
    }
   
//cout<<"Checking DBO TQ name "<<endl; 
    // Check DBO transfer queue name
    if (t_flag == 1)
    {
		if (!validTQName(transferqueue))
        {
			cerr << "Unreasonable value: " << transferqueue.c_str() << endl;
            return 3;
        }
		//toUpper(transferqueue);
    }
    
    // Everything is alright, send this ls order to DBO
    list<string> dList;
    bool found = false;
    unsigned int rCode;
	//cout<<"Send the ls order to DBO"<<endl;
    if (t_flag == 0)
{
        rCode = AES_DBO_TQManager::instance()->tqList(dList);
}   else
        rCode = AES_DBO_TQManager::instance()->tqList(dList, transferqueue);
    
    if(rCode == AES_NOSERVERACCESS)
    {
        string error = AES_DBO_TQManager::instance()->getErrorText(rCode);
        cerr << error << endl;
		  return 117;
    }

    if(! dList.empty())
    {
        string tq;
        string dest;
        string mirror;
        string userGroup;
        unsigned int delay;
        bool header = true;

        cout << "DBO TABLE" << endl;

        // Set default adjustment
        cout.setf(ios_base::left, ios_base::adjustfield);

        dList.sort();
        list<string>::iterator it;
        for ( it = dList.begin(); it != dList.end(); it++ )
        {
            istringstream s(*it);
            s >> tq >> dest >> mirror >> delay >> userGroup;

            // Check if only one tq should be printed
            if (t_flag == 1 && tq != transferqueue)
                continue;
            else
                found = true;

            // Check if there should be a header
            if (header || l_flag == 1)
            {
                cout << endl
                    << setw(41) << "TRANSFER QUEUE"
                    << setw(33) << "USER GROUP"
                    << endl;
                header = false;
            }

            // Printout the transfer queue
            cout << setw(41) << tq;

            // Check if usergroup should be printed
            if (userGroup != "\\")
                cout << setw(33) << userGroup;
            else
                cout << setw(33) << ' ';
            cout << endl;

            // Check if long listing
            if (l_flag == 1)
            {
                cout << endl
                    << setw(8) << ' '
                    << setw(33) << "DESTINATION SET"
                    << setw(6) << "MIRR"
                    << setw(7) << "R_DELAY"
                    << endl;
                cout << setw(8) << ' ';
                
                // Check if remove delay should be printed
                cout << setw(33) << dest
                    << setw(6) << mirror;
                if (mirror == "YES")
                    cout << setw(7) << right << delay << left;
                else
                    cout << setw(7) << ' ';
                cout << endl;
            }
        }
    }
    else if (t_flag == 0)
    {
        // Printout just the header if no TQ is defined and
        // no specifical TQ was requested
        cout << "DBO TABLE" << endl;
	cout<<"printout just the header as no TQ is defined"<<endl;
    }

    if (t_flag == 1 && !found)
    {
        rCode = AES_NOPROCORDER;
        string error = AES_DBO_TQManager::instance()->getErrorText(rCode);
        cerr << error << endl;
    }

    // Close session
    return rCode;
}


//*******************************************************************
//
void DBOls::printUsage()
{
    cerr << "Incorrect usage" << endl;
    cerr << "Usage:" << endl;
	cerr << "dbols [-l] [transferqueue]" << endl;
}

//*******************************************************************
//
int main(int argc, char **argv)
{
    DBOls dbols;
    return dbols.exec(argc, argv);
}


