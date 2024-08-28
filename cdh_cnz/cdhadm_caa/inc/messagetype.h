//******************************************************************************
// 
// .NAME
//      messagetype.h 
//      
// .LIBRARY 3C++
// .PAGENAME MessageType
// .HEADER  FMS
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE messagetype.h

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
//  This class handles message types, returnvalues.
//  The source code is adapted for WinNT only. 

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//      19089-CAA 109 0420 

// AUTHOR 
//      2002-02-13 by QABHEFE Helen Ferm

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//
//         

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      
//  MessageBlock 

//******************************************************************************

#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H


// returnvalues used between socketconnection and notification

typedef enum {
	// Terminate
	MT_HANGUP		= 0,

	// Send returns error
	MT_SENDERROR	= 1,

	// Select returns socket error
	MT_SOCKERROR	= 2,

	// Recv returns error
	MT_RECVERROR	= 3,
	
	// Select returns timeout
	MT_TIMEOUT		= 4,

	// Received message with "ack"
	MT_ACK			= 5,

	// Received message with "nak"
	MT_NAK			= 6,

	// Received message has protocol error
	MT_RECVPRTERROR	= 7,

	// Message to be sent has protocol error
	MT_SENDPRTERROR	= 8,

	// Message contains data
	MT_DATA			= 9,

        // Check connection manually
        MT_CHECKCONNECTION_MANUALLY = 10,

        // Check connection
        MT_CHECKCONNECTION          = 11,

        // Change attributes
        MT_CHANGEATTR               = 12,

        // General Ok
        MT_OK                       = 13,

        // General error
        MT_ERROR                    = 14,

        // Connect ok
        MT_CONNECTOK                = 15,

        // Connect error
        MT_CONNECTERR               = 16,
		
	// No message received at queue
	MT_EMPTYQUEUE				= 17,
	MT_STOPNOTIFY			= 18
		

	} MessageType;

#endif

