//******************************************************************************
//
// .NAME
//      aes_cdh_common.h
// .LIBRARY 3C++
// .PAGENAME BlockPairMapper
// .HEADER  AES
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE blockpairindex.h

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
//      <General description of the class>

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.


// AUTHOR
//      2013-09-28 by xshipap 

#ifndef AES_CDH_COMMON_H
#define AES_CDH_COMMON_H

#include <ace/Singleton.h>
#include <ace/Synch.h>
#include <ace/Thread_Mutex.h>
#include <string>



class AES_CDH_Common_Paths
{
private:
     std::string m_CDH_DataPath;
     std::string m_CDH_LogPath;
     ACE_Thread_Mutex m_CDH_DataPathMutex;
     ACE_Thread_Mutex m_CDH_LogPathMutex;

     AES_CDH_Common_Paths();

     ~AES_CDH_Common_Paths();


     bool setCDHDataPath(  ) ;
     bool setCDHLogPath(  ) ;


public:

     friend class ACE_Singleton<AES_CDH_Common_Paths, ACE_Recursive_Thread_Mutex>;
     bool  getCDHLogPath( std::string &logPath ) ;
     bool  getCDHDataPath( std::string &dataPath ) ;
     bool isRemoteHostValid(char* m_HostAddress);

};

//IPv6_feature:: newly introduced class AES_CDH_Common_Util common utilities like IP address validation
class AES_CDH_Common_Util
{
public:
	enum {
		VALID_ADDRESS = 0,
		INVALID_IPV4_IPV6 = 1,
		INVALID_IPV4_ADDRESS = 2,
		INVALID_IPV6_ADDRESS = 3,
		INVALID_IPV6_NATIVE = 4,
		INVALID_IPV4_MAPPED_IPV6_ADDRESS = 5,
		INVALID_UNABLE_TO_FETCH_NODE_TYPE = 6
	};
	static int isIPAddressValid(const char *p_IPAddress);
	static bool isIPv4AddressValid(const char *p_IPAddress);
	static bool isIPv6AddressValid(const char *p_IPAddress);
	static bool isIPv6Address(const std::string p_IPAddress);
private:
	AES_CDH_Common_Util();
	~AES_CDH_Common_Util();
};

typedef ACE_Singleton<AES_CDH_Common_Paths,
ACE_Recursive_Thread_Mutex> AES_CDH_Paths;

#endif
