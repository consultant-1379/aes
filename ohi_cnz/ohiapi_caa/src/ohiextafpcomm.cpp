/*=================================================================== */
	/**
	@file   ohiextafpcomm.cpp

	@brief
	Extended file based protocol
	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       26/08/2011     XNADNAR       Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */

#include <string>
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_extfilehandler2_r1.h"
#include "aes_ohi_extfilehandlerimplementation2.h"
#include "ohiextafpcomm.h"

/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
ohiExtAfpComm::ohiExtAfpComm(aes_ohi_extFileHandlerImplementation2* userFile)
{	
   theUserFile = userFile;
}
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
ohiExtAfpComm::~ohiExtAfpComm()
{
}
/*===================================================================
                    ROUTINE: event
=================================================================== */
unsigned int ohiExtAfpComm::event(AES_GCC_Eventcodes& afpEventCode)
{
   // send event to AES_OHI_File
	return theUserFile->event((AES_OHI_Eventcodes)afpEventCode);
}
