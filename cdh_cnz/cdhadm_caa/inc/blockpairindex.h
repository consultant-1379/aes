//******************************************************************************
// 
// .NAME
//  	BlockPairIndex
// .LIBRARY 3C++
// .PAGENAME BlockPairMapper
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE blockpairindex.h

// .COPYRIGHT
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

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	19089-CAA 109 1302

// AUTHOR 
// 	2005-06-18 by UAB/KB/AU  Hans-Erik Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//  

// .LINKAGE
//	<link information: libraries needed and order>

// .SEE ALSO
// 	<Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************


#ifndef BLOCKPAIRINDEX_H
#define BLOCKPAIRINDEX_H

#include <string>

class BlockPairIndex
{
private:
    
    static const std::string indexFileName_;

    struct indexData_
    {
        char destination_[32];
        char streamId_[256];
        char fileName_[256];
    };

    struct indexHeader_
    {
        int entries_;
    };

    ACE_HANDLE hFile;

    std::string destName_;
    std::string filePath_;

public:
    BlockPairIndex(const std::string &destName);
    
    virtual ~BlockPairIndex();


    bool open();

    bool close();
    

    // Locates the streamId in the index-file.
    bool findStreamId(const std::string &destination,
                      const std::string &streamId,
                      std::string &fileName,
                      std::string &fullFileName);


    bool append(const std::string &destination, const std::string &streamId, std::string &fullFileName);

    static bool remove(const std::string &destination);

    static bool init(const std::string &destination);

    bool exportFileNames(std::vector<std::string> &vec);

};

#endif
