/*=================================================================== */
/**
@file destinationalarm.cpp

Class method implementation for destinationalarm.h

DESCRIPTION
DestinationAlarm class serves as the API to the alarm handling functionality.It uses a nested class (AlarmRecord). There shall be one alarm
record for each destination. This alarm record stores all
subscribers (if any) for that destination.

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
#include <aes_gcc_event.h>
#include <aes_gcc_eventhandler.h>
#include <destinationalarm.h>
#include <event.h>
#include <ACS_CS_API_Common_R1.h>
#include <ACS_CS_API_NetworkElement_R1.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_DestinationAlarm);

// Initialize static variable
DestinationAlarm *DestinationAlarm::instance_ = NULL;
ACE_Recursive_Thread_Mutex DestinationAlarm::subscriberListMX_;
ACE_Mutex DestinationAlarm::destAlarmInstMx_;
string DestinationAlarm::myDestSetNm_ = "";
/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
// Initialize static constants
//##ModelId=3E2FF36A0300
const string DestinationAlarm::AlarmRecord::cause1_ = "DATA OUTPUT, AP TRANSMISSION FAULT";
const string DestinationAlarm::AlarmRecord::cause2_ = "DATA OUTPUT, AP TRANSMISSION FAULT";
//##ModelId=3DE49C1E03DB
const ACE_INT32 DestinationAlarm::AlarmRecord::eventCode_ = AES_CDH_ConnectFault;

const ACE_INT32 DestinationAlarm::AlarmRecord::eventCode1_ = AES_CDH_ReadyDirUsageLimExced; //For CDH Ready Directory Polling Functionality.
//##ModelId=3E2FF3110352
const string DestinationAlarm::AlarmRecord::destAlarmText_ = "The connection to the remote host lost or write access denied";
const string DestinationAlarm::AlarmRecord::destSetAlarmText_ = "All destinations in the destination set has failed";
//##ModelId=3E257092015B
const string DestinationAlarm::AlarmRecord::problemText1_ = "CONNECTION LOST TO REMOTE SYSTEM";
//##ModelId=3E2689EC0100
const string DestinationAlarm::AlarmRecord::problemText2_ = "WRITE FAILURE";
const string DestinationAlarm::AlarmRecord::problemText3_ = "NO REMOTE SYSTEM CONNECTION";
const string DestinationAlarm::AlarmRecord::Vcause_ = "DESTINATION RECOVERY PROBLEM";	// added for TR HH91483
const string DestinationAlarm::AlarmRecord::VAlarmText_ = "Virtual directory does not exist";	// added for TR HH91483
const string DestinationAlarm::AlarmRecord::SUcause_	=	"CDH RESPONDING READY DIRECTORY WARNING";  //For CDH Ready Directory Polling Functionality.
const string DestinationAlarm::AlarmRecord::SUAlarmText_ =	 "The Directory has been filled to pre-defined limit or it contains files older than pre-defined limit";  //For CDH Ready Directory Polling Functionality.
const string DestinationAlarm::AlarmRecord::SUproblemText4_ = "communication failure between APG and remote host";  //For CDH Ready Directory Polling Functionality.
//const string RefObject = "CDH";
const string readyDirName   = "Ready";

// **************************************
// *                                    *
// *  SubscriberStruct Implementation   *
// *                                    *
// **************************************

/*===================================================================
   ROUTINE:SubscriberStruct
=================================================================== */
SubscriberStruct::SubscriberStruct(CmdHandler *cmdHdlr) : cmdH_(cmdHdlr),
    alarmText_(""), fileDestination_(""), bConnectErrSent_(false)
{}


/* **********************************************************************
ROUTINE:SubscriberStruct
 ***********************************************************************/
SubscriberStruct::SubscriberStruct(CmdHandler *cmdHdlr,
                                   const string &fileDestination,
                                   const string &alarmText)
    : cmdH_(cmdHdlr), alarmText_(alarmText),
      fileDestination_(fileDestination), bConnectErrSent_(false)
{}


/*===================================================================
   ROUTINE:~SubscriberStruct
=================================================================== */
SubscriberStruct::~SubscriberStruct()
{}


/*===================================================================
   ROUTINE:getAlarmText
=================================================================== */
string SubscriberStruct::getAlarmText()
{
    return alarmText_;
}


/*===================================================================
   ROUTINE:getFileDestination
=================================================================== */
string SubscriberStruct::getFileDestination()
{
    return fileDestination_;
}


/*===================================================================
   ROUTINE:getCmdHandler
=================================================================== */
CmdHandler *SubscriberStruct::getCmdHandler()
{
    return cmdH_;
}


/*===================================================================
   ROUTINE:getConnectErrSent
=================================================================== */
bool SubscriberStruct::getConnectErrSent() const
{
    return bConnectErrSent_;
}


/*===================================================================
   ROUTINE:setConnectErrSent
=================================================================== */
void SubscriberStruct::setConnectErrSent(const bool status)
{
    bConnectErrSent_ = status;
}

// **************************************
// *                                    *
// *  AlarmRecord Implementation        *
// *                                    *
// **************************************

/*===================================================================
   ROUTINE:AlarmRecord
=================================================================== */
//##ModelId=3DE49C1F0133
DestinationAlarm::AlarmRecord::AlarmRecord(const string &dest)
    : dest_(dest), bAlarmRaised_(false), problemText_(""), oldCause(ok_prev),
    cause_(cause1_), defaultAlarmText_(destAlarmText_)
{}
void getAP(string &ap)
{
	ACS_DSD_Session session;
	ACS_DSD_Node node;
	session.get_local_node(node);
	ap = node.system_name ;
	return ;
}

