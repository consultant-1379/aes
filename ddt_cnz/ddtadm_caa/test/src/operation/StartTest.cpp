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
#include "operation/StartTest.h"
#include "cute.h"

#include "operation/start.h"
#include "common/programconstants.h"


void startTest_call_success()
{
	operation::Start start;
	int result = start.call();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

cute::suite make_suite_startTest()
{
	cute::suite s;
	s.push_back(CUTE(startTest_call_success));
	return s;
}



