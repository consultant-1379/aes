//******************************************************************************
// 
// .NAME
//      ParameterHandler - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME ParameterHandler
// .HEADER  ACS  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE <filename>

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
//      1999-11-10 by UAB/I/LN  Urban Siderberg

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>

//******************************************************************************
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <ace/Singleton.h>
#include <ace/Synch.h>

#include <string>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */
class AES_DBO_ParameterHandler
{
 private:

    std::string rootDir_;
    std::string cfgRootDir_;
    std::string dboDataRoot_;
    unsigned int streamSize_;

    AES_DBO_ParameterHandler();

    virtual ~AES_DBO_ParameterHandler();

    void setPSAPath();

    void setFileMPath();

    void setDataPath();

    void setRPCStreamSize();

public:

	friend class ACE_Singleton<AES_DBO_ParameterHandler, ACE_Recursive_Thread_Mutex>;

    void fetch();

    const std::string &getRootDir() const;

    const std::string &getConfigRootDir() const;

    const std::string& getDBODataPath() const;

    unsigned int getStreamSize() const;

//        Retrieves the delimiter string.
    static std::string delimiter();
};

typedef ACE_Singleton<AES_DBO_ParameterHandler, ACE_Recursive_Thread_Mutex> ParameterHandler;

#endif
