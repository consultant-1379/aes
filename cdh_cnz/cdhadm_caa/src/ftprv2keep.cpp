/*=================================================================== */
/**
@file ftprv2keep.cpp

Class method implementation for ftprv2keep.h

DESCRIPTION
The services provided by FTPRV2SubKeep facilitates FTP transfer mode for responding type..

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
#include <stdio.h>
#include <event.h>
#include <servr.h>
#include <aes_gcc_log.h>
#include <ftprv2keep.h>
#include <string>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/* PHA tables are not used in linux
#define CDH_PHA_PRODUCT                "AES/CXC1371173"
#define CDH_PHA_KEEPTIMESLICE          "AES_CDH_KeepTimeSlice"*/
#define DEFAULT_KEEPTIMESLICE_VALUE    1

#define DELETE_DIR                     0
#define KEEP_DIR                       1
#define SUBKEEP_DIR                    2
#define DATA_DIR                       3
#define MAX_PATH          			   260
using namespace std;

/*===================================================================
                        CONST DECLARATION SECTION
=================================================================== */
// Global variable
static const ACE_TCHAR* deleteDirName = "Delete";
static const ACE_TCHAR* keepDirName   = "Keep";

/*===================================================================
   ROUTINE:FTPRV2Keep
=================================================================== */
FTPRV2Keep::FTPRV2Keep() : keepTime(0),
                           keepTimeSlice(0)
{
   // Read keep time slice parameter from the PHA database
   if (!readKeepTimeSliceParameter())
   {
      // Using default value
      keepTimeSlice = DEFAULT_KEEPTIMESLICE_VALUE;
   }
}

/*===================================================================
   ROUTINE:~FTPRV2SubKeep
=================================================================== */
FTPRV2Keep::~FTPRV2Keep()
{
   subKeepItemList.clear();
}
/*===================================================================
   ROUTINE:changeParameters
=================================================================== */
// Change the values of the KEEP function parameters
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::changeParameters(FTPRV2Keep* keep)
{
   // Update the keepTime parameter
   setKeepTime(keep->getKeepTime());

   return AES_CDH_RC_OK;
}

/*===================================================================
   ROUTINE:checkConnection
=================================================================== */
// Check if any sub keep directories shall be removed and if a new sub keep
// directory shall be created. Also check if any new files/directories has
// been placed in the FTP DELETE directory. If that is the case, then the new
// file/directory shall be moved to the current sub keep directory.
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::checkConnection()
{
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

   if (keepTime > 0)
   {
      if ((returnCode = createKeepDir()) == AES_CDH_RC_OK)
      {
         // Check and move any files under DELETE directory to current sub keep
         moveFilesToSubKeep();

         // Delete old sub keep directories
         deleteOldSubKeep();
      }
   }
   else
   {
      // Delete the contents of the KEEP directory, but do not delete the
      // KEEP directory itself
      cleanUpBaseDir(keepDir.c_str(), KEEP_DIR, false);

      // Delete all files in the DELETE directory
      string deleteDir = rootDir + "/" + deleteDirName;
      returnCode = cleanUpBaseDir(deleteDir.c_str(), DELETE_DIR, false);
   }

   return returnCode;
}

