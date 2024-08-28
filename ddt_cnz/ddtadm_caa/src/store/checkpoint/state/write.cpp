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

#include "store/checkpoint/state/write.h"

#include "store/checkpoint/state/updateinfo.h"
#include "store/checkpoint/storebase.h"
#include "common/programconstants.h"
#include "common/utility.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"

	#include <boost/make_shared.hpp>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_State_Write)

namespace store {
namespace checkpoint {
namespace state {

	Write::Write(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, WRITE)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Write::~Write()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int Write::handleStep()
	{
		AES_DDT_TRACE_FUNCTION;
		m_lastError = m_checkpoint->isCheckPointFull();
		return m_lastError;
	}

	bool Write::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = false;

		if( common::errorCode::ERR_CKPT_SECTIONS_FULL == m_lastError )
		{
			nextStep = boost::make_shared<UpdateInfo>(m_checkpoint);
			result = true;
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
		return result;
	}

} /* namespace ddt_ckpt */
}
}
