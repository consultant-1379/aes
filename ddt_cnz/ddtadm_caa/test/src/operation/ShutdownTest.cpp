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
#include "cute.h"
#include "operation/ShutdownTest.h"

#include "operation/shutdown.h"
#include "common/programconstants.h"

void shutdownTest_call_success() {
	operation::Shutdown shutdown;
	int result = shutdown.call();
	ASSERT_EQUAL(common::errorCode::ERR_SVC_DEACTIVATE, result);
}

cute::suite make_suite_shutdownTest(){
	cute::suite s;
	s.push_back(CUTE(shutdownTest_call_success));
	return s;
}



