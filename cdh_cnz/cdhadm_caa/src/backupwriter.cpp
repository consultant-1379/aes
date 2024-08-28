/*=================================================================== */
/**
 @file backupwriter.cpp

Class method implementation for backupwriter.h

DESCRIPTION
The services provided by BackupWriter facilitates the backup storage.
Handles mapping of block-pair objects during runtime for destination in a destination set.
ERROR HANDLING
General rule:l

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       21/09/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <sstream>
#include <time.h>
#include <servr.h>
#include <backupwriter.h>
#include <parameter.h>
#include <event.h>
#include <sys/timeb.h>

#include <AES_DBO_DataBlock.h>
#include <aes_gcc_variable.h>
#include <aes_gcc_log.h>
#include <ftprv2backup.h>
#include <sshftprv2backup.h>
#include <ace/ACE.h>
#include <aes_cdh_common.h>

using namespace std;
/*===================================================================
                        DECLARATION SECTION
=================================================================== */
// ACS Trace definition
GCC_TDEF(AES_CDH_BackupWriter);

/*===================================================================
   ROUTINE:BackupWriter
=================================================================== */
//##ModelId=3DF4991801ED
BackupWriter::BackupWriter() : hFile(ACE_INVALID_HANDLE),
    hMapFile(hFile), hTarget(hFile), mapAddr(0), path(""),
    FTPRV2BackupPtr(0), theSourceId(""), SwriteFile(""), SSHFTPRV2BackupPtr(0)
{}

/*===================================================================
   ROUTINE:~BackupWriter
=================================================================== */
//##ModelId=3DF4992202C3
BackupWriter::~BackupWriter()
{}

