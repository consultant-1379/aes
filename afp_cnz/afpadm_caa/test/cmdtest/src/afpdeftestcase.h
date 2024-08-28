
#ifndef AFPDEFTESTCASE_H
#define AFPDEFTESTCASE_H

#include "autotest.h"

// Test cases for afpdef command in AFP

class AfpdefTestCase : public TestCase
{
public:
	AfpdefTestCase (std::string name) : TestCase (name) {}

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