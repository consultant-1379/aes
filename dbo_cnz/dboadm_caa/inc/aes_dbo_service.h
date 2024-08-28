/*=================================================================== */
/**
	@file   aes_dbo_service.h

	@brief
	This class is used to start and stop the DBO service

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
#ifndef AES_DBO_SERVICE_H
#define AES_DBO_SERVICE_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
// ACE header files
#include <ace/TP_Reactor.h>


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_DBO_Service

                 This class is used to start and stop the DBO service
 */
/*=================================================================== */
class AES_DBO_Service
{
public:
	AES_DBO_Service();
	~AES_DBO_Service();
	//bool setupIMMCallBacks(ACE_Reactor *);

private:

};

#endif
