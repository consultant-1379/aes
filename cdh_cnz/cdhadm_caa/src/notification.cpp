//******************************************************************************
//
// NAME
//      notification.cpp
//
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
//  This class handle sending files with ftp initiating and
//  responding. The source code is adapted for WinNT only. 

// DOCUMENT NO
//      19089-CAA 109 0420

// AUTHOR 
//      2002-02-12 by UAB/KB/AU Heln Ferm

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//                          

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
// 
//******************************************************************************

#include <notification.h>
#include <messageblock.h>
#include <messagetype.h>
#include <alarmhandler.h>
#include <cdhcriticalsection.h>
#include <destinationalarm.h>
#include <ace/ACE.h>
#include <parameter.h>
#include <aes_gcc_util.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_Notification);

//  ---------------------------------------------
//		Constructor()
//  ---------------------------------------------
Notification::Notification()
: m_DestSetName(""),  
  transQueue(0),
  sendSockMsg(0),
  notificationAlarm(false),
  connectionAlarm(false),
  recvCdhch(false),
  recvCdhver(false),
  recvCdhverm(false),
  reconnect(false),
  fileName(""),
  msgNr(1)
{
	state = NS_NORMAL;
	
}

//  ---------------------------------------------
//		Destructor()
//  ---------------------------------------------
Notification::~Notification()
{
}

