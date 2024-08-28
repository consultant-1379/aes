//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD6006A.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD6006A.cm

//## begin module%3DEF5FD6006A.cp preserve=no
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
//	   2003-02-03  UAB/UKB/AU DAPA
//
//	REVISION
//	   A 2003-02-03 DAPA
//	   B 2003-05-09 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3DEF5FD6006A.cp

//## Module: aes_afp_printoutdata%3DEF5FD6006A; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_printoutdata.h

#ifndef aes_afp_printoutdata_h
#define aes_afp_printoutdata_h 1

//## begin module%3DEF5FD6006A.additionalIncludes preserve=no
//## end module%3DEF5FD6006A.additionalIncludes

//## begin module%3DEF5FD6006A.includes preserve=yes
//## end module%3DEF5FD6006A.includes

#include <map>
#include <string>
//## begin module%3DEF5FD6006A.declarations preserve=no
//## end module%3DEF5FD6006A.declarations

//## begin module%3DEF5FD6006A.additionalDeclarations preserve=yes
//## end module%3DEF5FD6006A.additionalDeclarations


//## begin Filedata%3DEF5FD60114.preface preserve=yes
//## end Filedata%3DEF5FD60114.preface

//## Class: Filedata%3DEF5FD60114
//	This class is responsible for list of files in afpls
//	command.
//## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3E3F621403A4;string { -> }

class Filedata 
{
  //## begin Filedata%3DEF5FD60114.initialDeclarations preserve=yes
  //## end Filedata%3DEF5FD60114.initialDeclarations

  public:
    //## Constructors (generated)
      Filedata();

    //## Destructor (generated)
      virtual ~Filedata();

    // Data Members for Class Attributes

      //## Attribute: filename%3DEF5FD60115
      //	Name of file.
      //## begin Filedata::filename%3DEF5FD60115.attr preserve=no  public: std::string {V} ""
      std::string filename_;
      //## end Filedata::filename%3DEF5FD60115.attr

      //## Attribute: destinationSet%3DEF5FD60119
      //	Name of destination set.
      //## begin Filedata::destinationSet%3DEF5FD60119.attr preserve=no  public: std::string {V} ""
      std::string destinationSet_;
      //## end Filedata::destinationSet%3DEF5FD60119.attr

      //## Attribute: status%3DEF5FD6011E
      //	Status for file.
      //## begin Filedata::status%3DEF5FD6011E.attr preserve=no  public: std::string {V} ""
      std::string status_;
      //## end Filedata::status%3DEF5FD6011E.attr

      //## Attribute: isDirectory%3DEF5FD60122
      //	True is filename is a directory.
      //## begin Filedata::isDirectory%3DEF5FD60122.attr preserve=no  public: std::string {V} ""
      std::string isDirectory_;
      //## end Filedata::isDirectory%3DEF5FD60122.attr

      //## Attribute: removeTimer%3DEF5FD60126
      //	Remove delay.
      //## begin Filedata::removeTimer%3DEF5FD60126.attr preserve=no  public: int {V} 0
      int removeTimer_;
      //## end Filedata::removeTimer%3DEF5FD60126.attr

    // Additional Public Declarations
      //## begin Filedata%3DEF5FD60114.public preserve=yes
      //## end Filedata%3DEF5FD60114.public

  protected:
    // Additional Protected Declarations
      //## begin Filedata%3DEF5FD60114.protected preserve=yes
      //## end Filedata%3DEF5FD60114.protected

  private:
    // Additional Private Declarations
      //## begin Filedata%3DEF5FD60114.private preserve=yes
      //## end Filedata%3DEF5FD60114.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin Filedata%3DEF5FD60114.implementation preserve=yes
      //## end Filedata%3DEF5FD60114.implementation

};

//## begin Filedata%3DEF5FD60114.postscript preserve=yes
//## end Filedata%3DEF5FD60114.postscript

