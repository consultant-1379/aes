/*=================================================================== */
/**
@file database.cpp

Class method implementation for database.h

DESCRIPTION
The services provided by DataBase facilitates the handling of a DataBase.

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
#include <fstream>
#include <iostream>
#include <sstream>
#include <database.h>
#include <notificationrecord.h>
#include <list>
#include <aes_gcc_variable.h>
#include <event.h>
#include <parameter.h>
#include <ace/ACE.h>
#include <dirent.h>
#include <aes_gcc_util.h>
#include <servr.h>
#include <aes_cdh_common.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;
AES_CDH_TRACE_DEFINE(AES_CDH_DataBase);

// Initialize static pointer to the database
DataBase *DataBase::dbPtr = NULL;

/*===================================================================
   ROUTINE:init
=================================================================== */
#if 0
// init() - initializes the semaphore
bool DataBase::init()
{
    return true;
}
#endif

/*===================================================================
   ROUTINE:instance
=================================================================== */
//********************************************************************************************
// instance() - returns a pointer to the database
//********************************************************************************************
DataBase *DataBase::instance()
{
	if (dbPtr == NULL)
	{
		dbPtr = new DataBase;
	}

	return dbPtr;
}
/*===================================================================
ROUTINE:restore
=================================================================== */
bool DataBase::restore()
{
	ACE_TCHAR buf[4096];
	int r = 0;
	ifstream fin;
	NotificationRecord *rec;
	std::string dest("");
	std::string host("");
	std::string storePath("");
	std::string fullPath("");
	std::string fileFullPath("");
	std::string fileFulltPath("");
	std::string fileStorePath("");
	std::string path(dbPath);
	std::list<std::string> dirList;

	AES_CDH_TRACE_MESSAGE( "Entering , path = %s", path.c_str());
	DIR* pDir = opendir((ACE_TCHAR*)(path.c_str()));
	if ( !pDir)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "opendir failed for path, errno = %d", errno);
		return false;
	}
	dirent* pEntry=readdir(pDir);

	while ( pEntry != NULL)
	{
		string fileN = path + "/" + string(pEntry->d_name);
		struct stat buf;
		int ret = stat(fileN.c_str(), &buf);
		if (ret == 0)
		{
			if ( S_ISDIR(buf.st_mode) && ACE_OS::strcmp ( pEntry->d_name, ".") && ACE_OS::strcmp ( pEntry->d_name, ".."))
			{
				AES_CDH_TRACE_MESSAGE( "Directory %s is added to list", pEntry->d_name);
				dirList.push_back(pEntry->d_name);
			}
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Stat failed, errno = %d", errno);
		}

		pEntry=readdir(pDir);
	}
	r = closedir(pDir);
	if (r == -1)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Closedir failed, errno = %d", errno);
	}

	std::list<std::string>::iterator itr = dirList.begin();
	AES_CDH_TRACE_MESSAGE("ServR::isStopEventSignalled = %d", ServR::isStopEventSignalled);
	if ( itr == dirList.end())
		AES_CDH_TRACE_MESSAGE("no rec in db");

	while ( itr != dirList.end() && !ServR::isStopEventSignalled )
	{
		std::string destDirName = (*itr);
		path = dbPath + "/" +  destDirName;
		DIR * pDir1 = opendir((ACE_TCHAR*)(path.c_str()));
		if (!pDir1)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "opendir failed on %s, errno  = %d", path.c_str(),errno);
			return false;
		}
		AES_CDH_TRACE_MESSAGE("opendir on %s passed", (ACE_TCHAR*)(path.c_str()));
		dirent* pEntry1=readdir( pDir1);
		while ( pEntry1 && !ServR::isStopEventSignalled)
		{
			if (ACE_OS::strcmp ( pEntry1->d_name, ".") && ACE_OS::strcmp ( pEntry1->d_name, ".."))
			{
				fullPath = path + "/" + pEntry1->d_name;
				AES_CDH_TRACE_MESSAGE("opening file %s", fullPath.c_str());
				fin.open(fullPath.c_str());
				fin.clear();
				fin.getline(buf, 4096, '$'); //TBD add err handling
				fin.close();
				AES_CDH_TRACE_MESSAGE("getline buf %s", buf);

				rec = new NotificationRecord;
				if (createRecord(dest, buf, rec, host, storePath, fullPath))
				{
					AES_CDH_TRACE_MESSAGE( "########### fullPath = %s, storePath  = %s ",  fullPath.c_str(),storePath.c_str());

					std::string fName = rec->getStoreFileName();
					std::string fSize = rec->getFileSize();
					if( fullPath.find_first_of("\\") != std::string::npos)
					{
						convertToLinuxFormatFFP(fullPath, fileFulltPath);

						std::string convertedFFPPath = strToConvert(fileFulltPath, dest);

						fileFullPath = AES_DATA_PATH + convertedFFPPath ;
						//fileFullPath = AES_DATA_PATH + fileFulltPath ;

						convertedFFPPath = "";
						AES_CDH_TRACE_MESSAGE( "######## APPENDED %s #########", fileFullPath.c_str());
					}

					if(fileFullPath.empty())
						fileFullPath = fullPath;

					if(storePath.find_first_of("\\")  != std::string::npos)
					{
						convertToLinuxFormatFFP(storePath, fileStorePath);

						std::string convertedFSPPath = strToConvert(fileStorePath, dest);

						fileStorePath = convertedFSPPath ;

						convertedFSPPath = "";
					}

					if(fileStorePath.empty())
						fileStorePath = storePath;

					AES_CDH_TRACE_MESSAGE( "$$$$$$$$$$$***********add dest = %s, fName = %s, fullLinuxPath = %s, storePath = %s", dest.c_str(), fName.c_str(),fileFullPath.c_str(),fileStorePath.c_str());
					add(dest,
							fName,
							fSize,
							rec,
							host,
							fileStorePath,
							fileFullPath,
							false);

					fileStorePath="";
					fileFullPath="";
					fileFulltPath="";
				}

				else
				{
					// createRecord failed, rec is not stored in the database
					// delete it to avoid memory leak
					AES_CDH_TRACE_MESSAGE("createRecord failed");
					delete rec;
					break;
				}
			}
			pEntry1=readdir( pDir1);
		}
		r = closedir(pDir1);
		if (r == -1)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "closedir failed, errno = %d", errno);
		}

		++itr;
	}
	dirList.clear();
	AES_CDH_TRACE_MESSAGE("Leaving ...");
	return true;
}
/*===================================================================
   ROUTINE:createRecord
=================================================================== */
bool DataBase::createRecord(std::string &dest,
                            const std::string &msg,
                            NotificationRecord *nr,
                            std::string &host,
                            std::string &storePath,
                            std::string &fullPath)
{
	/* #ifdef DEBUGTEST
        cerr << "DataBase:: ENTERING createRecord()," << endl;
    #endif*/

	std::string param("");
	std::string value("");
	std::string field("");
	std::string s(msg);
	bool returnSts = true;

	// D   : Destination name
	// HN  : Host name
	// FSP : File Store Path
	// FFP : File Full Path
	// SFN : Store File Name
	// FS  : File Size
	// T   : Time stamp
	// MN  : Message Number


	ACE_INT32 pos = s.find_first_of("|");
	if (pos != -1)
	{
		while (pos != -1)
		{
			field = s.substr(0, pos);

			ACE_UINT32 left  = field.find_first_of('=');
			ACE_UINT32 right = left + 1;

			param = field.substr(0, left);
			value = field.substr(right);

			if (param == "D")
			{
				dest.assign(value);
			}

			if (param == "HN")
			{
				host.assign(value);
			}
			else if (param == "FSP")
			{
				storePath.assign(value);
			}
			else if (param == "FFP")
			{
				fullPath.assign(value);
			}
			else if (param == "SFN")
			{
				nr->setStoreFileName(value);
			}
			else if (param == "FS")
			{
				nr->setFileSize(value);
			}
			else if (param == "MN")
			{
				ACE_UINT32 iValue = atoi(value.c_str());
				nr->setMessageNumber(iValue);
			}
			else if (param == "T")
			{
				time_t lValue = atol(value.c_str());
				nr->setTime(lValue);
			}

			pos = s.find_first_of('|');
			s = s.substr(pos + 1);
			pos = s.find_first_of('|');
		}
	}
	else
	{
		returnSts = false;
	}

	/* #ifdef DEBUGTEST
        cerr << "DataBase:: LEAVING createRecord(), returning"
             << (returnSts == true ? "TRUE" : "FALSE") << endl;
    #endif*/

	return returnSts;
}


