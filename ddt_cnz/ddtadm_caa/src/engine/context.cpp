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
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

#include "engine/context.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#endif


AES_DDT_TRACE_DEFINE(AES_DDT_Engine_Context)

namespace engine
{
	namespace
	{
		static const ssize_t MAX_NUM_OF_PEERS = 2U;
	}


	Context::Context(const std::string& dataSourceName)
	: m_dataSourceName(dataSourceName), m_dataSource(), m_dataSink(), m_outputFormat(), m_fileFormat(),
	  m_peers()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Context::~Context()
	{
		AES_DDT_TRACE_FUNCTION;
		cleanup();
	}

	int Context::countFilePeers()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);

		int num_of_file_peers = 0;
		std::list< boost::shared_ptr<Peer> > peerList = m_peers.getPeerList();

		for (std::list< boost::shared_ptr<Peer>  >::const_iterator it = peerList.begin(); it != peerList.end(); ++it)
		{
			if ((*it)->isFilePeer()) num_of_file_peers++;
		}
		AES_DDT_TRACE_MESSAGE("DS:<%s>, Found <%d> file peer", m_dataSourceName.c_str(), num_of_file_peers);
		return num_of_file_peers;
	}

	int Context::countBlockPeers()
	{
		boost::lock_guard<boost::recursive_mutex> lock(m_contextMutex[peers_idx]);

		int num_of_block_peers = 0;
		std::list< boost::shared_ptr<Peer> > peerList = m_peers.getPeerList();

		for (std::list< boost::shared_ptr<Peer>  >::const_iterator it = peerList.begin(); it != peerList.end(); ++it)
		{
			if ((*it)->isBlockPeer()) num_of_block_peers++;
		}
		AES_DDT_TRACE_MESSAGE("DS:<%s>, Found <%d> block peer", m_dataSourceName.c_str(), num_of_block_peers);
		return num_of_block_peers;
	}

	std::list< boost::shared_ptr<Peer> > Context::getPeers()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);
		return m_peers.getPeerList();
	}

	std::list< boost::shared_ptr<Peer> > Context::getValidPeers()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);
		return m_peers.getValidPeerList();
	}

	std::list< boost::shared_ptr<Peer> > Context::getFaultyPeers()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);
		return m_peers.getFaultyPeerList();
	}

	boost::shared_ptr<Peer> Context::getPeerByName(const std::string& name)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);

		boost::shared_ptr<Peer> peer;
		std::list< boost::shared_ptr<Peer> > peerList = m_peers.getPeerList();

		for (std::list< boost::shared_ptr<Peer> >::const_iterator it = peerList.begin(); it != peerList.end(); ++it)
		{
			if ((*it)->getName().compare(name) == 0)
			{
				peer = *it;
				break;
			}
		}

		if(!peer)
		{
			AES_DDT_TRACE_MESSAGE("WARNING: DS:<%s>, Peer:<%s> not found in the Peer List", m_dataSourceName.c_str(), name.c_str());
			AES_DDT_LOG(LOG_LEVEL_WARN, "DS:<%s>, Peer:<%s> not found in the Peer List", m_dataSourceName.c_str(), name.c_str());
		}

		return peer;
	}

	int Context::resetPeerByName(const std::string& name)
	{
		AES_DDT_TRACE_FUNCTION;
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);

		int result = common::errorCode::ERR_PEER_NOT_FOUND;

		std::list< boost::shared_ptr<Peer> > peerList = m_peers.getPeerList();

		for(std::list< boost::shared_ptr<Peer> >::iterator it = peerList.begin(); it != peerList.end(); ++it)
		{
			if ((*it)->getName().compare(name) == 0)
			{
				result = common::errorCode::ERR_NO_ERRORS;

				m_peers.erase(*it);

				AES_DDT_TRACE_MESSAGE("DS:<%s>, Peer:<%s> removed from the Peer List", m_dataSourceName.c_str(), name.c_str());
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "DS:<%s>, Peer:<%s> removed from the Peer List", m_dataSourceName.c_str(), name.c_str());
				break;
			}
		}
		return result;
	}

	boost::shared_ptr<Peer> Context::getNextValidPeer()
	{
		AES_DDT_TRACE_FUNCTION;

		boost::shared_ptr<Peer> firstValidPeer;
		std::list< boost::shared_ptr<Peer> > validPeers = getValidPeers();

		if (!validPeers.empty())
		{
			firstValidPeer = *(validPeers.begin());
		}

		return firstValidPeer;
	}

	bool Context::isPeerActive(const std::string& peerName)
	{
		AES_DDT_TRACE_FUNCTION;

		boost::shared_ptr<Peer> peer = getPeerByName(peerName);

		return ( ( peer ) ? peer->isActive() : false );
	}

	void Context::cleanup()
	{
		resetDataSink();
		resetFileFormat();
		resetOutputFormat();
		resetDataSource();
		clearPeers();
	}

	void Context::markFaulty(boost::shared_ptr<Peer> peer)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);

		if ( m_peers.updateFaulty(peer) )
		{
			peer->setStatus(engine::Peer::FAULTY);
		}
	}

	void Context::markValid(boost::shared_ptr<Peer> peer)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);

		if ( m_peers.updateValid(peer) )
		{
			peer->setStatus(engine::Peer::PASSIVE);
		}
	}

	bool Context::hasDataSource()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSource_idx]);
		return (m_dataSource != 0);
	};

	void Context::setDataSource(boost::shared_ptr<DataSource> dataSource)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSource_idx]);
		m_dataSource = dataSource;
	};

	void Context::resetDataSource()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSource_idx]);
		m_dataSource.reset();
	};

	boost::shared_ptr<DataSource> Context::getDataSource()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSource_idx]);
		return m_dataSource;
	};

	bool Context::hasDataSink()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSink_idx]);
		return (m_dataSink != 0);
	};

	void Context::setDataSink(boost::shared_ptr<DataSink> dataSink)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSink_idx]);
		m_dataSink = dataSink;
	};

	void Context::resetDataSink()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSink_idx]);
		m_dataSink.reset();
	};

	boost::shared_ptr<DataSink> Context::getDataSink()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[dataSink_idx]);
		return m_dataSink;
	};

	bool Context::hasOutputFormat()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[outputFormat_idx]);
		return (m_outputFormat != 0);
	};

	void Context::setOutputFormat(boost::shared_ptr<OutputFormat> outputFormat)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[outputFormat_idx]);
		m_outputFormat = outputFormat;
	};

	void Context::resetOutputFormat()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[outputFormat_idx]);
		m_outputFormat.reset();
	};

	boost::shared_ptr<OutputFormat> Context::getOutputFormat()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[outputFormat_idx]);
		return m_outputFormat;
	};

	bool Context::hasFileFormat()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[fileFormat_idx]);
		return (m_fileFormat != 0);
	};

	void Context::setFileFormat(boost::shared_ptr<FileFormat> fileFormat)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[fileFormat_idx]);
		m_fileFormat = fileFormat;
	};

	void Context::resetFileFormat()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[fileFormat_idx]);
		m_fileFormat.reset();
	};

	boost::shared_ptr<FileFormat> Context::getFileFormat()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[fileFormat_idx]);
		return m_fileFormat;
	};

	ssize_t Context::peerCount()
	{
		boost::lock_guard<boost::recursive_mutex> lock(m_contextMutex[peers_idx]);
		return m_peers.size();
	};

	void Context::setFilePeer(boost::shared_ptr<FilePeer> peer)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);
		m_peers.setValid(peer);
	};

	void Context::setBlockPeer(boost::shared_ptr<BlockPeer> peer)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);
		m_peers.setValid(peer);
	};

	void Context::clearPeers()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_contextMutex[peers_idx]);
		m_peers.clear();
	}

} /* namespace engine */
