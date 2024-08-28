/*================================================================== */
/**
   @file   aes_cdh_cmdclient.h

   @brief Header file for AES_CDH_CmdClient type module.

          Handles communication with AES_CDH_Server using DSD

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011   XCHEMAD    Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_CMDCLIENT_H 
#define AES_CDH_CMDCLIENT_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ACS_APGCC_Command.H>
#include <ACS_APGCC_Cmd_Client.H>
#include <ACS_APGCC_DSD.H>
#include <ACS_APGCC_OS.H> 

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_CmdClient

                 Command client
*/
/*=================================================================== */
class  AES_CDH_CmdClient
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

      @brief           AES_CDH_CmdClient

                       Class constructor

      @param	       addr
			ACS_APGCC_DSD_Addr reference

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_CmdClient(const ACS_APGCC_DSD_Addr& addr);

/*=================================================================== */
   /**

	  @brief           AES_CDH_CmdClient

					   Class constructor

	  @param	       addr
			ACS_APGCC_DSD_Addr reference

	  @return          none

	  @exception       none
   */
/*=================================================================== */
	AES_CDH_CmdClient(const char* serviceName);

/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Class destructor

      @return          none

      @exception       none
   */
/*=================================================================== */
    ~AES_CDH_CmdClient();

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       This method is used to connect to the server

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    int connect(void);

/*=================================================================== */
   /**
      @brief       This method is used to send command object to the server

      @param	   cmd
		    	Command object

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    int send(ACS_APGCC_Command& cmd);
    
/*=================================================================== */
   /**
      @brief       This method is used to receive command object to the server

      @param       cmd
                        Command object

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    int receive(ACS_APGCC_Command& cmd);

/*=================================================================== */
   /**
      @brief       This method is used to get handle

      @return      ACE handle

      @exception   none
    */
/*=================================================================== */
    ACE_HANDLE getHandle(void); 

/*=================================================================== */
   /**
      @brief       This method is used to close connection from server

      @return      none

      @exception   none
    */
/*=================================================================== */
    void close(void);

/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:

/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */
    ACS_APGCC_DSD_Addr m_address;

/*=================================================================== */
   /**
      @brief   stream_

               ACS_APGCC_DSD_Stream object
   */
/*=================================================================== */
    ACS_APGCC_DSD_Stream    stream_;

/*=================================================================== */
   /**
      @brief   server_

               ACS_APGCC_DSD_Connector object
   */
/*=================================================================== */
    ACS_APGCC_DSD_Connector server_;

};

#endif // AES_CDH_CMDCLIENT_H
