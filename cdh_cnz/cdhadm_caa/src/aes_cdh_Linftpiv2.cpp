/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_cdh_Linftpiv2.h>
#include <aes_cdh_common.h>
#include <event.h>
#include <servr.h>
#include <sstream>
#include <sys/types.h>
#include <dirent.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

/*===================================================================
                        DECLARATION SECTION
=================================================================== */
AES_CDH_TRACE_DEFINE(AES_CDH_LinFTPIV2)

// To avoid warning about unused parameter
#define UNUSED(expr) do { (void)(expr); } while (0)

#define CURLE_LOGIN_DENIED 67  // TR HS55206

namespace FTP_PARAMETERS{

	const std::string PROTOCOL("ftp://");

	// Default FTP connection port
	const std::string DEFAULT_PORT("21");

	// The PORT instruction tells the remote server to connect to our specified IP address.
	// The string may be a plain IP address, a host name, a network interface name (under Unix)
	// or just a '-' symbol to let the library use your system's default IP address
	const char FTP_PORT_OPTION[] = "-";

	// The maximum time in seconds that you allow the libcurl transfer operation to take.
	const long TIMEOUT = 5L;

	// The maximum time in seconds that you allow the connection phase
	// to the server to take. This only limits the connection phase, it has no impact once it has connected.
	const long CONNECTION_TIMEOUT = 10L;
};

namespace FTP_COMMAND {

	const std::string CREATE_FOLDER("MKD");
	const std::string CHANGE_FOLDER("CWD");
	const std::string RENAME_FROM("RNFR");
	const std::string RENAME_TO("RNTO");
	const std::string DELETE("DELE");
};

namespace {

	const char DirDelim = '/';
	const char Colon = ':';
	const char WhiteSpace = ' ';
	const long ON = 1L;
	const long OFF = 0L;
	const int MAX_RETRY = 100;

	const std::string TemporaryExtensionName(".tmp");

};

/*===================================================================
   ROUTINE:LinFTPIV2
=================================================================== */
LinFTPIV2::LinFTPIV2()
:m_CurlSessionHandle(NULL),
  m_FtpRootUrl()
{
	m_NullDeviceHandle = ACE_OS::fopen("/dev/null", "w");
}

/*===================================================================
   ROUTINE:~LinFTPIV2
=================================================================== */
LinFTPIV2::~LinFTPIV2()
{
	ftpDisconnect();
	if( m_NullDeviceHandle != NULL )
	{
		fclose(m_NullDeviceHandle);
	}
}

/*===================================================================
   ROUTINE: ftpConnect
=================================================================== */
void LinFTPIV2::ftpConnect(const string& hostAddress,
							const string& portNr,
							const string& userName,
							const string& passWord,
							bool ascii) throw (AES_CDH_ResultCode)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	UNUSED(ascii);

	if(NULL != m_CurlSessionHandle)	ftpDisconnect();

	// get a curl easy session handle
	m_CurlSessionHandle = curl_easy_init();

	if(NULL == m_CurlSessionHandle)
	{
		Event::report(AES_CDH_ftpConnectFault, "FTP CONNECT ERROR"," ",	"curl_easy_init() fails");
		AES_CDH_LOG(LOG_LEVEL_FATAL, "curl api error curl_easy_init() failed!");
		AES_CDH_TRACE_MESSAGE("curl api error curl_easy_init() failed!");
		throw AES_CDH_RC_CONNECTERR;
	}

	if( passWord.empty() && userName.empty() )
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "username or password are mandatory");
		AES_CDH_TRACE_MESSAGE("username or password are mandatory");
		Event::report(AES_CDH_ftpConnectFault, "FTP CONNECT ERROR",	"Incorrect usage of username and password", "username and pwd are mandatory");

		throw AES_CDH_RC_CONNECTERR;
	}

	// Set User credentials
	std::string userCredentials(userName);

	if( !passWord.empty() )
	{
		userCredentials.push_back(Colon);
		userCredentials += passWord.c_str();
	}
	// Set User credentials as: <userName:password>
	curl_easy_setopt(m_CurlSessionHandle, CURLOPT_USERPWD, userCredentials.c_str());

	//-------------------------------
	//Only use the Active mode
	//To disable Extended Active mode (EPRT): curl_easy_setopt(curl, CURLOPT_FTP_USE_EPRT, OFF);

	//Use PORT
	curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FTPPORT, FTP_PARAMETERS::FTP_PORT_OPTION);

	// Set a Connection Timeout
	// It should contain the maximum time in seconds that you allow the connection phase
	// to the server to take. This only limits the connection phase, it has no impact once it has connected.
	curl_easy_setopt(m_CurlSessionHandle, CURLOPT_CONNECTTIMEOUT, FTP_PARAMETERS::CONNECTION_TIMEOUT);

	// Set Signal OFF
	// libcurl will not use any functions that install signal handlers or any functions that cause signals
	// to be sent to the process. This option is here to allow multi-threaded unix applications to
	// still set/use all timeout options etc, without risking getting signals.
	curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOSIGNAL, ON);

	// No DATA transfer open only control connection
	curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOBODY, ON);

	// Switch on full protocol/debug output
	curl_easy_setopt(m_CurlSessionHandle, CURLOPT_VERBOSE, OFF);

	// Assemble the remote url
	m_FtpRootUrl.assign(FTP_PARAMETERS::PROTOCOL);

	if(AES_CDH_Common_Util::isIPv6Address(hostAddress))	// IPv6 address - enclose addr str within '[]'
		m_FtpRootUrl += "[" + hostAddress + "]";
	else												// IPv4 address
		m_FtpRootUrl += hostAddress;

	m_FtpRootUrl.push_back(Colon);
	m_FtpRootUrl += (portNr.empty() ? FTP_PARAMETERS::DEFAULT_PORT : portNr);

	// Set remote url
	curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, m_FtpRootUrl.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO, "LinFTPIV2::ftpConnect() - m_FtpRootUrl built for CURL calls is {%s}", m_FtpRootUrl.c_str());
	// Redirect output
	if( m_NullDeviceHandle != NULL )
	{
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FILE, m_NullDeviceHandle);
	}

	// Login to the FTP server
	CURLcode connectionResult = curl_easy_perform(m_CurlSessionHandle);

	if(CURLE_OK != connectionResult)
	{
		char errorMsg[512]={0};
		snprintf(errorMsg, sizeof(errorMsg)-1,"%s, Logon failure to Destination:<%s> curl error< %d, %s>", __func__, destinationName.c_str(), connectionResult, curl_easy_strerror(connectionResult) );
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR: %s", errorMsg);
		AES_CDH_TRACE_MESSAGE(errorMsg);
		Event::report(AES_CDH_ftpConnectFault, "FTP CONNECT ERROR", "Logon failure", errorMsg);
		throw AES_CDH_RC_LOGONFAILURE;
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Connection to FTP server established");
	}

	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
   ROUTINE: ftpDisconnect
