#pragma warning(disable:4786)
#include "CdhdsdefTestCase.h"
#include "aes_cdh_resultimplementation.h"

// NOTE: sftpv2 is not tested because of password option! 

Test*
CdhdsdefTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhdsdef");

	testSuite->addTest (new TestCaller <CdhdsdefTestCase> ("test option -v", optionVTest));
	testSuite->addTest (new TestCaller <CdhdsdefTestCase> ("test option -s", optionSTest));
	testSuite->addTest (new TestCaller <CdhdsdefTestCase> ("test option -b", optionBTest));
	testSuite->addTest (new TestCaller <CdhdsdefTestCase> ("misc test", miscTest));

	return testSuite;
}

void
CdhdsdefTestCase::setUp()
{
	
}

void
CdhdsdefTestCase::tearDown()
{

}

void
CdhdsdefTestCase::optionVTest()
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
    
	// define 3 bgwrpc destinations
	result = system("cdhdef -a 127.0.0.1 -t bgwrpc bgwrpc1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t bgwrpc bgwrpc2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t bgwrpc bgwrpc3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// define 3 ftpv2 r destinations
	result = system("cdhdef -t ftpv2 -c r -h alias rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -h alias rftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -h alias rftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// define 3 ftpv2 i destinations
	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_OK
//  ********************************************************************************

	// define a dest set
	result = system("cdhdsdef -s iftpv1 rftpv1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_INCUSAGE
//  ********************************************************************************

	result = system("cdhdsdef -v rftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_UNREAS
//  ********************************************************************************

	result = system("cdhdsdef rftpv2 destsetaaaaaaaaaaaaaaaaaaaaaaaaa2 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_USRGNOTMEMBER
//  ********************************************************************************

//	result = system("cdhdsdef -v NOUSERGRP rftpv2 destset2 > cmdtest.out");
//	assertEquals(AES_CDH_RC_USRGNOTMEMBER, result, __LINE__, __FILE__);


//  ********************************************************************************
//  Clean up!
//  ********************************************************************************

	// remove dest set
	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    (void) system("vdrm -n \"Default FTP Site\" -v alias > cmdtest.out");
}

void
CdhdsdefTestCase::optionSTest()
{
	int result (0);

//  ********************************************************************************
//  AES_CDH_RC_OK
//  ********************************************************************************

	// bgwrpc and bgwrpc
	result = system("cdhdsdef -s bgwrpc1 bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// ftpv2 r and ftpv2 r
	result = system("cdhdsdef -s rftpv1 rftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// ftpv2 r and ftpv2 i
	result = system("cdhdsdef -s rftpv3 iftpv1 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// remove dest sets
	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// ftpv2 i and ftpv2 r
	result = system("cdhdsdef -s iftpv1 rftpv1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// ftpv2 i and ftpv2 i
	result = system("cdhdsdef -s iftpv2 iftpv3 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// remove dest sets
	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_INCUSAGE
//  ********************************************************************************

	result = system("cdhdsdef -s rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_NODEST
//  ********************************************************************************

	result = system("cdhdsdef -s asdfasdfasdfasdfasdfasdfasdfasdfa rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s 6789 rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_ILLEGALCOMB
//  ********************************************************************************

	// bgwrpc and ftpv2 r
	result = system("cdhdsdef -s bgwrpc1 rftpv1 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

	// bggwrpc and ftpv2 i
	result = system("cdhdsdef -s bgwrpc1 iftpv1 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

	// ftpv2 r and bgwrpc
	result = system("cdhdsdef -s rftpv1 bgwrpc1 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

	// ftpv2 i and bgwrpc
	result = system("cdhdsdef -s iftpv1 bgwrpc1 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);


//  ********************************************************************************
//  Clean up!
//  ********************************************************************************


}

void
CdhdsdefTestCase::optionBTest()
{
	int result(0);

//  ********************************************************************************
//  AES_CDH_RC_OK
//  ********************************************************************************

	// define a dest set
	result = system("cdhdsdef -s iftpv1 -b rftpv1 iftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// remove dest set
	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// define dest set
	result = system("cdhdsdef -s iftpv1 -b rftpv1 rftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s bgwrpc1 -b rftpv3 bgwrpc2 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// remove dest set
	result = system("cdhdsrm destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_INCUSAGE
//  ********************************************************************************

	result = system("cdhdsdef -s rftpv1 -b rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_NODEST
//  ********************************************************************************

	result = system("cdhdsdef -s rftpv1 -b asdfasdfasdfasdfasdfasdfasdfasdfa rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s rftpv1 -b 1223 rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_ILLEGALCOMB
//  ********************************************************************************

	result = system("cdhdsdef -s rftpv1 -b bgwrpc1 rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s rftpv1 -b iftpv1 rftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);


}

void
CdhdsdefTestCase::miscTest()
{
	int result (0);

//  ********************************************************************************
//  AES_CDH_RC_OK
//  ********************************************************************************

	// define a dest set
	result = system("cdhdsdef -s bgwrpc1 -b rftpv2 bgwrpc2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s iftpv1 -b rftpv1 iftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_INCUSAGE
//  ********************************************************************************

	result = system("cdhdsdef -s iftpv1 -b rftpv2 iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_UNREAS
//  ********************************************************************************

	result = system("cdhdsdef -s iftpv1 -b rftpv2 iftpv2 33destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s iftpv1 -b rftpv2 iftpv2 asdfghjklöasdfghjklöassd33destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_DESTSETDEF (11)
//  ********************************************************************************

	result = system("cdhdsdef -s iftpv1 -b rftpv2 iftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTSETDEF, result, __LINE__, __FILE__);

//  ********************************************************************************
//   AES_CDH_RC_PHYSFILEERR (20)
//  ********************************************************************************

    result = system("attrib +r c:\\ap\\aes\\data\\cdh\\destset\\cdhdestsetfile.txt > cmdtest.out");
	result = system("cdhdsdef iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_PHYSFILEERR, result, __LINE__, __FILE__);
    result = system("attrib -r c:\\ap\\aes\\data\\cdh\\destset\\cdhdestsetfile.txt > cmdtest.out");

//  ********************************************************************************
//   AES_CDH_RC_NODEST (22)
//  ********************************************************************************

	// s
	result = system("cdhdsdef -s nodest -b rftpv3 iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// b
	result = system("cdhdsdef -s rftpv3 -b nodest iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// p
	result = system("cdhdsdef -s iftpv3 -b rftpv3 nodest destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

//  ********************************************************************************
//   AES_CDH_RC_DESTUSEDINSET (25)
//  ********************************************************************************

	// s
	result = system("cdhdsdef -s iftpv1 -b rftpv3 iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s iftpv1 -b rftpv3 iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

    result = system("cdhdsdef -s iftpv3 -b rftpv3 iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	// b
	result = system("cdhdsdef -s rftpv3 -b rftpv1 iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s rftpv3 -b rftpv1 iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s rftpv3 -b rftpv3 iftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

    result = system("cdhdsdef -s iftpv3 -b rftpv3 rftpv3 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	// p
	result = system("cdhdsdef -s iftpv3 -b rftpv3 iftpv2 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s iftpv3 -b rftpv3 iftpv2 destset3 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	// delete dest set
	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//  ********************************************************************************
//   AES_CDH_RC_ILLEGALCOMB (27)
//  ********************************************************************************

//  ********************************************************************************
//   AES_CDH_RC_USRGNOTMEMBER (31)
//  ********************************************************************************

//  ********************************************************************************
//  Clean up! Remove all defined destinations.
//  ********************************************************************************

	result = system("cdhrm bgwrpc1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

}



