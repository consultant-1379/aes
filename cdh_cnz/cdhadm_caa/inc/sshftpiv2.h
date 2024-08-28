//******************************************************************************
// 
// .NAME
//      SSHFTPIV2 - SSH dependent parts of initiating FTP
// .LIBRARY 3C++
// .PAGENAME SSHFTPIV2
// .HEADER  AES
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE sshftpiv2.h

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
//  Handles FTP parts specific to SSH.

// .ERROR HANDLING
//
//  General rule:
//  The error handling is specified for each method.
//
//  No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0507

// AUTHOR 
//  2002-08-19 by UAB/KB/AU Gunnar Andersson

// .LINKAGE
//  -

// .SEE ALSO 
//  FTPIV2

//******************************************************************************

#ifndef SSHFTPIV2_H 
#define SSHFTPIV2_H

//SFTP API replacement
#include <aes_cdh_sftp.h>
#include <ftpiv2.h>
#include <aes_cdh_destinationset.h>
#include <aes_cdh_resultcode.h>
#include <string>
#include <list>
#include <ace/ACE.h>

#define SFTPAPI_API_SUCCESS 1

class  SSHFTPIV2 : public FTPIV2
{
public:
   SSHFTPIV2();

	// EGERPET: CR SFTP
	 void stopTransfer();	 

   virtual ~SSHFTPIV2();

private:
   static ACE_Thread_Mutex sftpApiMutex;

   SftpAdapterClass *sftpApiObj;

   bool sftpApiInUse;

   void ftpConnect(const string& hostAddress,
                   const string& portNr, 
                   const string& userName, 
                   const string& passWord,
			       bool ascii = false) throw (AES_CDH_ResultCode); // HM66365

   void ftpDisconnect(void) throw(); 

   void ftpDisconnect(bool closeconnection) throw(); //HY73707

   AES_CDH_ResultCode verifyConnection();

   void ftpCreateDirectory(const std::string& name ) throw (AES_CDH_ResultCode);

   bool ftpDeleteFile(const string& fileName,
                      string& errorText) throw();

   AES_CDH_ResultCode ftpSendFile(const string& fileName,
                                     const string& remoteFileName,
                                     AES_CDH_DestinationSet::transferMode trMode)
                                                       throw (AES_CDH_ResultCode);

   bool renameRemoteFile(const string& currentName, string& newName,
                         string &errorText) throw();

   bool renameRemoteDir(const string& currentName, string& newName,
                        string& errorText) throw();

   bool isRemoteFile(const string& fileName) const throw();

   bool isRemoteDir(const string& dirName) const throw();


   bool checkRemoteFile(std::list<string>& fileList,
                       const string& fileName) throw();

   string addNameToPath(const string& path, const string& name);

   string sftpErrorText(void) const;

   AES_CDH_ResultCode sendFileInitiating(const string& fileName,
		                                    const string& remoteSubDirName,
		                                    const string& newFileName,
		                                    AES_CDH_DestinationSet::transferMode trMode);

   AES_CDH_ResultCode sendDirectoryInitiating(const string& dirName,
		                                         const string& remoteSubDirName,
		                                         const string& newDirName,
		                                         AES_CDH_DestinationSet::transferMode trMode,
		                                         const string& fileMask);

   AES_CDH_ResultCode doSendDirInitiating(string path,
		                                     AES_CDH_DestinationSet::transferMode trMode,
                                           const string fileMask);
};

#endif