=================================================================== */
void LinFTPIV2::ftpDisconnect(void) throw()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if (m_CurlSessionHandle)
	{
		curl_easy_cleanup(m_CurlSessionHandle);
		m_CurlSessionHandle = NULL;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
ROUTINE: ftpVerifyConnection
=================================================================== */
AES_CDH_ResultCode LinFTPIV2::verifyConnection()
{
	AES_CDH_TRACE_MESSAGE("Entering");

	AES_CDH_ResultCode result = AES_CDH_RC_CONNECTERR;
	// get a curl easy session handle
	CURL* curlSessionHandle = curl_easy_init();

	if( NULL != curlSessionHandle)
	{
		if( passWord.empty() && userName.empty() )
		{
			AES_CDH_LOG(LOG_LEVEL_FATAL, "username or password are mandatory");
			AES_CDH_TRACE_MESSAGE("username or password are mandatory");
		}
		else
		{
			// Set User credentials
			std::string userCredentials(userName);

			if( !passWord.empty() )
			{
				userCredentials.push_back(Colon);
				userCredentials += passWord.c_str();
			}
			// Set User credentials as: <userName:password>
			curl_easy_setopt(curlSessionHandle, CURLOPT_USERPWD, userCredentials.c_str());

			// Set a Connection Timeout
			// It should contain the maximum time in seconds that you allow the connection phase
			// to the server to take. This only limits the connection phase, it has no impact once it has connected.
			curl_easy_setopt(curlSessionHandle, CURLOPT_CONNECTTIMEOUT, FTP_PARAMETERS::CONNECTION_TIMEOUT);

			// Set Signal OFF
			curl_easy_setopt(curlSessionHandle, CURLOPT_NOSIGNAL, ON);

			// No DATA transfer open only control connection
			curl_easy_setopt(curlSessionHandle, CURLOPT_NOBODY, ON);

			// Switch on full protocol/debug output
			curl_easy_setopt(curlSessionHandle, CURLOPT_VERBOSE, OFF);

			// Assemble the  remote url
			std::string ftpRootUrl(FTP_PARAMETERS::PROTOCOL);

			if(AES_CDH_Common_Util::isIPv6Address(hostAddress))	// IPv6 address - enclose addr str within '[]'
				ftpRootUrl += "[" + hostAddress + "]";
			else											// IPv4 address
				ftpRootUrl += hostAddress;
			ftpRootUrl.push_back(Colon);
			ftpRootUrl += (portNr.empty() ? FTP_PARAMETERS::DEFAULT_PORT : portNr);
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "LinFTPIV2::verifyConnection() - ftpRootUrl == %s", ftpRootUrl.c_str());
			// Set remote url
			curl_easy_setopt(curlSessionHandle, CURLOPT_URL, ftpRootUrl.c_str());

			// Login to the FTP server
			CURLcode connectionResult = curl_easy_perform(curlSessionHandle);

			if(CURLE_OK != connectionResult)
			{
				char errorMsg[512]={0};
				snprintf(errorMsg, sizeof(errorMsg)-1,"%s, Logon failure to Destination:<%s> curl error< %d, %s>", __func__, destinationName.c_str(), connectionResult, curl_easy_strerror(connectionResult) );
				AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR: %s", errorMsg);
				AES_CDH_TRACE_MESSAGE(errorMsg);
				if(connectionResult == CURLE_LOGIN_DENIED) // TR HS55206
					result = AES_CDH_RC_LOGONFAILURE;
				else
					result = AES_CDH_RC_CONNECTERR;
			}
			else
			{
				AES_CDH_TRACE_MESSAGE("Connection to FTP server established");
				result = AES_CDH_RC_OK;
			}

			// Cleanup curl resources
			curl_easy_cleanup(curlSessionHandle);
		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, "curl api error curl_easy_init() failed!");
		AES_CDH_TRACE_MESSAGE("curl api error curl_easy_init() failed!");
	}

	AES_CDH_TRACE_MESSAGE("Leaving");

	return result;
}

/*===================================================================
ROUTINE: ftpCreateDirectory
=================================================================== */
void LinFTPIV2::ftpCreateDirectory(const std::string& remoteFolderName) throw (AES_CDH_ResultCode)
{
	AES_CDH_TRACE_MESSAGE("Remote folder:<%s>", remoteFolderName.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO,"Remote folder to create in ftp : %s",remoteFolderName.c_str());
	if(!remoteFullPath.empty())
	{
		// create remote folder
		if(m_CurlSessionHandle)
		{
			// Prepare FTP commands to run
			struct curl_slist* quoteCmdList = NULL;

			std::stringstream createFolderCmd;
			createFolderCmd << FTP_COMMAND::CREATE_FOLDER << WhiteSpace << remoteFolderName ;

			quoteCmdList = curl_slist_append(quoteCmdList, createFolderCmd.str().c_str());

			// Set the quote commands to the Curl handle
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_QUOTE, quoteCmdList);

			// Set no data transfer
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOBODY, ON);
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_UPLOAD, OFF);

			// Create missing directory into FTP path
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FTP_CREATE_MISSING_DIRS , ON) ;

			// Perform the requested operations
			CURLcode curlOperationResult = curl_easy_perform(m_CurlSessionHandle);

			// Disable quote operations
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_QUOTE, NULL);

			// Free the quote commands list
			curl_slist_free_all(quoteCmdList);

			if(CURLE_OK != curlOperationResult)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "curl error: < %d, %s> on Folder:<%s> creation", curlOperationResult, curl_easy_strerror(curlOperationResult), remoteFolderName.c_str());
				AES_CDH_TRACE_MESSAGE("curl error: < %d, %s> on Folder:<%s> creation", curlOperationResult, curl_easy_strerror(curlOperationResult), remoteFolderName.c_str());
				throw AES_CDH_RC_SENDERR;
			}
			AES_CDH_TRACE_MESSAGE("Folder:<%s> created", remoteFolderName.c_str());
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR,"CURL Handle is NULL, failed to create remote folder <%s> failed",remoteFolderName.c_str());
			AES_CDH_TRACE_MESSAGE("CURL Handle is NULL, failed to create remote folder <%s> failed",remoteFolderName.c_str());
			throw AES_CDH_RC_SENDERR;
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}