/*===================================================================
   ROUTINE:open
=================================================================== */
//##ModelId=3DF4992B035C
bool BackupWriter::open(const string &destSetName, const string &sourceId, FTPRV2Backup *ptr)
{
    // (1) Locate the file with the same name as sourceId
    // (2) Get access to it as a memory mapped file
    // (3) Open the file in which we write the blocks


    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::open() - entering\n"));
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::open() sourceId = %S\n",sourceId.c_str()));
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::open() destSetName = %S\n",destSetName.c_str()));

    FTPRV2BackupPtr = ptr;
    theSourceId = sourceId;

    string aesApDataDir = "";

    if (AES_CDH_Paths::instance()->getCDHDataPath( aesApDataDir ) )
    {
	aesApDataDir.append("/");
	aesApDataDir.append(AES_CDH_RootDirectory);
    }
    else
    {
        Event::report(AES_CDH_variableFault, "VARIABLE PROBLEM",
            "Retrieval of variable (" + string(AES_CDH_AesDataDirectory) +
            ") from GCC failed.", "-");
        return false;
    }

    aesApDataDir.append("/bfi");


    // -------------------------------------------------------------------
    // Create <aesapdata>\cdh\bfi directory if it does not already exist
	if (ACE_OS::access(aesApDataDir.c_str(), 0) != -1)
    {
		if ((ACE_OS::mkdir(aesApDataDir.c_str(), NULL) == -1) &&
            (ACE_OS::last_error() != EEXIST))
        {
            // Directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + aesApDataDir);
            return false;
        }
    }

    
    aesApDataDir += "/" + destSetName;

     // -------------------------------------------------------------------
    // Create <aesapdata>\cdh\bfi\<destsetname> directory if it
    // does not already exist
  	if (ACE_OS::access(aesApDataDir.c_str(), 0) != -1)
    {
		if ((ACE_OS::mkdir(aesApDataDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
       
        {
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            ServR::NTErrorText(), "Failed to create directory: " + aesApDataDir);
            return false;
        }
    }
    
    string aesApDataBackupFile = aesApDataDir + "/files";
    aesApDataDir += "/streamid";

    // -------------------------------------------------------------------
    // Create <aesapdata>\bfi\cdh\<destsetname>\streamid directory
    // if it does not already exist
    if (ACE_OS::access(aesApDataDir.c_str(), 0) != -1)
    {
       	if ((ACE_OS::mkdir(aesApDataDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + aesApDataDir);
            return false;
        }
    }
    
    // ***
    // Done creating <aesapdata>... directory structure
    // ***
    
    //TODO
    string vdDestDir = FTPRV2BackupPtr->getVDPath() + "/-bfi";


    // -------------------------------------------------------------------
    // Create <virtualdir>\-bfi directory if
    // it does not alredy exist
 	if (ACE_OS::access(vdDestDir.c_str(), 0) != -1)
    {
        if ((ACE_OS::mkdir(aesApDataDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + vdDestDir);
            return false;
        }
        else
        {
            if (! ServR::clearAcl(vdDestDir))
                return false;
        }
    }

    vdDestDir += "/" + destSetName;

    // -------------------------------------------------------------------
    // Create <virtualdir>\-bfi\<destsetname> directory if
    // it does not alredy exist
  	if (ACE_OS::access(vdDestDir.c_str(), 0) != -1)
    {
        if ((ACE_OS::mkdir(vdDestDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + vdDestDir);
            return false;
        }
    }

    vdDestDir += "/files";
    path = vdDestDir;

    // -------------------------------------------------------------------
    // Create <virtualdir>\-bfi\<destsetname>\files directory if
    // it does not alredy exist
   	if (ACE_OS::access(vdDestDir.c_str(), 0) != -1)
    {
       	if ((ACE_OS::mkdir(vdDestDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + vdDestDir);
            return false;
        }
    }

    
    // ***
    // Done creating <virtualdir>... directory structure
    // ***
    
    aesApDataDir += "/" + sourceId;
    
    // -------------------------------------------------------------------
    // open/create the sourceId file to be used. If the file exists,
    // it is opened, if it does not exist, it is created

	hFile = ACE_OS::open(aesApDataDir.c_str(),O_RDWR,
                       ACE_DEFAULT_OPEN_PERMS,
                       NULL);
    if (hFile == ACE_INVALID_HANDLE)
    {
        GCC_TINFO((AES_CDH_BackupWriter, "(%t) BackupWriter::open()"
			"Failed to open %S , error code: %d\n", aesApDataDir.c_str(), ACE_OS::last_error()));
        
        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            ServR::NTErrorText(), "Failed to open file: " + aesApDataDir);
        return false;
    }
    
    GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::open() File opened %S\n", aesApDataDir.c_str()));
    
    // -------------------------------------------------------------------
    /*hMapFile = ::CreateFileMapping(hFile,
                                   NULL,
                                   PAGE_READWRITE,
                                   0,
                                   2 * sizeof(ACE_UINT32),
                                   NULL);*/

	 mapAddr = ACE_OS::mmap(0,2 * sizeof(ACE_UINT32),PROT_RDWR,MAP_SHARED,hFile,0);

    if (mapAddr == NULL)
    {
        ACE_OS::close(hFile);
        ACE_OS::close(hMapFile);
        
        Event::report(AES_CDH_intProgFault, "INTERNAL PROGRAM ERROR",
            ServR::NTErrorText(), "-");
        return false;
    }

    GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::open() File mapped %S\n", aesApDataDir.c_str()));
  
    ACE_TCHAR *cp = (ACE_TCHAR *)mapAddr;
    ACE_OS::memcpy((void *)&fileEntry, cp, sizeof(FileEntry));
    GCC_DEBUG(("(%t) BackupWriter::open() CDRBid = %d , fileSize = %d\n", fileEntry.CDRBid, fileEntry.fileSize));


    vdDestDir += "/CURRENT";
    
	hTarget = ACE_OS::open(vdDestDir.c_str(),                // file
						O_RDWR,        // access mode
                       ACE_DEFAULT_OPEN_PERMS,
                       NULL);

	if (hTarget == ACE_INVALID_HANDLE)
    {
		ACE_OS::close(hFile);
		ACE_OS::munmap(mapAddr,0);
        mapAddr = 0;
        
        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            ServR::NTErrorText(), "Failed to open file: " + vdDestDir);
        return false;
    }

    GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::open() File opened %S\n", vdDestDir.c_str()));

    SwriteFile = vdDestDir;

	this->fileEntry.fileSize = ACE_OS::filesize(hTarget);
    ACE_OS::memcpy(cp, (void *)&fileEntry, sizeof(FileEntry));

    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::open() - leaving\n"
        "Size: %S = %u\n"
        "Returning true\n", vdDestDir.c_str(), this->fileEntry.fileSize));

    return true;
}

/*===================================================================
   ROUTINE:open
=================================================================== */
// ***********************************************************************
// open()
// Added new method for SFTP responding IP
// ***********************************************************************

bool BackupWriter::open(const string &destSetName, const string &sourceId, SSHFTPRV2Backup *ptr)
{
    // (1) Locate the file with the same name as sourceId
    // (2) Get access to it as a memory mapped file
    // (3) Open the file in which we write the blocks


    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::open() - entering\n"));

    SSHFTPRV2BackupPtr = ptr;
    theSourceId = sourceId;

    string aesApDataDir = "";

    if (AES_CDH_Paths::instance()->getCDHDataPath( aesApDataDir ) )
    {
	aesApDataDir += "/";
	aesApDataDir += AES_CDH_RootDirectory;
    }
    else
    {
        Event::report(AES_CDH_variableFault, "VARIABLE PROBLEM",
            "Retrieval of variable (" + string(AES_CDH_AesDataDirectory) +
            ") from GCC failed.", "-");
        return false;
    }

    aesApDataDir.append("/bfi");


    // -------------------------------------------------------------------
    // Create <aesapdata>\cdh\bfi directory if it does not already exist

		if (ACE_OS::access(aesApDataDir.c_str(), 0) != -1)
		{
		if ((ACE_OS::mkdir(aesApDataDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + aesApDataDir);
            return false;
        }
    }

    aesApDataDir += "/" + destSetName;

    
    // -------------------------------------------------------------------
    // Create <aesapdata>\cdh\bfi\<destsetname> directory if it
    // does not already exist
	if (ACE_OS::access(aesApDataDir.c_str(), 0) != -1)
    {
        if ((ACE_OS::mkdir(aesApDataDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + aesApDataDir);
            return false;
        }
    }
    
    string aesApDataBackupFile = aesApDataDir + "/files";

    aesApDataDir += "/streamid";

    
    // -------------------------------------------------------------------
    // Create <aesapdata>\bfi\cdh\<destsetname>\streamid directory
    // if it does not already exist
  	if (ACE_OS::access(aesApDataDir.c_str(), 0) != -1)
    {
        if ((ACE_OS::mkdir(aesApDataDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + aesApDataDir);
            return false;
        }
    }

    
    // ***
    // Done creating <aesapdata>... directory structure
    // ***
    
    
	//TODO
    string vdDestDir = SSHFTPRV2BackupPtr->getVDPath() + "/-bfi";

    // -------------------------------------------------------------------
    // Create <virtualdir>\-bfi directory if
    // it does not alredy exist
 	if (ACE_OS::access(vdDestDir.c_str(), 0) != -1)
    {
       if ((ACE_OS::mkdir(vdDestDir.c_str(), NULL) == -1) &&
    	  (ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + vdDestDir);
            return false;
        }
        else
        {
            if (! ServR::clearAcl(vdDestDir))
                return false;
        }
    }

    vdDestDir += "/" + destSetName;

    // -------------------------------------------------------------------
    // Create <virtualdir>\-bfi\<destsetname> directory if
    // it does not alredy exist
	if (ACE_OS::access(vdDestDir.c_str(), 0) != -1)
    {
       if ((ACE_OS::mkdir(vdDestDir.c_str(), NULL) == -1) &&
			(ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + vdDestDir);
            return false;
        }
    }

    vdDestDir += "/files";
    path = vdDestDir;

    // -------------------------------------------------------------------
    // Create <virtualdir>\-bfi\<destsetname>\files directory if
    // it does not alredy exist
    
	if (ACE_OS::access(vdDestDir.c_str(), 0) != -1)
    {
       if ((ACE_OS::mkdir(vdDestDir.c_str(), NULL) == -1) &&
		  (ACE_OS::last_error() != EEXIST))
        {
            // Eventlog, directory is missing and could not be created !!
            Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
                ServR::NTErrorText(), "Failed to create directory: " + vdDestDir);
            return false;
        }
    }

    
    // ***
    // Done creating <virtualdir>... directory structure
    // ***
    
    
    aesApDataDir += "/" + sourceId;

	hFile = ACE_OS::open(aesApDataDir.c_str(),                // file
						O_RDWR,        // access mode
                       ACE_DEFAULT_OPEN_PERMS,
                       NULL);

    
    if (hFile == ACE_INVALID_HANDLE)
    {
        GCC_TINFO((AES_CDH_BackupWriter, "(%t) BackupWriter::open()"
            "Failed to open %S , error code: %d\n", aesApDataDir.c_str(), ACE_OS::last_error()));
        
        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            ServR::NTErrorText(), "Failed to open file: " + aesApDataDir);
        return false;
    }


    mapAddr = ACE_OS::mmap(0,2 * sizeof(ACE_UINT32),PROT_RDWR,MAP_SHARED,hFile,0);


    if (mapAddr == NULL)
    {
        ACE_OS::close(hFile);
        ACE_OS::close(hMapFile);
        
        Event::report(AES_CDH_intProgFault, "INTERNAL PROGRAM ERROR",
            ServR::NTErrorText(), "-");
        return false;
    }

    ACE_TCHAR *cp = (ACE_TCHAR *)mapAddr;
    ACE_OS::memcpy((void *)&fileEntry, cp, sizeof(FileEntry));
    GCC_DEBUG(("(%t) BackupWriter::open() CDRBid = %d , fileSize = %d\n", fileEntry.CDRBid, fileEntry.fileSize));


    vdDestDir += "/CURRENT";
    
    // -------------------------------------------------------------------
    // Open the <virtualdir>\-bfi\<destsetname>\files<sourceid> file.
   /* hTarget = ::CreateFile(vdDestDir.c_str(),
                           GENERIC_READ | GENERIC_WRITE,
                           0,
                           NULL,
                           OPEN_ALWAYS,
                           FILE_ATTRIBUTE_NORMAL,
                           NULL);*/

	hTarget = ACE_OS::open(vdDestDir.c_str(),                // file
						O_RDWR,        // access mode
                       ACE_DEFAULT_OPEN_PERMS,
                       NULL);

    if (hTarget == ACE_INVALID_HANDLE)
    {
        ACE_OS::close(hFile);
        ACE_OS::close(hMapFile);
		ACE_OS::munmap(mapAddr,0);
        mapAddr = 0;
        
        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            ServR::NTErrorText(), "Failed to open file: " + vdDestDir);
        return false;
    }

	this->fileEntry.fileSize = ACE_OS::filesize(hTarget);    //::GetFileSize(hTarget, NULL);
    ACE_OS::memcpy(cp, (void *)&fileEntry, sizeof(FileEntry));

    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::open() - leaving\n"
        "Size: %S = %u\n"
        "Returning true\n", vdDestDir.c_str(), this->fileEntry.fileSize));

    return true;
}
/*===================================================================
   ROUTINE:close
=================================================================== */
//##ModelId=3DF499570049
bool BackupWriter::close()
{
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::close()\n"));

    if (mapAddr != 0)
    {
        GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::close()"
            "mapAddr != 0, closing files\n"));

        ACE_OS::close(hFile);
        ACE_OS::close(hTarget);

		ACE_OS::munmap(mapAddr,0);
        mapAddr = 0;
    }
    else
    {
        GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::close()\n"
            "mapAddr == 0\n"));
    }

    return true;
}

