#pragma warning(disable:4786)
#include "CdhdsrmTestCase.h"
#include "aes_cdh_resultimplementation.h"

Test*
CdhdsrmTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhdsrm");

	testSuite->addTest (new TestCaller <CdhdsrmTestCase> ("misc test", miscTest));
	
	return testSuite;
}

void
CdhdsrmTestCase::setUp()
{
	
}

void
CdhdsrmTestCase::tearDown()
{

}

void
CdhdsrmTestCase::miscTest()
{

	int result(0);

//  ********************************************************************************
//  SETUP DEFINITION
//
//  Define destinations to be used in the test cases! Will be deleted in the
//  last test case miscTest().
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

	result = system("cdhdef -c r -t ftpv2 -h alias rftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// define dest sets
	// obs! måste gå igenom om åter in
//	result = system("cdhdsdef -s bgwrpc1 -b rftpv1 bgwrpc2 destset2 > cmdtest.out");
//	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s iftpv1 -b rftpv2 iftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_INCUSAGE
//  ********************************************************************************

	result = system("cdhdsrm -d destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsrm -d -q destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_DESTNOTINDSET (12)
//  ********************************************************************************

	result = system("cdhdsrm bgwrpc1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTNOTINDSET, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_DESTISACTORPRIM (19)
//  ********************************************************************************

	result = system("cdhdsrm iftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTISACTORPRIM, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_NODESTSET (21)
//  ********************************************************************************

	result = system("cdhdsrm destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);

	result = system("cdhdsrm 1428 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);

	result = system("cdhdsrm asdfghjklöasdfghjklöasdmmdestset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODESTSET, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_PROTECTEDDESTSET (24)
//  ********************************************************************************

	// ??? 

//  ********************************************************************************
//   AES_CDH_RC_CMDAUTHERR (30)
//  ********************************************************************************

	// hmm...

//  ********************************************************************************
//  AES_CDH_RC_OK
//  ********************************************************************************

	result = system("cdhdsrm iftpv1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//	result = system("cdhdsrm destset2 > cmdtest.out");
//	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//  ********************************************************************************
//  Clean up! Remove all defined destinations.
//  ********************************************************************************

	result = system("cdhrm bgwrpc1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    (void) system("vdrm -n \"Default FTP Site\" -v alias > cmdtest.out");
}
