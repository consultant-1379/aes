/*=================================================================== */
/**
@file alarmhandler.cpp

Class method implementation for alarmhandler.h

DESCRIPTION
The services provided by AlarmType and  AlarmHandler facilitates alarm handling.

ERROR HANDLING
General rule:

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       26/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <alarmhandler.h> 
#include <aes_gcc_eventhandler.h>
#include <event.h>
#include <string>
#include <transdest.h>
#include "aes_cdh_logger.h"

AlarmHandler* AlarmHandler::instance = 0;

/*===================================================================
                        ROUTINE DECLARATION SECTION
=================================================================== */

/*===================================================================
   ROUTINE:AlarmHandler
=================================================================== */
AlarmHandler::AlarmHandler()
: CONNECTION_FAULT_TEXT("DATA OUTPUT, AP TRANSMISSION FAULT "),
  CONNECTION_DATA_TEXT("The connection to the remote host lost."),
  NOTIFICATION_DATA_TEXT("Acknowledgement not received."),
  NOTIFICATION_FAULT_TEXT("FILE NOTIFICATION, AP CDH, ACKNOWLEDGEMENT NOT RECEIVED"),
  DESTINATION_TEXT("DESTINATION"),
  FILENAME_TEXT("FILE NAME")
{
	connection.setProbableCause(CONNECTION_FAULT_TEXT.c_str() );
	connection.setProblemData(CONNECTION_DATA_TEXT.c_str() );
	connection.setEventCode(AES_CDH_ConnectFault);

	notification.setProbableCause(NOTIFICATION_FAULT_TEXT.c_str() );
	notification.setProblemData(NOTIFICATION_DATA_TEXT.c_str() );
	notification.setEventCode(AES_CDH_NotifyFault);
}

/*===================================================================
   ROUTINE:~AlarmHandler
=================================================================== */
AlarmHandler::~AlarmHandler()
{
	delete instance;
	instance = 0;
}

/*===================================================================
   ROUTINE:Instance
=================================================================== */
AlarmHandler* AlarmHandler::Instance()
{
	if (instance == 0)
	{
		instance = new AlarmHandler;
	}
	return instance;
}

/*===================================================================
   ROUTINE:raiseConnectionAlarm
=================================================================== */
bool AlarmHandler::raiseConnectionAlarm(std::string destination, bool asEvent, std::string alarmText)
{
	alhMutex_.acquire();

	std::string problemText("");
	string myObjOfRef("");
	string destinationId_("");

	bool retStatus = true;

	if (alarmText.empty() )
	{
		problemText = DESTINATION_TEXT + '\n' + destination;
	}
	else
	{
		problemText = alarmText;
	}

	connection.setProblemText(problemText.c_str() );
	retStatus = ServR::getObjectOfRef("DESTINATION", destination, myObjOfRef );

	if(retStatus == false)
	{
		alhMutex_.release();
		return false;
	}

	destinationId_ = myObjOfRef + destination;
	connection.setObjectOfReference(destinationId_.c_str());

	// If only reported as event, end here.
	if (asEvent == true)
	{
		AES_GCC_EventHandler::instance().event(connection, false);
		alhMutex_.release();
		return true;
	}

	AlarmMap::iterator itr = alarmMap.find(destination);
	AlarmType* tmpType;

	if (itr == alarmMap.end() )
	{
		tmpType = new AlarmType();
		alarmPair.first = destination;
		alarmPair.second = tmpType;
		alarmMap.insert(alarmPair);
	}
	else
	{
		tmpType = (*itr).second;
	}

	if (tmpType->isConnectAlarm() )
	{
		alhMutex_.release();
		return false;
	}

	tmpType->setConnectAlarm(true);
	tmpType->setConnectText(problemText);

	AES_GCC_EventHandler::instance().alarm(connection, "A3");
	alhMutex_.release();
	return true;
}

/*===================================================================
   ROUTINE:raiseNotificationAlarm
=================================================================== */
bool AlarmHandler::raiseNotificationAlarm(std::string fileName, std::string destination, std::string alarmText)
{
	alhMutex_.acquire();
	std::string problemText("");
	string alarm_status("");

	if (alarmText.empty() )
	{
		problemText = FILENAME_TEXT + '\n' + fileName + '\n' + '\n' + DESTINATION_TEXT + '\n' + destination;
	}
	else
	{
		problemText = alarmText;
	}

	notification.setProblemText(problemText.c_str() );

	notification.setObjectOfReference(destination.c_str() );

	AlarmMap::iterator itr = alarmMap.find(destination);
	AlarmType* tmpType;

	if (itr == alarmMap.end() )
	{
		tmpType = new AlarmType();
		alarmPair.first = destination;
		alarmPair.second = tmpType;
		alarmMap.insert(alarmPair);
	}
	else
	{
		tmpType = (*itr).second;
	}
	if (tmpType->isNotifyAlarm() )
	{
		alhMutex_.release();
		return false;
	}
	tmpType->setNotifyAlarm(true);
	tmpType->setNotifyText(problemText);
	tmpType->setNotifyFile(fileName);

	AES_GCC_EventHandler::instance().alarm(notification, "A3");
	alhMutex_.release();
	return true;
}

