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
#include "daemon.h"

#include "haimplementer.h"
#include "engine/workingset.h"
#include "operation/creator.h"

#include "common/tracer.h"
#include "common/logger.h"
#include "common/programconstants.h"

#include <sys/file.h>
#include <sys/types.h>
#include <sys/capability.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Daemon)

namespace
{
	static std::string ddt_server_lock_file_path("/var/run/ap/aes_ddt.lck");
	static std::string ha_daemon_name("aes_ddtd");
	static std::string logger_appender_name("AES_DDTD");
}

int Daemon::m_systemSignalsToCatch[] = { SIGHUP, SIGINT, SIGTERM };

Daemon::Daemon()
:ACE_Task_Base(),
 m_signalHandler(),
 m_SignalsToCatch()
{

}

int Daemon::svc(void)
{
	AES_DDT_TRACE_FUNCTION;
	AES_DDT_LOG(LOG_LEVEL_DEBUG, "Starting Daemon worker thread...");

	// TODO Add a loop with state check
	engine::workingSet_t::instance()->getMainReactor().run_reactor_event_loop();

	AES_DDT_LOG(LOG_LEVEL_DEBUG, "...Terminated Daemon worker thread");
	return common::errorCode::ERR_NO_ERRORS;
}


int Daemon::work(const int& debug_mode)
{
	AES_DDT_TRACE_FUNCTION;

	int result = common::errorCode::ERR_NO_ERRORS;

	if(debug_mode)
	{
		// Check for multiple program instances running
		if ( (result = multiple_process_instance_running_check()) ) return result;

		engine::workingSet_t::instance()->setDebugModeOn();

		// TEST for SIGPIPE problem!!
		sigignore(SIGPIPE);

		//Start Server in debug mode
		result = debug();

	}
	else
	{
		pid_t parent_pid = ::getpid();

		// Demonize the server and prepare to register with AMF
		HaImplementer haImplementer(ha_daemon_name.c_str());

		// TEST for SIGPIPE problem!!
		sigignore(SIGPIPE);

		checkDemonizeResult(parent_pid);

		engine::workingSet_t::instance()->setDebugModeOff();

		if(set_process_capability())
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Error while setting process capabilities. errno: %d", errno);
			AES_DDT_TRACE_MESSAGE("Error while setting process capabilities. errno: %d", errno);

			AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Error while setting process capabilities");
		}

		// Initialize the server logger
		aes_ddt_logger::open(logger_appender_name.c_str());

		//////////////////////////////////////////////////
		//Initialize Operation Dispatcher
		if(engine::workingSet_t::instance()->startScheduler() != common::errorCode::ERR_NO_ERRORS)
		{
			AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot initialize the Operation Dispatcher thread");
			AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot initialize the Operation Dispatcher thread");
			//TODO exit in this case?
		}
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		// Activate this server work task with the minimum number of thread running
		if(activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED) != common::errorCode::ERR_NO_ERRORS)
		{
			// ERROR: activating this main task
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_FATAL, "Call 'activate' failed: cannot activate the daemon worker thread");
			AES_DDT_TRACE_MESSAGE("FATAL ERROR: Call 'activate' failed: cannot activate the daemon worker thread");
			//TODO exit in this case?
		}
		//////////////////////////////////////////////////

		//////////////////////////////////////////////////
		//Initialize signal handler
		if( init_signals_handler() != common::errorCode::ERR_NO_ERRORS)
		{
			AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot initialize the signal handler");
			AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot initialize the signal handler");
			//TODO exit in this case?
		}

		//////////////////////////////////////////////////

		//activate HA APP MANAGER to bind to the AMF framework
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Activating HA Implementer...");

		ACS_APGCC_HA_ReturnType ha_call_result = haImplementer.activate();

		logHAExitCode(ha_call_result);

		//////////////////////////////////////////////////
		//remove signal handler
		if( reset_signals_handler() != common::errorCode::ERR_NO_ERRORS)
		{
			AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot remove the signal handler");
			AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot remove the signal handler");
			//TODO exit in this case?
		}
		//////////////////////////////////////////////////

		//Stop Scheduler
		if (engine::workingSet_t::instance()->stopScheduler() != common::errorCode::ERR_NO_ERRORS)
		{
			AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot stop the operation scheduler thread");
			AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot stop the operation scheduler thread");
		}

		// wait svc thread termination
		wait();

	}

	aes_ddt_logger::close();
	return result;
}

