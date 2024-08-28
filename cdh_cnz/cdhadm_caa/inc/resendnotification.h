/*=================================================================== */
/**
	@file   resendnotification.h

	@brief
	This class will check that all files that are reported to
	a remote host with file notification also is fetched. If the
	file is not fetched in a certatin number of days, a new file
	notification is sent to remind the remote host to fetch the file.

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
#ifndef RESENDNOTIFICATION_H
#define RESENDNOTIFICATION_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <aes_cdh_resultcode.h>
#include <database.h>
#include <notificationrecord.h>
#include <aes_cdh_resultimplementation.h>
#include <ace/ACE.h>
#include <ace/Task.h>
#include <ace/Monotonic_Time_Policy.h>
#include <list>
#include <string>

using namespace std;
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ResendNotification

                 This class will check that all files that are reported to
				 a remote host with file notification also is fetched.
 */
/*=================================================================== */
class ResendNotification : public ACE_Task<ACE_MT_SYNCH, ACE_Monotonic_Time_Policy>
{
	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*=====================================================================
	                           PRIVATE ATTRIBUTES
	==================================================================== */
	/*=====================================================================
	                        ENUMERATED DECLARATION SECTION
	==================================================================== */
	/*=================================================================== */
	/**
	   @brief  WorkingState

	           -
	 */
	/*=================================================================== */
	enum WorkingState
	{
		WORKING_START,
		WORKING_RUNNING,
		WORKING_OFF
	};

	/*=====================================================================
                           PUBLIC DECLARATION SECTION
   ==================================================================== */
public:
	/*=====================================================================
   	     	                        CLASS CONSTRUCTOR
   	==================================================================== */
	/*=================================================================== */
	/**

   	   	      @brief          ResendNotification
   	   						  ResendNotification constructor.

   	   		  @return          void

   	   	      @exception       none
	 */
	/*=================================================================== */
	ResendNotification(void);
	/*=====================================================================
		 	     	                        CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

		 	   	      @brief          ~ResendNotification
		 	   						  ResendNotification destructor.

		 	   		  @return          void

		 	   	      @exception       none
	 */
	/*=================================================================== */
	~ResendNotification(void);

	/*===================================================================
		 	                      ROUTINE DECLARATION SECTION
	=================================================================== */
	/*=================================================================== */
	/**

						    	      @brief          open
						    						  Starts the thread

						    		  @param          thrmgr
													  -

						    	      @return         ACE_INT32		-

						    	      @exception      none
	 */
	/*=================================================================== */
	ACE_INT32 open(ACE_Thread_Manager *thrmgr);

	/*=================================================================== */
	/**

							    	      @brief          close
							    						  Close the thread. This function is not called,
														  it runs automatically when svc() returns with 0.

							    		  @param          flags
														  -

							    	      @return         ACE_INT32		-

							    	      @exception      none
	 */
	/*=================================================================== */
	ACE_INT32 close(ACE_UINT64 flags);

	/*=================================================================== */
	/**

							    	      @brief          svc
							    						  Main function. Runs while thread is alive.

							    	      @return         ACE_INT32

							    	      @exception      none
	 */
	/*=================================================================== */
	ACE_INT32 svc(void);

	/*=================================================================== */
	/**

							    	      @brief          define
							    						  Defines parameters needed.

							    	      @return         AES_CDH_ResultCode		-

							    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode define(void);

	/*=================================================================== */
	/**

							    	      @brief          changeAttr
							    						  Changes defined parameters.

							    		  @param          notifAddress
														  -
							      		  @param          notifPort
														  -

							    	      @return         AES_CDH_ResultCode

							    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode changeAttr(std::string &notifAddress,
			std::string &notifPort);

	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*=================================================================== */
	/**

							    	      @brief          toUpper
							    						  Change a string to upper case letters.

							    		  @param          str
														  -

							    	      @return         void

							    	      @exception      none
	 */
	/*=================================================================== */
	void toUpper(string &str);

