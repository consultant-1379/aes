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
#include "operation/scheduler.h"

#ifndef CUTE_TEST
	#include "operation/operationbase.h"
	#include "operation/creator.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include <ace/Method_Request.h>
#else
	#include "stubs/operation_stub.h"
	#include "stubs/creator_stub.h"
#endif

#include "common/macros.h"
#include "common/programconstants.h"
#include <memory>

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_Scheduler)

namespace operation
{
	Scheduler::~Scheduler()
	{
		AES_DDT_TRACE_FUNCTION;
		// Delete all queued request
		while(!((bool)m_ActivationQueue.is_empty()) )
		{
			// Dequeue the next method object
			std::auto_ptr<ACE_Method_Request> cmdRequest(m_ActivationQueue.dequeue());
		}
	}

	int Scheduler::svc(void)
	{
		AES_DDT_TRACE_FUNCTION;
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Starting execution...");
		bool svcRun = true;

		int result = common::errorCode::ERR_NO_ERRORS;

		while(svcRun)
		{
			AES_DDT_TRACE_MESSAGE("Waiting for operation requests");

			// Dequeue the next method object
			std::auto_ptr<ACE_Method_Request> cmdRequest(m_ActivationQueue.dequeue());

			//interrogate the auto_ptr to check if it is null
			if (cmdRequest.get())
			{
				AES_DDT_TRACE_MESSAGE("Executing Operation");

				if(cmdRequest->call() == common::errorCode::ERR_SVC_DEACTIVATE)
				{
					svcRun = false;
				}
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("WARNING: READ NULL POINTER");
				AES_DDT_LOG(LOG_LEVEL_WARN, "READ NULL POINTER");
			}
		}

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "...Terminating execution");
		return result;
	}

	int Scheduler::open(void *args)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(args);

		int result = common::errorCode::ERR_NO_ERRORS;

		if(activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED))
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot start svc thread");
			AES_DDT_TRACE_MESSAGE("ERROR: cannot start svc thread. errno: %d", errno);
			result = common::errorCode::ERR_SVC_ACTIVATE;
		}

		return result;
	}

	int Scheduler::start()
	{
		AES_DDT_TRACE_FUNCTION;

		int result = common::errorCode::ERR_NO_ERRORS;

		if( thr_count() > 0U )
		{
			//ERROR CASE
			AES_DDT_LOG(LOG_LEVEL_ERROR, "thread already running. thr_count(): %zu", thr_count());
			AES_DDT_TRACE_MESSAGE("ERROR: thread already running. thr_count(): %zu", thr_count());
			result = common::errorCode::ERR_OPEN;
		}
		else
		{
			result = open();
		}

		return result;
	}

	int Scheduler::stop()
	{
		AES_DDT_TRACE_FUNCTION;

		if (thr_count() > 0)
		{
			operation::Creator operationFactoryCreator;
			ACE_Method_Request* terminate = operationFactoryCreator.make(SHUTDOWN);
			int result = enqueue(terminate);
			AES_DDT_LOG(LOG_LEVEL_INFO, "Thread running, Shutdown request enqueue result:<%d>", result);
			AES_DDT_TRACE_MESSAGE("Thread running, Shutdown request enqueue result:<%d>", result);
		}

		return wait();
	}
}