int Daemon::debug ()
{
	AES_DDT_TRACE_FUNCTION;

	printWelcomeMessage();

	int call_result = common::errorCode::ERR_NO_ERRORS;

	if (set_process_capability())
	{
		AES_DDT_TRACE_MESSAGE("Error while setting process capabilities. errno: %d", errno);
		syslog(LOG_ERR, "Error while setting process capabilities for %s. errno: %d", ha_daemon_name.c_str(), errno);
	}

	// Initialize the server logger
	aes_ddt_logger::open(logger_appender_name.c_str());

	//////////////////////////////////////////////////
	//Initialize Operation Dispatcher
	if (engine::workingSet_t::instance()->startScheduler() != common::errorCode::ERR_NO_ERRORS)
	{
		AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot initialize the Operation Dispatcher thread");
		AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot initialize the Operation Dispatcher thread");
		//TODO exit in this case?
	}

	//////////////////////////////////////////////////
	//Initialize signal handler
	if( init_signals_handler() != common::errorCode::ERR_NO_ERRORS)
	{
		AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot initialize the signal handler");
		AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot initialize the signal handler");
		//TODO exit in this case?
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	// Simulate HA activate
	{
		operation::Creator operationFactoryCreator;
		operationFactoryCreator.schedule(operation::START);
	}
	//////////////////////////////////////////////////

	// TODO Add a loop with state check
	engine::workingSet_t::instance()->getMainReactor().run_reactor_event_loop();

	//////////////////////////////////////////////////
	// Simulate HA deactivate
	{
		operation::Creator operationFactoryCreator;
		operationFactoryCreator.schedule(operation::STOP);
	}
	//////////////////////////////////////////////////

	//////////////////////////////////////////////////
	//remove signal handler
	if( reset_signals_handler() != common::errorCode::ERR_NO_ERRORS)
	{
		AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot remove the signal handler");
		AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot remove the signal handler");
	}
	//////////////////////////////////////////////////

	//Stop Scheduler
	if (engine::workingSet_t::instance()->stopScheduler() != common::errorCode::ERR_NO_ERRORS)
	{
		AES_DDT_LOG(LOG_LEVEL_FATAL, "Daemon cannot stop the operation scheduler thread");
		AES_DDT_TRACE_MESSAGE("FATAL ERROR: Daemon cannot stop the operation scheduler thread");
	}

	return call_result;
}


int Daemon::multiple_process_instance_running_check () {
	AES_DDT_TRACE_FUNCTION;

	// Multiple server instance check: if there is another server instance
	// already running then exit immediately

	int lock_fd = ::open(ddt_server_lock_file_path.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0664);
	if (lock_fd < 0)
	{
		AES_DDT_TRACE_MESSAGE("FILE SYSTEM ERROR");
		return common::PROGRAM_EXIT_LOCK_FILE_OPEN_ERROR;
	}
	errno = 0;
	if (::flock(lock_fd, LOCK_EX | LOCK_NB) < 0)
	{
		int errno_save = errno;
		if (errno_save == EWOULDBLOCK)
		{
			::fprintf(::stderr, "Another Server instance running\n");
			AES_DDT_TRACE_MESSAGE("Another Server instance running");
			return common::PROGRAM_EXIT_ANOTHER_SERVER_RUNNING;
		}

		AES_DDT_TRACE_MESSAGE("Error locking file. errno: %d", errno);
		return common::PROGRAM_EXIT_LOCK_FILE_LOCKING_ERROR;
	}

	return common::errorCode::ERR_NO_ERRORS;
}

int Daemon::set_process_capability()
{
	// Clear CAP_SYS_RESOURCE bit thus root user cannot override disk quota limits
	int retStatus = 0;
	cap_t cap = cap_get_proc();
	if(NULL != cap)
	{
		cap_value_t cap_list[] = {CAP_SYS_RESOURCE};
		size_t NumberOfCap = sizeof(cap_list)/sizeof(cap_list[0]);

		// Clear capability CAP_SYS_RESOURCE
		if(cap_set_flag(cap, CAP_EFFECTIVE, NumberOfCap, cap_list, CAP_CLEAR) == -1)
		{
			// handle error
			char err_buff[128] = {0};
			snprintf(err_buff, sizeof(err_buff) - 1, "%s, cap_set_flag() failed, error=%s", __func__, strerror(errno) );
			retStatus = -1;
			syslog(LOG_ERR, err_buff);

		}
		if (cap_set_flag(cap, CAP_INHERITABLE, NumberOfCap, cap_list, CAP_CLEAR) == -1)
		{
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1, "%s, cap_set_flag() failed, error=%s", __func__, strerror(errno));
			retStatus = -1;
			syslog(LOG_ERR, err_buff);

		}
		// Change process capability
		if (cap_set_proc(cap) == -1)
		{
			// handle error
			char err_buff[128] = {0};
			snprintf(err_buff, sizeof(err_buff) - 1, "%s, cap_set_proc() failed, error=%s", __func__, strerror(errno) );
			retStatus = -1;
			syslog(LOG_ERR, err_buff);
		}

		if(cap_free(cap) == -1)
		{
			// handle error
			char err_buff[128] = {0};
			snprintf(err_buff, sizeof(err_buff) - 1, "%s, cap_free() failed, error=%s", __func__, strerror(errno) );
			retStatus = -1;
			syslog(LOG_ERR, err_buff);
		}
	}
	else
	{
		// handle error
		char err_buff[128] = {0};
		snprintf(err_buff, sizeof(err_buff)-1, "%s, cap_get_proc() failed, error=%s", __func__, strerror(errno) );
		retStatus = -1;
		syslog(LOG_ERR, err_buff);
	}

	syslog(LOG_INFO, "Capabilities modification status: %s", (retStatus == 0 ? "DONE" : "ERROR"));

	return retStatus;
}

