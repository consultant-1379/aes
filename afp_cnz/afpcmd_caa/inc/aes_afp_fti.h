//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3DEF5FD402DE.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3DEF5FD402DE.cm

//## begin module%3DEF5FD402DE.cp preserve=no
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
//## end module%3DEF5FD402DE.cp

//## Module: aes_afp_fti%3DEF5FD402DE; Package specification
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Source file: Z:\ntaes\afp\afpcmd_caa\inc\aes_afp_fti.h

#ifndef aes_afp_fti_h
#define aes_afp_fti_h 1

//## begin module%3DEF5FD402DE.additionalIncludes preserve=no
//## end module%3DEF5FD402DE.additionalIncludes

//## begin module%3DEF5FD402DE.includes preserve=yes
//## end module%3DEF5FD402DE.includes

#include "aes_afp_command.h"
#include <string>
#include <strstream>
//## begin module%3DEF5FD402DE.declarations preserve=no
//## end module%3DEF5FD402DE.declarations

//## begin module%3DEF5FD402DE.additionalDeclarations preserve=yes
//## end module%3DEF5FD402DE.additionalDeclarations


//## begin aes_afp_fti%3DEF5FD40374.preface preserve=yes
//## end aes_afp_fti%3DEF5FD40374.preface

//## Class: aes_afp_fti%3DEF5FD40374
//	This class is responsible for command afpfti.
//## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
//## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3DEF5FD403C7; { -> }
//## Uses: <unnamed>%3DEF5FD403CB; { -> }
//## Uses: <unnamed>%3DF834CD0281;strstream { -> }
//## Uses: <unnamed>%3DF834D202D8;string { -> }

class aes_afp_fti : public aes_afp_command  //## Inherits: <unnamed>%3DEF5FD5000C
{
  //## begin aes_afp_fti%3DEF5FD40374.initialDeclarations preserve=yes
  //## end aes_afp_fti%3DEF5FD40374.initialDeclarations

  private:
    //## begin aes_afp_fti::option_t%3DEF5FD403CE.preface preserve=yes
    //## end aes_afp_fti::option_t%3DEF5FD403CE.preface

    //## Class: option_t%3DEF5FD403CE; private
    //## Category: afpcmd_caa(CAA 109 0504)%3B023EDD00FC
    //## Subsystem: AFP::afpcmd_caa::inc%3DEF664600F6
    //## Persistence: Transient
    //## Cardinality/Multiplicity: n



    typedef enum option_t
       {
          FAILEDFILES, 
          FILENAME, 
          NEWDESTINATIONSET, 
          ISDIRECTORY,
          USERGROUP
       } option_t;

    //## begin aes_afp_fti::option_t%3DEF5FD403CE.postscript preserve=yes
    //## end aes_afp_fti::option_t%3DEF5FD403CE.postscript

  public:
    //## Constructors (specified)
      //## Operation: aes_afp_fti%C5B472EEFEED; C++
      //	Constructor.
      aes_afp_fti (int argc, char** argv);

    //## Destructor (generated)
      virtual ~aes_afp_fti();


    //## Other Operations (specified)
      //## Operation: parse%D51CCB9AFEED; C++
      //	Parse the supplied command options.
      bool parse ();

      //## Operation: execute%4F2D1D62FEED; C++
      //	Execute the command.
      unsigned int execute ();

      //## Operation: usage%7E1C6DBCFEED; C++
      //	Print usage message.
      void usage ();

    // Additional Public Declarations
      //## begin aes_afp_fti%3DEF5FD40374.public preserve=yes
      //## end aes_afp_fti%3DEF5FD40374.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_fti%3DEF5FD40374.protected preserve=yes
      //## end aes_afp_fti%3DEF5FD40374.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_fti%3DEF5FD40374.private preserve=yes
      //## end aes_afp_fti%3DEF5FD40374.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: filename%3DEF5FD403D8
      //	Name of the file or directory that is to be transferred.
      //## begin aes_afp_fti::filename%3DEF5FD403D8.attr preserve=no  private: std::string {V} ""
      std::string filename_;
      //## end aes_afp_fti::filename%3DEF5FD403D8.attr

