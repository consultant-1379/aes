/*=================================================================== */
/**
@file readypoll.cpp

Class method implementation for readypoll.h

DESCRIPTION
The services provided by ReadyPoll facilitates polling of data.

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

N/A       16/09/2014     XNADNAR       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <readypoll.h>
#include <event.h>
#include <cdhcriticalsection.h>
#include <time.h>
#include <fstream>
#include <ftprv2.h>
#include <sshftprv2.h>
#include <sys/stat.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
using namespace std;

/*===================================================================
                        DECLARATION SECTION
=================================================================== */
AES_CDH_TRACE_DEFINE(AES_CDH_ReadyPoll);

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */

namespace readydirpath
{
	const string delim("/");
	const string dir("Ready");
}
namespace readyPollParams
{
	const char* attrNameMaxFileNumber = "readyPollMaxFileNumber";
	const char* attrNameOldestFileDate = "readyPollOldestFileDate";
	const char* attrNamePollFrequency = "readyPollingPeriod";
	const unsigned int maxFileNumberLimit = 32000;
	const unsigned int oldestFileLimit = 720; //Hours
	const unsigned int maxPollFrequency = 7200; //Minutes
	const unsigned int ceaseTimeout = 300; //5 minutes
	const unsigned int initialPollFrequency = 1U; //1 sec
}
/*===================================================================
   ROUTINE:ReadyPoll
=================================================================== */
ReadyPoll::ReadyPoll(std::string pollDest): 
						m_actVdDestName(pollDest),
						m_emptyDestSet(),
						m_pollFrequency(1440),
						m_oldestFileDate(168),
						m_maxFileNumber(5000),
						m_oldestFileTimeStamp(0),
						m_currentNoOfFiles(0),
						m_readyDirPath(),
						m_isReadyDirAlarmActive(false),
						exitThread_(NULL)
{			
	readCDHReadyPollParams();		
} 

/*===================================================================
   ROUTINE:~ReadyPoll
=================================================================== */
ReadyPoll::~ReadyPoll()
{
}

/*===================================================================
   ROUTINE:open
=================================================================== */
int ReadyPoll::open(void*)
{
	exitThread_ = false;
	AES_CDH_LOG(LOG_LEVEL_INFO, "Activating thread for ReadyPOll");
	return this->activate();
}

/*===================================================================
   ROUTINE:shutdown
=================================================================== */
void ReadyPoll::shutdown(void)
{
	exitThread_ = true;
	AES_CDH_LOG(LOG_LEVEL_INFO, "Shutdown signal received for ReadyPoll thread");
	m_pollEv.signal();
	this->wait();
}
/*===================================================================
   ROUTINE:
=================================================================== */
void ReadyPoll::readCDHReadyPollParams()
{
	OmHandler myOmHandler;

	if(ACS_CC_FAILURE != myOmHandler.Init())
	{
		ACS_APGCC_ImmObject fileTraMInstance;
		fileTraMInstance.objName = AES_GCC_Util::dnOfFileTransferM;
		if(ACS_CC_FAILURE != myOmHandler.getObject(&fileTraMInstance))
		{
			//Fetch the attribute values
			std::vector<ACS_APGCC_ImmAttribute>::iterator fileMAttrItr;
			for(fileMAttrItr = fileTraMInstance.attributes.begin();fileMAttrItr != fileTraMInstance.attributes.end();fileMAttrItr++)
			{
				if(0 == ((*fileMAttrItr).attrName).compare(readyPollParams::attrNameMaxFileNumber))
				{
					m_maxFileNumber = (*reinterpret_cast<unsigned int*>((*fileMAttrItr).attrValues[0]));
				}
				if(0 == ((*fileMAttrItr).attrName).compare(readyPollParams::attrNameOldestFileDate))
				{
					m_oldestFileDate = (*reinterpret_cast<unsigned int*>((*fileMAttrItr).attrValues[0]));
				}
				if(0 == ((*fileMAttrItr).attrName).compare(readyPollParams::attrNamePollFrequency))
				{
					m_pollFrequency = (*reinterpret_cast<unsigned int*>((*fileMAttrItr).attrValues[0]));
				}	
			}
			AES_CDH_TRACE_MESSAGE("m_maxFileNumber = %d  m_pollFrequency = %lu m_oldestFileDate = %lu",m_maxFileNumber,m_pollFrequency,m_oldestFileDate);
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Not able to fetch polling attributes from IMM");
			AES_CDH_TRACE_MESSAGE("Not able to fetch polling attributes from IMM");
		}
		myOmHandler.Finalize();
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while initializing OmHandler");
		AES_CDH_TRACE_MESSAGE("Error occurred while initializing OmHandler");
	}
}

