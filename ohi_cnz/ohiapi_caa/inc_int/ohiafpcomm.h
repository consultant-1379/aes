/*=================================================================== */
	/**
	@file   ohiafpcomm.h

	@brief

	A wrapper to aes_afp_api to implement the call back event

	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       26/08/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef OHIAFPCOMM_H
#define OHIAFPCOMM_H
/*=====================================================================
                          INCLUDE SECTION
==================================================================== */
#include <string>
#include "aes_ohi_filehandlerimplementation.h"
#include "aes_gcc_eventcodes.h"
#include "aes_gcc_errorcodes.h"
#include "aes_afp_api.h"

/*=====================================================================
                          FORWARD DECLARATION SECTION
==================================================================== */
class aes_ohi_fileHandlerImplementation;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
class ohiAfpComm :public AES_AFP_Api
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

		  @brief           ohiAfpComm
						   Creates an ohiAfpComm object

		  @param           userFile
						   Reference of aes_ohi_fileHandlerImplementation object.

		  @return          void

		  @exception       none
	 */

/*=================================================================== */
    ohiAfpComm(aes_ohi_fileHandlerImplementation* userFile);
/*=====================================================================
							CLASS DESTRUCTOR
==================================================================== */
/*=================================================================== */
	   /**

		  @brief           ohiAfpComm
						   Destroys an ohiAfpComm object

		  @return          void

		  @exception       none
	 */

/*=================================================================== */
	~ohiAfpComm();
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
	unsigned int virtual event(AES_GCC_Eventcodes& afpEventCode);

/*=====================================================================
						  PRIVATE DECLARATION SECTION
==================================================================== */
private:
/*=================================================================== */
	   /**

		  @brief           theUserFile
						   Reference of aes_ohi_fileHandlerImplementation class.
	 */

/*=================================================================== */
	 aes_ohi_fileHandlerImplementation*	theUserFile;
};

#endif
