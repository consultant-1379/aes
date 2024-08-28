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
#ifndef DDTADM_CAA_INC_AES_DDT_UTILITY_H_
#define DDTADM_CAA_INC_AES_DDT_UTILITY_H_

#include <string>
#include <vector>
#include "common/programconstants.h"
#ifdef CUTE_TEST
	#include "stdint.h"
#endif

namespace common
{
	namespace utility
	{
		/** @brief
		 *
		 *  This method extracts the value of a RDN from a DN
		 *
		 *  @param fullDN : the DN of the object
		 *
		 *  @param RDN : the RDN to get
		 *
		 *  @param RDNValue : the field value
		 *
		 *  @return true on success, false otherwise
		 *
		 *  @remarks Remarks
		 */
		bool getRDNValue(const std::string& fullDN, const std::string& RDN, std::string& RDNValue);

		/** @brief	getDNbyTag
		 *
		 *  This method gets the DN of a object by its RDN
		 *
		 *  @param fullDN : A full DN
		 *
		 *  @param tagOfDN : The RDN
		 *
		 *  @param outDN :  The DN of the object
		 *
		 *  @return true on success, otherwise false
		 *
		 *  @remarks Remarks
		 */
		bool getDNbyTag(const std::string& fullDN, const char* tagOfDN, std::string& outDN);

		/** @brief	getLastFieldValue
		 *
		 *  This method extracts the last field value from a DN
		 *
		 *  @param stringToParser : the DN of the object
		 *
		 *  @param value : the field value
		 *
		 *  @return true on success, false otherwise
		 *
		 *  @remarks Remarks
		 */
		bool getLastFieldValue(const std::string& stringToParser, std::string& value);

		/** @brief	getSystemMemory
		 *
		 *  This method gets the size in bytes of system RAM memory
		 *
		 *  @return the RAM size of success otherwise a default value
		 *
		 *  @remarks Remarks
		 */
		uint64_t getSystemMemory();

		/** @brief	boolToString
		 *
		 *  This method gets boolean value to string
		 *
		 *  @return TRUE in case of true otherwise FALSE
		 *
		 *  @remarks Remarks
		 */
		inline const char* boolToString(bool b)
		{
			return b ? "TRUE" : "FALSE";
		}

		/** @brief	roundUpToPowerOfTw0
		 *
		 *  This method gets the nearest power of two higher than the input number
		 *
		 *	@param value : number to round up to a power of two
		 *
		 *  @return the next higher power of two
		 *
		 *  @remarks Remarks
		 */
		uint32_t roundUpToPowerOfTw0(const uint32_t& value);

		/** @brief	roundDownToPowerOfTw0
		 *
		 *  This method gets the nearest power of two lower than the input number
		 *
		 *	@param value : number to round down to a power of two
		 *
		 *  @return the next lower power of two
		 *
		 *  @remarks Remarks
		 */
		uint32_t roundDownToPowerOfTw0(const uint32_t& value);
		int isIPAddressValid(const char *p_IPAddress);
		bool isIPv4AddressValid(const char *p_IPAddress);
		bool isIPv6Address(const std::string p_IPAddress);
	}
}


#endif /* DDTADM_CAA_INC_AES_DDT_UTILITY_H_ */
