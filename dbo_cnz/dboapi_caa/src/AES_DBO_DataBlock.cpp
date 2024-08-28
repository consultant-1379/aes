
//******************************************************************************
// 
// .NAME
//  	AES_DBO_DataBlock - A containter for use with the stream
// .LIBRARY 3C++
// .PAGENAME AES_DBO_DataBlock
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_DataBlock.h

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
// 	The datablock is used with the stream and manages the
//  data encapsulation.

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

#include <aes_gcc_log.h>
#include "AES_DBO_DataBlock.h"

const unsigned int AES_DBO_DataBlock::hLength_ = sizeof(AES_DBO_DataBlock);

// ACS Trace definition
GCC_TDEF(aes_dbo_datablock);

//******************************************************************************
//
AES_DBO_DataBlock::AES_DBO_DataBlock(): length_(0), blockNr_(0)
{
    GCC_TRACE((aes_dbo_datablock,
        "(%t) AES_DBO_DataBlock::AES_DBO_DataBlock()\n"));
}

//******************************************************************************
//
AES_DBO_DataBlock::~AES_DBO_DataBlock()
{
    GCC_TRACE((aes_dbo_datablock,
        "(%t) AES_DBO_DataBlock::~AES_DBO_DataBlock()\n"));
}

//******************************************************************************
//
char *AES_DBO_DataBlock::getData() const
{
    GCC_TRACE((aes_dbo_datablock,
        "(%t) AES_DBO_DataBlock::getData()\n"));

    // Return the address to this object + the header
    char *ptr = (char *)this;
    return ptr + hLength_;
}

//******************************************************************************
//
void AES_DBO_DataBlock::operator delete(void *ptr)
{
    GCC_TRACE((aes_dbo_datablock,
        "(%t) AES_DBO_DataBlock::operator delete()\n"));

    delete [] (char *)ptr;
    ptr=NULL;
}

//******************************************************************************
//
AES_DBO_DataBlock *AES_DBO_DataBlock::create(unsigned int dataLen)
{
    GCC_TRACE((aes_dbo_datablock,
        "(%t) AES_DBO_DataBlock::create() - 1 param\n"));

    int blockLen = dataLen + hLength_;
    char *ptr = new (std::nothrow) char[blockLen];
	if( ptr != NULL)
	{
		unsigned int &hLen = (unsigned int &)*ptr;
		hLen = dataLen;
	}
    return (AES_DBO_DataBlock *)ptr;
}

//******************************************************************************
//
AES_DBO_DataBlock *AES_DBO_DataBlock::create(unsigned int nr, const char* buf,
                                             unsigned int dataLen)
{
    GCC_TRACE((aes_dbo_datablock,
        "(%t) AES_DBO_DataBlock::create() - 3 params\n"));

    int blockLen = dataLen + hLength_;
    char *ptr = new(std::nothrow) char[blockLen];
	if( ptr != NULL) 
	{
		memset( ptr, 0, sizeof(char[blockLen]));
		unsigned int &hLen = (unsigned int &)*ptr;
		unsigned int &hNr = (unsigned int &)*(ptr + 4);
		hLen = dataLen;
		hNr = nr;
		memcpy(ptr + hLength_, buf, dataLen);
	}
    return (AES_DBO_DataBlock *)ptr;
}
