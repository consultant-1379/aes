
//******************************************************************************
// 
// .NAME
//      aes_dbo_mirrormaintainer - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME aes_dbo_mirrormaintainer
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE aes_dbo_mirrormaintainer.h

// .COPYRIGHT
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

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      19089-CAA 109 0319

// AUTHOR 
//      2002-06-26 by UAB/UKB/AU  Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          020626  qabmnnn First draft

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************


#include "aes_dbo_mirrormaintainer.h"
#include "aes_dbo_macros.h"
#include "aes_dbo_server.h"

#include "EventCode.h"
#include "ParameterHandler.h"

#include <aes_gcc_tracer.h>
#include <aes_gcc_errorcodes.h>

#include <ACS_CS_API.h>

#include <ace/Process_Mutex.h>

#define BOOST_FILESYSTEM_VERSION 3

#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include <time.h>

GCC_TDEF(AES_DBO_Mirrormaintainer);
AES_GCC_TRACE_DEFINE(AES_DBO_Mirrormaintainer);

namespace DataMirroring
{
	const char MIRROR_PROCESS_MUTEX_NAME[] = "AES_DBO_MUTEX_STATE_";

	const char MIRROR_QUEUE_FILE[] = "/METADATA/queue";
	const char MIRROR_METADATA[] = "/METADATA/state";
	const char MIRROR_DATA_FOLDER[] = "/DATA/";
	const char MIRROR_FOLDER[] = "/blockmirror/";
	const char MIRROR_METADATA_FOLDER[]=  "/METADATA";

	const char MIRROR_FILE_SEP = '+';
	const uint32_t MAX_MIRROR_DATAFILE_INDEX = 4000000000U;
	const uint32_t DELETE_CHECK_INTERVAL_IN_SEC = 2U;
	const uint32_t DEFAULT_REMOVE_TIME_IN_MIN = 1440U;

	const off_t MIRROR_FILESTATE_SIZE = (4*sizeof(uint32_t));
	const uint32_t CURRENT_FILE_POS  = 0U;
	const uint32_t CURRENT_BLOCK_POS = 4U;
	const uint32_t DATAWRITTEN_POS = 8U;
	const uint32_t COMMITDONE_POS = 12U;

}

namespace
{
	const char MSL_NULL[]= "-EMPTY-";
	const uint32_t INDEX_ERROR = 99999U;
}

//******************************************************************************
//
void aes_dbo_mirrormaintainer::checkDirs()
{
	AES_GCC_TRACE_MESSAGE("Verifying internal directory");

    std::string tqMirrorFolder(cfgTqPath_);
    tqMirrorFolder.append(DataMirroring::MIRROR_METADATA_FOLDER);

    AES_GCC_TRACE_MESSAGE("check path:<%s>", tqMirrorFolder.c_str());

    try
	{
		boost::filesystem::create_directories(tqMirrorFolder);
	}
	catch(boost::filesystem::filesystem_error &ex)
	{
		GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::checkDirs(), failed to create folders:<%s>, error:<%s>", tqMirrorFolder.c_str(), ex.what()));
	}

    std::string tqDataFolder(tqPath_);
    tqDataFolder.append(DataMirroring::MIRROR_DATA_FOLDER);
    AES_GCC_TRACE_MESSAGE("check path:<%s>", tqDataFolder.c_str());

    try
	{
		boost::filesystem::create_directories(tqDataFolder);
	}
	catch(boost::filesystem::filesystem_error &ex)
	{
		GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::checkDirs(), failed to create folders:<%s>, error:<%s>", tqDataFolder.c_str(), ex.what()));
	}

}

