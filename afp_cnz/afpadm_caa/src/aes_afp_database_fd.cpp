//	INCLUDE aes_afp_database_fd.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	      190 89-CAA 109 0503
//
//	AUTHOR
//	 2003-01-18 UAB/KB/AU DAPA
//
//	REVISION
//	 A 2003-01-18 DAPA
//	 B 2003-05-08 DAPA
//
//	LINKAGE
//
//	SEE ALSO

#include <aes_afp_parameter.h>
#include <aes_afp_api.h>
#include <acs_apgcc_omhandler.h>
#include <aes_afp_command_handler.h>
#include <aes_gcc_log.h>
#include <aes_afp_defines.h>
#include <ACS_APGCC_Util.H>
#include <aes_afp_services.h>
#include <set>
#include <aes_afp_database_fd.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
#include <boost/filesystem.hpp>

//#include <ace/Task_T.h>
//#include <ace/Synch.h>
//#include <ace/Thread_Mutex.h>

AES_AFP_TRACE_DEFINE(AES_AFP_database_fd);
namespace DeleteThreadParam
{
	int deleteEvTimeout = 10;
}
unsigned long getTimeStamp();
extern aes_afp_database_fd *afpDatabase;

aes_afp_database_fd::aes_afp_database_fd()
: MAX_DESTINATIONSETS_(32),
  deleteThreadHandle_(0),
  isShutdownSignaled_(false)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	afpDatabase = this;

}


aes_afp_database_fd::~aes_afp_database_fd()
{
}

//	---------------------------------------------------------
//	       removeTransferQueue()
//	---------------------------------------------------------
bool aes_afp_database_fd::removeTransferQueue (std::string transferQueue, std::string username, AES_GCC_Errorcodes& error)
{
	(void)(username);
	AES_AFP_TRACE_MESSAGE("Removing TQ = %s, username %s", transferQueue.c_str(),username.c_str());

	bool foundOne(false);
	bool fileIsProtected(false);
	bool result(false);

	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Transfer queue %s not found", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("Transfer queue %s not found", transferQueue.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	TransferQueueMap::iterator itr;

	itr = transferQueueMap_.begin();
	while (itr != transferQueueMap_.end() )
	{
		if ((*itr).second->getName() == transferQueue)
		{

			foundOne = true;
			result = (*itr).second->removeAll(error);
			if (result == false)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove TQ = %s", transferQueue.c_str());
				AES_AFP_TRACE_MESSAGE("Not able to remove TQ = %s", transferQueue.c_str());

				if (error == AES_FILEISPROTECTED)
				{
					fileIsProtected = true;
					++itr;
				}
				else
				{
					m_synchOnTQMap.release();
					return false;
				}
			}
			else
			{
				aes_afp_objectfactory::Instance().releaseTransferQueueObject((*itr).second);
				transferQueueMap_.erase(itr);
				itr = transferQueueMap_.begin();
			}
		}
		else
		{
			++itr;
		}
	}
	if (fileIsProtected == true)
	{
		error = AES_FILEISPROTECTED;
		m_synchOnTQMap.release();
		return false;
	}
	if (foundOne == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not found", transferQueue.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
}

//	---------------------------------------------------------
//	       createFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::createFile (std::string transferQueue, std::string filename, std::string generation, AES_GCC_Filestates filenameState, AES_GCC_Errorcodes& error, std::string caller, std::string username)
{
	TransferQueueMap::iterator itr;
	bool result(false);
	bool found(false);
	bool doOnce(true);
	std::string tmpSendItem;

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found, Generation %s username %s", transferQueue.c_str(), generation.c_str(), username.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not found, Generation %s username %s", transferQueue.c_str(), generation.c_str(), username.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if (transferQueue == (*itr).second->getName() )
		{
			if ( (*itr).second->getRenameFile() == AES_LOCAL)
			{
				if (doOnce == true)
				{
					(*itr).second->localRenameFile(filename, tmpSendItem);
					doOnce = false;
				}
			}
			else
			{
				tmpSendItem = filename;
			}
			found = true;
			result = (*itr).second->insertFile(tmpSendItem, error, filenameState, "", caller, filename);
		}
	}

	if (found ==  true && result == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to create file %s in TQ %s", filename.c_str(), transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("Unable to create file %s in TQ %s", filename.c_str(), transferQueue.c_str());
		if (found == false)
		{
			error = AES_NOPROCORDER;
		}

		m_synchOnTQMap.release();
		return false;
	}
}

//	---------------------------------------------------------
//	       createExactFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::createExactFile (std::string transferQueue, std::string destinationSet, std::string filename, std::string generation, AES_GCC_Filestates fileState, AES_GCC_Errorcodes& error, std::string caller, std::string username, bool isDirectory)
{
	(void)(generation);
	(void)(username);
	AES_AFP_TRACE_MESSAGE( "Creating file %s in TQ %s, destination set %s, status %d, username %s, generation %s", filename.c_str(), transferQueue.c_str(), destinationSet.c_str(), fileState, username.c_str(),generation.c_str());

	TransferQueueMap::iterator itr;
	bool result(false);
	bool foundOrder(false);
	bool foundDest(false);

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not found", transferQueue.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if (transferQueue == (*itr).second->getName() )
		{
			foundOrder = true;

			if (destinationSet == (*itr).second->getDestinationSet() )
			{
				foundDest = true;
				result = (*itr).second->insertFile(filename, error, fileState, "", filename, caller, isDirectory);
			}
		}
	}

	if (foundOrder ==  true && foundDest == true && result == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to create %s in TQ %s, destination set %s", filename.c_str(), transferQueue.c_str(), destinationSet.c_str());
		AES_AFP_TRACE_MESSAGE("Unable to create %s in TQ %s, destination set %s", filename.c_str(), transferQueue.c_str(), destinationSet.c_str());

		if (foundOrder == false)
		{
			error = AES_NOPROCORDER;
		}
		if (foundDest == false)
		{
			error = AES_NODESTINATION;
		}
	}
	m_synchOnTQMap.release();
	return false;
}

//## Semantics:
//	---------------------------------------------------------
//	       findTransferQueue()
//	---------------------------------------------------------
bool aes_afp_database_fd::findTransferQueue (std::string transferQueue, std::string destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("TQ %s, destination set %s", transferQueue.c_str(), destinationSet.c_str());
	bool foundOrder(false);

	TransferQueueMap::iterator itr;

	m_synchOnTQMap.acquire();

	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ((*itr).second->getName() == transferQueue)
			{
				AES_AFP_TRACE_MESSAGE("TQ %s found", transferQueue.c_str());
				std::string transferQueueDn;
				if( (*itr).second->getTqDn( transferQueue, transferQueueDn ) == true )
				{
					(*itr).second->addTransferQueueInfo(listBuffer);
					listBuffer += transferQueueDn;
					listBuffer += "\n";
					error = AES_NOERRORCODE;
					char tmp[8];
					sprintf(tmp, "%d", ENDOFSTREAM);
					listBuffer += tmp;
					listBuffer += "\n";
					m_synchOnTQMap.release();
					return true;
				}
			}
		}

		//error = AES_NOPROCORDER;
		error = AES_TQ_NOTFOUND;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ((*itr).second->getName() == transferQueue)
		{
			foundOrder = true;
			if ( (*itr).second->getDestinationSet() == destinationSet)
			{
				AES_AFP_TRACE_MESSAGE("TQ %s found, destination set %s", transferQueue.c_str(), destinationSet.c_str());
				std::string transferQueueDn;
				if( (*itr).second->getTqDn( transferQueue, transferQueueDn ) == true )
				{
					(*itr).second->addTransferQueueInfo(listBuffer);
					listBuffer += transferQueueDn;
					listBuffer += "\n";
					error = AES_NOERRORCODE;
					char tmp[8];
					sprintf(tmp, "%d", ENDOFSTREAM);
					listBuffer += tmp;
					listBuffer += "\n";
					m_synchOnTQMap.release();
					return true;
				}
			}
		}
	}
	if (foundOrder == false)
	{
		//error = AES_NOPROCORDER;
		error = AES_TQ_NOTFOUND;
	}
	else
	{
		error = AES_NODESTINATION;
	}
	m_synchOnTQMap.release();
	return false;
	//## end aes_afp_database_fd::findTransferQueue%3B27207501C1.body
}

//## Operation: destroy%3B27207501E9; C++
//## Semantics:
//	---------------------------------------------------------
//	       destroy()
//	---------------------------------------------------------
bool aes_afp_database_fd::destroy (AES_GCC_Errorcodes& error)
{
	//## begin aes_afp_database_fd::destroy%3B27207501E9.body preserve=yes
	AES_AFP_TRACE_MESSAGE( "Removing database");
	TransferQueueMap::iterator itr;
	//shutdown deleteFiles thread
	deleteFilesEv_.signal();
	isShutdownSignaled_ = true;
	if(0 != deleteThreadHandle_)
	{
		ACE_Thread::join(deleteThreadHandle_);
	}
	//HP TR90477
	m_synchOnTQMap.acquire();
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();)
	{
		aes_afp_transferqueue* tempTQ = (*itr).second;
		tempTQ->shutDown();
		transferQueueMap_.erase(itr++);
		delete tempTQ;
		tempTQ = 0;
	}
	m_synchOnTQMap.release();
	error = AES_NOERRORCODE;
	return true;
	//## end aes_afp_database_fd::destroy%3B27207501E9.body
}

//## Operation: addDestinationSet%3B2720750212; C++
//## Semantics:
//	---------------------------------------------------------
//	       addDestinationSet()
//	---------------------------------------------------------
bool aes_afp_database_fd::addDestinationSet (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error)
{
	(void)(username);	
	//## begin aes_afp_database_fd::addDestinationSet%3B2720750212.body preserve=yes
	AES_AFP_TRACE_MESSAGE( "Adding destination set %s to TQ %s, username %s ",
			destinationSet.c_str(), transferQueue.c_str(), username.c_str());
	TransferQueueMap::iterator itr;
	aes_afp_transferqueue* tmpOrderObj;
	std::string tmpOrderName;
	bool result(false);

	tmpOrderName = transferQueue + "_" + destinationSet;

	m_synchOnTQMap.acquire();
	itr = transferQueueMap_.find(tmpOrderName);
	if (itr != transferQueueMap_.end() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Destination set %s already added to TQ %s",
				destinationSet.c_str(), transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("Destination set %s already added to TQ %s",
                                destinationSet.c_str(), transferQueue.c_str());
		error = AES_DESTINATIONEXIST;
		m_synchOnTQMap.release();
		return false;   // Already defined.
	}

	itr = transferQueueMap_.begin();
	while ( itr != transferQueueMap_.end() )
	{
		if ((*itr).second->getName() == transferQueue)
		{
			tmpOrderObj = aes_afp_objectfactory::Instance().makeTransferQueueObject();
			result = tmpOrderObj->init(transferQueue,
					destinationSet,
					(*itr).second->getUserGroup(),
					error,
					false,
					(*itr).second->getTransferQueueState(),
					false );
			if (result == false)
			{
				m_synchOnTQMap.release();
				return false;
			}
			transferQueue += "_";
			transferQueue += destinationSet;
			//TR HJ10892
			for(itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
			{
				if ((*itr).second->getName() == tmpOrderObj->getName())
				{
					tmpOrderObj->firstDs_ = true;
				}
			}
			//End Change for TR HJ10892

			transferQueuePair_.first = transferQueue;
			transferQueuePair_.second = tmpOrderObj;
			transferQueueMap_.insert(transferQueuePair_);
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
		++itr;
	}

	AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found", transferQueue.c_str());
	AES_AFP_TRACE_MESSAGE("TQ %s not found", transferQueue.c_str());
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
	//## end aes_afp_database_fd::addDestinationSet%3B2720750212.body
}

//## Operation: removeDestinationSet%3B272075023A; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeDestinationSet()
//	---------------------------------------------------------
bool aes_afp_database_fd::removeDestinationSet (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error)
{
	(void)(username);
	//## begin aes_afp_database_fd::removeDestinationSet%3B272075023A.body preserve=yes
	AES_AFP_TRACE_MESSAGE( "Destionationset %s, TQ %s, username %s",
			destinationSet.c_str(), transferQueue.c_str(), username.c_str());
	AES_CDH_DestinationSet cdhDest(destinationSet);
	TransferQueueMap::iterator itr;
	bool result(false);
	bool orderFound(false);
	std::string tmpOrderName;
	std::string persistantPath;         //TR_HK91962

	bool setpersistance = false;        //TR_HK91962
	m_synchOnTQMap.acquire();
	tmpOrderName = transferQueue;
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ((*itr).second->getName() == transferQueue)
		{
			orderFound = true;
		}
	}

	if (orderFound == false)
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	transferQueue += "_";
	transferQueue += destinationSet;
	itr = transferQueueMap_.find(transferQueue);

	if (itr != transferQueueMap_.end() )
	{

		if((*itr).second->firstDs_ == false)
		{
			unsigned int tmppersistentNr_ = 0;  //TR_HK91962
			persistantPath = (*itr).second->getTqObjectDirectory();
			persistantPath += aes_afp_parameter::delimiter() + "persist.nr";

			ifstream fin(persistantPath.c_str());
			if(fin.good())
			{
				setpersistance = true;
				fin >> tmppersistentNr_;
				fin.close();
			}

			if(setpersistance)
			{
				result = RetainPersistantNumber(tmppersistentNr_,(*itr).second->getName());
				if (result == false)
				{
					m_synchOnTQMap.release();
					return false;
				}
			}

		}
		result = false;
		result = (*itr).second->removeAll(error);
		if (result == false)
		{
			m_synchOnTQMap.release();
			return false;
		}
		aes_afp_objectfactory::Instance().releaseTransferQueueObject((*itr).second);
		transferQueueMap_.erase(itr);

		m_synchOnTQMap.release();
#if 0 
//Not needed for APG/L
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ((*itr).second->getName() == tmpOrderName)
			{
				protectTransferQueue(tmpOrderName);
			}
		}
#endif
		error = AES_NOERRORCODE;
		return true;
	}
	else if (cdhDest.exists()!=AES_CDH_RC_OK)// Fix for TR HF31769
	{
		error = AES_NODESTINATION;
		m_synchOnTQMap.release();
		return false;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to remove destination set %s, TQ %s not found",
				destinationSet.c_str(), transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("Unable to remove destination set %s, TQ %s not found",
                                destinationSet.c_str(), transferQueue.c_str());
		error = AES_TQDESTSETCOMBINATION;
		m_synchOnTQMap.release();
		return false;
	}
	//## end aes_afp_database_fd::removeDestinationSet%3B272075023A.body
}

