//******************************************************************************
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

#ifndef DBOCH_H
#define DBOCH_H

#include <string>
#include "command.h"

using namespace std;

//##ModelId=3CCD242901A6
class DBOch :public Command
{
public:
	//##ModelId=3CCD242901AF
    int exec(int argc, char **argv);

private:
	//##ModelId=3CCD242901C4
    void printUsage();
	//##ModelId=3CCD242901CD
};

#endif
