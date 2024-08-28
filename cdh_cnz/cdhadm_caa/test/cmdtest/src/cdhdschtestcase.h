#ifndef CDHDSCHTESTCASE_H
#define CDHDSCHTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhdssw command in CDH

class CdhdschTestCase : public TestCase
{
public:
	CdhdschTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void miscTest();

};

#endif
