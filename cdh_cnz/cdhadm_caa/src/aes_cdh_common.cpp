//******************************************************************************
//
// NAME
//      aes_cdh_common.cpp
//
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
//  A common file which sets the paths of data and log at startup 
//  and returns tha path whenever requested. 

// DOCUMENT NO
//  190 89-CAA 109 0507

// AUTHOR
//  2013-10-28 by xshipap

//******************************************************************************

#include <event.h>
#include <string.h>
#include <ACS_APGCC_CommonLib.h>
#include <aes_cdh_common.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
#include <ACS_CS_API.h>

AES_CDH_TRACE_DEFINE(AES_CDH_Common_Util);

namespace AES_CDH {

                const char DATAPATH_KEY[] = "AES_DATA";
                const char LOGPATH_KEY[] = "AES_LOGS";
}

//GCC_TDEF(AES_CDH_Common_Path);


AES_CDH_Common_Paths::AES_CDH_Common_Paths(): m_CDH_DataPath(), m_CDH_LogPath(),m_CDH_DataPathMutex(),m_CDH_LogPathMutex()
{
  setCDHDataPath();
  setCDHLogPath();
}

AES_CDH_Common_Paths::~AES_CDH_Common_Paths()
{
}

AES_CDH_Common_Util::AES_CDH_Common_Util()
{
}

AES_CDH_Common_Util::~AES_CDH_Common_Util()
{
}

// method to extract the data path at startup and use the same in later time
bool AES_CDH_Common_Paths::setCDHDataPath(   )
{
	bool result = false;

	// Make sure that we are alone
	ACE_Guard<ACE_Thread_Mutex> guard(m_CDH_DataPathMutex);

	//GCC_TTRACE((AES_CDH_Common_Path,"(%t) %s\n", "setCDHDataPath.."));

	// Try getting the datadisk path requested until we get the buffer size right
	if( m_CDH_DataPath.empty() )
	{
		ACS_APGCC_CommonLib myAPGCCCommonLib;
		ACS_APGCC_DNFPath_ReturnTypeT getResult;
		int bufferLength = 50;
		char buffer[bufferLength];

		// Clear the array and set to 0
		ACE_OS::memset(buffer, 0, bufferLength);

		getResult = myAPGCCCommonLib.GetDataDiskPath(AES_CDH::DATAPATH_KEY, buffer, bufferLength);
		//GCC_TTRACE((AES_CDH_Common_Path,"(%t) %s%d\n", "GetDiskPath return Value..",getResult));

		if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
		{
			m_CDH_DataPath.assign(buffer);
			result = true;
			//GCC_TDEBUG((AES_CDH_Common_Path,"(%t) %s%s\n","Extracted Data Path is ", m_CDH_DataPath.c_str()));

		}
		else  if(ACS_APGCC_STRING_BUFFER_SMALL == getResult )
		{
			char buffer2[bufferLength+1];

			ACE_OS::memset(buffer2, 0, bufferLength+1);
			getResult = myAPGCCCommonLib.GetDataDiskPath(AES_CDH::DATAPATH_KEY, buffer2, bufferLength);
			//GCC_TTRACE((AES_CDH_Common_Path,"(%t) %s%d\n", "GetDiskPath return Value..",getResult));

			if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
			{
				m_CDH_DataPath.assign(buffer2);
				result = true;
				//GCC_TDEBUG((AES_CDH_Common_Path,"(%t) %s%s\n","Extracted Data Path is ", m_CDH_DataPath.c_str()));

			}

		}
		// Condition for other error codes from GetDataDiskPath. We can do much about it
		// so return false and raise an event  
		if (ACS_APGCC_DNFPATH_SUCCESS != getResult)
		{
			ACE_TCHAR problemData[216] = { 0 };
			switch(getResult)
			{
			case  ACS_APGCC_DNFPATH_FAILURE :
				ACE_OS::sprintf(problemData,"Failed to fetch the dataDiskPath for aesapdata");
				break;
			case ACS_APGCC_FAULT_LOGICAL_NAME :
				ACE_OS::sprintf(problemData,"Faulty logical name given for aesapdata in fetching data disk path");
				break;
			default:
				break;
			}
			Event::report(1003,"AES Internal Fault",problemData," ");
		}
	}
	else
	{
		result = true;
	}


	return result;
}