//******************************************************************************
//
aes_dbo_mirrormaintainer::aes_dbo_mirrormaintainer(const std::string &tq):
running_(false),
tq_(tq),
tqPath_(ParameterHandler::instance()->getRootDir()),
cfgTqPath_(ParameterHandler::instance()->getDBODataPath()),
cfgTqFolder_(cfgTqPath_.append(tq_)),
m_ProcessMutexName(DataMirroring::MIRROR_PROCESS_MUTEX_NAME),
delay_(DataMirroring::DEFAULT_REMOVE_TIME_IN_MIN),
m_stateP_(MAP_FAILED),
m_curFile_(NULL),
m_curBlock_(NULL),
m_multiCPsystem(false),
exit_(false),
exitForced_(false)
{
	m_ProcessMutexName.append(tq_);
	stateMutex_ = new (std::nothrow) ACE_Process_Mutex(m_ProcessMutexName.c_str());

	AES_GCC_TRACE_MESSAGE("tq_:<%s>, process mutex name:<%s>", tq_.c_str(), m_ProcessMutexName.c_str());

	// Check if Blade Cluster
	bool isBladeCluster = false;
	ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::isMultipleCPSystem(isBladeCluster);

	if (result == ACS_CS_API_NS::Result_Success)
	{
		if (isBladeCluster)
		{
			m_multiCPsystem = true;
			AES_GCC_TRACE_MESSAGE("CS-result = %d MULTICP",result);
		}
	}
	else
	{
		AES_GCC_TRACE_MESSAGE("CS-result = %d",result);
	}

	if (m_multiCPsystem)
	{
		// init block mirror structrure for the max blade number
		for (uint32_t w = 0; w < MAX_BLADE_NUMBER; ++w)
		{
			msList[w].streamId_ = MSL_NULL;        // For example, "CP1OLLE"
			msList[w].stateP_ = MAP_FAILED;
			msList[w].curFile_ = NULL;
			msList[w].curBlock_ = NULL;
		}
	}

	// Get the path for this tq
	tqPath_.append(tq_);

	// Set the Configuration path for this tq
	cfgTqPath_.append(DataMirroring::MIRROR_FOLDER);

	AES_GCC_TRACE_MESSAGE(" tqPath_:<%s>,\n tq_:<%s>,\n cfgTqFolder_:<%s>,\n cfgTqPath_:<%s>", tqPath_.c_str(), tq_.c_str(), cfgTqFolder_.c_str(), cfgTqPath_.c_str());

	// Check for directory existence
	checkDirs();
}


//******************************************************************************
//
aes_dbo_mirrormaintainer::~aes_dbo_mirrormaintainer()
{
	AES_GCC_TRACE_MESSAGE("Destructor tq_:<%s>", tq_.c_str());

	if(!aes_dbo_server::isStopEventSignalled)
	{
		// Remove directory structures unless during shutdown scenarios.
		removeDir(tqPath_);
		removeDir(cfgTqFolder_);
	}

	if(NULL != stateMutex_)
	{
		delete stateMutex_;
	}
}


//******************************************************************************
//
void aes_dbo_mirrormaintainer::create()
{
	AES_GCC_TRACE_MESSAGE("Creating mirror maintainer without delay tq_:<%s>", tq_.c_str());
    // Call the create below with no change of delay
    create(delay_);
}


//******************************************************************************
//
void aes_dbo_mirrormaintainer::create(unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("Entering..., tq_:<%s> thread running:<%s>",tq_.c_str(),(running_ ? "yes" : "no"));

	if(running_)
    {
    	AES_GCC_TRACE_MESSAGE("Thread already running");
        // Accept the new order
        exit_ = false;
        delay_ = delay;
	}
    else
    {

		// Check directories and open the disc queue
		checkDirs();

		std::string mirrorQueueDataFile(cfgTqPath_);
		mirrorQueueDataFile.append(DataMirroring::MIRROR_QUEUE_FILE);
		AES_GCC_TRACE_MESSAGE("Open mirror queue data:<%s>", mirrorQueueDataFile.c_str());

		queue_.open(mirrorQueueDataFile.c_str());

		if (!m_multiCPsystem)
		{
			// Open state file
			bool existing = false;
			std::string stateName(cfgTqPath_);
			stateName.append(DataMirroring::MIRROR_METADATA);

			ACE_HANDLE fileStateDescriptor = openMirrorStateFile(stateName, existing);

			// Check if state file has been opened
			if( ACE_INVALID_HANDLE != fileStateDescriptor )
			{
				AES_GCC_TRACE_MESSAGE("Create a memory mapped of the state file");
				// Create the memory mapping of the file, it will be shared with DBO client
				m_stateP_ = ACE_OS::mmap(NULL, DataMirroring::MIRROR_FILESTATE_SIZE, PROT_RDWR, MAP_SHARED, fileStateDescriptor, 0);

				// Close the file handle since it is not more needed
				ACE_OS::close(fileStateDescriptor);

				// check if the mapping failed
				if(MAP_FAILED == m_stateP_)
				{
					int error = ACE_OS::last_error();
					GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::create() couldn't open file mapping of file %s, error: %u\n", stateName.c_str(), error));
				}
				else
				{
					// Initialize state variables
					m_curFile_ = static_cast<uint32_t*>(m_stateP_);
					m_curBlock_ = static_cast<uint32_t*>(m_stateP_) + DataMirroring::CURRENT_BLOCK_POS;

					if(existing)
					{
						AES_GCC_TRACE_MESSAGE("File has been created now, set initial values");
						// Set initial values
						stateMutex_->acquire();
						*m_curFile_ = 1U;
						*m_curBlock_ = 0U;

						uint32_t* dataWritten_ = static_cast<uint32_t*>(m_stateP_) + DataMirroring::DATAWRITTEN_POS;
						*dataWritten_ = 0U;
						uint32_t* commitDone_ = static_cast<uint32_t*>(m_stateP_) + DataMirroring::COMMITDONE_POS;
						*commitDone_ = 0U;

						stateMutex_->release();
					}

					AES_GCC_TRACE_MESSAGE("current setting: File:<%d>, Block:<%d>", (*m_curFile_), (*m_curBlock_));
				}
			}

		} // if (!m_multiCPsystem)

		// Save parameter and activate the thread
		delay_ = delay;
		exit_ = false;
		AES_GCC_TRACE_MESSAGE("Activating thread tq_:<%s>", tq_.c_str());

		if(this->activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED) < 0)
		{
			AES_GCC_TRACE_MESSAGE("Error. Failed to start svc thread");
		}
    }
}


