//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************
#include "common/utility.h"
#include "imm/imm.h"
#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include <ACS_CS_API.h>
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Utility)

namespace common
{
	namespace utility
	{

		namespace
		{
			uint64_t DEFAULT_RAMBYTES = 1024ULL*1024ULL*1024ULL;
		}

		bool getRDNValue(const std::string& fullDN, const std::string& RDN, std::string& RDNValue)
		{
			AES_DDT_TRACE_MESSAGE("DN:<%s>, RDN:<%s>", fullDN.c_str(), RDN.c_str());

			std::string subpartOfDN;

			bool result = getDNbyTag(fullDN, RDN.c_str(), subpartOfDN);

			if(result)
			{
				result = getLastFieldValue(subpartOfDN, RDNValue);
			}

			return result;
		}

		bool getDNbyTag(const std::string& fullDN, const char* tagOfDN, std::string& outDN)
		{
			AES_DDT_TRACE_MESSAGE("DN:<%s>, RDN:<%s>", fullDN.c_str(), tagOfDN);

			bool result = false;
			size_t tagStartPos;
			outDN.clear();
			tagStartPos = fullDN.find(tagOfDN);

			// Check if the tag is present
			if( std::string::npos != tagStartPos )
			{
				// get the DN
				outDN = fullDN.substr(tagStartPos);

				AES_DDT_TRACE_MESSAGE(" SubDN:<%s>", outDN.c_str() );
				result= true;
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "ParserTag:<%s> not found into DN:<%s>", tagOfDN, fullDN.c_str() );
				AES_DDT_TRACE_MESSAGE("ERROR: ParserTag:<%s> not found into DN:<%s>", tagOfDN, fullDN.c_str() );
			}

			return result;
		}

		bool getLastFieldValue(const std::string& stringToParser, std::string& value)
		{
			bool result = false;

			value.clear();

			// Get the last value from DN e.g: class1Id=xyz,class2Id=lmk,....
			// Split the field in RDN and Value e.g. : class1Id xyz
			size_t equalPos = stringToParser.find_first_of(imm::parserTag::equal);
			size_t commaPos = stringToParser.find_first_of(imm::parserTag::comma);

			// Check if some error happens
			if( (std::string::npos != equalPos) )
			{
				// check for a single field case
				if( std::string::npos == commaPos )
					value = stringToParser.substr(equalPos + 1);
				else
					value = stringToParser.substr(equalPos + 1, (commaPos - equalPos - 1) );

				result = true;
				AES_DDT_TRACE_MESSAGE(" last field value:<%s>", value.c_str());
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "ParserTag:<%c> not found into the string:<%s>", imm::parserTag::equal, stringToParser.c_str() );
				AES_DDT_TRACE_MESSAGE("ParserTag:<%c> not found into the string:<%s>", imm::parserTag::equal, stringToParser.c_str() );
			}

			return result;
		}

		uint64_t getSystemMemory()
		{
			uint64_t ramSize;

			// Get size of a page in bytes
			long pageSize = sysconf( _SC_PAGESIZE );

			// Get  the number of pages of physical memory
			long numberOfPages = sysconf( _SC_PHYS_PAGES );

			if( (pageSize > 0) && (numberOfPages > 0))
			{
				ramSize = pageSize * numberOfPages;
			}
			else
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to get system RAM size, pageSize:<%ld> numberOfPages:<%ld>", pageSize, numberOfPages);
				AES_DDT_TRACE_MESSAGE("Failed to get system RAM size, pageSize:<%ld> numberOfPages:<%ld>", pageSize, numberOfPages);

				ramSize = DEFAULT_RAMBYTES;
			}

			AES_DDT_LOG( LOG_LEVEL_INFO, "SYSTEM RAM SIZE:<%zu> bytes", ramSize);
			return ramSize;
		}

		uint32_t roundUpToPowerOfTw0(const uint32_t& value)
		{
			uint32_t powerOfTwo = value;

			powerOfTwo--;
			powerOfTwo |= powerOfTwo >> 1;
			powerOfTwo |= powerOfTwo >> 2;
			powerOfTwo |= powerOfTwo >> 4;
			powerOfTwo |= powerOfTwo >> 8;
			powerOfTwo |= powerOfTwo >> 16;
			powerOfTwo++;

			return powerOfTwo;
		}

		uint32_t roundDownToPowerOfTw0(const uint32_t& value)
		{
			uint32_t powerOfTwo = roundUpToPowerOfTw0(value);

			if (value != powerOfTwo)
			{
				powerOfTwo = powerOfTwo >> 1;
			}

			return powerOfTwo;
		}

		//IPv6_feature::Validating IPv4 and IPv6 addresses (from OI callbacks)
		int isIPAddressValid(const char *p_IPAddress)
		{
			int retCode = 0;	//TR HY37414 - Return success in NATIVE env when p_IPAddress is IPv4.
			std::string strIPAddress(p_IPAddress);
			if((strIPAddress.find('.') != std::string::npos) && (strIPAddress.find(':') != std::string::npos))
			{
				//IPv6_feature::IPv4-mapped IPv6 address will be rejected
				retCode = -1;
				return retCode;
			}
			ACS_CS_API_CommonBasedArchitecture::ArchitectureValue nodeArchitecture;

			ACS_CS_API_NS::CS_API_Result cs_call_result = ACS_CS_API_NetworkElement::getNodeArchitecture(nodeArchitecture);

			if (cs_call_result != ACS_CS_API_NS::Result_Success) {
				retCode = -2;
				return retCode;
			}

			if(nodeArchitecture != ACS_CS_API_CommonBasedArchitecture::VIRTUALIZED)	//TR HY37414
			{
				if(strIPAddress.find(':') != std::string::npos)  // IPv6 on Native is rejected
					retCode = -1;
			}
			// All other cases, retCode value is 0 i.e. IPv4 on Native and IPv4, IPv6 on Virtual are allowed
			return retCode;
		}

		//IPv6_feature:: isIPv6Address() - p_IPAddress will never be an invalid address like IPv4-mapped IPv6 address, wrong format etc. Just returns whether given IP addrss is IPv6 or not.
		bool isIPv6Address(const std::string p_IPAddress)
		{
			if((p_IPAddress.find(':') != std::string::npos) && (p_IPAddress.find('.') == std::string::npos))  // && (p_IPAddress.find('.') == std::string::npos))
				return true;		// IPv6 address

			return false;	// IPv4 address [other types like Ipv4-mapped IPv6 format are filtered during OI handling]
		}
	}
}



