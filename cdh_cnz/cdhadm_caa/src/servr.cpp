/*=================================================================== */
/**
   @file   servr.cpp

   Class method implementation for ServR type module.

   This module contains the implementation of class declared in
   the servr.h module

   @version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       05/10/2011   XCHEMAD     Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

// Internal header files
#include <servr.h>
#include <cmdhandler.h>
#include <destinationalarm.h>
#include <event.h>
#include <parameter.h>
#include <transdestset.h>
#include <aes_cdh_file_init_dest_handler.h>
#include <aes_cdh_file_resp_dest_handler.h>
#include <aes_cdh_Linftpiv2.h>
#include <aes_cdh_service.h>
// External library header files
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_Command.H>
#include <ACS_APGCC_CommonLib.h>
#include <ACS_APGCC_DSD.H>
#include <AES_DBO_TQManager.h>
#include <aes_cdh_common.h>
#include <crypto_status.h>
#include <crypto_api.h>
#include <libssh2.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

// Linux header files
#include <sys/statvfs.h>
#include <sys/eventfd.h>

// C++ header files
#include <map>
#include <list>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>


namespace DSDConfig{
        const int DSDSuccess = 0;
}

namespace
{
	const int CDH_PHYSICAL_SEPARATION_DISABLED = 0;
	const int CDH_PHYSICAL_SEPARATION_ENBLED = 1;
	const int CDH_FAILURE_RESULT = -1;
	string ipRouteCmd = "ip route get %s";
}
/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
const string destAlarmText_ = "The connection to the remote host lost or write access denied";
const string destSetAlarmText_ = "All destinations in the destination set has failed";
const std::string cause_ = "DATA OUTPUT, AP TRANSMISSION FAULT";

const ACE_INT32 eventCode_ = AES_CDH_ConnectFault;

const string problemText1_ = "CONNECTION LOST TO REMOTE SYSTEM";

AES_CDH_TRACE_DEFINE(AES_CDH_ServR);

// Declaration of ServR static variables
string ServR::destfile_;
string ServR::problemText_;
string ServR::destfileex_;
bool ServR::virtualAlarm_= false;
std::list<TransDest*> ServR::destList_;
ACE_Thread_Mutex* ServR::mutex_buf= NULL;
//ACE_Recursive_Thread_Mutex ServR::destMX_; 
pthread_mutex_t ServR::destMx_;
string ServR::destSetFile_;
std::list<TransDestSet*> ServR::destSetList_;
ACE_Recursive_Thread_Mutex ServR::destSetMX_;
std::list<CmdHandler*> ServR::cmdHandlerList_;
ACE_Recursive_Thread_Mutex ServR::cmdHandlerListMX_; 
map<string, CmdHandler*> ServR::destSetPair_;
ACE_Mutex ServR::destSetPairMX_; // TR HO68058 Mapping
std::list<ServR::sendFileTask> ServR::sendFileTaskList_;
ACE_Recursive_Thread_Mutex ServR::taskMX_; 
std::list<ServR::sendRecordFileTask> ServR::sendRecordFileTaskList_;
ACE_Recursive_Thread_Mutex ServR::recordtaskMX_; 
string ServR::fileSentStatusFile_;
ACE_Recursive_Thread_Mutex ServR::taskSentFileStatus_MX_; 
std::list<status *> ServR::statusList_;
ACE_Thread_Manager* ServR::threadManager_; 
void extractNameFromDn (const string dnName, string& name);
bool getDataTransferFolderPath( string& aDataTransferFolderPath );
bool getActiveDestForDestSet( const string& destSetName, string& activeDestName );
bool isDestSetAttached( const std::string& destSetName);
bool isFileMServiceRunning();
ACE_Mutex ServR::initdestsetMutex;
ACE_Mutex ServR::respdestsetMutex;
ACE_Mutex ServR::blockdestsetMutex;
bool ServR::isRespDestSetDeleteTrig = false;
bool ServR::isInitDestSetDeleteTrig = false;
bool ServR::isBlockDestSetDeleteTrig = false;
bool ServR::isDelOfInitDestSetSuccess = false;
bool ServR::isDelOfRespDestSetSuccess = false;
bool ServR::isDelOfBlockDestSetSuccess = false;
bool ServR::isStopEventSignalled = false;
ACE_Event* ServR::StopEvent = 0;
destInfo* ServR::pBlockDestInfo = 0;
destInfo* ServR::pFileInitDestInfo = 0;
destInfo* ServR::pFileRespDestInfo = 0;
ACE_HANDLE ServR::pipeHandles[2];



/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */
/*===================================================================
   ROUTINE:ServR
=================================================================== */
ServR::ServR():
		m_serverOnLine(false),
		m_DSDInitiate(false)
{
	m_ShutDown = eventfd(0,0);

	// Instance a Reactor to handle DSD events
	ACE_TP_Reactor* tp_reactor_impl = new ACE_TP_Reactor();

	// the reactor will delete the implementation on destruction
	m_reactor = new ACE_Reactor(tp_reactor_impl, true);

	// Initialize the ACE_Reactor
	m_reactor->open(1);

	ServR::cmdHandlerList_.clear();
	ServR::destList_.clear();
	ServR::destSetList_.clear();
	ServR::sendFileTaskList_.clear();
	ServR::sendRecordFileTaskList_.clear();
	ServR::statusList_.clear();

	threadManager_ = new (std::nothrow) ACE_Thread_Manager; // INGO3 GOH qabhall 010822

	// Stop Event
	StopEvent = new (std::nothrow) ACE_Event();
	ServR::isStopEventSignalled = false;

	//Block destination handlers
	m_poPrimBlockDestCmdHandler = 0;
	m_poSecBlockDestCmdHandler = 0;
	m_poAdvBlockDestCmdHandler = 0;
	m_poBlockDestSetHandler = 0;
	//Responding File destination handlers.
	m_poPrimRespDestCmdHandler = 0; 
	m_poSecRespDestCmdHandler = 0; 
	m_poAdvRespDestCmdHandler = 0; 

	// Initiating file destination handlers
	m_poFilePrimaryInitDestCmdHandler = 0;
	m_poFileSecondaryInitDestCmdHandler = 0;
	m_poFileAdvancedInitiatingCmdHandler = 0;

	m_poInitDestSetHandler = 0;
	m_poRespDestSetHandler = 0;

	pthread_mutexattr_init(&mutex_attr);
	pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutexattr_setprotocol(&mutex_attr, PTHREAD_PRIO_INHERIT);
	pthread_mutex_init(&destMx_, &mutex_attr);
}

ServR::~ServR()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	delete m_reactor;
	ACE_OS::close(m_ShutDown);
}

/*===================================================================
   ROUTINE:open
=================================================================== */
ACE_INT32 ServR::open()
{
	task_done_mutex.acquire();
	signal(SIGPIPE, SIG_IGN);
	AES_CDH_TRACE_MESSAGE("Entering");
	if( omHandler.Init() == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "OmHandler initialize failed.");
		AES_CDH_TRACE_MESSAGE("OmHandler initialize failed.");
		AES_CDH_TRACE_MESSAGE("Leaving");
		task_done_mutex.release();
		return -1;
	}

	if (!checkFileTransferObjInIMM()  )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to fetch the file trasnfer object in IMM");
		AES_CDH_TRACE_MESSAGE("Unable to fetch the file trasnfer object in IMM");
		AES_CDH_LOG(LOG_LEVEL_TRACE, "Trying again to fetch the file transfer object in IMM");
		task_done_mutex.release();
		return -1;
	}
	if( checkBlockTransferObjInIMM() == false )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Block Transfer Object is not retreived from IMM");
		AES_CDH_TRACE_MESSAGE("Block Transfer Object is not retreived from IMM");
		task_done_mutex.release();
		AES_CDH_TRACE_MESSAGE("Leaving");
		return -1;
	}


	//Initialize libcurl.

	int curlResult = curl_global_init(CURL_GLOBAL_ALL); 
	if ( curlResult != 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "curl_global_init failed with return value : %d", curlResult);
		AES_CDH_TRACE_MESSAGE("curl_global_init failed with return value : %d", curlResult);
		task_done_mutex.release();
		return -1;
	}

	//Initialize libssh2
	int sshResult = libssh2_init(0);
	if(AES_CDH_RC_OK != sshResult)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "libssh2_init failed with error code : %d", sshResult);
		AES_CDH_TRACE_MESSAGE("libssh2_init failed with error code : %d", sshResult);
		task_done_mutex.release();
		return -1;
	}

	AES_CDH_LOG(LOG_LEVEL_INFO, "libssh2_init successful!");
	AES_CDH_TRACE_MESSAGE("libssh2_init successful!");

	if(thread_setup() != 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "thread_setup failed");
		AES_CDH_TRACE_MESSAGE("thread_setup failed");
		task_done_mutex.release();
		return -1;
	}

	AES_CDH_Paths::instance();
	if ( setDataPath() )
	{
		for (; !createCDHDirs(); )
		{ 
			ACE_Time_Value tv(2, 0);
			ACE_INT32 dwRes = StopEvent->wait(&tv, 0);
			if(dwRes < 0)
			{
				int errnum = ACE_OS::last_error();
				AES_CDH_LOG(LOG_LEVEL_ERROR, "[data disk check] Service error in waiting on the stop event, error code: %d", ACE_OS::last_error());
				AES_CDH_TRACE_MESSAGE("[data disk check] Service error in waiting on the stop event, error code: %d", ACE_OS::last_error());
				if( errnum == 62 )
				{
					continue;
				}
				AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving");
				task_done_mutex.release();
				return false;
			}
			else if(dwRes == 0)
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "[data disk check] Stop Event caught. Service stopped");
				AES_CDH_TRACE_MESSAGE("[data disk check] Stop Event caught. Service stopped");
				AES_CDH_TRACE_MESSAGE("Leaving");
				task_done_mutex.release();
				return true;
			}
		}

		// recover destset and dest while startup
		AES_CDH_LOG(LOG_LEVEL_INFO,"Recovering destset and dest while startup");
		recoverDestinations();

		//INGO3 CDH qabulgu start persistence
		if (setDataPathForStatusFile())
		{
			fileSentStatusRecover();
		}
	}

	if( ACE_OS::pipe(pipeHandles) == -1 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while creating pipe for STOP event!!");
		AES_CDH_TRACE_MESSAGE("Error occured while creating pipe for STOP event!!");
		task_done_mutex.release();
		return -1;
	}


	AES_CDH_LOG(LOG_LEVEL_INFO, "Starting OI threads on Active node");
	if( StartOIThreads() == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while starting the OI Threads on the active node");
		AES_CDH_TRACE_MESSAGE("Error occured while starting the OI Threads on the active node");
		task_done_mutex.release();
		return 0;

	}	

	AES_CDH_TRACE_MESSAGE("Leaving");
	task_done_mutex.release();

	return 0;
}

/*===================================================================
   ROUTINE:close
=================================================================== */
ACE_INT32 ServR::close()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	task_done_mutex.acquire();
	std::list<TransDest*>::iterator itr;
	string destName;
	string destSetName;
	char buf[] = { 1 , 1};
	int bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
	while( bytes <= 0 )
	{
		AES_CDH_TRACE_MESSAGE("Resending the stop event after 1 sec");
		sleep(1);
		bytes = ACE_OS::write( pipeHandles[1], buf, sizeof(buf ));
	}
	try
	{

		DestinationAlarm::AlarmRecord::thisCause alarmCause,SUalarmCause;
		alarmCause = DestinationAlarm::AlarmRecord::ok;
		SUalarmCause = DestinationAlarm::AlarmRecord::SU_error;

		StopOIThreads();

		// set all ongoing tasks to stopped
		(void) stopTasks(0, 0, true);

		// Cease all alarms
		destSetMX_.acquire();

		for (itr = destList_.begin(); itr != destList_.end(); ++itr)
		{
			destName = (*itr)->getDestinationName();
			destSetName = (*itr)->getDestSetName();

			if ((*itr)->isNotificationDefined() == true)
			{
				DestinationAlarm::instance()->cease(destName + "_filenotification", "", alarmCause);
			}
			DestinationAlarm::instance()->cease(destName, destSetName, alarmCause);
			//Ceasing ready poll alarm if exists
			if((*itr)->GetPollingStatus())	
			{
				AES_CDH_TRACE_MESSAGE("Polling status is true for %s",destName.c_str());
				DestinationAlarm::instance()->cease(destName, destSetName, SUalarmCause);
			}
			// cease any destination set alarms
			DestinationAlarm::instance()->cease(destSetName, destSetName, alarmCause);

		}
		destSetMX_.release();

		taskSentFileStatus_MX_.acquire();
		std::list<status *>::iterator statusIter;
		for (statusIter = statusList_.begin(); statusIter != statusList_.end(); ++statusIter)
		{
			delete (*statusIter);
		}
		taskSentFileStatus_MX_.release();
		statusList_.clear();

		if( StopEvent )
		{
			delete StopEvent;
			StopEvent = 0;
		}
		if( threadManager_ != 0 )
		{
			delete threadManager_;
			threadManager_ = 0;
		}
		DestinationAlarm::instance()->clearMapsAtShutDown();

		std::list<CmdHandler*>::iterator cmdHandlerItr;

		cmdHandlerListMX_.acquire();
		for ( cmdHandlerItr = cmdHandlerList_.begin(); cmdHandlerItr != cmdHandlerList_.end(); )
		{
			(*cmdHandlerItr)->shutdown();
			++cmdHandlerItr;
		}
		cmdHandlerList_.clear();
		cmdHandlerListMX_.release();

		// After ServR's cmdHandlerList_ has been cleared we can notify
		// the shutdown event to all cmdHandlers
		eventfd_t stopEvent = 1U;
		eventfd_write(m_ShutDown, stopEvent);

		AES_CDH_TRACE_MESSAGE("shutting down of cmdHandler threads !!Done ");
		AES_CDH_LOG(LOG_LEVEL_INFO, "shutting down of cmdHandler threads !!Done");
		//Clear the destSetPair map.
		map<string, CmdHandler*>::iterator destSetPairItr;
		destSetPairMX_.acquire(); // TR HO68058 Added, not in APG/W.
		for( destSetPairItr = destSetPair_.begin(); destSetPairItr != destSetPair_.end(); )
		{
			destSetPair_.erase( destSetPairItr++);
		}
		destSetPairMX_.release(); // TR HO68058 Added, not in APG/W.

		AES_CDH_TRACE_MESSAGE("destSetPair_ map erased !!Done ");
		destSetMX_.acquire();
		AES_CDH_TRACE_MESSAGE("acquired destset mutex");
		std::list<TransDestSet *>::iterator itr1;
		std::list<TransDestSet *>::iterator itr2;
		itr2 = destSetList_.end();
		for (itr1 = destSetList_.begin(); (itr1 != itr2) ; )
		{
			TransDestSet *pDestSet = *itr1;
			destSetList_.erase(itr1++);
			delete pDestSet;
			pDestSet = 0;

		}
		destSetList_.clear();
		destSetMX_.release();
		AES_CDH_TRACE_MESSAGE("released destset mutex and destSetList_ map erased !!");
		pthread_mutex_lock(&destMx_);
		std::list<TransDest *>::iterator itr3;
		std::list<TransDest *>::iterator itr4;
		itr4 =  destList_.end();
		for (itr3 = destList_.begin(); itr3 != itr4;)
		{
			(*itr3)->shutDown();
			TransDest *pDest = *itr3;
			destList_.erase(itr3++);
			delete pDest;
			pDest = 0;

		}
		destList_.clear();
		pthread_mutex_unlock(&destMx_);
		//Destroy block pair mapaper pointer and erase the map
		BlockPairMapperManager::instance()->destroy();	

		//closing pipe handles
		ACE_OS::close(ServR::pipeHandles[1]);
		ACE_OS::close(ServR::pipeHandles[0]);


		//openssl unregistration
		thread_cleanup();

		//Perform libcurl cleanup.
		curl_global_cleanup();

		//Cleanup libssh2
		libssh2_exit();
		AES_CDH_TRACE_MESSAGE("libssh2_exit, DONE! ");

		omHandler.Finalize();
		AES_CDH_TRACE_MESSAGE("libcurl cleanup and omHandler finalize DONE ");
	}
	catch(...)
	{
	}
	task_done_mutex.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return 0;
}

/*===================================================================
   ROUTINE:handle_session
=================================================================== */
int ServR::handle_input(ACE_HANDLE fd)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)fd ;
	CmdHandler* client = new (std::nothrow) CmdHandler(m_ShutDown);
	if (NULL != client)
	{
		if( m_CmdServerAcceptor.accept(client->getStream()) == 0 )
		{
			client->open();
		}
		else
		{
			delete client;
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return 0;
}

/*===================================================================
   ROUTINE:define
=================================================================== */
AES_CDH_ResultCode ServR::define(const string destName,
		const string transType,
		ACE_INT32 &argc,
		ACE_TCHAR* argv[],
		const string& userGroup,
		const string& destRdn,
		const string destSetName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "Defining destination %s", destName.c_str());
#ifdef DEBUGTEST
	cout << " destName = " << destName << endl;
	cout << " transType = " << transType << endl;
	cout << " argc = " << argc << endl;

	for (int i = 0 ; i < argc ;  i++)
	{
		cout << "argv " << i << " = " << argv[i] << endl;
	}
#endif


	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	if (existDest(destName) == AES_CDH_RC_OK) // Destination already defined
	{
		rcode = AES_CDH_RC_DESTDEF;
	}
	else							          // Define new destination
	{
		TransDest* pdest = new TransDest;
		if ( pdest == 0 )
		{
			return AES_CDH_RC_INTPROGERR;
		}

		rcode = pdest->define(destName, transType, argc, argv, userGroup, true, false, destRdn, destSetName);
		if ( rcode == AES_CDH_RC_OK )     // Destination OK
		{
			pthread_mutex_lock(&destMx_);
			destList_.push_back(pdest);    // Add destination to list

			rcode = saveDestinations();   // Save to file

			if ( rcode != AES_CDH_RC_OK ) // Destinations could not be saved to file
			{
				destList_.remove(pdest);   // Remove destination from list
				pdest->remove();
				delete pdest;             // Delete reference to destination
				pdest = NULL;
			}
			pthread_mutex_unlock(&destMx_);
		}
		else						      // Destination could not be defined
		{
			pdest->remove();
			delete pdest;
			pdest = NULL;// Delete reference to destination
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving with rcode = %d", rcode);
	return rcode;
}

/*===================================================================
   ROUTINE:defineDestSet
=================================================================== */
AES_CDH_ResultCode ServR::defineDestSet(const string destSetName,
		const string primDestName,
		const string secDestName,
		const string backDestName,
		const string userGroup,
		const string &destSetRdn)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "Defining destination set %s", destSetName.c_str());

	AES_CDH_ResultCode rcode;
	//check for destset max limit reached.
	rcode = isDestSetLimitCrossed(destSetRdn);
	if (rcode != AES_CDH_RC_OK)
	{
		return rcode;
	}
	else if(existDestSet(destSetName) == AES_CDH_RC_OK) // Destination set already defined
	{
		rcode = AES_CDH_RC_DESTSETDEF;
	}
	else // Define new destination set
	{
		TransDestSet* pdestset = new TransDestSet;
		if ( pdestset == NULL)
		{
			return AES_CDH_RC_INTPROGERR;
		}

		rcode = pdestset->define(destSetName, primDestName, secDestName, backDestName,userGroup, destSetRdn);
		AES_CDH_TRACE_MESSAGE("destSetName = %s",destSetName.c_str());
		if ( rcode == AES_CDH_RC_OK )     // Destination set OK
		{
			destSetMX_.acquire();
			destSetList_.push_back(pdestset);    // Add destination set to list
			rcode = saveDestinationSets();   // Save to file

			if ( rcode != AES_CDH_RC_OK ) // Destination set could not be saved to file
			{
				destSetList_.remove(pdestset);   // Remove destination set from list
				delete pdestset;             // Delete reference to destination set
				pdestset = NULL;
			}

			destSetMX_.release();
		}
		else						      // Destination set could not be defined
		{
			delete pdestset;			      // Delete reference to destination set
			pdestset = NULL;
		}

		//check all connections in pdestset
		if(pdestset)
		{
			AES_CDH_TRACE_MESSAGE("Ceasing the alarms for destSetName = %s ", destSetName.c_str());
			//pdestset->checkAllConnections();
			AES_GCC_Event event;
			bool retStatus = true;

			string myObjOfRef("");

			retStatus =  ServR::getObjectOfRef("DESTINATION", primDestName,myObjOfRef);
			if (retStatus == false)
			{
				return AES_CDH_RC_INTPROGERR;
			}
			myObjOfRef +=primDestName;

			problemText_ = "";
			event.setProbableCause(cause_.c_str());
			event.setProblemText(problemText_.c_str());
			event.setProblemData(destAlarmText_.c_str());
			event.setEventCode(eventCode_);
			event.setObjectOfReference(myObjOfRef.c_str());
			event.setKind(AES_GCC_Event::EV_ALARM);

			AES_GCC_EventHandler::instance().cease(event, true);

			event = "";	
			string myObjOfRef_destSet("");
			retStatus = ServR::getObjectOfRef( "DESTINATIONSET",destSetName, myObjOfRef_destSet );

			if (retStatus == false)
				return AES_CDH_RC_INTPROGERR;

			myObjOfRef_destSet += destSetName;


			event.setProbableCause(cause_.c_str());
			event.setProblemText(problemText_.c_str());
			event.setProblemData(destSetAlarmText_.c_str());
			event.setEventCode(eventCode_);
			event.setObjectOfReference(myObjOfRef_destSet.c_str());
			event.setKind(AES_GCC_Event::EV_ALARM);

			AES_GCC_EventHandler::instance().cease(event, true);

		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}

/*===================================================================
   ROUTINE:changeAttr
=================================================================== */
AES_CDH_ResultCode ServR::changeAttr(const string destName, ACE_INT32 &argc, ACE_TCHAR* argv[], const string& userName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)userName;
	AES_CDH_ResultCode rcode;
	AES_CDH_Destination::destAttributes attr;
	TransDest* pdest = 0;
	ACE_INT32 i = 0;
	ACE_INT32 attrCount = 0;
	string transType("");
	vector<string>::iterator it;
	string destSetName("");

	rcode = getDest( destName, pdest ); // Get destination
	if (rcode != AES_CDH_RC_OK)
	{
		return rcode;
	}

	// Check if argv contains new transfer type or new connect type  HD41756
	// New user group is not allowed hefe GOH2.1
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			// Check if -g or -k option is specified, which is not allowed to change
			// New parameter. uabchsn 031014
			if (argv[i][1] == 't' ||
					argv[i][1] == 'c' ||
					argv[i][1] == 'v' ||
					argv[i][1] == 'h' /* ||
                                        argv[i][1] == 'g' ||
                                        argv[i][1] == 'k'*/)
			{
				// New transfer type, new connect type, new user group
				// or new alias is not allowed
				return AES_CDH_RC_INCUSAGE;
			}

			if ( argv[i][1] == 'g' || argv[i][1] == 'k')
			{
				// Get the destination set name
				rcode = pdest->getAttr(attr);
				destSetName =  attr.destSetName;
				// Check if file transfer in progress
				taskMX_.acquire();
				std::list<sendFileTask>::iterator itFT;
				for ( itFT = sendFileTaskList_.begin();
						itFT != sendFileTaskList_.end(); ++itFT )
				{
					if ( (*itFT).destSetName == destSetName )
					{
						taskMX_.release();
						return AES_CDH_RC_INTPROGERR;
					}
				}
				taskMX_.release();
			}

		}
	}

	if ( rcode == AES_CDH_RC_OK )
	{
		rcode = pdest->getAttr( attr ); // Get old attributes

	}

	if ( rcode == AES_CDH_RC_OK )
	{
		// Keep old transfer type
		transType = attr.transferType;
		/************************HK29526**************************************/
		// If existing destination is not defined with -f or -x option then
		// cdhch should not allow to add -f and -x, instead incorrect usage
		// will be displayed.
		bool ip_defined = false;
		bool port_defined = false;
		for (it = attr.otherAttrs.begin(); it!= attr.otherAttrs.end();++it)
		{
			if((*it) == "-f")
			{
				ip_defined = true;
			}
			if((*it) == "-x")
			{
				port_defined = true;
			}
			if(port_defined && ip_defined)
			{
				break;
			}
		}
		for(attrCount=1; attrCount<argc; attrCount++)
		{
			if(argv[attrCount][0] == '-')
			{
				if(argv[attrCount][1] == 'f')
				{
					if(!ip_defined)
					{
						return AES_CDH_RC_INCUSAGE;
					}
				}
				if(argv[attrCount][1] == 'x')
				{
					if(!port_defined)
					{
						return AES_CDH_RC_INCUSAGE;
					}
				}
			}
		}
		/************************HK29526**************************************/

		// Take old attributes, one by one
		for (it = attr.otherAttrs.begin(); it != attr.otherAttrs.end(); ++it)
		{
			// Compare with all argv
			for (i = 1; i < argc; i++)
			{
				// Don't send -v to changeAttr()!
				if (*it == argv[i] || (*it) == "-v") // Match! argv has new attribute
				{
					break;
				}
			}

			if (i == argc) // No argv matches old attribute
			{
				// Put old attribute in argv
				argv[argc] = new ACE_TCHAR[(*it).size() + 1];
				(void) strcpy(argv[argc++], (*(it++)).c_str());
				argv[argc] = new ACE_TCHAR[(*it).size() + 1];
				(void) strcpy(argv[argc++], (*(it)).c_str());
			}
			else ++it;
		}

		rcode = pdest->changeAttr(transType, argc, argv);
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}


/*===================================================================
   ROUTINE:listDest
=================================================================== */
AES_CDH_ResultCode ServR::listDest(const string destName,
		vector<AES_CDH_Destination::destAttributes> &attrs)

