/* =================================================================== */
	/**
	@file aes_gcc_stream.cpp

	Class method implementation for aes_gcc_stream.h

	DESCRIPTION
	The services provided by AES_GCC_Stream facilitates the creation of a stream.

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
#include "aes_gcc_stream.h"
#include "aes_gcc_tracer.h"
#include "aes_gcc_errorcodes.h"

#include <ace/ACE.h>
#include <ace/Event.h>
#include <ace/Time_Value.h>

#include <ace/Process_Semaphore.h>
#include <ace/Process_Mutex.h>

namespace Stream_Parameter {
	const char PROCESS_MUTEX_NAME_PREFIX[] = "AES_GCC_MUTEX_";
	const char PROCESS_SEMAPHORE_NAME_PREFIX[] = "AES_GCC_SEMAPHORE_";
	const char TQ_SHARED_MEM_PREFIX[] = "/MEM_";
}


/*===================================================================
                        DECLARATION SECTION
=================================================================== */
AES_GCC_TRACE_DEFINE(AES_GCC_Stream);

/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_Stream::AES_GCC_Stream()
: m_processMutex(NULL),
  m_procesSem(NULL),
  attr_(NULL),
  m_data(NULL),
  m_sharedMemorySize(0U),
  m_sharedMemoryAddr(MAP_FAILED)
{

}


/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_Stream::~AES_GCC_Stream()
{
	if( MAP_FAILED != m_sharedMemoryAddr )
	{
		ACE_OS::munmap( m_sharedMemoryAddr, m_sharedMemorySize );
	}
}