	/*=================================================================== */
	/**

							    	      @brief          readIPaddrFromFile
							    						  Temporarily solution to read cluster address from file.

							    		  @param          IPaddress
														  -

							    	      @return         true		on success
														  false     on unsuccessful

							    	      @exception      none
	 */
	/*=================================================================== */
	bool readIPaddrFromFile(string &IPaddress);

	/*=================================================================== */
	/**

							    	      @brief          sendToDataBase
							    						  Resend notification

							    		  @param          fileName
														  -
							      		  @param          destName
														  -
										  @param          fullPath
														  -
							    	      @param          storePath
														  -
							      		  @param          hostAddress
														  -
										  @param          maxAge
														  -

							    	      @return         true		on success
														  false     on unsuccessful

							    	      @exception      none
	 */
	/*=================================================================== */
	//
	bool sendToDataBase(const string& fileName,
			const string& destName,
			const string& fullPath,
			const string& storePath,
			const string& hostAddress,
			const ACE_UINT32 maxAge = 0);
	/*=================================================================== */
	/**

							    	      @brief          getFileSize
							    						  Calculate size of file/dir

							    		  @param          path
														  -
							      		  @param          fName
														  -
										  @param          errors
														  -

							    	      @return         ACE_UINT64	-

							    	      @exception      none
	 */
	/*=================================================================== */
	ACE_UINT64 getFileSize(const string& path,
			const string& fName,
			ACE_INT32& errors);
	/*=================================================================== */
	/**

							    	      @brief          Work
							    						  The main working function in the thread

							    	      @return         void

							    	      @exception      none
	 */
	/*=================================================================== */
	void Work(void);

	/*=================================================================== */
	/**

							    	      @brief          CheckDir
							    						  Check if "Ready" dir has any old files

							    		  @param          destName
														  -
							      		  @param          fullPath
														  -
										  @param          storePath
														  -
										  @param          hostAddress
														  -
							      		  @param          maxAge
														  -
										  @param          checkSend
														  -

							    	      @return         void

							    	      @exception      none
	 */
	/*=================================================================== */
	void CheckDir(const string& destName,
			const string& fullPath,
			const string& storePath,
			const string& hostAddress,
			const ACE_UINT32 maxAge,
			const bool checkSend);

	/*=================================================================== */
	/**

							    	      @brief          CalcRelativeTime
							    						  Calculate age in seconds

							    		  @param          creationTime
														  -
							      		  @param          currentTime
														  -

							    	      @return         ACE_UINT32	-

							    	      @exception      none
	 */
	/*=================================================================== */
	/*ACE_UINT32 CalcRelativeTime(const SYSTEMTIME creationTime,
                                 const SYSTEMTIME currentTime);*/
	ACE_UINT32 CalcRelativeTime(const time_t creationTime,struct tm * currentTime);
	/*=================================================================== */
	/**

							    	      @brief          ChangePath
							    						  Change path for Ready to Send

							    		  @param          path
														  -

							    	      @return         true		on success
														  false     on unsuccessful

							    	      @exception      none
	 */
	/*=================================================================== */
	bool ChangePath(string &path);

	/*====================================================================
			                       PRIVATE ATTRIBUTES
		==================================================================== */

	/*=================================================================== */
	/**
				      @brief   nextScanTime_
							   Next folder scan
	 */
	/*=================================================================== */
	time_t nextScanTime_;

	/*=================================================================== */
	/**
					      @brief   working_
								   Flag indicating if we are working
	 */
	/*=================================================================== */
	bool working_;

	/*=================================================================== */
	/**
					      @brief   workingState_
								   Working state
	 */
	/*=================================================================== */
	WorkingState workingState_;

	/*=================================================================== */
	/**
					      @brief   lDestNames_
								   All notification destinations
	 */
	/*=================================================================== */
	std::list<string> lDestNames_;

	/*=================================================================== */
	/**
					      @brief   lDestNamesIt_
								   Iterator for destination list
	 */
	/*=================================================================== */
	std::list<string>::iterator lDestNamesIt_;


};

#endif 
