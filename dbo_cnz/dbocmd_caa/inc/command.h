//******************************************************************************
//
// NAME
//      command.h
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
//      2002-06-19 by UAB/KB/AU  Ulf Gustafsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          020619  qabulfg First draft.

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#ifndef COMMAND_H
#define COMMAND_H

#include <string>

using namespace std;

class Command
{
public:
    
void toUpper(string &str);

bool validTQName(string str);

bool validDestName(string str);

};

#endif
