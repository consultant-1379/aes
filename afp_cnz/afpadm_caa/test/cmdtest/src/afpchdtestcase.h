
#ifndef AFPCHDTESTCASE_H
#define AFPCHDTESTCASE_H

#include "autotest.h"

// Test cases for afpchd command in AFP

class AfpchdTestCase : public TestCase
{
public:
	AfpchdTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void optionBTest();
	void optionDTest();
	void optionRTest();
	void optionTTest();
	void optionSTest();

};

#endif