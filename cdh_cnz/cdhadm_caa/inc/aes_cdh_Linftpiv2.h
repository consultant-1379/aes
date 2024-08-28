//******************************************************************************
//
// .NAME
//      aes_cdh_Linftpiv2 - Windows API dependent parts of initiating FTP
// .LIBRARY 3C++
// .PAGENAME aes_cdh_Linftpiv2
// .HEADER  AES
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE aes_cdh_LinFTPIV2.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
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
//      Handles FTP parts specific to the Windows FTP API.

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

#ifndef aes_cdh_Linftpiv2_H
#define aes_cdh_Linftpiv2_H

#include <ftpiv2.h>
#include <aes_cdh_destinationset.h>
#include <aes_cdh_resultcode.h>
#include <string>
#include <curl/curl.h>

#include <stdio.h>

class  LinFTPIV2 : public FTPIV2
{

 public:

  LinFTPIV2();
    
  virtual ~LinFTPIV2();

private:

   CURL* m_CurlSessionHandle;
   std::string m_FtpRootUrl;
   std::FILE* m_NullDeviceHandle;

   void ftpConnect(const std::string& hostAddress,
                   const std::string& portNr,
                   const std::string& userName,
                   const std::string& passWord,
			       bool ascii = false) throw (AES_CDH_ResultCode);

   void ftpDisconnect(void) throw();

   AES_CDH_ResultCode verifyConnection();

   void ftpCreateDirectory(const std::string& remoteFolderName) throw (AES_CDH_ResultCode);
  
   AES_CDH_ResultCode ftpSendFile(const std::string& localFile, const std::string& remoteFile );

   AES_CDH_ResultCode ftpSendFileWithRename(const std::string& localFileName, const std::string& remoteFilePath, const std::string& remoteFileName, bool deleteOldRemoteFile = false );

   bool ftpRenameObject(const std::string& currentName, const std::string& newName);

   bool renameRemoteDir(const std::string& remotePath, const std::string& newName);

   bool isRemoteFile(const string& fileName) const;

   bool isRemoteDir(const std::string& remotePath, const std::string& dirName) const;

   AES_CDH_ResultCode sendFileInitiating(const std::string& fileName,
										 const std::string& remoteSubDirName,
										 const std::string& newFileName,
										 AES_CDH_DestinationSet::transferMode trMode);

   AES_CDH_ResultCode sendDirectoryInitiating(const string& dirName,
											  const string& remoteSubDirName,
											  const string& newDirName,
											  AES_CDH_DestinationSet::transferMode trMode,
											  const string& fileMask);


   AES_CDH_ResultCode doSendDirInitiating(const std::string& localFolderPath, const std::string& remoteFolderPath);

   bool assembleRemoteFileName(const std::string& localFileName, std::string& remoteFileName);

   void assembleRemoteFolderName(const std::string& localFolderName, std::string& remoteFolderName);

   bool remoteFileNameWithTimeStamp(const std::string& remoteFilePath, std::string& remoteFileName);

   bool remoteFolderNameWithTimeStamp(const std::string& remoteFolderPath, std::string& remoteFolderName);

   static int progressCb(void * ptr, double dwld, double nowdwnld, double totup, double nowupload);

};

#endif
