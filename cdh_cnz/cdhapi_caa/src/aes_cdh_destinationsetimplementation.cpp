/*=================================================================== */
/**
   @file   aes_cdh_destinationsetimplementation.cpp

   Class method implementationn for AES_CDH_DestinationSetImplementation type module.

   This module contains the implementation of class declared in
   the aes_cdh_destinationsetimplementation.h module

   This program receives parameters sent from programs
   cdhdsdef, cdhdsrm, cdhdsch, cdhdssw and cdhdsls. The parameters
   is then analysed and
   sent further to method sendCmd in class DestinationSet.
   Cdhdsdef is calling method define and cdhdsls is calling
   method getAttr.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/OS.h>
#include <ace/ACE.h>
#include <ace/Get_Opt.h>
#include <ACS_APGCC_Command.H>
#include <ACS_APGCC_Cmd_Client.H>
#include <ACS_APGCC_DSD.H>
#include <aes_cdh_destinationsetimplementation.h>
#include <aes_cdh_cmdclient.h>
#include <servr.h>
#include <ctype.h>
#include <aes_cdh_result.h>
#include <aes_cdh_asynchreceiver.h>
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_DestinationSetImplementation);

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: AES_CDH_DestinationSetImplementation
=================================================================== */
AES_CDH_DestinationSetImplementation::AES_CDH_DestinationSetImplementation() 
: destinationSetName(),
  sessionIsOpen(false),
  lastErrorCode(AES_CDH_RC_OK),
  asynchReceiver(NULL),
  closeDone(true),
  myDestSet(NULL),
  m_ProcessPid(ACE_OS::getpid())
{

}

/*===================================================================
   ROUTINE: AES_CDH_DestinationSetImplementation
=================================================================== */
AES_CDH_DestinationSetImplementation::AES_CDH_DestinationSetImplementation(const std::string destSetName)
: destinationSetName(destSetName),
  sessionIsOpen(false),
  lastErrorCode(AES_CDH_RC_OK),
  asynchReceiver(NULL),
  closeDone(true),
  myDestSet(NULL),
  m_ProcessPid(ACE_OS::getpid())
{

}

/*===================================================================
   ROUTINE: ~AES_CDH_DestinationSetImplementation
=================================================================== */
AES_CDH_DestinationSetImplementation::~AES_CDH_DestinationSetImplementation() 
{
    if(!closeDone)
          close();

}

