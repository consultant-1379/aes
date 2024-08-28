
#ifndef PROTOCOLTESTCASE_H
#define PROTOCOLTESTCASE_H

#include "autotest.h"

// Test cases for protocol in AFP

class ProtocolTestCase : public TestCase
{
public:
	ProtocolTestCase (std::string name) : TestCase (name), fileDirectory("C:\\TEMP") {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:
	void addValuesFile();
	void addValuesFile_Error();
	void addValuesApi();
	void addValuesApi_Error();
	void addValuesTq();
	void addValuesTq_Error();
	void getString();
	void setString();
	void getStringValue_Error();
	void getIntValue_Error();
	void writeToDisc();
	void readFromFile();
	void clear();

private:
	const string fileDirectory;
	void removeTestFiles();
};

#endif