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
#include "stubs/state_stub.h"
#include "iostream"

namespace store {
namespace checkpoint {
namespace state {

	Open::Open(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, OPEN)
	{

	}

	Open::~Open()
	{

	}

	int Open::handleStep()
	{
		m_lastError = m_checkpoint->openCheckpoint();
		return m_lastError;
	}

	bool Open::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		bool result = true;

		if( common::errorCode::ERR_NO_ERRORS == m_lastError )
		{
			nextStep = boost::make_shared<ReadInfo>(m_checkpoint);
		}
		else
		{
			result = false;
		}

		return result;
	}

	Stop::Stop(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, STOP)
	{
		m_lastError = common::errorCode::ERR_CKPT_STOPPED;
	}

	Stop::~Stop()
	{

	}

	int Stop::handleStep()
	{
		return m_lastError;
	}

	bool Stop::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		bool result = true;

		return result;
	}

	Close::Close(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, CLOSE)
	{

	}

	Close::~Close()
	{

	}

	int Close::handleStep()
	{
		m_lastError = m_checkpoint->closeCheckpoint();
		return m_lastError;
	}

	bool Close::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		bool result = true;

		if( common::errorCode::ERR_NO_ERRORS == m_lastError )
		{
		//	nextStep = boost::make_shared<GetNew>(m_checkpoint);
		}
		else
		{
			result = false;
		}
		return result;
	}

	ReadInfo::ReadInfo(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, READINFO)
	{

	}

	ReadInfo::~ReadInfo()
	{

	}

	int ReadInfo::handleStep()
	{
		m_lastError = m_checkpoint->readInfoSection();
		return m_lastError;
	}

	bool ReadInfo::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		bool result = true;

		if( common::errorCode::ERR_NO_ERRORS == m_lastError )
		{
			nextStep = boost::make_shared<Write>(m_checkpoint);
		}
		else
		{
			result = false;
		}


		return result;
	}

	Write::Write(StoreBase* checkpointObj)
	: BaseInterface(checkpointObj, WRITE)
	{

	}

	Write::~Write()
	{

	}

	int Write::handleStep()
	{
		m_lastError = m_checkpoint->isCheckPointFull();
		return m_lastError;
	}

	bool Write::getNextStep(boost::shared_ptr<BaseInterface>& nextStep)
	{
		bool result = true;

		if( common::errorCode::ERR_CKPT_SECTIONS_FULL == m_lastError )
		{
			//	nextStep = boost::make_shared<UpdateInfo>(m_checkpoint);
		}
		else
		{
			result = false;
		}

		return result;
	}
} /* namespace ddt_ckpt */
}
}