{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	TransDest* pdest;
	AES_CDH_Destination::destAttributes attr;
	vector<TransDest*> destptrs;
	std::list<TransDest*>::iterator itl;

	if ( destName != "" )  // One destination
	{
		rcode = getDest( destName, pdest );

		if ( rcode == AES_CDH_RC_OK )
		{
			rcode = pdest->getAttr( attr );
		}
		if ( rcode == AES_CDH_RC_OK )
		{
			attrs.push_back( attr );
		}
	}
	else // All destinations
	{
		int ret;
		ret=pthread_mutex_lock(&destMx_);
		if (ret == EOWNERDEAD)//for cdhls hang
		{
			if (pthread_mutex_consistent_np(&destMx_) != 0)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error in repairing destMX_");
				AES_CDH_TRACE_MESSAGE("Error in repairing destMX_");
			}

			for ( itl = destList_.begin(); itl != destList_.end(); ++itl )
			{
				attr.otherAttrs.clear();
				rcode = (*itl)->getAttr( attr );
				if ( rcode != AES_CDH_RC_OK )
				{
					break;
				}
				else
				{
					attrs.push_back( attr );
				}
			}

		}
		else
		{
			AES_CDH_TRACE_MESSAGE("destMx_ accquired");
			for ( itl = destList_.begin(); itl != destList_.end(); ++itl )
			{
				attr.otherAttrs.clear();
				rcode = (*itl)->getAttr( attr );
				if ( rcode != AES_CDH_RC_OK )
				{
					break;
				}
				else
				{
					attrs.push_back( attr );
				}
			}
		}
		pthread_mutex_unlock(&destMx_);
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}

/*===================================================================
   ROUTINE:listDestSet
=================================================================== */
AES_CDH_ResultCode ServR::listDestSet(const string destSetName, 
		vector<AES_CDH_DestinationSet::destSetAttributes>& attrs)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	AES_CDH_DestinationSet::destSetAttributes attr;

	if  (! destSetName.empty()) // one dest set
	{
		TransDestSet* pdestset;
		rcode = getDestSet(destSetName, pdestset);
		if (rcode == AES_CDH_RC_OK) // Destination set exists
		{
			rcode = pdestset->getAttr(attr);
			if (rcode == AES_CDH_RC_OK)
			{
				attrs.push_back(attr);
			}
		}
	}
	else // all destination sets
	{
		destSetMX_.acquire();
		std::list<TransDestSet*>::iterator itr;
		for (itr = destSetList_.begin(); itr != destSetList_.end(); ++itr)
		{
			rcode = (*itr)->getAttr(attr);
			if (rcode == AES_CDH_RC_OK)
			{
				attrs.push_back(attr);
			}
			else
			{
				continue;
			}
		}
		destSetMX_.release();
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}

/*===================================================================
   ROUTINE:deleteDest
=================================================================== */
AES_CDH_ResultCode ServR::deleteDest(const string destName,
		string& detailInfo,
		const string& userName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "Deleting destination %s", destName.c_str());
	(void)detailInfo;
	(void)userName;
	AES_CDH_ResultCode rcode;
	TransDest* pdest = 0;
	TransDest* ptmpdest = 0;


	rcode = getDest(destName, pdest);

	if (rcode == AES_CDH_RC_OK)	       // Destination exists
	{
		ptmpdest = pdest;
		pthread_mutex_lock(&destMx_);
		rcode = pdest->remove();
		if (rcode == AES_CDH_RC_OK)
		{
			delete pdest;              // Delete reference to destination
			pdest = 0;
			destList_.remove(ptmpdest);
			ptmpdest = 0;
		}
		pthread_mutex_unlock(&destMx_);
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}

/*===================================================================
   ROUTINE:deleteDestSet
=================================================================== */
AES_CDH_ResultCode ServR::deleteDestSet(const string destSetName, 
		string& detailInfo,
		const string destName,
		const string userName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "Deleting destination set %s", destSetName.c_str());
	(void)userName;
	AES_CDH_ResultCode rcode;
	TransDestSet* pdestset;

	if ((rcode = getDestSet(destSetName, pdestset)) == AES_CDH_RC_OK)
	{

		if (destName.empty()) // remove destination set
		{
			if ( isDestSetAttached(destSetName) == true )
			{
				rcode = AES_CDH_RC_PROTECTEDDESTSET;
			}

			if(rcode != AES_CDH_RC_PROTECTEDDESTSET)
			{
				// Check if file transfer in progress
				taskMX_.acquire();
				std::list<sendFileTask>::iterator itFT;
				for ( itFT = sendFileTaskList_.begin();
						itFT != sendFileTaskList_.end(); ++itFT )
				{
					if ( (*itFT).destSetName == destSetName )
					{
						rcode = AES_CDH_RC_PROTECTEDDESTSET; // Destination set is protected
						detailInfo = "User: -";
						break;
					}
				}
				taskMX_.release();

				// Check if block transfer in progress
				recordtaskMX_.acquire();

				std::list<sendRecordFileTask>::iterator itBT;

				for ( itBT = sendRecordFileTaskList_.begin();
						itBT != sendRecordFileTaskList_.end(); ++itBT )
				{
					if ( (*itBT).destSetName == destSetName)
					{
						rcode = AES_CDH_RC_PROTECTEDDESTSET; // Destination set is protected
						detailInfo = "User: -";
						break;
					}
				}
				recordtaskMX_.release();
			}

			if (rcode == AES_CDH_RC_OK) // Destination set exists and can be removed
			{
				destSetMX_.acquire();
				rcode = pdestset->removeDestinationSet(); // HD77438, Delete blockpairmapper files
				destSetList_.remove(pdestset);
				delete pdestset;
				pdestset = 0;
				destSetMX_.release();
			}
		}
		else // Remove a destination from the destination set
		{
			rcode = getDestSet(destSetName, pdestset);


			if (rcode == AES_CDH_RC_OK)
			{
				rcode = pdestset->removeDestination(destName);

				if (rcode == AES_CDH_RC_OK)
				{
					rcode = saveDestinationSets();

					pdestset->checkAllConnections();
				}
			}
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}


/*===================================================================
   ROUTINE:changeDestSet
=================================================================== */
AES_CDH_ResultCode ServR::changeDestSet(const string& destSetName, const string& primaryDestName, 
		const string& secondaryDestName, const string& backupDestName,
		const string& userName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)userName;
	AES_CDH_ResultCode rcode;
	TransDestSet *pdestset;
	AES_CDH_DestinationSet::destSetAttributes attr;

	// Check if the destination set exists
	if ((rcode = getDestSet(destSetName, pdestset)) == AES_CDH_RC_OK)
	{
		AES_CDH_TRACE_MESSAGE("calling TransDestSet changeAttr()");
		return pdestset->changeAttr(primaryDestName, secondaryDestName, backupDestName);

	}
	else
		AES_CDH_LOG(LOG_LEVEL_ERROR, "getDestSet failed in changeDestSet()");

	if(rcode == AES_CDH_RC_OK)
	{	
		AES_CDH_TRACE_MESSAGE("Ceasing for secondaryDestName %s", secondaryDestName.c_str());
		string myObjOfRef_Sec("");
		AES_GCC_Event event;

		bool  retStatus =  ServR::getObjectOfRef("DESTINATION", secondaryDestName,myObjOfRef_Sec);
		if (retStatus == false)
		{
			return AES_CDH_RC_INTPROGERR;
		}
		myObjOfRef_Sec +=secondaryDestName;


		event.setProbableCause(cause_.c_str());
		event.setProblemText(problemText_.c_str());
		event.setProblemData(destAlarmText_.c_str());
		event.setEventCode(eventCode_);
		event.setObjectOfReference(myObjOfRef_Sec.c_str());
		event.setKind(AES_GCC_Event::EV_ALARM);

		AES_GCC_EventHandler::instance().cease(event, true);
	}


	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}


/*===================================================================
   ROUTINE:switchActiveDest
=================================================================== */
AES_CDH_ResultCode ServR::switchActiveDest(const string& destSetName, const string& destName)
{
	AES_CDH_TRACE_MESSAGE("%s", "Entering");
	AES_CDH_ResultCode rcode;
	TransDestSet *pdestset;
	rcode = getDestSet(destSetName, pdestset);

	if (rcode == AES_CDH_RC_OK)
	{
		rcode = pdestset->switchDestination(destName);
	}
	AES_CDH_TRACE_MESSAGE("%s", "Leaving");
	return rcode;
}


/*===================================================================
   ROUTINE:validCmdHandler
=================================================================== */
bool ServR::validCmdHandler(CmdHandler* cmdHdlr, const ACE_UINT64 ticks)
{
	AES_CDH_TRACE_MESSAGE("%s","Entering");
	cmdHandlerListMX_.acquire();
	bool result = false;
	std::list<CmdHandler*>::iterator it;

	AES_CDH_TRACE_MESSAGE("validCmdHandler()--> ticks = %lu",ticks);

	for ( it = cmdHandlerList_.begin();it != cmdHandlerList_.end(); ++it )
	{
		AES_CDH_TRACE_MESSAGE("getTicks = %lu", (*it)->getTicks());
		if ( (*it) == cmdHdlr )
		{
			// Check if number of ticks match. If not, a new CmdHandler object has been
			// created using the same pointer value (CmdHandler* cmdHdlr) as an old one.
			//The below code commented in APG43 windows for CNI CNI-55_1_1173
			// but in APG43Linux no issue found till date so, the code not commented
			if (ticks == cmdHdlr->getTicks())
			{
				result = true;
				AES_CDH_TRACE_MESSAGE("validCmdHandler, cmdHdlr found and ticks are identical. ticks1: %lu, ticks2: %lu",ticks,cmdHdlr->getTicks());
				break;
			}
			else
			{
				AES_CDH_TRACE_MESSAGE("validCmdHandler, cmdHdlr found but ticks differ. ticks1: %lu, ticks2: %lu",ticks,cmdHdlr->getTicks());
			}
		}
	}
	cmdHandlerListMX_.release();
	AES_CDH_TRACE_MESSAGE("Exiting");
	return result;
}


/*===================================================================
   ROUTINE:removeCmdHandler
=================================================================== */
void ServR::removeCmdHandler(CmdHandler* cmdHdlr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	cmdHandlerListMX_.acquire();
	std::list<CmdHandler*>::iterator it;

	for ( it = cmdHandlerList_.begin();
			it != cmdHandlerList_.end(); ++it )
	{
		//AES_CDH_LOG(LOG_LEVEL_INFO, "%s","cmdHandler = %x", *it));
		if ( (*it) == cmdHdlr )
		{
			cmdHandlerList_.erase(it);
			break;
		}
	}
	cmdHandlerListMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
}


/*===================================================================
   ROUTINE:insertCmdHandler
=================================================================== */
void ServR::insertCmdHandler(CmdHandler* cmdHdlr)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	cmdHandlerListMX_.acquire();

	bool alreadyIn = false;
	std::list<CmdHandler*>::iterator it;

	for ( it = cmdHandlerList_.begin();
			it != cmdHandlerList_.end(); ++it )
	{
		if ( (*it) == cmdHdlr )
		{
			alreadyIn = true;
			break;
		}
	}

	if (!alreadyIn)
	{
		cmdHandlerList_.push_back(cmdHdlr);
	}

	cmdHandlerListMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
}


/*===================================================================
   ROUTINE:sendFile
=================================================================== */
AES_CDH_ResultCode ServR::sendFile(const string destSetName,
		const string fileName,
		const string remoteSubDirName,
		const string newFileName,
		ACE_INT32 pid,
		ACE_INT32 destSetObj,
		CmdHandler* cmdHdlr,            // INGO3 GOH qabhall
		const ACE_UINT64 ticks,
		ACS_APGCC_Command *cmd,               // INGO3 GOH qabhall
		const string userUnique,        // INGO3 GOH qabhall
		const AES_CDH_DestinationSet::transferMode trMode,  // INGO3 GOH qabhall
		const string fileMask,          // INGO3 GOH qabhall
		const bool isDir,               // INGO3 GOH qabhall
		const bool retryAfterRestart)   // INGO3 GOH qabhall
{
	AES_CDH_TRACE_MESSAGE("Entering");
	(void)cmd;
	AES_CDH_ResultCode rcode;
	TransDestSet* destSet;
	sendFileTask task;
	std::list<sendFileTask>::iterator it;
	bool fileIsSent = false; //INGO3 CDH qabulgu persistence
	bool readListOk = false; //INGO3 CDH qabulgu persistence

	rcode = getDestSet(destSetName, destSet);
	if ( rcode == AES_CDH_RC_OK )
	{
		// Set task attributes
		task.destSetName      = destSetName;
		task.fileName         = fileName;
		task.remoteSubDirName = remoteSubDirName;
		task.newFileName      = newFileName;
		task.stopFlag         = false;
		task.CmdHdlr          = cmdHdlr;
		task.ticks            = ticks;
		task.sem              = NULL;
		task.pid              = pid;
		task.destSetObj       = destSetObj;
		task.pStopFileTransferEvent = new(std::nothrow) ACE_Event();

		taskMX_.acquire();
		for ( it = sendFileTaskList_.begin(); it != sendFileTaskList_.end(); ++it )
		{
			if ( (*it).destSetName == destSetName &&
					(*it).fileName == fileName &&
					(*it).remoteSubDirName == remoteSubDirName &&
					(*it).newFileName == newFileName )
			{
				taskMX_.release();
				if( task.pStopFileTransferEvent != 0 )
				{
					delete task.pStopFileTransferEvent;
					task.pStopFileTransferEvent = 0;
				}
				string newdr("");
				if (AES_GCC_Util::datapath_trn(fileName,AES_DATA_PATH,newdr)==false)
				{
					AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
					newdr = "Not Available";
				}
				Event::report(AES_CDH_intProgFault,
						"INTERNAL PROGRAM ERROR",
						"Task already exists : " + destSetName +
						"," + newdr + "," + remoteSubDirName + "," +
						newFileName, "-");
				return AES_CDH_RC_TASKEXISTS;
			}
		}

		//INGO3 CDH qabulgu start persistence
		//read from the statuslist
		if(retryAfterRestart)
		{
			readListOk = fileSentStatusRead(userUnique, fileName, fileIsSent);
			if (readListOk && fileIsSent) // According to cdh's statuslist,
			{                             // the file is already sent
				rcode = AES_CDH_RC_FILEALREADYSENT;
			}
			else
				fileIsSent = false;
		}

		if (!fileIsSent)
		{
			//INGO3 CDH qabulgu end persistence
			sendFileTaskList_.push_back(task);  // Put task in task list
			taskMX_.release();
			rcode = destSet->sendFile(cmdHdlr,
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

			//INGO3 CDH qabulgu persistence
		}
		else
			taskMX_.release();
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}


/*===================================================================
   ROUTINE:sendFileDone
=================================================================== */
AES_CDH_ResultCode ServR::sendFileDone( const string userUnique, // INGO3 GOH qabhall
		const string destSetName,
		const string fileName,
		const string remoteSubDirName,
		const string newFileName,
		AES_CDH_ResultCode sendResult)
{
	AES_CDH_TRACE_MESSAGE( "Entering");
	AES_CDH_TRACE_MESSAGE("In sendFileDone() for File is %s .", fileName.c_str());


	//INGO3 CDH qabulgu start
	// Add item to the file list and to the statusFile
	if(sendResult == AES_CDH_RC_OK)
	{
		fileSentStatusSave(userUnique, fileName);
	}
	else
	{
		removeUserUnique(userUnique);  // 010925
	}
	//INGO3 CDH qabulgu end

	std::list<sendFileTask>::iterator it;
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	taskMX_.acquire();

	// Remove task from list
	for ( it = sendFileTaskList_.begin(); it != sendFileTaskList_.end(); )
	{
		if ( (*it).destSetName == destSetName &&
				(*it).fileName == fileName &&
				(*it).remoteSubDirName == remoteSubDirName &&
				(*it).newFileName == newFileName )
		{
			if ((*it).stopFlag == true)
			{
				AES_CDH_TRACE_MESSAGE("In ServR::sendFileDone(), stopFlag is true for fileName ->%s, newFileName->%s ,destSetName->%s ",fileName.c_str(),newFileName.c_str(),destSetName.c_str());
				if ((*it).sem != NULL)
				{
					(*it).sem->release();
					AES_CDH_TRACE_MESSAGE("In sendFileDone() semphore released for file %s ", fileName.c_str());
				}
				rcode = AES_CDH_RC_TASKSTOPPED;
			}
			if( (*it).pStopFileTransferEvent != 0 )
			{
				delete (*it).pStopFileTransferEvent;
				(*it).pStopFileTransferEvent = 0;
			}
			sendFileTaskList_.erase(it++);
			break;
		}
		else
		{
			++it;
		}
	}
	taskMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}


/*===================================================================
   ROUTINE:sendRecordFile
=================================================================== */
AES_CDH_ResultCode ServR::sendRecordFile(const string destSetName, 
		const string streamName,
		const ACE_INT32 streamSize,
		const ACE_INT32 recordLength,
		const string mainFileName,
		string &fileName,
		ACE_UINT64 &recordsSent,
		ACE_INT32 pid,
		ACE_INT32 destSetObj,
		CmdHandler* cmdHdlr,      //INGO3 GOH qabhall
		const ACE_UINT64 ticks)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode rcode=AES_CDH_RC_ERROR;
	TransDestSet* destSet;
	std::list<sendRecordFileTask>::iterator it;

	AES_CDH_TRACE_MESSAGE("TR_ServR: sendRecordFile--> mainFileName = %s fileName = %s pid = %d destSetObj = %u ticks = %lu ",mainFileName.c_str(),fileName.c_str(),pid,destSetObj,ticks);
	rcode = getDestSet(destSetName, destSet);
	if ( rcode == AES_CDH_RC_OK )
	{
		recordtaskMX_.acquire();
		for ( it = sendRecordFileTaskList_.begin();
				it != sendRecordFileTaskList_.end(); ++it )
		{
			if ((*it).destSetName == destSetName && (*it).streamName == streamName)
			{
				recordtaskMX_.release();
				string newdr("");
				if (AES_GCC_Util::datapath_trn(mainFileName,AES_DATA_PATH,newdr)==false)
				{
					AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
					newdr = "Not Available";
				}
				Event::report(AES_CDH_intProgFault,
						"INTERNAL PROGRAM ERROR",
						"Task already exists : " + destSetName + "," + newdr,
						"-");

				return AES_CDH_RC_TASKEXISTS;
			}
		}

		// Set task attributes
		sendRecordFileTask task;
		task.destSetName  = destSetName;
		task.mainFileName = mainFileName;
		task.streamName	= streamName;  // CNI 212 55/1-1253 -  Rebase
		task.stopFlag     = false;
		task.CmdHdlr      = cmdHdlr;    // INGO3 GOH qabhall
		task.ticks        = ticks;
		task.sem          = NULL;
		task.pid          = pid;
		task.destSetObj   = destSetObj;
		sendRecordFileTaskList_.push_back(task);  // Put task in task list

		recordtaskMX_.release();

		rcode = destSet->sendRecordFile(streamName,
				streamSize,
				recordLength,
				mainFileName,
				fileName,
				recordsSent,
				cmdHdlr, //INGO3 GOH qabhall "obj" to "cmdHdlr"
				ticks);
	}
	return rcode;
}


/*===================================================================
   ROUTINE:sendRecordFileDone
=================================================================== */
AES_CDH_ResultCode ServR::sendRecordFileDone(const string destSetName, 
		const string mainFileName)
{
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	sendRecordFileTask task;
	std::list<sendRecordFileTask>::iterator it;
	recordtaskMX_.acquire();

	// Remove task from list
	for ( it = sendRecordFileTaskList_.begin();
			it != sendRecordFileTaskList_.end(); ++it )
	{
		if ( (*it).destSetName == destSetName &&
				(*it).mainFileName == mainFileName )
		{
			if ((*it).stopFlag == true)
			{
				if ((*it).sem != NULL)
					(*it).sem->release();

				rcode = AES_CDH_RC_TASKSTOPPED;
			}

			sendRecordFileTaskList_.erase(it);
			AES_CDH_TRACE_MESSAGE("TR_ServR: sendRecordFileDone(%s)--> erase task from list", mainFileName.c_str());

			break;
		}
	}

	recordtaskMX_.release();

	return rcode;
}


/*===================================================================
   ROUTINE:stopSendFile
=================================================================== */
AES_CDH_ResultCode ServR::stopSendFile(const string destSetName,
		const string fileName,
		const string remoteSubDirName,
		const string newFileName)
{
	std::list<sendFileTask>::iterator it;
	// 010925 qabhall qabhans HC81847 ++
	ACE_INT32 l_pid;
	ACE_INT32 l_destSetObj;
	// 010925 --

	taskMX_.acquire();

	for ( it = sendFileTaskList_.begin(); it != sendFileTaskList_.end(); ++it )
	{
		if ( (*it).destSetName == destSetName &&
				(*it).fileName == fileName &&
				(*it).remoteSubDirName == remoteSubDirName &&
				(*it).newFileName == newFileName )
		{
			(*it).stopFlag = true;
			// TR HR49216
			if(((*it).pStopFileTransferEvent) != 0 )
			{
				(*it).pStopFileTransferEvent->signal();
			}
			//SFTP dll replacement IP
			// EGERPET CR SFTP
			TransDestSet* pdestset = 0;
			TransDest* pdest = 0;
			//get DestinationSet given destsetname
			AES_CDH_ResultCode result = getDestSet((*it).destSetName, pdestset);
			if (result == AES_CDH_RC_OK)
			{
				// from destSet get Active Destination
				pdest = pdestset->activeDest;
			}

			CmdHandler* cmdHdlr;
			cmdHdlr = (*it).CmdHdlr;
			ACE_UINT64 ticks = (*it).ticks;

			// 010925 qabhall qabhans  HC81847 ++

			// TR: HC81847
			// Exchange these to local variables, they are deleted from the
			// list before they are needed in the call to sendFileReply() below
			l_pid = (*it).pid;
			l_destSetObj = (*it).destSetObj;

			// 010925 --

			ACE_Thread_Semaphore sem(0);
			(*it).sem = &sem;     // used by ServR::sendFileDone, Signalled when Transdest has stopped file transfer.

			taskMX_.release();

			ACE_Time_Value tv(ACE_OS::gettimeofday () + ACE_Time_Value(30));

			if (sem.acquire(tv) != 0)
			{
				// Semafore not signalled in time.
				taskMX_.acquire();

				// Our *it may be invalid, search again !
				for ( it = sendFileTaskList_.begin(); it != sendFileTaskList_.end(); ++it )
				{
					if ( (*it).destSetName == destSetName &&
							(*it).fileName == fileName &&
							(*it).remoteSubDirName == remoteSubDirName &&
							(*it).newFileName == newFileName )
					{
						(*it).sem = NULL;
						break;
					}
				}

				taskMX_.release();

				ServR::cmdHandlerListMX_.acquire();
				if(validCmdHandler(cmdHdlr, ticks))   // INGO3 GOH qabhall 010815 TR: HC77134
				{
					AES_CDH_TRACE_MESSAGE("sending sendfile reply due to file transfer stopped AES_CDH_RC_INTPROGERR");

					cmdHdlr->sendFileReply(AES_CDH_RC_INTPROGERR, destSetName,
							fileName, remoteSubDirName, newFileName,
							l_pid, l_destSetObj);
				}
				ServR::cmdHandlerListMX_.release();
				string newdr;
				if (AES_GCC_Util::datapath_trn(fileName,AES_DATA_PATH,newdr)==false)
				{
					AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
					newdr = "Not Available";
				}
				Event::report(AES_CDH_intProgFault, "INTERNAL PROGRAM ERROR",
						"Failed to stop task : " + destSetName +
						"," + newdr + "," + remoteSubDirName + "," +
						newFileName, "-");

				return AES_CDH_RC_INTPROGERR;
			}
			ServR::cmdHandlerListMX_.acquire();
			if(validCmdHandler(cmdHdlr, ticks))   // INGO3 GOH qabhall 010815 TR: HC77134
			{

				AES_CDH_TRACE_MESSAGE("sending sendfile reply due to file transfer stopped AES_CDH_RC_TASKSTOPPED");

				cmdHdlr->sendFileReply(AES_CDH_RC_TASKSTOPPED, destSetName,
						fileName, remoteSubDirName, newFileName,
						l_pid, l_destSetObj);
			}
			ServR::cmdHandlerListMX_.release();

			return AES_CDH_RC_OK;
		}
	}
	taskMX_.release();
	return AES_CDH_RC_NOTASK;
}


/*===================================================================
   ROUTINE:stopTasks
=================================================================== */
AES_CDH_ResultCode ServR::stopTasks(ACE_INT32 pid, ACE_INT32 destSetObj, bool allTasks)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	dumpFileTasks();

	std::list<sendFileTask>::iterator it;

	taskMX_.acquire();

	for( it = sendFileTaskList_.begin(); it != sendFileTaskList_.end(); )
	{
		std::list<sendFileTask>::iterator tempItr = it;
		++tempItr;
		if ( allTasks || ((*it).pid == pid && (*it).destSetObj == destSetObj) )
		{
			(*it).stopFlag = true;
			// TR HR49216
			if(((*it).pStopFileTransferEvent) != 0 )
			{
				(*it).pStopFileTransferEvent->signal();
			}
			TransDestSet* pdestset = 0;
			TransDest* pdest = 0;
			AES_CDH_ResultCode result = getDestSet((*it).destSetName, pdestset);
			AES_CDH_TRACE_MESSAGE("destSetName: %s", (*it).destSetName.c_str());
			if (result == AES_CDH_RC_OK)
			{
				// from destSet get Active Destination
				pdest = pdestset->activeDest;
				AES_CDH_TRACE_MESSAGE("destName: %s", (pdest->getDestinationName()).c_str());
			}

			CmdHandler* cmdHdlr;
			cmdHdlr = (*it).CmdHdlr;
			ACE_UINT64 ticks = (*it).ticks;
			ACE_INT32 l_pid = (*it).pid;
			ACE_INT32 l_destSetObj = (*it).destSetObj;
			string destSetName = (*it).destSetName;
			string fileName = (*it).fileName;
			string remoteSubDirName = (*it).remoteSubDirName;
			string newFileName = (*it).newFileName;
#if 0
			ACE_Thread_Semaphore sem(0);
			(*it).sem = &sem;     // used by ServR::sendFileDone, Signalled when Transdest has stopped file transfer.

			//taskMX_.release();

			ACE_Time_Value tv(ACE_OS::gettimeofday () + ACE_Time_Value(10));
			AES_CDH_LOG(LOG_LEVEL_WARN,"(%t) In ServR::stopTasks, waiting for ServR::sendFileDone for 5 secs"));
			if (sem.acquire(tv) != 0)
			{
				AES_CDH_LOG(LOG_LEVEL_WARN,"(%t) In ServR::stopTasks, timeout(5secs) occured in waiting for ServR::sendFileDone "));
				// Semafore not signalled in time.
				taskMX_.acquire();
				(*it).sem = NULL;
				taskMX_.release();
				ServR::cmdHandlerListMX_.acquire();
				if(validCmdHandler(cmdHdlr, ticks))   // INGO3 GOH qabhall 010815 TR: HC77134
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "(%t) %s\n", "sending sendfile reply due to file transfer stopped AES_CDH_RC_INTPROGERR"));

					cmdHdlr->sendFileReply(AES_CDH_RC_INTPROGERR, destSetName,
							fileName, remoteSubDirName, newFileName,
							l_pid, l_destSetObj);
				}
				ServR::cmdHandlerListMX_.release();
				string newdr;
				if (AES_GCC_Util::datapath_trn(fileName,AES_DATA_PATH,newdr)==false)
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "Directory name sent for event %s",newdr.c_str()));
					newdr = "Not Available";
				}
				Event::report(AES_CDH_intProgFault, "INTERNAL PROGRAM ERROR",
						"Failed to stop task : " + destSetName +
						"," + newdr + "," + remoteSubDirName + "," +
						newFileName, "-");

				return AES_CDH_RC_INTPROGERR;
			}
