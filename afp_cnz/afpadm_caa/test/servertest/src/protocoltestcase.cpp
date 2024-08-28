#pragma warning(disable:4786)

#include "ProtocolTestCase.h"
#include "AES_GCC_Errorcodes.h"
#include "AES_AFP_Protocol.h"
#include <fstream>
#include <direct.h>

Test*
ProtocolTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("protocol");

	testSuite->addTest (new TestCaller <ProtocolTestCase> ("add values file", addValuesFile));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("add values file - Error", addValuesFile_Error));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("add values api", addValuesApi));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("add values api - Error", addValuesApi_Error));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("add values tq", addValuesTq));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("add values tq - Error", addValuesTq_Error));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("get string", getString));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("set string", setString));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("get string value - Error", getStringValue_Error));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("get int value - Error", getIntValue_Error));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("write to disc", writeToDisc));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("read from file", readFromFile));
	testSuite->addTest (new TestCaller <ProtocolTestCase> ("clear values", clear));

	return testSuite;
}

void
ProtocolTestCase::setUp ()
{

}

void
ProtocolTestCase::tearDown()
{

}

//---------------------------------------------------------
//	addValuesFile()
//---------------------------------------------------------
void
ProtocolTestCase::addValuesFile()
{
	bool result(false);

	aes_afp_protocolfile fileprotocol;

	result = fileprotocol.addIntValue("STATUS", 2);
	assertEquals(true, result, __LINE__, __FILE__);

	result = fileprotocol.addBoolValue("REMOVEBEFORE", true);
	assertEquals(true, result, __LINE__, __FILE__);

	result = fileprotocol.addIntValue("REMOVEDELAY", 10);
	assertEquals(true, result, __LINE__, __FILE__);


	result = fileprotocol.addIntValue("TRANSFERMODE", 1);
	assertEquals(true, result, __LINE__, __FILE__);

	result = fileprotocol.addBoolValue("ISDIRECTORY", false);
	assertEquals(true, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	addValuesFile() - error test
//---------------------------------------------------------
void
ProtocolTestCase::addValuesFile_Error()
{
	bool result(false);

	aes_afp_protocolfile fileprotocol;

	result = fileprotocol.addIntValue("ERROR", 2);
	assertEquals(false, result, __LINE__, __FILE__);

	result = fileprotocol.addBoolValue("ERROR", true);
	assertEquals(false, result, __LINE__, __FILE__);

	result = fileprotocol.addStringValue("ERROR", "C:\\temp");
	assertEquals(false, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	addValuesApi()
//---------------------------------------------------------
void
ProtocolTestCase::addValuesApi()
{
	bool result(false);
	aes_afp_protocolapi apiprotocol;

	result = apiprotocol.addStringValue("SOURCEDIRECTORY", "C:\\temp");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("DESTINATIONSET", "DESTSET1");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("TRANSFERQUEUE", "TQ1");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("USERNAME", "DANNE");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("FILENAME", "nisse");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("USERGROUP", "OLAGROUP");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addIntValue("SENDRETRIES", 10);
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addIntValue("SENDRETRIESDELAY", 20);
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addBoolValue("MANUALINITIATED", false);
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addIntValue("RANGEBEGIN", 1);
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addIntValue("RANGEEND", 1);
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("NEWDESTINATIONSET", "NEWDESTSET");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addIntValue("RENAMETYPE", 1);
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("RENAMETEMPLATE", "pppp");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("TRANSFERMASK", "*.*");
	assertEquals(true, result, __LINE__, __FILE__);

	result = apiprotocol.addStringValue("APPLICATION", "AFP");
	assertEquals(true, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	addValuesApi() - error test
//---------------------------------------------------------
void
ProtocolTestCase::addValuesApi_Error()
{
	bool result(false);
	aes_afp_protocolapi apiprotocol;

	result = apiprotocol.addStringValue("ERROR", "error");
	assertEquals(false, result, __LINE__, __FILE__);

	result = apiprotocol.addIntValue("ERROR", 27);
	assertEquals(false, result, __LINE__, __FILE__);

	result = apiprotocol.addBoolValue("ERROR", false);
	assertEquals(false, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	addValuesTq()
//---------------------------------------------------------
void
ProtocolTestCase::addValuesTq()
{
	bool result(false);
	aes_afp_protocoltq tqprotocol;

	result = tqprotocol.addStringValue("TQOBJECTFILEPATH", "C:\\TEMP");
	assertEquals(true, result, __LINE__, __FILE__);

	result = tqprotocol.addStringValue("TQOBJECTDIRECTORY", "C:\\TEMP");
	assertEquals(true, result, __LINE__, __FILE__);

	result = tqprotocol.addStringValue("ROOTFILEPATH", "C:\\TEMP");
	assertEquals(true, result, __LINE__, __FILE__);

	result = tqprotocol.addStringValue("ORIGINALFILENAME", "FILENAME");
	assertEquals(true, result, __LINE__, __FILE__);

	result = tqprotocol.addBoolValue("TQLOCK", false);
	assertEquals(true, result, __LINE__, __FILE__);

	result = tqprotocol.addBoolValue("ALARM", false);
	assertEquals(true, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	addValuesTq() - error test
//---------------------------------------------------------
void
ProtocolTestCase::addValuesTq_Error()
{
	bool result(false);
	aes_afp_protocoltq tqprotocol;

	result = tqprotocol.addStringValue("ERROR", "error");
	assertEquals(false, result, __LINE__, __FILE__);

	result = tqprotocol.addIntValue("ERROR", 27);
	assertEquals(false, result, __LINE__, __FILE__);

	result = tqprotocol.addBoolValue("ERROR", false);
	assertEquals(false, result, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	getString()
//---------------------------------------------------------
void
ProtocolTestCase::getString()
{
	bool result(false);
	string str("");
	string testStr("APPLICATION=AFP\nDESTINATIONSET=DESTSET\nFILENAME=\nISDIRECTORY=\nMANUALINITIATED=\nNEWDESTINATIONSET=\nRANGEBEGIN=\nRANGEEND=\nREMOVEBEFORE=0\nREMOVEDELAY=30\nRENAMETEMPLATE=\nRENAMETYPE=\nSENDRETRIES=\nSENDRETRIESDELAY=\nSOURCEDIRECTORY=\nSTATUS=\nTRANSFERMASK=\nTRANSFERMODE=\nTRANSFERQUEUE=\nUSERGROUP=\nUSERNAME=\n");
	aes_afp_protocolapi apiprotocol;

	// Setup
	result = apiprotocol.addStringValue("APPLICATION", "AFP");
	if (result == false)
		{
			throw CppUnitException("Protocol test, parameter application", __LINE__, __FILE__);
		}

	result = apiprotocol.addStringValue("DESTINATIONSET", "DESTSET");
	if (result == false)
		{
			throw CppUnitException("Protocol test, parameter destinationset", __LINE__, __FILE__);
		}


	result = apiprotocol.addBoolValue("REMOVEBEFORE", false);
	if (result == false)
		{
			throw CppUnitException("Protocol test, parameter removebefore", __LINE__, __FILE__);
		}


	result = apiprotocol.addIntValue("REMOVEDELAY", 30);
	if (result == false)
		{
			throw CppUnitException("Protocol test, parameter removedelay", __LINE__, __FILE__);
		}

	// Execution
	result = apiprotocol.getString(str);
	assertEquals(0, str.compare(testStr), __LINE__, __FILE__);
}

//---------------------------------------------------------
//	setString()
//---------------------------------------------------------
void
ProtocolTestCase::setString()
{
	bool result(false);
	string str("");
	string testStr("APPLICATION=AFP\nDESTINATIONSET=DESTSET\nFILENAME=FILE1\nISDIRECTORY=1\nMANUALINITIATED=1\nNEWDESTINATIONSET=\nRANGEBEGIN=\nRANGEEND=\nREMOVEBEFORE=0\nREMOVEDELAY=30\nRENAMETEMPLATE=\nRENAMETYPE=\nSENDRETRIES=\nSENDRETRIESDELAY=\nSOURCEDIRECTORY=\nSTATUS=\nTRANSFERMASK=\nTRANSFERMODE=\nTRANSFERQUEUE=\nUSERGROUP=\nUSERNAME=\n");
	string strValue("");
	int intValue(0);
	long longValue(0);
	bool boolValue(false);
	aes_afp_protocolapi apiprotocol;

	apiprotocol.setString(testStr);
	strValue = apiprotocol.getStringValue("APPLICATION");
	assertEquals(0, strValue.compare("AFP"), __LINE__, __FILE__);

	strValue = apiprotocol.getStringValue("DESTINATIONSET");
	assertEquals(0, strValue.compare("DESTSET"), __LINE__, __FILE__);

	strValue = apiprotocol.getStringValue("FILENAME");
	assertEquals(0, strValue.compare("FILE1"), __LINE__, __FILE__);

	boolValue = apiprotocol.getBoolValue("ISDIRECTORY");
	assertEquals(true, boolValue, __LINE__, __FILE__);

	boolValue = apiprotocol.getBoolValue("MANUALINITIATED");
	assertEquals(true, boolValue, __LINE__, __FILE__);

	intValue = apiprotocol.getIntValue("REMOVEDELAY");
	assertEquals(30, intValue, __LINE__, __FILE__);

	longValue = apiprotocol.getLongValue("ABSOLUTECREATIONTIME");
	assertEquals(0, longValue, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	getStringValue_Error()
//---------------------------------------------------------
void
ProtocolTestCase::getStringValue_Error()
{
	bool result(false);
	string str("");
	string testStr("APPLICATION=AFP\nDESTINATIONSET=DESTSET\nFILENAME=FILE1\nISDIRECTORY=1\nMANUALINITIATED=1\nNEWDESTINATIONSET=\nRANGEBEGIN=\nRANGEEND=\nREMOVEBEFORE=0\nREMOVEDELAY=30\nRENAMETEMPLATE=\nRENAMETYPE=\nSENDRETRIES=\nSENDRETRIESDELAY=\nSOURCEDIRECTORY=\nSTATUS=\nTRANSFERMASK=\nTRANSFERMODE=\nTRANSFERQUEUE=\nUSERGROUP=\nUSERNAME=\n");
	string strValue("");
	aes_afp_protocolapi apiprotocol;

	apiprotocol.setString(testStr);

	strValue = apiprotocol.getStringValue("ERROR");
	assertEquals(0, strValue.compare(""), __LINE__, __FILE__);
}

//---------------------------------------------------------
//	getStringValue_Error()
//---------------------------------------------------------
void
ProtocolTestCase::getIntValue_Error()
{
	bool result(false);
	string str("");
	string testStr("APPLICATION=AFP\nDESTINATIONSET=DESTSET\nFILENAME=FILE1\nISDIRECTORY=1\nMANUALINITIATED=1\nNEWDESTINATIONSET=\nRANGEBEGIN=\nRANGEEND=\nREMOVEBEFORE=0\nREMOVEDELAY=30\nRENAMETEMPLATE=\nRENAMETYPE=\nSENDRETRIES=\nSENDRETRIESDELAY=\nSOURCEDIRECTORY=\nSTATUS=\nTRANSFERMASK=\nTRANSFERMODE=\nTRANSFERQUEUE=\nUSERGROUP=\nUSERNAME=\n");
	int intValue(0);
	aes_afp_protocolapi apiprotocol;

	apiprotocol.setString(testStr);

	intValue = apiprotocol.getIntValue("REMOVEDELAY");
	assertEquals(30, intValue, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	writeToDisc()
//---------------------------------------------------------
void
ProtocolTestCase::writeToDisc()
{
	bool result(false);
	string filePath(fileDirectory);
	aes_afp_protocolfile fileprotocol;
	AES_GCC_Errorcodes error;

	string testStr("CREATIONDATE=20030411100144001\nREADYDATE=20030411100144002\nSENDDATE=20030411100144002\nFAILEDDATE=20030411100144003\nDELETEDATE=20030411100144004\nNONEDATE=\nARCHIVEDATE=\nABSOLUTECREATIONTIME=2345678\nABSOLUTEDELETIONTIME=4567789\nISDIRECTORY=1\nTRANSFERMODE=1\nTRANSFERMASK=*.*\n");

	fileprotocol.setString(testStr);
	filePath += "\\afpfiles";
	_mkdir(filePath.c_str() );

	filePath += "\\protocolfile.txt";
	fileprotocol.writeToFile(filePath, error);
	assertEquals(AES_NOERRORCODE, error, __LINE__, __FILE__);
}

//---------------------------------------------------------
//	readFromFile()
//---------------------------------------------------------
void
ProtocolTestCase::readFromFile()
{
	bool result(false);
	string filePath(fileDirectory);
	string readStr("");
	long longValue(0);
	aes_afp_protocolfile fileprotocol;
	AES_GCC_Errorcodes error;

	string testStr("ABSOLUTECREATIONTIME=2345678\nABSOLUTEDELETIONTIME=4567789\nARCHIVEDATE=\nCREATIONDATE=20030411100144001\nDELETEDATE=20030411100144004\nFAILEDDATE=20030411100144003\nISDIRECTORY=1\nNONEDATE=\nREADYDATE=20030411100144002\nREMOVEBEFORE=\nREMOVEDELAY=\nSENDDATE=20030411100144002\nSTATUS=\nTRANSFERMASK=*.*\nTRANSFERMODE=1\n");

	filePath += "\\afpfiles\\protocolfile.txt";

	fileprotocol.readFromFile(filePath, error);
	assertEquals(AES_NOERRORCODE, error, __LINE__, __FILE__);

	fileprotocol.getString(readStr);

	longValue = fileprotocol.getLongValue("ABSOLUTECREATIONTIME");
	assertEquals(2345678, longValue, __LINE__, __FILE__);
	assertEquals(0, testStr.compare(readStr), __LINE__, __FILE__);

	removeTestFiles();

}

//---------------------------------------------------------
//	clear()
//---------------------------------------------------------
void
ProtocolTestCase::clear()
{
	bool result(false);
	string filePath(fileDirectory);
	string readStr("");
	aes_afp_protocolfile fileprotocol;

	string testStr("CREATIONDATE=20030411100144001\nREADYDATE=20030411100144002\nSENDDATE=20030411100144002\nFAILEDDATE=20030411100144003\nDELETEDATE=20030411100144004\nNONEDATE=\nARCHIVEDATE=\nABSOLUTECREATIONTIME=2345678\nABSOLUTEDELETIONTIME=4567789\nISDIRECTORY=1\nTRANSFERMODE=1\nTRANSFERMASK=*.*\n");
	string clearedStr("ABSOLUTECREATIONTIME=\nABSOLUTEDELETIONTIME=\nARCHIVEDATE=\nCREATIONDATE=\nDELETEDATE=\nFAILEDDATE=\nISDIRECTORY=\nNONEDATE=\nREADYDATE=\nREMOVEBEFORE=\nREMOVEDELAY=\nSENDDATE=\nSTATUS=\nTRANSFERMASK=\nTRANSFERMODE=\n");

	fileprotocol.setString(testStr);
	fileprotocol.clearValues();

	fileprotocol.getString(testStr);

	assertEquals(0, testStr.compare(clearedStr), __LINE__, __FILE__);
}


//---------------------------------------------------------
//	removeTestFiles()
//---------------------------------------------------------
void
ProtocolTestCase::removeTestFiles()
{
	string path(fileDirectory);
	string cmd("del /Q ");
	cmd += fileDirectory + "\\afpfiles\\*.* > cmd.out";
	system(cmd.c_str() );
	path += "\\afpfiles";
	_rmdir(path.c_str() );
}