// method to extract the log path at startup and use the same in later time
bool  AES_CDH_Common_Paths::setCDHLogPath(  )
{
	bool result = false;

	// Make sure that we are alone
	ACE_Guard<ACE_Thread_Mutex> guard(m_CDH_LogPathMutex);

	//GCC_TINFO((AES_CDH_Common_Path,"(%t) %s\n", "setCDHLogPath.."));

	// Try getting the datadisk path requested until we get the buffer size right
	if( m_CDH_LogPath.empty() )
	{
		ACS_APGCC_CommonLib myAPGCCCommonLib;
		ACS_APGCC_DNFPath_ReturnTypeT getResult;
		int bufferLength = 50;
		char buffer[bufferLength];

		// Clear the array and set to 0
		ACE_OS::memset(buffer,0,bufferLength);

		getResult = myAPGCCCommonLib.GetDataDiskPath(AES_CDH::LOGPATH_KEY, buffer, bufferLength);
		//GCC_TTRACE((AES_CDH_Common_Path,"(%t) %s%d\n", "GetDiskPath return Value..",getResult));

		if( ACS_APGCC_DNFPATH_SUCCESS == getResult)
		{
			m_CDH_LogPath.assign(buffer);
			result = true;
			//GCC_TDEBUG((AES_CDH_Common_Path,"(%t) %s%s\n","Extracted Log Path is ", m_CDH_LogPath.c_str()));

		}
		else if(ACS_APGCC_STRING_BUFFER_SMALL == getResult)
		{
			char buffer2[bufferLength+1];

			ACE_OS::memset(buffer2,0,bufferLength+1);
			getResult = myAPGCCCommonLib.GetDataDiskPath(AES_CDH::LOGPATH_KEY, buffer2, bufferLength);
			//GCC_TTRACE((AES_CDH_Common_Path,"(%t) %s%d\n", "GetDiskPath return Value..",getResult));

			if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
			{
				m_CDH_LogPath.assign(buffer2);
				result = true;
				//GCC_TDEBUG((AES_CDH_Common_Path,"(%t) %s%s\n","Extracted Log Path is ", m_CDH_LogPath.c_str()));

			}
		}

		// Condition for other error codes from GetDataDiskPath. We can do much about it
		// so return false and raise an event
		if (ACS_APGCC_DNFPATH_SUCCESS != getResult)
		{
			ACE_TCHAR problemData[216] = { 0 };
			switch(getResult)
			{
			case  ACS_APGCC_DNFPATH_FAILURE :
				ACE_OS::sprintf(problemData,"Failed to fetch the dataDiskPath for aesaplogs");
				break;
			case ACS_APGCC_FAULT_LOGICAL_NAME :
				ACE_OS::sprintf(problemData,"Faulty logical name given for aesaplogs in fetching data disk path");
				break;
			default:
				break;
			}
			Event::report(1003, "AES Internal Fault",problemData," ");
		}
	}
	else
	{
		result = true;
	}

	return result;
}

bool AES_CDH_Common_Paths::getCDHDataPath(std::string &dataPath)  
{
	bool getResult = true;

	if(m_CDH_DataPath.empty())
	{
		getResult =setCDHDataPath();
	}
	dataPath = m_CDH_DataPath;
	//GCC_TINFO((AES_CDH_Common_Path,"(%t)CDH Data path:<%s>", dataPath.c_str()));
	return getResult;
}

bool AES_CDH_Common_Paths::getCDHLogPath(std::string &logPath)  
{
	bool getResult = true;

	if(m_CDH_LogPath.empty())
	{
		getResult = setCDHLogPath();
	}

	logPath = m_CDH_LogPath;
	//GCC_TINFO((AES_CDH_Common_Path,"(%t) CDH Log path:<%s>", logPath.c_str()));
	return getResult;
}