//## Operation: changeFileStatus%3B2720750258; C++
//## Semantics:
//	---------------------------------------------------------
//	       changeFileStatus()
//	---------------------------------------------------------
bool aes_afp_database_fd::changeFileStatus (std::string transferQueue, std::string destinationSet, std::string filename, AES_GCC_Filestates newStatus, AES_GCC_Errorcodes& error)
{
	//## begin aes_afp_database_fd::changeFileStatus%3B2720750258.body preserve=yes
	AES_AFP_TRACE_MESSAGE("Changing state for %s, TQ %s, destination set %s, new status %d",
			filename.c_str(), transferQueue.c_str(), destinationSet.c_str(), newStatus);

	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	bool result(false);

	tmpOrderName = transferQueue + "_";
	m_synchOnTQMap.acquire();

	if (destinationSet.empty() )
	{
		for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				tmpOrderName += (*itr).second->getDestinationSet();
				break;
			}
		}
	}
	else
	{
		tmpOrderName += destinationSet;
	}
	AES_AFP_TRACE_MESSAGE("Searching for TQ %s", tmpOrderName.c_str());
	itr = transferQueueMap_.find(tmpOrderName);
	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->changeFileStatus(newStatus, filename, error);
		if (result == false)
		{
			m_synchOnTQMap.release();
			return false;
		}
		m_synchOnTQMap.release();
		return true;
	}

	AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to change status for %s, TQ %s not found", filename.c_str(), transferQueue.c_str());
	AES_AFP_TRACE_MESSAGE("Not able to change status for %s, TQ %s not found", filename.c_str(), transferQueue.c_str());
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
	//## end aes_afp_database_fd::changeFileStatus%3B2720750258.body
}

//## Operation: init%3B272075029E
//## Semantics:
//	---------------------------------------------------------
//	       init()
//	---------------------------------------------------------
bool aes_afp_database_fd::init (AES_GCC_Errorcodes& error)
{
	//## begin aes_afp_database_fd::init%3B272075029E.body preserve=yes
	(void)error;
	AES_AFP_TRACE_MESSAGE( "Initiating database");
	bool result = false;
	std::string dTask("datatask");
	transferQueueMap_.clear();

	if( loadTQsFromIMM() && loadTQsDataFromDisk() )
	{
		//start thread for cleanup expired files
		result = startDeleteFileThread();
	}
	return result;
	//## end aes_afp_database_fd::init%3B272075029E.body
}

//	---------------------------------------------------------
//	       getOneInformation()
//	---------------------------------------------------------
bool aes_afp_database_fd::getOneInformation (std::string transferQueue, std::string& destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error)
{

	AES_AFP_TRACE_MESSAGE("TQ %s", transferQueue.c_str());
	AES_CDH_DestinationSet cdhDest(destinationSet);
	TransferQueueMap::iterator itr;
	TransferQueueMap::iterator fileitr;
	bool foundOne(false);
	std::string tmpOrderName;

	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty())
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "No transfer queues defined");
		AES_AFP_TRACE_MESSAGE("No transfer queues defined");
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ((*itr).second->getName() == transferQueue)
			{
				(*itr).second->fillInfoBuffer(listBuffer);
				foundOne = true;
			}
		}

		if (foundOne == true)
		{
			char tmp[8];
			sprintf(tmp, "%d", ENDOFSTREAM);
			listBuffer += tmp;
			listBuffer += "\n";
			error = AES_NOERRORCODE;

			m_synchOnTQMap.release();
			return true;
		}
		else
		{
			error = AES_NOPROCORDER;
			m_synchOnTQMap.release();
			return false;          
		}
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ((*itr).second->getName() == transferQueue)
		{
			foundOne = true;     
		}
	}

	if (foundOne == false)
	{ 
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpOrderName = transferQueue + "_" + destinationSet;
	itr = transferQueueMap_.find(tmpOrderName);

	if (itr != transferQueueMap_.end() )
	{
		(*itr).second->fillInfoBuffer(listBuffer);
		char tmp[8];
		sprintf(tmp, "%d", ENDOFSTREAM);
		listBuffer += tmp;
		listBuffer += "\n";
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else if (cdhDest.exists()!=AES_CDH_RC_OK)// Fix for TR HF31769
	{
		error = AES_NODESTINATION;
		m_synchOnTQMap.release();
		return false;
	}
	else
	{
		error = AES_TQDESTSETCOMBINATION;
		m_synchOnTQMap.release();
		return false;
	}
}

vector<TqInfo> aes_afp_database_fd::infoAllDestset(std::string TqName)
{
        AES_AFP_TRACE_MESSAGE( "Get info for all associated destsets");

        TransferQueueMap::iterator itr;

        m_synchOnTQMap.acquire();

        vector<TqInfo> destsetList;
        if (transferQueueMap_.empty() )
        {
                AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ defined");
		AES_AFP_TRACE_MESSAGE( "No TQ defined");
                m_synchOnTQMap.release();
                return destsetList;
        }
        for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
        {
               if ((*itr).second->getName() == TqName)
               {
                  (*itr).second->fillDestsetInfo(destsetList);
               }
        }
	m_synchOnTQMap.release();
        return destsetList;
}

//	---------------------------------------------------------
//	       getAllInformation()
//	---------------------------------------------------------
bool aes_afp_database_fd::getAllInformation (std::string& listBuffer, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE( "Get info for all TQ");

	TransferQueueMap::iterator itr;

	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ defined");
		AES_AFP_TRACE_MESSAGE("No TQ defined");
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		(*itr).second->fillInfoBuffer(listBuffer);
	}

	char tmp[8];
	sprintf(tmp, "%d\n", ENDOFSTREAM);
	listBuffer += tmp;
	error = AES_NOERRORCODE;
	m_synchOnTQMap.release();
	return true;
	//## end aes_afp_database_fd::getAllInformation%3B27207502EE.body
}

bool aes_afp_database_fd::getAllInformationShort (std::string& listBuffer, AES_GCC_Errorcodes& error)
{
        AES_AFP_TRACE_MESSAGE("Get info for all TQ");

        TransferQueueMap::iterator itr;

        m_synchOnTQMap.acquire();

        if (transferQueueMap_.empty() )
        {
                AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ defined");
		AES_AFP_TRACE_MESSAGE("No TQ defined");
                error = AES_NOPROCORDER;
                m_synchOnTQMap.release();
                return false;
        }

        for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
        {
                (*itr).second->fillInfoBufferShort(listBuffer);
        }

        char tmp[8];
        sprintf(tmp, "%d\n", ENDOFSTREAM);
        listBuffer += tmp;
        error = AES_NOERRORCODE;
        m_synchOnTQMap.release();
        return true;
        //## end aes_afp_database_fd::getAllInformation%3B27207502EE.body
}
bool aes_afp_database_fd::loadTQsDataFromDisk()
{
	AES_AFP_TRACE_FUNCTION;
	bool result = true;

	for(TransferQueueMap::const_iterator tqMapItr = transferQueueMap_.begin(); tqMapItr != transferQueueMap_.end(); ++tqMapItr)
	{
		// Start thread to load data from disk
		result &= (tqMapItr->second->open() == 0);
	}

	// Wait that all threads loading data
	for(TransferQueueMap::const_iterator tqMapItr = transferQueueMap_.begin(); tqMapItr != transferQueueMap_.end(); ++tqMapItr)
	{
		// wait for thread to load data from disk
		tqMapItr->second->wait();
	}

	return result;
}

