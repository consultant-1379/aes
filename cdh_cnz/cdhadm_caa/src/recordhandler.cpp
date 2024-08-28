/*=================================================================== */
/**
@file recordhandler.cpp

Class method implementation for recordhandler.h

DESCRIPTION
The services provided by RecordHandler facilitates the handling of records.

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
#include <time.h> 
#include <iostream>
#include <recordhandler.h>
#include <servr.h>
#include <parameter.h>
#include <aes_gcc_util.h>

using namespace std;

/*===================================================================
   ROUTINE:RecordHandler
=================================================================== */
RecordHandler::RecordHandler() : hostName(""), fileStorePath(""), fileFullPath(""),
    lastTimeStamp(0),fileMaxAge(0)
{}


/*===================================================================
   ROUTINE:~RecordHandler
=================================================================== */
RecordHandler::~RecordHandler()
{
    std::map<const ACE_INT64, std::string>::iterator tItr = tdb.begin();
    std::map<std::string, NotificationRecord *>::iterator itr = db.begin();


    while (tItr != tdb.end())
    {
        tdb.erase(tItr);
        tItr = tdb.begin();
    }

    while (itr != db.end())
    {
        delete (*itr).second;
        db.erase(itr);
        itr = db.begin();
    }
}


/*===================================================================
   ROUTINE:add
=================================================================== */
bool RecordHandler::add(const std::string &fileName, NotificationRecord *nr, bool getTimeStamp)
{
    std::map<const ACE_INT64, std::string>::iterator tItr = tdb.begin();
    std::map<std::string, NotificationRecord *>::iterator itr = db.begin();
    
    itr = db.find(fileName);
    if (itr != db.end())
    {
        // Already exists, filename must be unique
        return false;
    }

    time_t timeStamp;
    if (getTimeStamp == true)
    {
        time(&timeStamp);
        
        // Make sure that the timestamp is unique
        if (timeStamp <= lastTimeStamp)
        {
            timeStamp = (lastTimeStamp + 1);
        }
        
        lastTimeStamp = timeStamp;
        nr->setTime(timeStamp);
    }

    bool b = false;
    timeStamp = nr->getTime();
    tdb.insert(std::make_pair(timeStamp, fileName));
    tItr = tdb.find(timeStamp);
    if (tItr != tdb.end())
    {
        db.insert(std::make_pair(fileName, nr));
        if (db.find(fileName) != db.end())
        {
            b = true;
        }
    }
    return b;
}


/*===================================================================
   ROUTINE:get
=================================================================== */
bool RecordHandler::get(const std::string &fileName, NotificationRecord *nr)
{
    std::map<const ACE_INT64, std::string>::iterator tItr = tdb.begin();
    std::map<std::string, NotificationRecord *>::iterator itr = db.begin();

    itr = db.find(fileName);
    if (itr == db.end())
    {
        return false;
    }

    // Copy from database
    nr->setStoreFileName( (*itr).second->getStoreFileName() );
    nr->setFileSize( (*itr).second->getFileSize() );
    nr->setMessageNumber( (*itr).second->getMessageNumber() );
    return true;
}


/*===================================================================
   ROUTINE:getFirst
=================================================================== */
bool RecordHandler::getFirst(NotificationRecord *rec)
{
    std::map<const ACE_INT64, std::string>::iterator tItr = tdb.begin();
    std::map<std::string, NotificationRecord *>::iterator itr = db.begin();

    itr = db.find((*tItr).second);
    if (itr == db.end())
    {
        // Filename not found although a timestamp exists for the file
        // Erase the timestamp reference to the file ????????
        
        // tdb.erase(tItr);
        return false;
    }

    // Copy from database
    rec->setStoreFileName( (*itr).second->getStoreFileName() );
    rec->setFileSize( (*itr).second->getFileSize() );
    rec->setMessageNumber( (*itr).second->getMessageNumber() );

    return true;
}


/*===================================================================
   ROUTINE:find
=================================================================== */
bool RecordHandler::find(const std::string &fileName, NotificationRecord *nr)const
{
	(void)fileName;
	(void)nr;
    return true;
}


/*===================================================================
   ROUTINE:remove
=================================================================== */
bool RecordHandler::remove(const std::string &fileName)
{
    std::map<const ACE_INT64, std::string>::iterator tItr = tdb.begin();
    std::map<std::string, NotificationRecord *>::iterator itr = db.begin();


    bool found = false;
    while (tItr != tdb.end() )
    {
        if ((*tItr).second == fileName)
        {
            found = true;
            break;
        }
        ++tItr;
    }

    if (!found)
    {
        return false;
    }

    itr = db.find((*tItr).second);
    if (itr == db.end())
    {
        // File name found in timestamp-database but not in database
        // What now??
    }
    else
    {
        delete (*itr).second;
        db.erase(itr);
        tdb.erase(tItr);
    }

    return true;
}