#endif
			AES_CDH_TRACE_MESSAGE("ServR:stoptasks() in sendfiletasklist-for");

			ServR::cmdHandlerListMX_.acquire();
			if(validCmdHandler(cmdHdlr, ticks))   // INGO3 GOH qabhall 010815 TR: HC77134
			{
				AES_CDH_TRACE_MESSAGE("sending sendfile reply due AES_CDH_RC_TASKSTOPPED");

				cmdHdlr->sendFileReply(AES_CDH_RC_TASKSTOPPED,
						destSetName,
						fileName,
						remoteSubDirName,
						newFileName,
						l_pid,
						l_destSetObj);

			}
			ServR::cmdHandlerListMX_.release();
		}
		it = tempItr;
	}

	taskMX_.release();

	dumpRecordTasks();

	std::list<sendRecordFileTask>::iterator itr;

	recordtaskMX_.acquire();

	for ( itr = sendRecordFileTaskList_.begin();
			itr != sendRecordFileTaskList_.end(); ++itr )
	{

		if ( allTasks || ((*itr).pid == pid && (*itr).destSetObj == destSetObj) )
		{
			(*itr).stopFlag = true;

			CmdHandler *cmdHdlr;
			cmdHdlr = (*itr).CmdHdlr;
			ACE_UINT64 ticks = (*itr).ticks;

			if(validCmdHandler(cmdHdlr, ticks))   // INGO3 GOH qabhall 010815 TR: HC77134
			{
				//                cmdHdlr->sendRecordFileReply( AES_CDH_RC_TASKSTOPPED,
				//                                              (*itr).destName,0); // destName ??
			}

			AES_CDH_TRACE_MESSAGE("ServR::stopTasks() Block task for %s stopped", (*itr).destSetName.c_str());
		}
	}

	recordtaskMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:checkStopFlag
=================================================================== */
bool ServR::checkStopFlag(const string destSetName,
		const string fileName,
		const string remoteSubDirName,
		const string newFileName)
{
	std::list<sendFileTask>::iterator it;

	taskMX_.acquire();

	for ( it = sendFileTaskList_.begin(); it != sendFileTaskList_.end(); ++it )
	{
		if ( (*it).destSetName == destSetName &&
				(*it).fileName == fileName &&
				(*it).remoteSubDirName == remoteSubDirName &&
				(*it).newFileName == newFileName )
		{
			taskMX_.release();
			return (*it).stopFlag;
		}
	}
	taskMX_.release();
	return false;
}


/*===================================================================
   ROUTINE:stopSendRecordFile
=================================================================== */
AES_CDH_ResultCode ServR::stopSendRecordFile(const string destSetName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	std::list<sendRecordFileTask>::iterator itr;

	recordtaskMX_.acquire();
	for (itr = sendRecordFileTaskList_.begin(); itr != sendRecordFileTaskList_.end(); ++itr)
	{
		if ((*itr).destSetName == destSetName)
		{
			(*itr).stopFlag = true;
			AES_CDH_LOG(LOG_LEVEL_INFO, "Task set to STOPPED for %s", (*itr).destSetName.c_str());
		}
	}

	recordtaskMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:checkStopRecordFlag
=================================================================== */
bool ServR::checkStopRecordFlag(const string destSetName,
		const string mainFileName)
{
	ACE_INT32 list_size;
	AES_CDH_TRACE_MESSAGE("ServR::checkStopRecordFlag, entering  destSetName = %s mainFileName = %s", destSetName.c_str(),mainFileName.c_str());

	std::list<sendRecordFileTask>::iterator it;

	recordtaskMX_.acquire();

	list_size = sendRecordFileTaskList_.size();

	for ( it = sendRecordFileTaskList_.begin(); it != sendRecordFileTaskList_.end(); ++it )
	{
		if ( (*it).destSetName == destSetName && (*it).mainFileName == mainFileName )
		{
			recordtaskMX_.release();

			AES_CDH_LOG(LOG_LEVEL_INFO, "Task is set to %s --- %d", (*it).stopFlag == true ? "STOPPED" : "RUNNING", list_size);
			return (*it).stopFlag;
		}
	}
	recordtaskMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving, task is NOT STOPPED --- %d", list_size);

	return false;
}

/*===================================================================
   ROUTINE:ExistRecordFileTask
=================================================================== */
bool ServR::ExistRecordFileTask(const string destSetName,const string mainFileName,CmdHandler *cmdHdlr, ACE_UINT64 ticks)
{
	ACE_INT32 list_size;
	AES_CDH_TRACE_MESSAGE("Entering  destSetName = %s mainFileName = %s", destSetName.c_str(),mainFileName.c_str());

	dumpRecordTasks();

	std::list<sendRecordFileTask>::iterator it;

	recordtaskMX_.acquire();

	list_size = sendRecordFileTaskList_.size();

	for ( it = sendRecordFileTaskList_.begin(); it != sendRecordFileTaskList_.end(); ++it )
	{
		if ( (*it).destSetName == destSetName && (*it).mainFileName == mainFileName )
		{
			if( (*it).CmdHdlr == cmdHdlr && (*it).ticks == ticks)
			{
				recordtaskMX_.release();

				AES_CDH_TRACE_MESSAGE("task exist!!!");
				return true;
			}
		}
	}
	recordtaskMX_.release();
	AES_CDH_LOG(LOG_LEVEL_TRACE, "task NOT EXIST!!!");

	return false;
}

/*===================================================================
   ROUTINE:IsStandardDestOption
=================================================================== */
bool ServR::IsStandardDestOption(const string option)
{
	const ACE_TCHAR cOption = *(option.c_str()+1);
	bool bResult = true;

	if (*(option.c_str()) != '-')
		return bResult;

	// Check if the option is one of the "standard" CDH options
	switch (cOption)
	{
	case 'a':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'h':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'x':
	case 'y':
	case 'z':
		bResult = true;
		break;
	default:
		bResult = false;
		break;
	};

	return bResult;
}


/*===================================================================
   ROUTINE:saveDestinations
=================================================================== */
AES_CDH_ResultCode ServR::saveDestinations()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	AES_CDH_TRACE_MESSAGE("Leaving");
	return rcode;
}

/*===================================================================
   ROUTINE:saveDestinationSets
=================================================================== */
AES_CDH_ResultCode ServR::saveDestinationSets()
{
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	return rcode;
}

/*===================================================================
   ROUTINE:NTErrorText
=================================================================== */
string ServR::NTErrorText()
{
	string err;
	return err;
}

/*===================================================================
   ROUTINE:getDataBaseDirectory
=================================================================== */
string ServR::getDataBaseDirectory()
{
	return dataBaseDirectory;
}

//Start - HU10376
/*===================================================================
   ROUTINE:getPsConfiguredIP
=================================================================== */
bool ServR::getPsConfiguredIP(const string& notificationAddress, string &strHostAddr)
{
	bool retVal = false;
	char temp[512] = {0};
	sprintf(temp,ipRouteCmd.c_str(),notificationAddress.c_str());
	FILE *fp = 0;

	//Execute the iproute command using popen.
	if((fp = popen(temp, "r")) == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while executing IP ROUTE COMMAND. ErrorCode: %d",errno);
		AES_CDH_TRACE_MESSAGE("Error occured while executing IP ROUTE COMMAND. ErrorCode: %d",errno);
	}
	else
	{   // popen() is sucessful
		// Get the output in to a buffer
		char strBuffer[1024] = {0};
		char *cmdOutputStr = NULL;
		int status = 0;
		if(NULL != (cmdOutputStr = fgets(strBuffer,1023,fp)))
		{
			// address read successfully from fp.
			AES_CDH_TRACE_MESSAGE("strBuffer = %s", strBuffer);

			//Check for only ETH2 if PS is defined
			string ipRouteOutput(strBuffer);

			if(string::npos != ipRouteOutput.find("eth2"))
			{
				AES_CDH_TRACE_MESSAGE("ETH2 found in IPROUTE CMD");

				size_t hostIpIndex = ipRouteOutput.find("src");
				string hostIp = ipRouteOutput.substr(hostIpIndex + AES_CDH_IPROUTE_HOST_OFFSET);

				//Get the Active Node IP and compare with the HostAddress
				string activeNodePsIp = readIpAddressFromFile(AES_CDH_PUBLIC2_NODE_ADDRESS);
				AES_CDH_LOG(LOG_LEVEL_INFO, "activeNodePsIp: %s and HostIP: %s.CHECK SPACES IN THE END",activeNodePsIp.c_str(),hostIp.c_str());
				AES_CDH_TRACE_MESSAGE("activeNodePsIp: %s and HostIP: %s.CHECK SPACES IN THE END",activeNodePsIp.c_str(),hostIp.c_str());

				//Trim the String in the end
				vector<string> splitVector;
				stringstream hostIpStream(hostIp);
				string tokenString;
				while(getline(hostIpStream, tokenString, ' '))
				{
					splitVector.push_back(tokenString);
				}

				//Append the Trimmed String
				hostIp = splitVector[0];
				AES_CDH_LOG(LOG_LEVEL_INFO, "hostIP = %s CHECK SPACES IN THE END", hostIp.c_str());
				AES_CDH_TRACE_MESSAGE("hostIP = %s CHECK SPACES IN THE END", hostIp.c_str());
				if(activeNodePsIp.compare(hostIp) == 0)
				{
					//Get the Cluster IP for the PS Interface
					strHostAddr = readIpAddressFromFile(AES_CDH_PUBLIC2_CLUSTER_ADDRESS);
					AES_CDH_LOG(LOG_LEVEL_INFO, "PS is reachable to REMOTE. Fetching PS Cluster Address: %s",strHostAddr.c_str());
					AES_CDH_TRACE_MESSAGE("PS is reachable to REMOTE. Fetching PS Cluster Address: %s",strHostAddr.c_str());
					retVal = true;
				}
			}
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Failed to read the pipe. ErrorCode: %d",errno);
			AES_CDH_TRACE_MESSAGE("Failed to read the pipe. ErrorCode: %d",errno);
		}
		//Close the pipe
		status = pclose(fp);
		if (status == -1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "pclose failed, errno = %d",  errno);
			AES_CDH_TRACE_MESSAGE("pclose failed, errno = %d",  errno);
		}
	}
	return retVal;
}

/*===================================================================
   ROUTINE:readIpAddressFromFile
=================================================================== */
string ServR::readIpAddressFromFile(const string& filePath)
{
	// Intialise ipAddress to getClusterIPPap() to be used in case of failure
	string ipAddress = getClusterIPPap();
	std::string readIPaddressCmd = "cat ";
	readIPaddressCmd.append(filePath);
	FILE *fp = 0;
	int status = 0;

	//Read the address from /etc/cluster using popen.
	if(NULL == (fp = popen(readIPaddressCmd.c_str(), "r")))
	{
		AES_CDH_TRACE_MESSAGE("Error occured while getting the interface name");
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting the interface name");
	}
	else
	{
		// popen() is successful
		char strBuffer[256] = { 0 };
		if( fscanf(fp, "%255s", strBuffer )  <=  0)
		{
			// reading from the file is failed ...
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while reading interface name");
			AES_CDH_TRACE_MESSAGE("Error occured while reading interface name");
		}
		else
		{
			//Now copy the address in buffer to be returned to the caller.
			ipAddress.assign(strBuffer);
		}
		status = pclose(fp);

		if (-1 == status)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "pclose failed, errno = %d",  errno);
			AES_CDH_TRACE_MESSAGE("pclose failed, errno = %d",  errno);
		}
	}
	return ipAddress;
}
//End - HU10376
/*===================================================================
   ROUTINE:getClusterIP
=================================================================== */
string ServR::getClusterIP(string notificationAddress)
{
	std::string clusterIpAddr("0.0.0.0");
	//Get the physical separation status.
	OmHandler myOmHandler;
	if(ACS_CC_FAILURE == myOmHandler.Init())
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error in initializing OmHandler.");
		AES_CDH_TRACE_MESSAGE("Error in initializing OmHandler.");
	}
	else
	{
		ACE_INT32 physicalSepStatus = CDH_PHYSICAL_SEPARATION_DISABLED;
		ACS_CC_ImmParameter attrPSStatus;
		attrPSStatus.attrName = (char *)PHYSICAL_SEPARATION_ATTR_NAME;
		std::string dnName = NORTHBOUND_OBJ_DN;
		if (ACS_CC_SUCCESS == myOmHandler.getAttribute(dnName.c_str(), &attrPSStatus))
		{
			if (attrPSStatus.attrName == (char*)PHYSICAL_SEPARATION_ATTR_NAME )
			{
				physicalSepStatus = *(reinterpret_cast<ACE_INT32*>(*(attrPSStatus.attrValues)));
				AES_CDH_TRACE_MESSAGE("Physical Separation Status = %d", physicalSepStatus);
				AES_CDH_LOG(LOG_LEVEL_INFO, "Physical Separation Status = %d", physicalSepStatus);
			}
			myOmHandler.Finalize();

			std::string strHostAddr("0.0.0.0");
			if((CDH_PHYSICAL_SEPARATION_ENBLED == physicalSepStatus) && getPsConfiguredIP(notificationAddress, strHostAddr)) //PS is configured
			{
				//Start - HU10376
				clusterIpAddr.assign(strHostAddr);
				//End - HU10376
			}
			else
			{	//Note: PS feature is disabled in VIRTUAL environment
				// In case of PS is not enabled.
				ACS_APGCC_CommonLib objCommonLib;
				ACS_APGCC_IpAddress_ReturnTypeT retValue = ACS_APGCC_IpAddress_FAILURE;
				ACS_APGCC_ApgAddresses_R2 apgAddr;
				retValue = objCommonLib.getIpAddresses(apgAddr);

				if (ACS_APGCC_IpAddress_FAILURE == retValue)
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "%s", "Error in getting cluster IP Addr from APGCC");
					AES_CDH_TRACE_MESSAGE("Error in getting cluster IP Addr from APGCC");
				}
				else
				{
					AES_CDH_TRACE_MESSAGE("\nACS_APGCC_CommonLib::getIpAddresses() returned -> \n stackType = %d\nclusterIpAddress = %s\nclusterIp1 = %s\nclusterIp2 = %s\nclusterIpAddress_IPv6 = %s\nclusterIp1_IPv6 = %s\nclusterIp2_IPv6 = %s",
							apgAddr.stackType,apgAddr.clusterIpAddress.c_str(),apgAddr.clusterIp1.c_str(),apgAddr.clusterIp2.c_str(),apgAddr.clusterIpAddress_IPv6.c_str(),apgAddr.clusterIp1_IPv6.c_str(),apgAddr.clusterIp2_IPv6.c_str());
					if(apgAddr.stackType == ACS_APGCC_IPv4_STACK)
						clusterIpAddr.assign(apgAddr.clusterIp1);
					else if(apgAddr.stackType == ACS_APGCC_IPv6_STACK)
						clusterIpAddr.assign(apgAddr.clusterIp1_IPv6);
					else if(apgAddr.stackType == ACS_APGCC_DUAL_STACK)
					{
						// In case of dual stack, return the Cluster IP based on the IP version of attribute ipNotificationAddress
						// In NATIVE, IPv6 address is rejected for attr ipNotificationAddress by OI, so no VIRTUAL env check needed here.
						if(notificationAddress.find(".") != string::npos)
							clusterIpAddr.assign(apgAddr.clusterIp1);
						else if(notificationAddress.find(":") != string::npos)
							clusterIpAddr.assign(apgAddr.clusterIp1_IPv6);
					}
					AES_CDH_LOG(LOG_LEVEL_INFO, "Selected Cluster IP Address = %s", clusterIpAddr.c_str());
					AES_CDH_TRACE_MESSAGE("Selected Cluster IP Address = %s", clusterIpAddr.c_str());
				}
			}
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error in getting physical separation status from IMM");
			AES_CDH_TRACE_MESSAGE("Error in getting physical separation status from IMM");
			myOmHandler.Finalize();
		}
	}
	return clusterIpAddr;
}

/*===================================================================
   ROUTINE:getClusterIPPap
=================================================================== */
string ServR::getClusterIPPap()
{
        ACE_TCHAR name[32];
        ACE_INT32 result;
        ACE_TCHAR zbuf[256];
        result = gethostname(name, sizeof(name));
        if ( result != 0 )
        {
                return "0.0.0.0";
        }
        else
        {
                struct hostent* pHostInfo = 0;
                pHostInfo = gethostbyname(name);
                if (pHostInfo == NULL)
                {
                        pHostInfo = NULL;
                        return "0.0.0.0";
                }
                else
                {
                        struct in_addr h_addr;
                        h_addr.s_addr = *((ACE_UINT64 *) pHostInfo->h_addr_list[0]);
                        ACE_OS::sprintf(zbuf, "%s", inet_ntoa(h_addr));
                        pHostInfo = NULL;
                        return zbuf;
                }
        }
}


/*===================================================================
   ROUTINE:checkDestinationSetType
=================================================================== */
AES_CDH_ResultCode ServR::checkDestinationSetType(const string &destset,
		AES_CDH_DestinationSet::TQConnectAttributes askAttr,
		AES_CDH_DestinationSet::TQConnectAttributes &realAttr)
{
	(void)askAttr;
	TransDestSet *pdestset;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	rCode = getDestSet(destset, pdestset);

	if (rCode == AES_CDH_RC_OK)
	{
		rCode = pdestset->checkDestinationSetType(realAttr); // FILE or BLOCK
	}

	return rCode;
}


/*===================================================================
   ROUTINE:existDest
=================================================================== */
AES_CDH_ResultCode ServR::existDest(const string destName)
{
	std::list<TransDest*>::iterator itl;

	pthread_mutex_lock(&destMx_);

	for ( itl = destList_.begin(); itl != destList_.end(); ++itl )
	{
		if ( ((*itl)->getDestinationName()) == destName )
		{
			pthread_mutex_unlock(&destMx_);
			return AES_CDH_RC_OK;
		}
	}
	pthread_mutex_unlock(&destMx_);
	return AES_CDH_RC_NODEST;
}

/*===================================================================
   ROUTINE:existDestSet
=================================================================== */
AES_CDH_ResultCode ServR::existDestSet(const string destSetName)
{
	std::list<TransDestSet*>::iterator itl;

	destSetMX_.acquire();

	for ( itl = destSetList_.begin(); itl != destSetList_.end(); ++itl )
	{
		if ( ((*itl)->getDestinationSetName()) == destSetName )
		{
			destSetMX_.release();
			return AES_CDH_RC_OK;
		}
	}
	destSetMX_.release();
	return AES_CDH_RC_NODESTSET;
}


/*===================================================================
   ROUTINE:getDest
=================================================================== */
AES_CDH_ResultCode ServR::getDest(const string destName,TransDest* &pdest)
{
	std::list<TransDest*>::iterator itl;

	pthread_mutex_lock(&destMx_);

	for ( itl = destList_.begin(); itl != destList_.end(); ++itl )
	{
#ifdef DEBUGTEST
		cout<<" Destination Names in server are "<<(*itl)->getDestinationName() <<endl;
#endif
		if ( ((*itl)->getDestinationName()) == destName )

		{
			pdest = (*itl);

			pthread_mutex_unlock(&destMx_);
			return AES_CDH_RC_OK;
		}
	}
	pthread_mutex_unlock(&destMx_);
	return AES_CDH_RC_NODEST;
}

/*===================================================================
   ROUTINE:getDestList
=================================================================== */
const std::list<TransDest*>* ServR::getDestList()
{
	return &destList_;
}

/*===================================================================
   ROUTINE:checkOtherIncompatibleRespDestExists
=================================================================== */
bool ServR::checkOtherIncompatibleRespDestExists(const string& destName,
		const string& vdDest,
		const ACE_INT32 KeepTime)
{
	std::list<TransDest*>::iterator iterList;
	vector<string>::iterator iter;
	AES_CDH_Destination::destAttributes attr;
	bool bResponding = false;
	bool bSuppressDirCreation = false;
	bool bKeepDest = false;

	pthread_mutex_lock(&destMx_);

	for ( iterList = destList_.begin(); iterList != destList_.end(); ++iterList )
	{
		TransDest* pDest = (*iterList);
		if (pDest->getDestinationName() != destName)
		{
			pDest->getAttr(attr);

			if (ACE_OS::strcmp(attr.destPath.c_str(), vdDest.c_str()) == 0)
			{
				bResponding = false;
				bSuppressDirCreation = false;
				bKeepDest = false;

				for (iter = attr.otherAttrs.begin(); iter != attr.otherAttrs.end(); ++iter)
				{
					if (*(iter) == "-c" && *(iter+1) == "r")
						bResponding = true;
					else if (*(iter) == "-g" && *(iter+1) == "yes")
						bSuppressDirCreation = true;
					else if (*(iter) == "-b" )
						bKeepDest = true;

					// We have found a destination that has the same VD path and suppress
					// subdirectory creation
					if (bResponding && bSuppressDirCreation)
					{
						if (KeepTime>-1 || bKeepDest)
						{
							attr.otherAttrs.clear();
							pthread_mutex_unlock(&destMx_);
							return true;
						}
					}

					++iter;
				}
			}
		}

		attr.otherAttrs.clear();
	}

	pthread_mutex_unlock(&destMx_);

	return false;
}
// END HG12384

/*===================================================================
   ROUTINE:checkOtherRespDestExists
=================================================================== */
bool ServR::checkOtherRespDestExists(const string& destName,
		const string& vdDest)
{
	std::list<TransDest*>::iterator iterList;
	vector<string>::iterator iter;
	AES_CDH_Destination::destAttributes attr;
	bool bResponding = false;
	bool bSuppressDirCreation = false;

	pthread_mutex_lock(&destMx_);

	for ( iterList = destList_.begin(); iterList != destList_.end(); ++iterList )
	{
		TransDest* pDest = (*iterList);
		if (pDest->getDestinationName() != destName)
		{
			pDest->getAttr(attr);

			if (ACE_OS::strcmp(attr.destPath.c_str(), vdDest.c_str()) == 0)
			{
				bResponding = false;
				bSuppressDirCreation = false;

				for (iter = attr.otherAttrs.begin(); iter != attr.otherAttrs.end(); ++iter)
				{
					if (*(iter) == "-c" && *(iter+1) == "r")
						bResponding = true;
					else if (*(iter) == "-g" && *(iter+1) == "yes")
						bSuppressDirCreation = true;

					// We have found a destination that has the same VD path and suppress
					// subdirectory creation
					if (bResponding && bSuppressDirCreation)
					{
						attr.otherAttrs.clear();
						pthread_mutex_unlock(&destMx_);
						return true;
					}

					++iter;
				}
			}
		}

		attr.otherAttrs.clear();
	}

	pthread_mutex_unlock(&destMx_);

	return false;
}

/*===================================================================
   ROUTINE:checkRespDestHasKeep
=================================================================== */
bool ServR::checkRespDestHasKeep(TransDest* pDest)
{
	vector<string>::iterator iter;
	AES_CDH_Destination::destAttributes attr;

	pthread_mutex_lock(&destMx_);

	pDest->getAttr(attr);
	for (iter = attr.otherAttrs.begin(); iter != attr.otherAttrs.end(); ++iter)
	{
		if (*(iter) == "-b")
		{
			attr.otherAttrs.clear();
			pthread_mutex_unlock(&destMx_);
			return true;
		}

		++iter;
	}


	attr.otherAttrs.clear();

	pthread_mutex_unlock(&destMx_);

	return false;
}

/*===================================================================
   ROUTINE:getDestSet
=================================================================== */
AES_CDH_ResultCode ServR::getDestSet(const string destSetName,TransDestSet* &destSet)
{
	std::list<TransDestSet*>::iterator itr;

	destSetMX_.acquire();
	AES_CDH_TRACE_MESSAGE("Entering %s", destSetName.c_str());
	for ( itr = destSetList_.begin(); itr != destSetList_.end(); ++itr )
	{
		if ( ((*itr)->getDestinationSetName()) == destSetName )
		{
			destSet = (*itr);

			destSetMX_.release();
			AES_CDH_TRACE_MESSAGE("Leaving");
			return AES_CDH_RC_OK;
		}
	}

	destSetMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");
	return AES_CDH_RC_NODESTSET;
}

