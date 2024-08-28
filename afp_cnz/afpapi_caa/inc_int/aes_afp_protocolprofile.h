//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3E2F9D5E03C5.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3E2F9D5E03C5.cm

//## begin module%3E2F9D5E03C5.cp preserve=no
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
//	   19089-CAA 109 0505
//
//	AUTHOR
//	   2003-01-22  UAB/UKB/AU DAPA
//
//	REVISION
//	   A 2003-01-22 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3E2F9D5E03C5.cp

//## Module: aes_afp_protocolprofile%3E2F9D5E03C5; Package specification
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Source file: Z:\ntaes\afp\afpapi_caa\inc\aes_afp_protocolprofile.h

#ifndef aes_afp_protocolprofile_h
#define aes_afp_protocolprofile_h 1

//## begin module%3E2F9D5E03C5.additionalIncludes preserve=no
//## end module%3E2F9D5E03C5.additionalIncludes

//## begin module%3E2F9D5E03C5.includes preserve=yes
//## end module%3E2F9D5E03C5.includes

//## begin module%3E2F9D5E03C5.declarations preserve=no
//## end module%3E2F9D5E03C5.declarations

//## begin module%3E2F9D5E03C5.additionalDeclarations preserve=yes
//## end module%3E2F9D5E03C5.additionalDeclarations


//## begin aes_afp_protocolprofile%3E2F9CF6019F.preface preserve=yes
//## end aes_afp_protocolprofile%3E2F9CF6019F.preface

//## Class: aes_afp_protocolprofile%3E2F9CF6019F
//	Profiles for protocol class.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef enum
   {
      API = 0,
      TRANSFERQUEUE = 1,
      FILES = 2
   } aes_afp_protocolprofile;

//## begin aes_afp_protocolprofile%3E2F9CF6019F.postscript preserve=yes
//## end aes_afp_protocolprofile%3E2F9CF6019F.postscript

//## begin module%3E2F9D5E03C5.epilog preserve=yes
//## end module%3E2F9D5E03C5.epilog


#endif
