
//******************************************************************************
//
// NAME
//      aes_dbo_tqdatabase.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
//      <General description of the class>

// DOCUMENT NO
//      19089-CAA 109 0423

// AUTHOR 
//      1999-11-18 by UAB/I/LN  Urban S�derberg

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION
//          010606  qabhall Exchanged hardcoded resultcodes with AES_GCC_Errorcodes. INGO3 GOH
//          010904  qabhall Debug print when using mutex are put within #ifdef
//		    011119	qabulfg	Event number changed 233xx -> 292xx

// SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//
//******************************************************************************

#include "aes_dbo_tqdatabase.h"

#include "aes_dbo_opentqsessions.h"
#include "aes_dbo_alarmdatabase.h"
#include <aes_dbo_blocktransfer_queue_handler.h>

#include <aes_dbo_macros.h>
#include "EventCode.h"

#include "ParameterHandler.h"

#include <aes_cdh_destinationset.h>

#include <aes_gcc_errorcodes.h>
#include <aes_gcc_tracer.h>
#include <aes_gcc_util.h>


AES_GCC_TRACE_DEFINE(AES_DBO_Server);

//******************************************************************************
//
aes_dbo_tqdatabase_impl::aes_dbo_tqdatabase_impl():
mtx_()
{
	AES_GCC_TRACE_MESSAGE("Constructor");
}

//******************************************************************************
//
int aes_dbo_tqdatabase_impl::cmpImmInfoInsTQ(string tqName,bool mirrored,ACE_INT32 removeDelay, string destSetName)
{
	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	FileRecord* fRec;
	fRec = new FileRecord();
	strcpy(fRec->tq, tqName.c_str());
	strcpy(fRec->destination, destSetName.c_str());
	fRec->mirror = mirrored;
	fRec->delay = removeDelay;
	fRec->open = 0;

	if(fRec != 0)
	{
		list_.push_back(fRec);
	}
	// Insert and start new session
	if (!aes_dbo_opentqsessions::instance()->insert(tqName))
	{
		// Insertion not ok...
		AES_GCC_TRACE_MESSAGE("Insertion in opentqessions failed for TQ: %s", tqName.c_str());
		return -1;
	}
	return 0;
}