bool AES_CDH_Common_Paths::isRemoteHostValid(char* m_hostAddress)
{
	struct sockaddr_in server;
	int sock = -1;
	bool retValue = true;
	ACE_OS::memset(&server,'\0',sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(m_hostAddress);
	server.sin_port = htons(111);

	sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == -1)
	{
		retValue = false;
	}
	else
	{
		// Getting the socket properties before setting the socket non blocking
		int flags = fcntl( sock, F_GETFL, NULL );

		// Setting the socket non blocking
		int result = fcntl( sock, F_SETFL, flags | O_NONBLOCK);

		if( result == -1)
		{
			retValue = false;
		}
		else
		{
			// Connecting to the remote ip address
			result = connect(sock, (struct sockaddr*)(&server),sizeof(struct sockaddr_in));

			if (result == -1)
			{
				if(errno == EINPROGRESS)
				{
					fd_set writefdSet;
					FD_ZERO( &writefdSet);
					FD_SET( sock, &writefdSet );

					ACE_Time_Value tv(2);
					// Waiting for 2 seconds for reply
					result = ACE_OS::select( sock+1, 0, &writefdSet, 0, &tv );
					if( result <=  0 ) //TIMEOUT or select error
					{
						retValue = false;

					}
					else if( result > 0 )
					{
						if(FD_ISSET(sock, &writefdSet))
						{
							retValue = true;
						}
						else
						{
							retValue = false;
						}
					}
				}
				else
				{
					retValue = false;
				}
			}
		}
		close(sock);
		sock = -1;
	}
	return retValue;
}

//IPv6_feature::Validating IPv4 and IPv6 addresses (from OI callbacks)
int AES_CDH_Common_Util::isIPAddressValid(const char *p_IPAddress)
{
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "AES_CDH_Common_Util::isIPAddressValid() Entered - p_IPAddress[%s]",p_IPAddress);
	if(p_IPAddress == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "isIPAddressValid() - Given IP address is NULL - ERROR!");
		return INVALID_IPV4_IPV6;
	}
	std::string strIPAddress(p_IPAddress);
	if((strIPAddress.find('.') != std::string::npos) && (strIPAddress.find(':') != std::string::npos))
	{
		//IPv6_feature::IPv4-mapped IPv6 address will be rejected
		AES_CDH_LOG(LOG_LEVEL_ERROR, "isIPAddressValid() - Given IP address has both '.' and ':' characters - it is a IPv4-mapped IPv6 address. Invalid format!!! ERROR!");
		return INVALID_IPV4_MAPPED_IPV6_ADDRESS;
	}
	AES_CDH_LOG(LOG_LEVEL_INFO, "isIPAddressValid() - Check if given IP is a valid IPv4 address");

	if(strIPAddress.find('.') != std::string::npos)
		return (isIPv4AddressValid(p_IPAddress)? VALID_ADDRESS : INVALID_IPV4_ADDRESS);

	if(strIPAddress.find(':') != std::string::npos)
	{
		//IPv6_feature::Get Node Architecture using CS API - Verify & accept IPv6 only if node is VIRTUALIZED
		ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;

		ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture);

		if (cs_call_result != ACS_CS_API_NS::Result_Success) {
			AES_CDH_LOG(LOG_LEVEL_ERROR, "isIPAddressValid() - CS API getNodeArchitecture() failed - No need check for valid IPv6 - fail validation!!! ERROR!");
			return INVALID_UNABLE_TO_FETCH_NODE_TYPE;
		}
		AES_CDH_LOG(LOG_LEVEL_INFO, "isIPAddressValid() - CS API getNodeArchitecture() -> env is [%d]",(int)nodeArchitecture);
		if(nodeArchitecture == ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)  // VIRTUALIZED = 3
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "isIPAddressValid() - Node is VIRTUALIZED - Check if given IP is a valid IPv6 address");
			return (isIPv6AddressValid(p_IPAddress)? VALID_ADDRESS : INVALID_IPV6_ADDRESS);
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, "isIPAddressValid() - Node is NATIVE - Reject IPv6 without validating!");
			return INVALID_IPV6_NATIVE;
		}
	}
	AES_CDH_LOG(LOG_LEVEL_ERROR, "isIPAddressValid() - Given IP is NOT IPv4 and NOT IPv6 - invalid!!! ERROR!"); // Shouldn't ideally occur because this is handled in MODEL
	return INVALID_IPV4_IPV6;
}

