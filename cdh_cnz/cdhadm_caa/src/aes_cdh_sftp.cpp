/*=================================================================== */
/**
 @file aes_cdh_sftp.cpp

Class method implementation for aes_cdh_sftp.h

DESCRIPTION
The services provided by this class facilitates the sftp transfer of files and directories.

General rule:l

The error handling is specified for each method.
No methods initiate or send error reports unless specified.

@version 1.1.1

HISTORY
This section contains reference to problem report and related
software correction performed inside this module

PR           DATE      INITIALS    DESCRIPTION
-----------------------------------------------------------

N/A       29/11/2011     xbhadur       Initial Release
=================================================================== */
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "aes_cdh_sftp.h"
#include "sshftpiv2.h"
#include <iostream>
#include <servr.h>
#include <ctype.h>
#include <ace/ACE.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ace/OS_NS_sys_select.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"
#include "aes_cdh_common.h"

using namespace std;


ACE_Thread_Mutex SftpAdapterClass::libssh2Mx;
int SftpAdapterClass::MAX_RETRY = 100;
const string legalChars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789/_-.:\\ %$+^!@#");
/*===================================================================
                        DECLARATION SECTION
=================================================================== */
AES_CDH_TRACE_DEFINE(AES_CDH_SftpAdapterClass);

/*===================================================================
   ROUTINE:init_attrs
=================================================================== */
void SftpAdapterClass::init_attrs(LIBSSH2_SFTP_ATTRIBUTES* attrs, struct stat* st)
{
	AES_CDH_TRACE_MESSAGE("Entering...");
	ACE_OS::memset(attrs,'\0',sizeof(LIBSSH2_SFTP_ATTRIBUTES));
	attrs->filesize = st->st_size;
	attrs->flags |= LIBSSH2_SFTP_ATTR_SIZE;
	attrs->permissions = st->st_mode;
	attrs->flags |= LIBSSH2_SFTP_ATTR_PERMISSIONS;
	attrs->atime = st->st_atime;
	attrs->mtime = st->st_mtime;
	attrs->flags |= LIBSSH2_SFTP_ATTR_ACMODTIME;
	AES_CDH_TRACE_MESSAGE("leaving ...");
}
/*===================================================================
   ROUTINE:SftpAdapterClass
=================================================================== */
SftpAdapterClass::SftpAdapterClass(): session(NULL),sftp_session(NULL),sock(-1),strIpAddress(""),nPort(-1),strUser(""), strPassword(""), breakTransm(false),parameterChanged(false),sftpsession1(false),SFTPRESULT(SFTPAPI_OK),sftpcheck(false)//,status(false),osTyp(false)
{
	AES_CDH_TRACE_MESSAGE("Entering ...");
	AES_CDH_TRACE_MESSAGE("Leaving ...");
}

