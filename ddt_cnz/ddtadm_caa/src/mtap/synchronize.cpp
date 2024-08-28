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
#include "mtap/synchronize.h"

#include "common/programconstants.h"
#include "common/macros.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_Synchronize)

namespace mtap
{
	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		FILE NUMBER
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4]		RECORD NUMBER
	 */
	namespace inputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_File_Number = 1,
			Index_Mtap_Individual = 2,
			Index_Record_Number = 4
		};
	}

	/**
	 * Response Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		FILE NUMBER
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4]		ERROR CODE
	 * 	[5]		CONTINUED - 0 means it is a unique response, 1 means we will send more responses
	 * 	[6]		NUMBER OF RECORDS
	 * 	[7]-[10]RECORD NUMBER
	 * 	[11]-[13]RECORD NUMBER
	 * 	...
	 */
	namespace outputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_File_Number = 1,
			Index_Mtap_Individual = 2,
			Index_Error_Code = 4,
			Index_Continued = 5,
			Index_Number_Of_Records = 6,
			Index_Record_Nr_First_Offset = 7
		};
	}

	namespace
	{
		static const ssize_t SYNCHRONIZE_SIZE = 8U;
		static const ssize_t SYNCHRONIZE_MIN_RESPONSE_SIZE = 7U;
	}

	Synchronize::Synchronize(unsigned char* rawBuffer, const ssize_t& bufferSize)
	: PDU(rawBuffer, bufferSize, protocol::primitive::SYNCHRONIZE),
	  m_recordNumber(0),
	  m_fileNumber(0U),
	  m_lastStored(0U)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Synchronize::~Synchronize()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void Synchronize::deserialize()
	{
		AES_DDT_TRACE_FUNCTION;

		//Check the size
		m_lastError = checkSize();

		if (protocol::ERR_NO_ERROR == m_lastError)
		{
			m_fileNumber = m_networkBuffer[inputBuffer::Index_File_Number];

			memcpy(&m_recordNumber, (m_networkBuffer + inputBuffer::Index_Record_Number), 4);

			m_lastStored = m_recordNumber;

			printf("SYNCHRONIZE REQUEST - RECORD NUMBER <%u>, FILE NUMBER <%u>\n", m_recordNumber, m_fileNumber);

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[SYNCHRONIZE] Record Number: <%u>, File Number: <%u>", m_recordNumber, m_fileNumber);
			AES_DDT_TRACE_MESSAGE("[SYNCHRONIZE] Record Number: <%u>, File Number: <%u>", m_recordNumber, m_fileNumber);
		}
	}

	int Synchronize::createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize)
	{
		AES_DDT_TRACE_FUNCTION;

		outBufferSize = 0;
		outBuffer = NULL;

		if(m_lastError != protocol::ERR_MALFORMED_PACKAGE)
		{
			// Base response size
			outBufferSize = SYNCHRONIZE_MIN_RESPONSE_SIZE;

			uint8_t numberOfMissingRecords = 0U;

			// Adjust the size to include missing records
			if( (protocol::ERR_NO_ERROR == m_lastError) && (m_lastStored < m_recordNumber) )
			{
				numberOfMissingRecords = (m_recordNumber - m_lastStored) + 1U;

				if(numberOfMissingRecords > protocol::DEFAULT_WIN_SIZE)
				{
					numberOfMissingRecords = protocol::DEFAULT_WIN_SIZE;
				}
				// increase the response size
				outBufferSize += numberOfMissingRecords * sizeof(uint32_t);
			}

			outBuffer = new (std::nothrow) unsigned char[outBufferSize];

			//reuse the received info to create the response
			memcpy(outBuffer, m_networkBuffer, outputBuffer::Index_Error_Code);

			// set the error code
			outBuffer[outputBuffer::Index_Error_Code] = m_lastError;

			// set continued to 0
			outBuffer[outputBuffer::Index_Continued] = 0U;

			// set Number of records
			outBuffer[outputBuffer::Index_Number_Of_Records] = numberOfMissingRecords;

			unsigned char* tmpOutBuffer = outBuffer + outputBuffer::Index_Record_Nr_First_Offset;

			for(uint32_t idx = 0; idx < numberOfMissingRecords; ++idx)
			{
				protocol::put4bytes(tmpOutBuffer, (m_lastStored + idx));
			}

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[SYNCHRONIZE] Missing Records:<%u>", numberOfMissingRecords);
			AES_DDT_TRACE_MESSAGE("[SYNCHRONIZE] Missing Records:<%u>", numberOfMissingRecords);
		}

		return m_lastError;
	}

	protocol::ErrorCode Synchronize::checkSize() const
	{
		AES_DDT_TRACE_FUNCTION;
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;
		if (SYNCHRONIZE_SIZE != m_networkBufferSize)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[SYNCHRONIZE] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, SYNCHRONIZE_SIZE);
			AES_DDT_TRACE_MESSAGE("ERROR: [SYNCHRONIZE] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, SYNCHRONIZE_SIZE);
			result = protocol::ERR_MALFORMED_PACKAGE;
		}

		return result;
	}

} /* namespace mtap */
