#pragma warning(disable:4786)
#include "AfpfteTestCase.h"
#include "AES_GCC_Errorcodes.h"
#include <fstream>
#include <direct.h>

Test*
AfpfteTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("afpfte");

	testSuite->addTest (new TestCaller <AfpfteTestCase> ("empty operands", emptyOperands));
	testSuite->addTest (new TestCaller <AfpfteTestCase> ("ok test", okTest));
	testSuite->addTest (new TestCaller <AfpfteTestCase> ("option test", faultyOptions));

	return testSuite;
}

void
AfpfteTestCase::setUp ()
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);	
	currentWorkDir = buffer;
	system("afpdef DANNE DEST2 > cmdtest.out");
	system("afpdef DANNE DEST1 > cmdtest.out");
}

void
AfpfteTestCase::tearDown()
{
	system("afprm DANNE > cmdtest.out");
}

//---------------------------------------------------------
//	emptyOperands()
//---------------------------------------------------------
void
AfpfteTestCase::emptyOperands()
{
	int result(0);

	
	// Files -c
	result = system("afpfte -c file1 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// Directories -z
	result = system("afpfte -z dir1 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	faultyOptions()
//---------------------------------------------------------
void
AfpfteTestCase::faultyOptions()
{
	int result(0);

	// Option -c and -z together
	result = system("afpfte -c file1 -z dir1 DANNE DEST1 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// No options and to many operands
	result = system("afpfte DANNE DEST1 DEST2 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// Incorrect option -g
	result = system("afpfte -g dir1 DANNE DEST1 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	faultyOperands()
//---------------------------------------------------------
void
AfpfteTestCase::faultyOperands()
{
	int result(0);

	// Non existent TQ
	result = system("afpfte -c file1 NONEEXIST DEST1> cmdout.out");
	assertEquals(AES_NOPROCORDER, result, __LINE__, __FILE__);

	// Non existent DEST
	result = system("afpfte -c file1 DANNE NONEEXIST > cmdout.out");
	assertEquals(AES_NODESTINATION, result, __LINE__, __FILE__);

	// Invalid TQ
	result = system("afpfte -c file1 !!!!!! DEST1 > cmdout.out");
	assertEquals(AES_FILENAMEINVALID, result, __LINE__, __FILE__);

	// Invalid destination
	result = system("afpfte -c file1 DANNE !!!!! > cmdout.out");
	assertEquals(AES_INVALIDDESTNAME, result, __LINE__, __FILE__);

	// Too long TQ name
	result = system("afpfte -c file1 tolongnametolongnametolongnameat33 DEST1 > cmdout.out");
	assertEquals(AES_FILENAMEINVALID, result, __LINE__, __FILE__);

	// Too long destination name
	result = system("afpfte -c file1 DANNE tolongnametolongnametolongnameat33 > cmdout.out");
	assertEquals(AES_INVALIDDESTNAME, result, __LINE__, __FILE__);

}

//---------------------------------------------------------
//	okTest()
//---------------------------------------------------------
void
AfpfteTestCase::okTest()
{
	int result(0);
}