/*===================================================================
   ROUTINE: open
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::open(
     AES_CDH_DestinationSet& destSet)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	AES_CDH_ResultCode r_code = AES_CDH_RC_INCUSAGE;

	if( (!sessionIsOpen) && (!destinationSetName.empty()) )
	{
		AES_CDH_TRACE_MESSAGE("create internal receiver");

		asynchReceiver = new (std::nothrow) AES_CDH_AsynchReceiver();

		if(NULL != asynchReceiver)
		{
			AES_CDH_TRACE_MESSAGE("open communication session");

			r_code = asynchReceiver->openSession(*this, destinationSetName);

			if(r_code != AES_CDH_RC_OK)
			{
				AES_CDH_TRACE_MESSAGE("open communication session failed!");
				delete asynchReceiver;
				asynchReceiver = NULL;
			}
			else
			{
				closeDone = false;
				myDestSet = &destSet;
				sessionIsOpen = true;
			}

			lastErrorCode = r_code;
		}
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>, session is <%s>, error:<%d>", destinationSetName.c_str(), (sessionIsOpen ? "OPEN" : "NOT OPEN"), r_code);

	return r_code;
}

/*===================================================================
   ROUTINE: close
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::close(void)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	AES_CDH_ResultCode r_code = AES_CDH_RC_OK;

	if((sessionIsOpen) && (!closeDone))
	{
		AES_CDH_TRACE_MESSAGE("Send CMD_STOPTASKS to CDH, DestinationSet:<%s>", destinationSetName.c_str());
		closeDone = true;

		ACS_APGCC_Command cmd;
		cmd.cmdCode = ServR::CMD_STOPTASKS;
		cmd.data[0] = m_ProcessPid;
		cmd.data[1] = reinterpret_cast<long>(this);

		r_code = asynchReceiver->sendCmd(cmd);

		if(r_code == AES_CDH_RC_OK)
		{
			AES_CDH_TRACE_MESSAGE("Waiting answer on CMD_STOPTASKS, DestinationSet:<%s>", destinationSetName.c_str());
			asynchReceiver->stopTasksAckReceived();
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Failed to send CMD_STOPTASKS to CDH, DestinationSet:<%s>", destinationSetName.c_str());
		}

		AES_CDH_TRACE_MESSAGE("Close internal communication session, DestinationSet:<%s>", destinationSetName.c_str());
		asynchReceiver->closeSession();

		sessionIsOpen = false;
	}

	if(NULL != asynchReceiver)
	{
		delete asynchReceiver;
		asynchReceiver=NULL;
	}
	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>, result:<%d>", destinationSetName.c_str(), r_code);
	return r_code;
}

/*===================================================================
   ROUTINE: setEventSubscription
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::setEventSubscription(
                                            const std::string fileTransferQueue,
                                            const std::string alarmText)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>, TQ:<%s> Alarm Text:<%s>",
			destinationSetName.c_str(), fileTransferQueue.c_str(), alarmText.c_str());

	AES_CDH_ResultCode r_code = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		AES_CDH_TRACE_MESSAGE("Send CMD_SETEVENTHANDLER to CDH, DestinationSet:<%s>", destinationSetName.c_str());
		ACS_APGCC_Command cmd;
		cmd.cmdCode = ServR::CMD_SETEVENTHANDLER;

		cmd.data[0] = fileTransferQueue;
		cmd.data[1] = alarmText;
		cmd.data[3] = m_ProcessPid;
		cmd.data[4] = reinterpret_cast<long>(this);

		r_code = asynchReceiver->sendCmd(cmd);
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Session is not open!, DestinationSet:<%s>", destinationSetName.c_str());
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>, result:<%d>", destinationSetName.c_str(), r_code);
	return r_code;
}

/*===================================================================
   ROUTINE: removeEventSubscription
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::removeEventSubscription(void)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	AES_CDH_ResultCode r_code = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		AES_CDH_TRACE_MESSAGE("Send CMD_REMOVEEVENTHANDLER to CDH, DestinationSet:<%s>", destinationSetName.c_str());

		ACS_APGCC_Command cmd;
		cmd.cmdCode = ServR::CMD_REMOVEEVENTHANDLER;

		cmd.data[0] = m_ProcessPid;
		cmd.data[1] = reinterpret_cast<long>(this);

		r_code = asynchReceiver->sendCmd(cmd);
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>, result:<%d>", destinationSetName.c_str(), r_code);

	return r_code;
}

/*===================================================================
   ROUTINE: exists
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::exists(void)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	int r_code;

	std::vector<AES_CDH_DestinationSet::destSetAttributes> attr;

	r_code = list(destinationSetName, attr);

	lastErrorCode = r_code;

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>, result:<%d>", destinationSetName.c_str(), r_code);

	return AES_CDH_Result(r_code).code();
}


/*===================================================================
   ROUTINE: exists
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::exists(
                        const AES_CDH_DestinationSet::TQConnectAttributes attr)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	std::vector<AES_CDH_DestinationSet::destSetAttributes> destAttr;

	int rCode = list(destinationSetName, destAttr);

	AES_CDH_ResultCode rc = AES_CDH_Result(rCode).code();

	if (rc == AES_CDH_RC_OK)
	{
		AES_CDH_TRACE_MESSAGE("Send CMD_CHECKDESTINATIONSET_TYPE to CDH, DestinationSet:<%s>", destinationSetName.c_str());

		ACS_APGCC_Command cmd;
		cmd.cmdCode = ServR::CMD_CHECKDESTINATIONSET_TYPE;

		cmd.data[0] = destinationSetName;
		cmd.data[1] = attr;

		// create the client
		ACS_APGCC_Cmd_Client<ACS_APGCC_DSD_Connector,ACS_APGCC_DSD_Stream,ACS_APGCC_DSD_Addr>
		client("AES_CDH_server:AES");

		// send to server
		rCode = client.execute(cmd);

		if (rCode != 0)
		{
			rCode = AES_CDH_RC_NOSERVER;
		}
		else
		{
			rCode = cmd.result;
		}

		if (rCode == AES_CDH_RC_OK)
		{
			if( cmd.numOfData() <= 0)
			{
				return AES_CDH_RC_NODEST;
			}

			// Retrieve reply from server
			std::string dsName = (std::string)cmd.data[0];
			int realAttr = cmd.data[1];

			if (dsName == destinationSetName && attr == realAttr)
				rCode = AES_CDH_RC_OK;
			else
			{
				switch (attr)
				{
				case AES_CDH_DestinationSet::FILE:
					rCode = AES_CDH_RC_NOTFILEDESTSET;
					break;

				case AES_CDH_DestinationSet::BLOCK:
					rCode = AES_CDH_RC_NOTBLOCKDESTSET;
					break;

				default:
					break;
				}
			}
		}
	}

	lastErrorCode = rCode;

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>, result:<%d>", destinationSetName.c_str(), rCode);

	return AES_CDH_Result(rCode).code();
}


/*===================================================================
   ROUTINE: sendFileEv
=================================================================== */
void AES_CDH_DestinationSetImplementation::sendFileEv(AES_CDH_ResultCode ret,
                                                   const std::string destinationSetName,
                                                   const std::string fileName,
                                                   const std::string newFileName,
                                                   const std::string newSubFileName)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>, resultCode:<%d>, fileName:<%s>, newFileName:<%s>, newSubFileName:<%s>",
			destinationSetName.c_str(), ret, fileName.c_str(), newFileName.c_str(), newSubFileName.c_str() );

	lastErrorCode = ret;

	if(NULL != myDestSet)
	{
		myDestSet->eventSendFile(ret, destinationSetName, fileName, newFileName,newSubFileName);
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("myDestSet is NULL!, DestinationSet:<%s>", destinationSetName.c_str());
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>", destinationSetName.c_str());
}

