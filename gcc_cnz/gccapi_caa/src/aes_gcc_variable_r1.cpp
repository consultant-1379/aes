/* =================================================================== */
	/**
	@file aes_gcc_variable_r1.cpp

	Class method implementation for aes_gcc_variable_r1.h

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

	N/A       21/07/2011     xbhadur       Initial Release
	*/
/* =================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "aes_gcc_variable_r1.h"
#include "aes_gcc_variableimplementation.h"
using namespace std;

/*===================================================================
				ROUTINE: DEFAULT CONSTRUCTOR
=================================================================== */
AES_GCC_Variable_R1::AES_GCC_Variable_R1 ()
{
    implementation = new AES_GCC_VariableImplementation;
};

/*===================================================================
                    ROUTINE: PARAMETRICAL CONSTRUCTOR
=================================================================== */
AES_GCC_Variable_R1::AES_GCC_Variable_R1 (const AES_GCC_Variable_R1& var)
{
    implementation = new AES_GCC_VariableImplementation(*(var.implementation));
};
/*===================================================================
                    ROUTINE: PARAMETRICAL CONSTRUCTOR
=================================================================== */
AES_GCC_Variable_R1::AES_GCC_Variable_R1 (std::string variableName)
{
    implementation = new AES_GCC_VariableImplementation(variableName);
};

/*===================================================================
                    ROUTINE: operator=
=================================================================== */
AES_GCC_Variable_R1& AES_GCC_Variable_R1::operator=(const AES_GCC_Variable_R1& var)
{
	if( implementation != 0 )
	{
		delete implementation;
		implementation = 0;
	}
    implementation = new AES_GCC_VariableImplementation(*(var.implementation));
    return *this;
}    
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_Variable_R1::~AES_GCC_Variable_R1 ()
{
	delete implementation;
}
/*===================================================================
                    ROUTINE: exists
=================================================================== */
bool AES_GCC_Variable_R1::exists(void)
{
	return implementation->exists();
};
/*===================================================================
                    ROUTINE: getStr
=================================================================== */
string AES_GCC_Variable_R1::getStr(void)
{
    return implementation->getStr();
}

