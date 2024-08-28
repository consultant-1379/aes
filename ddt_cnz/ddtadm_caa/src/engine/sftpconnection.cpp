//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************

#include "engine/sftpconnection.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

#include <string>
#include <sstream>
#include <algorithm>

#ifndef CUTE_TEST
#include "common/tracer.h"
#include "common/logger.h"
#else
#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_SftpConnection)

namespace engine
{

	namespace
	{
		const long CONNECTION_TIMEOUT = 2U;
		const int SSH2_BLOCKING_MODE = 1;
		const long SSH2_SESSION_TIMEOUT_MS = 5000U;
		const int SFTP_OPERATION_SUCCESS = 0;

		const size_t MAX_FILE_NAME_SIZE = 512;

		std::string getSftpErrorReason(unsigned long sftpError)
		{
			std::string reason;

			// SFTP Status Codes (returned by libssh2_sftp_last_error() )
			// see libssh2_sftp.h
			switch(sftpError)
			{
				case LIBSSH2_FX_OK: reason.assign("OK"); break;
				case LIBSSH2_FX_EOF: reason.assign("EOF"); break;
				case LIBSSH2_FX_NO_SUCH_FILE: reason.assign("NO_SUCH_FILE"); break;
				case LIBSSH2_FX_PERMISSION_DENIED: reason.assign("PERMISSION_DENIED"); break;
				case LIBSSH2_FX_FAILURE: reason.assign("FAILURE"); break;
				case LIBSSH2_FX_BAD_MESSAGE: reason.assign("BAD_MESSAGE"); break;
				case LIBSSH2_FX_NO_CONNECTION: reason.assign("NO_CONNECTION"); break;
				case LIBSSH2_FX_CONNECTION_LOST: reason.assign("CONNECTION_LOST"); break;
				case LIBSSH2_FX_OP_UNSUPPORTED: reason.assign("OP_UNSUPPORTED"); break;
				case LIBSSH2_FX_INVALID_HANDLE: reason.assign("INVALID_HANDLE"); break;
				case LIBSSH2_FX_NO_SUCH_PATH: reason.assign("NO_SUCH_PATH"); break;
				case LIBSSH2_FX_FILE_ALREADY_EXISTS: reason.assign("FILE_ALREADY_EXISTS"); break;
				case LIBSSH2_FX_WRITE_PROTECT: reason.assign("WRITE_PROTECT"); break;
				case LIBSSH2_FX_NO_MEDIA: reason.assign("NO_MEDIA"); break;
				case LIBSSH2_FX_NO_SPACE_ON_FILESYSTEM: reason.assign("NO_SPACE_ON_FILESYSTEM"); break;
				case LIBSSH2_FX_QUOTA_EXCEEDED: reason.assign("QUOTA_EXCEEDED"); break;
				case LIBSSH2_FX_UNKNOWN_PRINCIPAL: reason.assign("UNKNOWN_PRINCIPAL"); break;
				case LIBSSH2_FX_LOCK_CONFLICT: reason.assign("LOCK_CONFLICT"); break;
				case LIBSSH2_FX_DIR_NOT_EMPTY: reason.assign("DIR_NOT_EMPTY"); break;
				case LIBSSH2_FX_NOT_A_DIRECTORY: reason.assign("NOT_A_DIRECTORY"); break;
				case LIBSSH2_FX_INVALID_FILENAME: reason.assign("INVALID_FILENAME"); break;
				case LIBSSH2_FX_LINK_LOOP: reason.assign("LINK_LOOP"); break;
				default: reason.assign("UNKNOWN: "); reason.append("sftpError"); break;
			}

			return reason;
		}
	}

	SftpConnection::SftpConnection(const operation::filePeerInfo& data, const std::string& producer)
	: PeerConnection(data, producer),
	  m_sockfd(common::handle::INVALID),
	  m_sshSession(0),
	  m_sftpSession(0),
	  m_sftpHandle(0)
	{
	}

	SftpConnection::SftpConnection(const operation::filePeerInfo& data)
	: PeerConnection(data),
	  m_sockfd(common::handle::INVALID),
	  m_sshSession(0),
	  m_sftpSession(0),
	  m_sftpHandle(0)
	{
	}

	SftpConnection::~SftpConnection()
	{
		disconnect();
	}

	int SftpConnection::connect()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		updateConfiguration();