/*===================================================================
   ROUTINE: sendRecordFileEv
=================================================================== */
void AES_CDH_DestinationSetImplementation::sendRecordFileEv(AES_CDH_ResultCode ret)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>, resultCode:<%d>",
			destinationSetName.c_str(), ret);
	lastErrorCode = ret;
	if(NULL != myDestSet)
	{
		myDestSet->eventSendRecordFile(ret);//HH77667
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("myDestSet is NULL!, DestinationSet:<%s>", destinationSetName.c_str());
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s>", destinationSetName.c_str());
}

/*===================================================================
   ROUTINE: sendDestEv
=================================================================== */
void AES_CDH_DestinationSetImplementation::sendDestEv(AES_CDH_ResultCode ret, const std::string/* destSetName*/)
{
	AES_CDH_TRACE_MESSAGE("Entering..., destinationSetName:<%s>, resultCode:<%d>", destinationSetName.c_str(), ret);

	lastErrorCode = ret;

	std::string tmpDestSetName = destinationSetName;
	if(NULL != myDestSet)
	{
		myDestSet->eventDestinationSet(ret, tmpDestSetName);
	}

	AES_CDH_TRACE_MESSAGE("...Leaving");
}

/*===================================================================
   ROUTINE: sendFile
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::sendFile(
    const std::string fileName,
	const std::string remoteSubDirName,
	const std::string newFileName,
    const std::string userUnique,
    AES_CDH_DestinationSet::transferMode trMode,
    const std::string fileMask,
    bool isDir,
    bool retryAfterRestart)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>, fileName:<%s>, remoteSubDirName:<%s>, newFileName:<%s>",
			destinationSetName.c_str(), fileName.c_str(), remoteSubDirName.c_str(), newFileName.c_str());

	int r_code = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		AES_CDH_TRACE_MESSAGE("Send CMD_SENDFILE to CDH");

		ACS_APGCC_Command cmd;
		cmd.cmdCode = ServR::CMD_SENDFILE;

		cmd.data[0] = destinationSetName;
		cmd.data[1] = fileName;
		cmd.data[2] = remoteSubDirName;
		cmd.data[3] = m_ProcessPid;
		cmd.data[4] = reinterpret_cast<long>(this);
		cmd.data[5] = newFileName;
		cmd.data[6] = userUnique;
		cmd.data[7] = trMode;
		cmd.data[8] = fileMask;
		cmd.data[9] = isDir;
		cmd.data[10]= retryAfterRestart;

		r_code = asynchReceiver->sendCmd(cmd);
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Session is not Open!");
	}

	lastErrorCode = r_code;

	AES_CDH_TRACE_MESSAGE("...Leaving, result:<%d>", r_code);

	return AES_CDH_Result(r_code).code();
}

/*===================================================================
   ROUTINE: stopSendFile
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::stopSendFile(
    const std::string fileName,
    const std::string remoteSubDirName,
    const std::string newFileName)
{
	AES_CDH_TRACE_MESSAGE("Entering..., destinationSetName:<%s>, fileName:<%s>, remoteSubDirName:<%s>, newFileName:<%s>",
			destinationSetName.c_str(), fileName.c_str(), remoteSubDirName.c_str(), newFileName.c_str());

	ACS_APGCC_Command cmd;
	cmd.cmdCode = ServR::CMD_STOPSENDFILE;
	cmd.data[0] = destinationSetName;
	cmd.data[1] = fileName;
	cmd.data[2] = remoteSubDirName;
	cmd.data[3] = newFileName;

	// create the client
	ACS_APGCC_Cmd_Client<ACS_APGCC_DSD_Connector,ACS_APGCC_DSD_Stream,ACS_APGCC_DSD_Addr>
	client("AES_CDH_server:AES");

	int r_code = client.execute(cmd);  // send to server

	if (r_code != 0)
	{
		r_code = AES_CDH_RC_NOSERVER;
	}
	else
	{
		r_code = cmd.result;
	}

	lastErrorCode = r_code;

	AES_CDH_TRACE_MESSAGE("...Leaving, result:<%d>", r_code);
	return AES_CDH_Result(r_code).code();
}

/*===================================================================
   ROUTINE: sendRecordFile
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::sendRecordFile(
                                             AES_CDH_DestinationSet& destSet,
                                             string streamName,
                                             string streamId)
{
	AES_CDH_TRACE_MESSAGE("Entering..., Destination:<%s>, StreamName:<%s>, StreamIs:<%s>",
			destinationSetName.c_str(), streamName.c_str(), streamId.c_str() );
	(void) destSet;
	ACS_APGCC_Command	cmd;
	int r_code = AES_CDH_RC_OK;
	std::string	fname;
	AES_CDH_ResultCode returnCode;

	if(! sessionIsOpen)
	{
		AES_CDH_TRACE_MESSAGE("Session is not Opened!");
		return AES_CDH_RC_INCUSAGE;
	}

	cmd.cmdCode = ServR::CMD_SENDRECORDFILE;
	cmd.data[0] = destinationSetName;
	cmd.data[1] = streamName;
	cmd.data[2] = streamId;
	cmd.data[3] = m_ProcessPid;
	cmd.data[4] = reinterpret_cast<long>(this);

	//Start -  HS89201
	// create the client
	ACS_APGCC_Cmd_Client<ACS_APGCC_DSD_Connector,ACS_APGCC_DSD_Stream,ACS_APGCC_DSD_Addr>
	client("AES_CDH_server:AES");

	r_code = client.execute(cmd);  // send to server

	if (r_code != 0)
	{
		r_code = AES_CDH_RC_NOSERVER;
	}
	else
	{
		r_code = cmd.result;
	}

	returnCode = AES_CDH_Result(r_code).code();
	// End - HS89201

	lastErrorCode = returnCode;

	AES_CDH_TRACE_MESSAGE("...Leaving, result:<%d>", returnCode);
	return returnCode;
}

/*===================================================================
   ROUTINE: getError
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::getError(void)
{
    return AES_CDH_Result(lastErrorCode).code();
}

/*===================================================================
   ROUTINE: getErrorText
=================================================================== */
void AES_CDH_DestinationSetImplementation::getErrorText(std::string &errText)
{
    errText = AES_CDH_Result(lastErrorCode).errorText();
}