//## begin FiledataMap%3E3F65C80375.preface preserve=yes
//## end FiledataMap%3E3F65C80375.preface

//## Class: FiledataMap%3E3F65C80375; Instantiated Class
//	This class is the filedata map.
//## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3E3F6656026B;Filedata { -> }
//## Uses: <unnamed>%3E3F665C0093;string { -> }

typedef std::map< std::string , Filedata* , std::less<std::string>  > FiledataMap;

//## begin FiledataMap%3E3F65C80375.postscript preserve=yes
//## end FiledataMap%3E3F65C80375.postscript

//## begin Tqdata%3DEF5FD6012A.preface preserve=yes
//## end Tqdata%3DEF5FD6012A.preface

//## Class: Tqdata%3DEF5FD6012A
//	Class for holding printout data.
//## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3E3F635E01B4;string { -> }

class Tqdata 
{
  //## begin Tqdata%3DEF5FD6012A.initialDeclarations preserve=yes
  //## end Tqdata%3DEF5FD6012A.initialDeclarations

  public:
    //## Constructors (generated)
      Tqdata();

    //## Destructor (generated)
      virtual ~Tqdata();

    // Data Members for Class Attributes

      //## Attribute: destinationSet%3DEF5FD6012B
      //	Name of destination set.
      //## begin Tqdata::destinationSet%3DEF5FD6012B.attr preserve=no  public: std::string {V} ""
      std::string destinationSet_;
      //## end Tqdata::destinationSet%3DEF5FD6012B.attr

      //## Attribute: transferQueue%3DEF5FD6012F
      //	Name of transfer queue.
      //## begin Tqdata::transferQueue%3DEF5FD6012F.attr preserve=no  public: std::string {V} ""
      std::string transferQueue_;
      //## end Tqdata::transferQueue%3DEF5FD6012F.attr

      //## Attribute: status%3DEF5FD60133
      //	Status for transfer queue.
      //## begin Tqdata::status%3DEF5FD60133.attr preserve=no  public: std::string {V} ""
      std::string status_;
      //## end Tqdata::status%3DEF5FD60133.attr

      //## Attribute: removeDelay%3DEF5FD60137
      //	Remove delay.
      //## begin Tqdata::removeDelay%3DEF5FD60137.attr preserve=no  public: int {V} 0
      int removeDelay_;
      //## end Tqdata::removeDelay%3DEF5FD60137.attr

      //## Attribute: manual%3DEF5FD6013B
      //	True if transfer queue is manual initated.
      //## begin Tqdata::manual%3DEF5FD6013B.attr preserve=no  public: bool {V} false
      bool manual_;
      //## end Tqdata::manual%3DEF5FD6013B.attr

      //## Attribute: renameTemplate%3DEF5FD6013F
      //	Template for renaming files.
      //## begin Tqdata::renameTemplate%3DEF5FD6013F.attr preserve=no  public: std::string {V} ""
      std::string renameTemplate_;
      //## end Tqdata::renameTemplate%3DEF5FD6013F.attr

      //## Attribute: sourceDirectory%3DEF5FD60143
      //	Applications source directory.
      //## begin Tqdata::sourceDirectory%3DEF5FD60143.attr preserve=no  public: std::string {V} ""
      std::string sourceDirectory_;
      //## end Tqdata::sourceDirectory%3DEF5FD60143.attr

      //## Attribute: userGroup%3E2C0FA50219
      //	User group for transfer queue.
      //## begin Tqdata::userGroup%3E2C0FA50219.attr preserve=no  public: std::string {U} ""
      std::string userGroup_;
      //## end Tqdata::userGroup%3E2C0FA50219.attr

      //## Attribute: nameTag%3EBB944D01FB
      //## begin Tqdata::nameTag%3EBB944D01FB.attr preserve=no  public: std::string {U} ""
      std::string nameTag_;
      //## end Tqdata::nameTag%3EBB944D01FB.attr