/*===================================================================
   ROUTINE:cleanUpBaseDir
=================================================================== */
// Delete all sub keep directories and (optional) the KEEP base directory
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::cleanUpBaseDir(const ACE_TCHAR* pDir,
                                              const ACE_INT32   nBaseDir,
                                              const bool  bDeleteBaseDir)
{
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

   ACE_TCHAR szSearch[MAX_PATH];
   ACE_OS::sprintf(szSearch, "%s", pDir);

   ACE_TCHAR szFullFileName[512];
   bool bSuccess=true; //, bContinue = TRUE;
   DIR* pDir1 = opendir ( (ACE_TCHAR*)szSearch);
   if ( pDir1 == NULL)
   {
        returnCode = AES_CDH_RC_ERROR;
		return returnCode;
   }

   struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir1);
   while (pEntry != NULL)
   {
      // Compose full filename with path
      ACE_OS::sprintf(szFullFileName, "%s/%s", pDir, pEntry->d_name);

      // A directory found, delete the contents
      string fileN = string(pDir) + "/" + string(pEntry->d_name);
      struct stat buf;
      stat(fileN.c_str(), &buf);
      if (S_ISDIR(buf.st_mode))
      {
         if (ACE_OS::strcmp(pEntry->d_name, ".") != 0 &&
            ACE_OS::strcmp(pEntry->d_name, "..") != 0)
         {
            // Delete sub dir
            if (deleteDir(szFullFileName) != AES_CDH_RC_OK)
            	returnCode = AES_CDH_RC_DATAAREAERR;
         }
      }
      else
      {
         // A file found
         if (ACE_OS::unlink(szFullFileName)==-1)
         {
            // The file might have read only attributes
            if (ACE_OS::last_error() == EACCES)
            {
#if 0
               if (::SetFileAttributes(szFullFileName, FILE_ATTRIBUTE_NORMAL))//??
                  bSuccess = ::DeleteFile(szFullFileName);
               else
                  bSuccess = false;
#endif
            }
            else
               bSuccess = false;

            // Failed to delete file, report error and return
            if (!bSuccess)
            {
               ACE_TCHAR szText[512];
               ACE_OS::sprintf(szText, "Failed to remove output file: %s", szFullFileName);

               Event::report(AES_CDH_fileRemoveFault,
                             "FILE REMOVE ERROR",
                             ServR::NTErrorText(),
                             szText);

               returnCode = AES_CDH_RC_DATAAREAERR;
            }
         }
      }

     pEntry = ACE_OS::readdir(pDir1);
   }
   if(pDir1 != NULL)
   	closedir(pDir1);

   // Remove the directory itself
   if (returnCode == AES_CDH_RC_OK && bDeleteBaseDir)
   {
      if (ACE_OS::rmdir(pDir)== -1)
      {
         int dwLastError = ACE_OS::last_error();
         if (dwLastError != ENOENT)
        //&&dwLastError != ERROR_PATH_NOT_FOUND)
         {
            ACE_TCHAR szText[512];

            if (nBaseDir == KEEP_DIR || nBaseDir == DELETE_DIR)
            {
               // The KEEP or DELETE base directory could not be removed
               ACE_OS::sprintf(szText, "Directory could not be removed: %s", pDir);
               Event::report(AES_CDH_dataAreaFault,
                             "DATA AREA ERROR",
                             ServR::NTErrorText(),
                             szText);
            }
            else
            {
               // A sub KEEP directory or a data directory in some of the sub
               // KEEP directories could not be removed
               ACE_OS::sprintf(szText, "Failed to remove output directory: %s", pDir);
               Event::report(AES_CDH_fileRemoveFault,
                             "FILE REMOVE ERROR",
                             ServR::NTErrorText(),
                             szText);
            }

            returnCode = AES_CDH_RC_DATAAREAERR;
         }
      }
   }

   return returnCode;
}