/*===================================================================
   ROUTINE:sftpInitiate
=================================================================== */
AES_CDH_ResultCode SftpAdapterClass::sftpInitiate(void *psshftpiv2, const char * hostAddress, int portNr, const char * userName, const char * passWord,bool ascii)
{
	(void) ascii;
	AES_CDH_TRACE_MESSAGE("Entering...");
	if (strcmp(strIpAddress.c_str() ,hostAddress)!=0)
	{
		parameterChanged = true;
		strIpAddress.assign(hostAddress);
	}

	if (portNr != nPort)
	{
		parameterChanged = true;
		nPort = portNr;
	}
	if (strcmp(strUser.c_str(),userName)!=0)
	{
		parameterChanged = true;
		strUser.assign(userName);
	}

	if (strcmp(strPassword.c_str(),passWord)!=0)
	{
		parameterChanged = true;
		strPassword.assign(passWord);
	}
	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "SFTP is not connected as stop transmission signal");
		AES_CDH_TRACE_MESSAGE("SFTP is not connected as stop transmission signal");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return AES_CDH_RC_TASKSTOPPED;
	}
	// if a connection is already in place but a parameter was changed then disconnect from the old connection an re-connect againth new parameter
	if (session!=NULL && sftpsession1 && parameterChanged)
	{
		if (sftp_session!=NULL)
		{
			if(libssh2_sftp_shutdown(sftp_session)==0)
			{
				AES_CDH_TRACE_MESSAGE("libssh2_sftp_shutdown SUCCESS");
			}
			sftp_session =NULL;
		}
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);
		sock=-1;
		session=NULL;
	}
	else //if the parameters weren't changed and sshClient is connected not execute any connection
	{
		if (!parameterChanged && session!=NULL && sftpsession1)
		{
			AES_CDH_TRACE_MESSAGE("Session is Connected");
			libssh2_session_set_timeout(session, 10000);
			libssh2Mx.acquire();
			sftp_session = libssh2_sftp_init(session);
			libssh2Mx.release();
			//              int errNo = libssh2_session_last_errno(session);
			if (!sftp_session)
			{
				sftpcheck = false;
				AES_CDH_LOG(LOG_LEVEL_WARN, "sftp old session not-active");
				if (session != NULL)                                         //HV45803
				{
					libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing"); // HW46965
					libssh2_session_free(session);
					session=NULL;
				}
				close(sock);
				sock=-1;
			}
			else
			{
				AES_CDH_LOG(LOG_LEVEL_WARN, "sftp old session active");
				sftpcheck = true;
				SFTPRESULT= SFTPAPI_CONNECTED;
				return AES_CDH_RC_CONNECTOK;
			}
		}
	}

	//For all other cases we have to create a new connection and authenticate the user
	sftpsession1= false;
	parameterChanged = false;
	
	sftpcheck = false;    //CNI_690

	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "SFTP is not connected as stop transmission signal");
		AES_CDH_TRACE_MESSAGE("SFTP is not connected as stop transmission signal");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return AES_CDH_RC_TASKSTOPPED;
	}
	AES_CDH_TRACE_MESSAGE("IPaddress:%s, Username:%s, strPassword:%s ",strIpAddress.c_str(),strUser.c_str(), strPassword.c_str());
	struct sockaddr* server = NULL;
	bool isIPv6 = AES_CDH_Common_Util::isIPv6Address(strIpAddress);
	size_t addr_size;
	if(isIPv6)
	{
		AES_CDH_TRACE_MESSAGE("Creating IPv6 SOCKET ADDRESS STRUCT");
		struct in6_addr ipv6_addr;
		int res = inet_pton(AF_INET6,strIpAddress.c_str(),&ipv6_addr);
		if(res != 1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "inet_pton() FAILED(res == %d)! Converting IPv6[%s] string to struct in6_addr FAILED!!!", res, strIpAddress.c_str());
			if(server)
				delete server;
			SFTPRESULT=SFTPAPI_NOT_CONNECTED;
			return AES_CDH_RC_CONNECTERR;
		}
		struct sockaddr_in6* server_ipv6 = new struct sockaddr_in6;
		ACE_OS::memset(server_ipv6,'0',sizeof(struct sockaddr_in6));
		server_ipv6->sin6_family = AF_INET6;
		server_ipv6->sin6_addr = ipv6_addr;
		server_ipv6->sin6_port = htons(nPort);
		server = (struct sockaddr*)server_ipv6;
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Creating IPv4 SOCKET ADDRESS STRUCT");
		struct in_addr ipv4_addr;
		int res = inet_pton(AF_INET,strIpAddress.c_str(),&ipv4_addr);
		if(res != 1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "inet_pton() FAILED(res == %d)! Converting IPv4[%s] string to struct in_addr FAILED!!!", res, strIpAddress.c_str());
			if(server)
				delete server;
			SFTPRESULT=SFTPAPI_NOT_CONNECTED;
			return AES_CDH_RC_CONNECTERR;
		}
		struct sockaddr_in* server_ipv4 = new struct sockaddr_in;
		ACE_OS::memset(server_ipv4,'0',sizeof(struct sockaddr_in));
		server_ipv4->sin_family = AF_INET;
		server_ipv4->sin_addr = ipv4_addr;
		server_ipv4->sin_port = htons(nPort);
		server = (struct sockaddr*)server_ipv4;
	}

	if(isIPv6)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Creating IPv6 SOCKET ");
		sock = socket(AF_INET6, SOCK_STREAM, 0);
		addr_size = sizeof(struct sockaddr_in6);
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "Creating IPv4 SOCKET ");
		sock = socket(AF_INET, SOCK_STREAM, 0);
		addr_size = sizeof(struct sockaddr_in);
	}

	if (sock == -1)
	{
		int lastError = ACE_OS::last_error();
		AES_CDH_LOG(LOG_LEVEL_WARN, "returned INVALID_SOCKET lastError == %d", lastError);
		AES_CDH_TRACE_MESSAGE("returned INVALID_SOCKET lastError == %d", lastError);
		SFTPRESULT=SFTPAPI_NOT_CONNECTED;
		if(server)
			delete server;
		return AES_CDH_RC_CONNECTERR;
	}

	int result;

	// Getting the socket properties before setting the socket non blocking
	int arg1 = fcntl( sock, F_GETFL, NULL );

	// Setting the socket non blocking
	result = fcntl( sock, F_SETFL, O_NONBLOCK);

	// Connecting to the remote ip address
	result = connect(sock, server,addr_size);

	if(result == 0)
	{
		int lastError = ACE_OS::last_error();
		AES_CDH_LOG(LOG_LEVEL_WARN, "connect() - result == 0 & lastError == %d", lastError);
		AES_CDH_TRACE_MESSAGE("lastError == %d", lastError);
	}
	if (result != 0)
	{
		AES_CDH_LOG(LOG_LEVEL_INFO, "connect() - result != 0");
		fd_set writefdSet;
		fd_set readfdSet;
		FD_ZERO( &writefdSet);
		FD_ZERO( &readfdSet);
		FD_SET( sock, &writefdSet );
		FD_SET(ServR::pipeHandles[0],&readfdSet);
		int max_fd;

		if(sock > ServR::pipeHandles[0])
		{
			max_fd = sock;
		}	
		else
		{
			max_fd = ServR::pipeHandles[0];
		}
		ACE_Time_Value tv(5,0);
		// Waiting for 5 secs for reply
		int result1 = ACE_OS::select( max_fd+1, &readfdSet , &writefdSet, 0 ,&tv);
		if( result1 <=  0 )	//TIMEOUT 
		{
			int lastError = ACE_OS::last_error();
			AES_CDH_LOG(LOG_LEVEL_WARN, "Failed to connect socket... returned & lastError:: == %d", lastError);
			AES_CDH_TRACE_MESSAGE("failed to connect socket... returned & lastError:: == %d", lastError);
			close(sock);
			sock = -1;
			SFTPRESULT=SFTPAPI_NOT_CONNECTED;
			if(server)
				delete server;
			return AES_CDH_RC_CONNECTERR;
		}
		else if( result1 == 1 )
		{
			AES_CDH_TRACE_MESSAGE("connect() - result1 ==  1 ");
			if(FD_ISSET(sock, &writefdSet))
			{
				int so_error;
				socklen_t len = sizeof so_error;

				getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

				if (so_error == 0)
				{
					AES_CDH_TRACE_MESSAGE("Connection established successfully");
					// Setting the sock of blocking type
					fcntl( sock, F_SETFL, arg1);
				}
			}
			else if(FD_ISSET(ServR::pipeHandles[0],&readfdSet))
			{
				// Stop event signaled.
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "Stop signalled. Exting Check connection!! ");
				AES_CDH_TRACE_MESSAGE("Stop signalled. Exting Check connection!! ");
				close(sock);
				sock = -1;
				SFTPRESULT=SFTPAPI_NOT_CONNECTED;
				if(server)
					delete server;
				return AES_CDH_RC_CONNECTERR;
			}
		}
	}

	/* Create a session instance */
	session = libssh2_session_init();
	if(!session)
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Failed to create session - returning SFTPAPI_NOT_CONNECTED");
		AES_CDH_TRACE_MESSAGE("failed to create session");
		close(sock);
		sock = -1;
		SFTPRESULT=SFTPAPI_NOT_CONNECTED;
		if(server)
			delete server;
		return AES_CDH_RC_CONNECTERR; // To be changed to an appropriate one -Madhavi
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("libssh2_session_init successful");
	}
	//HU28269 -Start
	libssh2_session_set_timeout(session, 10000); //changing 5000 to 10000
	//HU28269 End
	AES_CDH_TRACE_MESSAGE("the current session timeout is %lu msecs",libssh2_session_get_timeout(session));
	int rc = 0;
	rc = libssh2_session_handshake(session, sock);
	if( rc != 0)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "libssh2_session_handshake unsuccessful, error code = %d",libssh2_session_last_errno(session));
		AES_CDH_TRACE_MESSAGE("libssh2_session_handshake unsuccessful, error code = %d",libssh2_session_last_errno(session));
		char *err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		AES_CDH_LOG(LOG_LEVEL_ERROR, "libssh2_session_handshake unsuccessful, error msg = %s",err_msg);
		AES_CDH_TRACE_MESSAGE("libssh2_session_handshake unsuccessful, error msg = %s",err_msg);
		libssh2_session_disconnect(session, "Normal Shutdown, Thank you for playing");
		libssh2_session_free(session);
		close(sock);
		sock = -1;
		session=NULL;
		SFTPRESULT=SFTPAPI_NOT_CONNECTED;
		if(server)
			delete server;
		return AES_CDH_RC_CONNECTERR;
	}
	AES_CDH_TRACE_MESSAGE("libssh2_session_handshake successful");
	libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);
	
	//TR HV43041 start
        libssh2_session_set_blocking(session, 0);
        /* We could authenticate via password */
        int rc1 = LIBSSH2_ERROR_EAGAIN;
        int count=0;
        while (rc1 == LIBSSH2_ERROR_EAGAIN)
        {
                count++;
                if(ServR::isStopEventSignalled == true || count==200)
                {
			if(ServR::isStopEventSignalled == true)
				AES_CDH_LOG(LOG_LEVEL_WARN,"Shutdown signal");
			else
				AES_CDH_LOG(LOG_LEVEL_WARN,"Timeout error");
                        AES_CDH_LOG(LOG_LEVEL_WARN,"Authentication by password failed with err %d",rc1);
			char *err_msg;
                	libssh2_session_last_error(session, &err_msg, NULL, 0);
                	AES_CDH_LOG(LOG_LEVEL_ERROR, "Authentication by password failed with  error msg = %s",err_msg);
                        libssh2_session_disconnect(session, "Normal Shutdown");
                        libssh2_session_free(session);
                        close(sock);
                        sock = -1;
                        session=NULL;
                		if(server)
                			delete server;
                        SFTPRESULT=SFTPAPI_AUTH_FAILED;
                        return AES_CDH_RC_LOGONFAILURE;
                }


                rc1 = libssh2_userauth_password(session,strUser.c_str(), strPassword.c_str());
		if(rc1 == LIBSSH2_ERROR_EAGAIN)
		{
			ACE_Time_Value tv(0,300*1000);
 			ACE_OS::sleep(tv);
		}
        }
        libssh2_session_set_blocking(session, 1);
	//TR HV43041 end

	/* We could authenticate via password */
	if (rc1)
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Authentication by password failed with err %d",libssh2_session_last_errno(session));
		AES_CDH_TRACE_MESSAGE("Authentication by password failed with err %d",libssh2_session_last_errno(session));
		char *err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Authentication by password failed with  error msg = %s",err_msg);
		AES_CDH_TRACE_MESSAGE("Authentication by password failed with  error msg = %s",err_msg);
		libssh2_session_disconnect(session, "Normal Shutdown");
		libssh2_session_free(session);
		close(sock);
		sock = -1;
		session=NULL;
		if(server)
			delete server;
		SFTPRESULT=SFTPAPI_AUTH_FAILED;
		return AES_CDH_RC_LOGONFAILURE;
	}
	sftpsession1= true;
	AES_CDH_TRACE_MESSAGE("Session creation successful.");
	SFTPRESULT=SFTPAPI_CONNECTED;
	if(server)
		delete server;
	return AES_CDH_RC_CONNECTOK;
}
/*===================================================================
   ROUTINE:sftpConnect
=================================================================== */
/*sftpConnect create a sftp client and connect it to SFTP server */
AES_CDH_ResultCode SftpAdapterClass::sftpConnect(void *psshftpiv2, const char * hostAddress,
							 int portNr,
							 const char * userName,
							 const char * passWord,
							 bool ascii)
{
	AES_CDH_TRACE_MESSAGE("Entering...");
	AES_CDH_ResultCode result = sftpInitiate(psshftpiv2, hostAddress,
			portNr,
			userName,
			passWord,
			ascii);
	if ( result != AES_CDH_RC_CONNECTOK)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to init session");
		AES_CDH_TRACE_MESSAGE("Unable to init session");
		return result;
	}
	if (sftpcheck == false)					//HV45803
	{
	// HU28269 - Start
		libssh2_session_set_timeout(session, 10000); // Implemented to avoid indefinite hangs in libssh2 blocking functions. changing 5000 to 10000
	//HU28269 - End
		libssh2Mx.acquire();
		sftp_session = libssh2_sftp_init(session);
		libssh2Mx.release();
	}
	int errNo = libssh2_session_last_errno(session);
	if (!sftp_session)
	{
		if( errNo == LIBSSH2_ERROR_TIMEOUT)
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, "Unable to init SFTP session as LIBSSH2_ERROR_TIMEOUT");
		}
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to init SFTP session with err %d ",libssh2_session_last_errno(session));
		AES_CDH_TRACE_MESSAGE("Unable to init SFTP session with err %d ",libssh2_session_last_errno(session));
		char *err_msg;
		libssh2_session_last_error(session, &err_msg, NULL, 0);
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Unable to init SFTP session with  error msg = %s",err_msg);
		parameterChanged=true;
		SFTPRESULT=SFTPAPI_CONNECTION_LOST;
		return AES_CDH_RC_CONNECTERR;
	}
	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_DEBUG, "SFTP is not connected as stop transmission signal");
		AES_CDH_TRACE_MESSAGE("SFTP is not connected as stop transmission signal");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return AES_CDH_RC_TASKSTOPPED;
	}
	AES_CDH_TRACE_MESSAGE("Able to establish  SFTP session");
	return AES_CDH_RC_CONNECTOK;
}