//  ---------------------------------------------
//		open()
//  ---------------------------------------------
ACE_INT32 Notification::open(ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy> *transQ, ACE_Thread_Manager *thrmgr)
{
	transQueue = transQ;
	AES_CDH_LOG(LOG_LEVEL_INFO, "Activating notification thread ");
	this->thr_mgr(thrmgr);
	ACE_INT32 result = this->activate(THR_NEW_LWP|THR_DETACHED);
	if (result == -1)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR,  "ERROR activating notification thread");
		AES_CDH_TRACE_MESSAGE("ERROR activating notification thread");
		return result;
	}
	// start socket thread
	if (startSocketTask() == -1)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR activating scocket task");
		AES_CDH_TRACE_MESSAGE("ERROR activating scocket task");
		socketTask.closeConnection();
		result = -1;
	}
	AES_CDH_LOG(LOG_LEVEL_INFO, "Notification thread spawned successfully");
	AES_CDH_TRACE_MESSAGE("activating thread, OK");
	return result;
}
//  ---------------------------------------------
//		close()
//  ---------------------------------------------
ACE_INT32 Notification::close(ACE_UINT64 flags)
{
	(void)flags;
	AES_CDH_TRACE_MESSAGE("Entering");
	return 0;	
}
//  ---------------------------------------------
//		svc()
//  ---------------------------------------------
ACE_INT32 Notification::svc(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	// received message from socket connection
	ACE_Message_Block *recvSockMessage = 0;
	MessageBlock *recvSockMsg = 0;
	ACE_INT32 sock_Qresult(0);
	ACE_INT32 resendCounter(1);
	bool socketCheckResult(true);
	bool resend(false);
	bool wait(false);
	bool timeOutFlag(false);
	bool notifyAlarmFlag(false);		//For TR HL19120
	bool l_connect(true);          //For TR HW11717
	bool rstFlag = false;          //For TR HV74524
	// start with state NS_NORMAL
	state = NS_NORMAL;
	// timeout used in select
	struct timeval timeout;
	timeout.tv_sec = 2;
	timeout.tv_usec = 0;
	// timeout used with thread queue
	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> queueTimeOut(this->gettimeofday());
	queueTimeOut += ACE_Time_Value (2,0); // Now + 2 sec
	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	string emptyDestSet("");
	emptyDestSet = m_DestSetName;
	// Connect To Socket
	bool retval = socketTask.connect2socket();
	if (retval == false)  //start fnst not running, cdh SIGPIPE
	{
		AES_CDH_TRACE_MESSAGE("No Socket connection at start");
		// No socket connection, alarm raised
		socketTask.closeConnection();
		setConnectAlarm(true);
		if (eventtype() == true)
		{
			// only as event
			AlarmHandler::Instance()->raiseConnectionAlarm(destName, true);
		}
		else
		{
			// raise alarm
			alarmCause = DestinationAlarm::AlarmRecord::connect_error_fn;
			// raise alarm with new cause
			DestinationAlarm::instance()->raise(destName + "_filenotification", emptyDestSet, alarmCause);
		}
		state = NS_ALARM;
		AES_CDH_TRACE_MESSAGE("state = NS_ALARM");
		resend = false;
	}                    //end fnst not running, cdh SIGPIPE
	for(;;) // ever...
	{
		do
		{
			switch (state)
			{

			case NS_DATABASE_EMPTY:
			{
				// check every 2 seconds if there is a new notification to send
				// also check that socket connection lives
				socketCheckResult = socketTask.isConnected(timeout.tv_sec);
				if (socketCheckResult == false)
				{
					// socket connection is lost, raise alarm
					AES_CDH_TRACE_MESSAGE("Notification::svc() socket connection lost state NS_ALARM");
					socketTask.closeConnection();
					setConnectAlarm(true);
					if (eventtype() == true)
					{
						// only as event
						AlarmHandler::Instance()->raiseConnectionAlarm(destName, true);
					}
					else
					{
						// raise alarm
						alarmCause = DestinationAlarm::AlarmRecord::connect_error_fn;
						// raise alarm with new cause
						DestinationAlarm::instance()->raise(destName + "_filenotification", emptyDestSet, alarmCause);
					}
					state = NS_ALARM;
				}
				else if (socketCheckResult == true)
				{
					//AES_CDH_LOG(LOG_LEVEL_WARN, "%s","(%t) Notification::svc() socket connection successful \n"));
					// check Q if a message is put to thread queue
					MessageType qResult = checkQ(0);

					if (qResult != MT_EMPTYQUEUE)
					{
						if (qResult == MT_HANGUP)
						{
							AES_CDH_TRACE_MESSAGE("qResult = MT_HANGUP");
							return 0;
						}
						else if (isCdhch())
						{
							if(!handle_cdhch())
							{
								AES_CDH_TRACE_MESSAGE("state = NS_ALARM");
								state = NS_ALARM;
							}
						}
						else if (isCdhver())
						{
							//Fix for TR_HL19125 start
							if(!handle_cdhver(NS_DATABASE_EMPTY))
							{
								socketTask.closeConnection();
								setConnectAlarm(true);
								state = NS_ALARM;
							}//Fix for TR_HL19125 end
						}
						else if (isCdhverm())
						{
							handle_cdhverm(NS_DATABASE_EMPTY);
						}
					}
				}
				if (state == NS_DATABASE_EMPTY)
				{
					// finally, check if there is a message in the database
					// else state is still NS_DATABASE_EMPTY
					bool empty;
					CDHCriticalSection::instance().enter();
					empty = DataBase::instance()->isEmpty(destName);
					CDHCriticalSection::instance().leave();

					if (!empty)
					{
						state = NS_NORMAL;
					}
				}
			}
			break;
			case NS_ALARM:
			{
				// try to reconnect every 4 minutes
				AES_CDH_TRACE_MESSAGE("Notification::svc() try to reconnect every 4 minutes");
				if (isConnectAlarm())
				{
					for (ACE_INT32 i = 0; i < 80; i++)
					{
						// check if a message is put to thread queue
						// wait 3 sec
						MessageType qResult = checkQ(3);
						if (qResult != MT_EMPTYQUEUE)
						{
							if (qResult == MT_HANGUP)
							{
								return 0;
							}
							else if (isCdhch())
							{
								if(handle_cdhch()) // connection alarm false
								{
									state = NS_NORMAL;
									break;
								}
							}
							else if (isCdhver())
							{
								if (handle_cdhver(NS_ALARM))
								{
								//	state = NS_NORMAL;  // HX18084
									break;
								}
							}
							else if (isCdhverm())
							{
								if (handle_cdhverm(NS_ALARM))
								{
									state = NS_NORMAL;
									break;
								}
							}
						}
					}
					// 4 minutes has passed, try to connect
					if (state == NS_ALARM)
					{
						if (socketTask.connect2socket())
						{
							if (eventtype())
							{
								AlarmHandler::Instance()->ceaseConnectionAlarm(destName);
							}
							else
							{
								alarmCause = DestinationAlarm::AlarmRecord::ok;
								DestinationAlarm::instance()->cease(destName + "_filenotification", emptyDestSet, alarmCause);
							}
							setConnectAlarm(false);
							state = NS_NORMAL;
                                                        l_connect = true;
						}
					}
				}

				if (isNotifyAlarm() && !isConnectAlarm())
				{
					if (state == NS_ALARM)
					{
						AES_CDH_TRACE_MESSAGE("state = NS_ALARM");
						//TR HW11717
						if(l_connect == false)
						{
							bool retval = socketTask.connect2socket();
							if (retval == false)  //start fnst not running, cdh SIGPIPE
							{
								AES_CDH_TRACE_MESSAGE("connect2socket failed");
								AES_CDH_LOG(LOG_LEVEL_ERROR, "connect2socket failed");
								// No socket connection, alarm raised
								socketTask.closeConnection();
								setConnectAlarm(true);
								if (eventtype() == true)
								{
									// only as event
									AlarmHandler::Instance()->raiseConnectionAlarm(destName, true);
								}
								else
								{
									// raise alarm
									alarmCause = DestinationAlarm::AlarmRecord::connect_error_fn;
									DestinationAlarm::instance()->raise(destName + "_filenotification", emptyDestSet, alarmCause);
								}
								state = NS_ALARM;
								AES_CDH_TRACE_MESSAGE("state = NS_ALARM");
							}
							else
							{ 
								l_connect = true;
								state = NS_NORMAL;
							}
						//TR HW11717
						}
						else
						{

						MessageType qResult = checkQ(2);
						if (qResult != MT_EMPTYQUEUE)
						{
							if (qResult == MT_HANGUP)
							{
								AES_CDH_TRACE_MESSAGE("qResult != MT_HANGUP");
								return 0;
							}
							else if (isCdhch())
							{
								if(handle_cdhch()) // connection alarm false
								{
									AES_CDH_TRACE_MESSAGE("state = NS_NORMAL");
									state = NS_NORMAL; // try to resend
								}
							}
							else if (isCdhver())
							{
								AES_CDH_TRACE_MESSAGE("isCdhver() TRUE");
								handle_cdhver(NS_ALARM);
							}
							else if(isCdhverm())
							{
								AES_CDH_TRACE_MESSAGE("isCdhverm() TRUE");
								handle_cdhverm(NS_ALARM);
								state = NS_NORMAL;
							}
						}
						AES_CDH_TRACE_MESSAGE("qResult = MT_EMPTYQUEUE");
					        }
					}
				}
			}
			break;

			default:
			{
				AES_CDH_TRACE_MESSAGE("switch(state) default");
				break;
			}

			} // switch

		} while (state != NS_NORMAL);
		AES_CDH_TRACE_MESSAGE("outside  while (state != NS_NORMAL)");
		while (state == NS_NORMAL)
		{
			resend = false;
			timeOutFlag = false;
			wait = false;
			msgNr = 1;
                        l_connect = true;
			resendCounter = 1;
			notifyAlarmFlag = false;	//For TR HL19120
			do
			{
				// read notification message from database
				if (!readFile(fileName, msgNr))
				{
					//database is empty
					if (isCdhverm())
					{
						AES_CDH_TRACE_MESSAGE("isCdhverm() TRUE");
						sendCmdResult(MT_CONNECTOK);
						setCdhverm(false);
					}
					AES_CDH_TRACE_MESSAGE("state = NS_DATABASE_EMPTY");
					state = NS_DATABASE_EMPTY;
					resend = false;
				}
				// Check thread Q if there is a message 
				if (state == NS_NORMAL && isCdhverm() == false)
				{
					MessageType qResult = checkQ(0);
					if (qResult != MT_EMPTYQUEUE || isCdhch())
					{
						if (qResult == MT_HANGUP)
						{
							AES_CDH_TRACE_MESSAGE("qResult == MT_HANGUP");
							return 0;
						}	
						else if (isCdhch())
						{
							AES_CDH_TRACE_MESSAGE("isCdhch() TRUE");
							if(!handle_cdhch())
							{
								AES_CDH_TRACE_MESSAGE("handle_cdhch() FALSE");
								state = NS_ALARM;
								resend = false;
							}
						}
						else if (isCdhver())
						{
							AES_CDH_TRACE_MESSAGE("isCdhver() TRUE");
							handle_cdhver(NS_NORMAL);
						}
						else if (isCdhverm())
						{
							AES_CDH_TRACE_MESSAGE("isCdhverm() TRUE");
							handle_cdhverm(NS_NORMAL);
						}
					}
				}

				if (state == NS_NORMAL)
				{
					AES_CDH_TRACE_MESSAGE("put notification message to queue");
					// send message that is read in readFile()
					socketTask.putq(sendSockMsg);
					do
					{
						queueTimeOut = this->gettimeofday();
						queueTimeOut += ACE_Time_Value (2,0); // Now + 2 sec
						sock_Qresult = sockQueue.dequeue_head(recvSockMessage, &queueTimeOut);
						if(sock_Qresult == -1) // no answer for 2 sec's
						{
							if (isCdhverm())
							{
								sendCmdResult(MT_CONNECTOK);
								setCdhverm(false);
							}
							MessageType qResult = checkQ(0);
							if ( qResult != MT_EMPTYQUEUE)
							{
								if (qResult == MT_HANGUP)
								{
									ACE_OS::closesocket(socketTask.hostSocket);

									// wait for socketTask to terminate
									queueTimeOut = this->gettimeofday();
									queueTimeOut += ACE_Time_Value (1,0); // Now + 2 sec
									ACE_Time_Value queueTimeOut1(1, 0);

									(this->thr_mgr())->wait(&queueTimeOut1);
									return 0;
								}
								if (isCdhch() && reconnect == false)
								{
									handle_cdhch();
								}			
								if (isCdhver())
								{
									handle_cdhver(NS_NORMAL);
								}	
								if (isCdhverm())
								{
									sendCmdResult(MT_CONNECTOK);
									setCdhverm(false);
								}
							}

							wait = true;
						}
						else 
						{
							recvSockMsg = (MessageBlock *)recvSockMessage;
		
							MessageType mt = recvSockMsg->msgType();
				
							if(mt != MT_RECVERROR)
                                                                rstFlag = false;

							if (isCdhverm())
							{
								if (mt == MT_SENDERROR || mt == MT_SOCKERROR || mt == MT_RECVERROR)
								{
									AES_CDH_TRACE_MESSAGE("(mt == MT_SENDERROR || mt == MT_SOCKERROR || mt == MT_RECVERROR");
									sendCmdResult(MT_CONNECTERR);
								}
								else
								{
									AES_CDH_TRACE_MESSAGE("MT_CONNECTOK");
									sendCmdResult(MT_CONNECTOK);
								}
								setCdhverm(false);
							}

							wait = false;
							AES_CDH_TRACE_MESSAGE("wait = FALSE");
						}

					}while (wait);
					AES_CDH_TRACE_MESSAGE("Outside while (wait)");

					// handle answer
					switch(recvSockMsg->msgType())
					{
					case MT_STOPNOTIFY:

					{
						AES_CDH_TRACE_MESSAGE( "MT_STOPNOTIFY received");

						socketTask.closeConnection();

						resend = false;
					} //STOP SIGNALLED

					break;

					case MT_SENDERROR:
					{
						AES_CDH_TRACE_MESSAGE("MT_SENDERROR received");
					} // fall through

					case MT_SOCKERROR:
					{
						AES_CDH_TRACE_MESSAGE("MT_SOCKERROR received");
					} // fall through

					case MT_RECVERROR:
					{
						AES_CDH_TRACE_MESSAGE("MT_RECVERROR received");
                                                bool setAlarm = false;
                                                socketTask.closeConnection();

                                                if (recvSockMsg->msgType() == MT_RECVERROR && recvSockMsg->getResetFlag() && !rstFlag)
                                                {
                                                        if (!socketTask.connect2socket())
                                                        {
                                                                setAlarm = true;
                                                        }
                                                        // set the flag to true to not create a new socket for consecutive RST ERROR
                                                        rstFlag = true;
                                                }
                                                else
                                                {
                                                        setAlarm = true;
                                                }

                                                if(setAlarm)
                                                {
                                                        setConnectAlarm(true);
                                                        if (eventtype() == true)
                                                        {
                                                                // only as event
                                                                AlarmHandler::Instance()->raiseConnectionAlarm(destName, true);
                                                        }
                                                        else
                                                        {
                                                                // raise alarm
                                                                alarmCause = DestinationAlarm::AlarmRecord::connect_error_fn;
                                                                // raise alarm with new cause
                                                                DestinationAlarm::instance()->raise(destName + "_filenotification", emptyDestSet, alarmCause);
                                                        }
                                                        state = NS_ALARM;
                                                        AES_CDH_TRACE_MESSAGE("state = NS_ALARM");
                                                }
                                                resend = false;
					}
					break;

					case MT_TIMEOUT:
					{
						AES_CDH_TRACE_MESSAGE("MT_TIMEOUT received, message number = %d", msgNr);
                                                AES_CDH_LOG(LOG_LEVEL_ERROR, "MT_TIMEOUT received");
						notifyAlarmFlag = true;		//For TR HK85790
						if (msgNr >= 10)
						{
							AES_CDH_TRACE_MESSAGE( "msgNr >= 10");
							/*TR HW11717*/
							if(l_connect == false)
							{
								socketTask.closeConnection();
								state = NS_ALARM;
								resend = false;
								AES_CDH_LOG(LOG_LEVEL_ERROR, "l_connect false closeConnection and connect");
							}/*TR HW11717*/
							else
							{

							resend = true;

							if (timeOutFlag == false)
							{
								timeOutFlag = true;
								// raise notification alarm
								setNotifyAlarm(true);
								AlarmHandler::Instance()->raiseNotificationAlarm(fileName, destName);
							}

							if (timeOutFlag == true)
							{
								// wait 4 minutes between resending
								// chop time up, check if socket connection is lost
								// and if there is a message at queue
								// timeout.tv_sec is set to 2 sec.
								for (ACE_INT32 i=0; i<120; i++)
								{
									socketCheckResult = socketTask.isConnected(timeout.tv_sec);
									if (socketCheckResult == false)
									{
										// socket connection is lost, raise alarm
										socketTask.closeConnection();
										setConnectAlarm(true);
										if (eventtype() == true)
										{
											// only as event
											AlarmHandler::Instance()->raiseConnectionAlarm(destName, true);
										}
										else
										{
											// raise alarm
											alarmCause = DestinationAlarm::AlarmRecord::connect_error_fn;
											// raise alarm with new cause
											DestinationAlarm::instance()->raise(destName + "_filenotification", emptyDestSet, alarmCause);
										}
										state = NS_ALARM;
										resend = false;
										break;
									}

									else if (socketCheckResult == true)
									{
										// check Q if a message is put to thread queue
										MessageType qResult = checkQ(0);

										if (qResult == MT_HANGUP)
										{
											AES_CDH_TRACE_MESSAGE("qResult == MT_HANGUP");
											return 0;
										}
										else if (isCdhverm())
										{
											break; // break loop, try to resend
										}
										else if (isCdhver())
										{
											handle_cdhver(NS_NORMAL);
										}
										else if (isCdhch())
										{
											if (handle_cdhch())
											{
												break;
											}
											else
											{
												state = NS_ALARM;
												resend = false;
												break;
											}
										}
									}
								} //for
								msgNr++;
								l_connect = false;
							}
							}
						}
						else
						{
							msgNr++;
							AES_CDH_TRACE_MESSAGE("MsgNr=%d, resend=true", msgNr);
							resend = true;
						}
					}
					break;
					case MT_NAK:
					{
						AES_CDH_TRACE_MESSAGE("MT_NAK received, resend counter = %d", resendCounter);
						if (resendCounter == 10)
						{
							// raise notification alarm
							if(!isNotifyAlarm()) // alarm
							{
								setNotifyAlarm(true);
								AlarmHandler::Instance()->raiseNotificationAlarm(fileName, destName);
							}
							socketTask.closeConnection(); // HW79012
							state = NS_ALARM;
							resend = false;
							l_connect = false; // HW79012
						}
						else if (resendCounter < 10)
						{
							resendCounter++;
							resend = true;
						}
					}
					break;
					case MT_ACK:
					{
						AES_CDH_TRACE_MESSAGE("MT_ACK received");
						timeOutFlag = false;
						notifyAlarmFlag = false;		//For TR HL19120
						if (isNotifyAlarm())
						{
							AlarmHandler::Instance()->ceaseNotificationAlarm(fileName, destName);
							setNotifyAlarm(false);
						}

						// remove from database
						CDHCriticalSection::instance().enter();
						//bool ret = 
						DataBase::instance()->removeFirst(destName);
						CDHCriticalSection::instance().leave();

						state = NS_NORMAL;
						resend = false;
                                                
					}
					break;
					default:
						AES_CDH_TRACE_MESSAGE("default message type received");
						break;

					} // switch
					// release memory of received message block
					recvSockMessage->release();
					recvSockMsg = NULL;

					if (isNotifyAlarm() && !timeOutFlag && !notifyAlarmFlag)	//For TR HL19120
					{
						AES_CDH_TRACE_MESSAGE("isNotifyAlarm() && !timeOutFlag && !notifyAlarmFlag,  state = NS_ALARM");
						state = NS_ALARM;
						resend = false;
					}
				} // if

			} while (resend);
			AES_CDH_TRACE_MESSAGE("Outside while(resend)");

		} // while state == NS_NORMAL
		AES_CDH_TRACE_MESSAGE("Outside while(state == NS_NORMAL)");
	} // for
	AES_CDH_TRACE_MESSAGE("Outside Outer for, End svc()***");
	return 0;
}
//  ---------------------------------------------
//		define()
//  ---------------------------------------------
AES_CDH_ResultCode 
Notification::define(const std::string &dest, const std::string &ipnr, const std::string &portnr,
					 const std::string &respTime, const std::string &eType, std::string destSet)
{
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	AES_CDH_TRACE_MESSAGE("Entering with dest = %s",dest.c_str());
	m_DestSetName = destSet;
	destName = dest;
	IPnr = ipnr;
	portNr = portnr;
	if (respTime.empty())
	{
		int defaultRespTime = 10;
		responseTime = "10";
		socketTask.setRespTime(defaultRespTime);
	}
	else
	{
		responseTime.assign(respTime); // string
		int rTime = atoi(responseTime.c_str());
		socketTask.setRespTime(rTime);
	}
	if (eType.empty())
	{
		string defaultEventType("alarm");
		eventType = defaultEventType;
	}
	else
	{
		eventType = eType;
	}
	CDHCriticalSection::instance().enter();
	DataBase::instance()->getFileFullPath(destName, fullPath);
	DataBase::instance()->getFileStorePath(destName, storePath);
	CDHCriticalSection::instance().leave();
	convFullPath(fullPath);
	convFullPath(storePath);
	string rpath("");
	string spath("");
	AES_GCC_Util::datapath_trn(fullPath,AES_DATA_PATH,rpath);
	AES_GCC_Util::datapath_trn(storePath,AES_DATA_PATH,spath);
	if(readIPaddrFromFile(hostAddress))
	{
		AES_CDH_TRACE_MESSAGE("NOTIFICATION::IpAddr = %s",IPnr.c_str());
		socketTask.setParameters(hostAddress, IPnr, portNr, rpath, spath);
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("NOTIFICATION::IPnr Not found");
		rCode = AES_CDH_RC_INTPROGERR;
	}
	return rCode;
}
//  ---------------------------------------------
//		changeAttr()
//  ---------------------------------------------
AES_CDH_ResultCode 
Notification::changeAttr(std::string &notifAddress,
                         std::string &notifPort,
                         std::string &respTime,
                         std::string &evType)
{
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	AES_CDH_TRACE_MESSAGE("notifAddress = %s", notifAddress.c_str());
	AES_CDH_TRACE_MESSAGE("notifPort = %s", notifPort.c_str());
	AES_CDH_TRACE_MESSAGE("respTime = %s", respTime.c_str());
	AES_CDH_TRACE_MESSAGE("evType = %s", evType.c_str());
	bool b = true;
	if (b == true)
	{
		if (! notifAddress.empty())
		{
			if (notifAddress != IPnr)
				reconnect = true;

			IPnr.assign(notifAddress);
		}
		if (! notifPort.empty())
		{
			if (notifPort != portNr)
				reconnect = true;

			portNr.assign(notifPort);
		}
		if (! respTime.empty())
		{
			responseTime.assign(respTime);
		}
		if (! evType.empty())
		{
			eventType.assign(evType);

		}
	}
	return rCode;
}
//  ---------------------------------------------
//		toUpper()
//  ---------------------------------------------
/*
void
Notification::toUpper(string& str)
{
	ACE_INT32 len = str.length();
	for (ACE_INT32 i = 0; i <= len-1; i++)
	{
		str[i] = ::toupper(str[i]);
	}
}
*/

