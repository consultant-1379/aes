/*=================================================================== */
/**
	@file   alarmhandler.h

	@brief
	The service provided by AlarmType and  AlarmHandler facilitates types of alarms and its handling.

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
#ifndef ALARMHANDLER_H
#define ALARMHANDLER_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <map>
#include <utility>
#include <ace/Mutex.h>
#include <aes_gcc_event.h>
#include <ace/ACE.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */

/*===================================================================*/
/**
		@brief AlarmType
	This class is only used internal in AlarmHandler
	The class stores connect/notify alarm per destination
 */
/*=================================================================== */
class AlarmType
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

	      @brief           default constructor
	                       Detailed description

	      @return          void

	      @exception       none
	   */
/*=================================================================== */
	AlarmType();

/*===================================================================
	                     CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
/**

	      @brief           default destructor
	                       Detailed description

	      @return          void

	      @exception       none
*/
/*=================================================================== */
 ~AlarmType();

/*=================================================================== */
/**

	      @brief			setConnectAlarm
							connectAlarm gets set.

		  @param           	set
							set variable is used to set the value of connectAlarm.

	      @return           void

	      @exception       none
 */
/*=================================================================== */
	void setConnectAlarm(bool set = true);

/*=================================================================== */
/**

		      @brief		setNotifyAlarm
							notifyAlarm gets set.

			  @param           	set
								set variable is used to set the value of notifyAlarm.

		      @return           void

		      @exception       none
 */
/*=================================================================== */
	void setNotifyAlarm(bool set = true);

/*=================================================================== */
/**

		      @brief		isConnectAlarm
							The value of connectAlarm is returned.

			  @return          true    if connectAlarm is true.
							   false   if connectAlarm is false.

		      @exception       none
 */
/*=================================================================== */
	bool isConnectAlarm();

/*=================================================================== */
/**

		      @brief		isNotifyAlarm
							gets value of notifyAlarm.

		      @return           true    if notifyAlarm is true.
								false   if notifyAlarm is false.

		      @exception       	none
 */
/*=================================================================== */
	bool isNotifyAlarm();

/*=================================================================== */
/**

		      @brief		setConnectText
							connectText is set here.

			  @param           	text
								This text is to be assigned to connectText.

			  @return           void.

		      @exception     	none
 */
/*=================================================================== */
	void setConnectText(std::string text);

/*=================================================================== */
/**

		      @brief		setNotifyText
							notifyText is set here.

			  @param       	text
							This text is to be assigned to notifyText.

		      @return       void.

		      @exception    none
 */
/*=================================================================== */
	void setNotifyText(std::string text);

/*=================================================================== */
/**

		      @brief		setNotifyFile
							notifyFile is set here.

			  @param        file
							This file gets assigned to notifyFile.

		      @return       void.

		      @exception    none
 */
/*=================================================================== */
	void setNotifyFile(std::string file);

/*=================================================================== */
/**

		      @brief		getNotifyFile
							Gets notifyFile.

			  @return       string
							notifyFile is returned.

		      @exception     none
 */
/*=================================================================== */
	std::string getNotifyFile();

/*=================================================================== */
/**

		      @brief		getConnectText
							Gets connectText.

		      @return       string
							connectText is returned.

		      @exception    none
 */
/*=================================================================== */
	std::string getConnectText();

/*=================================================================== */
/**

		      @brief		getNotifyText
							Gets notifyText.

		      @return           string
								notifyText is returned.

		      @exception       	none */
/*=================================================================== */
	std::string getNotifyText();

/*=====================================================================
                    PRIVATE DECLARATION SECTION
==================================================================== */
private:

/*====================================================================
                       PRIVATE ATTRIBUTES
==================================================================== */

/*=================================================================== */
/**
	      @brief   connectAlarm
               Detailed description
*/
/*=================================================================== */
	bool connectAlarm;

/*=================================================================== */
/**
	      @brief   notifyAlarm
	               Detailed description
*/
/*=================================================================== */
	bool notifyAlarm;

/*=================================================================== */
/**
		  @brief   connectText
	               Detailed description
*/
/*=================================================================== */
	std::string connectText;

/*=================================================================== */
/**
	      @brief   notifyText
	               Detailed description
*/
/*=================================================================== */
	std::string notifyText;

/*=================================================================== */
/**
	      @brief   notifyFile
	               Detailed description
*/
/*=================================================================== */
	std::string notifyFile;

};

/*=====================================================================
                        TYPE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief AlarmMap
          Description of AlarmMap.
 */
/*=================================================================== */
typedef std::map<std::string, AlarmType*, std::less<std::string> > AlarmMap;

/*=================================================================== */
/**
   @brief AlarmPair
          Description of AlarmPair.
 */
/*=================================================================== */
typedef	std::pair<std::string, AlarmType*> AlarmPair;


/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */
/*===================================================================*/
/**
		@brief AlarmHandler
	This class is used to raise and cease alarms for connection
	and notification.
 */
