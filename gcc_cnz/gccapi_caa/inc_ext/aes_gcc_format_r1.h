/*===================================================================== */
	/**
	@file aes_gcc_format_r1.h

	DESCRIPTION
		Provides with various formats of the file transfer.

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       09/08/2011    xnadnar      Initial Release
	*/
/* =================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef aes_gcc_format_r1_h
#define aes_gcc_format_r1_h 1

typedef enum
   {
       AES_BINARY = 0,
       AES_ASCII = 1,
       AES_DEFAULT = 2
   } aes_gcc_format_r1;

#endif
