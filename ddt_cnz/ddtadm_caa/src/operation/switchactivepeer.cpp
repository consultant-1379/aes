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

#include "operation/switchactivepeer.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/context.h"
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_SwitchActivePeer)

namespace operation
{

	SwitchActivePeer::SwitchActivePeer()
	: OperationBase(SWITCHACTIVEPEER_ACTION)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	SwitchActivePeer::~SwitchActivePeer()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void SwitchActivePeer::setOperationDetails(const void* op_details)
	{
		const operation::switchActivePeerInfo* pSwitchActivePeer = reinterpret_cast<const operation::switchActivePeerInfo*>(op_details);
		m_info = *pSwitchActivePeer;
	}

	int SwitchActivePeer::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_LOG(LOG_LEVEL_INFO, "Calling SwitchActivePeer Action on DataSink:<%s>", m_info.getDataSinkDN() );

		// get dataSource context
		engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(m_info.dataSourceName);

		context->getDataSink()->switchActivePeerAction();

		setResultToCaller();

		AES_DDT_LOG(LOG_LEVEL_INFO, "Done SwitchActivePeer Action on DataSink:<%s>", m_info.getDataSinkDN() );

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
