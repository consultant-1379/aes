
//*******************************************************************
// 
// .NAME
//  	CmdClient - Handles communication with AES_DBO_Server
// .LIBRARY 3C++
// .PAGENAME CmdClient
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE CmdClient.h

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
// 	Handles communication with AES_DBO_Server using DSD.

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 1300

// AUTHOR 
// 	2002-05-24 by UAB/UKB/AU Ulf Gustafsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R1A_DMDN6.lib

// .SEE ALSO 
// 	AES_Cmd_Client

//*******************************************************************
/*=====================================================================
						DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef CMDCLIENT_H 
#define CMDCLIENT_H

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <ACS_APGCC_Command.H>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */

class ACS_APGCC_DSD_Stream;
class ACS_APGCC_DSD_Connector;
class ACS_APGCC_DSD_Addr;

/*=====================================================================
					CLASS DECLARATION SECTION
==================================================================== */

class  CmdClient
{    
 public:
    enum
    {
        COK,
        CERROR,
        CTIMEOUT
    };

    CmdClient(const char* serviceName);

    virtual ~CmdClient();

    bool connect();

    bool send(ACS_APGCC_Command& cmd);

    unsigned int receive(ACS_APGCC_Command& cmd, unsigned long timeout = 0);

    ACE_HANDLE getHandle();

    void close();

    bool shutDown();

private:

    ACS_APGCC_DSD_Addr* m_address;
    ACS_APGCC_DSD_Stream    *stream_;
    ACS_APGCC_DSD_Connector *server_;

    /**
       	@brief	m_ConnectionHandle : handle of dsd stream
    */
    ACE_HANDLE m_ConnectionHandle;

    /**
    	@brief	m_StopEvent : signal to internal thread to exit
    */
    int m_StopEvent;
};
 
#endif
