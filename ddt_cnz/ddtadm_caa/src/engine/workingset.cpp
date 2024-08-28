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
#include "engine/workingset.h"

#include "common/tracer.h"
#include "common/logger.h"


AES_DDT_TRACE_DEFINE(AES_DDT_Engine_WorkingSet)

namespace engine
{
	WorkingSet::WorkingSet()
	: m_debugMode(false),
	  m_scheduler(),
	  m_immHandler(),
	  m_dataSourceManager(),
	  m_storingManager(),
	  m_reactorImpl(),
	  m_reactor(&m_reactorImpl),
	  m_alarmHandler()
	{
		pid_t parent_pid = ::getpid();
		//AES_DDT_SYSLOG( LOG_ERR, LOG_LEVEL_ERROR, "WorkingSet created by DDT daemon pid:<%d>", parent_pid);
	};

	WorkingSet::~WorkingSet()
	{
		pid_t parent_pid = ::getpid();
		//AES_DDT_SYSLOG( LOG_ERR, LOG_LEVEL_ERROR, "WorkingSet deleted by DDT daemon pid:<%d>", parent_pid);
	}
}
