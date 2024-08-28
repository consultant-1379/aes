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

#include "operation/modifydatasource.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_ModifyDataSource)

namespace operation
{

	ModifyDataSource::ModifyDataSource()
	: OperationBase(DATASOURCE_MOD)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ModifyDataSource::~ModifyDataSource()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void ModifyDataSource::setOperationDetails(const void* op_details)
	{
		AES_DDT_TRACE_FUNCTION;
		const operation::dataSourceInfo* pDataSourceInfo = reinterpret_cast<const operation::dataSourceInfo*>(op_details);

		m_info.name = pDataSourceInfo->name;
		m_info.moDN = pDataSourceInfo->moDN;
		m_info.recordSize = pDataSourceInfo->recordSize;
		m_info.alarmLevel = pDataSourceInfo->alarmLevel;
		m_info.changeMask = pDataSourceInfo->changeMask;
	}

	int ModifyDataSource::call()
	{
		AES_DDT_TRACE_FUNCTION;

		engine::workingSet_t::instance()->getDataSourceManager().modify(m_info, m_operationResult);

		setResultToCaller();

		if(m_info.changeMask & operation::changeMask::RECORDSIZE_CHANGE)
		{
			AES_DDT_TRACE_MESSAGE("Modify data source:<%s>, new recordSize:<%d>", m_info.getName(), m_info.recordSize );
		}

		if(m_info.changeMask & operation::changeMask::ALARMLEVEL_CHANGE)
		{
			AES_DDT_TRACE_MESSAGE("Modify data source:<%s>, new alarmLevel:<%d>", m_info.getName(), m_info.alarmLevel );
		}

		return common::errorCode::ERR_NO_ERRORS;
	}


} /* namespace operation */
