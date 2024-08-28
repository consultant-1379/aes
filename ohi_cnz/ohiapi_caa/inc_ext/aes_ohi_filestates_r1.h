/*=================================================================== */
/**
   @file   AES_OHI_FileStates_R1.h

   @brief Header file for File States in OHI module.

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
#if !defined(_AES_OHI_FILESTATES_R1_H_)
#define _AES_OHI_FILESTATES_R1_H_

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  aes_ohi_filestates_r1

           This Enum holds the file states of the OHI module.
*/
/*=================================================================== */

typedef enum
{
   AES_OHI_FSREADY = 0,			/*< File state ready */
   AES_OHI_FSSEND = 1,			/*< File state send */
   AES_OHI_FSARCHIVE = 2,		/*< File state archive */
   AES_OHI_FSDELETE = 3,		/*< File state delete */
   AES_OHI_FSFAILED = 4,		/*< File state failed */
   AES_OHI_FSUSEDEFAULT = 5,		/*< File state used default */
   AES_OHI_FSREMOVE = 6,		/*< File state remove */
   AES_OHI_FSSTOPPED = 7,		/*< File state stopped */
   AES_OHI_FSPENDING = 8,		/*< File state pending */
   AES_OHI_FSONFTPAREA = 9,		/*< File state no ftp area */
   AES_OHI_FSNONE = 255			/*< File state none */
} aes_ohi_filestates_r1;

#endif
