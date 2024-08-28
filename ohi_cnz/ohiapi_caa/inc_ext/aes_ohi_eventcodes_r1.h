/*=================================================================== */
/**
   @file   AES_OHI_EventCodes_R1.h

   @brief Header file for Event codes in OHI module.

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
#if !defined(_AES_OHI_EVENTCODES_R1_H_)
#define _AES_OHI_EVENTCODES_R1_H_

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  aes_ohi_eventcodes_r1

           This Enum holds the event codes of the OHI module.
*/
/*=================================================================== */

typedef enum
{
   AES_OHI_EVELOSTSERVER = 0,			/*< Lost connection to GOH Servers (afp server) */

   AES_OHI_EVEFILETRANSFERFAILED =1,		/*< File transfer failed (alarm is raised ) */

   AES_OHI_EVEFILETRANSFERFAILEDCEASE = 2,	/*< File transfer recover (alarm is cease ) */

   AES_OHI_EVELINKDOWN = 3,			/*< Link down (alarm is raised ) */

   AES_OHI_EVELINKDOWNCEASE = 4, 		/*< Link up (alarm is seized ) */

   AES_OHI_EVEBUFFERWARNING = 5,		/*< Reaching above buffer warning limit */

   AES_OHI_EVEBUFFERWARNINGCEASE = 6,		/*< Going below buffer warning limit */

   AES_OHI_EVEBUFFERLIMIT = 7,			/*< Reaching above buffer limit */

   AES_OHI_EVEBUFFERLIMITCEASE = 8,		/*< Going below buffer limit */

   AES_OHI_EVEREQUESTPATH = 9,			/*< Request path for file */

   AES_OHI_EVEBLOCKTRANSFERFAILED =10		/*< Block transfer failed */

} aes_ohi_eventcodes_r1;

#endif
