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

#include "operation/start.h"

#include "common/programconstants.h"
#include "common/utility.h"
#include "common/opensslthreading.h"

#include <boost/make_shared.hpp>

#include <libssh2.h>
#include <curl/curl.h>

#ifndef CUTE_TEST
	#include "store/checkpoint/strategy.h"
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/store_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_Start)

namespace operation
{

    Start::Start()
    : OperationBase(START)
    {
        AES_DDT_TRACE_FUNCTION;
    }

    int Start::call()
    {
        AES_DDT_TRACE_FUNCTION;
        AES_DDT_LOG(LOG_LEVEL_INFO, "Start executed");

        // Initialize the libssh2 functions and underlying crypto environment
        // NOTE:
        //		the flag LIBSSH2_INIT_NO_CRYPTO would indicate instead
        //		that libssh2_init shall not initialize crypto.
        if (libssh2_init(0) < 0)
        {
        	AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "'libssh2_init()' failed");
        	AES_DDT_TRACE_MESSAGE("ERROR: 'libssh2_init()' failed. errno: <%d>", errno);
        }
        else
        {
        	AES_DDT_LOG(LOG_LEVEL_DEBUG, "libssh2 initialized");
        	AES_DDT_TRACE_MESSAGE("libssh2 initialized");
        }

        // Initialize CURL library
        CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
        if (CURLE_OK != result)
        {
        	AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "'curl_global_init()' failed. Error text: %s", curl_easy_strerror(result));
        	AES_DDT_TRACE_MESSAGE("ERROR: 'curl_global_init()' failed. errno: <%s>", curl_easy_strerror(result));
        }
        else
        {
        	AES_DDT_LOG(LOG_LEVEL_DEBUG, "libcurl initialized");
        	AES_DDT_TRACE_MESSAGE("libcurl initialized");
        }

        // Setup OpenSSL multithreading settings
        if (common::OpenSSLThreading::setup() != common::errorCode::ERR_NO_ERRORS)
        {
        	const int errno_save = errno;
        	AES_DDT_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "'OpenSSLThreading::setup()' failed");
        	AES_DDT_TRACE_MESSAGE("ERROR: 'OpenSSLThreading::setup()' failed. errno: <%d>", errno_save);
        }
        else
        {
        	AES_DDT_LOG(LOG_LEVEL_DEBUG, "OpenSSLThreading initialized");
        	AES_DDT_TRACE_MESSAGE("OpenSSLThreading initialized");
        }

    	// Reload MOs
    	bool dsLoadResult = engine::workingSet_t::instance()->getDataSourceManager().loadFromImm();

    	AES_DDT_LOG(LOG_LEVEL_DEBUG, "Data reloaded from IMM result:<%s>", common::utility::boolToString(dsLoadResult) );
    	AES_DDT_TRACE_MESSAGE("Data reloaded from IMM result:<%s>", common::utility::boolToString(dsLoadResult) );

        // Storing Strategy
        boost::shared_ptr<store::StrategyInterface> storeStrategy = boost::make_shared<store::checkpoint::Strategy>();
        bool ckptReloadResult = engine::workingSet_t::instance()->getStoringManager().setStoreStrategy(storeStrategy);

        AES_DDT_LOG(LOG_LEVEL_DEBUG, "Storing manager status result:<%s>", common::utility::boolToString(ckptReloadResult) );
        AES_DDT_TRACE_MESSAGE("Storing manager status result:<%s>", common::utility::boolToString(ckptReloadResult) );


        // Register Object Implementers
        int immResult =  engine::workingSet_t::instance()->registerObjectImplementers();

        AES_DDT_LOG(LOG_LEVEL_DEBUG, "Object Implementers registration result:<%d>", immResult);
    	AES_DDT_TRACE_MESSAGE("Object Implementers registration result:<%d>", immResult);

    	// Start MOs
    	int startResult = engine::workingSet_t::instance()->getDataSourceManager().start();

    	AES_DDT_LOG(LOG_LEVEL_DEBUG, "Engine starting. Result:<%d>", startResult );
    	AES_DDT_TRACE_MESSAGE("Engine starting. Result:<%d>", startResult );

        return common::errorCode::ERR_NO_ERRORS;
    }
} /* namespace operation */