void getHostname(std::string &outputHostname)
{
	char hostName[1024];
	size_t len = 1024;
	if(ACE_OS::hostname(hostName, len) < 0)
	{
		hostName[0] = '\0';
	}

	outputHostname = "";
	for(int i=0; i<1024 && hostName[i] != '\0'; i++)
	{
		outputHostname  += hostName[i];
	}
}

void getNodeLetter(std::string &outputNodeLetter)
{
	FILE *fp = 0;
	char buff[512];
	int nodeId  = 0;
	outputNodeLetter =  "";

	/* popen creates a pipe so we can read the output
        of the program we are invoking */
	if (!(fp = popen("cat /etc/opensaf/slot_id" ,"r")))
	{
		return ;
	}

	/* read the output of route, one line at a time */
	if(fgets(buff, sizeof(buff), fp) != NULL )
	{
		nodeId = atoi(&buff[0]);
	}
	/* close the pipe */
	pclose(fp);

	if (nodeId == 1)
	{
		outputNodeLetter =  "A";
	}
	else if (nodeId == 2)
	{
		outputNodeLetter =  "B";
	}
}

/*===================================================================
   ROUTINE:createProblemText
=================================================================== */
//##ModelId=3E243EDC0369
bool DestinationAlarm::AlarmRecord::createProblemText( thisCause  alarmCause,
									std::string GlbDestPath, ACE_UINT64 CDH_Ready_File_Number, 
									ACE_INT64 OldestFileTimeStamp )
{
	AES_CDH_TRACE_MESSAGE("Entering");
	problemText_ = "";
	bool retStatus = true;
	string myObjOfRef_DestSet("");
	// remove _filenotification if present
	std::string tmpDest("");
	ACE_INT32 pos(0);
	pos = dest_.find_first_of("_");
	if (pos != -1)
		tmpDest = dest_.substr(0, pos);
	else
		tmpDest = dest_;

	if(!(myDestSetNm_.empty())) 
	{
		retStatus = ServR::getObjectOfRef( "DESTINATIONSET",myDestSetNm_, myObjOfRef_DestSet );
		if (retStatus == false)
			return false;
	}


	if (alarmCause == SU_error) 
	{
		std::string vDirectory(" ");
		std::string vOldestFileTimeStampCstr = "00:00:00";
		ACE_TCHAR vFileNum[15];
		std::string vPath(" ");

		if (CDH_Ready_File_Number)
			vOldestFileTimeStampCstr = ctime(&(OldestFileTimeStamp));
		vDirectory = readyDirName;
		ACE_OS::sprintf(vFileNum, "%lu", CDH_Ready_File_Number);
		vPath = GlbDestPath;

		problemText_ = "DIRECTORY\tPATH";
		// Adding number of tab spaces to align FILES and OLDESTFILE with vFileNum and
		// vOldestFileTimeStampCstr values
		for(unsigned int i = 0; i <= (vPath.length()/8); ++i)
			problemText_.append("\t");
		problemText_.append("FILES\tOLDESTFILE \n" +
				vDirectory + "\t\t" + vPath + "\t" + vFileNum +
				"\t" + vOldestFileTimeStampCstr + "\n\nProbable cause is "
				+ SUproblemText4_);

	}
	else if(alarmCause == vd_error)
	{
		string myObjOfRef("");
		retStatus = ServR::getObjectOfRef(  "DESTINATION", tmpDest, myObjOfRef);
		if (retStatus == false)
			return false;
		problemText_ = "DESTINATION\n" + myObjOfRef + tmpDest + "\n\n" + "CAUSE\n" +
				VAlarmText_;
	}
	else if (alarmCause == connect_error || alarmCause == connect_error_fn)
	{
		AES_CDH_TRACE_MESSAGE("Connect error for DESTINATION");

		string destSetNm_("");

		string myObjOfRef("");

		retStatus = ServR::getObjectOfRef( "DESTINATION",tmpDest, myObjOfRef );
		if (retStatus == false)
			return false;

		cause_ = cause1_;
		defaultAlarmText_ = destAlarmText_;
		problemText_.append("\n\nCAUSE\n");
		problemText_.append("DATA TRANSFER, DESTINATION: ");
		problemText_.append(problemText1_);
		problemText_.append("\n\nFILE NAME\n-\n" );
		problemText_.append("\nTRANSFER QUEUE\n-\n");
		problemText_.append("\nDESTINATION SET\n");
		//problemText_.append("\nDESTINATION SET\n-\n");
		problemText_.append(myObjOfRef_DestSet);
		problemText_.append(myDestSetNm_);
		problemText_.append("\n");
		problemText_.append("\nDESTINATION\n");
		problemText_.append(myObjOfRef);
		problemText_.append(tmpDest);
		problemText_.append("\n");
		AES_CDH_TRACE_MESSAGE("Problem Text  = %s",problemText_.c_str());
	}
	else if (alarmCause == destination_set_error) //HK75918:Alarm text for destinationset
	{
		AES_CDH_TRACE_MESSAGE("Connect error for DESTINATION SET");
		cause_ = cause2_;
		defaultAlarmText_ = destSetAlarmText_;

		string myObjOfRef("");
		retStatus = ServR::getObjectOfRef( "DESTINATIONSET", tmpDest,myObjOfRef ); 
		if (retStatus == false)
			return false;


		problemText_.append("\n\nCAUSE\n");
		problemText_.append("DATA TRANSFER, DESTINATION SET: ");
		problemText_.append(problemText1_);
		problemText_.append("\n\nFILE NAME\n-\n" );
		problemText_.append("\nTRANSFER QUEUE\n-\n");
		problemText_.append("\nDESTINATION SET\n");
		problemText_.append(myObjOfRef);
		problemText_.append(tmpDest);
		problemText_.append("\n\nDESTINATION");
		problemText_.append("\n-");
		AES_CDH_TRACE_MESSAGE("Problem Text  = %s",problemText_.c_str());

	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Connect error for WRITE FAILED");
		cause_ = cause1_;
		defaultAlarmText_ = destAlarmText_;

		string myObjOfRef("");
		retStatus = ServR::getObjectOfRef( "DESTINATION",tmpDest, myObjOfRef );

		if (retStatus == false)
			return false;
		problemText_.append("\n\nCAUSE\n");
		problemText_.append("DATA TRANSFER, DESTINATION: ");
		problemText_.append(problemText2_);
		problemText_.append("\n\nFILE NAME\n-\n" );
		problemText_.append("\nTRANSFER QUEUE\n-\n");
		problemText_.append("\nDESTINATION SET\n");
		problemText_.append(myObjOfRef_DestSet);
		problemText_.append(myDestSetNm_);
		problemText_.append("\n");
		problemText_.append("\nDESTINATION\n");
		problemText_.append(myObjOfRef);
		problemText_.append(tmpDest);
		problemText_.append("\n");
		AES_CDH_TRACE_MESSAGE("Problem Text = %s",problemText_.c_str());
		AES_CDH_TRACE_MESSAGE("Problem data = %s",defaultAlarmText_.c_str());

	}
	return true;
}

