/*=================================================================== */
/**
	@file   ftpiv2.h

	@brief
	This class handles sending files with ftp initiating.
	The source code is adapted for WinNT only.

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
#ifndef FTPIV2_H
#define FTPIV2_H
/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
//#include <ftpv2agent.h>
#include <aes_cdh_resultcode.h>
#include <aes_cdh_destinationset.h>

#include <ace/ACE.h>
#include <ace/Event.h>


#include <string>
#include <set>
#include <list>

using namespace std;
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief AES_FTP_FILE_NAME_SIZE

          -
 */
/*=================================================================== */
#define AES_FTP_FILE_NAME_SIZE 512

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     FTPIV2

                 This class supports sending files with initiating FTP protocol GOH V2.
 */
/*=================================================================== */
//##ModelId=3DE49B46018B
class  FTPIV2 //: public FTPV2Agent
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:

	/*=====================================================================
	     	                        CLASS CONSTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

	   	      @brief          FTPIV2
	   						  FTPIV2 constructor.

	   		  @return          void

	   	      @exception       none
	 */
	/*=================================================================== */

	FTPIV2()
	: userName("ftpuser01"),
	  passWord("ftpuser01"),
	  remoteDir("MYTEST/"),
	  hostAddress("10.35.15.85")
	{

	};
	/*=====================================================================
	 	     	                        CLASS DESTRUCTOR
	 ==================================================================== */
	/*=================================================================== */
	/**

	 	   	      @brief          ~FTPIV2
	 	   						  FTPIV2 destructor.

	 	   		  @return          void

	 	   	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4601C8
	virtual ~FTPIV2() {};
	/*===================================================================
	 	                      ROUTINE DECLARATION SECTION
	 =================================================================== */
	/*=================================================================== */
	/**

	 		    	      @brief          getVDDestPath
	 		    						  To get virtual Distination path

	 		    	      @return          string    virtual destination path.

	 		    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3E3E6F4801B6
	std::string getVDDestPath();
	/*=================================================================== */
	/**

	 		    	      @brief          sendFile
	 		    						  Send file to server

	 		    		  @param          fileName
	 		      						  file name
	 		      		  @param          remoteSubDirName
	               						  remote Sub Directory Name
	             		  @param          newFileName
	               						  file name
	               		  @param          trMode
	               						  transfer mode

	 		    	      @return          AES_CDH_ResultCode	status of send file

	 		    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B460205
	AES_CDH_ResultCode sendFile(const string& fileName,
			const string& remoteSubDirName = "",
			const string& newFileName = "",
			AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY);
	/*=================================================================== */
	/**

	 		    	      @brief          sendDirectory
	 		    						  To sen ddirectory

	 		    		  @param          dirName
	 		      						  directory name
	 		      		  @param          newDirName
	               						  new Directory Name
						  @param          trMode
	               						  transfer mode
	             		  @param          trMode
	               						  transfer mode
	               		  @param          fileMask
	               						  file mask

	 		    	      @return          AES_CDH_ResultCode	status of send directory
	 		    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B460219
	AES_CDH_ResultCode sendDirectory(const string& dirName,
			const string& remoteSubDirName = "",
			const string& newDirName = "",
			AES_CDH_DestinationSet::transferMode trMode = AES_CDH_DestinationSet::TR_BINARY,
			const string& fileMask = "");
	/*=================================================================== */
	/**

	 		    	      @brief          define
	 		    						  -

	 		    		  @param          argc
	 		      						  -
	 		      		  @param          argv
	               						  -
	             		  @param          define
	               						  -
	               		  @param          recovery
	               						  -

	 		    	      @return          AES_CDH_ResultCode    -

	 		    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4601DC
	AES_CDH_ResultCode define(ACE_INT32& argc,
			ACE_TCHAR* argv[],
			const bool define,
			const bool recovery);
	/*=================================================================== */
	/**

	 		    	      @brief          remove
	 		    						  -

	 		    	      @return          AES_CDH_ResultCode    -

	 		    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4601E7
	AES_CDH_ResultCode remove(void);
	/*=================================================================== */
	/**

	 		    	      @brief          getAttributes
	 		    						  To get the attributes

	 		    		  @param          transferType
	 		      						  transfer type
	 		      		  @param          attr
	               						  attributes

	 		    	      @return          AES_CDH_ResultCode    indicates the status of get attributes

	 		    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4601FA
	AES_CDH_ResultCode getAttributes(string& transferType, vector<string>& attr);
	/*=================================================================== */
	/**

			    	      @brief          getSingleAttr
			    						  To get the single attribute

			    		  @param          opt
										  option
			      		  @param          value
										  value

			    	      @return          AES_CDH_ResultCode    indicates the status of single attributes

			    	      @exception       none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode getSingleAttr(const ACE_TCHAR* opt, void* value);

	/*=================================================================== */
	/**

				    	      @brief          checkConnection
				    						  To check the Connection

				    	      @return          AES_CDH_ResultCode    indicates the status of connection

				    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B460237
	AES_CDH_ResultCode checkConnection(void);
	/*=================================================================== */
	/**

				    	      @brief          changeAttr
				    						  to change the Attributes

				    		  @param          agent
											  TransferAgent

				    	      @return         AES_CDH_ResultCode    indicates the status of change attributes

				    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B460241
	//AES_CDH_ResultCode changeAttr(TransferAgent *agent);

	/*=================================================================== */
	/**

					    	      @brief          readVDPath
					    						  To read virtual directory

												  -
					      		  @param          path
												  path

					    	      @return         AES_CDH_ResultCode    indicates the status of read virtual directory

					    	      @exception       none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode readVDPath(std::string& path);


	static ACE_Recursive_Thread_Mutex connectMutex;
	/*=================================================================== */
	/**
			      @brief   aNewMainFileName
						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B46025F
	string aNewMainFileName;
	/*=================================================================== */
	/**
			      @brief   aNewSubFileName
						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B460291
	string aNewSubFileName;
	/*=================================================================== */
	/**
			      @brief   aFileName
						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4602C3
	string aFileName;
	/*=====================================================================
	                        PROTECTED DECLARATION SECTION
	==================================================================== */