//******************************************************************************
//
aes_dbo_tqdatabase_impl::~aes_dbo_tqdatabase_impl()
{
	AES_GCC_TRACE_MESSAGE("Destructor");
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::defineTQ(const string &tq, const string &dest, unsigned int mirror, unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("In");

	if(isTqDefined(tq))
	{
		AES_GCC_TRACE_MESSAGE("tq = %s is already defined!", tq.c_str());
		return AES_PROCORDEREXIST;
	}

	if (destinationUsed(tq, dest))
	{
		AES_GCC_TRACE_MESSAGE("Destination already used by another tq");
		return AES_DESTINATIONSETINUSE; // Destination already used by another tq
	}

	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	// Update file list
	FileRecord *fRec = new FileRecord();
	strcpy(fRec->tq, tq.c_str());
	strcpy(fRec->destination, dest.c_str());
	fRec->mirror = mirror;
	fRec->delay = delay;
	fRec->open = 0;
	list_.push_back(fRec);

	// Insert and start new session against the application
	if (aes_dbo_opentqsessions::instance()->insert(tq) == false)
	{
		// Insertion not ok...
		AES_GCC_TRACE_MESSAGE("Insertion in opentqsessions failed for file: %s", tq.c_str());
		return AES_NOACCESS;
	}

	return AES_NOERRORCODE; // Entry added
}

//*********************************************************************************************************
bool isTQReferred(string tqName, string className)
{
     OmHandler omHandlerObj;
     std::vector<std::string> stsDnList;
     std::vector<std::string>::iterator it_dnList;

     if( omHandlerObj.Init() == ACS_CC_FAILURE )
     {
    	 AES_GCC_TRACE_MESSAGE("Error occurred while initializing OmHandler...Leaving");
    	 return false;
     }
     omHandlerObj.getClassInstances(className.c_str(), stsDnList);

     AES_GCC_TRACE_MESSAGE("Instances found for %s = %i", className.c_str(), stsDnList.size());

     ACS_CC_ImmParameter paramToFind;
     paramToFind.attrName = new char [50];
     if (ACE_OS::strcmp(className.c_str(),AES_DBO_RTR_CLIENT_CLASS) == 0)
     {
          ACE_OS::strcpy(paramToFind.attrName,"transferQueue");
     }
     else if (ACE_OS::strcmp(className.c_str(),AES_DBO_CPF_CLIENT_CLASS1) == 0)
     {
          ACE_OS::strcpy(paramToFind.attrName,"transferBlockPolicy");
     }

     string tmpString;

     for(it_dnList=stsDnList.begin(); it_dnList != stsDnList.end(); ++it_dnList)
     {
         if(omHandlerObj.getAttribute((*it_dnList).c_str(),&paramToFind) == ACS_CC_SUCCESS)
         {
              AES_GCC_TRACE_MESSAGE("getAttribute() for %s, attrValuesNum = %d", (*it_dnList).c_str(), paramToFind.attrValuesNum);

              if ( paramToFind.attrValuesNum > 0 && ACE_OS::strcmp(reinterpret_cast<char *>(paramToFind.attrValues[0]), "") != 0)
              {
                      if (ACE_OS::strcmp(reinterpret_cast<char *>(paramToFind.attrValues[0]), tqName.c_str()) == 0)
                      {
                           delete[] paramToFind.attrName;
                           omHandlerObj.Finalize();
                           return true;
                      }
              }
         }
         else
         {
        	 AES_GCC_TRACE_MESSAGE("Error occurred while getAttribute() for %s", (*it_dnList).c_str());
         }
     }
     delete[] paramToFind.attrName;
     omHandlerObj.Finalize();
     return false;
}



//******************************************************************************
//
int aes_dbo_tqdatabase_impl::deleteTQ(const string &tq)
{
    AES_GCC_TRACE_MESSAGE("In: TQ = %s", tq.c_str());

    ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    string tqName=tq;
    if ( isTQReferred(tqName, AES_DBO_RTR_CLIENT_CLASS) == true )
    {
            return AES_TQISPROTECTED;
    }
    
    if (list_.empty())
    {
    	return AES_NOPROCORDER; // Entry not found
    }

    unsigned int rCode;
    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
    	fRec = *it;
    	if (tq == fRec->tq)
    	{
    		AES_GCC_TRACE_MESSAGE("TQ found = %s, listSize = %i", fRec->tq, list_.size());

    		if (fRec->open != 0)
    		{
    			AES_GCC_TRACE_MESSAGE("file is open: tq = %s", fRec->tq);
    			return AES_FILEISOPEN; // File is open, cannot be removed!!!
    		}

    		AES_GCC_TRACE_MESSAGE("file is closed: tq = %s", fRec->tq);

    		// Remove tqsession object
    		rCode = aes_dbo_opentqsessions::instance()->terminate(tq);
    		if (rCode != AES_NOERRORCODE)
    			return rCode;

    		// Update file list
    		list_.erase(it);

    		// Remove alarm for the tq
    		AlarmRecord a;
    		strcpy(a.file, fRec->tq);
    		strcpy(a.destination, fRec->destination);
    		aes_dbo_alarmdatabase::instance()->remove(&a);

    		delete fRec;
    		AES_GCC_TRACE_MESSAGE("Entry TQ = %s removed", tq.c_str());
    		return AES_NOERRORCODE;           // Entry removed
    	}
    	++it;
    }
    AES_GCC_TRACE_MESSAGE("Entry TQ = %s NOT found", tq.c_str());
    return AES_NOPROCORDER;                  // Entry not found
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::changeTQDest(const string &tq, const string &dest)
{
	AES_GCC_TRACE_MESSAGE("In");

	if (destinationUsed(tq, dest))
	{
		AES_GCC_TRACE_MESSAGE("Destination already used by another tq");
		return AES_DESTINATIONSETINUSE; // Destination already used by another tq
	}

	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	if (list_.empty())
	{
		return AES_NOPROCORDER;  // Entry does not exists
	}

	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;

		if (tq == fRec->tq)
		{
			if (fRec->open == 0)
			{
				AES_GCC_TRACE_MESSAGE("Destination set exist");
				// Remove any existing alarm for the file and the old destination
				AlarmRecord a;
				strcpy(a.file, fRec->tq);
				strcpy(a.destination, fRec->destination);
				aes_dbo_alarmdatabase::instance()->remove(&a);
				// only the destination needs to be updated in fRec!!
				strcpy(fRec->destination, dest.c_str());

				//Update filelist
				list_.erase(it);
				list_.push_back(fRec);

				return AES_NOERRORCODE;            // Entry changed
			}
			else
			{
				return AES_FILEISOPEN;           // TQ is open, cannot change condition!!!
			}
		}
		++it;
	}
	return AES_NOPROCORDER;                   // Entry does not exists
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::changeTQMirror(const string &tq, unsigned int mirror)
{
    AES_GCC_TRACE_MESSAGE("In");

    ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    
    if (list_.empty())
    {
        return AES_NOPROCORDER;                   // Entry does not exists
    }
    
    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
        fRec = *it;
        
        if (tq == fRec->tq)
        {
            AES_GCC_TRACE_MESSAGE("Destination set exist");
            // Update parameters
            fRec->mirror = mirror;
            // Update filelist
            list_.erase(it);
            list_.push_back(fRec);
            return AES_NOERRORCODE;            // Entry changed
        }
        ++it;
    }
    return AES_NOPROCORDER;                   // Entry does not exists
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::changeTQDelay(const string &tq, unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("In");

	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    
    if (list_.empty())
    {
        return AES_NOPROCORDER;                   // Entry does not exists
    }
    
    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
        fRec = *it;
        
        if (tq == fRec->tq)
        {
        	AES_GCC_TRACE_MESSAGE("Destination set exist");
        	// Update parameters
        	fRec->delay = delay;
        	// Update filelist
        	list_.erase(it);
        	list_.push_back(fRec);
        	return AES_NOERRORCODE;            // Entry changed
        }
        ++it;
    }
    return AES_NOPROCORDER;                   // Entry does not exists
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::changeTQ(const string &tq, const string &dest, unsigned int mirror, unsigned int delay)
{
	AES_GCC_TRACE_MESSAGE("In");

	if (destinationUsed(tq, dest))
	{
		AES_GCC_TRACE_MESSAGE("Destination already used by another tq");
		return AES_DESTINATIONSETINUSE; // Destination already used by another tq
	}

	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	if (list_.empty())
	{
		return AES_NOPROCORDER;                   // Entry does not exists
	}

	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;

		if (tq == fRec->tq)
		{
			if (fRec->open == 0)
			{
				AES_GCC_TRACE_MESSAGE("Destination set exist");
				// Remove any existing alarm for the file and the old destination
				AlarmRecord a;
				strcpy(a.file, fRec->tq);
				strcpy(a.destination, fRec->destination);
				aes_dbo_alarmdatabase::instance()->remove(&a);
				// Update parameters
				strcpy(fRec->destination, dest.c_str());
				fRec->mirror = mirror;
				fRec->delay = delay;

				//Update filelist
				list_.erase(it);
				list_.push_back(fRec);

				return AES_NOERRORCODE;            // Entry changed
			}
			else
			{
				return AES_FILEISOPEN;           // TQ is open, cannot change condition!!!
			}
		}
		++it;
	}
	return AES_NOPROCORDER;                   // Entry does not exists
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::tqList(ACS_APGCC_Command &cmd, const std::string &tqName)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    if (list_.empty())
    {
        return -1;
    }

    int i = 0;
    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
    	fRec = *it;
    	// Check if only a specific tq should be listed
    	if (!tqName.empty() && tqName != fRec->tq)
    	{
    		++it;
    		continue;
    	}

    	stringstream str;
    	str << fRec->tq << " " << fRec->destination << " " << (fRec->mirror ? " YES" : " NO") << " " << fRec->delay;
    	cmd.data[i++] = str.str();
    	++it;
    }
    return 0;
}