/*===================================================================
   ROUTINE:setDataPath
=================================================================== */
bool ServR::setDataPath()
{
	string aesDataDirectory;
	string cdhRootDirectory;

	// Read parameters from PHA
	if ( readParameters(aesDataDirectory, cdhRootDirectory) )
	{
		// Assemble path to CDH root directory
		destfile_.assign(aesDataDirectory);
		destfile_.append("/");
		destfile_.append(cdhRootDirectory);
		destfile_.append("/");
		destfile_.append(destDirectory);
		destfile_.append("/");

		// The file cdhdestex.txt is located in the same path,
		// so we copy the path
		destfileex_ = destfile_;

		// Assemble complete path to CDH destination file
		destfile_.append(destfileName);

		resetFileInstanceTags(destfile_); //HF86848

		// Assemble complete path to CDH extended destination file
		destfileex_.append(destfileExName);

		resetFileInstanceTags(destfileex_); //HF86848

		// Assemble path to CDH destination set file
		destSetFile_.assign(aesDataDirectory);
		destSetFile_.append("/");
		destSetFile_.append(cdhRootDirectory);
		destSetFile_.append("/");
		destSetFile_.append(destSetDirectory);
		destSetFile_.append("/");
		destSetFile_.append(destSetFileName);

		return true;
	}
	else
	{
		return false;
	}
}


/*===================================================================
   ROUTINE:readParameters
=================================================================== */
bool ServR::readParameters(string &path1, string &path2)
{

	// Get parameters from PHA
	if (AES_CDH_Paths::instance()->getCDHDataPath( path1 ) )
	{
		path2 = AES_CDH_RootDirectory;
		return true;
	}
	else
	{
		// Event report, could not get parameters from PHA
		Event::report(AES_CDH_variableFault, "VARIABLE PROBLEM",
				"Retrieval of variable (" + string(AES_CDH_AesDataDirectory) +
				") from FCC failed.", "-");
	}

	return false;
}


