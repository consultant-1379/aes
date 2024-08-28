//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3B0B724602D2.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3B0B724602D2.cm

//## begin module%3B0B724602D2.cp preserve=no
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2001.
//	  All rights reserved.
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
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	 190 89-CAA 109 0418
//
//	AUTHOR
//	 2001-05-23 UAB/S/AU QABDAPA
//
//	REVISION
//	 PA1 2001-05-23 QABDAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3B0B724602D2.cp

//## Module: aes_afp_apimsg%3B0B724602D2; Package specification
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Source file: Z:\ntaes\afp\afpapi_caa\inc\aes_afp_apimsg.h

#ifndef aes_afp_apimsg_h
#define aes_afp_apimsg_h 1

//## begin module%3B0B724602D2.additionalIncludes preserve=no
//## end module%3B0B724602D2.additionalIncludes

//## begin module%3B0B724602D2.includes preserve=yes
//## end module%3B0B724602D2.includes

#include "aes_afp_msgtypes.h"
#include <string>
//#include <osf/message_block.h>
#include <ace/Message_Block.h>
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_eventcodes.h>
//## begin module%3B0B724602D2.declarations preserve=no
//## end module%3B0B724602D2.declarations

//## begin module%3B0B724602D2.additionalDeclarations preserve=yes
//## end module%3B0B724602D2.additionalDeclarations


//## begin aes_afp_apimsg%3B0B52D4002E.preface preserve=yes
//## end aes_afp_apimsg%3B0B52D4002E.preface

//## Class: aes_afp_apimsg%3B0B52D4002E
//	Message string containing result of an earlier command.
//## Category: afpapi_caa (CAA 0289)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



class aes_afp_apimsg : public ACE_Message_Block  //## Inherits: <unnamed>%3B0B532C0111
{
  //## begin aes_afp_apimsg%3B0B52D4002E.initialDeclarations preserve=yes
  //## end aes_afp_apimsg%3B0B52D4002E.initialDeclarations

  public:
    //## Constructors (generated)
      aes_afp_apimsg();

    //## Destructor (generated)
      virtual ~aes_afp_apimsg();


    //## Other Operations (specified)
      //## Operation: setMsgString%3B0B533B0162; C++
      //	Set the message string in the message block.
      void setMsgString (std::string& messageString);

      //## Operation: getMsgString%3B0B53A70081
      //	Retrieves the message string from a message block.
      std::string getMsgString ();

      //## Operation: setErrorCode%3B0B53ED00FA; C++
      //	Sets the error code in a message block.
      void setErrorCode (AES_GCC_Errorcodes code);

      //## Operation: getErrorCode%3B0B544001B8
      //	Retrieves the error code from a message block
      AES_GCC_Errorcodes getErrorCode ();

      //## Operation: setEventCode%3B0B705A00D6; C++
      //	Sets the event code.
      void setEventCode (AES_GCC_Eventcodes code);

      //## Operation: getEventCode%3B0B7064031F; C++
      AES_GCC_Eventcodes getEventCode ();

      //## Operation: setMessage%3B0BBA0B0082; C++
      void setMessage (aes_afp_msgtypes message);

      //## Operation: getMessage%3B0BBA210387; C++
      aes_afp_msgtypes getMessage ();

    // Additional Public Declarations
      //## begin aes_afp_apimsg%3B0B52D4002E.public preserve=yes
      //## end aes_afp_apimsg%3B0B52D4002E.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_apimsg%3B0B52D4002E.protected preserve=yes
      //## end aes_afp_apimsg%3B0B52D4002E.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_apimsg%3B0B52D4002E.private preserve=yes
      //## end aes_afp_apimsg%3B0B52D4002E.private

  private: //## implementation
    // Data Members for Associations

      //## Association: afpapi_caa (CAA 0289)::<unnamed>%3B0B552A03C7
      //## Role: aes_afp_apimsg::msgString%3B0B552B03E6
      //## begin aes_afp_apimsg::msgString%3B0B552B03E6.role preserve=no  private: string { -> 1UgN}
      std::string msgString;
      //## end aes_afp_apimsg::msgString%3B0B552B03E6.role

      //## Association: afpapi_caa (CAA 0289)::<unnamed>%3B0B556B012B
      //## Role: aes_afp_apimsg::errorCode%3B0B556C032C
      //## begin aes_afp_apimsg::errorCode%3B0B556C032C.role preserve=no  private: AES_GCC_Errorcodes { -> 1UgN}
      AES_GCC_Errorcodes errorCode;
      //## end aes_afp_apimsg::errorCode%3B0B556C032C.role

      //## Association: afpapi_caa (CAA 0289)::<unnamed>%3B0B702B001A
      //## Role: aes_afp_apimsg::eventCode%3B0B702C02F6
      //## begin aes_afp_apimsg::eventCode%3B0B702C02F6.role preserve=no  private: AES_GCC_Eventcodes { -> 1UgN}
      AES_GCC_Eventcodes eventCode;
      //## end aes_afp_apimsg::eventCode%3B0B702C02F6.role

      //## Association: afpapi_caa (CAA 0289)::<unnamed>%3B0BBD4F032D
      //## Role: aes_afp_apimsg::msg%3B0BBD53035B
      //## begin aes_afp_apimsg::msg%3B0BBD53035B.role preserve=no  private: aes_afp_msgtypes { -> 1UgN}
      aes_afp_msgtypes msg;
      //## end aes_afp_apimsg::msg%3B0BBD53035B.role

    // Additional Implementation Declarations
      //## begin aes_afp_apimsg%3B0B52D4002E.implementation preserve=yes
      //## end aes_afp_apimsg%3B0B52D4002E.implementation

};

//## begin aes_afp_apimsg%3B0B52D4002E.postscript preserve=yes
//## end aes_afp_apimsg%3B0B52D4002E.postscript

// Class aes_afp_apimsg 

//## begin module%3B0B724602D2.epilog preserve=yes
//## end module%3B0B724602D2.epilog


#endif
