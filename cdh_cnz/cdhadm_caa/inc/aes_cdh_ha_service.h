/*=================================================================== */
/**
   @file   aes_cdh_ha_service.h

   @brief Header file for CDH module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       12/10/2011   XBHADUR   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef AES_CDH_HA_Service_H_
#define AES_CDH_HA_Service_H_
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Task.h>
#include <ace/ACE.h>
#include <ace/OS_NS_poll.h>
#include <ace/ace_wchar.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Event.h>
#include <ace/Task.h>
#include <ace/Thread_Manager.h>
#include <string>
#include <vector>
#include <map>
#include <ACS_APGCC_ApplicationManager.h>
#include <servr.h>
using namespace std;

/**
 *      @brief          ThrExitHandler
 *                      Class for handling thread exit.
 *
 */

class ThrExitHandler : public ACE_At_Thread_Exit
{
public:
	/*=================================================================== */
	/**
						@brief                  This method initializes thread exit handler.
						@pre					none
						@post					none
						@return                 none
						@exception              none
	 */
	/*=================================================================== */
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

	/*=================================================================== */
	/**
					@brief                  This method cleanup thread exit handler.
					@pre					none
					@post					none
					@return                 none
					@exception              none
	 */
	/*=================================================================== */
	static void cleanup()
	{
		theThrMutex.remove();
		theThrStatusMap.clear();
	}

	/*=================================================================== */
	/**
				@brief                  This method checks all threads status in map.
				@pre					none
				@post					none
				@return                 bool
				@exception              none
	 */
	/*=================================================================== */
	static bool CheckAllFuncThreads( void )
	{
		//Lock the mutex.
		theThrMutex.acquire();

		if(theThrStatusMap.empty())
		{
			theThrMutex.release();
			return false;
		}

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


	virtual void apply(void)
	{
		if( !theThrStatusMap.empty())
		{
			setThrStatusInMap( ACE_Thread_Manager::instance()->thr_self(), false);
		}
	}

private:

	/*=================================================================== */
	/**
			@brief                  This method sets thread status in map.
			@pre					none
			@post					none
			@return                 none
			@exception              none
	 */
	/*=================================================================== */
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

	static map<ACE_thread_t, bool> theThrStatusMap;
	static ACE_Recursive_Thread_Mutex  theThrMutex;
};


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
 	@brief	AES_CDH_HA_Service
 */
/*===================================================================*/
class  AES_CDH_HA_Service: public ACS_APGCC_ApplicationManager
{
	/*=====================================================================
		                        PRIVATE DECLARATION SECTION
	==================================================================== */
   private:
	/*===================================================================
	                        PRIVATE ATTRIBUTE
	=================================================================== */

	/**
		@brief	applicationThreadId
	*/
	ACE_thread_t	m_applicationThreadId;

	/*=====================================================================
	                       PUBLIC DECLARATION SECTION
	==================================================================== */
   public:

	ServR *theServR;
	 /*=====================================================================
		                       CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief     Destructor for AES_CDH_HA_Service class.

		             Default destructor.

	*/
	/*=================================================================== */
	~AES_CDH_HA_Service();

	/*===================================================================
		                      PUBLIC ATTRIBUTE
	=================================================================== */
	/*=================================================================== 
		                       CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief     Constructor for AES_CDH_HA_Service class.

		             Default constructor, used to initialize variables.

	*/
	/*=================================================================== */
   	AES_CDH_HA_Service(const char* daemon_name, const char* username);

   	/*=================================================================== */
   	  /**
   	        @brief     performStateTransitionToActiveJobs

   	        @par       Deprecated
   	                   never

   	        @pre       none

   	        @post      none

   	        @param     previousHAState

   	        @return    ACS_APGCC_ReturnType

   	        @exception none
   	   */
   	/*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToActiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/*=================================================================== */
	   	  /**
	   	        @brief     performStateTransitionToPassiveJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @param     previousHAState

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	/*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToPassiveJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/*=================================================================== */
	   	  /**
	   	        @brief     performStateTransitionToQueisingJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @param     previousHAState

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	/*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToQueisingJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/*=================================================================== */
                  /**
                        @brief     performStateTransitionToQueiscedJobs

                        @par       Deprecated
                                   never

                        @pre       none

                        @post      none

                        @param     previousHAState

                        @return    ACS_APGCC_ReturnType

                        @exception none
                   */
     /*=================================================================== */
	ACS_APGCC_ReturnType performStateTransitionToQuiescedJobs(ACS_APGCC_AMF_HA_StateT previousHAState);

	/*=================================================================== */
	   	  /**
	   	        @brief     performComponentHealthCheck

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	 /*=================================================================== */
	ACS_APGCC_ReturnType performComponentHealthCheck(void);
	/*=================================================================== */
	   	  /**
	   	        @brief     performComponentTerminateJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	/*=================================================================== */
	ACS_APGCC_ReturnType performComponentTerminateJobs(void);
	/*=================================================================== */
	   	  /**
	   	        @brief     performComponentRemoveJobs

	   	        @par       Deprecated
	   	                   never

	   	        @pre       none

	   	        @post      none

	   	        @return    ACS_APGCC_ReturnType

	   	        @exception none
	   	   */
	 /*=================================================================== */
	ACS_APGCC_ReturnType performComponentRemoveJobs (void);
	/*=================================================================== */

	 /*=================================================================== */
		/**
			@brief		performApplicationShutdownJobs

			@pre		none

			@post		none

			@return 	ACS_APGCC_ReturnType

			@exception	none
		**/
	 /*=================================================================== */
	ACS_APGCC_ReturnType performApplicationShutdownJobs(void);
	 /*=================================================================== */

	 /*=================================================================== */
		/**
			@brief		goActive

			@pre		none

			@post		none

			@return		ACS_APGCC_ReturnType

			@exception	none
		**/
	 /*=================================================================== */
	ACS_APGCC_ReturnType goActive();
	 /*=================================================================== */

	 /*=================================================================== */
		/**
			@brief		goPassive

			@pre		none

			@post		none

			@return		ACS_APGCC_ReturnType

			@exception	none
		**/
	 /*=================================================================== */
	ACS_APGCC_ReturnType goPassive();
	 /*=================================================================== */

	 /*=================================================================== */
		/**
			@brief		shutdown

			@pre		none

			@post		none

			@return		ACS_APGCC_ReturnType

			@exception	none
		**/
	 /*=================================================================== */
	ACS_APGCC_ReturnType shutdown();
	 /*=================================================================== */
};
#endif


