//******************************************************************************
//
// NAME
//      socketconnection.cpp
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
//  A thread responsible for socket connection to remote host
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
//      
// 
//******************************************************************************

#include <socketconnection.h>
#include <messagetype.h>
#include <messageblock.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ace/OS_NS_sys_select.h>
#include <ace/Monotonic_Time_Policy.h>
#include <servr.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
#include "aes_cdh_common.h"

using namespace std;

// ACS Trace definition
AES_CDH_TRACE_DEFINE(AES_CDH_SocketConnection);

//  ---------------------------------------------
//		Constructor()
//  ---------------------------------------------
//##ModelId=3DE49B3E00C4
SocketConnection::SocketConnection()
:notifQueue(0),
 hostAddress(""),
 relPath(""),
 fullPath(""),
 IPaddress(""),
 portNr("")
{
	hostSocket = -1;
	m_isConnected = false;
	respTime=10;
}

//  ---------------------------------------------
//		Destructor()
//  ---------------------------------------------
//##ModelId=3DE49B3E00CD
SocketConnection::~SocketConnection()
{
	m_isConnected = false;
}

//  ---------------------------------------------
//		open()
//  ---------------------------------------------
//##ModelId=3DE49B3E00E0
int SocketConnection::open(ACE_Message_Queue<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy> *fileNotifQueue, ACE_Thread_Manager *thrMgr)
{
	this->thr_mgr(thrMgr);
	notifQueue = fileNotifQueue;

	int result = this->activate(THR_NEW_LWP|THR_DETACHED);

	if (result == -1)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Socket connection thread error, result %d", result);
		AES_CDH_TRACE_MESSAGE("Socket connection thread error, result %d", result);
		return result;
	}

	AES_CDH_TRACE_MESSAGE("SOCKCONN:open::activating thread, OK");
	AES_CDH_LOG(LOG_LEVEL_INFO, "SocketConnection activating thread succesfully");
	return result; 
}

//  ---------------------------------------------
//		close()
//  ---------------------------------------------
//##ModelId=3DE49B3E00EB
int SocketConnection::close(ACE_UINT64 flags)
{
	(void) flags;
	return 0;	

}

