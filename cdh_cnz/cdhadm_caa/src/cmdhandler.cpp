/*=================================================================== */
/**
@file cmdhandler.cpp

Class method implementation for cmdhandler.h

DESCRIPTION
The services provided by CmdHandler facilitates the handling of a commands.

ERROR HANDLING
General rule:
The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       21/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <cmdhandler.h>
#include <servr.h>
#include <event.h>
#include <sstream>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

/*===================================================================
                        DECLARATION SECTION
=================================================================== */
// Declaration of trace points
AES_CDH_TRACE_DEFINE(AES_CDH_CmdHandler);

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

#ifdef _DEBUG
   #define DEBUGTEST
#endif

/*===================================================================
   ROUTINE:CmdHandler
=================================================================== */
CmdHandler::CmdHandler(int& stopEvent):
stopEvent_(stopEvent),
m_cmdStreamIO(),
cmd(),
sessionIsOpen(false),
hSendMX(),
ticks(0)
{
	AES_CDH_TRACE_MESSAGE("Entering...");
}

/*===================================================================
   ROUTINE:~CmdHandler
=================================================================== */
CmdHandler::~CmdHandler()
{
	AES_CDH_TRACE_MESSAGE("Leaving...");
}

/*===================================================================
   ROUTINE:open
=================================================================== */
int CmdHandler::open()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO,"Activating cmdHandler thread");
	int result = this->activate(THR_NEW_LWP| THR_DETACHED | THR_INHERIT_SCHED);
	// Check if the svc thread is started
	if(0 != result)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "CmdHandler::open(), error on start svc thread");
		AES_CDH_TRACE_MESSAGE("CmdHandler::open(), error on start svc thread");
	}
	AES_CDH_LOG(LOG_LEVEL_INFO,"cmdHandler thread spawned successfully");
	return result;
}

/*===================================================================
   ROUTINE:close
=================================================================== */
int CmdHandler::close(u_long)
{
	AES_CDH_TRACE_MESSAGE("Leaving deleting this");
	delete this;
	return 0;
}

/*===================================================================
   ROUTINE:sendReply
=================================================================== */
ACE_INT32 CmdHandler::sendReply(ACS_APGCC_Command& cmd)   //INGO3 GOH qabhall
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(hSendMX);
	ACE_INT32 rcode = 0;
	rcode = cmd.send(m_cmdStreamIO);
	AES_CDH_TRACE_MESSAGE("Leaving, reply sent");
	return rcode;
}