      //## Attribute: startupSequenceNumber
      //## begin
      long startupSequenceNumber_;
      //## end

      //## Attribute: renameStringType%3E3F842F0290
      //	-
      //## begin Tqdata::renameStringType%3E3F842F0290.attr preserve=no  private: std::string {U} ""
      std::string renameStringType_;
      //## end Tqdata::renameStringType%3E3F842F0290.attr

    // Data Members for Associations

      //## Association: afpcmd_caa(CAA 109 0504)::<unnamed>%3E3F66600388
      //## Role: Tqdata::filedataMap%3E3F66620182
      //## begin Tqdata::filedataMap%3E3F66620182.role preserve=no  public: FiledataMap { -> 1UHgN}
      FiledataMap filedataMap_;
      //## end Tqdata::filedataMap%3E3F66620182.role

    // Additional Public Declarations
      //## begin Tqdata%3DEF5FD6012A.public preserve=yes
      //## end Tqdata%3DEF5FD6012A.public

  protected:
    // Additional Protected Declarations
      //## begin Tqdata%3DEF5FD6012A.protected preserve=yes
      //## end Tqdata%3DEF5FD6012A.protected

  private:
    // Additional Private Declarations
      //## begin Tqdata%3DEF5FD6012A.private preserve=yes
      //## end Tqdata%3DEF5FD6012A.private

  private: //## implementation
    // Additional Implementation Declarations
      //## begin Tqdata%3DEF5FD6012A.implementation preserve=yes
      //## end Tqdata%3DEF5FD6012A.implementation

};

//## begin Tqdata%3DEF5FD6012A.postscript preserve=yes
//## end Tqdata%3DEF5FD6012A.postscript

// Class Filedata 

inline Filedata::Filedata()
  //## begin Filedata::Filedata%3DEF5FD60114_const.hasinit preserve=no
      : filename_(""),
        destinationSet_(""),
        status_(""),
        isDirectory_(""),
        removeTimer_(0)
  //## end Filedata::Filedata%3DEF5FD60114_const.hasinit
  //## begin Filedata::Filedata%3DEF5FD60114_const.initialization preserve=yes
  //## end Filedata::Filedata%3DEF5FD60114_const.initialization
{
  //## begin Filedata::Filedata%3DEF5FD60114_const.body preserve=yes
  //## end Filedata::Filedata%3DEF5FD60114_const.body
}


inline Filedata::~Filedata()
{
  //## begin Filedata::~Filedata%3DEF5FD60114_dest.body preserve=yes
  //## end Filedata::~Filedata%3DEF5FD60114_dest.body
}


// Class Tqdata 

inline Tqdata::Tqdata()
  //## begin Tqdata::Tqdata%3DEF5FD6012A_const.hasinit preserve=no
      : destinationSet_(""),
        transferQueue_(""),
        status_(""),
        removeDelay_(0),
        manual_(false),
        renameTemplate_(""),
        sourceDirectory_(""),
        userGroup_(""),
        nameTag_(""),
        startupSequenceNumber_(0),
        renameStringType_("")

  //## end Tqdata::Tqdata%3DEF5FD6012A_const.hasinit
  //## begin Tqdata::Tqdata%3DEF5FD6012A_const.initialization preserve=yes
  //## end Tqdata::Tqdata%3DEF5FD6012A_const.initialization
{
  //## begin Tqdata::Tqdata%3DEF5FD6012A_const.body preserve=yes
  //## end Tqdata::Tqdata%3DEF5FD6012A_const.body
}


inline Tqdata::~Tqdata()
{
  //## begin Tqdata::~Tqdata%3DEF5FD6012A_dest.body preserve=yes
  //## end Tqdata::~Tqdata%3DEF5FD6012A_dest.body
}


//## begin module%3DEF5FD6006A.epilog preserve=yes
//## end module%3DEF5FD6006A.epilog


#endif