//  ---------------------------------------------
//		svc()
//  ---------------------------------------------
//##ModelId=3DE49B3E00F5
int SocketConnection::svc(void)
{
	ACE_Message_Block *recvNotifMess; // receive
	MessageBlock *recvNotifMsg;

	MessageBlock *sendNotifMsg; // send

	int queue_result(0);
	int recv_result(0);
	int send_result(0);
	int select_result(0);
	bool checkProtocol(false);
	bool read_again(false);

	string fileName;
	string fileSize;
	string msgNr;

	// select 
	fd_set fds;
	struct timeval timeout;

	for(;;)
	{

		timeout.tv_sec = respTime;
		timeout.tv_usec = 0;

		AES_CDH_TRACE_MESSAGE("Waiting for order from queue");

		queue_result = getq(recvNotifMess);

		if (queue_result != -1)
		{

			recvNotifMsg = (MessageBlock*) recvNotifMess;

			// terminate
			if (recvNotifMsg->msgType() == MT_HANGUP)
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "MT_HANGUP received");
				AES_CDH_TRACE_MESSAGE("MT_HANGUP received");
				closeConnection();
				recvNotifMess->release();
				return 0;
			}

			// else MT_DATA
			// add filename, filesize, messagenr to protocol
			string sendString("");

			fileName = recvNotifMsg->getFileName();
			fileSize = recvNotifMsg->getFileSize();
			msgNr = recvNotifMsg->getMsgNr();

			AES_CDH_TRACE_MESSAGE("Order received: Store file name = %s, fileSize = %s, Message number = %s", fileName.c_str(), fileSize.c_str(), msgNr.c_str());

			sendProtocol.change("STORE_FILE_NAME", fileName);
			sendProtocol.change("FILE_SIZE", fileSize);
			sendProtocol.change("MESSAGE_NUMBER", msgNr);

			sendProtocol.encode(sendString);

			char buf [10000];
			unsigned char sendBuf[10000];
			unsigned short int len(0);

			len = sprintf(buf, "%s\\0", sendString.c_str());

			AES_CDH_TRACE_MESSAGE("Sent message:%s", buf);

			// first 2 bytes is message size
			sendBuf[0] = len / 256;
			sendBuf[1] = len % 256;
			sendBuf[2] = '\0';

			strcat((char *)&sendBuf[2], buf);
			//commented due to core occurence
			//FD_ZERO(&fds);
			//FD_SET(hostSocket, &fds);
			// send notification message
			send_result = send(hostSocket, (const char *)sendBuf, len+2, 0);
			AES_CDH_TRACE_MESSAGE("Send result = %d",send_result);

			if (send_result == -1)
			{
				recvNotifMess->release();
				recvNotifMsg = NULL;
				sendNotifMsg = new MessageBlock(MT_SENDERROR);
				notifQueue->enqueue_tail(sendNotifMsg);

				AES_CDH_LOG(LOG_LEVEL_ERROR, "FAILED to send message: SOCKET_ERROR received");
				AES_CDH_TRACE_MESSAGE("FAILED to send message: SOCKET_ERROR received");

				continue; // start from beginning of for(;;)
			}

			else if( send_result == len+2 )	//Message sent successfully
			{
				AES_CDH_TRACE_MESSAGE("SocketConnection::svc(), Message sent OK");

				do
				{
					FD_ZERO(&fds);
					FD_SET(hostSocket, &fds);
					FD_SET(ServR::pipeHandles[0],&fds);
					int max_fd = (hostSocket > ServR::pipeHandles[0])?hostSocket:ServR::pipeHandles[0];
					AES_CDH_TRACE_MESSAGE("Waiting for reply from remote receiver...");
					// wait for answer from remote host
					select_result = select(max_fd+1, &fds, NULL, NULL, &timeout); 
					AES_CDH_TRACE_MESSAGE("Return value from select = %d", select_result);

					if(FD_ISSET(ServR::pipeHandles[0],&fds ) == true)
					{
						AES_CDH_LOG(LOG_LEVEL_INFO, "In select, stop event signalled ");
						sendNotifMsg = new MessageBlock(MT_STOPNOTIFY);
						notifQueue->enqueue_head(sendNotifMsg);
						AES_CDH_TRACE_MESSAGE("select, STOPNOTIFY ");
						return 0;
					}

					switch(select_result)
					{
					case 0: //timeout
					{
						AES_CDH_TRACE_MESSAGE("In Select, timeout expired");
						if (!read_again)
						{
							recvNotifMess->release();
							recvNotifMsg = NULL;
						}
						read_again = false;
						sendNotifMsg = new MessageBlock(MT_TIMEOUT);
						notifQueue->enqueue_tail(sendNotifMsg);

						AES_CDH_TRACE_MESSAGE("No reply from remote receiver! (timeout)");
					}
					break;

					case -1:
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "select result is SOCKET_ERROR");
						if (!read_again)
						{
							recvNotifMess->release();
							recvNotifMsg = NULL;
						}
						read_again = false;
						sendNotifMsg = new MessageBlock(MT_SOCKERROR);
						notifQueue->enqueue_tail(sendNotifMsg);

						AES_CDH_TRACE_MESSAGE("SOCKET_ERROR received from socket!");
					}
					break;

					default: // message is sent ok, timeout has not expired, no SOCKET_ERROR from select
					{
						unsigned char recvBuf[65792] = { 0 };
						if ( FD_ISSET(hostSocket, &fds ) == true )
						{
							AES_CDH_TRACE_MESSAGE("send is OK, now read");

							// read 2 bytes with size of message
							recv_result = recv(hostSocket, (char *)recvBuf, 2, 0);
							recvBuf[recv_result] = '\0';
						}

						int lastError = ACE_OS::last_error();
						AES_CDH_TRACE_MESSAGE("Returned from recv call result = %d lastError == %d", recv_result, lastError);
						if(recv_result == -1 || recv_result == 0)
						{
							int lerror = ACE_OS::last_error();
							if( lerror == ECONNRESET )
							{
								m_isConnected =  false;
							}

							AES_CDH_LOG(LOG_LEVEL_ERROR, "recv error");
							AES_CDH_TRACE_MESSAGE("recv error");
							if (!read_again)
							{
								recvNotifMess->release();
								recvNotifMsg = NULL;
							}
							read_again = false;

							sendNotifMsg = new MessageBlock(MT_RECVERROR);
							if(recv_result == 0)
							{
								sendNotifMsg->setResetFlag(true); //HV74524
							}
							notifQueue->enqueue_tail(sendNotifMsg);

							AES_CDH_TRACE_MESSAGE("Reply received from recv(): %d", recv_result);
						}
						else
						{
							len = 256 * recvBuf[0] + recvBuf[1];

							select_result = select(max_fd+1, &fds, NULL, NULL, &timeout);
							if (select_result <= 0 || (FD_ISSET(ServR::pipeHandles[0],&fds ) == true ))
							{
								if (! read_again)
								{
									recvNotifMess->release();
									recvNotifMsg = NULL;
								}

								read_again = false;
								if (select_result == 0)
								{
									sendNotifMsg = new MessageBlock(MT_TIMEOUT);
								}
								else if (select_result == -1)
								{
									sendNotifMsg = new MessageBlock(MT_SOCKERROR);
								}

								notifQueue->enqueue_tail(sendNotifMsg);

								AES_CDH_TRACE_MESSAGE("Result = %d received from select() #2", select_result);
							}
							else
							{
								// catch message
								recv_result = recv(hostSocket, (char *)recvBuf, len, 0);
								if( recv_result == 0 )
								{
									int lerror = ACE_OS::last_error();
									if( lerror == ECONNRESET )
									{
										m_isConnected =  false;
									}
								}

								recvBuf[recv_result] = '\0';

								int lastError = ACE_OS::last_error();
								AES_CDH_TRACE_MESSAGE("Returned from recv() #2 returned %d lastError == %d", recv_result, lastError);

								AES_CDH_TRACE_MESSAGE("received message %s", recvBuf);

								AES_CDH_TRACE_MESSAGE("Reply received from remote receiver: %s", recvBuf);

								if(len != recv_result)
								{
									if (!read_again)
									{
										recvNotifMess->release();
										recvNotifMsg = NULL;
									}
									read_again = true;

									AES_CDH_LOG(LOG_LEVEL_INFO, "Unexpected length of reply message, was %d, expected %d", recv_result, len);
									AES_CDH_TRACE_MESSAGE("Unexpected length of reply message, was %d, expected %d", recv_result, len);
								}

								string recvMessage((char *)recvBuf);
								//toUpper(recvMessage);
								if (recvMessage != "")
									AES_CDH_TRACE_MESSAGE("recvMsg = %s", recvMessage.c_str());

								recvProtocol.decode(recvMessage);
								checkProtocol = sendProtocol.compare(recvProtocol);

								if(!checkProtocol)
								{
									AES_CDH_LOG(LOG_LEVEL_INFO, "Reply verification failed!");

									if (!read_again)
									{
										recvNotifMess->release();
										recvNotifMsg = NULL;
									}
									read_again = true;
								}
								else
								{
									if (recvProtocol.ackReceived("ACK"))
									{
										AES_CDH_TRACE_MESSAGE("ACK received!");

										if (!read_again)
										{
											recvNotifMess->release();
											recvNotifMsg = NULL;
										}
										read_again = false;
										sendNotifMsg = new MessageBlock(MT_ACK);
										notifQueue->enqueue_tail(sendNotifMsg);
									}
									else if (recvProtocol.nakReceived("NAK"))
									{
										AES_CDH_TRACE_MESSAGE("NAK received!");

										AES_CDH_LOG(LOG_LEVEL_INFO, "NAK received!");
										if (!read_again)
										{
											recvNotifMess->release();
											recvNotifMsg = NULL;
										}
										read_again = false;
										sendNotifMsg = new MessageBlock(MT_NAK);
										notifQueue->enqueue_tail(sendNotifMsg);
									}
									else
									{
										AES_CDH_LOG(LOG_LEVEL_INFO, "ACK or NAK was NOT received!");
										AES_CDH_TRACE_MESSAGE("ACK or NAK was NOT received!");

										if (!read_again)
										{
											recvNotifMess->release();
											recvNotifMsg = NULL;
										}
										read_again = true;
									}
								}
							}
						} // else, read was OK

					}
					break;

					}
				} while(read_again);
			} 		
		}
	}

	return 0;
}