/*===================================================================
   ROUTINE:cease
=================================================================== */
bool DestinationAlarm::AlarmRecord::cease( const std::string &destSet,
					   thisCause alarmCause,
					   std::string GlbDestPath, ACE_UINT64 CDH_Ready_File_Number, 
					   ACE_INT64 OldestFileTimeStamp )
{
	AES_CDH_TRACE_MESSAGE("Entering with destSet:%s, alarmCause:%d",destSet.c_str(),alarmCause);

	AES_GCC_Event event;
	string myObjOfRef("");
	bool retStatus = true;
	myDestSetNm_=destSet;
	if  (alarmCause == destination_set_error)
	{
		retStatus =  ServR::getObjectOfRef("DESTINATIONSET", destSet,myObjOfRef);
		if (retStatus == false)
			return false;
		myObjOfRef +=destSet;
	}
	else
	{
		std::string tmpDest("");
		ACE_INT32 pos(0);

		pos = dest_.find_first_of("_");
		if (pos != -1)
			tmpDest = dest_.substr(0, pos);
		else
			tmpDest = dest_;
		retStatus = ServR::getObjectOfRef( "DESTINATION", tmpDest ,myObjOfRef );
		if (retStatus == false)
			return false;
		myObjOfRef +=tmpDest;
	}
	if(alarmCause == SU_error) //For CDH Ready Directory Polling Functionality.
	{
		retStatus = createProblemText(alarmCause,
				GlbDestPath,CDH_Ready_File_Number,OldestFileTimeStamp);
		if(retStatus == true)
		{
			event.setProbableCause(SUcause_.c_str());		
			event.setProblemText(problemText_.c_str());
			event.setProblemData(SUAlarmText_.c_str());
			event.setEventCode(eventCode1_);
			event.setObjectOfReference(myObjOfRef.c_str());
			event.setKind(AES_GCC_Event::EV_ALARM);	
			AES_GCC_EventHandler::instance().cease(event);
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("no need to raise alarm as destination is not found in the transdest list ");
			problemText_ = "";
			return false ;
		}
	}
	else if(alarmCause == vd_error ) // added for TR HH91483
	{
		event.setProbableCause(Vcause_.c_str());		
		event.setProblemText(problemText_.c_str());
		event.setProblemData(problemText3_.c_str());
		event.setEventCode(eventCode_);
		event.setObjectOfReference(myObjOfRef.c_str());
		event.setKind(AES_GCC_Event::EV_ALARM);	
		AES_GCC_EventHandler::instance().cease(event);
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("else part of ::AlarmRecord::cease");
		event.setProbableCause(cause_.c_str());
		event.setProblemText(problemText_.c_str());
		event.setProblemData(defaultAlarmText_.c_str());
		event.setEventCode(eventCode_);
		event.setObjectOfReference(myObjOfRef.c_str());
		event.setKind(AES_GCC_Event::EV_ALARM);
	}

	subscriberListMX_.acquire();
	std::list<SubscriberStruct *>::iterator itr;
	itr = subscriberList_.begin();

	if (bAlarmRaised_)       // Only cease if there is an alarm
	{
		// only cease if destination is OK OR if the alarm shall be raised with another cause
		if ((alarmCause == ok) ||
				(alarmCause == connect_error && oldCause == write_error_prev) ||
				(alarmCause == write_error && oldCause == connect_error_prev) ||
				(alarmCause == destination_set_error))
		{
			while (itr != subscriberList_.end())
			{
				string problemData = (*itr)->getAlarmText();
				event.setProblemData(problemData.c_str());

				AES_GCC_EventHandler::instance().cease(event);
				bAlarmRaised_ = false;
				oldCause = ok_prev;
				++itr;
			}

			if (bAlarmRaised_)
			{

				AES_GCC_EventHandler::instance().cease(event);
				bAlarmRaised_ = false;
				oldCause = ok_prev;
			}

			if (destSet != "")
			{
				// Send the result to all subscribers...
				itr = subscriberList_.begin();
				while (itr != subscriberList_.end())
				{
					SubscriberStruct *subscriber = (*itr);
					if ( subscriber->getConnectErrSent())
					{
						CmdHandler *cmdH = subscriber->getCmdHandler();
						if( cmdH != NULL )
						{
							ServR::cmdHandlerListMX_.acquire();
							if( ServR::checkCmdHandler( cmdH) == true )
							{
								cmdH->sendDestEvent(AES_CDH_RC_CONNECTOK, destSet);
								subscriber->setConnectErrSent(false);
							}
							ServR::cmdHandlerListMX_.release();
						}
					}

					++itr;
				}
			}
			subscriberListMX_.release();
			return true;
		}
		else
		{
			subscriberListMX_.release();
			return false;
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	subscriberListMX_.release();
	return false;
}


/*===================================================================
   ROUTINE:raise
=================================================================== */
void DestinationAlarm::AlarmRecord::raise(const std::string &destSet, 
					  thisCause alarmCause,
					  std::string GlbDestPath, 
					  ACE_UINT64 CDH_Ready_File_Number, 
					  ACE_INT64 OldestFileTimeStamp)
{
	subscriberListMX_.acquire();
	std::list<SubscriberStruct *>::iterator itr;
	itr = subscriberList_.begin();
	bool retStatus = true;
	myDestSetNm_ = destSet;
	if (! bAlarmRaised_)
	{
		string myObjOfRef("");
		if  (alarmCause == destination_set_error)
		{
			ServR::getObjectOfRef( "DESTINATIONSET", destSet,myObjOfRef );
			myObjOfRef +=destSet;
		}
		else
		{
			std::string tmpDest("");
			ACE_INT32 pos(0);

			pos = dest_.find_first_of("_");
			if (pos != -1)
				tmpDest = dest_.substr(0, pos);
			else
				tmpDest = dest_;
			ServR::getObjectOfRef( "DESTINATION", tmpDest ,myObjOfRef );
			myObjOfRef +=tmpDest;
		}
		retStatus = createProblemText(alarmCause,
				GlbDestPath,CDH_Ready_File_Number,OldestFileTimeStamp);
		if(retStatus == false)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "no need to raise alarm as destination is not found in the transdest list ");
			subscriberListMX_.release();
			return ;
		}


		AES_GCC_Event event;
		if(alarmCause == SU_error)  //For CDH Ready Directory Polling Functionality.
		{
			event.setProbableCause(SUcause_.c_str());
			event.setProblemText(problemText_.c_str());
			event.setProblemData(SUAlarmText_.c_str());
			event.setEventCode(eventCode1_);
			event.setObjectOfReference(myObjOfRef.c_str());
			event.setKind(AES_GCC_Event::EV_ALARM);
		}
		else if (alarmCause == vd_error ) // added for TR HH91483
		{
			event.setProbableCause(Vcause_.c_str());
			event.setProblemText(problemText_.c_str());
			event.setProblemData(problemText3_.c_str());
			event.setEventCode(eventCode_);
			event.setObjectOfReference(myObjOfRef.c_str());
			event.setKind(AES_GCC_Event::EV_ALARM);
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("in else part of  AlarmRecord::raise");
			//AES_CDH_LOG(LOG_LEVEL_DEBUG, "%s", " defaultAlarmText_ = %s",defaultAlarmText_.c_str()));
			event.setProbableCause(cause_.c_str());
			event.setProblemText(problemText_.c_str());
			event.setProblemData(defaultAlarmText_.c_str());
			event.setEventCode(eventCode_);
			event.setObjectOfReference(myObjOfRef.c_str());
			event.setKind(AES_GCC_Event::EV_ALARM);
		}

		while (itr != subscriberList_.end())
		{
			string problemData = (*itr)->getAlarmText();
			if( !problemData.empty())
			{
				AES_CDH_TRACE_MESSAGE(" problemData after getAlarmText = %s",problemData.c_str());
				event.setProblemData(problemData.c_str());
			}

			if(alarmCause == SU_error)  //For CDH Ready Directory Polling Functionality.
			{
				AES_GCC_EventHandler::instance().alarm(event, "A2");
			}
			else if (alarmCause == vd_error)		// added for TR HH91483
			{
				AES_GCC_EventHandler::instance().alarm(event, "A3");
			}
			else if (alarmCause == connect_error)
			{
				AES_GCC_EventHandler::instance().alarm(event, "A3");
				bAlarmRaised_ = true;
				oldCause = connect_error_prev;
			}
			else if (alarmCause == connect_error_fn)
			{
				AES_GCC_EventHandler::instance().alarm(event, "A3");
				bAlarmRaised_ = true;
			}
			else if (alarmCause == destination_set_error)
			{
				AES_GCC_EventHandler::instance().alarm(event, "A2");
				bAlarmRaised_ = true;
			}
			else // write error
			{
				event.setManualCease(true);
				AES_GCC_EventHandler::instance().alarm(event, "A3");
				bAlarmRaised_ = true;
				oldCause = write_error_prev;
			}
			++itr;
		}

		if (! bAlarmRaised_)
		{
			if(alarmCause == SU_error)  //For CDH Ready Directory Polling Functionality.
			{
				AES_GCC_EventHandler::instance().alarm(event, "A2");
				oldCause = SU_error_prev;
			}
			else if (alarmCause == vd_error)		// added for TR HH91483
			{
				AES_GCC_EventHandler::instance().alarm(event, "A3");
				oldCause = vd_error_prev;		// added for TR HH91483
			}
			else if (alarmCause == connect_error)
			{

				AES_GCC_EventHandler::instance().alarm(event, "A3");
				bAlarmRaised_ = true;
				oldCause = connect_error_prev;
			}
			else if (alarmCause == connect_error_fn)
			{
				AES_GCC_EventHandler::instance().alarm(event, "A3");
				bAlarmRaised_ = true;
			}
			else if (alarmCause == destination_set_error)
			{
				AES_GCC_EventHandler::instance().alarm(event, "A2");
				bAlarmRaised_ = true;
			}
			else
			{
				event.setManualCease(true);
				AES_GCC_EventHandler::instance().alarm(event, "A3");
				bAlarmRaised_ = true;
				oldCause = write_error_prev;
			}
		}

		if (alarmCause == connect_error && destSet != "")
		{
			// Send the result to all subscribers...
			itr = subscriberList_.begin();
			while (itr != subscriberList_.end())
			{
				SubscriberStruct *subscriber = (*itr);
				if (! subscriber->getConnectErrSent())
				{
					CmdHandler *cmdH = subscriber->getCmdHandler();
					if( cmdH != NULL )
					{
						ServR::cmdHandlerListMX_.acquire();
						if( ServR::checkCmdHandler( cmdH) == true )
						{
							cmdH->sendDestEvent(AES_CDH_RC_CONNECTERR, destSet);
							subscriber->setConnectErrSent(true);
						}
						ServR::cmdHandlerListMX_.release();
					}
				}

				++itr;
			}
		}
	}
	subscriberListMX_.release();
}