		if(DISCONNECTED == m_connectionStatus)
		{
			result = tcpConnect(m_sockfd);

			if (common::errorCode::ERR_NO_ERRORS == result)
			{
				result = sshHandshake(m_sockfd, m_sshSession);
				if (common::errorCode::ERR_NO_ERRORS == result)
				{
					result = openSftpChannel(m_sshSession, m_sftpSession);
					if (common::errorCode::ERR_NO_ERRORS == result)
					{
						//'OPEN REMOTE FILE' IS INVOKED LATER BY THE FILE ASSEMBLER
						m_connectionStatus = CONNECTED;
					}
					else
					{
						//SFTP not available
						sshDisconnect(m_sshSession);
						tcpDisconnect(m_sockfd);
					}
				}
				else
				{
					//SSH handshake failed
					tcpDisconnect(m_sockfd);
				}
			}
		}
		return result;
	}

	int SftpConnection::disconnect()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(CONNECTED == m_connectionStatus)
		{
			//close and cleanup resources
			closeRemoteFile(m_sftpHandle);
			closeSftpChannel(m_sftpSession);
			sshDisconnect(m_sshSession);
			tcpDisconnect(m_sockfd);

			m_connectionStatus = DISCONNECTED;
		}

		return result;
	}

	int SftpConnection::send(void* data, size_t dataLength)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if (CONNECTED == m_connectionStatus)
		{
			{	//LOGGING
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s::%s@%s] Sending <%zu> bytes",
							m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), dataLength);

				AES_DDT_TRACE_MESSAGE("[%s::%s::%s@%s] Sending <%zu> bytes",
							m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), dataLength);

				printf("[%s::%s::%s@%s] Sending <%zu> bytes\n",
						m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), dataLength);
			}

			if(m_sftpHandle && m_sftpSession && m_sshSession && (common::handle::INVALID != m_sockfd))
			{
				const char* dataPtr = static_cast<const char*>(data);
				size_t bytesToSend = dataLength;
				do {
					int bytesSent = libssh2_sftp_write(m_sftpHandle, dataPtr, bytesToSend);
					if(bytesSent < 0)
					{
						result = common::errorCode::ERR_API_CALL;

						char* err_msg;
						libssh2_session_last_error(m_sshSession, &err_msg, NULL, 0);
						int sessionErrno = libssh2_session_last_errno(m_sshSession);
						unsigned long sftpError = libssh2_sftp_last_error(m_sftpSession);
						std::string reason = getSftpErrorReason(sftpError);

						{	//LOGGING
							AES_DDT_LOG(LOG_LEVEL_ERROR,
										"[%s::%s::%s@%s] libssh2_sftp_write error: <%d>. Session errno: <%d>. Sftp error code: <%lu>. Reason: '%s'. Error text: <%s>. Bytes to send: <%zu>. Bytes sent: <%d>",
										m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										bytesSent, sessionErrno, sftpError, reason.c_str(), err_msg, bytesToSend, bytesSent);

							AES_DDT_TRACE_MESSAGE("[%s::%s::%s@%s] libssh2_sftp_write error: <%d>. Session errno: <%d>. Sftp error code: <%lu>. Reason: '%s'. Error text: <%s>. Bytes to send: <%zu>. Bytes sent: <%d>",
												  m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
												  bytesSent, sessionErrno, sftpError, reason.c_str(), err_msg, bytesToSend, bytesSent);

							printf("[%s::%s::%s@%s] libssh2_sftp_write error: <%d>. Session errno: <%d>. Sftp error code: <%lu>. Reason: '%s'. Error text: <%s>. Bytes to send: <%zu>. Bytes sent: <%d>\n",
								   m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
								   bytesSent, sessionErrno, sftpError, reason.c_str(), err_msg, bytesToSend, bytesSent);
						}
						break;
					}
#ifdef FILE_DUMP
					if(m_file.is_open()) m_file.write(static_cast<const char*>(dataPtr), bytesSent);
#endif
					dataPtr += bytesSent;
					bytesToSend -= bytesSent;
				} while (bytesToSend > 0);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "####################################\n"
							"## [%s::%s::%s@%s] Invalid handles:\n"
							"##     m_sftpHandle: <%s>\n"
							"##     m_sftpSession: <%s>\n"
							"##     m_sshSession: <%s>\n"
							"##     m_sockfd: <%d>\n",
							m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
							(m_sftpHandle ? "VALID" : "NULL"), (m_sftpSession ? "VALID" : "NULL"), (m_sshSession ? "VALID" : "NULL"), m_sockfd);
				AES_DDT_TRACE_MESSAGE("ERROR: ####################################\n"
						"## [%s::%s::%s@%s] Invalid handles:\n"
						"##     m_sftpHandle: <%s>\n"
						"##     m_sftpSession: <%s>\n"
						"##     m_sshSession: <%s>\n"
						"##     m_sockfd: <%d>\n",
						m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						(m_sftpHandle ? "VALID" : "NULL"), (m_sftpSession ? "VALID" : "NULL"), (m_sshSession ? "VALID" : "NULL"), m_sockfd);

				result = common::errorCode::ERR_API_CALL;
			}
		}
		else
		{
			//DISCONNECTED
			AES_DDT_LOG(LOG_LEVEL_ERROR, "####################################\n"
						"## [%s::%s::%s@%s] Someone is invoking 'send' on a DISCONNECTED connection!! Handles:\n"
						"##     m_sftpHandle: <%s>\n"
						"##     m_sftpSession: <%s>\n"
						"##     m_sshSession: <%s>\n"
						"##     m_sockfd: <%d>\n",
						m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						(m_sftpHandle ? "VALID" : "NULL"), (m_sftpSession ? "VALID" : "NULL"), (m_sshSession ? "VALID" : "NULL"), m_sockfd);

			AES_DDT_TRACE_MESSAGE("ERROR: ####################################\n"
					"## [%s::%s::%s@%s] Someone is invoking 'send' on a DISCONNECTED connection!! Handles:\n"
					"##     m_sftpHandle: <%s>\n"
					"##     m_sftpSession: <%s>\n"
					"##     m_sshSession: <%s>\n"
					"##     m_sockfd: <%d>\n",
					m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
					(m_sftpHandle ? "VALID" : "NULL"), (m_sftpSession ? "VALID" : "NULL"), (m_sshSession ? "VALID" : "NULL"), m_sockfd);

			result = common::errorCode::ERR_API_CALL;
		}

		return result;
	}

	int SftpConnection::openRemoteContainer(const std::string& remoteFileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(configurationChanged() && ((result = restartConnection()) != common::errorCode::ERR_NO_ERRORS))
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Cannot restart connection on MODIFY callback",	m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot restart connection on MODIFY callback",	m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			return result;
		}

		if(common::errorCode::ERR_NO_ERRORS == result )
		{
			result = connect();
			if(common::errorCode::ERR_NO_ERRORS == result )
			{
				std::string temporaryFileName(remoteFileName);

				refreshTempFileExtension();
				temporaryFileName.append(m_tempToken);//TEMPORARY_FILE_EXTENSION

				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s::%s@%s] Opening remote file: <%s>", m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), temporaryFileName.c_str());
				AES_DDT_TRACE_MESSAGE("[%s::%s::%s@%s] Opening remote file: <%s>", m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), temporaryFileName.c_str());
				printf("[%s::%s::%s@%s] Opening remote file: <%s>\n", m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), temporaryFileName.c_str());

				result = openRemoteFile(m_sftpSession, m_sftpHandle, temporaryFileName);

