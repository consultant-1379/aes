#include <ace/OS.h>
#include <aes_afp_services.h>
#include <aes_afp_server.h>
#include <aes_afp_ha_service.h>
#include <aes_afp_defines.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(AES_AFP_services);

ACE_thread_t aes_afp_services::applicationThreadId = 0;
ACE_Recursive_Thread_Mutex ThrExitHandler::theThrMutex;
map<ACE_thread_t, bool> ThrExitHandler::theThrStatusMap;
volatile bool aes_afp_services::afpStopEvt = false;
bool scpMcpFlag = false; // afpdef improvement

aes_afp_datatask* aes_afp_services::taskRef = 0;
aes_afp_datablock* aes_afp_services::m_db = 0;

aes_afp_services::aes_afp_services()
: AFP_ServiceName_("AES_AFP_server")
 {
	StopEvent = 0;
	StopEvent = new ACE_Event();
	m_afpMainThreadId = 0;
	aes_afp_services::afpStopEvt = false;
	threadManager_ =  new ACE_Thread_Manager();
	dataThread_ = 0;
	apiThread_ =0;
	m_poFileStreamCommandHandler = 0;
	m_poFileStreamParamCommandHandler = 0;
	m_poFileTransferMHandler = 0;
	//m_poDataTransferMHandler =0;//AAA

}

aes_afp_services::~aes_afp_services()
{

	theOmHandler.Finalize();

	if(m_poFileStreamCommandHandler!= 0)
	{
		delete m_poFileStreamCommandHandler;
		m_poFileStreamCommandHandler = 0;
	}
	if(m_poFileStreamParamCommandHandler!= 0)
	{
		delete m_poFileStreamParamCommandHandler;
		m_poFileStreamParamCommandHandler = 0;
	}
	if(m_poFileTransferMHandler!= 0)
	{
		delete m_poFileTransferMHandler;
		m_poFileTransferMHandler = 0;
	}
	/* AAA
	if( m_poDataTransferMHandler != 0 )
	{
		delete m_poDataTransferMHandler;
		m_poDataTransferMHandler = 0;
	}
	*/
	if( apiThread_ != 0 )
	{
		delete apiThread_;
		apiThread_ = 0;
	}

	if( dataThread_ != 0 )
	{
		delete dataThread_;
		dataThread_ = 0;
	}
	if( StopEvent != 0 )
	{
		delete StopEvent;
		StopEvent = 0;
	}

	delete  threadManager_;
	threadManager_ = 0;
}

//	---------------------------------------------------------
//	       start()
//	---------------------------------------------------------
bool aes_afp_services::start ()
{
	if(theOmHandler.Init()!=ACS_CC_FAILURE)
	{
		if((AES_GCC_Util::fetchDnOfFileTransferObjFromIMM(theOmHandler))==-1)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR,"Unable to fetch DN of FileStreamPolicy root from IMM.");
			AES_AFP_TRACE_MESSAGE("Unable to fetch DN of FileStreamPolicy root from IMM.");
			AES_AFP_TRACE_MESSAGE( "DataTransfer IMM files are not configured properly. Hence exiting from Service ...");
			return false;
		}
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Failed to initialize OmHandler");
		AES_AFP_TRACE_MESSAGE("Failed to initialize OmHandler");
		return false;
	}

	signal( SIGPIPE, SIG_IGN);

	AES_AFP_TRACE_MESSAGE("FileTransferRoot object dn = %s",AES_GCC_Util::dnOfFileTransferM.c_str());

	aes_afp_parameter::load();

	scpMcpFlag = isMultiCP();

	AES_AFP_TRACE_MESSAGE("Starting AFP");

	//-----------------------
	// Create data task
	AES_AFP_LOG(LOG_LEVEL_INFO,"Creating datatask thread...");
	dataThread_ = new(std::nothrow) aes_afp_datatask();
	if( dataThread_ == 0 )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for dataThread_.");
		AES_AFP_TRACE_MESSAGE("Memory allocation failed for dataThread_.");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "########## DATA TASK CREATED");
	AES_AFP_TRACE_MESSAGE("########## DATA TASK CREATED");
	taskRef = dataThread_;
	
	
