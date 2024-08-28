/*=================================================================== */
/**
	@file   cmdhandler.h

	@brief
	The services provided by CmdHandler facilitates the
	handling of a commands.

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       19/09/2011     xbhadur       Initial Release
 ============================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
 ==================================================================== */
#ifndef CMDHANDLER_H 
#define CMDHANDLER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */

#include <aes_cdh_resultcode.h>
#include <ACS_APGCC_Command.H>
#include <ACS_APGCC_DSD.H>
#include <ace/Auto_Event.h> 
#include <ace/Task.h>
#include <ace/Thread_Mutex.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */
/*===================================================================*/
/**
		@brief CmdHandler
	The services provided by CmdHandler facilities handling of commands
 */
/*=================================================================== */

class CmdHandler : public ACE_Task_Base
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

		  @brief           Class constructor

		  @return          none

		  @exception       none
	    */
	/*=================================================================== */
	CmdHandler(int& stopEvent);

	/*=================================================================== */
		/**

			@brief           Class destructor

			@return          none

		    @exception       none
		 */
	/*=================================================================== */
	virtual ~CmdHandler();

	/*=================================================================== */
	/**

		      @brief		open
								Hook method for spawning the svc thread.

			  @param        none

		      @return		ACE_INT32 Return value depending on status

		      @exception	none
	 */
	/*=================================================================== */
	virtual int open();

	/*=================================================================== */
	/**

			@brief			shutdown
							This method does the shutdown for cmdhandler thread.

			@return			bool 

			@exception		none
	 */
	/*=================================================================== */
	bool shutdown();

	/*=================================================================== */
	/**

		      @brief		close
							closes the session.

			  @param           	flags
								flag.

		      @return           ACE_INT32 Return value depending on status

		      @exception       none
	 */
	/*=================================================================== */
	virtual int close(u_long);

	/*=================================================================== */
	/**

		      @brief		svc
							starts service.

		      @return           ACE_INT32 Return value depending on status

		      @exception       none
	 */
	/*=================================================================== */
	virtual ACE_INT32 svc();

	/*=================================================================== */
	/**

		      @brief		sendRecordFileReply

			  @param           	result
								result..
			  @param           	fileName
								Name of the file.
			  @param           	recordsSent
								Number of records sent.

		      @return           ACE_INT32 Return value depending on status

		      @exception       none
	 */
	/*=================================================================== */
    ACE_INT32 sendRecordFileReply( AES_CDH_ResultCode result,
                             const std::string &fileName, 
                             ACE_UINT64 recordsSent);

    /*=================================================================== */
    /**

    	      @brief		sendFileReply
    						sendFileReply.

    		  @param           	result
    							Result.
    		  @param           	destName
    							Destination name.
		      @param           	fileName
    							Name of the file.
    		  @param           	newFileName
    							Name of the new file.
		      @param           	newSubFileName
    							Name of teh new sub file name.
    		  @param           	pid
    							Process id.

    		  @param           	destObj
    							destination object.

    	      @return           ACE_INT32 Return value depending on status

    	      @exception       none
     */
    /*=================================================================== */
    ACE_INT32 sendFileReply(AES_CDH_ResultCode result,
                      const std::string &destName, 
                      const std::string &fileName, 
                      const std::string &newFileName,
                      const std::string &newSubFileName,
                      ACE_INT32 pid, ACE_INT32 destObj);

    /*=================================================================== */
    /**

    	      @brief		sendDestEvent
    						send destination event.

    		  @param           	result
    							Result.
    		  @param           	destName
    							Destiantion name.

    	      @return           ACE_INT32 Return value depending on status

    	      @exception       none
     */
    /*=================================================================== */
    ACE_INT32 sendDestEvent(AES_CDH_ResultCode result,
                      const std::string &destName);

    /*=================================================================== */
    /**

    	      @brief		getTicks
    						To get number of ticks.

    	      @return           ACE_INT32 Return value depending on status

    	      @exception       none
     */
    /*=================================================================== */
    ACE_UINT64 getTicks();

    /*=================================================================== */

    /*=================================================================== */
    /**

    	         @brief  This method get the DSD stream

     */
    /*=================================================================== */
    ACS_APGCC_DSD_Stream& getStream() { return m_cmdStreamIO;}

    /*===================================================================

					 PRIVATE SECTION
	=================================================================== */
private:
    /*=================================================================== */
    	/**
    			  @brief   stopEvent_
    						   shutdown event notification
    	 */
    /*=================================================================== */
		int stopEvent_;

	/*=================================================================== */
        /**
            @brief  m_cmdStreamIO : DSD stream object
         */
	/*=================================================================== */
        ACS_APGCC_DSD_Stream m_cmdStreamIO;

    /*=================================================================== */
        /**
                    @brief  m_cmdStreamIO : DSD stream object
         */
    /*=================================================================== */
        ACS_APGCC_Command cmd;

	/*=================================================================== */
		/**
				  @brief   sessionIsOpen
						   Detailed description
		 */
	/*=================================================================== */
		bool sessionIsOpen;

	/*=================================================================== */
	   /**
			  @brief   hSendMX
					   Detailed description
	    */
	/*=================================================================== */
	  ACE_Recursive_Thread_Mutex hSendMX;

	/*=================================================================== */
	   /**
			  @brief   ticks
					   Used as an extra identifier to distinguish CmdHandler objects having the same (CmdHandler *) pointer value.
	    */
	/*=================================================================== */
	  ACE_UINT64 ticks;

	/*=================================================================== */
	  /**

			  @brief		sendReply
							To send reply.

			  @param           	cmd
								command.

			  @return           ACE_INT32 Return value depending on status

			  @exception       none
	    */
	 /*=================================================================== */
	  ACE_INT32 sendReply(ACS_APGCC_Command& cmd);

	 /*=================================================================== */
};

#endif
