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
#include "mtap/stop.h"

#include "common/programconstants.h"
#include "common/macros.h"

#include "common/tracer.h"
#include "common/logger.h"


AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_Stop)


namespace mtap
{

	/**
	 * Response Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]-[3]	ZERO
	 */
	namespace outputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_Zero = 1
		};
	}

	namespace
	{
		static const ssize_t STOP_RESPONSE_SIZE = 4U;
	}

	Stop::Stop() : PDU(protocol::primitive::STOP)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Stop::~Stop()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int Stop::createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize)
	{
		AES_DDT_TRACE_FUNCTION;

		outBufferSize = STOP_RESPONSE_SIZE;

		outBuffer = new (std::nothrow) unsigned char[outBufferSize];

		memset(outBuffer, 0, outBufferSize);

		// set the error code
		outBuffer[outputBuffer::Index_Primitive_ID] = protocol::Command_Stop;

		return m_lastError;
	}


} /* namespace mtap */
