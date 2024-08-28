#pragma warning(disable:4786)
#include "AfpchdTestCase.h"
#include "AES_GCC_Errorcodes.h"

Test*
AfpchdTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("afpchd");

	testSuite->addTest (new TestCaller <AfpchdTestCase> ("test option -b", optionBTest));
	testSuite->addTest (new TestCaller <AfpchdTestCase> ("test option -d", optionDTest));
	testSuite->addTest (new TestCaller <AfpchdTestCase> ("test option -r", optionRTest));
	testSuite->addTest (new TestCaller <AfpchdTestCase> ("test option -t", optionTTest));
	testSuite->addTest (new TestCaller <AfpchdTestCase> ("test option -s", optionSTest));

	return testSuite;
}

void
AfpchdTestCase::setUp ()
{
	system("afpdef -s none DANNE DEST2 > cmdout.out");
}

void
AfpchdTestCase::tearDown()
{
	system("afprm DANNE > cmdout.out");
}

void
AfpchdTestCase::optionBTest()
{
	int result(0);

	result = system("afpchd -b YES DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -b NO DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -b ODD DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALREMBEFORE, result, __LINE__, __FILE__);

	result = system("afpchd -b 10 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALREMBEFORE, result, __LINE__, __FILE__);

}
void
AfpchdTestCase::optionDTest()
{
	int result(0);

	result = system("afpchd -d 0 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -d 1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -d 59999 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -d 60000 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALDELAYVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -d -1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -d -2 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALDELAYVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -d AA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALDELAYVALUE, result, __LINE__, __FILE__);
}

void
AfpchdTestCase::optionRTest()
{
	int result(0);

	result = system("afpchd -r -2 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALRETRYVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -r -1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -r 0 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -r 1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -r 100 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -r 101 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALRETRYVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -r AA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALRETRYVALUE, result, __LINE__, __FILE__);

}

void
AfpchdTestCase::optionTTest()
{
	int result(0);

	result = system("afpchd -t -1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -t 9 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -t 10 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -t 59999 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -t 60000 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -t AA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

}

void
AfpchdTestCase::optionSTest()
{
	int result(0);

	result = system("afpchd -s none DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -s ready DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpchd -s delete DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ERRORCHANGESTATUS, result, __LINE__, __FILE__);

	result = system("afpchd -s stopped DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -s failed DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -s send DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -s AAA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpchd -s 100 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);
}