//  ---------------------------------------------
//		readIPaddrFromFile()
//  ---------------------------------------------
bool Notification::readIPaddrFromFile(string &IPaddress)
{
	CDHCriticalSection::instance().enter();
	AES_CDH_TRACE_MESSAGE("Entering with destname = %s",destName.c_str());
	DataBase::instance()->getHostName(destName, IPaddress);
	CDHCriticalSection::instance().leave();
	return true;
}
//  ---------------------------------------------
//		readFile()
//  ---------------------------------------------
bool Notification::readFile(string &fName, ACE_INT32 &messageNr)
{
	AES_CDH_TRACE_MESSAGE(" Entering");
	//check if there is a message in the database
	bool empty;
	CDHCriticalSection::instance().enter();
	empty = DataBase::instance()->isEmpty(destName);
	CDHCriticalSection::instance().leave();
	if(empty)
	{
		AES_CDH_LOG(LOG_LEVEL_TRACE, " NOTIFICATION::readFile DataBase::instance() is empty for destination %s", destName.c_str());
		return false;
	}
	else
	{
		AES_CDH_TRACE_MESSAGE(" DataBase::instance() is not empty for destination %s", destName.c_str());
		NotificationRecord *notifRec = new NotificationRecord;
		CDHCriticalSection::instance().enter();
		if(DataBase::instance()->getFirst(destName, notifRec))
		{
			AES_CDH_TRACE_MESSAGE("Able to fecth the first file ");
		}
		CDHCriticalSection::instance().leave();
		string fSize("");
		fName = notifRec->getStoreFileName();
		fSize = notifRec->getFileSize();
		delete notifRec;
		notifRec = 0;
		char buf[2];
		string msgNumber(ACE_OS::itoa(messageNr, buf, 10));
		sendSockMsg = new MessageBlock(MT_DATA);
		sendSockMsg->setFileName(fName);
		sendSockMsg->setFileSize(fSize);
		sendSockMsg->setMsgNr(msgNumber);

		AES_CDH_TRACE_MESSAGE("successful for file %s", fName.c_str());
		AES_CDH_TRACE_MESSAGE("Leaving");
		return true;
	}

}
//  ---------------------------------------------
//		checkQ()
//  ---------------------------------------------
MessageType Notification::checkQ(ACE_INT32 timeOut)
{
	//AES_CDH_LOG(LOG_LEVEL_TRACE, "%s","Entering Notification::checkQ"));

	ACE_Message_Block *recvMessage = 0;
	MessageBlock *recvMsg = 0;

	MessageBlock *sendMessage = 0;

	ACE_Time_Value_T<ACE_Monotonic_Time_Policy> timeOutTime(this->gettimeofday());
	timeOutTime += ACE_Time_Value (timeOut,0); // Now + timeOut sec

	int queue_result(0);

	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	string emptyDestSet("");

	emptyDestSet = m_DestSetName;

	queue_result = this->getq(recvMessage, &timeOutTime); 


	if (queue_result != -1) // something is received on thread queue
	{

		recvMsg = (MessageBlock *)recvMessage;

		AES_CDH_TRACE_MESSAGE("msgType = %d",recvMsg->msgType());

		switch(recvMsg->msgType())
		{
		case MT_HANGUP:
		{
			// cease alarm if true
			if (isNotifyAlarm())
			{
				AlarmHandler::Instance()->ceaseNotificationAlarm(fileName, destName);
			}

			if (isConnectAlarm())
			{
				if (eventtype())
				{
					AlarmHandler::Instance()->ceaseConnectionAlarm(destName);
				}
				else
				{
					alarmCause = DestinationAlarm::AlarmRecord::ok;
					DestinationAlarm::instance()->cease(destName + "_filenotification", emptyDestSet, alarmCause);
				}
			}


			recvMessage->release();
			recvMsg = NULL;

			// send forward to socket connection
			sendMessage = new MessageBlock(MT_HANGUP);


			(socketTask.msg_queue())->enqueue_head(sendMessage);

			// wait for socketTask to terminate
			timeOutTime = this->gettimeofday();
			timeOutTime += ACE_Time_Value (1,0); // Now + 1 sec

			ACE_Time_Value timeOutTime1(1, 0);

			(this->thr_mgr())->wait(&timeOutTime1, false, true);

			AES_CDH_TRACE_MESSAGE("NOTIFICATION:checkQ::MT_HANGUP !");
			return MT_HANGUP;
		}
		break;

		case MT_CHECKCONNECTION_MANUALLY:
		{

			setCdhverm(true);
			recvMessage->release();
			recvMsg = NULL;

			AES_CDH_TRACE_MESSAGE("NOTIFICATION:checkQ::MT_CHECKCONNECTION_MANUALLY !");
			return MT_CHECKCONNECTION_MANUALLY;
		}
		break;

		case MT_CHECKCONNECTION:
		{
			setCdhver(true);
			recvMessage->release();
			recvMsg = NULL;
			AES_CDH_TRACE_MESSAGE("NOTIFICATION:checkQ::MT_CHECKCONNECTION!");
			return MT_CHECKCONNECTION;
		}
		break;

		case MT_CHANGEATTR:
		{
			AES_CDH_TRACE_MESSAGE("NOTIFICATION:checkQ::MT_CHANGEATTR!");
			setCdhch(true);
			recvMessage->release();
			recvMsg = NULL;
			return MT_CHANGEATTR;
		}
		break;

		default:
		{
			AES_CDH_TRACE_MESSAGE("NOTIFICATION:checkQ::default message type!");
			recvMessage->release();
			recvMsg = NULL;
		}
		break;

		} // switch

		// should never be reached
		AES_CDH_TRACE_MESSAGE("MT_EMPTYQUEUE returned !!message received");
		return MT_EMPTYQUEUE;

	} // message received

	else
	{
		//AES_CDH_LOG(LOG_LEVEL_TRACE, "%s","Leaving Notification::checkQ, MT_EMPTYQUEUE returned !! message not received."));
		return MT_EMPTYQUEUE;
	}

}