/*===================================================================
   ROUTINE:ftpSendFile
=================================================================== */
AES_CDH_ResultCode LinFTPIV2::ftpSendFile(const std::string& localFile, const std::string& remoteFile)
{
	AES_CDH_TRACE_MESSAGE("FTP transfer of local file:<%s> to remote file:<%s> ", localFile.c_str(), remoteFile.c_str());

	AES_CDH_ResultCode sendResult = AES_CDH_RC_OK;
	// Local file handle
	std::FILE* localFileHandle = fopen(localFile.c_str(), "rb");
	if(NULL != localFileHandle)
	{
		// local file found and opened for read
		// check Curl handle
		if(NULL != m_CurlSessionHandle)
		{
			// Set remote file url
			char remoteFileUrl[1024] = {0};
			snprintf(remoteFileUrl, sizeof(remoteFileUrl)-1, "%s/%s", m_FtpRootUrl.c_str(), remoteFile.c_str());

			AES_CDH_TRACE_MESSAGE("remote file url:<%s>", remoteFileUrl);

			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, remoteFileUrl);
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_UPLOAD, ON);

			// Set the input local file handle
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_READDATA, localFileHandle);

			// Set on/off all wanted options
			// Enable ftp data connection
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOBODY, OFF);

			// Create missing directory into FTP path
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FTP_CREATE_MISSING_DIRS , ON) ;

			// Set the progress function, in oder to check the stop transfer request
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOPROGRESS, OFF);
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_PROGRESSFUNCTION, LinFTPIV2::progressCb);
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_PROGRESSDATA, this);

			CURLcode curlOperationResult = curl_easy_perform(m_CurlSessionHandle);

			if(CURLE_OK != curlOperationResult)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "curl error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult));
				AES_CDH_TRACE_MESSAGE("curl error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult));

				bool bStopFileTransfer = ServR::checkStopFlag(destinationPtr->getDestSetName(), aFileName, aNewMainFileName, aNewSubFileName);
				if( bStopFileTransfer )
				{
					AES_CDH_LOG(LOG_LEVEL_INFO, "file transfer stopped.");
					AES_CDH_TRACE_MESSAGE("file transfer stopped.");

					sendResult = AES_CDH_RC_TASKSTOPPED;
				}
				else
				{
					char errorMsg[512] = {0};
					snprintf(errorMsg, sizeof(errorMsg)-1, "when sending file: %s to %s", localFile.c_str(), remoteFileUrl );
					Event::report(AES_CDH_remoteFileFault, "REMOTE FILE FAULT",	curl_easy_strerror(curlOperationResult), errorMsg);

					sendResult = AES_CDH_RC_SENDERR;
					AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR %s", errorMsg);
					AES_CDH_TRACE_MESSAGE("ERROR %s", errorMsg);
				}
			}
			else
			{
				// File successfully transferred to the remote site
				AES_CDH_TRACE_MESSAGE("local file:<%s> has been transferred to the remote url:<%s>", localFile.c_str(), remoteFileUrl);
			}
		}
		else
		{
			// Curl handle error
			sendResult = AES_CDH_RC_SENDERR;
			AES_CDH_LOG(LOG_LEVEL_WARN, "CURL Handle is NULL, FTP transfer of local file <%s> failed", localFile.c_str());
			AES_CDH_TRACE_MESSAGE("CURL Handle is NULL, FTP transfer of local file <%s> failed", localFile.c_str());

		}

		// close the opened file
		fclose(localFileHandle);
	}
	else
	{
		// failed to open input file
		int fileOpenError = errno;
		char errorText[64] = {0};

		strerror_r(fileOpenError, errorText, sizeof(errorText)-1);
		char errorMsg[1024] = {0};
		snprintf(errorMsg, sizeof(errorMsg)-1, "local file:<%s> open failed, error:[ <%d>, %s}", localFile.c_str(), fileOpenError, errorText );
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR %s", errorMsg);
		AES_CDH_TRACE_MESSAGE("ERROR %s", errorMsg);
		Event::report(AES_CDH_fileErrFault, "FILE ERROR", errorText, errorMsg);

		sendResult = AES_CDH_RC_FILEERR;
	}

	AES_CDH_TRACE_MESSAGE("Leaving, result: %d", sendResult);
	return sendResult;
}

