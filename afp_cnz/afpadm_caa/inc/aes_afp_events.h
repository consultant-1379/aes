/*=================================================================== */
/**
   @file   	aes_afp_events_h

   @brief		This file holds enum of event numbers
			    and Event class to report an event to AEH
   @version 1.0.0
 */
/*
   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A      22/09/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef aes_afp_events_h
#define aes_afp_events_h 1
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <aes_gcc_eventhandler.h>

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  Class for event numbers
 */
/*=================================================================== */

typedef enum
{
	// from 29000 to 29099
	EVENT0				= 29000,
			EVENT_CREATEROOT		= 29001,	// can not create rootdir
			EVENT_STARTDATA		= 29002,	// can not strart datatask
			EVENT_STARTAPCOM		= 29003,	// can not start apcom
			EVENT_STARTCPCOM		= 29004,	// can not start cpcom
			EVENT_CPCOMACCEPT		= 29005,	// can not do jtp accept
			EVENT_CPCOMPUBLISH	= 29006,	// can not publish to jtp
			EVENT_FILENOTDEL		= 29007,	// A file could not be deleted.
			EVENT_NOACCESSAFP		= 29008,	// No access to AFP filesystem.
			EVENT_FILEPROTECT		= 29009,	// Not able to protect file.
			EVENT_DESTPROTECT		= 29010,	// Not able to protect destination.
			EVENT_DBASEINIT		= 29011,	// Not able to initiate database.
			EVENT_PUBLISHDSD		= 29012,	// Not able to publish server to DSD.
			EVENT_NOACCESSDIR		= 29013,	// Not able to access directory in afp filesystem.
			EVENT_RESERVEFILE		= 29014,	// Not able to reserve file in CPF.
			EVENT_UNRESERVEFILE	= 29015,	// Not able to unreserve file in CPF.
			EVENT_RENAMEFILE		= 29016,	// Not able to rename file locally.
			EVENT_DESTUNPROTECT	= 29017,	// Not able to unprotect destination.
			EVENT_TIMECHANGED		= 29018,	// Time has been changed backwards.
			EVENT_FILENOTOPEN		= 29019,	// Not able to open file.
			EVENT_FILENOTWRITE	= 29020,	// Not able to write file.
			EVENT_FILENOTMOVE		= 29021,	// Not able to move file.
			EVENT_FILENOTFOUND	= 29022,	// Not able to find file.
			EVENT_FILENOTRENAME	= 29023,	// Not able to rename file.
			EVENT_FILENOTCOPY	= 29024,	// Not able to copy file.
			EVENT_TRANSERROR		= 29025,	// Event transfer of a file failed.
			EVENT_SENDITEMCORRUPT = 29026,    // Senditem file corrupted


			ALARM_TRANSFAILED		= 29090,	// Transfer of a file failed.

			EVENTUNKNOWN			= 29099		// Unknown event

} aes_afp_events;

/*=====================================================================
                           CONSTANT DECLARATION SECTION
   ==================================================================== */
/*=================================================================== */
/**
      @brief 		AES_AFP_processName

					DSD server name of the AFP
 */
/*=================================================================== */
const std::string AES_AFP_processName = "AES_AFP_Server";
//const std::string AES_AFP_objClassAPZ = "APZ";

/*=====================================================================
                          CLASS DECLARATION SECTION
  ==================================================================== */
/*=================================================================== */
/**
     @brief 		Event

					Class to raise an eventS
 */
/*=================================================================== */
class  Event
{
	/*=====================================================================
	                         PUBLIC DECLARATION SECTION
	  ==================================================================== */
public:
	/*=================================================================== */
	/**

		@brief           	report
							Reports an event to AEH server
		@param				specificProblem
							specificProblem
		@param				probableCause
							probableCause
		@param				problemData
							problemData
		@param				problemText
							problemText
		@return          	void
		@exception       	none
	 */
	/*=================================================================== */
	static void report(aes_afp_events specificProblem, 
			const std::string& probableCause,
			const std::string& problemData,
			const std::string& problemText);

};

#endif