#ifdef FILE_DUMP
				std::string filePath("/data/opt/ap/internal_root/tools/");
				filePath.append(remoteFileName);
				filePath.append("_");
				filePath.append(m_producer);
				m_file.open(filePath.c_str(), std::fstream::out | std::fstream::app | std::fstream::binary );
#endif
			}
		}

		return result;
	}

	int SftpConnection::closeRemoteContainer(const std::string& remoteFileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		result = closeRemoteFile(m_sftpHandle);
		if (common::errorCode::ERR_NO_ERRORS == result)
		{
			std::string temporaryFileName(remoteFileName);
			temporaryFileName.append(m_tempToken);//TEMPORARY_FILE_EXTENSION

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s::%s@%s] Closing remote file: <%s>", m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), temporaryFileName.c_str());
			AES_DDT_TRACE_MESSAGE("[%s::%s::%s@%s] Closing remote file: <%s>", m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), temporaryFileName.c_str());
			printf("[%s::%s::%s@%s] Closing remote file: <%s>\n", m_producer.c_str(), m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), temporaryFileName.c_str());

			result = renameRemoteFile(m_sftpSession, temporaryFileName, remoteFileName);

#ifdef FILE_DUMP
			m_file.close();
#endif
		}

		return result;
	}

	int SftpConnection::renameTempFiles()
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		if(DISCONNECTED == m_connectionStatus)
		{
			result = tcpConnect(m_sockfd);

			if (common::errorCode::ERR_NO_ERRORS == result)
			{
				result = sshHandshake(m_sockfd, m_sshSession);
				if (common::errorCode::ERR_NO_ERRORS == result)
				{
					result = sftpRenameTempFiles(m_sshSession);
					if (common::errorCode::ERR_NO_ERRORS == result)
					{
						//LOG
						AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Fetched File List",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Fetched File List",
											  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
					}
				}
			}

			sshDisconnect(m_sshSession);
			tcpDisconnect(m_sockfd);
		}
		else
		{
			result = common::errorCode::ERR_API_CALL;
		}
		return result;
	}

	int SftpConnection::tcpConnect(int& sockfd, const char* ipAddress, uint32_t portNumber)
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		int rc;
		struct in_addr inaddr;
		struct in6_addr in6addr;
		//--------------------------------------------------
		// Connect to remote host

		//Fillup the server info structure through the local DNS resolver
		const char *host = ipAddress ? ipAddress: m_data.getIpAddress();
		u_short port =  portNumber ? portNumber: m_data.getPortNumber();

		struct addrinfo hints;
		::memset(&hints, 0, sizeof(hints));
		hints.ai_socktype = SOCK_STREAM;

		struct addrinfo *pResult = NULL;
		/********************************************************************/
		/* Check if we were provided the address of the server using        */
		/* inet_pton() to convert the text form of the address to binary    */
		/* form.													        */
		/********************************************************************/
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] BEFORE INET_PTON",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		rc = inet_pton(AF_INET, host, &inaddr);
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] INET_PTON IPV4 rc = %d",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),rc);
		if (rc == 1)    /* valid IPv4 text address? */
		{
			hints.ai_family = AF_INET;
			hints.ai_flags |= AI_NUMERICHOST;
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] connecting to IPv4 address %s:%u",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
		}
		else
		{
			rc = inet_pton(AF_INET6, host, &in6addr);
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] INET_PTON IPV6 rc = %d",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),rc);
			if (rc == 1) /* valid IPv6 text address? */
			{

				hints.ai_family = AF_INET6;
				hints.ai_flags |= AI_NUMERICHOST;
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] connecting to IPv6 address %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
			}
		}

		// Retrieve the IP address associated to the host name
		int errcode = getaddrinfo(host, 0, &hints, &pResult);

		if(common::errorCode::ERR_NO_ERRORS == errcode && pResult!=NULL)
		{
			// Print Server Address
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] connecting to %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] connecting to %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
			}

			if(pResult->ai_family == AF_INET)
			{
				//-------------------------------------------------------------------
				// Create Client Socket
				if( (sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) > 0)
				{

					//------------------------------------------------------------
					// Set Server Info
					struct sockaddr_in server_address;
					memset(&server_address, 0, sizeof(server_address));
					server_address.sin_family      = AF_INET;
					server_address.sin_port        = htons(port);

					server_address.sin_addr.s_addr = *((uint32_t*) & (((sockaddr_in*)pResult->ai_addr)->sin_addr));

					//------------------------------------------------------------
					//Set timeout on socket
					struct timeval timeout;
					timeout.tv_sec = CONNECTION_TIMEOUT;
					timeout.tv_usec = 0;

					if((setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == 0) &&
							(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == 0))
					{
						//--------------------------------------------------------------
						//Connect
						int connect_result = ::connect(sockfd, (struct sockaddr*) &server_address, sizeof(server_address));
						if (0 == connect_result)
						{
							//Success: the connection works
							AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] File Peer Reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] File Peer reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);

							//-----------------------------
							/* ELABORATION WILL CONTINUE */
							/*   WITH THE SSH HANDSHAKE  */
							//-----------------------------
						}
						else
						{
							//Error
							AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] connect failed! Error:<%d>. Cannot connect to %s:%u. Error Description:<%s>",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), connect_result, host, port, strerror(errno));

							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: connect failed!  Error:<%d>. Cannot connect to %s:%u - errno: %d. Error Description:<%s>",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), connect_result, host, port, errno, strerror(errno));

							result = common::errorCode::ERR_API_CALL;

							//Delete socket
							tcpDisconnect(sockfd);
						}
					}
					else
					{
						//Error
						AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] cannot set socket option!",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: cannot set socket option - errno: %d",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

						result = common::errorCode::ERR_API_CALL;

						//Delete socket
						tcpDisconnect(sockfd);
					}
				}
				else
				{
					//socket error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] Cannot Create Socket",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot Create Socket - errno: %d",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

					result = common::errorCode::ERR_API_CALL;
				}

			}//closing AF_INET4
			else if(pResult->ai_family == AF_INET6)
			{
				//-------------------------------------------------------------------
				// Create Client Socket
				if( (sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) > 0)
				{

					//------------------------------------------------------------
					// Set Server Info
					struct sockaddr_in6 server_address;
					memset(&server_address, 0, sizeof(server_address));
					server_address.sin6_family      = AF_INET6;
					server_address.sin6_port        = htons(port);

					inet_pton(AF_INET6,host,&server_address.sin6_addr);

					//------------------------------------------------------------
					//Set timeout on socket
					struct timeval timeout;
					timeout.tv_sec = CONNECTION_TIMEOUT;
					timeout.tv_usec = 0;

					if((setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == 0) &&
							(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == 0))
					{
						//--------------------------------------------------------------
						//Connect
						int connect_result = ::connect(sockfd, (struct sockaddr*) &server_address, sizeof(server_address));
						if (0 == connect_result)
						{
							//Success: the connection works
							AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] File Peer Reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] File Peer reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);

							//-----------------------------
							/* ELABORATION WILL CONTINUE */
							/*   WITH THE SSH HANDSHAKE  */
							//-----------------------------
						}
						else
						{
							//Error
							AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] connect failed! Error:<%d>. Cannot connect to %s:%u. Error Description:<%s>",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), connect_result, host, port, strerror(errno));

							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: connect failed!  Error:<%d>. Cannot connect to %s:%u - errno: %d. Error Description:<%s>",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), connect_result, host, port, errno, strerror(errno));

							result = common::errorCode::ERR_API_CALL;

							//Delete socket
							tcpDisconnect(sockfd);
						}
					}
					else
					{
						//Error
						AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] cannot set socket option!",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: cannot set socket option - errno: %d",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

						result = common::errorCode::ERR_API_CALL;

						//Delete socket
						tcpDisconnect(sockfd);
					}
				}
				else
				{
					//socket error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] Cannot Create Socket",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot Create Socket - errno: %d",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

					result = common::errorCode::ERR_API_CALL;
				}

			}//closing AF_INET6
			else
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG,"remoteAddressCheck(): Unknown family address");
				AES_DDT_TRACE_MESSAGE("remoteAddressCheck(): Unknown family address");
				result = common::errorCode::ERR_API_CALL;
			}
		freeaddrinfo(pResult);
		}//failure of getaddrinfo
		else
		{
			//getaddrinfo error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] getaddrinfo error: %s",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), gai_strerror(errcode));

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: getaddrinfo error: %s - errno: %d",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), gai_strerror(errcode), errno);

			result = common::errorCode::ERR_API_CALL;

		}//closing else of getaddrinfo

		return result;
	}

	int SftpConnection::sshHandshake(const int& sockfd, LIBSSH2_SESSION*& sshSession, const char* user, const char* pwd)
	{
		const char* userNameChar = user ? user : m_data.getUserName();
		const char* passwordChar = pwd ? pwd : m_data.getPassword();

		int result = common::errorCode::ERR_NO_ERRORS;

		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		// Create a session instance
		sshSession = libssh2_session_init();
		if(!sshSession)
		{
			result = common::errorCode::ERR_API_CALL;
			//socket error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] Cannot initialize SSH2",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot initialize SSH2 - errno: %d",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

			return result;
		}

		// libssh2_session_init was successful
		AES_DDT_LOG(LOG_LEVEL_INFO, "[%s::%s@%s] libssh2_session_init() was successful",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_session_init() was successful",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		printf("[%s::%s@%s] libssh2_session_init() was successful\n",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());


		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		// Set blocking mode on session
		libssh2_session_set_blocking(sshSession, SSH2_BLOCKING_MODE);

		// Set timeout in milliseconds for blocking functions
		libssh2_session_set_timeout(sshSession, SSH2_SESSION_TIMEOUT_MS);

		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		// ... start it up. This will trade welcome banners, exchange keys,
		// and setup crypto, compression, and MAC layers
		int handshake_result = 0;
		handshake_result = libssh2_session_handshake(sshSession, sockfd);
		if(handshake_result)
		{
			result = common::errorCode::ERR_API_CALL;

			char *err_msg;
			libssh2_session_last_error(sshSession, &err_msg, NULL, 0);

			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] libssh2_session_handshake() was unsuccessful. errno:<%d>. msg:<%s>",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						libssh2_session_last_errno(sshSession), err_msg);

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_session_handshake() was unsuccessful. errno:<%d>. msg:<%s>",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
								  libssh2_session_last_errno(sshSession), err_msg);

			sshDisconnect(sshSession);

			return result;
		}

		// libssh2_session_handshake was successful
		AES_DDT_LOG(LOG_LEVEL_INFO, "[%s::%s@%s] libssh2_session_handshake() was successful",
				   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_session_handshake() was successful",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		printf("[%s::%s@%s] libssh2_session_handshake() was successful\n",
			   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());


		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		// At this point we havn't yet authenticated.  The first thing to do
		// is check the hostkey's fingerprint against our known hosts Your app
		// may have it hard coded, may go to a file, may present it to the
		// user, that's your call

		libssh2_hostkey_hash(sshSession, LIBSSH2_HOSTKEY_HASH_SHA1);

		// Check what authentication methods are available
		char* userauthlist;

		std::string userName(userNameChar);

		if(!(userauthlist = libssh2_userauth_list(sshSession, userName.c_str(), userName.length())))
		{
			result = common::errorCode::ERR_API_CALL;

			char* err_msg;
			libssh2_session_last_error(sshSession, &err_msg, NULL, 0);

			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] libssh2_userauth_list() was unsuccessful. errno:<%d>. msg:<%s>",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
									libssh2_session_last_errno(sshSession), err_msg);

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: libssh2_userauth_list() was unsuccessful. errno:<%d>. msg:<%s>",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
						libssh2_session_last_errno(sshSession), err_msg);

			sshDisconnect(sshSession);

			return result;
		}

		// libssh2_userauth_list was successful
		AES_DDT_LOG(LOG_LEVEL_INFO, "[%s::%s@%s] Authentication methods available: <%s>",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), userauthlist);

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Authentication methods available: <%s>",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), userauthlist);

		printf("[%s::%s@%s] Authentication methods available: <%s>\n",
			   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), userauthlist);

		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		// Authenticate...
		if( strstr(userauthlist, "password") )
		{
			// We could authenticate via password
			if (libssh2_userauth_password(sshSession,  userNameChar, passwordChar))
			{
				result = common::errorCode::ERR_API_CALL;

				char* err_msg;
				libssh2_session_last_error(sshSession, &err_msg, NULL, 0);

				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Authentication by password failed. errno:<%d>. msg:<%s>",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
							libssh2_session_last_errno(sshSession), err_msg);

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Authentication by password failed. errno:<%d>. msg:<%s>",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
									  libssh2_session_last_errno(sshSession), err_msg);

				sshDisconnect(sshSession);

				return result;
			}
			// Authentication by password was successful
			AES_DDT_LOG(LOG_LEVEL_INFO, "[%s::%s@%s] Authentication by password was successful",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Authentication by password was successful",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			printf("[%s::%s@%s] Authentication by password was successful\n",
				   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		}
		else
		{
			result = common::errorCode::ERR_API_CALL;

			// No supported authentication methods found
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] No supported authentication methods found!",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: No supported authentication methods found!",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			printf("[%s::%s@%s] ERROR: No supported authentication methods found!\n",
				   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			sshDisconnect(sshSession);

			return result;
		}

		// ...we are authenticated!
		return result;
	}

	int SftpConnection::openSftpChannel(LIBSSH2_SESSION* sshSession, LIBSSH2_SFTP*& sftpSession, const char* folder)
	{
		std::string remoteFolderPath;
		if(folder) remoteFolderPath.assign(folder);
		else remoteFolderPath.assign(m_data.remoteFolderPath);

		int result = common::errorCode::ERR_NO_ERRORS;

		//-----------------------------------------------------------------
		//-----------------------------------------------------------------
		sftpSession = libssh2_sftp_init(sshSession);

		if (!sftpSession)
		{
			result = common::errorCode::ERR_API_CALL;

			char *err_msg;
			libssh2_session_last_error(sshSession, &err_msg, NULL, 0);

			int sessionErrno = libssh2_session_last_errno(sshSession);

			if( sessionErrno == LIBSSH2_ERROR_TIMEOUT)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open SFTP channel. LIBSSH2_ERROR_TIMEOUT. %s!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Unable to open SFTP channel. LIBSSH2_ERROR_TIMEOUT. %s!",
										m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open SFTP channel. Session Error: <%d>. ErrText: <%s>!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Unable to open SFTP channel. Session Error: <%d>. ErrText: <%s>!",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
			}

			return result;
		}
		else
		{
			// libssh2_sftp_init() was successful
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_init() was successful\n",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			printf("[%s::%s@%s] libssh2_sftp_init() was successful\n",
				   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			//Get remote folder
			if (!remoteFolderPath.empty())
			{
				result = createRemoteFolder(sftpSession, remoteFolderPath);

				// Make a directory via SFTP
				if(result)
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] libssh2_sftp_mkdir('%s') failed: %d\n",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str(), result);

					AES_DDT_TRACE_MESSAGE("ERROR: [%s::%s@%s] libssh2_sftp_mkdir('%s') failed: %d\n",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str(), result);

					closeSftpChannel(sftpSession);
				}
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] use default remote folder path\n",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
				printf("[%s::%s@%s] use default remote folder path\n",
					   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			}
		}

		return result;
	}

	int SftpConnection::sftpRenameTempFiles(LIBSSH2_SESSION* sshSession)
	{
		std::list<std::string> fileList;
		std::string remoteFolderPath(m_data.remoteFolderPath);

		int result = common::errorCode::ERR_NO_ERRORS;

		//-----------------------------------------------------------------
		LIBSSH2_SFTP* sftpSession = libssh2_sftp_init(sshSession);

		if (!sftpSession)
		{
			//----------------------------------------
			// ERROR
			result = common::errorCode::ERR_API_CALL;

			char *err_msg;
			libssh2_session_last_error(sshSession, &err_msg, NULL, 0);

			int sessionErrno = libssh2_session_last_errno(sshSession);

			if( sessionErrno == LIBSSH2_ERROR_TIMEOUT)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open SFTP channel. LIBSSH2_ERROR_TIMEOUT. %s!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Unable to open SFTP channel. LIBSSH2_ERROR_TIMEOUT. %s!",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open SFTP channel. Session Error: <%d>. ErrText: <%s>!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Unable to open SFTP channel. Session Error: <%d>. ErrText: <%s>!",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
			}

			return result;
		}
		else
		{
			//-------------------------------------------
			// Request a dir listing via SFTP
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_init() was successful\n",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			//-------------------------------------------------------------------------
			// Request a dir listing via SFTP
			LIBSSH2_SFTP_HANDLE* sftpHandle = libssh2_sftp_opendir(sftpSession, remoteFolderPath.c_str());

			if(!sftpHandle)
			{
				//---------------------------------
				// ERROR
				result = common::errorCode::ERR_API_CALL;

				char* err_msg;
				libssh2_session_last_error(sshSession, &err_msg, NULL, 0);
				int sessionErrno = libssh2_session_last_errno(sshSession);

				if( sessionErrno == LIBSSH2_ERROR_TIMEOUT)
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open remote dir. LIBSSH2_ERROR_TIMEOUT. %s!",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR - Unable to open remote dir. LIBSSH2_ERROR_TIMEOUT. %s!",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open remote dir. Session Error: <%d>. ErrText: <%s>!",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Unable to open remote dir. Session Error: <%d>. ErrText: <%s>!",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
				}

				libssh2_sftp_shutdown(sftpSession);

				return result;
			}


			{
				// LOG SUCCESS
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] libssh2_sftp_opendir() is done, now list...",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_opendir() is done, now list...",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			}


			//--------------------------------------------------------------------
			// Loop through all files
			int rc = 0;
			do
			{
				char entry[MAX_FILE_NAME_SIZE] = {0};
				LIBSSH2_SFTP_ATTRIBUTES attrs;

				//To get all the entries in a directory, call libssh2_sftp_readdir until 0 is returned.
				rc = libssh2_sftp_readdir(sftpHandle, entry, sizeof(entry), &attrs);
				if(rc > 0)
				{
					//On success, the length of the filename is returned.
					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] 'libssh2_sftp_readdir' read file: %s", m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), entry);

					//Check if it's a file
					if(LIBSSH2_SFTP_S_ISREG(attrs.permissions))
					{
						std::string fileName(entry);
						if(fileName.size() > common::datatransfer::TEMPORARY_FILE_EXTENSION.size())
						{
							//Store in the list the file name for further processing
							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] '%s' this is a regular file", m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), fileName.c_str());
							fileList.push_back(fileName);
						}
					}
				}
				else if (rc < 0)
				{
					//In case of error, -1 is returned, and libssh2_session_last_error() can be used to retrieve the error that occurred
					result = common::errorCode::ERR_API_CALL;

					char* err_msg;
					libssh2_session_last_error(sshSession, &err_msg, NULL, 0);
					int sessionErrno = libssh2_session_last_errno(sshSession);

					if( sessionErrno == LIBSSH2_ERROR_TIMEOUT)
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to read remote dir. LIBSSH2_ERROR_TIMEOUT. %s!",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);

						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR - Unable to read remote dir. LIBSSH2_ERROR_TIMEOUT. %s!",
											  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
					}
					else
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to read  remote dir. Session Error: <%d>. ErrText: <%s>!",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);

						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Unable to open read  dir. Session Error: <%d>. ErrText: <%s>!",
											  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
					}

				}
				else
				{
					//libssh2_sftp_readdir returned 0, which means that there's no more entry in the remote folder
					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_readdir terminated: no more entry to read.",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
				}

			} while (rc > 0);


			//--------------------------------------------------------------------
			// Check and Rename tmp files

			// set the token to temporary file extension
			std::string token(common::datatransfer::TEMPORARY_FILE_EXTENSION);

			//loop through all regular files
			for(std::list<std::string>::const_iterator fileIterator = fileList.begin(); fileIterator != fileList.end(); ++fileIterator)
			{
				std::string sourceFileName = *fileIterator;

				//find the token
				size_t lastOfIndex = sourceFileName.rfind(token);

				if(lastOfIndex != std::string::npos)
				{
					//token found: check if it's at the end of the file name
					if (sourceFileName.size() - 4 == lastOfIndex)
					{
						//token is at the end
						std::string destFileName(sourceFileName);

						//remove extension
						destFileName.replace(lastOfIndex, token.size(), "");

						//Adjust remote folder
						size_t slashIndex = remoteFolderPath.find_last_of('/');
						if (slashIndex == std::string::npos || slashIndex != remoteFolderPath.size() - 1)
						{
							remoteFolderPath.append("/");
						}

						//Build full source path name
						std::string sourcePath(remoteFolderPath);
						sourcePath.append(sourceFileName);

						//Build full destination path name
						std::string destinationPath(remoteFolderPath);
						destinationPath.append(destFileName);

						//Rename
						result = libssh2_sftp_rename(sftpSession, sourcePath.c_str(), destinationPath.c_str());

						if(result)
						{
							// Error case
							unsigned long sftpError = libssh2_sftp_last_error(sftpSession);

							std::string reason = getSftpErrorReason(sftpError);
							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: libssh2_sftp_rename(from: '%s', to: '%s'), result: <%d>, sftpError: <%lu>, reason: '%s'",
												  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
												  sourcePath.c_str(), destinationPath.c_str(), result, sftpError, reason.c_str());

							AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] libssh2_sftp_rename(from: '%s', to: '%s'), result: <%d>, sftpError: <%lu>, reason: '%s'",
										m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										sourcePath.c_str(), destinationPath.c_str(), result, sftpError, reason.c_str());

						}
						else
						{
							// Success case
							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_rename(from: '%s', to: '%s'), result: <%d>",
												  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
												  sourcePath.c_str(), destinationPath.c_str(), result);

							AES_DDT_LOG(LOG_LEVEL_INFO, "[%s::%s@%s] libssh2_sftp_rename(from: '%s', to: '%s'), result: <%d>",
										m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
										sourcePath.c_str(), destinationPath.c_str(), result);
						}
					}
				}
			}

			//Delete handles used in this method
			libssh2_sftp_closedir(sftpHandle);
			libssh2_sftp_shutdown(sftpSession);
		}

		return result;
	}

	int SftpConnection::createRemoteFolder(LIBSSH2_SFTP* sftpSession, std::string& remoteFolderPath)
	{
		int result = 0;
		char linuxFolderSeparator = '/';
		std::string item;
		std::string finalitem;
		std::vector<std::string> folderList;

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] parsing remote folder path: '%s'\n",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str());

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] parsing remote folder path: '%s'\n",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str());

		printf("[%s::%s@%s] parsing remote folder path: '%s'\n",
			   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str());

		std::stringstream remoteFolderPathStream(remoteFolderPath);
		while (std::getline(remoteFolderPathStream, item, linuxFolderSeparator))
		{
			if(!item.empty())
			{
				folderList.push_back(item);
			}
		}

		std::string sftppath;
		for(std::vector<std::string>::iterator it = folderList.begin(); it != folderList.end() && result == 0; ++it)
		{
			std::string folder = *it;

			if(sftppath.empty())
			{
				sftppath.assign(folder);
			}
			else
			{
				sftppath.append("/");
				sftppath.append(folder);
			}

			if(checkIfFolderExists(sftpSession, sftppath))
			{
				//Remote folder does not exist
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] libssh2_sftp_mkdir('%s')\n",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sftppath.c_str());

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_mkdir('%s')\n",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sftppath.c_str());

				printf("[%s::%s@%s] libssh2_sftp_mkdir('%s')\n",
					   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sftppath.c_str());
				fflush(stdout);//TODO to be removed

				// Make a directory via SFTP
				result = libssh2_sftp_mkdir(sftpSession, sftppath.c_str(),
											LIBSSH2_SFTP_S_IRWXU|
											LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IXGRP|
											LIBSSH2_SFTP_S_IROTH|LIBSSH2_SFTP_S_IXOTH);

				if(result)
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] libssh2_sftp_mkdir('%s') failed: %d\n",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), folder.c_str(), result);

					AES_DDT_TRACE_MESSAGE("ERROR: [%s::%s@%s] libssh2_sftp_mkdir('%s') failed: %d\n",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), folder.c_str(), result);

					printf("ERROR: [%s::%s@%s] libssh2_sftp_mkdir('%s') failed: %d\n",
						   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), folder.c_str(), result);
					fflush(stdout);//TODO to be removed
				}
			}
		}

		return result;
	}

	int SftpConnection::checkIfFolderExists(LIBSSH2_SFTP* sftpSession, std::string& remoteFolderPath)
	{
		int result = 0;
		LIBSSH2_SFTP_ATTRIBUTES attrs;
		ACE_OS::memset(&attrs,'\0',sizeof(LIBSSH2_SFTP_ATTRIBUTES));

		result = libssh2_sftp_stat(sftpSession, remoteFolderPath.c_str(), &attrs);

		if (result==0 && (attrs.flags & LIBSSH2_SFTP_ATTR_PERMISSIONS) && LIBSSH2_SFTP_S_ISDIR(attrs.permissions))
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Remote folder exists <%s>\n",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Remote folder exists <%s>\n",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str());
			printf("[%s::%s@%s] Remote folder exists <%s>\n", m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), remoteFolderPath.c_str());
		}
		else
		{

			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Call 'libssh2_sftp_stat' failed with error <%d> on folder <%s>\n",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), result, remoteFolderPath.c_str());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Call 'libssh2_sftp_stat' failed with error <%d> on folder <%s>\n",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), result, remoteFolderPath.c_str());

			printf("[%s::%s@%s] Call 'libssh2_sftp_stat' failed with error <%d> on folder <%s>\n",
											  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), result, remoteFolderPath.c_str());

			result = -1;
		}

		return result;
	}


	int SftpConnection::openRemoteFile(LIBSSH2_SFTP* sftpSession, LIBSSH2_SFTP_HANDLE*& sftpHandle, const std::string& fileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		// remote path ends with '/'
		// this is ensured by FilePeer_OI::validatePath and by DataReloadHelper::addFilePeerFromDN
		std::string sftppath(m_data.remoteFolderPath);

		sftppath.append(fileName);

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] OPEN REMOTE FILE %s!",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sftppath.c_str());

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] OPEN REMOTE FILE %s!",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sftppath.c_str());

		// Open remote file
		sftpHandle = libssh2_sftp_open(sftpSession, sftppath.c_str(),
									   LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
									   LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
									   LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);

		//See: http://www.libssh2.org/libssh2_sftp_open_ex.html
		//LIBSSH2_FXF_WRITE: Open the file for writing (POSIX: O_WRONLY)
		//LIBSSH2_FXF_CREAT: If the file does not exist, it will be created (POSIX: O_CREAT)
		//LIBSSH2_FXF_TRUNC: Forces an existing file with the same name to be truncated to zero length when creating a file by specifying LIBSSH2_FXF_CREAT. LIBSSH2_FXF_CREAT MUST also be specified if this flag is used

		//LIBSSH2_FXF_APPEND: Force all writes to append data at the end of the file.

		//user permissions: LIBSSH2_SFTP_S_I<action><who>
		//			<action> is R, W or X
		//			<who> is USR, GRP and OTH for user, group and other.


		if(!sftpHandle)
		{
			result = common::errorCode::ERR_API_CALL;

			char* err_msg;
			libssh2_session_last_error(m_sshSession, &err_msg, NULL, 0);
			int sessionErrno = libssh2_session_last_errno(m_sshSession);

			if( sessionErrno == LIBSSH2_ERROR_TIMEOUT)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open remote file. LIBSSH2_ERROR_TIMEOUT. %s!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR - Unable to open remote file. LIBSSH2_ERROR_TIMEOUT. %s!",
											m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
				printf("[%s::%s@%s] ERROR - Unable to open remote file. LIBSSH2_ERROR_TIMEOUT. %s!\n",
															m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to open remote file. Session Error: <%d>. ErrText: <%s>!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Unable to open remote file. Session Error: <%d>. ErrText: <%s>!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
				printf("[%s::%s@%s] Unable to open remote file. Session Error: <%d>. ErrText: <%s>!\n",
											m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
			}

			return result;
		}

		//set on file open
		m_currentFileName.clear();
		m_currentFileName.assign(sftppath);

		// libssh2_sftp_open() was successful
		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_open() was successful, file '%s' is open on remote system",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sftppath.c_str());

		printf("[%s::%s@%s] libssh2_sftp_open() was successful, file '%s' is open on remote system\n",
			   m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sftppath.c_str());

		return result;
	}

	int SftpConnection::renameRemoteFile(LIBSSH2_SFTP* sftpSession, const std::string& oldName, const std::string& newName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Rename remote file from <%s> to <%s>",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), oldName.c_str(), newName.c_str());

		if(sftpSession)
		{
			std::string oldFilePath(m_data.remoteFolderPath);
			oldFilePath.append(oldName);

			std::string newFilePath(m_data.remoteFolderPath);
			newFilePath.append(newName);

			long renameFlag = LIBSSH2_SFTP_RENAME_ATOMIC | LIBSSH2_SFTP_RENAME_NATIVE; // The option LIBSSH2_SFTP_RENAME_OVERWRITE doesn't work

			int renameResult = libssh2_sftp_rename_ex(sftpSession, oldFilePath.c_str(), oldFilePath.length(), newFilePath.c_str(), newFilePath.length(), renameFlag);

			if( SFTP_OPERATION_SUCCESS != renameResult )
			{
				unsigned long sftpError = libssh2_sftp_last_error(sftpSession);
				std::string reason = getSftpErrorReason(sftpError);

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Failed to rename remote file from <%s> to <%s>. Error: <%d> SFTP Error: <%lu>. Trying to rename the file. Reason: '%s'",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), oldFilePath.c_str(), newFilePath.c_str(), renameResult, sftpError, reason.c_str());

				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to rename remote file from <%s> to <%s>. Error: <%d> SFTP Error: <%lu>.  Trying to rename the file. Reason: '%s'",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), oldFilePath.c_str(), newFilePath.c_str(), renameResult, sftpError, reason.c_str());


				//try to append the timestamp!
				time_t		now = time(0);
				char		tmp[512] = {0};
				snprintf(tmp, sizeof(tmp), "_%ld", now);
				std::string timestamp(tmp);

				newFilePath.append(timestamp);
				renameResult = libssh2_sftp_rename_ex(sftpSession, oldFilePath.c_str(), oldFilePath.length(), newFilePath.c_str(), newFilePath.length(), renameFlag);
				if( SFTP_OPERATION_SUCCESS != renameResult )
				{
					// Error on rename
					result = common::errorCode::ERR_API_CALL;

					sftpError = libssh2_sftp_last_error(sftpSession);
					std::string reason = getSftpErrorReason(sftpError);

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Failed to rename remote file from <%s> to <%s>. Error: <%d> SFTP Error: <%lu>. Reason: '%s'",
										  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), oldFilePath.c_str(), newFilePath.c_str(), renameResult, sftpError, reason.c_str());

					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to rename remote file from <%s> to <%s>. Error: <%d> SFTP Error: <%lu>. Reason: '%s'",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), oldFilePath.c_str(), newFilePath.c_str(), renameResult, sftpError, reason.c_str());
				}
			}
		}
		else
		{
			// Invalid SFTP Handle
			result = common::errorCode::ERR_API_CALL;
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Unable to rename remote file from <%s> to <%s>, SFTP HANDLE INVALID",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), oldName.c_str(), newName.c_str());

			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] Unable to rename remote file from <%s> to <%s>. SFTP HANDLE INVALID",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), oldName.c_str(), newName.c_str());
		}

		return result;
	}

	int SftpConnection::closeRemoteFile(LIBSSH2_SFTP_HANDLE*& sftpHandle)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Closing remote file...",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		if (!sftpHandle) return result;

		int rc = libssh2_sftp_close_handle(sftpHandle);
		if( rc < 0 )
		{
			result = common::errorCode::ERR_API_CALL;

			char *err_msg;
			libssh2_session_last_error(m_sshSession, &err_msg, NULL, 0); //TODO is m_sshSession unique?
			int sessionErrno = libssh2_session_last_errno(m_sshSession); //TODO is m_sshSession unique?

			if(LIBSSH2_ERROR_TIMEOUT == rc)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] libssh2_sftp_close_handle() failed. LIBSSH2_ERROR_TIMEOUT. ErrText: <%s>!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), err_msg);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] libssh2_sftp_close_handle() failed. Session Error: <%d>. ErrText: <%s>!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), sessionErrno, err_msg);
			}

			sftpHandle = 0;
			return result;
		}

		sftpHandle = 0;

		//clean up on file close
		m_currentFileName.clear();

		// Remote file closed
		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] libssh2_sftp_close_handle() was successful",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		return result;
	}

	void SftpConnection::closeSftpChannel(LIBSSH2_SFTP*& sftpSession)
	{
		if(sftpSession)
		{
			AES_DDT_LOG(LOG_LEVEL_TRACE, "[%s::%s@%s] Closing SFTP Session\n",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Closing SFTP Session\n",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

			libssh2_sftp_shutdown(sftpSession);
			sftpSession = 0;
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_WARN, "[%s::%s@%s] Close SFTP Session has been requested on NULL pointer!!\n",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Close SFTP Session has been requested on NULL pointer!!\n",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
		}
	}

	void SftpConnection::sshDisconnect(LIBSSH2_SESSION*& sshSession)
	{
		if(sshSession)
		{
			libssh2_session_disconnect(sshSession, "Shutdown");
			libssh2_session_free(sshSession);
			sshSession = 0;
		}
	}

	void SftpConnection::tcpDisconnect(int& sockfd)
	{
		if(common::handle::INVALID != sockfd)
		{
			close(sockfd);
			sockfd = common::handle::INVALID;
		}
	}
} /* namespace engine */

