#pragma warning(disable:4786)
#include "AfpdefTestCase.h"
#include "AES_GCC_Errorcodes.h"

Test*
AfpdefTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("afpdef");

	testSuite->addTest (new TestCaller <AfpdefTestCase> ("test option -b", optionBTest));
	testSuite->addTest (new TestCaller <AfpdefTestCase> ("test option -d", optionDTest));
	testSuite->addTest (new TestCaller <AfpdefTestCase> ("test option -r", optionRTest));
	testSuite->addTest (new TestCaller <AfpdefTestCase> ("test option -t", optionTTest));
	testSuite->addTest (new TestCaller <AfpdefTestCase> ("test option -s", optionSTest));

	return testSuite;
}

void
AfpdefTestCase::setUp ()
{

}

void
AfpdefTestCase::tearDown()
{

}

void
AfpdefTestCase::optionBTest()
{
	int result(0);

	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -b YES DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -b NO DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afpdef -b ODD DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALREMBEFORE, result, __LINE__, __FILE__);

	result = system("afpdef -b 10 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALREMBEFORE, result, __LINE__, __FILE__);

}
void
AfpdefTestCase::optionDTest()
{
	int result(0);

	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -d 0 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);

	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -d 1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -d 59999 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -d 60000 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALDELAYVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -d -1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -d -2 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALDELAYVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -d AA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALDELAYVALUE, result, __LINE__, __FILE__);
}

void
AfpdefTestCase::optionRTest()
{
	int result(0);

	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -r -2 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALRETRYVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -r -1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE");

	result = system("afpdef -r 0 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -r 1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -r 100 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -r 101 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALRETRYVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -r AA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALRETRYVALUE, result, __LINE__, __FILE__);

}

void
AfpdefTestCase::optionTTest()
{
	int result(0);

	result = system("afprm DANNE > cmdtest.out");

	result = system("afpdef -t -1 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -t 9 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -t 10 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE");

	result = system("afpdef -t 59999 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	result = system("afprm DANNE");

	result = system("afpdef -t 60000 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -t AA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALTIMEVALUE, result, __LINE__, __FILE__);

}

void
AfpdefTestCase::optionSTest()
{
	int result(0);

	system("afprm DANNE > cmdtest.out");

	result = system("afpdef -s none DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	system("afprm DANNE > cmdtest.out");

	result = system("afpdef -s ready DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	system("afprm DANNE > cmdtest.out");

	result = system("afpdef -s delete DANNE DEST2 > cmdtest.out");
	assertEquals(AES_NOERRORCODE, result, __LINE__, __FILE__);
	system("afprm DANNE > cmdtest.out");

	result = system("afpdef -s stopped DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -s failed DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -s send DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -s AAA DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);

	result = system("afpdef -s 100 DANNE DEST2 > cmdtest.out");
	assertEquals(AES_ILLEGALSTATUSVALUE, result, __LINE__, __FILE__);
}

