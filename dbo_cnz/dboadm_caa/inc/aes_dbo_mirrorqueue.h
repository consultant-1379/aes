//******************************************************************************
//
// NAME
//      MirrorQueue.h
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
#ifndef AES_DBO_MIRRORQUEUE_H
#define AES_DBO_MIRRORQUEUE_H
#include <ace/ACE.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <string>
#include <map>
#include <algorithm>


// The structure which the file consists of
class MirrorRecord
{
public:
    unsigned long rtime_;       // The time for file removal
    char fName_[100];            // A mirror filename
};

class aes_dbo_mirrorqueue
{
private:
    ACE_HANDLE handle_;
    std::string path_;
    std::multimap<unsigned int, MirrorRecord> map_;
    ACE_Recursive_Thread_Mutex mapMutex_;
    /**
         *  save method: This method is used to save a mirror record.
         */
    void save();
    /**
         *  load method: This method is used to load a mirror record.
         */
    void load();

public:
    /**
         * @brief
         * constructor 
         */
    aes_dbo_mirrorqueue();
    /**
         * @brief
         * destructor
         */
    ~aes_dbo_mirrorqueue();
    /**
         *  open  method: This method is used to open a mirror queue.
         *  @param path : const string 
         */
    bool open(const std::string &path);
    /**
         * close method: This method is used to close a mirror queue.
         */
    void close();
    /**
         *   append  method: This method is used to append to a  mirror record.
         *  @param record: const MirrorRecord
         */
    bool append(const MirrorRecord &record);
    /**
         *   change  method: This method is used to change a  mirror record.
         *  @param record: const MirrorRecord
         */
    bool change(const MirrorRecord &record);
    /**
         *  head  method: This method is used to get the first element in the  mirror record.
         *  @param record: MirrorRecord
         */
    bool head(MirrorRecord &record);
    /**
         *  remove method: This method is used to remove the first element in the  mirror record.
         */
    void remove();
    /**
         * clear method: This method is used to clear the map.
         */
    void clear();
    /**
         * empty method: This method is used to check if map is empty.
         */
    bool empty();
    /**
     	 * searchFileName method: This method is used to verify if a file name is already present in the queue.
     	 */
    bool searchFileName(const char* fName);
};
#endif
