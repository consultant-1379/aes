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

#include "store/checkpoint/state/waitdata.h"
#include "store/checkpoint/state/iteratorinitialize.h"
#include "store/checkpoint/storebase.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/utility.h"

	#include <boost/make_shared.hpp>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_State_WaitData)

namespace store {
namespace checkpoint {
namespace state {

	WaitData::WaitData(StoreBase* checkpointObj, int lastError)
	: BaseInterface(checkpointObj, lastError, WAIT_DATA)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	WaitData::~WaitData()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int WaitData::handleStep()
	{
		AES_DDT_TRACE_MESSAGE("last Error:<%d>", m_lastError);
		return m_lastError;
	}

	bool WaitData::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = true;

		nextStep = boost::make_shared<IteratorInitialize>(m_checkpoint);

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
		return result;
	}


} /* namespace state */
} /* namespace checkpoint */
} /* namespace store */