//******************************************************************************
//
void aes_dbo_mirrormaintainer::remove()
{
	AES_GCC_TRACE_MESSAGE("Removing mirroring on tq_:<%s>",tq_.c_str());
    // Set time for the which is not completed
    // Get current time in seconds since January 1, 1970
    unsigned long now = time(NULL);
    const size_t fileNameMaxSize = 20;

    // Get filename for the report to queue
    stateMutex_->acquire();

    if (m_multiCPsystem)
	{
		for (uint32_t w = 0; w < MAX_BLADE_NUMBER; ++w)
		{
			if (msList[w].streamId_.compare(MSL_NULL) == 0)
			{
				break;
			}

			if(NULL != msList[w].curFile_)
			{
				char fileName[fileNameMaxSize]={0};
				ACE_OS::snprintf(fileName, fileNameMaxSize-1, "%s+%.10u", msList[w].streamId_.c_str(), (*msList[w].curFile_));

				// Create a new mirror record and append it to the queue
				MirrorRecord mr;
				strcpy(mr.fName_, fileName);
				mr.rtime_ = now + delay_ * 60;      // The time is when to remove the file

				// Check if there is a started file to close
				if (queue_.change(mr))
				{
					// Increase current file counter
					if (++(*msList[w].curFile_) > DataMirroring::MAX_MIRROR_DATAFILE_INDEX)
						(*msList[w].curFile_) = 1U;
				}
			}
		} // for (int w...)
    }
    else
	{
		// Not a multi CP system
    	if(NULL != m_curFile_)
    	{
    		char fileName[fileNameMaxSize]={0};
    		ACE_OS::snprintf(fileName, fileNameMaxSize-1, "%.10u", *m_curFile_);

    		// Create a new mirror record and append it to the queue
    		MirrorRecord mr;
    		strcpy(mr.fName_, fileName);
    		mr.rtime_ = now + delay_ * 60;      // The time is when to remove the file

    		// Check if there is a started file to close
    		if (queue_.change(mr))
    		{
    			// Increase current file counter
    			if (++(*m_curFile_) > DataMirroring::MAX_MIRROR_DATAFILE_INDEX)
    				*m_curFile_ = 1U;
    		}
    	}
	}

    stateMutex_->release();

    // Set the flags
    exit_ = true;
}


//******************************************************************************
//
void aes_dbo_mirrormaintainer::abort()
{
	AES_GCC_TRACE_MESSAGE("Aborting tq_:<%s>", tq_.c_str());
	queue_.clear();
	exit_ = true;
}