/*===================================================================
   ROUTINE:recoverDestinations
=================================================================== */
void ServR::recoverDestinations()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	string destName(""), emptyDestSet("");
	string transType("");
	string errtext("");
	string userGroup = "";
	std::vector<std::string> respsetDnList;
	std::vector<std::string> initsetDnList;
	std::vector<std::string> blocksetDnList;
	std::vector<std::string>::iterator respsetDnListItr;
	std::vector<std::string>::iterator initsetDnListItr;
	std::vector<std::string>::iterator blocksetDnListItr;
	string myClassName("");

	DestinationAlarm::AlarmRecord::thisCause ValarmCause;	// added for TR HH91483

	OmHandler myOmHandler;
	if( myOmHandler.Init() == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while initializing OmHandler");
		AES_CDH_TRACE_MESSAGE("Leaving");
		return ;
	}

	//Get the instances of the Responding DestinationSet class.
	myClassName = AES_CDH_RESP_DEST_SET_CLASS_NM; 

	myOmHandler.getClassInstances(myClassName.c_str(), respsetDnList);

	AES_CDH_TRACE_MESSAGE("Instances found for %s = %d", myClassName.c_str(), int(respsetDnList.size()));
	//Get the instances of the Initiating DestinationSet class.

	myClassName = AES_CDH_INIT_DEST_SET_CLASS_NM;

	myOmHandler.getClassInstances(myClassName.c_str(), initsetDnList);

	AES_CDH_TRACE_MESSAGE("Instances found for %s = %d", myClassName.c_str(), int(initsetDnList.size()));
	//Get the instances of the Initiating DestinationSet class.

	myClassName = AES_CDH_BLOCK_DEST_SET_CLASS_NM;

	myOmHandler.getClassInstances(myClassName.c_str(), blocksetDnList);

	AES_CDH_TRACE_MESSAGE("Instances found for %s = %d", myClassName.c_str(), int(blocksetDnList.size()));
	//Responding destination set
	// To be uncommented once FileM is running -- Madhavi
	for ( respsetDnListItr = respsetDnList.begin() ; (ServR::isStopEventSignalled != true )&& (respsetDnListItr != respsetDnList.end()) ; ++respsetDnListItr)
	{
		string myrespDestsetName(""),myDestinationName1,myDestinationName2;
		string myRespDestDn(""), myrespSecDestDn("") , myAdvRespParamsDn(""),myRespDestsetDn("");
		string myDestSetName("");
		string rdn1=(*respsetDnListItr);

		ACE_UINT32 keeptime,responsetime,notificationPortNo, notificationPortNo2;
		ACE_INT32 fileovr,TQprefix,resendtimer;
		bool keeptimef(false),fileovrf(false),TQprefixf(false),responsetimef(false),resendtimerf(false);
		string myNotificationAddr(""),myNotificationAddr2("");
		bool portnumf(false),portnumf2(false), PRIMARY(false),SECONDARY(false);

		ACE_INT32 argc = 0;
		ACE_INT32 i = 0;
		ACE_TCHAR* argv[32] = {0};


		//Invoke getObject for dn and retrieve all the parameters.
		ACS_APGCC_ImmObject myDestsetObj;
		myDestsetObj.objName=(*respsetDnListItr);

		if( myOmHandler.getObject(&myDestsetObj) == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, " Unable to retrieve attributes for %s", myDestsetObj.objName.c_str());
			continue;
		}
		std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr1;

		for ( attributesListItr1 = myDestsetObj.attributes.begin();
				attributesListItr1 != myDestsetObj.attributes.end() ;
				++attributesListItr1 )
		{

			//get destset name
			if( (*attributesListItr1).attrName == AES_CDH_RESP_DEST_SET_ID) //AES_CDH_LOCAL_DEST_RDN )
			{
				myRespDestsetDn = reinterpret_cast<char *>((*attributesListItr1).attrValues[0]);
				string::size_type pos1 = string::npos;
				unsigned int pos2 = myRespDestsetDn.length();
				pos1 = myRespDestsetDn.find_first_of("=");
				if(( pos1 != string::npos) && pos1 < pos2 )
				{
					myrespDestsetName = myRespDestsetDn.substr(pos1+1, pos2-pos1-1);
				}
			}
		}
		std::vector<std::string>  primrespdestList;
		string primrespdestDn("");
		std::string	myPrimRespDestClassName = AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM;
		if( myOmHandler.getClassInstances( myPrimRespDestClassName.c_str(), primrespdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_TRACE_MESSAGE("No instances of Primary Responding Destination for destset: %s", myrespDestsetName.c_str());
		}
		for( unsigned int pdestCtr = 0 ; pdestCtr < primrespdestList.size(); pdestCtr++ )
		{
			size_t pos1 = string(primrespdestList[pdestCtr]).find_first_of(",");
			if( primrespdestList[pdestCtr].substr(pos1+1) == (*respsetDnListItr ))
			{
				primrespdestDn = ( primrespdestList[pdestCtr]);
			}
		}
		if( !primrespdestDn.empty())
		{
			PRIMARY=true;

			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myrespDestObj;
			myrespDestObj.objName =  primrespdestDn ;

			if( myOmHandler.getObject(&myrespDestObj) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, " Unable to retrieve attributes for %s", myrespDestObj.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr2;
			AES_CDH_TRACE_MESSAGE("Creating primary responding destination");

			for ( attributesListItr2 = myrespDestObj.attributes.begin();
					attributesListItr2 != myrespDestObj.attributes.end() ;
					++attributesListItr2 )
			{

				if ( (*attributesListItr2).attrName==AES_CDH_PRIM_RESP_DESTINATION_ID)
				{
					myRespDestDn = (reinterpret_cast<char *>((*attributesListItr2).attrValues[0]));
					AES_CDH_LOG(LOG_LEVEL_INFO, "  RDN = %s", myRespDestDn.c_str());
					size_t pos2 = myRespDestDn.length();
					size_t pos1 = myRespDestDn.find_first_of("=");
					if(( pos1 != string::npos) && pos1 < pos2 )
					{
						myDestinationName1 = myRespDestDn.substr(pos1+1, pos2-pos1-1);
					}
				}
				else if ( (*attributesListItr2).attrName == AES_CDH_RESP_NOTIFICATION_ADDR)
				{
					if((*attributesListItr2).attrValuesNum!=0)
					{
						char *mytmp = reinterpret_cast<char *>((*attributesListItr2).attrValues[0]);
						myNotificationAddr.assign(mytmp);
					}

				}
				else if ((*attributesListItr2).attrName == AES_CDH_RESP_NOTIFICATION_PORT_NO )
				{
					if((*attributesListItr2).attrValuesNum !=0 )
					{
						portnumf=true;
						ACE_UINT32 mytmp = *reinterpret_cast<ACE_UINT32 *>((*attributesListItr2).attrValues[0]);
						notificationPortNo = mytmp;
					}

				}
			}//end of for (attributes of primary)
		}//end for primary class
		std::vector<std::string>  advrespdestList;
		string advrespdestDn("");
		std::string	myAdvRespDestClassName = AES_CDH_ADV_RESP_PARAMS_CLASS_NM;
		if( myOmHandler.getClassInstances( myAdvRespDestClassName.c_str(), advrespdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_TRACE_MESSAGE("No instances of Advanced Responding Destination for destset: %s", myrespDestsetName.c_str());
		}
		for( unsigned int advdestCtr = 0 ; advdestCtr < advrespdestList.size(); advdestCtr++ )
		{
			size_t pos1 = string(advrespdestList[advdestCtr]).find_first_of(",");
			if( advrespdestList[advdestCtr].substr(pos1+1) == (*respsetDnListItr ))
			{
				advrespdestDn = ( advrespdestList[advdestCtr]);
			}
		}
		if( !advrespdestDn.empty())
		{
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myrespAdvObj;
			myrespAdvObj.objName =  (advrespdestDn) ;

			if( myOmHandler.getObject(&myrespAdvObj) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_DEBUG, " Unable to retrieve attributes for %s", myrespAdvObj.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr3;
			AES_CDH_TRACE_MESSAGE("Getting Advanced parameters of responding destination");

			for ( attributesListItr3 = myrespAdvObj.attributes.begin();
					attributesListItr3 != myrespAdvObj.attributes.end() ;
					++attributesListItr3 )
			{

				if ((*attributesListItr3).attrName == AES_CDH_ADV_RESP_PARAM_ID)
				{
					myAdvRespParamsDn = (reinterpret_cast<char *>((*attributesListItr3).attrValues[0]));
					AES_CDH_TRACE_MESSAGE("RDN = %s", myAdvRespParamsDn.c_str());
				}
				else if ((*attributesListItr3).attrName == AES_CDH_RESP_DEST_KEEPTIME  )
				{
					if((*attributesListItr3).attrValuesNum != 0)
					{
						keeptime = *reinterpret_cast<ACE_UINT32 *>((*attributesListItr3).attrValues[0]);
						keeptimef = true;
						AES_CDH_TRACE_MESSAGE("Keep Time = %d", keeptime);
					}
				}
				else if ( (*attributesListItr3).attrName == AES_CDH_RESP_FILE_OVERWRITE )
				{
					if( (*attributesListItr3).attrValuesNum !=0 )
					{
						fileovr = *reinterpret_cast<ACE_INT32 *>((*attributesListItr3).attrValues[0]);
						fileovrf = true;
						AES_CDH_TRACE_MESSAGE("File Overwrite Option is %d", fileovr);
					}
				}
				else if ( (*attributesListItr3).attrName == AES_CDH_REMOVE_TQ_PREFIX )
				{
					if((*attributesListItr3).attrValuesNum !=0 )
					{
						TQprefix = *reinterpret_cast<ACE_INT32 *>((*attributesListItr3).attrValues[0]);
						TQprefixf = true;
						AES_CDH_TRACE_MESSAGE("Remove File Prefix  = %d", TQprefix);
					}
				}
				else if ( (*attributesListItr3).attrName == AES_CDH_RESPONSE_TIMER )
				{
					if((*attributesListItr3).attrValuesNum != 0)
					{
						responsetime = *reinterpret_cast<ACE_UINT32 *>((*attributesListItr3).attrValues[0]);
						responsetimef = true;
						AES_CDH_TRACE_MESSAGE( "Response Time = %d", responsetime);
					}
				}
				else if ( (*attributesListItr3).attrName == AES_CDH_RESEND_NOTIFICATION_TIMER)
				{
					if((*attributesListItr3).attrValuesNum !=0 )
					{
						resendtimer = *reinterpret_cast<ACE_INT32 *>((*attributesListItr3).attrValues[0]);
						resendtimerf = true;
						AES_CDH_TRACE_MESSAGE("Resend Notification Time = %d", resendtimer);
					}
				}
			}//end of for loop (attributes of adv)

		}//end of if for advancved parameter type
		std::vector<std::string>  secrespdestList;
		string secrespdestDn("");
		std::string	mySecRespDestClassName = AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM;
		if( myOmHandler.getClassInstances( mySecRespDestClassName.c_str(), secrespdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_TRACE_MESSAGE("No instances of Secondary Responding Destination for destset: %s", myrespDestsetName.c_str());
		}
		for( unsigned int secdestCtr = 0 ; secdestCtr < secrespdestList.size(); secdestCtr++ )
		{
			size_t pos1 = string(secrespdestList[secdestCtr]).find_first_of(",");
			if( secrespdestList[secdestCtr].substr(pos1+1) == (*respsetDnListItr ))
			{
				secrespdestDn = ( secrespdestList[secdestCtr]);
			}
		}
		if( !secrespdestDn.empty())
		{
			//get the secondary destination attributes.
			SECONDARY=true;
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myrespDestObj2;
			myrespDestObj2.objName =  secrespdestDn ;

			if( myOmHandler.getObject(&myrespDestObj2) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Unable to retrieve attributes for %s", myrespDestObj2.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr4;
			AES_CDH_TRACE_MESSAGE("Getting attributes of secondary responding destination");

			for ( attributesListItr4 = myrespDestObj2.attributes.begin();
					attributesListItr4 != myrespDestObj2.attributes.end() ;
					++attributesListItr4)
			{

				if ((*attributesListItr4).attrName == AES_CDH_SEC_RESP_DESTINATION_ID )
				{
					myrespSecDestDn = (reinterpret_cast<char *>((*attributesListItr4).attrValues[0]));
					AES_CDH_TRACE_MESSAGE("  RDN = %s", myrespSecDestDn.c_str());
					size_t pos1 = string::npos;
					size_t pos2 = myrespSecDestDn.length();
					pos1 = myrespSecDestDn.find_first_of("=");
					if(( pos1 != string::npos) && pos1 < pos2 )
					{
						myDestinationName2 = myrespSecDestDn.substr(pos1+1, pos2-pos1-1);
					}
				}
				else if ( (*attributesListItr4).attrName == AES_CDH_RESP_NOTIFICATION_ADDR )
				{
					if((*attributesListItr4).attrValuesNum!=0)
					{
						char *mytmp = reinterpret_cast<char *>((*attributesListItr4).attrValues[0]);
						myNotificationAddr2.assign(mytmp);
					}

				}
				else if ( (*attributesListItr4).attrName == AES_CDH_RESP_NOTIFICATION_PORT_NO )
				{
					if((*attributesListItr4).attrValuesNum!=0)
					{
						portnumf2 = true;
						ACE_UINT32 mytmp = *reinterpret_cast<ACE_UINT32 *>((*attributesListItr4).attrValues[0]);
						notificationPortNo2 = mytmp;
					}

				}
			}//end of for (attributes of secondary)
		}//end for secondary class
		if (PRIMARY)//creating the primary destiantion
		{
			argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

			// Copy connection type
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-c");
			argv[argc] = new char[2];
			ACE_OS::strcpy(argv[argc++], "r");

			//Prepare argc and argv and invoke transdest, define.

			//Copy the notification address option
			if((myNotificationAddr!=""))
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-f");
				//Copy the notification address
				argv[argc] = new char[ACE_OS::strlen(myNotificationAddr.c_str())+1];
				ACE_OS::strcpy(argv[argc++], myNotificationAddr.c_str());
			}
			if( fileovrf)
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-o");
				switch( fileovr )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_YES);
					break;
				}
			}

		    //Enabling suppressSubFolderCreation in responding traffic might lead to problems in the traffic mix.
		    //So, the attribute made as hidden and the value set to false always.
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-g");
			argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
			ACE_OS::strcpy(argv[argc++],AES_NO);

			if( TQprefixf)
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-k");
				switch( TQprefix )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_YES);
					break;
				}
			}
			if(portnumf==true)
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-x");
				char myPortNo[11] = { 0};
				ACE_OS::sprintf( myPortNo, "%d", notificationPortNo);
				argv[argc] = new char[ACE_OS::strlen(myPortNo)+1];
				ACE_OS::strcpy(argv[argc++], myPortNo);
			}


			for( int ctr = 0 ; ctr < argc ;  ctr++)
			{
				if (ctr == 6)
				{
					AES_CDH_TRACE_MESSAGE(" Argument %d = *****",ctr); //do not print password
				}
				else
				{
					AES_CDH_TRACE_MESSAGE(" Argument %d = %s",ctr, argv[ctr]);
				}
			}


			// Define destination
			TransDest* pdest = new TransDest;

			if ( pdest != 0 ) // Allocation of new TransDest OK
			{
				rcode = pdest->define(myDestinationName1, AES_CDH_SFTPV2_TRANSFER_TYPE, argc, argv, userGroup, true, true, AES_CDH_PRIM_RESP_DESTINATION_ID, myrespDestsetName);
				if (rcode == AES_CDH_RC_OK)
				{
					pthread_mutex_lock(&destMx_); //HL76154
					destList_.push_back(pdest); // Add destination to list
					pthread_mutex_unlock(&destMx_); //HL76154
				}
				else
				{	// Event report, could not recover destination
					Event::report(AES_CDH_recoveryFault,
							"DESTINATION RECOVERY PROBLEM",
							myDestinationName1,
							errtext);
					// Cancel destination
					(void)pdest->remove();
					delete pdest;
					pdest = 0;
					// Get error text
					errtext = AES_CDH_Result(rcode).errorText();

					// raise alarm with new cause
					ValarmCause = DestinationAlarm::AlarmRecord::vd_error;	// added for TR HH91483
					DestinationAlarm::instance()->raise(myDestinationName1, emptyDestSet, ValarmCause);
					virtualAlarm_= true;
				}
			}
			else
			{
				// Event report, could not allocate memory for new destination
				Event::report(AES_CDH_memoryFault,
						"MEMORY ALLOCATION PROBLEM",
						"Storage space for destinations could not be allocated at server start-up",
						"-");
			}
			// Release argv
			for (i = 0; i < argc; i++)
			{
				delete [] argv[i];
				argv[i] = 0;
			}
			//define destset with this dest as primary
			string secDestName(""), backupDestName("");

			int result = ServR::defineDestSet(myrespDestsetName, myDestinationName1, secDestName,backupDestName,userGroup, AES_CDH_RESP_DEST_SET_ID);
			if (result != AES_CDH_RC_OK)
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "Error occured while defining destination");
			}
		}//end of if PRIMARY
		if(SECONDARY)
		{
			ACE_INT32 argc = 0;
			ACE_INT32 i = 0;
			ACE_TCHAR* argv[32] = {0};
			argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

			// Copy connection type
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-c");
			argv[argc] = new char[2];
			ACE_OS::strcpy(argv[argc++], "r");

			//Prepare argc and argv and invoke transdest, define.

			//Copy the notification address option
			if( myNotificationAddr2!="")
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-f");
				//Copy the notification address
				argv[argc] = new char[ACE_OS::strlen(myNotificationAddr2.c_str())+1];
				ACE_OS::strcpy(argv[argc++], myNotificationAddr2.c_str());
			}
			if( fileovrf)
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-o");
				switch( fileovr )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_YES);
					break;
				}
			}
			    //Enabling suppressSubFolderCreation in responding traffic might lead to problems in the traffic mix.
			    //So, the attribute made as hidden and the value set to false always.
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-g");
				argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
				ACE_OS::strcpy(argv[argc++],AES_NO);

			if( TQprefixf)
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-k");
				switch( TQprefix )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_YES);
					break;
				}
			}
			if(portnumf2==true)
			{
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-x");
				char myPortNo[11] = { 0};
				ACE_OS::sprintf( myPortNo, "%d", notificationPortNo2);
				argv[argc] = new char[ACE_OS::strlen(myPortNo)+1];
				ACE_OS::strcpy(argv[argc++], myPortNo);
			}

			for( int ctr = 0 ; ctr < argc ;  ctr++)
			{
				if (ctr == 6)
				{
					AES_CDH_TRACE_MESSAGE(" Argument %d = *****",ctr); //do not print password
				}
				else
				{
					AES_CDH_TRACE_MESSAGE(" Argument %d = %s",ctr, argv[ctr]);
				}
			}


			// Define destination
			TransDest* pdest2 = new TransDest;

			if ( pdest2 != 0 ) // Allocation of new TransDest OK
			{
				rcode = pdest2->define(myDestinationName2, AES_CDH_SFTPV2_TRANSFER_TYPE, argc, argv, userGroup, true, true, AES_CDH_SEC_RESP_DESTINATION_ID, myrespDestsetName);
				if (rcode == AES_CDH_RC_OK)
				{
					pthread_mutex_lock(&destMx_); //HL76154
					destList_.push_back(pdest2); // Add destination to list
					pthread_mutex_unlock(&destMx_); //HL76154
				}
				else
				{	// Event report, could not recover destination
					Event::report(AES_CDH_recoveryFault,
							"DESTINATION RECOVERY PROBLEM",
							myDestinationName2,
							errtext);
					// Cancel destination
					(void)pdest2->remove();
					delete pdest2;
					pdest2 = 0;

					// Get error text
					errtext = AES_CDH_Result(rcode).errorText();

					// raise alarm with new cause
					ValarmCause = DestinationAlarm::AlarmRecord::vd_error;	// added for TR HH91483
					DestinationAlarm::instance()->raise(myDestinationName2, emptyDestSet, ValarmCause);
					virtualAlarm_= true;
				}
			}
			else
			{
				// Event report, could not allocate memory for new destination
				Event::report(AES_CDH_memoryFault,
						"MEMORY ALLOCATION PROBLEM",
						"Storage space for destinations could not be allocated at server start-up",
						"-");
			}
			// Release argv
			for (i = 0; i < argc; i++)
			{
				delete [] argv[i];
				argv[i] = 0;
			}
			//define destset with this dest as primary
			string secDestName(""), backupDestName("");

			if( ServR::changeDestSet(myrespDestsetName, secDestName, myDestinationName2, backupDestName, userGroup) != AES_CDH_RC_OK )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while modifying Responding destset");
			}
		}//end of if secondary
	}//end of for loop of responding destinations/sets
	/* -- To be uncommented once fileM is running -- Madhavi
	}
	 */
	//For initiating destset and destinations
	for ( initsetDnListItr = initsetDnList.begin() ; ((ServR::isStopEventSignalled != true ) && (initsetDnListItr != initsetDnList.end())) ; ++initsetDnListItr)
	{
		string myinitDestsetName(""),myDestinationName1,myDestinationName2;
		string myinitDestDn(""), myinitSecDestDn("") , myAdvinitParamsDn(""),myInitDestsetDn("");
		string myDestSetName("");
		string rdn2=(*initsetDnListItr);

		ACE_INT32 fileOverwrite = -1;
		ACE_INT32 sendRetries = -1;
		ACE_INT32 retryDelay = -1;
		ACE_INT32 supSubFolderCreation = -1;

		string ipAddress1(""),ipAddress2(""),password1(""),password2(""),remoteFolderPath1(""),remoteFolderPath2(""),userName1(""),userName2(""),transferProtocol1(""),transferProtocol2("");
		ACE_INT32 portNumber1 = -1, portNumber2=-1;

		bool PRIMARY2(false),SECONDARY2(false);

		//Invoke getObject for dn and retrieve all the parameters.
		ACS_APGCC_ImmObject myDestsetObj;
		myDestsetObj.objName =  (*initsetDnListItr) ;

		if( myOmHandler.getObject(&myDestsetObj) == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to retrieve attributes for %s", myDestsetObj.objName.c_str());
			continue;
		}
		std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr5;

		for ( attributesListItr5 = myDestsetObj.attributes.begin();
				attributesListItr5 != myDestsetObj.attributes.end() ;
				++attributesListItr5 )
		{

			//get destset name
			if( (*attributesListItr5).attrName == AES_CDH_INIT_DEST_SET_ID) 
			{
				myInitDestsetDn = reinterpret_cast<char *>((*attributesListItr5).attrValues[0]);
				string::size_type pos1 = string::npos;
				unsigned int pos2 = myInitDestsetDn.length();
				pos1 = myInitDestsetDn.find_first_of("=");
				if(( pos1 != string::npos) && pos1 < pos2 )
				{
					myinitDestsetName = myInitDestsetDn.substr(pos1+1, pos2-pos1-1);
				}
			}
		}
		std::vector<std::string>  priminitdestList;
		string priminitdestDn("");
		std::string	myPrimInitDestClassName = AES_CDH_PRIMARYINITDEST_IMM_CLASS_NM;
		if( myOmHandler.getClassInstances( myPrimInitDestClassName.c_str(), priminitdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting instances of Primary Initiating Destination.");
		}
		for( unsigned int idestCtr = 0 ; idestCtr < priminitdestList.size(); idestCtr++ )
		{
			size_t pos1 = string(priminitdestList[idestCtr]).find_first_of(",");
			if( priminitdestList[idestCtr].substr(pos1+1) == (*initsetDnListItr ))
			{
				priminitdestDn = ( priminitdestList[idestCtr]);
			}
		}
		if( !priminitdestDn.empty())
		{
			//get the primary destination attributes.
			PRIMARY2=true;
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myinitDestObj;
			myinitDestObj.objName =  priminitdestDn;

			if( myOmHandler.getObject(&myinitDestObj) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to retrieve attributes for %s", myinitDestObj.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr6;
			AES_CDH_TRACE_MESSAGE("Getting attributes of primary initiating destination: %s", priminitdestDn.c_str());

			for ( attributesListItr6 = myinitDestObj.attributes.begin();
					attributesListItr6 != myinitDestObj.attributes.end() ;
					++attributesListItr6 )
			{

				if ( (*attributesListItr6).attrName == AES_CDH_PRIMARYINITDEST_RDN )
				{
					myinitDestDn = (reinterpret_cast<char *>((*attributesListItr6).attrValues[0]));
					AES_CDH_TRACE_MESSAGE("RDN = %s", myinitDestDn.c_str());
					unsigned int pos2 = myinitDestDn.length();
					size_t pos1 = myinitDestDn.find_first_of("=");
					if(( pos1 != string::npos) && pos1 < pos2 )
					{
						myDestinationName1 = myinitDestDn.substr(pos1+1, pos2-pos1-1);
					}

				}
				else if ((*attributesListItr6).attrName == AES_CDH_REMOTE_IP_ADDR)
				{

					char *mytmp = reinterpret_cast<char *>((*attributesListItr6).attrValues[0]);
					if( mytmp != 0 )
					{
						ipAddress1.assign(mytmp);

					}
				}
				else if ( (*attributesListItr6).attrName == AES_CDH_PASSWORD )
				{
					if((*attributesListItr6).attrValuesNum !=0 )
					{
						password1 = reinterpret_cast<char *>((*attributesListItr6).attrValues[0]);

						ACS_CC_ImmParameter attrPasswd;
						attrPasswd.attrName = (char *)AES_ECIM_PWD_ATTRIBUTE;
						std::string dnName = password1;
						//retrieving the password from ECIM password struct
						if (myOmHandler.getAttribute( dnName.c_str(), &attrPasswd) == ACS_CC_SUCCESS )
						{
							password1.clear();
							password1 = reinterpret_cast<char*>(attrPasswd.attrValues[0]);
							if(!password1.empty())
							{
								AES_CDH_TRACE_MESSAGE("Decrypting the password - %s", password1.c_str());
								char * plaintext = NULL;
								string ciphertext = password1;
								SecCryptoStatus decryptStatus = sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str());
								if(decryptStatus != SEC_CRYPTO_OK )
								{
									//decryption could fail for two reasons:
									// 1) the password is not encrypted at all. in this case there's no fault
									// 2) there's an internal error
									AES_CDH_LOG(LOG_LEVEL_ERROR, "Password decryption failed for %s ", myinitDestDn.c_str());
								}
								else
								{
									password1= plaintext;
								}

								if(plaintext !=0 )
								{
									free(plaintext);
								}
							}
							AES_CDH_TRACE_MESSAGE("destination Password = %s", password1.c_str());

						}
					}
				}
				else if ( (*attributesListItr6).attrName == AES_CDH_DEST_PORTNUMBER )
				{
					if((*attributesListItr6).attrValuesNum !=0 )
					{
						portNumber1 = *reinterpret_cast<ACE_INT32 *>((*attributesListItr6).attrValues[0]);
					}
				}
				else if ( (*attributesListItr6).attrName == AES_CDH_FILE_REMOTE_FOLDER_PATH )
				{
					if((*attributesListItr6).attrValuesNum!=0)
					{
						remoteFolderPath1 = reinterpret_cast<char *>((*attributesListItr6).attrValues[0]);
					}
				}
				else if ((*attributesListItr6).attrName == AES_CDH_FILE_USERNAME )
				{
					if ((*attributesListItr6).attrValuesNum!=0)
					{
						userName1 = reinterpret_cast<char *>((*attributesListItr6).attrValues[0]);
					}

				}
				else if ((*attributesListItr6).attrName == AES_CDH_TRANSFER_PROTOCOL )
				{
					if ((*attributesListItr6).attrValuesNum!=0)
					{
						ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>((*attributesListItr6).attrValues[0]);
						switch( mytmp )
						{
						case 0:
							transferProtocol1 = AES_CDH_SFTPV2_TRANSFER_TYPE;
							break;

						case 1:
							transferProtocol1 = AES_CDH_FTPV2_TRANSFER_TYPE;
							break;
						}
					}
				}
			}//end of for (attributes of primary)
		}//end for primary class
		std::vector<std::string>  advinitdestList;
		string advinitdestDn("");
		std::string	myAdvInitDestClassName = AES_CDH_ADVINITPARAMS_IMM_CLASS_NM;
		if( myOmHandler.getClassInstances( myAdvInitDestClassName.c_str(), advinitdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_TRACE_MESSAGE("No instances of Advanced Initiating Destination for destination set %s",myinitDestsetName.c_str());
		}
		for( unsigned int idestCtr = 0 ; idestCtr < advinitdestList.size(); idestCtr++ )
		{
			size_t pos1 = string(advinitdestList[idestCtr]).find_first_of(",");
			if( advinitdestList[idestCtr].substr(pos1+1) == (*initsetDnListItr ))
			{
				advinitdestDn = ( advinitdestList[idestCtr]);
			}
		}
		if( !advinitdestDn.empty() )
		{
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myinitAdvObj;
			myinitAdvObj.objName =  advinitdestDn ;

			if( myOmHandler.getObject(&myinitAdvObj) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to retrieve attributes for %s", myinitAdvObj.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr7;
			AES_CDH_TRACE_MESSAGE("Getting Advanced parameters of initiating destination");

			for ( attributesListItr7 = myinitAdvObj.attributes.begin();
					attributesListItr7 != myinitAdvObj.attributes.end() ;
					++attributesListItr7 )
			{
				// To avoid the issue of not displaying default values in cdhls printout after CDH service restart.
				// fileOverwrite,retryDelay,sendRetries,supSubFolderCreation values are assigned directly from IMM
				// without checking wthether it is default or not
				if( (*attributesListItr7).attrName == AES_CDH_FILE_FILEOVERWRITE )
				{
					if((*attributesListItr7).attrValuesNum != 0)
					{
						fileOverwrite = *reinterpret_cast<ACE_INT32 *>((*attributesListItr7).attrValues[0]);
					}
				}
				else if((*attributesListItr7).attrName == AES_CDH_FILE_RETRYDELAY )
				{
					if((*attributesListItr7).attrValuesNum != 0)       
					{
						retryDelay = *reinterpret_cast<ACE_INT32 *>((*attributesListItr7).attrValues[0]);
					}
				}
				else if ((*attributesListItr7).attrName == AES_CDH_FILE_SENDRETRY )
				{
					if((*attributesListItr7).attrValuesNum != 0)
					{
						sendRetries = *reinterpret_cast<ACE_INT32 *>((*attributesListItr7).attrValues[0]);
					}
				}
				else if((*attributesListItr7).attrName == AES_CDH_SUPPRESS_SUB_FOLDER_CREATION )
				{
					if((*attributesListItr7).attrValuesNum != 0)
					{
						supSubFolderCreation = *reinterpret_cast<ACE_INT32 *>((*attributesListItr7).attrValues[0]);
					}
				}

			}//end of for loop (attributes of adv)

		}//end of if loop of advancved parameter type
		std::vector<std::string>  secinitdestList;
		string secinitdestDn("");
		std::string	mySecInitDestClassName = AES_CDH_SECONDARYINITDEST_IMM_CLASS_NM;
		if( myOmHandler.getClassInstances( mySecInitDestClassName.c_str(), secinitdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_TRACE_MESSAGE("No instances of Secondary Initiating Destination for destination set %s",myinitDestsetName.c_str());
		}
		for( unsigned int idestCtr = 0 ; idestCtr < secinitdestList.size(); idestCtr++ )
		{
			size_t pos1 = string(secinitdestList[idestCtr]).find_first_of(",");
			if( secinitdestList[idestCtr].substr(pos1+1) == (*initsetDnListItr ))
			{
				secinitdestDn = ( secinitdestList[idestCtr]);
			}
		}
		if( !secinitdestDn.empty() )
		{
			//get the secondary destination attributes.
			SECONDARY2=true;
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myinitDestObj2;
			myinitDestObj2.objName =  secinitdestDn ;

			if( myOmHandler.getObject(&myinitDestObj2) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, " Unable to retrieve attributes for %s", myinitDestObj2.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr8;
			AES_CDH_TRACE_MESSAGE("Getting attributes of secondary initiating destination");

			for ( attributesListItr8 = myinitDestObj2.attributes.begin();
					attributesListItr8 != myinitDestObj2.attributes.end() ;
					++attributesListItr8)
			{
				if ( (*attributesListItr8).attrName == AES_CDH_SECONDARYINITDEST_RDN )
				{
					myinitDestDn = (reinterpret_cast<char *>((*attributesListItr8).attrValues[0]));
					AES_CDH_TRACE_MESSAGE("RDN = %s", myinitDestDn.c_str());
					unsigned int pos2 = myinitDestDn.length();
					size_t pos1 = myinitDestDn.find_first_of("=");
					if(( pos1 != string::npos) && pos1 < pos2 )
					{
						myDestinationName2 = myinitDestDn.substr(pos1+1, pos2-pos1-1);
					}

				}
				else if ((*attributesListItr8).attrName == AES_CDH_REMOTE_IP_ADDR)
				{
					char *mytmp = reinterpret_cast<char *>((*attributesListItr8).attrValues[0]);
					if( mytmp != 0 )
					{
						ipAddress2.assign(mytmp);

					}
				}
				else if ( (*attributesListItr8).attrName == AES_CDH_PASSWORD )
				{
					if((*attributesListItr8).attrValuesNum !=0 )
					{
						password2 = reinterpret_cast<char *>((*attributesListItr8).attrValues[0]);

						ACS_CC_ImmParameter attrPasswd;
						attrPasswd.attrName = (char *)AES_ECIM_PWD_ATTRIBUTE;
						std::string dnName = password2;
						//retrieving the password from ECIM password struct
						if ( myOmHandler.getAttribute( dnName.c_str(), &attrPasswd) == ACS_CC_SUCCESS )
						{
							password2.clear();
							password2 = reinterpret_cast<char*>(attrPasswd.attrValues[0]);
						}
						if(!password2.empty())
						{
							AES_CDH_TRACE_MESSAGE("Decrypting the sec dest password - %s", password2.c_str());
							char * plaintext = NULL;
							string ciphertext = password2;
							SecCryptoStatus decryptStatus = sec_crypto_decrypt_ecimpassword(&plaintext, ciphertext.c_str());
							if(decryptStatus != SEC_CRYPTO_OK )
							{
								//decryption could fail for two reasons:
								// 1) the password is not encrypted at all. in this case there's no fault
								// 2) there's an internal error
								AES_CDH_LOG(LOG_LEVEL_ERROR, "Password decryption failed for sec destination - %s ", myinitDestDn.c_str());
							}
							else
							{
								password2= plaintext;
							}

							if(plaintext !=0 )
							{
								free(plaintext);
							}
						}
						AES_CDH_TRACE_MESSAGE("sec destination Password = %s", password2.c_str());

					}
				}
				else if ( (*attributesListItr8).attrName == AES_CDH_DEST_PORTNUMBER )
				{
					if((*attributesListItr8).attrValuesNum!=0)
					{
						portNumber2 = *reinterpret_cast<ACE_INT32 *>((*attributesListItr8).attrValues[0]);
					}
				}
				else if ( (*attributesListItr8).attrName == AES_CDH_FILE_REMOTE_FOLDER_PATH )
				{
					if((*attributesListItr8).attrValuesNum!=0)
					{
						remoteFolderPath2 = reinterpret_cast<char *>((*attributesListItr8).attrValues[0]);
					}
				}
				else if ( (*attributesListItr8).attrName == AES_CDH_FILE_USERNAME )
				{
					if ((*attributesListItr8).attrValuesNum!=0)
					{
						userName2 = reinterpret_cast<char *>((*attributesListItr8).attrValues[0]);
					}

				}
				else if ( (*attributesListItr8).attrName == AES_CDH_TRANSFER_PROTOCOL )
				{
					if ((*attributesListItr8).attrValuesNum!=0)
					{
						ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>((*attributesListItr8).attrValues[0]);
						switch( mytmp )
						{
						case 0:
							transferProtocol2 = AES_CDH_SFTPV2_TRANSFER_TYPE;
							break;

						case 1:
							transferProtocol2 = AES_CDH_FTPV2_TRANSFER_TYPE;
							break;
						}
					}

				}

			}//end of for (attributes of secondary)
		}//end for secondary class
		if (PRIMARY2)//creating the primary destiantion
		{

			int argc = 0;
			char *argv[32] = { 0 };

			//Copy the name of the command.
			argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

			// Copy connection type
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-c");
			argv[argc] = new char[2];
			ACE_OS::strcpy(argv[argc++], "i");

			if( !ipAddress1.empty())
			{
				//Copy the destination address option.
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-a");

				//Copy the destination address.
				argv[argc] = new char[ipAddress1.size() + 1];
				ACE_OS::strcpy(argv[argc++], ipAddress1.c_str());
			}
			if( !password1.empty())
			{
				//Copy the password option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-p");

				//Copy the pasword
				argv[argc] = new char[password1.size() + 1];
				ACE_OS::strcpy(argv[argc++], password1.c_str());
			}
			if ( !userName1.empty())
			{
				//Copy the user name option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-u");

				//Copy the user name
				argv[argc] = new char[userName1.size() + 1];
				ACE_OS::strcpy(argv[argc++], userName1.c_str());
			}
			if ( !remoteFolderPath1.empty())
			{
				//Copy the remote folder path option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-r");

				//Copy the remote folder path
				argv[argc] = new char[remoteFolderPath1.size() + 1];
				ACE_OS::strcpy(argv[argc++], remoteFolderPath1.c_str());
			}
			if ( portNumber1 != -1)
			{
				//Copy the port number option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-n");

				//Copy the port number
				char myPortNo[11] = { 0 };
				ACE_OS::sprintf( myPortNo, "%d", portNumber1);
				argv[argc] = new char[ACE_OS::strlen(myPortNo)+1];
				ACE_OS::strcpy(argv[argc++], myPortNo);
			}

			if ( fileOverwrite != -1 )
			{
				// Copy the file overwrite option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-o");

				// Copy the file overwrite option
				switch( fileOverwrite )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
					break;
				}
			}

			if ( retryDelay != -1 )
			{
				// Copy the retryDelay option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-d");

				// Copy the retryDelay value
				char myRetryDelay[11] = { 0 };
				ACE_OS::sprintf( myRetryDelay, "%d", retryDelay);
				argv[argc] = new char[ACE_OS::strlen(myRetryDelay)+1];
				ACE_OS::strcpy(argv[argc++], myRetryDelay);

			}

			if ( sendRetries != -1 )
			{
				// Copy the sendRetries option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-s");

				// Copy the sendRetries value
				char mySendRetries[11] = { 0 };
				ACE_OS::sprintf( mySendRetries, "%d", sendRetries);
				argv[argc] = new char[ACE_OS::strlen(mySendRetries)+1];
				ACE_OS::strcpy(argv[argc++], mySendRetries);

			}

			if ( supSubFolderCreation != -1 )
			{
				// Copy the supSubFolderCreation option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-g");

				// Copy the supSubFolderCreation value
				switch( supSubFolderCreation )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
					break;
				}
			}

			for( int ctr = 0 ; ctr < argc ;  ctr++)
			{
				if (ctr == 6)
				{
					AES_CDH_TRACE_MESSAGE("Argument %d = *****",ctr); //do not print password
				}
				else
				{
					AES_CDH_TRACE_MESSAGE("Argument %d = %s",ctr, argv[ctr]);
				}
			}

			// Define destination
			TransDest* pdest = new TransDest;

			if ( pdest != 0 ) // Allocation of new TransDest OK
			{
				rcode = pdest->define(myDestinationName1, transferProtocol1, argc, argv, userGroup, true, true, AES_CDH_PRIMARYINITDEST_RDN, myinitDestsetName);
				if (rcode == AES_CDH_RC_OK)
				{
					pthread_mutex_lock(&destMx_); //HL76154
					destList_.push_back(pdest); // Add destination to list
					pthread_mutex_unlock(&destMx_); //HL76154
				}
				else
				{	// Event report, could not recover destination
					Event::report(AES_CDH_recoveryFault,
							"DESTINATION RECOVERY PROBLEM",
							myDestinationName1,
							errtext);
					// Cancel destination
					(void)pdest->remove();
					delete pdest;
					pdest = 0;

					// Get error text
					errtext = AES_CDH_Result(rcode).errorText();

					// raise alarm with new cause
					ValarmCause = DestinationAlarm::AlarmRecord::vd_error;	// added for TR HH91483
					DestinationAlarm::instance()->raise(myDestinationName1, emptyDestSet, ValarmCause);
					virtualAlarm_= true;
				}
			}
			else
			{
				// Event report, could not allocate memory for new destination
				Event::report(AES_CDH_memoryFault,
						"MEMORY ALLOCATION PROBLEM",
						"Storage space for destinations could not be allocated at server start-up",
						"-");
			}
			// Release argv
			for (int i = 0; i < argc; i++)
			{
				delete [] argv[i];
				argv[i] = 0;
			}

			//define destset with this dest as primary
			string secDestName(""), backupDestName("");

			int result = ServR::defineDestSet(myinitDestsetName, myDestinationName1, secDestName,backupDestName,userGroup, AES_CDH_INIT_DEST_SET_ID);
			if (result != AES_CDH_RC_OK)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while defining destination");
			}
		}//end of if PRIMARY

		if(SECONDARY2==true)
		{
			int argc = 0;
			char *argv[32] = { 0 };

			//Copy the name of the command.
			argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

			// Copy connection type
			argv[argc] = new char[3];
			ACE_OS::strcpy(argv[argc++], "-c");
			argv[argc] = new char[2];
			ACE_OS::strcpy(argv[argc++], "i");

			if( !ipAddress2.empty())
			{
				//Copy the destination address option.
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-a");

				//Copy the destination address.
				argv[argc] = new char[ipAddress2.size() + 1];
				ACE_OS::strcpy(argv[argc++], ipAddress2.c_str());
			}

			if( !password2.empty())
			{
				//Copy the password option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-p");

				//Copy the pasword
				argv[argc] = new char[password2.size() + 1];
				ACE_OS::strcpy(argv[argc++], password2.c_str());
			}

			if ( !userName2.empty())
			{
				//Copy the user name option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-u");

				//Copy the user name
				argv[argc] = new char[userName2.size() + 1];
				ACE_OS::strcpy(argv[argc++], userName2.c_str());
			}

			if ( !remoteFolderPath2.empty())
			{
				//Copy the remote folder path option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-r");

				//Copy the remote folder path
				argv[argc] = new char[remoteFolderPath2.size() + 1];
				ACE_OS::strcpy(argv[argc++], remoteFolderPath2.c_str());
			}


			if ( portNumber2 != -1)
			{
				//Copy the port number option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-n");

				//Copy the port number
				char myPortNo[11] = { 0 };
				ACE_OS::sprintf( myPortNo, "%d", portNumber2);
				argv[argc] = new char[ACE_OS::strlen(myPortNo)+1];
				ACE_OS::strcpy(argv[argc++], myPortNo);
			}

			if ( fileOverwrite != -1 )
			{
				// Copy the file overwrite option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-o");

				// Copy the file overwrite option
				switch( fileOverwrite )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
					break;
				}
			}

			if ( retryDelay != -1 )
			{
				// Copy the retryDelay option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-d");

				// Copy the retryDelay value
				char myRetryDelay[11] = { 0 };
				ACE_OS::sprintf( myRetryDelay, "%d", retryDelay);
				argv[argc] = new char[ACE_OS::strlen(myRetryDelay)+1];
				ACE_OS::strcpy(argv[argc++], myRetryDelay);

			}

			if ( sendRetries != -1 )
			{
				// Copy the sendRetries option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-s");

				// Copy the sendRetries value
				char mySendRetries[11] = { 0 };
				ACE_OS::sprintf( mySendRetries, "%d", sendRetries);
				argv[argc] = new char[ACE_OS::strlen(mySendRetries)+1];
				ACE_OS::strcpy(argv[argc++], mySendRetries);

			}

			if ( supSubFolderCreation != -1 )
			{
				// Copy the supSubFolderCreation option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-g");

				// Copy the supSubFolderCreation value
				switch( supSubFolderCreation )
				{
				case 0:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_NO)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_NO);
					break;

				case 1:
					argv[argc] = new char[ACE_OS::strlen(AES_CDH_INIT_YES)+1];
					ACE_OS::strcpy(argv[argc++],AES_CDH_INIT_YES);
					break;
				}
			}


			for( int ctr = 0 ; ctr < argc ;  ctr++)
			{
				if (ctr == 6)
				{
					AES_CDH_TRACE_MESSAGE("Argument %d = *****",ctr); //do not print password
				}
				else
				{
					AES_CDH_TRACE_MESSAGE("Argument %d = %s",ctr, argv[ctr]);
				}
			}


			// Define destination
			TransDest* pdest2 = new TransDest;

			if ( pdest2 != 0 ) // Allocation of new TransDest OK
			{
				rcode = pdest2->define(myDestinationName2, transferProtocol2, argc, argv, userGroup, true, true, AES_CDH_SECONDARYINITDEST_RDN, myinitDestsetName);
				if (rcode == AES_CDH_RC_OK)
				{
					pthread_mutex_lock(&destMx_); //HL76154
					destList_.push_back(pdest2); // Add destination to list
					pthread_mutex_unlock(&destMx_); //HL76154
				}
				else
				{	// Event report, could not recover destination
					Event::report(AES_CDH_recoveryFault,
							"DESTINATION RECOVERY PROBLEM",
							myDestinationName2,
							errtext);
					// Cancel destination
					(void)pdest2->remove();
					delete pdest2;
					pdest2 = 0;

					// Get error text
					errtext = AES_CDH_Result(rcode).errorText();

					// raise alarm with new cause
					ValarmCause = DestinationAlarm::AlarmRecord::vd_error;	// added for TR HH91483
					DestinationAlarm::instance()->raise(myDestinationName2, emptyDestSet, ValarmCause);
					virtualAlarm_= true;
				}
			}
			else
			{
				// Event report, could not allocate memory for new destination
				Event::report(AES_CDH_memoryFault,
						"MEMORY ALLOCATION PROBLEM",
						"Storage space for destinations could not be allocated at server start-up",
						"-");
			}
			// Release argv
			for (int i = 0; i < argc; i++)
			{
				delete [] argv[i];
				argv[i] = 0;
			}

			//Add secondary destination to destset
			string secDestName(""), backupDestName("");

			if( ServR::changeDestSet(myinitDestsetName, secDestName, myDestinationName2, backupDestName, userGroup) != AES_CDH_RC_OK )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, " Error occured while secondary ot destset");

			}
		}//end of if secondary
	}//end of for loop of initiating destinations/sets

	//For Block destset and destinations
	
	for ( blocksetDnListItr = blocksetDnList.begin() ; ((ServR::isStopEventSignalled != true ) && (blocksetDnListItr != blocksetDnList.end())) ; ++blocksetDnListItr)
	{
		AES_CDH_TRACE_MESSAGE("Recovering Block DestSets and Destinations");
		string myblockDestsetName(""),myblockDestinationName1,myblockDestinationName2;
		string myblockDestDn(""), myBlockSecDestDn("") , myAdvblockParamsDn(""),myblockDestsetDn("");
		string myblockDestSetName("");
		string rdn2=(*blocksetDnListItr);

		ACE_INT32 sendRetries = -1;
		ACE_INT32 retryDelay = -1;

		string ipAddress1(""),ipAddress2("");

		bool PRIMARY1(false),SECONDARY1(false);

		//Invoke getObject for dn and retrieve all the parameters.
		ACS_APGCC_ImmObject myblockDestsetObj;
		myblockDestsetObj.objName =  (*blocksetDnListItr) ;

		AES_CDH_TRACE_MESSAGE("To retrieve attributes for %s", myblockDestsetObj.objName.c_str());
		if( myOmHandler.getObject(&myblockDestsetObj) == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "Unable to retrieve attributes for %s", myblockDestsetObj.objName.c_str());
			continue;
		}
		std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr9;

		for ( attributesListItr9 = myblockDestsetObj.attributes.begin();
				attributesListItr9 != myblockDestsetObj.attributes.end() ;
				++attributesListItr9 )
		{

			//get destset name
			if( (*attributesListItr9).attrName == AES_CDH_BLOCK_DEST_SET_ID) 
			{
				myblockDestsetDn = reinterpret_cast<char *>((*attributesListItr9).attrValues[0]);
				string::size_type pos1 = string::npos;
				unsigned int pos2 = myblockDestsetDn.length();
				pos1 = myblockDestsetDn.find_first_of("=");
				if(( pos1 != string::npos) && pos1 < pos2 )
				{
					myblockDestsetName = myblockDestsetDn.substr(pos1+1, pos2-pos1-1);
				}
				AES_CDH_TRACE_MESSAGE("myblockDestsetName = %s",myblockDestsetName.c_str());
			}
		}
		std::vector<std::string>  primblockdestList;
		string primblockdestDn("");
		std::string	myPrimBlockDestClassName = AES_CDH_PRIMARY_BLOCK_DEST_IMM_CLASS_NM;
		if( myOmHandler.getClassInstances( myPrimBlockDestClassName.c_str(), primblockdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting instances of Primary block Destination.");
		}
		for( unsigned int idestCtr = 0 ; idestCtr < primblockdestList.size(); idestCtr++ )
		{
			size_t pos1 = string(primblockdestList[idestCtr]).find_first_of(",");
			if( primblockdestList[idestCtr].substr(pos1+1) == (*blocksetDnListItr ))
			{
				primblockdestDn = ( primblockdestList[idestCtr]);
			}
		}
		if( !primblockdestDn.empty())
		{
			//get the primary destination attributes.
			PRIMARY1=true;
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myblockDestObj;
			myblockDestObj.objName =  primblockdestDn;

			if( myOmHandler.getObject(&myblockDestObj) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Unable to retrieve attributes for %s", myblockDestObj.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr10;
			AES_CDH_TRACE_MESSAGE("Getting attributes of primary block destination: %s", primblockdestDn.c_str());

			for ( attributesListItr10 = myblockDestObj.attributes.begin();
					attributesListItr10 != myblockDestObj.attributes.end() ;
					++attributesListItr10 )
			{

				if ( (*attributesListItr10).attrName == AES_CDH_PRIMARY_BLOCK_DEST_RDN )
				{
					myblockDestDn = (reinterpret_cast<char *>((*attributesListItr10).attrValues[0]));
					AES_CDH_TRACE_MESSAGE("RDN = %s", myblockDestDn.c_str());
					unsigned int pos2 = myblockDestDn.length();
					size_t pos1 = myblockDestDn.find_first_of("=");
					if(( pos1 != string::npos) && pos1 < pos2 )
					{
						myblockDestinationName1 = myblockDestDn.substr(pos1+1, pos2-pos1-1);
					}

				}
				else if ((*attributesListItr10).attrName == AES_CDH_BLOCK_DEST_IP_ADDR)
				{

					char *mytmp = reinterpret_cast<char *>((*attributesListItr10).attrValues[0]);
					if( mytmp != 0 )
					{
						ipAddress1.assign(mytmp);

					}
				}
			}
		}//end for primary class
		std::vector<std::string>  advblockdestList;
		string advblockdestDn("");
		std::string	myAdvBlockDestClassName = AES_CDH_BLOCK_ADV_PARAMS_IMM_CLASS_NM;
		if( myOmHandler.getClassInstances( myAdvBlockDestClassName.c_str(), advblockdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_TRACE_MESSAGE("No instances of Advanced Block Destination for destination set %s",myblockDestsetName.c_str());
		}
		for( unsigned int idestCtr = 0 ; idestCtr < advblockdestList.size(); idestCtr++ )
		{
			size_t pos1 = string(advblockdestList[idestCtr]).find_first_of(",");
			if( advblockdestList[idestCtr].substr(pos1+1) == (*blocksetDnListItr ))
			{
				advblockdestDn = ( advblockdestList[idestCtr]);
			}
		}
		if( !advblockdestDn.empty() )
		{
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myblockAdvObj;
			myblockAdvObj.objName =  advblockdestDn ;

			if( myOmHandler.getObject(&myblockAdvObj) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to retrieve attributes for %s", myblockAdvObj.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr11;
			AES_CDH_TRACE_MESSAGE("Getting Advanced parameters of Block destination");

			for ( attributesListItr11 = myblockAdvObj.attributes.begin();
					attributesListItr11 != myblockAdvObj.attributes.end() ;
					++attributesListItr11 )
			{

				if((*attributesListItr11).attrName == AES_CDH_BLOCK_RETRYDELAY )
				{
					ACE_INT32 mytmp = *reinterpret_cast<ACE_INT32 *>((*attributesListItr11).attrValues[0]);
					retryDelay = mytmp;
				}
				else if ((*attributesListItr11).attrName == AES_CDH_BLOCK_SENDRETRY )
				{
					ACE_INT32 mytmp  = *reinterpret_cast<ACE_INT32 *>((*attributesListItr11).attrValues[0]);
					sendRetries = mytmp;
				}

			}//end of for loop (attributes of adv)

		}//end of if loop of advancved parameter type
		std::vector<std::string>  secblockdestList;
		string secblockdestDn("");
		std::string	mySecBlockDestClassName = AES_CDH_SECONDARY_BLOCK_DEST_IMM_CLASS_NM;
		if( myOmHandler.getClassInstances( mySecBlockDestClassName.c_str(), secblockdestList) == ACS_CC_FAILURE )
		{
			AES_CDH_TRACE_MESSAGE("No instances of Secondary Block Destination for destination set %s",myblockDestsetName.c_str());
		}
		for( unsigned int idestCtr = 0 ; idestCtr < secblockdestList.size(); idestCtr++ )
		{
			size_t pos1 = string(secblockdestList[idestCtr]).find_first_of(",");
			if( secblockdestList[idestCtr].substr(pos1+1) == (*blocksetDnListItr ))
			{
				secblockdestDn = ( secblockdestList[idestCtr]);
			}
		}
		if( !secblockdestDn.empty() )
		{
			//get the secondary destination attributes.
			SECONDARY1=true;
			//Invoke getObject for dn and retrieve all the parameters.
			ACS_APGCC_ImmObject myblockDestObj2;
			myblockDestObj2.objName =  secblockdestDn ;

			if( myOmHandler.getObject(&myblockDestObj2) == ACS_CC_FAILURE )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, " Unable to retrieve attributes for %s", myblockDestObj2.objName.c_str());
				continue;
			}
			std::vector<ACS_APGCC_ImmAttribute>::iterator attributesListItr12;
			AES_CDH_TRACE_MESSAGE("Getting attributes of secondary block destination");

			for ( attributesListItr12 = myblockDestObj2.attributes.begin();
					attributesListItr12 != myblockDestObj2.attributes.end() ;
					++attributesListItr12)
			{
				if ( (*attributesListItr12).attrName == AES_CDH_SECONDARY_BLOCK_DEST_RDN)
				{
					myblockDestDn = (reinterpret_cast<char *>((*attributesListItr12).attrValues[0]));
					AES_CDH_TRACE_MESSAGE("RDN = %s", myblockDestDn.c_str());
					unsigned int pos2 = myblockDestDn.length();
					size_t pos1 = myblockDestDn.find_first_of("=");
					if(( pos1 != string::npos) && pos1 < pos2 )
					{
						myblockDestinationName2 = myblockDestDn.substr(pos1+1, pos2-pos1-1);
					}

				}
				else if ((*attributesListItr12).attrName == AES_CDH_BLOCK_DEST_IP_ADDR)
				{
					char *mytmp = reinterpret_cast<char *>((*attributesListItr12).attrValues[0]);
					if( mytmp != 0 )
					{
						ipAddress2.assign(mytmp);

					}
				}

			}//end of for (attributes of secondary)
		}//end for secondary class
		if (PRIMARY1)//creating the primary destiantion
		{

			int argc = 0;
			char *argv[32] = { 0 };

			//Copy the name of the command.
			argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

			if( !ipAddress1.empty())
			{
				//Copy the destination address option.
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-a");

				//Copy the destination address.
				argv[argc] = new char[ipAddress1.size() + 1];
				ACE_OS::strcpy(argv[argc++], ipAddress1.c_str());
			}

			if ( retryDelay != -1 )
			{
				// Copy the retryDelay option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-m");

				// Copy the retryDelay value
				char myRetryDelay[11] = { 0 };
				ACE_OS::sprintf( myRetryDelay, "%d", retryDelay);
				argv[argc] = new char[ACE_OS::strlen(myRetryDelay)+1];
				ACE_OS::strcpy(argv[argc++], myRetryDelay);

			}

			if ( sendRetries != -1 )
			{
				// Copy the sendRetries option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-s");

				// Copy the sendRetries value
				char mySendRetries[11] = { 0 };
				ACE_OS::sprintf( mySendRetries, "%d", sendRetries);
				argv[argc] = new char[ACE_OS::strlen(mySendRetries)+1];
				ACE_OS::strcpy(argv[argc++], mySendRetries);

			}


			for( int ctr = 0 ; ctr < argc ;  ctr++)
			{
				if (ctr == 6)
				{
					AES_CDH_TRACE_MESSAGE(" Argument %d = *****",ctr); //do not print password
				}
				else
				{
					AES_CDH_TRACE_MESSAGE("Argument %d = %s",ctr, argv[ctr]);
				}
			}

			// Define destination
			TransDest* pdest = new TransDest;

			if ( pdest != 0 ) // Allocation of new TransDest OK
			{
				rcode = pdest->define(myblockDestinationName1, "BGWRPC", argc, argv, userGroup, true, true, AES_CDH_PRIMARY_BLOCK_DEST_RDN);
				if (rcode == AES_CDH_RC_OK)
				{
					pthread_mutex_lock(&destMx_); //HL76154
					destList_.push_back(pdest); // Add destination to list
					pthread_mutex_unlock(&destMx_); //HL76154
				}
				else
				{	// Event report, could not recover destination
					Event::report(AES_CDH_recoveryFault,
							"DESTINATION RECOVERY PROBLEM",
							myblockDestinationName1,
							errtext);
					// Cancel destination
					(void)pdest->remove();
					delete pdest;
					pdest = 0;

					// Get error text
					errtext = AES_CDH_Result(rcode).errorText();

					// raise alarm with new cause
					ValarmCause = DestinationAlarm::AlarmRecord::vd_error;	// added for TR HH91483
					DestinationAlarm::instance()->raise(myblockDestinationName1, emptyDestSet, ValarmCause);
					virtualAlarm_= true;
				}
			}
			else
			{
				// Event report, could not allocate memory for new destination
				Event::report(AES_CDH_memoryFault,
						"MEMORY ALLOCATION PROBLEM",
						"Storage space for destinations could not be allocated at server start-up",
						"-");
			}
			// Release argv
			for (int i = 0; i < argc; i++)
			{
				delete [] argv[i];
				argv[i] = 0;
			}

			//define destset with this dest as primary
			string secDestName(""), backupDestName("");

			int result = ServR::defineDestSet(myblockDestsetName, myblockDestinationName1, secDestName,backupDestName,userGroup, AES_CDH_BLOCK_DEST_SET_ID);
			if (result != AES_CDH_RC_OK)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR,"Error occured while defining destination");
			}
		}//end of if PRIMARY

		if(SECONDARY1==true)
		{
			int argc = 0;
			char *argv[32] = { 0 };

			//Copy the name of the command.
			argv[argc] = new char [ ACE_OS::strlen(AES_CDH_CDHDEFINE) + 1] ;
			ACE_OS::strcpy( argv[argc++], AES_CDH_CDHDEFINE);

			if( !ipAddress2.empty())
			{
				//Copy the destination address option.
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-a");

				//Copy the destination address.
				argv[argc] = new char[ipAddress2.size() + 1];
				ACE_OS::strcpy(argv[argc++], ipAddress2.c_str());
			}

			if ( retryDelay != -1 )
			{
				// Copy the retryDelay option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-m");

				// Copy the retryDelay value
				char myRetryDelay[11] = { 0 };
				ACE_OS::sprintf( myRetryDelay, "%d", retryDelay);
				argv[argc] = new char[ACE_OS::strlen(myRetryDelay)+1];
				ACE_OS::strcpy(argv[argc++], myRetryDelay);

			}

			if ( sendRetries != -1 )
			{
				// Copy the sendRetries option
				argv[argc] = new char[3];
				ACE_OS::strcpy(argv[argc++], "-s");

				// Copy the sendRetries value
				char mySendRetries[11] = { 0 };
				ACE_OS::sprintf( mySendRetries, "%d", sendRetries);
				argv[argc] = new char[ACE_OS::strlen(mySendRetries)+1];
				ACE_OS::strcpy(argv[argc++], mySendRetries);

			}



			for( int ctr = 0 ; ctr < argc ;  ctr++)
			{
				if (ctr == 6)
				{
					AES_CDH_TRACE_MESSAGE(" Argument %d = *****",ctr); //do not print password
				}
				else
				{
					AES_CDH_TRACE_MESSAGE(" Argument %d = %s",ctr, argv[ctr]);
				}
			}


			// Define destination
			TransDest* pdest2 = new TransDest;

			if ( pdest2 != 0 ) // Allocation of new TransDest OK
			{
				rcode = pdest2->define(myblockDestinationName2, "BGWRPC", argc, argv, userGroup, true, true, AES_CDH_SECONDARY_BLOCK_DEST_RDN);
				if (rcode == AES_CDH_RC_OK)
				{
					pthread_mutex_lock(&destMx_); //HL76154
					destList_.push_back(pdest2); // Add destination to list
					pthread_mutex_unlock(&destMx_); //HL76154
				}
				else
				{	// Event report, could not recover destination
					Event::report(AES_CDH_recoveryFault,
							"BLOCK DESTINATION RECOVERY PROBLEM",
							myblockDestinationName2,
							errtext);
					// Cancel destination
					(void)pdest2->remove();
					delete pdest2;
					pdest2 = 0;

					// Get error text
					errtext = AES_CDH_Result(rcode).errorText();

					// raise alarm with new cause
					ValarmCause = DestinationAlarm::AlarmRecord::vd_error;	// added for TR HH91483
					DestinationAlarm::instance()->raise(myblockDestinationName2, emptyDestSet, ValarmCause);
					virtualAlarm_= true;
				}
			}
			else
			{
				// Event report, could not allocate memory for new destination
				Event::report(AES_CDH_memoryFault,
						"MEMORY ALLOCATION PROBLEM",
						"Storage space for destinations could not be allocated at server start-up",
						"-");
			}
			// Release argv
			for (int i = 0; i < argc; i++)
			{
				delete [] argv[i];
				argv[i] = 0;
			}

			//Add secondary destination to destset
			string secDestName(""), backupDestName("");

			if( ServR::changeDestSet(myblockDestsetName, secDestName, myblockDestinationName2, backupDestName, userGroup) != AES_CDH_RC_OK )
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR," Error occured while secondary block destset");

			}
		}//end of if secondary
	}//end of for loop of block destinations/sets

	myOmHandler.Finalize();

	AES_CDH_TRACE_MESSAGE("Leaving");
}

