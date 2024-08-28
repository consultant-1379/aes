#ifndef AFPFTESTCASE_H
#define AFPFTESTCASE_H

#include "autotest.h"
// Test cases for afprm command in AFP

class AfpfteTestCase : public TestCase
{
public:
	AfpfteTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void emptyOperands();
	void faultyOptions();
	void faultyOperands();
	void okTest();

private:
	string currentWorkDir;

};

#endif