/*===================================================================
   ROUTINE:insertSubscriber
=================================================================== */
//##ModelId=3DE49C1F015B
void
DestinationAlarm::AlarmRecord::insertSubscriber(SubscriberStruct *sub,
												const std::string &destSet)
{
	bool bAlreadyIn = false;
	std::list<SubscriberStruct *>::iterator itr;
	subscriberListMX_.acquire();

	itr = subscriberList_.begin();
	while ( itr != subscriberList_.end())
	{
		if ( *itr == sub )
		{
			// The subscriber is already present in the list,
			// thus, we do not insert it a second time.
			bAlreadyIn = true;
		}

		++itr;
	}

	if ( bAlarmRaised_ )
	{
		// Send reply immediately if the link is down


		CmdHandler *cmdH = sub->getCmdHandler();
		if( cmdH != NULL )
		{
			ServR::cmdHandlerListMX_.acquire();
			if( ServR::checkCmdHandler( cmdH) == true )
			{
				sub->getCmdHandler()->sendDestEvent(AES_CDH_RC_CONNECTERR, destSet);
				sub->setConnectErrSent(true);
			}
			ServR::cmdHandlerListMX_.release();
		}
	}

	if (! bAlreadyIn )
	{
		subscriberList_.push_back(sub);
	}
	subscriberListMX_.release();
}



