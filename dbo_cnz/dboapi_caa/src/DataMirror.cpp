
//******************************************************************************
// 
// .NAME
//  	DataMirror - The block data mirror
// .LIBRARY 3C++
// .PAGENAME DataMirror
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_TransferQueue.h

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
// 	This class handles the mirroring of data to disk.
//  

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 xxxx

// AUTHOR 
// 	2001-05-02 by UAB/KB/AU Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R1A_DMDN6.lib

// .SEE ALSO 
//  

//******************************************************************************
#include "DataMirror.h"
#include "EventCode.h"
#include "ParameterHandler.h"
#include "CommandCode.h"
#include "AES_DBO_AsynchComm.h"

#include <aes_gcc_errorcodes.h>
#include <aes_gcc_tracer.h>

#include <ace/Process_Mutex.h>

#include "syslog.h"

#include <string>
#include <fstream>


namespace{

	const char MIRROR_PROCESS_MUTEX_NAME[] = "AES_DBO_MUTEX_STATE_";
	const char MIRROR_METADATA[] = "/blockmirror/METADATA/state";
	const char BLADE_SEPARATOR = '-';
	const char MIRROR_DATA_FOLDER[] = "/DATA/";
	const off_t MIRROR_FILESTATE_SIZE = (4*sizeof(uint32_t));

	const off64_t MAX_MIRROR_DATAFILE_SIZE = (3*1024*1024); // max 3MB of data
	const uint32_t MAX_MIRROR_DATAFILE_INDEX = 4000000000U;

	const uint32_t CURRENT_FILE_POS  = 0;
	const uint32_t CURRENT_BLOCK_POS = 4;
	const uint32_t DATAWRITTEN_POS = 8;
	const uint32_t COMMITDONE_POS = 12;
}

// ACS Trace definition
AES_GCC_TRACE_DEFINE(AES_DBO_DataMirror);


//******************************************************************************
//
DataMirror::DataMirror(AES_DBO_AsynchComm *comm, const std::string &tq, const std::string &streamId)
: multiCPsystem(false),
  allOK_(true),
  data_(ACE_INVALID_HANDLE),
  m_TQname(tq),
  m_CurrentDataFile(),
  tqPath_(ParameterHandler::instance()->getRootDir()),
  streamId_(streamId),
  m_MirrorStateFile(),
  m_ProcessMutexName(MIRROR_PROCESS_MUTEX_NAME),
  curFile_(NULL),
  curBlock_(NULL),
  dataWritten_(NULL),
  commitDone_(NULL),
  curBytes_(0),
  stateP_(MAP_FAILED),
  comm_(comm)
{
	m_ProcessMutexName.append(m_TQname);
	stateMutex_ = new (std::nothrow) ACE_Process_Mutex(m_ProcessMutexName.c_str());

	// Check if Blade Cluster, since streamID is empty on SCP
	multiCPsystem = !streamId_.empty();

	// Get the path for this TQ
	tqPath_.append(m_TQname);

	// Load previous state
	if( (NULL != stateMutex_) && loadState() )
	{
		// Create the filename
		setCurrentFileName();

		data_ = ACE_OS::open(m_CurrentDataFile.c_str(), O_CREAT | O_WRONLY);

		if( ACE_INVALID_HANDLE == data_ )
		{
			char errorMsg[64]={0};
			std::string errorDetail(strerror_r(ACE_OS::last_error(), errorMsg, 63));
			syslog(LOG_ERR,"DataMirror::DataMirror() couldn't open: %s, error detail: %s\n", m_CurrentDataFile.c_str(), errorDetail.c_str());
			AES_GCC_TRACE_MESSAGE("Failed to open data file: %s, error detail: %s", m_CurrentDataFile.c_str(), errorDetail.c_str());
			allOK_ = false;
		}
		else
		{
			// Get file length, save it to current written bytes and move to the end of file
			curBytes_ = ACE_OS::filesize(data_);
			ACE_OS::lseek(data_, 0, SEEK_END);

			if( reportToServer(CMD_MIRRORFILEBEG) != AES_NOERRORCODE )
			{
				AES_GCC_TRACE_MESSAGE("TQ:<%s>, Failed to send CMD_MIRRORFILEBEG to DBO", m_TQname.c_str() );
				allOK_ = false;
			}
		}
	}
	else
	{
		allOK_ = false;
	}
}


//******************************************************************************
//
DataMirror::~DataMirror()
{
	// Close the map to the state file
	if(MAP_FAILED != stateP_)
	{
		ACE_OS::munmap( stateP_, MIRROR_FILESTATE_SIZE );
	}

	if( NULL != stateMutex_ )
	{
		delete	stateMutex_;
		stateMutex_ = NULL;
	}
	// Close the data file descriptor
	ACE_OS::close(data_);

}

