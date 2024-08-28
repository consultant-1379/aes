//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3B0B7A690331.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3B0B7A690331.cm

//## begin module%3B0B7A690331.cp preserve=no
//	INCLUDE aes_afp_apimsg.h
//
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
//## end module%3B0B7A690331.cp

//## Module: aes_afp_apimsg%3B0B7A690331; Package body
//## Subsystem: AFP::afpapi_caa::src%37DF37FE018C
//## Source file: Z:\ntaes\afp\afpapi_caa\src\aes_afp_apimsg.cpp

//## begin module%3B0B7A690331.additionalIncludes preserve=no
//## end module%3B0B7A690331.additionalIncludes

//## begin module%3B0B7A690331.includes preserve=yes
//## end module%3B0B7A690331.includes

#include "aes_afp_apimsg.h"
//## begin module%3B0B7A690331.declarations preserve=no
//## end module%3B0B7A690331.declarations

//## begin module%3B0B7A690331.additionalDeclarations preserve=yes
//## end module%3B0B7A690331.additionalDeclarations


// Class aes_afp_apimsg 

aes_afp_apimsg::aes_afp_apimsg():msg(UNKNOWN)
  //## begin aes_afp_apimsg::aes_afp_apimsg%3B0B52D4002E_const.hasinit preserve=no
  //## end aes_afp_apimsg::aes_afp_apimsg%3B0B52D4002E_const.hasinit
  //## begin aes_afp_apimsg::aes_afp_apimsg%3B0B52D4002E_const.initialization preserve=yes
  //## end aes_afp_apimsg::aes_afp_apimsg%3B0B52D4002E_const.initialization
{
  //## begin aes_afp_apimsg::aes_afp_apimsg%3B0B52D4002E_const.body preserve=yes
	errorCode = AES_NOERRORCODE;
  //## end aes_afp_apimsg::aes_afp_apimsg%3B0B52D4002E_const.body
}


aes_afp_apimsg::~aes_afp_apimsg()
{
  //## begin aes_afp_apimsg::~aes_afp_apimsg%3B0B52D4002E_dest.body preserve=yes
  //## end aes_afp_apimsg::~aes_afp_apimsg%3B0B52D4002E_dest.body
}



//## Other Operations (implementation)
//## Operation: setMsgString%3B0B533B0162; C++
//## Semantics:
//	---------------------------------------------------------
//	       setMsgString()
//	---------------------------------------------------------
void aes_afp_apimsg::setMsgString (std::string& messageString)
{
  //## begin aes_afp_apimsg::setMsgString%3B0B533B0162.body preserve=yes
	msgString = messageString;
  //## end aes_afp_apimsg::setMsgString%3B0B533B0162.body
}

//## Operation: getMsgString%3B0B53A70081
//## Semantics:
//	---------------------------------------------------------
//	       getMsgString()
//	---------------------------------------------------------
std::string aes_afp_apimsg::getMsgString ()
{
  //## begin aes_afp_apimsg::getMsgString%3B0B53A70081.body preserve=yes
	return msgString;
  //## end aes_afp_apimsg::getMsgString%3B0B53A70081.body
}

//## Operation: setErrorCode%3B0B53ED00FA; C++
//## Semantics:
//	---------------------------------------------------------
//	       setErrorCode()
//	---------------------------------------------------------
void aes_afp_apimsg::setErrorCode (AES_GCC_Errorcodes code)
{
  //## begin aes_afp_apimsg::setErrorCode%3B0B53ED00FA.body preserve=yes
	errorCode = code;
  //## end aes_afp_apimsg::setErrorCode%3B0B53ED00FA.body
}

//## Operation: getErrorCode%3B0B544001B8
//## Semantics:
//	---------------------------------------------------------
//	       getErrorCode()
//	---------------------------------------------------------
AES_GCC_Errorcodes aes_afp_apimsg::getErrorCode ()
{
  //## begin aes_afp_apimsg::getErrorCode%3B0B544001B8.body preserve=yes
	return errorCode;
  //## end aes_afp_apimsg::getErrorCode%3B0B544001B8.body
}

//## Operation: setEventCode%3B0B705A00D6; C++
//## Semantics:
//	---------------------------------------------------------
//	       setEvenCode()
//	---------------------------------------------------------
void aes_afp_apimsg::setEventCode (AES_GCC_Eventcodes code)
{
  //## begin aes_afp_apimsg::setEventCode%3B0B705A00D6.body preserve=yes
	eventCode = code;
  //## end aes_afp_apimsg::setEventCode%3B0B705A00D6.body
}

//## Operation: getEventCode%3B0B7064031F; C++
//## Semantics:
//	---------------------------------------------------------
//	       getEventCode()
//	---------------------------------------------------------
AES_GCC_Eventcodes aes_afp_apimsg::getEventCode ()
{
  //## begin aes_afp_apimsg::getEventCode%3B0B7064031F.body preserve=yes
	return eventCode;
  //## end aes_afp_apimsg::getEventCode%3B0B7064031F.body
}

//## Operation: setMessage%3B0BBA0B0082; C++
//## Semantics:
//	---------------------------------------------------------
//	       setMessage()
//	---------------------------------------------------------
void aes_afp_apimsg::setMessage (aes_afp_msgtypes message)
{
  //## begin aes_afp_apimsg::setMessage%3B0BBA0B0082.body preserve=yes
	msg = message;
  //## end aes_afp_apimsg::setMessage%3B0BBA0B0082.body
}

//## Operation: getMessage%3B0BBA210387; C++
//## Semantics:
//	---------------------------------------------------------
//	       getMessage()
//	---------------------------------------------------------
aes_afp_msgtypes aes_afp_apimsg::getMessage ()
{
  //## begin aes_afp_apimsg::getMessage%3B0BBA210387.body preserve=yes
	return msg;
  //## end aes_afp_apimsg::getMessage%3B0BBA210387.body
}

// Additional Declarations
  //## begin aes_afp_apimsg%3B0B52D4002E.declarations preserve=yes
  //## end aes_afp_apimsg%3B0B52D4002E.declarations

//## begin module%3B0B7A690331.epilog preserve=yes
//## end module%3B0B7A690331.epilog
