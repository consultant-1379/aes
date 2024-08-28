//*******************************************************************
//
// NAME
//      command.cpp
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

#include "command.h"

//*******************************************************************
//method toUpper
//*******************************************************************
void Command::toUpper(string &str)
{
    int len = str.length();
    
    for (int i = 0; i <= len - 1; i++)
        if (str[i] >= 97 && str[i] <= 122) 
            str[i] = str[i] - 32;
}

//*******************************************************************
//method validTQName
//*******************************************************************

bool Command::validTQName(string str)
{
    const string legalChars(
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    const string legalChars2(
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    
    if (str.find_first_not_of(legalChars) == string::npos)
    {
        if (str.substr(0,1).find_first_not_of(legalChars2) == string::npos)
        {
			if (str.length() < 33)
			{
				return true;
			}
        }
    }   
    return false;
}

//*******************************************************************
//method validDestName
//*******************************************************************
bool Command::validDestName(string str)
{
    const string legalChars(
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    const string legalChars2(
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    
    if (str.find_first_not_of(legalChars) == string::npos)
    {
        if (str.substr(0,1).find_first_not_of(legalChars2) == string::npos)
        {
            if (str.length() < 33)
			{
				return true;
			}
        }
    } 
    return false;
}
