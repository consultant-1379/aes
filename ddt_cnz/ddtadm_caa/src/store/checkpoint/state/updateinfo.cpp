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

#include "store/checkpoint/state/updateinfo.h"

#include "store/checkpoint/state/close.h"
#include "store/checkpoint/storebase.h"
#include "common/programconstants.h"
#include "common/utility.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"

	#include <boost/make_shared.hpp>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_State_UpdateInfo)

namespace store {
namespace checkpoint {
namespace state {

	UpdateInfo::UpdateInfo(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, UPDATEINFO)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	UpdateInfo::~UpdateInfo()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int UpdateInfo::handleStep()
	{
		AES_DDT_TRACE_FUNCTION;
		m_lastError = m_checkpoint->updateInfoSection();
		return m_lastError;
	}

	bool UpdateInfo::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = true;

		if( common::errorCode::ERR_NO_ERRORS == m_lastError )
		{
			nextStep = boost::make_shared<Close>(m_checkpoint);
		}
		else
		{
			result = false;
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s> of DS:<%s> tried to get next step from a failed step, checkpoint:<%s>",
					m_checkpoint->getProducerName(), m_checkpoint->getDataSourceName(), m_checkpoint->getCheckPointName());

			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> tried to get next step from a failed step, checkpoint:<%s>",
					m_checkpoint->getProducerName(), m_checkpoint->getDataSourceName(), m_checkpoint->getCheckPointName());

		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
		return result;
	}


} /* namespace ddt_ckpt */
}
}
