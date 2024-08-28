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
#include "engine/datasink.h"

#include "engine/context.h"
#include "engine/peer.h"
#include "engine/blockassembler.h"
#include "engine/fileassembler.h"
#include "imm/objectmanagerhelper.h"

#include <boost/make_shared.hpp>
#include <sys/eventfd.h>
#include <poll.h>

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "engine/workingset.h"
#else
	#include "stubs/macro_stub.h"
	#include "stubs/workingset_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_DataSink)

namespace engine
{
	namespace
	{
		const int MAX_RETRY_ON_CLOSE_ERROR = 2;
	}

	DataSink::DataSink(const operation::dataSinkInfo& data)
	: m_data(data),
	  m_pushers(),
	  m_stopEvent(eventfd(0,0)),
	  m_serialStreamMutex(),
	  m_activePeer(),
	  m_running(false),
	  m_peerMonitor(data.getDataSourceName())
	{
		AES_DDT_TRACE_MESSAGE("DATA SINK IS GOING LIVE <%s/%s>", data.getDataSourceName(), data.getName());
		printf("DATA SINK IS GOING LIVE <%s/%s>\n", data.getDataSourceName(), data.getName());
	}

	DataSink::~DataSink()
	{
		AES_DDT_TRACE_FUNCTION;
		::close(m_stopEvent);
	}