/*===================================================================
   ROUTINE:removeSubscriber
=================================================================== */
//##ModelId=3DE49C1F0179
void
DestinationAlarm::AlarmRecord::removeSubscriber(SubscriberStruct *sub)
{
	std::list<SubscriberStruct *>::iterator itr;
	subscriberListMX_.acquire();
	itr = subscriberList_.begin();
	while ( itr != subscriberList_.end() )
	{
		if ( (*itr)->getCmdHandler() == sub->getCmdHandler() )
		{
			SubscriberStruct * temp = *itr;
			subscriberList_.erase(itr);
			delete temp;
			break;
		}
		++itr;
	}
	subscriberListMX_.release();
}


/*===================================================================
   ROUTINE:destroy
=================================================================== */
//##ModelId=3DE49C1F018D
void DestinationAlarm::AlarmRecord::destroy()
{
	std::list<SubscriberStruct *>::iterator itr;
	subscriberListMX_.acquire();
	itr = subscriberList_.begin();
	while ( itr != subscriberList_.end() )
	{
		// Delete the item
		delete *itr;

		// Erase it from the list
		subscriberList_.erase(itr);

		// Reposition the iterator since it may be invalid
		itr = subscriberList_.begin();
	}
	subscriberListMX_.release();
}



// **************************************
// *                                    *
// *  DestinationAlarm Implementation   *
// *                                    *
// **************************************


/*===================================================================
   ROUTINE:DestinationAlarm
=================================================================== */
//##ModelId=3DE49C1E01B4
DestinationAlarm::DestinationAlarm()
{
}


/*===================================================================
   ROUTINE:~DestinationAlarm
=================================================================== */
//##ModelId=3DE49C1E01C8
DestinationAlarm::~DestinationAlarm()
{}


/*===================================================================
   ROUTINE:instance
=================================================================== */
//##ModelId=3DE49C1E027C
DestinationAlarm* DestinationAlarm::instance()
{
	destAlarmInstMx_.acquire();
	if ( instance_ == NULL )
	{
		instance_ = new DestinationAlarm();
	}
	destAlarmInstMx_.release();
	return instance_;
}


/*===================================================================
   ROUTINE:insertSubscriber
=================================================================== */
//##ModelId=3DE49C1E0287
void DestinationAlarm::insertSubscriber(const std::string &dest,
                                        SubscriberStruct *sub,
					const std::string &destSet)
{
	std::map<std::string, AlarmRecord *>::iterator itr;
	mutex_.acquire();
	itr = destMap_.find(dest);
	if ( itr != destMap_.end() )
	{
		AlarmRecord *alarmRecord = (*itr).second;
		alarmRecord->insertSubscriber(sub, destSet);
	}
	else
	{
		// Destination does not exist in map. We need to create
		// a new alarm record for this destination and insert
		// the subscriber into it.

		AlarmRecord *alarmRecord = new AlarmRecord(dest);
		alarmRecord->insertSubscriber(sub, destSet);

		destMap_.insert( std::make_pair( dest, alarmRecord) );
	}
	mutex_.release();
}

