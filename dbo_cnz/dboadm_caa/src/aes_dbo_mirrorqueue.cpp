
//******************************************************************************
//
// NAME
//      aes_dbo_mirrorqueue.cpp
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
//      Sorted Disc Queue

// DOCUMENT NO
//      19089-CAA 109 xxxx

// AUTHOR 
//      2002-09-06 by EAB/UKY/AU    Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include "aes_dbo_mirrorqueue.h"
#include "EventCode.h"
#include <aes_gcc_log.h>
#include <aes_gcc_tracer.h>
#include <limits.h>
#include <sstream>

GCC_TDEF(AES_DBO_Mirrorqueue);
AES_GCC_TRACE_DEFINE(AES_DBO_Mirrorqueue);

//******************************************************************************
//
void aes_dbo_mirrorqueue::save()
{
	// Write the data into the disk
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
	// First of all truncate the file to zero size
	ACE_OS::ftruncate(handle_, 0);
	// Set the write position at the file beginning
    ACE_OS::lseek(handle_, 0, SEEK_SET);
    unsigned int count = 0;
    unsigned int iter = 0;
    ssize_t written = 0;
    map<unsigned int, MirrorRecord>::iterator it = map_.begin();
    map<unsigned int, MirrorRecord>::iterator e = map_.end();
    while (it != e)
    {
        written = ACE_OS::write(handle_, (char *)&(*it).second, sizeof(MirrorRecord));	

        // Check if the file is OK
		if(written == -1)
		{
            int error = ACE_OS::last_error();
            GCC_TERROR((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::save() error: %u\n", error));
		}
		else if (written != sizeof(MirrorRecord))
		{
			// Set the write position at the last successfully stored record and truncate the queue file
			off_t lastRecordEndPos = ACE_OS::lseek(handle_, -written, SEEK_END);
			ACE_OS::ftruncate(handle_, lastRecordEndPos);
			GCC_TERROR((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::save() couldn't update file: %s\n", path_.c_str()));
		}
		else
		{
			// Mirror record successfully written into the queue file
			count++;
		}
		
        ++it;
        iter++;
    }
    
    ACE_OS::fsync(handle_);
    
    AES_GCC_TRACE_MESSAGE("Written %u records out of %u", count, iter);
}


//******************************************************************************
//
void aes_dbo_mirrorqueue::load()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
    // Read the contents of the file
    ACE_OS::lseek(handle_, 0, SEEK_SET);
    while (true)
    {
        MirrorRecord mr;
        long readBytes = 0;
 	
		readBytes= ACE_OS::read(handle_, &mr, sizeof(MirrorRecord));

		if (readBytes == 0)
		{
			break;
		}
		else if(readBytes == -1)
		{
			int error = ACE_OS::last_error();
			GCC_TERROR((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::load() error: %u\n", error));
			break;
		}
		// Check if the data is correctly read from the file
		else if (readBytes != sizeof(MirrorRecord))
		{
			int error = ACE_OS::last_error();
			GCC_TERROR((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::load() corrupted file/read error, file: %s, error: %u\n", path_.c_str(), error));
			break;  // Bail out
		}
		else
		{
			GCC_TDEBUG((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::load() file: %s, time: %u\n", mr.fName_, mr.rtime_));

			// Check if the mirror file name is empty
			if(std::string(mr.fName_).empty())
			{
				GCC_TDEBUG((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::load() error: mr.fName_ is empty.\n"));
			}
			else
			{
				map_.insert(make_pair(mr.rtime_, mr));
			}
		}
    }
    GCC_TDEBUG((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::load() file_ OK: %s\n", path_.c_str()));
    GCC_TDEBUG((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::load() %u entries in queue.\n", map_.size()));
}


//******************************************************************************
//
aes_dbo_mirrorqueue::aes_dbo_mirrorqueue():
handle_(ACE_INVALID_HANDLE),
path_(),
map_(),
mapMutex_()
{
}


//******************************************************************************
//
aes_dbo_mirrorqueue::~aes_dbo_mirrorqueue()
{
    // Close
    close();
}


//******************************************************************************
//
bool aes_dbo_mirrorqueue::open(const std::string &path)
{
    GCC_TDEBUG((AES_DBO_Mirrorqueue,"Entering aes_dbo_mirrorqueue::open()\n"));
    // Save path
    path_ = path;

    // Open the file
    handle_ = ACE_OS::open(path.c_str(),O_CREAT|O_RDWR, ACE_DEFAULT_OPEN_PERMS, NULL);

    // Check if the file is OK
    if (handle_ == ACE_INVALID_HANDLE)
    {
        int error = ACE_OS::last_error();
        GCC_TERROR((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::open() Couldn't open: %s, error: %u\n", path_.c_str(), error));
        return false;
    }

    // Load the map from file
    load();

	GCC_TDEBUG((AES_DBO_Mirrorqueue,"Leaving aes_dbo_mirrorqueue::open()\n"));
    return true;
}


//******************************************************************************
//
void aes_dbo_mirrorqueue::close()
{
    // Check if the handle is closed
    if (handle_ != ACE_INVALID_HANDLE)
    {
        GCC_TERROR((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::close() handler for the file path : %s is now closed\n", path_.c_str()));
        // Close the file
		ACE_OS::close(handle_);
        handle_ = ACE_INVALID_HANDLE;
    }
}


//******************************************************************************
//
bool aes_dbo_mirrorqueue::append(const MirrorRecord &record)
{
    GCC_TTRACE((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::append() file: %s, time: %u\n", record.fName_, record.rtime_));
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
    // Insert into map and save it to disk
    map_.insert(make_pair(record.rtime_, record));
    save();
    return true;
}


//******************************************************************************
//
bool aes_dbo_mirrorqueue::change(const MirrorRecord &record)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
	GCC_TTRACE((AES_DBO_Mirrorqueue,"(%t) aes_dbo_mirrorqueue::change() file: %s, time: %u\n", record.fName_, record.rtime_));
	std::pair <std::multimap<unsigned int, MirrorRecord>::iterator, std::multimap<unsigned int, MirrorRecord>::iterator> uint_max_range;
	int count = 0;
	bool result = false;
	// Find the relative infinite-time record and remove it
	uint_max_range = map_.equal_range(UINT_MAX);
	std::multimap<unsigned int, MirrorRecord>::iterator A, B, C;
	A = uint_max_range.first;
	B = uint_max_range.second;
	C = A;
	while(C != B)
	{
		for(C = A; C != B; ++C)
		{
			if(!strcmp((C->second).fName_, record.fName_))
			{
				map_.erase(C);
				uint_max_range = map_.equal_range(UINT_MAX);
				A = uint_max_range.first;
				B = uint_max_range.second;
				count++;
				break;
			}
		}
	}
	if(count)
	{
		AES_GCC_TRACE_MESSAGE("Erased <%i> infinite-time record relative to the file to close!", count);
		// Insert into map and save it to disk
		map_.insert(make_pair(record.rtime_, record));
		save();
		result = true;
	}
	AES_GCC_TRACE_MESSAGE("Leaving with result = %s", result ? "TRUE":"FALSE");
	return result;
}


//******************************************************************************
//
bool aes_dbo_mirrorqueue::head(MirrorRecord &record)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
    // Check if there is any elements
    if (map_.empty())
	{
        return false;
	}

    // Get first element
    record = (*(map_.begin())).second;
    return true;
}


//******************************************************************************
//
void aes_dbo_mirrorqueue::remove()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
    // Remove the first element and save the map
    map_.erase(map_.begin());
    save();
}


//******************************************************************************
//
void aes_dbo_mirrorqueue::clear()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
    // Clear the map
    map_.clear();
}


//******************************************************************************
//
bool aes_dbo_mirrorqueue::empty()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
    // Check if map is empty
    return map_.empty();
}


//******************************************************************************
//
bool aes_dbo_mirrorqueue::searchFileName(const char* fName)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mapMutex_);
	std::multimap<unsigned int, MirrorRecord>::iterator it;
	bool found = false;
    // Check if the record is already inserted in the queue
	for(it=map_.begin(); it!=map_.end(); ++it)
	{
		if(!strcmp((it->second).fName_, fName))
		{
			found = true;
			break;
		}
	}
    return found;
}