/*=================================================================== */
class AlarmHandler
{

/*=====================================================================
                     PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*===================================================================
	                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
/**

	      @brief           ~AlarmHandler
	                       Detailed description

	      @return          void

	      @exception       none
*/
/*=================================================================== */
virtual ~AlarmHandler();

/*=================================================================== */
/**

	      @brief		raiseConnectionAlarm
						Operations for raising alarms.
						Example of usage:
						AlarmHandler::Instance()->raiseConnectionAlarm("DEST1", false, "");
						AlarmHandler::Instance()->raiseNotificationAlarm("FILE1", "DEST1", false, "");

		  @param           	destination
							destination.
		  @param           	asEvent
							to be event or not.
		  @param           	alarmText
							The text of the alarm to be raised.

	      @return           true    to indicate that everything is OK.
							false   to indicate that something went wrong.

	      @exception       none
 */
/*=================================================================== */
bool raiseConnectionAlarm(std::string destination, bool asEvent = false, std::string alarmText = "");


	/*=================================================================== */
	/**

		      @brief		raiseNotificationAlarm
							Operations for raising notification alarms.

			  @param           	fileName
								filename.
			  @param           	destination
								destination.
			  @param           	alarmText
								The text of the alarm to be raised.

		      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

		      @exception       	none
	 */
	/*=================================================================== */
	bool raiseNotificationAlarm(std::string fileName, std::string destination, std::string alarmText = "");


	/*=================================================================== */
	/**

		      @brief		ceaseConnectionAlarm
							Operation for ceasing alarms
							Example of usage:
							AlarmHandler::Instance()->ceaseConnectionAlarm("DEST1");
							AlarmHandler::Instance()->ceaseNotificationAlarm("FILE1", "DEST1");

			  @param           	destination
								destination.

		      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

		      @exception       none
	 */
	/*=================================================================== */
	//

	bool ceaseConnectionAlarm(std::string destination);


	/*=================================================================== */
	/**

		      @brief		ceaseNotificationAlarm
							ceases the notifiaction alarm.

			  @param           	fileName
								file.
			  @param           	destination
								destination to be.

		      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

		      @exception       none
	 */
	/*=================================================================== */
	bool ceaseNotificationAlarm(std::string fileName, std::string destination);


	/*=================================================================== */
	/**

		      @brief		ceaseAllAlarms
							Cease all alarms, only to be used when ending CDH.

			  @return           void

		      @exception       none
	 */
	/*=================================================================== */
	void ceaseAllAlarms();

	/*=================================================================== */
	/**

		      @brief		Instance
							create an Instance if previously not exists.

		      @return           Instance     Instance which got created

		      @exception       none
	 */
	/*=================================================================== */
	static AlarmHandler* Instance ();

/*===================================================================
                  PROTECTED DECLARATION SECTION
================================================================== */
protected:
/*====================================================================
                      PROTECTED ATTRIBUTES
==================================================================== */

/*=================================================================== */
/**
	      @brief   alhMutex_
	               Detailed description
*/
/*=================================================================== */
	ACE_Mutex alhMutex_;

/*===================================================================
                    PRIVATE DECLARATION SECTION
=================================================================== */
private:
/*=====================================================================
                       CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**

	      @brief           AlarmHandler
	                       Detailed description

	      @return          void

	      @exception       none
 */
/*=================================================================== */
	AlarmHandler();

/*====================================================================
	               PRIVATE ATTRIBUTES
==================================================================== */
/*=================================================================== */
/**
	      @brief   alarmMap
	               Detailed description
*/
/*=================================================================== */
	AlarmMap alarmMap;
/*=================================================================== */
/**
	      @brief   alarmPair
	               Detailed description
*/
/*=================================================================== */
	AlarmPair alarmPair;
/*=================================================================== */
/**
	      @brief   CONNECTION_FAULT_TEXT
	               Detailed description
*/
/*=================================================================== */
	const std::string CONNECTION_FAULT_TEXT;
/*=================================================================== */
  /**
	      @brief   CONNECTION_DATA_TEXT

	               Detailed description
 */
/*=================================================================== */
	const std::string CONNECTION_DATA_TEXT;
/*=================================================================== */
   /**
	      @brief   NOTIFICATION_DATA_TEXT
	               Detailed description
   */
/*=================================================================== */
	const std::string NOTIFICATION_DATA_TEXT;
/*=================================================================== */
	 /**
	      @brief   NOTIFICATION_FAULT_TEXT
	               Detailed description
	 */
/*=================================================================== */
	const std::string NOTIFICATION_FAULT_TEXT;
/*=================================================================== */
   /**
	      @brief   DESTINATION_TEXT
	               Detailed description
   */
/*=================================================================== */
	const std::string DESTINATION_TEXT;
/*=================================================================== */
   /**
		      @brief   FILENAME_TEXT
		               Detailed description
   */
/*=================================================================== */
	const std::string FILENAME_TEXT;
/*=================================================================== */
   /**
		      @brief   connection
		               Detailed description
   */
/*=================================================================== */
	AES_GCC_Event connection;
/*=================================================================== */
   /**
		      @brief   notification
		               Detailed description
   */
/*=================================================================== */
	AES_GCC_Event notification;
/*=================================================================== */
   /**
		      @brief   instance
		               Detailed description
   */
/*=================================================================== */
	static AlarmHandler* instance;

};

#endif
