//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD30318.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD30318.cm

//## begin module%3DEF5FD30318.cp preserve=no
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
//## end module%3DEF5FD30318.cp

//## Module: aes_afp_def%3DEF5FD30318; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_def.h

#ifndef aes_afp_def_h
#define aes_afp_def_h 1

//## begin module%3DEF5FD30318.additionalIncludes preserve=no
//## end module%3DEF5FD30318.additionalIncludes

//## begin module%3DEF5FD30318.includes preserve=yes
//## end module%3DEF5FD30318.includes

#include "aes_afp_command.h"
#include <string>
#include <sstream>

//## begin module%3DEF5FD30318.declarations preserve=no
//## end module%3DEF5FD30318.declarations

//## begin module%3DEF5FD30318.additionalDeclarations preserve=yes
//## end module%3DEF5FD30318.additionalDeclarations


//## begin aes_afp_def%3DEF5FD303AF.preface preserve=yes
//## end aes_afp_def%3DEF5FD303AF.preface

//## Class: aes_afp_def%3DEF5FD303AF
//	This class is responsible for the afpdef command.
//## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3DEF5FD40024; { -> }
//## Uses: <unnamed>%3DF7353401CE;string { -> }
//## Uses: <unnamed>%3DF7354D0044;strstream { -> }

class aes_afp_def : public aes_afp_command  //## Inherits: <unnamed>%3DEF5FD40078
{
  //## begin aes_afp_def%3DEF5FD303AF.initialDeclarations preserve=yes
  //## end aes_afp_def%3DEF5FD303AF.initialDeclarations

  private:
    //## begin aes_afp_def::option_t%3DEF5FD4002B.preface preserve=yes
    //## end aes_afp_def::option_t%3DEF5FD4002B.preface

    //## Class: option_t%3DEF5FD4002B; private
    //## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
    //## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
    //## Persistence: Transient
    //## Cardinality/Multiplicity: n


    // uabmha: CNI 1135, added STARTUPSEQUENCENUMBER, for -k option
    typedef enum option_t 
       {
          DEFAULTSTATUS,
          TEMPLATE,
          NAMETAG,
          STARTUPSEQUENCENUMBER,
          RENAME,
          REMOVEDELAY,
          REMOVEBEFORE,
          SENDRETRIES,
          SENDRETRIESDELAY,
          SOURCEDIRECTORY,
          USERGROUP} option_t;

    //## begin aes_afp_def::option_t%3DEF5FD4002B.postscript preserve=yes
    //## end aes_afp_def::option_t%3DEF5FD4002B.postscript

  public:

     OmHandler omHandler;
    //## Constructors (specified)
      //## Operation: aes_afp_def%DC9B98D5FEED; C++
      //	Constructor.
      aes_afp_def (int argc, char** argv);

    //## Destructor (generated)
      virtual ~aes_afp_def();


    //## Other Operations (specified)
      //## Operation: parse%B89B8F59FEED; C++
      //	Parses the command string.
      bool parse ();

      //## Operation: execute%14045781FEED; C++
      //	Executes the command.
      unsigned int execute ();

      //## Operation: usage%46C48380FEED; C++
      //	Prints usage message.
      void usage ();

    // Additional Public Declarations
      //## begin aes_afp_def%3DEF5FD303AF.public preserve=yes
      //## end aes_afp_def%3DEF5FD303AF.public

	  // uabmha, CNI 1135, fix for default startup sequence number
	  bool startupSequenceNumberSetByOption();
	void extractClassNmFromRdn( const string myObjRdn,string &myClassName);

    // Additional Protected Declarations
      //## begin aes_afp_def%3DEF5FD303AF.protected preserve=yes
      //## end aes_afp_def%3DEF5FD303AF.protected


    // Additional Private Declarations
      //## begin aes_afp_def%3DEF5FD303AF.private preserve=yes
      //## end aes_afp_def%3DEF5FD303AF.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: transferQueue%3DEF5FD40035
      //	Transfer queue name.
      //## begin aes_afp_def::transferQueue%3DEF5FD40035.attr preserve=no  private: std::string {V} ""
      std::string transferQueue_;
      //## end aes_afp_def::transferQueue%3DEF5FD40035.attr

      //## Attribute: destinationSet%3DEF5FD4003F
      //	Destination set name.
      //## begin aes_afp_def::destinationSet%3DEF5FD4003F.attr preserve=no  private: std::string {V} ""
      std::string destinationSet_;
      //## end aes_afp_def::destinationSet%3DEF5FD4003F.attr
      std::string initdestinationSet_;
      std::string respdestinationSet_;
      //## Attribute: remBeforeString%3DEF5FD40043
      //	Remove before string.
      //## begin aes_afp_def::remBeforeString%3DEF5FD40043.attr preserve=no  private: std::string {V} ""
      std::string remBeforeString_;
      //## end aes_afp_def::remBeforeString%3DEF5FD40043.attr

