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

#include "store/checkpoint/state/read.h"

#include "store/checkpoint/state/iteratorinitialize.h"
#include "store/checkpoint/state/waitcommit.h"
#include "store/checkpoint/state/waitdata.h"
#include "store/checkpoint/state/close.h"

#include "store/checkpoint/storebase.h"
#include "common/programconstants.h"
#include "common/utility.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"

	#include <boost/make_shared.hpp>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_State_Read)

namespace store {
namespace checkpoint {
namespace state {

	Read::Read(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, READ)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Read::~Read()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int Read::handleStep()
	{
		AES_DDT_TRACE_FUNCTION;
		m_lastError = m_checkpoint->getNextSectionToRead();
		return m_lastError;
	}

	bool Read::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = true;

		// Checkpoint empty, delete it and get a new one
		if( common::errorCode::ERR_CKPT_SECTIONS_EMPTY == m_lastError )
		{
			if( m_checkpoint->checkForCommit() == 0U)
			{
				nextStep = boost::make_shared<Close>(m_checkpoint);
			}
			else
			{
				// There are records to commit
				nextStep = boost::make_shared<WaitCommit>(m_checkpoint);
			}
		}
		else
		{
			nextStep = boost::make_shared<WaitData>(m_checkpoint, m_lastError);
		}


		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
		return result;
	}

} /* namespace state */
}/* namespace checkpoint */
}/* namespace store */
