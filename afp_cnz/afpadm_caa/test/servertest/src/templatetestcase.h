
#ifndef TEMPLATETESTCASE_H
#define TEMPLATETESTCASE_H

#include "autotest.h"

// Test cases for template in AFP

class TemplateTestCase : public TestCase
{
public:
	TemplateTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:
	void faultyTemplate();
	void correctTemplate();
	void fileRename();
	void createFileName();

private:
	string currentWorkDir;
	string yearShort;
	string yearLong;
	string month;
	string day;
	string hour;
	string minute;

};

#endif