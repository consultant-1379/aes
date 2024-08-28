/*=================================================================== */
/**
	@file   aes_gcc_eventcodes_r1.h

	@brief
	Provides the event codes for various events.
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       22/06/2011     XNADNAR       Initial Release
==================================================================== */
#ifndef aes_gcc_eventcodes_r1_h
#define aes_gcc_eventcodes_r1_h 1

typedef enum
{
    //Lost connection to GOH Servers (afp server)
    AES_EVELOSTSERVER = 0,
        
    //File transfere failed (alarm is raised )
    AES_EVEFILETRANSFERFAILED =1,
    
    //File transfere recover (alarm is cease )
    AES_EVEFILETRANSFERFAILEDCEASE = 2,
    
    //Link down (alarm is raised )
    AES_EVELINKDOWN = 3,
    
    //Link up (alarm is seized )
    AES_EVELINKDOWNCEASE = 4, 
    
    //Reaching above buffer warning limit
    AES_EVEBUFFERWARNING = 5,
    
    //Going below buffer warning limit
    AES_EVEBUFFERWARNINGCEASE = 6,
    
    //Reaching above buffer limit
    AES_EVEBUFFERLIMIT = 7,
    
    //Going below buffer limit
    AES_EVEBUFFERLIMITCEASE = 8,

    AES_EVEBLOCKTRANSFERFAILED = 10,

    // Destination set down
    AES_EVEDESTSETDOWN = 12,

    // Destination set up
    AES_EVEDESTSETDOWNCEASE = 13,

    // Shutdown mirror
    AES_EVMIRROROFF = 100,

    // Startup mirror
    AES_EVMIRRORON = 101,

    // It is an API event. This is the API event bit
    AES_APIEVENT = 2147483648
    
} AES_GCC_Eventcodes_R1;

#endif