int Daemon::init_signals_handler()
{
	AES_DDT_TRACE_FUNCTION;

	int result = common::errorCode::ERR_NO_ERRORS;

	// Adding the process signal handler for the signals DDT server has to catch
	for(size_t signalIndex = 0U; signalIndex < AES_DDT_ARRAY_SIZE(m_systemSignalsToCatch); ++signalIndex)
	{
		int signal = m_systemSignalsToCatch[signalIndex];

		// Try to add the process signal handler for signal
		if(!m_SignalsToCatch.is_member(signal))
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Process signal handling: adding the handler for the signal: [ <%d>, <%s>]", signal, ::strsignal(signal));

			// signal not present, adding it
			if( engine::workingSet_t::instance()->getMainReactor().register_handler(signal, &m_signalHandler) < 0)
			{
				//ERROR: adding the signal handler
				result = common::errorCode::ERR_REACTOR_HANDLER_REGISTER_FAILURE;
				AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Process signal handling: call 'register_handler' failed: cannot register the handler for the signal: [<%d>, <%s>]", signal, ::strsignal(signal));
			}
			else
			{
				//OK: remember this signal was added.
				m_SignalsToCatch.sig_add(signal);
			}
		}
	}

	return result;
}

int Daemon::reset_signals_handler()
{
	AES_DDT_TRACE_FUNCTION;

	int result = common::errorCode::ERR_NO_ERRORS;

	// Removing the process signal handling
	for( size_t signalIndex = 0U; signalIndex < AES_DDT_ARRAY_SIZE(m_systemSignalsToCatch); ++signalIndex )
	{
		int signal = m_systemSignalsToCatch[signalIndex];

		// Try to remove the process signal handler for signal
		if( m_SignalsToCatch.is_member(signal) )
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Process signal handling: removing the handler for the signal: [<%d>, <%s>]", signal, ::strsignal(signal));

			// signal present, removing it
			if( engine::workingSet_t::instance()->getMainReactor().remove_handler(signal, reinterpret_cast<ACE_Sig_Action *>(0)) < 0 )
			{
				//ERROR: removing the signal handler
				result = common::errorCode::ERR_REACTOR_HANDLER_REMOVE_FAILURE;
				AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "PProcess signal handling: call 'remove_handler' failed: cannot remove the handler for the signal: [<%d>, <%s>]", signal, ::strsignal(signal));
			}
			else
			{
				//OK: remember this signal was removed.
				m_SignalsToCatch.sig_del(signal);
			}
		}
	}

	return result;
}