/*===================================================================
   ROUTINE:deleteDir
=================================================================== */
// Delete a directory and all it's contents
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::deleteDir(const ACE_TCHAR* pDir)
{
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

   ACE_TCHAR szSearch[MAX_PATH];
   ACE_OS::sprintf(szSearch, "%s", pDir);

   ACE_TCHAR szFullFileName[512];
   bool bContinue = true;
   DIR* pDir1 = opendir ( (ACE_TCHAR*)szSearch);
   if ( !pDir1)
   {
        returnCode = AES_CDH_RC_ERROR;
        return returnCode;
   }

   struct ACE_DIRENT * pEntry = ACE_OS::readdir(pDir1);
   while((pEntry!=NULL) && bContinue)
   {
      // Compose full filename with path

      ACE_OS::sprintf(szFullFileName, "%s/%s", pDir, pEntry->d_name);
      string fileN = string(pDir) + "/"+ string(pEntry->d_name);
      struct stat buf;
      stat(fileN.c_str(), &buf);
      if (S_ISDIR(buf.st_mode))

      // A directory found, delete the contents
      {
         if (ACE_OS::strcmp(pEntry->d_name, ".") != 0 &&
            ACE_OS::strcmp(pEntry->d_name, "..") != 0)
         {
            // Make a recursive call to ourself with the sub directory
            if ((returnCode = deleteDir(szFullFileName)) != AES_CDH_RC_OK)
               bContinue = false;
         }
      }
      else
      {
         // A file found
         if (ACE_OS::unlink(szFullFileName)==-1)
         {
            // The file might have read only attributes
            if (ACE_OS::last_error() == EACCES)
            {
#if 0
            	//should not set attributes in linux
               if (::SetFileAttributes(szFullFileName, FILE_ATTRIBUTE_NORMAL))
               {
                  if (!::DeleteFile(szFullFileName))
                     bContinue = false;
               }
               else
#endif
                  bContinue = false;
            }
            else
               bContinue = false;

            // Failed to delete file, report error and return
            if (!bContinue)
            {
               ACE_TCHAR szText[512];
               ACE_OS::sprintf(szText, "Failed to remove output file: %s", szFullFileName);

               Event::report(AES_CDH_fileRemoveFault,
                             "FILE REMOVE ERROR",
                             ServR::NTErrorText(),
                             szText);

               returnCode = AES_CDH_RC_DATAAREAERR;
            }
         }
      }

      if (bContinue)
    	  pEntry = ACE_OS::readdir(pDir1);
   }

   if(pDir1 != NULL)
   	closedir(pDir1);


   // Remove the directory itself
   if (returnCode == AES_CDH_RC_OK)
   {
      if (ACE_OS::rmdir(pDir)==-1)
      {
         int dwLastError = ACE_OS::last_error();
         if (dwLastError != ENOENT)
        	 //&& dwLastError != ERROR_PATH_NOT_FOUND)
         {
            ACE_TCHAR szText[512];

            // A sub KEEP directory or a data directory in some of the sub
            // KEEP directories could not be removed
            ACE_OS::sprintf(szText, "Failed to remove output directory: %s", pDir);
            Event::report(AES_CDH_fileRemoveFault,
                          "FILE REMOVE ERROR",
                          ServR::NTErrorText(),
                          szText);

            returnCode = AES_CDH_RC_DATAAREAERR;
         }
      }
   }

   return returnCode;
}

/*===================================================================
   ROUTINE:createKeepDir
=================================================================== */
// Create KEEP base directory
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::createKeepDir(const bool bReadKeepDirContents)
{
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

   if (ACE_OS::mkdir(keepDir.c_str(), NULL)==-1)
   {
      if (ACE_OS::last_error() == EEXIST)
      {
         if (bReadKeepDirContents)
         {
            // Read the sub keep directories to a list (if any exist)
            readKeepDirContents();
         }
      }
      else
      {
         ACE_TCHAR szText[256];
         ACE_OS::sprintf(szText,
                 "FTP Keep Directory \"%s\" could not be created",
                 keepDir.c_str());

         Event::report(AES_CDH_dataAreaFault,
                       "DATA AREA ERROR",
                       ServR::NTErrorText(),
                       szText);
         
         returnCode = AES_CDH_RC_DATAAREAERR;
      }
   }

   return returnCode;
}

/*===================================================================
   ROUTINE:createNewSubKeep
=================================================================== */
// Create a new sub keep item and creates a new sub keep directory with a
// timestamp in UTC rounded to the current hour
//============================================================================
#if 0
FTPRV2SubKeep* FTPRV2Keep::createNewSubKeep()
{
#if 0
	//to be ported after light AES
   ACE_UINT64 nSeconds = (keepTimeSlice*60*60);
   time_t nTimeStamp = (time(NULL) / nSeconds) * nSeconds;

   // Define new sub keep directory in the form "Keep.timestamp"
   ACE_TCHAR szSubKeepDir[64];
   ACE_OS::sprintf(szSubKeepDir, "Keep.%lu", nTimeStamp);

   ACE_TCHAR szFullSubKeepName[MAX_PATH];
   ACE_OS::sprintf(szFullSubKeepName, "%s/%s", keepDir.c_str(), szSubKeepDir);

   BY_HANDLE_FILE_INFORMATION srctFileInfo;
   if (!ServR::checkFile(szFullSubKeepName, true, &srctFileInfo))
   {
      if (createSubKeepDir(szFullSubKeepName) != AES_CDH_RC_OK)
         return NULL;

      // Get directory creation time from the file system
      if (!ServR::checkFile(szFullSubKeepName, true, &srctFileInfo))
         return NULL;
   }

   LARGE_INTEGER nFileTime;
   nFileTime.HighPart = srctFileInfo.ftCreationTime.dwHighDateTime;
   nFileTime.LowPart = srctFileInfo.ftCreationTime.dwLowDateTime;

   subKeepItemList.push_back(FTPRV2SubKeep(nFileTime.QuadPart, szSubKeepDir));

   return &(*subKeepItemList.rbegin());
#endif
   return NULL;
}
#endif

