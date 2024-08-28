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
#include "mtap/getparameter.h"
#include "mtap/pduhandler.h"

#ifndef CUTE_TEST
	#include "mtap/datachannel.h"
	#include "engine/workingset.h"
	#include "common/configuration.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/workingset_stub.h"
	#include "stubs/mtap_stubs.h"
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_GetParameter)

namespace mtap
{

	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		ZERO
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4] -	NUMBER OF PARAMETERS
	 */
	namespace inputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_Zero = 1,
			Index_Mtap_Individual = 2,

			Index_Number_Of_Parameters = 4,
			Index_Offset_First_Parameter = 5
		};
	}

	/**
	 * Response Buffer Format:
	 * 	[0]		 PRIMITIVE ID
	 * 	[1]		 ZERO
	 * 	[2]-[3]	 MTAP INDIVIDUAL
	 * 	[4] -	 ERROR CODE
	 * 	[5] -	 NUMBER OF PARAMETERS
	 * 	[6] -	 PARAMETER CODE
	 * 	[7]-[10] PARAMETER VALUE
	 * 	......
	 * 	[X] -	 PARAMETER CODE
	 * 	[X+1]-[X+4] PARAMETER VALUE
	 */
	namespace outputBuffer
	{
		enum Index
		{
			Index_Primitive_ID = 0,
			Index_Zero = 1,
			Index_Mtap_Individual = 2,

			Index_Error_Code = 4,
			Index_Number_Of_Parameters = 5,
			Index_Offset_First_Parameter = 6
		};
	}

	namespace
	{
		static const ssize_t GET_PARAMETER_MIN_SIZE = 5U;

		static const ssize_t GET_PARAMETER_BASE_RESPONSE_SIZE = 6U;
		static const ssize_t GET_PARAMETER_CODE_SIZE = 1U;
		static const ssize_t GET_PARAMETER_VALUE_SIZE = 4U;
	}

	GetParameter::GetParameter(PDUHandler* myHandler, unsigned char *rawBuffer, ssize_t bufferSize)
	: PDU(rawBuffer, bufferSize, protocol::primitive::GET_PARAMETER),
	  m_handler(myHandler),
	  m_numberOfParameters(0U),
	  m_parameters()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	GetParameter::~GetParameter()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	/**
	 * Buffer Format:
	 * 	[0]		PRIMITIVE ID
	 * 	[1]		ZERO
	 * 	[2]-[3]	MTAP INDIVIDUAL
	 * 	[4] -	NUMBER OF PARAMETERS
	 */
	void GetParameter::deserialize()
	{
		AES_DDT_TRACE_FUNCTION;

		//Check the size and set m_numberOfParameters
		m_lastError = checkSize();
		std::string dataSourceName;
		uint32_t recordSize;
		uint32_t maxNumberOfRecords;

		if(protocol::ERR_NO_ERROR == m_lastError)
		{
			// Size is correct, proceed with parsing...

			// assemble list of pairs:  <RequestedParameterCode, ParameterValue>
			for(int parameterIndex = 0; parameterIndex < m_numberOfParameters; ++parameterIndex)
			{
				unsigned char parameter = m_networkBuffer[inputBuffer::Index_Offset_First_Parameter + parameterIndex];

				AES_DDT_TRACE_MESSAGE("[GET-PARAMETER] FOUND PARAMETER: <0x%02X>", parameter);

				switch (parameter)
				{
					case protocol::Parameter_WinSize:
						m_parameters.push_back( std::make_pair(parameter, protocol::DEFAULT_WIN_SIZE) );
						break;

					case protocol::Parameter_NoOfConn:
						m_parameters.push_back( std::make_pair(parameter, protocol::DEFAULT_NUM_OF_CONNECTIONS) );
						break;

					case protocol::Parameter_NoOfRecords:

						// get the record size of data source
						dataSourceName.assign(m_handler->getDataChannel()->getDataSourceName());
						recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(dataSourceName);

						// get the max number of records for this recordSize
						maxNumberOfRecords = engine::workingSet_t::instance()->getStoringManager().getMaxNumberOfRecordsPerStore(recordSize);

						m_parameters.push_back( std::make_pair(parameter, maxNumberOfRecords) );
						break;

					case protocol::Parameter_NoOfResends:
						m_parameters.push_back( std::make_pair(parameter, protocol::DEFAULT_NUM_OF_RESENDS) );
						break;

					case protocol::Parameter_ResendTime:
						m_parameters.push_back( std::make_pair(parameter, protocol::DEFAULT_RESEND_TIME) );
						break;

					case protocol::Parameter_ReconnTime:
						m_parameters.push_back( std::make_pair(parameter, protocol::DEFAULT_RECONN_TIME) );
						break;

					case protocol::Parameter_FAV:
						m_parameters.push_back( std::make_pair(parameter, protocol::DEFAULT_FAV) );
						break;

					case protocol::Parameter_Echo:
						m_parameters.push_back( std::make_pair(parameter, protocol::DEFAULT_ECHO_MODE) );
						break;

					case protocol::Parameter_RecordSize:

						// get the record size of data source
						dataSourceName.assign(m_handler->getDataChannel()->getDataSourceName());
						recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(dataSourceName);

						m_parameters.push_back( std::make_pair(parameter, recordSize) );
						break;

					default:
						AES_DDT_LOG(LOG_LEVEL_ERROR, "[GET-PARAMETER] Unknown parameter:<0x%02X>, skipping it", parameter);
						AES_DDT_TRACE_MESSAGE("ERROR: [GET-PARAMETER] Unknown parameter:<0x%02X>, skipping it", parameter);
						break;
				}
			}

			// If we have found any parameter, the result is always OK.
			m_numberOfParameters = static_cast<unsigned char>(m_parameters.size());
			m_lastError = ( ( m_numberOfParameters > 0U) ? protocol::ERR_NO_ERROR : protocol::ERR_MALFORMED_PACKAGE );
		}
	}

	int GetParameter::createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize)
	{
		AES_DDT_TRACE_FUNCTION;
		outBufferSize = 0U;
		outBuffer = NULL;

		if(m_lastError != protocol::ERR_MALFORMED_PACKAGE)
		{
			// calculate output buffer size according to the number of parameters
			outBufferSize = GET_PARAMETER_BASE_RESPONSE_SIZE + (GET_PARAMETER_CODE_SIZE * m_parameters.size()) + (GET_PARAMETER_VALUE_SIZE * m_parameters.size()) ;
			outBuffer = new (std::nothrow) unsigned char[outBufferSize];

			AES_DDT_TRACE_MESSAGE("[GET-PARAMETER] Response Buffer Size:<%zu>", outBufferSize);

			// Copy the contents of the 'Get-parameter' primitive to the buffer
			memcpy(outBuffer, m_networkBuffer, GET_PARAMETER_MIN_SIZE - 1U);

			// TODO **** int or protocol::error
			// set the error code
			outBuffer[outputBuffer::Index_Error_Code] = m_lastError;

			// set the number of parameters
			outBuffer[outputBuffer::Index_Number_Of_Parameters] = static_cast<unsigned char>(m_numberOfParameters);

			std::list<parameterPair_t>::const_iterator parameterPair;
			unsigned char* tmpBuffer = &outBuffer[outputBuffer::Index_Offset_First_Parameter];

			// Set all requested parameters with their values
			for(parameterPair = m_parameters.begin(); m_parameters.end() != parameterPair; ++parameterPair)
			{
				*tmpBuffer++ = parameterPair->first;
				protocol::put4bytes(tmpBuffer, parameterPair->second);
			}
		}

		return m_lastError;
	}

	protocol::ErrorCode GetParameter::checkSize()
	{
		AES_DDT_TRACE_FUNCTION;

		protocol::ErrorCode result = protocol::ERR_NO_ERROR;
		if (GET_PARAMETER_MIN_SIZE > m_networkBufferSize)
		{
			//Minimum size not respected
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[GET-PARAMETER] MALFORMED PACKAGE! Received Size: <%zu>, Expected Size: <%zu>", m_networkBufferSize, GET_PARAMETER_MIN_SIZE);
			AES_DDT_TRACE_MESSAGE("ERROR: [GET-PARAMETER] MALFORMED PACKAGE! Received Size: <%zu>, Expected Size: <%zu>", m_networkBufferSize, GET_PARAMETER_MIN_SIZE);
			result = protocol::ERR_MALFORMED_PACKAGE;
		}
		else
		{
			m_numberOfParameters = m_networkBuffer[inputBuffer::Index_Number_Of_Parameters];

			if (m_networkBufferSize != (GET_PARAMETER_MIN_SIZE + m_numberOfParameters))
			{
				//number of parameters does not match the package size
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[GET-PARAMETER] MALFORMED PACKAGE! Received Size: <%zu>, Expected Size: <%zu>", m_networkBufferSize, (GET_PARAMETER_MIN_SIZE + m_numberOfParameters));
				AES_DDT_TRACE_MESSAGE("ERROR: [GET-PARAMETER] MALFORMED PACKAGE! Received Size: <%zu>, Expected Size: <%zu>", m_networkBufferSize, (GET_PARAMETER_MIN_SIZE + m_numberOfParameters));
				result = protocol::ERR_MALFORMED_PACKAGE;
			}
		}
		return result;
	}

} /* namespace mtap */
