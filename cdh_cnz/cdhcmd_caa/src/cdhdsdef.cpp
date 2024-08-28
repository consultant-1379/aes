
//*************************************************************************
//
// NAME
//      cdhdsdef.cpp
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
//  arguments is sent further to method define in class
//  AES_CDH_DestinationSet. When the service have been performed,
//  a return code is supplied. According to this code an
//  answer printout is created in function print_message
//  specified below. The source code is adapted for WinNT only.

// DOCUMENT NO
//  190 89-CAA 109 0508

// AUTHOR 
//  2002-11-28 by UAB/UKY/AU Ulf Gustafsson

// CHANGES
//
// RELEASE REVISION HISTORY
// 
// DATE			NAME	DESCRIPTION
// 2002-12-13	qabhefe	First Version
//
// SEE ALSO 
//  AES_CDH_DestinationSet
//
//*************************************************************************

#ifdef WIN32
#pragma warning(disable:4786)
#endif
#include <osf/Get_Opt.h>
#include "aes_cdh_destinationset.h"
#include "aes_cdh_resultcode.h"
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
        cout << "cdhdsdef [-v usergroup] [-s secondarydestination]"<< endl;
        cout << "         [-b backupdestination] primarydestination destinationset"<< endl;
    }
    else
    {
        cout << result << endl;
    }
}

/*
//-------------------------------------------------------------------------
//  Function parse_cdhdsdef
//  Parse the -v user group
//-------------------------------------------------------------------------
int parse_cdhdef(int& argc, char *argv[])
{

    int opt;
    int r_code = AES_CDH_RC_OK;
    int vFlag = 0;
        
    OSF_Get_Opt getopt(argc, argv, "v");

    while ((opt = getopt()) != EOF)

	{
		switch (opt) 
        {
			case 'v':
            {

                vFlag++;
                //read verification
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

	if (vFlag > 1)
		r_code = AES_CDH_RC_INCUSAGE;

    return r_code;
}
*/

//-------------------------------------------------------------------------
//  Main program cdhdsdef
//-------------------------------------------------------------------------
int main(int argc, char* argv[]) 
{
    AES_CDH_Result result;

    AP_InitProcess ("AES_CDH_Cdhdsdef", AP_COMMAND); 
    AP_SetCleanupAndCrashRoutine("AES_CDH_Cdhdsdef",NULL);

    if (argc <= 31)
    {
        result = AES_CDH_DestinationSet::define(argc, argv);
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