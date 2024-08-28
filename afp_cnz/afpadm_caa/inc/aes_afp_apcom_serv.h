/*=================================================================== */
/**
   @file   aes_afp_apcom_serv.h

   @brief Header file for aes_afp_apcom_serv class
				 Holds the server part of the AFP API.

   @version 1.0.0
 */
/*
   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A      22/09/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef aes_afp_apcom_serv_h
#define aes_afp_apcom_serv_h 1

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Task.h> // Must be first
#include <ace/Monotonic_Time_Policy.h>
#include "aes_afp_events.h"
#include "aes_afp_msgblock.h"
#include <utility>
#include <map>
#include <aes_gcc_eventhandler.h>
#include <aes_gcc_log.h>

#include <ACS_DSD_Server.h>

/*====================================================================
                        FARWARD DECLARATION SECTION
==================================================================== */
/*==================================================================== */
/** @brief 			aes_afp_apcom_hand
					This class is responsible for the server thread in API.
 */
/*==================================================================== */
class aes_afp_apcom_hand;
/*====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/*==================================================================== */
/** @brief 			AceTask_ApcomservTask
					This class stores the connected API clients.
 */
/*==================================================================== */
typedef ACE_Task< ACE_MT_SYNCH, ACE_Monotonic_Time_Policy > AceTask_ApcomservTask;
/*==================================================================== */
/** @brief 			AceTask_ApcomservTask
					This Map is used to insert a API client name and
					corresponding client object in to map HandMap.
 */
/*==================================================================== */
typedef std::map< std::string , aes_afp_apcom_hand* , std::less<std::string>  > HandMap;

/*==================================================================== */
/** @brief 			HandPair
					Pair of server name and corresponding client object.
 */
/*==================================================================== */
typedef std::pair< std::string , aes_afp_apcom_hand*  > HandPair;

/*====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class aes_afp_apcom_serv : public AceTask_ApcomservTask
{
	/*====================================================================
						PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
					CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
/**
	  @brief           Parametrical constructor
						<Need to be update>
	  @param 			DSDService
						Dsd server object

	  @return          void

	  @exception       none
 */
/*=================================================================== */
	aes_afp_apcom_serv ();
/*===================================================================
					CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
/**

	  @brief           Destructor

	  @exception       none
 */
/*=================================================================== */
	virtual ~aes_afp_apcom_serv();
/*=================================================================== */
/**

		@brief         open
					   Opens the thread.

		@param         queue
					   ACE_Task object to handle dsd communication
		@param         thrMgr
					   ACE_Thread_Manager instance

		@return 	   ACE_INT32

		@exception     none
 */
/*=================================================================== */
	ACE_INT32 open ( ACE_Thread_Manager* thrMgr = 0);
/*=================================================================== */

	bool publishToDSD();

	void unpublishToDSD();

/**

		@brief         	close
						Closes the thread.

		@param         	flags

		@return 	   	ACE_INT32

		@exception     	none
 */
/*=================================================================== */
	ACE_INT32 close (ACE_UINT64 flags = 0);
/*=================================================================== */
/**

		@brief         	svc
						Threads working method.

		@return 	   	ACE_INT32

		@exception     	none
 */
/*=================================================================== */
	ACE_INT32 svc (void );

	// Data Members for Associations
/*=================================================================== */
/**

		@brief         	handMap_
						This Map instance is used to insert a API client name
						and corresponding client object in to map HandMap.
*/
/*=================================================================== */
	HandMap handMap_;
/*==================================================================== */
/**
		@brief 			handPair_
						Stores server name of the AFP API and
						corresponding client object
*/
/*==================================================================== */
	HandPair handPair_;
	static ACE_Recursive_Thread_Mutex afpMutex;
/*====================================================================
				 PRIVATE DECLARATION SECTION
==================================================================== */
private:

/*==================================================================== */
/**
		@brief 			running_
						True if the server is running.
*/
/*==================================================================== */
	bool running_;

/*==================================================================== */
/**
		@brief 			m_DSDServer
						Pointer to the DSD server object
*/
/*==================================================================== */
	ACS_DSD_Server *m_DSDServer;  // HY27277 
/*==================================================================== */
/**
		@brief 			convSession_
						Pointer to the DSD session object
*/
/*==================================================================== */

	ACS_DSD_Session *convSession_;
	void sendHangUpToHand();
};
#endif
