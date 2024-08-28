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
#include "operation/deletedatasink.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_DeleteDataSink)

namespace operation
{

	DeleteDataSink::DeleteDataSink()
	: OperationBase(DATASINK_DEL)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	DeleteDataSink::~DeleteDataSink()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void DeleteDataSink::setOperationDetails(const void* op_details)
	{
		AES_DDT_TRACE_FUNCTION;
		const operation::dataSinkInfo* pInfo = reinterpret_cast<const operation::dataSinkInfo*>(op_details);

		m_info = *pInfo;
	}

	int DeleteDataSink::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("Delete data sink:<%s/%s>", m_info.getDataSourceName(), m_info.getName() );

		std::cout << "Deleting... DATA SINK INFO:\n"
				"####               name: <" << m_info.name << ">\n"
				"####               moDN: <" << m_info.moDN << ">\n"
				"####               dataSourceName: <" << m_info.dataSourceName << ">\n"
				"####               retryDelay: <" << m_info.retryDelay << ">\n"
				"####               retryAttempts: <" << m_info.retryAttempts << ">\n"
				"####               changeMask: <" << m_info.changeMask << ">\n";

		//remove the Data Sink
		engine::workingSet_t::instance()->getDataSourceManager().remove(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