/*===================================================================
   ROUTINE: ftpSendFileWithRename
=================================================================== */
AES_CDH_ResultCode LinFTPIV2::ftpSendFileWithRename(const std::string& localFileName, const std::string& remoteFilePath, const std::string& remoteFileName, bool deleteOldRemoteFile )
{
	AES_CDH_TRACE_MESSAGE("FTP transfer of local file:<%s> to remote path:<%s> remote file:<%s> ", localFileName.c_str(), remoteFilePath.c_str(), remoteFileName.c_str());
	AES_CDH_ResultCode sendResult = AES_CDH_RC_OK;
	// Local file handle
	std::FILE* localFileHandle = fopen(localFileName.c_str(), "rb");

	if(NULL != localFileHandle)
	{
		// local file found and opened for read
		// check Curl handle
		if(NULL != m_CurlSessionHandle)
		{
			// make temporary file name
			std::string remoteTmpFile(remoteFileName);
			remoteTmpFile.append(TemporaryExtensionName);

			// Prepare FTP commands to run after that transfer is completed
			// in order to rename the remote file
			struct curl_slist* postQuoteCmdList = NULL;

			// Delete the remote file already present in case of overwrite mode
			if(deleteOldRemoteFile)
			{
				AES_CDH_TRACE_MESSAGE("remote file already exists delete it before transfer");
				std::stringstream deleteCmd;
				deleteCmd << FTP_COMMAND::DELETE << WhiteSpace << remoteFileName;

				postQuoteCmdList = curl_slist_append(postQuoteCmdList, deleteCmd.str().c_str());
			}

			std::stringstream renameCmdFrom;
			renameCmdFrom << FTP_COMMAND::RENAME_FROM << WhiteSpace << remoteTmpFile;

			postQuoteCmdList = curl_slist_append(postQuoteCmdList, renameCmdFrom.str().c_str());

			std::stringstream renameCmdTo;
			renameCmdTo << FTP_COMMAND::RENAME_TO << WhiteSpace << remoteFileName;

			postQuoteCmdList = curl_slist_append(postQuoteCmdList, renameCmdTo.str().c_str());

			// Set the post quote commands to the Curl handle
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_POSTQUOTE, postQuoteCmdList);
			AES_CDH_TRACE_MESSAGE("post quote cmds: <%s> <%s>", renameCmdFrom.str().c_str(), renameCmdTo.str().c_str());


			// Set remote file url
			char remoteFileUrl[1024] = {0};
			snprintf(remoteFileUrl, sizeof(remoteFileUrl)-1, "%s/%s%s", m_FtpRootUrl.c_str(), remoteFilePath.c_str(), remoteTmpFile.c_str());
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, remoteFileUrl);
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_UPLOAD, ON);

			AES_CDH_TRACE_MESSAGE("remote file url:<%s>", remoteFileUrl);

			// Set the input local file handle
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_READDATA, localFileHandle);

			// Set on/off all wanted options
			// Enable ftp data connection
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOBODY, OFF);

			// Create missing directory into FTP path
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FTP_CREATE_MISSING_DIRS, ON) ;

			// Set the progress function, in oder to check the stop transfer request
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOPROGRESS, OFF);
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_PROGRESSFUNCTION, LinFTPIV2::progressCb);
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_PROGRESSDATA, this);


			CURLcode curlOperationResult = curl_easy_perform(m_CurlSessionHandle);

			if(CURLE_OK != curlOperationResult)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "curl error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult) );
				AES_CDH_TRACE_MESSAGE("curl error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult));

				bool bStopFileTransfer = ServR::checkStopFlag(destinationPtr->getDestSetName(), aFileName, aNewMainFileName, aNewSubFileName);
				if( bStopFileTransfer )
				{
					AES_CDH_TRACE_MESSAGE("file transfer stopped.");

					sendResult = AES_CDH_RC_TASKSTOPPED;
				}
				else
				{
					char errorMsg[512] = {0};
					snprintf(errorMsg, sizeof(errorMsg)-1, "when sending file: %s to %s", localFileName.c_str(), remoteFileUrl );
					Event::report(AES_CDH_remoteFileFault, "REMOTE FILE FAULT",	curl_easy_strerror(curlOperationResult), errorMsg);

					sendResult = AES_CDH_RC_SENDERR;
					AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR %s", errorMsg);
					AES_CDH_TRACE_MESSAGE("ERROR %s", errorMsg);
				}
			}
			else
			{
				// File successfully transferred to the remote site
				AES_CDH_TRACE_MESSAGE("local file:<%s> has been transferred to the remote url:<%s>", localFileName.c_str(), remoteFileUrl);
			}

			// Disable quote operations
			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_POSTQUOTE, NULL);
			// free the post quote commands list
			curl_slist_free_all(postQuoteCmdList);
		}
		else
		{
			// Curl handle error
			sendResult = AES_CDH_RC_SENDERR;
			AES_CDH_TRACE_MESSAGE("CURL Handle is NULL, FTP transfer of local file <%s> failed", localFileName.c_str());

		}

		// close the opened file
		fclose(localFileHandle);
	}
	else
	{
		// failed to open input file
		int fileOpenError = errno;
		char errorBuffer[64] = {0};

		std::string errorText(strerror_r(fileOpenError, errorBuffer, sizeof(errorBuffer)-1));
		char errorMsg[1024] = {0};
		snprintf(errorMsg, sizeof(errorMsg)-1, "local file:<%s> open failed, error:[<%d>, %s]", localFileName.c_str(), fileOpenError, errorText.c_str() );
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR %s", errorMsg);
		AES_CDH_TRACE_MESSAGE("ERROR %s", errorMsg);
		Event::report(AES_CDH_fileErrFault, "FILE ERROR", errorText, errorMsg);

		sendResult = AES_CDH_RC_FILEERR;
	}

	AES_CDH_TRACE_MESSAGE("Leaving, result: %d", sendResult);
	return sendResult;
}

/*===================================================================
   ROUTINE: ftpRenameObject
=================================================================== */
bool LinFTPIV2::ftpRenameObject(const std::string& remotePath, const std::string& newName)
{
	AES_CDH_TRACE_MESSAGE("Entering, rename temporary remote object on <%s> to <%s>", remotePath.c_str(), newName.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO, "Rename temporary remote object on <%s> to <%s>", remotePath.c_str(), newName.c_str());

	bool result = false;

	if(m_CurlSessionHandle)
	{
		// NO Data transfer
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOBODY, ON);
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_UPLOAD, OFF);

		// Set remote url
		char remotePathUrl[1024] = {0};
		snprintf(remotePathUrl, sizeof(remotePathUrl)-1, "%s/%s", m_FtpRootUrl.c_str(), remotePath.c_str());
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, remotePathUrl);

		// Prepare FTP commands to run
		struct curl_slist* postQuoteCmdList = NULL;

		std::stringstream renameCmdFrom;
		renameCmdFrom << FTP_COMMAND::RENAME_FROM << WhiteSpace << newName << TemporaryExtensionName;

		postQuoteCmdList = curl_slist_append(postQuoteCmdList, renameCmdFrom.str().c_str());

		std::stringstream renameCmdTo;
		renameCmdTo << FTP_COMMAND::RENAME_TO << WhiteSpace << newName;

		postQuoteCmdList = curl_slist_append(postQuoteCmdList, renameCmdTo.str().c_str());

		// Set the quote commands to the Curl handle
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_POSTQUOTE, postQuoteCmdList);

		// Perform the requested operations
		CURLcode curlOperationResult = curl_easy_perform(m_CurlSessionHandle);

		result = (CURLE_OK == curlOperationResult);

		// Disable quote operations
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_POSTQUOTE, NULL);

		// Free the quote commands list
		curl_slist_free_all(postQuoteCmdList);

		if(!result)
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, "rename of <%s> FAILED, curl error:<%d> ", newName.c_str(), curlOperationResult);
			AES_CDH_TRACE_MESSAGE("rename of <%s> FAILED, curl error:<%d> ", newName.c_str(), curlOperationResult);

		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "CURL Handle is NULL, rename of <%s> failed", newName.c_str());
		AES_CDH_TRACE_MESSAGE("CURL Handle is NULL, rename of <%s> failed", newName.c_str());

	}

	AES_CDH_TRACE_MESSAGE("Leaving");
	return result;
}

