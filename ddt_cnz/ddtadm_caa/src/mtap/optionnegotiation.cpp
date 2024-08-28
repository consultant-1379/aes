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
#include "mtap/optionnegotiation.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_OptionNegotiation)

namespace mtap
{
	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		OPTION CODE
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4] -	OPTION DATA
	 */
	enum BufferIndex
	{
		Index_Primitive_ID = 0,
		Index_Option_Version_Code = 1,
		Index_Mtap_Individual = 2,

		Index_Option_Version_Data = 4
	};

	namespace
	{
		static const ssize_t OPTION_NEGOTIATION_SIZE = 5;
	}

	OptionNegotiation::OptionNegotiation(unsigned char *buf, ssize_t size)
	: PDU(buf, size, protocol::primitive::DO_WILL)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	OptionNegotiation::~OptionNegotiation()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		OPTION CODE
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4] -	OPTION DATA
	 */
	void OptionNegotiation::deserialize()
	{
		AES_DDT_TRACE_FUNCTION;

		//Check the size
		m_lastError = checkSize();

		if (protocol::ERR_NO_ERROR == m_lastError)
		{
			// Size is correct, proceed with parsing...
			protocol::Option option = static_cast<protocol::Option>(m_networkBuffer[1]);

			if (protocol::Option_Version == option)
			{
				m_lastError = checkVersion();
			}
		}
	}

	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		OPTION CODE
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4] -	OPTION DATA
	 */

	int OptionNegotiation::createResponse(unsigned char *&outBuffer, ssize_t &outBufferSize)
	{
		outBufferSize = 0;

		if(m_lastError != protocol::ERR_MALFORMED_PACKAGE)
		{
			//For the Option-Negotiation primitive, the Response length is the same of the Request one
			outBufferSize = m_networkBufferSize;

			outBuffer = new (std::nothrow) unsigned char[outBufferSize];

			//reuse the received info to create the response
			memcpy(outBuffer, m_networkBuffer, m_networkBufferSize);

			// Will/Will not response
			outBuffer[Index_Primitive_ID] = (protocol::ERR_NO_ERROR == m_lastError ? protocol::primitive::DO_WILL : protocol::primitive::DO_NOT_WILL_NOT);
		}

		return m_lastError;
	}

	protocol::ErrorCode OptionNegotiation::checkSize() const
	{
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;
		if (OPTION_NEGOTIATION_SIZE != m_networkBufferSize)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[OPTION-NEGOTIATION] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, OPTION_NEGOTIATION_SIZE);
			AES_DDT_TRACE_MESSAGE("ERROR: [OPTION-NEGOTIATION] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, OPTION_NEGOTIATION_SIZE);
			result = protocol::ERR_MALFORMED_PACKAGE;
		}
		return result;
	}

	protocol::ErrorCode OptionNegotiation::checkVersion() const
	{
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;
		unsigned char optionValue = m_networkBuffer[Index_Option_Version_Data];

		AES_DDT_TRACE_MESSAGE("[OPTION-NEGOTIATION] VERSION: <0x%02X>", optionValue);

		if (protocol::SUPPORTED_VERSION_3 != optionValue)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[OPTION-NEGOTIATION] VERSION NOT SUPPORTED! Received: <0x%02X>, Expected: <0x%02X>", optionValue, protocol::SUPPORTED_VERSION_3);
			AES_DDT_TRACE_MESSAGE("ERROR: [OPTION-NEGOTIATION] VERSION NOT SUPPORTED! Received: <0x%02X>, Expected: <0x%02X>", optionValue, protocol::SUPPORTED_VERSION_3);
			result = protocol::ERR_UNKNOWN_COMMAND;
		}

		return result;
	}

} /* namespace mtap */
