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
#include "engine/outputformat.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_OutputFormat)

namespace engine
{

	OutputFormat::OutputFormat(const operation::outputFormatInfo& data)
	: m_data(data),
	  m_holdTime(boost::posix_time::seconds(m_data.holdTime))
	{
		AES_DDT_TRACE_FUNCTION;
	}

	OutputFormat::OutputFormat(const boost::shared_ptr<OutputFormat>& rhs)
	: m_data(rhs->m_data),
	  m_holdTime(rhs->m_holdTime)

	{

	}

	OutputFormat::~OutputFormat()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int OutputFormat::modify(const operation::outputFormatInfo& newData)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		if (newData.changeMask & operation::changeMask::DATACHUNKSIZE_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying data chunk size value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.dataChunkSize, newData.dataChunkSize);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying data chunk size value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.dataChunkSize, newData.dataChunkSize);

			m_data.dataChunkSize = newData.dataChunkSize;
		}

		if (newData.changeMask & operation::changeMask::DATALENGHTTYPE_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying data length type value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.dataLengthType, newData.dataLengthType);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying data length value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.dataLengthType, newData.dataLengthType);

			m_data.dataLengthType = newData.dataLengthType;
		}

		if (newData.changeMask & operation::changeMask::HOLDTIME_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying hold time value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.holdTime, newData.holdTime);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying hold time value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.holdTime, newData.holdTime);

			m_data.holdTime = newData.holdTime;
			m_holdTime = boost::posix_time::seconds(m_data.holdTime);
		}

		if (newData.changeMask & operation::changeMask::PADDINGCHAR_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying padding char value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.paddingChar, newData.paddingChar);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying padding char value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.paddingChar, newData.paddingChar);

			m_data.paddingChar = newData.paddingChar;
		}

		return result;

	}

} /* namespace engine */