/*===================================================================
   ROUTINE:createSubKeepDir
=================================================================== */
// Create a new sub keep directory
//============================================================================
#if 0
AES_CDH_ResultCode FTPRV2Keep::createSubKeepDir(const ACE_TCHAR* subKeepDir)
{
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

   if (ACE_OS::mkdir(subKeepDir, NULL)==-1)
   {
      if (ACE_OS::last_error() != EEXIST)
      {
         ACE_TCHAR szText[256];
         ACE_OS::sprintf(szText,
                 "FTP Keep Directory \"%s\" could not be created",
                 subKeepDir);

            Event::report(AES_CDH_dataAreaFault,
                          "DATA AREA ERROR",
                          ServR::NTErrorText(),
                          szText);
         
         returnCode = AES_CDH_RC_DATAAREAERR;
      }
   }

   return returnCode;
}
#endif

/*===================================================================
   ROUTINE:define
=================================================================== */
// Create KEEP base directory and a new sub keep directory
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::define(const ACE_TCHAR* pRootDir,
                                      const ACE_UINT64 nKeepTime,
                                      const bool recovery)
{
	(void)recovery;
   // Path to the root directory
   rootDir = pRootDir;

   // Path to the "... \Keep" directory
   keepDir = rootDir + "/" + keepDirName;

   // Keep time parameter
   keepTime = nKeepTime;

   // Create KEEP base directory (if it does not already exist) but do not
   // read directory contents if the directory already exist (will be done
   // from checkConnection and changeParameters)
   AES_CDH_ResultCode returnCode;
   if ( (returnCode = createKeepDir(false)) != AES_CDH_RC_OK)
      return returnCode;

   return returnCode;
}

/*===================================================================
   ROUTINE:deleteOldSubKeep
=================================================================== */
// Delete sub keep directories older that keepTimeSlice
//============================================================================
void FTPRV2Keep::deleteOldSubKeep()
{
#if 0
	//to be ported after light AES
   // If list is empty (no sub keep exist) it is no use to continue
   if (subKeepItemList.empty())
      return;

   FTPRV2SubKeep* pSubKeep = getLatestCreatedSubKeep();
   if (!pSubKeep)
      return;

   list<FTPRV2SubKeep>::iterator iter = subKeepItemList.begin();
   ACE_TCHAR szFullFileName[MAX_PATH];

   // Calculate the time point at which we will delete an sub keep. The delete
   // time is UTC time rounded to whole hour (1300, 1400, ... etc)
   ACE_UINT64 dwDeleteTimeHour = ((getCurrentHour() + 1) - keepTimeSlice - keepTime);
   bool bContinue = true;

   // Check if any sub keep exists that is created during last time slice
   while (iter != subKeepItemList.end() && bContinue)
   {
      // Convert the sub keep filetime to hour and compare
      if (getFileCreateHour((*iter).createTime) < dwDeleteTimeHour)
      {
         ACE_OS::sprintf(szFullFileName,
                 "%s/%s",
                 keepDir.c_str(),
                 (*iter).subKeepName.c_str());

         // Delete the contents in the sub keep directory recursive and
         // finally the directory itself
         if (deleteDir(szFullFileName) == AES_CDH_RC_OK)
        	 iter = subKeepItemList.erase(iter);
         else
        	 iter++;
      }
      else
         bContinue = false;
   }
#endif
}
/*===================================================================
   ROUTINE:getAttributes
=================================================================== */
// Returns the attribute(s) of the KEEP parameters
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::getAttributes(std::vector<std::string>& attr)
{
   ACE_TCHAR szTemp[16];
   ACE_OS::sprintf(szTemp, "%lu", keepTime);
   string strKeepTime = szTemp;

   // Get the attributes for the KEEP parameters
   attr.push_back((std::string)"-b");
   attr.push_back(strKeepTime);

   return AES_CDH_RC_OK;
}
/*===================================================================
   ROUTINE:getSingleAttr
=================================================================== */
// getSingleAttr
// Returns the value of a specific attribute
AES_CDH_ResultCode FTPRV2Keep::getSingleAttr(const ACE_TCHAR* opt, void* value)
{
   AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

   switch (*(opt+1))
   {
   case 'b':
      // Copy the keeptime value
      if (value)
         *((ACE_UINT64*)value) = keepTime;
      break;

   default:
      returnCode = AES_CDH_RC_INCUSAGE;
   }

   return returnCode;
}

