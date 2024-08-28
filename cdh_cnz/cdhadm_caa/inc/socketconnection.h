//******************************************************************************
//
// NAME
//      socketconnection.h
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2000.
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
//  This thread is responsible for socket connection to remote host
//  handling sending notification messages and receiving
//  ACK / NAK

// DOCUMENT NO
//      19089-CAA 109 0420

// AUTHOR 
//      2002-01-11 UAB/S/AU qabhefe Helen Ferm

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//  PA1     020111  qabhefe 
//                          
//                          

// SEE ALSO 
//      notification.h OSF_Task.h
//  
//******************************************************************************
#ifndef SOCKETCONNECTION_H
#define SOCKETCONNECTION_H

#include <notificationprotocol.h>
#include <messageblock.h> 
#include <ace/Task.h>
#include <string>
#include <ace/ACE.h>
#include <ace/Event.h>

using namespace std;

//##ModelId=3DE49B3E0049
class SocketConnection : public ACE_Task<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy>
{

  public:
	// Sets the necessary parameters at start up.
	// Input: string - parameters
	// Output:
	//##ModelId=3DE49B3E0109
	void setParameters(	string& hAddr,
				string& ipaddr,
				string& pNr,
				string& fPath,
				string& rPath);

	
	// Constructor
	//##ModelId=3DE49B3E00C4
	SocketConnection(void);
	
	// Destructor
	//##ModelId=3DE49B3E00CD
	virtual ~SocketConnection(void);

	// Starts socket thread
	// Input: pointer to notification queue, thread manager
	// Output: returns -1 if thread could not start.
	//##ModelId=3DE49B3E00E0
	ACE_INT32 open( ACE_Message_Queue<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy> *fileNotifQueue,
			ACE_Thread_Manager* thrMgr = 0);

	// Closes the thread
	//##ModelId=3DE49B3E00EB
	ACE_INT32 close(ACE_UINT64 flags=0);

	// Main method, runs while thread is alive
	//##ModelId=3DE49B3E00F5
	ACE_INT32 svc(void);
	
	// Sets the response time waiting for an answer from the application.
	// Input: new value.
	// Output: void. The command checks that the value is within range. 
	//##ModelId=3DE49B3E00FF
	void setRespTime(ACE_INT32 &sek);

	// Connect socket to remote host.
	// Input: 
	// Output: Returns true if connection was successful, otherwise false. 
	//##ModelId=3DE49B3E011D
	bool connect2socket(void);
	
	// Close the socket connection.
	// Input: 
	// Output: Returns true if the connection was gracefully closed, 
	// otherwise false.
	//##ModelId=3DE49B3E0126
	bool closeConnection(void);
	
	// Socket handle. If notification class receives HANGUP when send/receive is ongoing, 
	// the handle needs to be closed in order to terminate properly.
	//##ModelId=3DE49B3E013B
	//SOCKET hostSocket;
	int hostSocket; // written temporarily for SOCKET(need to analyse more)??


	//##ModelId=3DE49B3E014E
	bool isConnected(ACE_INT32 timeout);

private:
	void toUpper(string &strl);
	bool  m_isConnected;
	
	// Converts from 
	// Input: string - reference to the string to be converted.
	// Output: 
	//##ModelId=3DE49B3E0159

	// Pointer to queue in Notification
	//##ModelId=3DE49B3E01D2
	ACE_Message_Queue<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy> *notifQueue;

	// Parameters.
	//##ModelId=3DE49B3E01F9
	string hostAddress;
	//##ModelId=3DE49B3E0221
	string relPath;
	//##ModelId=3DE49B3E0249
	string fullPath;
	//##ModelId=3DE49B3E027B
	string IPaddress;
	//##ModelId=3DE49B3E02AD
	string portNr;

	// Time to wait for an answer from remote host. Default value is 10 sec.
	//##ModelId=3DE49B3E02D4
	ACE_INT32 respTime;

	// To handle send/receive protocols
	//##ModelId=3DE49B3E02FD
	NotificationProtocol sendProtocol;
	//##ModelId=3DE49B3E0330
	NotificationProtocol recvProtocol;
};

#endif
