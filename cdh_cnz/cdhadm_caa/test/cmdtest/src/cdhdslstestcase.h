
#ifndef CDHDSLSTESTCASE_H
#define CDHDSLSTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhdsls command in CDH

class CdhdslsTestCase : public TestCase
{
public:
	CdhdslsTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void optionLTest();
	void miscTest();

};

#endif