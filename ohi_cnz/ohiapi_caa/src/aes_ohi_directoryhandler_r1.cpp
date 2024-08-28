/*=================================================================== */
   /**
   @file aes_ohi_directoryhandler_r1.cpp

   Class method implementationn for OHI type module.

   This module contains the implementation of class declared in
   the aes_ohi_directoryhandler_r1.h module

   @version 1.0.0

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/06/2011    XCHEMAD   Initial Release
   **/
/*=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <string>
#include "aes_ohi_eventcodes.h"
#include "aes_ohi_errorcodes.h"
#include "aes_ohi_filehandler_r2.h"
#include "aes_ohi_directoryhandler_r1.h"	
#include "aes_ohi_filehandlerimplementation.h"	

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE: aes_ohi_directoryHandler_r1
=================================================================== */

aes_ohi_directoryHandler_r1::aes_ohi_directoryHandler_r1(const char* subSys,
                                                         const char* appName,
                                                         const char* fileDest,
                                                         const char* eventText):
aes_ohi_fileHandler_r1(subSys, appName, fileDest, eventText)
{
}

/*===================================================================
   ROUTINE: aes_ohi_directoryHandler_r1
=================================================================== */
aes_ohi_directoryHandler_r1::aes_ohi_directoryHandler_r1(const char* subSys,
                                                         const char* appName,
                                                         const char* fileDest,
                                                         const char* eventText,
                                                         const char* path):
aes_ohi_fileHandler_r1(subSys, appName, fileDest, eventText, path)
{
}

/*===================================================================
   ROUTINE: ~aes_ohi_directoryHandler_r1
=================================================================== */
aes_ohi_directoryHandler_r1::~aes_ohi_directoryHandler_r1()
{
}

/*===================================================================
   ROUTINE: send
=================================================================== */
unsigned int aes_ohi_directoryHandler_r1::send(const char* dirName,
                                               AES_OHI_Format sendFormat,
                                               const char* sendMask)
{
   return implementation->send(dirName, sendFormat, sendMask, true);
}
