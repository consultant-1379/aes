/******************************************************************************/
/**
@file transferagent.cpp

Class method implementation for transferagent.h

DESCRIPTION
A common API to different transfer types.

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

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include <ctype.h>
#include <bgwrpc.h>
#include <transferagent.h>
#include <ftpv2agent.h>
#include <transdest.h>
#include <ace/ACE.h>
#include <ACS_CS_API.h>

/*===================================================================
   ROUTINE:create
=================================================================== */
AES_CDH_ResultCode TransferAgent::create(TransDest* destPtr,
                                         const string transferType, 
                                         ACE_INT32& argc,
                                         ACE_TCHAR* argv[],
                                         const bool define,
                                         const bool recovery,
                                         TransferAgent*& trans)
{
    string tempType = transferType;
    AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;
    string destName;
    
    trans = NULL;
    destName = destPtr->getDestinationName();



    if((destName.length() > 32) || (destName.length() == 0)) // 010827 qabhall
        return AES_CDH_RC_UNREAS;

    lowerToUpper(tempType);
    if (tempType == BGWRPCType)
    {
        TransferAgent* transtemp = new BGWRPC;
        transtemp->destinationName = destName;
        transtemp->destinationPtr = destPtr;
        returnCode = transtemp->define(argc, argv, define, recovery);
        if (returnCode != AES_CDH_RC_OK)
        {
            delete transtemp;
        }
        else
        {
            trans = transtemp;
        }
    }
    else if (tempType == FTPTypeV2 || tempType == SFTPTypeV2)
    {
        TransferAgent* transtemp;
#ifdef DEBUGTEST
	std::cout << "TransferAgent() try to call FTPV2Agent::create()\n"; 
#endif
        returnCode = FTPV2Agent::create(destName,
                                        tempType,
                                        argc,
                                        argv,
                                        transtemp);
#ifdef DEBUGTEST
	std::cout << "TransferAgent() FTPV2Agent::create() called\n"; 
#endif

        if (returnCode == AES_CDH_RC_OK)
        {
            transtemp->destinationName = destName;
            transtemp->destinationPtr = destPtr;

            returnCode = transtemp->define(argc, argv, define, recovery);

#ifdef DEBUGTEST
	    cout << "Return code from transtemp->define = " << returnCode << endl;
#endif

            if (returnCode != AES_CDH_RC_OK)
            {
                delete transtemp;
            }
            else
            {
                trans = transtemp;
            }
        }
    }
    else
    {
        returnCode = AES_CDH_RC_NOTRANS;
    }
    return returnCode;
}  


/*===================================================================
   ROUTINE:lowerToUpper
=================================================================== */
void TransferAgent::lowerToUpper(string& str)
{
    ACE_INT32 len = str.length();

    for (ACE_INT32 i = 0; i <= len-1; i++)
    {
		if ((str[i] >= 'a') && (str[i] <= 'z'))
        {
            str[i] = ::toupper(str[i]);
        }
    }
}


/*===================================================================
   ROUTINE:upperToLower
=================================================================== */
void TransferAgent::upperToLower(string& str)
{
	ACE_INT32 len = str.length();

	for (ACE_INT32 i = 0; i <= len-1; i++)
    {
		if ((str[i] >= 'A') && (str[i] <= 'Z'))
        {
            str[i] = ::tolower(str[i]);
        }
    }
}


/*===================================================================
   ROUTINE:cleanUpArgv
=================================================================== */
void TransferAgent::cleanUpArgv(ACE_INT32& argc, ACE_TCHAR* argv[], ACE_INT32 firstOper)
{
    ACE_INT32 i = 0;
    ACE_INT32 j = 0;
    
    if (argv[firstOper-1] == NULL && firstOper < argc)
    {
        argv[firstOper-1] = new ACE_TCHAR [3];
        (void) ACE_OS::strcpy(argv[firstOper-1], "--");
    }
    
    while (j < argc)
    {
        if (argv[j] != NULL)
        {
            argv[i] = argv[j];
            i++;
        }
        j++;
    }
    argc = i;
    argv[argc] = 0;
}


/*===================================================================
   ROUTINE:getClusterName
=================================================================== */
bool TransferAgent::getClusterName(string& name)
{
	bool bStatus=false;
	ACS_CS_API_Name neid;
        ACS_CS_API_NS::CS_API_Result result;
        result = ACS_CS_API_NetworkElement::getNEID(neid);
        if(result == ACS_CS_API_NS::Result_Success)
        {
                char neName[256];
                size_t size = sizeof(neName);
                neid.getName(neName, size);
		name=neName;
		bStatus=true;
        }
    return bStatus;
}

