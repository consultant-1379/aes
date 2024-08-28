//******************************************************************************
// 
// .NAME
//  	BlockPairMapperManager
// .LIBRARY 3C++
// .PAGENAME BlockPairMapperManager
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE blockpairmappermanager.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
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
// 	<General description of the class>

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//  190 89-CAA 109 0507

// AUTHOR 
//  2003-01-14 by UAB/UKY/AU Hans-Erik Nilsson

// .LINKAGE
//	<link information: libraries needed and order>

// .SEE ALSO
// 	<Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************

#ifndef BLOCKPAIRMAPPERMANAGER_H
#define BLOCKPAIRMAPPERMANAGER_H

#include <ace/Synch.h>
#include <map>
#include <string>

#include <blockpairmapper.h>

//##ModelId=3E22DCC201B6
class BlockPairMapperManager
{
private:
	//##ModelId=3E8AA6A2001F
	std::map<std::string, BlockPairMapper *> db;

	//##ModelId=3E8A9D600111
	ACE_Mutex mapperMX_;

	//##ModelId=3E22DCD900F1
    static BlockPairMapperManager *instance_;
    
	//##ModelId=3E22DCEC01A2
    BlockPairMapperManager();

	//##ModelId=3E22DCF400FA
    virtual ~BlockPairMapperManager();

public:
	//Fetches the blockpair-mapper specified by name.
	//May return a NULL-pointer if memory allocation for name 
	//failed
	//##ModelId=3E22DD6B03C2
	BlockPairMapper * getMapper(const std::string & name);

	//Removes the blockpair-mapper specified by name
	//##ModelId=3E22DDB9002B
	void remove(const std::string & name);
	
	//Returns the only pointer to the manager
	//##ModelId=3E22DD3C0143
    static BlockPairMapperManager *instance();
    // Returns the only pointer to the manager

	//##ModelId=3E8AAB2401E1
    void destroy();
};

#endif
