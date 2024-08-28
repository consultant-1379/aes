//******************************************************************************
//
// NAME
//      ParameterHandler_Imp.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//      <General description of the class>

// DOCUMENT NO
//      19089-CAA 109 0319

// AUTHOR 
//      1999-11-18 by UAB/I/LN  Urban Sderberg

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************
#include "ParameterHandler.h"

#include <aes_gcc_tracer.h>

#include "ACS_APGCC_Util.H"
#include "ACS_APGCC_CommonLib.h"
#include <acs_apgcc_omhandler.h>

#include <fstream>

namespace AES_DBO
{
	const std::string DELIMITER("/");
	const char STORAGE[] = "aes_dbo";
	const char AES_DATA[] = "AES_DATA";
	const char DBO_FOLDER[] = "dbo/";

	// PSA info
	const char PSACONFIGFILE[] = "/usr/share/pso/storage-paths/config";
	const char PSACONFIGPATH[] = "/storage/system/config/";
	const char FileMTQRootParameter[]= "mirroring";
	const char FileMTQRoot[] = "/data/opt/ap/internal_root/data_transfer/data_mirrored";
	const char dataDiskPath[] = "/data/aes/data";

	// IMM info
	const char BLOCKTRANSFER_ROOT_MOC[] = "blockTransferManagerId=1,AxeDataTransferdataTransferMId=1";
	const char RPC_STREAM_SIZE_ATTRIBUTE[] = "rpcStreamSize";
	const int RPCSTREAMSIZE = 4194304U;
}

// ACS Trace definition
AES_GCC_TRACE_DEFINE(AES_DBO_ParameterHandler);

//******************************************************************************
//
AES_DBO_ParameterHandler::AES_DBO_ParameterHandler()
:rootDir_(),
cfgRootDir_(),
streamSize_(AES_DBO::RPCSTREAMSIZE)
{
    // Do the fetch
    fetch();
}

AES_DBO_ParameterHandler::~AES_DBO_ParameterHandler()
{

}

//******************************************************************************
//
void AES_DBO_ParameterHandler::fetch()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

    // Read parameter for mirroring path under fileM
    setFileMPath();

    AES_GCC_TRACE_MESSAGE("DBO FileM path:<%s>", rootDir_.c_str() );

    // Value for AES_DBO_Directory is set
    // Check if the directory exists
    ACE_stat folderInfo;
    // check if already exist
	if( ( ACE_OS::stat(rootDir_.c_str(), &folderInfo) != 0) && (errno == ENOENT) )
	{
		ACE_OS::umask(0);

		//create TQ folder
		if(ACS_APGCC::create_directories(rootDir_.c_str()) !=  1 )
		{
			// log error
			AES_GCC_TRACE_MESSAGE("error:<%d> on DBO FileM folder creation", errno);
		}
	}

	 // Read parameter for aes data path under IMM
	 setDataPath();

	// check if already exist
	if( ( ACE_OS::stat(rootDir_.c_str(), &folderInfo) != 0) && (errno == ENOENT) )
	{
		ACE_OS::umask(0);

		//create TQ folder
		if(ACS_APGCC::create_directories(rootDir_.c_str()) !=  1 )
		{
			// log error
			AES_GCC_TRACE_MESSAGE("error:<%d> on DBO data folder creation", errno);
		}
	}

	// get rpc stream size
	setRPCStreamSize();

	AES_GCC_TRACE_MESSAGE("...Leaving");
}

//******************************************************************************
//
const std::string &AES_DBO_ParameterHandler::getRootDir() const
{
    AES_GCC_TRACE_MESSAGE("DBO root folder:<%s>", rootDir_.c_str());
    return rootDir_;
}

//******************************************************************************
//
unsigned int AES_DBO_ParameterHandler::getStreamSize() const
{
    AES_GCC_TRACE_MESSAGE("rpcStreamSize:<%d>", streamSize_);
    return streamSize_;
}

