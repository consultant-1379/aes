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

#include "operation/createoutputformat.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_CreateOutputFormat)


namespace operation
{

	CreateOutputFormat::CreateOutputFormat()
	: OperationBase(OUTPUTFORMAT_ADD)
	{
	}

	CreateOutputFormat::~CreateOutputFormat()
	{
	}

	void CreateOutputFormat::setOperationDetails(const void* op_details)
	{
		const operation::outputFormatInfo* pOutputFormatInfo = reinterpret_cast<const operation::outputFormatInfo*>(op_details);
		m_info = *pOutputFormatInfo;
	}

	int CreateOutputFormat::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("\n"
				"Create a new OutputFormat\n"
				"##### CREATE\n"
				"#####    OutputFormat: <%s>\n"
				"#####    Source/Sink: <%s>/<%s>\n"
				"#####    dataChunkSize: <%u>\n"
				"#####    dataLengthType: <%u>\n"
				"#####    holdTime: <%u>\n"
				"#####    paddingChar: <0x%02X>\n",
				m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(),
				m_info.dataChunkSize, m_info.dataLengthType, m_info.holdTime, m_info.paddingChar);

		printf("\n"
						"Create a new OutputFormat\n"
						"##### CREATE\n"
						"#####    OutputFormat: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    dataChunkSize: <%u>\n"
						"#####    dataLengthType: <%u>\n"
						"#####    holdTime: <%u>\n"
						"#####    paddingChar: <0x%02X>\n",
						m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(),
						m_info.dataChunkSize, m_info.dataLengthType, m_info.holdTime, m_info.paddingChar);

		engine::workingSet_t::instance()->getDataSourceManager().add(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