/*===================================================================
   ROUTINE: getAttr
=================================================================== */
AES_CDH_Result AES_CDH_DestinationSetImplementation::getAttr(int argc, char* argv[],
		std::vector<AES_CDH_DestinationSet::destSetAttributes> &attr)
{
	// Clear the vector before using it
	// It might contain old destination set data or garbage
	attr.clear();
	switch(argc)
	{
	case 1:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_DestinationSet::getAttr %d",argc);
		break;
	}
	case 2:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_DestinationSet::getAttr %d %s",argc,argv[1]);
		break;
	}
	case 3:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_DestinationSet::getAttr %d %s %s",argc,argv[1],argv[2]);
		break;
	}
	case 4:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_DestinationSet::getAttr %d %s %s %s",argc,argv[1],argv[2],argv[3]);
		break;
	}
	default:
	{
		AES_CDH_TRACE_MESSAGE("Entering AES_CDH_DestinationSet::getAttr %d %s %s %s %s",argc,argv[1],argv[2],argv[3],argv[4]);
		break;
	}
	}

	int r_code = parse_cdhdsls(argc, argv, attr);
	AES_CDH_TRACE_MESSAGE("Exiting AES_CDH_DestinationSet::getAttr %d ",r_code);

	return AES_CDH_Result(r_code);
}

