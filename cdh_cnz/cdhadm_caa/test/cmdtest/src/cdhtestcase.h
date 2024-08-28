
#ifndef CDHDEFTESTCASE_H
#define CDHDEFTESTCASE_H

#include "autotest.h"

// Test cases for afpchd command in AFP

class CdhdefTestCase : public TestCase
{
public:
	CdhdefTestCase (std::string name) : TestCase (name) {}

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