bool aes_afp_database_fd::loadTQsFromIMM()
{
	AES_AFP_TRACE_FUNCTION;
	bool result = false;

	aes_afp_protocoltq tmpProtocol;
	TransferQueueMap::iterator itr; //HK91962

	//Get the instances of the class FileStreamPolicy.
	OmHandler myOmHandler;

	if( myOmHandler.Init() == ACS_CC_FAILURE )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Omhandler Initialize failed" );
		AES_AFP_TRACE_MESSAGE("ERROR: Omhandler Initialize failed" );
		return result;
	}

	AES_AFP_LOG(LOG_LEVEL_INFO, "Service is initializing TQs by reading information from IMM");
	AES_AFP_TRACE_MESSAGE("Service is initializing TQs by reading information from IMM");

	//----------------------------------------------------------------------
	// Load and Set the root moc DN into AES_GCC_Util::dnOfFileTransferM
	if((AES_GCC_Util::fetchDnOfFileTransferObjFromIMM(myOmHandler))==-1)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Failed to Initialize fileTransfer object dn name");
		AES_AFP_TRACE_MESSAGE("ERROR: Failed to Initialize fileTransfer object dn name");
		return result;
	}

	//---------------------------------------------
	// Load all defined File Transfer queue MO
	string myClassName = TQ_CL_FILETQ;
	std::vector<std::string> transferQueueDNs;
	myOmHandler.getClassInstances(myClassName.c_str(), transferQueueDNs );

	AES_AFP_LOG(LOG_LEVEL_INFO, "Loaded <%zu> defined File Transfer queues MO <%s>", transferQueueDNs.size(), myClassName.c_str());
	AES_AFP_TRACE_MESSAGE("Loaded <%zu> defined File Transfer queues MO <%s>", transferQueueDNs.size(), myClassName.c_str());

	for (std::vector<std::string>::const_iterator tqDnItr = transferQueueDNs.begin(); (tqDnItr !=  transferQueueDNs.end()) && (aes_afp_services::afpStopEvt == false); ++tqDnItr)
	{
		AES_AFP_LOG(LOG_LEVEL_INFO, "Found File Transfer queues MO <%s>", tqDnItr->c_str());
		AES_AFP_TRACE_MESSAGE("Found File Transfer queues MO <%s>", tqDnItr->c_str());

		//-------------------------
		// Get the TQ name
		string tqName;
		AES_AFP_CommandHandler::extractNameFromDn( *tqDnItr , tqName );

		//Get the list of destination sets Initiating.
		std::vector<ACS_APGCC_ImmAttribute *> initiatingDsAttributes;

		//-----------------------------------------------------------------------------
		// INITIATING DESTINATION SET
		// Load all the initiating destination sets for the current TQ (multi value)
		ACS_APGCC_ImmAttribute attrDestSet1;
		attrDestSet1.attrName = TQ_FILETQINITDEST;
		initiatingDsAttributes.push_back( &attrDestSet1 );

		std::vector<std::string> initiatingDestSetNameList; //hold all the destination set names

		if ( myOmHandler.getAttribute( tqDnItr->c_str(), initiatingDsAttributes ) == ACS_CC_SUCCESS)
		{
			for( unsigned int idx = 0 ; idx < initiatingDsAttributes[0]->attrValuesNum ; idx++)
			{
				std::string dsName(reinterpret_cast<char *>(initiatingDsAttributes[0]->attrValues[idx]));

				AES_AFP_LOG(LOG_LEVEL_INFO, "Found initiating destination <%s> for TQ <%s>", dsName.c_str(), tqName.c_str());
				AES_AFP_TRACE_MESSAGE("Found initiating destination <%s> for TQ <%s>", dsName.c_str(), tqName.c_str());

				initiatingDestSetNameList.push_back(dsName);
			}
		}


		//-------------------------------------------
		// For each initiating destination, assemble the TQ MAP
		for (std::vector<std::string>::const_iterator dsNameItr = initiatingDestSetNameList.begin(); (dsNameItr != initiatingDestSetNameList.end()) && (aes_afp_services::afpStopEvt == false); ++dsNameItr)
		//for ( unsigned int ctr1 = 0 ; ctr1 < initiatingDestSetNameList.size() ; ctr1++ )
		{


			std::string destsetName = *dsNameItr; //initiatingDestSetNameList[ctr1];

			AES_AFP_TRACE_MESSAGE("Adding transfer queue object for destination :<%s>", destsetName.c_str());

			ACS_CC_ImmParameter paramToFind;
			paramToFind.attrName = const_cast<char*>(TQ_FILETQINITDESTID);
			std::string fileDestination(TQ_FILETQINITDESTID);
			fileDestination.push_back('=');
			fileDestination.append(destsetName);
			fileDestination.push_back(',');
			fileDestination.append(AES_GCC_Util::dnOfFileTransferM);

			// Check for inconsistences here: check for initiating destination set present or no (could be removed during service lock)
			int immResult = myOmHandler.getAttribute(fileDestination.c_str(), &paramToFind );
			if ( (immResult == ACS_CC_FAILURE)&&(destsetName!="-"))
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Initiating Destination Set = %s does not exist", destsetName.c_str());
				AES_AFP_TRACE_MESSAGE("ERROR: Initiating Destination Set = %s does not exist", destsetName.c_str());
			}
			else
			{
				aes_afp_transferqueue* tmpFileDestObj = aes_afp_objectfactory::Instance().makeTransferQueueObject();

				if (tmpFileDestObj)
				{
					tmpFileDestObj->setDestSetType(INIT); //set to INITIATING
					tmpFileDestObj->setTqName(tqName);
					tmpFileDestObj->setDestinationName(destsetName);

					result = tmpFileDestObj->initFromIMM(*tqDnItr, tqName, destsetName);
					if( result == false )
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Cannot load IMM data for <%s, %s>", tqName.c_str(), destsetName.c_str());
						AES_AFP_TRACE_MESSAGE("ERROR: Cannot load IMM data for <%s, %s>", tqName.c_str(), destsetName.c_str());
						delete tmpFileDestObj;
						continue;
					}

					//------------------------------------
					// POPULATE THE MAP

					//TR HK91962
					for(TransferQueueMap::const_iterator tqMapItr = transferQueueMap_.begin(); tqMapItr != transferQueueMap_.end(); ++tqMapItr)
					{
						if ((*tqMapItr).second->getName() == tmpFileDestObj->getName())
						{
							tmpFileDestObj->firstDs_ = true;
							break;
						}
					}//End Change for TR HK91962

					std::string key(tqName);
					key.push_back('_');
					key.append(destsetName);

					transferQueueMap_.insert(std::make_pair(key, tmpFileDestObj));

				}
				else
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Memory allocation error for <%s, %s>", tqName.c_str(), destsetName.c_str());
					AES_AFP_TRACE_MESSAGE("ERROR: Memory allocation error for <%s, %s>", tqName.c_str(), destsetName.c_str());
				}
			}
		}


		//--------------------------------------------------------------------
		// RESPONDING

		//Get the list of destination sets of responding.
		std::vector<ACS_APGCC_ImmAttribute *> respondingDsAttributes;

		ACS_APGCC_ImmAttribute attrDestSet2;
		attrDestSet2.attrName = TQ_FILETQRESPDEST ;
		respondingDsAttributes.push_back( &attrDestSet2 );

		std::vector<std::string> respondingDestSetNameList;

		if ( myOmHandler.getAttribute( tqDnItr->c_str(), respondingDsAttributes ) == ACS_CC_SUCCESS)
		{
			for( unsigned int idx = 0 ; idx < respondingDsAttributes[0]->attrValuesNum ; idx++)
			{
				std::string dsName(reinterpret_cast<char *>(respondingDsAttributes[0]->attrValues[idx]));

				AES_AFP_LOG(LOG_LEVEL_INFO, "Found responding destination <%s> for TQ <%s>", dsName.c_str(), tqName.c_str());
				AES_AFP_TRACE_MESSAGE("Found responding destination <%s> for TQ <%s>", dsName.c_str(), tqName.c_str());

				respondingDestSetNameList.push_back(dsName);
			}
		}

		//-------------------------------------------
		// For each initiating destination, assemble the TQ MAP
		for (std::vector<std::string>::const_iterator dsNameItr = respondingDestSetNameList.begin(); (dsNameItr != respondingDestSetNameList.end()) && (aes_afp_services::afpStopEvt == false); ++dsNameItr)
		{
			std::string destsetName = *dsNameItr;

			ACS_CC_ImmParameter paramToFind;
			paramToFind.attrName = const_cast<char*>(TQ_FILETQRESPDESTID);
			std::string fileDestination(TQ_FILETQRESPDESTID);
			fileDestination.push_back('=');
			fileDestination.append(destsetName);
			fileDestination.push_back(',');
			fileDestination.append(AES_GCC_Util::dnOfFileTransferM);

			int immResult = myOmHandler.getAttribute(fileDestination.c_str(), &paramToFind );
			if ( (ACS_CC_FAILURE == immResult) && (destsetName != "-") )
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR,"Responding Destination Set = %s does not exist", destsetName.c_str());
				AES_AFP_TRACE_MESSAGE("Responding Destination Set = %s does not exist", destsetName.c_str());
			}
			else
			{

				aes_afp_transferqueue* tmpFileDestObj = aes_afp_objectfactory::Instance().makeTransferQueueObject();

				if (tmpFileDestObj)
				{
					tmpFileDestObj->setDestSetType(RESP); //set RESPONDING
					tmpFileDestObj->setTqName(tqName);
					tmpFileDestObj->setDestinationName(destsetName);

					result = tmpFileDestObj->initFromIMM(*tqDnItr, tqName, destsetName);
					if( result == false )
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Cannot load IMM data for <%s, %s>", tqName.c_str(), destsetName.c_str());
						AES_AFP_TRACE_MESSAGE("ERROR: Cannot load IMM data for <%s, %s>", tqName.c_str(), destsetName.c_str());
						delete tmpFileDestObj;
						continue;
					}

					//------------------------------------
					// POPULATE THE MAP

					//TR HK91962
					for(TransferQueueMap::const_iterator tqMapItr = transferQueueMap_.begin(); tqMapItr != transferQueueMap_.end(); ++tqMapItr)
					{
						if ((*tqMapItr).second->getName() == tmpFileDestObj->getName())
						{
							tmpFileDestObj->firstDs_ = true;
							break;
						}
					}//End Change for TR HK91962

					std::string key(tqName);
					key.push_back('_');
					key.append(destsetName);

					transferQueueMap_.insert(std::make_pair(key, tmpFileDestObj));
				}
				else
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR,"Memory allocation error for <%s, %s>", tqName.c_str(), destsetName.c_str());
					AES_AFP_TRACE_MESSAGE("ERROR: Memory allocation error for <%s, %s>", tqName.c_str(), destsetName.c_str());
				}
			}
		}

	}

	myOmHandler.Finalize();

	//Remove the removed TQ_Dest directories from data path during node restore
	string tqDataPath = aes_afp_parameter::dataDir();
	deleteTQDirIfNotExistsInIMM(tqDataPath);

	AES_AFP_LOG(LOG_LEVEL_INFO, "Initializing TQs is completed !!");
	return true;
}

//	---------------------------------------------------------
//	       syncProtectedTransferQueues()
//	---------------------------------------------------------
bool aes_afp_database_fd::syncProtectedTransferQueues()
{
	std::string transferQueueNames;

	// We do not know in advance how much memory is required to hold
	// all destinationset-names.
	size_t bytesNeeded = 0;

	size_t semiColons = 0;

	// Iterate through the map and create a string containing all
	// transferqueue-names. End each transferqueue-name with a
	// semi-colon!
	m_synchOnTQMap.acquire();
	TransferQueueMap::iterator itr = transferQueueMap_.begin();
	while (itr != transferQueueMap_.end())
	{
		// Transferqueue part
		aes_afp_transferqueue *tq = (*itr).second;
		transferQueueNames.append(tq->getName());
		transferQueueNames.append(";");

		// Destinationset part
		bytesNeeded += tq->getDestinationSet().length();
		semiColons  += 1;

		++itr;
	}

	// Synchronize protected transferqueue-names
	const std::string subSys    = "aes";
	const std::string blockName = "afp";
	//    AES_GCC_FileDestProtection transferQueueProtection(subSys, blockName);
	//   int tqResult = transferQueueProtection.synchronize(transferQueueNames);


	//
	// Synchronize protected destinationset-names.
	//

	// Make sure to include space for semi-colons
	bytesNeeded += semiColons;

	// Make sure to include space for the terminating '\0';
	bytesNeeded += 1;

	// Allocate memory to hold all destinationset-names
	char *destinationSetNames = new char[bytesNeeded];

	if (destinationSetNames != NULL)
	{
		// Terminator required by 'strcat' below!
		destinationSetNames[0] = '\0';

		// Create the array of destinationset-names
		itr = transferQueueMap_.begin();
		while (itr != transferQueueMap_.end())
		{
			aes_afp_transferqueue *tq = (*itr).second;
			const std::string destSetName = tq->getDestinationSet();
			(void)::strcat(destinationSetNames, destSetName.c_str());
			(void)::strcat(destinationSetNames, ";");
			++itr;
		}

		// AES_GCC_NameProtection destinationSetProtection("AFP", AES_CDH_NameProtectionType);
		// int destSetResult = destinationSetProtection.synchronize(destinationSetNames);
	}
	else
	{
		// Memory allocation failed!!
	}

	delete[] destinationSetNames;
	destinationSetNames = NULL;
	m_synchOnTQMap.release();

	return false;
}


//	---------------------------------------------------------
//	       deleteFiles()
//	---------------------------------------------------------
void aes_afp_database_fd::deleteFiles ()
{
	TransferQueueMap::iterator itr;
	AES_GCC_Errorcodes code;
	bool result(false);
	m_synchOnTQMap.acquire();
	for (itr = transferQueueMap_.begin();(!isShutdownSignaled_)&&(transferQueueMap_.end() != itr);++itr)
	{
		result = (*itr).second->deleteFiles(code, deleteMap_);
		if (result == true)
		{
			//xcsrajm For TR HH54446 changed removedestset 3rd param from "" to uname
			if ( (*itr).second->getManual() == true)
			{
				removeDestinationSet((*itr).second->getName(),
						(*itr).second->getDestinationSet(), uname, code);
				if (transferQueueMap_.empty() )
				{
					m_synchOnTQMap.release();
					return;
				}
				itr = transferQueueMap_.begin();
			}
		}
	}
	m_synchOnTQMap.release();
}

//	---------------------------------------------------------
//	       getFileStatus()
//	---------------------------------------------------------
bool aes_afp_database_fd::getFileStatus (std::string transferQueue,
		std::string destinationSet,
		std::string filename,
		aes_afp_datablock* dBlock,
		AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	bool result(false);
	bool orderFound(false);

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{
			orderFound = true;
			tmpOrderName = transferQueue;
			tmpOrderName += "_";
			break;
		}
	}

	if (orderFound == false)
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	if (destinationSet.empty() )
	{
		tmpOrderName += (*itr).second->getDestinationSet();

	}
	else
	{

		tmpOrderName += destinationSet;
	}

	itr = transferQueueMap_.find(tmpOrderName);

	if (itr == transferQueueMap_.end() )
	{
		error = AES_NODESTINATION;
		m_synchOnTQMap.release();
		return false;
	}

	result = (*itr).second->getFileStatus(filename, dBlock, error);
	if (result == false)
	{
		m_synchOnTQMap.release();
		return false;
	}

	error = AES_NOERRORCODE;
	m_synchOnTQMap.release();
	return true;
}

//	---------------------------------------------------------
//	       getStatusOldFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::getStatusOldFile (std::string transferQueue,
		std::string destinationSet,
		aes_afp_datablock* dBlock,
		AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("TQ %s, destinationset %s", transferQueue.c_str(), destinationSet.c_str());

	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	bool result(false);

	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ defined");
		AES_AFP_TRACE_MESSAGE("No TQ defined");
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	if (destinationSet.empty() == true)
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ((*itr).second->getName() == transferQueue)
			{
				result = (*itr).second->getOldestFileStatus(dBlock, error);
				if (result == false)
				{
					m_synchOnTQMap.release();
					return false;
				}

				error = AES_NOERRORCODE;
				m_synchOnTQMap.release();
				return true;
			}
		}

		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not found", transferQueue.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	else
	{
		tmpOrderName = transferQueue + "_" + destinationSet;
		itr = transferQueueMap_.find(tmpOrderName);
		if (itr == transferQueueMap_.end() )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ %s defined to destinationset %s", transferQueue.c_str(), destinationSet.c_str());
			AES_AFP_TRACE_MESSAGE("No TQ %s defined to destinationset %s", transferQueue.c_str(), destinationSet.c_str());
			error = AES_NOPROCORDER;
			m_synchOnTQMap.release();
			return false;
		}
		else
		{
			result = (*itr).second->getOldestFileStatus(dBlock, error);
			if (result ==false)
			{
				m_synchOnTQMap.release();
				return false;
			}
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
	}

}

//	---------------------------------------------------------
//	       getStatusNewFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::getStatusNewFile (std::string transferQueue,
		std::string destinationSet,
		aes_afp_datablock* dBlock,
		AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("TQ %s, destinationset %s", transferQueue.c_str(), destinationSet.c_str());
	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	bool result(false);

	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ defined");
		AES_AFP_TRACE_MESSAGE("No TQ defined");
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	if (destinationSet.empty() == true)  // if no destination get the first process order that
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ((*itr).second->getName() == transferQueue)
			{
				result = (*itr).second->getYoungestFileStatus(dBlock, error);
				if (result == false)
				{
					m_synchOnTQMap.release();
					return false;
				}

				error = AES_NOERRORCODE;
				m_synchOnTQMap.release();
				return true;
			}
		}
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not defined", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not defined", transferQueue.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	else
	{
		tmpOrderName = transferQueue + "_" + destinationSet;
		itr = transferQueueMap_.find(tmpOrderName);
		if (itr == transferQueueMap_.end() )
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ %s defined to destination set %s", transferQueue.c_str(), destinationSet.c_str());
			AES_AFP_TRACE_MESSAGE("No TQ %s defined to destination set %s", transferQueue.c_str(), destinationSet.c_str());
			error = AES_NOPROCORDER;
			m_synchOnTQMap.release();
			return false;
		}
		else
		{
			result = (*itr).second->getYoungestFileStatus(dBlock, error);
			if (result == false)
			{
				m_synchOnTQMap.release();
				return false;
			}
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
	}
}