/*===================================================================
   ROUTINE:sftpFinalize
=================================================================== */
/*sftpFinalize disconnects a ssh client from SSH server*/

bool SftpAdapterClass::sftpFinalize()
{
	AES_CDH_TRACE_MESSAGE("Entering...");
	AES_CDH_LOG(LOG_LEVEL_INFO, "disconnects a ssh client from SSH server");
	if (sftp_session != NULL)
	{
		if(libssh2_sftp_shutdown(sftp_session)==0)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "libssh2_sftp_shutdown");
			AES_CDH_TRACE_MESSAGE("libssh2_sftp_shutdown");
		}
		sftp_session =NULL;
	}
	if (session != NULL)
	{
		libssh2_session_free(session);
		session=NULL;
	}
	close(sock);
	sock=-1;
	sftpsession1 = false;
	AES_CDH_TRACE_MESSAGE("Returning  with success");
	return true;
}
/*===================================================================
   ROUTINE:sftpDisconnect
=================================================================== */
/* sftpDisconnect disconnectls a sftp client from SFTP server */

bool SftpAdapterClass::sftpDisconnect(bool closeconnection)
{
	AES_CDH_TRACE_MESSAGE(" Entering...");
	AES_CDH_TRACE_MESSAGE("disconnects a sftp client from SFTP server");
	if (sftp_session != NULL)
	{
		if(libssh2_sftp_shutdown(sftp_session)==0)
		{
			AES_CDH_TRACE_MESSAGE("libssh2_sftp_shutdown");
		}
		sftp_session = NULL;
	}
	
	if ((closeconnection) && (session != NULL)) //HY73707
	{
		AES_CDH_TRACE_MESSAGE("disconnected ssh session");
		libssh2_session_disconnect(session, "Normal Shutdown");
		libssh2_session_free(session);
		if (sock != -1)
		{
			close(sock);
			sock = -1;
		}
		session = NULL;
	}
        AES_CDH_TRACE_MESSAGE("SFTP Disconnected");			     
 	
	return true;
}
/*===================================================================
   ROUTINE:sftpCreateDirectories
=================================================================== */
/*Creates remote folders on sftp server. Creates only one directory. If more than one directory needs to be created,
* path shuld be splitted and given  (i.e path = test/test1/test2, till test1, path should be there, then only test2 will be created)
*/
bool SftpAdapterClass::sftpCreateDirectories(string path)
{
	AES_CDH_TRACE_MESSAGE("Need to create directory:%s",path.c_str());
	AES_CDH_LOG(LOG_LEVEL_INFO, "Creates remote folder on sftp server %s",path.c_str());
	if (sftp_session)
	{

		int	rc = libssh2_sftp_mkdir(sftp_session, path.c_str(),
				LIBSSH2_SFTP_S_IRWXU|
				LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IXGRP|
				LIBSSH2_SFTP_S_IROTH|LIBSSH2_SFTP_S_IXOTH);
		if(rc)
		{
			if(rc == LIBSSH2_ERROR_TIMEOUT)
			{
				AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_mkdir failed as LIBSSH2_ERROR_TIMEOUT");
				AES_CDH_TRACE_MESSAGE("libssh2_sftp_mkdir failed as LIBSSH2_ERROR_TIMEOUT");
			}
			AES_CDH_LOG(LOG_LEVEL_WARN, "Directory creation failed,SFTP session active with %lu",libssh2_sftp_last_error(sftp_session));
			AES_CDH_TRACE_MESSAGE("Directory creation failed,SFTP session active with %lu",libssh2_sftp_last_error(sftp_session));
			return false;
		}

		AES_CDH_TRACE_MESSAGE("Directory creation success");
		return true;
	}
	AES_CDH_TRACE_MESSAGE("Directory creation failed, SFTP session not active ");
	AES_CDH_LOG(LOG_LEVEL_ERROR, "Directory creation failed, SFTP session not active, path : %s",path.c_str());
	return false;
}

/*===================================================================
   ROUTINE:~SftpAdapterClass
=================================================================== */
SftpAdapterClass::~SftpAdapterClass()
{
	sftpFinalize();
}