      //## Attribute: destinationSet%3DEF5FD403DC
      //	Name of a destination set that is a part of a transfer
      //	queue.
      //## begin aes_afp_fti::destinationSet%3DEF5FD403DC.attr preserve=no  private: std::string {V} ""
      std::string destinationSet_;
      //## end aes_afp_fti::destinationSet%3DEF5FD403DC.attr

      //## Attribute: newDestinationSet%3DEF5FD403E0
      //	Name of a new destination set when transferring failed
      //	files or directories.
      //## begin aes_afp_fti::newDestinationSet%3DEF5FD403E0.attr preserve=no  private: std::string {V} ""
      std::string newDestinationSet_;
      //## end aes_afp_fti::newDestinationSet%3DEF5FD403E0.attr

      //## Attribute: transferQueue%3DEF5FD403E4
      //## begin aes_afp_fti::transferQueue%3DEF5FD403E4.attr preserve=no  private: std::string {V} ""
      std::string transferQueue_;
      //## end aes_afp_fti::transferQueue%3DEF5FD403E4.attr

      //## Attribute: sourceDirectory%3DEF5FD403E8
      //	Name of the directory where the file or directory is
      //	located.
      //## begin aes_afp_fti::sourceDirectory%3DEF5FD403E8.attr preserve=no  private: std::string {V} ""
      std::string sourceDirectory_;
      //## end aes_afp_fti::sourceDirectory%3DEF5FD403E8.attr

      //## Attribute: SENDRETRIES%3DEF5FD403F0
      //	Number of send retries for the file or directory in case
      //	of a failure.
      //## begin aes_afp_fti::SENDRETRIES%3DEF5FD403F0.attr preserve=no  private: int {VC} 0
      const int SENDRETRIES_;
      //## end aes_afp_fti::SENDRETRIES%3DEF5FD403F0.attr

      //## Attribute: transferFailed%3DEF5FD403F4
      //	True if failed files is to be transferred.
      //## begin aes_afp_fti::transferFailed%3DEF5FD403F4.attr preserve=no  private: bool {V} false
      bool transferFailed_;
      //## end aes_afp_fti::transferFailed%3DEF5FD403F4.attr

      //## Attribute: isDirectory%3DEF5FD403F8
      //	True if filename is a directory.
      //## begin aes_afp_fti::isDirectory%3DEF5FD403F8.attr preserve=no  private: bool {V} false
      bool isDirectory_;
      //## end aes_afp_fti::isDirectory%3DEF5FD403F8.attr

      //## Attribute: SENDRETRIESDELAY%3DEF5FD50004
      //	Delay time between send retries in case of failure.
      //## begin aes_afp_fti::SENDRETRIESDELAY%3DEF5FD50004.attr preserve=no  private: int {VC} 10
      const int SENDRETRIESDELAY_;
      //## end aes_afp_fti::SENDRETRIESDELAY%3DEF5FD50004.attr

      //## Attribute: userGroup%3E4765B60008
      //	User group for transfer queue.
      //## begin aes_afp_fti::userGroup%3E4765B60008.attr preserve=no  private: std::string {U} ""
      std::string userGroup_;
      //## end aes_afp_fti::userGroup%3E4765B60008.attr

    // Additional Implementation Declarations
      //## begin aes_afp_fti%3DEF5FD40374.implementation preserve=yes
      //## end aes_afp_fti%3DEF5FD40374.implementation

};

//## begin aes_afp_fti%3DEF5FD40374.postscript preserve=yes
//## end aes_afp_fti%3DEF5FD40374.postscript

// Class aes_afp_fti 

//## begin module%3DEF5FD402DE.epilog preserve=yes
//## end module%3DEF5FD402DE.epilog


#endif