/*===================================================================
ROUTINE:renameRemoteDir
=================================================================== */
bool LinFTPIV2::renameRemoteDir(const std::string& remotePath, const std::string& newName)
{
	AES_CDH_TRACE_MESSAGE("Entering, rename temporary remote folder on <%s> to <%s>", remotePath.c_str(), newName.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO,"Entering, rename temporary remote folder on <%s> to <%s>", remotePath.c_str(), newName.c_str());

	bool result = ftpRenameObject(remotePath, newName);

	if(!result )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "First attempt to rename temporary remote folder failed, remotePath:<%s>, folderName:<%s>", remotePath.c_str(), newName.c_str());
		AES_CDH_TRACE_MESSAGE("First attempt to rename temporary remote folder failed, remotePath:<%s>, folderName:<%s>", remotePath.c_str(), newName.c_str());

		// Search an alternative name
		std::string alternativeName(newName);
		if(isRemoteDir(remotePath, newName) && remoteFolderNameWithTimeStamp(remotePath, alternativeName) )
		{
			// Remote folder already exist, found alternative name
			result = ftpRenameObject(remotePath, alternativeName);
		}

		if(!result)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Second attempt to rename temporary remote folder failed, remotePath:<%s>, folderName:<%s>", remotePath.c_str(), alternativeName.c_str());
			AES_CDH_TRACE_MESSAGE("Second attempt to rename temporary remote folder failed, remotePath:<%s>, folderName:<%s>", remotePath.c_str(), alternativeName.c_str());
		}
	}

	AES_CDH_TRACE_MESSAGE("Leaving, result:<%s>", (result ? "OK" : "NOT OK"));

	return result;
}

/*===================================================================
   ROUTINE:isRemoteFile
   	Returns true if fileName exists at the remote location
=================================================================== */
bool LinFTPIV2::isRemoteFile(const string& fileName) const
{
	AES_CDH_TRACE_MESSAGE("Entering, remote file to find:<%s>", fileName.c_str());

	bool result = false;
	if(m_CurlSessionHandle)
	{
		// Remote File URL : <IP>:<FTPORT>/<REMOTE FILE> (10.35.85.1:21/FTPFOLDER/TQ/FILE1)
		char remoteFileURL[1024] = {0};
		snprintf(remoteFileURL, sizeof(remoteFileURL)-1, "%s/%s", m_FtpRootUrl.c_str(), fileName.c_str() );

		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, remoteFileURL);

		// No DATA transfer
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOBODY, ON);
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FILETIME, ON);

		result = (curl_easy_perform(m_CurlSessionHandle) == CURLE_OK);
		AES_CDH_TRACE_MESSAGE("remote file %s", (result ? "EXIST" : "NOT EXIST"));

		// Go back to the FTP root folder
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, m_FtpRootUrl.c_str());
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FILETIME, OFF);

		CURLcode curlOperationResult = curl_easy_perform(m_CurlSessionHandle);
		if( curlOperationResult != CURLE_OK )
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, "FAILED to go back to the FTP root folder error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult));
			AES_CDH_TRACE_MESSAGE("FAILED to go back to the FTP root folder error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult));
		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "CURL Handle is NULL");
		AES_CDH_TRACE_MESSAGE("CURL Handle is NULL");

	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return result;
}

/*=================================================================
   ROUTINE:isRemoteDir
 	 Returns true if dirName exists at the remote location
=================================================================== */
bool LinFTPIV2::isRemoteDir(const std::string& remotePath, const std::string& dirName) const
{
	AES_CDH_TRACE_MESSAGE("Entering");
	bool result = false;
	if(m_CurlSessionHandle)
	{
		// Remote File URL : <IP>:<FTPORT>/<REMOTE FILE> (10.35.85.1:21/FTPFOLDER/TQ/FILE1)
		char remoteFolderURL[1024] = {0};
		snprintf(remoteFolderURL, sizeof(remoteFolderURL)-1,"%s/%s%s", m_FtpRootUrl.c_str(), remotePath.c_str(), dirName.c_str() );

		AES_CDH_TRACE_MESSAGE("remoteFolderURL:<%s>", remoteFolderURL);

		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, remoteFolderURL);

		// No DATA transfer
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_NOBODY, ON);
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FILETIME, ON);

		CURLcode curlOperationResult = curl_easy_perform(m_CurlSessionHandle);

		//result = ( (CURLE_FTP_COULDNT_RETR_FILE != curlOperationResult) && (CURLE_REMOTE_ACCESS_DENIED != curlOperationResult));
		result = ( curlOperationResult == CURLE_OK );      //IA49319
		AES_CDH_TRACE_MESSAGE("remote folder %s, result:%d", (result ? "EXIST" : "NOT EXIST"), curlOperationResult);

		// Check if the remote folder already exists
		if(!result)
		{
			// Remote folder does not exist, check if already exist the temporary folder ".tmp"
			std::string tmpFolder(remoteFolderURL);
			tmpFolder.append(TemporaryExtensionName);

			curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, tmpFolder.c_str() );
			curlOperationResult = curl_easy_perform(m_CurlSessionHandle);
			//result = ( (CURLE_FTP_COULDNT_RETR_FILE != curlOperationResult) && (CURLE_REMOTE_ACCESS_DENIED != curlOperationResult));
			result = ( curlOperationResult == CURLE_OK );  //IA49319
			AES_CDH_TRACE_MESSAGE("remote temporary folder %s, res: %d", (result ? "EXIST" : "NOT EXIST"), curlOperationResult);

		}

		// Go back to the FTP root folder
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_URL, m_FtpRootUrl.c_str());
		curl_easy_setopt(m_CurlSessionHandle, CURLOPT_FILETIME, OFF);

		curlOperationResult = curl_easy_perform(m_CurlSessionHandle);
		if( CURLE_OK != curlOperationResult )
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "FAILED to go back to the FTP root folder error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult));
			AES_CDH_TRACE_MESSAGE("FAILED to go back to the FTP root folder error: < %d, %s>", curlOperationResult, curl_easy_strerror(curlOperationResult));
		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "CURL Handle is NULL");
		AES_CDH_TRACE_MESSAGE("CURL Handle is NULL");
	}
	AES_CDH_TRACE_MESSAGE("Leaving, remote folder result:<%s>",(result ? "EXISTS" : "NOT EXISTS"));
	return result;
}

