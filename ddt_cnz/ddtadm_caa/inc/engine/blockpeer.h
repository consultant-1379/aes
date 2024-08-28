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
#ifndef DDTADM_CAA_INC_ENGINE_BLOCKPEER_H_
#define DDTADM_CAA_INC_ENGINE_BLOCKPEER_H_

#include "operation/operation.h"
#include "engine/peer.h"

#include <string>
#include <map>

struct CLIENT;

namespace engine
{
	/**
	 * @class BlockPeer
	 *
	 * @brief
	 *
	 * This class provides a concrete implementation to manage the block transfer
	 * to remote peer.
	 *
	 */
	class BlockPeer : public Peer
	{
 	public:

		/// Constructor
		BlockPeer(const operation::blockPeerInfo& data);

		///	Destructor
		virtual ~BlockPeer();

		/**	@brief
		 *
		 *	This method gets the name.
		 *
		 *	@return Returns name
		 *
		 *	@remarks Remarks
		 */
		inline std::string getName() const { return m_data.name; }

		/**	@brief
		 *
		 *	This method connects to remote destination.
		 *
		 *	@return Returns zero value on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int connect();

		/**	@brief
		 *
		 *	This method disconnects from remote destination.
		 *
		 *	@return Returns zero value on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int disconnect();

		/**	@brief
		 *
		 *	This method restarts connection to remote destination.
		 *
		 *	@param producerName : name of the producer
		 *
		 *	@param remoteFileName : name of the remote file
		 *
		 *	@return Returns zero value on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int restartConnection(const char* producerName = 0, const char* remoteFileName = 0);

		/**	@brief
		 *
		 *	This method checks connection is successful or not.
		 *
		 *	@return Returns true on success otherwise false
		 *
		 *	@remarks Remarks
		 */
		virtual bool isReadyForConnection();

		/**	@brief
		 *
		 *	This method sends data chunks to remote destination using rpc.
		 *
		 *	@param data : data to be sent
		 *
		 *	@param dataLength : length of the data
		 *
		 *	@param sourceId : ID of the source
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		virtual int send(void* data, size_t dataLength, const char* sourceId);

		/**	@brief
		 *
		 *	This method opens remote container.
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks This method is not applicable in case of block transfer.
		 */
		virtual int openRemoteContainer(const std::string& , const std::string& ) {return common::errorCode::ERR_GENERIC;};

		/**	@brief
		 *
		 *	This method closes remote container.
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks This method is not applicable in case of block transfer.
		 */
		virtual int closeRemoteContainer(const std::string&, const std::string& ) {return common::errorCode::ERR_GENERIC;};

		/**	@brief
		 *
		 *	This method gets configuration changed details.
		 *
		 *	@return Returns true
		 *
		 *	@remarks This method is not applicable in case of block transfer.
		 */
		inline bool configurationChanged() { return true; };

		/**	@brief
		 *
		 *	This method updates configuration details.
		 *
		 *	@return Returns void
		 *
		 *	@remarks This method is not applicable in case of block transfer.
		 */
		inline virtual void updateConfiguration(){ /* do nothing */ };

 	private:

		/**	@brief
		 *
		 *	This method checks remote address is valid or not.
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		int remoteAddressCheck();

		/**	@brief
		 *
		 *	This method creates and configures rpc client handle
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		int rpcConnect();

		/**	@brief
		 *
		 *	This method gets last succeeded transaction ID
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		int rpcGetLast();

		/**	@brief
		 *
		 *	This method sends the given buffer to remote using rpc.
		 *
		 *	@param buffer : pointer to a data buffer
		 *
		 *	@param bufferLength : length of the buffer
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		int rpcSend(void* buffer, unsigned int bufferLength);

		/**	@brief
		 *
		 *	This method destroys
		 *
		 *	@return Returns zero on success otherwise non zero value
		 *
		 *	@remarks Remarks
		 */
		int rpcDisconnect();

		/// Block Peer details
		operation::blockPeerInfo m_data;

		/// root stream ID
		std::string m_rootStreamId;

		/// current stream ID
		std::string m_currentStreamId;

		/**
		 * 	@brief	m_lastTransactionId
		 *
		 * 	record BGWRPC Transaction ID. The max ID is ~0U that is 4294967295
		 */
		uint32_t m_lastTransactionId;

		typedef std::map<std::string, uint32_t> mapOfLastTransactionIds_t;

		/// map of last transaction IDs
		mapOfLastTransactionIds_t m_lastTransactionIds;

		/// client handler used in BGWRPC transfer
		CLIENT* m_rpcClientHandle;
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_BLOCKPEER_H_ */