//  ---------------------------------------------
//		connect2socket()
//  ---------------------------------------------
//##ModelId=3DE49B3E011D
bool SocketConnection::connect2socket(void)
{
	bool resSockClose = closeConnection();		// fix for bug during DATABASE_EMTPY case, where connectionVerify action is opening new socket without closing existing one
	if(!resSockClose)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "closing socket connection failed inside connect2socket()!!!");
		AES_CDH_TRACE_MESSAGE("closing socket connection failed inside connect2socket()!!!");
	}
	unsigned int nPort(atoi(portNr.c_str()));
	int result(0);
	struct sockaddr* server = NULL;
	bool isIPv6 = AES_CDH_Common_Util::isIPv6Address(IPaddress);
	size_t addr_size;
	if(isIPv6)
	{
		AES_CDH_TRACE_MESSAGE("Creating IPv6 SOCKET ADDRESS STRUCT");
		struct in6_addr ipv6_addr;
		int res = inet_pton(AF_INET6,IPaddress.c_str(),&ipv6_addr);
		if(res != 1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "inet_pton() FAILED(res == %d)! Converting IPv6[%s] string to struct in6_addr FAILED!!!", res, IPaddress.c_str());
			return AES_CDH_RC_CONNECTERR;
		}
		struct sockaddr_in6* server_ipv6 = new struct sockaddr_in6;
		ACE_OS::memset(server_ipv6,'0',sizeof(struct sockaddr_in6));
		server_ipv6->sin6_family = AF_INET6;
		server_ipv6->sin6_addr = ipv6_addr;
		server_ipv6->sin6_port = htons(nPort);
		server = (struct sockaddr*)server_ipv6;
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Creating IPv4 SOCKET ADDRESS STRUCT");
		struct in_addr ipv4_addr;
		int res = inet_pton(AF_INET,IPaddress.c_str(),&ipv4_addr);
		if(res != 1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "inet_pton() FAILED(res == %d)! Converting IPv4[%s] string to struct in_addr FAILED!!!", res, IPaddress.c_str());
			return AES_CDH_RC_CONNECTERR;
		}
		struct sockaddr_in* server_ipv4 = new struct sockaddr_in;
		ACE_OS::memset(server_ipv4,'0',sizeof(struct sockaddr_in));
		server_ipv4->sin_family = AF_INET;
		server_ipv4->sin_addr = ipv4_addr;
		server_ipv4->sin_port = htons(nPort);
		server = (struct sockaddr*)server_ipv4;
	}

	if(isIPv6)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Creating IPv6 SOCKET ");
		hostSocket = socket(AF_INET6, SOCK_STREAM, 0);
		addr_size = sizeof(struct sockaddr_in6);
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Creating IPv4 SOCKET ");
		hostSocket = socket(AF_INET, SOCK_STREAM, 0);
		addr_size = sizeof(struct sockaddr_in);
	}

	if (hostSocket == -1)
	{
		int lastError = ACE_OS::last_error();
		perror("test");
		AES_CDH_LOG(LOG_LEVEL_ERROR, "socket() returned INVALID_SOCKET lastError == %d", lastError);
		AES_CDH_TRACE_MESSAGE("socket() returned INVALID_SOCKET lastError == %d", lastError);
		if(server) //HZ26672
			delete server;		

		return false;
	}

	// Getting the socket properties before setting the socket non blocking
	int arg1 = fcntl( hostSocket, F_GETFL, NULL );

	// Setting the socket non blocking
	result = fcntl( hostSocket, F_SETFL, O_NONBLOCK);

	result = connect(hostSocket, server, addr_size);

	int lastError = ACE_OS::last_error();
	AES_CDH_TRACE_MESSAGE("After connect() = %d lastError == %d", result, lastError);

	if (result != 0)
	{
		AES_CDH_TRACE_MESSAGE("Inside (result != 0) branch");
		fd_set writefdSet;
		fd_set readset;
		FD_ZERO( &writefdSet);
		FD_ZERO( &readset );
		FD_SET( hostSocket, &writefdSet );
		FD_SET( ServR::pipeHandles[0] ,&readset);
		int max_fd = (hostSocket >  ServR::pipeHandles[0])?hostSocket:ServR::pipeHandles[0];

		ACE_Time_Value tv(2,0);
		// Waiting for 2 secs for reply
		int result1 = ACE_OS::select( max_fd+1, &readset, &writefdSet, 0, &tv );

		if( result1 <=  0 || (FD_ISSET(ServR::pipeHandles[0],&readset)== true))     //TIMEOUT
		{
			int lastError = ACE_OS::last_error();
			AES_CDH_LOG(LOG_LEVEL_ERROR, "connect() returned %d lastError == %d", result, lastError);
			AES_CDH_TRACE_MESSAGE("connect() returned %d lastError == %d", result, lastError);
			ACE_OS::closesocket( hostSocket);
			hostSocket = -1;
			m_isConnected = false;
			if(server) //HZ26672
				delete server;

			return false;
		}
		else if( result1 == 1 )
		{
			AES_CDH_TRACE_MESSAGE("Inside (result1 == 1) branch");
			int so_error;
			socklen_t len = sizeof so_error;

			getsockopt(hostSocket, SOL_SOCKET, SO_ERROR, &so_error, &len);

			if (so_error == 0)
			{
				AES_CDH_TRACE_MESSAGE("Connection established successfully");
				// Setting the sock of blocking type
				fcntl( hostSocket, F_SETFL, arg1);
			}
			else
			{
				m_isConnected = false ;
				ACE_OS::closesocket( hostSocket);
				hostSocket = -1;
				if(server) //HZ26672
					delete server;

				return false;
			}
			AES_CDH_TRACE_MESSAGE("sConnected = true, so_error = %d", so_error);
			m_isConnected = true ;
		}
	}
	m_isConnected = true;
	if(server) //HZ26672
		delete server;
	AES_CDH_TRACE_MESSAGE("Socket Connection is OK");
	return true;
}

