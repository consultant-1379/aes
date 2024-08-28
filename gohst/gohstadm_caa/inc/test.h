//	INCLUDE <file>
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//	  All rights reserved.
//
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//
//	      General rule:
//	      The error handling is specified for each method.
//
//	      No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	      190 89-CAA 109 xxxx
//
//	AUTHOR
//	      1999-04-	UAB/I/LN	xxxx
//
//	REVISION
//	      PA1  2001-06-13  xxxx  Prel. version.
//
//	LINKAGE
//
//	SEE ALSO

#ifndef __TEST_H__
#define __TEST_H__

#include "supervisor.h"
#include "threadgen.h"
#include <ace/ACE.h>

// Version information used in main() and in this class
#define PRODUCTNAME "GOHST"
#define PRODUCTNUMBER "CAL 109 01"
#define GOHSTVERSION "P5D (BC)" // Previos versions: P1A, P2A, R1A, R2A, R3A, R4A
#define UPDATED __DATE__" ("__TIME__")"

// Updated for P1E for GOH INGO4 Block Transfer
// Updated for R4A IFO sending consecutive

class Test 
{
public:
	// Modified from qmicsal 21/02/2007
	Test(ACE_TCHAR * pCPName);
	// End Modified from qmicsal
	
	virtual ~Test();
		
	bool getCommand ();
	
	ACE_TCHAR* parameter (ACE_TCHAR* kommando, ACE_INT32& i);
	
	void* gohstdef (ACE_TCHAR* kommando);
	
	ACE_INT32 help (ACE_INT32 in);
	
	bool checkParam(ACE_TCHAR *commandToCheck);

	bool validParam(ACE_TCHAR p1, ACE_TCHAR p2);

	bool checkIfNumeric(ACE_TCHAR* buf);
	
protected:
	
private: 
	
	threadGen *tg_[101]; // Can use 101 different destinations, (0 - 100)

	Supervisor *s_;

	ACE_TCHAR command_[100];
	
	ACE_TCHAR helpCmd_[25];
	
	ACE_TCHAR *tmpBuffer_;

	ACE_TCHAR *referencePath_;        //referens path   dir

	ACE_TCHAR *newOption_;

	bool helpSession_;

	bool continue_;  //int value_;

	ACE_INT32 destNr_;  // static?

	ACE_INT32 recLength_;

	ACE_INT32 nrOfRec_;

	ACE_INT32 transActLength_;

	ACE_INT32 freq_;

	ACE_INT32 threadNumber_;

	ACE_INT32 transType_;

	ACE_INT32 amount_;

	ACE_INT32 amount_block_;

//	int numberOfDest_;

	ACE_INT32 status_;

	std::string eventText_;

	std::string streamID_;

	std::string gohstDefaultPath_;   //path -p

	std::string logDefaultPath_;  //logpath  -pl

	std::string mask_;  // -m
};
#endif
