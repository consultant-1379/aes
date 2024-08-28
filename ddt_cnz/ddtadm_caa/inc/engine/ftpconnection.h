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

#ifndef DDTADM_CAA_INC_ENGINE_FTPCONNECTION_H_
#define DDTADM_CAA_INC_ENGINE_FTPCONNECTION_H_

#include "engine/filepeer.h"
#include "engine/peerconnection.h"
#include "operation/operation.h"

#include <curl/curl.h>


namespace engine
{
	/**
	 * @class FtpConnection
	 *
	 * @brief This class provides concrete implementation to manage
	 *
	 * the FTP connection.
	 *
	 */
	class FtpConnection : public PeerConnection
	{
 	public:

		/// constructor
		FtpConnection(const operation::filePeerInfo& data, const std::string& producer);

		/// destructor
		FtpConnection(const operation::filePeerInfo& data);

		/// constructor
		virtual ~FtpConnection();

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

		struct Data
		{
			void* buffer;
			size_t iterator;
			size_t size;
		};

		/**	@brief
		 *
		 *	readCallback
		 *
		 *	@remarks Remarks
		 */
		static size_t readCallback(void *ptr, size_t size, size_t nmemb, void *stream);

		/**	@brief
		 *
		 *	listCallback
		 *
		 *	@remarks Remarks
		 */
		static size_t listCallback(void *ptr, size_t size, size_t nmemb, void *userdata);

		/**	@brief
		 *
		 *	This method helps to rename temp file name.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int renameRemoteFile(const std::string& oldName, const std::string& newName);

		/// curl handle
		CURL *m_curlHandle;

	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_FTPCONNECTION_H_ */
