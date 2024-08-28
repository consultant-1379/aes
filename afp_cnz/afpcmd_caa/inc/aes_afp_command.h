//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD202DB.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD202DB.cm

//## begin module%3DEF5FD202DB.cp preserve=no
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
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
//	   B 2003-04-25 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FD202DB.cp

//## Module: aes_afp_command%3DEF5FD202DB; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_command.h

#ifndef aes_afp_command_h
#define aes_afp_command_h 1

//## begin module%3DEF5FD202DB.additionalIncludes preserve=no
//## end module%3DEF5FD202DB.additionalIncludes

//## begin module%3DEF5FD202DB.includes preserve=yes
#include <ace/OS.h>
#include <cstdlib>
#include <climits>
#include <cerrno>
//## end module%3DEF5FD202DB.includes

#include <ACS_TRA_trace.h>
//#include "acs_prc_process.h"
#include "aes_afp_getopt.h"
#include "aes_afp_api.h"
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_filestates.h>
#include <aes_gcc_log.h>
#include <acs_apgcc_omhandler.h>
#include <aes_afp_defines.h>
//## begin module%3DEF5FD202DB.declarations preserve=no
#define DirSeparator '\\'
//## end module%3DEF5FD202DB.declarations
//## begin module%3DEF5FD202DB.additionalDeclarations preserve=yes
//## end module%3DEF5FD202DB.additionalDeclarations


//## begin optpair_t%3DEF5FD3026E.preface preserve=yes
//## end optpair_t%3DEF5FD3026E.preface

//## Class: optpair_t%3DEF5FD3026E
//	Option and argument
//## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



struct optpair_t 
{
  //## begin optpair_t%3DEF5FD3026E.initialDeclarations preserve=yes
  //## end optpair_t%3DEF5FD3026E.initialDeclarations

    // Data Members for Class Attributes

      //## Attribute: opt%3DEF5FD30278
      //	Options.
      //## begin optpair_t::opt%3DEF5FD30278.attr preserve=no  public: char {V} 
      char opt_;
      //## end optpair_t::opt%3DEF5FD30278.attr

      //## Attribute: arg%3DEF5FD3027C
      //	Arguments.
      //## begin optpair_t::arg%3DEF5FD3027C.attr preserve=no  public: char* {V} 
      char* arg_;
      //## end optpair_t::arg%3DEF5FD3027C.attr

  public:
    // Additional Public Declarations
      //## begin optpair_t%3DEF5FD3026E.public preserve=yes
      //## end optpair_t%3DEF5FD3026E.public

  protected:
    // Additional Protected Declarations
      //## begin optpair_t%3DEF5FD3026E.protected preserve=yes
      //## end optpair_t%3DEF5FD3026E.protected

  private:
    // Additional Private Declarations
      //## begin optpair_t%3DEF5FD3026E.private preserve=yes
      //## end optpair_t%3DEF5FD3026E.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin optpair_t%3DEF5FD3026E.implementation preserve=yes
      //## end optpair_t%3DEF5FD3026E.implementation

};

//## begin optpair_t%3DEF5FD3026E.postscript preserve=yes
//## end optpair_t%3DEF5FD3026E.postscript

//## begin aes_afp_command%3DEF5FD3005B.preface preserve=yes
//## end aes_afp_command%3DEF5FD3005B.preface

//## Class: aes_afp_command%3DEF5FD3005B; Abstract
//	Base class for afp command classes.
//## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3DEF5FD300CA; { -> }
//## Uses: <unnamed>%3DEF5FD300D4; { -> }
//## Uses: <unnamed>%3DF6FB8B013D;AES_AFP_Api { -> }
//## Uses: <unnamed>%3DF6FBC902A5;AES_GCC_Errorcodes { -> }
//## Uses: <unnamed>%3DF6FBEA025C;AES_GCC_Filestates { -> }
//## Uses: <unnamed>%3DF6FC3203B4;ACS_TRA_trace { -> }
//## Uses: <unnamed>%3DF6FC58006F;ACS_PRC_Process { -> }
//## Uses: <unnamed>%3DF891B00084;aes_afp_getopt { -> }
//## Uses: <unnamed>%3EA9332B004B;AES_GCC_Log { -> }
//## Uses: <unnamed>%3EA933E202E3;optpair_t { -> }

class aes_afp_command 
{
  //## begin aes_afp_command%3DEF5FD3005B.initialDeclarations preserve=yes
  //## end aes_afp_command%3DEF5FD3005B.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: aes_afp_command%7822341DFEED; C++
      //	Constructor.
      aes_afp_command ();

    //## Destructor (generated)
      virtual ~aes_afp_command();