/*===================================================================
   ROUTINE:svc
=================================================================== */
int ReadyPoll::svc(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	//Initial poll frequency is one second.
	ACE_Time_Value polltv(readyPollParams::initialPollFrequency);

	//If the alarm raised then try to cease it every 5 mins....
	ACE_Time_Value ceasetv(readyPollParams::ceaseTimeout,0);

	do
	{
		if(!m_isReadyDirAlarmActive)
		{
			if(m_pollEv.wait(&polltv,0))
			{
				if(exitThread_)
				{
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "stop event signalled");
					AES_CDH_TRACE_MESSAGE("stop event signalled");
					break;
				}
				directoryPolling();
				raiseVdPollAlarm();
				polltv.sec(m_pollFrequency);
			}
		}
		//If the alarm raised then try to cease it every 5 mins....
		else
		{
			if(m_pollEv.wait(&ceasetv,0))
			{
				if(exitThread_)
				{
					AES_CDH_LOG(LOG_LEVEL_DEBUG, "stop event signalled");
					AES_CDH_TRACE_MESSAGE("stop event signalled");
					break;
				}
				directoryPolling();
				if ( m_maxFileNumber && m_oldestFileDate)
				{
					unsigned int currentOledestFileDate = time(NULL) - m_oldestFileTimeStamp;
					if ( (m_currentNoOfFiles == 0) || ( (m_currentNoOfFiles < m_maxFileNumber) &&
							(currentOledestFileDate < m_oldestFileDate ) ) )
						ceaseVdPollAlarm();
					continue;
				}
				if (m_maxFileNumber)
				{
					if (m_currentNoOfFiles < m_maxFileNumber)
						ceaseVdPollAlarm();
					continue;
				}
				if ( m_oldestFileDate )
				{
					unsigned int currentOledestFileDate = time(NULL) - m_oldestFileTimeStamp;
					if ( (m_currentNoOfFiles == 0) || currentOledestFileDate < m_oldestFileDate)
						ceaseVdPollAlarm();
				}
			} 
		}	
	}while(! exitThread_);
	return 0;
}


/*===================================================================
   ROUTINE:setCDHReadyPollParams
=================================================================== */
void ReadyPoll::setCDHReadyPollParams(TransferAgent* TransPtr_)
{
	if(AES_CDH_RC_OK == TransPtr_->readVDPath(m_readyDirPath))
	{
		m_readyDirPath += readydirpath::delim+m_actVdDestName+readydirpath::delim+readydirpath::dir;
		AES_CDH_TRACE_MESSAGE("For dest %s , path = %s",m_actVdDestName.c_str(),m_readyDirPath.c_str());
	}
	else
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Failed");
}

/*===================================================================
   ROUTINE:directoryPolling
=================================================================== */
void ReadyPoll::directoryPolling()
{
	m_currentNoOfFiles = 0;
	m_oldestFileTimeStamp = 0;
	ACE_stat statcurrentFile;

	DIR* pDir = opendir (m_readyDirPath.c_str());

	if ( pDir !=  NULL)
	{
		dirent* pEntry;
		std::string filePath;
		while ((pEntry = readdir(pDir)) != NULL) 
		{
			// for getting the modifcation time. Here modification time is being used instead of creation time. if needed, need to analyse in future more on it
			if (ACE_OS::strcmp ( pEntry->d_name, ".") && strcmp ( pEntry->d_name, ".."))
			{
				filePath = m_readyDirPath +readydirpath::delim+ string(pEntry->d_name);
				if(ACE_OS::stat(filePath.c_str(), &statcurrentFile) == 0)
				{
					m_currentNoOfFiles++;

					if (1 == m_currentNoOfFiles) //for first file oldestfile is equal to the current file
					{				
						m_oldestFileTimeStamp = statcurrentFile.st_mtime;
					}
					else
					{
						if(statcurrentFile.st_mtime < m_oldestFileTimeStamp)
						{
							m_oldestFileTimeStamp = statcurrentFile.st_mtime; 
						}
					}
				}
				else
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "Stat failed for %s",filePath.c_str());
					AES_CDH_TRACE_MESSAGE("Stat failed for %s",filePath.c_str());
				}
			}// end of main if in while
			if(exitThread_)
			{
				AES_CDH_LOG(LOG_LEVEL_INFO, "Stop flag set to true, exiting..");
				break;
			}
		}//end while
		AES_CDH_TRACE_MESSAGE("Leaving , current no of Files %d , Oldfiletimestamp %u ",m_currentNoOfFiles,m_oldestFileTimeStamp);
		if(pDir != NULL)
			ACE_OS::closedir(pDir);
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving, opendir failed for %s",m_readyDirPath.c_str());
	}
}

