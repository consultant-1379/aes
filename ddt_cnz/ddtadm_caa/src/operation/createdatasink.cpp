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

#include "operation/createdatasink.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_CreateDataSink)

namespace operation
{

	CreateDataSink::CreateDataSink()
	: OperationBase(DATASINK_ADD)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	CreateDataSink::~CreateDataSink()
	{
		;
	}

	void CreateDataSink::setOperationDetails(const void* op_details)
	{
		const operation::dataSinkInfo* pDataSinkInfo = reinterpret_cast<const operation::dataSinkInfo*>(op_details);
		m_info = *pDataSinkInfo;
	}

	int CreateDataSink::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("\n"
				"Create the new Data Sink\n"
				"##### CREATE\n"
				"#####    Data Sink: <%s>\n"
				"#####    Data Source: <%s>\n"
				"#####    retryDelay: <%u>\n"
				"#####    retryAttempts: <%u>\n",
				m_info.getName(), m_info.getDataSourceName(), m_info.retryDelay, m_info.retryAttempts);

		printf("\n"
						"Create the new Data Sink\n"
						"##### CREATE\n"
						"#####    Data Sink: <%s>\n"
						"#####    Data Source: <%s>\n"
						"#####    retryDelay: <%u>\n"
						"#####    retryAttempts: <%u>\n",
						m_info.getName(), m_info.getDataSourceName(), m_info.retryDelay, m_info.retryAttempts);

		engine::workingSet_t::instance()->getDataSourceManager().add(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
