
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

#ifndef AES_DBO_DATABLOCK_H
#define AES_DBO_DATABLOCK_H

class AES_DBO_DataBlock
{
public:
    static const unsigned int hLength_;
    const unsigned int length_;
    const unsigned int blockNr_;
    /**
         * @brief
         * destructor 
         */
    ~AES_DBO_DataBlock();
    char *getData() const;
    void operator delete(void *ptr);
    /**
         *  create method: This method is responsible for crearting a data block.
         *  @param  dataLen          : unsigned int 
         */
    static AES_DBO_DataBlock *create(unsigned int dataLen);
     /**
         *  create method: This method is responsible for crearting a data block.
         *  @param  nr          : unsigned int
         *  @param  buf          : const char pointer
         *  @param  dataLen          : unsigned int 
         */
    static AES_DBO_DataBlock *create(unsigned int nr, const char *buf,
    		unsigned int dataLen);

private:
   /**
         * @brief
         * constructor 
         */
   AES_DBO_DataBlock();
};

#endif
