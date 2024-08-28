
#pragma warning(disable:4786)
#include "CdhchTestCase.h"
#include "aes_cdh_resultimplementation.h"


Test* CdhchTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhch");
/*
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -a", optionATest));
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -t", optionTTest));
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -c", optionCTest));
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -o", optionOTest));
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -r", optionRTest));
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -n", optionNTest));
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -u", optionUTest));

    testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -f", optionFTest));
    testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -x", optionXTest));
    testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -y", optionYTest));

	//-p prompts for password
	//testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -p", optionPTest));
*/
    testSuite->addTest (new TestCaller <CdhchTestCase> ("test option -t", optionTTest));

    testSuite->addTest (new TestCaller <CdhchTestCase> ("test option -h", optionSTest));

    testSuite->addTest (new TestCaller <CdhchTestCase> ("test option -s", optionSTest));
    testSuite->addTest (new TestCaller <CdhchTestCase> ("test option -d", optionDTest));
    testSuite->addTest (new TestCaller <CdhchTestCase> ("test option -m", optionMTest));

	return testSuite;
}

void CdhchTestCase::setUp ()
{
    (void) system("vdcreate -n \"Default FTP Site\" -a alias -d C:\\  > cmdtest.out");
}

void CdhchTestCase::tearDown()
{
    (void) system("vdrm -n \"Default FTP Site\" -v alias > cmdtest.out");
}



void CdhchTestCase::optionTTest()
{
	int result(0);

// ********************************************************************************
// SETUP DEFINITION
//
// In order to change something already specified for a destination,
// a destination is needed. Therefore, define a destination to be used for testing
// ********************************************************************************
    
	result = system("cdhdef -a 127.0.0.1 -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // There is no valid input because the transfer type may not be changed

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

	result = system("cdhch -t ftpv1 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


	result = system("cdhch -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


	result = system("cdhch -t sftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


	result = system("cdhch -t iforpc DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


	result = system("cdhch -t bgwrpc DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


// ********************************************************************************
// REMOVE DEFINITION
//
// A destination for created for testing purposes above. Remove it to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

	result = system("cdhrm DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}

void CdhchTestCase::optionHTest()
{
	int result(0);

// ********************************************************************************
// SETUP DEFINITION
//
// In order to change something already specified for a destination,
// a destination is needed. Therefore, define a destination to be used for testing
// ********************************************************************************
    
    result = system("cdhdef -a 127.0.0.1 -t bgwrpc DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -t ftpv2 -c r -h alias DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // There is no valid input because virtual directory may not be changed

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

	result = system("cdhch -h alias DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

    result = system("vdcreate -n \"Default FTP Site\" -a alias2 -d C:\\  > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

	result = system("cdhch -h alias2 DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

    result = system("vdrm -n \"Default FTP Site\" -v alias2 > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

// ********************************************************************************
// REMOVE DEFINITION
//
// A destination for created for testing purposes above. Remove it to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

	result = system("cdhrm DEST3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm DEST4 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}

void CdhchTestCase::optionSTest()
{
	int result(0);

// ********************************************************************************
// SETUP DEFINITION
//
// In order to change something already specified for a destination,
// a destination is needed. Therefore, define a destination to be used for testing
// ********************************************************************************
    
	result = system("cdhdef -a 127.0.0.1 -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 127.0.0.1 -t sftpv2 -u anyname -p DEST2 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -a 127.0.0.1 -t bgwrpc DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -t ftpv2 -c r -h alias DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    result = system("cdhch -s 0 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
    result = system("cdhch -s 100 -d 1 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
    result = system("cdhch -s 100 DEST2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhch -s 1 -d 60 DEST2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhch -s 0 DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhch -s 100 -m 10000 DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhch -s -1 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
//    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    
    result = system("cdhch -s 101 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    
    result = system("cdhch -s 10 DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


// ********************************************************************************
// REMOVE DEFINITION
//
// A destination for created for testing purposes above. Remove it to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

	result = system("cdhrm DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm DEST2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm DEST3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm DEST4 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}

void CdhchTestCase::optionDTest()
{
	int result(0);

// ********************************************************************************
// SETUP DEFINITION
//
// In order to change something already specified for a destination,
// a destination is needed. Therefore, define a destination to be used for testing
// ********************************************************************************
    
	result = system("cdhdef -a 127.0.0.1 -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 127.0.0.1 -t sftpv2 -u anyname -p DEST2 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -a 127.0.0.1 -t bgwrpc DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -t ftpv2 -c r -h alias DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    result = system("cdhch -d 60 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
    result = system("cdhch -d 1 DEST2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhch -d 0 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    
    result = system("cdhch -d 61 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    
    result = system("cdhch -d 10 DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

    result = system("cdhch -d 10 DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

// ********************************************************************************
// REMOVE DEFINITION
//
// A destination for created for testing purposes above. Remove it to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

	result = system("cdhrm DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm DEST2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm DEST3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm DEST4 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}

void CdhchTestCase::optionMTest()
{
	int result(0);

// ********************************************************************************
// SETUP DEFINITION
//
// In order to change something already specified for a destination,
// a destination is needed. Therefore, define a destination to be used for testing
// ********************************************************************************
    
	result = system("cdhdef -a 127.0.0.1 -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 127.0.0.1 -t sftpv2 -u anyname -p DEST2 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -a 127.0.0.1 -t bgwrpc DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -t ftpv2 -c r -h alias DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    result = system("cdhch -m 200 DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhch -m 199 DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    
    result = system("cdhch -m 10001 DEST3 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    
    result = system("cdhch -m 300 DEST4 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

    result = system("cdhch -m 300 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

    result = system("cdhch -m 300 DEST2 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

// ********************************************************************************
// REMOVE DEFINITION
//
// A destination for created for testing purposes above. Remove it to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

	result = system("cdhrm DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm DEST2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm DEST3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm DEST4 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}