void extractNameFromDn (const string dnName, string& name)
{
	name = "";

	string::size_type pos1 = string::npos;

	string::size_type pos2 = string::npos;

	pos1 = dnName.find_first_of("=");
	pos2 = dnName.find_first_of(",");
	if(( pos1 != string::npos) && (pos2 != string::npos)&& pos1 < pos2 )
	{
		name = dnName.substr(pos1+1, pos2-pos1-1);
	}
}

/*===================================================================
   ROUTINE:setEventHandler
=================================================================== */
AES_CDH_ResultCode ServR::setEventHandler(const string destSetName,
		const string transferQueueName,
		const string alarmText,
		CmdHandler* cmdHdlr)
{
	AES_CDH_ResultCode rcode;
	TransDestSet*  pdestset;
	rcode = getDestSet(destSetName, pdestset);

        destSetPairMX_.acquire(); // TR HO68058 Added, not in APG/W.
	if (destSetPair_.find(destSetName) == destSetPair_.end()) {
		destSetPair_.insert(pair<string, CmdHandler*>(destSetName, cmdHdlr));
	}
        destSetPairMX_.release(); // TR HO68058 Added, not in APG/W.

	if(rcode == AES_CDH_RC_OK)
		pdestset->setEventHandler(transferQueueName, alarmText, cmdHdlr);

	return rcode;
}


CmdHandler* ServR::getCmdHandler(const string destSetName)
{
	map<string, CmdHandler*>::iterator it;

        destSetPairMX_.acquire(); // TR HO68058 Added, not in APG/W.
	it = destSetPair_.find(destSetName);
	if (it == destSetPair_.end()) {
               destSetPairMX_.release(); // TR HO68058 Added, not in APG/W.
		// no match
		return NULL;
	}
        destSetPairMX_.release(); // TR HO68058 Added, not in APG/W.

	return (*it).second;
}

/*===================================================================
   ROUTINE:removeEventHandler
=================================================================== */
AES_CDH_ResultCode ServR::removeEventHandler(const string destSetName,
		CmdHandler* cmdHdlr)
{
	AES_CDH_ResultCode rcode;
	TransDestSet*  pdestset;
	map<string, CmdHandler*>::iterator it;
	rcode = getDestSet(destSetName, pdestset);

	AES_CDH_TRACE_MESSAGE("destSetName = %s",destSetName.c_str());

        destSetPairMX_.acquire(); // TR HO68058 Added, not in APG/W.
	it = destSetPair_.find(destSetName);
	if (it != destSetPair_.end()) {
		destSetPair_.erase(it);
	}
        destSetPairMX_.release(); // TR HO68058 Added, not in APG/W.

	if(rcode == AES_CDH_RC_OK)
		pdestset->removeEventHandler(cmdHdlr);
	AES_CDH_TRACE_MESSAGE("exit with retcode = %d",rcode);
	return rcode;
}


/*===================================================================
   ROUTINE:setDataPathForStatusFile
=================================================================== */
bool ServR::setDataPathForStatusFile()
{
	string aesDataDirectory;
	string cdhRootDirectory;

	// Read parameters from PHA
	if ( readParameters(aesDataDirectory, cdhRootDirectory) )
	{
		// Assemble path to CDH root directory
		fileSentStatusFile_.assign(aesDataDirectory);
		fileSentStatusFile_.append("/");
		fileSentStatusFile_.append(cdhRootDirectory);
		fileSentStatusFile_.append("/");
		fileSentStatusFile_.append(destDirectory);
		fileSentStatusFile_.append("/");

		// Assemble complete path to fileSentStatus backup file
		fileSentStatusFile_.append(fstatname);

		taskSentFileStatus_MX_.acquire();

		resetFileInstanceTags(fileSentStatusFile_); //HF86848

		taskSentFileStatus_MX_.release();

		return true;
	}
	else
	{
		return false;
	}
}


/*===================================================================
   ROUTINE:fileSentStatusSave
=================================================================== */
bool ServR::fileSentStatusSave(const string userUnique,
		const string filename)
{
	std::list<status *>::iterator statusIter;
	std::string dataBuffer;
	taskSentFileStatus_MX_.acquire();

	// don't allow empty strings as parameters
	if ((! userUnique.compare("")) || (! filename.compare("")))
	{
		taskSentFileStatus_MX_.release();
		return false;
	}

	// if userUnique is found in list, delete this post

	for (statusIter = statusList_.begin();
			statusIter != statusList_.end(); ++statusIter)
	{
		if (( *statusIter)->filedest == userUnique)
		{
			delete (*statusIter);
			statusList_.erase(statusIter);
			break;
		}
	}

	// Update the fileStatusList

	struct status *sp = new (status);
	sp->filedest = userUnique;
	sp->fname = filename;
	statusList_.insert(statusList_.end(), sp);

	// write the fileStatusList to the status file

	for (statusIter = statusList_.begin();
			statusIter != statusList_.end(); ++statusIter)
	{
		dataBuffer = (*statusIter)->filedest.c_str();
		dataBuffer += " ";
		dataBuffer += (*statusIter)->fname.c_str();
		dataBuffer += "\n";

		AES_CDH_TRACE_MESSAGE("fileSentStatusFile_ =  %s",fileSentStatusFile_.c_str());
		if(!writeToFile(fileSentStatusFile_, dataBuffer)) //HF86848
		{
			taskSentFileStatus_MX_.release();
			return false;
		}
	}

	taskSentFileStatus_MX_.release();
	return true;
}


/*===================================================================
   ROUTINE:fileSentStatusRead
=================================================================== */
bool ServR::fileSentStatusRead(const string userUnique,
		const string filename,
		bool &fileIsSent)
{
	taskSentFileStatus_MX_.acquire();
	std::list<status *>::iterator statusIter;

	// check if combination userUnique and filename is in the list

	fileIsSent=false;
	for (statusIter = statusList_.begin()
			;statusIter != statusList_.end(); ++statusIter)
	{
		if (((*statusIter)->filedest == userUnique) &&
				((*statusIter)->fname == filename))
		{
			fileIsSent=true;
			break;
		}
	}

	taskSentFileStatus_MX_.release();
	return true;  // no error found
}


/*===================================================================
   ROUTINE:fileSentStatusRecover
=================================================================== */
bool ServR::fileSentStatusRecover()
{
	taskSentFileStatus_MX_.acquire();

	string textline, fname, senderId;
	ACE_INT32 pos;

	// open the status file for read
	ifstream infile( fileSentStatusFile_.c_str());

	if (infile) {

		// read the data from the status file and put the data
		// into the fileStatusList

		while (getline(infile,textline))
		{
			// read from the status file
			pos = textline.find_first_of(" ");
			senderId = textline.substr(0,pos);
			fname = textline.substr(pos+1);

			// put the data in the fileStatusList
			struct status *sp = new (status);
			sp->filedest = senderId;
			sp->fname = fname;
			statusList_.insert(statusList_.end(), sp);

		}
	}
	else
	{
		//"infile is not opened"
		taskSentFileStatus_MX_.release();
		return false;
	}


	// close the status file
	infile.close();
	taskSentFileStatus_MX_.release();

	return true;
}


/*===================================================================
   ROUTINE:fileSentStatusRemove
=================================================================== */
bool ServR::fileSentStatusRemove(const string userUnique)
{
	std::list<status *>::iterator statusIter;
	std::string dataBuffer;
	taskSentFileStatus_MX_.acquire();

	// remove the specified item from the fileStatusList
	bool found = false;
	for (statusIter = statusList_.begin()
			;statusIter != statusList_.end(); ++statusIter)
	{
		if ((*statusIter)->filedest == userUnique) {
			delete (*statusIter);
			statusList_.erase(statusIter);
			found=true;
			break;
		}
	}

	// Open fileSentStatus file for write
	// If file does not exist, it will be created
	// If file exists, the contents will be discarded
	if (!found)
	{
		taskSentFileStatus_MX_.release();
		return false;
	}

	// write the fileStatusList to the status file
	for (statusIter = statusList_.begin();
			statusIter != statusList_.end(); ++statusIter)
	{
		dataBuffer = (*statusIter)->filedest.c_str();
		dataBuffer += " ";
		dataBuffer += (*statusIter)->fname.c_str();
		dataBuffer += "\n";

		if(!writeToFile(fileSentStatusFile_, dataBuffer)) //HF86848
		{
			taskSentFileStatus_MX_.release();
			return false;
		}
	}

	taskSentFileStatus_MX_.release();
	return true;
}

/*===================================================================
   ROUTINE:removeUserUnique
=================================================================== */
AES_CDH_ResultCode ServR::removeUserUnique(const string userUnique)
{
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;

	ServR::fileSentStatusRemove(userUnique);

	return rcode;
}


/*===================================================================
   ROUTINE:checkConnection
=================================================================== */
AES_CDH_ResultCode ServR::checkConnection(string destName, bool force,
		vector<checkConnAttributes> &attrs)
{
	std::list<TransDest*>::iterator itl;
	TransDest* pdest;
	AES_CDH_ResultCode rCode=AES_CDH_RC_ERROR;
	checkConnAttributes attr;

	pthread_mutex_lock(&destMx_);

	for ( itl = destList_.begin(); itl != destList_.end(); ++itl)
	{
		if(destName != "")
		{
			if ( ((*itl)->getDestinationName()) == destName )
			{
				pdest = *itl;
				rCode = pdest->checkConnection(force);

				attr.destName = destName;
				attr.status = rCode;
				attrs.push_back(attr);

				// Check if the destination has file notification
				if ( pdest->isNotificationDefined() == true )
				{
					rCode = pdest->checkNotification();

					attr.destName = destName;
					attr.status = rCode;
					attrs.push_back(attr);
				}
				else    // Notification is not defined
				{

					attr.destName = destName;

					// Destination not defined with file notification
					attr.status = AES_CDH_RC_NOTNOTIFDEST;

					attrs.push_back(attr);
				}
				break;
			}
		}
		else
		{
			pdest = *itl;
			rCode = pdest->checkConnection(false);
			attr.destName = pdest->getDestinationName();
			attr.status = rCode;
			attrs.push_back( attr );
		}
	}
	pthread_mutex_unlock(&destMx_);
	string errtext = AES_CDH_Result(rCode).errorText();
	AES_CDH_TRACE_MESSAGE("Return code for connection verification is %d. Error text: %s",rCode, errtext.c_str());
	return rCode;
}


/*===================================================================
   ROUTINE:checkConnectionManually
=================================================================== */
AES_CDH_ResultCode ServR::checkConnectionManually(string destName, bool force,
		vector<checkConnAttributes> &attrs)
{
	(void)force;
	std::list<TransDest*>::iterator itl;
	TransDest *pdest;
	AES_CDH_ResultCode rCode=AES_CDH_RC_ERROR;
	checkConnAttributes attr;

	pthread_mutex_lock(&destMx_);

	for ( itl = destList_.begin(); itl != destList_.end(); ++itl )
	{
		if (destName != "")
		{
			if ( ((*itl)->getDestinationName()) == destName )
			{
				pdest = *itl;
				rCode = pdest->checkConnectionManually();
				attr.destName = destName;
				attr.status = rCode;
				attrs.push_back( attr );
				break;
			}
		}
		else
		{
			pthread_mutex_unlock(&destMx_); //HL76154
			return AES_CDH_RC_INCUSAGE;
		}
	}

	pthread_mutex_unlock(&destMx_);
	return rCode;
}


/*===================================================================
   ROUTINE:upperToLower
=================================================================== */
void ServR::upperToLower(string& str)
{
	ACE_INT32 len = str.length();
	for (int i = 0; i <= len-1; i++)
		str[i] = ::tolower(str[i]);
}


/*===================================================================
   ROUTINE:cipher
=================================================================== */
void ServR::cipher(string& str)
{
	for (ACE_UINT32 i = 0; i < str.length(); i++)
		str[i] = str[i] + str.length()%11 + i%7;
}


/*===================================================================
   ROUTINE:decipher
=================================================================== */
void ServR::decipher(string& str)
{
	for (ACE_UINT32 i = 0; i < str.length(); i++)
		str[i] = str[i] - str.length()%11 - i%7;
}