/*===================================================================
   ROUTINE:svc
=================================================================== */
ACE_INT32 CmdHandler::svc()
{
	string destName;
	string transType;
	string fileName;
	string newFileName;
	string newSubFileName;
	string streamName;
	string mainFileName;
	string alarmText;
	string fileDest;
	string tmpstr;
	string destSetName;
	string primaryDestName;
	string secondaryDestName;
	string userGroup;
	string userName;
	string backupDestName;
	string suppressDirCreate;
	ACE_INT32    streamSize = 0;
	ACE_INT32    recordLength = 0;
	ACE_INT32    i;
	ACE_INT32    j;
	ACE_INT32    argc;
	ACE_TCHAR*  argv[32];
	ACE_UINT64 recordsSent = 0;
	vector<AES_CDH_Destination::destAttributes> attrs;
	vector<AES_CDH_Destination::destAttributes>::iterator ita;
	vector<AES_CDH_DestinationSet::destSetAttributes> dsattrs;
	vector<AES_CDH_DestinationSet::destSetAttributes>::iterator dsita;
	vector<string>::iterator itb;
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	ACE_INT32 pid;
	ACE_INT32 destSetObj;
	string detailInfo;
	string userUnique;
	ACE_INT32 trMode;
	string fileMask;
	bool isDir;
	bool retryAfterRestart;
	ACE_INT32 ret = 0;
	struct timespec tv;
	if(clock_gettime(CLOCK_MONOTONIC,&tv)!= 0)
	{
		ticks = 0;
	}
	else
	{
		ticks = (tv.tv_sec * 1000) + (tv.tv_nsec / 1000000);
	}

	ServR::insertCmdHandler(this);

	acs_dsd::HANDLE dsdHandle;
	int dsdHandleCount = 1;

	const nfds_t nfds = 2;
	struct pollfd fds[nfds];
	ACE_INT32 pollret;

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	m_cmdStreamIO.get_handles(&dsdHandle, dsdHandleCount);

	fds[0].fd = stopEvent_;
	fds[0].events = POLLIN;

	fds[1].fd = dsdHandle;
	fds[1].events = POLLIN;

	sessionIsOpen = false;
	do
	{
		AES_CDH_TRACE_MESSAGE("CmdHandler->svc(): waiting on poll");
		pollret = ACE_OS::poll(fds, nfds);

		if( -1 == pollret )
		{
			if(errno == EINTR)
			{
				continue;
			}
			break;
		}
		if( (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) || (fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) )
		{
			ServR::stopSendRecordFile(destSetName);
			ServR::setAPIClosed(destSetName);
			ServR::removeEventHandler(destSetName,this);
			// exit from the while loop
			break;
		}

		// Server shutdown request
		if(fds[0].revents & POLLIN)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO,"Request for shutdown of cmdHandler thread");
			ServR::requestShutdownOfCmdHdlrThread(destSetName, this);
			// exit from the while loop
			break;
		}

		if(fds[1].revents & POLLIN)
		{
			if (cmd.recv(m_cmdStreamIO) != 0)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR in cmd.recv(m_cmdStreamIO)");
				AES_CDH_TRACE_MESSAGE("ERROR in cmd.recv(m_cmdStreamIO)");
				ServR::stopSendRecordFile(destSetName);
				ServR::setAPIClosed(destSetName);
				ServR::removeEventHandler(destSetName,this);

				Event::report(AES_CDH_clientReplyFault, "CLIENT CONTACT ERROR", "No contact with client", "-");
				// exit from the while loop
				break;
			}

			if( ServR::isStopEventSignalled == true )
			{
				if( cmd.cmdCode == ServR::CMD_SENDFILE )
				{
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "Stop signal is true when SENDFILE is recieved.");
					cmd.cmdCode += ServR::CMD_OK;
					cmd.result = AES_CDH_RC_TASKSTOPPED;
				}
				else if( cmd.cmdCode == ServR::CMD_STOPTASKS )
				{
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "Stop signal is true when STOPTASKS is recieved.");
					cmd.cmdCode += ServR::CMD_OK;
					cmd.result = AES_CDH_RC_OK;
				}
				else if( cmd.cmdCode == ServR::CMD_CLOSESESSION )
				{
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "Stop signal is true when CLOSESESSION  is recieved.");
					cmd.cmdCode += ServR::CMD_OK;
					cmd.result = AES_CDH_RC_OK;
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "Stop signal is true. Sending CMD_CDH_STOPPING");
					cmd.cmdCode = ServR::CMD_CDH_STOPPING;
					cmd.result = AES_CDH_RC_OK;
				}
				sendReply(cmd);
				continue;
			}

			switch (cmd.cmdCode)
			{

			case ServR::CMD_LIST:

				AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_LIST = %s", (cmd.data[0]).c_str());
				// Retrieve parameters
				destName  = (std::string)cmd.data[0];

				// Get destination information
				cmd.result = ServR::listDest(destName, attrs);

				// Pack return data
				if (cmd.result == AES_CDH_RC_OK)
				{
					cmd.clear();
					i=0;
					for (ita = attrs.begin(); ita != attrs.end(); ++ita)
					{
						cmd.data[i++] = (*ita).destName.c_str();
						cmd.data[i++] = (*ita).destSetName.c_str();
						cmd.data[i++] = (*ita).destPath.c_str();
						cmd.data[i++] = (*ita).transferType.c_str();
						j = i++;
						cmd.data[j] = (int)((*ita).otherAttrs.size());
						for (itb = (*ita).otherAttrs.begin(); itb != (*ita).otherAttrs.end(); ++itb)
						{
							if ( *itb != "-p" ) // Don't send password to client
							{
								cmd.data[i++] = (*itb).c_str();
							}
							else
							{
								cmd.data[j] = (int)((*ita).otherAttrs.size() - 2);
								++itb;
							}
						}
					}
					cmd.result = AES_CDH_RC_OK;
				}

				sendReply(cmd);

				AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_list reply sent, return code = %d", cmd.result);

				break;

			case ServR::CMD_CHANGE:

				switch((int)cmd.data[1])
				{

				case 1:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHANGE %s %s ", (cmd.data[1]).c_str(),(cmd.data[0]).c_str());
					break;
				}

				case 2:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHANGE %s %s %s",(cmd.data[1]).c_str(),(cmd.data[0]).c_str(), (cmd.data[3]).c_str());

					break;
				}

				case 3:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHANGE %s %s %s %s", (cmd.data[1]).c_str(),(cmd.data[0]).c_str(), (cmd.data[3]).c_str(),(cmd.data[4]).c_str());
					break;
				}

				case 4:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHANGE %s %s %s %s %s", (cmd.data[1]).c_str(),(cmd.data[0]).c_str(), (cmd.data[3]).c_str(), (cmd.data[4]).c_str(), (cmd.data[5]).c_str());
					break;
				}

				default:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHANGE %s %s %s %s %s %s", (cmd.data[1]).c_str(),(cmd.data[0]).c_str(), (cmd.data[3]).c_str(), (cmd.data[4]).c_str(),(cmd.data[5]).c_str(),(cmd.data[6]).c_str());
					break;
				}
				}

				// Retrieve parameters
				destName = (std::string)cmd.data[0];
				userName = (std::string)cmd.data[1];
				argc     = cmd.data[2];
				for (i = 3; i < (argc+3); i++)
				{
					tmpstr = (std::string)cmd.data[i];
					argv[i-3] = new ACE_TCHAR[tmpstr.size() + 1];
					(void) strcpy(argv[i-3], tmpstr.c_str());
				}

				// Change attributes
				cmd.result = ServR::changeAttr(destName, argc, argv, userName);

				sendReply(cmd);

				AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_CHANGE reply sent, return code = %d", cmd.result);
				// Release argv
				for (i = 0; i < argc; i++)
				{
					delete [] argv[i];
				}
				break;

				case ServR::CMD_DELETE:

					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_DELETE %s", (cmd.data[0]).c_str());
					// Retrieve parameters
					destName  = (std::string)cmd.data[0];
					userName  = (std::string)cmd.data[1];
					detailInfo = "";

					// Get destination information
					rcode = ServR::deleteDest(destName, detailInfo, userName);

					cmd.clear();
					cmd.data[0] = detailInfo;
					cmd.result  = rcode;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_DELETE reply sent, return code = %d detailInfo = %s", cmd.result, detailInfo.c_str());
					break;


				case ServR::CMD_SENDFILE:

					if (sessionIsOpen )
					{
						AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_SENDFILE %s %s %s %s", (cmd.data[0]).c_str(),(cmd.data[1]).c_str(), (cmd.data[2]).c_str(),(cmd.data[5]).c_str());
						// Retrieve parameters
						destSetName       = (std::string)cmd.data[0];
						fileName          = (std::string)cmd.data[1];
						newSubFileName    = (std::string)cmd.data[2];
						pid               = cmd.data[3];
						destSetObj        = cmd.data[4];
						newFileName       = (std::string)cmd.data[5];
						userUnique        = (std::string)cmd.data[6];
						trMode            = cmd.data[7];
						fileMask          = (std::string)cmd.data[8];
						isDir             = (((int)cmd.data[9]  > 0) ? true : false);
						retryAfterRestart = (((int)cmd.data[10] > 0) ? true : false);


						// Send file
						rcode = ServR::sendFile(destSetName,
								fileName,
								newSubFileName,
								newFileName, 
								pid,
								destSetObj,
								this,
								ticks,
								&cmd,
								userUnique,
								AES_CDH_DestinationSet::transferMode(trMode),
								fileMask,
								isDir,
								retryAfterRestart);

						if (rcode == AES_CDH_RC_TASKEXISTS || rcode == AES_CDH_RC_FILEALREADYSENT)
						{
							if (rcode == AES_CDH_RC_FILEALREADYSENT)
							{
								rcode = AES_CDH_RC_OK;
							}

							cmd.result  = rcode;
							cmd.cmdCode += ServR::CMD_OK;
							sendReply(cmd);
						}
						AES_CDH_TRACE_MESSAGE("Exiting CmdHandler::svc_SENDFILE rCode = %d",rcode);
					}
					else
					{
						cmd.result  = AES_CDH_RC_INCUSAGE;
						cmd.cmdCode += ServR::CMD_OK;
						sendReply(cmd);
					}

					break;

				case ServR::CMD_SENDRECORDFILE:
				{

					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_SENDRECORDFILE %s %s %s", (cmd.data[0]).c_str(),(cmd.data[1]).c_str(), (cmd.data[2]).c_str());

					// Retrieve parameters
					destSetName  = (std::string)cmd.data[0];
					streamName   = (std::string)cmd.data[1];
					mainFileName = (std::string)cmd.data[2];
					pid          = cmd.data[3];
					destSetObj   = cmd.data[4];

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_SENDRECORDFILE");

					// Send record
					//Get the original CmdHandler opened in OPENSESSION
					CmdHandler *cmdHdlr = ServR::getCmdHandler(destSetName);

					if(cmdHdlr)
					{
						rcode = ServR::sendRecordFile(destSetName, streamName, streamSize,
								recordLength, mainFileName,
								fileName, recordsSent,
								pid, destSetObj, cmdHdlr, ticks);

						// Pack return data
						cmd.result  = rcode;
						cmd.data[0] = fileName.c_str();
						cmd.data[1] = (int)recordsSent;
						cmd.cmdCode += ServR::CMD_OK;
					}
					else
					{
						cmd.result = AES_CDH_RC_INTPROGERR;
					}
					sendReply(cmd);
					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_SENDRECORDFILE reply sent, return result = AES_CDH_RC_TASKEXISTS");

					AES_CDH_TRACE_MESSAGE("Exiting CmdHandler::svc_SENDRECORDFILE rcode = %u fileName = %s recordsSent= %lu", rcode, fileName.c_str(), recordsSent);
				}

				break;

				case ServR::CMD_STOPSENDFILE:

					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_STOPSENDFILE %s%s%s%s", (cmd.data[0]).c_str(),(cmd.data[1]).c_str(), (cmd.data[2]).c_str(),(cmd.data[3]).c_str());
					// Retrieve parameters
					destName       = (std::string)cmd.data[0];
					fileName       = (std::string)cmd.data[1];
					newFileName    = (std::string)cmd.data[2];
					newSubFileName = (std::string)cmd.data[3];

					// Stop send file
					cmd.result = ServR::stopSendFile(destName, fileName,
							newFileName, newSubFileName);

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_STOPSENDFILE reply sent, return code =  %d", cmd.result);
					AES_CDH_TRACE_MESSAGE("Exiting CmdHandler::svc_STOPSENDFILE res = %d",cmd.result);

					break;


				case ServR::CMD_STOPTASKS:

					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_STOPTASKS %s%s", (cmd.data[0]).c_str(), (cmd.data[1]).c_str());
					// Retrieve parameters
					pid        = cmd.data[0];
					destSetObj = cmd.data[1];

					// Stop tasks
					cmd.result = ServR::stopTasks(pid, destSetObj);

					cmd.cmdCode = ServR::CMD_STOPTASKS + ServR::CMD_OK;

					ret = sendReply(cmd);
					AES_CDH_TRACE_MESSAGE( "CmdHandler::svc(), CMD_STOPTASKS reply sent, sendReply() returned %d", ret);
					AES_CDH_TRACE_MESSAGE( "Exiting CmdHandler::svc_STOPTASKS %d",cmd.result);

					break;


				case ServR::CMD_REMOVEUSERUNIQUE:

					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_REMOVEUSERUNIQUE %s", (cmd.data[0]).c_str());
					// Retrieve parameters
					userUnique       = (std::string)cmd.data[0];

					cmd.result = ServR::removeUserUnique(userUnique);

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_REMOVEUSERUNIQUE reply sent, return code = %u", cmd.result);

					break;

				case ServR::CMD_OPENSESSION:

					// Retrieve parameters
					destSetName = (std::string)cmd.data[0];
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_OPENSESSION %s",destSetName.c_str());
					// Check if destination set exist
					cmd.result = ServR::existDestSet(destSetName);

					if (cmd.result == AES_CDH_RC_OK)
					{
						sessionIsOpen = true;
						AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_OPENSESSION, session opened");
					}

					cmd.cmdCode = ServR::CMD_OPENSESSION + ServR::CMD_OK;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_OPENSESSION reply sent");
					AES_CDH_TRACE_MESSAGE("Exiting , result = %d",cmd.result);
					break;


				case ServR::CMD_CLOSESESSION:

					if(sessionIsOpen)
					{
						AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CLOSESESSION %s",destSetName.c_str());
					}
					else
					{
						AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CLOSESESSION Session not open");
					}

					if(sessionIsOpen)
					{
						ServR::removeEventHandler(destSetName,this);
						ServR::removeCmdHandler(this);
						sessionIsOpen = false;
						cmd.result = AES_CDH_RC_OK;
						ServR::setAPIClosed(destSetName);
						AES_CDH_TRACE_MESSAGE("setApiClosed ended");
					}

					cmd.cmdCode = ServR::CMD_CLOSESESSION + ServR::CMD_OK;
					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_CLOSESESSION reply sent");
					AES_CDH_TRACE_MESSAGE("Exiting , result : %u", cmd.result);
					break;

				case ServR::CMD_SETEVENTHANDLER:

					fileDest = (std::string)cmd.data[0];
					alarmText = (std::string)cmd.data[1];

					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_SETEVENTHANDLER %s%s%s",destSetName.c_str(),fileDest.c_str(),alarmText.c_str());
					if(sessionIsOpen)
					{
						AES_CDH_TRACE_MESSAGE("CMD_SETEVENTHANDLER");
						cmd.result = ServR::setEventHandler(destSetName,fileDest,alarmText,this);
						cmd.cmdCode += ServR::CMD_OK;
						sendReply(cmd);
					}
					else
					{
						cmd.result  = AES_CDH_RC_INCUSAGE;
						cmd.cmdCode += ServR::CMD_OK;
						sendReply(cmd);
					}

					AES_CDH_TRACE_MESSAGE("CMD_SETEVENTHANDLER reply sent, return code = %d", cmd.result);
					break;

				case ServR::CMD_REMOVEEVENTHANDLER:

					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_REMOVEEVENTHANDLER %s",destSetName.c_str());
					if(sessionIsOpen)
					{
						cmd.result = ServR::removeEventHandler(destSetName, this);
						cmd.cmdCode += ServR::CMD_OK;
						sendReply(cmd);

						AES_CDH_TRACE_MESSAGE("CMD_REMOVEEVENTHANDLER reply sent");

					}
					else
					{
						cmd.result   = AES_CDH_RC_INCUSAGE;
						cmd.cmdCode += ServR::CMD_OK;
						sendReply(cmd);
					}

					AES_CDH_TRACE_MESSAGE("CMD_REMOVEEVENTHANDLER reply sent, return code = %d", cmd.result);
					break;

				case ServR::CMD_CHECKCONNECTION:
				{
					string dName = (std::string)cmd.data[0];
					vector<ServR::checkConnAttributes> cAttrs;
					vector<ServR::checkConnAttributes>::iterator itc;
					bool force   = (((int)cmd.data[1]  > 0) ? true : false);
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHECKCONNECTION %s",dName.c_str());
					cmd.clear();
					cmd.result = ServR::checkConnection(dName, force, cAttrs);
					cmd.cmdCode = ServR::CMD_CHECKCONNECTION;
					i=0;
					for (itc = cAttrs.begin(); itc != cAttrs.end(); ++itc)
					{
						cmd.data[i++] = (*itc).destName.c_str();
						cmd.data[i++] = (*itc).status;
						cmd.data[i++] = 0;
					}
					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_CHECKCONNECTION reply sent, return code = %d", cmd.result);
				}
				break;

				case ServR::CMD_CHECKCONNECTION_MANUALLY:
				{
					string dName = (std::string)cmd.data[0];
					vector<ServR::checkConnAttributes> cAttrs;
					vector<ServR::checkConnAttributes>::iterator itc;

					bool force = (((int)cmd.data[1] > 0) ? true : false);
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHECKCONNECTION_MANUALLY %s",dName.c_str());
					cmd.clear();
					cmd.result = ServR::checkConnectionManually(dName, force, cAttrs);
					cmd.cmdCode = ServR::CMD_CHECKCONNECTION_MANUALLY;
					i=0;
					for (itc = cAttrs.begin(); itc != cAttrs.end(); ++itc)
					{
						cmd.data[i++] = (*itc).destName.c_str();
						cmd.data[i++] = (*itc).status;
						cmd.data[i++] = 0;
					}
					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_CHECKCONNECTION_MANUALLY reply sent, return code = %d", cmd.result);
				}
				break;

				case ServR::CMD_CHECKDESTINATIONSET_TYPE:
				{
					string dsName = (std::string)cmd.data[0];
					ACE_INT32 askAttr  = cmd.data[1];
					ACE_INT32 realAttr = 0;
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHECKDESTINATIONSET_TYPE");
					cmd.clear();
					cmd.result = ServR::checkDestinationSetType(dsName,
							(AES_CDH_DestinationSet::TQConnectAttributes)askAttr,
							(AES_CDH_DestinationSet::TQConnectAttributes &)realAttr);

					cmd.cmdCode = ServR::CMD_OK;
					cmd.data[0] = dsName;
					cmd.data[1] = realAttr;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE(" CMD_CHECKDESTINATIONSET_TYPE reply sent, return code = %d", cmd.result);
				}
				break;

				case ServR::CMD_TRANSACTIONBEGIN:
				{
					string dsName = (std::string)cmd.data[0];
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_TRANSACTIONBEGIN");
					cmd.result = ServR::transactionBegin(dsName);
					cmd.data[0] = dsName;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_TRANSACTIONBEGIN reply sent, return code = %d", cmd.result);
				}
				break;

				case ServR::CMD_TRANSACTIONEND:
				{
					string dsName            = (std::string)cmd.data[0];
					unsigned int applBlockNr = cmd.data[1];
					cmd.result = ServR::transactionEnd(dsName, applBlockNr);

					cmd.data[0] = dsName;
					cmd.data[1] = (int)applBlockNr;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_TRANSACTIONEND reply sent, return code = %d application block nr = %d", cmd.result, applBlockNr);
				}
				break;

				case ServR::CMD_TRANSACTIONCOMMIT:
				{
					string dsName            = (std::string)cmd.data[0];
					unsigned int applBlockNr = 0;
					int temp = cmd.data[1];
					applBlockNr = (unsigned int) temp;

					cmd.result = ServR::transactionCommit(dsName, applBlockNr);
					cmd.data[0] = dsName;
					cmd.data[1] = (int)applBlockNr;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_TRANSACTIONCOMMIT reply sent, return code = %d", cmd.result);
				}
				break;

				case ServR::CMD_TRANSACTIONTERMINATE:
				{
					string dsName = (std::string)cmd.data[0];
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_TRANSACTIONTERMINATE");
					cmd.result = ServR::transactionTerminate(dsName);
					cmd.data[0] = dsName;
					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_TRANSACTIONTERMINATE reply sent, return code = %d", cmd.result);
					AES_CDH_TRACE_MESSAGE("Exiting CmdHandler::svc_TRANSACTIONTERMINATE");
				}
				break;

				case ServR::CMD_GETLASTCOMMITTEDBLOCK:
				{
					string dsName              = (std::string)cmd.data[0];
					unsigned int applBlockNr = cmd.data[1];

					cmd.result = ServR::getLastCommittedBlock(dsName, applBlockNr);
					cmd.data[0] = dsName;
					cmd.data[1] = applBlockNr;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_GETLASTCOMMITTEDBLOCK reply sent, return code = %d", cmd.result);
				}
				break;

				case ServR::CMD_DEFINEDESTSET:
				{
				}
				break;

				case ServR::CMD_DELETEDESTSET:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_DELETEDESTSET %s %s", (cmd.data[0]).c_str(), (cmd.data[1]).c_str());
					// Retrieve parameters
					destSetName  = (std::string)cmd.data[0];
					detailInfo   = "";
					destName     = (std::string)cmd.data[1];
					userName     = (std::string)cmd.data[2];

					// Delete destination in destinationset or the entire destinationset
					rcode = ServR::deleteDestSet(destSetName, detailInfo, destName, userName);
					cmd.clear();
					cmd.data[0] = detailInfo;
					cmd.result  = rcode;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_DELETEDESTSET reply sent, return code = %d", rcode);
					AES_CDH_TRACE_MESSAGE("Exiting CmdHandler::svc_DELETEDESTSET %d",cmd.result);
				}
				break;

				case ServR::CMD_LISTDESTSET:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_LISTDESTSET %s", (cmd.data[0]).c_str());
					// Retrieve parameters
					destSetName  = (std::string)cmd.data[0];

					// List destinationset
					cmd.result = ServR::listDestSet(destSetName, dsattrs);

					// Pack return data
					if (cmd.result == AES_CDH_RC_OK)
					{
						cmd.clear();
						i=0;
						for (dsita = dsattrs.begin(); dsita != dsattrs.end(); ++dsita)
						{
							cmd.data[i++] = (*dsita).destSetName.c_str();
							cmd.data[i++] = (*dsita).destSetType;
							cmd.data[i++] = (*dsita).primDest.c_str();
							cmd.data[i++] = (*dsita).secDest.c_str();
							cmd.data[i++] = (*dsita).backDest.c_str();
							cmd.data[i++] = (*dsita).active.c_str();
							cmd.data[i++] = (*dsita).userGroup.c_str();
						}
						cmd.result = AES_CDH_RC_OK;
					}
					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), CMD_LISTDESTSET reply sent, return code = %u", cmd.result);
				}
				break;

				case ServR::CMD_CHANGEDESTSET:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_CHANGEDESTSET %s%s%s%s%s", (cmd.data[0]).c_str(), (cmd.data[1]).c_str(), (cmd.data[2]).c_str(), (cmd.data[3]).c_str(), (cmd.data[4]).c_str());
					// Retrieve parameters
					destSetName       = (std::string)cmd.data[0];
					primaryDestName	  = (std::string)cmd.data[1];
					secondaryDestName = (std::string)cmd.data[2];
					backupDestName    = (std::string)cmd.data[3];
					userName          = (std::string)cmd.data[4];

					// Change or add destination in destinationset
					rcode = ServR::changeDestSet(destSetName, primaryDestName, secondaryDestName, 							  backupDestName, userName);
					cmd.clear();
					cmd.result  = rcode;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_CHANGEDESTSET reply sent, return code = %d", rcode);
					AES_CDH_TRACE_MESSAGE("Exiting CmdHandler::svc_CHANGEDESTSET %d",cmd.result);
				}
				break;

				case ServR::CMD_SWITCHACTIVEDEST:
				{
					AES_CDH_TRACE_MESSAGE("Entering CmdHandler::svc_SWITCHACTIVEDEST %s%s", (cmd.data[0]).c_str(), (cmd.data[1]).c_str());
					// Retrieve parameters
					destSetName  = (std::string)cmd.data[0];
					destName	 = (std::string)cmd.data[1];

					// Switch active destination in destination set
					rcode = ServR::switchActiveDest(destSetName, destName);

					cmd.clear();
					cmd.result  = rcode;

					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CMD_SWITCHACTIVEDEST reply sent, return code = %d", rcode);
					AES_CDH_TRACE_MESSAGE("Exiting CmdHandler::svc_SWITCHACTIVEDEST %u",cmd.result);
				}
				break;

				default:

					ostringstream err_back;
					err_back << "Unknown command code : " << cmd.cmdCode;

					Event::report(AES_CDH_unknownCmdFault, "UNKNOWN COMMAND CODE", err_back.str(), "-");

					// Unknown command code
					cmd.result = AES_CDH_RC_UNREAS;
					sendReply(cmd);

					AES_CDH_TRACE_MESSAGE("CmdHandler::svc(), Unknown command!!!, reply sent");

			} // end of switch (cmd.cmdCode)
		}// dsd handle signaled
	}
	while (sessionIsOpen );

	ServR::removeCmdHandler(this);

	AES_CDH_TRACE_MESSAGE("Exiting!! ");

	return 0;
}