/*===================================================================
   ROUTINE:renameRemoteFile
=================================================================== */
bool SftpAdapterClass::renameRemoteFile(string currentName, string newName)
{
	AES_CDH_TRACE_MESSAGE("CurrentName:%s, newName:%s ",currentName.c_str(), newName.c_str());

	if(sftp_session)
	{
		AES_CDH_TRACE_MESSAGE("SFTP session is active");
		AES_CDH_TRACE_MESSAGE("active sftp_session");
		int rc = libssh2_sftp_rename(sftp_session, currentName.c_str(), newName.c_str());
		if(rc == 0)
		{
			AES_CDH_TRACE_MESSAGE("Rename success");
			SFTPRESULT=SFTPAPI_OK;
			return true;
		}
		else
		{
			if(rc == LIBSSH2_ERROR_TIMEOUT)
                        {
                                AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_rename failed as LIBSSH2_ERROR_TIMEOUT");
                                if(libssh2_sftp_last_error(sftp_session) == 0)  // HW61869 Start
                                {
                                        if(checkRemoteFile(newName))
                                        {
                                                SFTPRESULT=SFTPAPI_OK;
                                                return true;
                                        }
                                }                                               // HW61869 End
                        }
			AES_CDH_LOG(LOG_LEVEL_WARN, "rename Failure due to API failure with %lu",libssh2_sftp_last_error(sftp_session));
			AES_CDH_TRACE_MESSAGE("rename Failure due to API failure with %lu",libssh2_sftp_last_error(sftp_session));
			SFTPRESULT=SFTPAPI_FAILURE;
			return false;
		}
	}

	AES_CDH_TRACE_MESSAGE("Rename Failure, no sftp session ");
	SFTPRESULT=SFTPAPI_FAILURE;
	return false;
}
/*===================================================================
   ROUTINE:renameRemoteDir
=================================================================== */
// It renames a remote folder.
bool SftpAdapterClass::renameRemoteDir(string currentName, string newName)
{
	AES_CDH_TRACE_MESSAGE("Entering...");
	return renameRemoteFile(currentName,newName);
}
/*===================================================================
   ROUTINE:sftpDeleteFile
=================================================================== */
/*sftpDeleteFile allows to delete file. Full path of the file needs to be given as input */
bool SftpAdapterClass::sftpDeleteFile(string fileName)
{
	if (sftp_session)
	{
		int rc = libssh2_sftp_unlink(sftp_session, fileName.c_str());
		if (rc == 0)
		{
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "success..");
			return true;
		}
		else if(rc == LIBSSH2_ERROR_TIMEOUT)
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_stat failed as LIBSSH2_ERROR_TIMEOUT with err = %lu",libssh2_sftp_last_error(sftp_session));
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, " libssh2_sftp_stat failed with err = %lu",libssh2_sftp_last_error(sftp_session));
		}	
	}
	AES_CDH_LOG(LOG_LEVEL_WARN, "failure..");
	return false;

}
/*===================================================================
   ROUTINE:isTransmissionInterrupted
=================================================================== */
// Returns true if the transmission has been interrupted by afptfte command
bool SftpAdapterClass::isTransmissionInterrupted(void *psshftpiv2)
{
	SSHFTPIV2 *pSshftpiv2 = (SSHFTPIV2 *)psshftpiv2;
	string destSetName = pSshftpiv2->destinationPtr->getDestSetName();
	bool bStopFileTransfer = ServR::checkStopFlag(destSetName,
			pSshftpiv2->aFileName,
			pSshftpiv2->aNewMainFileName,
			pSshftpiv2->aNewSubFileName);
	return bStopFileTransfer;
}
/*===================================================================
   ROUTINE:checkRemoteFile
=================================================================== */
// It checks if exists the filename on the remote server
bool SftpAdapterClass::checkRemoteFile(string fileName)
{
	AES_CDH_TRACE_MESSAGE("to be check: %s",fileName.c_str());
	LIBSSH2_SFTP_ATTRIBUTES attrs;
	ACE_OS::memset(&attrs,'\0',sizeof(attrs));

	if(sftp_session)
	{
		AES_CDH_TRACE_MESSAGE("before calling libssh2_sftp_stat");
		int rc=libssh2_sftp_stat(sftp_session, fileName.c_str(), &attrs);
		if(rc==0)
		{
			AES_CDH_TRACE_MESSAGE("Given file/Directory found");
			return true;
		}
		else if(rc == LIBSSH2_ERROR_TIMEOUT)
		{
			AES_CDH_TRACE_MESSAGE("libssh2_sftp_stat failed as LIBSSH2_ERROR_TIMEOUT with error %lu", libssh2_sftp_last_error(sftp_session));
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("libssh2_sftp_stat  with err = %lu",libssh2_sftp_last_error(sftp_session));
		}
	}
	AES_CDH_TRACE_MESSAGE("Given file/Directory not found");
	return false;

}
/*===================================================================
   ROUTINE:isRemoteFile
=================================================================== */
bool SftpAdapterClass::isRemoteFile(string fileName)
{
	LIBSSH2_SFTP_ATTRIBUTES attrs;
	ACE_OS::memset(&attrs,'\0',sizeof(attrs));

	if (sftp_session)
	{
		int rc=libssh2_sftp_stat(sftp_session, fileName.c_str(), &attrs);
		if (rc==0 && attrs.flags&LIBSSH2_SFTP_ATTR_PERMISSIONS && (LIBSSH2_SFTP_S_ISREG(attrs.permissions)||LIBSSH2_SFTP_S_ISCHR (attrs.permissions)||LIBSSH2_SFTP_S_ISBLK
				(attrs.permissions)||LIBSSH2_SFTP_S_ISFIFO (attrs.permissions)) )
		{
			AES_CDH_TRACE_MESSAGE( "%s Given file found",fileName.c_str());
			return true;
		}
		else if(rc == LIBSSH2_ERROR_TIMEOUT)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "libssh2_sftp_stat failed as LIBSSH2_ERROR_TIMEOUT %lu",libssh2_sftp_last_error(sftp_session));
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "libssh2_sftp_stat  with err = %lu",libssh2_sftp_last_error(sftp_session));
		}
	}
	AES_CDH_LOG(LOG_LEVEL_TRACE, "%s Given file not found ", fileName.c_str());
	return false;

}
/*===================================================================
   ROUTINE:isRemoteDir
=================================================================== */
bool SftpAdapterClass::isRemoteDir(string dirName)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	LIBSSH2_SFTP_ATTRIBUTES attrs;
	ACE_OS::memset(&attrs,'\0',sizeof(LIBSSH2_SFTP_ATTRIBUTES));

	if (sftp_session)
	{
		int rc=libssh2_sftp_stat(sftp_session, dirName.c_str(), &attrs);


		if (rc==0 && attrs.flags&LIBSSH2_SFTP_ATTR_PERMISSIONS && LIBSSH2_SFTP_S_ISDIR(attrs.permissions))
		{
			AES_CDH_TRACE_MESSAGE("%s Given dir found", dirName.c_str());
			return true;
		}
		else if(rc == LIBSSH2_ERROR_TIMEOUT)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "libssh2_sftp_stat failed as LIBSSH2_ERROR_TIMEOUT with %lu ",libssh2_sftp_last_error(sftp_session));
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "libssh2_sftp_stat failed  with err = %lu",libssh2_sftp_last_error(sftp_session));
		}
	}
	AES_CDH_LOG(LOG_LEVEL_TRACE, "%s Given dir not found",dirName.c_str());
	return false;

}
/*===================================================================
   ROUTINE:sftpSendFile
=================================================================== */
bool SftpAdapterClass::sftpSendFile(void *psshftpiv2, string fileName, string remoteFileName, string remoteDirName, string remoteSubdirName, bool overwrite,bool ascii)
{
	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Sendfile failed due to breaktransmission");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return false;
	}
	AES_CDH_TRACE_MESSAGE( "Entered , File to be stransfered=%s  & Remote file name= %s",fileName.c_str(),remoteFileName.c_str());
	AES_CDH_TRACE_MESSAGE( "remoteDirName=%s , remoteSubdirName=%s",remoteDirName.c_str(), remoteSubdirName.c_str());


	FILE * fs = fopen(fileName.c_str(),"r" );
	if(!fs)
	{
		int lastError = ACE_OS::last_error();
		AES_CDH_LOG(LOG_LEVEL_WARN, " Unable to open origin file with lastError == %d", lastError);
		SFTPRESULT=SFTPAPI_NO_SUCH_FILE;
		return false;
	}
	fclose(fs);

	string sftpdirpath("");
	string sftpfilepath("");
	string temp_dirpath1("");
	string temp_dirpath2("");
	if(remoteDirName!="")
	{
		if(checkRemoteFile(remoteDirName)==false)
		{
			//logic for creating the given path
			size_t pos1 = remoteDirName.find_first_of("/");
			size_t pos2 = remoteDirName.find_first_of("\\");
			size_t pos3=0;
			int count =0;
			while((pos1!=string::npos)||(pos2!=string::npos ))
			{
				if((pos1!=string::npos))
				{
					pos3=pos3+pos1;
				}
				else if((pos2!=string::npos ))
				{
					pos3=pos3+pos2;
				}
				if(pos3==0)
				{
					temp_dirpath2=remoteDirName.substr(0,(strlen(remoteDirName.c_str())));
					pos1 = temp_dirpath2.find_first_of("/",1);
					pos2 = temp_dirpath2.find_first_of("\\",1);
				}
				else
				{
					if(count == 0)
					{
						temp_dirpath1 = remoteDirName.substr(0,pos3);
						count=1;
					}
					else
					{
						temp_dirpath1 = remoteDirName.substr(0,pos3+1);
						pos3=pos3+1;
					}

					if(!(createDirectories(temp_dirpath1)))
					{
						SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
						AES_CDH_LOG(LOG_LEVEL_ERROR, "createDirectories failed for %s",temp_dirpath1.c_str());
						return false;
					}
					temp_dirpath2=remoteDirName.substr(pos3+1,(strlen(remoteDirName.c_str()))-1);
					pos1 = temp_dirpath2.find_first_of("/");
					pos2 = temp_dirpath2.find_first_of("\\");

				}
			}
			if(!(createDirectories(remoteDirName)))
			{
				SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
				AES_CDH_LOG(LOG_LEVEL_ERROR, "createDirectories failed for %s",remoteDirName.c_str());
				return false;
			}
			sftpdirpath=remoteDirName;
		}
		else
		{
			sftpdirpath=remoteDirName;
		}
	}
	if(checkRemoteFile(remoteDirName)==false)
	{
		if(!(createDirectories(remoteDirName)))
		{
			AES_CDH_LOG(LOG_LEVEL_DEBUG, "createDirectories failed for %s",remoteDirName.c_str());
			SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
			return false;
		}
		AES_CDH_TRACE_MESSAGE( "createDirectories success for %s",remoteDirName.c_str());
	} 
	if(remoteSubdirName!="")
	{
		if (sftpdirpath=="")
		{
			sftpdirpath= remoteSubdirName;
		}
		else
		{
			sftpdirpath= sftpdirpath +"/"+ remoteSubdirName;
		}
		if(!(checkRemoteFile(sftpdirpath)))
		{
			if (!createDirectories(sftpdirpath))
			{
				SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
				AES_CDH_LOG(LOG_LEVEL_DEBUG, "createDirectories failed for %s",sftpdirpath.c_str());
				return false;
			}
		}

	}
	if ( sftpdirpath != "")
	{
		sftpfilepath=sftpdirpath +"/"+ remoteFileName;
	}
	else
	{
		sftpfilepath = remoteFileName;
	}
	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Sendfile failed due to breaktransmission");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return false;
	}
	string tmpFile = sftpfilepath + ".tmp";
	AES_CDH_TRACE_MESSAGE( "created dirs:%s & %s",remoteDirName.c_str(),sftpdirpath.c_str());
	AES_CDH_TRACE_MESSAGE( "sftpfilepath:%s & temfilepath: %s",sftpfilepath.c_str(),tmpFile.c_str());
	// check if the tmp file exists on remote server
	bool found = checkRemoteFile(tmpFile);
	if (found)
	{
		AES_CDH_TRACE_MESSAGE("temp: %s file found",tmpFile.c_str());
		// if tmp file exist remove it before start transmission
		if (!sftpDeleteFile(tmpFile))
		{
			AES_CDH_LOG(LOG_LEVEL_WARN, "%s file deletion failed",tmpFile.c_str());
			SFTPRESULT=SFTPAPI_FAILURE;
			return false;
		}
	}
	// if overwrite remove filename from remoteserver
	bool isFound = checkRemoteFile(sftpfilepath);

	// Either the overwrite option (-o option in cdhdef command)
	// is enabled or is not enabled but the file is not found on remote server,
	// then try to send the file to the remote server
	if (overwrite)
	{
		AES_CDH_TRACE_MESSAGE("overwrite is true");
		if (isFound)
		{
			// if file exist remove it before start transmission
			if (!sftpDeleteFile(sftpfilepath))
			{
				SFTPRESULT=SFTPAPI_FAILURE;
				return false;
			}
		}
		// Let's send the file directly to the remote server
		if (sendFile(psshftpiv2, fileName,tmpFile, ascii))//HK53278
		{
			//if transfer is ok, rename the file deleting .tmp
			AES_CDH_TRACE_MESSAGE("Returned true from sendfile and going to rename temp file");
			AES_CDH_TRACE_MESSAGE("temp file:%s , original filename:%s", tmpFile.c_str(),sftpfilepath.c_str());
			return renameRemoteFile(tmpFile, sftpfilepath);
		}
		SFTPRESULT=SFTPAPI_FAILURE;
		return false;
	}
	else 			// if the file mustn't be overwritten and is found on remote server then try to rename the file using a timestamp as extension
	{
		if (isFound)
		{
			return renameAndSendFileWithRetry(psshftpiv2, fileName,sftpfilepath,MAX_RETRY, ascii);
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_TRACE, "Going to sendfile:%s",tmpFile.c_str());

			// If remote server not found let's send the file directly to the remote server
			if (sendFile(psshftpiv2, fileName,tmpFile,ascii))//HK53278
			{
				//if transfer is ok, rename the file deleting .tmp

				AES_CDH_TRACE_MESSAGE("Returned true from sendfile and going to rename temp file");
				AES_CDH_TRACE_MESSAGE("temp file:%s , original filename:%s", tmpFile.c_str(),sftpfilepath.c_str());
				return renameRemoteFile(tmpFile, sftpfilepath);
			}
		}
		SFTPRESULT=SFTPAPI_FAILURE;
		return false;
	}


	AES_CDH_TRACE_MESSAGE("coming out of sFTPsend file:error");
	SFTPRESULT=SFTPAPI_FAILURE;
	return false;
}
/*===================================================================
   ROUTINE:sendFile
=================================================================== */
bool SftpAdapterClass::sendFile(void *psshftpiv2, string localFileName, string remoteFileName,bool ascii)
{
	AES_CDH_TRACE_MESSAGE("Entering...");
	struct stat stat_buff;
	if(stat(localFileName.c_str(),&stat_buff)==-1)
	{

		AES_CDH_LOG(LOG_LEVEL_WARN, "unable to open the origin file");
		return false;
	}
	LIBSSH2_SFTP_ATTRIBUTES attrs;
	init_attrs(&attrs, &stat_buff);
	if(S_ISDIR(stat_buff.st_mode))
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Not a file, Directory found ");
		return false;
	}
	else
	{
		if ((stat_buff.st_size==0)&& (sftp_session)&&(!isTransmissionInterrupted(psshftpiv2)))
		{
			LIBSSH2_SFTP_HANDLE * sftphand = libssh2_sftp_open_ex(sftp_session, remoteFileName.c_str(),strlen(remoteFileName.c_str()),
					LIBSSH2_FXF_CREAT,attrs.permissions&0777,LIBSSH2_SFTP_OPENFILE);
			/*LIBSSH2_FXF_CREAT,LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
				 LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH,LIBSSH2_SFTP_OPENFILE );*/
			int errNo = libssh2_session_last_errno(session);
			if(sftphand==NULL)
			{
				if( errNo == LIBSSH2_ERROR_TIMEOUT)
				{
					AES_CDH_LOG(LOG_LEVEL_WARN, "unable to open file in remote system as LIBSSH2_ERROR_TIMEOUT");
				}
				AES_CDH_LOG(LOG_LEVEL_WARN, " Failed to open new file:%s with %ld",remoteFileName.c_str(),libssh2_sftp_last_error(sftp_session));
				return false;
			}
			int rc = libssh2_sftp_close_handle(sftphand);
			if( rc < 0 )
			{
				if( rc  == LIBSSH2_ERROR_TIMEOUT)
				{
					AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_close_handle failed as LIBSSH2_ERROR_TIMEOUT for %s with err %lu", remoteFileName.c_str(),libssh2_sftp_last_error(sftp_session));
				}
				AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_close_handle failed as with err no %lu",libssh2_sftp_last_error(sftp_session));

			}
			//libssh2_sftp_setstat(sftp_session, remoteFileName.c_str(), &attrs);  //Commented as part of TR HR72797
			return true;
		}
		else
		{
			bool isFileSent = sendFile1(psshftpiv2, localFileName, remoteFileName,ascii);
			AES_CDH_TRACE_MESSAGE("Returned from sendfile1. Status of return is: %d ",isFileSent);
			return isFileSent;
		}
	}

}

