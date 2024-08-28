#pragma warning(disable:4786)
#include "CdhdsswTestCase.h"
#include "aes_cdh_resultimplementation.h"

Test*
CdhdsswTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhdssw");

	testSuite->addTest (new TestCaller <CdhdsswTestCase> ("misc test", miscTest));
	
	return testSuite;
}

void
CdhdsswTestCase::setUp()
{
	
}

void
CdhdsswTestCase::tearDown()
{

}

void
CdhdsswTestCase::miscTest()
{

	int result(0);

//  ********************************************************************************
//  SETUP DEFINITION
//
//  Define destinations to be used in the test cases! Will be deleted in the
//  end of this method.
//  
//  ********************************************************************************

    (void) system("vdcreate -n \"Default FTP Site\" -a alias -d C:\\  > cmdtest.out");
    
	// define destinations
	result = system("cdhdef -a 127.0.0.1 -t bgwrpc bgwrpc1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t bgwrpc bgwrpc2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -c r -t ftpv2 -h alias rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// define a dest set
	result = system("cdhdsdef -s bgwrpc1 -b rftpv1 bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	
//  ********************************************************************************
//  AES_CDH_RC_OK
//  ********************************************************************************

	result = system("cdhdssw bgwrpc1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdssw rftpv1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_INCUSAGE
//  ********************************************************************************

	result = system("cdhdssw -x destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdssw > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdssw iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdssw destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_DESTNOTINDSET (12)
//  ********************************************************************************

	result = system("cdhdssw iftpv1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTNOTINDSET, result, __LINE__, __FILE__);

	result = system("cdhdssw bgwrpc3 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTNOTINDSET, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_NODESTSET (21)
//  ********************************************************************************

	result = system("cdhdssw bgwrpc2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);

	result = system("cdhdssw bgwrpc1 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);

//  ********************************************************************************
//  Clean up! Remove destination set and destinations.
//  ********************************************************************************

	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    (void) system("vdrm -n \"Default FTP Site\" -v alias > cmdtest.out");
}
