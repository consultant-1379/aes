#ifndef CDHDSRMTESTCASE_H
#define CDHDSRMTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhdsrm command in CDH

class CdhdsrmTestCase : public TestCase
{
public:
	CdhdsrmTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void miscTest();

};

#endif