    //## Other Operations (specified)
      //## Operation: parse%8A8B66EDFEED; C++
      //	Parses the command line.
      virtual bool parse () = 0;

      //## Operation: execute%3133A4DEFEED; C++
      //	Executes the commands.
      virtual unsigned int execute () = 0;

      //## Operation: usage%23614C6CFEED; C++
      //	Prints usage message.
      virtual void usage () = 0;

      //## Operation: launch%8410772FFEED; C++
      //	Starts command execution.
      int launch ();
//      int launchrm ();

      //## Operation: printErrorMessage%4BAF4D3BFEED; C++
      //	Prints error message.
      void printErrorMessage (unsigned int err);

	  void printErrorMessage (const string& errorMessage);

      //## Operation: checkNumericValue%1CE69C14FEED; C++
      //	Checks numeric values.
      bool checkNumericValue (std::string str);

      // Additional Public Declarations
      //## begin aes_afp_command%3DEF5FD3005B.public preserve=yes
      //## end aes_afp_command%3DEF5FD3005B.public

  protected:

    //## Other Operations (specified)
      //## Operation: toUpper%3DF7012D00E6; C++
      //	Changes a string to capitals.
      char* toUpper (char* str);

    // Data Members for Class Attributes

      //## Attribute: argC%3DEF5FD300E8
      //## begin aes_afp_command::argC%3DEF5FD300E8.attr preserve=no  protected: int {V} 
      int argC_;
      //## end aes_afp_command::argC%3DEF5FD300E8.attr

      //## Attribute: argV%3DEF5FD300EC
      //## begin aes_afp_command::argV%3DEF5FD300EC.attr preserve=no  protected: char** {V} 
      char** argV_;
      //## end aes_afp_command::argV%3DEF5FD300EC.attr

      //## Attribute: DEFAULT_REMOVEDELAY%3DEF5FD300F1
      //## begin aes_afp_command::DEFAULT_REMOVEDELAY%3DEF5FD300F1.attr preserve=no  protected: const int {VC} 10
      const int DEFAULT_REMOVEDELAY_;
      //## end aes_afp_command::DEFAULT_REMOVEDELAY%3DEF5FD300F1.attr

      //## Attribute: DEFAULT_NROFRETRIES%3DEF5FD300F5
      //## begin aes_afp_command::DEFAULT_NROFRETRIES%3DEF5FD300F5.attr preserve=no  protected: const int {VC} 0
      const int DEFAULT_NROFRETRIES_;
      //## end aes_afp_command::DEFAULT_NROFRETRIES%3DEF5FD300F5.attr

      //## Attribute: DEFAULT_RETRYTIME%3DEF5FD300F9
      //## begin aes_afp_command::DEFAULT_RETRYTIME%3DEF5FD300F9.attr preserve=no  protected: const int {VC} 10
      const int DEFAULT_RETRYTIME_;
      //## end aes_afp_command::DEFAULT_RETRYTIME%3DEF5FD300F9.attr

      //## Attribute: DEFAULT_STARTUPSEQUENCENUMBER
      //##
      const int DEFAULT_STARTUPSEQUENCENUMBER_;
      //##

      //## Attribute: numericCharsWithMinusSign%3DEF5FD300FD
      //## begin aes_afp_command::numericCharsWithMinusSign%3DEF5FD300FD.attr preserve=no  protected: const std::string {VC} "-0123456789"
      const std::string numericCharsWithMinusSign_;
      //## end aes_afp_command::numericCharsWithMinusSign%3DEF5FD300FD.attr

      //## Attribute: api%3DEF5FD30102
      //## begin aes_afp_command::api%3DEF5FD30102.attr preserve=no  protected: AES_AFP_Api {V} 
      AES_AFP_Api api_;
      //## end aes_afp_command::api%3DEF5FD30102.attr

    // Additional Protected Declarations
      //## begin aes_afp_command%3DEF5FD3005B.protected preserve=yes
      //## end aes_afp_command%3DEF5FD3005B.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_command%3DEF5FD3005B.private preserve=yes
      //## end aes_afp_command%3DEF5FD3005B.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin aes_afp_command%3DEF5FD3005B.implementation preserve=yes
      //## end aes_afp_command%3DEF5FD3005B.implementation

};

//## begin aes_afp_command%3DEF5FD3005B.postscript preserve=yes
std::ostream& operator << (std::ostream& s, optpair_t option);
//## end aes_afp_command%3DEF5FD3005B.postscript

// Class optpair_t 

// Class aes_afp_command 

//## begin module%3DEF5FD202DB.epilog preserve=yes
//## end module%3DEF5FD202DB.epilog


#endif
