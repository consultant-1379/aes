//******************************************************************************
//
// NAME
//      blockpairmapper.cpp
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
// 	2002-05-21 by UAB/KB/AU  Hans-Erik Nilsson

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

#include <time.h>           // for time(..)
#include <string>
#include <vector>
#include <blockpairmapper.h>
#include <blockpairindex.h>
#include <event.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;

AES_CDH_TRACE_DEFINE(AES_CDH_BlockPairMapper);


// ***********************************************************************
// makeObjectName
// ***********************************************************************
bool BlockPairMapper::makeObjectName()
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	if (fileName_ == "")        // filename is missing!
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "fileName_ is empty, Leaving BlockPairMapper::makeObjectName");
		AES_CDH_TRACE_MESSAGE("fileName_ is empty, Leaving BlockPairMapper::makeObjectName");
		return false;
	}

	objName_.assign("AES_CDH_FILEMAPPING_");
	objName_.append(fileName_);


	string::size_type pos = string::npos;

	// The filename may be a path, containing ':\', replace ':\' with '__'
	if ((pos = objName_.find_first_of(":/")) != string::npos)
		objName_.replace(pos, 2, "__");


	// replaces all '\' with '_';
	pos = string::npos;
	while ((pos = objName_.find_first_of("/")) != string::npos)
		objName_.replace(pos, 1, "_");

	AES_CDH_TRACE_MESSAGE( "objName_ = %s", objName_.c_str());

	AES_CDH_TRACE_MESSAGE("Leaving ...");
	return true;
}


// ***********************************************************************
// makeFileName
// ***********************************************************************
void BlockPairMapper::makeFileName()
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	time_t ltime = time( &ltime );

	char buf[128] = { 0 };

	unsigned long tempTime = ltime;

	sprintf(buf, "%ld", tempTime);
	AES_CDH_TRACE_MESSAGE("Leaving ...");
}


// ***********************************************************************
// constructor
// ***********************************************************************
BlockPairMapper::BlockPairMapper(const std::string &destSetName)
    : hFile(ACE_INVALID_HANDLE),mapAddr_(MAP_FAILED), fileName_(""), destSetName_(destSetName), objName_(""), RPCName_(""),bIndexError_(false),needClose_(false), bpi_(NULL)
{						
	AES_CDH_TRACE_MESSAGE("Entering  for destset = %s", destSetName.c_str());

	pairId.applid_ = APPLID_MAX + 1;
	pairId.cdrbid_ = 0;
	pairId.nextApplid_ = APPLID_MAX + 1;
	pairId.committed_   = 0;

	pairId.RPCName[0] = '\0';       // ++

	AES_CDH_TRACE_MESSAGE("pairId.applid_ = %u", pairId.applid_);

	bpi_ = new(std::nothrow) BlockPairIndex(destSetName);
	if (bpi_ == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "%s", " BlockPairMapper::BlockPairMapper , FAILED to allocate BlockPairIndex");
		AES_CDH_TRACE_MESSAGE("BlockPairMapper::BlockPairMapper , FAILED to allocate BlockPairIndex");
		bIndexError_ = true;
	}
	AES_CDH_TRACE_MESSAGE("Leaving  for destset = %s", destSetName.c_str());
}


// ***********************************************************************
// destructor
// ***********************************************************************
BlockPairMapper::~BlockPairMapper()
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	// Do not close the file if it is not open
	if ( needClose_ )
		this->close();

	delete bpi_;
	bpi_ = NULL;
	AES_CDH_TRACE_MESSAGE("Leaving ...");
}