//******************************************************************************
//
const std::string &AES_DBO_ParameterHandler::getConfigRootDir() const
{
    AES_GCC_TRACE_MESSAGE("DBO PSA config folder:<%s>", cfgRootDir_.c_str());
    return cfgRootDir_;
}

const std::string& AES_DBO_ParameterHandler::getDBODataPath() const
{
	 AES_GCC_TRACE_MESSAGE("DBO data folder:<%s>", dboDataRoot_.c_str());
	 return dboDataRoot_;
}
//      ----------------------------------------
//                delimiter
//      ----------------------------------------
std::string AES_DBO_ParameterHandler::delimiter()
{
	return AES_DBO::DELIMITER;
}

void AES_DBO_ParameterHandler::setPSAPath()
{
	AES_GCC_TRACE_MESSAGE("...Entering");

	ifstream configFileStream;
	// open the clear file
	configFileStream.open(AES_DBO::PSACONFIGFILE, ios::binary );

	// check for open error
	if(configFileStream.good())
	{
		int bufferLength;
		// get length of stored path:
		configFileStream.seekg(0, ios::end);
		bufferLength = configFileStream.tellg();
		configFileStream.seekg(0, ios::beg);

		// allocate the buffer
		char buffer[bufferLength+1];
		ACE_OS::memset(buffer, 0, bufferLength+1);

		// read data
		configFileStream.read(buffer, bufferLength);

		if(buffer[bufferLength-1] == '\n') buffer[bufferLength-1] = 0;

		cfgRootDir_ = buffer;
		cfgRootDir_.append(AES_DBO::DELIMITER);
	}
	else
	{
		AES_GCC_TRACE_MESSAGE("open of PSA config file:<%s> failed error:<%d>", AES_DBO::PSACONFIGFILE, errno );
		cfgRootDir_ = AES_DBO::PSACONFIGPATH;
	}

	configFileStream.close();
	AES_GCC_TRACE_MESSAGE("...Leaving path:<%s>", cfgRootDir_.c_str() );
}

void AES_DBO_ParameterHandler::setFileMPath()
{
	AES_GCC_TRACE_MESSAGE("...Entering");

	bool result = false;
	ACS_APGCC_DNFPath_ReturnTypeT getResult;
	ACS_APGCC_CommonLib fileMPathHandler;

	int bufferLength = 512;
	char buffer[bufferLength];

	ACE_OS::memset(buffer, 0, bufferLength);
	// get the physical path
	getResult = fileMPathHandler.GetFileMPath(AES_DBO::FileMTQRootParameter, buffer, bufferLength);

	if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
	{
		// path get successful
		rootDir_ = buffer;
		result = true;
	}
	else if(ACS_APGCC_STRING_BUFFER_SMALL == getResult)
	{
		// Buffer too small, but now we have the right size
		char buffer2[bufferLength+1];
		ACE_OS::memset(buffer2, 0, bufferLength+1);
		// try again to get
		getResult = fileMPathHandler.GetFileMPath(AES_DBO::FileMTQRootParameter, buffer2, bufferLength);

		// Check if it now is ok
		if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
		{
			// path get successful now
			rootDir_ = buffer2;
			result = true;
		}
	}

	if(!result)
	{
		AES_GCC_TRACE_MESSAGE(" set DBO FileM path to:<%s>", AES_DBO::FileMTQRoot );
		rootDir_ = AES_DBO::FileMTQRoot;
	}

	rootDir_.append(AES_DBO::DELIMITER);

	AES_GCC_TRACE_MESSAGE("...Leaving path:<%s>", rootDir_.c_str() );
}