//	---------------------------------------------------------
//	       removeFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::removeFile (std::string transferQueue,
		std::string destinationSet,
		std::string filename,
		std::string username,
		AES_GCC_Errorcodes& error)
{
	(void)(username);
	AES_AFP_TRACE_MESSAGE( "Remove file %s from TQ %s, destinationset %s, username %s ", filename.c_str(), transferQueue.c_str(), destinationSet.c_str(),username.c_str());
	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	bool result(false);
	bool orderFound(false);
	bool sendItemFound(false);

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ defined");
		AES_AFP_TRACE_MESSAGE("No TQ defined");
		orderFound = false;
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end(); ++itr)
		{
			if ((*itr).second->getName() == transferQueue)
			{

				orderFound = true;
				result = (*itr).second->removeFile(filename, error);

				if (result == false)
				{
					switch (error)
					{
					case AES_NOACCESS:
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, no access", filename.c_str());
						AES_AFP_TRACE_MESSAGE("Not able to remove %s, no access", filename.c_str());
						m_synchOnTQMap.release();
						return false;
					}
					case AES_FILEISPROTECTED:
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, TQ is protected", filename.c_str());
						AES_AFP_TRACE_MESSAGE("Not able to remove %s, TQ is protected", filename.c_str());
						m_synchOnTQMap.release();
						return false;
					}
					default: 
					{
					}
					}
				}
				else
				{
					sendItemFound = true;
				}
			} // if
		} // for

		if (orderFound == false)
		{
			error = AES_NOPROCORDER;
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, TQ not found", filename.c_str());
			AES_AFP_TRACE_MESSAGE("Not able to remove %s, TQ not found", filename.c_str());
			m_synchOnTQMap.release();
			return false;
		}
		if (sendItemFound == false)
		{
			error = AES_SENDITEMNOTREP;
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, file not found", filename.c_str());
			AES_AFP_TRACE_MESSAGE("Not able to remove %s, file not found", filename.c_str());
			m_synchOnTQMap.release();
			return false;
		}
	}
	else
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ((*itr).second->getName() == transferQueue)
			{

				orderFound = true;
			}
		}

		if (orderFound == false)
		{
			error = AES_NOPROCORDER;
			m_synchOnTQMap.release();
			return false;
		}

		tmpOrderName = transferQueue + "_" + destinationSet;
		itr = transferQueueMap_.find(tmpOrderName);
		if (itr != transferQueueMap_.end() )
		{
			result = (*itr).second->removeFile(filename, error);

			if (result == false)
			{
				switch (error)
				{  
				case AES_NOACCESS:
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "No access to file %s", filename.c_str());
					AES_AFP_TRACE_MESSAGE("No access to file %s", filename.c_str());
					// Send event not able to remove send items.
					break;
				}
				case AES_FILENOTFOUND:
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s not found", filename.c_str());
					AES_AFP_TRACE_MESSAGE("File %s not found", filename.c_str());
					break;
				}
				case AES_SENDITEMNOTREP:
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s not reported", filename.c_str());
					AES_AFP_TRACE_MESSAGE("File %s not reported", filename.c_str());
					break;
				}
				default:
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Default reached");
					AES_AFP_TRACE_MESSAGE("Default reached");
					break;
				}
				}
				m_synchOnTQMap.release();
				return false;
			}
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not combined with destination set %s", transferQueue.c_str(), destinationSet.c_str());
			AES_AFP_TRACE_MESSAGE("TQ %s not combined with destination set %s", transferQueue.c_str(), destinationSet.c_str());
			error = AES_NODESTINATION;
			m_synchOnTQMap.release();
			return false;
		}
	}

	m_synchOnTQMap.release();
	error = AES_NOERRORCODE;
	return true;
}

//	---------------------------------------------------------
//	       transferQueueDefined()
//	---------------------------------------------------------
bool aes_afp_database_fd::transferQueueDefined (std::string transferQueue,
		AES_GCC_Errorcodes& error)
{
	m_synchOnTQMap.acquire();
	TransferQueueMap::iterator itr;
	for (itr=transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ((*itr).second->getName() == transferQueue)
		{
			AES_AFP_TRACE_MESSAGE("TQ %s is defined", transferQueue.c_str());
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
	}

	AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s is not defined", transferQueue.c_str());
	AES_AFP_TRACE_MESSAGE("TQ %s is not defined", transferQueue.c_str());
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//## Operation: transferQueueDestinationSetDefined%3B2720760050; C++
//## Semantics:
//	---------------------------------------------------------
//	       transferQueueDestinationSetDefined()
//	---------------------------------------------------------
bool aes_afp_database_fd::transferQueueDestinationSetDefined (std::string transferQueue, std::string destinationSet, AES_GCC_Errorcodes& error)
{
	//## begin aes_afp_database_fd::transferQueueDestinationSetDefined%3B2720760050.body preserve=yes
	m_synchOnTQMap.acquire();
	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	tmpOrderName = transferQueue + "_" + destinationSet;
	itr = transferQueueMap_.find(tmpOrderName);
	if (itr != transferQueueMap_.end() )
	{
		AES_AFP_TRACE_MESSAGE("TQ %s, destination set %s is defined",
                                transferQueue.c_str(), destinationSet.c_str());
		m_synchOnTQMap.release();
		error = AES_NOERRORCODE;
		return true;
	}
	AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s, destination set %s is not defined",
			transferQueue.c_str(), destinationSet.c_str());
	AES_AFP_TRACE_MESSAGE("TQ %s, destination set %s is not defined",
                        transferQueue.c_str(), destinationSet.c_str());
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
	//## end aes_afp_database_fd::transferQueueDestinationSetDefined%3B2720760050.body
}

//## Operation: getTransferQueueAttributes%3B2720760078; C++
//## Semantics:
//	---------------------------------------------------------
//	       getTransferQueueAttributes()
//	---------------------------------------------------------
bool aes_afp_database_fd::getTransferQueueAttributes (std::string transferQueue, std::string destinationSet, aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("TQ %s destination set %s", transferQueue.c_str(), destinationSet.c_str());

	m_synchOnTQMap.acquire();

	TransferQueueMap::iterator itr;
	AES_CDH_DestinationSet cdhDest(destinationSet); 
	std::string tmpOrderDest;
	bool foundOne(false); 

	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found, map is empty", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not found, map is empty", transferQueue.c_str());
		m_synchOnTQMap.release();
		error = AES_NOPROCORDER;
		return false;
	}

	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				foundOne = true;
				(*itr).second->getAttributes(dataBlock);
				break;
			}
		}
	}
	else if (destinationSet != "-" && cdhDest.exists()!=AES_CDH_RC_OK)// Fix for TR HF31769
	{                                                              //Fix for TR HG99088    
		error = AES_NODESTINATION;
		m_synchOnTQMap.release();
		return false;
	}
	else
	{
		tmpOrderDest = transferQueue + "_" + destinationSet;
		itr = transferQueueMap_.find(tmpOrderDest);
		if (itr != transferQueueMap_.end() )
		{
			foundOne = true;
			(*itr).second->getAttributes(dataBlock);
		}
		else
		{     // Fix for TR HE58211 merged in from TR branch  
			for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
			{
				if ( (*itr).second->getName() == transferQueue)
				{
					// Found the order, missing destination
					error = AES_TQDESTSETCOMBINATION;
					m_synchOnTQMap.release();
					return false;
				}
			}
		}

	}
	if (foundOne == true)
	{
		AES_AFP_TRACE_MESSAGE("TQ %s found", transferQueue.c_str());
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found", transferQueue.c_str());
	AES_AFP_TRACE_MESSAGE("TQ %s not found", transferQueue.c_str());
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       setTransferQueueAttributes()
//	---------------------------------------------------------
bool aes_afp_database_fd::setTransferQueueAttributes (std::string transferQueue,
		std::string destinationSet,
		std::string username,
		AES_GCC_Errorcodes& error,
		int sendRetries,
		unsigned int removeDelay,
		AES_GCC_Filestates defaultState,
		int retryDelayTime,
		std::string renameTemplate,
		std::string nameTag,
		long startupSequenceNumber, //HT50930
		bool removeDelayBefore,
		bool changeRenameTemplate)
{
	(void)(username);	
	AES_AFP_TRACE_MESSAGE("TQ %s destination set %s, username %s , renameTemplate %s, nameTag %s, startupSequenceNumber %ld", transferQueue.c_str(), destinationSet.c_str(), username.c_str(), renameTemplate.c_str(), nameTag.c_str(), startupSequenceNumber); //HT50930

	m_synchOnTQMap.acquire();

	TransferQueueMap::iterator itr;
	bool result(false);
	std::string tmpOrderDest;
	bool foundOne(false);

	if (transferQueueMap_.empty() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found, map is empty", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not found, map is empty", transferQueue.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				foundOne = true;

				result = (*itr).second->setAttributes(sendRetries, removeDelay, defaultState, retryDelayTime, removeDelayBefore, renameTemplate, nameTag, startupSequenceNumber, error, changeRenameTemplate); //HT50930
				if (result == false)
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to set attributes");
					AES_AFP_TRACE_MESSAGE("Not able to set attributes");
					m_synchOnTQMap.release();
					return result;
				}
			}
		}
	}
	else
	{
		tmpOrderDest = transferQueue + "_" + destinationSet;
		itr = transferQueueMap_.find(tmpOrderDest);
		if (itr != transferQueueMap_.end() )
		{
			foundOne = true;
			result = (*itr).second->setAttributes(sendRetries,
					removeDelay,
					defaultState,
					retryDelayTime,
					removeDelayBefore,
					renameTemplate,
					nameTag,
					startupSequenceNumber,  //HT50930
					error,
					changeRenameTemplate);
			if (result == false)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to change attributes");
				AES_AFP_TRACE_MESSAGE("Not able to change attributes");
				m_synchOnTQMap.release();
				return result;
			}
		}
	}
	if (foundOne == true)
	{
		AES_AFP_TRACE_MESSAGE("Attributes changed");
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found", transferQueue.c_str());
	AES_AFP_TRACE_MESSAGE( "TQ %s not found", transferQueue.c_str());
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       infoExactFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::infoExactFile (std::string transferQueue,
		std::string destinationSet,
		std::string filename,
		std::string& listBuffer,
		AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE( "File %s TQ %s destination set %s", filename.c_str(), transferQueue.c_str(), destinationSet.c_str());

	TransferQueueMap::iterator itr;
	std::string tmpStr;
	bool result(false);

	tmpStr = transferQueue + "_" + destinationSet;

	m_synchOnTQMap.acquire();
	itr = transferQueueMap_.find(tmpStr);
	if (itr == transferQueueMap_.end() )
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "File %s not found", filename.c_str());
		AES_AFP_TRACE_MESSAGE("File %s not found", filename.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	result = (*itr).second->findExactFile(filename, listBuffer, error);
	char tmp[8];
	sprintf(tmp, "%d", ENDOFSTREAM);
	listBuffer += tmp;
	listBuffer += "\n";
	if (result ==  false)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to collect");
		AES_AFP_TRACE_MESSAGE("Not able to collect");
		m_synchOnTQMap.release();
		return false;
	}
	m_synchOnTQMap.release();
	return true;
}

