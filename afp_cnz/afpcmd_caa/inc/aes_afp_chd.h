//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD20231.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD20231.cm

//## begin module%3DEF5FD20231.cp preserve=no
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
//## end module%3DEF5FD20231.cp

//## Module: aes_afp_chd%3DEF5FD20231; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_chd.h

#ifndef aes_afp_chd_h
#define aes_afp_chd_h 1

//## begin module%3DEF5FD20231.additionalIncludes preserve=no
//## end module%3DEF5FD20231.additionalIncludes

//## begin module%3DEF5FD20231.includes preserve=yes
//## end module%3DEF5FD20231.includes

#include <string>
#include "aes_afp_command.h"
//## begin module%3DEF5FD20231.declarations preserve=no
//## end module%3DEF5FD20231.declarations

//## begin module%3DEF5FD20231.additionalDeclarations preserve=yes
//## end module%3DEF5FD20231.additionalDeclarations


//## begin aes_afp_chd%3DEF5FD20390.preface preserve=yes
//## end aes_afp_chd%3DEF5FD20390.preface

//## Class: aes_afp_chd%3DEF5FD20390
//## Category: afpcmd_caa(CAA 109 0417%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3DEF5FD3000E; { -> }
//## Uses: <unnamed>%3DEF5FD30013; { -> }
//## Uses: <unnamed>%3DF7512201F8;string { -> }

class aes_afp_chd : public aes_afp_command  //## Inherits: <unnamed>%3DEF5FD3005C
{
  //## begin aes_afp_chd%3DEF5FD20390.initialDeclarations preserve=yes
  //## end aes_afp_chd%3DEF5FD20390.initialDeclarations

  private:
    //## begin aes_afp_chd::option_t%3DEF5FD30016.preface preserve=yes
    //## end aes_afp_chd::option_t%3DEF5FD30016.preface

    //## Class: option_t%3DEF5FD30016; private
    //## Category: afpcmd_caa(CAA 109 0417%3B023EDD00FC
    //## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
    //## Persistence: Transient
    //## Cardinality/Multiplicity: n



    typedef enum option_t {DEFAULTSTATE, REMOVEDELAY, REMOVEBEFORE, NROFRETRIES, RETRYTIME} option_t;

    //## begin aes_afp_chd::option_t%3DEF5FD30016.postscript preserve=yes
    //## end aes_afp_chd::option_t%3DEF5FD30016.postscript

  public:
    //## Constructors (specified)
      //## Operation: aes_afp_chd%86CC6304FEED; C++
      aes_afp_chd (int argc, char** argv);

    //## Destructor (generated)
      virtual ~aes_afp_chd();


    //## Other Operations (specified)
      //## Operation: parse%F06555D2FEED; C++
      bool parse ();

      //## Operation: execute%CE08DA99FEED; C++
      unsigned int execute ();

      //## Operation: usage%A6DB3689FEED; C++
      void usage ();

    // Additional Public Declarations
      //## begin aes_afp_chd%3DEF5FD20390.public preserve=yes
      //## end aes_afp_chd%3DEF5FD20390.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_chd%3DEF5FD20390.protected preserve=yes
      //## end aes_afp_chd%3DEF5FD20390.protected

  private:
    // Data Members for Class Attributes

      //## Attribute: changeNumberOfRetries%3DEF5FD30034
      //## begin aes_afp_chd::changeNumberOfRetries%3DEF5FD30034.attr preserve=no  private: bool {VA} false
      bool changeNumberOfRetries_;
      //## end aes_afp_chd::changeNumberOfRetries%3DEF5FD30034.attr

    // Additional Private Declarations
      //## begin aes_afp_chd%3DEF5FD20390.private preserve=yes
      //## end aes_afp_chd%3DEF5FD20390.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: fileDestination%3DEF5FD3001C
      //## begin aes_afp_chd::fileDestination%3DEF5FD3001C.attr preserve=no  private: std::string {V} ""
      std::string fileDestination_;
      //## end aes_afp_chd::fileDestination%3DEF5FD3001C.attr