/*===================================================================
   ROUTINE:getCurrentHour
 ===================================================================*/
// Returns the number of hours elapsed since 1 Jan 1601
#if 0
ACE_UINT64 FTPRV2Keep::getCurrentHour()
{
#if 0
	// to be ported after light AES
	// Get current time i UTC
	//SYSTEMTIME srctSysTime;
	//  GetSystemTime(&srctSysTime);
	// Current system date time
	time_t rawtime(NULL);

	time(&rawtime);

	struct tm* srctSysTime = gmtime(&rawtime);


	FILETIME srctFileTime;
	SystemTimeToFileTime(&srctSysTime, &srctFileTime);

	LARGE_INTEGER nCurrentTime;
	nCurrentTime.HighPart = srctFileTime.dwHighDateTime;
	nCurrentTime.LowPart = srctFileTime.dwLowDateTime;

	// Convert to number of hours elapsed since 1 Jan 1601
	LONGLONG nSec = (LONGLONG)(nCurrentTime.QuadPart / (LONGLONG)10000000);

	return (ACE_UINT64)(LONGLONG)(nSec/(LONGLONG)(60*60));
#endif
   return 1000;
}
#endif

/*===================================================================
   ROUTINE:getCurrentSubKeep
=================================================================== */
// Get the current sub keep item. If item list is empty a new sub keep item
// and directory is created. If sub keep directories exists, the creation time
// is checked against the current time - time slice (in hours) to descide it a
// new sub keep shall be created.
//============================================================================
#if 0
FTPRV2SubKeep* FTPRV2Keep::getCurrentSubKeep()
{
#if 0
   if (subKeepItemList.empty())
      readKeepDirContents();

   // Get the sub keep with the latest (newest) created file creation time
   // from the sub keep list
   FTPRV2SubKeep* pSubKeep = getLatestCreatedSubKeep();

   if (pSubKeep)
   {
      ACE_UINT64 dwFileCreateHour = getFileCreateHour(pSubKeep->createTime);
      ACE_UINT64 dwCurrentHour = getCurrentHour();

      // Check if the current sub keep shall be closed and a new created
      if (( (dwFileCreateHour - 1) + keepTimeSlice) >= dwCurrentHour)
         return pSubKeep; // use existing
   }

   return createNewSubKeep();
#endif
   return NULL;
}
#endif

/*===================================================================
   ROUTINE:getFileCreateHour
=================================================================== */
// Returns the number of hours elapsed from 1 Jan 1601 to file creation time
//============================================================================
#if 0
ACE_UINT64 FTPRV2Keep::getFileCreateHour(ACE_INT64 lpFileTime)  //LPFILETIME lpFileTime)
{
	(void)lpFileTime;

   LARGE_INTEGER nFileTime;
   nFileTime.HighPart = lpFileTime->dwHighDateTime;
   nFileTime.LowPart = lpFileTime->dwLowDateTime;

   // Convert to number of hours elapsed from 1 Jan 1601 to file creation time
   LONGLONG nSec = (LONGLONG)(nFileTime.QuadPart / (LONGLONG)10000000);

   return (ACE_UINT64)(LONGLONG)(nSec/(LONGLONG)(60*60));

}
#endif

/*===================================================================
   ROUTINE:getFileCreateHour
=================================================================== */
// Returns the number of minutes elapsed from 1 Jan 1601 to file creation time
//============================================================================
#if 0
ACE_UINT64 FTPRV2Keep::getFileCreateHour(ACE_INT64 nFileTime)//LONGLONG nFileTime)
{
	(void)nFileTime;
#if 0
   // Convert to number of hours elapsed from 1 Jan 1601 to file creation time
   LONGLONG nSec = (LONGLONG)(nFileTime / (LONGLONG)10000000);

   return (ACE_UINT64)(LONGLONG)(nSec/(LONGLONG)(60*60));
#endif
   return 1000;
}
#endif
/*===================================================================
   ROUTINE:FTPRV2Keep
=================================================================== */
// Returns the value of keepTime parameter
//============================================================================
inline /*const*/ ACE_UINT64 FTPRV2Keep::getKeepTime() const
{
   return keepTime;
}

