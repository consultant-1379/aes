#pragma warning(disable:4786)
#include "AfprmTestCase.h"
#include "AES_GCC_Errorcodes.h"
#include <fstream>
#include <direct.h>

Test*
AfprmTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("afprm");

	testSuite->addTest (new TestCaller <AfprmTestCase> ("empty operands", emptyOperands));
	testSuite->addTest (new TestCaller <AfprmTestCase> ("ok test", okTest));
	testSuite->addTest (new TestCaller <AfprmTestCase> ("remove destination test", destRemove));
	testSuite->addTest (new TestCaller <AfprmTestCase> ("remove file test", fileRemove));
	testSuite->addTest (new TestCaller <AfprmTestCase> ("option test", faultyOptions));
	testSuite->addTest (new TestCaller <AfprmTestCase> ("operand test", faultyOperands));

	return testSuite;
}

void
AfprmTestCase::setUp ()
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);	
	currentWorkDir = buffer;
	system("afpdef DANNE DEST2 > cmdtest.out");
	system("afpdef DANNE DEST1 > cmdtest.out");
	system("cdhdef -a localhost -t ftpv2 ARNE");
	system("cdhdsdef ARNE DEST9DEST9DEST9DEST9DEST9DEST9DE");
	system("cdhdef -a localhost -t ftpv2 NISSE");
	system("cdhdsdef NISSE DEST1");
}

void
AfprmTestCase::tearDown()
{
	system("afprm DANNE > cmdtest.out");
	system("cdhdsrm DEST9DEST9DEST9DEST9DEST9DEST9DE");
	system("cdhrm ARNE");
	system("cdhdsrm DEST1");
	system("cdhrm NISSE");
}

