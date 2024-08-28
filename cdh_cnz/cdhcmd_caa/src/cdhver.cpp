//******************************************************************************
//
// NAME
//      cdhver.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2001.
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
//  This program is activated by the operator. The attached
//  arguments is sent further to method remove in class
//  AES_CDH_Destination. When the service have been performed,
//  a return code is supplied. According to this code an
//  answer printout is created in function print_message
//  specified below.

// DOCUMENT NO
//	190 89-CAA 109 0421

// AUTHOR 
// 	2001-09-26 by UAB/S/AU qabhans

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          020227  qabhans Changed usage printout and made modifications
//                          for File Notification.


// SEE ALSO 
// 	AES_CDH_Destination
//
//******************************************************************************

#ifdef WIN32
	#pragma warning( disable : 4786 )
#endif
#include "aes_cdh_destination.h"
#include <iostream>
#include <string>
#include "ACS_PRC_Process.H"
#include "ACS_ExceptionHandler.H"

using namespace std;

//-------------------------------------------------------------------------
//  Function print_message. 
//-------------------------------------------------------------------------
void print_message(AES_CDH_Result result)
{
    if (result.code() == AES_CDH_RC_INCUSAGE)
    {
        cout << result << endl;
        cout << "Usage: cdhver [-m] destination" << endl;
    }
    else
    {
        cout << result << endl;
    }
}



//-------------------------------------------------------------------------
//	Main program cdhver
//-------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    AES_CDH_Result result;
    vector<AES_CDH_Destination::connAttributes>	attr;
    vector<AES_CDH_Destination::connAttributes>::iterator itr;
    int exitCode = AES_CDH_RC_OK;

    AP_InitProcess ("AES_CDH_Cdhver", AP_COMMAND);
    AP_SetCleanupAndCrashRoutine("AES_CDH_Cdhver",NULL);

    result = AES_CDH_Destination::checkConnection(argc, argv, attr);

    if (result.code() != AES_CDH_RC_OK)
    {
        print_message(result);
        exitCode = result.code();
    }

    else
    {
        itr = attr.begin();
        string dest = (*itr).destName;
        AES_CDH_ResultCode statusResult = (*itr).rCode;

        if (strcmp(argv[1], "-m") == 0)
        {
            if (statusResult != AES_CDH_RC_CONNECTOK)
            {
				if (statusResult == AES_CDH_RC_TIMEOUT)
				{
					// Even if timeout expired when option -m
					// has been used, diagnostic code:
					// "Error connecting to remote system"
					// should be printed.
	                
					// HD82813 in order not to confuse by different
					// error codes, change to AES_CDH_RC_CONNECTERR
			        
					statusResult = AES_CDH_RC_CONNECTERR;
				}

				AES_CDH_Result res(statusResult);
				cout << res << endl;
				exit(statusResult);
            }
            else
            {
                exit(statusResult);
            }
        }

        itr++;
        AES_CDH_ResultCode notifResult  = (*itr).rCode;
        if (notifResult == AES_CDH_RC_INTPROGERR)
        {
            AES_CDH_Result err(notifResult);
            cerr << err << endl;
            exit(notifResult);
        }
        else
        {
            char header[100];
            memset((char *)header, 0, sizeof(header));
            sprintf(header, "%-34s%-15s%-15s", "DESTINATION", "STATUS", "NOTIFICATION");
            
            char destination[100];
            memset((char *)destination, 0, sizeof(header));
            sprintf(destination, "%-34s", dest.c_str());

            char status[100];
            memset((char *)status, 0, sizeof(header));
            char notification[100];
            
            switch(statusResult)
            {

                case AES_CDH_RC_CONNECTOK:
                {
                    sprintf(status, "%-15s", "OK");
                    break;
                }

                case AES_CDH_RC_CONNECTERR:
                {
                    sprintf(status, "%-15s", "Not OK");
                    break;
                }

                case AES_CDH_RC_LOGONFAILURE:
                {
                    sprintf(status, "%-15s", "Logon failure");
                    break;
                }

                default:
                {
                    AES_CDH_Result err(AES_CDH_RC_INTPROGERR);
                    cerr << err << endl;
                    exit(AES_CDH_RC_INTPROGERR);
                }
            }

            switch(notifResult)
            {
                case AES_CDH_RC_CONNECTOK:
                {
                    sprintf(notification, "%-15s", "OK");
                    break;
                }

                case AES_CDH_RC_CONNECTERR:
                {
                    sprintf(notification, "%-15s", "Not OK");
                    break;
                }
    
                case AES_CDH_RC_NOTNOTIFDEST:
                {
                    // The destination exists but is not defined with file notification
                    sprintf(notification, "%-15s", "-");
                    break;
                }

				case AES_CDH_RC_TIMEOUT:
				{
					// This is returned if the timeout expired for the
					// command. During period of 8 seconds server was
					// not able to connect with notification destination.
                    sprintf(notification, "%-15s", "Timeout");
					break;
				}

                default:
                {
                    AES_CDH_Result err(AES_CDH_RC_INTPROGERR);
                    cerr << err << endl;
                    exit(AES_CDH_RC_INTPROGERR);
                }
            }
            
            char printOut[320];
            strcpy(printOut, destination);
            strcat(printOut, status);
            strcat(printOut, notification);
            
            cout << header << endl;
            cout << printOut << endl;

        }
    } 

    if (result.code() == AES_CDH_RC_NOSERVER)
       return 117;

    return result.code();
}
