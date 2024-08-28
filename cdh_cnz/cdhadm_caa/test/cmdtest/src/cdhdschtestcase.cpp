#pragma warning(disable:4786)
#include "CdhdschTestCase.h"
#include "aes_cdh_resultimplementation.h"

Test*
CdhdschTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhdsch");

	testSuite->addTest (new TestCaller <CdhdschTestCase> ("misc test", miscTest));
	
	return testSuite;
}

void
CdhdschTestCase::setUp()
{
	
}

void
CdhdschTestCase::tearDown()
{

}

void
CdhdschTestCase::miscTest()
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

	result = system("cdhdef -a 127.0.0.1 -t bgwrpc bgwrpc3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -h alias rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -h alias rftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -h alias rftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -a 127.0.0.1 -t ftpv2 iftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// define dest sets
	result = system("cdhdsdef -s bgwrpc1 -b rftpv1 bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsdef -s iftpv1 -b rftpv2 iftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


//  ********************************************************************************
//  AES_CDH_RC_OK
//  ********************************************************************************

	// change S not active
	result = system("cdhdsch -s bgwrpc3 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// make S active
	result = system("cdhdssw bgwrpc3 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// change S active
	result = system("cdhdsch -s bgwrpc1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsch -s bgwrpc3 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// change B not active
	result = system("cdhdsch -b rftpv3 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// make B active
	result = system("cdhdssw rftpv3 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// change B active
	result = system("cdhdsch -b rftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsch -b rftpv3 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// change P
	result = system("cdhdssw iftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	
	result = system("cdhdsch iftpv3 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// make prim dest active
	result = system("cdhdsch -x destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdssw bgwrpc3 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // remove and add S
	result = system("cdhdsrm bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsch -s bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
    // dest set status after changes:
	// destset1: S=bgwrpc2, B=rftpv1, P=bgwrpc3
	// destset2: S=iftpv1,  B=rftpv3, P=iftpv3

//  ********************************************************************************
//  AES_CDH_RC_INCUSAGE
//  ********************************************************************************

	result = system("cdhdsch -q -b rftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsch -s iftpv1 -b rftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsch -- destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

	result = system("cdhdsch -x rftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);

//  ********************************************************************************
//   AES_CDH_RC_PHYSFILEERR (20)
//  ********************************************************************************

    result = system("attrib +r c:\\ap\\aes\\data\\cdh\\destset\\cdhdestsetfile.txt > cmdtest.out");
	result = system("cdhdsch -x destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_PHYSFILEERR, result, __LINE__, __FILE__);

	result = system("cdhdsch bgwrpc1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_PHYSFILEERR, result, __LINE__, __FILE__);
    
    result = system("attrib -r c:\\ap\\aes\\data\\cdh\\destset\\cdhdestsetfile.txt > cmdtest.out");

    result = system("cdhdsrm bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("attrib +r c:\\ap\\aes\\data\\cdh\\destset\\cdhdestsetfile.txt > cmdtest.out");
    result = system("cdhdsch -s bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_PHYSFILEERR, result, __LINE__, __FILE__);

    result = system("attrib -r c:\\ap\\aes\\data\\cdh\\destset\\cdhdestsetfile.txt > cmdtest.out");
    result = system("cdhdsch -s bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_NODEST (22)
//  ********************************************************************************

	result = system("cdhdsch -s bgwrpc4 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdsch -b rftpv4 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdsch iftpv4 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_DESTUSEDINSET (25)
//  ********************************************************************************

	result = system("cdhdsch -b rftpv1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	result = system("cdhdsch -s iftpv3 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

	result = system("cdhdsch -s iftpv3 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTUSEDINSET, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_ILLEGALCOMB (27)
//  ********************************************************************************

	result = system("cdhdsch -s iftpv2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

	result = system("cdhdsch -s bgwrpc1 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

	result = system("cdhdsch -b bgwrpc1 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

	result = system("cdhdsch -b iftpv2 destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_ILLEGALCOMB, result, __LINE__, __FILE__);

//  ********************************************************************************
//  AES_CDH_RC_CMDAUTHERR (30)
//  ********************************************************************************

//  ********************************************************************************
//  AES_CDH_RC_DESTNOTINDSET
//  ********************************************************************************

	// make a swap P <---> S but there is no S destination! 

	result = system("cdhdsrm bgwrpc2 destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsch -x destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_DESTNOTINDSET, result, __LINE__, __FILE__);

//  ********************************************************************************
//  Clean up! Remove all defined destinations.and destination sets
//  ********************************************************************************

	result = system("cdhdsrm destset1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdsrm destset2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm bgwrpc3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm rftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv2 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhrm iftpv3 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    (void) system("vdrm -n \"Default FTP Site\" -v alias > cmdtest.out");
}