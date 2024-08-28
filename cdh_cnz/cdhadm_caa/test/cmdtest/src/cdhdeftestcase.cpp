#pragma warning(disable:4786)
#include "CdhdefTestCase.h"
#include "aes_cdh_resultimplementation.h"


Test*
CdhdefTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("cdhdef");

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
    testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -e", optionETest));

    testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -s", optionSTest));
    testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -d", optionDTest));
    testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -m", optionMTest));
	testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -h", optionHTest));
    //-p prompts for password
	//testSuite->addTest (new TestCaller <CdhdefTestCase> ("test option -p", optionPTest));
	
	return testSuite;
}

void
CdhdefTestCase::setUp ()
{
    (void) system("vdcreate -n \"Default FTP Site\" -a alias -d C:\\  > cmdtest.out");
}

void
CdhdefTestCase::tearDown()
{
    (void) system("vdrm -n \"Default FTP Site\" -v alias > cmdtest.out");
}

void
CdhdefTestCase::optionATest()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // normal IP address
	result = system("cdhdef -a 123.123.123.123 -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// normal text IP address
	result = system("cdhdef -a baretta.osd.se -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    // too long IP address
	result = system("cdhdef -a tolongIPaddressnametobeaccepted -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// -a option claims -t option
	result = system("cdhdef -a baretta.osd.se > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // initiating ftpv2 without -a option 
	result = system("cdhdef -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // initiating sftpv2 without -a option 
    cout << "Write any password and enter to continue";
	result = system("cdhdef -t sftpv2 -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // BGWRPC without -a option
	result = system("cdhdef -t bgwrpc DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// responding ftpv2 with -a option
	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -c r DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
}

void
CdhdefTestCase::optionTTest()
{
	//
    // -t specifies the transfer type for the destination
    //

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
   
    // initiating ftpv2
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	
    // initiating sftpv2
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	
    // BGW RPC
    result = system("cdhdef -a 123.123.123.123 -t bgwrpc DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    // Illegal transfer type ftpv1
	result = system("cdhdef -a 123.123.123.123 -t ftpv1 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NOTRANS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    
    // Illegal transfer type ftpv3
	result = system("cdhdef -a 123.123.123.123 -t ftpv3 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NOTRANS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // Illegal transfer type iforpc
    result = system("cdhdef -a 123.123.123.123 -t iforpc DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_NOTRANS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
}

void
CdhdefTestCase::optionCTest()
{
    //
    // option -c specifies the connect type
    //

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
   
    // initiating ftpv2
	result = system("cdhdef -a baretta.osd.se -t ftpv2 -c i DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


    // initiating sftpv2
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a baretta.osd.se -t sftpv2 -c i -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


    // responding ftpv2
	result = system("cdhdef -t ftpv2 -c r -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    // initiating specified for BGW RPC
	result = system("cdhdef -a baretta.osd.se -t bgwrpc -c i DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // responding specified for BGW RPC
	result = system("cdhdef -a baretta.osd.se -t bgwrpc -c r -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // responding specified for sftpv2
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a baretta.osd.se -t sftpv2 -c r -u anyname -p -h alias DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // invalid connect type for ftpv2
	result = system("cdhdef -a baretta.osd.se -t ftpv2 -c v DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    
    // invalid connect type for sftpv2
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a baretta.osd.se -t sftpv2 -c v -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    
    // invalid connect type specified for BGW RPC
	result = system("cdhdef -a baretta.osd.se -t bgwrpc -c p DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
}

void
CdhdefTestCase::optionOTest()
{
    //
    // Overwrite is only valid for ftpv2
    //

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // Overwrite -o yes
	result = system("cdhdef -t ftpv2 -c r -o yes -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// Overwrite -o no
	result = system("cdhdef -t ftpv2 -c r -o no -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // Overwrite -o yes
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a baretta.osd.se -t sftpv2 -c i -o yes -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	// Overwrite -o no
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a  baretta.osd.se -t sftpv2 -c i -o no -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

	// Not valid transfer type
	result = system("cdhdef -a 123.123.123.123 -t bgwrpc -c r -o yes -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// Not valid -o type 
	result = system("cdhdef -t ftpv2 -c r -o 4 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
}

void
CdhdefTestCase::optionRTest()
{
    //
    // -r specifies the remote directory and is only valid for
    // initiating ftpv2 and sftpv2
    //

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    
    // initiating ftpv2 and remote directory without -c i  (-c i is default if not specified)
	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -r /anydir DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
    // initiating ftpv2 and remote directory
	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -c i -r /anydir DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // initiating sftpv2 and remote directory without -c i  (-c i is default if not specified)
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a 123.123.123.123 -t sftpv2 -r /anydir -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
    // initiating sftpv2 and remote directory
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a 123.123.123.123 -t sftpv2 -c i -r /anydir -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

      
// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    
    // responding ftpv2 and remote directory
	result = system("cdhdef -t ftpv2 -c r -r /anydir -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // BGW RPC as initiating with remote directory
	result = system("cdhdef -a 123.123.123.123 -t bgwrpc -c i -r /anydir DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
	
}

void
CdhdefTestCase::optionNTest()
{
	int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
    
    // option -n portnr, demands -t ftpv2 and -c i
	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -n 100 -c i DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // option -n portnr, demands -t sftpv2 and -c i
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a 123.123.123.123 -t sftpv2 -n 100 -c i -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
    
// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************
    
    // -n range 0-65535
	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -n -1 -c i DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// -n range 0-65535
	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -n 65536 -c i DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // -n range 0-65535
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a 123.123.123.123 -t sftpv2 -n -1 -c i -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// -n range 0-65535
    cout << "Write any password and enter to continue";
	result = system("cdhdef -a 123.123.123.123 -t sftpv2 -n 65536 -c i -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// -c must be i since port number is valid only for initiating FTP
	result = system("cdhdef -t ftpv2 -n 100 -c r -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // It should not be possible to specify a port number for BGW RPC
    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -n 94 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

}

void
CdhdefTestCase::optionUTest()
{
    // Option -u specifies the user name to be used at the remote system.
    // Only valid for -t ftpv2 and -c i

    int result(0);
	
// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // initiating ftpv2 with user name using default connect type
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -u anyname DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


    // initiating sftpv2 with user name using default connect type
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


    // initiating ftpv2 with user name
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -u anyname -c i DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
    // initiating sftpv2 with user name
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -u anyname -c i -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
    // initiating ftpv2 and user name with max allowded characters (128)
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -u abcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmn -c i DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


    // initiating sftpv2 and user name with max allowded characters (128)
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -u abcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmn -c i -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


    // initiating ftpv2 and user name with max allowded characters (128) using default connect type
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -u abcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmn DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // initiating sftpv2 and user name with max allowded characters (128) using default connect type
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -u abcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmn -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    // initiating ftpv2 and user name with 129 characters (max is 128)
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -u abcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmno -c i DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // initiating sftpv2 and user name with 129 characters (max is 128)
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -u abcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmnopqrstuvwxyzabcdefghikjlklmno -c i -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // initiating sftpv2 with user name and no password
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -u anyname -c i DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    // initiating sftpv2 with no user name and no password
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -c i DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    // initiating sftpv2 with no user name
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -c i -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // responding ftpv2 with user name
    result = system("cdhdef -t ftpv2 -u anyname -c r -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    
    // bgwrpc with user name
	result = system("cdhdef -a 123.123.123.123 -t bgwrpc -u anyname DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
}


void
CdhdefTestCase::optionFTest()
{
    // Option -f specifies file notification

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// 010204 start

    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -y 10 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// 010204 end

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    // ftpv2 with file notification without port
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	// ftpv2 with file notification without alias
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);



// 010204 start
    
    // ftpv2 with two -f 
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -f 127.0.0.1 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // ftpv2 with two -f and port number
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -f 127.0.0.1 -x 11001 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    // ftpv2 with -f and two -x
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -x 11001 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);    
    
// 010204 end

}



void
CdhdefTestCase::optionXTest()
{
    // Option -x specifies the file notification port number

    int result(0);
// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************

    // ftpv2 with notification address and notification port
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 8000 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    // ftpv2 with notification address and notification port
    result = system("cdhdef -t ftpv2 -c r -x 8000 -f 127.0.0.1 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    // ftpv2 with port without file notification
    result = system("cdhdef -t ftpv2 -c r -x 8000 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // ftpv2 with file notification and with bad port (port nr range: 0 - 65535)
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x -1 -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__); // Should it be AES_CDH_RC_UNREAS instead ????
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // ftpv2 with bad port with file notification  (port nr range: 0 - 65535)
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 65536 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // ftpv2 with two -x
    result = system("cdhdef -t ftpv2 -c r -x 11001 -x 11001 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

}



void
CdhdefTestCase::optionYTest()
{
    // Option -y specifies response time

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************




// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    // ftpv2 with file notification and with bad respond time
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -y -1 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    
    // ftpv2 with file notification and with bad respond time
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -y 65536 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    // ftpv2 with respond_time only
    result = system("cdhdef -t ftpv2 -c r -y 12 -h alias DEST1 > cmdtest.out");
	assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
	result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

}


void
CdhdefTestCase::optionETest()
{
    // Option -e specifies the event type

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
    
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -e alarm -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    
    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -e event -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    



// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 11001 -e bogus -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);


    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x -1 -e alarm -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    result = system("cdhdef -t ftpv2 -c r -f 127.0.0.1 -x 65536 -e alarm -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

}

void
CdhdefTestCase::optionSTest()
{
    // Option -s specifies send retries

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
    
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -s 0 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -s 100 -d 1 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -s 100 -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -s 1 -d 60 -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -s 0 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -s 100 -m 10000 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -s -1 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -s 101 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    result = system("cdhdef -t ftpv2 -c r -s 10 -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdef -a 123.123.123.123 -t bgwrpc -s 0 -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

}

void
CdhdefTestCase::optionDTest()
{
    // Option -d specifies retry delay for file transfer

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
    
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -d 60 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    
    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -d 1 -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -d 0 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -d 61 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    result = system("cdhdef -t ftpv2 -c r -d 10 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -d 10 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -d 60 -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

}

void
CdhdefTestCase::optionMTest()
{
    // Option -d specifies retry delay for file transfer

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
    
    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -m 200 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -m 199 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -m 10001 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_UNREAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
    
    result = system("cdhdef -t ftpv2 -c r -m 300 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -m 300 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    cout << "Write any password and enter to continue";
    result = system("cdhdef -a 123.123.123.123 -t sftpv2 -m 300 -u anyname -p DEST1 > cmdtest.out");
    cout << endl;
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdef -a 123.123.123.123 -t bgwrpc -m 200 -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);
}

void
CdhdefTestCase::optionHTest()
{
    // Option -d specifies retry delay for file transfer

    int result(0);

// ********************************************************************************
// VALID DEFINITIONS
// ********************************************************************************
    
    (void) system("vdcreate -n \"Default FTP Site\" -a alias1 -d C:\\  > cmdtest.out");
    (void) system("vdcreate -n \"Default FTP Site\" -a alias2 -d C:\\  > cmdtest.out");

    result = system("cdhdef -t ftpv2 -c r -h alias1 dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm dest1 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -f 123.123.123.123 -x 11001 -h alias2 dest2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);
    result = system("cdhrm dest2 > cmdtest.out");
    assertEquals(AES_CDH_RC_OK, result, __LINE__, __FILE__);


// ********************************************************************************
// INVALID DEFINITIONS
// ********************************************************************************

    result = system("cdhdef -a 123.123.123.123 -t bgwrpc -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    result = system("cdhdef -a 123.123.123.123 -t ftpv2 -c i -h alias DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -c i -h alias -h DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdef -a 123.123.123.123 -t ftpv2 -c i -h alias -h alias2 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_INCUSAGE, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -h doesnotexist DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NOALIAS, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    result = system("vdcreate -n \"Default FTP Site\" -a alias3 -d C:\\doesnotexit  > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

	result = system("cdhdef -t ftpv2 -c r -h alias3 DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NOVIRTUALPATH, result, __LINE__, __FILE__);
    result = system("cdhrm DEST1 > cmdtest.out");
    assertEquals(AES_CDH_RC_NODEST, result, __LINE__, __FILE__);

    result = system("vdrm -n \"Default FTP Site\" -v alias3 > cmdtest.out");
    assertEquals(0, result, __LINE__, __FILE__);

    (void) system("vdrm -n \"Default FTP Site\" -v alias1 > cmdtest.out");
    (void) system("vdrm -n \"Default FTP Site\" -v alias2 > cmdtest.out");
}