//-------------------------------------------------------------------------------------
// BEGIN Waiting for IMM Callbacks 
	ACE_INT32 retCode = 0;
	bool disaster = false; 
	bool stop_received = false; 

	for (; (!setupIMMCallBacks((*dataThread_), threadManager_)) && (!disaster) && (!stop_received); )
	{ // check CDH online
		ACE_Time_Value tv(2, 0);
		retCode = StopEvent->wait(&tv, 0);
		if(retCode<0)
		{
			int errnum = ACE_OS::last_error();
			AES_AFP_LOG(LOG_LEVEL_ERROR, "setupIMMCallBacks check Could not initiate Command handler");
		    AES_AFP_TRACE_MESSAGE("setupIMMCallBacks check Could not initiate Command handler");
			
			if( errnum == 62 ) //Timer Expired
			{
			  AES_AFP_TRACE_MESSAGE("setupIMMCallBacks check retry ongoing ..");
			  
			 if(m_poFileStreamCommandHandler!= 0)
	         {
			   m_poFileStreamCommandHandler->shutdown();
		       delete m_poFileStreamCommandHandler;
		       m_poFileStreamCommandHandler = 0;
	         }
	         if(m_poFileStreamParamCommandHandler!= 0)
	         {
			   m_poFileStreamParamCommandHandler->shutdown();		        
			   delete m_poFileStreamParamCommandHandler;
		       m_poFileStreamParamCommandHandler = 0;
	         }
	         if(m_poFileTransferMHandler!= 0)
	         {
			   m_poFileTransferMHandler->shutdown();			 
		       delete m_poFileTransferMHandler;
		       m_poFileTransferMHandler = 0;
	         }
	
			  continue;
			}
			else
			{
				disaster = true;
				return false;
			}
		}
		else if(retCode == 0)
		{
			AES_AFP_TRACE_MESSAGE( "[setupIMMCallBacks] Stop Event caught. Service stopped");
			stop_received = true;
			return true;
		}
	}

//-------------------------------------------------------------------------------------
// END Waiting for IMM Callbacks 
	//------------------------------------
	// Setup IMM Object implementers...
/*	if(!setupIMMCallBacks((*dataThread_), threadManager_))
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Could not initiate Command handler");
		AES_AFP_TRACE_MESSAGE("Could not initiate Command handler");
		return false;
	} */

	//-----------------------------
	// initializing database...
	AES_AFP_LOG(LOG_LEVEL_INFO, "initializing database...");
	AES_AFP_TRACE_MESSAGE("initializing database...");

	if (dataThread_->loadTQsDataBase() == false)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Cannot initialize TQ database");
		AES_AFP_TRACE_MESSAGE("ERROR: Cannot initialize TQ database");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "### TQ database initialized!");
	AES_AFP_TRACE_MESSAGE("### TQ database initialized!");

	//-----------------------------
	// Starting data task...
	AES_AFP_LOG(LOG_LEVEL_INFO, "starting data task...");
	AES_AFP_TRACE_MESSAGE("starting data task...");
	if (dataThread_->open(0, threadManager_) == -1)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Could not initiate database");
		AES_AFP_TRACE_MESSAGE("ERROR: Could not initiate database");
		Event::report(EVENT_STARTDATA, "SERVICE START FAULT", "Database init fault", "Can not start dataBase thread at service startup");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "### data task thread started");
	AES_AFP_TRACE_MESSAGE("### data task thread started");

	//-----------------------
	// Create apcom server
	AES_AFP_LOG(LOG_LEVEL_INFO, "Creating apcom_server thread");
	AES_AFP_TRACE_MESSAGE("Creating apcom_server thread");
	apiThread_ = new(std::nothrow) aes_afp_apcom_serv();
	if( apiThread_ == 0 )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Memory allocation failed for apiThread_.");
		AES_AFP_TRACE_MESSAGE("ERROR: Memory allocation failed for apiThread_.");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "### apcom_server thread created");
	AES_AFP_TRACE_MESSAGE("### apcom_server thread created");

	//-----------------------
	// Starting apcom server
	AES_AFP_LOG(LOG_LEVEL_INFO, "Starting apcom_server thread");
	AES_AFP_TRACE_MESSAGE("Starting apcom_server thread");
	if (apiThread_->open(threadManager_) == -1)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Could not initiate api server");
		AES_AFP_TRACE_MESSAGE("ERROR: Could not initiate api server");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "### apcom_server thread started");
	AES_AFP_TRACE_MESSAGE("### apcom_server thread started");

	//----------------------------------
	// Start using the apcom server
	// for internal communication...
	dataThread_->setServer(apiThread_);

	AES_AFP_TRACE_MESSAGE("Api server initiated");
	return true;
}