//  ---------------------------------------------
//		eventtype()
//  ---------------------------------------------
//##ModelId=3DE49B4003D2
bool Notification::eventtype(void)
{
	// return true if event_type is event
	// else return false

	// only connection alarm is optional
	// notification alarm is always alarm

	string tmpEvent("event");

	if (eventType == tmpEvent)
	{
		return true;
	}
	else 
	{
		return false;
	}

}
//  ---------------------------------------------
//		startSocketTask()
//  ---------------------------------------------
//##ModelId=3DE49B400327
ACE_INT32 Notification::startSocketTask(void)
{
	// open socket thread
	ACE_INT32 result = socketTask.open(&sockQueue, this->thr_mgr());


	return result;

}

//  ---------------------------------------------
//		isConnectAlarm()
//  ---------------------------------------------
//##ModelId=3DE49B410008
bool Notification::isConnectAlarm(void)const
{
	return connectionAlarm;
}

//  ---------------------------------------------
//		setConnectAlarm()
//  ---------------------------------------------
//##ModelId=3DE49B41001C
void Notification::setConnectAlarm(bool set)
{
	if(set == true)
		connectionAlarm = true;
	else
		connectionAlarm = false;
}

//  ---------------------------------------------
//		isNotifyAlarm()
//  ---------------------------------------------
//##ModelId=3DE49B410030
bool Notification::isNotifyAlarm(void)const
{
	return notificationAlarm;
}