/*===================================================================
   ROUTINE:appendRecordToFile
=================================================================== */
bool DataBase::appendRecordToFile(const std::string &dest,
                                  const std::string &fileName,
                                  const std::string &fileSize,
                                  NotificationRecord *rec)
{
	ofstream fout;
	//std::strstream str;
	std::stringstream str;
	AES_CDH_TRACE_MESSAGE("Entering , dest = %s", dest.c_str());

	std::string path = dbPath + "/" + dest;

	std::map<std::string, RecordHandler *>::iterator itr = db.begin();
	AES_CDH_TRACE_MESSAGE("path = %s", path.c_str());

	itr = db.find(dest);
	if (itr == db.end())
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "dest not found, returning false");
		return false;
	}

	rec->setStoreFileName(fileName);
	rec->setFileSize(fileSize);
	bool b = (*itr).second->exportSingleRecord(str, dest, rec);
	str << ends;
	//    str.rdbuf()->freeze(true);

	std::string p = str.str();


	if (b == true)
	{
		// Store the record in the database

		AES_CDH_TRACE_MESSAGE("storing record in database");
		std::string fName = path + "/" + fileName;
		AES_CDH_TRACE_MESSAGE("Opening fName = %s", fName.c_str());
		fout.open(fName.c_str(), ios::out);
		if (!fout)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "file open failed");
			b = false;
		}
		else
		{
			fout << p;
		}
		fout.close();
	}
	// Free memory allocated by the stream
	return b;
}
/*===================================================================
   ROUTINE:DataBase
=================================================================== */
DataBase::DataBase() : dbPath("")
{
	std::string dataDir("");
	std::string cdhRootDir("");

	// bool readPar =
	readParameters(dbPath);

	bool b = restore();
	if (b == true)
	{
		//cerr << "DataBase restored" << endl;
	}
	else
	{
		//cerr << "DataBase NOT restored" << endl;
	}
}
/*===================================================================
   ROUTINE:~DataBase
=================================================================== */
DataBase::~DataBase()
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	while (itr != db.end())
	{
		delete (*itr).second;
		itr = db.begin();
	}
	AES_CDH_TRACE_MESSAGE("Leaving ...");
}
/*===================================================================
   ROUTINE:add
=================================================================== */
bool DataBase::add(const std::string &dest,
                   const std::string &fileName,
                   const std::string &fileSize,
                   NotificationRecord *nr,
                   const std::string &host,
                   const std::string &storePath,
                   const std::string &fullPath,
                   bool writeToFile,
                   ACE_UINT32 maxAge)
{
	// Note: The parameter writeFile has a default value of true,
	//       If set to false, the record will not be written to disk.
	//       It is also used to determine whether a timestamp should
	//       be retrieved or not, see variable 'getTimeStamp' below

	/* #ifdef DEBUGTEST
        cerr << "DataBase:: add()," << endl;
    #endif*/
	AES_CDH_TRACE_MESSAGE( "Entering , writeToFile = %d", writeToFile);
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	itr = db.find(dest);
	if (itr == db.end())        // Dest not found, add to database
	{
		RecordHandler *recH = new RecordHandler;
		if (recH == NULL)
		{
			/*#ifdef DEBUGTEST
                cerr << "DataBase:: add(), Memory Allocation Failure, returning FALSE" << endl;
            #endif*/
			// Memory allocation error
			AES_CDH_LOG(LOG_LEVEL_FATAL, "RecordHandler allocation failed");
			return false;
		}

		recH->setHostName(host);
		recH->setFileStorePath(storePath);
		recH->setFileFullPath(fullPath);
		recH->setFileMaxAge(maxAge);
		AES_CDH_TRACE_MESSAGE("adding to db with dest = %s",dest.c_str());
		db.insert(std::make_pair(dest, recH));
		itr = db.find(dest);
		if (itr == db.end())
		{
			/*#ifdef DEBUGTEST
                cerr << "DataBase:: add(), Insert Failure, returning FALSE" << endl;
            #endif*/

			// Insert operation failed
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Database insertion failed");
			return false;
		}
	}

	bool getTimeStamp = true;
	if (writeToFile == false)
	{
		getTimeStamp = false;
	}

	// Destination exists in the database, add the record

	AES_CDH_TRACE_MESSAGE( "Calling  with fileName = %s",fileName.c_str());
	bool b = (*itr).second->add(fileName, nr, getTimeStamp);
	if (b == true && writeToFile == true)
	{
		AES_CDH_TRACE_MESSAGE("Appending record to Database");
		b = appendRecordToFile(dest, fileName, fileSize, nr);
		AES_CDH_TRACE_MESSAGE("Appending record to Database returned = %d", b);

	}

	/*#ifdef DEBUGTEST
        cerr << "DataBase:: LEAVING add(), returning"
             << (returnSts == true ? "TRUE" : "FALSE") << endl;
    #endif*/

	AES_CDH_TRACE_MESSAGE( "Leaving , b = %d", b);
	return b;
}
/*===================================================================
   ROUTINE:get
=================================================================== */
bool DataBase::get(const std::string &dest,
                   const std::string &fileName,
                   std::string &hostName,
                   std::string &storePath,
                   std::string &fullPath,
                   NotificationRecord *rec)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	itr = db.find(dest);
	if (itr == db.end())        // Dest does not exist
	{
		return false;
	}

	(*itr).second->getHostName(hostName);
	(*itr).second->getFileStorePath(storePath);
	(*itr).second->getFileFullPath(fullPath);
	bool b = (*itr).second->get(fileName, rec);
	return b;
}
/*===================================================================
   ROUTINE:getFirst
=================================================================== */
bool DataBase::getFirst(const std::string &dest,
                        std::string &hostName,
                        std::string &storePath,
                        std::string &fullPath,
                        NotificationRecord *rec)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	itr = db.find(dest);
	if (itr == db.end())
	{
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "returning FALSE");
		return false;
	}

	(*itr).second->getHostName(hostName);
	(*itr).second->getFileStorePath(storePath);
	(*itr).second->getFileFullPath(fullPath);
	bool b = (*itr).second->getFirst(rec);
	AES_CDH_TRACE_MESSAGE("Leaving with return %d",b);
	return b;
}
/*===================================================================
   ROUTINE:getFirst
=================================================================== */
bool DataBase::getFirst(const std::string &dest, NotificationRecord *rec)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	itr = db.find(dest);
	if (itr == db.end())
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "2 param returning FALSE");
		return false;
	}

	bool b = (*itr).second->getFirst(rec);


	/* #ifdef DEBUGTEST
        cerr << "DataBase:: LEAVING getFirst(), returning"
             << (returnSts == true ? "TRUE" : "FALSE") << endl;
    #endif*/

	AES_CDH_TRACE_MESSAGE("getFirst with 2 params returning %d",b);
	return b;
}
/*===================================================================
   ROUTINE:remove
=================================================================== */
bool DataBase::remove(const std::string &dest, const std::string &fileName)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();
	std::string path(dbPath);

	itr = db.find(dest);
	if (itr == db.end())
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "returning FALSE");
		return false;
	}

	bool b = (*itr).second->remove(fileName);

	if (b == true)
	{
		AES_CDH_TRACE_MESSAGE("b = TRUE");
		std::string fileName = path + fileName;
		if (::remove(fileName.c_str()) == -1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "remove failed,b = FALSE");
			b = false;
		}
	}

	AES_CDH_TRACE_MESSAGE("Leaving with return b = %d",b);
	return b;
}
/*===================================================================
   ROUTINE:removeFirst
=================================================================== */
bool DataBase::removeFirst(const std::string &dest)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();
	std::string path(dbPath);

	itr = db.find(dest);
	if (itr == db.end())
	{
		/*#ifdef DEBUGTEST
            cerr << "DataBase:: removeFirst(), destination "
                 << dest.c_str() << " not found" << endl;
        #endif*/

		AES_CDH_LOG(LOG_LEVEL_ERROR, "Returning FALSE");
		return false;
	}

	NotificationRecord *rec = new NotificationRecord;
	if ( rec == NULL)
		AES_CDH_LOG(LOG_LEVEL_FATAL, "Notification record allocation failure");


	bool b = (*itr).second->getFirst(rec);
	b = (*itr).second->removeFirst();

	if (b == true)
	{
		AES_CDH_TRACE_MESSAGE("DataBase::removeFirst b = TRUE");
		std::string fileName = path + "/" + dest + "/" + rec->getStoreFileName();
		AES_CDH_TRACE_MESSAGE( " removing file %s",fileName.c_str());
		if (::remove(fileName.c_str()) == -1)
		{
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "DataBase::removeFirst b = FALSE");
			b = false;
		}
	}

	/*#ifdef DEBUGTEST
        cerr << "DataBase:: LEAVING removeFirst(), returning"
             << (returnSts == true ? "TRUE" : "FALSE") << endl;
    #endif*/

	delete rec;
	AES_CDH_TRACE_MESSAGE("Leaving with return b = %d",b);
	return b;

}
/*===================================================================
   ROUTINE:isEmpty
=================================================================== */
bool DataBase::isEmpty(const std::string &dest)
{
	// Note: This method checks whether the database is empty with respect
	//       to the parameter dest. The database may contain entries for
	//       other destinations, although it returns that the database is
	//       empty for the destination specified by the parameter dest.

	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	AES_CDH_TRACE_MESSAGE("Entering with dest = %s",dest.c_str());
	AES_CDH_TRACE_MESSAGE("map size = %d",int(db.size()));
	itr = db.find(dest);
	if (itr == db.end())
	{
		AES_CDH_TRACE_MESSAGE("no record in db returning true");
		return true;
	}
	else
	{
		bool b =  (*itr).second->isEmpty();
		AES_CDH_TRACE_MESSAGE("returning with b = %d",b);
		return b;
	}
	AES_CDH_TRACE_MESSAGE("Leaving with return true");
	return true;
}
/*===================================================================
   ROUTINE:size
=================================================================== */
ACE_INT32 DataBase::size(const std::string &dest)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	itr = db.find(dest);
	if (itr == db.end())
		return -1;
	else
		return (*itr).second->size();

	return -1;
}
/*===================================================================
   ROUTINE:getFileStorePath
=================================================================== */
bool DataBase::getFileStorePath(const std::string &dest, std::string &fileStorePath)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	if ((itr = db.find(dest)) == db.end())
	{
		// Destination not found in database
		return false;
	}

	(*itr).second->getFileStorePath(fileStorePath);
	return true;
}
/*===================================================================
   ROUTINE:getFileFullPath
=================================================================== */
bool DataBase::getFileFullPath(const std::string &dest, std::string &fileFullPath)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	if ((itr = db.find(dest)) == db.end())
	{
		// Destination not found in database
		return false;
	}

	(*itr).second->getFileFullPath(fileFullPath);
	return true;
}
/*===================================================================
ROUTINE:getHostName
=================================================================== */
bool DataBase::getHostName(const std::string &dest, std::string &hostName)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	if ((itr = db.find(dest)) == db.end())
	{
		// Destination not found in database
		return false;
	}

	(*itr).second->getHostName(hostName);
	return true;
}
/*===================================================================
   ROUTINE:getFileMaxAge
=================================================================== */
bool DataBase::getFileMaxAge(const std::string &dest,ACE_UINT32 &maxAge)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();

	if ((itr = db.find(dest)) == db.end())
	{
		// Destination not found in database
		return false;
	}

	(*itr).second->getFileMaxAge(maxAge);
	return true;
}
/*===================================================================
   ROUTINE:createDestinationEntry
=================================================================== */
bool DataBase::createDestinationEntry(const std::string &dest,
                                      const std::string &fileStorePath,
                                      const std::string &fileFullPath,
                                      const std::string &hostName,
                                      const ACE_UINT32 maxAge)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();
	AES_CDH_TRACE_MESSAGE("Entering ...");

	itr = db.find(dest);
	if (itr == db.end())        // Dest not found, create entry
	{
		AES_CDH_TRACE_MESSAGE("createDestinationEntry dest not found");
		RecordHandler *recH = new RecordHandler;
		if (recH == NULL)
		{
			AES_CDH_LOG(LOG_LEVEL_FATAL, "DataBase::createDestinationEntry RecordHandler allocation failure");
			return false;
		}

		recH->setHostName(hostName);
		recH->setFileStorePath(fileStorePath);
		recH->setFileFullPath(fileFullPath);
		recH->setFileMaxAge(maxAge);
		AES_CDH_TRACE_MESSAGE("Adding to db with dest = %s",dest.c_str());
		db.insert(std::make_pair(dest, recH));
		itr = db.find(dest);
		if (itr == db.end())
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "DataBase::createDestinationEntry database insertion failure");
			// Insert operation failed
			return false;
		}
	}
	else
	{
		// If dest is found only set max age property
		if(itr->second != NULL)
		{
			AES_CDH_TRACE_MESSAGE("Dest is found only set max age property");
			itr->second->setFileMaxAge(maxAge);
			itr->second->setHostName(hostName);
		}
	}

	AES_CDH_TRACE_MESSAGE("Leaving ..");
	return true;
}
/*===================================================================
   ROUTINE:destroyDestinationEntry
=================================================================== */
void DataBase::destroyDestinationEntry(const std::string &dest)
{
	std::map<std::string, RecordHandler *>::iterator itr = db.begin();
	AES_CDH_TRACE_MESSAGE("Entering ...");

	itr = db.find(dest);
	if (itr == db.end())        // Dest not found
	{
		AES_CDH_TRACE_MESSAGE("dest not found");
		// Nothing to destroy
	}
	else
	{
		// Free memory used by this destination
		RecordHandler *recH = (*itr).second;
		delete recH;

		// Remove records from disk for this destination
		std::string path = dbPath + "/" + dest ;
		std::string fileN("");

		DIR* pDir2 = opendir((ACE_TCHAR*)path.c_str());
		if ( !pDir2)
		{ 
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Dir open failed %s", path.c_str());
			AES_CDH_LOG(LOG_LEVEL_TRACE, "Leaving ..");
			return;
		}
		dirent* pEntry2 = readdir(pDir2);
		while ( pEntry2 )
		{
			if (ACE_OS::strcmp ( pEntry2->d_name, ".") && ACE_OS::strcmp ( pEntry2->d_name, ".."))
			{
				fileN = dbPath + "/" + dest + "/" + pEntry2->d_name;

				if (::remove(fileN.c_str()) != 0)
				{
					AES_CDH_LOG(LOG_LEVEL_ERROR, "failed to remove file %s",fileN.c_str());
					// Failed to remove file
				}
			}

			pEntry2 = readdir( pDir2);
		}
		closedir(pDir2);
		db.erase(itr);
	}
	AES_CDH_TRACE_MESSAGE("Leaving..");
}


