/*=================================================================== */
/**
	@file   ftprv2.h

	@brief
	This class supports sending files with responding FTP protocol GOH V2.
	This class handles sending files with ftp responding

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------
   N/A       27/09/2011     xbhadur       Initial Release
============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef FTPRV2_H
#define FTPRV2_H
/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <ftpv2agent.h>
#include <ftprv2keep.h>
#include <aes_cdh_resultcode.h>
#include <aes_cdh_destinationset.h>
#include <database.h>
#include <string>
#include <vector>
#include <ace/ACE.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     FTPRV2

                 This class supports sending files with responding FTP protocol GOH V2.
*/
/*=================================================================== */
//##ModelId=3DE49B4401F7
class  FTPRV2 : public FTPV2Agent
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
   //##ModelId=3E6DF453026F
   std::string getVDPath();

   //##ModelId=3DE49B440234
   FTPRV2();

   //##ModelId=3DE49B44023D
   virtual ~FTPRV2();

   //##ModelId=3E3E75EE0217
   std::string getVDDestPath();

   //##ModelId=3DF9AF710319
   TransferAgent::TransferAgentAttributes  getAgentType();

   //##ModelId=3DE49B440247
   AES_CDH_ResultCode define(ACE_INT32& argc,
                             ACE_TCHAR* argv[],
                             const bool define,
                             const bool recovery); // 020115 qabhans

   //##ModelId=3DE49B44025B
   AES_CDH_ResultCode getAttributes(string& transferType, vector<string>& attr);    // 020115 qabhans

   AES_CDH_ResultCode getSingleAttr(const ACE_TCHAR* opt, void* value);

   //##ModelId=3DE49B44026F
   AES_CDH_ResultCode sendFile(const string& fileName,
                               const string& remoteSubDirName = "",
                               const string& newFileName = "",
                               AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY);
   
   //##ModelId=3DE49B440283
   AES_CDH_ResultCode sendDirectory(const string& dirName,
                                    const string& remoteSubDirName = "",
                                    const string& newDirName = "",
                                    AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY,
                                    const string& fileMask = "");

   //##ModelId=3DE49B440298
   AES_CDH_ResultCode remove(void);

   //##ModelId=3DE49B4402A2
   AES_CDH_ResultCode checkConnection(void);

   //##ModelId=3DE49B4402B5
   AES_CDH_ResultCode changeAttr(TransferAgent *agent);

	//Check if the provided alias name for a virtual 
   //directory exists and in that case it find out the path 
   //where the files are to be stored.
   //##ModelId=3E68A7A40126
   AES_CDH_ResultCode readVDPath(std::string& path);
   
   /*=====================================================================
                           PROTECTED DECLARATION SECTION
   ==================================================================== */
protected:
	string streamId_;

    //##ModelId=3DE49B4501B2
    AES_CDH_ResultCode sendFileResponding(const string& fileName,
                                          const string& remoteSubDirName,
                                          const string& newFileName,
                                          const bool moveFile = false);

	//##ModelId=3E7EBE8D030A
    AES_CDH_ResultCode setFTPPermissions(void);

    /*=====================================================================
                            PRIVATE DECLARATION SECTION
    ==================================================================== */
private:

   //##ModelId=3DE49B4500B8
   AES_CDH_ResultCode parseFTPR(ACE_INT32& argc,
                                ACE_TCHAR* argv[],
                                const bool define,
                                const bool recovery);

   //##ModelId=3DE49B4500EA
   ACE_UINT64 getFileSize(const string& path,
                             const string& fName,
                             ACE_INT32& errors);

   //##ModelId=3DE49B450126
   bool sendToDataBase(const std::string& fileName);

   //##ModelId=3DE49B45013A
   bool createDataBaseEntry();

   //##ModelId=3DE49B450144
   bool isLocalFile(const string& fileName) const;

   //##ModelId=3DE49B450162
   bool cleanUpFtpDeleteDir(void);

   //##ModelId=3DE49B450176
   bool doDelete(string path);

   AES_CDH_ResultCode IsDirectoryLocked(const string& path);

   //##ModelId=3DE49B450194
   AES_CDH_ResultCode createFtpDestDir(void);

   //##ModelId=3DE49B4501E4
   AES_CDH_ResultCode sendDirectoryResponding(const string& dirName,
                                              const string& remoteSubDirName,
                                              const string& newDirName);

   //##ModelId=3DE49B450220
   AES_CDH_ResultCode doSendDirResponding(string path, string outPath);
   
   //##ModelId=3DE49B450248
   AES_CDH_ResultCode rCopyFile(const string inPath,
                                const string outPath,
                                const string fName,
                                const bool noOwrWrite);

   //##ModelId=3E32904002CA
   AES_CDH_ResultCode rMoveFile(const string& inPath,
                                const string& outPath,
                                const string& remoteFileName);

   //##ModelId=3DE49B45028E
   void rCreateDirectory(string name);

   //##ModelId=3DE49B4502A2
   AES_CDH_ResultCode changeParameters();
   void fetchAttrValues(FTPRV2 * pftprv2, vector<string>& attr);
   /*=====================================================================
                           PUBLIC DECLARATION SECTION
   ==================================================================== */
private:

    //##ModelId=3DE49B4402FC
     string vdDestPath;


     //##ModelId=3DE49B4402CA
     string vdPath;

     //##ModelId=3DE49B450018
     string eventType;

    //##ModelId=3DE49B4403CE
    string respondTime;


    //##ModelId=3DE49B45007D
    string clusterIPAddress;


    //##ModelId=3DE49B440360
    string notificationAddress;

    //##ModelId=3DE49B440392
    string notificationPortNr;

    // Suppress creation of directory with destination name
   string suppressDirCreate;

   // Remove file prefix with TQ name
   string removeTQFilePrefix;

   //##ModelId=3DE49B440341
   bool isFileNotification;			// qabhefe 010214 File Notification start---

   //##ModelId=3E7EBE5300B8
   bool badFTPPermissions;
   ACE_Recursive_Thread_Mutex m_ftpAgentMx;
   
   /*=====================================================================
                           PUBLIC DECLARATION SECTION
   ==================================================================== */
protected:
   // Using KEEP functionality
   FTPRV2Keep* keep;

   /*=====================================================================
                           PUBLIC DECLARATION SECTION
   ==================================================================== */
private :
   ACE_INT64 keepTime;

   string fileMaxAge;

   DataBase *db;                     // Pointer to the database

};

#endif