/*===================================================================
   ROUTINE: sendFileInitiating
    localFile = <path + filename> of local file
    remoteSubDirName = TQ name or empty if the attribute "suppressSubFolderCreation" is true
    newFileName = remote file name not equal to the local file name
=================================================================== */
AES_CDH_ResultCode LinFTPIV2::sendFileInitiating(const std::string& localFile,
												 const std::string& remoteSubDirName,
												 const std::string& newFileName,
												 AES_CDH_DestinationSet::transferMode trMode)
{
	AES_CDH_TRACE_MESSAGE("Entering, localFile: <%s>, remoteTQsubfolder: <%s>, remoteFileName: <%s>",  localFile.c_str(), remoteSubDirName.c_str(), newFileName.c_str());

	UNUSED(trMode);
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	std::string remoteFileName(newFileName);

	if( !assembleRemoteFileName(localFile, remoteFileName) )
	{
		// Error on assemble the remote file name
		Event::report(AES_CDH_fileNameFault, "FILE NAME ERROR", "Remote file name too long : ",	remoteFileName);

		AES_CDH_TRACE_MESSAGE("remote file name:<%s> too long", remoteFileName.c_str());

		returnCode = AES_CDH_RC_FILENAMETOOLONG;
	}
	else
	{
		// The remote path without the remote file name
		remoteFullPath.assign(remoteDir);

		// Check for TQ name extra folder adding
		if(!remoteSubDirName.empty())
		{
			//Specifies the creation of a subdirectory with the transfer queue name on the remote system
			remoteFullPath.append(remoteSubDirName);
			remoteFullPath.push_back(DirDelim);
		}

		// The remote file: <path + file name>
		std::string remoteFile(remoteFullPath);
		remoteFile.append(remoteFileName);

		AES_CDH_TRACE_MESSAGE("full remote file path: <%s>", remoteFile.c_str());

		try
		{
			// Open a ftp control connection towards the ftp server
			ftpConnect(hostAddress, portNr, userName, passWord);

			// Check if this remote file already exists
			bool remoteFileAlreadyExist = isRemoteFile(remoteFile);

			if( overWrite.compare("yes") == 0U )
			{
				// ******** OVERWRITE SECTION START ********
				AES_CDH_TRACE_MESSAGE("FTP transfer in remote file overwrite mode");
				returnCode = ftpSendFileWithRename(localFile, remoteFullPath, remoteFileName, remoteFileAlreadyExist);
			}
			else
			{
				// ******** NO OVERWRITE SECTION START ********
				AES_CDH_TRACE_MESSAGE("FTP transfer in NO remote file overwrite mode");

				if(remoteFileAlreadyExist)
				{
					// The file already exists at the remote location and must
					// therefore be renamed before it is sent to prevent data loss
					AES_CDH_TRACE_MESSAGE("a remote file named:<%s> already exists to the remote path:<%s>", remoteFileName.c_str(), remoteFullPath.c_str());

					// Search for a remote file name that does not already present
					if( remoteFileNameWithTimeStamp(remoteFullPath, remoteFileName) )
					{
						// Remote file name found, send it
						returnCode = ftpSendFileWithRename(localFile, remoteFullPath, remoteFileName);
					}
					else
					{
						// Failure to find a unique file name. 100 retries have been made and none was successful.
						// Return some kind of error here
						char errorMsg[1024] = {0};
						snprintf(errorMsg, 1023, "Attempt to rename %s with a sequence number higher than 100. Because of this, the file was not sent to the destination", remoteFile.c_str() );
						Event::report(AES_CDH_physFileFault, "PHYSICAL FILE FAULT", " ", errorMsg );
						AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR %s", errorMsg);
						AES_CDH_TRACE_MESSAGE("ERROR %s", errorMsg);
						returnCode = AES_CDH_RC_SENDERR;
					}
				}
				else
				{
					AES_CDH_TRACE_MESSAGE("remote file does not exist");
					// The file does not exist at the remote location
					// Send the file now
					returnCode = ftpSendFileWithRename(localFile, remoteFullPath, remoteFileName);
				}
			}
		}
		catch(const AES_CDH_ResultCode& exception)
		{
			returnCode = exception;
			AES_CDH_LOG(LOG_LEVEL_INFO, "caught error:<%d>", returnCode);
			AES_CDH_TRACE_MESSAGE("caught error:<%d>", returnCode);
		}

		ftpDisconnect();
	}

	AES_CDH_TRACE_MESSAGE("Leaving, local file:<%s> transfer result:<%d>", localFile.c_str(), returnCode);
	return returnCode;
}