/*===================================================================
   ROUTINE:ceaseConnectionAlarm
=================================================================== */
bool AlarmHandler::ceaseConnectionAlarm(std::string destination)
{
	alhMutex_.acquire();

	std::string problemText("");
	std::string myObjOfRef("");
	std::string destinationId_("");
	bool retStatus = true;
	AlarmMap::iterator itr = alarmMap.find(destination);

	if (itr != alarmMap.end() )
	{
		if ( (*itr).second->isNotifyAlarm() )
		{
			(*itr).second->setConnectAlarm(false);
			(*itr).second->setConnectText("");
		}
		else
		{
			alarmMap.erase(itr);
		}
	}
	else
	{

		alhMutex_.release();
		return false;
	}

	problemText = DESTINATION_TEXT + '\n' + destination;

	connection.setProblemText(problemText.c_str() );
	retStatus= ServR::getObjectOfRef( "DESTINATION", destination, myObjOfRef );
	if(retStatus == false)
	{
		alhMutex_.release();
		return false;
	}
	destinationId_ = myObjOfRef + destination;
	connection.setObjectOfReference(destinationId_.c_str());
	AES_GCC_EventHandler::instance().cease(connection);
	alhMutex_.release();

	return true;
}

/*===================================================================
   ROUTINE:ceaseNotificationAlarm
=================================================================== */
bool AlarmHandler::ceaseNotificationAlarm(std::string fileName, std::string destination)
{
	alhMutex_.acquire();

	std::string problemText("");
	AlarmMap::iterator itr = alarmMap.find(destination);

	if (itr != alarmMap.end() )
	{
		if ( (*itr).second->isConnectAlarm() )
		{
			(*itr).second->setNotifyAlarm(false);
			(*itr).second->setNotifyText("");
			(*itr).second->setNotifyFile("");
		}
		else
		{
			alarmMap.erase(itr);
		}
	}
	else
	{
		alhMutex_.release();
		return false;
	}

	problemText = FILENAME_TEXT + '\n' + fileName + '\n' + '\n' + DESTINATION_TEXT + '\n' + destination;

	notification.setProblemText(problemText.c_str() );
	notification.setObjectOfReference(destination.c_str() );
	AES_GCC_EventHandler::instance().cease(notification);
	alhMutex_.release();
	return true;
}

/*===================================================================
   ROUTINE:ceaseAllAlarms
=================================================================== */
void AlarmHandler::ceaseAllAlarms()
{
	alhMutex_.acquire();

	AlarmMap::iterator itr;
	string myObjOfRef("");
	string destinationId_("");
	bool retStatus = true;

	for (itr=alarmMap.begin();itr!=alarmMap.end();++itr)
	{
		if ( (*itr).second->isConnectAlarm() )
		{
			connection.setProblemText( (*itr).second->getConnectText().c_str() );
			retStatus = ServR::getObjectOfRef( "DESTINATION", ( (*itr).first.c_str() ), myObjOfRef );
			if(retStatus == false)
			{
				AES_CDH_LOG(LOG_LEVEL_TRACE, "NO need to raise alarm in ceaseAllAlarms :: isConnectAlarm as not found in map  --> AlarmHandler::ceaseNotificationAlarms");
			}
			else
			{
				destinationId_ = myObjOfRef + ((*itr).first);
				connection.setObjectOfReference(destinationId_.c_str());
				//  connection.setObjectOfReference( (*itr).first.c_str() );
				AES_GCC_EventHandler::instance().cease(connection);
			}
		}

		if ( (*itr).second->isNotifyAlarm() )
		{
			notification.setProblemText( (*itr).second->getNotifyText().c_str() );
			notification.setObjectOfReference( (*itr).first.c_str() );
			AES_GCC_EventHandler::instance().cease(notification);
		}
	}
	alhMutex_.release();
	alarmMap.clear();
}

/*===================================================================
   ROUTINE:AlarmType
=================================================================== */
AlarmType::AlarmType()
: connectAlarm(false),
    notifyAlarm(false),
    connectText(""),
    notifyText(""),
    notifyFile("")
{
    
}

/*===================================================================
   ROUTINE:~AlarmType
=================================================================== */
AlarmType::~AlarmType()
{

}


/*===================================================================
   ROUTINE: setConnectAlarm
=================================================================== */
void AlarmType::setConnectAlarm(bool set)
{
	if (set == true)
	{
		connectAlarm = true;
	}
	else
	{
		connectAlarm = false;
		connectText = "";
	}
}

/*===================================================================
   ROUTINE:setNotifyAlarm
=================================================================== */
void AlarmType::setNotifyAlarm(bool set)
{
	if (set == true)
	{
		notifyAlarm = true;
	}
	else
	{
		notifyAlarm = false;
		notifyText = "";
	}
}


/*===================================================================
   ROUTINE: isConnectAlarm
=================================================================== */
bool AlarmType::isConnectAlarm()
{
    return connectAlarm;
}

/*===================================================================
   ROUTINE:isNotifyAlarm
=================================================================== */
bool AlarmType::isNotifyAlarm()
{
    return notifyAlarm;
}


/*===================================================================
   ROUTINE:setConnectText
=================================================================== */
void
AlarmType::setConnectText(std::string text)
{
    connectText = text;
}

/*===================================================================
   ROUTINE:setNotifyText
=================================================================== */
void AlarmType::setNotifyText(std::string text)
{
    notifyText = text;
}


/*===================================================================
   ROUTINE:getConnectText
=================================================================== */
std::string AlarmType::getConnectText()
{
    return connectText;
}


/*===================================================================
   ROUTINE:getNotifyText
=================================================================== */
std::string AlarmType::getNotifyText()
{
    return notifyText;
}


/*===================================================================
   ROUTINE:setNotifyFile
=================================================================== */
void AlarmType::setNotifyFile(std::string file)
{
    notifyFile = file;
}


/*===================================================================
   ROUTINE:getNotifyFile
=================================================================== */
std::string AlarmType::getNotifyFile()
{
    return notifyFile;
}

// EOF AlarmHandler.cpp
