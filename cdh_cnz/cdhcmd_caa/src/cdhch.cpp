
//*************************************************************************
//
// NAME
//      cdhch.cpp
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

// DESCRIPTION 
//  This program is activated by the operator. The attached
//  arguments is sent further to method changeAttr in class
//  AES_CDH_Destination. When the service have been performed,
//  a return code is supplied. According to this code an
//  answer printout is created in function print_message
//  specified below. The source code is adapted for WinNT only.

// DOCUMENT NO
//  190 89-CAA 109 1303

// AUTHOR 
//  1999-09-14 by I/LD Gunnar Andersson

// CHANGES
//
// RELEASE REVISION HISTORY
// 
// DATE			NAME	DESCRIPTION
// 2001-05-29	QABULFG	INGO3 GOH. Printout "usage" message changed.
// 2002-02-14  QABHANS  File Notification. Printout "usage" message changed.
// 2002-08-01  QABHANS  INGO4 GOH. Printout "usage" message changed.
//                      Connect type should not be possible to change.
//                      See HD41756
// 2005-12-23  XTBBODA  Printout "usage" message changed.
//                      Added -z for resend file notification
//
// SEE ALSO 
//  AES_CDH_Destination
//
//*************************************************************************

#ifdef WIN32
#pragma warning(disable:4786)
#endif
#include <osf/Get_Opt.h>
#include "aes_cdh_destination.h"
#include <iostream>
#include "ACS_PRC_Process.H"
#include "ACS_ExceptionHandler.H"
#ifdef WIN32
#include <conio.h>
#endif

using namespace std;

//-------------------------------------------------------------------------
//  Function print_message. 
//-------------------------------------------------------------------------
void print_message(AES_CDH_Result result)
{
    if (result.code() == AES_CDH_RC_INCUSAGE)
    {
        cout << result << endl;
        cout << "Usage: " << endl;
        cout << "cdhch [-a address] [-s retries] [-m block_retrydelay]"<< endl;
		cout << "      destination"<< endl;

        cout << "cdhch [-a address] [-r remote_dir] [-n portnr]"<< endl;
        cout << "      [-u user_name] [-p] [-o overwrite]"<< endl;
		cout << "      [-s retries] [-d file_retrydelay ] destination"<< endl;

        cout << "cdhch [-o overwrite] [-b keeptime] destination"<< endl;
    
        cout << "cdhch [-e event_type] [-b keeptime] [-f address] [-o overwrite] "<< endl;
        cout << "      [-x portnr] [-y response_time]"<<endl;
        cout << "      [-z resend_notification_time] destination"<< endl;
    }
    else
    {
        cout << result << endl;
    }
}

//-------------------------------------------------------------------------
//  Function parse_cdhch
//-------------------------------------------------------------------------
int parse_cdhch(int& argc, char *argv[])
{
    int opt;
    int r_code = AES_CDH_RC_OK;
    int pFlag = 0;
    int parg;
        
    OSF_Get_Opt getopt(argc, argv, "p");

    while ((opt = getopt()) != EOF)

	{
		switch (opt) 
        {
			case 'p':
            {

                pFlag++;
                parg = getopt.optind-1;
				break;
            }
			case '?':
            {
                if ((getopt.optind + 1) < argc)
                {
                    if (argv[getopt.optind][0] != '-' && 
                        argv[getopt.optind + 1][0] == '-')
                        getopt.optind++;
                }
				break;
            }
            default:
            {
                return AES_CDH_RC_INTPROGERR;
            }
		}
	}

    if (pFlag > 1) return AES_CDH_RC_INCUSAGE;

    if (pFlag == 1)
    {
        string password = "";
        int ch;
        
        cout << "Password\03: " << flush;

        while(true)
        {
            ch = _getch();
            if (ch == 13) break;

            if (ch != 10)
            {
                password = password + (char)ch;
                cout << "*" << flush;
            }
        }

        cout << endl;

        for(int i = argc; i > parg+1; i--)
        {
            argv[i] = argv[i-1];
        }
        argv[i] = new char[password.length()+1];
        (void) strcpy(argv[i], password.c_str());
        argc++;
    }

    return r_code;
}

//-------------------------------------------------------------------------
//  Main program cdhch
//-------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
	int    newArgc;
	char*  newArgv[32];
    AES_CDH_Result result;
    int exitCode;

    AP_InitProcess ("AES_CDH_Cdhch", AP_COMMAND); 
    AP_SetCleanupAndCrashRoutine("AES_CDH_Cdhch",NULL);

    if (argc <= 31)
    {
        newArgc = argc;

        for (int i = 0; i < argc; i++)
	    {
            newArgv[i] = new char[strlen(argv[i])+1];
            (void) strcpy(newArgv[i], argv[i]);
        }

        exitCode = parse_cdhch(newArgc, newArgv);

        if (exitCode == AES_CDH_RC_OK)
        {
	        result = AES_CDH_Destination::changeAttr(newArgc, newArgv);
        }
        else
        {
            result = AES_CDH_Result(exitCode);
        }

        // Release newArgv
	    for (i = 0; i < newArgc; i++)
        {
            delete [] newArgv[i];
        }
    }
    else
    {
        result = AES_CDH_Result(AES_CDH_RC_INCUSAGE);
    }

	if (result.code() != AES_CDH_RC_OK)
		print_message(result);

   if (result.code() == AES_CDH_RC_NOSERVER)
      return 117;

	return result.code();
}