//******************************************************************************
//
int aes_dbo_mirrormaintainer::svc()
{
	AES_GCC_TRACE_MESSAGE("tq_:<%s>, exit:<%s>", tq_.c_str(), (exit_ ? "true" : "false"));

    // Mark the maintainer as running to avoid multiples
    running_ = true;

    ACE_Time_Value tv(DataMirroring::DELETE_CHECK_INTERVAL_IN_SEC, 0);
    // Thread loop
    while ((!exit_ || !queue_.empty()) && (!exitForced_))
    {
        // Sleep until a check should be made
        ACE_OS::sleep(tv);
        // Get current time in seconds since January 1, 1970
        unsigned long now = time(NULL);

        // Go through the queue for expired records
        MirrorRecord mr;
        while((queue_.head(mr) && mr.rtime_ <= now) && (!exitForced_))
        {
        	// Remove the file
            std::string fullFilePath(tqPath_);
            fullFilePath.append(DataMirroring::MIRROR_DATA_FOLDER);
            fullFilePath.append(mr.fName_);

            AES_GCC_TRACE_MESSAGE("time to remove mirror file:<%s> of tq_:<%s>", fullFilePath.c_str(), tq_.c_str() );

            if( (-1 == ::remove(fullFilePath.c_str())) && ( ENOENT != errno))
            {
                int error = ACE_OS::last_error();
                GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::svc() couldn't remove: %s, error: %u\n", fullFilePath.c_str(), error));
            }
            else
            {
            	AES_GCC_TRACE_MESSAGE("File removed, update queue of file");
            	// Remove record from queue
            	queue_.remove();
            }
        }
    }

    AES_GCC_TRACE_MESSAGE("After while loop: starting cleanup for tq_:<%s>", tq_.c_str());

    // Do some cleanup
    if (!m_multiCPsystem)
	{
		// Close the map to the state file
		if(MAP_FAILED != m_stateP_)
		{
			ACE_OS::munmap( m_stateP_, DataMirroring::MIRROR_FILESTATE_SIZE );
			m_stateP_ = MAP_FAILED;
		}

	    m_curFile_ = NULL;
	    m_curBlock_ = NULL;
    }
    else
	{
		// Multi CP system
		for(uint32_t w = 0; w < MAX_BLADE_NUMBER; ++w)
		{
			if (msList[w].streamId_.compare(MSL_NULL) == 0)
			{
				break;
			}

			if(MAP_FAILED != msList[w].stateP_)
			{
				ACE_OS::munmap( msList[w].stateP_, DataMirroring::MIRROR_FILESTATE_SIZE );
				msList[w].stateP_ = MAP_FAILED;
			}
			msList[w].curBlock_ = NULL;
			msList[w].curFile_ = NULL;
			msList[w].streamId_.clear();
			msList[w].streamId_ = MSL_NULL;
		}
    }

    queue_.close();

    AES_GCC_TRACE_MESSAGE("Cleanup activities terminated for tq_:<%s>", tq_.c_str());
    // Now it's possible to accept a new maintainer
    running_ = false;
    AES_GCC_TRACE_MESSAGE("Leaving");
    return 0;
}


//******************************************************************************
//
void aes_dbo_mirrormaintainer::changeDelay(unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("Changing Delay in <%d> of tq:<%s>", delay, tq_.c_str());
    // Save the new parameter
    delay_ = delay;
}

