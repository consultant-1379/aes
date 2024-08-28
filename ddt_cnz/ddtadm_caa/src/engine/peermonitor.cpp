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
#include "engine/peermonitor.h"
#include <boost/thread.hpp>

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/macros.h"
	#include "engine/context.h"
#else
	#include "stubs/macro_stub.h"
	#include "stubs/workingset_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_PeerMonitor)

namespace engine
{
	namespace
	{
      //timer delay in sec for ACE_REACTOR schedule_timer
	  const int RETRY_INTERVAL=10;
	  //invalid timer handle
	  const int INVALID_TIMER_HANDLE=-1;
	}

	PeerMonitor::PeerMonitor(const std::string& dataSourceName)
	: m_shutDown(false),
	  m_timerId(INVALID_TIMER_HANDLE),
	  m_timerMutex(),
	  m_dataSourceName(dataSourceName)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	PeerMonitor::~PeerMonitor()
	{
		AES_DDT_TRACE_FUNCTION;

		//Releases all resources from the message queue
		msg_queue_->flush();
	}

	int PeerMonitor::open(void* /*args*/)
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

	int PeerMonitor::svc()
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		ACE_Message_Block* message;
		while(!m_shutDown)
		{
			int get_result = getq(message);
			if(get_result >= 0)
			{
				// Check request type
				if(message->msg_type() == ACE_Message_Block::MB_START)
				{
					runSurveillance();
				}
				else if(message->msg_type() == ACE_Message_Block::MB_HANGUP)
				{
					stopAllSurveillance();
				}
				message->release();
			}
		}