//******************************************************************************
//
bool aes_dbo_tqdatabase_impl::isDefined(const string &tq)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    if (list_.empty())
    {
        return false;     // Entry does not exists
    }

    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
        fRec = *it;
        if (tq == fRec->tq)
        {
            return true; // Entry exists
        }
        ++it;
    }
    return false;       // Entry does not exists
}


bool aes_dbo_tqdatabase_impl::isDefined(const string &tq, string& tqDN)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    if (list_.empty())
    {
        return false;     // Entry does not exists
    }

    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
        fRec = *it;
        if (tq == fRec->tq)
        {
			//get the DN of the tq from IMM.

			tqDN.clear();

			string mytqDN = AES_DBO_BLOCK_TRANSFER_QUEUE_RDN;
			mytqDN += "=";
			mytqDN += tq;
			mytqDN += ",";
		    mytqDN += AES_GCC_Util::dnOfBlockTransferM;		


			tqDN = mytqDN;
            return true; // Entry exists
        }
        ++it;
    }
    return false;       // Entry does not exists
}
//******************************************************************************
//
bool aes_dbo_tqdatabase_impl::isOpened(const string &tq)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		if (tq == fRec->tq)
		{
			AES_GCC_TRACE_MESSAGE("fRec->open = %d for TQ = %s", fRec->open, tq.c_str());
			if (fRec->open == 1)
			{
				return true;   // File is opened
			}

			else
			{
				return false;  // File is NOT opened
			}
		}
		++it;
	}

	return false;
}


