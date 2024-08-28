//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD4011B.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD4011B.cm

//## begin module%3DEF5FD4011B.cp preserve=no
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
//	   2002-12-06  DAPA
//
//	REVISION
//	   A 2002-12-06 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FD4011B.cp

//## Module: aes_afp_fte%3DEF5FD4011B; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_fte.h

#ifndef aes_afp_fte_h
#define aes_afp_fte_h 1

//## begin module%3DEF5FD4011B.additionalIncludes preserve=no
//## end module%3DEF5FD4011B.additionalIncludes

//## begin module%3DEF5FD4011B.includes preserve=yes
#include <ACS_ExceptionHandler.h>
//## end module%3DEF5FD4011B.includes

#include <string>
#include "aes_afp_command.h"
//## begin module%3DEF5FD4011B.declarations preserve=no
//## end module%3DEF5FD4011B.declarations

//## begin module%3DEF5FD4011B.additionalDeclarations preserve=yes
//## end module%3DEF5FD4011B.additionalDeclarations


//## begin aes_afp_fte%3DEF5FD401C5.preface preserve=yes
//## end aes_afp_fte%3DEF5FD401C5.preface

//## Class: aes_afp_fte%3DEF5FD401C5
//## Category: afpcmd_caa(CAA 109 0417%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3DEF5FD4020B; { -> }
//## Uses: <unnamed>%3DEF5FD40215; { -> }
//## Uses: <unnamed>%3DEF5FD40216; { -> }
//## Uses: <unnamed>%3DEF5FD4021A; { -> }
//## Uses: <unnamed>%3DF6E3F60150;string { -> }

class aes_afp_fte : public aes_afp_command  //## Inherits: <unnamed>%3DEF5FD40235
{
  //## begin aes_afp_fte%3DEF5FD401C5.initialDeclarations preserve=yes
  //## end aes_afp_fte%3DEF5FD401C5.initialDeclarations

  private:
    //## begin aes_afp_fte::option_t%3DEF5FD4021D.preface preserve=yes
    //## end aes_afp_fte::option_t%3DEF5FD4021D.preface

    //## Class: option_t%3DEF5FD4021D; private
    //## Category: afpcmd_caa(CAA 109 0417%3B023EDD00FC
    //## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
    //## Persistence: Transient
    //## Cardinality/Multiplicity: n



    typedef enum option_t {FILENAME, DIRECTORY} option_t;

    //## begin aes_afp_fte::option_t%3DEF5FD4021D.postscript preserve=yes
    //## end aes_afp_fte::option_t%3DEF5FD4021D.postscript

  public:
    //## Constructors (specified)
      //## Operation: aes_afp_fte%E0FA9D23FEED; C++
      aes_afp_fte (int argc, char** argv);

    //## Destructor (generated)
      virtual ~aes_afp_fte();


    //## Other Operations (specified)
      //## Operation: parse%E78C1E30FEED; C++
      bool parse ();

      //## Operation: execute%7B2EEF47FEED; C++
      unsigned int execute ();

      //## Operation: usage%5AD518E4FEED; C++
      void usage ();

    // Additional Public Declarations
      //## begin aes_afp_fte%3DEF5FD401C5.public preserve=yes
      //## end aes_afp_fte%3DEF5FD401C5.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_fte%3DEF5FD401C5.protected preserve=yes
      //## end aes_afp_fte%3DEF5FD401C5.protected

  private:
    // Data Members for Class Attributes

      //## Attribute: directory%3DEF5FD40229
      //## begin aes_afp_fte::directory%3DEF5FD40229.attr preserve=no  private: std::string {VA} ""
      std::string directory_;
      //## end aes_afp_fte::directory%3DEF5FD40229.attr

    // Additional Private Declarations
      //## begin aes_afp_fte%3DEF5FD401C5.private preserve=yes
      //## end aes_afp_fte%3DEF5FD401C5.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: fileDestination%3DEF5FD4021F
      //## begin aes_afp_fte::fileDestination%3DEF5FD4021F.attr preserve=no  private: std::string {V} ""
      std::string fileDestination_;
      //## end aes_afp_fte::fileDestination%3DEF5FD4021F.attr

      //## Attribute: filename%3DEF5FD40223
      //## begin aes_afp_fte::filename%3DEF5FD40223.attr preserve=no  private: std::string {V} ""
      std::string filename_;
      //## end aes_afp_fte::filename%3DEF5FD40223.attr

      //## Attribute: itemName%3DEF5FD4022D
      //## begin aes_afp_fte::itemName%3DEF5FD4022D.attr preserve=no  private: std::string {V} ""
      std::string itemName_;
      //## end aes_afp_fte::itemName%3DEF5FD4022D.attr

      //## Attribute: destination%3DEF5FD40231
      //## begin aes_afp_fte::destination%3DEF5FD40231.attr preserve=no  private: std::string {V} ""
      std::string destination_;
      //## end aes_afp_fte::destination%3DEF5FD40231.attr

    // Additional Implementation Declarations
      //## begin aes_afp_fte%3DEF5FD401C5.implementation preserve=yes
      //## end aes_afp_fte%3DEF5FD401C5.implementation

};

//## begin aes_afp_fte%3DEF5FD401C5.postscript preserve=yes
//## end aes_afp_fte%3DEF5FD401C5.postscript

// Class aes_afp_fte 

//## begin module%3DEF5FD4011B.epilog preserve=yes
//## end module%3DEF5FD4011B.epilog


#endif