//  ---------------------------------------------
//		setNotifyAlarm()
//  ---------------------------------------------
//##ModelId=3DE49B41003A
void Notification::setNotifyAlarm(bool set)
{
	if(set == true)
		notificationAlarm = true;
	else
		notificationAlarm = false;
}
	
//  ---------------------------------------------
//		isCdhver()
//  ---------------------------------------------
//##ModelId=3DE49B41004E
bool Notification::isCdhver(void)const
{
	return recvCdhver;
}

//  ---------------------------------------------
//		setCdhver()
//  ---------------------------------------------
//##ModelId=3DE49B410062
void Notification::setCdhver(bool set)
{
	if(set == true)
		recvCdhver = true;
	else
		recvCdhver = false;
}

//  ---------------------------------------------
//		isCdhverm()
//  ---------------------------------------------
bool Notification::isCdhverm(void)const
{
	return recvCdhverm;
}

//  ---------------------------------------------
//		setCdhverm()
//  ---------------------------------------------
void Notification::setCdhverm(bool set)
{
	if(set == true)
		recvCdhverm = true;
	else
		recvCdhverm = false;
}

//  ---------------------------------------------
//		isCdhch()
//  ---------------------------------------------
bool Notification::isCdhch(void)const
{
	return recvCdhch;
}