//	---------------------------------------------------------
//	       infoOldestFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::infoOldestFile (std::string transferQueue, std::string destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	std::string tmpOrderDest;
	bool result(false);
	AES_CDH_DestinationSet cdhDest(destinationSet);
	m_synchOnTQMap.acquire();
	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				result = (*itr).second->findOldestFile(listBuffer, error);
				char tmp[8];
				sprintf(tmp, "%d", ENDOFSTREAM);
				listBuffer += tmp;
				listBuffer += "\n";
				if (result == true)
				{
					m_synchOnTQMap.release();
					return true;
				}
				else
				{
					error = AES_FILENOTFOUND;
					m_synchOnTQMap.release();
					return false;
				}
			}
		}
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpOrderDest = transferQueue + "_" + destinationSet;
	itr = transferQueueMap_.find(tmpOrderDest);
	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->findOldestFile(listBuffer, error);
		if (result == true)
		{
			char tmp[8];
			sprintf(tmp, "%d", ENDOFSTREAM);
			listBuffer += tmp;
			listBuffer += "\n";
			m_synchOnTQMap.release();
			return true;
		}
		else
		{
			m_synchOnTQMap.release();
			return false;
		}
	}
	else if (cdhDest.exists()!=AES_CDH_RC_OK)// Fix for TR HF31769
	{
		error = AES_NODESTINATION;
		m_synchOnTQMap.release();
		return false;
	}
	else
	{
		for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				// Found the order, missing destination
				error = AES_TQDESTSETCOMBINATION;
				m_synchOnTQMap.release();
				return false;
			}
		}
	}
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       infoNewestFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::infoNewestFile (std::string transferQueue, std::string destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	std::string tmpOrderDest;
	bool result(false);
	AES_CDH_DestinationSet cdhDest(destinationSet);
	m_synchOnTQMap.acquire();
	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				result = (*itr).second->findNewestFile(listBuffer, error);
				if (result == true)
				{
					char tmp[8];
					sprintf(tmp, "%d", ENDOFSTREAM);
					listBuffer += tmp;
					listBuffer += "\n";
					m_synchOnTQMap.release();
					return true;
				}
				else
				{
					m_synchOnTQMap.release();
					return false;
				}
			}
		}
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpOrderDest = transferQueue + "_" + destinationSet;
	itr = transferQueueMap_.find(tmpOrderDest);
	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->findNewestFile(listBuffer, error);
		if (result == true)
		{
			char tmp[8];
			sprintf(tmp, "%d", ENDOFSTREAM);
			listBuffer += tmp;
			listBuffer += "\n";
			m_synchOnTQMap.release();
			return true;
		}
		else
		{
			m_synchOnTQMap.release();
			return false;
		}
	}
	else if (cdhDest.exists()!=AES_CDH_RC_OK)// Fix for TR HF31769
	{
		error = AES_NODESTINATION;
		m_synchOnTQMap.release();
		return false;
	}
	else
	{
		for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				// Found the order, missing destination
				error = AES_TQDESTSETCOMBINATION;
				m_synchOnTQMap.release();
				return false;
			}
		}
	}

	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       stopSendFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::stopSendFile (std::string transferQueue, std::string destinationSet, std::string filename, std::string username, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	std::string tmpOrderDest;
	bool result(false);

	tmpOrderDest = transferQueue + "_" + destinationSet;

	m_synchOnTQMap.acquire();
	itr = transferQueueMap_.find(tmpOrderDest);
	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->stopSendFile(filename, error);
		if (result == true)
		{
			AES_AFP_TRACE_MESSAGE("File %s in TQ %s is stopped username %s ", filename.c_str(), transferQueue.c_str(), username.c_str());
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
		else
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Unable to stop file %s in TQ %s",
					filename.c_str(), transferQueue.c_str());
			AES_AFP_TRACE_MESSAGE("Unable to stop file %s in TQ %s",
                                        filename.c_str(), transferQueue.c_str());
			m_synchOnTQMap.release();
			return false;
		}
	}

	else
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s has no destinationset %s", transferQueue.c_str(), destinationSet.c_str());
				AES_AFP_TRACE_MESSAGE("TQ %s has no destinationset %s", transferQueue.c_str(), destinationSet.c_str());
				error = AES_TQDESTSETCOMBINATION;
				m_synchOnTQMap.release();
				return false;
			}
		}
	}

	AES_AFP_TRACE_MESSAGE( "Unable to stop file %s, TQ %s not found",
			filename.c_str(), transferQueue.c_str());

	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//## Semantics:
//	---------------------------------------------------------
//	       stopSendAll()
//	---------------------------------------------------------
bool aes_afp_database_fd::stopSendAll (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error)
{
	(void)(username);
	TransferQueueMap::iterator itr;
	std::string tmpOrderDest;
	bool result(false);
	bool foundOne(false);
	m_synchOnTQMap.acquire();
	AES_AFP_TRACE_MESSAGE( "username %s",username.c_str());
	if (destinationSet.empty() )
	{
		for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				foundOne = true;
				(*itr).second->stopSendAll(error);
			}
		}
	}
	else
	{
		tmpOrderDest = transferQueue + "_" + destinationSet;
		itr = transferQueueMap_.find(tmpOrderDest);
		if (itr != transferQueueMap_.end() )
		{
			foundOne = true;
			result = (*itr).second->stopSendAll(error);
			if (result == true)
			{
				m_synchOnTQMap.release();
				return true;
			}
			else
			{
				m_synchOnTQMap.release();
				return false;
			}
		}
	}
	if (foundOne == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}

	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//## Semantics:
//	---------------------------------------------------------
//	       getInformationDestinationSets()
//	---------------------------------------------------------
bool aes_afp_database_fd::getInformationDestinationSets (std::string transferQueue, std::string& streamBuffer, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("TQ %s", transferQueue.c_str());
	char tmpBuf[16];

	TransferQueueMap::iterator itr;
	bool foundOne(false);

	m_synchOnTQMap.acquire();
	streamBuffer = "";

	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}


	sprintf(tmpBuf, "%d", PROCORDERSTART);
	streamBuffer += tmpBuf;
	streamBuffer += "\n";

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{
			foundOne = true;
			AES_AFP_TRACE_MESSAGE("Adding %s",(*itr).second->getDestinationSet().c_str() );
			streamBuffer += (*itr).second->getDestinationSet();
			streamBuffer += "\n";
		}
	}
	if (foundOne == true)
	{
		sprintf(tmpBuf, "%d", ENDOFSTREAM);
		streamBuffer += tmpBuf;
		streamBuffer += "\n";
		error = AES_NOERRORCODE;

		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
}

//	---------------------------------------------------------
//	       lockTransferQueue()
//	---------------------------------------------------------
bool aes_afp_database_fd::lockTransferQueue (std::string transferQueue, std::string destinationSet, std::string application, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("TQ %s, destinationset %s, application %s", transferQueue.c_str(), destinationSet.c_str(), application.c_str());
	TransferQueueMap::iterator itr;
	bool result(false);
	std::string tmpString;

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpString = transferQueue + "_" + destinationSet;

	itr = transferQueueMap_.find(tmpString);
	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->lockTransferQueue(application, error);
	}
	else
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	if (result == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		error = AES_UNABLETOLOCK;
	}
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       unlockTransferQueue()
//	---------------------------------------------------------
bool aes_afp_database_fd::unlockTransferQueue (std::string transferQueue, std::string destinationSet, std::string application, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("Unlock TQ %s, destinationset %s, application %s", transferQueue.c_str(), destinationSet.c_str(), application.c_str());
	TransferQueueMap::iterator itr;
	bool result(false);
	std::string tmpString;
	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpString = transferQueue + "_" + destinationSet;

	itr = transferQueueMap_.find(tmpString);
	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->unlockTransferQueue(application, error);
	}
	else
	{
		error = AES_NOPROCORDER;
	}

	if (result == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		error = AES_UNABLETOUNLOCK;
	}
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       isTransferQueueLocked()
//	---------------------------------------------------------
bool aes_afp_database_fd::isTransferQueueLocked (std::string transferQueue, std::string destinationSet, std::string& application, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE( "TQ %s, destination set %s", transferQueue.c_str(), destinationSet.c_str());
	TransferQueueMap::iterator itr;
	bool result(false);
	std::string tmpString;

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpString = transferQueue + "_" + destinationSet;

	itr = transferQueueMap_.find(tmpString);
	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->isTransferQueueLocked(application);
	}
	else
	{
		error = AES_NOPROCORDER;
	}

	if (result == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		error = AES_NOERRORCODE;
	}
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       deletePhysFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::deletePhysFile (const std::string& sourceFile)
{
	TransferQueueMap::iterator itr;
	std::string tmpFileName;
	std::string tmpSendItem;
	std::string tmpAppDirectory;
	size_t index;
	bool searchItem(false);
	index = sourceFile.find_last_of('/');
	if (std::string::npos != index)
	{
		tmpAppDirectory = sourceFile.substr(0, index);
		tmpSendItem = sourceFile.substr(index+1);
	}
	else
	{
		tmpSendItem = sourceFile;
	}
	// Check if there is a send item that is not deletable in database
	// Changes for TR HD52107: Before no check was made if
	// the same file name was in different application directories, eg
	// not the same file. Now the application dir and the search path to
	// file is controlled, if its not the same, the file can be removed.

	m_synchOnTQMap.acquire();
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		searchItem = (*itr).second->findExactFile(tmpSendItem);
		if (searchItem == true)
		{
			AES_AFP_TRACE_MESSAGE("Found file %s in TQ %s", tmpSendItem.c_str(), (*itr).second->getName().c_str() );
			if (tmpAppDirectory == (*itr).second->getDirectory() )
			{
				AES_AFP_TRACE_MESSAGE("File exist for other TQ");
				break;
			}
			else
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Found filename but not the same appl.directory, file will be removed");
				AES_AFP_TRACE_MESSAGE( "Found filename but not the same appl.directory, file will be removed");
				searchItem = false;
			}
		}
	}
	m_synchOnTQMap.release();

	if (searchItem == false)
	{
		removeExpiredFile(sourceFile);
	}
	return true;
}
//	---------------------------------------------------------
//	       protectTransferQueuej()
//	---------------------------------------------------------
bool aes_afp_database_fd::protectTransferQueue (std::string transferQueue)
{
	AES_AFP_TRACE_MESSAGE("TQ %s",transferQueue.c_str());

	m_synchOnTQMap.acquire();
	if (transferQueueProtected(transferQueue) == false)
	{ 
	}
	m_synchOnTQMap.release();
	return true;
}

//	---------------------------------------------------------
//	       unprotectTransferQueue()
//	---------------------------------------------------------

void aes_afp_database_fd::unprotectTransferQueue (std::string transferQueue)
{
	(void)(transferQueue);
	AES_AFP_TRACE_MESSAGE("TQ %s",transferQueue.c_str());
	if (transferQueueProtected(transferQueue) == true)
	{
		// AES_GCC_FileDestProtection fileDestProtect("AES", "AFP");
		//fileDestProtect.Unprotect(transferQueue);
	}
}

//	---------------------------------------------------------
//	       transferQueueProtected()
//	---------------------------------------------------------
bool aes_afp_database_fd::transferQueueProtected (std::string transferQueue)
{
	(void)(transferQueue);
	bool result(true);

	//AES_GCC_FileDestProtection fileDestProtect("AES", "AFP");

	//result = fileDestProtect.isProtected(transferQueue);

	AES_AFP_TRACE_MESSAGE("%s result %d",transferQueue.c_str(),result);
	return result;
}

//	---------------------------------------------------------
//	       rewriteRootFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::rewriteRootFile (std::string& skipThisLine) const
{
	//RootFile has been removed in APG43L.
	(void) skipThisLine;
	return true;
}

//	-------------------------------------------------
//	       resendOne()
//	-------------------------------------------------
bool aes_afp_database_fd::resendOne (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error, std::string newDestinationSet)
{
	(void)(username);
	bool result(false);
	bool foundOne(false);
	std::string tmpOrderString;
	std::string tmpAppDir;
	std::string tmpTemplate;
	std::string tmpNameTag;
	bool tmpIsDir(false);
	char tmpSenditem[128];
	char tmpSendGen[128];
	TransferQueueMap::iterator itr;
	TransferQueueMap::iterator tqItr;
	std::stringstream streamBuffer;
	std::string tmpUserGroup;

	m_synchOnTQMap.acquire();
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if (transferQueue == (*itr).second->getName() )
		{
			foundOne = true;
			break;
		}
	}

	if (foundOne == false)
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpOrderString = transferQueue + "_" + destinationSet;

	itr = transferQueueMap_.find(tmpOrderString);
	if (itr != transferQueueMap_.end() )
	{
		/*	if (checkSecurity(username, (*itr).second->getUserGroup() ) == false)
				{
					error = AES_NOTAUTHORIZED;
					m_synchOnTQMap.release();
					return false;
				}*/

		if (newDestinationSet == "")
		{
			result = (*itr).second->transferFailed(error);
		}
		else
		{
			AES_AFP_Renametypes tmpRename(AES_NONE);
			tmpAppDir = (*itr).second->getDirectory();
			tmpTemplate = (*itr).second->getTemplate();
			tmpRename = (*itr).second->getRenameFile();
			tmpUserGroup = (*itr).second->getUserGroup();
			tmpNameTag = (*itr).second->getNameTag();
			int tmpStartupSequenceNumber = (*itr).second->getStartupSequenceNumber();

			result = createTransferQueue(transferQueue,
					newDestinationSet,
					error,
					tmpAppDir,
					AES_FSREADY,
					10,
					0,
					10,
					0,
					tmpRename,
					tmpTemplate,
					tmpUserGroup,
					tmpNameTag,
					tmpStartupSequenceNumber);

			if (result == false)
			{
				m_synchOnTQMap.release();
				return false;
			}


			setTransferQueueManually(transferQueue, newDestinationSet);

			result = (*itr).second->getSendItemNames(streamBuffer, AES_FSFAILED);
			if (result == false)
			{
				error = AES_FILENOTFOUND;
				m_synchOnTQMap.release();
				return false;
			}
			streamBuffer.getline(tmpSenditem,128, '\n');

			while (streamBuffer.good() )
			{
				tmpIsDir = (*itr).second->getSendItemDirectoryFlag(tmpSenditem);

				createExactFile(transferQueue, newDestinationSet, tmpSenditem, tmpSendGen, AES_FSREADY, error, "", "", tmpIsDir);

				(*itr).second->changeFileStatus(AES_FSDELETE, tmpSenditem, error);
				streamBuffer.getline(tmpSenditem,128, '\n');
			}
		}
	}
	else
	{
		error = AES_TQDESTSETCOMBINATION;
		m_synchOnTQMap.release();
		return false;
	}

	m_synchOnTQMap.release();
	return true;
}

