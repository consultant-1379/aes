#ifndef CDHDSSWTESTCASE_H
#define CDHDSSWTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhdssw command in CDH

class CdhdsswTestCase : public TestCase
{
public:
	CdhdsswTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void miscTest();

};

#endif