//	---------------------------------------------------------
//	       stop()
//	---------------------------------------------------------
bool aes_afp_services::stop ()
{
	AES_AFP_TRACE_MESSAGE("Sending HANGUP to database and apiserver");
	AES_AFP_LOG(LOG_LEVEL_INFO,"Sending HANGUP to database and apcom_server threads");
	aes_afp_services::afpStopEvt = true;
	if( apiThread_ !=0 )
	{
		sendHangup(apiThread_->msg_queue());
	}

	if( dataThread_ != 0 )
	{
		dataThread_->set_stop(true);
		sendHangup(dataThread_->msg_queue());

	}
	AES_AFP_LOG(LOG_LEVEL_INFO,"Invoking Shutdown on all object implementer threads");
	if(m_poFileStreamParamCommandHandler!= 0)
	{
		m_poFileStreamParamCommandHandler->shutdown();
		AES_AFP_TRACE_MESSAGE("Shutting down FileStreamParamCommandHandler");
	}
	if(m_poFileStreamCommandHandler!= 0)
	{
		AES_AFP_TRACE_MESSAGE("Shutting down FileStreamCommandHandler");
		m_poFileStreamCommandHandler->shutdown();
	}
	if(m_poFileTransferMHandler!= 0)
	{
		AES_AFP_TRACE_MESSAGE( "Shutting down FileTransferMHandler");
		m_poFileTransferMHandler->shutdown();
	}

	/* AAA
	if(m_poDataTransferMHandler != 0)
	{
		AES_AFP_TRACE_MESSAGE("Shutting down DataTransferMHandler");
		m_poDataTransferMHandler->shutdown();
	}
	*/

	threadManager_->wait();


	AES_AFP_TRACE_MESSAGE( "Database and apiserver finished");

	AES_AFP_TRACE_MESSAGE("exiting");

	return true;
}

//	---------------------------------------------------------
//	       cmdStart()
//	---------------------------------------------------------
bool aes_afp_services::cmdStart ()
{
	//## begin aes_afp_services::cmdStart%3869F8900384.body preserve=yes
	threadManager_->wait();
	AES_AFP_TRACE_MESSAGE("Entering");

	AES_AFP_TRACE_MESSAGE("Exiting");
	return true;
	//## end aes_afp_services::cmdStart%3869F8900384.body
}

//## Operation: sendHangup%386A0143032C; C++
//## Semantics:
//	---------------------------------------------------------
//	       sendHangup()
//	---------------------------------------------------------
void aes_afp_services::sendHangup (ACE_Message_Queue<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy>* receiver)
{
	AES_AFP_TRACE_MESSAGE( "Entering");
	aes_afp_msgblock* msgBlock;

	msgBlock = new aes_afp_msgblock(0, 0);
	msgBlock->setCallerId(this);
	msgBlock->setCallerName(AFP_ServiceName_);
	msgBlock->setMsgType(HANGUP);
	receiver->enqueue_head(msgBlock);
	AES_AFP_TRACE_MESSAGE("receiver = %ld ", (long int)receiver );
	AES_AFP_TRACE_MESSAGE( "Leaving");
}

//## Operation: suspendThreads%386B1BFA01CE; C++
//## Semantics:
//	---------------------------------------------------------
//	       suspendThreads()
//	---------------------------------------------------------
void aes_afp_services::suspendThreads ()
{
	//## begin aes_afp_services::suspendThreads%386B1BFA01CE.body preserve=yes
	dataThread_->suspend();
	apiThread_->suspend();
	//## end aes_afp_services::suspendThreads%386B1BFA01CE.body
}

//## Operation: resumeThreads%386B1C2003C7; C++
//## Semantics:
//	---------------------------------------------------------
//	       resumeThreads()
//	---------------------------------------------------------
void aes_afp_services::resumeThreads ()
{
	//## begin aes_afp_services::resumeThreads%386B1C2003C7.body preserve=yes
	dataThread_->resume();
	apiThread_->resume();
	//## end aes_afp_services::resumeThreads%386B1C2003C7.body
}