/*===================================================================
   ROUTINE:sendRecordFileReply
=================================================================== */
ACE_INT32 CmdHandler::sendRecordFileReply(AES_CDH_ResultCode result,
	                                  const std::string &fileName, 
        		                  ACE_UINT64 recordsSent)
{
#ifdef DEBUGTEST
	cout << "CmdHandler::sendRecordFileReply  result="<< result
			<< " fileName=" << fileName
			<< " recordsSent=" << recordsSent
			<< endl;
#endif

	ACS_APGCC_Command cmd;
	cmd.cmdCode = ServR::CMD_SENDRECORDFILE + ServR::CMD_OK;
	cmd.result  = result;
	cmd.data[0] = fileName.c_str();
	cmd.data[1] = (int)recordsSent;
	sendReply(cmd);
	return 0;
}
/*===================================================================
   ROUTINE:sendFileReply
=================================================================== */
ACE_INT32 CmdHandler::sendFileReply(AES_CDH_ResultCode result,
                              const std::string &destSetName, 
                              const std::string &fileName, 
                              const std::string &newFileName,
                              const std::string &newSubFileName,
                              ACE_INT32 pid, ACE_INT32 destSetObj)
{
#ifdef DEBUGTEST
	cout << "CmdHandler::sendFileReply  result="<< result
			<< " destSetName=" << destSetName
			<< " fileName=" << fileName
			<< " newFileName=" << newFileName
			<< " newSubFileName=" << newSubFileName
			<< endl;
#endif

	ACS_APGCC_Command cmd;
	cmd.cmdCode = ServR::CMD_SENDFILE + ServR::CMD_OK;
	cmd.data[0] = destSetName;
	cmd.data[1] = fileName;
	cmd.data[2] = newFileName;
	cmd.data[3] = pid;
	cmd.data[4] = destSetObj;
	cmd.data[5] = newSubFileName;
	cmd.result  = result;
	sendReply(cmd);
	return 0;
}
/*===================================================================
   ROUTINE:sendDestEvent
=================================================================== */
ACE_INT32 CmdHandler::sendDestEvent(AES_CDH_ResultCode result,
                              const std::string &destName)
{
#ifdef DEBUGTEST
	cout << "CmdHandler::sendDestEvent  result="<< result
			<< " destName=" << destName
			<< endl;
#endif
	ACS_APGCC_Command cmd;
	cmd.cmdCode = ServR::CMD_DESTEVENT;
	cmd.data[0] = destName;
	cmd.result  = result;
	sendReply(cmd);
	return 0;
}

/*===================================================================
   ROUTINE:getTicks
=================================================================== */
ACE_UINT64 CmdHandler::getTicks(void)
{
	return ticks;
}
/*===================================================================
   ROUTINE:shutdown
=================================================================== */
bool CmdHandler::shutdown()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	ACS_APGCC_Command cmd;
	cmd.cmdCode = ServR::CMD_CDH_STOPPED;
	sendReply(cmd);
	AES_CDH_LOG(LOG_LEVEL_INFO,"Sending shutdown request for cmdHandler thread");
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;
}