/*===================================================================
   ROUTINE:createCDHDirs
=================================================================== */
bool ServR::createCDHDirs(void)
{
	string aesDataDirectory("");
	string cdhRootDirectory("");
	string newDirectory("");
	string cdhPath("");

	// Read parameters from PHA
	if ( !readParameters(aesDataDirectory, cdhRootDirectory) )
	{
		return false;
	}


	//------------------------------------
	// Assemble path to CDH root directory
	AES_CDH_LOG(LOG_LEVEL_INFO, "Assemble path to CDH root directory");
	newDirectory.assign(aesDataDirectory);
	newDirectory.append("/");
	newDirectory.append(cdhRootDirectory);

	if (ACE_OS::mkdir(newDirectory.c_str()) == -1  &&
			ACE_OS::last_error() != EEXIST)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(newDirectory,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
			
		}
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				NTErrorText(), "CDH directory could not be created: "
				+ newdr);
		return false;
	}


	//------------------------------------
	// Assemble path to CDH dest directory
	AES_CDH_LOG(LOG_LEVEL_INFO, "Assemble path to CDH dest directory");
	cdhPath.assign(newDirectory);
	cdhPath.append("/").append(destDirectory);

	if (ACE_OS::mkdir(cdhPath.c_str()) == -1 &&
			ACE_OS::last_error() != EEXIST)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(cdhPath,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
		}
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				NTErrorText(), "CDH dest directory could not be created: "
				+ newdr);
		return false;
	}

	//if (clearAcl(cdhPath) == false) return false;

	//if (changePermissions(cdhPath, "AESADMG", GENERIC_READ|GENERIC_WRITE,
	//"set", SUB_CONTAINERS_AND_OBJECTS_INHERIT) == false) return false;


	//------------------------------------
	// Assemble path to CDH dest set directory
	AES_CDH_LOG(LOG_LEVEL_INFO, "Assemble path to CDH dest set directory");
	cdhPath.assign(newDirectory);
	cdhPath.append("/").append(destSetDirectory);

	if (ACE_OS::mkdir(cdhPath.c_str()) == -1 &&
			ACE_OS::last_error() != EEXIST)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(cdhPath,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
		}
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				NTErrorText(), "CDH dest set directory could not be created: "
				+ newdr);
		return false;
	}

	//if (clearAcl(cdhPath) == false) return false;

	/*if (changePermissions(cdhPath, "AESADMG", GENERIC_READ|GENERIC_WRITE,
        "set", SUB_CONTAINERS_AND_OBJECTS_INHERIT) == false) return false;*/


	//------------------------------------
	// Assemble path to CDH database directory
	AES_CDH_LOG(LOG_LEVEL_INFO, "Assemble path to CDH database directory");
	cdhPath.assign(newDirectory);
	cdhPath.append("/").append(dataBaseDirectory);

	if (ACE_OS::mkdir(cdhPath.c_str()) == -1 &&
			ACE_OS::last_error() != EEXIST)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(cdhPath,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
		}
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				NTErrorText(), "CDH database directory could not be created: "
				+ newdr);
		return false;
	}

	//if (clearAcl(cdhPath) == false) return false;

	/*if (changePermissions(cdhPath, "AESADMG", GENERIC_READ|GENERIC_WRITE,
        "set", SUB_CONTAINERS_AND_OBJECTS_INHERIT) == false) return false;*/


	//------------------------------------
	// Assemble path to BIF directory
	AES_CDH_LOG(LOG_LEVEL_INFO, "Assemble path to BIF directory");
	cdhPath.assign(newDirectory);
	cdhPath.append("/").append(blockidDirectory);

	if (ACE_OS::mkdir(cdhPath.c_str()) == -1 &&
			ACE_OS::last_error() != EEXIST)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(cdhPath,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
		}
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				NTErrorText(), "CDH block-id directory could not be created: "
				+ newdr);
		return false;
	}

	//if (clearAcl(cdhPath) == false) return false;

	/*if (changePermissions(cdhPath, "AESADMG", GENERIC_READ|GENERIC_WRITE,
        "set", SUB_CONTAINERS_AND_OBJECTS_INHERIT) == false) return false;*/
	#if 0
	//------------------------------------
	// Assemble path to BFI directory
	cdhPath.assign(newDirectory);
	cdhPath.append("/").append(backupDestDirectory);

	if (ACE_OS::mkdir(cdhPath.c_str()) == -1 &&
			ACE_OS::last_error() != EEXIST)
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(cdhPath,AES_DATA_PATH,newdr)==false)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Directory name sent for event %s",newdr.c_str());
		}
		Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
				NTErrorText(), "CDH block-backup directory could not be created: "
				+ newdr);
		return false;
	}
	#endif
	//if (clearAcl(cdhPath) == false) return false;

	/*if (changePermissions(cdhPath, "AESADMG", GENERIC_READ|GENERIC_WRITE,
        "set", SUB_CONTAINERS_AND_OBJECTS_INHERIT) == false) return false;*/

	return true;
}


/*===================================================================
   ROUTINE:clearAcl
=================================================================== */
bool ServR::clearAcl(string fileName)
{
	(void)fileName;
	return true;
}


/*===================================================================
   ROUTINE:changePermissions
=================================================================== */
bool ServR::changePermissions(string fileName,
		ACE_TCHAR* trusteeName,
		ACE_UINT64 accessMask,
		ACE_TCHAR* accessMode,
		ACE_UINT64 inheritFlag)
{
	(void)fileName;
	(void)trusteeName;
	(void)accessMask;
	(void)accessMode;
	(void)inheritFlag;
	return true;
}


/*===================================================================
   ROUTINE:transactionBegin
=================================================================== */
AES_CDH_ResultCode ServR::transactionBegin(const string &destSetName)
{
	TransDestSet *destSet;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	rCode = getDestSet(destSetName, destSet);

	if (rCode == AES_CDH_RC_OK)
	{
		rCode = destSet->transactionBegin();
	}

	return rCode;
}


/*===================================================================
   ROUTINE:transactionEnd
=================================================================== */
AES_CDH_ResultCode ServR::transactionEnd(const string &destSetName,
		unsigned int &applBlockNr)
{
	TransDestSet *destSet;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	rCode = getDestSet(destSetName, destSet);

	if (rCode == AES_CDH_RC_OK)
	{
		rCode = destSet->transactionEnd(applBlockNr);
	}

	return rCode;
}


/*===================================================================
   ROUTINE:transactionCommit
=================================================================== */
AES_CDH_ResultCode ServR::transactionCommit(const string &destSetName,
		unsigned int &applBlockNr)
{
	TransDestSet *destSet;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	rCode = getDestSet(destSetName, destSet);

	if (rCode == AES_CDH_RC_OK)
	{
		rCode = destSet->transactionCommit(applBlockNr);
	}

	return rCode;

}


/*===================================================================
   ROUTINE:getLastCommittedBlock
=================================================================== */
AES_CDH_ResultCode ServR::getLastCommittedBlock(const string &destSetName,
		unsigned int &applBlockNr)
{
	TransDestSet *destSet;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	rCode = getDestSet(destSetName, destSet);

	if (rCode == AES_CDH_RC_OK)
	{
		rCode = destSet->getLastCommittedBlock(applBlockNr);
	}

	return rCode;
}


/*===================================================================
   ROUTINE:transactionTerminate
=================================================================== */
AES_CDH_ResultCode ServR::transactionTerminate(const string &destSetName)
{
	TransDestSet *destSet;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	rCode = getDestSet(destSetName, destSet);

	if (rCode == AES_CDH_RC_OK)
	{
		rCode = destSet->transactionTerminate();
	}

	return rCode;
}


/*===================================================================
   ROUTINE:setAPIClosed
=================================================================== */
AES_CDH_ResultCode ServR::setAPIClosed(const string &destSetName)
{
	TransDestSet *destSet;
	AES_CDH_ResultCode rCode = AES_CDH_RC_OK;

	AES_CDH_TRACE_MESSAGE("Entering with destSetName = %s",destSetName.c_str());

	rCode = getDestSet(destSetName, destSet);

	if (rCode == AES_CDH_RC_OK)
	{
		rCode = destSet->setAPIClosed();
	}
	AES_CDH_TRACE_MESSAGE("Leaving with rCode = %d",rCode);
	return rCode;
}

/*===================================================================
   ROUTINE:matchUserByGroup
=================================================================== */
#if 0
AES_CDH_ResultCode ServR::matchUserByGroup(const string &userName, const string &userGroup)
{
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;

	AES_GCC_User user(userName);
	if (user.init())
	{
		if (! user.isMember(userGroup))
		{
			rcode = AES_CDH_RC_CMDAUTHERR;
		}
	}
	else
	{
		rcode = AES_CDH_RC_INTPROGERR;
	}

	return rcode;
}
#endif

/*===================================================================
   ROUTINE:writeToFile
=================================================================== */
bool ServR::writeToFile(const std::string& file, const std::string& data)
{

	/***************************************************************************

	  HF86848
	  -------
	  This function was created to prevent a CHKDSK problem.
	  CHKDSK gave message "Adjusting Instance Tags to prevent rollover of file xxxxx"
	  pointing to the file accessed in this function.

	  This new handling will prevent the "Instance Tag" getting to large.
	  (Maximum value 61440 (0xF000)).

	 ***************************************************************************/

	// Open fileSentStatus file for write
	// If file does not exist, it will be created
	// If file exists, the contents will be discarded


	ACE_HANDLE filehandle = ACE_INVALID_HANDLE;
	std::string problemData = "";

	AES_CDH_TRACE_MESSAGE("writeToFile(), opening file  %s", file.c_str());
	filehandle = ACE_OS::open(file.c_str(),  O_CREAT |O_WRONLY| O_TRUNC | O_BINARY, S_IRWXU | S_IRWXO | S_IRWXG);

	if(filehandle == ACE_INVALID_HANDLE)
	{
		AES_CDH_TRACE_MESSAGE("writeToFile(), opening file  %s failed, errno = %d", file.c_str(), errno);
		problemData = "Could not open to file: ";
		problemData += file;
		problemData += ", ";
		problemData += NTErrorText();

		AES_CDH_TRACE_MESSAGE("writeToFile() %s", problemData.c_str());

		Event::report(	AES_CDH_physFileFault,
				"PHYSICAL FILE PROBLEM",
				problemData,
				"ServR::writeToFile() failed.");
		return false;

	}


	if( ACE_OS::write(filehandle, data.c_str(), data.size()) == -1)
	{

		problemData = "Could not write to file: ";
		problemData += file;
		problemData += ", ";
		problemData += ACE_OS::last_error();

		AES_CDH_TRACE_MESSAGE("writeToFile(), errno = %d %s", errno, problemData.c_str());

		if((ACE_OS::close(filehandle)) == -1)
			AES_CDH_LOG(LOG_LEVEL_INFO, "writeToFile(),close filehandle failed as errno = %d", errno);

		Event::report(	AES_CDH_physFileFault,
				"PHYSICAL FILE PROBLEM",
				problemData,
				"ServR::writeToFile() failed.");
		return false;
	}

	if((ACE_OS::close(filehandle)) == -1)
		AES_CDH_LOG(LOG_LEVEL_INFO, "writeToFile(),close filehandle %s failed as errno = %d",  data.c_str(),errno);

	return true;

}

/*===================================================================
   ROUTINE:resetFileInstanceTags
=================================================================== */
void ServR::resetFileInstanceTags(const std::string &file)
{
	/************************************************************************	

		HF86848
		-------
		This handling is designes to recover a CHKDSK problem.
		CHKDSK gave message "Adjusting Instance Tags to prevent rollover of file xxxxx"
		pointing to the file fileSentStatusFile_.

		This handling will reset the "Instance Tag".
		(Maximum value 61440 (0xF000)).

	 ************************************************************************/
	//ULARGE_INTEGER lnSize1, lnSize2, lnSize3;//HH57437
	std::string tempFile = file;
	struct statvfs diskFreeSpace;

	ACE_HANDLE filehandle;

	tempFile	+= ".tmp";


	//filehandle = CreateFile(file.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	filehandle = ACE_OS::open(file.c_str(), O_RDONLY);

	if(filehandle != ACE_INVALID_HANDLE)
	{
		AES_CDH_TRACE_MESSAGE("File %s is existing. Resets Instance Tag.", file.c_str());

		if((ACE_OS::close(filehandle)) == -1)
			AES_CDH_LOG(LOG_LEVEL_INFO, "Close filehandle %s failed as errno = %d",  file.c_str(),errno);

		string aesDataDir ;		
		//Get parameters from PHA
		if(AES_CDH_Paths::instance()->getCDHDataPath(aesDataDir))
		{
			if ( statvfs (aesDataDir.c_str(), &diskFreeSpace) == -1 )
			{
				ACE_UINT64 u64FreeSize = diskFreeSpace.f_bfree * diskFreeSpace.f_bsize;
				if(u64FreeSize > 10240)//HH57437 - check for 10Kb free space
				{
					AES_GCC_Util::copyFile(file, tempFile);
					ACE_OS::unlink(file.c_str());
					ACE_OS::rename(tempFile.c_str(), file.c_str());
				}
			}
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "getCDHDataPath has failed !!");
		}
	}
	else
	{
		//filehandle = CreateFile(tempFile.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		filehandle = ACE_OS::open(tempFile.c_str(), O_RDONLY);

		if(filehandle != ACE_INVALID_HANDLE)
		{

			AES_CDH_LOG(LOG_LEVEL_INFO, "Original file %s is missing.\n Creates original from temporary.", file.c_str());

			if((ACE_OS::close(filehandle)) == -1)
				AES_CDH_LOG(LOG_LEVEL_INFO, "tempFile,close filehandle %s failed as errno = %d",  tempFile.c_str(),errno);

			ACE_OS::rename(tempFile.c_str(), file.c_str());

		}
	}

}

/*===================================================================
   ROUTINE:dumpRecordTasks
=================================================================== */
void ServR::dumpRecordTasks()
{

	AES_CDH_TRACE_MESSAGE("Entering");

	std::list<sendRecordFileTask>::iterator it;


	recordtaskMX_.acquire();

	AES_CDH_TRACE_MESSAGE("recordtaskMX_.acquired");
	for ( it = sendRecordFileTaskList_.begin(); 
			it != sendRecordFileTaskList_.end(); ++it )
	{
		AES_CDH_TRACE_MESSAGE("pid = %d destSetObj = %d destsetname = %s mainFileName = %s stopflag = %d ticks = %lu",
				(*it).pid,(*it).destSetObj,(*it).destSetName.c_str(),(*it).mainFileName.c_str(),(*it).stopFlag,(*it).ticks);
	}
	recordtaskMX_.release();

	AES_CDH_TRACE_MESSAGE("recordtaskMX_.release() ended...");
}

/*===================================================================
   ROUTINE:dumpFileTasks
=================================================================== */
void ServR::dumpFileTasks()
{

	AES_CDH_TRACE_MESSAGE("Entering");

	std::list<sendFileTask>::iterator it;


	taskMX_.acquire();

	for ( it = sendFileTaskList_.begin();
			it != sendFileTaskList_.end(); ++it )
	{
		AES_CDH_TRACE_MESSAGE("pid = %d destSetObj = %d destsetname = %s filename = %s remotesubdirName = %s newfilename = %s stopflag = %d ticks = %lu",
				(*it).pid,(*it).destSetObj,(*it).destSetName.c_str(),(*it).fileName.c_str(),(*it).remoteSubDirName.c_str(),
				(*it).newFileName.c_str(),(*it).stopFlag,(*it).ticks);
	}
	taskMX_.release();
	AES_CDH_TRACE_MESSAGE("Leaving");

}


/*===================================================================
   ROUTINE:StartOIThreads
=================================================================== */
ACS_CC_ReturnType ServR::StartOIThreads()
{
	AES_CDH_TRACE_MESSAGE("Entering");
#ifdef DEBUGTEST
	cout<<" Entering StartOIThreads"<<endl;
#endif
	if( ServR::isStopEventSignalled == true )
	{
		AES_CDH_TRACE_MESSAGE("Stop Event is signalled.");
		AES_CDH_TRACE_MESSAGE("Leaving");
		return ACS_CC_SUCCESS;
	}

	//Create the Primary  Block Destination OI Thread
	AES_CDH_LOG(LOG_LEVEL_INFO, "Create OI Thread for class %s",AES_CDH_PRIMARY_BLOCK_DEST_IMM_CLASS_NM);
	m_poPrimBlockDestCmdHandler = new AES_CDH_BlockDestCmdHandler
				(AES_CDH_PRIMARY_BLOCK_DEST_IMM_CLASS_NM,
				AES_CDH_PRIMARY_BLOCK_DEST_IMPLEMENTER,	
				ACS_APGCC_ONE);

	if(m_poPrimBlockDestCmdHandler == 0)
	{
              AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while allocating memory for Block Primary Destination Handler");
              return ACS_CC_FAILURE;
        }

	if( m_poPrimBlockDestCmdHandler->setupDestOIThread(m_poPrimBlockDestCmdHandler, threadManager_) ==
                        ACS_CC_FAILURE )
        {
                AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while starting the OI Thread for handling Block Primary Destination");
                return ACS_CC_FAILURE;
        }
	

	//Create the Secondary Block Destination OI Thread
	AES_CDH_LOG(LOG_LEVEL_INFO, "Create OI Thread for class %s",AES_CDH_SECONDARY_BLOCK_DEST_IMM_CLASS_NM);
	m_poSecBlockDestCmdHandler = new AES_CDH_BlockDestCmdHandler
				(AES_CDH_SECONDARY_BLOCK_DEST_IMM_CLASS_NM,
                                 AES_CDH_SECONDARY_BLOCK_DEST_IMPLEMENTER,
                                 ACS_APGCC_ONE);

	if ( m_poSecBlockDestCmdHandler == 0 )
        {
                AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while allocating memory for Secondaryy Block Destination Handler");
                return ACS_CC_FAILURE;
        }

	 if ( m_poSecBlockDestCmdHandler->setupDestOIThread( m_poSecBlockDestCmdHandler, threadManager_) ==
                        ACS_CC_FAILURE )
        {
                AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while starting the OI Thread for handling Secondary Block Destinations");
                return ACS_CC_FAILURE;
        }

	
	 //Create the AdvancedRespParameters OI Thread.
	 AES_CDH_LOG(LOG_LEVEL_INFO, "Create OI Thread for class %s",AES_CDH_BLOCK_ADV_PARAMS_IMM_CLASS_NM);
	m_poAdvBlockDestCmdHandler = new AES_CDH_BlockDestCmdHandler
				(AES_CDH_BLOCK_ADV_PARAMS_IMM_CLASS_NM ,
				AES_CDH_BLOCK_ADV_PARAMS_IMPLEMENTER,
				ACS_APGCC_ONE);

	if ( m_poAdvBlockDestCmdHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while allocating memory for Advanced Block Destination Handler");
		return ACS_CC_FAILURE;
	}

	if ( m_poAdvBlockDestCmdHandler->setupDestOIThread( m_poAdvBlockDestCmdHandler,threadManager_) == 
			ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while starting the OI Thread for handling Advanced Block Destinations");
		return ACS_CC_FAILURE;
	}

	//Create the Primary Resp Destination OI Thread.
	AES_CDH_LOG(LOG_LEVEL_INFO, "Create OI Thread for class %s",AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM);
	m_poPrimRespDestCmdHandler = new AES_CDH_FileRespDestCmdHandler( AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM,
			AES_CDH_FILE_RESP_DEST_IMM_PRIM_IMPL,
			ACS_APGCC_ONE);

	if ( m_poPrimRespDestCmdHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while allocating memory for Primary Responding Destination Handler");
		return ACS_CC_FAILURE;
	}

	if ( m_poPrimRespDestCmdHandler->setupDestOIThread( m_poPrimRespDestCmdHandler, threadManager_) == 
			ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while starting the OI Thread for handling Primary Responding Destinations");
		return ACS_CC_FAILURE;
	}

	//Create the Secondary Resp Destination OI Thread.
	AES_CDH_LOG(LOG_LEVEL_INFO, "Create OI Thread for class %s",AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM);
	m_poSecRespDestCmdHandler = new AES_CDH_FileRespDestCmdHandler( AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM,
			AES_CDH_FILE_RESP_DEST_IMM_SEC_IMPL,
			ACS_APGCC_ONE);

	if ( m_poSecRespDestCmdHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while allocating memory for Secondaryy Responding Destination Handler");
		return ACS_CC_FAILURE;
	}

	if ( m_poSecRespDestCmdHandler->setupDestOIThread( m_poSecRespDestCmdHandler, threadManager_) == 
			ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL,"Error occured while starting the OI Thread for handling Secondary Responding Destinations");
		return ACS_CC_FAILURE;
	}

	//Create the AdvancedRespParameters OI Thread.
	AES_CDH_LOG(LOG_LEVEL_INFO,"Create OI Thread for class %s",AES_CDH_ADV_RESP_PARAMS_CLASS_NM);
	m_poAdvRespDestCmdHandler = new AES_CDH_FileRespDestCmdHandler( AES_CDH_ADV_RESP_PARAMS_CLASS_NM,
			AES_CDH_ADV_RESP_DEST_IMM_IMPL,
			ACS_APGCC_ONE);

	if ( m_poAdvRespDestCmdHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while allocating memory for Advanced Responding Destination Handler");
		return ACS_CC_FAILURE;
	}

	if ( m_poAdvRespDestCmdHandler->setupDestOIThread( m_poAdvRespDestCmdHandler,threadManager_) == 
			ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while starting the OI Thread for handling Advanced Responding Destinations");
		return ACS_CC_FAILURE;
	}
	AES_CDH_LOG(LOG_LEVEL_INFO,"Create OI Thread for class %s",AES_CDH_PRIMARYINITDEST_IMM_CLASS_NM);
	m_poFilePrimaryInitDestCmdHandler = new AES_CDH_FileInitDestCmdHandler(
			AES_CDH_PRIMARYINITDEST_IMM_CLASS_NM,
			AES_CDH_PRIMARYINITDEST_IMPLEMENTER,
			ACS_APGCC_ONE);

	if ( m_poFilePrimaryInitDestCmdHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while allocating memory for AES_CDH_FileInitDestCmdHandler");

		return ACS_CC_FAILURE;
	}

	if ( m_poFilePrimaryInitDestCmdHandler->setupDestOIThread( m_poFilePrimaryInitDestCmdHandler, threadManager_) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while starting the OI Thread for handling File Primary Initiating destination");
		return ACS_CC_FAILURE;
	}

	AES_CDH_LOG(LOG_LEVEL_INFO,"Create OI Thread for class %s",AES_CDH_SECONDARYINITDEST_IMM_CLASS_NM);
	m_poFileSecondaryInitDestCmdHandler = new AES_CDH_FileInitDestCmdHandler(
			AES_CDH_SECONDARYINITDEST_IMM_CLASS_NM,
			AES_CDH_SECONDARYINITDEST_IMPLEMENTER,
			ACS_APGCC_ONE);

	if ( m_poFileSecondaryInitDestCmdHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while allocating memory for AES_CDH_FileInitDestCmdHandler");

		return ACS_CC_FAILURE;
	}

	if ( m_poFileSecondaryInitDestCmdHandler->setupDestOIThread( m_poFileSecondaryInitDestCmdHandler, threadManager_) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while starting the OI Thread for handling File Secondary Initiating destination");
		return ACS_CC_FAILURE;
	}

	AES_CDH_LOG(LOG_LEVEL_INFO,"Create OI Thread for class %s",AES_CDH_ADVINITPARAMS_IMM_CLASS_NM);
	m_poFileAdvancedInitiatingCmdHandler = new AES_CDH_FileInitDestCmdHandler(
			AES_CDH_ADVINITPARAMS_IMM_CLASS_NM,
			AES_CDH_ADVINITPARAMS_IMPLEMENTER,
			ACS_APGCC_ONE);

	if (  m_poFileAdvancedInitiatingCmdHandler == 0 )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while allocating memory for AES_CDH_FileInitDestCmdHandler");

		return ACS_CC_FAILURE;
	}

	if ( m_poFileAdvancedInitiatingCmdHandler->setupDestOIThread( m_poFileAdvancedInitiatingCmdHandler, threadManager_) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while starting the OI Thread for handling File Adavanced Initiating params");
		return ACS_CC_FAILURE;
	}

	if( ServR::isStopEventSignalled == true )
	{
		AES_CDH_TRACE_MESSAGE("Stop Event is signalled.");
		AES_CDH_TRACE_MESSAGE("Leaving");
		return ACS_CC_SUCCESS;
	}

	m_poRespDestSetHandler = new AES_CDH_FileRespDestSetCmdHandler(AES_CDH_RESPDESTSET_CLASSNAME,AES_CDH_RESPDESTSET_IMPLEMENTER, threadManager_);

	m_poInitDestSetHandler = new AES_CDH_FileInitDestSetCmdHandler(AES_CDH_INITDESTSET_CLASSNAME,AES_CDH_INITDESTSET_IMPLEMENTER,threadManager_);

	m_poBlockDestSetHandler = new AES_CDH_BlockDestSetCmdHandler( AES_CDH_BLOCK_DESTSET_CLASSNAME, AES_CDH_BLOCK_DESTSET_IMPLEMENTER, threadManager_ );

	if( m_poBlockDestSetHandler )
	{
		if( ACS_CC_FAILURE == (m_poBlockDestSetHandler->setObjImpl()))
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while adding class impl for Block Destination set");
			return ACS_CC_FAILURE;
		}
		m_poBlockDestSetHandler->activate();
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while starting the OI Thread for handling Block Destination set");
		return ACS_CC_FAILURE;
	}


	if(m_poRespDestSetHandler)
	{
		if( ACS_CC_FAILURE == (m_poRespDestSetHandler->setObjImpl()))
		{
			AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while adding class impl for File Responding Destination set");
			return ACS_CC_FAILURE;
		}
		m_poRespDestSetHandler->activate();
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while starting the OI Thread for handling File Responding Destination set");
		return ACS_CC_FAILURE;
	}

	if(m_poInitDestSetHandler)
	{
		if( ACS_CC_FAILURE == (m_poInitDestSetHandler->setObjImpl()))
		{
			AES_CDH_LOG(LOG_LEVEL_FATAL,"Error occured while adding class impl for File Initiating Destination set");
			return ACS_CC_FAILURE;
		}
		m_poInitDestSetHandler->activate();
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Error occured while starting the OI Thread for handling File Initiating Destination set");
		return ACS_CC_FAILURE;
	}


	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;

}