// ***********************************************************************
// open()
// ***********************************************************************
bool BlockPairMapper::open(const std::string &fName, const std::string &RPCName)
{

	AES_CDH_TRACE_MESSAGE("Entering ...");

	fileName_.assign(fName);

	AES_CDH_TRACE_MESSAGE("fileName_ = %s", fileName_.c_str());


	// Do not open the file if it is open
	if (needClose_ == true)
	{
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "needClose_ is true");
		AES_CDH_TRACE_MESSAGE("needClose_ is true");
		return false;
	}



	// Used to indicate whether the file (if created) should be deleted or
	// not if this method failed for some reason.
	bool deleteFile = false;


	// Used to indicate whether the file has been created in this call.
	// Used to determine if file should be initialized or not below.
	bool fileExisted = true;


	std::string fileName("");
	std::string fullFileName("");
	if (! bIndexError_)
	{
		bool b = false;
		if ((b = bpi_->open()) == true)
		{
			bool b1 = bpi_->findStreamId(destSetName_, fileName_, fileName, fullFileName);
			if (b1 == false)
			{
				bpi_->append(destSetName_, fileName_, fullFileName);
			}

			bpi_->close();
		}

		if (! b)
		{
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "b is false, returning BlockPairMapper::open");
			AES_CDH_TRACE_MESSAGE("b is false, returning BlockPairMapper::open");
			return b;
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("bIndexError_ is true, returning BlockPairMapper::open");
		return bIndexError_;
	}

	fileName_ = fullFileName;

	AES_CDH_TRACE_MESSAGE("fileName_ retrieved from streamId = %s", fileName_.c_str());
	// Check if the file does NOT exist
	if ( access(fileName_.c_str(), 0) == -1 )
	{
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "Unable to access file = %s, making fileExisted as false and deleteFile as true", fileName_.c_str());
		AES_CDH_TRACE_MESSAGE("Unable to access file = %s, making fileExisted as false and deleteFile as true", fileName_.c_str());
		fileExisted = false;
		deleteFile = true;
	}



	// open/create the file to be used. If the file exists,
	// it is opened, if it does not exist, it is created

	hFile = ACE_OS::open(fileName_.c_str(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG );

	// Unable to open/create file ??
	if ( hFile == ACE_INVALID_HANDLE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "open failed for file = %s", fileName_.c_str());
		AES_CDH_TRACE_MESSAGE("open failed for file = %s", fileName_.c_str());
		return false;
	}


	// construct object name for the file mapping
	if ( this->makeObjectName() != true )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "makeObjectName returned false, returning open");
		AES_CDH_TRACE_MESSAGE("makeObjectName returned false, returning open");
		ACE_OS::close(hFile);
		hFile=ACE_INVALID_HANDLE;
		if (deleteFile)
			::remove(fileName_.c_str());
		return false;
	}

	if( fileExisted == false )
	{
		int result = ACE_OS::lseek(hFile, 3000-1, SEEK_SET);
		if (result == -1)
		{
			ACE_OS::close(hFile);
			hFile=ACE_INVALID_HANDLE;
			if (deleteFile)
				::remove(fileName_.c_str());
			return false;
		}

		result = ACE_OS::write(hFile, "", 1);
		if (result != 1)
		{
			ACE_OS::close(hFile);
			hFile=ACE_INVALID_HANDLE;
			if (deleteFile)
				::remove(fileName_.c_str());
			return false;
		}
	}

	// create a named file-mapping object for the specified file

	mapAddr_ = ACE_OS::mmap( 0, 3000, PROT_READ| PROT_WRITE, MAP_SHARED, hFile, 0);
	int errNum = ACE_OS::last_error();

	// Unable to open/create of file-mapping object ??
	if( MAP_FAILED == mapAddr_)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "mmap for file %s is not success with errno value %d ", fileName_.c_str(), errNum);
		AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving ");
		ACE_OS::close(hFile);
		hFile=ACE_INVALID_HANDLE;
		if (deleteFile)
			::remove(fileName_.c_str());
		return false;
	}

	// The file is now open, set flag to indicate this.
	needClose_ = true;

	// We need to do a bit of initialization to be able to translate
	// CDRBid numbers to application block numbers
	pairId.applid_ = APPLID_MAX + 1;
	pairId.cdrbid_ = 0;
	pairId.nextApplid_ = APPLID_MAX + 1;
	pairId.committed_   = 0;
	// TR-HH72841
	tempFileName_.assign (fileName_);
	tempFileName_ = tempFileName_ + "_TEMP";
	// TR-HH72841


	// If the file was created we need to make sure that it does not
	// contain garbage by initializing it
	if (fileExisted == false)
	{
		AES_CDH_TRACE_MESSAGE("fileExisted is false");
		// obtain pointer to the first position in the file
		char *cp = (char *)mapAddr_;

		// copy the contents of pairId into the memory area pointed to by cp
		memcpy(cp, (void *)&pairId, sizeof(PairId));

		//start - TR HH72841
		ACE_HANDLE hFile1 = ACE_OS::open(tempFileName_.c_str(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG );

		if ( hFile1 != ACE_INVALID_HANDLE )
		{

			ACE_OS::write(hFile1, (void *)&pairId, sizeof(pairId));	//copy contents from tempfile
			ACE_OS::close(hFile1);
			hFile1=ACE_INVALID_HANDLE;
		}
		//End - TR HH72841
	}
	else
	{
		// Store RPCName before initializing the file
		(void)::strcpy(pairId.RPCName, RPCName.c_str());

		// obtain pointer to the first position in the file
		char *cp = (char *)mapAddr_;

		// copy the contents of the memory area pointed to by cp into pairId
		memcpy((void *)&pairId, cp, sizeof(PairId));

		//start - TR HH72841
		unsigned int committed = pairId.committed_ ;	//save previous committed state

		ACE_HANDLE hFile1 = ACE_OS::open(tempFileName_.c_str(), O_RDWR);


		if ( hFile1 != ACE_INVALID_HANDLE )
		{

			ACE_OS::read(hFile1, (void *)&pairId, sizeof(pairId));	//copy contents from tempfile
			ACE_OS::close(hFile1);
			hFile1=ACE_INVALID_HANDLE;
		}

		pairId.committed_ = committed;					//restore committed state
		memcpy(cp, (void *)&pairId, sizeof(PairId));	//copy contents to original file.
		AES_CDH_TRACE_MESSAGE("APPLID : %u, CDRBID : %u, COMMITTED : %u, RPCNAME : %s ",  pairId.applid_, pairId.cdrbid_, pairId.committed_, pairId.RPCName);
	}

	AES_CDH_TRACE_MESSAGE(" Leaving ...");
	return true;
}