protected:
	/*====================================================================
	                       PROTECTED ATTRIBUTES
	==================================================================== */

	/*=================================================================== */
	/**
		      @brief   userName
					   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B470039
	string userName;
	/*=================================================================== */
	/**
			      @brief   passWord
						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B470075
	string passWord;
	/*=================================================================== */
	/**
			      @brief   portNr
						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4700A8
	string portNr;
	/*=================================================================== */
	/**
			      @brief   remoteDir
						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4700E4
	string remoteDir;   // value of remoteFolderPath attribute
	/*=================================================================== */
	/**
			      @brief   hostAddress
						   -
	 */
	/*=================================================================== */
	//##ModelId=3DE49B470116
	string hostAddress;
	/*=================================================================== */

	/**

					    	      @brief          maskInFileName
					    						  For masking

					    		  @param          pattern
												  pattern for masking
					      		  @param          fileName
												  fileName

					    	      @return         true     indicates masking is successful
											      false    indicates masking is unsuccessful

					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B47016F
	bool maskInFileName(string pattern, string fileName) ;

	/*=================================================================== */
	/**

					    	      @brief          ftpConnect
					    						  To do ftp Connection

					    		  @param          hostAddress
												  hostAddress
					      		  @param          portNr
												  port
								  @param          userName
												  userName
					      		  @param          passWord
												  passWord
								  @param          ascii
												  ascii

					    	      @return          void

					    	      @exception       throw (AES_CDH_ResultCode)
	 */
	/*=================================================================== */
	//##ModelId=3DE49B4602EA
	virtual void ftpConnect(const string& hostAddress,
			const string& portNr,
			const string& userName,
			const string& passWord,
			bool ascii = false) throw (AES_CDH_ResultCode) = 0; // HM66365
	/*=================================================================== */
	/**

					    	      @brief          ftpDisconnect
					    						  For ftp Disconnection

					    	      @return          void

					    	      @exception       throw()
	 */
	/*=================================================================== */
	//##ModelId=3DE49B46031C
	virtual void ftpDisconnect(void) = 0;
	/*=================================================================== */
	/**

					    	      @brief          ftpCreateDirectory
					    						  To create FTP directory

					    		  @param          name
												  -
					      		  @param          silent
												  -

					    	      @return          void

					    	      @exception       throw (AES_CDH_ResultCode)
	 */

	virtual AES_CDH_ResultCode ftpVerifyConnection() = 0;

	/*=================================================================== */
	//##ModelId=3DE49B460330
	virtual void ftpCreateDirectory(const std::string& remoteFolderName) throw (AES_CDH_ResultCode) = 0;
	/*=================================================================== */
	/**

					    	      @brief          ftpChangeDirectoryUp
					    						  -

					    	      @return         true		on success
												  false     on unsuccessful

					    	      @exception       throw()
	 */

	/*=================================================================== */
	//##ModelId=3DE49B460376
	virtual AES_CDH_ResultCode ftpSendFile(const string& fileName, 	const string& remoteFileName) = 0;
	/*=================================================================== */
	/**

					    	      @brief          renameRemoteFile
					    						  -

					    		  @param          currentName
												  -
					      		  @param          newName
												  -
								  @param          errorText
												  -

					    	      @return         true		on success
												  false     on unsuccessful

					    	      @exception      throw()
	 */

	//##ModelId=3DE49B4603C6
	virtual bool renameRemoteDir(const std::string& currentName, const std::string& newName) = 0;
	/*=================================================================== */
	/**

					    	      @brief          isRemoteFile
					    						  -

					    		  @param          fileName
												  -

					    	      @return         true 		if the file is remote
												  false 	if the file is not remote

					    	      @exception      none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B470006
	virtual bool isRemoteFile(const string& fileName) const = 0;

	//*=================================================================== */
	/**

			    	      @brief          getSingleAttr
			    						  -

			    		  @param          dirName
										  -

			    	      @return         true		on success
										  false     on unsuccessful

					      @exception      throw()
	 */
	/*=================================================================== */
	//##ModelId=3DE49B47001A
	virtual bool isRemoteDir(const std::string& remotePath, const std::string& dirName) const = 0;
	/*=================================================================== */
	/**

					    	      @brief          getRemoteFileList
					    						  -

					    		  @param          fileList
												  -
					      		  @param          fileName
												  -

					    	      @return         true		on success
												  false     on unsuccessful

								  @exception      throw()
	 */

	/*=================================================================== */
	virtual  AES_CDH_ResultCode sendFileInitiating(const string& fileName,
			const string& remoteSubDirName,
			const string& newFileName,
			AES_CDH_DestinationSet::transferMode trMode) = 0;
	/*=================================================================== */
	/**

					    	      @brief          sendDirectoryInitiating
					    						  -

					    		  @param          dirName
												  -
					    		  @param          remoteSubDirName
												  -
					      		  @param          newDirName
												  -
								  @param          trMode
												  -
					      		  @param          fileMask
												  -

					    	      @return          AES_CDH_ResultCode    indicates the status of sendDirectoryInitiating

					    	      @exception       none
	 */
	/*=================================================================== */
	virtual AES_CDH_ResultCode sendDirectoryInitiating(const string& dirName,
														const string& remoteSubDirName,
														const string& newDirName,
														AES_CDH_DestinationSet::transferMode trMode,
														const string& fileMask) = 0;
	/*=================================================================== */


	/*=====================================================================
	                         PRIVATE DECLARATION SECTION
	 ==================================================================== */
