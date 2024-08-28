#include <aes_cdh_daemon.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
AES_CDH_TRACE_DEFINE(AES_CDH_DAEMON);

aes_cdh_daemon::aes_cdh_daemon(const char* name)
:ACS_APGCC_Daemon(name)
{
}

aes_cdh_daemon::~aes_cdh_daemon()
{
}

int aes_cdh_daemon::StartService()
{

	AES_CDH_TRACE_MESSAGE("Entering");
	if( ACS_APGCC_Daemon::instance()->reactor_ != 0 )
	{
		reactor_->owner(ACE_Thread::self());
		reactor_->run_reactor_event_loop();
	}
	return 0;
}

int aes_cdh_daemon::StopService()
{
	AES_CDH_TRACE_MESSAGE(" closing cdh functionality");
	reactor_->end_reactor_event_loop();
	AES_CDH_TRACE_MESSAGE(" end_event_loop done");
	int count = reactor_->purge_pending_notifications(0); // Purge any waiting notifications
	if (count > 0) {
		AES_CDH_LOG(LOG_LEVEL_INFO, "UNEXPECTED, Exiting with waiting notifications!!!");
	}
	usleep(500 * 1000); //sleep needed for delay in close APGCC reactor // sometime
	this->wait();

	if ( ACS_APGCC_Daemon::instance() != 0 )
	{
		AES_CDH_TRACE_MESSAGE("Resetting APGCC reactor");
		reactor_->reset_reactor_event_loop(); //needed to restart reactor
		AES_CDH_TRACE_MESSAGE(" Closed APGCC reactor");
	}

	return 0;
}
