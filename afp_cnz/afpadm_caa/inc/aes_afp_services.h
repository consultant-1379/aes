#ifndef aes_afp_services_h
#define aes_afp_services_h 1



#include <aes_gcc_log.h>
#include <aes_afp_datatask.h>
#include <aes_afp_apcom_serv.h>
#include <aes_afp_events.h>
#include <aes_afp_msgblock.h>
#include <aes_afp_parameter.h>
#include <aes_afp_command_handler.h>
#include <aes_afp_defines.h>
#include <acs_apgcc_omhandler.h>
#include <aes_afp_datatransferm_handler.h>
#include <aes_afp_transfer_entity.h>

#include <string>

#include <ace/ACE.h>
#include <ace/Thread_Manager.h>
#include <ace/OS.h>
#include <ace/Message_Queue.h>
#include <ace/Event_Handler.h>
const string AES_DATA_PATH = "/data_transfer/";
//## Class: aes_afp_services%3869B7FF02DC

class ThrExitHandler : public ACE_At_Thread_Exit
{
	/*=====================================================================
				PUBLIC DECLARATION SECTION
==================================================================== */
public:

	/*===================================================================

	@brief		init
				This method initializes thread exit handler.

	@return		bool
				TRUE/FALSE

=================================================================== */

	static bool init()
	{
		ThrExitHandler *thrExitHandler = new ThrExitHandler();
		if( thrExitHandler == 0 )
		{
			return false;
		}
		thrExitHandler->setThrStatusInMap(ACE_Thread_Manager::instance()->thr_self(), true);

		int result = ACE_Thread_Manager::instance()->at_exit( thrExitHandler );

		if( result == -1 )
		{
			return false;
		}
		else
		{
			return true;
		}
	}


	/*===================================================================

	@brief		cleanup
				This method cleanup thread exit handler.

	@return		void
				TRUE/FALSE

=================================================================== */
	static void cleanup()
	{
		theThrMutex.remove();
		theThrStatusMap.clear();
	}

	/*===================================================================

	@brief		CheckAllFuncThreads
				This method checks all threads status in map.

	@param		void

	@return		bool
				TRUE/FALSE

=================================================================== */
	static bool CheckAllFuncThreads( void )
	{
		//Lock the mutex.
		theThrMutex.acquire();

		//Check if any thread has exited or not.
		for( map<ACE_thread_t, bool>::iterator itr = theThrStatusMap.begin();
				itr != theThrStatusMap.end();  ++itr )
		{
			if( itr->second == false )
			{
				theThrMutex.release();
				return false;
			}
		}
		theThrMutex.release();
		return true;
	}

	/*===================================================================

	@brief		apply

	@param		void

	@return		void

=================================================================== */
	virtual void apply(void)
	{
		if( !theThrStatusMap.empty())
		{
			setThrStatusInMap( ACE_Thread_Manager::instance()->thr_self(), false);
		}
	}

	/*=====================================================================
			PRIVATE DECLARATION SECTION
==================================================================== */
private:

	/*===================================================================

	@brief		setThrStatusInMap
				This method sets thread status in map.

	@param		thrId

	@param		isThrAlive

	@return		void

=================================================================== */
	void setThrStatusInMap( ACE_thread_t thrId, bool isThrAlive)
	{
		bool isThrPresentInMap = false;

		//Acquire the Mutex
		theThrMutex.acquire();

		//Now update the data in the global map.
		map<ACE_thread_t, bool>::iterator itr =  theThrStatusMap.begin();

		while( itr != theThrStatusMap.end() )
		{
			if( itr->first == thrId )
			{
				isThrPresentInMap = true;
				break;
			}
			++itr;
		}
		if( isThrPresentInMap )
		{
			theThrStatusMap.erase(thrId);
		}
		theThrStatusMap.insert(std::pair<ACE_thread_t, bool>(thrId, isThrAlive));
		theThrMutex.release();
	}

	/*=====================================================================
			STATIC	DECLARATION SECTION
==================================================================== */
	/*===================================================================

	@brief		theThrStatusMap

=================================================================== */
	static map<ACE_thread_t, bool> theThrStatusMap;

	/*===================================================================

	@brief		theThrMutex

=================================================================== */
	static ACE_Recursive_Thread_Mutex  theThrMutex;
};


class aes_afp_services 
{
public:
	//## Constructors (generated)
	aes_afp_services();

	//## Destructor (generated)
	virtual ~aes_afp_services();


	bool isMultiCP();
	bool start ();
	//	Stops the service.
	bool stop ();

	//	Starts the service from the command line.
	bool cmdStart ();

	//	Sends hangup to the started threads.
	void sendHangup (ACE_Message_Queue<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy>* receiver);

	//	Suspends all started threads.
	void suspendThreads ();

	//	Resumes all previosly suspended threads.
	void resumeThreads ();

	bool setupIMMCallBacks( aes_afp_datatask &dataThread, 
							ACE_Thread_Manager* thrMgr);
	//Backup and restore functions
	void createBrfcThread();
	bool initializeBrfcRunTimeOwner();
	void createBrfcRunTimeOwner();
	int createParticipant();
	void shutdownBrfcThread(void);
	bool deleteBrfcObject();

 	static aes_afp_datablock* m_db;
	/*=================================================================== */
	static ACS_CC_ReturnType setupAFPThread(aes_afp_services * aAFPPtr);
	/*===================================================================

   		@brief		startAFPFunctionality

   		@return		ACS_CC_ReturnType
   					ACS_CC_CUCCESS/ACS_CC_FAILURE

   	=================================================================== */
	ACS_CC_ReturnType startAFPFunctionality();
	/*===================================================================

   		@brief		AFPThreadFunctionalMethod

   		@param		aAfpPtr

   		@return		ACE_THR_FUNC_RETURN

	=================================================================== */
	static  ACE_THR_FUNC_RETURN AFPThreadFunctionalMethod(void* aAfpPtr);

	/*===================================================================

   		@brief		getAppThreadId

   		@return		ACE_thread_t

   	=================================================================== */
	ACE_thread_t getAppThreadId()const;
	/*===================================================================*/

	/*===================================================================*/
	void publish();
	/*===================================================================*/

	/*=================================================================== */
	ACE_Event *StopEvent;
	/*=================================================================== */

	/*=================================================================== */
	bool isCSonline();
	/*=================================================================== */

	/*=================================================================== */
	static bool isCDHonline();
	/*=================================================================== */

	/*=================================================================== */
	bool AFP_ServiceMain();
	/*=================================================================== */


	static volatile bool afpStopEvt;
	static aes_afp_datatask* taskRef;

	ACE_thread_t m_afpMainThreadId;

	static aes_afp_datatask* getTask();
private:
// Data Members for Associations

	ACE_Thread_Manager *threadManager_;

	const std::string AFP_ServiceName_;


	aes_afp_datatask *dataThread_;

	aes_afp_apcom_serv *apiThread_;

	static	ACE_thread_t applicationThreadId;

	OmHandler theOmHandler;

public:
	AES_AFP_CommandHandler *m_poFileStreamCommandHandler;
	AES_AFP_CommandHandler *m_poFileStreamParamCommandHandler;
	//AES_AFP_DataTransferMCmdHandler* m_poDataTransferMHandler;
	AES_AFP_DataTransferMCmdHandler* m_poFileTransferMHandler;

#if 0  //Transfer Entity removel from model
	static aes_afp_transfer_entity* pTransEntity_;
	aes_afp_transferEntity_runtimeHandlerThread* pTransEntityRunTimeHandlerThrd_;
#endif

};
#endif