/*===================================================================
   ROUTINE:removeSubscriber
=================================================================== */
//##ModelId=3DE49C1E02AE
void DestinationAlarm::removeSubscriber(const std::string &dest,
                                        SubscriberStruct *sub)
{
	std::map<std::string, AlarmRecord *>::iterator itr;
	mutex_.acquire();
	itr = destMap_.find(dest);

	if ( itr != destMap_.end() )
	{
		AlarmRecord *alarmRecord = (*itr).second;
		alarmRecord->removeSubscriber(sub);
	}
	else
	{
		// Destination does not exist in map !!!
	}
	mutex_.release();
}


/*===================================================================
   ROUTINE:destroyDestinationEntry
=================================================================== */
//##ModelId=3DE49C1E02C2
void DestinationAlarm::destroyDestinationEntry(const std::string &dest)
{
	std::map<std::string, AlarmRecord *>::iterator itr;
	mutex_.acquire();
	itr = destMap_.find(dest);
	if ( itr != destMap_.end() )
	{
		AlarmRecord *alarmRecord = (*itr).second;

		// Delete and remove everything from the subscriber-list
		alarmRecord->destroy();

		// Delete the subscriber object for this destination
		delete alarmRecord;

		// Remove this destination from the map
		destMap_.erase(itr);
	}
	else
	{
		// The destination does not exist in the map,
		// there is nothing to destroy
	}
	mutex_.release();
}


/*===================================================================
   ROUTINE:raise
=================================================================== */
//##ModelId=3DE49C1E02D6
void DestinationAlarm::raise(const std::string &dest,
							 const std::string &destSet, AlarmRecord::thisCause  alarmCause,
							 std::string GlbDestPath, ACE_UINT64 CDH_Ready_File_Number, ACE_INT64 OldestFileTimeStamp)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	//bool retStatus = true;
	std::map<std::string, AlarmRecord *>::iterator itr;
	mutex_.acquire();
	if ( !destSet.empty())
	{
		myDestSetNm_= destSet;
	}
	itr = destMap_.find(dest);
	if ( itr != destMap_.end() )
	{
		AlarmRecord *alarmRecord = (*itr).second;
		// Raise alarm and send event to each subscriber
		alarmRecord->raise(destSet, alarmCause, GlbDestPath, CDH_Ready_File_Number, OldestFileTimeStamp);
	}
	else
	{
		// Destination does not exist in the map !!
		AlarmRecord *alarmRecord = new AlarmRecord(dest);
		destMap_.insert( std::make_pair( dest, alarmRecord) );
#if 0
		retStatus = alarmRecord->createProblemText(alarmCause, GlbDestPath, CDH_Ready_File_Number, OldestFileTimeStamp);
		if(retStatus == false)
		{
			mutex_.release();
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "%s", "no need to raise alarm as destination is not found in the transdest list "));
			return ;
		}
