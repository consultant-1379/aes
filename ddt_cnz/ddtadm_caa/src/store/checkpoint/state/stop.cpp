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

#include "store/checkpoint/state/stop.h"

#include "store/checkpoint/storebase.h"
#include "common/programconstants.h"
#include "common/utility.h"
#include "common/macros.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#endif


AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_State_Stop)


namespace store {
namespace checkpoint {
namespace state {

	Stop::Stop(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, STOP)
	{
		AES_DDT_TRACE_FUNCTION;
		m_lastError = common::errorCode::ERR_CKPT_STOPPED;
	}

	Stop::~Stop()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int Stop::handleStep()
	{
		AES_DDT_TRACE_FUNCTION;
		return m_lastError;
	}

	bool Stop::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		UNUSED(nextStep);
		bool result = true;

		AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> Writer is in STOPPED STATE",
				m_checkpoint->getProducerName(), m_checkpoint->getDataSourceName());

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Producer:<%s> of DS:<%s> Writer is in STOPPED STATE",
				m_checkpoint->getProducerName(), m_checkpoint->getDataSourceName());

		return result;
	}

} /* namespace ddt_ckpt */
}
}
