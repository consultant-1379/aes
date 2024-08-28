
#ifndef AFPFTITESTCASE_H
#define AFPFTITESTCASE_H

#include "autotest.h"

// Test cases for afpfti command in AFP

class AfpftiTestCase : public TestCase
{
public:
	AfpftiTestCase (std::string name) : TestCase (name), fileDirectory("C:\\TEMP") {}

	void setUp ();
	void tearDown();
	static Test* suite ();

protected:

	// Test of empty operands
	// 1) afpfti is called with no TQ and no destination
	// 2) afpfti is called with TQ but no destination
	// 3) afpfti is called with no TQ but with destination
	void emptyOperands();

	// Test of correct usage of afpfti
	// All tests should pass with AES_NOERRORCODE
	void okTest();

	// Test of sending a file as a dir and a dir as a file
	void fileOrDir();

	// Test of search path for files or dirs
	// testing with appdir\filename and
	// \appdir\filename
	void searchPathBackSlash();
	void searchPathNoBackSlash();

private:
	const string fileDirectory;

};

#endif