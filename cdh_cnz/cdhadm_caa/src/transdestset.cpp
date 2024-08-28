/******************************************************************************/
/**
@file transdestset.cpp

Class method implementation for transdestset.h

DESCRIPTION
This class describes a destination set in the server part of CDH.

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

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
// only for _stat64 usage
#include <sys/types.h>
#include <sys/stat.h>
#include <ace/Message_Block.h>
#include <transdestset.h>
#include <servr.h>
#include <bgwrpc.h>
#include <destinationalarm.h>
#include <aes_gcc_errorcodes.h>
#include <blockpairmappermanager.h>
#include <ftpv2agent.h>
#include <parameter.h>
#include <aes_gcc_variable.h>
#include <AES_DBO_DataBlock.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;
/*===================================================================
                        DECLARATION SECTION
=================================================================== */
AES_CDH_TRACE_DEFINE(AES_CDH_TransDestSet);

/*===================================================================
   ROUTINE:TransDestSet
=================================================================== */
TransDestSet::TransDestSet() :
		activeDest(NULL),
		TempConVar(false),
		name(),
        primaryDest(NULL),
        secondaryDest(NULL),
        backupDest(NULL),
        usergrp(),
        exitThread(false),
        transferInProgress(false),
        newActiveDest(NULL),
        changeDestPtr(NULL)
{
	/*for CPPCheck errors*/
	type = AES_CDH_DestinationSet::FILE;
	usedDests.usedPrimDest = false;
	usedDests.usedSecDest = false;
	usedDests.usedBackDest = false;
	/*End for CPPCheck errors*/
}