bool aes_afp_services::setupIMMCallBacks( aes_afp_datatask &dataThread,
		ACE_Thread_Manager *threadManager_)
{
	AES_AFP_TRACE_MESSAGE("Entering");

	m_poFileStreamParamCommandHandler = new(std::nothrow) AES_AFP_CommandHandler( TQ_CL_FILETQPARAMNAME,TQ_IMPL_FILETQPARAMNAME,ACS_APGCC_ONE,dataThread, threadManager_);
	if(m_poFileStreamParamCommandHandler)
	{
		if( m_poFileStreamParamCommandHandler->setImpl() != ACS_CC_SUCCESS )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Param Command handler svc failed");
			AES_AFP_TRACE_MESSAGE("Param Command handler svc failed");
			return false;
		}
		m_poFileStreamParamCommandHandler->activate();
	} 
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Param Command handler svc failed");
		AES_AFP_TRACE_MESSAGE("Param Command handler svc failed");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "ObjectImplementer thread for AdvancedFileTransferQueueInfo is activated");
	m_poFileStreamCommandHandler = new(std::nothrow) AES_AFP_CommandHandler( TQ_CL_FILETQ,TQ_IMPL_FILETQ, ACS_APGCC_ONE,dataThread, threadManager_);
	if(m_poFileStreamCommandHandler)
	{
		if( m_poFileStreamCommandHandler->setImpl() != ACS_CC_SUCCESS )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Command handler svc failed");
			AES_AFP_TRACE_MESSAGE("Command handler svc failed");
			return false;
		}
		m_poFileStreamCommandHandler->activate();
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Command handler svc failed");
		AES_AFP_TRACE_MESSAGE("Command handler svc failed");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO,"ObjectImplementer thread for FileTransferQueue is activated");

	/*
	m_poDataTransferMHandler = new(std::nothrow) AES_AFP_DataTransferMCmdHandler(AES_AFP_DATATRANSFERM_CLASSNAME,
			AES_AFP_DATATRANSFERM_IMPLEMENTER,
			ACS_APGCC_ONE,dataThread,  threadManager_);
	if(m_poDataTransferMHandler)
	{
		if( m_poDataTransferMHandler->setImpl() != ACS_CC_SUCCESS )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "DataTransferM handler failed");
			AES_AFP_TRACE_MESSAGE( "DataTransferM handler failed");
			return false;
		}
		m_poDataTransferMHandler->activate();
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "DataTransferM set handler object is null");
		AES_AFP_TRACE_MESSAGE("DataTransferM set handler object is null");
		return false;
	}
	*/

	AES_AFP_LOG(LOG_LEVEL_INFO,"ObjectImplementer thread for DataTransferM is activated");
	m_poFileTransferMHandler = new(std::nothrow) AES_AFP_DataTransferMCmdHandler(AES_AFP_FILETRANSFERM_CLASSNAME,
			AES_AFP_FILETRANSFERM_IMPLEMENTER,
			ACS_APGCC_ONE,dataThread, threadManager_);
	if(m_poFileTransferMHandler)
	{
		if( m_poFileTransferMHandler->setImpl() != ACS_CC_SUCCESS )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "FileTransferM handler failed");
			AES_AFP_TRACE_MESSAGE("FileTransferM handler failed");
			return false;
		}
		m_poFileTransferMHandler->activate();
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "FileTransferM set handler object is null");
		AES_AFP_TRACE_MESSAGE("FileTransferM set handler object is null");
		return false;
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "ObjectImplementer thread for FileTransferManager is activated");
	AES_AFP_TRACE_MESSAGE("Leaving");

	return true;
}

