
#pragma warning(disable:4786)
#include "CdhdslsTestCase.h"
#include "aes_cdh_resultimplementation.h"


Test* CdhdslsTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhdsls");

    testSuite->addTest (new TestCaller <CdhdslsTestCase> ("test option -l", optionLTest));
	testSuite->addTest (new TestCaller <CdhdslsTestCase> ("misc test", miscTest));
	
	return testSuite;
}

void CdhdslsTestCase::setUp ()
{
	
}

void CdhdslsTestCase::tearDown()
{

}



void CdhdslsTestCase::optionLTest()
{
	int result(0);

// ********************************************************************************
// SETUP DEFINITION
//
//  Define destinations to be used in the test cases! Will be deleted in the
//  last test case miscTest().
//
// ********************************************************************************

    (void) system("vdcreate -n \"Default FTP Site\" -a alias -d C:\\  > cmdtest.out");
    
	// define 3 destinations
	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -c r -t ftpv2 -h alias rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// define a dest set
	result = system("cdhdsdef -s iftpv1 -b rftpv1 iftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
// ********************************************************************************
// AES_CDH_RC_OK
// ********************************************************************************

    // cdhdsls -l destset
	result = system("cdhdsls -l destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// cdhdsls -l
	result = system("cdhdsls -l > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    (void) system("vdrm -n \"Default FTP Site\" -v alias > cmdtest.out");
}

void CdhdslsTestCase::miscTest()
{
	int result(0);

// ********************************************************************************
// AES_CDH_RC_OK
// ********************************************************************************

	// cdhdsls destset
	result = system("cdhdsls destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// cdhdsls
	result = system("cdhdsls > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

	// AES_CDH_RC_INCUSAGE
	result = system("cdhdsls -a > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsls -a 2020 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsls -b destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsls iftpv1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsls - > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsls -? > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	// AES_CDH_RC_NODESTSET
	result = system("cdhdsls destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);

	result = system("cdhdsls -l destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);

	result = system("cdhdsls iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);


// ********************************************************************************
// REMOVE DEFINITION
//
// A destination/destination set was created for testing purposes above. 
// Remove it to make sure that CDH is not polluted before we continue
// ********************************************************************************

	// remove destinations and dest set
	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

}
