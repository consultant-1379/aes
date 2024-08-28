//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD301D8.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD301D8.cm

//## begin module%3DEF5FD301D8.cp preserve=no
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
//## end module%3DEF5FD301D8.cp

//## Module: aes_afp_getopt%3DEF5FD301D8; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_getopt.h

#ifndef aes_afp_getopt_h
#define aes_afp_getopt_h 1

//## begin module%3DEF5FD301D8.additionalIncludes preserve=no
//## end module%3DEF5FD301D8.additionalIncludes

//## begin module%3DEF5FD301D8.includes preserve=yes
#include <cstdio>
#include <cstdlib>
#include <cstring>

//## end module%3DEF5FD301D8.includes

//## begin module%3DEF5FD301D8.declarations preserve=no
extern char *optarg_;
extern int  optind_;
//## end module%3DEF5FD301D8.declarations

//## begin module%3DEF5FD301D8.additionalDeclarations preserve=yes
//## end module%3DEF5FD301D8.additionalDeclarations


//## begin aes_afp_getopt%3DEF5FD500FF.preface preserve=yes
//## end aes_afp_getopt%3DEF5FD500FF.preface

//## Class: aes_afp_getopt%3DEF5FD500FF
//## Category: afpcmd_caa(CAA 109 0417%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3DEF5FD50169; { -> }
//## Uses: <unnamed>%3DEF5FD5016B; { -> }
//## Uses: <unnamed>%3DEF5FD50171; { -> }

class aes_afp_getopt 
{
  //## begin aes_afp_getopt%3DEF5FD500FF.initialDeclarations preserve=yes
  //## end aes_afp_getopt%3DEF5FD500FF.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: aes_afp_getopt%0B859EFDFEED; C++
      aes_afp_getopt (int argc, char** argv, const char* optstring);

    //## Destructor (generated)
      virtual ~aes_afp_getopt();


    //## Other Operations (specified)
      //## Operation: operator ()%2D096EA8FEED; C++
      int operator () (void );

      //## Operation: getopt%3DF8869E004D; C++
      int getopt (int argc, char** argv, const char* optstring);
      void reInit(void) const; //for APZ21230/5-758
    // Data Members for Class Attributes

      //## Attribute: optarg%3DEF5FD5014F
      //	Index in ARGV of the next element to be scanned.
      //	This is used for communication to and from the caller
      //	and for communication between successive calls to
      //	`getopt'.
      //	On entry to `getopt', zero means this is the first call;
      //	initialize.
      //
      //	When `getopt' returns EOF, this is the index of the
      //	first of the
      //	non-option elements that the caller should itself scan.
      //
      //	Otherwise, `optind' communicates from one call to the
      //	next
      //	how much of ARGV has been scanned so far.
      //## begin aes_afp_getopt::optarg%3DEF5FD5014F.attr preserve=no  public: char* {V} 
      char* optarg_;
      //## end aes_afp_getopt::optarg%3DEF5FD5014F.attr

      //## Attribute: optind%3DEF5FD50153
      //	Callers store zero here to inhibit the error message for
      //	unrecognized options.
      //## begin aes_afp_getopt::optind%3DEF5FD50153.attr preserve=no  public: int {V} 
      int optind_;
      //## end aes_afp_getopt::optind%3DEF5FD50153.attr

      //## Attribute: opterr%3DEF5FD50157
      //## begin aes_afp_getopt::opterr%3DEF5FD50157.attr preserve=no  public: int {V} 
      int opterr_;
      //## end aes_afp_getopt::opterr%3DEF5FD50157.attr

      //## Attribute: nargc%3DEF5FD5015B
      //## begin aes_afp_getopt::nargc%3DEF5FD5015B.attr preserve=no  public: int {V} 
      int nargc_;
      //## end aes_afp_getopt::nargc%3DEF5FD5015B.attr

      //## Attribute: nargv%3DEF5FD5015F
      //## begin aes_afp_getopt::nargv%3DEF5FD5015F.attr preserve=no  public: char** {V} 
      char** nargv_;
      //## end aes_afp_getopt::nargv%3DEF5FD5015F.attr

      //## Attribute: noptstring%3DEF5FD50164
      //## begin aes_afp_getopt::noptstring%3DEF5FD50164.attr preserve=no  public: char* {VC} 
      const char* noptstring_;
      //## end aes_afp_getopt::noptstring%3DEF5FD50164.attr

    // Additional Public Declarations
      //## begin aes_afp_getopt%3DEF5FD500FF.public preserve=yes
      //## end aes_afp_getopt%3DEF5FD500FF.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_getopt%3DEF5FD500FF.protected preserve=yes
      //## end aes_afp_getopt%3DEF5FD500FF.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_getopt%3DEF5FD500FF.private preserve=yes
      //## end aes_afp_getopt%3DEF5FD500FF.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: nextchar%3DEF5FD50174
      //	Describe the part of ARGV that contains non-options that
      //	have
      //	been skipped.  `first_nonopt' is the index in ARGV of
      //	the first of them;
      //	`last_nonopt' is the index after the last of them.
      //## begin aes_afp_getopt::nextchar%3DEF5FD50174.attr preserve=no  private: static char* {V} 
      static char* nextchar_;
      //## end aes_afp_getopt::nextchar%3DEF5FD50174.attr

      //## Attribute: first_nonopt%3DEF5FD50178
      //## begin aes_afp_getopt::first_nonopt%3DEF5FD50178.attr preserve=no  private: static int {V} 
      static int first_nonopt_;
      //## end aes_afp_getopt::first_nonopt%3DEF5FD50178.attr

      //## Attribute: last_nonopt%3DEF5FD5017C
      //## begin aes_afp_getopt::last_nonopt%3DEF5FD5017C.attr preserve=no  private: static int {V} 
      static int last_nonopt_;
      //## end aes_afp_getopt::last_nonopt%3DEF5FD5017C.attr

    // Additional Implementation Declarations
      //## begin aes_afp_getopt%3DEF5FD500FF.implementation preserve=yes
      //## end aes_afp_getopt%3DEF5FD500FF.implementation

};

//## begin aes_afp_getopt%3DEF5FD500FF.postscript preserve=yes
//## end aes_afp_getopt%3DEF5FD500FF.postscript

// Class aes_afp_getopt 

//## begin module%3DEF5FD301D8.epilog preserve=yes
//## end module%3DEF5FD301D8.epilog


#endif