/*===================================================================
   ROUTINE:sendFile1
=================================================================== */
bool SftpAdapterClass::sendFile1(void *psshftpiv2, string  stream, string remoteFileName,bool ascii)
{
	AES_CDH_TRACE_MESSAGE( "********** RemoteFileName: %s ############ ", remoteFileName.c_str());
	(void)ascii;
	char mem[1024 * 100]= { 0 };
	//char mem[1024 * 500];//failing nread=51200
	//char mem[1024 * 32]; //transfering

	char *ptr = 0;

	LIBSSH2_SFTP_HANDLE *sftp_handle1;
	struct stat stat_buff;
	if(stat(stream.c_str(),&stat_buff)==-1)
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "In unable to open the origin file");
		return false;
	}
	LIBSSH2_SFTP_ATTRIBUTES attrs;
	init_attrs(&attrs, &stat_buff);

	// set breakTransmission to false;
	if((sftp_session)&&(!isTransmissionInterrupted(psshftpiv2)))
	{
		AES_CDH_TRACE_MESSAGE("in sftp_session");

		sftp_handle1 = libssh2_sftp_open(sftp_session, remoteFileName.c_str(),LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,attrs.permissions&0777);

		AES_CDH_TRACE_MESSAGE("file opened");

		int errNo = libssh2_session_last_errno(session);


		AES_CDH_TRACE_MESSAGE("file opened with last error %lu", libssh2_sftp_last_error(sftp_session));

		if (!sftp_handle1)
		{
			if( errNo == LIBSSH2_ERROR_TIMEOUT)
			{
				AES_CDH_LOG(LOG_LEVEL_WARN, "In unable to open file in remote system as LIBSSH2_ERROR_TIMEOUT %s", remoteFileName.c_str());
			}
			AES_CDH_LOG(LOG_LEVEL_WARN, "In unable to open file in remote system with %lu",libssh2_sftp_last_error(sftp_session));
			return false;
		}
		FILE * fs = fopen(stream.c_str(),"r" );
		if( fs == 0 )
		{

			AES_CDH_LOG(LOG_LEVEL_WARN, "In unable to open file in local system %d", ACE_OS::last_error());
			return false;
		}

		AES_CDH_TRACE_MESSAGE("opened local file in ");
		int nread=0;
		int rc=0;
		int rc1=0;
		do {
			AES_CDH_TRACE_MESSAGE("Into 1st do while");
			nread = fread(mem, 1, sizeof(mem), fs);
			if (nread <= 0)
			{
				/* end of file */
				AES_CDH_TRACE_MESSAGE("Reached to end of file.. ");
				AES_CDH_TRACE_MESSAGE("In Reached to end of file.. ");
				break;
			}
			ptr = mem;
			AES_CDH_TRACE_MESSAGE("ptr value assigned");
			do {
				AES_CDH_TRACE_MESSAGE( "Into 2nd do while:: nread=%d, rc=%d  for file %s",nread,rc,remoteFileName.c_str());

				/* write data in a loop until we block */
				rc = libssh2_sftp_write(sftp_handle1, ptr, nread);
				if(rc < 0)
				{
					switch(rc)
					{
					case LIBSSH2_ERROR_ALLOC :
						AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_write returned with error LIBSSH2_ERROR_ALLOC");
						break;
					case LIBSSH2_ERROR_SOCKET_SEND :
						AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_write returned with error LIBSSH2_ERROR_SOCKET_SEND");
						break;
					case LIBSSH2_ERROR_SOCKET_TIMEOUT :
						AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_write returned with error LIBSSH2_ERROR_SOCKET_TIMEOUT");
						break;
					case LIBSSH2_ERROR_SFTP_PROTOCOL :
						AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_write returned with error LIBSSH2_ERROR_SFTP_PROTOCOL");
						break;
					case LIBSSH2_ERROR_EAGAIN :
						AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_write returned with error LIBSSH2_ERROR_EAGAIN");
						break;
					case LIBSSH2_ERROR_TIMEOUT :
						AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_write returned with error LIBSSH2_ERROR_TIMEOUT");
						break;
					default :
						AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_write returned with error UNKNOWN with %d ",libssh2_session_last_errno(session));
						break;
					}
					AES_CDH_LOG(LOG_LEVEL_WARN, "rc is less than 0 with %lu",libssh2_sftp_last_error(sftp_session));
					break;
				}
				ptr += rc;
				nread -= rc;
				// AES_CDH_LOG(LOG_LEVEL_TRACE, "%s"," nread=%d, rc=%d ",nread,rc));
				// AES_CDH_LOG(LOG_LEVEL_TRACE, "%s", "In sendFile1:: nread=%d, rc=%d ",nread,rc));
			} while ((nread)&&(!isTransmissionInterrupted(psshftpiv2)));
		} while ((rc > 0) &&(!isTransmissionInterrupted(psshftpiv2)));
		fclose(fs);
		AES_CDH_TRACE_MESSAGE("In  before closing the file");

		if (LIBSSH2_ERROR_TIMEOUT == rc)
		{

			//Note:: If write hangs, close would too. Should be fixed with latest libssh library
			AES_CDH_LOG(LOG_LEVEL_WARN, "Avoiding close, libssh2_sftp_write failed with error LIBSSH2_ERROR_TIMEOUT for %s", remoteFileName.c_str());
		}
		else
		{
			rc1 = libssh2_sftp_close_handle(sftp_handle1);
			if( rc1 < 0 )
			{
				if( rc1  == LIBSSH2_ERROR_TIMEOUT)
				{
					AES_CDH_LOG(LOG_LEVEL_WARN, "libssh2_sftp_close_handle failed as LIBSSH2_ERROR_TIMEOUT for %s with err %lu", remoteFileName.c_str(),libssh2_sftp_last_error(sftp_session));
				}
				AES_CDH_LOG(LOG_LEVEL_WARN, " libssh2_sftp_close_handle failed as LIBSSH2_ERROR_TIMEOUT for %s with err %lu",remoteFileName.c_str(),libssh2_sftp_last_error(sftp_session));

			}
			AES_CDH_TRACE_MESSAGE("libssh2_sftp_close_handle DONE ..");
		}

		// if transmission breaked by user using command afpfte then return false;
		if ((rc < 0) || (isTransmissionInterrupted(psshftpiv2)))
		{
			AES_CDH_TRACE_MESSAGE(" TransmissionInterrupted is true");
			return false;
		}
		else
		{
			AES_CDH_TRACE_MESSAGE("transmission of data in file done.. ");
			//libssh2_sftp_setstat(sftp_session, remoteFileName.c_str(), &attrs); //Commented as part of TR HR72797
			return true;
		}
	}
	return false;

}
 /*===================================================================
    ROUTINE:sftpSendDir
 =================================================================== */
 // sftpSendDir sends a directory to remoteServer
