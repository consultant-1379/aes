//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%38856B0100F6.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%38856B0100F6.cm

//## begin module%38856B0100F6.cp preserve=no
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
//	 190 89-CXA 110 0109
//
//	AUTHOR
//	 2001-05-17 UAB/S/AU QABDAPA
//
//	REVISION
//	 PA1 2001-05-17 QABDAPA First rev, name changed
//	 when moved from FMS
//
//	LINKAGE
//
//	SEE ALSO
//## end module%38856B0100F6.cp

//## Module: aes_afp_divide_r1%38856B0100F6; Package specification
//## Subsystem: AFP::afpapilib_cxa::inc%387C52560099
//## Source file: Z:\ntaes\afp\afpapilib_cxa\inc\aes_afp_divide_r1.h

#ifndef aes_afp_divide_r1_h
#define aes_afp_divide_r1_h 1

//## begin module%38856B0100F6.additionalIncludes preserve=no
//## end module%38856B0100F6.additionalIncludes

//## begin module%38856B0100F6.includes preserve=yes
//## end module%38856B0100F6.includes

//## begin module%38856B0100F6.declarations preserve=no
//## end module%38856B0100F6.declarations

//## begin module%38856B0100F6.additionalDeclarations preserve=yes
//## end module%38856B0100F6.additionalDeclarations


//## begin aes_afp_divide_r1%388568D402E0.preface preserve=yes
//## end aes_afp_divide_r1%388568D402E0.preface

//## Class: aes_afp_divide_r1%388568D402E0
//	This class holds the dividers used in listing of files.
//	Trace points: no
//## Category: afpapilib_cxa (CXA 110 0109)%387C517E0392
//## Subsystem: AFP::afpapilib_cxa::inc%387C52560099
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef enum
    {
         PROCORDERSTART = 110,
         SENDITEMSTART = 111,
         ENDOFSTREAM = 112
     } aes_afp_divide_r1;

//## begin aes_afp_divide_r1%388568D402E0.postscript preserve=yes
//## end aes_afp_divide_r1%388568D402E0.postscript

//## begin module%38856B0100F6.epilog preserve=yes
//## end module%38856B0100F6.epilog


#endif