/*===================================================================
   ROUTINE:removeFirst
=================================================================== */
bool RecordHandler::removeFirst()
{
    std::map<const ACE_INT64, std::string>::iterator tItr = tdb.begin();
    std::map<std::string, NotificationRecord *>::iterator itr = db.begin();

    itr = db.find((*tItr).second);
    if (itr == db.end())
    {
        return false;
    }

    delete (*itr).second;
    db.erase(itr);
    tdb.erase(tItr);
    return true;
}

/*===================================================================
   ROUTINE:exportSingleRecord
=================================================================== */
bool RecordHandler::exportSingleRecord(std::stringstream &str, const std::string &dest,
                                       NotificationRecord *rec)
{
    str.clear();

    str << "D=" << dest.c_str() << "|";
    str << "HN=" << hostName << "|";
    str << "FSP=" << fileStorePath << "|";
    str << "FFP=" << fileFullPath << "|";
    str << "SFN=" << rec->getStoreFileName().c_str() << "|";
    str << "FS=" << rec->getFileSize().c_str() << "|";
    str << "T=" << rec->getTime() << "|";
    str << "MN=" << rec->getMessageNumber() << "|";
    str << "$" << endl;
    return true;
}


/*===================================================================
   ROUTINE:isEmpty
=================================================================== */
bool RecordHandler::isEmpty() const
{
    return tdb.size() == 0 ? true : false;
}


/*===================================================================
   ROUTINE:setHostName
=================================================================== */
void RecordHandler::setHostName(const std::string &name)
{
    hostName.assign(name);
}


/*===================================================================
   ROUTINE:setFileStorePath
=================================================================== */
void RecordHandler::setFileStorePath(const std::string &path)
{
	string spath("");
	string path1("");

	path1.assign(path);
	if( AES_GCC_Util::datapath_trn(path1,AES_DATA_PATH,spath) == false)
	{
		spath = path1;
	}
	fileStorePath.assign(spath);
}


/*===================================================================
   ROUTINE:setFileFullPath
=================================================================== */
void RecordHandler::setFileFullPath(const std::string &path)
{
	string rpath("");
	AES_GCC_Util::datapath_trn(path,AES_DATA_PATH,rpath);

	fileFullPath.assign(rpath);
}

/*===================================================================
   ROUTINE:setFileMaxAge
=================================================================== */
void RecordHandler::setFileMaxAge(const ACE_UINT32 &maxAge)
{
    fileMaxAge = maxAge;
}


/*===================================================================
   ROUTINE:getHostName
=================================================================== */
//std::string RecordHandler::getHostName() const
void RecordHandler::getHostName(std::string &hName) const
{
    hName.assign(hostName);
}


/*===================================================================
   ROUTINE:getFileStorePath
=================================================================== */
//std::string RecordHandler::getFileStorePath() const
void RecordHandler::getFileStorePath(std::string &path) const
{
	string dataTransferDir("");
	string spath("");
	if( ServR::getDataTransferFolderPath(dataTransferDir))
	{
		if(fileStorePath.length()>0)
		{

			size_t pos = fileStorePath.find(AES_DATA_PATH);
			if((pos!=string::npos))
			{
				spath = fileStorePath.substr(pos+strlen(AES_DATA_PATH.c_str()),strlen(fileStorePath.c_str()));
			}
			else
			{
				spath = fileStorePath;
			}
			dataTransferDir=dataTransferDir + "/";
			dataTransferDir=dataTransferDir + spath;
			path.assign(dataTransferDir);
			return;
		}
	}
	path.assign(fileStorePath);
}


/*===================================================================
   ROUTINE:getFileFullPath
=================================================================== */
//std::string RecordHandler::getFileFullPath() const
void RecordHandler::getFileFullPath(std::string &path) const
{
	 string dataTransferDir("");
	 string rpath("");
	 if( ServR::getDataTransferFolderPath(dataTransferDir))
	 {

		 if(fileFullPath.length()>0)
		{
			//string  match("/data_transfer/");
			size_t pos = fileFullPath.find(AES_DATA_PATH);
			if((pos!=string::npos))
			{
				rpath = fileFullPath.substr(pos+strlen(AES_DATA_PATH.c_str()),strlen(fileFullPath.c_str()));
			}
			else
			{
				rpath=fileFullPath;
			}
		}
			dataTransferDir=dataTransferDir + "/";
			dataTransferDir=dataTransferDir + rpath;
			path.assign(dataTransferDir);

			return;
		}

	path.assign(fileFullPath);
}

/*===================================================================
   ROUTINE:getFileMaxAge
=================================================================== */
void RecordHandler::getFileMaxAge(ACE_UINT32 &maxAge) const
{
    maxAge = fileMaxAge;
}


/*===================================================================
   ROUTINE:size
=================================================================== */
ACE_UINT32 RecordHandler::size()
{
	//ACE_UINT32 dbSize = db.size();
	ACE_UINT32 tdbSize = tdb.size();

    return tdbSize;     // tdbSize should be equal to dbSize !!
                        // but what if they differ, which one
                        // should be returned ??? 
}