private:
	/*====================================================================
		                       PRIVATE ATTRIBUTES
	==================================================================== */
	//ACE_Recursive_Thread_Mutex m_ftpAgentMx;
	/*=================================================================== */
	/**
			      @brief   fileRetryDelay
						   -
	 */
	/*=================================================================== */
	string fileRetryDelay;
	/*=================================================================== */
	/**
			      @brief   fileSendRetries
						   -
	 */
	/*=================================================================== */
	string fileSendRetries;
	/*=================================================================== */
	/**
			      @brief   suppressDirCreate
						   -
	 */
	/*=================================================================== */
	// Suppress creation of directory on remote system with TQ name
	string suppressDirCreate;
	/*===================================================================
    	                               ROUTINE DECLARATION SECTION
    =================================================================== */
	/*=================================================================== */
	/**

					    	      @brief          parseFTPI
					    						  -

					    		  @param          argc
												  -
					      		  @param          argv[]
												  -

					    	      @return          AES_CDH_ResultCode    indicates the status of parseFTPI

					    	      @exception       none
	 */
	/*=================================================================== */
	//##ModelId=3DE49B470147
	AES_CDH_ResultCode parseFTPI(ACE_INT32& argc, ACE_TCHAR* argv[]);
	/*=================================================================== */
	/**

					    	      @brief          changeParameters
					    						  -

					    	      @return          AES_CDH_ResultCode    indicates the status of changeParameters

					    	      @exception       none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode changeParameters();
	void fetchAttrValues(FTPIV2 * pftpiv2, vector<string>& attr);

};

#endif