/*===================================================================
   ROUTINE: sendDirectoryInitiating
    dirName = <path + folder> of local folder
    remoteSubDirName = TQ name or empty if the attribute "suppressSubFolderCreation" is true
    newDirName = remote folder name not equal to the local file name
=================================================================== */
AES_CDH_ResultCode LinFTPIV2::sendDirectoryInitiating(const std::string& dirName,
													  const std::string& remoteSubDirName,
													  const std::string& newDirName,
													  AES_CDH_DestinationSet::transferMode trMode,
													  const std::string& fileMask)
{
	AES_CDH_TRACE_MESSAGE("Entering, local Folder:<%s>, remotesubdir: %s, remote Folder:<%s>", dirName.c_str(), remoteSubDirName.c_str(), newDirName.c_str());
	UNUSED(trMode);
	UNUSED(fileMask);
	AES_CDH_ResultCode returnCode = AES_CDH_RC_SENDERR;

	aRemoteDirName.assign(newDirName);
	assembleRemoteFolderName(dirName, aRemoteDirName);

	try
	{
		// Open a control connection towards the ftp server
		ftpConnect(hostAddress, portNr, userName, passWord);

		// The remote path without the remote file name
		remoteFullPath.assign(remoteDir);

		if(!remoteSubDirName.empty())
		{
			//Specifies the creation of a subdirectory with the transfer queue name on the remote system
			remoteFullPath.append(remoteSubDirName);
			remoteFullPath.push_back(DirDelim);
		}

		// Check if the remote folder already exists
		if( isRemoteDir( remoteFullPath, aRemoteDirName) )
		{
			AES_CDH_TRACE_MESSAGE("remote folder or temporary folder already exist");

			// Search for a remote folder name that does not already present
			if( remoteFolderNameWithTimeStamp(remoteFullPath, aRemoteDirName) )
			{
				// Remote folder name found, send it
				AES_CDH_TRACE_MESSAGE("remote folder name:<%s> found", aRemoteDirName.c_str());

				// make temporary file name
				std::string remoteTmpFolder(remoteFullPath);
				remoteTmpFolder.append(aRemoteDirName);
				remoteTmpFolder.append(TemporaryExtensionName);

				returnCode = doSendDirInitiating(dirName, remoteTmpFolder);
			}
			else
			{
				// Failure to find a unique folder name. 100 retries have been made and none was successful.
				// Return some kind of error here
				char errorMsg[1024] = {0};
				snprintf(errorMsg, 1023, "Attempt to rename %s with a sequence number higher than 100. Because of this, the file was not sent to the destination", aRemoteDirName.c_str() );
				Event::report(AES_CDH_physFileFault, "PHYSICAL FILE FAULT", " ", errorMsg );
				AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR %s", errorMsg);
				AES_CDH_TRACE_MESSAGE("ERROR %s", errorMsg);
				returnCode = AES_CDH_RC_SENDERR;
			}
		}
		else
		{
			// make temporary file name
			std::string remoteTmpFolder(remoteFullPath);
			remoteTmpFolder.append(aRemoteDirName);
			remoteTmpFolder.append(TemporaryExtensionName);

			returnCode = doSendDirInitiating(dirName, remoteTmpFolder);
		}

		// Rename transferred folder
		if(AES_CDH_RC_OK == returnCode)
		{
			if(renameRemoteDir(remoteFullPath, aRemoteDirName))
			{
				returnCode = AES_CDH_RC_OK;
				AES_CDH_TRACE_MESSAGE("remote folder <%s> successfully renamed",aRemoteDirName.c_str());
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Failed to rename remote folder <%s>.",aRemoteDirName.c_str());
			}
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Failed to send the directory <%s>:<%d>", aRemoteDirName.c_str(),returnCode);
		}
	}
	catch(const AES_CDH_ResultCode& exception)
	{
		returnCode = exception;
		AES_CDH_LOG(LOG_LEVEL_ERROR, "caught error:<%d>", returnCode);
		AES_CDH_TRACE_MESSAGE("caught error:<%d>", returnCode);
	}

	ftpDisconnect();

	AES_CDH_TRACE_MESSAGE("Leaving");
	return returnCode;
}

/*===================================================================
   ROUTINE: doSendDirInitiating
=================================================================== */
AES_CDH_ResultCode LinFTPIV2::doSendDirInitiating(const std::string& localFolderPath, const std::string& remoteFolderPath)
{
	AES_CDH_TRACE_MESSAGE("Entering, localFolder:<%s>, remoteFolder:<%s>", localFolderPath.c_str(), remoteFolderPath.c_str());
	AES_CDH_ResultCode returnCode = AES_CDH_RC_OK;

	// Open the local directory
	DIR* localFolderHandler = opendir(localFolderPath.c_str());
	bool bStopFileTransfer = false;
	if(NULL == localFolderHandler )
	{
		int openError = errno;
		char errorMsg[1204] = {0};
		snprintf(errorMsg, sizeof(errorMsg)-1, "%s, open folder:<%s> failed, error:<%d>", __func__, localFolderPath.c_str(), openError );
		AES_CDH_LOG(LOG_LEVEL_FATAL, "ERROR %s", errorMsg);
		AES_CDH_TRACE_MESSAGE("ERROR %s", errorMsg);
		returnCode = AES_CDH_RC_FILEERR;;
	}
	else
	{
		// Local folder successfully opened
		struct dirent* finddata;
		// Scan all directory elements
		while (((finddata = readdir(localFolderHandler)) != NULL) && (AES_CDH_RC_OK == returnCode) && (!bStopFileTransfer ))
		{
			// Skip the "." and ".." folders
			if (strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, "..") )
			{
				// Check if a folder or a file
				if(DT_DIR == finddata->d_type)
				{
					AES_CDH_TRACE_MESSAGE("found folder:<%s>", finddata->d_name);

					// Folder to send
					std::string innerLocalFolderPath(localFolderPath);
					innerLocalFolderPath.push_back(DirDelim);
					innerLocalFolderPath.append(finddata->d_name);

					std::string innerRemoteFolderPath(remoteFolderPath);
					innerRemoteFolderPath.push_back(DirDelim);
					innerRemoteFolderPath.append(finddata->d_name);
					// Recall recursively this same method to send the inner folder
					returnCode = doSendDirInitiating(innerLocalFolderPath, innerRemoteFolderPath);
				}
				else
				{
					// File to send
					AES_CDH_TRACE_MESSAGE("found file:<%s>", finddata->d_name);
					// Assemble the local file path
					std::string localFile(localFolderPath);
					localFile.push_back(DirDelim);
					localFile.append(finddata->d_name);

					// Assemble the remote file path
					std::string remoteFile(remoteFolderPath);
					remoteFile.push_back(DirDelim);
					remoteFile.append(finddata->d_name);
					// Send the file
					returnCode = ftpSendFile(localFile, remoteFile);
				}
			}

			bStopFileTransfer = ServR::checkStopFlag(destinationPtr->getDestSetName(), aFileName, aNewMainFileName, aNewSubFileName);
		} //end of while loop

		// Close directory handle
		closedir(localFolderHandler);
	}

	AES_CDH_TRACE_MESSAGE("Leaving, result:<%d>", returnCode);
	return returnCode;
}

