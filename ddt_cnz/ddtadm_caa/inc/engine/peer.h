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
#ifndef DDTADM_CAA_INC_ENGINE_PEER_H_
#define DDTADM_CAA_INC_ENGINE_PEER_H_

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <map>
#include <stdint.h>

namespace engine
{

	class PeerConnection;

	/**
	 * @class Peer
	 *
	 * @brief This class helps to manage Peer activities
	 *
	 */
	class Peer : private boost::noncopyable
	{

	 public:
		//Enumerations
		enum Type
		{
			BLOCK_PEER = 0,
			FILE_PEER
		};

		enum Status
		{
			PASSIVE = 0,	//Peer is not currently involved in the transfer
			ACTIVE,			//Peer is currently used for the data transfer
			FAULTY			//Peer connection is faulty suspected
		};

		enum Mode
		{
			SERIAL = 0,	//Classic single connection: the same connection serves all blades
			PARALLEL	//Parallel connections: one connection per blade
		};

		/// constructor
		Peer(const std::string& ipAddress, const std::string& peerMoDN, const uint16_t& portNumber, const int32_t& status, const Type& type);

		/// constructor
		Peer(const std::string& ipAddress, const std::string& peerMoDN, const int32_t& status, const Type& type);

		/// destructor
		virtual ~Peer();

		/**	@brief
		 *
		 *	This method gets peer type.
		 *
		 *	@return type of the peer.
		 *
		 *	@remarks Remarks
		 */
		inline Type getType() const { return m_type; };

		/**	@brief
		 *
		 *	This method checks peer is of type "FilePeer" or not.
		 *
		 *	@return true if peer is FilePeer type, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		inline bool isFilePeer() const { return (FILE_PEER == m_type); };

		/**	@brief
		 *
		 *	This method checks peer is of type "BlockPeer" or not.
		 *
		 *	@return true if peer is BlockPeer type, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		inline bool isBlockPeer() const { return (BLOCK_PEER == m_type); };

		/**	@brief
		 *
		 *	This method gets the status.
		 *
		 *	@return ACTIVE/PASSIVE/FAULTY.
		 *
		 *	@remarks Remarks
		 */
		inline int getStatus() const { return m_status; }

		/**	@brief
		 *
		 *	This method sets the status to given value.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		virtual void setStatus(Status newStatus);

		/**	@brief
		 *
		 *	This method checks the current peer status is ACTIVE or not.
		 *
		 *	@return true if it is active, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		inline bool isActive() const { return (ACTIVE == getStatus()); };

		/**	@brief
		 *
		 *	This method gets the IP address.
		 *
		 *	@return IP address.
		 *
		 *	@remarks Remarks
		 */
		inline const char* getIpAddress() const { return m_ipAddress.c_str(); }

		/**	@brief
		 *
		 *	This method gets the peer MO DN.
		 *
		 *	@return DN as const char*.
		 *
		 *	@remarks Remarks
		 */
		inline const char* getDN() const { return m_peerMoDN.c_str(); }

		/// pure virtual function
		virtual std::string getName() const = 0;

		/// pure virtual function
		virtual int connect() = 0;

		/// pure virtual function
		virtual int disconnect() = 0;

		/// pure virtual function
		virtual int restartConnection(const char* producerName = 0, const char* remoteFileName = 0) = 0;

		/// pure virtual function
		virtual bool isReadyForConnection() = 0;

		/// pure virtual function
		virtual int send(void* data, size_t dataLength, const char* sourceId) = 0;

		/// pure virtual function
		virtual int openRemoteContainer(const std::string& producerName, const std::string& remoteFileName) = 0;

		/// pure virtual function
		virtual int closeRemoteContainer(const std::string& producerName, const std::string& remoteFileName) = 0;

	 protected:

		//Key = producer, Value = Connection
		std::map< std::string, boost::shared_ptr<PeerConnection> > m_connections;

		/// ip address of remote
		std::string m_ipAddress;

		/// peer MO DN
		std::string m_peerMoDN;

		/// port number
		uint32_t m_portNumber;

		/// status of the peer
		Status m_status;

		/// type of the peer
		Type m_type;
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_PEER_H_ */