//	-------------------------------------------------
//	       resendOneFile()
//	-------------------------------------------------
bool aes_afp_database_fd::resendOneFile (std::string transferQueue, std::string filename, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error, std::string newDestinationSet)
{
	(void)(username);
	bool result(false);
	size_t index = std::string::npos;
	std::string tmpOrderString;
	std::string tmpGenName;
	std::string tmpNameTag;
	TransferQueueMap::iterator itr;
	std::stringstream streamBuffer;
	aes_afp_datablock* db = new aes_afp_datablock;
	std::string tmpTemplate;
	std::string tmpUserGroup;
	std::string tmpSourceDirectory;

	tmpOrderString = transferQueue + "_" + destinationSet;

	index = filename.find_first_of("-");
	if (std::string::npos != index)
	{
		index++;
		tmpGenName = filename.substr(index, filename.length());
	}

	m_synchOnTQMap.acquire();
	itr = transferQueueMap_.find(tmpOrderString);
	if (itr != transferQueueMap_.end() )
	{

		/*	if (checkSecurity(username, (*itr).second->getUserGroup() ) == false)
				{
					error = AES_NOTAUTHORIZED;
					m_synchOnTQMap.release();
					return false;
				}
		 */

		result = (*itr).second->getFileStatus(filename, db, error);
		if (result == false)
		{
			error = AES_FILENOTFOUND;
			delete db;
			db = 0;	
			m_synchOnTQMap.release();
			return false;
		}
		if (db->getStatus() != AES_FSFAILED)
		{
			error = AES_ILLEGALSTATUSVALUE;
			 delete db;
                        db = 0;
			m_synchOnTQMap.release();
			return false;
		}

		delete db;
		db = 0;
		AES_AFP_Renametypes tmpRename(AES_NONE);
		tmpRename = (*itr).second->getRenameFile();
		tmpTemplate = (*itr).second->getTemplate();
		tmpSourceDirectory = (*itr).second->getDirectory();
		tmpUserGroup = (*itr).second->getUserGroup();
		tmpNameTag = (*itr).second->getNameTag();
		int tmpStartupSequenceNumber = (*itr).second->getStartupSequenceNumber();

		if (newDestinationSet == "")
		{
			result = (*itr).second->transferFailedFile(filename, error);
		}
		else
		{
			result = createTransferQueue(transferQueue,
					newDestinationSet,
					error,
					tmpSourceDirectory,
					AES_FSREADY,
					10,
					false,
					10,
					0,
					tmpRename,
					tmpTemplate,
					tmpUserGroup,
					tmpNameTag,
					tmpStartupSequenceNumber);

			if (result == true || error == AES_PROCORDEREXIST)
			{
				result = createExactFile(transferQueue, newDestinationSet, filename, tmpGenName, AES_FSREADY, error, "", "", false);
				m_synchOnTQMap.release();
				return result;
			}
			m_synchOnTQMap.release();
			return false;
		}
	}
	else
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		delete db;
                db = 0;
		return false;
	}

	m_synchOnTQMap.release();
	return true;

}

//	-------------------------------------------------
//	       resendAll()
//	-------------------------------------------------
bool aes_afp_database_fd::resendAll (std::string transferQueue, std::string username, AES_GCC_Errorcodes& error)
{
	(void)(username);	
	TransferQueueMap::iterator itr;
	bool foundOne(false);

	m_synchOnTQMap.acquire();

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{
			/*	if (checkSecurity(username, (*itr).second->getUserGroup() ) == false)
						{
							error = AES_NOTAUTHORIZED;
							m_synchOnTQMap.release();
							return false;
						}*/
			foundOne = true;
			if(!((*itr).second->transferFailed(error)))
			{
				m_synchOnTQMap.release();
				return false;
			}
		}
	}

	if (foundOne == true)
	{
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
}

//	-------------------------------------------------
//	       createTransferQueue()
//	-------------------------------------------------
bool aes_afp_database_fd::createTransferQueue (std::string transferQueue,
		std::string destinationSet,
		AES_GCC_Errorcodes& error,
		std::string sourceDirectory,
		AES_GCC_Filestates defaultState,
		int removeDelay,
		bool removeBefore,
		int retryTimeInterval,
		int retryTimes,
		AES_AFP_Renametypes renameFile,
		std::string renameTemplate,
		std::string userGroup,
		std::string nameTag,
		long startupSequenceNumber,
                Dest_Set_Type dType)
{
	AES_AFP_TRACE_MESSAGE("TQ = %s, destination set = %s, source directory = %s, user group = %s", transferQueue.c_str(), destinationSet.c_str(), sourceDirectory.c_str(), userGroup.c_str());
	AES_AFP_TRACE_MESSAGE("defaultState = %d,removeDelay = %d, retryTimeInterval = %d , retryTimes = %d, removeBefore = %d", defaultState, removeDelay, retryTimeInterval, retryTimes, removeBefore);
	TransferQueueMap::iterator itr;
	std::string tmpOrderString;
	char tmpString[512];
	unsigned short destCounter(0);

	size_t index = transferQueue.find_last_of('/');
	if (std::string::npos != index)
	{
		int len = transferQueue.length();
		unsigned int cplen = transferQueue.copy(tmpString, len, index+1);
		tmpString[cplen] = 0;
		tmpOrderString = tmpString;
	}
	else
	{
		tmpOrderString = transferQueue;
	}

	bool result = transferQueueDestinationSetDefined(tmpOrderString, destinationSet, error);
	if (result == true)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Transfer queue %s already defined", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("Transfer queue %s already defined", transferQueue.c_str());
		error = AES_PROCORDEREXIST;
		return false;
	}

	m_synchOnTQMap.acquire();
	// Check for more than 20 TQ's to the same destination
	for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getDestinationSet() == destinationSet)
		{
			destCounter++;
			if (destCounter >= MAX_DESTINATIONSETS_)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "More than %d destinationsets to the same transfer queue", destCounter);
				AES_AFP_TRACE_MESSAGE("More than %d destinationsets to the same transfer queue", destCounter);
				error = AES_TOOMANYTQ;
				m_synchOnTQMap.release();
				return false;
			}
		}
	}

	for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == tmpOrderString)
		{
			// Check that rename file is same.
			if ( (*itr).second->getRenameFile() != renameFile)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Incorrect rename type");
				AES_AFP_TRACE_MESSAGE("Incorrect rename type");
				error = AES_WRONGRENAMETYPE;
				m_synchOnTQMap.release();
				return false;
			}

			// Check that rename template is same.
			if ( (*itr).second->getTemplate() != renameTemplate)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Incorrect rename template");
				AES_AFP_TRACE_MESSAGE("Incorrect rename template");
				error = AES_WRONGTEMPLATE;
				m_synchOnTQMap.release();
				return false;
			}

			// If source directory is given, check that it is the same.
			if (sourceDirectory.empty() )
			{
				sourceDirectory = (*itr).second->getDirectory();
			}
			else
			{
				if ( (*itr).second->getDirectory() != sourceDirectory)
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Not the same source directory");
					AES_AFP_TRACE_MESSAGE("Not the same source directory");
					error = AES_INCONSISTENTDIR;
					m_synchOnTQMap.release();
					return false;

				}
			}

			// Check that name tag is the same
			if (!nameTag.empty() )
			{
				if ( (*itr).second->getNameTag() != nameTag)
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Not the same name tag");
					AES_AFP_TRACE_MESSAGE("Not the same name tag");
					error = AES_WRONGNAMETAG;
					m_synchOnTQMap.release();
					return false;
				}
			}
			else
			{
				nameTag = (*itr).second->getNameTag();
			}

			// uabmha: CNI 1135, Check that startup sequence number is the same
			// otherwise problems when -j (local rename) is used... different file names
			// towards different destinations...
			if ( (*itr).second->getStartupSequenceNumber() != startupSequenceNumber)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Not the same startup sequence number");
				AES_AFP_TRACE_MESSAGE("Not the same startup sequence number");
				error = AES_WRONGSTARTUPSEQUENCENUMBER;
				m_synchOnTQMap.release();
				return false;
			}

			break;

		} // end of if same TQ name

	} // end of for

	aes_afp_transferqueue* tmpOrderObj = aes_afp_objectfactory::Instance().makeTransferQueueObject();
	if (tmpOrderObj)
	{
		result = tmpOrderObj->init(transferQueue,
				destinationSet,
				userGroup,
				error,
				sourceDirectory,
				defaultState,
				removeDelay,
				removeBefore,
				retryTimeInterval,
				retryTimes,
				renameFile,
				renameTemplate,
				true,
				false,
				nameTag,
				startupSequenceNumber,
                                dType);

		if (result == false)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to initiate TQ object");
			AES_AFP_TRACE_MESSAGE("Not able to initiate TQ object");
			aes_afp_objectfactory::Instance().releaseTransferQueueObject(tmpOrderObj);
			m_synchOnTQMap.release();
			return false;
		}


		tmpOrderString += "_";
		tmpOrderString += destinationSet;
		for(itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
		{
			if ((*itr).second->getName() == tmpOrderObj->getName())
			{
				tmpOrderObj->firstDs_ = true;
			}
		}    //End Change for TR HK91962

		transferQueuePair_.first = tmpOrderString;
		transferQueuePair_.second = tmpOrderObj;
		transferQueueMap_.insert(transferQueuePair_);

		error = AES_NOERRORCODE;
		AES_AFP_TRACE_MESSAGE("TQ %s created OK", transferQueue.c_str());
		m_synchOnTQMap.release();
		return true;
	}

	AES_AFP_LOG(LOG_LEVEL_ERROR, "Error, could not create TQ %s", transferQueue.c_str());
	AES_AFP_TRACE_MESSAGE("Error, could not create TQ %s", transferQueue.c_str());
	error = AES_CATASTROPHIC;
	m_synchOnTQMap.release();
	return false;
}

//	-------------------------------------------------
//	       sendFile()
//	-------------------------------------------------
bool aes_afp_database_fd::sendFile (std::string transferQueue, std::string filename, AES_GCC_Errorcodes& error, AES_GCC_Format mode, std::string mask, bool isDirectory, AES_GCC_Filestates fileState)
{
	TransferQueueMap::iterator itr;
	bool foundOne(false);
	bool doOnce(true);
	std::string tmpString;
	std::string tmpFileName;
	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getManual() == true)
		{
			// Dont insert file for manual order
			continue;
		}
		else
		{
			AES_AFP_TRACE_MESSAGE("NOT MANUAL filename = %s,fileState = %d",filename.c_str(),fileState);
		}
		if ( (*itr).second->getName() == transferQueue)
		{
			foundOne = true;
			if ( (*itr).second->getRenameFile() == AES_LOCAL)
			{
				AES_AFP_LOG(LOG_LEVEL_DEBUG, "TQ Name = %s , \n filename = %s \n , rename = %d \n", transferQueue.c_str(),filename.c_str(),(*itr).second->getRenameFile());
				if (doOnce == true)
				{
					if (!((*itr).second->firstDs_))  //TR HK91962 Check If the DestinationSet is introduced for the first time.
					{
						(*itr).second->localRenameFile(filename, tmpFileName);
						doOnce = false;
					}
				}
			}
			else
			{
				tmpFileName = filename;
			}
		}
	}

	//TR HK91962 To send file to all the destinations of transferQueue.
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getManual() == true)
		{
			// Dont insert file for manual order
			continue;
		}
		if ( (*itr).second->getName() == transferQueue)
		{
			foundOne = true;

			(*itr).second->sendFile(tmpFileName, error, mode, mask, isDirectory, filename, fileState);
		}
	}//End of TR HK91962
	if (foundOne == true)
	{
		if (error == AES_NOERRORCODE)
		{
			m_synchOnTQMap.release();
			return true;
		}
		else
		{
			m_synchOnTQMap.release();
			return false;
		}
	}
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	-------------------------------------------------
//	       getSourceDirectory()
//	-------------------------------------------------
bool aes_afp_database_fd::getSourceDirectory (std::string transferQueue, std::string& sourceDirectory, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	bool foundOne(false);
	std::string tmpString;

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		sourceDirectory = "";
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{
			foundOne = true;
			sourceDirectory = (*itr).second->getDirectory();
		}
	}
	if (foundOne == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;

}