void AES_DBO_ParameterHandler::setDataPath()
{
	AES_GCC_TRACE_MESSAGE("...Entering");
	bool result = false;
	ACS_APGCC_DNFPath_ReturnTypeT getResult;
	ACS_APGCC_CommonLib dataDiskHandler;

	int bufferLength = 512;
	char buffer[bufferLength];

	ACE_OS::memset(buffer, 0, bufferLength);
	// get the physical path
	getResult = dataDiskHandler.GetDataDiskPath(AES_DBO::AES_DATA, buffer, bufferLength);

	if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
	{
		// path get successful
		dboDataRoot_ = buffer;
		result = true;
	}
	else if(ACS_APGCC_STRING_BUFFER_SMALL == getResult)
	{
		// Buffer too small, but now we have the right size
		char buffer2[bufferLength+1];
		ACE_OS::memset(buffer2, 0, bufferLength+1);
		// try again to get
		getResult = dataDiskHandler.GetDataDiskPath(AES_DBO::AES_DATA, buffer2, bufferLength);

		// Check if it now is ok
		if(ACS_APGCC_DNFPATH_SUCCESS == getResult)
		{
			// path get successful now
			dboDataRoot_ = buffer2;
			result = true;
		}
	}

	if(!result)
	{
		AES_GCC_TRACE_MESSAGE(" set DBO data path to:<%s>", AES_DBO::dataDiskPath );
		dboDataRoot_ = AES_DBO::dataDiskPath;
	}

	dboDataRoot_.append(AES_DBO::DELIMITER);
	dboDataRoot_.append(AES_DBO::DBO_FOLDER);

	AES_GCC_TRACE_MESSAGE("...Leaving path:<%s>", dboDataRoot_.c_str() );
}

void AES_DBO_ParameterHandler::setRPCStreamSize()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

	int rpcStreamSize = AES_DBO::RPCSTREAMSIZE;

	// try to get the value into IMM
	OmHandler myOmHandler;
    if( myOmHandler.Init() != ACS_CC_FAILURE )
    {
    	ACS_APGCC_ImmObject blockTransferRootMoC;
    	blockTransferRootMoC.objName = AES_DBO::BLOCKTRANSFER_ROOT_MOC;

        if( myOmHandler.getObject(&blockTransferRootMoC) != ACS_CC_FAILURE )
        {
        	AES_GCC_TRACE_MESSAGE(" get IMM object:<%s>", AES_DBO::BLOCKTRANSFER_ROOT_MOC);

        	std::vector<ACS_APGCC_ImmAttribute>::const_iterator attribute;
        	attribute = blockTransferRootMoC.attributes.begin();

        	// Search the rpcStreamSize attribute
        	for( ; attribute != blockTransferRootMoC.attributes.end(); ++attribute)
            {
        		// compare the current attribute
                if((*attribute).attrName.compare(AES_DBO::RPC_STREAM_SIZE_ATTRIBUTE) == 0 )
                {
                	// attribute found
                	if((*attribute).attrValuesNum != 0)
                    {
                		// get its value
                		rpcStreamSize = *(reinterpret_cast<int *>((*attribute).attrValues[0]));

                		AES_GCC_TRACE_MESSAGE(" get rpc stream size from IMM, <%s=%d>", AES_DBO::RPC_STREAM_SIZE_ATTRIBUTE, rpcStreamSize );
                        break;
                    }
                }
		    }
        }
        else
        {
        	AES_GCC_TRACE_MESSAGE("Unable to retrieve attributes of MO:<%s>, error:<%d>", AES_DBO::BLOCKTRANSFER_ROOT_MOC,
        																	myOmHandler.getInternalLastError() );
        }
        myOmHandler.Finalize();
    }
    else
    {
    	AES_GCC_TRACE_MESSAGE("OmHandler initialize failed, error<%d>", myOmHandler.getInternalLastError() );
    }

	 //Copy the stream size.
	 streamSize_ = rpcStreamSize;

	 if (streamSize_ == 0)//HR44097
		 streamSize_ = AES_DBO::RPCSTREAMSIZE;

	 AES_GCC_TRACE_MESSAGE("...Leaving, rpcStreamSize:<%d>", streamSize_ );
}
