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
#ifndef DDTADM_CAA_INC_ENGINE_PEERCONNECTION_H_
#define DDTADM_CAA_INC_ENGINE_PEERCONNECTION_H_

#include "operation/operation.h"
#include <ace/Recursive_Thread_Mutex.h>
#include <sys/time.h>

#include <list>
#include <string>

#ifdef FILE_DUMP
	#include <fstream>
#endif

namespace engine
{
	/**
	 * @class PeerConnection
	 *
	 * @brief This class helps to manage Peer Connection activities.
	 *	It is base class for FtpConnection and SftpConnection.
	 *
	 */
	class PeerConnection
	{
	 public:

		/// Constructor.
		inline PeerConnection(const operation::filePeerInfo& data, const std::string& producer)
		: m_data(data), m_producer(producer), m_configChangeMutex(), m_modifiedData(), m_currentFileName(), m_connectionStatus(DISCONNECTED), m_tempToken() {}

		/// Constructor.
		inline PeerConnection(const operation::filePeerInfo& data)
		: m_data(data), m_producer(), m_configChangeMutex(), m_modifiedData(), m_currentFileName(), m_connectionStatus(DISCONNECTED), m_tempToken() {}

		/// Destructor.
		virtual ~PeerConnection() {}

		/// pure virtual function
		virtual int connect() = 0;

		/// pure virtual function
		virtual int disconnect()  = 0;

		/// pure virtual function
		virtual int send(void* data, size_t dataLength) = 0;

		/// pure virtual function
		virtual int openRemoteContainer(const std::string& remoteFileName) = 0;

		/// pure virtual function
		virtual int closeRemoteContainer(const std::string& remoteFileName) = 0;

		/// pure virtual function
		virtual int renameTempFiles() = 0;

		/**	@brief
		 *
		 *	This method restarts connection to remote.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		virtual int restartConnection()
		{
			int result = disconnect();
			if (common::errorCode::ERR_NO_ERRORS == result)
			{
				result = connect();
			}

			return result;
		}

		/**	@brief
		 *
		 *	This method modify the current peer info with the given values.
		 *
		 *	@param modifiedInfo : file peer info
		 *
		 *	@return none
		 *
		 *	@remarks Remarks
		 */
		virtual void modify(const operation::filePeerInfo& modifiedInfo)
		{
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_configChangeMutex);
			m_modifiedData.update(modifiedInfo);
		}

		/**	@brief
		 *
		 *	This method checks configuration changed or not.
		 *
		 *	@return boolean
		 *
		 *	@remarks Remarks
		 */
		bool configurationChanged()
		{
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_configChangeMutex);

			return (!m_modifiedData.empty());
		}

		/**	@brief
		 *
		 *	This method updates configuration.
		 *
		 *	@return none
		 *
		 *	@remarks Remarks
		 */
		void updateConfiguration()
		{
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_configChangeMutex);

			if(!m_modifiedData.empty())
			{
				m_data.update(m_modifiedData);
				m_modifiedData.reset();
			}
		}

	 protected:

		enum ConnectionStatus
		{
			CONNECTED,
			DISCONNECTED
		};

		//utility function
		inline std::string getTempFileExtension() {	return m_tempToken;	};

		//utility function
		inline void refreshTempFileExtension()
		{
			time_t now = time(0);
			char tmp[512] = {0};
			snprintf(tmp, sizeof(tmp), "_%ld", now);
			m_tempToken.assign(tmp);
			m_tempToken.append(common::datatransfer::TEMPORARY_FILE_EXTENSION);
		};

		/// file peer info
		operation::filePeerInfo m_data;

		/// producer
		std::string m_producer;
		/**
		 * 	@brief m_configChangeMutex
		 *
		 * 	Mutex to synch the access to the m_modifiedData attribute
		 *
		 */
		ACE_Recursive_Thread_Mutex m_configChangeMutex;

		/// modified file peer info
		operation::filePeerInfo m_modifiedData;

		/// current file name
		std::string m_currentFileName;

		/// connection status
		ConnectionStatus m_connectionStatus;

		/// token
		std::string m_tempToken;

#ifdef FILE_DUMP
		std::fstream m_file;
#endif

	};
} /* namespace engine */


#endif /* DDTADM_CAA_INC_ENGINE_PEERCONNECTION_H_ */