//	-------------------------------------------------
//	       setSourceDirectory()
//	-------------------------------------------------
bool aes_afp_database_fd::setSourceDirectory (std::string transferQueue, std::string sourceDirectory, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	bool foundOne(false);
	std::string tmpString;
	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	ACE_stat f_stat;
	int res = ACE_OS::stat(sourceDirectory.c_str(),&f_stat);
	if (res == 0)
	{
		if (!(f_stat.st_mode & S_IFDIR))
		{
			error = AES_NOTDIRECTORY;
			m_synchOnTQMap.release();
			return false;
		}

	}
	else
	{
		string newdr("");
		if (AES_GCC_Util::datapath_trn(sourceDirectory,AES_DATA_PATH,newdr)==false)
		{
			AES_AFP_TRACE_MESSAGE("Directory name sent for event %s",newdr.c_str());
			newdr = "Not Available";
		}
		Event::report(EVENT_FILENOTFOUND, "FIND FILE FAULT", newdr, "Not able to find file or directory: " + newdr);
		error = AES_APPLDIRNOTFOUND;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{
			foundOne = true;
			(*itr).second->setDirectory(sourceDirectory);
		}
	}
	if (foundOne == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       openTransferQueue()
//	---------------------------------------------------------
bool aes_afp_database_fd::openTransferQueue (std::string transferQueue, ACS_DSD_Session* dsdChannel, AES_GCC_Errorcodes& error)
{

	AES_AFP_TRACE_MESSAGE("transferQueue = %s",transferQueue.c_str());
	TransferQueueMap::iterator itr;
	bool foundOne(false);
	std::string tmpString;
	TransferQueueEventMap::iterator itr2;

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{
			foundOne = true;
			(*itr).second->openTransferQueue(dsdChannel, error);
		}
	}

	itr2 = transferQueueEvMap_.find(transferQueue);
	if (itr2 != transferQueueEvMap_.end()) {
		transferQueueEvMap_.erase(itr2);
	}

	if (foundOne == true)
	{
		if (error == AES_NOERRORCODE)
		{
			m_synchOnTQMap.release();
			return true;
		}
		else
		{
			m_synchOnTQMap.release();
			return false;
		}
	}
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       closeTransferQueue()
//	---------------------------------------------------------
bool aes_afp_database_fd::closeTransferQueue (std::string transferQueue, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE("transferQueue = %s",transferQueue.c_str());
	TransferQueueMap::iterator itr;
	bool foundOne(false);

	std::string tmpString;
	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{
			(*itr).second->closeTransferQueue();
			foundOne = true;//HF74701
		}
	}

	if (foundOne)//HF74701
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else
	{
		error = AES_NOPROCORDER;
	}
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       getLastReported()
//	---------------------------------------------------------
bool aes_afp_database_fd::getLastReported (std::string transferQueue, std::string& strBuffer, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getName() == transferQueue)
		{

			strBuffer += (*itr).second->getLastReportedFileOrig() + '\n';
			strBuffer += (*itr).second->getLastReportedFileGen() + '\n';
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
	}
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       getDestinationSetList()
//	---------------------------------------------------------
bool aes_afp_database_fd::getDestinationSetList (std::string& strBuffer, 
		AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	ACE_UINT32 cntr = 0;

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}


	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		// HP32415 start
		cntr++;
		strBuffer += (*itr).second->getName();
		if ( cntr != transferQueueMap_.size()  )
		{
			strBuffer += "\n";
		}
		// HP32415 end
	}
	error = AES_NOERRORCODE;
	m_synchOnTQMap.release();
	return true;
}

//	---------------------------------------------------------
//	       getFileStatus()
//	---------------------------------------------------------
bool aes_afp_database_fd::getFileStatus (std::string filename, AES_GCC_Filestates& status, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if ( (*itr).second->getStatusFile(filename, status) == true )
		{
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
	}
	error = AES_FILENOTFOUND;
	m_synchOnTQMap.release();
	return false;
}

//	-------------------------------------------------
//	       sendManually()
//	-------------------------------------------------
bool aes_afp_database_fd::sendManually (std::string transferQueue, std::string filename, std::string sourceDirectory, std::string destinationSet, int retryTimes, int retryInterval, bool isDirectory, std::string username, std::string userGroup, AES_GCC_Errorcodes& error)
{
	std::string tmpOrderString;
	std::string tmpFullPath;
	std::string tmpSourceDir;
	std::string tmpUserGroup;
	TransferQueueMap::iterator itr;
	(void)retryTimes;
	(void)retryInterval;
	(void)userGroup;
	bool result(false);
	// Check what was sent to us
	tmpOrderString = transferQueue;
	tmpFullPath = sourceDirectory;
	tmpFullPath += aes_afp_parameter::delimiter() + filename;
	//xcsrajm For TR HH54446
	uname=username;
	AES_AFP_TRACE_MESSAGE("Send file %s, TQ %s, destinationSet %s Source directory %s", tmpFullPath.c_str(), transferQueue.c_str(), destinationSet.c_str(), sourceDirectory.c_str());

	m_synchOnTQMap.acquire();

	tmpOrderString = transferQueue + "_" + destinationSet;
	itr = transferQueueMap_.find(tmpOrderString);


	if (itr != transferQueueMap_.end() )
	{
		AES_AFP_TRACE_MESSAGE("TQ %s, destination set %s found inserting file",transferQueue.c_str(), destinationSet.c_str());
		tmpSourceDir = (*itr).second->getDirectory();
		AES_AFP_TRACE_MESSAGE("tmpSourceDir is %s",tmpSourceDir.c_str());
		if (tmpSourceDir.empty() )
		{
			(*itr).second->setDirectory(sourceDirectory);
			tmpSourceDir = sourceDirectory;
		}
		if (tmpSourceDir != sourceDirectory)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Inconsistent source directories");
			AES_AFP_TRACE_MESSAGE("Inconsistent source directories");
			error = AES_INCONSISTENTDIR;
			m_synchOnTQMap.release();
			return false;
		}
		// Check if user is member of transfer queue user group
		tmpUserGroup = (*itr).second->getUserGroup();
		tmpFullPath = tmpSourceDir + aes_afp_parameter::delimiter() + filename;

		result = (*itr).second->insertFile(filename, error, AES_FSREADY, tmpFullPath, "", filename, isDirectory);
		if (result == false)
		{
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Could not insert file");
			AES_AFP_TRACE_MESSAGE("Could not insert file");
			m_synchOnTQMap.release();
			return false;
		}
		AES_AFP_TRACE_MESSAGE("File inserted");
		m_synchOnTQMap.release();
		return true;
	}
        else
        {
		m_synchOnTQMap.release();
	       AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ not found tq = %s, destset = %s",transferQueue.c_str(), destinationSet.c_str());
		AES_AFP_TRACE_MESSAGE( "TQ not found tq = %s, destset = %s",transferQueue.c_str(), destinationSet.c_str());
                return false;
        }

}

//	---------------------------------------------------------
//	       doDelete()
//	---------------------------------------------------------
bool aes_afp_database_fd::doDelete (std::string path)
{
	bool result = true;
	boost::filesystem::path dir_path(path);
	if(exists(dir_path))
	{
		try
		{	uintmax_t num_of_deleted_items = remove_all(dir_path);
			AES_AFP_TRACE_MESSAGE("deleted <%lu> items from directory <%s>", num_of_deleted_items, path.c_str());
			AES_AFP_LOG(LOG_LEVEL_INFO, "deleted <%lu> items from directory <%s>", num_of_deleted_items, path.c_str());
		}
		catch(const boost::filesystem::filesystem_error& ex)
		{
			AES_AFP_TRACE_MESSAGE("ERROR: cannot delete directory: <%s>. Error: <%s>", path.c_str(), ex.what());
			AES_AFP_LOG(LOG_LEVEL_ERROR, "cannot delete directory: <%s>. Error: <%s>", path.c_str(), ex.what());
			AES_AFP_LOG(LOG_LEVEL_ERROR, "Creating empty directory: <%s> again!!",path.c_str());
			result = false;
		}
	}
	else
	{
		AES_AFP_TRACE_MESSAGE("Directory <%s> not exists", path.c_str());
		AES_AFP_LOG(LOG_LEVEL_INFO, "Directory <%s> not exists", path.c_str());
	}
	return result;
}

//	-------------------------------------------------
//	       setTransferQueueManually()
//	-------------------------------------------------
bool aes_afp_database_fd::setTransferQueueManually (std::string transferQueue, std::string destinationSet)
{
	TransferQueueMap::iterator itr;

	std::string tmpOrder;

	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		m_synchOnTQMap.release();
		return false;
	}

	tmpOrder = transferQueue;
	tmpOrder += '_' + destinationSet;

	itr = transferQueueMap_.find(tmpOrder);
	if (itr != transferQueueMap_.end() )
	{
		(*itr).second->setManual();
	}
	m_synchOnTQMap.release();
	return true;
}

//	---------------------------------------------------------
//	       getNextFilename()
//	---------------------------------------------------------
bool aes_afp_database_fd::getNextFilename (std::string transferQueue, std::string destinationSet, std::string filename, std::string& nextFilename, AES_GCC_Errorcodes& error)
{
	TransferQueueMap::iterator itr;
	std::string tmpTQDest(transferQueue);
	bool result(false);

	m_synchOnTQMap.acquire();

	if (transferQueueMap_.empty() )
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	tmpTQDest += "_";

	if (destinationSet.empty() )
	{
		for (itr=transferQueueMap_.begin();itr!=transferQueueMap_.end();++itr)
		{
			if ( (*itr).second->getName() == transferQueue)
			{
				tmpTQDest += (*itr).second->getDestinationSet();
				break;
			}
		}
	}
	else
	{
		tmpTQDest += destinationSet;
	}

	itr = transferQueueMap_.find(tmpTQDest);

	if (itr != transferQueueMap_.end() )
	{
		result = (*itr).second->getNextFilename(filename, nextFilename);
		if (result == true)
		{
			error = AES_NOERRORCODE;
			m_synchOnTQMap.release();
			return true;
		}
		else
		{
			nextFilename = "";
			error = AES_SENDITEMNOTREP;
			m_synchOnTQMap.release();
			return false;
		}
	}
	else
	{
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
}

//	---------------------------------------------------------
//	       cdhChangeFileStatus()
//	---------------------------------------------------------
bool aes_afp_database_fd::cdhChangeFileStatus (std::string transferQueue, std::string destinationSet, std::string filename, AES_GCC_Filestates newStatus, AES_GCC_Errorcodes& error)
{
	AES_AFP_TRACE_MESSAGE( "Filename %s, TQ %s, destination set %s, new status %d",
			filename.c_str(), transferQueue.c_str(), destinationSet.c_str(), newStatus);
	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	bool result(false);
	AES_GCC_Filestates oldStatus(AES_FSNONE);

	tmpOrderName = transferQueue + "_" + destinationSet;

	m_synchOnTQMap.acquire();

	itr = transferQueueMap_.find(tmpOrderName);
	if (itr != transferQueueMap_.end() )
	{
		// CDH not allowed to change status for files in DELETE
		result = (*itr).second->getStatusFile(filename, oldStatus);
		if (oldStatus == AES_FSDELETE)
		{
			m_synchOnTQMap.release();
			return false;
		}
		result = (*itr).second->changeFileStatus(newStatus, filename, error);
		if (result == false)
		{
			m_synchOnTQMap.release();
			return false;
		}
		m_synchOnTQMap.release();
		return true;
	}
	AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to change status for %s, TQ %s not found", filename.c_str(), transferQueue.c_str());
	AES_AFP_TRACE_MESSAGE("Not able to change status for %s, TQ %s not found", filename.c_str(), transferQueue.c_str());
	error = AES_NOPROCORDER;
	m_synchOnTQMap.release();
	return false;
}

//	---------------------------------------------------------
//	       checkSecurity()
//	---------------------------------------------------------
bool aes_afp_database_fd::checkSecurity (std::string& username, std::string& userGroup)
{
	(void)(username);
	(void)(userGroup);
	AES_AFP_TRACE_MESSAGE("Check if %s, is member in %s",
			username.c_str(), userGroup.c_str());	
	return true;
}


bool aes_afp_database_fd::sendApplEvent(std::string transferQueue, AES_GCC_Eventcodes evc, AES_GCC_Errorcodes errc)
{
	

	AES_AFP_TRACE_MESSAGE( "Entering,modified for HP90477");
	std::string s;
	TransferQueueMap::iterator itr;
	//TR HP90477
	itr = transferQueueMap_.find(transferQueue);
	if( itr != transferQueueMap_.end() )
	{
		if ((*itr).second->getName() == transferQueue)
		{
			if (filterDestSetEvent(transferQueue, evc) != true)
			{
				(*itr).second->sendApplEvent(evc, transferQueue, errc);
			}
		}
	}
	return false;
}

bool aes_afp_database_fd::filterDestSetEvent(std::string transferQueue, AES_GCC_Eventcodes evc)
{
	bool dontSendEvent = false;
	TransferQueueEventPair tqPair;
	TransferQueueEventMap::iterator itr;

	if (evc != AES_EVEDESTSETDOWN && evc != AES_EVEDESTSETDOWNCEASE) {
		// do not filter the event
		return false;
	}

	m_synchOnTQMap.acquire();
	itr = transferQueueEvMap_.find(transferQueue);
	if (itr == transferQueueEvMap_.end()) {
		// not found
		tqPair.first = transferQueue;
		tqPair.second = evc;
		transferQueueEvMap_.insert(tqPair);

		// dont send cease event without having sent the
		// destination set down event
		if (evc == AES_EVEDESTSETDOWNCEASE) {
			dontSendEvent = true;
		}

	} else if ((*itr).second == evc) {
		// already notified application with this event
		dontSendEvent = true;

	} else {
		// new event! update event id
		(*itr).second = evc;
	}
	m_synchOnTQMap.release();
	return dontSendEvent;
}


//	---------------------------------------------------------
//	       checkAuthorityFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::checkAuthorityFile (std::string& fileName, std::string& userName, AES_GCC_Errorcodes& error)
{
	(void)(fileName);
	(void)(userName);
	(void)(error);
	AES_AFP_TRACE_MESSAGE( "Filename %s, username %s %d",fileName.c_str(), userName.c_str(),error);
	return true;

}

//	---------------------------------------------------------
//	       convertRootFile()
//	---------------------------------------------------------
bool aes_afp_database_fd::convertRootFile (std::string& rootFilePath)const
{
	//Code removed for APG43L.
	(void) rootFilePath;
	return true;
}

//	---------------------------------------------------------
//	       removeSourceDirectoryFiles()
//	---------------------------------------------------------
bool aes_afp_database_fd::removeSourceDirectoryFiles (std::string transferQueue, std::string username, AES_GCC_Errorcodes& error)		//for APZ21230/4-699
{
	(void)(username);
	AES_AFP_TRACE_MESSAGE( "Removing Files in Source Directory for TQ = %s, username %s", transferQueue.c_str(),username.c_str());

	bool foundOne(false);
	bool fileIsProtected(false);
	bool result(false);

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() ) //check if transfer queue present
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Transfer queue %s not found", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("Transfer queue %s not found", transferQueue.c_str());
		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

	TransferQueueMap::iterator itr;

	itr = transferQueueMap_.begin();
	while (itr != transferQueueMap_.end() )
	{
		if ((*itr).second->getName() == transferQueue)
		{
			/*					if (checkSecurity(username, (*itr).second->getUserGroup() ) == false) // chech user authenticity
						{
							error = AES_NOTAUTHORIZED;
							m_synchOnTQMap.release();
							return false;
						}*/

			foundOne = true;
			result = (*itr).second->removeSourceDirectoryAll(error);
			if (result == false)
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove SourceDirectory for TQ = %s", transferQueue.c_str());
				AES_AFP_TRACE_MESSAGE("Not able to remove SourceDirectory for TQ = %s", transferQueue.c_str());
				if (error == AES_FILEISPROTECTED)
				{
					fileIsProtected = true;
					++itr;
				}
				else
				{
					m_synchOnTQMap.release();
					return false;
				}
			}
			else
			{
				aes_afp_objectfactory::Instance().releaseTransferQueueObject((*itr).second);
				transferQueueMap_.erase(itr);
				itr = transferQueueMap_.begin();
			}
		}
		else
		{
			++itr;
		}
	}
	if (fileIsProtected == true)
	{
		error = AES_FILEISPROTECTED;
		m_synchOnTQMap.release();
		return false;
	}
	if (foundOne == true)
	{
		error = AES_NOERRORCODE;
		m_synchOnTQMap.release();
		return true;
	}
	else   // transfer queue not found
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "TQ %s not found", transferQueue.c_str());
		AES_AFP_TRACE_MESSAGE("TQ %s not found", transferQueue.c_str());

		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}

}