      //## Attribute: defaultStatus%3DEF5FD40049
      //	Default state for transfer queue.
      //## begin aes_afp_def::defaultStatus%3DEF5FD40049.attr preserve=no  private: AES_GCC_Filestates {V} AES_FSREADY
      AES_GCC_Filestates defaultStatus_;
     // aes_afp_tqstatus defaultStatus_;
      //## end aes_afp_def::defaultStatus%3DEF5FD40049.attr

      //## Attribute: removeDelay%3DEF5FD4004D
      //	Remove delay time.
      //## begin aes_afp_def::removeDelay%3DEF5FD4004D.attr preserve=no  private: unsigned int {V} DEFAULT_REMOVEDELAY_
      unsigned int removeDelay_;
      //## end aes_afp_def::removeDelay%3DEF5FD4004D.attr

      //## Attribute: removeBefore%3DEF5FD40051
      //	True if remove delay counter will start when a file is
      //	reported.
      //## begin aes_afp_def::removeBefore%3DEF5FD40051.attr preserve=no  private: bool {V} false
      bool removeBefore_;
      //## end aes_afp_def::removeBefore%3DEF5FD40051.attr

      //## Attribute: sendRetries%3DEF5FD40055
      //	Send retries value.
      //## begin aes_afp_def::sendRetries%3DEF5FD40055.attr preserve=no  private: int {V} DEFAULT_NROFRETRIES_
      int sendRetries_;
      //## end aes_afp_def::sendRetries%3DEF5FD40055.attr

      //## Attribute: sendRetriesDelay%3DEF5FD40059
      //	Delay between retries.
      //## begin aes_afp_def::sendRetriesDelay%3DEF5FD40059.attr preserve=no  private: int {V} DEFAULT_RETRYTIME_
      int sendRetriesDelay_;
      //## end aes_afp_def::sendRetriesDelay%3DEF5FD40059.attr

      //## Attribute: isADirectory%3DEF5FD4005D
      //	True if it is a directory.
      //## begin aes_afp_def::isADirectory%3DEF5FD4005D.attr preserve=no  private: bool {V} false
      bool isADirectory_;
      //## end aes_afp_def::isADirectory%3DEF5FD4005D.attr

      //## Attribute: sourceDirectory%3DEF5FD40061
      //	Name of source directory.
      //## begin aes_afp_def::sourceDirectory%3DEF5FD40061.attr preserve=no  private: std::string {V} ""
      std::string sourceDirectory_;
      //## end aes_afp_def::sourceDirectory%3DEF5FD40061.attr

      //## Attribute: template%3DEF5FD4006A
      //	Template for renaming files.
      //## begin aes_afp_def::template%3DEF5FD4006A.attr preserve=no  private: std::string {V} 
      std::string template_;
      //## end aes_afp_def::template%3DEF5FD4006A.attr

      //## Attribute: rename%3DEF5FD4006F
      //	Rename type, local, remote or none.
      //## begin aes_afp_def::rename%3DEF5FD4006F.attr preserve=no  private: AES_AFP_Renametypes {V} AES_NONE
      AES_AFP_Renametypes rename_;
      //## end aes_afp_def::rename%3DEF5FD4006F.attr

      //## Attribute: outStream%3DEF5FD40074
      //	Answer stream for command.
      //## begin aes_afp_def::outStream%3DEF5FD40074.attr preserve=no  private: std::strstream {V} 
      std::stringstream outStream_;
      //## end aes_afp_def::outStream%3DEF5FD40074.attr

      //## Attribute: userGroup%3DF74E540304
      //	Used user group.
      //## begin aes_afp_def::userGroup%3DF74E540304.attr preserve=no  private: std::string {U} ""
      std::string userGroup_;
      //## end aes_afp_def::userGroup%3DF74E540304.attr

      //## Attribute: nameTag%3EA932790154
      //## begin aes_afp_def::nameTag%3EA932790154.attr preserve=no  private: std::string {U} ""
      std::string nameTag_;
      //## end aes_afp_def::nameTag%3EA932790154.attr

      //## 
      //	Startup sequence number / rollover value.
      //##  uabmha: CNI 1135, added for -k option
      long startupSequenceNumber_;
      //## 

      bool changedefstatus_;
      bool changeRemoveDelay_;
      bool changeRemoveBefore_;
      bool changetemplate_;
      bool checkNameTag_;
      bool changeSendRetries_;
      bool changeSendRetriesDelay_;
      // Additional Implementation Declarations
      //## begin aes_afp_def%3DEF5FD303AF.implementation preserve=yes
      //## end aes_afp_def%3DEF5FD303AF.implementation

};

//## begin aes_afp_def%3DEF5FD303AF.postscript preserve=yes
//## end aes_afp_def%3DEF5FD303AF.postscript

// Class aes_afp_def 

//## begin module%3DEF5FD30318.epilog preserve=yes
//## end module%3DEF5FD30318.epilog


#endif
