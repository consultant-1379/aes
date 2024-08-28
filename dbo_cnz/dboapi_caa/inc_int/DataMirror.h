
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
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef DATAMIRROR_H
#define DATAMIRROR_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include <ace/ACE.h>
/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */

class AES_DBO_AsynchComm;
class ACE_Process_Mutex;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class DataMirror 
{
  private:

	bool multiCPsystem;
    bool allOK_;

    ACE_HANDLE data_;

    std::string m_TQname;
    std::string m_CurrentDataFile;
    std::string tqPath_;
	std::string streamId_;
	std::string m_MirrorStateFile;
	std::string m_ProcessMutexName;

    uint32_t* curFile_;
    uint32_t* curBlock_;
    uint32_t* dataWritten_;
    uint32_t* commitDone_;
    off64_t curBytes_;
    void* stateP_;

    AES_DBO_AsynchComm* comm_;

    ACE_Process_Mutex* stateMutex_;


    void setCurrentFileName();

    bool loadState();

    ACE_HANDLE openMirrorStateFile(bool& fileCreated);

    unsigned int switchFile();

    unsigned int reportToServer(unsigned int cmdCode);

public:
	DataMirror(AES_DBO_AsynchComm *comm, const std::string &tq, const std::string &streamId);
    ~DataMirror();

    void clear();
	unsigned int write(const unsigned int blockNr, const char *buf, const unsigned int len);
    void commitIsDone();
};

#endif
