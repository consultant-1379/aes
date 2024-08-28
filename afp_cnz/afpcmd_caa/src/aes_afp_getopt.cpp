//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FDA0160.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FDA0160.cm

//## begin module%3DEF5FDA0160.cp preserve=no
//	INCLUDE GetOpt.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
//	   All rights reserved.
//	   The Copyright to the computer program(s) herein
//	   is the property of Ericsson Utvecklings AB, Sweden.
//	   The program(s) may be used and/or copied only with
//	   the written permission from Ericsson Utvecklings AB or
//	   in accordance with the terms and conditions stipulated
//	   in the agreement/contract under which the program(s)
//	   have been supplied.
//
//	DESCRIPTION:
//
//	ERROR HANDLING
//	   General rule:
//	   The error handling is specified for each method.
//	   No methods initiate or send error reports unless
//	   specified.
//
//	DOCUMENT NO
//	   19089-CAA 109 0504
//
//	AUTHOR
//	   2002-12-12  UAB/UKB/AU DAPA
//
//	REVISION
//	   A 2002-12-12 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FDA0160.cp

//## Module: aes_afp_getopt%3DEF5FDA0160; Package body
//## Subsystem: AFP::afpcmd_caa::src%3DEF666D0066
//## Source file: Z:\ntaes\afp\afpcmd_caa\src\aes_afp_getopt.cpp

//## begin module%3DEF5FDA0160.additionalIncludes preserve=no
//## end module%3DEF5FDA0160.additionalIncludes

//## begin module%3DEF5FDA0160.includes preserve=yes
//## end module%3DEF5FDA0160.includes

#include "aes_afp_getopt.h"
#include <sstream>

//## begin module%3DEF5FDA0160.declarations preserve=no
int   i_opterr(1);
int   i_optind(1);
int   i_optopt;
char* i_optarg;
char* optarg_;
int optind_;
//## end module%3DEF5FDA0160.declarations

//## begin module%3DEF5FDA0160.additionalDeclarations preserve=yes
//## end module%3DEF5FDA0160.additionalDeclarations


// Class aes_afp_getopt 

//## begin aes_afp_getopt::nextchar%3DEF5FD50174.attr preserve=no  private: static char* {V} 
char* aes_afp_getopt::nextchar_;
//## end aes_afp_getopt::nextchar%3DEF5FD50174.attr

//## begin aes_afp_getopt::first_nonopt%3DEF5FD50178.attr preserve=no  private: static int {V} 
int aes_afp_getopt::first_nonopt_;
//## end aes_afp_getopt::first_nonopt%3DEF5FD50178.attr

//## begin aes_afp_getopt::last_nonopt%3DEF5FD5017C.attr preserve=no  private: static int {V} 
int aes_afp_getopt::last_nonopt_;
//## end aes_afp_getopt::last_nonopt%3DEF5FD5017C.attr

//## Operation: aes_afp_getopt%0B859EFDFEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_getopt::aes_afp_getopt (int argc, char** argv, const char* optstring)
  //## begin aes_afp_getopt::aes_afp_getopt%0B859EFDFEED.hasinit preserve=no
  //## end aes_afp_getopt::aes_afp_getopt%0B859EFDFEED.hasinit
  //## begin aes_afp_getopt::aes_afp_getopt%0B859EFDFEED.initialization preserve=yes
  //## end aes_afp_getopt::aes_afp_getopt%0B859EFDFEED.initialization
{
  //## begin aes_afp_getopt::aes_afp_getopt%0B859EFDFEED.body preserve=yes
  nargc_ = argc;
  nargv_ = argv;
  noptstring_ = optstring;
  optind_ = 1;
  opterr_=1;
  this->optarg_ = 0;
  //## end aes_afp_getopt::aes_afp_getopt%0B859EFDFEED.body
}


aes_afp_getopt::~aes_afp_getopt()
{
  //## begin aes_afp_getopt::~aes_afp_getopt%3DEF5FD500FF_dest.body preserve=yes
  //## end aes_afp_getopt::~aes_afp_getopt%3DEF5FD500FF_dest.body
}



//## Other Operations (implementation)
//## Operation: operator ()%2D096EA8FEED; C++
//## Semantics:
//	---------------------------------------------------------
//	       operator() ()
//	---------------------------------------------------------
int aes_afp_getopt::operator () (void )
{
  //## begin aes_afp_getopt::operator ()%2D096EA8FEED.body preserve=yes
  int c;
  c = getopt(nargc_, nargv_, noptstring_);
  optarg_ = i_optarg;
  optind_ = i_optind;

  return c;
  //## end aes_afp_getopt::operator ()%2D096EA8FEED.body
}

//## Operation: getopt%3DF8869E004D; C++
//## Semantics:
//	---------------------------------------------------------
//	       getopt()
//	---------------------------------------------------------
int aes_afp_getopt::getopt (int argc, char** argv, const char* optstring)
{
  //## begin aes_afp_getopt::getopt%3DF8869E004D.body preserve=yes
	static int sp = 1;
	register char c;
	register char* cp;
	
	if (sp == 1)
	{
		if (i_optind >= argc || argv[i_optind][0] != '-' || argv[i_optind][1] == '\0')
		{
			return -1;
		}
		else if (strcmp (argv[i_optind], "--") == 0) 
		{
			i_optind++;
			return -1;
		}
	}
	i_optopt = c = argv[i_optind][sp];
	if (c == ':' || (cp = const_cast<char *> (strchr(optstring, c))) == 0) 
	{
		if (argv[i_optind][++sp] == '\0') 
		{
			i_optind++;
			sp = 1;
		}
		return '?';
	}
	if (*++cp == ':')
	{
		if (argv[i_optind][sp+1] != '\0')
			i_optarg = &argv[i_optind++][sp+1];
		else if (++i_optind >= argc) 
		{
			sp = 1;
			return '?';
		} 
		else
		{
			i_optarg = argv[i_optind++];
		}
		sp = 1;
	}
	else
	{
		if (argv[i_optind][++sp] == '\0') 
		{
			sp = 1;
			i_optind++;
		}
		i_optarg = 0;
	}
	return c;
  //## end aes_afp_getopt::getopt%3DF8869E004D.body
}
//-----------------for APZ21230/5-758------------------------
void aes_afp_getopt::reInit() const// for reinitializing to 1st arguement
{ 
	i_optind = 1;
	i_optind++;

}


// Additional Declarations
  //## begin aes_afp_getopt%3DEF5FD500FF.declarations preserve=yes
  //## end aes_afp_getopt%3DEF5FD500FF.declarations

//## begin module%3DEF5FDA0160.epilog preserve=yes
//## end module%3DEF5FDA0160.epilog
