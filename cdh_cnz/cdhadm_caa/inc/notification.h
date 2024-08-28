//******************************************************************************
// 
// .NAME
//      Notification - <Short description of the class>
// .LIBRARY 3C++
// .PAGENAME Notification
// .HEADER  AES
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE notification.h

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
//  This class is responsible of getting filenotifications from 
//  the database and to send them (via socketconnection class) to
//  a remote host.
//      

// .ERROR HANDLING
//
//      General rule:
//

// DOCUMENT NO
//      19089-CAA 109 0420 

// AUTHOR 
//      2002-02-12 by UAB/KB/AU Heln Ferm

// CHANGES
//		2002-05-23	QABHEHE	Added method convFullPath
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//

// .LINKAGE
//      <link information: libraries needed and order>

// .SEE ALSO 
//      OSF_Task
//

//******************************************************************************


#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <ace/Task.h>
#include <ace/Monotonic_Time_Policy.h>
#include <ace/ACE.h>
#include <string>
#include <aes_cdh_resultcode.h>
#include <socketconnection.h>
#include <database.h>
#include <notificationrecord.h>
#include <aes_cdh_resultimplementation.h>

using namespace std;

typedef enum{
	// No files in database, wait 
	NS_DATABASE_EMPTY		= 0,

	// State is alarm
	NS_ALARM				= 1,

	// Normal state
	NS_NORMAL				= 2

} NotificationState;


//##ModelId=3DE49B400219
class Notification : public ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>
{
    
public:  

	//##ModelId=3DE49B40026A
	Notification(void);

	//##ModelId=3DE49B400274
	~Notification(void);

	//	Starts the thread.
	//	Input: Pointer to transfer queue, Threadmanager.
	//	Output: Returns -1 if failure starting the thread.
	//##ModelId=3DE49B400287
    ACE_INT32 open(ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy> *transQ, ACE_Thread_Manager *thrmgr);

	//	Close the thread. This function is not called, 
	//  it runs automatically when svc() returns with 0.
	//##ModelId=3DE49B400292
	ACE_INT32 close(ACE_UINT64 flags);
	
	//	Main function. Runs while thread is alive.
	//##ModelId=3DE49B4002A5
    ACE_INT32 svc(void);

	//	Defines parameters needed.
	//	Input: string - parameters.
	//	Output: AES_CDH_ResultCode returncode.
	//##ModelId=3DE49B4002B0
    AES_CDH_ResultCode define(const std::string &dest, const std::string &ipnr, 
							  const std::string &portnr, const std::string &respTime, 
							  const std::string &eType, std::string destSet);

	//	Changes defined parameters. 
	//	Input: string - parameters
	//	Output: AES_CDH_ResultCode returncode.
	//##ModelId=3DE49B4002CD
    AES_CDH_ResultCode changeAttr(std::string &notifAddress,
                                  std::string &notifPort,
                                  std::string &respTime,
                                  std::string &evType);

	//  Queue for receiving messages from socket connection
	//##ModelId=3DE49B40030A
	ACE_Message_Queue<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy> sockQueue;
	std::string m_DestSetName;

private:
	
	//	Start SocketConnection thread.
	//	Input: 
	//	Output: Returns -1 if socket connection thread could
	//  not start.
	//##ModelId=3DE49B400327
	ACE_INT32 startSocketTask(void);

	//	Change a string to upper case letters.
	//	Input: string
	//##ModelId=3DE49B40033B
	//void toUpper(string &str);

	//	Temporarily solution to read cluster address from file.
	//	Input: string
	//	Output: Returns true if successful, false otherwise.
	//##ModelId=3DE49B40034F
	bool readIPaddrFromFile(string &IPaddress);

	//	Read first file from database.
	//	Input: string - filename and int - message number
	//	Output: Returns true if a record is read. False if 
	//  database is empty.
	//##ModelId=3DE49B400363
	bool readFile(string &fName, ACE_INT32 &messageNr);

	//  Change the slashes from \ to / in full path. 
	//  Input: string - fullPath
	//##ModelId=3DE49B400377
	void convFullPath(string &fPath);

	//	Cmd cdhch is executed and the parameters needs to be
	//  reset in SocketConnection. The socket is closed down
	//  and restarted.
	//	Input: 
	//	Output: Returns true if the socket connection was
	//  opened OK, false if the socket not could be opened.
	//##ModelId=3DE49B40038C
	bool handle_cdhch(void);
	
	//	Cmd cdhver is executed to find out connection status.
	//  If the socket connection is down, an attempt to 
	//  reconnect is done.
	//	Input: Current state.
	//	Output: Returns true if the socket connection is
	//  OK, false if the socket connection is down..
	//##ModelId=3DE49B400397
	bool handle_cdhver(NotificationState cstate);
	