bool SftpAdapterClass::sftpSendDir(void *psshftpiv2, string dirname, string newRemoteDirName1, string remoteDirName, string remoteSubdirName1)
{

	AES_CDH_TRACE_MESSAGE("Entering");
	AES_CDH_TRACE_MESSAGE("dirname: %s , remoteDirName:%s, remoteSubdirName:%s, newRemoteDirName:%s",dirname.c_str(),remoteDirName.c_str(),remoteSubdirName1.c_str(),newRemoteDirName1.c_str());
	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Sendfile failed due to breaktransmission");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return false;
	}
	DIR *dir;
	dir = opendir(dirname.c_str());
	if (dir == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "getfilefromdir::not able to open original dir:%s", dirname.c_str());
		AES_CDH_LOG(LOG_LEVEL_WARN, "getfilefromdir::not able to open original dir with err  %d", ACE_OS::last_error());
		SFTPRESULT=SFTPAPI_NOT_VALID_NAME;
		return false;
	}
	if(dir != NULL)
		closedir(dir);
	//***********************************

	int count =0;

	string temp_dirpath1("");
	string temp_dirpath2("");
	string remoteSubdirName("");
	string newRemoteDirName("");
	if(remoteDirName!="")
	{
		if(checkRemoteFile(remoteDirName)==false)
		{
			//logic to create whole path
			size_t pos1 = remoteDirName.find_first_of("/");
			size_t pos2 = remoteDirName.find_first_of("\\");
			size_t pos3=0;
			while((pos1!=string::npos)||(pos2!=string::npos ))
			{

				if((pos1!=string::npos))
				{
					pos3=pos3+pos1;

				}
				else if((pos2!=string::npos ))
				{
					pos3=pos3+pos2;

				}
				if(pos3==0)
				{
					temp_dirpath2=remoteDirName.substr(pos3,(strlen(remoteDirName.c_str())));
					pos1 = temp_dirpath2.find_first_of("/",1);
					pos2 = temp_dirpath2.find_first_of("\\",1);
				}
				else
				{
					if(count==0)
					{
						temp_dirpath1 = remoteDirName.substr(0,pos3);
						count=1;
					}
					else{
						temp_dirpath1 = remoteDirName.substr(0,pos3+1);
						pos3=pos3+1;
					}


					if(!(createDirectories(temp_dirpath1)))
					{
						SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
						return false;
					}
					temp_dirpath2=remoteDirName.substr(pos3+1,(strlen(remoteDirName.c_str()))-1);
					pos1 = temp_dirpath2.find_first_of("/");
					pos2 = temp_dirpath2.find_first_of("\\");

				}
			}
			if(!(createDirectories(remoteDirName)))
			{
				SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
				return false;
			}
			remoteSubdirName=remoteDirName;
		}
		else
		{
			remoteSubdirName=remoteDirName;
		}
	}

	if(remoteSubdirName1!="")
	{
		if(remoteSubdirName=="")
		{
			remoteSubdirName=remoteSubdirName1;
		}
		else
		{
			remoteSubdirName= remoteSubdirName +"/"+ remoteSubdirName1;
		}
		if(!(createDirectories(remoteSubdirName)))
		{
			SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
			return false;
		}
	}


	if ( remoteSubdirName != "")
	{
		newRemoteDirName=remoteSubdirName +"/"+ newRemoteDirName1;
	}
	else
	{
		newRemoteDirName = newRemoteDirName1;
	}
	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Sendfile failed due to breaktransmission");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return false;
	}
	string tmpDirName =  newRemoteDirName + ".tmp";
	// check if remote dir and tmp dir exist on remote server
	bool foundTmp = false;
	bool found = false;
	int maxretry = MAX_RETRY;

	do
	{
		foundTmp = checkRemoteFile(tmpDirName);
		found = checkRemoteFile(newRemoteDirName);
		AES_CDH_LOG(LOG_LEVEL_TRACE, "tmpDirName:%s & newRemoteDirName:%s",tmpDirName.c_str(),newRemoteDirName.c_str());
		if (found || foundTmp)
		{
			newRemoteDirName = addTimestampToFilename(newRemoteDirName);
		}

		tmpDirName = newRemoteDirName + ".tmp";
		maxretry--;
	}while ((found || foundTmp) && (maxretry > 0) && (!isTransmissionInterrupted(psshftpiv2)));

	if (maxretry == 0)
	{
		//return also a code to indicate an error in the directory renaming operation
		SFTPRESULT=SFTPAPI_MAX_RETRY_OVER;
		return false;
	}
	else
	{
		// dir and temp dir not found:procede with creation
		// create the Directory at Level 0 and execute change down
		if (!createDirectories(tmpDirName))
		{
			SFTPRESULT=SFTPAPI_DIR_CREATE_FAIL;
			return false;
		}
		// recursive search in the local directory from Level 1 on and createRemoteDir
		if (!recursiveDirCreation(psshftpiv2, dirname,tmpDirName))
		{
			SFTPRESULT=SFTPAPI_FAILURE;
			return false;
		}

		// if renameDir fails return false otherwise true
		return renameRemoteDir(tmpDirName,newRemoteDirName);

	}

	AES_CDH_LOG(LOG_LEVEL_TRACE, "not able to create directories");
	SFTPRESULT=SFTPAPI_FAILURE;
	return false;
}
 /*===================================================================
    ROUTINE:recursiveDirCreation
 =================================================================== */