      //## Attribute: destination%3DEF5FD30020
      //## begin aes_afp_chd::destination%3DEF5FD30020.attr preserve=no  private: std::string {V} ""
      std::string destination_;
      //## end aes_afp_chd::destination%3DEF5FD30020.attr

      //## Attribute: remBefore%3DEF5FD30024
      //## begin aes_afp_chd::remBefore%3DEF5FD30024.attr preserve=no  private: std::string {V} ""
      std::string remBefore_;
      //## end aes_afp_chd::remBefore%3DEF5FD30024.attr

      //## Attribute: changeDefaultState%3DEF5FD30028
      //## begin aes_afp_chd::changeDefaultState%3DEF5FD30028.attr preserve=no  private: bool {V} false
      bool changeDefaultState_;
      //## end aes_afp_chd::changeDefaultState%3DEF5FD30028.attr

      //## Attribute: changeRemoveDelay%3DEF5FD3002C
      //## begin aes_afp_chd::changeRemoveDelay%3DEF5FD3002C.attr preserve=no  private: bool {V} false
      bool changeRemoveDelay_;
      //## end aes_afp_chd::changeRemoveDelay%3DEF5FD3002C.attr

      //## Attribute: changeRemoveBefore%3DEF5FD30030
      //## begin aes_afp_chd::changeRemoveBefore%3DEF5FD30030.attr preserve=no  private: bool {V} false
      bool changeRemoveBefore_;
      //## end aes_afp_chd::changeRemoveBefore%3DEF5FD30030.attr

      //## Attribute: changeRetryTime%3DEF5FD30038
      //## begin aes_afp_chd::changeRetryTime%3DEF5FD30038.attr preserve=no  private: bool {V} false
      bool changeRetryTime_;
      //## end aes_afp_chd::changeRetryTime%3DEF5FD30038.attr

      //## Attribute: defaultState%3DEF5FD3003D
      //## begin aes_afp_chd::defaultState%3DEF5FD3003D.attr preserve=no  private: AES_GCC_Filestates {V} AES_FSREADY
      AES_GCC_Filestates defaultState_;
      //## end aes_afp_chd::defaultState%3DEF5FD3003D.attr

      //## Attribute: removeDelay%3DEF5FD30041
      //## begin aes_afp_chd::removeDelay%3DEF5FD30041.attr preserve=no  private: int {V} DEFAULT_REMOVEDELAY_
      int removeDelay_;
      //## end aes_afp_chd::removeDelay%3DEF5FD30041.attr

      //## Attribute: removeBefore%3DEF5FD30045
      //## begin aes_afp_chd::removeBefore%3DEF5FD30045.attr preserve=no  private: bool {V} false
      bool removeBefore_;
      //## end aes_afp_chd::removeBefore%3DEF5FD30045.attr

      //## Attribute: numberOfRetries%3DEF5FD30049
      //## begin aes_afp_chd::numberOfRetries%3DEF5FD30049.attr preserve=no  private: int {V} DEFAULT_NROFRETRIES_
      int numberOfRetries_;
      //## end aes_afp_chd::numberOfRetries%3DEF5FD30049.attr

      //## Attribute: retryTime%3DEF5FD3004D
      //	AES_AFP_Api	api;
      //## begin aes_afp_chd::retryTime%3DEF5FD3004D.attr preserve=no  private: int {V} DEFAULT_RETRYTIME_
      int retryTime_;
      //## end aes_afp_chd::retryTime%3DEF5FD3004D.attr

      // Additional Implementation Declarations
      char trace_[128];
      //## begin aes_afp_chd%3DEF5FD20390.implementation preserve=yes
      //## end aes_afp_chd%3DEF5FD20390.implementation

};

//## begin aes_afp_chd%3DEF5FD20390.postscript preserve=yes
//## end aes_afp_chd%3DEF5FD20390.postscript

// Class aes_afp_chd 

//## begin module%3DEF5FD20231.epilog preserve=yes
//## end module%3DEF5FD20231.epilog


#endif