//  ---------------------------------------------
//		setCdhch()
//  ---------------------------------------------
void Notification::setCdhch(bool set)
{
	if(set == true)
		recvCdhch = true;
	else
		recvCdhch = false;
}

//  ---------------------------------------------
//		sendCmdResult()
//  ---------------------------------------------
void Notification::sendCmdResult(MessageType mt)
{
	MessageBlock *mb = new MessageBlock(mt);
	transQueue->enqueue_tail(mb);
}

//  ---------------------------------------------
//		handle_cdhch()
//  ---------------------------------------------
bool Notification::handle_cdhch()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	ACE_INT32 rTime = atoi(responseTime.c_str());

	socketTask.setRespTime(rTime);

	setCdhch(false);

	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	string emptyDestSet("");

	emptyDestSet = m_DestSetName;
	if (reconnect == true)
	{
		socketTask.closeConnection();

		readIPaddrFromFile(hostAddress); //hostAddress needs to be updated in case cdhch has changed the notification server

		string rpath(""), spath("");

		//Convert the full path to avoid the complete data transfer folder path in notification messages.
		AES_GCC_Util::datapath_trn(fullPath, AES_DATA_PATH, rpath);
		AES_GCC_Util::datapath_trn(storePath, AES_DATA_PATH, spath);

		// from a Public network to a Public_2 network or vice versa

		socketTask.setParameters(hostAddress, IPnr, portNr, rpath, spath);

		reconnect = false;

		if (!socketTask.connect2socket()) // could not connect
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "NOTIFICATION:handle_cdhch, could not reconnect");
			if (!isConnectAlarm())
			{
				if (eventtype() == true)
				{
					// only as event
					AlarmHandler::Instance()->raiseConnectionAlarm(destName, true);
				}
				else
				{
					// raise alarm
					alarmCause = DestinationAlarm::AlarmRecord::connect_error_fn;
					// raise alarm with new cause
					DestinationAlarm::instance()->raise(destName + "_filenotification", emptyDestSet, alarmCause);
				}
				setConnectAlarm(true);
			}
			socketTask.closeConnection();

			return false;
		}
		else // connect was ok 
		{
			AES_CDH_TRACE_MESSAGE("reconnect OK");
			if (isConnectAlarm())
			{
				if (eventtype())
				{
					AlarmHandler::Instance()->ceaseConnectionAlarm(destName);
				}
				else
				{
					alarmCause = DestinationAlarm::AlarmRecord::ok;
					DestinationAlarm::instance()->cease(destName + "_filenotification", emptyDestSet, alarmCause);
				}
				setConnectAlarm(false);
			}

			return true;
		}
	}

	return true;
}
//  ---------------------------------------------
//		handle_cdhver()
//  ---------------------------------------------
bool Notification::handle_cdhver(NotificationState cstate)
{
	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	string emptyDestSet("");

	emptyDestSet = m_DestSetName;

	bool connected = socketTask.isConnected(2);

	if (connected)
	{
		AES_CDH_TRACE_MESSAGE("socketTask.isConnected is success");
	}

	switch(cstate)
	{
	case NS_DATABASE_EMPTY:
	{
		AES_CDH_TRACE_MESSAGE("case NS_DATABASE_EMPTY");

		// No connection to the remote receiver...
		//First connect to Socket & then check for lve connection.(TR HH54191)

		if (!socketTask.connect2socket())
		{
			AES_CDH_TRACE_MESSAGE("connection Not ok");
			sendCmdResult(MT_CONNECTERR);
			socketTask.closeConnection();
			setCdhver(false);
			return false;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("connection ok");

			sendCmdResult(MT_CONNECTOK);
			setCdhver(false);
			return true;
		}
	}

	break;

	case NS_ALARM:
	{
		AES_CDH_TRACE_MESSAGE("case NS_ALARM");

		// No connection to the remote receiver...
		//First connect to Socket & then check for lve connection.(TR HL63246)
		if (!socketTask.connect2socket())
		{
			sendCmdResult(MT_CONNECTERR);
			setCdhver(false);
			socketTask.closeConnection();
		     	setConnectAlarm(true);
			return false;
		}
		else
		{

			sendCmdResult(MT_CONNECTOK);
			setCdhver(false);
			return true;
		}
	}
	break;

	case NS_NORMAL:
	{
		sendCmdResult(MT_CONNECTOK);
		setCdhver(false);
		return true;
	}
	break;

	default:
		break;

	}

	return false;
}

