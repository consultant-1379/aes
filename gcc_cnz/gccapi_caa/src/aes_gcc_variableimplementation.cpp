/* =================================================================== */
/**
	@file aes_gcc_variableimplementation.cpp

	Class method implementation for aes_gcc_variableimplementation.h

	DESCRIPTION
	Data disk path can be obtained using this API

	ERROR HANDLING
	General rule:

	The error handling is specified for each method.

	No methods initiate or send error reports unless specified.

	@version 1.1.1

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------

	N/A       12/08/2011     xnadnar      Initial Release
 */
/* =================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include "aes_gcc_variableimplementation.h"
#include "ACS_APGCC_CommonLib.h"
#include "event.h"
#include "aes_gcc_log.h"
#include <sstream>
#include <ace/ACE.h>

using namespace std;
GCC_TDEF(aes_gcc_variable);
/*===================================================================
                    ROUTINE: AES_GCC_VariableImplementation
=================================================================== */
AES_GCC_VariableImplementation::AES_GCC_VariableImplementation () :
    																						variable(""),aesDirPath("")
{
};

/*===================================================================
                    ROUTINE: AES_GCC_VariableImplementation
=================================================================== */
AES_GCC_VariableImplementation::AES_GCC_VariableImplementation (std::string variableName) :
    																						variable(variableName),aesDirPath("")
{
	GCC_TRACE((aes_gcc_variable,"In AES_GCC_VariableImplementation constructor, variableName : %s  ",variableName.c_str()));
};

/*===================================================================
                    ROUTINE: ~AES_GCC_VariableImplementation
=================================================================== */
AES_GCC_VariableImplementation::~AES_GCC_VariableImplementation ()
{
	GCC_TRACE((aes_gcc_variable,"%s","In AES_GCC_VariableImplementation destructor "));
}
/*===================================================================
                    ROUTINE: exists
=================================================================== */
bool AES_GCC_VariableImplementation::exists(void)
{
	GCC_TRACE((aes_gcc_variable,"%s","In AES_GCC_VariableImplementation::exists method "));
	bool aesDirPathExist = updateStrVar(variable, aesDirPath);
	GCC_TRACE((aes_gcc_variable,"exists status: %d",aesDirPathExist));

	return aesDirPathExist;
};
/*===================================================================
                    ROUTINE: getStr
=================================================================== */
string AES_GCC_VariableImplementation::getStr(void)
{

	updateStrVar(variable, aesDirPath);
	GCC_TRACE((aes_gcc_variable,"getStr, datapath obtained as : %s",aesDirPath.c_str()));
	return aesDirPath;
}
/*===================================================================
                    ROUTINE: updateStrVar
=================================================================== */
bool AES_GCC_VariableImplementation::updateStrVar(std::string inVar, std::string& outVar)
{
	ACS_APGCC_CommonLib myAPGCCCommonLib;
	ACE_TCHAR * szLogicName =  new char[10];

	ACS_APGCC_DNFPath_ReturnTypeT path;
	int dwlen = 50;
	ACE_TCHAR *szPath = NULL;
	bool exist = false;

	// Clear the array and set to NULL
	ACE_OS::memset((void *)szLogicName,0,10);

	// Check for the type of query
	if (inVar == aesDataDirectory)
	{
		ACE_OS::strcpy(szLogicName, "AES_DATA");
	}
	else if (inVar == aesLogsDirectory)
	{ 
		// else check for for Log directory
		ACE_OS::strcpy(szLogicName, "AES_LOGS");
	}

	// Try getting the datadisk path requested until we get the buffer size right

	szPath = new ACE_TCHAR[dwlen];
	if (szPath != NULL)
	{
		path = myAPGCCCommonLib.GetDataDiskPath(szLogicName, szPath, dwlen);
	}
	// Condition for other error codes from GetDataDiskPath. We can do much about it
	// so return false and raise an event

	if (path != ACS_APGCC_DNFPATH_SUCCESS)
	{
		ACE_TCHAR problemData[216] = { 0 };
		switch(path)
		{
		case  ACS_APGCC_DNFPATH_FAILURE :
			ACE_OS::sprintf(problemData,"Failed to fetch the dataDiskPath for %s",inVar.c_str());
			break;
		case ACS_APGCC_FAULT_LOGICAL_NAME :
			ACE_OS::sprintf(problemData,"Faulty logical name given for %s in fetching data disk path",inVar.c_str());
			break;
		case ACS_APGCC_STRING_BUFFER_SMALL :
			ACE_OS::sprintf(problemData,"Output buffer length is too small to hold the data disk path of %s",inVar.c_str());
			break;
		default:
			break;
		}

		Event::report(1003,
				"AES Internal Fault",
				problemData,
				" ");
		GCC_TRACE((aes_gcc_variable,"Error : %s",szPath));
	}
	else
	{
		outVar = string(szPath);
		exist = true;
	}

	delete[] szLogicName;
	szLogicName = 0;

	delete[] szPath;
	szPath = NULL;

	return exist;

}