bool SftpAdapterClass::recursiveDirCreation(void *psshftpiv2, string dirPath,string newRemoteDirName)
{
	DIR *dir;
	struct dirent * finddata;
	std::string searchPattern = dirPath;// + "/" + "*";
	std::string fileN;
	if(isTransmissionInterrupted(psshftpiv2))
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "Sendfile failed due to breaktransmission");
		AES_CDH_TRACE_MESSAGE("Sendfile failed due to breaktransmission");
		SFTPRESULT=SFTPAPI_BREAK_TRANSM;
		return false;
	}
	dir = opendir(searchPattern.c_str());
	if (dir == NULL)
	{
		AES_CDH_LOG(LOG_LEVEL_WARN, "not able to open file or directory with err %d", ACE_OS::last_error());
		AES_CDH_TRACE_MESSAGE("not able to open file or directory with err %d", ACE_OS::last_error());
		return false;
	}
	while (((finddata = readdir(dir)) != NULL) && (!isTransmissionInterrupted(psshftpiv2)))
	{
		if ((strcmp(finddata->d_name, ".") && strcmp(finddata->d_name, ".."))&& (!isTransmissionInterrupted(psshftpiv2)))

		{
			fileN = dirPath +  "/" + finddata->d_name;
			struct stat stat_buff;
			stat(fileN.c_str(),&stat_buff);
			if(S_ISDIR(stat_buff.st_mode))
			{
				string fileN1("");
				fileN1 = newRemoteDirName +"/"+ finddata->d_name;
				sftpSendDir(psshftpiv2, fileN, finddata->d_name, newRemoteDirName, "");
			}
			else
			{
				if(fileN.c_str()!=NULL)
				{
					fileN = dirPath +"/"+ finddata->d_name;
					string newremotefile=newRemoteDirName+"/"+ finddata->d_name;
					if (!sendFile(psshftpiv2, fileN,newremotefile,0))
					{
						if(dir != NULL)
							closedir(dir);
						return false;
					}
				}
				else{
				}

			}
		}
	}
	if(dir != NULL)
		closedir(dir);
	return true;
}
 /*===================================================================
    ROUTINE:createDirectories
 =================================================================== */
