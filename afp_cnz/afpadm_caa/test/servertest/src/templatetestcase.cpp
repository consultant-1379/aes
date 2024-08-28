#pragma warning(disable:4786)

#include "TemplateTestCase.h"
#include "AES_GCC_Errorcodes.h"
#include "AES_AFP_Template.h"
#include <direct.h>
#include <fstream>

Test*
TemplateTestCase::suite ()
{
	TestSuite* testSuite = new TestSuite ("template");

	testSuite->addTest (new TestCaller <TemplateTestCase> ("test faulty template", faultyTemplate));
	testSuite->addTest (new TestCaller <TemplateTestCase> ("test correct template", correctTemplate));
	testSuite->addTest (new TestCaller <TemplateTestCase> ("test create file name", createFileName));
	testSuite->addTest (new TestCaller <TemplateTestCase> ("test file rename", fileRename));

	return testSuite;
}

void
TemplateTestCase::setUp ()
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);	
	currentWorkDir = buffer;

	char buf[64];

	struct tm *newtime;
  long ltime;
	time( &ltime );
	newtime = gmtime( &ltime );

	sprintf(buf, "%02d\0", newtime->tm_year);
	yearShort = buf;
	yearShort.erase(0, 1);
	sprintf(buf, "%04d\0", newtime->tm_year+1900);
	yearLong = buf;
	sprintf(buf, "%02d\0", newtime->tm_mon+1);
	month = buf;
	sprintf(buf, "%02d\0", newtime->tm_mday);
	day = buf;
	sprintf(buf, "%02d\0", newtime->tm_hour);
	hour = buf;
	sprintf(buf, "%02d\0", newtime->tm_min);
	minute = buf;
	
}

void
TemplateTestCase::tearDown()
{
	string tmpStr(currentWorkDir);
	string tmpStrFile("");

	tmpStr += "\\persistendir";
	tmpStrFile = tmpStr + "\\pers.nr";
	remove(tmpStrFile.c_str() );
	rmdir(tmpStr.c_str() );
}