/*===================================================================
   ROUTINE: assembleRemoteFileName
=================================================================== */
bool LinFTPIV2::assembleRemoteFileName(const std::string& localFileName, std::string& remoteFileName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	if(remoteFileName.empty())
	{
		// Remote file name named as the local file
		size_t slashPos = localFileName.find_last_of("\\/:");

		// If '/' or '\' or ':' is not found then copy whole filename
		if( std::string::npos == slashPos)
		{
			// local file name without path
			remoteFileName = localFileName;
		}
		else
		{
			// remove path part from the local file name
			remoteFileName = localFileName.substr(slashPos + 1);
		}
	}

	// Check remote file name length
	bool result = (remoteFileName.length() < AES_FTP_FILE_NAME_SIZE);

	AES_CDH_TRACE_MESSAGE("Leaving");
	return result;
}

/*===================================================================
   ROUTINE: assembleRemoteFolderName
=================================================================== */
void LinFTPIV2::assembleRemoteFolderName(const std::string& localFolderName, std::string& remoteFolderName)
{
	AES_CDH_TRACE_MESSAGE("Entering, localFolderName:<%s>",  localFolderName.c_str());

	if(remoteFolderName.empty())
	{
		// Remote file name named as the local file, skip in the search last char since it can be "/"
		size_t slashPos = localFolderName.find_last_of("\\/", localFolderName.length() - 2U );

		// If '/' or '\' or ':' is not found then copy whole filename
		if( std::string::npos == slashPos)
		{
			// local file name without path
			remoteFolderName = localFolderName;
		}
		else
		{
			// remove path part from the local file name
			remoteFolderName = localFolderName.substr(slashPos + 1);

			// Search and remove the final "/" if it is present
			slashPos = remoteFolderName.find_last_of("\\/");
			if( std::string::npos != slashPos)
				remoteFolderName.erase(slashPos);
		}
	}

	AES_CDH_TRACE_MESSAGE("Leaving, remoteFolderName:<%s>", remoteFolderName.c_str());
}

/*===================================================================
   ROUTINE: remoteFileNameWithTimeStamp
=================================================================== */
bool LinFTPIV2::remoteFileNameWithTimeStamp(const std::string& remoteFilePath, std::string& remoteFileName)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	bool result = false;
	int maxRetries = 0;

	do{
		// Get the timestamp
		time_t ltime;
		time(&ltime);
		// make the file name with timestamp
		char tmpFileName[AES_FTP_FILE_NAME_SIZE + 1 ] = {0};
		snprintf(tmpFileName, AES_FTP_FILE_NAME_SIZE, "%s_%ld", remoteFileName.c_str(), ltime);

		std::string remoteFileTimeStamped(remoteFilePath);
		remoteFileTimeStamped.append(tmpFileName);

		// Check if already exists a remote file with this name
		if( isRemoteFile(remoteFileTimeStamped) )
		{
			// Remote File already present, try again
			++maxRetries;
			// wait a bit before retry
			usleep(500);
		}
		else
		{
			// A valid remote file name found
			remoteFileName.assign(tmpFileName);
			result = true;
			break;
		}

	}while( MAX_RETRY >= maxRetries );

	AES_CDH_TRACE_MESSAGE("Leaving,  remoteFileName:<%s>", remoteFileName.c_str());

	return result;
}

/*===================================================================
   ROUTINE: remoteFolderNameWithTimeStamp
=================================================================== */
bool LinFTPIV2::remoteFolderNameWithTimeStamp(const std::string& remoteFolderPath, std::string& remoteFolderName)
{
	AES_CDH_TRACE_MESSAGE("Entering");

	bool result = false;
	int maxRetries = 0;

	do
	{
		// Get the timestamp
		time_t ltime;
		time(&ltime);
		// make the file name with timestamp
		char tmpFolderName[AES_FTP_FILE_NAME_SIZE + 1 ] = {0};
		snprintf(tmpFolderName, AES_FTP_FILE_NAME_SIZE, "%s_%ld", remoteFolderName.c_str(), ltime);

		// Check if already exists a remote file with this name
		if( isRemoteDir(remoteFolderPath, tmpFolderName) )
		{
			// Remote File already present, try again
			++maxRetries;
			// wait a bit before retry
			usleep(500);
		}
		else
		{
			// A valid remote file name found
			remoteFolderName.assign(tmpFolderName);
			result = true;
			break;
		}

	}while( MAX_RETRY >= maxRetries );

	AES_CDH_TRACE_MESSAGE("Leaving,  remoteFolder:<%s>", remoteFolderName.c_str());
	return result;
}

/*===================================================================
   ROUTINE: progressCb
=================================================================== */
int LinFTPIV2::progressCb(void * ptr, double dwld, double nowdwnld, double totup, double nowupload)
{
	UNUSED(dwld);
	UNUSED(nowdwnld);
	UNUSED(totup);
	UNUSED(nowupload);
	int result = 0;
	LinFTPIV2* temp= reinterpret_cast<LinFTPIV2*>(ptr);

	if(temp)
	{
		bool bStopFileTransfer = ServR::checkStopFlag(temp->destinationPtr->getDestSetName(), temp->aFileName, temp->aNewMainFileName, temp->aNewSubFileName);
		result = (bStopFileTransfer ? 5 : 0);
	}

	return result;
}

