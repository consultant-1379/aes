/*=================================================================== */
/**
   @file   AES_OHI_Format_R1.h

   @brief Header file for Formats in OHI module.

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
#if !defined(_AES_OHI_FORMAT_R1_H_)
#define _AES_OHI_FORMAT_R1_H_

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  aes_ohi_format_r1

           This Enum holds the formats of the OHI module.
*/
/*=================================================================== */

typedef enum
{
   AES_OHI_BINARY = 0,		/*< Binary format */
   AES_OHI_ASCII = 1,		/*< Ascii format */
   AES_OHI_DEFAULT = 2		/*< Default format */
} aes_ohi_format_r1;

#endif