//---------------------------------------------------------
//	emptyOperands()
//---------------------------------------------------------
void
AfprmTestCase::emptyOperands()
{
	int result(0);

	
	// Files (-c)
	result = system("afprm -c file1 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// Directories (-z)
	result = system("afprm -z dir1 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	faultyOptions()
//---------------------------------------------------------
void
AfprmTestCase::faultyOptions()
{
	int result(0);

	// Option -c and -z together
	result = system("afprm -c file1 -z dir1 DANNE DEST1 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// No options and to many operands
	result = system("afprm DANNE DEST1 DEST2 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// Option -f and -c
	result = system("afprm -f -c file1 DANNE DEST1 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// Option -f and -z
	result = system("afprm -f -z dir1 DANNE DEST1 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// Option -f -c and -z
	result = system("afprm -f -c file1 -z dir1 DANNE DEST1 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// To many operands -c
	result = system("afprm -c file1 DANNE DEST1 DEST2 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// To many operands -z
	result = system("afprm -z dir1 DANNE DEST1 DEST2 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	// Incorrect option -g
	result = system("afprm -g dir1 DANNE DEST1 > cmdout.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	faultyOperands()
//---------------------------------------------------------
void
AfprmTestCase::faultyOperands()
{
	int result(0);

	// Non existent TQ
	result = system("afprm NONEEXIST > cmdout.out");
	assertEquals(AES_NOPROCORDER, result, __LINE__, __FILE__);

	// Non existent DEST
	result = system("afprm DANNE NONEEXIST > cmdout.out");
	assertEquals(AES_NODESTINATION, result, __LINE__, __FILE__);

	// Invalid TQ
	result = system("afprm !!!!!! > cmdout.out");
	assertEquals(AES_FILENAMEINVALID, result, __LINE__, __FILE__);

	// Invalid destination
	result = system("afprm DANNE !!!!! > cmdout.out");
	assertEquals(AES_INVALIDDESTNAME, result, __LINE__, __FILE__);

	// Too long TQ name
	result = system("afprm tolongnametolongnametolongnameat33 > cmdout.out");
	assertEquals(AES_FILENAMEINVALID, result, __LINE__, __FILE__);

	// Too long destination name
	result = system("afprm DANNE tolongnametolongnametolongnameat33 > cmdout.out");
	assertEquals(AES_INVALIDDESTNAME, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	okTest()
//---------------------------------------------------------
void
AfprmTestCase::okTest()
{
	int result(0);

	// 32 chars TQ name
	system("afpdef 32charlong32charlong32charlong32 DEST2");
	result = system("afprm 32charlong32charlong32charlong32 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	// 32 chars DEST name
	system("afpdef DANNE DEST9DEST9DEST9DEST9DEST9DEST9DE");
	result = system("afprm DANNE DEST9DEST9DEST9DEST9DEST9DEST9DE > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	// Remove a TQ
	result = system("afprm DANNE > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
// removeDest()
//---------------------------------------------------------
void
AfprmTestCase::destRemove()
{
	int result(0);

	system("afpdef DANNE DEST1 > cmdtest.out");
	system("afpdef DANNE DEST2 > cmdtest.out");

	// Remove a dest from a TQ
	result = system("afprm DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	// Try to remove a non existent dest from a TQ
	result = system("afprm DANNE NONEXIST > cmdtest.out");
	assertEquals(AES_NODESTINATION, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	removeFile()
//---------------------------------------------------------
void
AfprmTestCase::fileRemove()
{
	int result(0);

	string tmpString("");
	tmpString = "c:\\temp\\afpfiles";
	mkdir(tmpString.c_str() );

	// SETUP START ------------------------------------------
	// Create file in dir afpfiles 
	tmpString += "\\file1";
	ofstream fout(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("afprm test, file1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	// Create dir1 and file1
	tmpString = "";
	tmpString = "c:\\temp\\afpfiles\\dir1";
	mkdir(tmpString.c_str() );

	tmpString += "\\file1";
	fout.open(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("afprm test, file1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	// Report file1 to some different destinations
	result = system("afpfti -c c:\\temp\\afpfiles\\file1 DANNE DEST1 > cmdtest.out");
	if (result != AES_NOERRORCODE)
		{
			throw CppUnitException("afprm test, afpfti command failed", __LINE__, __FILE__);
		}

	result = system("afpfti -c c:\\temp\\afpfiles\\file1 DANNE DEST2 > cmdtest.out");
	if (result != AES_NOERRORCODE)
		{
			throw CppUnitException("afprm test, afpfti command failed", __LINE__, __FILE__);
		}

	result = system("afpfti -c c:\\temp\\afpfiles\\file1 DANNE DEST3 > cmdtest.out");
	if (result != AES_NOERRORCODE)
		{
			throw CppUnitException("afprm test, afpfti command failed", __LINE__, __FILE__);
		}

	// Report dir1 to a destination
	result = system("afpfti -z c:\\temp\\afpfiles\\dir1 DANNE DEST4 > cmdtest.out");
	if (result != AES_NOERRORCODE)
		{
			throw CppUnitException("afprm test, afpfti command failed", __LINE__, __FILE__);
		}

// SETUP END ----------------------------------------------

	// Remove file1 from dest1
	result = system("afprm -c file1 DANNE DEST1 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	// Try to remove file with search path, does not matter what path since
	// it's not used in afprm. This should return invalid file or directory name
	result = system("afprm -c c:\\TEMP\\file1 DANNE DEST2 > cmdout.out");
	assertEquals(AES_SENDITEMNAMEINVAL, result, __LINE__, __FILE__);

	// Try to remove dir with search path, does not matter what path since
	// it's not used in afprm.This should return invalid file or directory name

	result = system("afprm -z c:\\TEMP\\dir1 DANNE DEST4 > cmdout.out");
	assertEquals(AES_SENDITEMNAMEINVAL, result, __LINE__, __FILE__);

	// Try to remove nonexisten file2 from all destinations
	result = system("afprm -c file2 DANNE > cmdtest.out");
	assertEquals(AES_SENDITEMNOTREP, result, __LINE__, __FILE__);

	// Remove file1 from all destinations
	result = system("afprm -c file1 DANNE > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
}