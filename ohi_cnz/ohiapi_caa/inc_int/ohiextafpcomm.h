/*=================================================================== */
	/**
	@file   ohiafpcomm.h

	@brief

	Class for general purpose usage for filebased destinations
	ie Inquire,
	The sequence of operations are:
	- attach()
	- Inquire
	- detach()

	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       26/08/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef OHIEXTAFPCOMM_H
#define OHIEXTAFPCOMM_H
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_filestates.h"
#include "aes_ohi_format.h"
#include "aes_ohi_extfilehandlerimplementation2.h"
#include "aes_afp_api.h"
/*=====================================================================
                          FORWARD DECLARATION SECTION
==================================================================== */
class aes_ohi_extFileHandlerImplementation2;
/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
class ohiExtAfpComm :public AES_AFP_Api
{
/*=====================================================================
						  PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*=====================================================================
						  CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
	   /**

		  @brief           ohiExtAfpComm
						   Creates an ohiExtAfpComm object

		  @param           userFile
						   Reference of aes_ohi_extFileHandlerImplementation2 object.

		  @return          void

		  @exception       none
	 */

/*=================================================================== */
   ohiExtAfpComm(aes_ohi_extFileHandlerImplementation2* userFile);
/*=====================================================================
					  CLASS DESTRUCTOR
==================================================================== */
/*=================================================================== */
   /**

	  @brief           ohiExtAfpComm
					   Destroys an ohiExtAfpComm object
	  @return          void

	  @exception       none
 */

/*=================================================================== */
   ~ohiExtAfpComm();
/*=================================================================== */
   /**

	  @brief           event
					   Raises an event related to the particular event code.

	  @param           afpEventCode
					   Reference of AES_GCC_Eventcodes

	  @return          status of raising event

	  @exception       none
 */

/*=================================================================== */
   virtual unsigned int event(AES_GCC_Eventcodes& afpEventCode);

/*=====================================================================
					  PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
   /**

	  @brief           theUserFile
					   Reference of aes_ohi_extFileHandlerImplementation2 class.
 */

/*=================================================================== */
   aes_ohi_extFileHandlerImplementation2*	theUserFile;
};

#endif