ACE_HANDLE AES_GCC_Stream::openSharedDataFile(const std::string& dataFile, const ACE_UINT32& fileSize)
{
	AES_GCC_TRACE_MESSAGE("Entering..., shared data file:<%s>", dataFile.c_str());

	// try to open the data file if it already exists
	ACE_HANDLE dataFileHandle = ACE_OS::open(dataFile.c_str(), O_RDWR );

	if(ACE_INVALID_HANDLE == dataFileHandle )
	{
		// data file not exist, create it
		dataFileHandle = ACE_OS::open(dataFile.c_str(), O_CREAT | O_RDWR);

		if( ACE_INVALID_HANDLE != dataFileHandle )
		{
			// Prepare a file large enough to hold all data
			off_t result = ACE_OS::lseek(dataFileHandle, (fileSize + 1U), SEEK_SET);

			if( (fileSize + 1U) == result)
			{
				// write a terminating cap
				if( ACE_OS::write(dataFileHandle, "", 1) != -1)
				{
					// reposition to begin
					ACE_OS::lseek(dataFileHandle, 0, SEEK_SET);
					AES_GCC_TRACE_MESSAGE("data file:<%s> created!", dataFile.c_str());
				}
				else
				{
					// some error happens on write
					AES_GCC_TRACE_MESSAGE("write termination cap on data file:<%s> fails, error:<%d>", dataFile.c_str(), ACE_OS::last_error());
					ACE_OS::close(dataFileHandle);
					dataFileHandle = ACE_INVALID_HANDLE;
					// remove the created file
					::remove(dataFile.c_str());
				}
			}
			else
			{
				// some error happens on lseek
				AES_GCC_TRACE_MESSAGE("lseek on data file:<%s> fails, error:<%d>", dataFile.c_str(), ACE_OS::last_error());
				ACE_OS::close(dataFileHandle);
				dataFileHandle = ACE_INVALID_HANDLE;
				// remove the created file
				::remove(dataFile.c_str());
			}
		}
		else
		{
			// error on file create
			AES_GCC_TRACE_MESSAGE("create of data file:<%s> fails, error:<%d>", dataFile.c_str(), ACE_OS::last_error());
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%s>", ((ACE_INVALID_HANDLE != dataFileHandle) ? "OK" : "NOT OK") );
	return dataFileHandle;
}

/*===================================================================
                    ROUTINE: createSyncObject
=================================================================== */
bool AES_GCC_Stream::createSyncObject(const std::string& TqName)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", TqName.c_str());
	bool result = false;

	std::string nameOfMutex(Stream_Parameter::PROCESS_MUTEX_NAME_PREFIX);
	nameOfMutex.append(TqName);

	AES_GCC_TRACE_MESSAGE("create process mutex name:<%s>", nameOfMutex.c_str());

	//Create a process mutex to synchronize access to the shared memory
	m_processMutex = new (std::nothrow) ACE_Process_Mutex(nameOfMutex.c_str());

	if(NULL != m_processMutex)
	{
		std::string nameOfSem(Stream_Parameter::PROCESS_SEMAPHORE_NAME_PREFIX);
		nameOfSem.append(TqName);

		AES_GCC_TRACE_MESSAGE("create process semaphore name:<%s>", nameOfSem.c_str());
		//Create a process semaphore to signal data ready
		// initial state is not signaled and max reachable value is one
		m_procesSem = new (std::nothrow) ACE_Process_Semaphore(0, nameOfSem.c_str(), NULL, 1);

		if (NULL != m_procesSem)
		{
			result = true;
		}
		else
		{
			AES_GCC_TRACE_MESSAGE("Failed to create process semaphore:<%s>, error:<%d>", nameOfSem.c_str(), ACE_OS::last_error());
		}
	}
	else
	{
		AES_GCC_TRACE_MESSAGE("Failed to create process mutex:<%s>, error:<%d>", nameOfMutex.c_str(), ACE_OS::last_error());
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%s>", (result ? "OK" : "NOT OK") );
	return result;
}

/*===================================================================
                    ROUTINE: waitDataReady
=================================================================== */
bool AES_GCC_Stream::waitDataReady(unsigned int timeoutMs, ACE_Event* cancelReadEvent)
{
	AES_GCC_TRACE_MESSAGE("Entering...");
	bool dataReady = false;

	const ACE_UINT32 checkIntervalMs = 10U;
	ACE_UINT32 numLoop = static_cast<ACE_UINT32> (timeoutMs / checkIntervalMs);
	ACE_Time_Value waitBeforeRetry(0,checkIntervalMs*1000);

	AES_GCC_TRACE_MESSAGE("waiting time:<%u>ms, check interval:<%u>, num of attempts:<%u>",
							timeoutMs, checkIntervalMs, numLoop);

	do
	{
		// check if data are ready without blocking
		if( m_procesSem->tryacquire() < 0)
		{
			// data are not ready
			numLoop--;

			if(NULL != cancelReadEvent)
			{
				// check cancel event
				ACE_Time_Value absTimeout(ACE_OS::gettimeofday());
				if( cancelReadEvent->wait(&absTimeout) == 0)
				{
					// Cancel read signaled, terminate wait
					AES_GCC_TRACE_MESSAGE("Cancel read arrived");
					break;
				}
			}
			// wait before retry
			ACE_OS::sleep(waitBeforeRetry);
		}
		else
		{
			// data are ready
			dataReady = true;
		}

	}while( !dataReady && (numLoop > 0));

	AES_GCC_TRACE_MESSAGE("...Leaving, data are ready:<%s>", (dataReady ? "YES" : "NO") );
	return dataReady;
}

/*===================================================================
                    ROUTINE: create
=================================================================== */
bool AES_GCC_Stream::create(const std::string& TqName, const std::string& TqPath, const ACE_UINT32& sharedMemSize)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>, TQ path:<%s> memory size:<%u>",
							TqName.c_str(), TqPath.c_str(), sharedMemSize);

	bool result = false;

	if( createSyncObject(TqName) )
	{
		// Assemble the shared data file name
		std::string fileNameToMap(TqPath);
		fileNameToMap.append(Stream_Parameter::TQ_SHARED_MEM_PREFIX);
		fileNameToMap.append(TqName);

		// Calculate the memory size in bytes
		m_sharedMemorySize = sharedMemSize + sizeof(SharedAttributes);

		ACE_HANDLE fileDataHandle = openSharedDataFile(fileNameToMap, m_sharedMemorySize);

		if(ACE_INVALID_HANDLE != fileDataHandle )
		{
			AES_GCC_TRACE_MESSAGE("map a shared memory of size:<%u>, file handle:<%d>", m_sharedMemorySize, fileDataHandle);

			// Create the memory mapping of the data file, it will be shared with CDH server
			m_sharedMemoryAddr = ACE_OS::mmap( 0, m_sharedMemorySize, PROT_READ| PROT_WRITE, MAP_SHARED, fileDataHandle);

			// Close the file handle since it is not more needed
			ACE_OS::close(fileDataHandle);

			if( MAP_FAILED != m_sharedMemoryAddr )
			{
				// Get the pointer of the shared memory control structure
				attr_= static_cast<SharedAttributes*>(m_sharedMemoryAddr);

				// Get a pointer to the beginning of data
				m_data = static_cast<char*>(m_sharedMemoryAddr) + sizeof(SharedAttributes);

				// Save size and reset positions
				attr_->size_ = sharedMemSize;
				attr_->rPos_ = 0U;
				attr_->wPos_ = 0U;
				attr_->clst_ = 0U;

				result = true;
			}
			else
			{
				AES_GCC_TRACE_MESSAGE("Failed to map into memory data file:<%s>, error:<%d>", fileNameToMap.c_str(), ACE_OS::last_error());
			}
		}
	}
	
	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%s>", (result ? "TRUE" : "FALSE") );

	return result;
}