//******************************************************************************
//
bool aes_dbo_tqdatabase_impl::destinationUsed(const std::string &tq, const std::string &dest)
{
    AES_GCC_TRACE_MESSAGE("In");

    ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
        fRec = *it;
        // Check if the destination is used with another TQ
        if (tq != fRec->tq && dest == fRec->destination)
        {
            return true;
        }
        ++it;
    }
    return false;
}


//******************************************************************************
//
void aes_dbo_tqdatabase_impl::open(const string &tq)
{

	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		if (tq == fRec->tq)
		{
			fRec->open = 1;  // Mark file opened
			AES_GCC_TRACE_MESSAGE("setting in ATTACH: fRec->open = %d for TQ = %s", fRec->open, tq.c_str());
			break;
		}
		++it;
	}

}


//******************************************************************************
//
void aes_dbo_tqdatabase_impl::close(const string &tq)
{
	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		if (tq == fRec->tq)
		{
			fRec->open = 0; // Mark file closed
			AES_GCC_TRACE_MESSAGE("resetting in DETACH: fRec->open = %d for TQ = %s", fRec->open, tq.c_str());
			break;
		}
		++it;
	}
}

bool aes_dbo_tqdatabase_impl::isTQAttached( string &destSet)
{
	AES_GCC_TRACE_MESSAGE("In");

	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	if (list_.empty())
	{
		AES_GCC_TRACE_MESSAGE("List empty!");
		return false;        // Entry does not exists
	}

	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		if(destSet == fRec->destination)
		{
			AES_GCC_TRACE_MESSAGE("returning TRUE");
			return true;
		}
		++it;
	}
	AES_GCC_TRACE_MESSAGE("returning FALSE");
	return false;
}

	

//******************************************************************************
//
unsigned int aes_dbo_tqdatabase_impl::getDestinationSet(const string &tq, string &dest)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	if (list_.empty())
		return AES_NODESTINATIONSET;        // Entry does not exists

	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		if (tq == fRec->tq)
		{
			dest = fRec->destination;
			return AES_DESTINATIONSETEXIST; // Entry exists
		}
		++it;
	}
	return AES_NODESTINATIONSET;            // Entry does not exists
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::getRemoveDelay(const string &tq, int &delay)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	if (list_.empty())
		return AES_NODESTINATION;        // Entry does not exists

	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		if (tq == fRec->tq)
		{
			delay = fRec->delay;
			return AES_DESTINATIONEXIST; // Entry exists
		}
		++it;
	}
	return AES_NODESTINATION;            // Entry does not exists
}


//******************************************************************************
//
unsigned int aes_dbo_tqdatabase_impl::mirrored(const string &tq)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		if (tq == fRec->tq)
		{
			return fRec->mirror;
		}
		++it;
	}
	return 0;
}


//******************************************************************************
//
int aes_dbo_tqdatabase_impl::getNrElements(void)
{
	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);
    int nrElem = list_.size();
    return nrElem;
}

//******************************************************************************
//
bool aes_dbo_tqdatabase_impl::isTqDefined(const std::string &tq)
{
	AES_GCC_TRACE_MESSAGE("In");

	ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	FileRecord *fRec;
	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		fRec = *it;
		// Check if the TQ is already defined
		if (tq == fRec->tq)
		{
			return true;
		}
		++it;
	}
	return false;
}

//******************************************************************************
//
bool aes_dbo_tqdatabase_impl::getTqDetails(const std::string &tq, std::string &dest, unsigned int &mirror,unsigned int &delay)
{
    AES_GCC_TRACE_MESSAGE("In");

    ACE_Read_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

    FileRecord *fRec;
    std::list<FileRecord *>::iterator it;
    it = list_.begin();
    while (it != list_.end())
    {
        fRec = *it;
        // Check if the TQ is already defined
        if (tq == fRec->tq)
        {
        	dest = fRec->destination;
        	mirror = fRec->mirror;
        	delay = fRec->delay;
        	AES_GCC_TRACE_MESSAGE("TQ FOUND: tq = %s, dest = %s, mirror = %d, delay = %d", tq.c_str(), dest.c_str(), mirror,delay);
        	return true;
        }
        ++it;
    }
    AES_GCC_TRACE_MESSAGE("TQ NOT FOUND");
    return false;
}

//******************************************************************************
//
void aes_dbo_tqdatabase_impl::clearTQListAtShutdown()
{
	ACE_Write_Guard<ACE_Recursive_Thread_Mutex> guard(mtx_);

	std::list<FileRecord *>::iterator it;
	it = list_.begin();
	while (it != list_.end())
	{
		FileRecord *fRec = *it;
		list_.erase(it);
		delete fRec;
		it = list_.begin();
	}
}

