
#pragma warning(disable:4786)
#include "CdhsfTestCase.h"
#include "aes_cdh_resultimplementation.h"
#include "iostream"


Test* CdhsfTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhsf");

    testSuite->addTest (new TestCaller <CdhsfTestCase> ("test long and short", longAndShort));
    testSuite->addTest (new TestCaller <CdhsfTestCase> ("responding test long and short", longAndShortResp));
    testSuite->addTest (new TestCaller <CdhsfTestCase> ("test SFTP long and short", longAndShortSFTP));
    testSuite->addTest (new TestCaller <CdhsfTestCase> ("test transfer", sendFile));
    testSuite->addTest (new TestCaller <CdhsfTestCase> ("responding test transfer", sendFileResp));
//    testSuite->addTest (new TestCaller <CdhsfTestCase> ("test start & stop transfer", stopTransfer));
    testSuite->addTest (new TestCaller <CdhsfTestCase> ("test SFTP transfer", sendFileSFTP));
	return testSuite;
}

void CdhsfTestCase::setUp ()
{
	
}

void CdhsfTestCase::tearDown()
{

}


void CdhsfTestCase::longAndShort()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // send file - min length destination name
    result = system("cdhdef -a guanntw2 -t ftpv2 -u cdhftp dest0 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest0 a > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf a Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - min length destination name
    result = system("cdhsf a Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm a > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - max length destination name
    result = system("cdhdsdef dest0 a1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf a1234567890123456789012345678901 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - max length destination name
    result = system("cdhsf a1234567890123456789012345678901 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - min length remote directory name
    result = system("cdhdef -a guanntw2 -t ftpv2 -r a -u cdhftp dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest1 destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - min length remote directory name
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - max length remote directory name
    cout << "Type 'rpcuser' and enter to continue";
    result = system("cdhdef -a 172.17.234.4 -t ftpv2 -r a12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234/ -u rpcuser -p dest2 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest2 destset2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf destset2 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - max length remote directory name
    result = system("cdhsf destset2 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - min length remote system address
    cout << "Please wait a moment..." << endl;
    result = system("cdhdef -a 1 -t ftpv2 -u cdhftp dest4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest4 destset4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf destset4 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_CONNECTERR, result, __LINE__, __FILE__);

    // send dir - min length remote system address
    result = system("cdhsf destset4 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_CONNECTERR, result, __LINE__, __FILE__);

    // send file - max length remote system address
    result = system("cdhdef -a 123456789012345678901234567890 -t ftpv2 -u cdhftp dest3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest3 destset3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf destset3 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_CONNECTERR, result, __LINE__, __FILE__);

    // send dir - max length remote system address
    result = system("cdhsf destset3 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_CONNECTERR, result, __LINE__, __FILE__);
    cout << "Finished" << endl;

// ********************************************************************************
// REMOVE DEFINITIONS
//
// Destinations created for testing purposes above. Remove them to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

    result = system("cdhdsrm a1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm destset2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm destset3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm destset4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest0 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest4 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

}

void CdhsfTestCase::longAndShortResp()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    result = system("vdrm -n \"Default FTP Site\" -v cmdtest > cmdtest.out");
    result = system("md C:\\ap\\Aes\\data\\cmdtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("vdcreate -n \"Default FTP Site\" -a cmdtest -d C:\\ap\\Aes\\data\\cmdtest  > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - min length destination name
    result = system("cdhdef -t ftpv2 -c r -h cmdtest b > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef b a > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf a Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("del /f C:\\ap\\Aes\\data\\cmdtest\\b\\ready\\cdhtestfil.txt > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

    // send dir - min length destination name
    result = system("cdhsf a Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("rmdir /q /s C:\\ap\\Aes\\data\\cmdtest\\b\\ready\\cdhtest > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

    // send file - max length destination name
    result = system("cdhdef -t ftpv2 -c r -h cmdtest b1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef b1234567890123456789012345678901 a1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf a1234567890123456789012345678901 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("del /f C:\\ap\\Aes\\data\\cmdtest\\b1234567890123456789012345678901\\ready\\cdhtestfil.txt > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

    // send dir - max length destination name
    result = system("cdhsf a1234567890123456789012345678901 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("rmdir /q /s C:\\ap\\Aes\\data\\cmdtest\\b1234567890123456789012345678901\\ready\\cdhtest > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

// ********************************************************************************
// REMOVE DEFINITIONS
//
// Destinations created for testing purposes above. Remove them to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

    result = system("cdhdsrm a1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm b1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm a > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm b > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("rmdir C:\\ap\\Aes\\data\\cmdtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("vdrm -n \"Default FTP Site\" -v cmdtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}

void CdhsfTestCase::longAndShortSFTP()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // send file - min length destination name
    cout << "Type 'rpcuser' and enter to continue";
    result = system("cdhdef -a 172.17.234.4 -t sftpv2 -u rpcuser -p dest0 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest0 a > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf a Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - min length destination name
    result = system("cdhsf a Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm a > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - max length destination name
    result = system("cdhdsdef dest0 a1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf a1234567890123456789012345678901 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - max length destination name
    result = system("cdhsf a1234567890123456789012345678901 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - min length remote directory name
    cout << "Type 'rpcuser' and enter to continue";
    result = system("cdhdef -a 172.17.234.4 -t sftpv2 -r a -u rpcuser -p dest1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest1 destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - min length remote directory name
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file - max length remote directory name
    cout << "Type 'rpcuser' and enter to continue";
    result = system("cdhdef -a 172.17.234.4 -t sftpv2 -r a12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234/ -u rpcuser -p dest2 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest2 destset2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf destset2 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - max length remote directory name
    result = system("cdhsf destset2 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

/*
    // send file - min length remote system address
    result = system("cdhdef -a 1 -t sftpv2 -u cdhftp dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhsf dest1 c:\\temp\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send dir - min length remote system address
    result = system("cdhsf dest1 c:\\temp\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
*/
    // send file - max length remote system address
    cout << "Type 'rpcuser' and enter to continue";
    result = system("cdhdef -a 123456789012345678901234567890 -t sftpv2 -u cdhftp -p dest3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest3 destset3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    cout << "Please wait a moment..." << endl;
    result = system("cdhsf destset3 c:\\temp\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_CONNECTERR, result, __LINE__, __FILE__);

    // send dir - max length remote system address
    result = system("cdhsf destset3 c:\\temp\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_CONNECTERR, result, __LINE__, __FILE__);
    cout << "Finished" << endl;


// ********************************************************************************
// REMOVE DEFINITIONS
//
// Destinations created for testing purposes above. Remove them to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

    result = system("cdhdsrm a1234567890123456789012345678901 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm destset2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdsrm destset3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm dest0 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest3 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}

void CdhsfTestCase::sendFile()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    result = system("cdhdef -a guanntw2 -t ftpv2 -r a -o yes -u cdhftp dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest1 destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhch -o no dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file to directory testfile
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt testfile > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory to directory testdir
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest testdir > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file to directory testfile with new name newfile.txt
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt testfile newfile.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory to directory testdir with new name newdir
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest testdir newdir > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// REMOVE DEFINITIONS
//
// Destinations created for testing purposes above. Remove them to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

    result = system("cdhdsrm destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhrm dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

}

void CdhsfTestCase::sendFileResp()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    result = system("vdrm -n \"Default FTP Site\" -v cmdtest > cmdtest.out");
    result = system("md C:\\ap\\Aes\\data\\cmdtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("vdcreate -n \"Default FTP Site\" -a cmdtest -d C:\\ap\\Aes\\data\\cmdtest  > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -t ftpv2 -c r -o yes -h cmdtest dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest1 destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file
    result = system("attrib -r C:\\ap\\Aes\\data\\cmdtest\\dest1\\ready\\cdhtestfil.txt > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhch -o no dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file to directory testfile
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt testfile > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory to directory testdir
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest testdir > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file to directory testfile with new name newfile.txt
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt testfile newfile.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory to directory testdir with new name newdir
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest testdir newdir > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// REMOVE DEFINITIONS
//
// Destinations created for testing purposes above. Remove them to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

    result = system("rmdir /q /s C:\\ap\\Aes\\data\\cmdtest\\dest1\\ready > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);
    result = system("md C:\\ap\\Aes\\data\\cmdtest\\dest1\\ready > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);
    result = system("cdhdsrm destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("rmdir C:\\ap\\Aes\\data\\cmdtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("vdrm -n \"Default FTP Site\" -v cmdtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
}

void CdhsfTestCase::sendFileSFTP()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    cout << "Type 'rpcuser' and enter to continue";
    result = system("cdhdef -a 172.17.234.4 -t sftpv2 -r a -o yes -u rpcuser -p dest1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhdsdef dest1 destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhch -o no dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file to directory testfile with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt testfile > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory to directory testdir with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest testdir > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send file to directory testfile with new name newfile.txt with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtestfil.txt testfile newfile.txt > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // send directory to directory testdir with new name newdir with SFTP
    result = system("cdhsf destset1 Z:\\ntaes\\cdh\\test\\data\\cdhtest testdir newdir > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// REMOVE DEFINITIONS
//
// Destinations created for testing purposes above. Remove them to make
// sure that CDH is not polluted before we continue
// ********************************************************************************

    result = system("cdhdsrm destset1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

}

void CdhsfTestCase::stopTransfer()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************


// ********************************************************************************
// REMOVE DEFINITIONS
//
// Destinations created for testing purposes above. Remove them to make
// sure that CDH is not polluted before we continue
// ********************************************************************************


}