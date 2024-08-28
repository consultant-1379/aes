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



#ifndef DDTADM_CAA_INC_ENGINE_PEERCONTAINER_H_
#define DDTADM_CAA_INC_ENGINE_PEERCONTAINER_H_

#include "engine/peer.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/recursive_mutex.hpp>


#include <set>
#include <list>


namespace engine {

	/**
	 * @class AES_DDT_PeerContainer
	 *
	 * @brief Container class for Peer objects
	 *
	 */
	class PeerContainer {
	public:

		/// Constructor
		PeerContainer();

		/// Destructor
		virtual ~PeerContainer();

		/**
		 * @brief Invoked to add a valid Peer to the container. If the Peer already exists as faulty, it is marked as valid.
		 *
		 * @param peer : pointer to the peer object to add.
		 *
		 * @return true if the peer is successfully marked as valid, otherwise false
		 */
		bool setValid( const boost::shared_ptr<Peer>& peer);

		/**
		 * @brief Invoked to update a Peer in the container from faulty to valid.
		 *
		 * @param peer : pointer to the peer object to update.
		 *
		 * @return true if the peer is found in the container as faulty and is updated as valid, otherwise false.
		 */
		bool updateValid( const boost::shared_ptr<Peer>& peer);

		/**
		 * @brief Invoked to add a faulty Peer to the container. If the Peer already exists as valid, it is marked as faulty.
		 *
		 * @param peer : pointer to the peer object to add.
		 *
		 * @return true if the peer is successfully marked as faulty, otherwise false
		 */
		bool setFaulty( const boost::shared_ptr<Peer>& peer);

		/**
		 * @brief Invoked to update a Peer in the container from valid to faulty.
		 *
		 * @param peer : pointer to the peer object to update.
		 *
		 * @return true if the peer is found in the container as valid and is updated as faulty, otherwise false.
		 */
		bool updateFaulty( const boost::shared_ptr<Peer>& peer);

		/**
		 * @brief Invoked to erase a Peer from the container.
		 *
		 * @param peer : pointer to the peer object to remove.
		 */
		void erase( const boost::shared_ptr<Peer>& );

		/**
		 * @brief Returns the size (number of peers) of the container.
		 *
		 */
		size_t size();

		/**
		 * @brief Ivonked to empty the container
		 *
		 */
		void clear();

		/**	@brief
		 *
		 *	This method gets the list of peers defined.
		 *
		 *	@return list of peers.
		 *
		 *	@remarks Remarks
		 */
		std::list< boost::shared_ptr<Peer> > getPeerList();

		/**	@brief
		 *
		 *	This method gets the valid list of peers.
		 *
		 *	@return list of valid peers.
		 *
		 *	@remarks Remarks
		 */
		std::list< boost::shared_ptr<Peer> > getValidPeerList();

		/**	@brief
		 *
		 *	This method gets the faulty list of peers.
		 *
		 *	@return list of faulty peers.
		 *
		 *	@remarks Remarks
		 */
		std::list< boost::shared_ptr<Peer> > getFaultyPeerList();


	private:

		/**	@brief
		 *
		 *	This method checks the given peer is present in the list or not.
		 *
		 *	@param peer : peer to be checked.
		 *
		 *	@return true if exits, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool exists( const boost::shared_ptr<Peer>& peer );

		/**	@brief
		 *
		 *	This method checks the given peer is valid or not.
		 *
		 *	@param peer : peer to be checked.
		 *
		 *	@return true if it is valid, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool isValid ( const boost::shared_ptr<Peer>& peer );

		/**	@brief
		 *
		 *	This method checks the given peer is faulty or not.
		 *
		 *	@param peer : peer to be checked.
		 *
		 *	@return true if it is faulty, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool isFaulty ( const boost::shared_ptr<Peer>& peer );

		/**	@brief
		 *
		 *	This method adds the given peer to faulty peer list.
		 *
		 *	@param peer : peer to be added.
		 *
		 *	@return true if it is added, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool addFaultyPeer( const boost::shared_ptr<Peer>& peer );

		/**	@brief
		 *
		 *	This method removes the given peer from faulty peer list.
		 *
		 *	@param peer : peer to be removed.
		 *
		 *	@return true if it is removed, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		void removeFaultyPeer( const boost::shared_ptr<Peer>& peer );

		/**	@brief
		 *
		 *	This method adds the given peer to valid peer list.
		 *
		 *	@param peer : peer to be added.
		 *
		 *	@return true if it is added, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool addValidPeer( const boost::shared_ptr<Peer>& peer );

		/**	@brief
		 *
		 *	This method removes the given peer from valid peer list.
		 *
		 *	@param peer : peer to be removed.
		 *
		 *	@return true if it is removed, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		void removeValidPeer( const boost::shared_ptr<Peer>& peer );

		/**	@brief
		 *
		 *	This method gets the count of valid peers.
		 *
		 *	@return valid peers count.
		 *
		 *	@remarks Remarks
		 */
		size_t validPeerCount();

		/**	@brief
		 *
		 *	This method gets the count of faulty peers.
		 *
		 *	@return faulty peers count.
		 *
		 *	@remarks Remarks
		 */
		size_t faultyPeerCount();

		/**	@brief
		 *
		 *	This method clears all the peers from valid peers list.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void clearValidPeers();

		/**	@brief
		 *
		 *	This method clears all the peers from faulty peers list.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void clearFaultyPeers();

		/// valid peers set
		std::set< boost::shared_ptr<Peer> > m_validPeers;

		/// faulty peers set
		std::set< boost::shared_ptr<Peer> > m_faultyPeers;

	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_PEERCONTAINER_H_ */
