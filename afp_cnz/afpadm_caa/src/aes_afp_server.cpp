//	DESCRIPTION:
//				AP File Processing (AFP) handles automatic
//				transmission and removal of subfiles, simple files
//				and AP native files.
//				aes_afp_server is the service/daemon that handles
//				the background work of AFP.


//-------------------------------------------------------------------
//  AFP_ServiceMain()
//-------------------------------------------------------------------
#include <aes_afp_server.h>
#include <aes_afp_ha_service.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/capability.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
#define AFP_ServiceName "AES_AFP_server"
AES_AFP_TRACE_DEFINE(AES_AFP_ServiceMain);
namespace {

bool setProcessCapability() {
	// Clear CAP_SYS_RESOURCE bit thus root user cannot override disk quota limits
	bool retStatus = true;
	cap_t cap = cap_get_proc();
	if (NULL != cap) {
		cap_value_t cap_list[] = { CAP_SYS_RESOURCE };
		size_t NumberOfCap = sizeof(cap_list)/sizeof(cap_list[0]);

		// Clear capability CAP_SYS_RESOURCE
		if (cap_set_flag(cap, CAP_EFFECTIVE, NumberOfCap, cap_list, CAP_CLEAR) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_set_flag() failed, error=%s", __func__, strerror(
							errno));
			retStatus = false;
			AES_AFP_LOG(LOG_LEVEL_ERROR, err_buff);
			AES_AFP_TRACE_MESSAGE(err_buff);

		}

		if (cap_set_flag(cap, CAP_INHERITABLE, NumberOfCap, cap_list, CAP_CLEAR) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_set_flag() failed, error=%s", __func__, strerror(
							errno));
			retStatus = false;
			AES_AFP_LOG(LOG_LEVEL_ERROR,err_buff);
			AES_AFP_TRACE_MESSAGE(err_buff);

		}
		// Change process capability
		if (cap_set_proc(cap) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_set_proc() failed, error=%s", __func__, strerror(
							errno));
			retStatus = false;
			AES_AFP_LOG(LOG_LEVEL_ERROR, err_buff);
			AES_AFP_TRACE_MESSAGE(err_buff);
		}

		if (cap_free(cap) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_free() failed, error=%s", __func__,
					strerror(errno));
			retStatus = false;
			AES_AFP_LOG(LOG_LEVEL_ERROR, err_buff);
			AES_AFP_TRACE_MESSAGE(err_buff);

		}
	} else {
		// handle error
		char err_buff[128] = { 0 };
		snprintf(err_buff, sizeof(err_buff) - 1,
				"%s, cap_get_proc() failed, error=%s", __func__,
				strerror(errno));
		retStatus = false;
		AES_AFP_LOG(LOG_LEVEL_ERROR,err_buff);
		AES_AFP_TRACE_MESSAGE(err_buff);
	}
	AES_AFP_TRACE_MESSAGE("Capabilities modification status %u:", retStatus);
	return retStatus;
}
}

aes_afp_services *afpServices = 0;
//-------------------------------------------------------------------
//  usage()
//-------------------------------------------------------------------
void usage() {
	cout << "Usage: aes_afp_server -d <in debug mode> " << endl;
}