	//	Cmd cdhver -m is executed to manually send a file notification. 
	//  If the socket connection is down, an attempt to 
	//  reconnect is done. A file notification is sent if
	//  connection is OK. 
	//	Input: Current state.
	//	Output: Returns true if the socket connection is
	//  OK and the file notification is sent ok. Returns false
	//  if socket connection is down and if the file notification 
	//  could not be sent.
	//##ModelId=3DE49B4003AA
	bool handle_cdhverm(NotificationState cstate);

	//	Check if there is a message from transdest in 
	//  notification thread queue.
	//	Input: int - timeout time.
	//	Output: Returns the MessageType received. If no message
	//  is received, it returns MT_EMPTYQUEUE.
	//##ModelId=3DE49B4003BE
	MessageType checkQ(ACE_INT32 timeOut);

	//	Check event type.
	//	Input:
	//	Output: Return true if event type is "event, returns
	//  false if event type is alarm.
	//##ModelId=3DE49B4003D2
	bool eventtype(void);

	//  Returns a result to transdest when a command like 
	//  cdhver or cdhch is executed.
	//  Input: MessageType - what message type to return
	//  Output: 
	//##ModelId=3DE49B4003E6
	void sendCmdResult(MessageType mt);

	//  Returns true if connection fault alarm is true,
	//  otherwise false
	//##ModelId=3DE49B410008
	bool isConnectAlarm(void) const;

	//	Sets connection fault alarm to true or false
	//##ModelId=3DE49B41001C
	void setConnectAlarm(bool set);

	//  Returns true if notification alarm is true
	//  otherwise false
	//##ModelId=3DE49B410030
	bool isNotifyAlarm(void)const;

	//	Sets notification alarm to true or false
	//##ModelId=3DE49B41003A
	void setNotifyAlarm(bool set);

	//  Returns true if recvCdhver flag is true
	//  otherwise false
	//##ModelId=3DE49B41004E
	bool isCdhver(void)const;

	//	Sets recvCdhver flag to true or false
	//##ModelId=3DE49B410062
	void setCdhver(bool set);

	//  Returns true if recvCdhverm flag is true
	//  otherwise false
	//##ModelId=3DE49B410076
	bool isCdhverm(void)const;

	//	Sets recvCdhverm flag to true or false
	//##ModelId=3DE49B41008A
	void setCdhverm(bool set);

	//  Returns true if recvCdhch flag is true
	//  otherwise false
	//##ModelId=3DE49B410094
	bool isCdhch(void)const;

	//	Sets recvCdhch flag to true or false
	//##ModelId=3DE49B4100A8
	void setCdhch(bool set);

	//  Thread of class SocketConnetion that handles the socket
	//  connection and sending and receiving notification messages.
	//##ModelId=3DE49B4100C7
	SocketConnection socketTask;

	//  Pointer to thread queue in transdest
	//##ModelId=3DE49B41010D
	ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy> *transQueue;
	
	// notification message to send to socket connection
	//##ModelId=3DE49B410135
	MessageBlock *sendSockMsg;

	//	IP address to APG40's cluster
	//##ModelId=3DE49B41015D
	string hostAddress;

	//	Complete path to file, not including filename
	//##ModelId=3DE49B410185
	string fullPath;

	//##ModelId=3E6C85BE00B4
	std::string storePath;

	//	Destination name
	//##ModelId=3DE49B4101C1
	string destName;

	//	Remote address
	//##ModelId=3DE49B4101F3
	string IPnr;

	//	Remote port nr
	//##ModelId=3DE49B41022F
	string portNr;

	//	User can choose event_type 'event' or 'alarm', 
	//	alarm is default
	//##ModelId=3DE49B410262
	string eventType;

    //	Socket response time, that is the time waiting 
	//  for an answer from remote.
	//##ModelId=3DE49B41029E
    string responseTime;



	//  To keep track of current state
	//##ModelId=3DE49B41030C
	NotificationState state;

	// This flag is true when a notification fault alarm is set
	//##ModelId=3DE49B410333
	bool notificationAlarm;

	// This flag is true when a connection lost alarm is set
	//##ModelId=3DE49B410365
	bool connectionAlarm;

	// This flag is true when cmd cdhch is executed
	//##ModelId=3DE49B41038D
	bool recvCdhch;

	// This flag is true when cmd cdhver is executed
	//##ModelId=3DE49B4103BF
	bool recvCdhver;

	// This flag is true when cmd cdhver -m is executed
	//##ModelId=3DE49B4103E7
	bool recvCdhverm;

	// This flag is true when a change of attributes demands a reconnection
	//##ModelId=3DE49B420027
	bool reconnect;
	
	//  Last read fileName
	//##ModelId=3DE49B4102DA
	string fileName;

	// Message number for last sent notification message
	//##ModelId=3DE49B420059
	ACE_INT32 msgNr;

};

#endif 
