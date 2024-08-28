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

#include "engine/timer.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/utility.h"
	#include "common/programconstants.h"
#endif

#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <poll.h>
//#include <ace/Task.h>


AES_DDT_TRACE_DEFINE(AES_DDT_Engine_Timer)

namespace engine
{

	Timer::Timer()
	: m_handle(common::event::INVALID),
	  m_status(DISARMED)
	{
		AES_DDT_TRACE_FUNCTION;
		createHandle();
	}

	Timer::~Timer()
	{
		AES_DDT_TRACE_FUNCTION;
		if(common::event::INVALID != m_handle)
			::close(m_handle);
	}

	void Timer::createHandle()
	{
		if(common::event::INVALID == m_handle)
		{

			if( (m_handle = timerfd_create(CLOCK_MONOTONIC, 0)) > common::event::INVALID )
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "Created Timer Handle, fd:<%d>", m_handle);
				AES_DDT_TRACE_MESSAGE("Created Timer Handle, fd:<%d>", m_handle);
			}
			else
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot create Timer Handle");
				AES_DDT_TRACE_MESSAGE("ERROR: Cannot create Timer Handle. errno: %d", errno);
			}
		}
	}

	Timer::TimerStatus Timer::start(int timeSec)
	{
		if( ARMED != m_status )
		{
			createHandle();

			struct itimerspec timeout;
			timeout.it_value.tv_sec = timeSec;
			timeout.it_value.tv_nsec = 0U;

			timeout.it_interval.tv_sec = 0U;
			timeout.it_interval.tv_nsec = 0U;

			// set the timer
			m_status = (timerfd_settime(m_handle, 0, &timeout, NULL) >= 0)? ARMED: DISARMED;

			if( ARMED != m_status )
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to start timer, fd:<%d>", m_handle);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to start timer, fd:<%d>", m_handle);
			}

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Armed Timer, <%d> secs, start result: <%s>", timeSec, common::utility::boolToString(ARMED == m_status));
			AES_DDT_TRACE_MESSAGE("Armed Timer, <%d> secs, start result: <%s>", timeSec, common::utility::boolToString(ARMED == m_status));
		}

		return m_status;
	}

	Timer::TimerStatus Timer::stop()
	{
		AES_DDT_TRACE_FUNCTION;

		if( DISARMED != m_status )
		{
			struct itimerspec timeout;
			timeout.it_value.tv_sec = timeout.it_value.tv_nsec = 0U;

			timeout.it_interval.tv_sec = timeout.it_interval.tv_nsec = 0U;

			// set the timer
			m_status = (timerfd_settime(m_handle, 0, &timeout, NULL) >= 0)? DISARMED: ARMED;

			if( DISARMED != m_status )
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to stop timer, fd:<%d>", m_handle);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to stop timer, fd:<%d>", m_handle);
			}

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Timer stop result:<%s>", common::utility::boolToString(DISARMED == m_status));
			AES_DDT_TRACE_MESSAGE("Timer stop result:<%s>", common::utility::boolToString(DISARMED == m_status));
		}

		return m_status;

	}

	bool Timer::isExpired()
	{
		bool expired = false;
		if(common::event::INVALID != m_handle)
		{
			nfds_t nfds = 1;
			int holdTime_pos = 0;
			struct pollfd fds[nfds];
			// Initialize the pollfd structure
			memset(fds, 0, sizeof(fds));

			fds[holdTime_pos].fd = m_handle;
			fds[holdTime_pos].events = POLLIN;

			int pollResult = poll(fds, nfds, 0);
			// poll error check
			if(pollResult < 0)
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Error on poll! pollResult:<%d>", pollResult);
				AES_DDT_TRACE_MESSAGE("Error on poll! pollResult:<%d>, errno:<%d>", pollResult, errno);
			}
			else if (0 == pollResult)
			{
				//timeout expired (0 seconds)
				AES_DDT_TRACE_MESSAGE("TIMER IS NOT EXPIRED YET...");
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("TIMER EXPIRED!! Status: <%d>", m_status);
				expired = true;
			}
		}

		return expired;
	}

} /* namespace engine */