/*===================================================================
   ROUTINE:~TransDestSet
=================================================================== */
TransDestSet::~TransDestSet()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	shutDown();
	wait();

	if (primaryDest != NULL && primaryDest->getDestSetName() == name)
	{
		primaryDest->setDestSetName("");
	}

	if (secondaryDest != NULL && secondaryDest->getDestSetName() == name)
	{
		secondaryDest->setDestSetName("");
	}

	if (backupDest != NULL && backupDest->getDestSetName() == name)
	{
		backupDest->setDestSetName("");
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
   ROUTINE:GetDateTimeString
=================================================================== */
// Returns the current date + time as a string. Usefull when debugging
//******************************************************************************
const ACE_TCHAR* TransDestSet::GetDateTimeString()
{
	static ACE_TCHAR szDateTime[64] = "";

	//SYSTEMTIME srctLocalTime;
	//::GetLocalTime(&srctLocalTime);
	/* Current system date time    */
	time_t rawtime(NULL);
	struct tm timeinfo;
	time(&rawtime);
	timeinfo = *ACE_OS::localtime(&rawtime);

	ACE_OS::sprintf(szDateTime,
			"%04d-%02d-%02d  %02d:%02d:%02d",
			(timeinfo.tm_year + 1900),
			(timeinfo.tm_mon + 1),
			timeinfo.tm_wday,
			timeinfo.tm_hour,
			timeinfo.tm_min,
			timeinfo.tm_sec);

	return szDateTime;
}

/*===================================================================
   ROUTINE:define
=================================================================== */
AES_CDH_ResultCode TransDestSet::define(const string& destSetName, 
                                        const string& primaryDestName, 
                                        const string& secondaryDestName, 
                                        const string& backupDestName, 
                                        const string& userGroup,
										const string& destSetRdn)
{
	AES_CDH_ResultCode rcode;
	ACE_UINT32 primaryAgentType;
	ACE_UINT32 secondaryAgentType;
	bool bPrimDestHasKeep = false;
	m_destSetRdn = destSetRdn;


	rcode = ServR::getDest(primaryDestName, primaryDest);
	if (rcode != AES_CDH_RC_OK)
	{
		return rcode;
	}

	// Check if primary destination is already used in a different destination set.
	if (primaryDest->getDestSetName() != "")
	{
		return AES_CDH_RC_DESTUSEDINSET;
	}

	AES_CDH_TRACE_MESSAGE("Assigned primaryAgentType as Block");

	primaryAgentType = primaryDest->getAgentType() &
			(TransferAgent::FILE |
					TransferAgent::BLOCK);
	if (primaryAgentType == TransferAgent::BLOCK)
	{
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "Setting type as BLOCK");
		AES_CDH_TRACE_MESSAGE("Setting type as BLOCK");
		type = AES_CDH_DestinationSet::BLOCK;
	}
	else
	{
		type = AES_CDH_DestinationSet::FILE;

		if (primaryDest->getAgentType() == TransferAgent::FILEBACKUP)
		{
			if (ServR::checkRespDestHasKeep(primaryDest))
			{
				bPrimDestHasKeep = true;
			}
		}
	}

	if (secondaryDestName != "")
	{
		if (bPrimDestHasKeep)
		{
			return AES_CDH_RC_ILLEGALCOMB;
		}

		if (secondaryDestName == primaryDestName)
		{
			return AES_CDH_RC_DESTUSEDINSET;
		}

		rcode = ServR::getDest(secondaryDestName, secondaryDest);
		if (rcode != AES_CDH_RC_OK)
		{
			return rcode;
		}

		// Check if secondary destination is already used in a different
		// destination set.
		if (secondaryDest->getDestSetName() != "")
		{
			return AES_CDH_RC_DESTUSEDINSET;
		}

		// Check that secondary destination is of the same type as primary
		secondaryAgentType = secondaryDest->getAgentType() &
				(TransferAgent::FILE | TransferAgent::BLOCK);

		if (secondaryAgentType != primaryAgentType)
		{
			return AES_CDH_RC_ILLEGALCOMB;
		}

		if (secondaryDest->getAgentType() == TransferAgent::FILEBACKUP)
		{
			if (ServR::checkRespDestHasKeep(secondaryDest))
			{
				return AES_CDH_RC_ILLEGALCOMB;
			}
		}
	}

	if (backupDestName != "")
	{
		if (bPrimDestHasKeep)
		{
			return AES_CDH_RC_ILLEGALCOMB;
		}

		if (backupDestName == primaryDestName || backupDestName == secondaryDestName)
		{
			return AES_CDH_RC_DESTUSEDINSET;
		}

		rcode = ServR::getDest(backupDestName, backupDest);
		if (rcode != AES_CDH_RC_OK)
		{
			return rcode;
		}

		// Check if backup destination is already used in a different
		// destination set.
		if (backupDest->getDestSetName() != "")
		{
			return AES_CDH_RC_DESTUSEDINSET;
		}

		if ((backupDest->getAgentType() & TransferAgent::BACKUP) == 0)
		{
			return AES_CDH_RC_ILLEGALCOMB;
		}

		if (ServR::checkRespDestHasKeep(backupDest))
		{
			return AES_CDH_RC_ILLEGALCOMB;
		}
	}

	primaryDest->setDestSetName(destSetName);

	if (secondaryDestName != "")
	{
		secondaryDest->setDestSetName(destSetName);
	}

	if (backupDestName != "")
	{
		backupDest->setDestSetName(destSetName);
	}

	primaryDest->switchAgent(false);
	activeDest = primaryDest;
	name = destSetName;
	usergrp = userGroup;

	open();

	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:getAttr
=================================================================== */
AES_CDH_ResultCode TransDestSet::getAttr(AES_CDH_DestinationSet::destSetAttributes& attr)
{
	attr.destSetName = name;
	attr.primDest = primaryDest->getDestinationName();

	if (secondaryDest == NULL)
	{
		attr.secDest = "";
	}
	else
	{
		attr.secDest = secondaryDest->getDestinationName();
	}

	if (backupDest == NULL)
	{
		attr.backDest = "";
	}
	else
	{
		attr.backDest = backupDest->getDestinationName();
	}

	attr.destSetType = type;
	attr.userGroup = usergrp;
	attr.active = activeDest->getDestinationName();

	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:removeDestination
=================================================================== */
AES_CDH_ResultCode TransDestSet::removeDestination(const string& destName)
{
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "Remove a destination from the destination set %s",name.c_str());
	transDestMutex.acquire();
	AES_CDH_ResultCode rcode;
	AES_CDH_Destination::destAttributes attr;
	vector<string>::iterator iter;
	bool initOrRespFlag(false);
	rcode = primaryDest->getAttr(attr); // Get attributes
	if (rcode != AES_CDH_RC_OK)
	{
		return rcode;
	}
	for (iter = attr.otherAttrs.begin(); iter != attr.otherAttrs.end(); ++iter)
	{
		if( attr.transferType == FTPTypeV2 || attr.transferType == SFTPTypeV2 )
		{
			if (*(iter) == "-c")
			{
				if ( *(iter+1) == "r")
				{
					initOrRespFlag = ServR::isRespDestSetDeleteTrig;
				}
				else if ( *(iter+1) == "i")
				{
					initOrRespFlag = ServR::isInitDestSetDeleteTrig;
				}

			}
		}
		else if( attr.transferType == BGWRPCType)
		{
			initOrRespFlag = ServR::isBlockDestSetDeleteTrig;
		} 
	}

	/* ServR::isInitDestSetDeleteTrig check has been added since this code hits for both destination set deletion and secondary destination deletion. HP63694 */
	if (destName == primaryDest->getDestinationName())
	{
		transDestMutex.release();
		return AES_CDH_RC_DESTISACTORPRIM;
	}
	if( destName == activeDest->getDestinationName() )
	{
		if( initOrRespFlag == false )
		{
			transDestMutex.release();
			return AES_CDH_RC_DESTISACTORPRIM;
		}
		else if( ServR::isDestSetAttached(name )  == true )
		{
			transDestMutex.release();
			return AES_CDH_RC_PROTECTEDDESTSET;
		}
	}

	if (secondaryDest != NULL && destName == secondaryDest->getDestinationName())
	{
		if (newActiveDest == secondaryDest)
		{
			newActiveDest = NULL;
		}
		secondaryDest->setDestSetName("");
		secondaryDest = NULL;
	}
	else if (backupDest != NULL && destName == backupDest->getDestinationName())
	{
		if (newActiveDest == backupDest)
		{
			newActiveDest = NULL;
		}
		backupDest->setDestSetName("");
		backupDest = NULL;
	}
	else
	{
		transDestMutex.release();
		return AES_CDH_RC_DESTNOTINDSET;
	}

	transDestMutex.release();
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:removeDestinationSet
=================================================================== */
AES_CDH_ResultCode TransDestSet::removeDestinationSet()
{
	DestinationAlarm::AlarmRecord::thisCause alarmCause;
	alarmCause = DestinationAlarm::AlarmRecord::destination_set_error;
	DestinationAlarm::instance()->cease(name, name, alarmCause);

	DestinationAlarm::instance()->removeDestSet(name);

	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:getDestinationSetName
=================================================================== */
string TransDestSet::getDestinationSetName(void ) const
{
	return name;
}

/*===================================================================
   ROUTINE:checkDestinationSetType
=================================================================== */
AES_CDH_ResultCode TransDestSet::checkDestinationSetType(AES_CDH_DestinationSet::TQConnectAttributes& realAttr) const
{
	if (name == "")
	{
		return AES_CDH_RC_INTPROGERR;
	}
	else
	{
		realAttr = type;
	}

	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:changeAttr
=================================================================== */
AES_CDH_ResultCode TransDestSet::changeAttr(const string& primaryDestName, 
                                            const string& secondaryDestName, 
                                            const string& backupDestName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	TransDest *tmp;
	TransDest *pdest;
	ACE_INT32 agentType;
	string changeDestName("");
	TransDest** tmpChangePtr;

	// If empty strings in prim, sec, back destination -->
	// swap prim with sec
	if (primaryDestName.empty() && secondaryDestName.empty() && backupDestName.empty())
	{
		// check that sec exists!
		if (secondaryDest != NULL)
		{
			tmp = primaryDest;
			primaryDest = secondaryDest;
			secondaryDest = tmp;

			if ((rcode = ServR::saveDestinationSets()) != AES_CDH_RC_OK)
			{
				// dest set file could not be saved,
				// swap back and return error message

				tmp = primaryDest;
				primaryDest = secondaryDest;
				secondaryDest = tmp;
				tmp = NULL;

				return rcode;
			}

			bool tempUsed;

			tempUsed = usedDests.usedPrimDest;
			usedDests.usedPrimDest = usedDests.usedSecDest;
			usedDests.usedSecDest = tempUsed;
		}
		else
		{
			// No secondary destination is defined in this destination set
			return AES_CDH_RC_DESTNOTINDSET;
		}
	}
	else
	{
		// change or add a destination
		if (!primaryDestName.empty())
		{
			changeDestName = primaryDestName;
		}
		else if (!secondaryDestName.empty())
		{
			changeDestName = secondaryDestName;
		}
		else if (!backupDestName.empty())
		{
			changeDestName = backupDestName;
		}

		// check if dest exist
		if ((rcode = ServR::getDest(changeDestName, pdest)) != AES_CDH_RC_OK)
		{
			return AES_CDH_RC_NODEST;
		}

		// check that destination is not in a dest set
		if(pdest->getDestSetName() != "")
		{
			return AES_CDH_RC_DESTUSEDINSET;
		}

		// Check if this destination is a responding with KEEP
		bool bDestHasKeep = false;
		if (pdest->getAgentType() == TransferAgent::FILEBACKUP)
		{
			if (ServR::checkRespDestHasKeep(pdest))
			{
				bDestHasKeep = true;
			}
		}

		// primary destination changes
		if (!primaryDestName.empty())
		{
			tmpChangePtr = &primaryDest;

			// If there is some responding destination using KEEP  it allows only
			// one destination in the destination set
			if (secondaryDest)
			{
				if (secondaryDest->getAgentType() == TransferAgent::FILEBACKUP)
				{
					if (ServR::checkRespDestHasKeep(secondaryDest))
					{
						return AES_CDH_RC_ILLEGALCOMB;
					}
				}
			}

			if (backupDest)
			{
				if (backupDest->getAgentType() == TransferAgent::FILEBACKUP)
				{
					if (ServR::checkRespDestHasKeep(backupDest))
					{
						return AES_CDH_RC_ILLEGALCOMB;
					}
				}
			}

			if (bDestHasKeep && (secondaryDest || backupDest))
			{
				return AES_CDH_RC_ILLEGALCOMB;
			}
		}
		else if (!secondaryDestName.empty())
		{
			// secondary destination changes
			tmpChangePtr = &secondaryDest;

			// If there is some responding destination using KEEP  it allows only
			// one destination in the destination set
			if (primaryDest)
			{
				if (primaryDest->getAgentType() == TransferAgent::FILEBACKUP)
				{
					if (ServR::checkRespDestHasKeep(primaryDest))
					{
						return AES_CDH_RC_ILLEGALCOMB;
					}
				}
			}

			if (backupDest)
			{
				if (backupDest->getAgentType() == TransferAgent::FILEBACKUP)
				{
					if (ServR::checkRespDestHasKeep(backupDest))
					{
						return AES_CDH_RC_ILLEGALCOMB;
					}
				}
			}

			if (bDestHasKeep && (primaryDest || backupDest))
			{
				return AES_CDH_RC_ILLEGALCOMB;
			}
		}
		else
		{
			tmpChangePtr = &backupDest;

			// If there is some responding destination using KEEP it allows only
			// one destination in the destination set
			if (primaryDest)
			{
				if (primaryDest->getAgentType() == TransferAgent::FILEBACKUP)
				{
					if (ServR::checkRespDestHasKeep(primaryDest))
					{
						return AES_CDH_RC_ILLEGALCOMB;
					}
				}
			}

			if (secondaryDest)
			{
				if (secondaryDest->getAgentType() == TransferAgent::FILEBACKUP)
				{
					if (ServR::checkRespDestHasKeep(secondaryDest))
					{
						return AES_CDH_RC_ILLEGALCOMB;
					}
				}
			}

			if (bDestHasKeep && (primaryDest || secondaryDest))
			{
				return AES_CDH_RC_ILLEGALCOMB;
			}
		}

		// backup destination changes
		if (!backupDestName.empty())
		{
			// check the destination type
			if ((pdest->getAgentType() & TransferAgent::BACKUP) == 0)
			{
				return AES_CDH_RC_ILLEGALCOMB;
			}
		}
		else
		{
			// check the destination type

			agentType = pdest->getAgentType() & (TransferAgent::FILE | TransferAgent::BLOCK);

			if (agentType != type)
			{
				return AES_CDH_RC_ILLEGALCOMB;
			}
		}

		transDestMutex.acquire();

		// change existing
		if (*tmpChangePtr != NULL)
		{
			bool transferActive = false;

			// destination is not the active destination
			if ((*tmpChangePtr != activeDest) || !transferInProgress)
			{
				(*tmpChangePtr)->switchAgent(false);
				(*tmpChangePtr)->setDestSetName("");
				pdest->setDestSetName(name);
			}
			else
			{
				transferActive = true;
			}

			tmp = *tmpChangePtr;
			*tmpChangePtr = pdest;

			// fail to save -> return to old
			if ((rcode = ServR::saveDestinationSets()) != AES_CDH_RC_OK)
			{
				(*tmpChangePtr)->setDestSetName("");
				*tmpChangePtr = tmp;
				(*tmpChangePtr)->setDestSetName(name);
				tmp = NULL;

				transDestMutex.release();
				return rcode;
			}

			// destination is the active destination
			if (transferActive)
			{
				*tmpChangePtr = tmp;

				(*tmpChangePtr)->switchAgent(true);
				changeDestPtr = tmpChangePtr;
				newActiveDest = pdest;

				//Check connection on all destinations
				checkAllConnections();

				transDestMutex.release();
				return AES_CDH_RC_OK;
			}
			else
			{
				if (tmp == activeDest)
				{
					activeDest = pdest;
				}
			}
		}
		else
		{
			pdest->setDestSetName(name);
			*tmpChangePtr = pdest;

			// fail to save -> return to old
			if ((rcode = ServR::saveDestinationSets()) != AES_CDH_RC_OK)
			{
				(*tmpChangePtr)->setDestSetName("");
				*tmpChangePtr = NULL;

				transDestMutex.release();
				return rcode;
			}
		}
		//Check connection on all destinations
		checkAllConnections();

		transDestMutex.release();
	}

	return rcode;
}

/*===================================================================
   ROUTINE:switchDestination
=================================================================== */
AES_CDH_ResultCode TransDestSet::switchDestination(const string& destName)
{
	TransDest* pdest;

	transDestMutex.acquire();

	// check if destination exists and if destination is in the destination set
	if ((ServR::getDest(destName, pdest)) != AES_CDH_RC_OK ||
			pdest->getDestSetName() != name)
	{
		transDestMutex.release();
		return AES_CDH_RC_DESTNOTINDSET;
	}

	// check that destination is not already active
	if (pdest == activeDest)
	{
		transDestMutex.release();
		return AES_CDH_RC_OK;
	}

	// transfer --> wait
	if (transferInProgress)
	{
		activeDest->switchAgent(true);
		newActiveDest = pdest;
	}
	// switch active destination
	else
	{
		activeDest = pdest;
		activeDest->switchAgent(false);
	}

	transDestMutex.release();

	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:sendFile
=================================================================== */
AES_CDH_ResultCode TransDestSet::sendFile(CmdHandler* cmdHdlr,
					  const ACE_UINT64 ticks,
					  ACE_UINT32 pid,
					  ACE_UINT32 destSetObj,
                                          const string fileName, 
                                          const string remoteSubDirName, 
                                          const string newFileName, 
                                          const string userUnique, 
                                          const AES_CDH_DestinationSet::transferMode trMode, 
                                          const string fileMask, const bool isDir)
{
	setEvMsg *aMsg = new(setEvMsg);
	aMsg->cmdC = TRDMSG_SENDFILE;
	aMsg->cmdHdnlr = cmdHdlr;
	aMsg->ticks    = ticks;
	aMsg->sData[0] = fileName;
	aMsg->sData[1] = remoteSubDirName;
	aMsg->sData[2] = newFileName;
	aMsg->sData[3] = userUnique;
	aMsg->sData[4] = fileMask;
	aMsg->iData[0] = pid;
	aMsg->iData[1] = destSetObj;
	aMsg->iData[2] = isDir;
	aMsg->iData[3] = trMode;

	ACE_Message_Block* mb = new ACE_Message_Block((ACE_TCHAR*)aMsg,sizeof(*aMsg));
	putq(mb);

	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:setEventHandler
=================================================================== */
AES_CDH_ResultCode TransDestSet::setEventHandler(const string& transferQueueName, 
                                                 const string& alarmText, 
                                                 CmdHandler* cmdHdlr) const
{
	SubscriberStruct *subscriber = new SubscriberStruct(cmdHdlr, transferQueueName, alarmText);
	DestinationAlarm::instance()->insertSubscriber(activeDest->getDestinationName(), subscriber, name);
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:removeEventHandler
=================================================================== */
AES_CDH_ResultCode TransDestSet::removeEventHandler(CmdHandler* cmdHdlr) const
{
    SubscriberStruct subscriber(cmdHdlr);

    DestinationAlarm::instance()->removeSubscriber(activeDest->getDestinationName(),
                                                   &subscriber);
    return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:sendRecordFile
=================================================================== */
AES_CDH_ResultCode TransDestSet::sendRecordFile(const string& streamName, 
                                                const ACE_UINT32 streamSize, 
                                                const ACE_UINT32 recordLength, 
                                                const string& mainFileName, 
                                                string fileName, 
                                                ACE_UINT64 recordsSent, 
                                                CmdHandler* cmdHdlr,
                                                const ACE_UINT64 ticks)
{
	(void) streamName;
	(void) streamSize;
	(void) recordLength;
	(void) mainFileName;
	(void) fileName;
	(void) recordsSent;
	(void) cmdHdlr;
	(void) ticks;

	setEvMsg *aMsg = new(setEvMsg);
	aMsg->cmdC = TRDMSG_SENDBLOCK;
	aMsg->cmdHdnlr = cmdHdlr;
	aMsg->ticks    = ticks;
	aMsg->sData[0] = streamName;
	aMsg->sData[1] = mainFileName;
	aMsg->sData[2] = fileName;
	aMsg->iData[2] = streamSize;
	aMsg->iData[3] = recordLength;
	aMsg->lData[0] = recordsSent;

	ACE_Message_Block* mb = new ACE_Message_Block((ACE_TCHAR*)aMsg,sizeof(*aMsg));
	putq(mb);

	AES_CDH_TRACE_MESSAGE("%s putq(TRDMSG_SENDBLOCK)", streamName.c_str());
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:getLastCommittedBlock
=================================================================== */
AES_CDH_ResultCode TransDestSet::getLastCommittedBlock(ACE_UINT32& applBlockNr)
{
    return activeDest->getLastCommittedBlock(applBlockNr);
}

/*===================================================================
   ROUTINE:transactionBegin
=================================================================== */
AES_CDH_ResultCode TransDestSet::transactionBegin()
{
    return activeDest->transactionBegin();
}

/*===================================================================
   ROUTINE:transactionEnd
=================================================================== */
AES_CDH_ResultCode TransDestSet::transactionEnd(ACE_UINT32& applBlockNr)
{
    return activeDest->transactionEnd(applBlockNr);
}

/*===================================================================
   ROUTINE:transactionCommit
=================================================================== */
AES_CDH_ResultCode TransDestSet::transactionCommit(ACE_UINT32& applBlockNr)
{
    return activeDest->transactionCommit(applBlockNr);
}

/*===================================================================
   ROUTINE:transactionTerminate
=================================================================== */
AES_CDH_ResultCode TransDestSet::transactionTerminate(void )
{
    return activeDest->transactionTerminate();
}

/*===================================================================
   ROUTINE:setAPIClosed
=================================================================== */
AES_CDH_ResultCode TransDestSet::setAPIClosed()
{// TR- HKXXX
	
	AES_CDH_ResultCode ret = activeDest->setAPIClosed();

		return ret;
}

/*===================================================================
   ROUTINE:verifyUserGroup
=================================================================== */
bool TransDestSet::verifyUserGroup(const string& userGroup) const
{
	(void)userGroup;
    return false;
}

/*===================================================================
   ROUTINE:handleMessage
=================================================================== */
void TransDestSet::handleMessage(setEvMsg* data)
{
	switch(data->cmdC)
	{
	case TRDMSG_DELETE:
	{
		AES_CDH_TRACE_MESSAGE("TRDMSG_DELETE");
		exitThread = true;
	}
	break;

	case TRDMSG_SENDFILE:
	{
		CmdHandler* cmdH  = data->cmdHdnlr;
		ACE_UINT64 ticks  = data->ticks;
		string fileName("");
		fileName           = data->sData[0];
		string remoteSubDirName("");
		remoteSubDirName   = data->sData[1];
		string newFileName("");
		newFileName        = data->sData[2];
		string userUnique("");
		userUnique         = data->sData[3];
		string fileMask("");
		fileMask          = data->sData[4];
		ACE_UINT32 pid                   = data->iData[0];
		ACE_UINT32 destSetObj            = data->iData[1];
		bool isDir                = ((data->iData[2] > 0) ? true : false);
		AES_CDH_DestinationSet::transferMode trMode = (AES_CDH_DestinationSet::transferMode)data->iData[3];

		intSendFile(cmdH,
				ticks,
				pid,
				destSetObj,
				fileName,
				remoteSubDirName,
				newFileName,
				userUnique,
				trMode,
				fileMask,
				isDir);
	}
	break;

	case TRDMSG_SENDBLOCK:
	{
		CmdHandler* cmdH          = data->cmdHdnlr;
		ACE_UINT64 ticks               = data->ticks;
		string streamName         = data->sData[0];
		string mainFileName       = data->sData[1];
		string fileName           = data->sData[2];
		ACE_UINT32 streamSize            = data->iData[2];
		ACE_UINT32 recordLength          = data->iData[3];
		ACE_UINT64 recordsSent          = data->lData[0];

		AES_CDH_TRACE_MESSAGE("%s ticks=%lu, fileName=%s, mainFileName=%s", "TRDMSG_SENDBLOCK", ticks, fileName.c_str(), mainFileName.c_str());
		if ( ServR::ExistRecordFileTask(name, mainFileName, cmdH, ticks)== false) return;

		intSendRecordFile(streamName,
				streamSize,
				recordLength,
				mainFileName,
				fileName,
				recordsSent,
				cmdH,
				ticks);
	}
	break;

	default:
		AES_CDH_TRACE_MESSAGE("TRDMSG_DEFAULT");
		break;

	} // switch
}

/*===================================================================
   ROUTINE:intSendFile
=================================================================== */
AES_CDH_ResultCode TransDestSet::intSendFile(CmdHandler* cmdHdlr,
                                             const ACE_UINT64 ticks,
                                             ACE_UINT32 pid, 
                                             ACE_UINT32 destSetObj,
                                             const string fileName, 
                                             const string remoteSubDirName, 
                                             const string newFileName, 
                                             const string userUnique, 
                                             const AES_CDH_DestinationSet::transferMode trMode, 
                                             const string fileMask,
                                             const bool isDir)
{
	AES_CDH_ResultCode rCode;

	// The threads message queue may hold several files (tasks) to send even if tasks
	// has been stopped. Therefore must stopFlag be checked before sending.
	bool stopFileTransfer = ServR::checkStopFlag(name,
			fileName,
			remoteSubDirName,
			newFileName);

	// qabjsti, INGO4PRA2 start
	bool cont = true;
	usedDests.usedPrimDest = false;
	usedDests.usedSecDest = false;
	usedDests.usedBackDest = false;
	DestinationAlarm::AlarmRecord::thisCause alarmCause;

	transferInProgress = true;

	while (cont)
	{
		if(! stopFileTransfer)
		{
			rCode = activeDest->sendFile(fileName,
					remoteSubDirName,
					newFileName,
					trMode,
					fileMask,
					isDir);
			AES_CDH_TRACE_MESSAGE("sendFile status :%d",rCode);
		}
		else
		{
			// file or dir transfer should be stopped
			rCode = AES_CDH_RC_TASKSTOPPED;
			AES_CDH_LOG(LOG_LEVEL_INFO, "sendFile status set to AES_CDH_RC_TASKSTOPPED");
			AES_CDH_TRACE_MESSAGE("sendFile status set to AES_CDH_RC_TASKSTOPPED");
		}

		if (rCode == AES_CDH_RC_LOGONFAILURE)
		{
			rCode = AES_CDH_RC_CONNECTERR;
		}

		// sending was ok or it was never initiated
		if (rCode == AES_CDH_RC_OK || rCode == AES_CDH_RC_TASKSTOPPED)
		{
			cont = false;
			if (rCode == AES_CDH_RC_OK)
			{
				alarmCause = DestinationAlarm::AlarmRecord::ok;
				DestinationAlarm::instance()->cease(activeDest->getDestinationName(),
						name,
						alarmCause);
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "Exiting while due to AES_CDH_RC_TASKSTOPPED");
				AES_CDH_TRACE_MESSAGE("Exiting while due to AES_CDH_RC_TASKSTOPPED");
			}


		}
		// sending was not ok
		else if (rCode == AES_CDH_RC_CONNECTERR || rCode == AES_CDH_RC_SENDERR)
		{
			// set alarm cause, to be used when the alarm is raised
			if (rCode == AES_CDH_RC_SENDERR)
			{
				alarmCause = DestinationAlarm::AlarmRecord::write_error;
			}
			else
			{
				alarmCause = DestinationAlarm::AlarmRecord::connect_error;
			}

			// cease alarm with another cause
			DestinationAlarm::instance()->cease(activeDest->getDestinationName(),
					name,
					alarmCause);
			// rasise alarm with new cause
			DestinationAlarm::instance()->raise(activeDest->getDestinationName(),
					name,
					alarmCause);

			// check if file or dir transfer should be stopped
			stopFileTransfer = ServR::checkStopFlag(name,
					fileName,
					remoteSubDirName,
					newFileName);

			if (!stopFileTransfer)
			{
				// try to switch to another destination in the destinationset
				cont = switchDestAuto();
			}
		}
		else
		{
			cont = false;
		}
	} //end while

	// change active destination
	switchDestManual();

	transferInProgress = false;

	// qabjsti, INGO4PRA2 stop

	if(ServR::sendFileDone(userUnique, name, fileName, remoteSubDirName, newFileName, rCode) != AES_CDH_RC_TASKSTOPPED)
	{
		ServR::cmdHandlerListMX_.acquire();
		if(ServR::validCmdHandler(cmdHdlr, ticks))
		{
			if( cmdHdlr != 0 )
			{
				cmdHdlr->sendFileReply(rCode, name, fileName, remoteSubDirName, newFileName, pid, destSetObj);
			}
		}
		ServR::cmdHandlerListMX_.release();
	}
	AES_CDH_TRACE_MESSAGE("Exiting");
	return rCode;
}

/*===================================================================
   ROUTINE:intSendRecordFile
=================================================================== */
AES_CDH_ResultCode TransDestSet::intSendRecordFile(const string& streamName, 
                                                   const ACE_UINT32 streamSize, 
                                                   const ACE_UINT32 recordLength, 
                                                   const string& mainFileName, 
                                                   string fileName, 
                                                   ACE_UINT64 recordsSent, 
                                                   CmdHandler* cmdHdlr,
                                                   const ACE_UINT64 ticks)
{
	(void)streamSize;
	(void)recordLength;
	(void) streamName;
	(void) mainFileName;
	(void) fileName;
	(void) recordsSent;
	(void) cmdHdlr;
	(void) ticks;

	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;

	//Required for block transfer after Lite AES.

	AES_DBO_DataBlock* block = NULL;

	transDestMutex.acquire();
	transferInProgress = true;
	transDestMutex.release();

	usedDests.usedPrimDest = false;
	usedDests.usedSecDest = false;
	usedDests.usedBackDest = false;
	// cmdHdlr->setInstanceOpen(); TR- XXX

	bool stopRecordTransfer=  ServR::checkStopRecordFlag(name, mainFileName);
	if(!stopRecordTransfer)
	{

		while ( rcode == AES_CDH_RC_OK ||
				rcode == AES_CDH_RC_SENDERR ||
				rcode == AES_CDH_RC_CONNECTERR ||
				rcode == AES_CDH_RC_SWITCH )
		{
			if(ServR::checkStopRecordFlag(name, mainFileName))
			{
				rcode = AES_CDH_RC_TASKSTOPPED;
				AES_CDH_LOG(LOG_LEVEL_INFO, "stopRecordTransfer is true rcode = %u ", rcode);
				AES_CDH_TRACE_MESSAGE("stopRecordTransfer is true rcode = %u ", rcode);
				break;
			}

			AES_CDH_TRACE_MESSAGE("%s intSendRecordFile", streamName.c_str());
			rcode = activeDest->sendRecordFile(streamName, mainFileName, block, cmdHdlr, ticks);
			switch (rcode)
			{

			case AES_CDH_RC_SENDERR:
			case AES_CDH_RC_CONNECTERR:
			{
				AES_CDH_TRACE_MESSAGE("AES_CDH_RC_SENDERR/AES_CDH_RC_CONNECTERR rcode = %u ", rcode);
				if (rcode == AES_CDH_RC_CONNECTERR)
				{
					// cease alarm EXCEPT with connect error
					DestinationAlarm::instance()->cease(activeDest->getDestinationName(),
							name,
							DestinationAlarm::AlarmRecord::connect_error);

					DestinationAlarm::instance()->raise(activeDest->getDestinationName(),
							name,
							DestinationAlarm::AlarmRecord::connect_error);
				}
				else
				{
					// cease alarm EXCEPT with write error
					DestinationAlarm::instance()->cease(activeDest->getDestinationName(),
							name,
							DestinationAlarm::AlarmRecord::write_error);

					DestinationAlarm::instance()->raise(activeDest->getDestinationName(),
							name,
							DestinationAlarm::AlarmRecord::write_error);
				}

				if (switchDestAuto())
				{
					rcode = AES_CDH_RC_SWITCH;

					if(NoDestAvailable())
						rcode = AES_CDH_RC_SENDERR;
					ServR::cmdHandlerListMX_.acquire();
					if ( ServR::validCmdHandler(cmdHdlr, ticks))
					{
						// Send event to application. This event is reported
						// through method eventSendRecordFile() in CDH's API
						cmdHdlr->sendRecordFileReply(rcode, fileName, recordsSent);
					}
					else
					{
						// There is no valid CmdHandler
						AES_CDH_LOG(LOG_LEVEL_INFO, "%s NO VALID CmdHandler [case AES_CDH_RC_SENDERR]", name.c_str());
						AES_CDH_TRACE_MESSAGE("%s NO VALID CmdHandler [case AES_CDH_RC_SENDERR]", name.c_str());
					}
					ServR::cmdHandlerListMX_.release();

					activeDest->switchAgent(false);
				}
				else
				{
					ServR::cmdHandlerListMX_.acquire();
					if ( ServR::validCmdHandler(cmdHdlr, ticks))
					{
						// Send event to application. This event is reported
						// through method eventSendRecordFile() in CDH's API
						cmdHdlr->sendRecordFileReply(rcode, fileName, recordsSent);
					}
					else
					{
						// There is no valid CmdHandler
						AES_CDH_LOG(LOG_LEVEL_INFO, "%s NO VALID CmdHandler [case AES_CDH_RC_SENDERR]", name.c_str());
						AES_CDH_TRACE_MESSAGE("%s NO VALID CmdHandler [case AES_CDH_RC_SENDERR]", name.c_str());

					}
					ServR::cmdHandlerListMX_.release();
					switchDestManual();

					// Checks if the transfer should stop according to the task list
					if ( ServR::checkStopRecordFlag(name, mainFileName) == true )
					{
						rcode = AES_CDH_RC_TASKSTOPPED;
					}
				}
				break;
			}


			case AES_CDH_RC_OK:
			{
				usedDests.usedPrimDest = false;
				usedDests.usedSecDest = false;
				usedDests.usedBackDest = false;
				AES_CDH_TRACE_MESSAGE("AES_CDH_RC_OK rcode = %u", rcode);
				// Checks if the transfer should stop according to the task list
				if ( ServR::checkStopRecordFlag(name, mainFileName) == true )
				{
					rcode = AES_CDH_RC_TASKSTOPPED;
				}
				else
				{
					ServR::cmdHandlerListMX_.acquire();
					if ( ServR::validCmdHandler(cmdHdlr, ticks))
					{
						// Send event to application. This event is reported
						// through method eventSendRecordFile() in CDH's API
						cmdHdlr->sendRecordFileReply(rcode, fileName, recordsSent);
					}
					else
					{
						AES_CDH_LOG(LOG_LEVEL_INFO, "%s:  NO VALID CmdHandler [case AES_CDH_RC_OK]", name.c_str());
						AES_CDH_TRACE_MESSAGE("%s:  NO VALID CmdHandler [case AES_CDH_RC_OK]", name.c_str());
					}
					ServR::cmdHandlerListMX_.release();
					DestinationAlarm::instance()->cease(activeDest->getDestinationName(),
							name,
							DestinationAlarm::AlarmRecord::ok);
				}
				break;
			}

			case AES_CDH_RC_TASKSTOPPED:
			{
				AES_CDH_TRACE_MESSAGE("AES_CDH_RC_TASKSTOPPED rcode = %u ", rcode);

				if ( ServR::checkStopRecordFlag(name, mainFileName) == true )
					rcode = AES_CDH_RC_TASKSTOPPED;
				else
				{
					ServR::cmdHandlerListMX_.acquire();
					if ( ServR::validCmdHandler(cmdHdlr, ticks))
					{
						cmdHdlr->sendRecordFileReply(rcode, fileName, recordsSent);
					}
					else
					{
						AES_CDH_LOG(LOG_LEVEL_INFO, "%s:  NO VALID CmdHandler [case AES_CDH_RC_TASKSTOPPED]", name.c_str());
						AES_CDH_TRACE_MESSAGE("%s:  NO VALID CmdHandler [case AES_CDH_RC_TASKSTOPPED]", name.c_str());
					}
					ServR::cmdHandlerListMX_.release();
				}
				break;
			}

			default:
				// Checks if the transfer should stop according to the task list
				if ( ServR::checkStopRecordFlag(name, mainFileName) == true )
				{
					rcode = AES_CDH_RC_TASKSTOPPED;
				}

				if (rcode == AES_CDH_RC_SWITCH)
				{
					switchDestManual();
				}

				break;
			}
			break;//to make the while loop break
		} // while
	}
	else
	{
		rcode = AES_CDH_RC_TASKSTOPPED;
		AES_CDH_LOG(LOG_LEVEL_INFO, "Status set to AES_CDH_RC_TASKSTOPPED");
		AES_CDH_TRACE_MESSAGE("status set to AES_CDH_RC_TASKSTOPPED");
	}

	switchDestManual();

	transDestMutex.acquire();
	transferInProgress = false;
	transDestMutex.release();
	ServR::cmdHandlerListMX_.acquire();
	if (ServR::validCmdHandler(cmdHdlr, ticks))
	{
		AES_CDH_TRACE_MESSAGE("VALID CmdHandler");
		cmdHdlr->sendRecordFileReply(rcode, fileName, recordsSent);
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR NO VALID CmdHandler");
		AES_CDH_TRACE_MESSAGE("ERROR NO VALID CmdHandler");
	}
	ServR::cmdHandlerListMX_.release();

	AES_CDH_TRACE_MESSAGE("Exiting with exitCode %u", rcode);
	ServR::sendRecordFileDone(name, mainFileName);

	return rcode;
}

/*===================================================================
   ROUTINE:open
=================================================================== */
ACE_UINT32 TransDestSet::open(void )
{
    exitThread = false;
	return this->activate(THR_NEW_LWP| THR_JOINABLE);
}

/*===================================================================
   ROUTINE: svc
=================================================================== */
ACE_INT32 TransDestSet::svc(void )
{
	ACE_Message_Block* mb;
	setEvMsg *data;
	AES_CDH_TRACE_MESSAGE("THREAD started for dest set = %s", name.c_str());
	do
	{
		if (getq(mb) >= 0)
		{
			AES_CDH_TRACE_MESSAGE("TransDestSet::svc new msg for dest set = %s", name.c_str());
			data = (setEvMsg*)mb->base();
			handleMessage(data);
			delete data;
			mb->release();
		}

	}
	while(!exitThread);

	AES_CDH_TRACE_MESSAGE("THREAD exited for dest set = %s", name.c_str());

	return 0;
}

/*===================================================================
   ROUTINE:shutDown
=================================================================== */
void TransDestSet::shutDown(void )
{
	setEvMsg *aMsg = new(setEvMsg);
	aMsg->cmdC = TRDMSG_DELETE;
	ACE_Message_Block* mb = new ACE_Message_Block((ACE_TCHAR*)aMsg,sizeof(*aMsg));
	putq(mb);
}

/*===================================================================
   ROUTINE:switchDestAuto
=================================================================== */
bool TransDestSet::switchDestAuto()
{
	transDestMutex.acquire();

	// Primary failed
	if (primaryDest == activeDest)
	{
		if ((secondaryDest != NULL) && (usedDests.usedSecDest == false))
		{
			// There is a secondary which has not been used
			usedDests.usedPrimDest = true;
			activeDest = secondaryDest;
			AES_CDH_LOG(LOG_LEVEL_INFO, "PRIMARY --> SECONDARY");
			AES_CDH_TRACE_MESSAGE("PRIMARY --> SECONDARY");
		}
		else if ((backupDest != NULL) && (usedDests.usedBackDest == false))
		{
			// There is a backup which has not been used
			usedDests.usedPrimDest = true;
			activeDest = backupDest;
			AES_CDH_LOG(LOG_LEVEL_INFO, "PRIMARY --> BACKUP");
			AES_CDH_TRACE_MESSAGE("PRIMARY --> BACKUP");
		}
		else
		{
			// All existing destinations in the destination set have been used once
			usedDests.usedPrimDest = false;
			usedDests.usedSecDest  = false;
			usedDests.usedBackDest = false;
			transDestMutex.release();
			AES_CDH_LOG(LOG_LEVEL_INFO, "PRIMARY active, returning false");
			AES_CDH_TRACE_MESSAGE("PRIMARY active, returning false");
			return false;
		}
	}
	else if (secondaryDest == activeDest)    // Secondary failed
	{
		if ((backupDest != NULL) && (usedDests.usedBackDest == false))
		{
			// There is a backup which has not been used
			usedDests.usedSecDest = true;
			activeDest = backupDest;
			AES_CDH_LOG(LOG_LEVEL_INFO, "SECONDARY --> BACKUP");
			AES_CDH_TRACE_MESSAGE("SECONDARY --> BACKUP");
		}

		else if (usedDests.usedPrimDest == false)
		{
			// Primary has not been used
			usedDests.usedSecDest = true;
			activeDest = primaryDest;
			AES_CDH_LOG(LOG_LEVEL_INFO, "SECONDARY --> PRIMARY");
			AES_CDH_TRACE_MESSAGE("SECONDARY --> PRIMARY");
		}

		else
		{
			// All existing destinations in the destination set have been used once
			usedDests.usedPrimDest = false;
			usedDests.usedSecDest  = false;
			usedDests.usedBackDest = false;
			transDestMutex.release();
			AES_CDH_LOG(LOG_LEVEL_INFO, "SECONDARY active, returning false");
			AES_CDH_TRACE_MESSAGE("SECONDARY active, returning false");
			return false;
		}
	}
	else if (usedDests.usedPrimDest == false)   // Backup failed
	{
		// Primary has not been used
		usedDests.usedBackDest = true;
		activeDest = primaryDest;
		AES_CDH_LOG(LOG_LEVEL_INFO, "BACKUP --> PRIMARY");
		AES_CDH_TRACE_MESSAGE("BACKUP --> PRIMARY");
	}
	else if ((secondaryDest != NULL) && (usedDests.usedSecDest == false))
	{
		// There is a secondary which has not been used
		usedDests.usedBackDest = true;
		activeDest = secondaryDest;
		AES_CDH_LOG(LOG_LEVEL_INFO, "BACKUP --> SECONDARY");
		AES_CDH_TRACE_MESSAGE("BACKUP --> SECONDARY");
	}
	else
	{
		// All existing destinations in the destination set have been used once
		usedDests.usedPrimDest = false;
		usedDests.usedSecDest  = false;
		usedDests.usedBackDest = false;
		transDestMutex.release();
		AES_CDH_LOG(LOG_LEVEL_INFO, "BACKUP active, returning false");
		AES_CDH_TRACE_MESSAGE("BACKUP active, returning false");
		return false;
	}

	transDestMutex.release();
	return true;
}

/*===================================================================
   ROUTINE:switchDestManual
=================================================================== */
void TransDestSet::switchDestManual(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	transDestMutex.acquire();

	if (newActiveDest != NULL)
	{		
		if (changeDestPtr != NULL) 
		{
			(*changeDestPtr)->setDestSetName("");
			*changeDestPtr = NULL;
			newActiveDest->setDestSetName(name);
			*changeDestPtr = newActiveDest;
		}

		activeDest = newActiveDest;
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "Switch destinatgion manually for destset %s",name.c_str());
		activeDest->switchAgent(false);
	}

	newActiveDest = NULL;
	changeDestPtr = NULL;

	transDestMutex.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
   ROUTINE:checkAllConnections
=================================================================== */
void TransDestSet::checkAllConnections()
{
	if (primaryDest != NULL)
		primaryDest->checkCon();
	if (secondaryDest != NULL)
		secondaryDest->checkCon();
	if (backupDest != NULL)
		backupDest->checkCon();
}
//HI58671
/*===================================================================
   ROUTINE:NoDestAvailable
=================================================================== */
bool TransDestSet::NoDestAvailable()
{
	AES_CDH_ResultCode retCode1=AES_CDH_RC_CONNECTERR, retCode2=AES_CDH_RC_CONNECTERR, retCode3=AES_CDH_RC_CONNECTERR;

	if (primaryDest != NULL)
	{
		primaryDest->CheckDestAvailable();
		retCode1 = primaryDest->GetDestAvailable();
	}
	if (secondaryDest != NULL)
	{
		secondaryDest->CheckDestAvailable();
		retCode2 = secondaryDest->GetDestAvailable();
	}
	if (backupDest != NULL)
	{
		backupDest->CheckDestAvailable();
		retCode3 = backupDest->GetDestAvailable();
	}
	if( retCode1 == AES_CDH_RC_CONNECTERR && retCode2 == AES_CDH_RC_CONNECTERR && retCode3== AES_CDH_RC_CONNECTERR)
	{
		TempConVar = true;
	}
	else 
		TempConVar = false;

	AES_CDH_TRACE_MESSAGE("retCode1 = %u retCode2 = %u retCode3 = %u TempConVar =%u", retCode1, retCode2, retCode3, TempConVar);
	return TempConVar;

}
void TransDestSet::getDestSetRdn(string& destSetRdn ) 
{
	//Copy the destination set RDN into the string.
	//destination rdn is of form initiating/respondingDestinationSetId
	destSetRdn = m_destSetRdn ;
}
