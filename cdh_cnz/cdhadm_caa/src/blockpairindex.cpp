//******************************************************************************
//
// NAME
//      blockpairindex.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
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
// 	<General description of the class>

// DOCUMENT NO
//	19089-CAA 109 0419

// AUTHOR 
// 	2002-06-18 by UAB/KB/AU  Hans-Erik Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//  

// SEE ALSO 
// 	<Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include <servr.h>
#include <stdio.h>
#include <time.h>
#include <blockpairindex.h>
#include <aes_gcc_variable.h>
#include <parameter.h>
#include <event.h>
#include <aes_gcc_util.h>
#include <aes_cdh_common.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_BlockPairIndex);
// Initialize static variable. This is the name of the index-file
// that contains the filenames and the streamIds
const std::string BlockPairIndex::indexFileName_ = "indexFile";


//******************************************************************************
//  constructor
//******************************************************************************
BlockPairIndex::BlockPairIndex(const std::string &destName)
    : hFile(ACE_INVALID_HANDLE), destName_(destName), filePath_("")
{}


//******************************************************************************
//  destructor
//******************************************************************************
BlockPairIndex::~BlockPairIndex()
{}


//******************************************************************************
//  open()
//******************************************************************************
bool BlockPairIndex::open()
{
	AES_CDH_TRACE_MESSAGE("Entering ...");

	std::string path("");
	if ( AES_CDH_Paths::instance()->getCDHDataPath( path )  )
	{
		path.append("/");
		path.append(AES_CDH_RootDirectory);
		path.append("/bif/");
		path.append(destName_);
	}
	else
	{
		Event::report(AES_CDH_variableFault, "VARIABLE PROBLEM",
				"Retrieval of variable (" + string(AES_CDH_AesDataDirectory) +
				") from GCC failed.", "-");
		return false;
	}

	AES_CDH_TRACE_MESSAGE("path = %s , destName_ = %s ", path.c_str(), destName_.c_str());

	// create the directory if it does not exist
	if (ACE_OS::mkdir(path.c_str()) != 0 &&
			ACE_OS::last_error() != EEXIST)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "mkdir failed for path = %s ", path.c_str());
		string newPath("");
		AES_GCC_Util::datapath_trn(path, AES_DATA_PATH, newPath);
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				"Error creating directory " + newPath, "-");

		return false;
	}

	path.append("/");

	// Save path for later use
	filePath_.assign(path);

	path.append(indexFileName_);

	AES_CDH_TRACE_MESSAGE("path after append  = %s, filePath_  = %s", path.c_str(), filePath_.c_str());

	// open/create the file to be used. If the file exists,
	// it is opened, if it does not exist, it is created
	hFile = ACE_OS::open(path.c_str(),O_CREAT| O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG );                        // file

	if ( hFile == ACE_INVALID_HANDLE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "failed to open path = %s", path.c_str());
		AES_CDH_TRACE_MESSAGE("failed to open path = %s", path.c_str());
		return false;
	}
	AES_CDH_TRACE_MESSAGE("Leaving ...");
	return true;
}


//******************************************************************************
//  close()
//******************************************************************************
bool BlockPairIndex::close()
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	ACE_OS::close(hFile);
	AES_CDH_TRACE_MESSAGE("Leaving ...");
	return true;
}