void sighandler(int signum) {
	if (signum == SIGTERM || signum == SIGINT || signum == SIGTSTP) {
		AES_AFP_LOG(LOG_LEVEL_FATAL, "***** afp kill is executed *****");
		AES_AFP_TRACE_MESSAGE("***** afp kill is executed ******");
		if (afpServices != 0) {
			AES_AFP_TRACE_MESSAGE("STOP EVENT SIGNALED");
			afpServices->StopEvent->signal();
			aes_afp_services::afpStopEvt = true;
		}
	}
}
//-------------------------------------------------------------------
//  main()
//-------------------------------------------------------------------
int main(int argc, char *argv[]) {
	AES_AFP_TRACE_MESSAGE("argc = %d,  argv = %s", argc, argv[1]);
	signal(SIGPIPE, SIG_IGN);
	if (argc > 1) {

		if (argc > 2) {
			usage();
			return 1;
		} else if (argc == 2 && !(strcmp(argv[1], "-d"))) {

			struct sigaction sa;
			memset(&sa,0,sizeof(sa));
			sa.sa_handler = sighandler;
			sa.sa_flags = 0;
			sigemptyset(&sa.sa_mask);

			if (sigaction(SIGINT, &sa, NULL) == -1) {
				cout << "Error occurred while handling SIGINT" << endl;
				return -1;
			}
			if (sigaction(SIGTERM, &sa, NULL) == -1) {
				cout << "Error occurred while handling SIGTERM" << endl;
				return -1;
			}
			if (sigaction(SIGTSTP, &sa, NULL) == -1) {
				cout << "Error occurred while handling SIGTSTP" << endl;
				return -1;
			}
			aes_afp_logger::open("AFP");
			if (!setProcessCapability()) {
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Failed to drop capabilities for AFP");
				AES_AFP_TRACE_MESSAGE("Failed to drop capabilities for AFP");
			}
			afpServices = new aes_afp_services();
			int mythreadGroupId = ACE_Thread_Manager::instance()->spawn(
					&afpServices->AFPThreadFunctionalMethod,
					(void*) afpServices, THR_NEW_LWP | THR_JOINABLE
					| THR_INHERIT_SCHED,
					&afpServices->m_afpMainThreadId, 0,
					ACE_DEFAULT_THREAD_PRIORITY, -1, 0,
					ACE_DEFAULT_THREAD_STACKSIZE);
			if (mythreadGroupId == -1) {
				AES_AFP_LOG(LOG_LEVEL_ERROR, "aes_afp_server::Error occured while spawning main thread.");
				AES_AFP_TRACE_MESSAGE("aes_afp_server::Error occured while spawning main thread.");
				delete afpServices;
				afpServices = 0;
				return -1;
			}
			ACE_Thread_Manager::instance()->join(afpServices->m_afpMainThreadId);

			delete afpServices;
			afpServices = 0;
		}
	} else {
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;
		aes_afp_logger::open("AFP");

		//Allocate memory for MCS_AIAP_HA_Service
		AES_AFP_HA_Service* afp_ha_service = new AES_AFP_HA_Service("aes_afpd", "root");
		if (!setProcessCapability()) {
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to drop capabilities for AFP");
			AES_AFP_TRACE_MESSAGE("Failed to drop capabilities for AFP");
		}
		//Exit, if memory allocation fails.

		if (afp_ha_service == 0) {
			AES_AFP_LOG(LOG_LEVEL_ERROR, " Memory allocation failed for afpAdmPtr");
			AES_AFP_TRACE_MESSAGE(" Memory allocation failed for afpAdmPtr");
			AES_AFP_TRACE_MESSAGE(" Leaving aes_afpd");
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_INFO, "Starting aes_afpd service..");
			AES_AFP_TRACE_MESSAGE("Starting aes_afpd service..");
			errorCode = afp_ha_service->activate();

			if (errorCode == ACS_APGCC_HA_FAILURE) {
				AES_AFP_LOG(LOG_LEVEL_ERROR, " HA Activation Failed for aes_afpd ");
				AES_AFP_TRACE_MESSAGE(" HA Activation Failed for aes_afpd ");
			} else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE) {
				AES_AFP_LOG(LOG_LEVEL_ERROR, " HA Application Failed to Gracefullly closed for aes_afpd ");
				AES_AFP_TRACE_MESSAGE(" HA Application Failed to Gracefullly closed for aes_afpd ");
			} else if (errorCode == ACS_APGCC_HA_SUCCESS) {
				AES_AFP_TRACE_MESSAGE(" HA Application Gracefully closed for aes_afpd ");
			} else {
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Error occurred while integrating aes_afpd with HA ");
				AES_AFP_TRACE_MESSAGE("Error occurred while integrating aes_afpd with HA ");
			}
			delete afp_ha_service;
			afp_ha_service = 0;
		}
	}
	return 0;
}

// EOF aes_afp_server.cpp