/*===================================================================
   ROUTINE:write
=================================================================== */
bool BackupWriter::write(const AES_DBO_DataBlock *block)
{

    GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::write() theSourceId = %s\n",theSourceId.c_str()));
    GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::write() SwriteFile  = %s\n",SwriteFile.c_str()));

    ACE_INT64 bWritten = 0;
    //GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::write() theSourceId = %s\n",theSourceId));
    //GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::write() SwriteFile  = %s\n",SwriteFile));
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::write() theSourceId = %s\n",theSourceId.c_str()));
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::write() SwriteFile  = %s\n",SwriteFile.c_str()));
	bWritten = ACE_OS::write(hTarget, block->getData(), block->length_,0);
	if(bWritten!= -1)
    {
        GCC_TINFO((AES_CDH_BackupWriter, "(%t) BackupWriter::write(), error writing applblock %u to file with CDRBid %u\n", block->blockNr_, fileEntry.CDRBid));

        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            "Error writing block to file", "-");
        return false;
    }
    else
    {
        if (bWritten == block->length_)
        {
            GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::write(), applblock %u written to file with CDRBid %u\n", block->blockNr_, fileEntry.CDRBid));

            if (fileEntry.CDRBid == 4294967295)
            {
                fileEntry.CDRBid = 0;
            }
            else
            {
                fileEntry.CDRBid++;
            }

            fileEntry.fileSize += block->length_;
            
            ACE_TCHAR *cp = (ACE_TCHAR *)mapAddr;
			ACE_OS::memcpy(cp, (void *)&fileEntry, sizeof(FileEntry));
        }
        else
        {
            GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::write(), %u written to file, should be %u\n", bWritten, block->blockNr_));
            return false;
        }
    }

    return true;
}