void Daemon::checkDemonizeResult(const pid_t& parent_pid)
{
	AES_DDT_TRACE_FUNCTION;
	pid_t child_pid = ::getpid();

	if (parent_pid != child_pid )
	{
		// OK: server successfully demonized
		AES_DDT_LOG(LOG_LEVEL_DEBUG,
					"server successfuly demonized: new child PID == %d",
					child_pid); AES_DDT_TRACE_MESSAGE("server successfuly demonized: new child PID == %d", child_pid);
	}
	else
	{
		// ERROR: server was not demonized
		AES_DDT_LOG(
				LOG_LEVEL_WARN,
				"Server was not demonized correctly: child process was aborted on creation: the parent process (PID == %d) continues taking the control",
				parent_pid);

		AES_DDT_TRACE_MESSAGE(
				"Server was not demonized correctly: child process was aborted on creation: the parent process (PID == %d) continues taking the control",
				parent_pid);
	}
}


void Daemon::logHAExitCode(int ha_result)
{
	AES_DDT_TRACE_FUNCTION;

	switch (ha_result)
	{
		case ACS_APGCC_HA_SUCCESS:
			AES_DDT_SYSLOG(LOG_DEBUG, LOG_LEVEL_DEBUG,
						   "HA Application Gracefully closing...");
			break;
		case ACS_APGCC_HA_FAILURE:
			AES_DDT_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR,
						   "HA Activation Failed: ha_call_result == %d",
						   ha_result);
			break;
		case ACS_APGCC_HA_FAILURE_CLOSE:
			AES_DDT_SYSLOG(LOG_ERR, LOG_LEVEL_ERROR,
					"HA Application Failed to Gracefully closing: ha_call_result == %d",
					ha_result);
			break;
		default:
			AES_DDT_SYSLOG(LOG_WARNING, LOG_LEVEL_WARN,
					"HA Application error code unknown: ha_call_result == %d",
					ha_result);
			break;
	}

	AES_DDT_LOG(LOG_LEVEL_DEBUG,
				"HA Implementer STOPPED!!! ha_call_result = <%d>",
				ha_result);
}

void Daemon::printWelcomeMessage()
{
	AES_DDT_TRACE_MESSAGE("\n\n"
			"=========================\n"
			"=     SERVER RUNNING    =\n"
			"=========================\n"
			"=       ,__o            =\n"
			"=       _-\\_<,         =\n"
			"=      (*)/'(*)         =\n"
			"=========================\n");

	std::cout << std::endl;
	std::cout << "        ,__o" << std::endl;
	std::cout << "       _-\\_<," << std::endl;
	std::cout << "      (*)/'(*)" << std::endl;
	std::cout << std::endl;

	time_t		now = time(0);
	struct tm	tstruct;
	char		buf[80] = {0};

	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%d/%m/%Y %X", &tstruct);

	if(tstruct.tm_hour > 12 && tstruct.tm_hour < 18) std::cout << buf << " Good Afternoon!" << std::endl;
	else if((tstruct.tm_hour > 18) && (tstruct.tm_hour < 21) ) std::cout << buf << " Good Evening!" << std::endl;
	else if(tstruct.tm_hour > 21) std::cout << buf << " Good Night!" << std::endl;
	else std::cout << buf << " Good Morning!" << std::endl;

}