//  ---------------------------------------------
//		closeConnection()
//  ---------------------------------------------
//##ModelId=3DE49B3E0126
bool SocketConnection::closeConnection()
{
	if (hostSocket == -1)
		return true;
	int returnValue (0);

	returnValue = ACE_OS::closesocket(hostSocket);

	if (returnValue == 0)
	{
		AES_CDH_TRACE_MESSAGE("closeConnection() success");
		m_isConnected = false;
		hostSocket = -1;
		return true;
	}

	else
	{
		AES_CDH_TRACE_MESSAGE("Returning false, returnValue == %d", returnValue);
		return false;
	}

}

//  ---------------------------------------------
//		toUpper()
//  ---------------------------------------------
//##ModelId=3DE49B3E0159
void SocketConnection::toUpper(string& strl)
{
	unsigned int i = 0;

	while(i < strl.length())
	{
		strl[i] = toupper(strl[i]);
		i++;
	}
}

//  ---------------------------------------------
//		setParameters()
//  ---------------------------------------------
//##ModelId=3DE49B3E0109
void SocketConnection::setParameters(string& hAddr, string& ipaddr, string& pNr, string& fPath, string& rPath)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	sendProtocol.clearPrt();
	hostAddress = hAddr;
	IPaddress = ipaddr;

	//HI55918 		
	bool bIsIPaddress = true;	
	string strTempIPaddress = IPaddress.c_str();	
	//Check whether it is an IP address or hostname
	for ( basic_string <char>::iterator str_Iter = strTempIPaddress.begin( ); str_Iter != strTempIPaddress.end( ); ++str_Iter )
	{
		if ( (*str_Iter < 48 || *str_Iter > 57) && ( *str_Iter != 46 ) )
		{
			bIsIPaddress = false;
			break;
		}      
	}

	//If it is a hostname, find the IP address corresponding to the hostname from DNS server or hosts file,
	//and replace the hostname with its IP address.
	if ( !bIsIPaddress ) 
	{	
		bool IPfoundInDNS = true;

		struct hostent *remoteHost = 0;
		struct in_addr addr;
		remoteHost = gethostbyname(IPaddress.c_str() );
		if( remoteHost == NULL )
		{
			herror("gethostbyname failed");
			IPfoundInDNS = false;
		}
		else
		{	// IP address found in DNS
			addr = *((in_addr *) remoteHost->h_addr);
			IPaddress = inet_ntoa(addr);
		}

		if ( !IPfoundInDNS ) // IP address NOT found in DNS
		{
			// Get IP address from hosts file
			string textline = "";
			string strTempHostName = "";
			string HostnameFromHostsFile = "";
			string IPaddFromHostsFile = "";    	

			ifstream infile("/etc/hosts");    
			if (infile)
			{   
				while (getline(infile,textline))
				{	
					basic_string <char>::size_type indexTabFind = 0;
					basic_string <char>::size_type indexChFirst = 0;
					basic_string <char>::size_type indexChLast = 0;
					static const basic_string <char>::size_type npos = -1;


					// find TAB position
					indexTabFind = textline.find_first_of("\t");
					if ( indexTabFind != npos )
					{
						int HostNameStartPos = 0;
						int HostNameEndPos = 0;
						IPaddFromHostsFile = textline.substr(0,indexTabFind);
						strTempHostName = textline.substr(indexTabFind+1);
						HostNameEndPos = strTempHostName.length();

						indexChFirst = strTempHostName.find_first_not_of(" ",0);
						indexChLast = strTempHostName.find_last_not_of(" ");
						if ( indexChFirst != npos )
							HostNameStartPos = indexChFirst;

						if ( indexChLast != npos )
							HostNameEndPos = indexChLast;

						HostnameFromHostsFile = strTempHostName.substr(HostNameStartPos, HostNameEndPos-HostNameStartPos+1);

						if ( strcmp(IPaddress.c_str(),HostnameFromHostsFile.c_str()) == 0 ) //working
						{
							IPaddress = IPaddFromHostsFile;
							break;
						}
					} // Tab find end       
				} // while loop end
			} // infile end
			if (infile.is_open())
			{
				infile.close();
			}
		} // IPfoundInDNS end
	} // bIsIPaddress end
	//HI55918

	portNr = pNr;
	fullPath = fPath;
	relPath	= rPath;

	// construct the send protocol
	sendProtocol.add("HOST_NAME", hostAddress);
	sendProtocol.add("FILE_STORE_PATH", relPath);
	sendProtocol.add("FILE_FULL_PATH", fullPath);
	sendProtocol.add("STORE_FILE_NAME", "");
	sendProtocol.add("FILE_SIZE","");
	sendProtocol.add("MESSAGE_NUMBER", "");
}

//  ---------------------------------------------
//		setRespTime()
//  ---------------------------------------------
//##ModelId=3DE49B3E00FF
void SocketConnection::setRespTime(ACE_INT32 &sek)
{
	//AES_CDH_LOG(LOG_LEVEL_DEBUG, "%s", "SOCKCONN:setRespTime = %d\n", sek));
	respTime = sek;
}

//  ---------------------------------------------
//		isConnected()
//  ---------------------------------------------
//##ModelId=3DE49B3E014E
bool SocketConnection::isConnected(ACE_INT32 timeout)
{
	//sleep(timeout);
	if( ServR::isStopEventSignalled == false )
	{
		ACE_Time_Value tv(timeout,0);
		ServR::StopEvent->wait(&tv, 0);
	}
	return  m_isConnected;
}