/*===================================================================
   ROUTINE: list
=================================================================== */
int AES_CDH_DestinationSetImplementation::list(std::string destSetName,
		std::vector<AES_CDH_DestinationSet::destSetAttributes>& attr)
{
	ACS_APGCC_Command    cmd;
	AES_CDH_DestinationSet::destSetAttributes tmpattr;
	int r_code = AES_CDH_RC_OK;
	cmd.clear();
	cmd.cmdCode = ServR::CMD_LISTDESTSET;
	cmd.data[0] = destSetName;

	ACS_APGCC_Cmd_Client<ACS_APGCC_DSD_Connector,ACS_APGCC_DSD_Stream,ACS_APGCC_DSD_Addr>
	client("AES_CDH_server:AES"); // create the client

	r_code = client.execute(cmd);  // send to server

	if (r_code != 0)
	{
		r_code = AES_CDH_RC_NOSERVER;
	}
	else
	{
		r_code = cmd.result;
	}


	if (r_code == AES_CDH_RC_OK)
	{
		for ( int j = 0; j < cmd.numOfData(); )
		{
			int tmp = 0;
			tmpattr.destSetName = (std::string)cmd.data[j++];
			tmp                 = cmd.data[j++];
			tmpattr.primDest    = (std::string)cmd.data[j++];
			tmpattr.secDest     = (std::string)cmd.data[j++];
			tmpattr.backDest    = (std::string)cmd.data[j++];
			tmpattr.active      = (std::string)cmd.data[j++];
			tmpattr.userGroup   = (std::string)cmd.data[j++];

			switch (tmp)
			{
			case AES_CDH_DestinationSet::FILE:
				tmpattr.destSetType = AES_CDH_DestinationSet::FILE;
				break;

			case AES_CDH_DestinationSet::BLOCK:
				tmpattr.destSetType = AES_CDH_DestinationSet::BLOCK;
				break;

			default:
				break;
			}

			attr.push_back(tmpattr);
		}
	}

	return r_code;
}