//IPv6_feature:: AES_CDH_Common_Util::isIPv4AddressValid() - validates whether p_IPv4Address is a valid IPv4 address
bool AES_CDH_Common_Util::isIPv4AddressValid(const char *p_IPv4Address)
{
	AES_CDH_LOG(LOG_LEVEL_INFO, "AES_CDH_Common_Util::isIPv4AddressValid() Entered - p_IPv4Address[%s]",p_IPv4Address);

	if ( p_IPv4Address == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "isIPv4AddressValid() - p_IPv4Address is EMPTY!!! ERROR!");
		return false;
	}
	unsigned int b1 = 0, b2 =0, b3 = 0, b4 = 0;
	unsigned char c = 0;
	if( sscanf(p_IPv4Address, "%3u.%3u.%3u.%3u%c", &b1, &b2, &b3, &b4, &c ) != 4 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "isIPv4AddressValid() - sscanf() for IPv4 format failed with address[%s]!!! IPv4 format ERROR!",p_IPv4Address);
		return false;
	}

	if(( b1 | b2 | b3 | b4 ) > 255 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "isIPv4AddressValid() - one of the fields of IPv4 address[%s] is greater than 255! IPv4 format ERROR!", p_IPv4Address);
		return false;
	}


	if( strspn( p_IPv4Address, "0123456789.") < ACE_OS::strlen( p_IPv4Address ))
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "isIPv4AddressValid() - Invalid character present in IPv4 address[%s]! IPv4 format  ERROR!", p_IPv4Address);
		return false;
	}

	AES_CDH_LOG(LOG_LEVEL_INFO, "isIPv4AddressValid() - Leaving - VALID IPv4 address[%s]",p_IPv4Address);
	return true;
}

//IPv6_feature:: AES_CDH_Common_Util::isIPv6AddressValid() - validates whether p_IPv6Address is a valid IPv6 address
bool AES_CDH_Common_Util::isIPv6AddressValid(const char *p_IPv6Address)
{
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "AES_CDH_Common_Util::isIPv6AddressValid() Entered - p_IPv6Address[%s]",p_IPv6Address);
	//this method is just for blind acceptance because all IPv6 format validation happens
	//with the regular expression in MP.xml for ipAddress attribute. And the IPv4-mapped IPv6 format is rejected before this method is called.
	//So hopefully, the IPv6 address in p_IPv6Address is completely correct by this time.
	//method can be used for any more validation like rejecting internal IP range etc if requirement arises

	//todo: [DONE] Test the regex and IPv4-mapped IPv6 format rejection combination
	AES_CDH_LOG(LOG_LEVEL_WARN, "isIPv6AddressValid() - Leaving - VALID IPv6 address[%s]",p_IPv6Address);
	return true;
}

//IPv6_feature:: AES_CDH_Common_Util::isIPv6Address() - p_IPAddress will never be an invalid address like IPv4-mapped IPv6 address, wrong format etc. Just returns whether given IP addrss is IPv6 or not.
bool AES_CDH_Common_Util::isIPv6Address(const std::string p_IPAddress)
{
	if((p_IPAddress.find(':') != std::string::npos) && (p_IPAddress.find('.') == std::string::npos))  // && (p_IPAddress.find('.') == std::string::npos))
		return true;		// IPv6 address

	return false;	// IPv4 address [other types like Ipv4-mapped IPv6 format are filtered during OI handling]
}