//******************************************************************************
//
void DataMirror::setCurrentFileName()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", m_TQname.c_str());

	m_CurrentDataFile.clear();

	// Create the filename
	const size_t fileNameMaxSize = 20;
	char fileName[fileNameMaxSize]={0};

	if (multiCPsystem)
		ACE_OS::snprintf(fileName, fileNameMaxSize-1, "%s+%.10u", streamId_.c_str(), *curFile_);
	else
		ACE_OS::snprintf(fileName, fileNameMaxSize-1, "%.10u", *curFile_);

	m_CurrentDataFile = tqPath_;
	m_CurrentDataFile.append(MIRROR_DATA_FOLDER);
	m_CurrentDataFile.append(fileName);

	AES_GCC_TRACE_MESSAGE("...Leaving, data file path:<%s>", m_CurrentDataFile.c_str());
}

//******************************************************************************
//
bool DataMirror::loadState()
{
	AES_GCC_TRACE_MESSAGE("Entering..., System:<%s>, TQ path:<%s>", (multiCPsystem ? "MCP" : "SCP"), tqPath_.c_str());

	bool loadResult = false;

	// flag to indicates when the state file has been created
	bool newFileState = false;

	ACE_HANDLE fileStateDescriptor = openMirrorStateFile(newFileState);

	// Check if state file has been opened
	if( ACE_INVALID_HANDLE != fileStateDescriptor )
	{
		// Create the memory mapping of the file, it will be shared with DBO server
		stateP_ = ACE_OS::mmap(NULL, MIRROR_FILESTATE_SIZE, PROT_RDWR, MAP_SHARED, fileStateDescriptor, 0);
		// Close the file handle since it is not more needed
		ACE_OS::close(fileStateDescriptor);

		// check if the mapping failed
		if(MAP_FAILED == stateP_)
		{
			char errorMsg[64]={0};
			std::string errorDetail = strerror_r(ACE_OS::last_error(), errorMsg, 63);
			syslog(LOG_ERR,"DataMirror::loadState() couldn't open file mapping of file: %s, error detail: %s\n", m_MirrorStateFile.c_str(), errorDetail.c_str());
			AES_GCC_TRACE_MESSAGE("Unable to create map of state file: %s, error detail: %s", m_MirrorStateFile.c_str(), errorDetail.c_str());
		}
		else
		{
			loadResult = true;

			// Initialize state variables
			curFile_ = static_cast<uint32_t*>(stateP_);
			curBlock_ = static_cast<uint32_t*>(stateP_) + CURRENT_BLOCK_POS;
			dataWritten_ = static_cast<uint32_t*>(stateP_) + DATAWRITTEN_POS;
			commitDone_ = static_cast<uint32_t*>(stateP_) + COMMITDONE_POS;

			if(newFileState)
			{
				AES_GCC_TRACE_MESSAGE("File has been created now, set initial values");
				// Set initial values
				stateMutex_->acquire();
				*curFile_ = 1U;
				*curBlock_ = 0U;
				*dataWritten_ = 0U;
				*commitDone_ = 0U;
				stateMutex_->release();
			}

			AES_GCC_TRACE_MESSAGE("current setting: File:<%d>, Block:<%d>, dataWritten:<%d>, commitDone_:<%d>",
								  (*curFile_), (*curBlock_), (*dataWritten_), (*commitDone_));
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, load result:<%s>", (loadResult ? "OK" : "NOT OK") );
	return loadResult;
}

ACE_HANDLE DataMirror::openMirrorStateFile(bool& fileCreated)
{
	AES_GCC_TRACE_MESSAGE("Entering...,TQ<%s>", m_TQname.c_str());

	ACE_HANDLE fileStateDescriptor = ACE_INVALID_HANDLE;
	fileCreated = false;

	std::string errorDetail;

	m_MirrorStateFile = ParameterHandler::instance()->getDBODataPath();

	char partialFilePath[256] = {0};

	if (multiCPsystem)
	{
		ACE_OS::snprintf(partialFilePath, 255, "%s%s-%s", m_TQname.c_str(), MIRROR_METADATA, streamId_.c_str() );
	}
	else
		ACE_OS::snprintf(partialFilePath, 255, "%s%s", m_TQname.c_str(), MIRROR_METADATA );

	m_MirrorStateFile.append(partialFilePath);

	AES_GCC_TRACE_MESSAGE("TQ state file:<%s>", m_MirrorStateFile.c_str());

	// try to open the state file if it already exists
	fileStateDescriptor = ACE_OS::open(m_MirrorStateFile.c_str(), O_RDWR);

	if(ACE_INVALID_HANDLE == fileStateDescriptor)
	{
		AES_GCC_TRACE_MESSAGE("TQ:<%s> state file not exist error:<%d>, create it", m_TQname.c_str(), ACE_OS::last_error());

		// create the state file of the tQ
		fileStateDescriptor = ACE_OS::open(m_MirrorStateFile.c_str(), O_CREAT | O_RDWR);

		if( ACE_INVALID_HANDLE != fileStateDescriptor )
		{
			// Prepare a file large enough to hold all data
			off_t result = ACE_OS::lseek(fileStateDescriptor, (MIRROR_FILESTATE_SIZE + 1), SEEK_SET);

			if( (MIRROR_FILESTATE_SIZE + 1) == result)
			{
				// write a terminating cap
				if( ACE_OS::write(fileStateDescriptor, "", 1) != -1)
				{
					// reposition to begin
					ACE_OS::lseek(fileStateDescriptor, 0, SEEK_SET);
					fileCreated = true;
					AES_GCC_TRACE_MESSAGE("state file:<%s> created!", m_MirrorStateFile.c_str());
				}
				else
				{
					// some error happens on write
					char errorMsg[64]={0};
					errorDetail = strerror_r(ACE_OS::last_error(), errorMsg, 63);
					AES_GCC_TRACE_MESSAGE("write of termination cap on state file:<%s> failed, error:<%s>", m_MirrorStateFile.c_str(), errorDetail.c_str());
					ACE_OS::close(fileStateDescriptor);
					fileStateDescriptor = ACE_INVALID_HANDLE;
					ACE_OS::unlink(m_MirrorStateFile.c_str());
				}
			}
			else
			{
				// some error happens on lseek
				char errorMsg[64]={0};
				errorDetail = strerror_r(ACE_OS::last_error(), errorMsg, 63);
				AES_GCC_TRACE_MESSAGE("lseek on state file:<%s> failed, error:<%s>", m_MirrorStateFile.c_str(), errorDetail.c_str());
				ACE_OS::close(fileStateDescriptor);
				fileStateDescriptor = ACE_INVALID_HANDLE;
				ACE_OS::unlink(m_MirrorStateFile.c_str());
			}
		}
		else
		{
			// error on file create
			char errorMsg[64]={0};
			errorDetail = strerror_r(ACE_OS::last_error(), errorMsg, 63);
			AES_GCC_TRACE_MESSAGE("create of state file:<%s> failed, error:<%s>", m_MirrorStateFile.c_str(), errorDetail.c_str());
		}

		if(ACE_INVALID_HANDLE == fileStateDescriptor)
		{
			syslog(LOG_ERR,"DataMirror::openMirrorStateFile() couldn't open: %s, error detail: %s\n", m_MirrorStateFile.c_str(), errorDetail.c_str());
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, state file descriptor <%d>", fileStateDescriptor);
	return fileStateDescriptor;
}

//******************************************************************************
//
unsigned int DataMirror::switchFile()
{
	AES_GCC_TRACE_MESSAGE("Entering...,TQ<%s>", m_TQname.c_str());

	// Report to server
	unsigned int retCode = reportToServer(CMD_MIRRORFILEEND);
	if (retCode != AES_NOERRORCODE)
	{
		AES_GCC_TRACE_MESSAGE("TQ:<%s>, Failed to send CMD_MIRRORFILEEND to DBO", m_TQname.c_str() );
		return retCode;
	}

	// Create a new filename
	stateMutex_->acquire();
	if (++(*curFile_) > MAX_MIRROR_DATAFILE_INDEX)
		*curFile_ = 1U;

	setCurrentFileName();

	stateMutex_->release();

	// Close the previous file
	ACE_OS::close(data_);
	// open the new file and check if it's OK
	data_ = ACE_OS::open(m_CurrentDataFile.c_str(), O_CREAT | O_WRONLY);

	if( ACE_INVALID_HANDLE == data_ )
	{
		char errorMsg[64]={0};
		std::string errorDetail(strerror_r(ACE_OS::last_error(), errorMsg, 63));
		syslog(LOG_ERR,"DataMirror::switchFile() couldn't open: %s, error detail: %s\n", m_CurrentDataFile.c_str(), errorDetail.c_str());
		AES_GCC_TRACE_MESSAGE("couldn't open: %s, error detail: %s", m_CurrentDataFile.c_str(), errorDetail.c_str());
		allOK_ = false;
	}
	else
	{
		retCode = reportToServer(CMD_MIRRORFILEBEG);

		if (retCode != AES_NOERRORCODE)
		{
			AES_GCC_TRACE_MESSAGE("TQ:<%s>, Failed to send CMD_MIRRORFILEBEG to DBO", m_TQname.c_str() );
			allOK_ = false;
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, TQ<%s>", m_TQname.c_str());

	return retCode;
}


//******************************************************************************
//
unsigned int DataMirror::reportToServer(unsigned int cmdCode)
{
	AES_GCC_TRACE_MESSAGE("Entering ..., TQ:<%s> send cmd:<%d>", m_TQname.c_str(), cmdCode);

	// Get filename for the report to server
	stateMutex_->acquire();

	// Create the filename
	const size_t fileNameMaxSize = 20;
	char fileName[fileNameMaxSize]={0};

	if (multiCPsystem)
		ACE_OS::snprintf(fileName, fileNameMaxSize-1, "%s+%.10u", streamId_.c_str(), *curFile_);
	else
		ACE_OS::snprintf(fileName, fileNameMaxSize-1, "%.10u", *curFile_);

	AES_GCC_TRACE_MESSAGE("current file name:<%s>", fileName);

	stateMutex_->release();

	ACS_APGCC_Command cmd;
	cmd.cmdCode = cmdCode;
	cmd.data[0] = fileName;

	// send the command to server
	unsigned int rCode = comm_->sendCmd(cmd);

	AES_GCC_TRACE_MESSAGE("...Leaving, send result:<%d>", rCode);

	return rCode;
}


//******************************************************************************
//
void DataMirror::clear()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", m_TQname.c_str());
	// Set initial values
	stateMutex_->acquire();
	*curBlock_ = 0U;
	stateMutex_->release();
	AES_GCC_TRACE_MESSAGE("...Leaving");
}


//******************************************************************************
//
unsigned int DataMirror::write(unsigned int blockNr, const char *buf, unsigned int len)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s> blockNr:<%d>", m_TQname.c_str(), blockNr);
	// Check if all is OK
	if (!allOK_)
	{
		// Something is wrong, but don't tell the application
		AES_GCC_TRACE_MESSAGE("Data Mirror state flag allOK_ is false!");
		return AES_NOERRORCODE;
	}

	// Check if it is a new or old block
	if (blockNr <= *curBlock_)
	{
		AES_GCC_TRACE_MESSAGE("block number is less than of the current block:<%d>", *curBlock_ );

		if(dataWritten_ && !commitDone_ && (blockNr == 0 || blockNr == 1))
		{
			AES_GCC_TRACE_MESSAGE("It is an old block");
			return AES_NOERRORCODE;                 // It's an old block
		}
		else if (blockNr > 1)
		{
			AES_GCC_TRACE_MESSAGE("It is an old block");
			return AES_NOERRORCODE;                 // It's an old block
		}
	}
	// Check for file switch
	if (curBytes_ >= MAX_MIRROR_DATAFILE_SIZE)
	{
		switchFile();
		curBytes_ = 0U;
	}

	// Write the data and check if it went OK
	ssize_t written = ACE_OS::write(data_, buf, len);

	if(written == -1)
	{
		char errorMsg[64]={0};
		std::string errorDetail(strerror_r(ACE_OS::last_error(), errorMsg, 63));
		syslog(LOG_ERR,"DataMirror::write() couldn't write to: %s, error detail: %s\n", m_CurrentDataFile.c_str(), errorDetail.c_str());
		AES_GCC_TRACE_MESSAGE("couldn't write to: %s, error detail: %s", m_CurrentDataFile.c_str(), errorDetail.c_str());
	}
	else if(written != len)
	{
		// Set the write position at the last successfully stored block and truncate the mirror file
		off_t lastBlockEndPos = ACE_OS::lseek(data_, -written, SEEK_END);
		ACE_OS::ftruncate(data_, lastBlockEndPos);

		char errorMsg[64]={0};
		std::string errorDetail(strerror_r(ACE_OS::last_error(), errorMsg, 63));
		syslog(LOG_ERR,"DataMirror::write() couldn't write to: %s, error detail: %s\n", m_CurrentDataFile.c_str(), errorDetail.c_str());
		AES_GCC_TRACE_MESSAGE("couldn't write to: %s, error detail: %s", m_CurrentDataFile.c_str(), errorDetail.c_str());
	}
	else
	{
		// Block successfully written into the mirror file
		// Save block number written to disk
		stateMutex_->acquire();
		*curBlock_ = blockNr;
		*dataWritten_ = 1U;
		stateMutex_->release();
		// Increase the number of bytes written
		curBytes_ += written;
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, TQ:<%s>, current written bytes:<%lld>", m_TQname.c_str(), curBytes_);

	return AES_NOERRORCODE;
}


//******************************************************************************
//
void DataMirror::commitIsDone()
{
	// Set flag
	*commitDone_ = 1U;
}
