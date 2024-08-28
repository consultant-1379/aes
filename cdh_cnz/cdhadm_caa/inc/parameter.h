
//******************************************************************************
// 
// .NAME
//  	Parameter - Parameter Handler
// .LIBRARY 3C++
// .PAGENAME Parameter
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE parameter.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	A common interface within CDH for using parameters in PHA.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0506

// AUTHOR 
// 	1999-10-18 by I/LD Gunnar Andersson

// .LINKAGE
//	-

// .SEE ALSO 
// 	-

//******************************************************************************

#ifndef PARAMETER_H 
#define PARAMETER_H

// GCC parameters

const char AES_CDH_AesDataDirectory[] = "aesapdata";  // AES data directory
const char AES_CDH_AesLogsDirectory[] = "aesaplogs";  // AES logs directory

const char AES_CDH_RootDirectory[] = "cdh";  // CDH root directory
const char AES_AFP_RootDirectory[] = "afp";  // AFP root directory

const char AES_CDH_FtpDirectory[] = "ftp";  // Ftp directory

//const char AES_CDH_BGW_RPC_ProgramNumber[] = "AES_CDH_BGW_RPC_ProgramNumber";

//const char AES_CDH_BGW_RPC_Timeout[] = "AES_CDH_BGW_RPC_Timeout";
const std::string AES_DATA_PATH = "/data_transfer/destinations/";

struct string127
{
	char str[128];
};


#endif // PARAMETER_H
