/*=================================================================== */
/**
   @file   aes_ohi_directoryhandler_r1.h

   @brief Header file for aes_ohi_directoryHandler_r1 in OHI module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       21/06/2011   XCHEMAD     Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_AES_OHI_DIRECTORYHANDLER_R1_H_)
#define _AES_OHI_DIRECTORYHANDLER_R1_H_

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include "aes_ohi_eventcodes.h"
#include "aes_ohi_filestates.h"
#include "aes_ohi_filehandler_r2.h"

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_ohi_directoryHandler_r1

                 This class handles the directory transfer functionality
*/
/*=================================================================== */

class aes_ohi_directoryHandler_r1 : public aes_ohi_fileHandler_r1 
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

      @brief           Constructor

      @pre             none

      @post            none

      @param           subSys
                                Name of the Sub system.

      @param           appName
                                Name of the application block.

      @param           fileDest
                                Name of the file destination.

      @param           eventText
                                Event text used in the event generated for an alarm.

      @return          none

      @exception       none
   */
/*=================================================================== */

   aes_ohi_directoryHandler_r1(const char* subSys,
                               const char* appName,
                               const char* fileDest,
                               const char* eventText = "");

/*=================================================================== */
   /**

      @brief           Constructor

      @pre             none

      @post            none

      @param           subSys
                                Name of the Sub system.

      @param           appName
                                Name of the application block.

      @param           fileDest
                                Name of the file destination.

      @param           eventText
                                Event text used in the event generated for an alarm.

      @param           path
                                Physical path to the directory

      @return          none

      @exception       none
   */
/*=================================================================== */

   aes_ohi_directoryHandler_r1(const char* subSys,
                               const char* appName,
                               const char* fileDest,
                               const char* eventText,
                               const char* path);

/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Destructor

      @pre             none

      @post            none

      @return          none

      @exception       none
   */
/*=================================================================== */

   ~aes_ohi_directoryHandler_r1();

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Send a directory to the attached destination

      @pre         Attach should be called prior to send.

      @post        none

      @param       dirName
                         The name of the directory to be sent

      @param       sendFormat
                         When using a FTP destination the format can be one 	
			 of the following 
			 AES_OHI_BINARY or AES_OHI_ASCII

      @param       sendMask
                         Mask for which files in the directory the 
			 sendFormat applies

      @return      Error code

      @exception   none
    */
/*=================================================================== */

   unsigned int send(const char* dirName,
                     AES_OHI_Format sendFormat = AES_OHI_BINARY,
                     const char* sendMask = "");
};

#endif
