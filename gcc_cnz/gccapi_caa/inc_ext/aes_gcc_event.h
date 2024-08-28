/*=================================================================== */
/**
	@file   aes_gcc_event.h

	@brief
	The services provided by AES_GCC_Event facilitates the
	creation of an event to be sent to AES_GCC_EventHandler.

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.
	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       22/06/2011     XNADNAR       Initial Release
==================================================================== */

#ifndef AES_GCC_EVENT_H 
#define AES_GCC_EVENT_H

#include "aes_gcc_event_r1.h"

typedef AES_GCC_Event_R1 AES_GCC_Event;

#endif