ACS_CC_ReturnType aes_afp_services::setupAFPThread(aes_afp_services * aAFPPtr)
{

	AES_AFP_TRACE_MESSAGE("Entering");
	const ACE_TCHAR* lpszThreadName1 = "AFPFunctionalThread";
	int mythread = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;
	AES_AFP_TRACE_MESSAGE("Spawning AFP Functional Thread");
	AES_AFP_LOG(LOG_LEVEL_INFO,"Spawning AFP Functional Thread");
	mythread = ACE_Thread_Manager::instance()->spawn(&AFPThreadFunctionalMethod,
			(void*)aAFPPtr ,
			THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED,
			&aAFPPtr->m_afpMainThreadId,
			0,
			ACE_DEFAULT_THREAD_PRIORITY,
			-1,
			0,
			ACE_DEFAULT_THREAD_STACKSIZE,
			&lpszThreadName1);
	if(mythread == -1)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Failure occurred while spawning AFP Functional Thread");
		AES_AFP_TRACE_MESSAGE("Failure occurred while spawning AFP Functional Thread");
		rc = ACS_CC_FAILURE;
	}

	AES_AFP_TRACE_MESSAGE("Leaving");
	return rc;

}/*end setupAIAPThread*/

ACE_THR_FUNC_RETURN aes_afp_services::AFPThreadFunctionalMethod(void* aAfpPtr)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	aes_afp_services *myAFPPtr = NULL;
	myAFPPtr        = reinterpret_cast<aes_afp_services*>(aAfpPtr);

	if(myAFPPtr == NULL)
	{
		AES_AFP_LOG(LOG_LEVEL_INFO,"myAFPPtr is NULL");
		AES_AFP_TRACE_MESSAGE("myAFPPtr is NULL");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return NULL;
	}

	if((myAFPPtr->startAFPFunctionality() == ACS_CC_FAILURE))
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Failure occurred while starting AFP Functionality");
		AES_AFP_TRACE_MESSAGE("Failure occurred while starting AFP Functionality");
		return NULL;
	}

	AES_AFP_TRACE_MESSAGE( "Leaving");
	return aAfpPtr;
}
ACS_CC_ReturnType aes_afp_services::startAFPFunctionality()
{
	AES_AFP_TRACE_MESSAGE("Entering");
	AES_AFP_TRACE_MESSAGE("HA is Starting AFP Functionality");
	if( this->AFP_ServiceMain() == false )
	{
		AES_AFP_LOG(LOG_LEVEL_FATAL,"Error in Start of Main Thread.");
		AES_AFP_TRACE_MESSAGE("Error in Start of Main Thread.");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return ACS_CC_FAILURE;
	}
	AES_AFP_TRACE_MESSAGE("Leaving");
	return ACS_CC_SUCCESS;
}

ACE_thread_t aes_afp_services::getAppThreadId()const
{
	return applicationThreadId;
}

bool aes_afp_services::AFP_ServiceMain()
{
	AES_AFP_TRACE_MESSAGE( "Entering");


	if( StopEvent == 0 )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Unable to allocate memory for Stop Event.");
		AES_AFP_TRACE_MESSAGE("Unable to allocate memory for Stop Event.");
		AES_AFP_TRACE_MESSAGE("Leaving");
		return false;
	}

	for (; !isCSonline(); )
	{ // check CS online
		ACE_Time_Value tv(2, 0);
		ACE_INT32 dwRes = StopEvent->wait(&tv, 0);
		if(dwRes < 0)
		{
			int errnum = ACE_OS::last_error();
			AES_AFP_LOG(LOG_LEVEL_ERROR, "[cs online check] Service error in waiting on the stop event, error code: %d", ACE_OS::last_error());
			AES_AFP_TRACE_MESSAGE("[cs online check] Service error in waiting on the stop event, error code: %d", ACE_OS::last_error());
			if( errnum == 62 )
			{
				continue;
			}
			AES_AFP_TRACE_MESSAGE("Leaving");
			return false;
		}
		else if(dwRes == 0)
		{
			AES_AFP_LOG(LOG_LEVEL_INFO,"[cs online check] Stop Event caught. Service stopped");
			AES_AFP_TRACE_MESSAGE("[cs online check] Stop Event caught. Service stopped");
			AES_AFP_TRACE_MESSAGE("Leaving");
			return true;
		}
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "CS service is online");
	ACE_INT32 retCode = 0;
	bool disaster = false; 
	bool stop_received = false; 

	for (; (!isCDHonline()) && (!disaster) && (!stop_received); )
	{ // check CDH online
		ACE_Time_Value tv(2, 0);
		retCode = StopEvent->wait(&tv, 0);
		if(retCode<0)
		{
			int errnum = ACE_OS::last_error();
			AES_AFP_LOG(LOG_LEVEL_ERROR, "[cdh online check] Service error in event wait. error code: %d", ACE_OS::last_error());
			AES_AFP_TRACE_MESSAGE("[cdh online check] Service error in event wait. error code: %d", ACE_OS::last_error());
			if( errnum == 62 ) //Timer Expired
			{
				continue;
			}
			else
			{
				disaster = true;
				return false;
			}
		}
		else if(retCode == 0)
		{
			AES_AFP_TRACE_MESSAGE( "[cdh online check] Stop Event caught. Service stopped");
			stop_received = true;
			return true;
		}
	}
	AES_AFP_LOG(LOG_LEVEL_INFO, "CDH service is online !");
	if ((disaster == false) && (stop_received == false))
	{
		//starts the data thread
		if (start() == true)
		{
			retCode = StopEvent->wait();
			stop();
		}
		else
		{
			stop();
			return false;
		}
	}
	return true;
}

