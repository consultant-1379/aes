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
#include "operation/deletedatasource.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_DeleteDataSource)

namespace operation
{

	DeleteDataSource::DeleteDataSource()
	: OperationBase(DATASOURCE_DEL)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	DeleteDataSource::~DeleteDataSource()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void DeleteDataSource::setOperationDetails(const void* op_details)
	{
		AES_DDT_TRACE_FUNCTION;
		const operation::dataSourceInfo* pInfo = reinterpret_cast<const operation::dataSourceInfo*>(op_details);

		m_info = *pInfo;
	}

	int DeleteDataSource::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("Delete data source:<%s>", m_info.getName() );

		std::cout << "Deleting... DATA SOURCE INFO:\n"
						"####               name: <" << m_info.name << ">\n"
						"####               moDN: <" << m_info.moDN << ">\n"
						"####               recordSize: <" << m_info.recordSize << ">\n"
						"####               alarmLevel: <" << m_info.alarmLevel << ">\n"
						"####               changeMask: <" << m_info.changeMask << ">\n";

		//remove the Data Source
		engine::workingSet_t::instance()->getDataSourceManager().remove(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
