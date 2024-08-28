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
#include "operation/shutdown.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_Shutdown)

namespace operation
{

	Shutdown::Shutdown()
	: OperationBase(SHUTDOWN)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int Shutdown::call()
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_INFO, "Shutdown ongoing");

		engine::workingSet_t::instance()->stopMainReactor();

		return common::errorCode::ERR_SVC_DEACTIVATE;
	}

} /* namespace operation */
