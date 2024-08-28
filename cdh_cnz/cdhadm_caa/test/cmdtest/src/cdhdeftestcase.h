
#ifndef CDHDEFTESTCASE_H
#define CDHDEFTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhdef command in CDH

class CdhdefTestCase : public TestCase
{
public:
	CdhdefTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void optionATest();
	void optionCTest();
	void optionOTest();
	void optionTTest();
	void optionRTest();
	void optionNTest();
	void optionUTest();

    void optionFTest();
    void optionXTest();
    void optionYTest();
    void optionETest();

    void optionSTest();
    void optionDTest();
    void optionMTest();
	void optionHTest();
};

#endif
