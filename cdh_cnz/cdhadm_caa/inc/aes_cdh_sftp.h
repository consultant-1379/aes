/*=================================================================== */
/**
	@file   aes_cdh_sftp.h

	@brief
	The services provided by SftpAdapterClass facilitates the SFTP transfer of data.

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
   N/A       29/11/2011     xbhadur       Initial Release
============================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
 ==================================================================== */
#ifndef AES_CDH_SFTP_H
#define AES_CDH_SFTP_H
/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <libssh2.h>
#include <libssh2_sftp.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <string>
#include <ace/ACE.h>
#include <arpa/inet.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/stat.h>
#include <aes_cdh_resultcode.h>
#include <ace/Thread_Mutex.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/* macros to check for specific file types, added in 1.2.5 */
 #define LIBSSH2_SFTP_S_ISLNK(m) \
(((m) & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFLNK)
#define LIBSSH2_SFTP_S_ISREG(m) \
(((m) & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFREG)
#define LIBSSH2_SFTP_S_ISDIR(m) \
(((m) & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFDIR)
#define LIBSSH2_SFTP_S_ISCHR(m) \
(((m) & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFCHR)
 #define LIBSSH2_SFTP_S_ISBLK(m) \
(((m) & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFBLK)
#define LIBSSH2_SFTP_S_ISFIFO(m) \
(((m) & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFIFO)
#define LIBSSH2_SFTP_S_ISSOCK(m) \
(((m) & LIBSSH2_SFTP_S_IFMT) == LIBSSH2_SFTP_S_IFSOCK)
/*=====================================================================*/
typedef enum
{
   SFTPAPI_ERROR	      		= 0, /* Error when executing */
		   SFTPAPI_OK                 = 1, /*No error  */
		   SFTPAPI_AUTH_FAILED           = 101, /*Either a wrong user name or a wrong password */
		   SFTPAPI_NOT_CONNECTED        = 102, /* This SFTP object has no active connection to a server  */
		   SFTPAPI_CONNECTED = 103,		/* This SFTP object has already an active connection to a server  */
		   SFTPAPI_CONNECTION_LOST = 104,  /*The connection to the server has been lost for some reason   */
		   SFTPAPI_FILE_ALREADY_EXISTS = 105,	/*The target file for rename already exists   */
		   SFTPAPI_INVALID_HANDLE = 106,	/*The supplied file or directory handle was not valid   */
		   SFTPAPI_DIRECTORY_ALREADY_EXISTS = 107,	/*The directory that is created or is target for a rename operation already exists   */
		   SFTPAPI_PERMISSION_DENIED = 108,	/*No permission to execute the operation   */
		   SFTPAPI_NO_SUCH_DIRECTORY = 109,	/*The directory does not exist   */
		   SFTPAPI_FAILURE = 110,	/*Sftp failure   */
		   SFTPAPI_NOT_YET_IMPLEMENTED = 111,	/*Not yet implemented   */
		   SFTPAPI_INVALID_PORT = 112,	/*The port was invalid   */
		   SFTPAPI_INVALID_HOST = 113,	/* The host was invalid  */
		   SFTPAPI_NO_SUCH_FILE = 114,	/* The file does not exist  */
		   SFTPAPI_NOT_VALID_NAME = 115,	/* The file/Dir name not valid  */
		  SFTPAPI_DIR_CREATE_FAIL = 116,	/* The Dir creation failed  */
		  SFTPAPI_MAX_RETRY_OVER = 117,	/* The Dir creation failed  */
		  SFTPAPI_BREAK_TRANSM = 118	/* The break transmission */
} SFTP_ResultCode;
/*=====================================================================*/
using namespace std;


/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */
/*===================================================================*/
/**
		@brief SftpAdapterClass
	The services provided by SftpAdapterClass facilities SFTP data transfer.
 */
/*=================================================================== */
class SftpAdapterClass
{
	/*=====================================================================
	  	  	            PRIVATE DECLARATION SECTION
	==================================================================== */
private:

	/*=================================================================== */
	/**
	    		      @brief   MAX_RETRY
	    		               MAX_RETRY to change the filename with timestamp
	 */
	/*=================================================================== */
	static int MAX_RETRY;
	/*=================================================================== */
	/**
		    		      @brief   session
		    		               estabilish a sessioin with SSH server
	 */
	/*=================================================================== */
	LIBSSH2_SESSION *session;
	/*=================================================================== */
	/**
		    		      @brief   sftp_session
		    		               sftp_session
	 */
	/*=================================================================== */
	LIBSSH2_SFTP *sftp_session;
	/*=================================================================== */
	/**
		    		      @brief   sock
		    		               used to do socket connection
	 */
	/*=================================================================== */
	int sock;
	/*=================================================================== */
	/**
		    		      @brief   strIpAddress
		    		               SFTP server IP address
	 */
	/*=================================================================== */
	string strIpAddress;
	/*=================================================================== */
	/**
		    		      @brief   nPort
		    		               SFTP server port number
	 */
	/*=================================================================== */
	int nPort;
	/*=================================================================== */
	/**
		    		      @brief   strUser
		    		               username
	 */
	/*=================================================================== */
	string strUser;
	/*=================================================================== */
	/**
		    		      @brief   strPassword
		    		               password
	 */
	/*=================================================================== */
	string strPassword;
	/*=================================================================== */
	/**
		    		      @brief   breakTransm
		    		              transmission break
	 */
	/*=================================================================== */
	bool breakTransm;
	/*=================================================================== */
	/**
		    		      @brief   parameterChanged
		    		               true if parameters changed otherwise false
	 */
	/*=================================================================== */
	bool parameterChanged ;
	/*=================================================================== */
	/**
		    		      @brief   sftpsession1
		    		               sftpsession1 is true if session is established
	 */
	/*=================================================================== */
	bool sftpsession1;
	 /*=================================================================== */
        /**
                                      @brief   sftpcheck
                                               sftpcheck is true if session is active after previous conncetion
         */
        /*=================================================================== */
	bool sftpcheck;    //HV45803
	static ACE_Thread_Mutex libssh2Mx;
#if 0
	/*=================================================================== */
	/**
		    		      @brief   status
		    		               True when the remote is connected using sshClient and the file transfer mode is ascii.
	 */
	/*=================================================================== */
	bool status;
	/*=================================================================== */
	/**
		    		      @brief   osTyp
		    		               True when the remote machine is other than windows.
	 */
	/*=================================================================== */
	bool osTyp ;
#endif

    /*=================================================================== */
    /**

    	      @brief		sftpInitiate
    						Initiate SFTP connection

    		  @param           	hostAddress
    							IP address.
    		  @param           	portNr
    							portNumber.
     		  @param           	userName
    							username.
     		  @param           	passWord
    							password.
     		  @param           	ascii
    							format of sending file.

    	      @return           AES_CDH_ResultCode    to indicate that connection is done or not

    	      @exception       none
     */
    /*=================================================================== */
	AES_CDH_ResultCode sftpInitiate(void *psshftpiv2, const char * hostAddress, int portNr, const char * userName, const char * passWord,bool ascii);
    /*=================================================================== */
    /**

    	      @brief		sendFile1
    						To transfer the file.

    		  @param           	stream1
    							Local file name.
    		  @param           	remoteFileName
    							remote file name.
    		  @param           	ascii
    							Format of file transfer.

    	      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

    	      @exception       none
     */
    /*=================================================================== */
	bool sendFile1(void *psshftpiv2, string stream1, string remoteFileName,bool ascii);
    /*=================================================================== */
    /**

    	      @brief		createDirectories
    						Creates the directory.

    		  @param           	path
    							Path to create the directory.

    	      @return           true    to indicate that DIR is created.
								false   to indicate that DIR is not created.

    	      @exception       none
     */
    /*=================================================================== */
	bool createDirectories(string path);
    /*=================================================================== */
    /**

    	      @brief		addTimestampToFilename
    						To add timestamp for filename.

    		  @param           	filename
    							filename to which time stamp needs to be added.

    	      @return           string    timestamped filenane

    	      @exception       none
     */
    /*=================================================================== */
	string addTimestampToFilename(string filename);
    /*=================================================================== */
    /**

    	      @brief		renameAndSendFileWithRetry
    						to rename and send the file

    		  @param           	localfilename
    							localfilename on local host.
     		  @param           	remotefilename
    							remotefilename on remote host.
    		  @param           	retriesNumber
    							number of retries for changing the filename.
     		  @param           	ascii
    							File format for transfer.

    	      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

    	      @exception       none
     */
    /*=================================================================== */
	bool renameAndSendFileWithRetry(void *psshftpiv2, string localfilename, string remotefilename, int retriesNumber, bool ascii);
    /*=================================================================== */
    /**

    	      @brief		init_attrs
    						To initate the local file attributes to rmote file attributes

    		  @param           	attrs
    							Local file attributes.
      		  @param           	st
    							Remote file attributes.

			  @return          void

    	      @exception       none
     */
    /*=================================================================== */
	void init_attrs(LIBSSH2_SFTP_ATTRIBUTES* attrs, struct stat* st);
    /*=================================================================== */
	SFTP_ResultCode SFTPRESULT;
	/*=====================================================================
	  	  	            PRIVATE DECLARATION SECTION
	==================================================================== */
public:

	/*===================================================================
	                         CLASS CONSTRUCTOR
	=================================================================== */
	/*=================================================================== */
	/**

	          @brief           SftpAdapterClass
							   constructor of SftpAdapterClass

	          @return          void

	          @exception       none
	*/
	/*=================================================================== */
		SftpAdapterClass();
	    /*=================================================================== */
	    /**

	    	      @brief		sftpFinalize
	    						sftpFinalize disconnects a ssh client from SSH server.

	    	      @return           true    if disconnection is ok.
									false   if disconnection is not ok.

	    	      @exception       none
	     */
	    /*=================================================================== */
		bool sftpFinalize();

	    /*=================================================================== */
	    /**

	    	      @brief		sftpConnect
	    						sftpConnect create a sftp client and connect it to SFTP server

	    		  @param       	hostAddress
								IP address.
				  @param        portNr
								portNumber.
				  @param        userName
								username.
				  @param        passWord
								password.
				  @param        ascii
								format of sending file.

	    	      @return       AES_CDH_ResultCode    to indicate connection is done or not.

	    	      @exception    none
	     */
	    /*=================================================================== */
		AES_CDH_ResultCode sftpConnect( void * psshftpiv2, const char * hostAddress, int portNr,  const char * userName, const char * passWord,bool ascii);
	    /*=================================================================== */
	    /**

	    	      @brief		sftpDisconnect
	    						sftpDisconnect disconnectls a sftp client from SFTP server

	    	      @return       true    to indicate that sftp is disconnected.
								false   to indicate that sftp is not disconnected.

	    	      @exception     none
	     */
	    /*=================================================================== */
		bool sftpDisconnect(bool closeconnection = false); //HY73707
	    /*=================================================================== */
	    /**

	    	      @brief		renameRemoteFile
	    						It renames a remote filename.

	    		  @param        currentName
	    						current remote filename with full path of the file.
				  @param        newName
	    						new filename to apply with full path of the file.

	    	      @return       true    to indicate remote file is renamed.
								false   to indicate that remote file is not renamed.

	    	      @exception    none
	     */
	    /*=================================================================== */
		bool renameRemoteFile(string currentName, string newName);
	    /*=================================================================== */
	    /**

	    	      @brief			renameRemoteDir
									It renames a remote folder.

	    		  @param          	currentName
	    							current remote folder name.
	    		  @param           	newName
	    							new folder name to apply.

	    	      @return           true    to indicate that folder is renamed.
									false   to indicate that folder is not renamed.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool renameRemoteDir(string currentName, string newName);
	    /*=================================================================== */
	    /**

	    	      @brief			sftpDeleteFile
									sftpDeleteFile allows to delete file

	    		  @param           	fileName
	    							Full path of the file needs to be given as input.

	    	      @return           true    to indicate that file is deleted.
									false   to indicate that file is not deleted.

	    	      @exception       none
	     */
	    /*=================================================================== */
		bool sftpDeleteFile(string fileName);
	    /*=================================================================== */
	    /**

	    	      @brief			isTransmissionInterrupted
									Returns true if the transmission has been interrupted by afptfte command

	    	      @return           true    to indicate that transmission is interrupted.
									false   to indicate that transmission is not interrupted.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool isTransmissionInterrupted(void *psshftpiv2);
	    /*=================================================================== */
	    /**

	    	      @brief			checkRemoteFile
									It checks if exists the filename on the remote server.

	    		  @param           	fileName
	    							filename to check.

	    	      @return           true    to indicate that file exists on remote.
									false   to indicate that file does not exist on remote.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool checkRemoteFile(string fileName);
	    /*=================================================================== */
	    /**

	    	      @brief		isRemoteFile
	    						It checks if the filename is a file on remote server

	    		  @param           	fileName
	    							full path with filename to check.

	    	      @return           true    to indicate that file exists on remote.
									false   to indicate that file does not exist on remote.

	    	      @exception       none
	     */
	    /*=================================================================== */
		bool isRemoteFile(string fileName);
	    /*=================================================================== */
	    /**

	    	      @brief			isRemoteDir
									It checks if the dirname is a directory on remote server.

	    		  @param           	dirName
	    							folder name to check with full path.

	    	      @return           true    to indicate that folder exists on remote.
									false   to indicate that folder does not exist on remote.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool isRemoteDir(string dirName);
	    /*=================================================================== */
	    /**

	    	      @brief			sftpSendFile
									Send the file to remote.

	    		  @param           	fileName
	    							Local filename with full path.
	    		  @param           	remoteFileName
	    							remote filename.
	    		  @param           	remoteDirName
	    							directory name on remopte system.
	    		  @param           	remoteSubdirName
	    							sub-directory name on remopte system.
				  @param           	overwrite
	    							boolean to notify if the file needs to be overwrite or not.
				  @param           	ascii
	    							Format of transfer.

	    	      @return           true    to indicate that file is transfered.
									false   to indicate that file is not transfered.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool sftpSendFile(void *psshftpiv2, string fileName, string remoteFileName, string remoteDirName, string remoteSubdirName, bool overwrite,bool ascii);
	    /*=================================================================== */
	    /**

	    	      @brief			sendFile
									send the file to remote server.

	    		  @param           	localFileName
	    							Full path of local file.
	    		  @param           	remoteFileName
	    							Full path of remote file.

	    	      @return           true    to indicate that file is transfered.
									false   to indicate that file is not transfered.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool sendFile(void *psshftpiv2, string localFileName, string remoteFileName,bool ascii);
	    /*=================================================================== */
	    /**

	    	      @brief			sftpCreateDirectories
									createDirectories creates a remote directory on sftp server.

	    		  @param           	path
	    							the path to create on the remote server.

	    	      @return           true    to indicate that dir is created.
									false   to indicate that dir is not created.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool sftpCreateDirectories(string path);
	    /*=================================================================== */
	    /**

	    	      @brief			recursiveDirCreation
									create the directories.

	    		  @param           	dirPath
	    							the path to create on the remote server.
	    		  @param           	newRemoteDirName
	    							remote directory name to be created.

	    	      @return           true    to indicate that dir is created.
									false   to indicate that dir is not created.

	    	      @exception       	none
	     */
	    /*=================================================================== */
		bool recursiveDirCreation(void *psshftpiv2, string dirPath,string newRemoteDirName);
	    /*=================================================================== */
	    /**

	    	      @brief		sftpSendDir
	    						sftpSendDir sends a directory to remoteServer.

	    		  @param           	dirname
	    							directory to send.
	    		  @param           	newRemoteDirName
	    							new dirname to be created on remote.
	    		  @param           	remoteDirName
	    							dirname used on remote server.
	    		  @param           	remoteSubdirName
	    							remote subdirectory used to store the directory.

	    	      @return           true    to indicate that everything is OK.
									false   to indicate that something went wrong.

	    	      @exception       none
	     */
	    /*=================================================================== */
		bool sftpSendDir(void *psshftpiv2, string dirname, string newRemoteDirName, string remoteDirName, string remoteSubdirName);
		/*===================================================================
		                         CLASS DESTRUCTOR
		=================================================================== */
		/*=================================================================== */
		/**

		          @brief           ~SftpAdapterClass
									Destructor of SftpAdapterClass

		          @return          void

		          @exception       none
		*/
		/*=================================================================== */
		~SftpAdapterClass();

		string sftpErrorText();
};
#endif
