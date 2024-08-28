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
#include "engine/filepeer.h"
#include <boost/scoped_ptr.hpp>

#include "engine/sftpconnection.h"
#include "engine/ftpconnection.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_FilePeer)

namespace engine
{

	FilePeer::FilePeer(const operation::filePeerInfo& data)
	: Peer(data.ipAddress, data.moDN, data.portNumber, data.status, Peer::FILE_PEER),
	  m_data(data), m_protocol(data.transferProtocol),
	  m_connectionsMapMutex()
	{
	}

	FilePeer::~FilePeer()
	{
		disconnect();
	}

	int FilePeer::connect()
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		boost::scoped_ptr<PeerConnection> testConnection;

		if(common::PROTOCOL_SFTP == m_protocol)
		{
			testConnection.reset(new SftpConnection(m_data));
		}
		else
		{
			testConnection.reset(new FtpConnection(m_data));
		}

		result = testConnection->connect();
		testConnection->disconnect();

		return result;
	}

	int FilePeer::disconnect()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_connectionsMapMutex);
		m_connections.clear();

		return result;
	}

	int FilePeer::restartConnection(const char* producerName, const char* remoteFileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		//NOTE: restartConnection with no parameters is only allowed for the block peer
		if(producerName && remoteFileName)
		{
			result = disconnect(producerName);
			if (common::errorCode::ERR_NO_ERRORS == result)
			{
				result = connect(producerName);
				if (common::errorCode::ERR_NO_ERRORS == result)
				{
					if(remoteFileName)
					{
						result = openRemoteContainer(producerName, remoteFileName);
					}
				}
			}

			//Log error
			if (common::errorCode::ERR_NO_ERRORS != result)
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Cannot restart the connection to the peer <%s>",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
							m_data.ipAddress.c_str());

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot restart the connection to the peer <%s>",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),
									  m_data.ipAddress.c_str());
			}
		}

		return result;
	}

	bool FilePeer::isReadyForConnection()
	{
		return (connect() == common::errorCode::ERR_NO_ERRORS);
	}

	int FilePeer::send(void* data, size_t dataLength, const char* sourceId)
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		boost::shared_ptr<PeerConnection> connection = getPeerConnection(sourceId);

		result = connection->send(data, dataLength);
		return result;
	}

	void FilePeer::modify(const operation::filePeerInfo& modifiedInfo)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_connectionsMapMutex);

		//Keep m_data always updated
		m_data.update(modifiedInfo);

		//Propagate the modify to all connections
		for(std::map< std::string, boost::shared_ptr<PeerConnection> >::iterator connectionIterator = m_connections.begin(); connectionIterator != m_connections.end(); ++connectionIterator)
		{
			boost::shared_ptr<PeerConnection> connection = connectionIterator->second; //map value
			connection->modify(modifiedInfo);
		}
	}

	int FilePeer::openRemoteContainer(const std::string& producerName, const std::string& remoteFileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		boost::shared_ptr<PeerConnection> connection = getPeerConnection(producerName);

		result = connection->openRemoteContainer(remoteFileName);

		return result;
	}

	int FilePeer::closeRemoteContainer(const std::string& producerName, const std::string& remoteFileName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		boost::shared_ptr<PeerConnection> connection = getPeerConnection(producerName);

		result = connection->closeRemoteContainer(remoteFileName);

		return result;
	}

	int FilePeer::connect(const std::string& producerName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		boost::shared_ptr<PeerConnection> connection = getPeerConnection(producerName);

		result = connection->connect();

		return result;
	}

	int FilePeer::disconnect(const std::string& producerName)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		boost::shared_ptr<PeerConnection> connection = getPeerConnection(producerName);

		result = connection->disconnect();

		return result;
	}

	boost::shared_ptr<PeerConnection> FilePeer::getPeerConnection(const std::string& producer)
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_connectionsMapMutex);

		boost::shared_ptr<PeerConnection> connection;
		std::map< std::string, boost::shared_ptr<PeerConnection> >::iterator connectionIterator = m_connections.find(producer);

		if( m_connections.end() == connectionIterator)
		{
			// First time connectionIterator is setup
			if(common::PROTOCOL_SFTP == m_protocol)
			{
				connection.reset(new SftpConnection(m_data, producer));
			}
			else
			{
				connection.reset(new FtpConnection(m_data, producer));
			}

			m_connections[producer] = connection;

		}
		else
		{
			connection = connectionIterator->second;
		}

		return connection;
	}

} /* namespace engine */