/*===================================================================
   ROUTINE:lastTransaction
=================================================================== */
AES_CDH_ResultCode BackupWriter::lastTransaction(ACE_UINT32 &blockNr)
{
    fileEntry.CDRBid == 0 ? blockNr = 4294967295 : blockNr = fileEntry.CDRBid - 1;

    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::lastTransaction()\n"
        "blocknumber == %u\n", blockNr));
    
    return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:getFileSize
=================================================================== */
//##ModelId=3E5495B1015C
ACE_UINT64 BackupWriter::getFileSize()
{
    ACE_UINT64 fileSize = this->fileEntry.fileSize;

    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFileSize()\n"
        "filesize == %u\n", fileSize));

    return fileSize;
}

/*===================================================================
   ROUTINE:getFilePath
=================================================================== */
//##ModelId=3E3155160120

// Modified for SFTP responding IP. Added a bool argument to differentiate trasfertype. 
const string BackupWriter::getFilePath(std::vector<std::string> &vec,string transferType)
{
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"));

    this->close();
	
	struct timeb  currTime;
    ACE_UINT16 millitm1;

    string sourcePath = path;
    sourcePath += "/CURRENT";
    
    ostringstream targetPath;

    targetPath << path;
    targetPath << "/";
	targetPath << ::time(NULL);
	targetPath << "_";
	ftime(&currTime);
	millitm1 = currTime.millitm;
	targetPath << millitm1;

    string sTargetPath = targetPath.str();
    
	if (ACE_OS::rename(sourcePath.c_str(), sTargetPath.c_str()) == -1)

    {
        // Error moving file
        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            ServR::NTErrorText(), "Failed to rename file:" + sourcePath);

        GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"
            "Failed to rename file\n", sourcePath.c_str()));
    }
    else
    {
        GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"
            "File switched\n"));
    }

    // HD80596 start
    this->fetchFileNames(vec);
    // HD80596 end
	//Changes for SFTP responding IP 
	if(transferType.compare("ftprv2")==0)
    this->open(FTPRV2BackupPtr->getDestSetName(), theSourceId, FTPRV2BackupPtr);
	else if(transferType.compare("sshftprv2")==0)
	this->open(SSHFTPRV2BackupPtr->getDestSetName(), theSourceId, SSHFTPRV2BackupPtr);
	//Ends 

    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"
        "Returning path: %S\n", sTargetPath.c_str()));

    // return sTargetPath;
    return path;
}