/*===================================================================
   ROUTINE:getLatestCreatedSubKeep
=================================================================== */
// Returns a reference to the current (newest) creted sub keep list item, i.e
// the last element in the list
//============================================================================
#if 0
FTPRV2SubKeep* FTPRV2Keep::getLatestCreatedSubKeep()
{
   FTPRV2SubKeep* pSubKeep = NULL;

   // If list is empty, no use to continue
   if (!subKeepItemList.empty())
      pSubKeep = &(*subKeepItemList.rbegin());

   return pSubKeep;
}
#endif
/*===================================================================
   ROUTINE:moveFilesToSubKeep
=================================================================== */
// Move a file or directory from DELETE directory to the current sub keep
// directory. When a file/directory os moved to the current sub keep, a time
// stamp in UTC is added last to the filename
//============================================================================
void FTPRV2Keep::moveFilesToSubKeep()
{
#if 0
	//to be ported after light AES
	ACE_TCHAR szSearch[MAX_PATH];
	ACE_OS::sprintf(szSearch, "%s/%s", rootDir.c_str(), deleteDirName);

	ACE_TCHAR szSourceFileName[512];
	ACE_TCHAR szDestFileName[512];
	//   HANDLE hFile = ::FindFirstFile(szSearch, &srctFileData);
	bool bContinue = true;
	FTPRV2SubKeep* pSubKeep = NULL;

	while (bContinue && hFile != INVALID_HANDLE_VALUE)
   	{
      if (strcmp(srctFileData.cFileName, ".") != 0 &&
         strcmp(srctFileData.cFileName, "..") != 0)
      {
         // Get reference to the current sub keep
         if (!pSubKeep)
            pSubKeep = getCurrentSubKeep();

         if (pSubKeep)
         {
            // Compose full source filename with path
            ACE_OS::sprintf(szSourceFileName,
                    "%s/%s/%s",
                    rootDir.c_str(),
                    deleteDirName,
                    srctFileData.cFileName);

            // Compose full destination filename with path
            ACE_INT32 nLen = ACE_OS::sprintf(szDestFileName,
                               "%s/%s/%s.%lu",
                               keepDir.c_str(),
                               pSubKeep->subKeepName.c_str(),
                               srctFileData.cFileName,
                               time(NULL));

            // Move file to sub keep directory
            if (!::MoveFile(szSourceFileName, szDestFileName))
            {
               ACE_UINT64 dwLastError = ACE_OS::last_error();
               if (dwLastError != ERROR_FILE_NOT_FOUND && 
                  dwLastError != ERROR_PATH_NOT_FOUND)
               {
                  ACE_TCHAR szText[512];
                  ACE_OS::sprintf(szText, "Failed to move output file: %s", szSourceFileName);

                  Event::report(AES_CDH_fileMoveFault,
                                "FILE MOVE ERROR",
                                ServR::NTErrorText(),
                                szText);
               }
               else if (nLen >= MAX_PATH)
               {
                  ACE_TCHAR szText[512];
                  ACE_OS::sprintf(szText, "Failed to move output file: %s", szSourceFileName);

                  Event::report(AES_CDH_fileMoveFault,
                                "FILE MOVE ERROR",
                                "File- or directoryname too long",
                                szText);
               }
            }
         }
         else
            bContinue = false;
      }

      if (bContinue)
         bContinue = ::FindNextFile(hFile, &srctFileData);
   }

   if (hFile != INVALID_HANDLE_VALUE)
      ::FindClose(hFile);
#endif
}

