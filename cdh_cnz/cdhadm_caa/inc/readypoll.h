/*=================================================================== */
/**
	@file   readypoll.h

	@brief
	The services provided by ReadyPoll facilitates polling of data.

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
   N/A       27/09/2011     xbhadur       Initial Release
============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef READYPOLL_H
#define READYPOLL_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <transferagent.h>
#include <ftprv2keep.h>
#include <aes_cdh_resultcode.h>
#include <aes_cdh_destinationset.h>
#include <database.h>
#include <destinationalarm.h>
#include <transdest.h>
#include <string>
#include <vector>
#include <time.h>
#include <ace/ACE.h>
#include <ace/Task.h>
#include <ace/Monotonic_Time_Policy.h>
#include <ace/Event.h>
#include <ace/Time_Value.h>
#include <aes_gcc_util.h>
#include <parameter.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ReadyPoll

                 The services provided by ReadyPoll facilitates polling of data.
 */
/*=================================================================== */
class ReadyPoll : public ACE_Task<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
		     	                        CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

		   	      @brief          ReadyPoll
		   						  ReadyPoll constructor.

		   		  @return          void

		   	      @exception       none
	 */
	/*=================================================================== */
	ReadyPoll (std::string pollDest);

	/*=====================================================================
		     	                        CLASS DESTRUCTOR
		==================================================================== */
	/*=================================================================== */
	/**

		   	      @brief          ~ReadyPoll
		   						  ReadyPoll destructor.

		   		  @return          void

		   	      @exception       none
	 */
	/*=================================================================== */
	virtual ~ReadyPoll ();

	/*=================================================================== */
	/**

						    	      @brief          open
						    						  -

						    	      @return         ACE_INT32			-

						    	      @exception      none
	 */
	/*=================================================================== */
	int open(void *args = 0);
	/*=================================================================== */
	/**

						    	      @brief          svc
						    						  -

						    	      @return         ACE_INT32 	-

						    	      @exception      none
	 */
	/*=================================================================== */
	int svc(void);
	/*=================================================================== */
	/*=================================================================== */
	void shutdown(void);
	/**

						    	      @brief          readCDHReadyPollParams
						    						  -

						    	      @return         void

						    	      @exception      none
	 */
	/*=================================================================== */
	void  readCDHReadyPollParams();	
	/*=================================================================== */
	/**

						    	      @brief          setCDHReadyPollParams
						    						  -

													  -
						      		  @param          pollDest
													  -
									  @param          transPtr
													  -

						    	      @return         void

						    	      @exception      none
	 */
	/*=================================================================== */
	void setCDHReadyPollParams(TransferAgent* transPtr);
	/*=================================================================== */
	/*=================================================================== */
	/**

						    	      @brief          directoryPolling
						    						  -

						    	      @return         void

						    	      @exception      none
	 */
	/*=================================================================== */
	void directoryPolling(void);
	/*=================================================================== */
	/**

						    	      @brief          raiseVdPollAlarm
						    						  -

						    	      @return         void

						    	      @exception      none
	 */
	/*=================================================================== */
	void raiseVdPollAlarm();
	/*=================================================================== */
	/**

						    	      @brief          ceaseVdPollAlarm
						    						  -

						    	      @return         void

						    	      @exception      none
	 */
	/*=================================================================== */
	void ceaseVdPollAlarm();
	/*=================================================================== */
	/**

						    	      @brief          vdAlarmStatus

						    	      @return         true		on success
													  false     on unsuccessful

						    	      @exception      none
	 */
	/*=================================================================== */
	bool vdAlarmStatus();
	/*=================================================================== */
	/**

						    	      @brief         validatePollParam 

						    	      @return         true on success
									      false on unsuccessful

						    	      @exception      none
	 */
	/*=================================================================== */
	bool validatePollParams();
	private:
	/*====================================================================
				                       PRIVATE ATTRIBUTES
	==================================================================== */

	/*=================================================================== */
	/**
					      @brief   m_actVdDestName
								   -
	 */
	/*=================================================================== */
	std::string m_actVdDestName;
	/*=================================================================== */
	/**
						      @brief   m_emptyDestSet
									   -
	 */
	/*=================================================================== */
	std::string m_emptyDestSet;
	/*=================================================================== */
	/**
						      @brief   m_pollFrequency
									   -
	 */
	/*=================================================================== */
	ACE_UINT64 m_pollFrequency;
	/*=================================================================== */
	/**
						      @brief   m_oldestFileDate
									   -
	 */
	/*=================================================================== */
	ACE_UINT64 m_oldestFileDate;
	/*=================================================================== */
	/**
						      @brief   m_maxFileNumber
									   -
	 */
	/*=================================================================== */
	ACE_UINT32 m_maxFileNumber;
	/*=================================================================== */
	/**
						      @brief   m_oldestFileTimeStamp
									   -
	 */
	/*=================================================================== */
	ACE_UINT32 m_oldestFileTimeStamp;
	/*=================================================================== */
	/**
				      @brief   m_currentNoOfFiles
							   -
	 */
	/*=================================================================== */
	ACE_UINT32 m_currentNoOfFiles;
	/*=================================================================== */
	/**
				      @brief   m_readyDirPath
							   -
	 */
	/*=================================================================== */
	std::string m_readyDirPath;
	/*=================================================================== */
	/**
				      @brief   m_isReadyDirAlarmActive
							   -
	 */
	/*=================================================================== */
	bool m_isReadyDirAlarmActive;
	/*=================================================================== */
	/*=================================================================== */
	/**
				      @brief   exitThread_
							   -
	 */
	/*=================================================================== */
	bool exitThread_;
	ACE_Event m_pollEv;

};

#endif