//******************************************************************************
//  findStreamId()
//******************************************************************************
bool BlockPairIndex::findStreamId(const std::string &destination,
                                  const std::string &streamId,
                                  std::string &fileName,
                                  std::string &fullFileName)
{
	AES_CDH_TRACE_MESSAGE("Entering...");
	bool rc = false;
	indexData_ index;

	int b = 0;
	while ((b = (ACE_OS::read(hFile, (char *)&index, sizeof(index)))))
	{
		if (b == 0)
		{
			// We have reached end-of-file
			break;
		}


		if ( (strcmp(index.streamId_, streamId.c_str()) == 0) && (strcmp(index.destination_, destination.c_str()) == 0) )
		{
			fileName.assign(index.fileName_);
			fullFileName.assign(filePath_);
			fullFileName.append(fileName);
			rc = true;
			break;
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving  filName = %s, fullFileName = %s", fileName.c_str(), fullFileName.c_str());

	return rc;
}


//******************************************************************************
// append()
//******************************************************************************
bool BlockPairIndex::append(const std::string &destination,
                            const std::string &streamId,
                            std::string &fullFileName)
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	indexData_ index;

	// a unique filename must be created for this streamid,
	time_t t;
	::time(&t);

	char fileName[64];
	(void)::sprintf(fileName, "%ld", t);

	(void)::strcpy(index.destination_, destination.c_str());
	(void)::strcpy(index.fileName_, fileName);
	(void)::strcpy(index.streamId_, streamId.c_str());

	int b = ACE_OS::write(hFile, (char *)&index, sizeof(index));

	fullFileName.assign(filePath_);
	fullFileName.append(fileName);
	AES_CDH_LOG(LOG_LEVEL_INFO,"unique filename : %s created for streamid : %s, destination : %s",fullFileName.c_str(),streamId.c_str(),destination.c_str());
	AES_CDH_TRACE_MESSAGE(  "Leaving   fullFileName = %s, destination =%s, streamId = %s", fullFileName.c_str(), destination.c_str(), streamId.c_str());

	return b > 0 ? true : false;
}


//******************************************************************************
// static remove()
//******************************************************************************
bool BlockPairIndex::remove(const std::string &destination)
{
	AES_CDH_TRACE_MESSAGE("Entering , destination = %s", destination.c_str());

	std::string path("");
	//To check later in rel2
	if (AES_CDH_Paths::instance()->getCDHDataPath( path ) )
	{
		path.append("/");
		path.append(AES_CDH_RootDirectory);

		path.append("/bif/");
		path.append(destination);
	}
	AES_CDH_LOG(LOG_LEVEL_INFO,"Removing directory path %s for destination %s",path.c_str(),destination.c_str());
	AES_CDH_TRACE_MESSAGE( "  path = %s", path.c_str());

	DIR *dir = 0;
	struct dirent * finddata = 0;
	std::string searchPattern = path;
	std::string fileN("");
	dir = opendir(searchPattern.c_str());
	if (dir == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "open dir failed for path = %s", path.c_str());
		AES_CDH_TRACE_MESSAGE("open dir failed for path = %s", path.c_str());
		string newdr("");
		if (AES_GCC_Util::datapath_trn(searchPattern,AES_DATA_PATH,newdr) == false)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO,  "Directory name sent for event %s",newdr.c_str());
		}
		AES_CDH_LOG(LOG_LEVEL_INFO,  "Directory name %s",newdr.c_str());

		Event::report(AES_CDH_fileRemoveFault, "FILE REMOVE ERROR", newdr.c_str(), "Not able to open file or directory: " + newdr);
	}
	else
	{
		while ((finddata = readdir(dir)) != NULL)
		{
			fileN = searchPattern + "/" + finddata->d_name;
			struct stat stat_buff;
			stat(fileN.c_str(),&stat_buff);
			if(!(S_ISDIR(stat_buff.st_mode)))
			{
				int status  = ::remove(fileN.c_str());
				if(status != 0)
				{
					string newdr("");
					if (AES_GCC_Util::datapath_trn(fileN,AES_DATA_PATH,newdr)==false)
					{
						AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
					}

					Event::report(AES_CDH_fileRemoveFault, "FILE REMOVE ERROR", newdr, "Not able to delete file or directory: " + newdr);
					if(dir != NULL)
						closedir(dir);

					return false;
				}
			}
			else
			{
				if (ACE_OS::strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, ".."))
				{
					fileN = path + "/" + finddata->d_name;
					remove(fileN);
					int resultRemoveDir = ACE_OS::rmdir(fileN.c_str());
					if (resultRemoveDir != 0)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "remove - Unable to delete directory %s", fileN.c_str());
						Event::report(AES_CDH_fileRemoveFault, "FILE REMOVE ERROR", fileN, "Failed to remove directory: " + fileN);
					}
					else
						AES_CDH_TRACE_MESSAGE( "Directory deleted , dir path : %s",fileN.c_str());

				}
			}

		}
		closedir(dir);
		int resultRemoveDir = ACE_OS::rmdir(path.c_str());
		if (resultRemoveDir != 0)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "remove - Unable to delete directory %s", path.c_str());
			AES_CDH_TRACE_MESSAGE("remove - Unable to delete directory %s", path.c_str());
			Event::report(AES_CDH_fileRemoveFault, "FILE REMOVE ERROR", path, "Failed to remove directory: " + path);
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Directory deleted , dir path : %s",path.c_str());
		}			
	}	

	AES_CDH_TRACE_MESSAGE("Leaving , destination = %s", destination.c_str());
	return true;
}


