#ifndef CDHDSDEFTESTCASE_H
#define CDHDSDEFTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhdsdef command in CDH

class CdhdsdefTestCase : public TestCase
{
public:
	CdhdsdefTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void optionVTest();
	void optionBTest();
	void optionSTest();
	void miscTest();

};

#endif
