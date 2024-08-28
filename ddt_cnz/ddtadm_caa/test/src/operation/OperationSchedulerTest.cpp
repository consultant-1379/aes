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
#include "operation/SchedulerTest.h"
#include "operation/scheduler.h"
#include "common/programconstants.h"

void scheduler_start_success()
{
	operation::Scheduler scheduler;
	int result = scheduler.start();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void scheduler_start_failure()
{
	operation::Scheduler scheduler;
	scheduler.setThrCount(1);
	int result = scheduler.start();
	ASSERT_EQUAL(common::errorCode::ERR_OPEN, result);
}

void scheduler_open_success()
{
	operation::Scheduler scheduler;
	int result = scheduler.open();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void scheduler_open_failure()
{
	operation::Scheduler scheduler;
	scheduler.setThrCount(1);
	int result = scheduler.open();
	ASSERT_EQUAL(common::errorCode::ERR_SVC_ACTIVATE, result);
}


void scheduler_stop_success_1()
{
	//simulate start and stop of the thread
	operation::Scheduler scheduler;
	scheduler.incrThrCount();
	scheduler.stop();
	scheduler.decrThrCount();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, scheduler.thr_count());
}

void scheduler_stop_success_2()
{
	//Thread never started

	operation::Scheduler scheduler;
	scheduler.stop();
	ASSERT_EQUAL(0, scheduler.thr_count());
}

void scheduler_svc_success()
{
	//simulate start and stop of the thread
	operation::Scheduler scheduler;
	scheduler.start();
	scheduler.stop();
	int result = scheduler.simulateExecution();

	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}


cute::suite make_suite_schedulerTest(){
	cute::suite s;
	s.push_back(CUTE(scheduler_start_success));
	s.push_back(CUTE(scheduler_start_failure));
	s.push_back(CUTE(scheduler_open_success));
	s.push_back(CUTE(scheduler_open_failure));
	s.push_back(CUTE(scheduler_stop_success_1));
	s.push_back(CUTE(scheduler_stop_success_2));
	s.push_back(CUTE(scheduler_svc_success));

	return s;
}



