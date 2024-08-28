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
#ifndef DDTADM_CAA_INC_ENGINE_FILEPEER_H_
#define DDTADM_CAA_INC_ENGINE_FILEPEER_H_

#include "operation/operation.h"
#include "engine/peer.h"

#include <ace/Task_T.h>

namespace engine
{
	/**
	 * @class FilePeer
	 *
	 * @brief This class provides concrete implementation to manage
	 *
	 * the FilePeer MO life cycle.
	 *
	 */
	class FilePeer : public Peer
	{
 	public:

		/// Constructor
		FilePeer(const operation::filePeerInfo& data);

		/// Destructor
		virtual ~FilePeer();

		/**	@brief
		 *
		 *	This method gets file peer name.
		 *
		 *	@return name of file peer.
		 *
		 *	@remarks Remarks
		 */
		inline std::string getName() const { return m_data.name; }

		/**	@brief
		 *
		 *	This method verify the connection to the remote is successful or not
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		virtual int connect();

		/**	@brief
		 *
		 *	This method clears all peer connections
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		virtual int disconnect();

		/**	@brief
		 *
		 *	This method restarts peer connection.
		 *
		 *	@param producerName : name of the producer.
		 *
		 *	@param remoteFileName : name of the remote file.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		virtual int restartConnection(const char* producerName = 0, const char* remoteFileName = 0);

		/**	@brief
		 *
		 *	This method checks peer connection status.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		virtual bool isReadyForConnection();

		/**	@brief
		 *
		 *	This method sends the given data to the remote.
		 *
		 *	@param data : data to be sent to remote
		 *
		 *	@param dataLength : length of the data
		 *
		 *	@param sourceId : source Id.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		virtual int send(void* data, size_t dataLength, const char* sourceId);

		/**	@brief
		 *
		 *	This method modifies the current info with the given file peer info.
		 *
		 *	@param modifiedInfo : file peer info
		 *
		 *	@return none
		 *
		 *	@remarks Remarks
		 */
		virtual void modify(const operation::filePeerInfo& modifiedInfo);

		/**	@brief
		 *
		 *	This method opens remote container for the given producer and
		 *	remote file.
		 *
		 *	@param producerName : name of the producer
		 *
		 *	@param remoteFileName : remote file
		 *
		 *	@return zero on success, otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int openRemoteContainer(const std::string& producerName, const std::string& remoteFileName );

		/**	@brief
		 *
		 *	This method closes remote container for the given producer and
		 *	remote file.
		 *
		 *	@param producerName : name of the producer
		 *
		 *	@param remoteFileName : remote file
		 *
		 *	@return zero on success, otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int closeRemoteContainer(const std::string& producerName, const std::string& remoteFileName);
 	private:

		/**	@brief
		 *
		 *	This method connects to remote for the given producer.
		 *
		 *	@param producerName : name of the producer
		 *
		 *	@return zero on success, otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int connect(const std::string& producerName);

		/**	@brief
		 *
		 *	This method disconnects from remote for the given producer.
		 *
		 *	@param producerName : name of the producer
		 *
		 *	@return zero on success, otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int disconnect(const std::string& producerName);

 	protected:

		/**	@brief
		 *
		 *	This method gets the peer connection.
		 *
		 *	@param producerName : name of the producer
		 *
		 *	@return peer connection
		 *
		 *	@remarks Remarks
		 */
		boost::shared_ptr<PeerConnection> getPeerConnection(const std::string& producer);

 	protected:
		/// file peer info
		operation::filePeerInfo m_data;

		/// type of the protocol
		int m_protocol;

		/**
		 * 	@brief m_connectionsMapMutex
		 *
		 * 	Mutex to synch the access to the m_data attribute
		 *
		 */
		ACE_Recursive_Thread_Mutex m_connectionsMapMutex;

	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_FILEPEER_H_ */