#endif

		alarmRecord->raise(destSet, alarmCause, GlbDestPath, CDH_Ready_File_Number, OldestFileTimeStamp);
	}
	// only for destinations that are included in a set and for an alarm that isn't a destination set alarm
	if (destSet.length() > 0 && alarmCause != AlarmRecord::destination_set_error)
	{
		AES_CDH_TRACE_MESSAGE("Alarm raised. Checking for destination set alarm");

		map<string, TransDestData>::iterator mi_item;
		AES_CDH_DestinationSet::destSetAttributes attrs;

		AES_CDH_TRACE_MESSAGE("Acquiring destpairMX_ mutex");

		destpairMX_.acquire();

		AES_CDH_TRACE_MESSAGE("destpairMX_ mutex acquired.");

		mi_item = destPair_.find(destSet);
		if (mi_item == destPair_.end()) 
		{
			AES_CDH_TRACE_MESSAGE("Destset data not found.");

			//new transdestdata
			TransDestData tdest;

			tdest.priOk = tdest.secOk = tdest.backOk = true;
			tdest.priDestName = tdest.secDestName = tdest.backDestName = "";

			AES_CDH_TRACE_MESSAGE("Acquiring destset data from server");


			AES_CDH_TRACE_MESSAGE( " Get the destination set 4 %s", destSet.c_str());
			std::list<TransDestSet*>::iterator itr;
			bool found = false;
			for ( itr = ServR::destSetList_.begin(); itr != ServR::destSetList_.end(); ++itr )
			{
				if ( ((*itr)->getDestinationSetName()) == destSet )
				{
					tdest.tds = (*itr);
					found = true;
				}
			}
			if(found == false)
			{
				AES_CDH_TRACE_MESSAGE("Destset not found 4.");
				destpairMX_.release();
				mutex_.release();
				return;
			}

			AES_CDH_TRACE_MESSAGE("Destset data acquired from server.");

			// fetch destset attributes
			tdest.tds->getAttr(attrs);
			tdest.priDestName = attrs.primDest;
			tdest.secDestName = attrs.secDest;
			tdest.backDestName = attrs.backDest;

			//insert into map
			mi_item = ((pair<map<string, TransDestData>::iterator, bool>)
					destPair_.insert(DestSet_Pair(destSet, tdest))).first;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Destset data found.");
			TransDestData tdest;

			// compare destset attributes to see if destset changed

			AES_CDH_TRACE_MESSAGE(" Get the destination set 3 %s", destSet.c_str());
			bool found = false;
			std::list<TransDestSet*>::iterator itr;
			for ( itr = ServR::destSetList_.begin(); itr != ServR::destSetList_.end(); ++itr )
			{
				if ( ((*itr)->getDestinationSetName()) == destSet )
				{
					tdest.tds = (*itr);
					found = true;
				}
			}
			if(found == false)
			{
				AES_CDH_TRACE_MESSAGE("Destset not found 3.");
				destpairMX_.release();
				mutex_.release();
				return;
			}

			mi_item->second.tds->getAttr(attrs);
			if(mi_item->second.priDestName != attrs.primDest)
			{
				mi_item->second.priDestName = attrs.primDest;
				mi_item->second.priOk = true;
			}
			if(mi_item->second.secDestName != attrs.secDest)
			{
				mi_item->second.secDestName = attrs.secDest;
				mi_item->second.secOk = true;
			}
			if(mi_item->second.backDestName != attrs.backDest)
			{
				mi_item->second.backDestName = attrs.backDest;
				mi_item->second.backOk = true;
			}
		}

		//update according to this alarm
		if (dest == mi_item->second.priDestName) 
		{
			// primary
			AES_CDH_TRACE_MESSAGE("Primary destination set to 'Not OK'");
			mi_item->second.priOk = false;
		} 
		else if (dest == mi_item->second.secDestName) 
		{
			// secondary
			AES_CDH_TRACE_MESSAGE("Secondary destination set to 'Not OK'");
			mi_item->second.secOk = false;
		} 
		else if (dest == mi_item->second.backDestName) 
		{
			// backup
			AES_CDH_TRACE_MESSAGE("Backup destination set to 'Not OK'");
			mi_item->second.backOk = false;
		}

		AES_CDH_TRACE_MESSAGE("Status of destinations:");

		AES_CDH_TRACE_MESSAGE("Primary destination name %s", mi_item->second.priDestName.c_str());

		AES_CDH_TRACE_MESSAGE("Primary destination status %s", (mi_item->second.priOk == true ? std::string("OK").c_str() : std::string("Not OK").c_str()));

		AES_CDH_TRACE_MESSAGE("Secondary destination name %s",mi_item->second.secDestName.c_str());

		AES_CDH_TRACE_MESSAGE("Secondary destination status %s",(mi_item->second.secOk == true ? "OK" : "Not OK"));

		//get command handler for destset
		CmdHandler *cmd;
		cmd = ServR::getCmdHandler(attrs.destSetName);
		DestinationAlarm::AlarmRecord::thisCause cause;
		if ( mi_item->second.priOk == false && (mi_item->second.secOk == false || mi_item->second.secDestName == ""))
		{
			cause = DestinationAlarm::AlarmRecord::destination_set_error;
			destpairMX_.release();
			mutex_.release();
			// use destination set name instead of destination name
			DestinationAlarm::instance()->raise(destSet, destSet, cause);
			if (cmd != NULL) 
			{
				ServR::cmdHandlerListMX_.acquire();
				if( ServR::checkCmdHandler( cmd) == true )
				{
					cmd->sendDestEvent(AES_CDH_RC_DESTSETDOWN, attrs.destSetName);
				}
				ServR::cmdHandlerListMX_.release();
			}
			AES_CDH_TRACE_MESSAGE( "Destination set alarm raised.");
			AES_CDH_TRACE_MESSAGE("Leaving");
			return;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE(" No Destination set alarm. Cease any existing");
			cause = DestinationAlarm::AlarmRecord::destination_set_error;

			destpairMX_.release();
			mutex_.release();

			// use destination set name instead of destination name
			DestinationAlarm::instance()->cease(destSet, destSet, cause);
			if (cmd != NULL)
			{
				ServR::cmdHandlerListMX_.acquire();
				if( ServR::checkCmdHandler( cmd) == true )
				{
					cmd->sendDestEvent(AES_CDH_RC_DESTSETDOWNCEASE, attrs.destSetName);
				}
				ServR::cmdHandlerListMX_.release();
			}

			AES_CDH_TRACE_MESSAGE("Leaving");
			return;
		}
	}
	mutex_.release();

	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
   ROUTINE:cease
