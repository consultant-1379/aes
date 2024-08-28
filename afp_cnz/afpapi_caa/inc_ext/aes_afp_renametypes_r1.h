//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3B1C9A7F0063.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3B1C9A7F0063.cm

//## begin module%3B1C9A7F0063.cp preserve=no
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
//	 190 89-CXA 110 0756
//
//	AUTHOR
//	 2001-06-05 UAB/S/AU QABDAPA
//
//	REVISION
//	 PA1 2001-06-05 QABDAPA First rev.
//
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3B1C9A7F0063.cp

//## Module: aes_afp_renametypes_r1%3B1C9A7F0063; Package specification
//## Subsystem: AFP::afpapilib_cxa::inc%387C52560099
//## Source file: Z:\ntaes\afp\afpapilib_cxa\inc\aes_afp_renametypes_r1.h

#ifndef aes_afp_renametypes_r1_h
#define aes_afp_renametypes_r1_h 1

//## begin module%3B1C9A7F0063.additionalIncludes preserve=no
//## end module%3B1C9A7F0063.additionalIncludes

//## begin module%3B1C9A7F0063.includes preserve=yes
//## end module%3B1C9A7F0063.includes

//## begin module%3B1C9A7F0063.declarations preserve=no
//## end module%3B1C9A7F0063.declarations

//## begin module%3B1C9A7F0063.additionalDeclarations preserve=yes
//## end module%3B1C9A7F0063.additionalDeclarations


//## begin aes_afp_renametypes_r1%3B1C9A180083.preface preserve=yes
//## end aes_afp_renametypes_r1%3B1C9A180083.preface

//## Class: aes_afp_renametypes_r1%3B1C9A180083
//## Category: afpapilib_cxa (CXA 110 0109)%387C517E0392
//## Subsystem: AFP::afpapilib_cxa::inc%387C52560099
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef enum
   {
       AES_LOCAL = 0,
       AES_REMOTE = 1,
       AES_NONE = 2,
			 AES_RTUNKNOWN = 255
   } aes_afp_renametypes_r1;

//## begin aes_afp_renametypes_r1%3B1C9A180083.postscript preserve=yes
//## end aes_afp_renametypes_r1%3B1C9A180083.postscript

//## begin module%3B1C9A7F0063.epilog preserve=yes
//## end module%3B1C9A7F0063.epilog


#endif
