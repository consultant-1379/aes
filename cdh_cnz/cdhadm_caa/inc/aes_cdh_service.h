/*=================================================================== */
/**
	@file   aes_cdh_service.h

	@brief
	This class is used to start and stop the CDH service

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
   N/A       05/10/2011    XCHEMAD    Initial Release
============================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_SERVICE_H
#define AES_CDH_SERVICE_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
// ACE header files
#include <ace/TP_Reactor.h>
#include <acs_aeh_signalhandler.h>
#include <stdio.h>
#include <ace/ACE.h>

#define AES_CDH_PROCESS_NAME "aes_cdhd"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_Service

                 This class is used to start and stop the CDH service
 */
/*=================================================================== */
class AES_CDH_Service
{
public:
	AES_CDH_Service();
	~AES_CDH_Service();
};

#endif
