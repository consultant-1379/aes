
#ifndef CDHCHTESTCASE_H
#define CDHCHTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhch command in CDH

class CdhchTestCase : public TestCase
{
public:
	CdhchTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void optionTTest();

    void optionHTest();

    void optionSTest();
    void optionDTest();
    void optionMTest();
};

#endif