//******************************************************************************
// static init()
//******************************************************************************
bool BlockPairIndex::init(const std::string &destination)
{
	AES_CDH_TRACE_MESSAGE("Entering , destination = %s", destination.c_str());

	std::string path("");
	if (AES_CDH_Paths::instance()->getCDHDataPath( path ) )
	{
		path.append("/");
		path.append(AES_CDH_RootDirectory);

		path.append("/bif/");
		path.append(destination);
	}
	AES_CDH_TRACE_MESSAGE("path = %s", path.c_str());

	if ((mkdir(path.c_str(), S_IRWXU | S_IRWXO | S_IRWXG) != 0) && (ACE_OS::last_error() != EEXIST))
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "mkdir failed for, path = %s", path.c_str());
		AES_CDH_TRACE_MESSAGE("mkdir failed for, path = %s", path.c_str());
		string newPath("");
		AES_GCC_Util::datapath_trn(path, AES_DATA_PATH, newPath);
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM", ServR::NTErrorText(), "CDH block-id directory could not be created: " + newPath);
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving , destination = %s", destination.c_str());
		return false;
	}

	const std::string fileName("/indexFile");
	path.append(fileName);

	AES_CDH_TRACE_MESSAGE("after path = %s", path.c_str());

	if (access(path.c_str(), 0) == -1)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO,  "this path = %s does not exist", path.c_str());
		AES_CDH_TRACE_MESSAGE("this path = %s does not exist", path.c_str());
		ACE_HANDLE hFile = ACE_INVALID_HANDLE;
		hFile = ACE_OS::open(path.c_str(), O_CREAT |O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG );

		if (hFile == ACE_INVALID_HANDLE)
		{
			string newPath("");
			AES_GCC_Util::datapath_trn(path, AES_DATA_PATH, newPath);
			Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
					ServR::NTErrorText(), "CDH block-id index file could not be created: "
					+ newPath);

			return false;
		}

		ACE_OS::close(hFile);
	}
	AES_CDH_LOG(LOG_LEVEL_INFO, "Index file created for destination %s is : %s",destination.c_str(),path.c_str());
	AES_CDH_TRACE_MESSAGE( "Leaving , destination = %s", destination.c_str());
	return true;
}


//******************************************************************************
// exportFileNames()
//******************************************************************************
bool BlockPairIndex::exportFileNames(std::vector<std::string> &vec)
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	bool rc = false;
	indexData_ index;

	if ( this->open() )
	{

		int b = 0;
		while ((b = (ACE_OS::read(hFile, (char *)&index, sizeof(index)))))
		{
			if (b == 0)
			{
				// We have reached end-of-file
				break;
			}

			vec.push_back(filePath_ + index.fileName_);
		}

		rc = true;

		this->close();
	}

	AES_CDH_TRACE_MESSAGE("Leaving ...");

	return rc;
}