/*===================================================================
   ROUTINE:raiseVdPollAlarm
=================================================================== */
void ReadyPoll::raiseVdPollAlarm()
{
	if(m_isReadyDirAlarmActive== false)
	{
		unsigned int curOldFileTimeStamp = time(NULL) - m_oldestFileTimeStamp;
		//m_currentNoOfFiles should be greater than zero
		if( (0U != m_currentNoOfFiles) && ( ((0U != m_maxFileNumber)&&(m_currentNoOfFiles > m_maxFileNumber)) || ((0U != m_oldestFileDate) && (curOldFileTimeStamp > m_oldestFileDate)) ) )
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, "Destination : %s readyPollMaxFileNumber = %u , current file count = %u readyPollOldestFileDate = %lu, current oldest file = %u ",m_actVdDestName.c_str(),m_maxFileNumber,m_currentNoOfFiles,m_oldestFileDate,curOldFileTimeStamp);
			DestinationAlarm::AlarmRecord::thisCause SUalarmCause;
			std::string tmpGlbDestPath;
			SUalarmCause = DestinationAlarm::AlarmRecord::SU_error;
			if (AES_GCC_Util::datapath_trn(m_readyDirPath,AES_DATA_PATH,tmpGlbDestPath)==false)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to strip the nbi path from data path");
			}

			DestinationAlarm::instance()->raise(m_actVdDestName, m_emptyDestSet, SUalarmCause,
					tmpGlbDestPath,m_currentNoOfFiles,m_oldestFileTimeStamp);
			m_isReadyDirAlarmActive= true;
			AES_CDH_TRACE_MESSAGE("Alarm raised");
		}
	}
	else
		AES_CDH_TRACE_MESSAGE("Active alarm aleardy present");
}
/*===================================================================
   ROUTINE:ceaseVdPollAlarm
=================================================================== */
void ReadyPoll::ceaseVdPollAlarm()
{
	DestinationAlarm::AlarmRecord::thisCause SUalarmCause;
	SUalarmCause = DestinationAlarm::AlarmRecord::SU_error; 

	std::string tmpGlbDestPath;
	SUalarmCause = DestinationAlarm::AlarmRecord::SU_error;
	if (AES_GCC_Util::datapath_trn(m_readyDirPath,AES_DATA_PATH,tmpGlbDestPath)==false)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to strip the nbi path from data path");
	}
	DestinationAlarm::instance()->cease(m_actVdDestName, m_emptyDestSet, SUalarmCause,
			tmpGlbDestPath,m_currentNoOfFiles,m_oldestFileTimeStamp);
	m_isReadyDirAlarmActive = false;
	AES_CDH_TRACE_MESSAGE("Alarm ceased");
}

/*===================================================================
   ROUTINE:vdAlarmStatus
=================================================================== */
bool ReadyPoll::vdAlarmStatus()
{
	return m_isReadyDirAlarmActive;
}

/*===================================================================
   ROUTINE:validatePollParams
=================================================================== */
bool ReadyPoll::validatePollParams()
{
	bool result = false;
	if(m_oldestFileDate > readyPollParams::oldestFileLimit)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "readyPollOldestFileDate is out of range 0..720 hours");
		return result;
	}
	if(m_pollFrequency > readyPollParams::maxPollFrequency)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR,"readyPollingPeriod is out of range 0..7200 minutes");
		return result;
	}
	if(m_maxFileNumber > readyPollParams::maxFileNumberLimit)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "readyPollMaxFileNumber is out of range 0..32000");
		return result;
	}
	//hours to secs
	m_oldestFileDate = m_oldestFileDate*60*60;
	//minutes to secs
	m_pollFrequency = m_pollFrequency*60;

	if(m_pollFrequency && (m_oldestFileDate || m_maxFileNumber))
		result = true;

	return result;
}
