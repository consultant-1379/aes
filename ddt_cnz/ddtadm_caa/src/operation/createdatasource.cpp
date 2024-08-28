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

#include "operation/createdatasource.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_CreateDataSource)

namespace operation
{

	CreateDataSource::CreateDataSource()
	: OperationBase(DATASOURCE_ADD)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	CreateDataSource::~CreateDataSource()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void CreateDataSource::setOperationDetails(const void* op_details)
	{
		const operation::dataSourceInfo* pDataSourceInfo = reinterpret_cast<const operation::dataSourceInfo*>(op_details);
		m_info = *pDataSourceInfo;
	}

	int CreateDataSource::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("\n"
				"Create a new data source\n"
				"##### CREATE\n"
				"#####    Data Source: <%s>\n"
				"#####    recordSize: <%d>\n"
				"#####    alarmLevel: <%d>\n",
				m_info.getName(), m_info.recordSize, m_info.alarmLevel);

		printf("\n"
						"Create a new data source\n"
						"##### CREATE\n"
						"#####    Data Source: <%s>\n"
						"#####    recordSize: <%d>\n"
						"#####    alarmLevel:<%d>\n",
						m_info.getName(), m_info.recordSize, m_info.alarmLevel);
		engine::workingSet_t::instance()->getDataSourceManager().add(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