=================================================================== */
//##ModelId=3DE49C1E02E0
void DestinationAlarm::cease(const std::string &dest,
							 const std::string &destSet, AlarmRecord::thisCause  alarmCause,
							 std::string GlbDestPath, ACE_UINT64 CDH_Ready_File_Number, ACE_INT64 OldestFileTimeStamp)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	bool ceased = false; //whether an alarm was actually ceased
	std::map<std::string , AlarmRecord *>::iterator itr;
	mutex_.acquire();
	if ( !destSet.empty())
	{
		myDestSetNm_=destSet;
	}
	itr = destMap_.find(dest);
	if ( itr != destMap_.end() )
	{
		AlarmRecord *alarmRecord = (*itr).second;

		AES_CDH_TRACE_MESSAGE("Destination exists in the map and about to cease alarm");
		// Send event to all alarmRecords
		ceased = alarmRecord->cease(destSet, alarmCause, GlbDestPath, CDH_Ready_File_Number, OldestFileTimeStamp);//HK67469:For ceasing alarm
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Destination does not exist in the map !!!");
	}

	// only for destinations that are included in a set and for an alarm that isn't a destination set alarm
	if (ceased && destSet.length() > 0 && alarmCause != AlarmRecord::destination_set_error)
	{
		map<string, TransDestData>::iterator mi_item;
		AES_CDH_DestinationSet::destSetAttributes attrs;
		destpairMX_.acquire();
		mi_item = destPair_.find(destSet);
		if (mi_item == destPair_.end()) 
		{
			//new transdestdata
			TransDestData tdest;

			tdest.priOk = tdest.secOk = tdest.backOk = true;
			tdest.priDestName = tdest.secDestName = tdest.backDestName = "";

			// item not found


			std::list<TransDestSet*>::iterator itr;
			bool found = false;
			//destSetMX_.acquire();
			AES_CDH_TRACE_MESSAGE("Get the destination set 5 %s", destSet.c_str());
			for ( itr = ServR::destSetList_.begin(); itr != ServR::destSetList_.end(); ++itr )
			{
				if ( ((*itr)->getDestinationSetName()) == destSet )
				{
					tdest.tds = (*itr);
					found = true;
				}
			}
			if(found == false)
			{
				AES_CDH_TRACE_MESSAGE("Destset not found 5 !!!");
				destpairMX_.release();
				mutex_.release();
				return;
			}




			// fetch destset attributes
			tdest.tds->getAttr(attrs);
			tdest.priDestName = attrs.primDest;
			tdest.secDestName = attrs.secDest;
			tdest.backDestName = attrs.backDest;

			//insert into map
			mi_item = ((pair<map<string, TransDestData>::iterator, bool>)
					destPair_.insert(DestSet_Pair(destSet, tdest))).first;
		}
		else
		{
			TransDestData tdest;
			// compare destset attributes to see if destset changed

			bool found = false;
			std::list<TransDestSet*>::iterator itr;
			AES_CDH_TRACE_MESSAGE(" Get the destination set 2  %s", destSet.c_str());
			for ( itr = ServR::destSetList_.begin(); itr != ServR::destSetList_.end(); ++itr )
			{
				if ( ((*itr)->getDestinationSetName()) == destSet )
				{
					tdest.tds = (*itr);
					found = true;
				}
			}
			if(found == false)
			{
				AES_CDH_TRACE_MESSAGE(" Destset not found 2 ");
				destpairMX_.release();
				mutex_.release();
				return;
			}


			mi_item->second.tds->getAttr(attrs);
			if(mi_item->second.priDestName != attrs.primDest)
			{
				mi_item->second.priDestName = attrs.primDest;
				mi_item->second.priOk = true;
			}
			if(mi_item->second.secDestName != attrs.secDest)
			{
				mi_item->second.secDestName = attrs.secDest;
				mi_item->second.secOk = true;
			}
			if(mi_item->second.backDestName != attrs.backDest)
			{
				mi_item->second.backDestName = attrs.backDest;
				mi_item->second.backOk = true;
			}
		}

		//update according to this alarm
		if (dest == mi_item->second.priDestName) 
		{
			// primary
			mi_item->second.priOk = true;
		} 
		else if (dest == mi_item->second.secDestName) 
		{
			// secondary
			mi_item->second.secOk = true;
		} 
		else if (dest == mi_item->second.backDestName) 
		{
			// backup
			mi_item->second.backOk = true;
		}

		AES_CDH_TRACE_MESSAGE("Status of destinations:");

		AES_CDH_TRACE_MESSAGE("Primary destination name %s",mi_item->second.priDestName.c_str());

		AES_CDH_TRACE_MESSAGE("Primary destination status %s",(mi_item->second.priOk == true ? std::string("OK").c_str() : std::string("Not OK").c_str()));

		AES_CDH_TRACE_MESSAGE("Secondary destination name %s",mi_item->second.secDestName.c_str());

		AES_CDH_TRACE_MESSAGE("Secondary destination status %s",(mi_item->second.secOk == true ? "OK" : "Not OK"));

		AES_CDH_TRACE_MESSAGE("Backup destination name %s",mi_item->second.backDestName.c_str());

		AES_CDH_TRACE_MESSAGE("Backup destination status %s", (mi_item->second.backOk == true ? "OK" : "Not OK"));

		//get commandhandler for destset
		CmdHandler *cmd;
		cmd = ServR::getCmdHandler(attrs.destSetName);

		DestinationAlarm::AlarmRecord::thisCause cause;

		if ( (mi_item->second.priOk == false
				&&  (mi_item->second.secOk == false || mi_item->second.secDestName == "")
				&&  (mi_item->second.backOk == false || mi_item->second.backDestName == "")) == false )
		{
			cause = DestinationAlarm::AlarmRecord::destination_set_error;

			// use destination set name instead of destination name
			AES_CDH_TRACE_MESSAGE("Ceasing destination set alarm.");

			destpairMX_.release();
			mutex_.release();

			DestinationAlarm::instance()->cease(destSet, destSet, cause);
			if (cmd != NULL) 
			{
				ServR::cmdHandlerListMX_.acquire();
				if( ServR::checkCmdHandler( cmd) == true )
				{
					cmd->sendDestEvent(AES_CDH_RC_DESTSETDOWNCEASE, attrs.destSetName);
				}
				ServR::cmdHandlerListMX_.release();
			}
			AES_CDH_TRACE_MESSAGE("Leaving");

			return;
		}
		destpairMX_.release();
	}
	mutex_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
}
/*===================================================================
   ROUTINE:removeDestSet
=================================================================== */
void DestinationAlarm::removeDestSet(const std::string& destSet)
{
	destpairMX_.acquire();
	destPair_.erase(destSet);
	destpairMX_.release();

}

/*===================================================================
   ROUTINE:removeDestSet
=================================================================== */
void DestinationAlarm::clearMapsAtShutDown()
{
	std::map<std::string , AlarmRecord *>::iterator destMapItr;
	mutex_.acquire();
	for( destMapItr = destMap_.begin(); destMapItr != destMap_.end() ; )
	{
		if( destMapItr->second != 0 )
		{
			(destMapItr->second)->destroy();
			delete destMapItr->second;
			destMapItr->second = 0;
			destMap_.erase(destMapItr++);
		}
	}
	mutex_.release();
	map<string, TransDestData>::iterator destPairIterator;
	destpairMX_.acquire();
	for( destPairIterator = destPair_.begin(); destPairIterator != destPair_.end() ; )
	{
		destPair_.erase(destPairIterator++);
	}
	destpairMX_.release();
}
