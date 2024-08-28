/*=================================================================== */
	/**
	@file   ohiafpcomm.cpp

	@brief
	A wrapper to aes_afp_api
	HISTORY

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       26/08/2011     XNADNAR       Initial Release
==================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include "aes_gcc_eventcodes.h"
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_afp_api.h"
#include "aes_ohi_filehandlerimplementation.h"
#include "ohiafpcomm.h"

/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
ohiAfpComm::ohiAfpComm(aes_ohi_fileHandlerImplementation* userFile)
{
   theUserFile = userFile;
}

/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
ohiAfpComm::~ohiAfpComm()
{
   theUserFile = NULL;
}

/*===================================================================
                    ROUTINE: event
=================================================================== */
unsigned int ohiAfpComm::event(AES_GCC_Eventcodes& afpEventCode)
{
   // send event to AES_OHI_File
   return theUserFile->event((AES_OHI_Eventcodes)afpEventCode);
}
