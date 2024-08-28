/*=================================================================== */
/**
	@file   aes_gcc_eventhandler.h

	@brief
	The services provided by AES_GCC_EventHandler facilitates the
	reporting of event to AEH.

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
#ifndef AES_GCC_EVENTHANDLER_H 
#define AES_GCC_EVENTHANDLER_H

#include "aes_gcc_eventhandler_r1.h"


typedef AES_GCC_EventHandler_R1 AES_GCC_EventHandler;

#endif // AES_GCC_EVENTHANDLER_H
