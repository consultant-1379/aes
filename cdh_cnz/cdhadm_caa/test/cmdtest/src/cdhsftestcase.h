
#ifndef CDHSFTESTCASE_H
#define CDHSFTESTCASE_H

#include "autotest.h"
#pragma warning(disable:4786)

// Test cases for cdhch command in CDH

class CdhsfTestCase : public TestCase
{
public:
	CdhsfTestCase (std::string name) : TestCase (name) {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	void longAndShort();
	void longAndShortResp();
	void longAndShortSFTP();
	void sendFile();
	void sendFileResp();
    void stopTransfer();
	void sendFileSFTP();

};

#endif
