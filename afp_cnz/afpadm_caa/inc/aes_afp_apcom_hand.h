/*=================================================================== */
/**
   @file   aes_afp_apcom_hand.h

   @brief Header file for aes_afp_apcom_hand class
             Holds the handle class for the server part of AFP API.

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
#ifndef aes_afp_apcom_hand_h
#define aes_afp_apcom_hand_h_1

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Task.h> // Must be first


#include "aes_afp_datablock.h"
#include "aes_afp_msgblock.h"
#include "aes_afp_msgtypes.h"
#include "aes_afp_threadmap.h"
#include "aes_afp_criticalsection.h"
#include "aes_afp_dsdsessionmap.h"
#include "aes_afp_divide.h"
#include "aes_afp_renametypes.h"
#include "aes_afp_protocol.h"
#include <string>
#include <aes_gcc_log.h>

/*====================================================================
                        FARWARD DECLARATION SECTION
==================================================================== */
class aes_afp_apcom_serv;
class ACS_DSD_Session;

/*====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  Type of ACE_Task <ACE_MT_SYNCH,ACE_Monotonic_Time_Policy> type
           This type is responsible for the client thread in API.
*/
/*=================================================================== */
typedef ACE_Task< ACE_MT_SYNCH , ACE_Monotonic_Time_Policy > AceTask_apcomhandTask;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_afp_apcom_hand

                 The handle class for the server part of AFP API.
*/
/*=================================================================== */
class aes_afp_apcom_hand : public AceTask_apcomhandTask
{
/*=====================================================================
						PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
						CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

	  @brief           Default constructor

	  @return          void

	  @exception       none
   */
/*=================================================================== */
	aes_afp_apcom_hand();
/*=================================================================== */
   /**

	  @brief           Parametrical constructor
					   Provide dsdservice instance and aes_afp_apcom_serv instance as arguments
	  @param           service
					   ACS_DSD_Server instance
	  @param           mother
					   aes_afp_apcom_serv instance
	  @return          void
	  @exception       none
   */
/*=================================================================== */
	aes_afp_apcom_hand ( aes_afp_apcom_serv *mother);

/*===================================================================
						CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

	  @brief           Destructor

	  @exception       none
   */
/*=================================================================== */
	virtual ~aes_afp_apcom_hand();

/*=================================================================== */
   /**

	  @brief           open

					   Opens the thread.

	  @param           sess
					   ACS_DSD_Session object

	  @return          ACE_INT32

	  @exception       none
   */
/*=================================================================== */
	ACE_INT32 open (ACS_DSD_Session *sess, ACE_Thread_Manager* thrMgr);

/*=================================================================== */
   /**

	  @brief          	close

						Closes the thread.

	  @param          	flags
						Update the flags description
	  @return          	ACE_INT32

	  @exception       	none
   */
/*=================================================================== */
	ACE_INT32 close (ACE_UINT64 flags = 0);


/*=================================================================== */
   /**

	  @brief          	svc
						Threads working method.

	  @return          	ACE_INT32

	  @exception       	none
   */
/*=================================================================== */
	ACE_INT32 svc (void );

	//
/*=================================================================== */
   /**

	  @brief          	toUpper

						Converts a string to upper case.

	  @param          	str

	  @return         	void

	  @exception       	none
   */
/*=================================================================== */
	void toUpper (std::string& str);

/*=====================================================================
						PRIVATE DECLARATION SECTION
==================================================================== */
private:

/*=================================================================== */
   /**

	  @brief          	decodeDataPart

						Decodes the datapart of the string received from API.

	  @param          	dataBuffer

	  @param          	startPosition

	  @param          	dataBlock

	  @return         	bool

	  @exception       	none
   */
/*=================================================================== */
	//
	bool decodeDataPart (std::string& dataBuffer, ACE_UINT16& startPosition, aes_afp_datablock* dataBlock);
/*=================================================================== */
   /**

	  @brief          	toUpper

						Converts a string to upper case.

	  @param          	str

	  @return         	ACE_TCHAR*

	  @exception       	none
   */
/*=================================================================== */
	ACE_TCHAR* toUpper (ACE_TCHAR* str);
/*=================================================================== */
/**
 * @brief 		protocol_
 * 				aes_afp_protocolapi instance
 */
/*=================================================================== */
	aes_afp_protocolapi protocol_;

/*=================================================================== */
/**
 * @brief 		motherTask_
 * 				Pointer to apcom server object.
 */
/*=================================================================== */
	aes_afp_apcom_serv* motherTask_;

/*=================================================================== */
/**
 * @brief 		applAttached_
 * 				Flag for determining if an application is attached to the file destination.
 */
/*=================================================================== */
	bool applAttached_;
/*=================================================================== */
/**
 * @brief 		nameOfDatatask_
 * 				Name of the data task
 */
/*=================================================================== */
	std::string nameOfDatatask_;
/*=================================================================== */
/**
 * @brief 		myThreadName_
 * 				Thread name
 */
/*=================================================================== */
	std::string myThreadName_;
/*=================================================================== */
/**
 * @brief 		attachedFileDest_
 * 				Name of the file destination
 */
/*=================================================================== */
	std::string attachedFileDest_;
/*=================================================================== */
/**
 * @brief 		EMPTYTEMPLATE_
 * 				EMPTYTEMPLATE_
 */
/*=================================================================== */
	std::string EMPTYTEMPLATE_;
/*=================================================================== */

/*=================================================================== */
/**
 * @brief 		convSession_
 * 				DSD session object pointer
 */
/*=================================================================== */
	ACS_DSD_Session *convSession_;

};
#endif
