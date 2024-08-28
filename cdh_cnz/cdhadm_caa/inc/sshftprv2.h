//******************************************************************************
// 
// .NAME
//      SSHFTPRV2 - SFTP Responding Version 2
// .LIBRARY 3C++
// .PAGENAME FTPRV2
// .HEADER  AES
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE ftprv2.h

// .COPYRIGHT
//  

// .DESCRIPTION 
//      This class supports sending files with responding SFTP protocol GOH V2.
//      

// .ERROR HANDLING
//
//      General rule:
//      The error handling is specified for each method.
//
//      No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	

// AUTHOR 
//		2007-12-14 by XCSAPVA/Aparna Vadala	for SFTP Responding IP

// .SEE ALSO 
//      <Related classes, (abstract) baseclasses, friend classes, ...>
//  FTPV2Agent

//******************************************************************************



#ifndef SSHFTPRV2_H 
#define SSHFTPRV2_H

#include <ftpv2agent.h>
#include <ftprv2keep.h>
#include <aes_cdh_resultcode.h>
#include <aes_cdh_destinationset.h>
#include <database.h>
#include <destinationalarm.h>
#include <transdest.h>
#include <string>
#include <vector>
#include <time.h>
#include <ace/ACE.h>
#include <aes_gcc_errorcodes_r1.h>


class SSHFTPRV2 : public FTPV2Agent
{
public:

	std::string getVDPath();

	SSHFTPRV2();

	virtual ~SSHFTPRV2();

	std::string getVDDestPath();

   	TransferAgent::TransferAgentAttributes  getAgentType();

   
   AES_CDH_ResultCode define(ACE_INT32& argc,
                             ACE_TCHAR* argv[],
                             const bool define,
                             const bool recovery); 

   
   AES_CDH_ResultCode getAttributes(string& transferType, vector<string>& attr);   

   AES_CDH_ResultCode getSingleAttr(const ACE_TCHAR* opt, void* value);

   
   AES_CDH_ResultCode sendFile(const string& fileName,
                               const string& remoteSubDirName = "",
                               const string& newFileName = "",
                               AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY);
   
   
   AES_CDH_ResultCode sendDirectory(const string& dirName,
                                    const string& remoteSubDirName = "",
                                    const string& newDirName = "",
                                    AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY,
                                    const string& fileMask = "");

  
   AES_CDH_ResultCode remove(void);

   
   AES_CDH_ResultCode checkConnection(void);

   
   AES_CDH_ResultCode changeAttr(TransferAgent *agent);

	//Check if the provided alias name for a virtual 
   //directory exists and in that case it find out the path 
   //where the files are to be stored.
  
   AES_CDH_ResultCode readVDPath(std::string& path);


   ACE_INT32 copyFile(string sourceFile , string destinationFile);
  
protected:
    
    AES_CDH_ResultCode sendFileResponding(const string& fileName,
                                          const string& remoteSubDirName,
                                          const string& newFileName,
                                          const bool moveFile = false);

	
    AES_CDH_ResultCode setSSHFTPPermissions(void);

	/*=================================================================== */
	/**
	  @brief   aNewMainFileName
	 */
	/*=================================================================== */
	string aNewMainFileName;
	/*=================================================================== */
	/**
		@brief   aNewSubFileName
	**/
	/*=================================================================== */
	string aNewSubFileName;

	/*=================================================================== */
	/**
		@brief   aFileName
	**/
	/*=================================================================== */
	string aFileName;
	/*=================================================================== */


private:

   
   AES_CDH_ResultCode parseSSHFTPR(ACE_INT32& argc,
                                ACE_TCHAR* argv[],
                                const bool define,
                                const bool recovery);

  
   ACE_UINT64 getFileSize(const string& path,
                             const string& fName,
                             ACE_INT32& errors);

   
   bool sendToDataBase(const std::string& fileName);

   
   bool createDataBaseEntry();

  
   bool isLocalFile(const string& fileName) const;

   
   void cleanUpSSHFtpDeleteDir(void);

   
   void doDelete(string path);

   AES_CDH_ResultCode IsDirectoryLocked(const string& path);

   
   AES_CDH_ResultCode createSSHFtpDestDir(void);

   
   AES_CDH_ResultCode sendDirectoryResponding(const string& dirName,
                                              const string& remoteSubDirName,
                                              const string& newDirName);

  
   AES_CDH_ResultCode doSendDirResponding(string path, string outPath);
   
   
   AES_CDH_ResultCode rCopyFile(const string inPath,
                                const string outPath,
                                const string fName,
                                const bool noOwrWrite);

   
   AES_CDH_ResultCode rMoveFile(const string& inPath,
                                const string& outPath,
                                const string& remoteFileName);

   
   void rCreateDirectory(string name);

  
   AES_CDH_ResultCode changeParameters();
   void fetchAttrValues(SSHFTPRV2 * psshftprv2, vector<string>& attr);

private:
	   ACE_Recursive_Thread_Mutex m_ftpAgentMx;
	   string vdDestPath;

	   string vdPath;

	   string eventType;

	   string respondTime;

	   string clusterIPAddress;

	   string notificationAddress;

	   string notificationPortNr;

	   string suppressDirCreate;

	   string removeTQFilePrefix;

	   bool isFileNotification;
  
	   bool badSSHFTPPermissions;

protected:

	FTPRV2Keep* keep;

private:
	   ACE_INT64 keepTime;
   
	   DataBase *db;

};

#endif