unsigned int aes_dbo_mirrormaintainer::newFile(const std::string &fName)
{
	AES_GCC_TRACE_MESSAGE("Adding new mirror file:<%s> of tq:<%s>", fName.c_str(), tq_.c_str());

	if (m_multiCPsystem)
	{
		// For example, fName = "CP1OLLE+0000000001"
		// get the stream id from file name
		std::string stream_ID;

		// Get the stream id
		size_t tagPlusPos = fName.find(DataMirroring::MIRROR_FILE_SEP);

		if( std::string::npos != tagPlusPos )
		{
			stream_ID = fName.substr(0, tagPlusPos);
		}

		AES_GCC_TRACE_MESSAGE("current stream_ID:<%s>", stream_ID.c_str());

		bool f_found = false;
		uint32_t index = INDEX_ERROR;

		// search stream id in the current opened state file
		for(uint32_t w = 0; w < MAX_BLADE_NUMBER; ++w)
		{
			if(msList[w].streamId_.compare(stream_ID) == 0)
			{
				AES_GCC_TRACE_MESSAGE("state file already mapped at index:<%d>", w);
				f_found = true;
				break;
			}
			else if(msList[w].streamId_.compare(MSL_NULL) == 0)
			{
				// there is not other field with a value
				AES_GCC_TRACE_MESSAGE("state file not mapped, insert it to index:<%d>", w);
				index = w; // First free location
				break;
			}
		}

		if (!f_found)
		{
			// The state file related to this stream ID is not mapped
			if (index == INDEX_ERROR)
			{
				return AES_CATASTROPHIC;
			}

			// Open state file
			bool existing = false;

			char stateFilePath[512] = {0};
			ACE_OS::snprintf(stateFilePath, 511, "%s%s-%s", cfgTqPath_.c_str(), DataMirroring::MIRROR_METADATA, stream_ID.c_str() );

			std::string stateName(stateFilePath);

			ACE_HANDLE fileStateDescriptor = openMirrorStateFile(stateName, existing);

			// Check if state file has been opened
			if( ACE_INVALID_HANDLE != fileStateDescriptor )
			{
				AES_GCC_TRACE_MESSAGE("Create a memory mapped of the state file");

				// Create the memory mapping of the file, it will be shared with DBO client
				void* stateFileMap = ACE_OS::mmap(NULL, DataMirroring::MIRROR_FILESTATE_SIZE, PROT_RDWR, MAP_SHARED, fileStateDescriptor, 0);

				// Close the file handle since it is not more needed
				ACE_OS::close(fileStateDescriptor);

				// check if the mapping failed
				if(MAP_FAILED == stateFileMap)
				{
					int error = ACE_OS::last_error();
					GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::aes_dbo_mirrormaintainer() couldn't open file mapping of file: %s, error: %u\n", stateName.c_str(), error));
				}
				else
				{
					// Initialize state variables
					unsigned int* curFile_ = static_cast<uint32_t*>(stateFileMap);
					unsigned int* curBlock_ = static_cast<uint32_t*>(stateFileMap) + DataMirroring::CURRENT_BLOCK_POS;

					if(existing)
					{
						AES_GCC_TRACE_MESSAGE("File has been created now, set initial values");
						// Set initial values
						stateMutex_->acquire();
						*curFile_ = 1U;
						*curBlock_ = 0U;
						uint32_t* dataWritten_ = static_cast<uint32_t*>(stateFileMap) + DataMirroring::DATAWRITTEN_POS;
						*dataWritten_ = 0U;
						uint32_t* commitDone_ = static_cast<uint32_t*>(stateFileMap) + DataMirroring::COMMITDONE_POS;
						*commitDone_ = 0U;
						stateMutex_->release();
					}

					AES_GCC_TRACE_MESSAGE("Current setting: File:<%d>, Block:<%d>", (*curFile_), (*curBlock_));

					// Update entry in msList
					msList[index].streamId_ = stream_ID;
					msList[index].stateP_ = stateFileMap;
					msList[index].curFile_ = curFile_;
					msList[index].curBlock_ = curBlock_;
				}
			}
		}
	} // if (m_multiCPsystem)

	// Check if a MirrorRecord with the same 'fName_' is already stored in the queue
	if(!queue_.searchFileName(fName.c_str()))
	{
		// Create a new mirror record and append it to the queue
		MirrorRecord mr;
		strcpy(mr.fName_, fName.c_str());
		mr.rtime_ = UINT_MAX;           // The time is set to UINT_MAX for a new file
		queue_.append(mr);
	}

	return AES_NOERRORCODE;
}

unsigned int aes_dbo_mirrormaintainer::closeFile(const std::string &fName)
{
	AES_GCC_TRACE_MESSAGE("Closing mirror file:<%s> with delay:<%u> in tq:<%s>", fName.c_str(), delay_, tq_.c_str());

    // Get current time in seconds since January 1, 1970
    unsigned long now = time(NULL);

    // Create a new mirror record and append it to the queue
    MirrorRecord mr;
    strcpy(mr.fName_, fName.c_str());
    mr.rtime_ = now + delay_ * 60;      // The time is when to remove the file
    queue_.change(mr);
    return AES_NOERRORCODE;
}


