/* =================================================================== */
	/**
	@file aes_gcc_filestates_r1.h

	DESCRIPTION
		Provides with the various file states.

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       10/08/2011    xnadnar      Initial Release
	*/
/* =================================================================== */

#ifndef aes_gcc_filestates_r1_h
#define aes_gcc_filestates_r1_h 1

typedef enum
     {
          AES_FSREADY = 0,
          AES_FSSEND = 1,
          AES_FSARCHIVE = 2,
          AES_FSDELETE = 3,
          AES_FSFAILED = 4,
          AES_FSUSEDEFAULT = 5,
          AES_FSREMOVE = 6,
          AES_FSSTOPPED = 7,
          AES_FSPENDING = 8,
          AES_FSONFTPAREA = 9,
          AES_FSNONE = 255
     } aes_gcc_filestates_r1;
#endif
