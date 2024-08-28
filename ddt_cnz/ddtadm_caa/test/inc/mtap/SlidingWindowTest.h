
#ifndef DDTADM_CAA_TEST_INC_MTAP_SLIDINGWINDOWTEST_H_
#define DDTADM_CAA_TEST_INC_MTAP_SLIDINGWINDOWTEST_H_

#include "cute_suite.h"

#include "mtap/protocol.h"
#include "common/programconstants.h"
#include "stubs/ACE_Task_Base_stub.h"
#include <stdio.h>

class SlidingWindowTest
{
public:
	SlidingWindowTest(){;}
	~SlidingWindowTest(){;}

	static cute::suite make_suite_SlidingWindowTest();
	static void testSyncWithDiffMoreThanWindowSize();
	static void testSyncWithDiffLessThanWindowSize();
	static void testUnlink();
	static void testAdd();
	static void testCheckRange_1();
	static void testCheckRange_2();
};
#endif
