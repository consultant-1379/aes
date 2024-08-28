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

#include "alarm/alarmhandler.h"
#include "alarm/alarm.h"
#include "alarm/datasourcewarning.h"
#include "alarm/connectionfaultalarm.h"
#include "imm/imm.h"
#include "engine/workingset.h"
#include "common/utility.h"
#include "common/programconstants.h"
#include "common/tracer.h"
#include "common/logger.h"



#include <ACS_APGCC_Util.H>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>

AES_DDT_TRACE_DEFINE(AES_DDT_Alarm_AlarmHandler)

namespace alarms
{

	AlarmHandler::AlarmHandler():
		m_processName(),
		m_evrep(),
		m_alarms(),
		m_mutex()
	{
		ACS_APGCC::getProcessName(&m_processName);
	}

	AlarmHandler::~AlarmHandler()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void AlarmHandler::raiseDataSourceWarning(const std::string& objOfReference, const int& threshold)
	{
		AES_DDT_TRACE_FUNCTION;
		boost::shared_ptr<alarms::Alarm> alarmData;

		if(!exists(objOfReference, Alarm::DATASOURCE_WARNING, alarmData))
		{
			AES_DDT_TRACE_MESSAGE("Raising DataSourceWarning alarm");

			alarmData = boost::make_shared<alarms::DataSourceWarning>(objOfReference, threshold);

			if(!raise(alarmData))
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to raise Data Source Warning, objectReference:<%s>",
						alarmData->getObjName());
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to raise Data Source Warning, objectReference:<%s>",
						alarmData->getObjName());
			}
		}
	}

	void AlarmHandler::ceaseDataSourceWarning(const std::string& objOfReference)
	{
		AES_DDT_TRACE_FUNCTION;

		boost::shared_ptr<alarms::Alarm> alarmData;

		if(exists(objOfReference, Alarm::DATASOURCE_WARNING, alarmData))
		{
			AES_DDT_TRACE_MESSAGE("Ceasing DataSourceWarning alarm");

			if(!cease(alarmData))
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to cease DataSourceWarning alarm, objectReference:<%s>",
						alarmData->getObjName());
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to cease DataSourceWarning alarm, objectReference:<%s>",
						alarmData->getObjName());
			}
		}
	}

	void AlarmHandler::raiseConnectionFaultAlarm(const std::string& objOfReference)
	{
		AES_DDT_TRACE_FUNCTION;
		boost::shared_ptr<alarms::Alarm> alarmData;

		if(!exists(objOfReference, Alarm::CONNECTION_FAULT, alarmData))
		{
			alarmData = boost::make_shared<alarms::ConnectionFaultAlarm>(objOfReference);

			if(!raise(alarmData))
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to raise connection fault alarm for <%s>", objOfReference.c_str());
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to raise connection fault alarm for <%s>", objOfReference.c_str());
			}
		}
	}

	void AlarmHandler::ceaseConnectionFaultAlarm(const std::string& objOfReference)
	{
		AES_DDT_TRACE_FUNCTION;

		boost::shared_ptr<alarms::Alarm> alarmData;

		if(exists(objOfReference, Alarm::CONNECTION_FAULT, alarmData))
		{
			if(!cease(alarmData))
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to cease connection fault alarm for <%s>", objOfReference.c_str());
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to cease connection fault alarm for <%s>", objOfReference.c_str());
			}
		}

	}

	bool AlarmHandler::raise(boost::shared_ptr<Alarm> alarmData )
	{
		// Synchronize alarm map
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		bool result = false;
		ACS_AEH_ReturnType status =  m_evrep.sendEventMessage( m_processName.c_str(),
				alarmData->getSpecificProblem(),
				alarmData->getPercSeverity(),
				alarmData->getProbableCause(),
				alarmData->getObjectClassOfReference(),
				alarmData->getObjectOfReference(),
				alarmData->getProblemData(),
				alarmData->getProblemText() );

		if( (result = (ACS_AEH_ok == status)) )
		{
			AES_DDT_LOG(LOG_LEVEL_INFO, "Alarm <%zu> raised for :<%s> ", alarmData->getSpecificProblem(), alarmData->getObjName());
			AES_DDT_TRACE_MESSAGE("Alarm <%zu> raised for :<%s> ", alarmData->getSpecificProblem(), alarmData->getObjName());

			m_alarms[std::make_pair(alarmData->getObjName(), alarmData->getType())] = alarmData;
		}

		return result;
	}

	bool AlarmHandler::cease(boost::shared_ptr<Alarm> alarmData )
	{
		// Synchronize alarm map
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		bool result = false;
		ACS_AEH_ReturnType status =  m_evrep.sendEventMessage( m_processName.c_str(),
				alarmData->getSpecificProblem(),
				alarmData->getCeasingSeverity(),
				alarmData->getProbableCause(),
				alarmData->getObjectClassOfReference(),
				alarmData->getObjectOfReference(),
				alarmData->getProblemData(),
				alarmData->getProblemText() );

		if( (result = (ACS_AEH_ok == status)) )
		{
			AES_DDT_LOG(LOG_LEVEL_INFO, "Alarm <%zu> ceased for :<%s>", alarmData->getSpecificProblem(), alarmData->getObjName());
			AES_DDT_TRACE_MESSAGE("Alarm <%zu> ceased for :<%s> ", alarmData->getSpecificProblem(), alarmData->getObjName());

			m_alarms.erase(std::make_pair(alarmData->getObjName(), alarmData->getType()));
			alarmData.reset();
		}
		return result;
	}

	bool AlarmHandler::exists(const std::string& objName, const Alarm::Type& type, boost::shared_ptr<Alarm>& alarmData)
	{
		// Synchronize alarm map
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);
		bool exists = false;

		// check for alarm, if not present then raise.
		mapOfAlarms_t::const_iterator element = m_alarms.find(std::make_pair(objName, type));

		if( (exists = m_alarms.end() != element) )
		{
			AES_DDT_LOG(LOG_LEVEL_INFO, "Alarm of type <%d> found for :<%s> ", type, objName.c_str());
			AES_DDT_TRACE_MESSAGE("Alarm of type <%d> found for :<%s> ", type, objName.c_str());
			alarmData = element->second;
		}
		return exists;
	}

	void AlarmHandler::ceaseAll()
	{
		AES_DDT_TRACE_FUNCTION;

		mapOfAlarms_t::const_iterator element = m_alarms.begin();
		for(; m_alarms.end() != element; ++element)
		{
			cease(element->second);
		}

		m_alarms.clear();
	}

} /* namespace alarm */
