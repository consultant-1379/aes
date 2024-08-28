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


#include "engine/peercontainer.h"
#include "engine/workingset.h"
#include "alarm/alarmhandler.h"
#include "common/tracer.h"
#include "common/logger.h"

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_PeerContainer)

namespace engine {

	PeerContainer::PeerContainer():
			m_validPeers(),
			m_faultyPeers()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	PeerContainer::~PeerContainer()
	{
		AES_DDT_TRACE_FUNCTION;

		clear();
	}

	bool PeerContainer::setValid( const boost::shared_ptr<Peer> &element )
	{
		AES_DDT_TRACE_FUNCTION;

		if( isFaulty(element) )
		{
			AES_DDT_TRACE_MESSAGE("Added peer <%s> was already defined as faulty. Removing previous assignment.",element->getName().c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Added peer <%s> was already defined as faulty. Removing previous assignment", element->getName().c_str());

			removeFaultyPeer(element);
		}

		AES_DDT_TRACE_MESSAGE("Adding peer <%s> as valid.",element->getName().c_str());
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Adding peer <%s> as valid.", element->getName().c_str());

		return addValidPeer(element);

	}

	bool PeerContainer::updateValid( const boost::shared_ptr<Peer> &element )
	{
		AES_DDT_TRACE_FUNCTION;

		bool result = false;

		if( isFaulty(element) )
		{
			AES_DDT_TRACE_MESSAGE("Peer <%s> was previously faulty, updating it as valid.",element->getName().c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Peer <%s> was previously faulty, updating it as valid.", element->getName().c_str());

			removeFaultyPeer(element);
			result = addValidPeer(element);
		}

		return result;
	}

	bool PeerContainer::setFaulty( const boost::shared_ptr<Peer> &element )
	{
		AES_DDT_TRACE_FUNCTION;

		if( isValid(element) )
		{
			AES_DDT_TRACE_MESSAGE("Added peer <%s> was already defined as valid. Removing previous assignment.",element->getName().c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Added peer <%s> was already defined as valid. Removing previous assignment", element->getName().c_str());

			removeValidPeer(element);
		}

		AES_DDT_TRACE_MESSAGE("Adding peer <%s> as valid.",element->getName().c_str());
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Adding peer <%s> as valid.", element->getName().c_str());

		return addFaultyPeer(element);
	}

	bool PeerContainer::updateFaulty( const boost::shared_ptr<Peer> &element )
	{
		AES_DDT_TRACE_FUNCTION;

		bool result = false;

		if( isValid(element) )
		{
			AES_DDT_TRACE_MESSAGE("Peer <%s> was previously valid, updating it as faulty.",element->getName().c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Peer <%s> was previously valid, updating it as faulty.", element->getName().c_str());

			removeValidPeer(element);
			result = addFaultyPeer(element);
		}

		return result;
	}

	void PeerContainer::erase( const boost::shared_ptr<Peer>& element )
	{
		AES_DDT_TRACE_FUNCTION;

		if( isValid(element) )
		{
			removeValidPeer(element);
		}
		else if( isFaulty(element) )
		{
			removeFaultyPeer(element);
		}
	}

	size_t PeerContainer::size()
	{
		AES_DDT_TRACE_FUNCTION;

		return faultyPeerCount() + validPeerCount();
	}

	void PeerContainer::clear()
	{
		AES_DDT_TRACE_FUNCTION;

		clearValidPeers();
		clearFaultyPeers();
	}

	std::list< boost::shared_ptr<Peer> > PeerContainer::getPeerList()
	{
		AES_DDT_TRACE_FUNCTION;

		std::list< boost::shared_ptr<Peer> > outputList;
		std::set_union(m_validPeers.begin(), m_validPeers.end(), m_faultyPeers.begin(), m_faultyPeers.end(), std::back_inserter(outputList));
		return outputList;
	}

	std::list< boost::shared_ptr<Peer> >  PeerContainer::getValidPeerList()
	{
		AES_DDT_TRACE_FUNCTION;

		std::list< boost::shared_ptr<Peer> > outputList(m_validPeers.begin(), m_validPeers.end());

		return outputList;
	}

	std::list< boost::shared_ptr<Peer> >  PeerContainer::getFaultyPeerList()
	{
		AES_DDT_TRACE_FUNCTION;

		std::list< boost::shared_ptr<Peer> > outputList = std::list< boost::shared_ptr<Peer> >(m_faultyPeers.begin(), m_faultyPeers.end());
		return outputList;
	}

	//PRIVATE

	bool PeerContainer::exists( const boost::shared_ptr<Peer>& peer )
	{
		AES_DDT_TRACE_FUNCTION;

		return ( m_validPeers.end() != m_validPeers.find(peer) ) || ( m_faultyPeers.end() != m_faultyPeers.find(peer) );
	}

	bool PeerContainer::isValid ( const boost::shared_ptr<Peer>& peer )
	{
		AES_DDT_TRACE_FUNCTION;

		return ( m_validPeers.end() != m_validPeers.find(peer) );
	}

	bool PeerContainer::isFaulty ( const boost::shared_ptr<Peer>& peer )
	{
		AES_DDT_TRACE_FUNCTION;

		return ( m_faultyPeers.end() != m_faultyPeers.find(peer) );
	}

	bool PeerContainer::addFaultyPeer( const boost::shared_ptr<Peer>& peer )
	{
		AES_DDT_TRACE_FUNCTION;

		std::pair<std::set< boost::shared_ptr<Peer> >::iterator, bool> result = m_faultyPeers.insert(peer);

		if (result.second)
		{
			AES_DDT_TRACE_MESSAGE("Raising alarm for faulty peer <%s>",peer->getName().c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Raising alarm for faulty peer <%s>", peer->getName().c_str());

			engine::workingSet_t::instance()->getAlarmhandler().raiseConnectionFaultAlarm(peer->getDN());
		}

		return result.second;
	}

	void PeerContainer::removeFaultyPeer( const boost::shared_ptr<Peer>& peer )
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("Ceasing alarm for faulty peer <%s>",peer->getName().c_str());
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm for faulty peer <%s>", peer->getName().c_str());

		engine::workingSet_t::instance()->getAlarmhandler().ceaseConnectionFaultAlarm(peer->getDN());
		m_faultyPeers.erase(peer);
	}

	bool PeerContainer::addValidPeer( const boost::shared_ptr<Peer>& peer )
	{
		AES_DDT_TRACE_FUNCTION;

		std::pair<std::set< boost::shared_ptr<Peer> >::iterator, bool> result = m_validPeers.insert(peer);

		return result.second;
	}

	void PeerContainer::removeValidPeer( const boost::shared_ptr<Peer>& peer )
	{
		AES_DDT_TRACE_FUNCTION;

		m_validPeers.erase(peer);
	}

	size_t PeerContainer::validPeerCount()
	{
		AES_DDT_TRACE_FUNCTION;

		return m_validPeers.size();
	}

	size_t PeerContainer::faultyPeerCount()
	{
		AES_DDT_TRACE_FUNCTION;

		return m_faultyPeers.size();
	}

	void PeerContainer::clearValidPeers()
	{
		AES_DDT_TRACE_FUNCTION;

		m_validPeers.clear();
	}

	void PeerContainer::clearFaultyPeers()
	{
		AES_DDT_TRACE_FUNCTION;

		for (std::set< boost::shared_ptr<Peer> >::iterator element = m_faultyPeers.begin(); element != m_faultyPeers.end(); ++element)
		{
			AES_DDT_TRACE_MESSAGE("Ceasing alarm for faulty peer <%s>", (*element)->getName().c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Ceasing alarm for faulty peer <%s>", (*element)->getName().c_str());

			engine::workingSet_t::instance()->getAlarmhandler().ceaseConnectionFaultAlarm((*element)->getDN());
		}

		m_faultyPeers.clear();
	}


} /* namespace engine */