//---------------------------------------------------------
//	fileRename()
//---------------------------------------------------------
void
TemplateTestCase::fileRename()
{
	string tmpStr(currentWorkDir);
	string tmpNewFile("arne");
	bool result(false);
	string tmpOldFile(currentWorkDir);

	tmpStr += "\\persistentdir";
	mkdir(tmpStr.c_str() );

	// Create test files
	string tmpString(currentWorkDir);
	tmpString += "\\file1";
	ofstream fout(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("template test, file1 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	tmpString = currentWorkDir;
	tmpString += "\\file2";
	fout.open(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("template test, file2 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	tmpString = currentWorkDir;
	tmpString += "\\file3";
	fout.open(tmpString.c_str() );
	if (!fout)
		{
			throw CppUnitException("template test, file3 could not be opened", __LINE__, __FILE__);
		}
	fout << "Hejsan" << endl;
	fout.close();

	// End create test files

	aes_afp_template* fileTemplate = new aes_afp_template(tmpStr);
	fileTemplate->setTemplate("yy");
	tmpString = currentWorkDir;
	tmpOldFile = tmpString + "\\file1";
	result = fileTemplate->renameFileLocal(tmpOldFile, tmpNewFile);
	assertEquals(true, result, __LINE__, __FILE__);
	result = tmpNewFile.compare("01");
	assertEquals(true, result, __LINE__, __FILE__);

	tmpString = currentWorkDir;
	tmpOldFile = tmpString + "\\file2";
	result = fileTemplate->renameFileLocal(tmpOldFile, tmpNewFile);
	assertEquals(true, result, __LINE__, __FILE__);

	result = tmpNewFile.compare("01");
	assertEquals(true, result, __LINE__, __FILE__);

	tmpString = currentWorkDir;
	tmpOldFile = tmpString + "\\file3";
	result = fileTemplate->renameFileLocal(tmpOldFile, tmpNewFile);
	assertEquals(true, result, __LINE__, __FILE__);

	result = tmpNewFile.compare("01");
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;
}

//---------------------------------------------------------
//	createFileName()
//---------------------------------------------------------
void
TemplateTestCase::createFileName()
{
	string tmpStr(currentWorkDir);
	string newFile("");
	string testFilename("\\arne");
	bool result(false);
	string tmpTestAgainst("");

	tmpStr += "\\persistentdir";
	mkdir(tmpStr.c_str() );

	aes_afp_template* fileTemplate = new aes_afp_template(tmpStr);

	fileTemplate->setTemplate("yyyy");
	newFile = fileTemplate->createFileName(testFilename);
	if (newFile == yearLong)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("yy");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst += yearShort;
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("yyyymm");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst += yearLong + month;
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);


	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("yyyymmdd");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst += yearLong + month + day;
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	//Flip to mmddyyyy
	fileTemplate = new aes_afp_template(tmpStr);

	fileTemplate->setTemplate("yyyy");

	tmpTestAgainst = "";
	tmpTestAgainst += yearLong;
	newFile = fileTemplate->createFileName(testFilename);
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("mmyyyy");

	tmpTestAgainst = "";
	tmpTestAgainst += month + yearLong;
	newFile = fileTemplate->createFileName(testFilename);
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);


	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("ddmmyyyy");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst += day + month + yearLong;
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("n");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "0";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "1";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("nnnn");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "0000";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "0001";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("ddnnnnmmyy");


	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst += day + "0000" + month + yearShort;
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("p");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "0";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "1";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("pppp");
	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "0000";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	newFile = fileTemplate->createFileName(testFilename);

	tmpTestAgainst = "";
	tmpTestAgainst = "0001";
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;


	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("ddfmmppppyyyy");
	newFile = fileTemplate->createFileName(testFilename);
	tmpTestAgainst = "";
	tmpTestAgainst += day + testFilename.substr(1) + month + "0000" + yearLong;
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	newFile = fileTemplate->createFileName(testFilename);
	tmpTestAgainst = "";
	tmpTestAgainst += day + testFilename.substr(1) + month + "0001" + yearLong;
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = false;
	fileTemplate->setTemplate("pmmyyyyddf");
	newFile = fileTemplate->createFileName(testFilename);
	tmpTestAgainst = "";
	tmpTestAgainst += "0" + month + yearLong + day + testFilename.substr(1);
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	newFile = fileTemplate->createFileName(testFilename);
	tmpTestAgainst = "";
	tmpTestAgainst += "1" + month + yearLong + day + testFilename.substr(1);
	if (newFile == tmpTestAgainst)
		{
			result = true;
		}
	assertEquals(true, result, __LINE__, __FILE__);

	delete fileTemplate;

	// Clean up
	string tmpRem(tmpStr);
	tmpRem += "\\pers.nr";
	remove(tmpRem.c_str() );
	rmdir(tmpStr.c_str() );
}

//---------------------------------------------------------
//	faultyTemplate()
//---------------------------------------------------------
void
TemplateTestCase::faultyTemplate()
{
	string tmpStr(currentWorkDir);
	bool result(false);

	tmpStr += "\\persistentdir";
	mkdir(tmpStr.c_str() );
	aes_afp_template* fileTemplate = new aes_afp_template(tmpStr);

	result = fileTemplate->setTemplate("y");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyy");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yymmy");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("1111");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyy");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyyymmddHHMMSSnnnnppppf");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyyyfyyyy");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyyymmddHHMMSSnpf");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yymmdddHHMMSSnf");
	assertEquals(false, result, __LINE__, __FILE__);
	delete fileTemplate;
}

//---------------------------------------------------------
//	correctTemplate()
//---------------------------------------------------------
void
TemplateTestCase::correctTemplate()
{
	string tmpStr(currentWorkDir);
	bool result(false);

	tmpStr += "\\persistentdir";
	mkdir(tmpStr.c_str() );

	aes_afp_template* fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yy");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyyy");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("mm");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("dd");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("HH");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("MM");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("SS");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("n");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("nnnn");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("p");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("pppp");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("z");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("f");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyyymmddHHMMSSnnnnzf");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;

	fileTemplate = new aes_afp_template(tmpStr);
	result = fileTemplate->setTemplate("yyyymmddHHMMSSppppzf");
	assertEquals(true, result, __LINE__, __FILE__);
	delete fileTemplate;
}