		return result;
	}

	int PeerMonitor::start()
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

	int PeerMonitor::stop()
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_API_CALL;
		m_shutDown = true;

		ACE_Message_Block* msg = new (std::nothrow) ACE_Message_Block;
		if(msg)
		{
			msg->msg_type(ACE_Message_Block::MB_HANGUP);
			int putq_result = putq(msg);
			if (-1 != putq_result)
			{
				result = common::errorCode::ERR_NO_ERRORS;
				wait();
			}
			else
			{
				int errno_save = errno;
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] putq failed with error code:<%d>; errno:<%d>", m_dataSourceName.c_str(), putq_result, errno_save);
				AES_DDT_TRACE_MESSAGE("[%s] ERROR: putq failed with error code:<%d>; errno:<%d>", m_dataSourceName.c_str(), putq_result, errno_save);
			}
		}
		else
		{
			int errno_save = errno;
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] memory allocation failed! errno:<%d>", m_dataSourceName.c_str(), errno_save);
			AES_DDT_TRACE_MESSAGE("[%s] ERROR: memory allocation failed! errno:<%d>", m_dataSourceName.c_str(), errno_save);
		}

		if(common::errorCode::ERR_API_CALL == result)
		{
			// In case of errors, force the deactivation
			AES_DDT_LOG(LOG_LEVEL_WARN, "[%s] forcing the queue deactivation", m_dataSourceName.c_str());
			AES_DDT_TRACE_MESSAGE("[%s] WARNING: forcing the queue deactivation", m_dataSourceName.c_str());

			// Deactivate the queue and wakeup all threads waiting on the queue so they can continue
			// No messages are removed from the queue, however.
			// Any other operations called until the queue is activated again will immediately return -1 with errno == ESHUTDOWN
			// Returns WAS_INACTIVE if queue was inactive before the call and WAS_ACTIVE if queue was active before the call.
			msg_queue_->deactivate();

			AES_DDT_LOG(LOG_LEVEL_WARN, "[%s] waiting for task stop", m_dataSourceName.c_str());
			AES_DDT_TRACE_MESSAGE("[%s] WARNING: waiting for task stop", m_dataSourceName.c_str());
			wait();
		}

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] task stopped", m_dataSourceName.c_str());
		AES_DDT_TRACE_MESSAGE("[%s] task stopped", m_dataSourceName.c_str());

		return result;
	}

	void PeerMonitor::startSurveillance(boost::shared_ptr<Peer> peer)
	{
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] startSurveillance <%s / %s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());
		AES_DDT_TRACE_MESSAGE("[%s] startSurveillance <%s / %s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());

		Context* context = workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName);
		context->markFaulty(peer);

		boost::lock_guard<boost::recursive_mutex> guard(m_timerMutex);

		scheduleTimer();
	}

	void PeerMonitor::stopAllSurveillance()
	{
		AES_DDT_TRACE_FUNCTION;

		m_shutDown = true;

		boost::lock_guard<boost::recursive_mutex> guard(m_timerMutex);

		cancelTimer();
	}

	int PeerMonitor::handle_timeout(const ACE_Time_Value&, const void*)
	{
		AES_DDT_TRACE_FUNCTION;

		ACE_Message_Block* msg = new (std::nothrow) ACE_Message_Block;
		if(msg)
		{
			msg->msg_type(ACE_Message_Block::MB_START);
			if (putq(msg) != -1)
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] SURVEILLANCE TRIGGERED", m_dataSourceName.c_str());
				AES_DDT_TRACE_MESSAGE("[%s] SURVEILLANCE TRIGGERED", m_dataSourceName.c_str());
			}
			else
			{
				int errno_save = errno;
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] CANNOT TRIGGER SURVEILLANCE. errno:<%d>", m_dataSourceName.c_str(), errno_save);
				AES_DDT_TRACE_MESSAGE("[%s] ERROR: CANNOT TRIGGER SURVEILLANCE. errno:<%d>", m_dataSourceName.c_str(), errno_save);

				if(!m_shutDown)
				{
					//Rechedule timer
					boost::lock_guard<boost::recursive_mutex> guard(m_timerMutex);
					rescheduleTimer();
				}
			}
		}

		return common::errorCode::ERR_NO_ERRORS;
	}

	void PeerMonitor::runSurveillance()
	{
		//verifying FAULTY PEERS
		verifyFaultyPeers();

		//IMPORTANT: Mutex shall be acquired only after the check on faulty peers is completed and before checking the number of peers still faulty.
		//In this way it is granted it won't interfere with startSurveillance() method
		boost::lock_guard<boost::recursive_mutex> guard(m_timerMutex);

		size_t nrOfFaultyPeers =  workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName)->getFaultyPeers().size();

		if( (0U < nrOfFaultyPeers) &&  !m_shutDown )
		{
			//reschedule timer as at least one FAULTY peer is found
			rescheduleTimer();
		}
		else
		{
			resetTimer();

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] NO MORE FAULTY PEERS FOUND", m_dataSourceName.c_str());
			AES_DDT_TRACE_MESSAGE("[%s] NO MORE FAULTY PEERS FOUND ",m_dataSourceName.c_str());
		}
	}

	size_t PeerMonitor::verifyFaultyPeers()
	{
		AES_DDT_TRACE_FUNCTION;

		Context* context = workingSet_t::instance()->getDataSourceManager().getContext(m_dataSourceName);

		// Get faulty Peer list
		std::list< boost::shared_ptr<Peer> > peerList = context->getFaultyPeers();

		size_t numberOfFaultyPeers = peerList.size();

		{
			// Log
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Faulty peers to verify:<%lu>", m_dataSourceName.c_str(), numberOfFaultyPeers);
			AES_DDT_TRACE_MESSAGE("[%s] Faulty peers to verify:<%lu>", m_dataSourceName.c_str(), numberOfFaultyPeers);
		}

		for(std::list< boost::shared_ptr<Peer> >::const_iterator peerIterator = peerList.begin(); !m_shutDown && (peerIterator != peerList.end()); ++peerIterator)
		{
			boost::shared_ptr<Peer> peer = *peerIterator;

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] verifying Peer <%s / %s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());
			AES_DDT_TRACE_MESSAGE("[%s] verifying Peer <%s / %s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());

			if(peer->isReadyForConnection())
			{
				// Peer is ready. Mark it as valid...
				context->markValid(peer);

				// ...notify the data sink about it...
				context->getDataSink()->notifyValidPeer();

				// ...and decrease the number of faulty peers.
				numberOfFaultyPeers--;

				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Connection Success Peer <%s /%s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());
				AES_DDT_TRACE_MESSAGE("[%s] Connection Success  Peer  <%s /%s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Connection Failed Peer <%s /%s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());
				AES_DDT_TRACE_MESSAGE("[%s] Connection Failed  Peer  <%s /%s>", m_dataSourceName.c_str(), peer->getName().c_str(), peer->getIpAddress());
			}
		}

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] Peers still faulty:<%lu>", m_dataSourceName.c_str(), numberOfFaultyPeers);
		AES_DDT_TRACE_MESSAGE("[%s] Peers still faulty:<%lu>", m_dataSourceName.c_str(), numberOfFaultyPeers);

		return numberOfFaultyPeers;
	}

	void PeerMonitor::scheduleTimer()
	{
		if( INVALID_TIMER_HANDLE == m_timerId)
		{
			ACE_Time_Value interval(RETRY_INTERVAL);

			m_timerId = engine::workingSet_t::instance()->getMainReactor().schedule_timer(this, 0, interval);

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] schedule timer:<%ld>, check every <%d> sec",	m_dataSourceName.c_str(), m_timerId, RETRY_INTERVAL);
			AES_DDT_TRACE_MESSAGE("[%s] schedule timer:<%ld>, check every <%d> sec", m_dataSourceName.c_str(), m_timerId, RETRY_INTERVAL);
		}
	}

	void PeerMonitor::rescheduleTimer()
	{
		//resets the timer...
		resetTimer();

		//...and then schedule it again
		scheduleTimer();
	}

	void PeerMonitor::resetTimer()
	{
		//resets the timer...
		m_timerId = INVALID_TIMER_HANDLE;
	}

	void PeerMonitor::cancelTimer()
	{
		if( INVALID_TIMER_HANDLE != m_timerId)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s] cancel_timer:<%ld>", m_dataSourceName.c_str(), m_timerId);
			AES_DDT_TRACE_MESSAGE("[%s]  cancel_timer:<%ld>",m_dataSourceName.c_str(),  m_timerId);

			engine::workingSet_t::instance()->getMainReactor().cancel_timer(m_timerId);
			m_timerId = INVALID_TIMER_HANDLE;
		}
	}

} /* namespace engine */