/*===================================================================
   ROUTINE:readParameters
=================================================================== */
bool DataBase::readParameters(std::string &path)
{
	AES_CDH_TRACE_MESSAGE("Entering ..");

	// Get parameters from PHA
	if (AES_CDH_Paths::instance()->getCDHDataPath( path ) )
	{
		AES_CDH_TRACE_MESSAGE( "aesDataDirectory.getStr() = %s", path.c_str());
		path += "/";
		path += AES_CDH_RootDirectory;
		AES_CDH_TRACE_MESSAGE( "path = %s", path.c_str());

		path += "/ndb";
		AES_CDH_TRACE_MESSAGE("ndb path = %s", path.c_str());

		return true;
	}
	else
	{
		// Event report, could not get parameters from PHA
		Event::report(AES_CDH_variableFault, "VARIABLE PROBLEM",
				"Retrieval of variable (" + string(AES_CDH_AesDataDirectory) +
				") from GCC failed.", "-");
	}
	AES_CDH_TRACE_MESSAGE("%s", "Leaving with FALSE");
	return false;
}


void DataBase::convertToLinuxFormat(std::string& orgPath, std::string& convPath)
{
	AES_CDH_TRACE_MESSAGE("To be converted path %s ", orgPath.c_str());
	char *cstr = new char[orgPath.length() + 1];
	char *ptr = NULL;
	strcpy(cstr, orgPath.c_str());
	ptr=cstr;

	while(*ptr)
	{
		if(*ptr=='\\')
			*ptr='/';
		++ptr;
	}

	convPath = cstr;
	AES_CDH_TRACE_MESSAGE("converted LINUX PATH %s ", convPath.c_str());

	delete [] cstr;
	cstr = NULL;	

}

