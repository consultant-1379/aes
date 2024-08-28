//******************************************************************************
// 
// .NAME
//  	BlockPairMapper
// .LIBRARY 3C++
// .PAGENAME BlockPairMapper
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE blockpairmapper.h

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
// 	2005-05-21 by UAB/KB/AU  Hans-Erik Nilsson

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

#ifndef BLOCKPAIRMAPPER_H
#define BLOCKPAIRMAPPER_H


#include <string>
#include <ace/ACE.h>
#include <ace/Mutex.h>

const ACE_UINT32 APPLID_MAX = 4000000000;

class BlockPairIndex;

class BlockPairMapper
{
private:

    ACE_HANDLE hFile;
    
    // pointer to the beginning of the mapped memory
    void *mapAddr_;

    // name of the file in the physical file system
    std::string fileName_;

    std::string tempFileName_;       //For TR HH72841

    // name of the destination set to which this mapper belongs
    std::string destSetName_;

    // The name of this object
    std::string objName_;

    std::string RPCName_ ;	// HH72841
    
    // true if there is no instance of BlockPairIndex
    bool bIndexError_;
    
    // true if files are open and need to be closed
    bool needClose_;

    // constructs the file-mapping object name
    bool makeObjectName();

    // constructs the file name
    void makeFileName();

    bool setChanged(const std::string &fileName);

    void setCommitted();

    // Pointer to the index
    BlockPairIndex *bpi_;

public:
    
    struct PairId
    {
        unsigned int cdrbid_;
        unsigned int  applid_;
        unsigned int nextApplid_; // appl id corresponding CDRBid after commited

        // This is a flag which is set to 0 (zero) if the file has never been
        // used in a commit-operation. Once the first commit-operation is
        // performed on the file, this value is never 0 (zero)
        unsigned int committed_;

        char RPCName [40] ;
    };

    // holds the last committed number pair
    struct PairId pairId;

    // constructor
    BlockPairMapper(const std::string &destSetName);

    // destructor
    virtual ~BlockPairMapper();
    
    //opens/creates the file
    bool open(const std::string &fName, const std::string &RPCName);

    // closes the file
    bool close();

    // checks if the file is open
    bool isOpen() const;

    // updates the file with the specified number pair
    void commit(const PairId &id);

    void commitChange(const PairId &id, const std::string &RPCName);

    // retrieves the last committed number pair.
    void getLastCommitted(PairId &id);

    // retrieves the last committed application block-id
    unsigned int getApplBlockId();

    // calculates the corresponding application block-id based
    // on the CDRBid passed by value and the offset
    unsigned int getApplBlockId(const unsigned int CDRBid);

    // set a appl block-id correspondig to the block after
    // the committed block and sets an unvalid committed 
    // application block-id
    void setNextApplBlockId(unsigned int id, unsigned int  prevCDRBid);

    // retrieves the last committed CDRBid
    unsigned int getCDRBid();

    //Sets a new CDRBid and RPCName.
    //applid is set to APPLIX_MAX + 1
    void setCDRBid(unsigned int cdrbid, const std::string &RPCName);
    
    //Sets a new CDRBid and RPCName
    void setCDRBidOnly(unsigned int cdrbid, const std::string &RPCName);
    
    // checks if the file has been subject to a commit-operation
    bool validFile() const;

    static bool remove(const std::string &destination);

    static bool init(const std::string &destination);

    bool setAllChanged();

    void makeRPCName(std::string &RPCName);
    
    unsigned int getFileStatus(const std::string &RPCName) ;

    void readFromTempFile (unsigned int &cdrbid , const std::string& RPCName );	 //HH72841
};

#endif