/*===================================================================
   ROUTINE: cleanUpArgv
=================================================================== */
void AES_CDH_DestinationSetImplementation::cleanUpArgv(int& argc, char* argv[], int firstOper)
{
	int i = 0;
	int j = 0;

	if (argv[firstOper-1] == NULL && firstOper < argc)
	{
		argv[firstOper-1] = new char [3];
		(void) strcpy(argv[firstOper-1], "--");
	}

	while (j < argc)
	{
		if (argv[j] != NULL)
		{
			argv[i] = argv[j];
			i++;
		}
		j++;
	}
	argc = i;
}

/*===================================================================
   ROUTINE: validCharacters
=================================================================== */
int AES_CDH_DestinationSetImplementation::validCharacters(std::string str)
{
	const std::string legalChars("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
	const std::string legalChars2("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	if (str.find_first_not_of(legalChars) == std::string::npos)
	{
		if (str.substr(0,1).find_first_not_of(legalChars2) == std::string::npos)
		{
			return AES_CDH_RC_OK;
		}
	}

	return AES_CDH_RC_UNREAS;
}

/*===================================================================
   ROUTINE: validName
=================================================================== */
int AES_CDH_DestinationSetImplementation::validName(std::string& str)
{
	// Verify correct length of destination name
	if ((str.length() < 1 ) || (str.length() > 32 ))
	{
		return AES_CDH_RC_UNREAS;
	}

	// Verify that the destination name does not contain
	// any illegal characters
	int status = validCharacters(str);
	return status;
}

/*===================================================================
   ROUTINE: parse_cdhdsls
=================================================================== */
int AES_CDH_DestinationSetImplementation::parse_cdhdsls(int& argc, char* argv[],
		std::vector<AES_CDH_DestinationSet::destSetAttributes>& attr)
{
	int exitcode = AES_CDH_RC_INCUSAGE;
	int opt;
	std::string destSet = "";

	ACE_Get_Opt getopt(argc, argv, "");

	while ((opt = getopt()) != EOF)
	{
		switch (opt)
		{
		case '?':
			{
				return AES_CDH_RC_INCUSAGE;
			}

		default:
		{
			return AES_CDH_RC_INTPROGERR;
		}
		}
	}

	if (getopt.optind + 1 < argc)
		return AES_CDH_RC_INCUSAGE;

	if (argc > 2)
	{
		exitcode = AES_CDH_RC_INCUSAGE;
	}

	if (getopt.optind < argc)
	{
		destSet = argv[getopt.optind];
	}

	exitcode = list(destSet,attr);

	return exitcode;
}

/*===================================================================
   ROUTINE: transactionBegin
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::transactionBegin()
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());
	AES_CDH_ResultCode result = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		ACS_APGCC_Command cmd;

		cmd.cmdCode = ServR::CMD_TRANSACTIONBEGIN;
		cmd.data[0] = destinationSetName;
		// Send CMD_TRANSACTIONBEGIN to CDH and wait the answer
		result = asynchReceiver->sendCmdAndWait(cmd);
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Session towards CDH is not open!, DestinationSet:<%s>", destinationSetName.c_str());
	}
	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s> result:<%d>", destinationSetName.c_str(), result);

	return result;
}

/*===================================================================
   ROUTINE: transactionEnd
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::transactionEnd(unsigned int &applBlockNr)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	AES_CDH_ResultCode result = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		ACS_APGCC_Command cmd;
		cmd.cmdCode = ServR::CMD_TRANSACTIONEND;
		cmd.data[0] = destinationSetName;
		cmd.data[1] = (int)applBlockNr;

		result = asynchReceiver->sendCmdAndWait(cmd);

		if (result == AES_CDH_RC_OK)
		{
			// Retrieve parameters from server
			applBlockNr = (int)cmd.data[1];
			AES_CDH_TRACE_MESSAGE("current handled block:<%d>, DestinationSet:<%s>", applBlockNr, destinationSetName.c_str());
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Session towards CDH is not open!, DestinationSet:<%s>", destinationSetName.c_str());
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s> result:<%d>", destinationSetName.c_str(), result);

	return result;
}

/*===================================================================
   ROUTINE: transactionCommit
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::transactionCommit(unsigned int &applBlockNr)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	AES_CDH_ResultCode result = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		ACS_APGCC_Command cmd;

		cmd.cmdCode = ServR::CMD_TRANSACTIONCOMMIT;
		cmd.data[0] = destinationSetName;
		cmd.data[1] = (int)applBlockNr;

		result = asynchReceiver->sendCmdAndWait(cmd);

		if (result == AES_CDH_RC_OK)
		{
			// Retrieve parameters from server
			applBlockNr = (int)cmd.data[1];
			AES_CDH_TRACE_MESSAGE("current committed block:<%d>, DestinationSet:<%s>", applBlockNr, destinationSetName.c_str());
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Session towards CDH is not open!, DestinationSet:<%s>", destinationSetName.c_str());
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s> result:<%d>", destinationSetName.c_str(), result);

	return result;
}

/*===================================================================
   ROUTINE: getLastCommittedBlock
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::getLastCommittedBlock(unsigned int &applBlockNr)
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	AES_CDH_ResultCode result = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		ACS_APGCC_Command cmd;

		cmd.cmdCode = ServR::CMD_GETLASTCOMMITTEDBLOCK;
		cmd.data[0] = destinationSetName;
		cmd.data[1] = (int)applBlockNr;

		result = asynchReceiver->sendCmdAndWait(cmd);

		if (result == AES_CDH_RC_OK)
		{
			// Retrieve parameters from server
			applBlockNr = (int)cmd.data[1];
			AES_CDH_TRACE_MESSAGE("get last committed block:<%d>, DestinationSet:<%s>", applBlockNr, destinationSetName.c_str());
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Session towards CDH is not open!, DestinationSet:<%s>", destinationSetName.c_str());
	}

	AES_CDH_TRACE_MESSAGE("...Leaving DestinationSet:<%s> result:<%d>", destinationSetName.c_str(), result);

	return result;
}

/*===================================================================
   ROUTINE: transactionTerminate
=================================================================== */
AES_CDH_ResultCode AES_CDH_DestinationSetImplementation::transactionTerminate()
{
	AES_CDH_TRACE_MESSAGE("Entering..., DestinationSet:<%s>", destinationSetName.c_str());

	AES_CDH_ResultCode result = AES_CDH_RC_INCUSAGE;

	if(sessionIsOpen)
	{
		ACS_APGCC_Command cmd;

		cmd.cmdCode = ServR::CMD_TRANSACTIONTERMINATE;
		cmd.data[0] = destinationSetName;
		result = asynchReceiver->sendCmdAndWait(cmd);
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Session towards CDH is not open!, DestinationSet:<%s>", destinationSetName.c_str());
	}

	AES_CDH_TRACE_MESSAGE("...Leaving, DestinationSet:<%s> result:<%d>", destinationSetName.c_str(), result);

	return result;
}


/*===================================================================
   ROUTINE: isSessionOpen
=================================================================== */
bool AES_CDH_DestinationSetImplementation::isSessionOpen()
{
	return sessionIsOpen;
}