bool AES_GCC_Stream::open(const std::string& TqName, const std::string& TqPath, const ACE_UINT32& sharedMemSize)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>, TQ path:<%s> memory size:<%u>",
							TqName.c_str(), TqPath.c_str(), sharedMemSize);

	bool result = false;

	if( createSyncObject(TqName) )
	{
		// Assemble the shared data file name
		std::string fileNameToMap(TqPath);
		fileNameToMap.append(Stream_Parameter::TQ_SHARED_MEM_PREFIX);
		fileNameToMap.append(TqName);

		AES_GCC_TRACE_MESSAGE("open data file to share:<%s>", fileNameToMap.c_str() );

		// open the data file, it must already exists
		ACE_HANDLE dataFileHandle = ACE_OS::open(fileNameToMap.c_str(), O_RDWR );

		if(ACE_INVALID_HANDLE != dataFileHandle )
		{
			// Calculate the memory size in bytes
			m_sharedMemorySize = sharedMemSize + sizeof(SharedAttributes);
			AES_GCC_TRACE_MESSAGE("map a shared memory of size:<%u>, file handle:<%d>", m_sharedMemorySize, dataFileHandle);

			// Create the memory mapping of the data file, it will be shared with CDH server
			m_sharedMemoryAddr = ACE_OS::mmap( 0, m_sharedMemorySize, PROT_READ| PROT_WRITE, MAP_SHARED, dataFileHandle);

			// Close the file handle since it is not more needed
			ACE_OS::close(dataFileHandle);

			if( MAP_FAILED != m_sharedMemoryAddr )
			{
				// Get the pointer of the shared memory control structure
				attr_= static_cast<SharedAttributes*>(m_sharedMemoryAddr);

				// Get a pointer to the beginning of data
				m_data = static_cast<char*>(m_sharedMemoryAddr) + sizeof(SharedAttributes);

				result = true;
			}
			else
			{
				AES_GCC_TRACE_MESSAGE("Failed to map into memory data file:<%s>, error:<%d>", fileNameToMap.c_str(), ACE_OS::last_error());
			}
		}
		else
		{
			AES_GCC_TRACE_MESSAGE("open data file:<%s> fails, error:<%d>", fileNameToMap.c_str(), ACE_OS::last_error());
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%s>", (result ? "TRUE" : "FALSE") );
    return result;
}

/*===================================================================
                    ROUTINE: close
=================================================================== */
void AES_GCC_Stream::close()
{
	AES_GCC_TRACE_MESSAGE("Entering...");

	// Close the shared memory
	if( MAP_FAILED != m_sharedMemoryAddr )
	{
		ACE_OS::munmap( m_sharedMemoryAddr, m_sharedMemorySize);
		m_sharedMemoryAddr = MAP_FAILED;
	}

	// Close the synchronization objects
	if( NULL != m_processMutex )
	{
		delete m_processMutex;
		m_processMutex = NULL;
	}

	if( NULL != m_procesSem )
	{
		delete m_procesSem;
		m_procesSem = NULL;
	}

	AES_GCC_TRACE_MESSAGE("...Leaving");
}

/*===================================================================
                    ROUTINE: read
=================================================================== */
ACE_UINT32 AES_GCC_Stream::read(char *buf, unsigned int &len, ACE_UINT32 timeout)
{
	return read(buf, len, timeout, NULL);
}


/*===================================================================
                    ROUTINE: read
=================================================================== */
ACE_UINT32 AES_GCC_Stream::read(char *buf, unsigned int &len,
		unsigned int timeout, ACE_Event* hCancel)
{
	AES_GCC_TRACE_MESSAGE("Entering..., acquiring the process mutex in read");

	// Process synchronization APPs:(RTR/CPF) and CDH
	m_processMutex->acquire();
	AES_GCC_TRACE_MESSAGE("Read pos:<%u>, Write pos:<%u>, Clear status:<%u>",
								attr_->rPos_, attr_->wPos_, attr_->clst_);

	if(1U == attr_->clst_ )
	{
		AES_GCC_TRACE_MESSAGE("Data are cleared!");
		attr_->clst_ = 0U;
		m_processMutex->release();
		return AES_BUFFEREMPTY;
	}

	// checks for data ready to read
	while(attr_->wPos_ == attr_->rPos_)
	{
		// No data ready to read
		// Release process mutex to allow write data
		m_processMutex->release();

		AES_GCC_TRACE_MESSAGE("wait on semaphore signal for data ready");

		if( !waitDataReady(timeout, hCancel) )
		{
			AES_GCC_TRACE_MESSAGE("No data ready! timeout elapsed");
			return AES_TIMEOUT;
		}
		// Acquire process mutex to check condition
		m_processMutex->acquire();
	}

	// Data are ready, check for a data clear
	if( 1U == attr_->clst_ )
	{
		AES_GCC_TRACE_MESSAGE("Data cleared by application");
		attr_->clst_ = 0U;
		m_processMutex->release();
		return AES_BUFFEREMPTY;
	}
	
	AES_GCC_TRACE_MESSAGE("Current: Read pos:<%u>, Write pos:<%u>, len:<%u>",
								attr_->rPos_, attr_->wPos_, len);

	// Check if we have enough data in the stream as
	// specified by len, otherwise adjust len
	if (attr_->wPos_ >= attr_->rPos_)
	{
		AES_GCC_TRACE_MESSAGE("attr_->wPos_ major of attr_->rPos_");
		// Adjust len to be what is available up to
		// the 'write-offset' in the stream
		if ( attr_->wPos_ - attr_->rPos_ < len )
		{
			len = attr_->wPos_ - attr_->rPos_;
			AES_GCC_TRACE_MESSAGE("length of the buffer to read adjusted to :<%d>", len);
		}
	}
	else
	{
		AES_GCC_TRACE_MESSAGE("attr_->wPos_ major of attr_->rPos_");
		// Adjust len to be what is available to the end of the stream +
		// what is available before the 'write-offset'
		if ( attr_->size_ - attr_->rPos_ + attr_->wPos_ < len )
		{
			len = attr_->size_ - attr_->rPos_ + attr_->wPos_;
			AES_GCC_TRACE_MESSAGE("length of the buffer to read adjusted to :<%d>", len);
		}
	}

	// Check if it should turn around to the beginning
	if (attr_->rPos_ + len > attr_->size_)
	{
		// Read all data until end of stream
		ACE_UINT32 subLen = attr_->size_ - attr_->rPos_;
		AES_GCC_TRACE_MESSAGE("Read all data until end of stream, subLength:<%u>", subLen);

		ACE_OS::memcpy(buf, m_data + attr_->rPos_, subLen);

		// Reset the read pointer and read the rest
		// of the chunk from the beginning of the stream
		ACE_UINT32 subLen2 = len - subLen;
		AES_GCC_TRACE_MESSAGE("Read remaining part of data, subLength:<%u>", subLen2);
		ACE_OS::memcpy(buf + subLen, m_data, subLen2);
		attr_->rPos_ = subLen2;
	}
	else
	{
		// Read data from the stream
		AES_GCC_TRACE_MESSAGE("read data length:<%u>", len);
		ACE_OS::memcpy(buf, m_data + attr_->rPos_, len);
		attr_->rPos_ += len;
	}

	AES_GCC_TRACE_MESSAGE("New: Read pos:<%u>, Write pos:<%u>",	attr_->rPos_, attr_->wPos_);

	// Check if attr_->rPos_ is out of boundaries
	if(attr_->rPos_ >= attr_->size_)
	{
		AES_GCC_TRACE_MESSAGE("reset read position to begin");
		attr_->rPos_ = 0;
	}

	// Check if it is any data left in the stream
	if (attr_->rPos_ != attr_->wPos_)
	{
		AES_GCC_TRACE_MESSAGE("there is other data ready");
		// If so, increase the semaphore
		m_procesSem->release();
	}

	m_processMutex->release();
	AES_GCC_TRACE_MESSAGE("...Leaving");
	return AES_NOERRORCODE;
}

/*===================================================================
                    ROUTINE: write
=================================================================== */
ACE_UINT32 AES_GCC_Stream::write(const char *buf, unsigned int len)
{
	// Process synchronization APPs:(RTR/CPF) and CDH
	m_processMutex->acquire();

	AES_GCC_TRACE_MESSAGE("Entering..., current: Read pos:<%u>, Write pos:<%u>, Buffer length:<%u>",
									attr_->rPos_, attr_->wPos_, len);

	// Check if it should turn around to the beginning
	if( (attr_->wPos_ + len) > attr_->size_)
	{

		// Check if we will be behind the reader
		if( ( (len - (attr_->size_ - attr_->wPos_) ) > (attr_->rPos_ - 1) ) ||
				(attr_->wPos_ < (attr_->rPos_ - 1)) )
		{
			AES_GCC_TRACE_MESSAGE("Buffer Full!");
			m_processMutex->release();
			return AES_BUFFERFULL;
		}

		AES_GCC_TRACE_MESSAGE("Write the data to the stream");

		// Write the data to the stream
		ACE_UINT32 subLen = attr_->size_ - attr_->wPos_;
		ACE_OS::memcpy(m_data + attr_->wPos_, buf, subLen);

		ACE_OS::memcpy(m_data , buf + subLen, len - subLen);
		attr_->wPos_ = len - subLen;

		// Signal new data ready
		m_procesSem->release();
	}
	else
	{
		// Check if we will be behind the reader
		if( (attr_->wPos_ < (attr_->rPos_ - 1) ) && ( (attr_->wPos_ + len) > (attr_->rPos_ - 1) ) )
		{
			AES_GCC_TRACE_MESSAGE("Buffer Full!");
			m_processMutex->release();
			return AES_BUFFERFULL;
		}

		AES_GCC_TRACE_MESSAGE("Write the data to the stream");

		// Write the data to the stream
		ACE_OS::memcpy(m_data + attr_->wPos_, buf, len);
		attr_->wPos_ += len;
		// Signal new data ready
		m_procesSem->release();
	}

	// Check if pos_ is out of boundaries
	if (attr_->wPos_ >= attr_->size_)
	{
		AES_GCC_TRACE_MESSAGE("reset Write pos to zero!");
		attr_->wPos_ = 0;
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, new Write pos:<%u>", attr_->wPos_ );
	m_processMutex->release();
	return AES_NOERRORCODE;
}

/*===================================================================
                    ROUTINE: clear
=================================================================== */
void AES_GCC_Stream::clear()
{
	AES_GCC_TRACE_MESSAGE("Entering..., acquire process mutex");

	// Process synchronization APPs:(RTR/CPF) and CDH
	m_processMutex->acquire();

	AES_GCC_TRACE_MESSAGE("reset read and write position to zero!");

	// Reset positions
	attr_->rPos_ = 0;
	attr_->wPos_ = 0;
	attr_->clst_ = 1;

	m_processMutex->release();

	AES_GCC_TRACE_MESSAGE("...Leaving");
}