/*===================================================================
   ROUTINE:StopOIThreads
=================================================================== */
ACS_CC_ReturnType ServR::StopOIThreads()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_LOG(LOG_LEVEL_INFO, "On shutdown, Stopping all OI threads");

	if( m_poPrimBlockDestCmdHandler != 0 )
	{
		m_poPrimBlockDestCmdHandler->shutdown();
	}
	if( m_poSecBlockDestCmdHandler!= 0 )
	{
		m_poSecBlockDestCmdHandler->shutdown();
	}
	if( m_poAdvBlockDestCmdHandler != 0 )
	{
		m_poAdvBlockDestCmdHandler->shutdown();
	}

	if( m_poPrimRespDestCmdHandler != 0 )
	{
		m_poPrimRespDestCmdHandler->shutdown();
	}
	if( m_poSecRespDestCmdHandler != 0 )
	{
		m_poSecRespDestCmdHandler->shutdown();
	}
	if( m_poAdvRespDestCmdHandler != 0 )
	{
		m_poAdvRespDestCmdHandler->shutdown();
	}

	if( m_poFilePrimaryInitDestCmdHandler != 0 )
	{
		m_poFilePrimaryInitDestCmdHandler->shutdown();
	}
	if( m_poFileSecondaryInitDestCmdHandler != 0 )
	{
		m_poFileSecondaryInitDestCmdHandler->shutdown();
	}
	if( m_poFileAdvancedInitiatingCmdHandler!= 0 )
	{
		m_poFileAdvancedInitiatingCmdHandler->shutdown();
	}
	if( m_poRespDestSetHandler != 0)
	{
		m_poRespDestSetHandler->shutdown();
	}
	if(m_poInitDestSetHandler != 0)
	{
		m_poInitDestSetHandler->shutdown();
	}

	if( m_poBlockDestSetHandler != 0 )
	{
		m_poBlockDestSetHandler->shutdown();
	}

	threadManager_->wait();

	if( m_poPrimBlockDestCmdHandler != 0 )
	{
		delete m_poPrimBlockDestCmdHandler;
		m_poPrimBlockDestCmdHandler = 0;
	}
	if( m_poSecBlockDestCmdHandler!= 0 )
	{
		delete m_poSecBlockDestCmdHandler;
		m_poSecBlockDestCmdHandler = 0;
	}
	if( m_poAdvBlockDestCmdHandler != 0 )
	{
		delete m_poAdvBlockDestCmdHandler;
		m_poAdvBlockDestCmdHandler = 0;
	}

	if( m_poBlockDestSetHandler != 0 )
	{
		delete m_poBlockDestSetHandler;
		m_poBlockDestSetHandler=0;
	}
	if ( m_poPrimRespDestCmdHandler != 0 )
	{
		delete m_poPrimRespDestCmdHandler;
		m_poPrimRespDestCmdHandler = 0;
	}

	if ( m_poSecRespDestCmdHandler != 0 )
	{
		delete m_poSecRespDestCmdHandler;
		m_poSecRespDestCmdHandler = 0;
	}

	if ( m_poAdvRespDestCmdHandler != 0 )
	{
		delete m_poAdvRespDestCmdHandler;
		m_poAdvRespDestCmdHandler = 0;
	}

	if ( m_poFilePrimaryInitDestCmdHandler != 0 )
	{
		delete m_poFilePrimaryInitDestCmdHandler;
		m_poFilePrimaryInitDestCmdHandler = 0;
	}
	if ( m_poFileSecondaryInitDestCmdHandler != 0 )
	{
		delete m_poFileSecondaryInitDestCmdHandler;
		m_poFileSecondaryInitDestCmdHandler = 0;
	}
	if ( m_poFileAdvancedInitiatingCmdHandler != 0 )
	{
		delete m_poFileAdvancedInitiatingCmdHandler;
		m_poFileAdvancedInitiatingCmdHandler = 0;
	}

	if( m_poRespDestSetHandler != 0)
	{
		delete m_poRespDestSetHandler;
		m_poRespDestSetHandler  = 0;
	}

	if( m_poInitDestSetHandler != 0)
	{
		delete m_poInitDestSetHandler;
		m_poInitDestSetHandler  = 0;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE:getDataTransferFolderPath
=================================================================== */
bool ServR::getDataTransferFolderPath( string &aDataTransferFolderPath )
{
	AES_CDH_TRACE_MESSAGE("Entering");

	ACS_APGCC_DNFPath_ReturnTypeT returnType = ACS_APGCC_DNFPATH_FAILURE;

	string fileMFuncName = AES_CDH_DATA_TRANSFER_NBI_FOLDER_ATTR_NM;

	char folderPath[1024] = { 0 };

	int folderPathLen = 1024;

	ACS_APGCC_CommonLib objCommonLib;

	returnType = objCommonLib.GetFileMPath( fileMFuncName.c_str(),
			folderPath,
			folderPathLen );

	if( returnType != ACS_APGCC_DNFPATH_SUCCESS )
	{
		AES_CDH_TRACE_MESSAGE("Leaving");
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while getting the FileMPath.");
		return false;
	}

	AES_CDH_TRACE_MESSAGE( "Leaving with %s", folderPath);

	aDataTransferFolderPath = folderPath;

	return true;
}
/*===================================================================
   ROUTINE:getLastDataTransferFolderPath
=================================================================== */
bool ServR::getLastDataTransferFolderPath( string &lastFolder )
{
	string dataTransferFolderPath("");
	if(getDataTransferFolderPath(dataTransferFolderPath))
	{
		size_t p = dataTransferFolderPath.find_last_of("/");
		if (p == string::npos)
		{
			return false;
		}
		if (p+1 == dataTransferFolderPath.size()) //last char is '/'
		{
			dataTransferFolderPath.resize(dataTransferFolderPath.size() - 1);
			p = dataTransferFolderPath.find_last_of("/");
			if (p == string::npos)
			{
				return false;
			}
		}
		lastFolder.assign(dataTransferFolderPath.substr(p+1));
		return true;
	}
	return false;
}




/*===================================================================
   ROUTINE:getActiveDestForDestSet
=================================================================== */
bool ServR::getActiveDestForDestSet( const string& destSetName,
		string& activeDestName )
{
	AES_CDH_TRACE_MESSAGE("Entering with destset: %s", destSetName.c_str());

	activeDestName = "";

	TransDestSet *ptrTransDestSet = 0;

	if( ServR::getDestSet( destSetName, ptrTransDestSet) != AES_CDH_RC_OK )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error in getting TransDestSet object for a destination set.");

		AES_CDH_TRACE_MESSAGE("Leaving with return value false");
		return false;
	}

	//Now retrieve the active destination name from TransdestSet object.

	AES_CDH_DestinationSet::destSetAttributes destSetAttrs;

	if( ptrTransDestSet->getAttr( destSetAttrs ) != AES_CDH_RC_OK )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error in getting destSetAttributes from TransDestSet object");
		AES_CDH_TRACE_MESSAGE("Leaving with return value false");
		return false;
	}

	//Now copy the value of active attr of destSetAttrs to active dest.

	activeDestName = destSetAttrs.active;

	AES_CDH_TRACE_MESSAGE( "Leaving with activeDestName as %s", activeDestName.c_str());
	return true;
}

/*===================================================================
   ROUTINE:isDestSetAttached
=================================================================== */
bool ServR::isDestSetAttached(const std::string &destSetName)
{
	AES_CDH_TRACE_MESSAGE("Entering for DestSet:: %s", destSetName.c_str());
	DIR *dip = 0;
	struct dirent   *dit;
	string tempDestSetName("");
	string path("");
	TransDestSet *pdestset;
        AES_CDH_ResultCode rCode = AES_CDH_RC_OK;
	ACE_INT32 realAttr = 0;

        rCode = getDestSet(destSetName, pdestset);

        if (rCode == AES_CDH_RC_OK)
        {
                rCode = pdestset->checkDestinationSetType((AES_CDH_DestinationSet::TQConnectAttributes &)realAttr); // FILE or BLOCK
        }

	if(realAttr == AES_CDH_DestinationSet::FILE)
	{
	if (AES_CDH_Paths::instance()->getCDHDataPath( path ) )
	{
		path.append("/");
		path.append(AES_AFP_RootDirectory);
		AES_CDH_TRACE_MESSAGE("path for opendir %s",path.c_str());
	}
  
	dip = opendir(path.c_str());
	if( dip == NULL )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ServR::isDestSetAttached, opendir failed for path, errno = %d", errno);
		AES_CDH_TRACE_MESSAGE("Leaving, DestSet:: %s is not attached", destSetName.c_str());
		return false;
	}

	while((dit = readdir(dip)) != NULL )
	{
			size_t pos1;
		tempDestSetName.clear();
		pos1 =  string::npos;
		tempDestSetName = string(dit->d_name);
		if ( tempDestSetName == "." || tempDestSetName == "..")
		{
			continue;
		}
		pos1 = tempDestSetName.find("_");
		if ( pos1 != string::npos)
		{
			if ( tempDestSetName.substr(pos1+2, 1) == "-")
			{
				continue;
			}
			tempDestSetName = tempDestSetName.substr(pos1+1);
			size_t pos2 = tempDestSetName.find("_");
			if ( pos2 != string::npos)
			{
				tempDestSetName = tempDestSetName.substr(0, pos2);
			}

			if ( destSetName == tempDestSetName)
			{
				if(dip != NULL)
					closedir(dip);

				AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving ServR::isDestSetAttached. DestSet:: %s is attached", destSetName.c_str());
				return true;
			}
		}
	}
	if(dip != NULL)
		closedir(dip);
	}
        else if(realAttr == AES_CDH_DestinationSet::BLOCK)
	{
	       unsigned int res = AES_NOERRORCODE;
	   //   AES_DBO_TQManager *tqMan =new  AES_DBO_TQManager;
               
	      // if(tqMan != NULL)
                 if(AES_DBO_TQManager::instance())
	       {
		      // res = tqMan->isDestSetAttached(destSetName);
                       res=AES_DBO_TQManager::instance()->isDestSetAttached(destSetName);
                       AES_CDH_TRACE_MESSAGE("AES_DBO_TQManager::instance() res = %d", res);
		       //tqMan->close();
		       //delete tqMan;
		       //tqMan =0;
		       if(res == AES_TQISPROTECTED || res == AES_NOSERVERACCESS  )
		       {
			       return true;
		       }
			    
	       }

	}


	AES_CDH_TRACE_MESSAGE("Leaving , DestSet:: %s is not attached", destSetName.c_str());
	return false;
}


/*===================================================================
        ROUTINE: isFileMServiceRunning
=================================================================== */
bool ServR::isFileMServiceRunning()
{
	FILE *fp = NULL;
	std::string cmd = "ps -ea | grep ";
	cmd += AES_CDH_FILEM_SERVICE_NM;
	cmd += " 1>/dev/null 2>/dev/null";
	int ret = -1;
	int status = -1;

	fp = popen(cmd.c_str(),"r");

	if (fp == NULL)
	{
		return false;
	}

	status = pclose(fp);

	if (WIFEXITED(status))
	{
		ret=WEXITSTATUS(status);
	}

	if (ret == 0)
	{
		return true;
	}
	else
	{
		return false;
	}

}


/*===================================================================
        ROUTINE: checkFileTransferObjInIMM
=================================================================== */

bool ServR::checkFileTransferObjInIMM()
{
	OmHandler omhandler;
	ACE_Time_Value tv(0, 10000);
	AES_CDH_TRACE_MESSAGE("Trying to initialize OmHandler");
	for (  ; ( omhandler.Init() == ACS_CC_FAILURE)  ; )
	{
		ACE_INT32 ret = StopEvent->wait(&tv, 0);
		if (ret < 0)
		{
			int errnum = ACE_OS::last_error();
			if( errnum == 62 )
			{
				continue;
			}
			return false;
		}
		else if(ret == 0)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Stop Event caught. Service stopped");
			AES_CDH_TRACE_MESSAGE("Leaving");
			return false;
		}

	}
	AES_CDH_TRACE_MESSAGE("OmHandler initialized successfully. Trying to fetch the file tranfer object from IMM");
	for ( ; (AES_GCC_Util::fetchDnOfFileTransferObjFromIMM(omhandler)) == -1 ; )
	{
		ACE_INT32 ret = StopEvent->wait(&tv, 0);
		if (ret < 0)
		{
			int errnum = ACE_OS::last_error();
			if( errnum == 62 )
			{
				continue;
			}
			omhandler.Finalize();
			return false;
		}
		else if(ret == 0)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Stop Event caught. Service stopped");
			AES_CDH_TRACE_MESSAGE("Leaving");
			omhandler.Finalize();
			return false;
		}
	}
	omhandler.Finalize();
	AES_CDH_TRACE_MESSAGE("Fetched the file tranfer object from IMM");
	return true;
}

/*===================================================================
   ROUTINE:switchActiveDest
=================================================================== */
AES_CDH_ResultCode ServR::switchActiveDest(const string& destSetName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode rcode ;
	string destName("");
	AES_CDH_DestinationSet::destSetAttributes attr;
	TransDestSet *pdestset;

	// Get the destination set object 
	rcode = getDestSet(destSetName, pdestset);
	if ( rcode == AES_CDH_RC_OK )
	{
		// Get the attributes of the destination set object
		rcode = pdestset->getAttr(attr);

		if ( rcode == AES_CDH_RC_OK )
		{
			if ( attr.secDest == "") // If secondary destination is empty, then only primary is existing
			{
				destName = attr.primDest;
			}
			else // If secondary also exists
			{
				if ( attr.primDest == attr.active ) // If primary is active, switch to secondary
				{
					destName = attr.secDest;
				}
				else // If secondary is active, switch to primary
				{
					destName = attr.primDest;
				} 
			}
		}

		rcode = pdestset->switchDestination(destName);
	}
	AES_CDH_LOG(LOG_LEVEL_DEBUG, "Leaving Switch to destination %s",destName.c_str());
	AES_CDH_TRACE_MESSAGE( "Leaving");
	return rcode;
}
/*===================================================================
   ROUTINE:getFileTransferStopEvent
=================================================================== */
bool ServR::getFileTransferStopEvent(const string destSetName,
		const string fileName,
		const string remoteSubDirName,
		const string newFileName,
		ACE_Event **fileTransferStopEvent)
{
	std::list<sendFileTask>::iterator it;

	taskMX_.acquire();

	for ( it = sendFileTaskList_.begin(); it != sendFileTaskList_.end(); ++it )
	{
		if ( (*it).destSetName == destSetName &&
				(*it).fileName == fileName &&
				(*it).remoteSubDirName == remoteSubDirName &&
				(*it).newFileName == newFileName )
		{
			taskMX_.release();
			*fileTransferStopEvent = (*it).pStopFileTransferEvent;
			return true;
		}
	}
	taskMX_.release();
	return false;
}

/*===================================================================
   ROUTINE:checkCmdHandler
=================================================================== */
bool ServR::checkCmdHandler(CmdHandler* cmdHdlr)
{
	//This method assumes that the CmdHandler mutex is already acquired.
	//It is currently called from DestinationAlarm::raise and DestinationAlarm::cease.
	
	AES_CDH_TRACE_MESSAGE("Entering");
    bool result = false;
    std::list<CmdHandler*>::iterator it;

    cmdHandlerListMX_.acquire();
    for ( it = cmdHandlerList_.begin();it != cmdHandlerList_.end(); ++it )
    {
        if ( (*it) == cmdHdlr )
        {
        	AES_CDH_TRACE_MESSAGE("CmdHandler is present in the list.");
			result = true;
			break;
		}
    }
    cmdHandlerListMX_.release();
    AES_CDH_TRACE_MESSAGE("Leaving");
    return result;
}

bool  ServR::getObjectOfRef(const string& i_objType,
                                const string& i_objName,
                                string& o_objRef)
{
	AES_CDH_TRACE_MESSAGE("Entering");
        o_objRef = "";
        if( i_objType == "DESTINATION" )
        {
                TransDest *pdest = 0;
                std::list<TransDest*>::iterator itl;

                for ( itl = ServR::destList_.begin(); itl != ServR::destList_.end(); ++itl )
                {
                        if ( ((*itl)->getDestinationName()) == i_objName )
                        {
                                pdest = (*itl);
                        }
                }
                if( pdest != 0 )
                {
                        pdest->getDestinationRdn(o_objRef);
                }
                else
                { 
                       return false ;
                }
        }
        else if ( i_objType == "DESTINATIONSET" )
        {
                //Find the type of the destination set.

                TransDestSet *pdestSet = 0;
                std::list<TransDestSet*>::iterator itl;

                for ( itl = ServR::destSetList_.begin(); itl != ServR::destSetList_.end(); ++itl )
                {
                        if ( ((*itl)->getDestinationSetName()) == i_objName )
                        {
                                pdestSet = (*itl);
                        }
                }

                if( pdestSet  != 0 )
                {
                        pdestSet->getDestSetRdn(o_objRef);
                }
				else
				{
					return false;
				}
        }
        if( !o_objRef.empty())
        {
                o_objRef += "=";
        }
        AES_CDH_TRACE_MESSAGE("Leaving");
       return true;
}

AES_CDH_ResultCode ServR::isDestSetLimitCrossed(const std::string &destSetRdn)
{
	AES_CDH_ResultCode rcode = AES_CDH_RC_OK;
	int numOfFileDestset = 0;
	int numOfBlockDestset = 0;
	std::list<TransDestSet*>::iterator it;
	destSetMX_.acquire();
	if(!destSetList_.empty())
	{
		for(it= destSetList_.begin();it != destSetList_.end();++it)
		{
			AES_CDH_DestinationSet::TQConnectAttributes destsetType;
			(*it)->checkDestinationSetType(destsetType);
			if(destsetType == AES_CDH_DestinationSet::FILE) 
			{	
				numOfFileDestset++;
			}
			else if(destsetType == AES_CDH_DestinationSet::BLOCK)
			{
				numOfBlockDestset++;
			}

		}
	}
        if (destSetRdn == AES_CDH_BLOCK_DEST_SET_ID)  
	{
		if(numOfBlockDestset >= BLOCK_DESTSET_MAX_LIMIT)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Number of block destination sets crossed the limit of %d destination sets in GOH ",BLOCK_DESTSET_MAX_LIMIT);
			rcode=AES_CDH_BLOCKDESTSETLIMIT;
		}
	} 
	else 
	{
		if (numOfFileDestset >= FILE_DESTSET_MAX_LIMIT)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Number of file destination sets crossed the limit of %d destination sets in GOH ",FILE_DESTSET_MAX_LIMIT);
			rcode=AES_CDH_FILEDESTSETLIMIT;
		}
	}
	destSetMX_.release();
	return rcode;
}

bool ServR::checkBlockTransferObjInIMM()
{
	OmHandler omhandler;
	ACE_Time_Value tv(0, 10000);
	AES_CDH_TRACE_MESSAGE("Trying to initialize OmHandler");
	for (  ; ( omhandler.Init() == ACS_CC_FAILURE)  ; )
	{
		ACE_INT32 ret = StopEvent->wait(&tv, 0);
		if (ret < 0)
		{
			int errnum = ACE_OS::last_error();
			if( errnum == 62 )
			{
				continue;
			}
			return false;
		}
		else if(ret == 0)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Stop Event caught. Service stopped");
			AES_CDH_TRACE_MESSAGE("Leaving");
			return false;
		}

	}

	AES_CDH_TRACE_MESSAGE("OmHandler initialized successfully. Trying to fetch the block tranfer object from IMM");
	for ( ; (AES_GCC_Util::fetchDnOfBlockTransferObjFromIMM(omhandler)) == -1 ; )
	{
		ACE_INT32 ret = StopEvent->wait(&tv, 0);
		if (ret < 0)
		{
			int errnum = ACE_OS::last_error();
			if( errnum == 62 )
			{
				continue;
			}
			omhandler.Finalize();
			return false;
		}
		else if(ret == 0)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Stop Event caught. Service stopped");
			AES_CDH_TRACE_MESSAGE("Leaving");
			omhandler.Finalize();
			return false;
		}
	}
	omhandler.Finalize();
	AES_CDH_TRACE_MESSAGE("Fetched the block tranfer object from IMM");
	return true;
}

bool ServR::isSecondaryDestExists(const string& destSetName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_ResultCode rcode ;
	string destName("");
	AES_CDH_DestinationSet::destSetAttributes attr;
	TransDestSet *pdestset;

	// Get the destination set object
	rcode = getDestSet(destSetName, pdestset);
	if ( rcode == AES_CDH_RC_OK )
	{
		rcode = pdestset->getAttr(attr);
		if ( rcode == AES_CDH_RC_OK )
		{
			if ( attr.secDest == "")
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

int ServR::StartService()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	while(!ServR::isStopEventSignalled && !m_DSDInitiate)
	{
		if( initDSDServer() && registerDSDHandles() )
		{
			// set the flag to exit by the while loop;
			m_DSDInitiate = true;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("Error on init DSD server retry after 1 sec");
			stopDSDServer();
			// wait 1 second between each retry
			ACE_OS::sleep(1);
		}
	}

	if( m_reactor && !ServR::isStopEventSignalled )
	{
		//m_reactor->owner(ACE_Thread::self());
		AES_CDH_TRACE_MESSAGE("Service opened and started...waiting on run_reactor_event_loop");
		int result = m_reactor->run_reactor_event_loop();
		if (result < 0)
			AES_CDH_TRACE_MESSAGE(" ERROR : run_event_loop failed! result = %d", result);
		else
		{
			AES_CDH_TRACE_MESSAGE("Signaled end_reactor_loop - result = %d", result);
		}
	}
	return 0;
}

int ServR::StopService()
{
	AES_CDH_TRACE_MESSAGE("Closing cdh functionality ");
	AES_CDH_LOG(LOG_LEVEL_INFO,"Closing cdh functionality ");
	removeDSDHandles();

	m_reactor->end_reactor_event_loop();
	usleep(500 * 1000);

	int count = m_reactor->purge_pending_notifications(0); // Purge any waiting notifications
	if (count > 0)
	{
		AES_CDH_TRACE_MESSAGE("Exiting with waiting notifications!!!");
	}

	return 0;
}

bool ServR::initDSDServer()
{
	AES_CDH_TRACE_MESSAGE("Entering in initDSDServer()");

	// to avoid some erroneous publishing
	if(!m_serverOnLine)
	{
		AES_CDH_TRACE_MESSAGE("initDSDServer(), publish by DSD");

		int result = m_CmdServerAcceptor.open("AES_CDH_server", "AES");

		if(DSDConfig::DSDSuccess == result)
		{
			AES_CDH_TRACE_MESSAGE("initDSDServer(), DSD server published, applName=AES_CDH_server, domain=AES");
			AES_CDH_LOG(LOG_LEVEL_INFO, "Published in DSD successfully");
			// set online flag
			m_serverOnLine = true;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("initDSDServer(), DSD server published FAILED");
		}

	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return m_serverOnLine;
}

bool ServR::registerDSDHandles()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	bool result = true;
	int idx = 0;
	ACE_HANDLE dsdHnl;

	fdList.clear();

	// get the first DSD handle
	dsdHnl =  m_CmdServerAcceptor.get_handle(idx);

	// Check if a valid handle
	while( ACE_INVALID_HANDLE != dsdHnl )
	{
		AES_CDH_TRACE_MESSAGE("registerDSDHandles(), handle=%i registered to reactor", idx);
		// Register to reactor the DSD handle
		m_reactor->register_handler(dsdHnl,  this, ACE_Event_Handler::ACCEPT_MASK );
		// Store the handle for remove it from reactor
		fdList.push_back(dsdHnl);
		// get the next DSD handle
		dsdHnl =  m_CmdServerAcceptor.get_handle(++idx);
	}

	// Checks if at least one handle is valid
	if(0 == idx)
	{
		AES_CDH_TRACE_MESSAGE("registerDSDHandles(), no valid DSD handles");
		result = false;
	}
	if (true == result)
		AES_CDH_LOG(LOG_LEVEL_INFO, "registered DSD handles successfully");

	AES_CDH_TRACE_MESSAGE("Leaving");
	return result;
}

void ServR::removeDSDHandles()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	std::vector<ACE_HANDLE>::iterator it;

	// Remove all registered Handle from reactor
	for( it = fdList.begin(); it != fdList.end(); ++it )
	{
		m_reactor->remove_handler( (*it), ACE_Event_Handler::ACCEPT_MASK );
	}

	fdList.clear();
	AES_CDH_LOG(LOG_LEVEL_INFO, "Removed DSD handles successfully");
	AES_CDH_TRACE_MESSAGE("Leaving");
}

void ServR::stopDSDServer()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if(m_serverOnLine)
	{
		// Close the DSD server
		int result = m_CmdServerAcceptor.close();

		// check the closure result
		if(DSDConfig::DSDSuccess == result)
		{
			AES_CDH_TRACE_MESSAGE("DSD server closed");
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "DSD server closed successfully");
			// set online flag
			m_serverOnLine = false;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("error on DSD server closure");
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("DSD server already closed");
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

int ServR::handle_close()
{
	AES_CDH_TRACE_MESSAGE("Entering in handle_close()");
	stopDSDServer();
	AES_CDH_TRACE_MESSAGE("Leaving handle_close()");
	return 0;
}



void ServR::locking_function(int mode, int n, const char * file, int line)
{
(void)line;
(void)file;

  //GCC_DEBUG((aes_cdh_servr,"%s", "locking_function"));
  if (mode & CRYPTO_LOCK)
  {
        //AES_CDH_LOG(LOG_LEVEL_DEBUG, " %s\n", "acquire the lock"));
        mutex_buf[n].acquire();
  }
  else
  {
        //AES_CDH_LOG(LOG_LEVEL_DEBUG, " %s\n", "release the lock"));
        mutex_buf[n].release();
  }
}
 
unsigned long ServR::id_function(void)
{
 //  AES_CDH_LOG(LOG_LEVEL_DEBUG, " %s\n", "id_function %u",ACE_OS::thr_self()));
   return ((unsigned long)ACE_OS::thr_self() ); 
}
 
int ServR::thread_setup(void)
{
  AES_CDH_LOG(LOG_LEVEL_INFO, "thread_setup");
  mutex_buf= new(std::nothrow) ACE_Thread_Mutex[CRYPTO_num_locks()];
  if (mutex_buf == NULL)
   return 1;

  CRYPTO_set_id_callback((unsigned long (*)())ServR::id_function );
  CRYPTO_set_locking_callback((void (*)(int,int,const char*,int))ServR::locking_function );
  return 0;
}
 
int ServR::thread_cleanup(void)
{
	AES_CDH_LOG(LOG_LEVEL_INFO, "thread_cleanup");
	CRYPTO_set_id_callback(NULL);
	CRYPTO_set_locking_callback(NULL);

	if (mutex_buf != NULL) 
	{
		delete[] mutex_buf;
		mutex_buf = NULL;
	}
	return  0;
}

void ServR::requestShutdownOfCmdHdlrThread(const string destSetName,
						CmdHandler* pCmdHdlr )
{
	ServR::destSetMX_.acquire();
        ServR::stopSendRecordFile(destSetName);
        ServR::setAPIClosed(destSetName);
        ServR::removeEventHandler(destSetName,pCmdHdlr);
        ServR::destSetMX_.release();
}
