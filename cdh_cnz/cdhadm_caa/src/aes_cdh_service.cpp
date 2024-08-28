/*=================================================================== */
/**
 @file   aes_cdh_service.cpp

 Class method implementation for AES_CDH_Service type module.

 This module contains the implementation of class declared in
 the aes_cdh_service.h module

 @version 1.0.0
 */
/*
 HISTORY
 This section contains reference to problem report and related
 software correction performed inside this module


 PR           DATE      INITIALS    DESCRIPTION
 -----------------------------------------------------------
 N/A       05/10/2011   XCHEMAD     Initial Release
 ==================================================================== */
/*===================================================================
 INCLUDE DECLARATION SECTION
 =================================================================== */

// C++ header files

// Internal header files
#include <aes_cdh_service.h>
#include <servr.h>
#include <syslog.h>
#include <aes_cdh_ha_service.h>
#include <aes_cdh_daemon.h>
#include <aes_cdh_common.h>
#include <sys/types.h>
#include <sys/capability.h>
#include "aes_cdh_logger.h"
AES_CDH_HA_Service *pcdhObj = 0;
bool theInteractiveMode = false;
ServR *gServR = 0;
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
			AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR: %s", err_buff);

		}
		if (cap_set_flag(cap, CAP_INHERITABLE, NumberOfCap, cap_list, CAP_CLEAR) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_set_flag() failed, error=%s", __func__, strerror(
							errno));
			retStatus = false;
			AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR: %s", err_buff);
		}

		// Change process capability
		if (cap_set_proc(cap) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_set_proc() failed, error=%s", __func__, strerror(
							errno));
			retStatus = false;
			AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR: %s", err_buff);
		}

		if (cap_free(cap) == -1) {
			// handle error
			char err_buff[128] = { 0 };
			snprintf(err_buff, sizeof(err_buff) - 1,
					"%s, cap_free() failed, error=%s", __func__,
					strerror(errno));
			retStatus = false;
			AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR: %s", err_buff);
		}
	}
	else {
		// handle error
		char err_buff[128] = { 0 };
		snprintf(err_buff, sizeof(err_buff) - 1,
				"%s, cap_get_proc() failed, error=%s", __func__,
				strerror(errno));
		retStatus = false;
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR: %s", err_buff);
	}
	AES_CDH_LOG(LOG_LEVEL_INFO, "%s %u", "Capabilities modification status :", retStatus);
	return retStatus;
}
}

void sighandler(int signum) {
	if (signum == SIGTERM || signum == SIGINT || signum == SIGTSTP) {
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "CDH kill is executed");
		if (gServR != 0) {
			ServR::StopEvent->signal();
			ServR::isStopEventSignalled = true;
			gServR->close();
			sleep(1);
			gServR->StopService();

		}
	}
	if (signum == SIGPIPE) {
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "************** SIGPIPE RECEIVED *****************");
		printf("SIGPIPE received\n");
	}
}
void PrintUsage() {
	cout << "Usage:\naes_cdhd -d for debug mode." << endl;
}

ACE_THR_FUNC_RETURN CDH_serviceMain(void *ptr);

ACE_INT32 ACE_TMAIN(ACE_INT32 argc, ACE_TCHAR* argv[]) {
	if (argc > 1) {
		if ((argc == 2) && (strcmp(argv[1], "-d") == 0)) {
			aes_cdh_logger::open("CDH");

			//Register the signal handler

			struct sigaction sa;
			memset(&sa, 0, sizeof(sa));
			sa.sa_handler = sighandler;
			sa.sa_flags = 0;
			sigemptyset(&sa.sa_mask);

			if (sigaction(SIGINT, &sa, NULL) == -1) {
#ifdef DEBUGTEST
				cout << "Error occurred while handling SIGINT" << endl;
#endif
				return -1;
			}
			if (sigaction(SIGTERM, &sa, NULL) == -1) {
#ifdef DEBUGTEST
				cout << "Error occurred while handling SIGTERM" << endl;
#endif
				return -1;
			}
			if (sigaction(SIGTSTP, &sa, NULL) == -1) {
#ifdef DEBUGTEST
				cout << "Error occurred while handling SIGTSTP" << endl;
#endif
				return -1;
			}
			if (!setProcessCapability()) {
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error on modifying capabilities for aes_cdhd");
			}
			gServR = new ServR();
			if (gServR == 0) {
#ifdef DEBUGTEST
				cout << " Memory Allocated failed for servR " << endl;
#endif
				return 0;
			}

			ACE_thread_t applicationThreadId = 0;

			int threadGroupId = ACE_Thread_Manager::instance()->spawn(
					&CDH_serviceMain, (void *) gServR, THR_NEW_LWP
					| THR_JOINABLE | THR_INHERIT_SCHED,
					&applicationThreadId, 0, ACE_DEFAULT_THREAD_PRIORITY, -1,
					0, ACE_DEFAULT_THREAD_STACKSIZE);
			if (threadGroupId == -1) {
#ifdef DEBUGTEST
				cout << " Thread spawn failed. " << endl;
#endif
				delete gServR;
				gServR = 0;
				return 0;
			}

			ACE_Thread_Manager::instance()->join(applicationThreadId);
			AES_CDH_LOG(LOG_LEVEL_INFO, "Invoking the destructor for servR");
			delete gServR;
			gServR = 0;
			aes_cdh_logger::close();

		} else {
			PrintUsage();
		}
	}
	else if (argc == 1) {
		//HA Mode
		ACS_APGCC_HA_ReturnType errorCode = ACS_APGCC_HA_SUCCESS;

		//Create an object of aes_cdh_ha_service class with user as root.
		pcdhObj = new AES_CDH_HA_Service("aes_cdhd", "root");

		//The LOG open must be invoked after the HA instantiation
		aes_cdh_logger::open("CDH");

		if (!setProcessCapability()) {
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error on modifying capabilities for aes_cdhd");
			// Add Trace message
		}
		//Check for Memory allocation
		if (pcdhObj == 0) {
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for aes_cdhd");
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving aes_cdhd");
			aes_cdh_logger::close();
			return -1;
		}
		else {
			//Register with core middleware.
			AES_CDH_LOG(LOG_LEVEL_INFO, "Starting aes_cdhd service with HA.. ");

			errorCode = pcdhObj->activate();
			if (errorCode == ACS_APGCC_HA_FAILURE) {
				AES_CDH_LOG(LOG_LEVEL_ERROR, "HA Activation Failed for aes_cdhd");
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving aes_cdhd");
				delete pcdhObj;
				pcdhObj = 0;
				aes_cdh_logger::close();
				return -1;

			}
			else if (errorCode == ACS_APGCC_HA_FAILURE_CLOSE) {
				AES_CDH_LOG(LOG_LEVEL_ERROR, "HA Failed to close aes_cdhd gracefully");
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving aes_cdhd");
				delete pcdhObj;
				pcdhObj = 0;
				aes_cdh_logger::close();
				return -1;

			}
			else if (errorCode == ACS_APGCC_HA_SUCCESS) {
				AES_CDH_LOG(LOG_LEVEL_INFO, "HA Gracefully closed aes_cdhd!!");
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving aes_cdhd");
				delete pcdhObj;
				pcdhObj = 0;
				aes_cdh_logger::close();
				return 0;
			}
			else {
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while integrating aes_cdhd with HA");
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving aes_cdhd");
				delete pcdhObj;
				pcdhObj = 0;
				aes_cdh_logger::close();
				return -1;
			}

		}
	}
	return 0;
}