//  ---------------------------------------------
//		handle_cdhverm()
//  ---------------------------------------------
//##ModelId=3DE49B4003AA
bool Notification::handle_cdhverm(NotificationState cstate)
{
	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	string emptyDestSet("");

	emptyDestSet = m_DestSetName;
	bool connected = socketTask.isConnected(2);

	switch(cstate)
	{
	case NS_DATABASE_EMPTY:
	{
		if (connected)
		{
			AES_CDH_TRACE_MESSAGE("case NS_DATABASE_EMPTY, connection ok");

			sendCmdResult(MT_CONNECTOK);
			setCdhverm(false);
			return true;
		}

		AES_CDH_TRACE_MESSAGE("case NS_DATABASE_EMPTY, connection Not ok");

		// No connection to the remote receiver...
		if (!socketTask.connect2socket())
		{
			sendCmdResult(MT_CONNECTERR);
			setCdhverm(false);
			socketTask.closeConnection();
			return false;
		}
		else
		{
			sendCmdResult(MT_CONNECTOK);
			setCdhverm(false);
			if (eventtype())
			{
				AlarmHandler::Instance()->ceaseConnectionAlarm(destName);
			}
			else
			{
				alarmCause = DestinationAlarm::AlarmRecord::ok;
				DestinationAlarm::instance()->cease(destName + "_filenotification", emptyDestSet, alarmCause);
			}
			setConnectAlarm(false);
			return true;
		}
	}
	break;

	case NS_ALARM:
	{
		if (connected)
		{
			sendCmdResult(MT_CONNECTOK);
			setCdhverm(false);
			return true;
		}

		// try to reconnect
		if (!socketTask.connect2socket()) // connect not ok
		{
			sendCmdResult(MT_CONNECTERR);
			setCdhverm(false);
			socketTask.closeConnection();
			return false;
		}
		else // connect ok
		{
			// cease alarm, try to resend notification
			if (eventtype())
			{
				AlarmHandler::Instance()->ceaseConnectionAlarm(destName);
			}
			else
			{
				alarmCause = DestinationAlarm::AlarmRecord::ok;
				DestinationAlarm::instance()->cease(destName + "_filenotification", emptyDestSet, alarmCause);
			}

			setConnectAlarm(false);
			return true;
		}
	}
	break;

	case NS_NORMAL:
	{
		return true;
	}
	break;

	default:
		break;
	}
	return false;
}

//  ---------------------------------------------
//		convFullPath()
//  ---------------------------------------------
void Notification::convFullPath(string &fPath)
{
	// hefe 2003-02-03
	string pathcpy = fPath;
	string tmpPath("");

	ACE_INT32 len = pathcpy.length();

	// change \ to /
	ACE_INT32 pos = pathcpy.find_first_of("/");
	while (pos > 0)
	{
		tmpPath.append(pathcpy.substr(0, pos)).append("/");
		pathcpy = pathcpy.substr(pos+1, len);
		pos = pathcpy.find_first_of("/");
	}
	tmpPath.append(pathcpy);
	tmpPath = "/" + tmpPath;

	fPath.assign(tmpPath);

}
