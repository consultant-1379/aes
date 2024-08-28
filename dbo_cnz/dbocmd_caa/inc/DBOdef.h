//******************************************************************************
//
// NAME
//      DBOdef.h
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
//      <General description of the class>

// DOCUMENT NO
//      19089-CAA 109 1299

// AUTHOR 
//      2002-06-19 by UAB/UKB/AU  Ulf Gustafsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//  PA1     020619  qabulfg First issue.
//          

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#ifndef DBODEF_H
#define DBODEF_H

#include <string>
#include "command.h"

using namespace std;

class DBOdef :public Command
{
public:
    int exec(int argc, char **argv);

private:
    void printUsage();
};

#endif
