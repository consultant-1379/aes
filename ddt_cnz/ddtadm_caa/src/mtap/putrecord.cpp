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
#include "mtap/putrecord.h"

#include "common/programconstants.h"
#include "common/macros.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_PutRecord)

namespace mtap
{
	static void hex_print(const char * TAG, const void* pv, size_t len, bool isNumber)
	{
		printf("%s: ", TAG);

		const unsigned char * p = (const unsigned char*)pv;
		if (NULL == pv)
		{
			printf("NULL");
		}
		else
		{
			size_t i = 0;
			for (; i < len; ++i)
			{
				if ((i > 0) && (i%16 == 0))
				{
					printf("\n");
				}
				else if (i > 0)
				{
					printf(":");
				}

				printf("%02X", *p++);
			}
		}

		printf(" ");
		if (isNumber)
		{
			if (len == sizeof(unsigned char))
			{
				const unsigned char value = *(reinterpret_cast<const unsigned char *>(pv));
				printf("[%u]", value);
			}

			else if (len == sizeof(uint32_t))
			{
				const uint32_t value = *(reinterpret_cast<const uint32_t *>(pv));
				printf("[%u]", value);
			}
		}

		printf("\n");
	}


	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		FILE NUMBER
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4]-[7]	RECORD NUMBER
	 * 	[8] ... DATA
	 */
	namespace inputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_File_Number = 1,
			Index_Mtap_Individual = 2,
			Index_Record_Number = 4,
			Index_Primitive_Format = 8,
			Index_Version_Number = 9,
			Index_Data_Offset = 10
		};
	}

	/**
	 * Response Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		FILE NUMBER
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4]-[7]	RECORD NUMBER
	 * 	[8] 	 ERROR CODE
	 */
	namespace outputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_File_Number = 1,
			Index_Mtap_Individual = 2,
			Index_Record_Number = 4,
			Index_Error_Code = 8
		};
	}

	namespace
	{
		static const ssize_t PUT_RECORD_MIN_SIZE = 10U;
		static const ssize_t PUT_RECORD_RESPONSE_SIZE = 9U;
	}

	PutRecord::PutRecord(unsigned char* rawBuffer, const ssize_t& bufferSize)
	: PDU(protocol::primitive::PUT_RECORD),
	  m_recordNumber(0),
	  m_fileNumber(0U)
	{
		AES_DDT_TRACE_FUNCTION;
		m_networkBufferSize = bufferSize;
		m_networkBuffer = new (std::nothrow) unsigned char[m_networkBufferSize];
		memcpy(m_networkBuffer, rawBuffer, m_networkBufferSize);
	}

	PutRecord::~PutRecord()
	{
		AES_DDT_TRACE_FUNCTION;
		delete[] m_networkBuffer;
	}

	void PutRecord::deserialize()
	{
		AES_DDT_TRACE_FUNCTION;

		//Check the size
		m_lastError = checkSize();

		if (protocol::ERR_NO_ERROR == m_lastError)
		{
			//hex_print("PUT-RECORD", m_networkBuffer, 10, false);

			m_fileNumber = m_networkBuffer[inputBuffer::Index_File_Number];
			//hex_print("FILE NR: ", &m_fileNumber, sizeof(unsigned char), true);

			memcpy(&m_recordNumber, (m_networkBuffer + inputBuffer::Index_Record_Number), 4);
			//hex_print("RECORD NR: ", &m_recordNumber, sizeof(uint32_t), true);

			//printf("[PUT-RECORD] FileNr: <%u>, RecordNumber: <%u>\n", m_fileNumber, m_recordNumber);

			AES_DDT_TRACE_MESSAGE("[PUT-RECORD] FileNr: <0x%02X> <%u>, RecordNumber: <0x%04X> <%u>, RecordSize: <%u> #####", m_fileNumber, m_fileNumber, m_recordNumber, m_recordNumber, m_networkBufferSize);
		}
	}

	int PutRecord::createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize)
	{
		AES_DDT_TRACE_FUNCTION;

		outBufferSize = 0U;
		outBuffer = NULL;

		if(m_lastError != protocol::ERR_MALFORMED_PACKAGE)
		{
			//For the Option-Negotiation primitive, the Response length is the same of the Request one
			outBufferSize = PUT_RECORD_RESPONSE_SIZE;

			outBuffer = new (std::nothrow) unsigned char[outBufferSize];

			//reuse the received info to create the response
			memcpy(outBuffer, m_networkBuffer, outputBuffer::Index_Error_Code);

			// set the error code
			outBuffer[outputBuffer::Index_Error_Code] = (m_lastError == protocol::ERR_RECORD_ALREADY_STORED) ? protocol::ERR_NO_ERROR : m_lastError;
		}

		return m_lastError;
	}

	void PutRecord::getRecordData(unsigned char*& dataBuffer, uint32_t& dataLength)
	{
		dataLength = 0U;
		dataBuffer = NULL;

		if(m_lastError != protocol::ERR_MALFORMED_PACKAGE)
		{
			dataLength = m_networkBufferSize - PUT_RECORD_MIN_SIZE;
			dataBuffer = m_networkBuffer + inputBuffer::Index_Data_Offset;
		}
	}

	protocol::ErrorCode PutRecord::checkSize() const
	{
		AES_DDT_TRACE_FUNCTION;
		protocol::ErrorCode result = protocol::ERR_NO_ERROR;
		if (PUT_RECORD_MIN_SIZE > m_networkBufferSize)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[PUT-RECORD] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, PUT_RECORD_MIN_SIZE);
			AES_DDT_TRACE_MESSAGE("ERROR: [PUT-RECORD] MALFORMED PACKAGE! Received Size: <%ld>, Expected Size: <%zu>", m_networkBufferSize, PUT_RECORD_MIN_SIZE);
			result = protocol::ERR_MALFORMED_PACKAGE;
		}

		return result;
	}

} /* namespace mtap */