//-------------------------------------------------------------------
//// isCSonline
////-------------------------------------------------------------------
bool aes_afp_services::isCSonline()
{
	bool online = false;
	ACS_CS_API_CP * cpTable = ACS_CS_API::createCPInstance();
	if (cpTable)
	{
		ACS_CS_API_IdList cpList;
		ACS_CS_API_NS::CS_API_Result result = cpTable->getCPList(cpList);
		if (result == ACS_CS_API_NS::Result_Success) online = true;
		ACS_CS_API::deleteCPInstance(cpTable);
	}
	return online;
}

//-------------------------------------------------------------------
//// isAFPonline()
////-------------------------------------------------------------------
bool aes_afp_services::isCDHonline()
{
	bool online = false;
	ACS_DSD_Client * client = new ACS_DSD_Client();
	std::vector<ACS_DSD_Node>  fn;
	std::vector<ACS_DSD_Node>  un;
	std::string serviceName = "AES_CDH_server";
	std::string domainName = "AES";
	ACS_DSD_Node node;
	int ret = client->get_local_node(node);
	AES_AFP_TRACE_MESSAGE("status of get_local_node :%d", ret);
	ACE_INT32  ownSysID =node.system_id;
	AES_AFP_TRACE_MESSAGE("Query on this node %u", ownSysID);
	const int queryTimeout = 5000;
	int res = client->query(serviceName.c_str(), domainName.c_str(), acs_dsd::SYSTEM_TYPE_AP,  fn, un,queryTimeout);
	AES_AFP_TRACE_MESSAGE( "Query status :%d", res);
	if (res == 0 || res == acs_dsd::WAR_TIMEOUT_EXPIRED )
	{
		for (unsigned int i = 0; i < fn.size(); i++)
		{
			AES_AFP_TRACE_MESSAGE("Reachable: system_id == %d, node_state == %d", fn[i].system_id, fn[i].node_state);
		}

		for (unsigned int i = 0; i < un.size(); i++)
		{
			AES_AFP_TRACE_MESSAGE("Unreachable: system_id == %d, node_state == %d", un[i].system_id, un[i].node_state);
		}
		for (ACE_UINT32 i = 0; i < fn.size(); i++)
		{
			if (ownSysID == fn[i].system_id)
			{
				online = true;
				break;
			}
		}
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_DEBUG, "Query failed! err code: %u, err text: %s", client->last_error(), client->last_error_text());
		AES_AFP_TRACE_MESSAGE("Query failed! err code: %u, err text: %s", client->last_error(), client->last_error_text());
	}
	AES_AFP_TRACE_MESSAGE("CDH %s!", (online ? "online" : "offline"));
	delete client;
	return online;
}

aes_afp_datatask* aes_afp_services::getTask()
{
	return taskRef;
}

bool aes_afp_services::isMultiCP()
{
	bool isMultipleCPSystem = false;

	ACS_CS_API_NS::CS_API_Result multiCP = ACS_CS_API_NetworkElement::isMultipleCPSystem(isMultipleCPSystem);
	if ((multiCP == ACS_CS_API_NS::Result_Success) && (isMultipleCPSystem))
	{
		return true;
	}					
	return false;
}
