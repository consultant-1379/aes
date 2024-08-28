#ifndef AFPRMSTCASE_H
#define AFPRMSTCASE_H

#include "autotest.h"

// Test cases for afprm command in AFP

class AfprmTestCase : public TestCase
{
public:
	AfprmTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void emptyOperands();
	void faultyOptions();
	void faultyOperands();
	void okTest();
	void destRemove();
	void fileRemove();
	void dirRemove();

private:
	string currentWorkDir;

};

#endif