//	---------------------------------------------------------
//	       removeFileFromSourceDirectory()
//	---------------------------------------------------------
bool aes_afp_database_fd::removeFileFromSourceDirectory (std::string transferQueue, std::string destinationSet, std::string filename, std::string username, AES_GCC_Errorcodes& error)		//for APZ21230/4-699
{
	(void)(username);
	(void)(error);
	(void)(destinationSet);
	AES_AFP_TRACE_MESSAGE("Remove file %s from TQ %s, destinationset %s username %s", filename.c_str(), transferQueue.c_str(), destinationSet.c_str(),username.c_str());
	TransferQueueMap::iterator itr;
	std::string tmpOrderName;
	bool result(false);
	bool orderFound(false);
	bool sendItemFound(false);

	m_synchOnTQMap.acquire();
	if (transferQueueMap_.empty() )  //check if transfer queue present
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "No TQ defined");
		AES_AFP_TRACE_MESSAGE("No TQ defined");
		orderFound = false;

		error = AES_NOPROCORDER;
		m_synchOnTQMap.release();
		return false;
	}
	for (itr = transferQueueMap_.begin();itr != transferQueueMap_.end(); ++itr)
	{
		if ((*itr).second->getName() == transferQueue)
		{
			/*	if (checkSecurity(username, (*itr).second->getUserGroup() ) == false) //check user authenticity
				{
					error = AES_NOTAUTHORIZED;
					m_synchOnTQMap.release();
					return false;
				}*/

			orderFound = true;
			result = (*itr).second->removeFileFromSourceDirectory(filename, error);

			if (result == false)
			{
				switch (error)
				{
				case AES_NOACCESS:
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, no access", filename.c_str());
					AES_AFP_TRACE_MESSAGE("Not able to remove %s, no access", filename.c_str());
					m_synchOnTQMap.release();
					return false;
				}
				case AES_FILEISPROTECTED:
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, TQ is protected", filename.c_str());
					AES_AFP_TRACE_MESSAGE("Not able to remove %s, TQ is protected", filename.c_str());
					m_synchOnTQMap.release();
					return false;
				}
				default:
				{
					// Do nothing
				}
				}
			}
			else
			{
				sendItemFound = true;
			}
		}  // if
	} // for

	if (orderFound == false)// TQ not found
	{
		error = AES_NOPROCORDER;
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, TQ not found",
				filename.c_str());
		AES_AFP_TRACE_MESSAGE("Not able to remove %s, TQ not found",
                                filename.c_str());
		m_synchOnTQMap.release();
		return false;
	}
	if (sendItemFound == false) // file not reported to AFP
	{
		error = AES_SENDITEMNOTREP;
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to remove %s, file not found", filename.c_str());
		AES_AFP_TRACE_MESSAGE("Not able to remove %s, file not found", filename.c_str());
		m_synchOnTQMap.release();
		return false;
	}


	error = AES_NOERRORCODE;
	m_synchOnTQMap.release();
	return true;
}

//	---------------------------------------------------------
//	       RetainPersistantNumber() 
//  Retain the persistant number to the first transferqueue in the map.
//  And set the firstds_ of first transferqueue to false.
//  Returns true if the function is successfull.
//  Returns False if the function fails.
//	---------------------------------------------------------
bool aes_afp_database_fd::RetainPersistantNumber(unsigned int tmppersistentNr_,std::string transferqueue)
{
	TransferQueueMap::iterator itr;
	std::string tmppersistentpath;
	m_synchOnTQMap.acquire();

	for(itr = transferQueueMap_.begin();itr != transferQueueMap_.end();++itr)
	{
		if((*itr).second->getName() == transferqueue)
		{
			if((*itr).second->firstDs_ == false)
			{
				continue;
			}
			else
			{
				(*itr).second->firstDs_ = false;

				tmppersistentpath = (*itr).second->getTqObjectDirectory();
				tmppersistentpath += aes_afp_parameter::delimiter() + "persist.nr";

				ofstream fout(tmppersistentpath.c_str());
				if(!fout.good())
				{
					m_synchOnTQMap.release();
					return false;
				}
				fout << tmppersistentNr_;
				fout.close();
				break;
			}

		}

	}
	m_synchOnTQMap.release();

	return true;
}

void aes_afp_database_fd::deleteTQDirIfNotExistsInIMM(std::string& path)
{
	AES_AFP_TRACE_MESSAGE("Entering");
	DIR *dir;
	struct dirent * finddata;
	std::string searchPattern = path;
	std::string fileN;
	bool isTQFound = false;
	dir = opendir(searchPattern.c_str());
	AES_AFP_TRACE_MESSAGE("path = %s", path.c_str());
	if (dir == NULL)
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR,"Not able to open file or directory: %s" , searchPattern.c_str());
		AES_AFP_TRACE_MESSAGE("Not able to open file or directory: %s" , searchPattern.c_str());
	}
	else
	{
		while ((finddata = readdir(dir)) != NULL)
		{
			if (strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, ".."))

			{
				fileN = path + "/"+finddata->d_name;
				struct stat stat_buff;
				stat(fileN.c_str(),&stat_buff);
				if(S_ISDIR(stat_buff.st_mode))
				{
					isTQFound = false;
					fileN = finddata->d_name;
					size_t pos = fileN.find_last_of('_');
					string tmpPath = fileN.substr(pos+1,fileN.length()+1);
					if(tmpPath == "temp")
						fileN = fileN.substr(0,pos);
					TransferQueueMap::iterator itr;
					for(itr = transferQueueMap_.begin();itr != transferQueueMap_.end();itr++)
					{
						if(fileN  == (*itr).first)
						{
							isTQFound = true;
							break;
						}
					}
				}
				if(!isTQFound)
				{
					fileN = path + "/" + finddata->d_name;
					AES_AFP_TRACE_MESSAGE("Deleting the directory :%s ",fileN.c_str());
					doDelete(fileN);
					int resultRemoveDir = ACE_OS::rmdir(fileN.c_str());
					if(resultRemoveDir != 0)
					{
						AES_AFP_LOG(LOG_LEVEL_ERROR,"Directory not deleted successfully, errno :%d ",errno);
						AES_AFP_TRACE_MESSAGE("Directory not deleted successfully, errno :%d ",errno);
					}
				}

			}
		}
		if(dir != NULL)
			closedir(dir);
	}
}
//	---------------------------------------------------------
//	       startDeleteFileThread()
//         - spawns deleteExpiredFiles as separate thread
//	---------------------------------------------------------
bool aes_afp_database_fd::startDeleteFileThread()
{
	ACE_thread_t tId = 0;
	int ret = -1;
	bool result = false;
	//spawn deleteExpiredFiles as JOINABLE thread
	ret = ACE_Thread::spawn((ACE_THR_FUNC)deleteExpiredFiles, (void*)this, THR_NEW_LWP|THR_JOINABLE, &tId, &deleteThreadHandle_);
	if(0 == ret)
	{
		AES_AFP_TRACE_MESSAGE("Thread is spawned for delete operation !!");
		AES_AFP_LOG(LOG_LEVEL_INFO,"deleteExpiredFiles Thread is spawned !!");
		result = true;
	}
	else
	{
		AES_AFP_LOG(LOG_LEVEL_ERROR, "Spawning a separate thread for delete operation is failed !!");
		AES_AFP_TRACE_MESSAGE("Spawning a separate thread for delete operation is failed !!");
	}
	return result;
}
//	------------------------------------------------------------------------------------------------------------
//	ROUTINE : deleteExpiredFiles()
//         - perfroms cleanup of status and source file periodically i.e every 10 sec or predefined timeout value
//         - Clears entry in deleteMap_ on successful removal of status & source files
//	-------------------------------------------------------------------------------------------------------------
void* aes_afp_database_fd::deleteExpiredFiles(void * arg)
{
	aes_afp_database_fd * thisDb = static_cast<aes_afp_database_fd*>(arg);
	ACE_Time_Value tv(DeleteThreadParam::deleteEvTimeout,0);
	AES_AFP_TRACE_MESSAGE("Entering");
	while((!thisDb->isShutdownSignaled_) && (thisDb->deleteFilesEv_.wait(&tv,0)))
	{
		AES_AFP_TRACE_MESSAGE("event signaled!");
		thisDb->deleteFiles();
		DeleteMap::iterator itr = thisDb->deleteMap_.begin();
		while((!thisDb->isShutdownSignaled_) && (thisDb->deleteMap_.end() !=itr))
		{
			//Remove status file
			if(thisDb->removeExpiredFile(itr->first))
			{
				//Remove source file
				if(thisDb->deletePhysFile(itr->second))
				{
					//GCC_TDEBUG((aes_afp_Database_fd, "Status file and source file deleted , %s ",(itr->first).c_str()));
					thisDb->deleteMap_.erase(itr++);
					continue;
				}
				else
				{
					AES_AFP_LOG(LOG_LEVEL_ERROR, "source file %s deletion failed, error %d ",(itr->first).c_str(),errno);
					AES_AFP_TRACE_MESSAGE("source file %s deletion failed, error %d ",(itr->first).c_str(),errno);
				}
			}
			else
			{
				AES_AFP_LOG(LOG_LEVEL_ERROR, "Status file %s deletion failed, error %d ",(itr->second).c_str(),errno);
				AES_AFP_TRACE_MESSAGE("Status file %s deletion failed, error %d ",(itr->second).c_str(),errno);
			}
			++itr;
		}
	}
	return (void*)0;
}
//	---------------------------------------------------------
//	       ROUTINE: removeExpiredFile()
//         - removes given file or directory from the disk
//	---------------------------------------------------------
bool aes_afp_database_fd::removeExpiredFile(const std::string& fileName)
{
	bool result = false;
	ACE_stat fstat;
	if(ACE_OS::stat(fileName.c_str(),&fstat) == 0)
	{
		if(fstat.st_mode & S_IFDIR)
		{
			result = doDelete(fileName); //TR HK91965 for the removal of the directories.
			if (result)
			{
				if ((0 == ACE_OS::rmdir(fileName.c_str())) || (ENOENT ==  errno))
				{
					result = true;
				}
			}
		}
		else
		{
			if((0 == ACE_OS::unlink(fileName.c_str())) || (ENOENT == errno))
			{
				result = true;
			}
		}

		if(!result)
		{
			Event::report(EVENT_FILENOTDEL, "REMOVE FAULT", fileName , "Not able to delete file or directory: " + fileName);
		}
	}
	return result;
}

//      TR HW88888
//      ---------------------------------------------------------
//             ROUTINE: checkFileStatus()
//         - Checks file state(READY/SEND) of the TransferQueue
//      ---------------------------------------------------------
bool aes_afp_database_fd::checkFileStatus(std::string transferQueue, std::string destinationSet, AES_GCC_Errorcodes& error)
{
        AES_AFP_TRACE_MESSAGE( "Destionationset %s, TQ %s", destinationSet.c_str(), transferQueue.c_str());
        TransferQueueMap::iterator itr;
        std::string tmpOrderName;
        bool result(false);

        tmpOrderName = transferQueue + "_" + destinationSet;
        m_synchOnTQMap.acquire();
        AES_AFP_TRACE_MESSAGE("Searching for TQ %s", tmpOrderName.c_str());
        itr = transferQueueMap_.find(tmpOrderName);
        if (itr != transferQueueMap_.end() )
        {
                result = (*itr).second->checkFileStatus();
                m_synchOnTQMap.release();
                return result;
        }

        AES_AFP_LOG(LOG_LEVEL_ERROR, "Not able to check file status, due to %s not found", transferQueue.c_str());
        AES_AFP_TRACE_MESSAGE("Not able to check file status, due to %s not found", transferQueue.c_str());
        error = AES_NOPROCORDER;
        m_synchOnTQMap.release();
        return false;
}