	int DataSink::open(void* /*args*/)
	{
		AES_DDT_TRACE_FUNCTION;

		int result = common::errorCode::ERR_NO_ERRORS;

		if(activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED) != common::errorCode::ERR_NO_ERRORS)
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot start svc thread");
			AES_DDT_TRACE_MESSAGE("ERROR: cannot start svc thread. errno: %d", errno);
			result = common::errorCode::ERR_SVC_ACTIVATE;
		}

		return result;
	}

	int DataSink::svc()
	{
		AES_DDT_TRACE_FUNCTION;
		m_running = true;
		int result = common::errorCode::ERR_NO_ERRORS;

		//Give time to the Data Source and to the checkpoint to get initialized.
		sleep(2);

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] DataSink Task is up & running", m_data.getDataSourceName(), m_data.getName());
		AES_DDT_TRACE_MESSAGE("[%s@%s] DataSink Task is up & running", m_data.getDataSourceName(), m_data.getName());

		//Search and set the ACTIVE peer
		setActivePeer();

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Setup Data Assemblers...", m_data.getDataSourceName(), m_data.getName());
		AES_DDT_TRACE_MESSAGE("[%s@%s] Setup Data Assemblers...", m_data.getDataSourceName(), m_data.getName());

		// get Peer type in order to understand the dataAssembler to build
		engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(m_data.dataSourceName);
		bool buildBlockAssembler = (context->countBlockPeers() > 0);

		//Get the list of producers (blades) for this Data Source
		std::list<std::string> producers;
		workingSet_t::instance()->getStoringManager().getProducers(m_data.dataSourceName, producers);

		//Create a DATA ASSEMBLER Thread per each producers. The ASSEMBLER will create the Store Reader
		for(std::list<std::string>::const_iterator it = producers.begin(); it != producers.end(); ++it)
		{
			boost::shared_ptr<DataAssembler> assembler;
			std::string producerName = *it;

			if(buildBlockAssembler)
				assembler = boost::make_shared<BlockAssembler>(m_data.dataSourceName, producerName, m_stopEvent, this);
			else
				assembler = boost::make_shared<FileAssembler>(m_data.dataSourceName, producerName, m_stopEvent, this);

			AES_DDT_TRACE_MESSAGE("[%s@%s] Starting assembler for <%s>", m_data.getDataSourceName(), m_data.getName(), producerName.c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Starting assembler for <%s>", m_data.getDataSourceName(), m_data.getName(), producerName.c_str());

			// start assembler thread
			assembler->start();

			// save the assembler
			m_pushers.push_back(assembler);
		}

		//Subscribe for new producers (blades)
		workingSet_t::instance()->getStoringManager().subscribe(m_data.dataSourceName, this);

		//Start Peer Monitoring Thread
		m_peerMonitor.start();

		AES_DDT_TRACE_MESSAGE("[%s@%s] DataSink has been initialized, hence this task terminates", m_data.getDataSourceName(), m_data.getName());
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] DataSink has been initialized, hence this task terminates", m_data.getDataSourceName(), m_data.getName());

		return result;
	}

	int DataSink::start()
	{
		AES_DDT_TRACE_FUNCTION;

		int result = common::errorCode::ERR_NO_ERRORS;

		if( thr_count() > 0U )
		{
			//ERROR CASE
			AES_DDT_LOG(LOG_LEVEL_ERROR, "thread already running. thr_count(): %zu", thr_count());
			AES_DDT_TRACE_MESSAGE("ERROR: thread already running. thr_count(): %zu", thr_count());
			result = common::errorCode::ERR_OPEN;
		}
		else
		{
			result = open();
		}

		return result;
	}

	int DataSink::stop()
	{
		AES_DDT_TRACE_FUNCTION;

		eventfd_t shutEventOn = 1U;
		if(eventfd_write(m_stopEvent, shutEventOn) != 0)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] FAILED to set shutdown event, fd:<%d> error:<%d>", m_data.getDataSourceName(), m_data.getName(), m_stopEvent, errno);
			AES_DDT_TRACE_MESSAGE("[%s@%s] FAILED to set shutdown event, fd:<%d> error:<%d>", m_data.getDataSourceName(), m_data.getName(), m_stopEvent, errno);
		}

		// Avoid some code execution during shutdown
		m_running = false;

		//Stop surveillance
		m_peerMonitor.stopAllSurveillance();
		m_peerMonitor.stop();

		//Unsubscribe for new producers (blades)
		workingSet_t::instance()->getStoringManager().unsubscribe(m_data.dataSourceName);

		std::list< boost::shared_ptr<DataAssembler> >::iterator pusherIterator = m_pushers.begin();
		for(; m_pushers.end() != pusherIterator; ++pusherIterator )
		{
			AES_DDT_TRACE_MESSAGE("[%s@%s] Stop pushers now...", m_data.getDataSourceName(), m_data.getName());
			printf("[%s@%s] Stop pushers now...\n", m_data.getDataSourceName(), m_data.getName());
			(*pusherIterator)->stop();
		}

		// Wait for svc thread termination
		wait();

		//DISCONNECT PEERS NOW
		Context* context = workingSet_t::instance()->getDataSourceManager().getContext(m_data.dataSourceName);
		std::list< boost::shared_ptr<Peer> > peerList = context->getPeers();
		for(std::list< boost::shared_ptr<Peer> >::iterator it = peerList.begin(); it != peerList.end(); ++it)
		{
			AES_DDT_TRACE_MESSAGE("[%s@%s] Disconnect peers now...", m_data.getDataSourceName(), m_data.getName());
			printf("[%s@%s] Disconnect peers now...\n", m_data.getDataSourceName(), m_data.getName());
			(*it)->disconnect();
		}

		m_pushers.clear();
		return common::errorCode::ERR_NO_ERRORS;
	}

	void DataSink::update(const std::string& producerName)
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("[%s] NOTIFICATION RECEIVED. NEW PRODUCER: <%s>", m_data.dataSourceName.c_str(), producerName.c_str());
		printf("[%s] NOTIFICATION RECEIVED. NEW PRODUCER: <%s>\n", m_data.dataSourceName.c_str(), producerName.c_str());


		bool found = false;
		std::list< boost::shared_ptr<DataAssembler> >::const_iterator element;

		// check if already exist a pusher for this producer
		for(element = m_pushers.begin(); !found && m_pushers.end() != element; ++element)
		{
			found = ( (*element)->getProducerName().compare(producerName) == 0U );
		}

		if(!found)
		{
			// DataPusher not found, create it
			boost::shared_ptr<DataAssembler> assembler;

			// get Peer type in order to understand the dataAssembler to build
			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(m_data.dataSourceName);

			if(context->countBlockPeers() > 0)
				assembler = boost::make_shared<BlockAssembler>(m_data.dataSourceName, producerName, m_stopEvent, this);
			else
				assembler = boost::make_shared<FileAssembler>(m_data.dataSourceName, producerName, m_stopEvent, this);

			// start assembler thread
			assembler->start();

			// save the assembler
			m_pushers.push_back(assembler);
		}
	}

	//This is executed by the block assembler thread
	int DataSink::pushData(void* chunk, size_t chunkSize, const char* sourceId)
	{
		AES_DDT_TRACE_MESSAGE("[%s@%s] %s is pushing %lu bytes", m_data.getDataSourceName(), m_data.getName(), sourceId, chunkSize);
		int result = common::errorCode::ERR_NO_VALID_PEER;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

		if(m_activePeer)
		{
			uint32_t retryAttempt = 0U;

			result = m_activePeer->send(chunk, chunkSize, sourceId);

			//Send failed. Retry...
			while( (common::errorCode::ERR_NO_ERRORS != result) &&
					(retryAttempt < m_data.retryAttempts) &&
					sleepOnCommunicationError() )
			{
				// reset peer connection ...
				if ((result = m_activePeer->restartConnection()) == common::errorCode::ERR_NO_ERRORS)
				{
					// ... and retry send
					result = m_activePeer->send(chunk, chunkSize, sourceId);
				}
				++retryAttempt;
			}

			//-------------------------------------------------
			//This is the case where no retries are configured
			if((common::errorCode::ERR_NO_ERRORS != result) && (0 == m_data.retryAttempts))
			{
				//reset the not working peer connection anyway...
				m_activePeer->disconnect();
			}

			//--------------------------------------------------
			//clean up and try to switch peer
			if(common::errorCode::ERR_NO_ERRORS != result)
			{
				//disconnect all the connections managed by the active peer
				m_activePeer->disconnect();

				//on error switch active peer and continue
				switchActivePeerOnError();
			}
		}

		return result;
	}

	//This is executed by the file assembler thread
	int DataSink::pushData(void* chunk, size_t chunkSize, const char* sourceId, const char* remoteFileName)
	{
		AES_DDT_TRACE_MESSAGE("[%s@%s] %s is pushing %lu bytes into remote file: <%s>", m_data.getDataSourceName(), m_data.getName(), sourceId, chunkSize, remoteFileName);
		int result = common::errorCode::ERR_NO_VALID_PEER;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

		if(m_activePeer)
		{
			uint32_t retryAttempt = 0U;

			result = m_activePeer->send(chunk, chunkSize, sourceId);

			//Send failed. Retry...
			while( (common::errorCode::ERR_NO_ERRORS != result) &&
					(retryAttempt < m_data.retryAttempts) &&
					sleepOnCommunicationError() )
			{
				// reset peer connection ...
				if ((result = m_activePeer->restartConnection(sourceId, remoteFileName)) == common::errorCode::ERR_NO_ERRORS)
				{
					// ... and retry to send
					result = m_activePeer->send(chunk, chunkSize, sourceId);
				}

				++retryAttempt;
			}

			//-----------------------------------------------------
			// This is the case where no retries are configured
			if((common::errorCode::ERR_NO_ERRORS != result) && (0 == m_data.retryAttempts))
			{
				//reset the not working peer connection anyway...
				m_activePeer->disconnect();
			}

			//-------------------------------------------------------
			// if transfer failed, clean up and try to switch peer
			if(common::errorCode::ERR_NO_ERRORS != result)
			{
				//disconnect all the connections managed by the active peer
				m_activePeer->disconnect();

				//on error switch active peer and continue
				switchActivePeerOnError();

				if(!m_activePeer)
				{
					//Note: ERR_NO_VALID_PEER will warn the FileAssembler about the faulty peer
					result = common::errorCode::ERR_NO_VALID_PEER;
				}
			}
			else
			{
				//Transfer successful. Close the remote file.
				int retry = 0;
				do
				{
					result = m_activePeer->closeRemoteContainer(sourceId, remoteFileName);
				} while((common::errorCode::ERR_NO_ERRORS != result)&&
						(++retry < MAX_RETRY_ON_CLOSE_ERROR)&&
						sleepOnCommunicationError());

				if(common::errorCode::ERR_NO_ERRORS == result)
				{
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s --> %s@%s] Closed remote file <%s>", sourceId, m_data.dataSourceName.c_str(), m_data.name.c_str(), remoteFileName);
					AES_DDT_TRACE_MESSAGE("[%s --> %s@%s] Closed remote file <%s>", sourceId, m_data.dataSourceName.c_str(), m_data.name.c_str(), remoteFileName);
				}
				else
				{
					AES_DDT_TRACE_MESSAGE("ERROR:[%s --> %s@%s] Failed to close remote file <%s>", sourceId, m_data.dataSourceName.c_str(), m_data.name.c_str(), remoteFileName);
					AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s --> %s@%s] Failed to close remote file <%s>", sourceId, m_data.dataSourceName.c_str(), m_data.name.c_str(), remoteFileName);
				}
			}
		}

		return result;
	}

	void  DataSink::switchActivePeerAction()
	{
		AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] Switch Active Peer", m_data.getDataSourceName(), m_data.getName());
		AES_DDT_TRACE_MESSAGE("[%s@%s] Switch Active Peer", m_data.getDataSourceName(), m_data.getName());

		imm::objectManagerHelper_t::instance()->initProgressReportForSwitchActivePeer(m_data.moDN);

		Context* context = workingSet_t::instance()->getDataSourceManager().getContext(m_data.dataSourceName);

		if( 1U == context->peerCount() )
		{
			// Only one peer defined
			// Update the progress report structure with error message
			imm::objectManagerHelper_t::instance()->updateActionResult(m_data.moDN, imm::actionprogress::FAILURE);
			imm::objectManagerHelper_t::instance()->updateActionResultInfo(m_data.moDN, imm::actionprogress::resultInfo::SECOND_PEER_MISSING);
		}
		else
		{
			// get exclusive access to dataSink data
			ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

			if (m_activePeer)
			{
				std::list< boost::shared_ptr<Peer> > peerList = context->getValidPeers();
				std::list< boost::shared_ptr<Peer> >::const_iterator peerIterator;

				// search the passive peer towards defined peers
				for( peerIterator = peerList.begin(); peerIterator != peerList.end(); ++peerIterator)
				{
					// Do not allow switch towards a FAULTY peer!
					if( (*peerIterator)->getStatus() == engine::Peer::PASSIVE )
					{
						//Disconnect current peer
						m_activePeer->disconnect();
						m_activePeer->setStatus(engine::Peer::PASSIVE);

						//Change Peer
						m_activePeer = *peerIterator;
						m_activePeer->setStatus(engine::Peer::ACTIVE);

						imm::objectManagerHelper_t::instance()->updateActionResult(m_data.moDN, imm::actionprogress::SUCCESS);
						imm::objectManagerHelper_t::instance()->updateActionResultInfo(m_data.moDN, imm::actionprogress::resultInfo::SUCCESS);

						AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s] Now Active Peer is <%s, %s>",
								m_data.getDataSourceName(), m_data.getName(), m_activePeer->getName().c_str(), m_activePeer->getIpAddress());
						AES_DDT_TRACE_MESSAGE("[%s@%s] Now Active Peer is <%s, %s>",
								m_data.getDataSourceName(), m_data.getName(), m_activePeer->getName().c_str(), m_activePeer->getIpAddress());

						break;
					}
				}

				if( peerList.end() == peerIterator )
				{
					// Additional passive peer not found
					imm::objectManagerHelper_t::instance()->updateActionResult(m_data.moDN, imm::actionprogress::FAILURE);
					imm::objectManagerHelper_t::instance()->updateActionResultInfo(m_data.moDN, imm::actionprogress::resultInfo::SECOND_PEER_FAULTY);
				}
			}
			else
			{
				// No active peer
				// Update the progress report structure with error message
				imm::objectManagerHelper_t::instance()->updateActionResult(m_data.moDN, imm::actionprogress::FAILURE);
				imm::objectManagerHelper_t::instance()->updateActionResultInfo(m_data.moDN, imm::actionprogress::resultInfo::ACTIVE_PEER_MISSING);
			}
		}

		imm::objectManagerHelper_t::instance()->finalizeProgressReportForSwitchActivePeer(m_data.moDN);
	}

	bool DataSink::sleepOnCommunicationError()
	{
		AES_DDT_TRACE_MESSAGE("[%s@%s] Sleep on Communication error... (retryDelay: <%d>)", m_data.getDataSourceName(), m_data.getName(), m_data.retryDelay);

		bool noStopReceived = true;

		struct pollfd fds[1];
		nfds_t nfds =1;

		ACE_Time_Value timeout;

		// Initialize the pollfd structure
		ACE_OS::memset(fds, 0 , sizeof(fds));

		fds[0].fd = m_stopEvent;
		fds[0].events = POLLIN;

		__time_t secs = m_data.retryDelay;
		__suseconds_t usecs = 0;
		timeout.set(secs, usecs);

		int pollresult = ACE_OS::poll(fds, nfds, &timeout);

		if( -1 == pollresult )
		{
			int pollError = errno;
			// Error on poll
			if( EINTR == pollError)
			{
				AES_DDT_TRACE_MESSAGE("[%s@%s] DataSink error on stop event poll, Interrupted System Call. error:<%s>", m_data.getDataSourceName(), m_data.getName(), ::strerror(pollError));
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s]  DataSink error on stop event poll, Interrupted System Call. error:<%s>", m_data.getDataSourceName(), m_data.getName(), ::strerror(pollError));
			}

			AES_DDT_TRACE_MESSAGE("[%s@%s] DataSink error on stop event poll, error:<%s>", m_data.getDataSourceName(), m_data.getName(), ::strerror(pollError));
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s]  DataSink error on stop event poll, error:<%s>", m_data.getDataSourceName(), m_data.getName(), ::strerror(pollError));
			return noStopReceived;
		}

		if( 0 ==  pollresult)
		{
			// Timeout out happened
			AES_DDT_TRACE_MESSAGE("[%s@%s] RetryDelay expired. Continue...", m_data.getDataSourceName(), m_data.getName() );
			return noStopReceived;
		}

		// check stop event
		if( (fds[0].revents & POLLIN) )
		{
			// Received shutdown event
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] DataSink receives shutdown event", m_data.getDataSourceName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s@%s] DataSink receives shutdown event", m_data.getDataSourceName(), m_data.getName());
			noStopReceived = m_running = false;
			return noStopReceived;
		}

		return noStopReceived;
	}

	void DataSink::setActivePeer()
	{
		AES_DDT_TRACE_MESSAGE("[%s@%s] Search active peer", m_data.getDataSourceName(), m_data.getName());

		Context* context = workingSet_t::instance()->getDataSourceManager().getContext(m_data.dataSourceName);

		std::list< boost::shared_ptr<Peer> > peerList = context->getPeers();

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] search active peer", m_data.getDataSourceName(), m_data.getName());
		AES_DDT_TRACE_MESSAGE("[%s@%s] search active peer", m_data.getDataSourceName(), m_data.getName());

		// Search the Active Peer
		for(std::list< boost::shared_ptr<Peer> >::iterator peerIterator = peerList.begin(); peerIterator != peerList.end(); ++peerIterator)
		{
			boost::shared_ptr<Peer> peer = *peerIterator;
			if( peer->getStatus() == engine::Peer::ACTIVE )
			{
				// Previous Active peer found, check if it is still available
				if( common::errorCode::ERR_NO_ERRORS == tryPeerConnection(peer) )
				{
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Previous Active peer is still ACTIVE, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(),peer->getName().c_str(), peer->getIpAddress());
					AES_DDT_TRACE_MESSAGE("[%s@%s] Previous Active peer is still ACTIVE, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(), peer->getName().c_str(), peer->getIpAddress());

					// Active peer found
					m_activePeer = peer;
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Previous Active peer is now Faulty, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(), peer->getName().c_str(), peer->getIpAddress());
					AES_DDT_TRACE_MESSAGE("[%s@%s] Previous Active peer is now Faulty, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(), peer->getName().c_str(), peer->getIpAddress());

					// Previous active is now faulty
					// mark as faulty
					m_peerMonitor.startSurveillance(peer);
				}

				continue;
			}

			if( peer->getStatus() == engine::Peer::FAULTY )
			{

				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Found peer <%s, %s> previously marked as faulty, start monitoring",
						m_data.getDataSourceName(), m_data.getName(), peer->getName().c_str(), peer->getIpAddress());
				AES_DDT_TRACE_MESSAGE("[%s@%s] Found peer <%s, %s> previously marked as faulty, start monitoring",
						m_data.getDataSourceName(), m_data.getName(), peer->getName().c_str(), peer->getIpAddress());

				// Peer was already marked as faulty. Pass it to peer monitor
				m_peerMonitor.startSurveillance(peer);

				continue;
			}

		}

		// check active peer search result
		if(!m_activePeer)
		{
			// Active peer not found or not up&running
			// search Passive peer
			std::list< boost::shared_ptr<Peer> > validPeerList = context->getValidPeers();

			for(std::list< boost::shared_ptr<Peer> >::iterator peerIterator = validPeerList.begin(); peerIterator != validPeerList.end(); ++peerIterator)
			{
				boost::shared_ptr<Peer> peer = *peerIterator;

				// Passive peer found, check if it is available
				if( common::errorCode::ERR_NO_ERRORS == tryPeerConnection(peer) )
				{
					// Active peer found
					m_activePeer = peer;
					m_activePeer->setStatus(engine::Peer::ACTIVE);
					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Previous Passive now is Active peer, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(), m_activePeer->getName().c_str(), m_activePeer->getIpAddress());
					AES_DDT_TRACE_MESSAGE("[%s@%s] Previous Passive now is Active peer, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(), m_activePeer->getName().c_str(), m_activePeer->getIpAddress());
					break;
				}
				else
				{
					// Previous passive is now faulty
					// mark as faulty
					m_peerMonitor.startSurveillance(peer);

					AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Previous Passive peer is now Faulty, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(), peer->getName().c_str(), peer->getIpAddress());
					AES_DDT_TRACE_MESSAGE("[%s@%s] Previous Passive peer is now Faulty, <%s, %s>",
							m_data.getDataSourceName(), m_data.getName(), peer->getName().c_str(), peer->getIpAddress());
				}
			}
		}

		// check valid peer search result
		if(!m_activePeer)
		{
			// All defined peers are faulty
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] No Valid peer found. Active peer not set",
					m_data.getDataSourceName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s@%s] No Valid peer found. Active peer not set",
					m_data.getDataSourceName(), m_data.getName());
		}
	}

	int DataSink::tryPeerConnection(boost::shared_ptr<Peer> peer)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = peer->connect();

		// Connecting to the remote server, so that we are ready to send data
		if(common::errorCode::ERR_NO_ERRORS == result )
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Connected to <%s>", m_data.getDataSourceName(), m_data.getName(), peer->getIpAddress());
			AES_DDT_TRACE_MESSAGE("[%s@%s] Connected to <%s>", m_data.getDataSourceName(), m_data.getName(), peer->getIpAddress());
		}

		return result;
	}

	void DataSink::switchActivePeerOnError()
	{
		AES_DDT_TRACE_FUNCTION;

		if(m_running)
		{
			m_activePeer->disconnect();

			//START MONITORING THE PEER
			m_peerMonitor.startSurveillance(m_activePeer);

			Context* context = workingSet_t::instance()->getDataSourceManager().getContext(m_data.dataSourceName);
			m_activePeer = context->getNextValidPeer();

			if( m_activePeer )
			{
				m_activePeer->setStatus(engine::Peer::ACTIVE);
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Active Peer <%s>", m_data.getDataSourceName(), m_data.getName(), m_activePeer->getIpAddress());
				AES_DDT_TRACE_MESSAGE("[%s@%s] Active Peer <%s>", m_data.getDataSourceName(), m_data.getName(), m_activePeer->getIpAddress());
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] No valid peer found.", m_data.getDataSourceName(), m_data.getName());
				AES_DDT_TRACE_MESSAGE("[%s@%s] No valid peer found.", m_data.getDataSourceName(), m_data.getName());
			}
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_WARN, "[%s@%s] ignoring switchActivePeerOnError during shutdown", m_data.getDataSourceName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s@%s] ignoring switchActivePeerOnError during shutdown", m_data.getDataSourceName(), m_data.getName());

			//reset the active peer
			m_activePeer.reset();
		}

	}

	int  DataSink::modify(const operation::dataSinkInfo& newData)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

		if (newData.changeMask & operation::changeMask::RETRYATTEMPTS_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying retry attempts value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.retryAttempts, newData.retryAttempts);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying retry attempts value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.retryAttempts, newData.retryAttempts);

			m_data.retryAttempts = newData.retryAttempts;
		}

		if (newData.changeMask & operation::changeMask::RETRYDELAY_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying retry delay value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.retryDelay, newData.retryDelay);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying retry delay value, from <%d> to <%d>.", m_data.getDataSourceName(), m_data.getName(),
					m_data.retryDelay, newData.retryDelay);

			m_data.retryDelay = newData.retryDelay;
		}

		return result;
	}

	void DataSink::notifyChange()
	{
		for (std::list< boost::shared_ptr<DataAssembler> >::iterator pusher = m_pushers.begin(); m_pushers.end() != pusher; ++pusher)
		{
			(*pusher)->notifyChange();
		}
	}

	void DataSink::notifyValidPeer()
	{
		AES_DDT_TRACE_FUNCTION;

		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

		//There's a valid peer, check if m_activePeer is already assigned
		if( !m_activePeer )
		{
			Context* context = workingSet_t::instance()->getDataSourceManager().getContext(m_data.dataSourceName);
			m_activePeer = context->getNextValidPeer();

			if( m_activePeer )
			{
				//TODO:: TEMPORARY!!! IT MUST BE REMOVED!!!
				sleep(2);

				//Select this peer as active and update the status accordingly
				m_activePeer->setStatus(engine::Peer::ACTIVE);

				// At this stage is not needed to
				// re-open connection towards the remote server
				// by invoking m_activePeer->restartConnection()
				// because it will be done as openRemoteContainer
				// will be invoked on the PeerConnection

				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Active Peer <%s>", m_data.getDataSourceName(), m_data.getName(), m_activePeer->getIpAddress());
				AES_DDT_TRACE_MESSAGE("[%s@%s] Active Peer <%s>", m_data.getDataSourceName(), m_data.getName(), m_activePeer->getIpAddress());

				//Notify Pushers
				for (std::list< boost::shared_ptr<DataAssembler> >::iterator pusher = m_pushers.begin(); m_pushers.end() != pusher; ++pusher)
				{
					(*pusher)->notifyValidPeer();
				}
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s@%s]Received a valid peer notification from peer monitor but no valid peer found.", m_data.getDataSourceName(), m_data.getName());
				AES_DDT_TRACE_MESSAGE("[%s@%s] ERROR: Received a valid peer notification from peer monitor but no valid peer found.", m_data.getDataSourceName(), m_data.getName());
			}
		}
	}

	int DataSink::openRemoteFile(const std::string& producerName, const std::string& remoteFileName)
	{
		AES_DDT_TRACE_FUNCTION;
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

		int result = common::errorCode::ERR_NO_VALID_PEER;

		if(m_activePeer)
		{
			uint32_t retryAttempt = 0U;

			result = m_activePeer->openRemoteContainer(producerName, remoteFileName);

			//Send failed. Retry...
			while( (common::errorCode::ERR_NO_ERRORS != result) &&
					(retryAttempt < m_data.retryAttempts) &&
					sleepOnCommunicationError() )
			{
				// reset peer connection ...
				m_activePeer->restartConnection(producerName.c_str(), remoteFileName.c_str());

				// ... and retry to send
				result = m_activePeer->openRemoteContainer(producerName, remoteFileName);

				++retryAttempt;
			}

			if(common::errorCode::ERR_NO_ERRORS != result)
			{
				//disconnect all the connections managed by the active peer
				m_activePeer->disconnect();

				//on error switch active peer and continue
				switchActivePeerOnError();

				if(!m_activePeer)
				{
					//Note: ERR_NO_VALID_PEER will warn the FileAssembler about the faulty peer
					result = common::errorCode::ERR_NO_VALID_PEER;
				}
			}
		}

		return result;
	}

	int DataSink::closeRemoteFile(const std::string& producerName, const std::string& remoteFileName)
	{
		AES_DDT_TRACE_FUNCTION;
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_serialStreamMutex);

		int result = common::errorCode::ERR_NO_ERRORS;

		if (m_activePeer)
		{
			result = m_activePeer->closeRemoteContainer(producerName, remoteFileName);
		}

		return result;
	}

} /* namespace engine */
