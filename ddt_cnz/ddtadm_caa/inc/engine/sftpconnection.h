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

#ifndef DDTADM_CAA_INC_ENGINE_SFTPCONNECTION_H_
#define DDTADM_CAA_INC_ENGINE_SFTPCONNECTION_H_

#include "engine/filepeer.h"
#include "engine/peerconnection.h"

#include "operation/operation.h"

#include <libssh2.h>
#include <libssh2_sftp.h>

namespace engine
{
	/**
	 * @class SftpConnection
	 *
	 * @brief This class provides concrete implementation to manage
	 *
	 * the SFTP connection.
	 *
	 */
	class SftpConnection : public PeerConnection
	{
 	public:

		/// Constructor
		SftpConnection(const operation::filePeerInfo& data, const std::string& producer);

		/// Constructor
		SftpConnection(const operation::filePeerInfo& data);

		/// Destructor
		virtual ~SftpConnection();

		/**	@brief
		 *
		 *	This method helps to initialize connection handle.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		virtual int connect();

		/**	@brief
		 *
		 *	This method helps to clean connection handle.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		virtual int disconnect();

		/**	@brief
		 *
		 *	This method helps to send the data to remote.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		virtual int send(void* data, size_t dataLength);

		/**	@brief
		 *
		 *	This method helps to open remote container for given remote file name.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		virtual int openRemoteContainer(const std::string& remoteFileName);

		/**	@brief
		 *
		 *	This method helps to close remote container for given remote file name.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		virtual int closeRemoteContainer(const std::string& remoteFileName);

		/**	@brief
		 *
		 *	This method helps to rename temp file(s) name.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		virtual int renameTempFiles();

 	private:

		/**	@brief
		 *
		 *	This method helps to connect to remote
		 *
		 *	@param sockfd : socket fd
		 *
		 *	@param ipAddress : ip address
		 *
		 *	@param portNumber : port number
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int tcpConnect(int& sockfd, const char* ipAddress = 0, uint32_t portNumber = 0U);

		/**	@brief
		 *
		 *	This method does ssh handshake.
		 *
		 *	@param sockfd : socket fd
		 *
		 *	@param sshSession : SSH session
		 *
		 *	@param user : user name
		 *
		 *	@param pwd : password
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int sshHandshake(const int& sockfd, LIBSSH2_SESSION*& sshSession, const char* user = 0, const char* pwd = 0);

		/**	@brief
		 *
		 *	This method opens sftp channel.
		 *
		 *	@param sshSession : SSH session
		 *
		 *	@param sftpSession : SFTP session
		 *
		 *	@param folder : folder name
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int openSftpChannel(LIBSSH2_SESSION* sshSession, LIBSSH2_SFTP*& sftpSession, const char* folder = 0);

		/**	@brief
		 *
		 *	This method renames temp files present at remote.
		 *
		 *	@param sshSession : SSH session
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int sftpRenameTempFiles(LIBSSH2_SESSION* sshSession);

		/**	@brief
		 *
		 *	This method creates remote folder.
		 *
		 *	@param sftpSession : SFTP session
		 *
		 *	@param remoteFolderPath : remote folder path
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int createRemoteFolder(LIBSSH2_SFTP* sftpSession, std::string& remoteFolderPath);

		/**	@brief
		 *
		 *	This method checks folder exist or not.
		 *
		 *	@param sftpSession : SFTP session
		 *
		 *	@param remoteFolderPath : remote folder path
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int checkIfFolderExists(LIBSSH2_SFTP* sftpSession, std::string& remoteFolderPath);

		/**	@brief
		 *
		 *	This method opens remote file.
		 *
		 *	@param sftpSession : SFTP session
		 *
		 *	@param sftpHandle : SFTP handle
		 *
		 *	@param fileName : name of the file
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int openRemoteFile(LIBSSH2_SFTP* sftpSession, LIBSSH2_SFTP_HANDLE*& sftpHandle, const std::string& fileName);

		/**	@brief
		 *
		 *	This method renames remote file.
		 *
		 *	@param sftpSession : SFTP session
		 *
		 *	@param oldName : old name
		 *
		 *	@param newName : new name
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int renameRemoteFile(LIBSSH2_SFTP* sftpSession, const std::string& oldName, const std::string& newName);

		/**	@brief
		 *
		 *	This method closes remote file.
		 *
		 *	@param sftpHandle : SFTP handle
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int closeRemoteFile(LIBSSH2_SFTP_HANDLE*& sftpHandle);

		/**	@brief
		 *
		 *	This method closes sftp channel.
		 *
		 *	@param sftpSession : SFTP session
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void closeSftpChannel(LIBSSH2_SFTP*& sftpSession);

		/**	@brief
		 *
		 *	This method closes ssh session.
		 *
		 *	@param sshSession : SSH session
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void sshDisconnect(LIBSSH2_SESSION*& sshSession);

		/**	@brief
		 *
		 *	This method disconnects from remote.
		 *
		 *	@param sockfd : socket fd
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void tcpDisconnect(int& sockfd);

		/// socket fd
		int	m_sockfd;

		/// ssh session
		LIBSSH2_SESSION* m_sshSession;

		///sftp session
		LIBSSH2_SFTP* m_sftpSession;

		/// sftp handle
		LIBSSH2_SFTP_HANDLE* m_sftpHandle;
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_SFTPCONNECTION_H_ */