bool SftpAdapterClass::createDirectories(string path)
{
	AES_CDH_TRACE_MESSAGE( "Entering with path:%s",path.c_str());
	bool res = true;
	// if remote dir length > 0 try to create it
	if (strlen(path.c_str())> 0)
	{
		// check if Remote Dir exist
		res = isRemoteDir(path);
		if (!res)
		{
			//case 1: isRemoteDir returns false and ex == Missing Directory means that remote dir doesn't exist,Let's create it!
			res = sftpCreateDirectories(path);
		}
		AES_CDH_TRACE_MESSAGE("Leaving...");
	}
	return res;
}
/*===================================================================
    ROUTINE:addTimestampToFilename
 =================================================================== */
 // This method add a timestamp to filename	i.e.: filename = file01 should become file01_20071201154523.
string SftpAdapterClass::addTimestampToFilename(string filename)
{
	AES_CDH_TRACE_MESSAGE( "Entering with  filename:%s",filename.c_str());
	char buffer[30];
	string buffer1("");
	string buffer2("");
	string buffer3("");
	string buffer4("");
	struct timeval tv;
	time_t curtime;
	gettimeofday(&tv, NULL);
	curtime=tv.tv_sec;
	strftime(buffer,30,"%Y%m%d%T",localtime(&curtime));

	buffer1.assign(buffer);
	int indexy = buffer1.find_last_of(':');
	if (indexy != (ACE_INT32)buffer1.npos)
	{
		buffer2 = buffer1.substr(indexy+1,(strlen(buffer1.c_str()))-1);
		buffer1 = buffer1.substr(0, indexy);

	}

	int indexm = buffer1.find_last_of(':');
	if (indexm != (ACE_INT32)buffer1.npos)
	{
		buffer3 =buffer1.substr(indexm+1,(strlen(buffer1.c_str()))-1);
		buffer1 = buffer1.substr(0, indexm);

	}

	if(buffer1!="" && buffer2!="" && buffer3!="" )
	{
		buffer4.assign(buffer1);
		buffer4 = buffer4 + buffer3 + buffer2;
	}
	if (buffer4!="")
	{
		filename += "_" + buffer4;
	}
	AES_CDH_TRACE_MESSAGE("Returning with Timestamp filename:%s",filename.c_str());
	return filename;
}

/*===================================================================
   ROUTINE:renameAndSendFileWithRetry
=================================================================== */
// This method rename a file added a timestamp before to send it.

bool SftpAdapterClass::renameAndSendFileWithRetry(void *psshftpiv2, string localfilename, string remotefilename, int retriesNumber, bool ascii)
{
	AES_CDH_TRACE_MESSAGE("Entering... ");
	string newremotefilename =  addTimestampToFilename(remotefilename);
	// loop for retriesNumber times
	while (retriesNumber > 0)
	{
		// check if filename + ".tmp" file exists on remote server
		if (checkRemoteFile(newremotefilename + ".tmp"))
		{
			if (!sftpDeleteFile(newremotefilename + ".tmp"))
			{
				SFTPRESULT=SFTPAPI_FAILURE;
				return false;
			}
		}
		else
		{
			//if it means that file doesn't exist on remote server then let's continue to check if filename exists on remote server
			if (!checkRemoteFile(newremotefilename))
			{
				// if check remote file returns false
				// if it means that the file is not found send the file
				if (sendFile(psshftpiv2, localfilename,newremotefilename + ".tmp",ascii))
				{
					return renameRemoteFile(newremotefilename + ".tmp", newremotefilename);
				}
				else
				{
					SFTPRESULT=SFTPAPI_FAILURE;
					return false;
				}
			}
			retriesNumber--;
		}
	}
	SFTPRESULT=SFTPAPI_FAILURE;
	return false;
}
//------------------------------------------
string SftpAdapterClass::sftpErrorText()
{
	string attr;

	switch(SFTPRESULT)
	{
	case SFTPAPI_ERROR:
		attr = " Error when executing ";
		break;

	case SFTPAPI_OK:
		attr = " No error: Operation Success ";
		break;

	case SFTPAPI_AUTH_FAILED:
		attr = "Either a wrong user name or a wrong password ";
		break;

	case SFTPAPI_NOT_CONNECTED:
		attr = " This SFTP object has no active connection to a server  ";
		break;

	case SFTPAPI_CONNECTED:
		attr = " This SFTP object has already an active connection to a server ";
		break;

	case SFTPAPI_CONNECTION_LOST:
		attr = "The connection to the server has been lost for some reason  ";
		break;

	case SFTPAPI_FILE_ALREADY_EXISTS:
		attr = " The target file for rename already exists  ";
		break;

	case SFTPAPI_INVALID_HANDLE:
		attr = " The supplied file or directory handle was not valid  ";
		break;

	case SFTPAPI_DIRECTORY_ALREADY_EXISTS:
		attr = " The directory that is created or is target for a rename operation already exists ";
		break;

	case SFTPAPI_PERMISSION_DENIED:
		attr = " No permission to execute the operation ";
		break;

	case SFTPAPI_NO_SUCH_DIRECTORY:
		attr = " The directory does not exist  ";
		break;

	case SFTPAPI_FAILURE:
		attr = " Sftp operation failed  ";
		break;

	case SFTPAPI_NOT_YET_IMPLEMENTED:
		attr = " Not yet implemented  ";
		break;

	case SFTPAPI_INVALID_PORT:
		attr = " The port was invalid  ";
		break;

	case SFTPAPI_INVALID_HOST:
		attr = " The host was invalid  ";
		break;

	case SFTPAPI_NO_SUCH_FILE:
		attr = " The file does not exist  ";
		break;

	case SFTPAPI_NOT_VALID_NAME:
		attr=" The file/Dir name not valid ";
		break;

	case SFTPAPI_DIR_CREATE_FAIL:
		attr=" Not able to create directory in remote system ";
		break;

	case SFTPAPI_MAX_RETRY_OVER:
		attr= "Max retry limit over for creation of DIR with timestamp ";
		break;

	case SFTPAPI_BREAK_TRANSM:
		attr= "stop transmission signal encountered ";
		break;

	default:
	{

		string err_back(" Unknown error ") ;
		attr = err_back;
	}
	}

	return attr;
}