// ***********************************************************************
// close()
// ***********************************************************************
//##ModelId=3DE49B4A0322
bool BlockPairMapper::close()
{
	// Close open file handles and unmap the view of the file

	if (needClose_)
	{
		AES_CDH_TRACE_MESSAGE("BlockPairMapper::close() ,needClose_ true");
		ACE_OS::close(hFile);
		hFile=ACE_INVALID_HANDLE;
		if( MAP_FAILED != mapAddr_)
		{
			int b = ACE_OS::munmap(mapAddr_, 3000);
			mapAddr_ = MAP_FAILED;

			if (b == -1)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "BlockPairMapper::close(), failed to unmap view of file");
				AES_CDH_TRACE_MESSAGE("BlockPairMapper::close(), failed to unmap view of file");
			}
		}

		// Set flag to indicate no access
		needClose_ = false;
	}
	AES_CDH_LOG(LOG_LEVEL_TRACE, "BlockPairMapper::close(), return value: true");
	return true;
}


// ***********************************************************************
// isOpen()
// ***********************************************************************
//##ModelId=3DE49B4A032C
bool BlockPairMapper::isOpen() const
{
	return needClose_;
}


// ***********************************************************************
// commit()
// ***********************************************************************
//##ModelId=3DE49B4A0336
void BlockPairMapper::commit(const PairId &id)
{
	AES_CDH_TRACE_MESSAGE(" Entering ...");
	AES_CDH_TRACE_MESSAGE("pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE("pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE("pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE("pairId.committed_=%u", pairId.committed_);

	// obtain pointer to the first position in the file
	char *cp = (char *)mapAddr_;

	// save the last number pair to be committed, but do not
	// overwrite the existing committed flag.
	unsigned int oldCommitted = pairId.committed_;

	// Do not overwrite the existing RPCName
	memcpy((void *)&pairId, (void *)&id, sizeof(id) - 40);

	pairId.committed_ = oldCommitted;

	// Indicate commit operation in the file

	if (pairId.committed_ != 2)
		pairId.committed_ = 1;

	// copy the contents of pairId into the memory area pointed to by cp
	memcpy(cp, (void *)&pairId, sizeof(PairId));

	//-------start of HH72841--------------
	(void)::strcpy(pairId.RPCName, RPCName_.c_str());

	ACE_HANDLE hFile = ACE_OS::open(tempFileName_.c_str(), O_RDWR);

	if ( hFile != ACE_INVALID_HANDLE )
	{
		ACE_OS::write(hFile, (void *)&pairId, sizeof(pairId));
		ACE_OS::close(hFile);
	}
	//------END of HH72841---------------

	AES_CDH_TRACE_MESSAGE("pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE("pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE("pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE("pairId.committed_=%u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE("pairId.RPCName = %s", pairId.RPCName);
}


// ***********************************************************************
// commitChange()
// ***********************************************************************
//##ModelId=3DE49B4A034A
void BlockPairMapper::commitChange(const PairId &id, const std::string &RPCName)
{
	// obtain pointer to the first position in the file
	char *cp = (char *)mapAddr_;

	// save the last number pair to be committed
	pairId = id;

	// Indicate commit operation in the file
	pairId.committed_ = 1;

	// Update the RPCName to indicate 'change' in the file
	(void)::strcpy(pairId.RPCName, RPCName.c_str());

	// copy the contents of pairId into the memory area pointed to by cp
	memcpy(cp, (void *)&pairId, sizeof(PairId));
}


// ***********************************************************************
// getLastCommitted()
// ***********************************************************************
//##ModelId=3DE49B4A035E
void BlockPairMapper::getLastCommitted(PairId &id)
{
	// obtain pointer to the first position in the file
	char *cp = (char *)mapAddr_;

	// copy the contents of cp into our id
	memcpy((void *)&id, cp, sizeof(PairId));

	AES_CDH_TRACE_MESSAGE("id.nextApplid_=%u", id.nextApplid_);
	AES_CDH_TRACE_MESSAGE("id.applid_=%u", id.applid_);
	AES_CDH_TRACE_MESSAGE("id.cdrbid_=%u", id.cdrbid_);
	AES_CDH_TRACE_MESSAGE("id.committed_=%u", id.committed_);
}


// ***********************************************************************
// getApplBlockId()
// ***********************************************************************
//##ModelId=3DE49B4A0372
unsigned int BlockPairMapper::getApplBlockId()
{
	AES_CDH_TRACE_MESSAGE("pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE("pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE("pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE("pairId.committed_=%u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE("pairId.RPCName=%s", pairId.RPCName);
	return pairId.applid_;
}


// ***********************************************************************
// getApplBlockId()
// ***********************************************************************
//##ModelId=3DE49B4A037C
unsigned int BlockPairMapper::getApplBlockId(const unsigned int CDRBid)
{
	AES_CDH_TRACE_MESSAGE("CDRBid=%u", CDRBid);
	AES_CDH_TRACE_MESSAGE("pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE("pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE("pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE("pairId.committed_=%u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE("pairId.RPCName=%s", pairId.RPCName);

	if (CDRBid == pairId.cdrbid_ )
	{
		AES_CDH_TRACE_MESSAGE("BlockPairMapper::getApplBlockId(..), Formula 1");
		return pairId.applid_;
	}
	else if (pairId.nextApplid_ > APPLID_MAX)
	{
		AES_CDH_TRACE_MESSAGE("BlockPairMapper::getApplBlockId(..), Formula 2");
		return pairId.nextApplid_;
	}
	else if (CDRBid > pairId.cdrbid_ )
	{
		if ( CDRBid - pairId.cdrbid_ - 1 > APPLID_MAX - pairId.nextApplid_ )
		{
			AES_CDH_TRACE_MESSAGE("BlockPairMapper::getApplBlockId(..), Formula 3");

			return APPLID_MAX + 1;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("BlockPairMapper::getApplBlockId(..), Formula 4");

			// applid + offset between new CDRBid and stored CDRBid
			return ( pairId.nextApplid_ + (CDRBid - pairId.cdrbid_) - 1 );
		}
	}
	else
	{   // current CDRBid has turned around
		if ( (4294967295 - pairId.cdrbid_) + CDRBid > APPLID_MAX - pairId.nextApplid_ )
		{
			AES_CDH_TRACE_MESSAGE("BlockPairMapper::getApplBlockId(..), Formula 5");
			return APPLID_MAX + 1;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("BlockPairMapper::getApplBlockId(..), Formula 6");
			return ( pairId.nextApplid_ + ((4294967295 - pairId.cdrbid_) + CDRBid));
		}
	}
}


// ***********************************************************************
// getCDRBId()
// ***********************************************************************
//##ModelId=3DE49B4A0390
void BlockPairMapper::setNextApplBlockId(unsigned int id, unsigned int prevCDRBid)
{
	// obtain pointer to the first position in the file
	char *cp = (char *)mapAddr_;


	// The application block id has been reset to some initial value,
	// indicate by setting it to an invalid id
	pairId.applid_ = APPLID_MAX + 1;


	pairId.cdrbid_ = prevCDRBid;


	// Because pairId.applid has been set to some initial value,
	// we need to update pairId.nextApplid_ as well.
	pairId.nextApplid_ = id;


	// Indicate in the file that there has not been a commit-operation

	pairId.committed_ = 1;

	// copy the contents of id into the memory area pointed to by cp
	memcpy(cp, (void *)&pairId, sizeof(PairId));

	AES_CDH_TRACE_MESSAGE( "pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE( "pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE( "pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE( "pairId.committed_=%u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE( "pairId.RPCName=%s", pairId.RPCName);
}


// ***********************************************************************
// getCDRBId()
// ***********************************************************************
//##ModelId=3DE49B4A03AE
unsigned int BlockPairMapper::getCDRBid()
{
	AES_CDH_TRACE_MESSAGE( "pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE( "pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE( "pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE( "pairId.committed_=%u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE( "pairId.RPCName=%s", pairId.RPCName);

	return pairId.cdrbid_;
}


// ***********************************************************************
// setCDRBId()
// ***********************************************************************
void BlockPairMapper::setCDRBid(unsigned int cdrbid, const std::string &RPCName)
{
	pairId.applid_ = APPLID_MAX + 1;
	pairId.cdrbid_ = cdrbid;

	pairId.committed_   = 1;
	(void)::strcpy(pairId.RPCName, RPCName.c_str());
	RPCName_ = RPCName;      // HH72841

	// obtain pointer to the first position in the file
	char *cp = (char *)mapAddr_;

	// copy the contents of pairId into the memory area pointed to by cp
	memcpy(cp, (void *)&pairId, sizeof(PairId));

	AES_CDH_TRACE_MESSAGE( "pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE( "pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE( "pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE( "pairId.committed_=%u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE( "pairId.RPCName=%s", pairId.RPCName);
}


// ***********************************************************************
// setCDRBidOnly()
// ***********************************************************************
void BlockPairMapper::setCDRBidOnly(unsigned int cdrbid, const std::string &RPCName)
{
	AES_CDH_TRACE_MESSAGE( "CDRBid=%u", cdrbid);

	pairId.cdrbid_ = cdrbid;

	pairId.committed_   = 1;
	(void)::strcpy(pairId.RPCName, RPCName.c_str());
	RPCName_ = RPCName;  // HH72841
	// obtain pointer to the first position in the file
	char *cp = (char *)mapAddr_;

	// copy the contents of pairId into the memory area pointed to by cp
	memcpy(cp, (void *)&pairId, sizeof(PairId));

	AES_CDH_TRACE_MESSAGE( "pairId.nextApplid_=%u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE( "pairId.applid_=%u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE( "pairId.cdrbid_=%u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE( "pairId.committed_=%u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE( "pairId.RPCName=%s", pairId.RPCName);
}


// ***********************************************************************
// validFile()
// ***********************************************************************
//##ModelId=3DE49B4A03E0
bool BlockPairMapper::validFile() const
{
	PairId id;

	id.applid_    = 0;
	id.cdrbid_    = 0;
	id.committed_ = 0;

	// obtain pointer to the first position in the file
	char *cp = (char *)mapAddr_;

	// copy the contents of cp into our id
	memcpy((void *)&id, cp, sizeof(PairId));

	if (id.committed_ == 1)
		return true;

	return false;
}


// ***********************************************************************
// static remove()
// ***********************************************************************
//##ModelId=3DE49B4B0002
bool BlockPairMapper::remove(const std::string &destination)
{
	return BlockPairIndex::remove(destination);
}


// ***********************************************************************
// static init()
// ***********************************************************************
//##ModelId=3DE49B4B0016
bool BlockPairMapper::init(const std::string &destination)
{
	return BlockPairIndex::init(destination);
}


// ***********************************************************************
// PRIVATE: setChanged()
// ***********************************************************************
//##ModelId=3DE49B4A028C
bool BlockPairMapper::setChanged(const std::string &fileName)
{
	(void)fileName;
	char *cp = (char *)mapAddr_;

	memcpy((void *)&pairId, cp, sizeof(PairId));


	pairId.committed_ = 2;


	memcpy((void *)cp, &pairId, sizeof(PairId));

	ACE_OS::msync(mapAddr_, 3000, MS_SYNC);

	AES_CDH_TRACE_MESSAGE( "pairId.cdrbid_ = %u", pairId.cdrbid_);
	AES_CDH_TRACE_MESSAGE( "pairId.applid_ = %u", pairId.applid_);
	AES_CDH_TRACE_MESSAGE( "pairId.nextApplid_ = %u", pairId.nextApplid_);
	AES_CDH_TRACE_MESSAGE( "pairId.committed_ = %u", pairId.committed_);
	AES_CDH_TRACE_MESSAGE( "pairId.RPCName = %s", pairId.RPCName);

	return true;
}


// ***********************************************************************
// setAllChanged()
// ***********************************************************************
//##ModelId=3DE49B4B002A
bool BlockPairMapper::setAllChanged()
{
	std::vector<string> fvec;

	bpi_->exportFileNames(fvec);

	std::vector<string>::iterator itr = fvec.begin();
	while ( itr != fvec.end() )
	{
		if ( (this->fileName_ == *itr) && this->isOpen() )
		{
			this->setChanged(*itr);
		}
		else
		{
			ACE_HANDLE hFile = ACE_OS::open((*itr).c_str(),O_CREAT |O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG );

			if ( hFile != ACE_INVALID_HANDLE )
			{
				PairId id;

				int b = 0;
				b = ACE_OS::read(hFile, (char *)&id, sizeof(id));

				id.committed_ = 2;      // 2 = CHANGED

				b = ACE_OS::write(hFile, (char *)&id, sizeof(id));

				ACE_OS::close(hFile);

				AES_CDH_TRACE_MESSAGE("File %s set to CHANGED", (*itr).c_str());
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "Error changing file %s to CHANGED", (*itr).c_str());
				AES_CDH_TRACE_MESSAGE("Error changing file %s to CHANGED", (*itr).c_str());
			}
		}

		itr++;
	}

	return true;
}


// ***********************************************************************
// makeRPCName()
// ***********************************************************************
//##ModelId=3E81B6C901D4
void BlockPairMapper::makeRPCName(std::string &RPCName)
{
	RPCName += "_";
}


// ***********************************************************************
// setCommitted()
// ***********************************************************************
//##ModelId=3E8A87EA01CB
void BlockPairMapper::setCommitted()
{
	AES_CDH_TRACE_MESSAGE("Entering..");

	char *cp = (char *)mapAddr_;

	memcpy((void *)&pairId, cp, sizeof(PairId));

	pairId.committed_ = 1;

	memcpy((void *)cp, &pairId, sizeof(PairId));
}

// ***********************************************************************
// getFileStatus()
// ***********************************************************************
//##ModelId=3DE49B4B0034
unsigned int BlockPairMapper::getFileStatus(const std::string &RPCName) 
{
	AES_CDH_TRACE_MESSAGE("RPCName = %s File status is: %u", RPCName.c_str(), pairId.committed_);

	if (pairId.committed_ == 0)
		return pairId.committed_;

	if (::strcmp(pairId.RPCName, RPCName.c_str()) == 0)
	{
		this->setCommitted();
		return 1;
		/** NOT REACHED **/
	}

	// RPCNames are not the same
	return 2;
}


   //*************************************************************************
// readFromTempFile() - Added for TR HH72841
//*************************************************************************
void BlockPairMapper::readFromTempFile (unsigned int &cdrbid, const std::string& RPCName)
{
	ACE_HANDLE hFile = ACE_OS::open( tempFileName_.c_str(), O_CREAT | O_RDWR, S_IRWXU | S_IRWXO | S_IRWXG );

	if ( hFile != ACE_INVALID_HANDLE )
	{

		ACE_OS::read(hFile, (void *)&pairId, sizeof(pairId));

		RPCName_ = RPCName ;
		if (pairId.RPCName != RPCName)
		{
			(void)::strcpy(pairId.RPCName, RPCName.c_str());
			pairId.cdrbid_ = cdrbid;
			ACE_OS::write(hFile, (void *)&pairId, sizeof(pairId));
			char *cp = (char *)mapAddr_;
			memcpy((void *)cp, &pairId, sizeof(PairId));
		}
		ACE_OS::close(hFile);

		cdrbid=pairId.cdrbid_ ;
		AES_CDH_TRACE_MESSAGE("APPLID : %u, CDRBID : %u, COMMITTED : %u, RPCNAME : %s ",  pairId.applid_, pairId.cdrbid_, pairId.committed_, pairId.RPCName);
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Opening Error tempfile %s ", tempFileName_.c_str());
		AES_CDH_TRACE_MESSAGE("Opening Error tempfile %s ", tempFileName_.c_str());
	}

}