//******************************************************************************
//
unsigned int aes_dbo_mirrormaintainer::checkDataArea()
{
    // Get the path for this tq
	AES_GCC_TRACE_MESSAGE("Check Data Area in tq:<%s>", tq_.c_str());
    int result = ::access(tqPath_.c_str(),F_OK);
    if (result != 0)
    {
    	result = errno;
    	GCC_TDEBUG((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::checkDataArea() Error file not exists error = %d\n", result));
    	return AES_DATAAREAERROR;
    }
    return AES_NOERRORCODE;
}

///    ------------------------------------------------------------------------
//     doDirDelete()
//     For the removal of sub directories and files
//     ------------------------------------------------------------------------
//  Returns true,if the function deletes all the files and sub directories.
//  Returns false if the function failed to delete any file or sub directory.

void aes_dbo_mirrormaintainer::removeDir(const std::string &path)
{
	AES_GCC_TRACE_MESSAGE("Entering..., folder:<%s>", path.c_str() );

	try
	{
		boost::filesystem::remove_all(path);
	}
	catch(boost::filesystem::filesystem_error &ex)
	{
		GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::removeDir(), failed to remove folder:<%s>, error:<%s>\n", path.c_str(), ex.what()));
	}

	AES_GCC_TRACE_MESSAGE("...Leaving");
}

//******************************************************************************
//
void aes_dbo_mirrormaintainer::makeDir(const std::string &dir)
{
	ACE_stat statBuffer;
	int ret =0;
	AES_GCC_TRACE_MESSAGE("Make new directory:<%s>", dir.c_str());
	ret = ACE_OS::stat(dir.c_str(), &statBuffer);
	if( ret != 0 )
	{
		int error = ACE_OS::last_error();
		GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::makeDir() error: %d for directory: %s\n", error, dir.c_str()));

		// The directory doesn't exist, create it!
		if (ACE_OS::mkdir(dir.c_str(), NULL) == -1)
		{
			GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::makeDir() create directory failed: %s\n", dir.c_str()));
		}
	}
}

//******************************************************************************
//
ACE_HANDLE aes_dbo_mirrormaintainer::openMirrorStateFile(const std::string& stateFile, bool& fileCreated)
{
	AES_GCC_TRACE_MESSAGE("Entering..., state file:<%s>", stateFile.c_str());

	ACE_HANDLE fileStateDescriptor = ACE_INVALID_HANDLE;
	fileCreated = false;
	std::string errorDetail;

	// try to open the state file if it already exists
	fileStateDescriptor = ACE_OS::open(stateFile.c_str(), O_RDWR);

	if(ACE_INVALID_HANDLE == fileStateDescriptor)
	{
		AES_GCC_TRACE_MESSAGE("state file does not exist, error:<%d>, create it", ACE_OS::last_error());

		// create the state file of the tq
		fileStateDescriptor = ACE_OS::open(stateFile.c_str(), O_CREAT | O_RDWR);

		if( ACE_INVALID_HANDLE != fileStateDescriptor )
		{
			// Prepare a file large enough to hold all data
			off_t result = ACE_OS::lseek(fileStateDescriptor, (DataMirroring::MIRROR_FILESTATE_SIZE + 1), SEEK_SET);

			if( (DataMirroring::MIRROR_FILESTATE_SIZE + 1) == result)
			{
				// write a terminating cap
				if( ACE_OS::write(fileStateDescriptor, "", 1) != -1)
				{
					// reposition to begin
					ACE_OS::lseek(fileStateDescriptor, 0, SEEK_SET);
					fileCreated = true;
					AES_GCC_TRACE_MESSAGE("state file:<%s> created!", stateFile.c_str());
				}
				else
				{
					// some error happens on write
					char errorMsg[64]={0};
					errorDetail = strerror_r(ACE_OS::last_error(), errorMsg, 63);
					AES_GCC_TRACE_MESSAGE("write of termination cap on state file:<%s> failed, error:<%s>", stateFile.c_str(), errorDetail.c_str());
					ACE_OS::close(fileStateDescriptor);
					fileStateDescriptor = ACE_INVALID_HANDLE;
					ACE_OS::unlink(stateFile.c_str());
				}
			}
			else
			{
				// some error happens on lseek
				char errorMsg[64]={0};
				errorDetail = strerror_r(ACE_OS::last_error(), errorMsg, 63);
				AES_GCC_TRACE_MESSAGE("lseek on state file:<%s> failed, error:<%s>", stateFile.c_str(), errorDetail.c_str());
				ACE_OS::close(fileStateDescriptor);
				fileStateDescriptor = ACE_INVALID_HANDLE;
				ACE_OS::unlink(stateFile.c_str());
			}
		}
		else
		{
			// error on file create
			char errorMsg[64]={0};
			errorDetail = strerror_r(ACE_OS::last_error(), errorMsg, 63);
			AES_GCC_TRACE_MESSAGE("create of state file:<%s> failed, error:<%s>", stateFile.c_str(), errorDetail.c_str());
		}

		if(ACE_INVALID_HANDLE == fileStateDescriptor)
		{
			int error = ACE_OS::last_error();
			GCC_TERROR((AES_DBO_Mirrormaintainer,"(%t) aes_dbo_mirrormaintainer::openMirrorStateFile() couldn't open: %s, error: %u\n", stateFile.c_str(), error));
		}
	}

	return fileStateDescriptor;
}

//******************************************************************************
//
void aes_dbo_mirrormaintainer::forceExit()
{
	exitForced_ = true;
}