/*===================================================================
   ROUTINE:readKeepTimeSliceParameter
=================================================================== */
// Fetch the parameter AES_CDH_KeepTimeSlice from the PHA database.
// The parameter definies the amout of hours a sub keep directory is open.
//============================================================================
bool FTPRV2Keep::readKeepTimeSliceParameter()
{
#if 0 // PHA tables are removed

	ACS_PHA_ParamReturnType ReturnType = ACS_PHA_PARAM_RC_OK;
	ACS_PHA_Tables Table(CDH_PHA_PRODUCT);

	ACS_PHA_Parameter<unsigned long> Param(CDH_PHA_KEEPTIMESLICE);
	if ((ReturnType = Param.get(Table)) != ACS_PHA_PARAM_RC_OK)
	{
      ACE_TCHAR szText[256];
      ACE_OS::sprintf(szText,
              "Failed to read the PHA parameter \"%s\"\r\nUsing default value: %d",
              CDH_PHA_KEEPTIMESLICE,
              DEFAULT_KEEPTIMESLICE_VALUE);

      // PHA error. send event
      Event::report(AES_CDH_parameterFault,
                    "PARAMETER HANDLING PROBLEM",
                    szText,
                    "-");

      return false;
	}

	keepTimeSlice = Param.data();
	if (keepTimeSlice == 0)
	{
      ACE_TCHAR szText[256];
      ACE_OS::sprintf(szText,
              "Failed to read the PHA parameter \"%s\"\r\nUsing default value: %d",
              CDH_PHA_KEEPTIMESLICE,
              DEFAULT_KEEPTIMESLICE_VALUE);

      // PHA error. send event
      Event::report(AES_CDH_parameterFault,
                    "PARAMETER HANDLING PROBLEM",
                    szText,
                    "-");

      return false;
	}
#endif

   return true;
}

/*===================================================================
   ROUTINE:readKeepDirContents
=================================================================== */
// Read the contents of the KEEP base directory and create a sub keep item
// for each sub keep directory and put the items in a list
//============================================================================
bool FTPRV2Keep::readKeepDirContents()
{
#if 0
   //to be ported after light AES
   ACE_TCHAR szSearch[MAX_PATH];
   ACE_OS::sprintf(szSearch, "%s", keepDir.c_str());

   // If the list is not empty we already have read the directory contents
   if (!subKeepItemList.empty())
      return true;

   WIN32_FIND_DATA srctFileData;
   HANDLE hFile = ::FindFirstFile(szSearch, &srctFileData);
   bool bContinue = true;
   LARGE_INTEGER nTime;

   while (bContinue && hFile != INVALID_HANDLE_VALUE)
   {
      if (srctFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
      {
         if (strcmp(srctFileData.cFileName, ".") != 0 &&
            strcmp(srctFileData.cFileName, "..") != 0)
         {
            // Convert from FILETIME to LARGE_INTEGER
            nTime.LowPart = srctFileData.ftCreationTime.dwLowDateTime;
            nTime.HighPart = srctFileData.ftCreationTime.dwHighDateTime;

            // Create a new FTPRV2Keep object and put it in the list
            subKeepItemList.push_back(FTPRV2SubKeep(nTime.QuadPart,
                                                    srctFileData.cFileName));
         }
      }

      bContinue = ::FindNextFile(hFile, &srctFileData);
   }

   if (hFile != INVALID_HANDLE_VALUE)
      ::FindClose(hFile);

   // Sorting the list after file creation time
   subKeepItemList.sort();
#endif
   return true;
}

/*===================================================================
   ROUTINE:remove
=================================================================== */
// Delete all sub keep directories and finally the KEEP base directory
//============================================================================
AES_CDH_ResultCode FTPRV2Keep::remove()
{
   AES_CDH_ResultCode returnCode = cleanUpBaseDir(keepDir.c_str(), KEEP_DIR);

   // Empties the sub keep item list
   subKeepItemList.clear();

   return returnCode;
}

/*===================================================================
   ROUTINE:setKeepTime
=================================================================== */
// Sets a new value of the keepTime parameter
//============================================================================
void FTPRV2Keep::setKeepTime(const ACE_UINT64 newKeepTime)
{
   if (newKeepTime != keepTime)
   {
      keepTime = newKeepTime;

      // If keepTime is set to 0 (zero), we clean up in the KEEP directory
      if (keepTime == 0)
      {
         // Delete the contents of the KEEP directory, but do not delete the
         // KEEP directory itself
         cleanUpBaseDir(keepDir.c_str(), KEEP_DIR, false);

         string deleteDir = rootDir + "/" + deleteDirName;
         cleanUpBaseDir(deleteDir.c_str(), DELETE_DIR, false);

         // Empties the sub keep item list
         subKeepItemList.clear();
      }
      else
      {
         if (createKeepDir() == AES_CDH_RC_OK)
         {
            // Move files from DELETE directory to current sub keep directory
            moveFilesToSubKeep();

            // Delete old sub keep directories
            deleteOldSubKeep();
         }
      }
   }
}