void DataBase::convertToLinuxFormatFFP(std::string& orgPath, std::string& convpath)
{
	char* pch = NULL;
	char *str = new char[orgPath.length() + 1];
	char finpath[1024];

	char last[256]={};
	char lastbutt[256]={};

	strcpy(str, orgPath.c_str());

	memset(finpath, '\0', sizeof(finpath));
	memset(last, '\0', sizeof(last));
	memset(lastbutt, '\0', sizeof(lastbutt));


	AES_CDH_TRACE_MESSAGE("To be converted orgpath %s and str = %s ", orgPath.c_str(),str);

	pch = strtok (str," \\");

	while (pch != NULL)
	{
		strcpy(lastbutt,last);
		strcpy(last,pch);
		printf ("\n%s\n",pch);
		pch = strtok (NULL, "\\");
	}

	strcpy(finpath,str);
	if(strcmp(last,str)==0)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "NOT WINDOWS PATH %s ", str);
		delete[] str;
		return;
	}
	else if(lastbutt[0]=='\0')
	{
		sprintf(finpath,"%s",last);
	}
	else
	{
		sprintf(finpath,"%s/%s",lastbutt,last);
	}

	convpath = finpath;		

	AES_CDH_TRACE_MESSAGE("converted FFP path %s ", convpath.c_str());
	delete[] str;
}

std::string DataBase::strToConvert(std::string stringToModify, std::string destName )
{
	size_t foundAt= 0;

	std::string convertToS = destName;
	std::transform(convertToS.begin(), convertToS.end(), convertToS.begin(), ::tolower);

	AES_CDH_TRACE_MESSAGE("converted to caps path %s ", convertToS.c_str());
	while((foundAt = stringToModify.find(convertToS)) != std::string::npos)
	{
		stringToModify.replace(foundAt, destName.length(), destName);
	}

	AES_CDH_TRACE_MESSAGE(" final converted to caps path %s ", stringToModify.c_str());

	return stringToModify;
}
