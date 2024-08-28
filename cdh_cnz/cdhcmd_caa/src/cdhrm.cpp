//******************************************************************************
//
// NAME
//      cdhrm.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
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
// 	1999-06-23 by UAB/I/LD Per Lindström

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
		cout << "Usage: cdhrm destination" << endl;
    }
    else
    {
        cout << result << endl;
    }
}

//------------------------------------------------------------------------------
//	Main program cdhrm
//------------------------------------------------------------------------------

int main(int argc, char *argv[]) 
{
	AES_CDH_Result result;

    AP_InitProcess ("AES_CDH_Cdhrm", AP_COMMAND); 
    AP_SetCleanupAndCrashRoutine("AES_CDH_Cdhrm",NULL);

	result = AES_CDH_Destination::remove(argc, argv);

    if (result.code() != AES_CDH_RC_OK)
        print_message(result);

    if (result.code() == AES_CDH_RC_NOSERVER)
       return 117;

    return result.code();
}