/*===================================================================
   ROUTINE:fetchFileNames
=================================================================== */
//##ModelId=3E7DCC5B00B4
bool BackupWriter::fetchFileNames(std::vector<std::string>& vec)
{
   	string searchPattern = path ;

	DIR* pDir = opendir ( (ACE_TCHAR*)searchPattern.c_str ());
	if ( !pDir) return false;
	struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir);
    while(pEntry!=NULL)
    {
		if ( (ACE_OS::strcmp(pEntry->d_name, ".") != 0) &&
		   (ACE_OS::strcmp(pEntry->d_name,"..") != 0) )
        {
			if (ACE_OS::strcmp(pEntry->d_name, "CURRENT") != 0)
                vec.push_back(pEntry->d_name);
        }

		pEntry = ACE_OS::readdir(pDir);
    }
    if(pDir !=NULL)
	ACE_OS::closedir(pDir);
    return true;
}


/*===================================================================
   ROUTINE:getFilePath
=================================================================== */
//##ModelId=3E3155160120
const string BackupWriter::getFilePath()
{
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"));

    this->close();
    string sourcePath = path;
    sourcePath += "/CURRENT";
    
    ostringstream targetPath;

    targetPath << path;
    targetPath << "/";
    targetPath << ::time(NULL);

    string sTargetPath = targetPath.str();
    
    if (ACE_OS::rename(sourcePath.c_str(), sTargetPath.c_str()) == -1)
    {
        // Error moving file
        Event::report(AES_CDH_physFileFault, "PHYSICAL FILE PROBLEM",
            ServR::NTErrorText(), "Failed to rename file:" + sourcePath);

        GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"));
    }
    else
    {
        GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"
            "File switched\n"));
    }
    this->open(FTPRV2BackupPtr->getDestSetName(), theSourceId, FTPRV2BackupPtr);

    GCC_TDEBUG((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"
        "Returning path: %s\n", sTargetPath.c_str()));
    GCC_TTRACE((AES_CDH_BackupWriter, "(%t) BackupWriter::getFilePath()\n"));

    return sTargetPath;
}

