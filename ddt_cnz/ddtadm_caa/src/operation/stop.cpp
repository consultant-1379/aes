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
#include "operation/stop.h"

#include "common/programconstants.h"
#include "common/opensslthreading.h"

#include <libssh2.h>
#include "curl/curl.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif


AES_DDT_TRACE_DEFINE(AES_DDT_Operation_Stop)

namespace operation
{
	Stop::Stop()
	: OperationBase(STOP)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int Stop::call()
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_INFO, "Stop executed");

		int immResult = engine::workingSet_t::instance()->unregisterObjectImplementers();

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Object Implementers unregistration result:<%d>", immResult);
		AES_DDT_TRACE_MESSAGE("Object Implementers unregistration result:<%d>", immResult);

		int dsResult = engine::workingSet_t::instance()->getDataSourceManager().stop();

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Data Source stop result:<%d>", dsResult);
		AES_DDT_TRACE_MESSAGE("Data Source stop result:<%d>", dsResult);

		engine::workingSet_t::instance()->getStoringManager().shutdown();
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Storing manager stopped!");
		AES_DDT_TRACE_MESSAGE("Storing manager stopped");

		// Cease All alarms
		engine::workingSet_t::instance()->getAlarmhandler().ceaseAll();

		// Clueanup OpenSSL multithreading settings
		//common::OpenSSLThreading::cleanup();

		// Exit libssh2 functions and free internal memory
		libssh2_exit();

		// Cleanup libcurl internal memory
		curl_global_cleanup();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
