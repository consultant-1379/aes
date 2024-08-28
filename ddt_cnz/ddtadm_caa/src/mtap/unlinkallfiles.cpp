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

#include "mtap/unlinkallfiles.h"
#include "common/programconstants.h"
#include "common/macros.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_UnlinkAllFiles)

namespace mtap
{
	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		ZERO
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 */
	namespace inputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_Zero = 1,
			Index_Mtap_Individual = 2,
		};
	}

	/**
	 * Response Buffer Format:
	 * 	[0]		 PRIMITIVE ID
	 * 	[1]		 ZERO
	 * 	[2]-[3]	 MTAP INDIVIDUAL
	 * 	[4] -	 ERROR CODE
	 */
	namespace outputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_Zero = 1,
			Index_Mtap_Individual = 2,
			Index_Error_Code = 4,
		};
	}

	namespace
	{
		static const ssize_t UNLINK_ALL_FILES_SIZE = 4U;
		static const ssize_t UNLINK_ALL_FILES_RESPONSE_SIZE = 5U;
	}

	UnlinkAllFiles::UnlinkAllFiles(unsigned char* rawBuffer, const ssize_t& bufferSize)
	: PDU(rawBuffer, bufferSize, protocol::primitive::UNLINK_ALL_FILES)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	UnlinkAllFiles::~UnlinkAllFiles()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	/**
	 * Input Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		OPTION CODE
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 */
	void UnlinkAllFiles::deserialize()
	{
		AES_DDT_TRACE_FUNCTION;

		//Check the size
		m_lastError = checkSize();

		if (protocol::ERR_NO_ERROR == m_lastError)
		{
			// Since DDT does not use files, do nothing
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[UNLINK_ALL_FILES] Since DDT does not use files, do nothing");
			AES_DDT_TRACE_MESSAGE("[UNLINK_ALL_FILES] Since DDT does not use files, do nothing");
			printf("##### [UNLINK_ALL_FILES] #####\n");
		}
	}

	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		OPTION CODE
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4] -	ERROR CODE
	 */
	int UnlinkAllFiles::createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize)
	{
		AES_DDT_TRACE_FUNCTION;
		outBufferSize = 0;
		outBuffer = NULL;

		if(m_lastError != protocol::ERR_MALFORMED_PACKAGE)
		{
			//For the Option-Negotiation primitive, the Response length is the same of the Request one
			outBufferSize = UNLINK_ALL_FILES_RESPONSE_SIZE;

			outBuffer = new (std::nothrow) unsigned char[outBufferSize];

			//reuse the received info to create the response
			memcpy(outBuffer, m_networkBuffer, UNLINK_ALL_FILES_SIZE);

			// set the error code
			outBuffer[outputBuffer::Index_Error_Code] = m_lastError;
		}

		return m_lastError;
	}

	protocol::ErrorCode UnlinkAllFiles::checkSize() const
	{
		AES_DDT_TRACE_FUNCTION;
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;
		if (UNLINK_ALL_FILES_SIZE != m_networkBufferSize)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[UNLINK_ALL_FILES] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, UNLINK_ALL_FILES_SIZE);
			AES_DDT_TRACE_MESSAGE("ERROR: [UNLINK_ALL_FILES] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, UNLINK_ALL_FILES_SIZE);
			result = protocol::ERR_MALFORMED_PACKAGE;
		}
		return result;
	}


} /* namespace mtap */
