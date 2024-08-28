#pragma warning(disable:4786)
#include "AfpftiTestCase.h"
#include "AES_GCC_Errorcodes.h"
#include <fstream>
#include <direct.h>

Test*
AfpftiTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("afpfti");

	testSuite->addTest (new TestCaller <AfpftiTestCase> ("empty operands", emptyOperands));
	testSuite->addTest (new TestCaller <AfpftiTestCase> ("ok test", okTest));
	testSuite->addTest (new TestCaller <AfpftiTestCase> ("fileOrDir test", fileOrDir));
	testSuite->addTest (new TestCaller <AfpftiTestCase> ("searchPathBackSlash test", searchPathBackSlash));
	testSuite->addTest (new TestCaller <AfpftiTestCase> ("searchPathNoBackSlash test", searchPathNoBackSlash));

	return testSuite;
}

void
AfpftiTestCase::setUp ()
{

}

void
AfpftiTestCase::tearDown()
{
	string removeString("");

	system("afprm DANNE > cmdout.out");

	removeString = fileDirectory;
	removeString += "\\afpfiles\\file1";
	::remove(removeString.c_str() );

	removeString = fileDirectory;
	removeString += "\\afpfiles\\dir1\\file1";
	::remove(removeString.c_str() );

	removeString = fileDirectory;
	removeString += "\\afpfiles\\dir1";
	rmdir(removeString.c_str() );

}

//---------------------------------------------------------
//	emptyOperands()
//---------------------------------------------------------
void
AfpftiTestCase::emptyOperands()
{
	int result(0);

	// Files (-c)
	result = system("afpfti -c c:\\temp\\afpfiles\\file1 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	result = system("afpfti -c c:\\temp\\afpfiles\\file1 DANNE > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	result = system("afpfti -c c:\\temp\\afpfiles\\file1 DEST2 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);


	// Directories (-z)
	result = system("afpfti -z c:\\temp\\afpfiles\\dir1 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	result = system("afpfti -z c:\\temp\\afpfiles\\dir1 DANNE > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);

	result = system("afpfti -z c:\\temp\\afpfiles\\dir1 DEST2 > cmdtest.out");
	assertEquals(AES_INCORRECTCOMMAND, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	okTest()
//---------------------------------------------------------
void
AfpftiTestCase::okTest()
{
	int result(0);

	string tmpString(fileDirectory);
	tmpString += "\\afpfiles";
	mkdir(tmpString.c_str() );

	// Create file in dir afpfiles

	tmpString += "\\file1";
	ofstream fout(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("afpfti test, file1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	// Make dir "dir1" in c:\temp\afpfiles
	tmpString = fileDirectory;
	tmpString += "\\afpfiles\\dir1";
	mkdir(tmpString.c_str() );

	// Create a file1 in dir1
	tmpString += "\\file1";
	fout.open(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("afpfti test, file1 in dir1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	// Send a file
	result = system("afpfti -c c:\\temp\\afpfiles\\file1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	// Send a directory

	result = system("afpfti -z c:\\temp\\afpfiles\\dir1 DANNE DEST1 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
// fileOrDir()
//---------------------------------------------------------
void
AfpftiTestCase::fileOrDir()
{
	int result(0);

	string tmpString(fileDirectory);
	
	tmpString += "\\afpfiles\\file1";
	ofstream fout(tmpString.c_str());
	if (!fout)
		{
			throw CppUnitException("afpfti test, file1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	tmpString = "";
	tmpString += fileDirectory + "\\afpfiles\\dir1";
	mkdir(tmpString.c_str());

	tmpString += "\\file1";
	// Create a file1 in dir1
	fout.open(tmpString.c_str());
	if (!fout)
		{
			throw CppUnitException("afpfti test, file1 in dir1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	// Send a file as a directory.
	result = system("afpfti -z c:\\temp\\afpfiles\\file1 DANNE DEST2  > cmdout.out");
	assertEquals(AES_SENDITEMNAMEINVAL, result, __LINE__, __FILE__);

	// Send a directory as a file

	result = system("afpfti -c c:\\temp\\afpfiles\\dir1 DANNE DEST1  > cmdout.out");
	assertEquals(AES_SENDITEMNAMEINVAL, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	searchPathBackSlash()
//---------------------------------------------------------
void
AfpftiTestCase::searchPathBackSlash()
{
	int result(0);
	string tmpString(fileDirectory);

	tmpString += "\\afpfiles";
		// Create file in dir c:\temp\afpfiles
	mkdir(tmpString.c_str());

	tmpString += "\\file1";
	
	ofstream fout(tmpString.c_str());
	if (!fout)
		{
			throw CppUnitException("afpfti test, file1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	// Send \afpfiles\file1
	_chdir(fileDirectory.c_str() );
	result = system("afpfti -c \\afpfiles\\file1 DANNE DEST2 > cmdout.out");
	if (result == AES_NOERRORCODE)
		{
			system("afprm DANNE  > cmdout.out");
		}
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

}

//---------------------------------------------------------
//	searchPathNoBackSlash()
//---------------------------------------------------------
void
AfpftiTestCase::searchPathNoBackSlash()
{
	int result(0);

	// Create dir afpfiles
	string tmpString(fileDirectory);
	tmpString += "\\afpfiles";
	mkdir(tmpString.c_str() );

		// Create file in dir afpfiles
	tmpString += "\\file1";
	ofstream fout(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("afpfti test, file1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	_chdir(fileDirectory.c_str() );
	// Send afpfiles\file1
	result = system("afpfti -c afpfiles\\file1 DANNE DEST2 > cmdout.out");
	if (result == AES_NOERRORCODE)
		{
			system("afprm DANNE  > cmdout.out");
		}
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
}