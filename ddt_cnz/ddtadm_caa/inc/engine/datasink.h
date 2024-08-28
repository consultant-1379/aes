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
#ifndef DDTADM_CAA_INC_ENGINE_DATASINK_H_
#define DDTADM_CAA_INC_ENGINE_DATASINK_H_

#include "operation/operation.h"
#include "engine/peermonitor.h"

#include <list>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#ifndef CUTE_TEST
	#include <ace/Task_T.h>
	#include "store/observer.h"
#else
	#include "stubs/ACE_Task_Base_stub.h"
	#include "stubs/store_stub.h"
#endif

namespace engine
{
	class DataAssembler;
	class Peer;

	class DataSink : public store::Observer, public ACE_Task<ACE_MT_SYNCH>, private boost::noncopyable
	{

 	public:
		DataSink(const operation::dataSinkInfo& data);

		virtual ~DataSink();

		/**
		 * @brief Activates the thread.
		 */
		virtual int open(void *args = 0);

		/**
		 *	@brief life cycle
		 */
		virtual int svc(void);

		/**
		 * @brief	Initializes the task and prepare it to run as thread.
		 */
		virtual int start();

		/**
		 * @brief Stops the thread execution and waits for the thread termination.
		 */
		virtual int stop();

		/** @brief
		 *
		 *	This is invoked by the Observed Subject to notify the observer about a new producer.
		 *
		 *  @param producerName: the new producer.
		 *
		 *  @return void
		 *
		 *	@remarks Remarks
		 */
		virtual void update(const std::string& producerName);

		/** @brief
		 *
		 *	Used by pusher threads to send out data to the Peer.
		 *
		 *  @param chunk: formatted chunk
		 *  @param chunkSize: length of the chunk
		 *  @param sourceId: CP name
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		int pushData(void* chunk, size_t chunkSize, const char* sourceId);

		/** @brief
		 *
		 *	Used by pusher threads to send out data to the Peer.
		 *
		 *  @param chunk: formatted chunk
		 *  @param chunkSize: length of the chunk
		 *  @param sourceId: CP name
		 *  @param remoteFileName: name of the remote file
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		int pushData(void* chunk, size_t chunkSize, const char* sourceId, const char* remoteFileName);

		/** @brief
		 *
		 *  This method makes active the current passive
		 *
		 *	@remarks Remarks
		 */
		void switchActivePeerAction();

		/** @brief
		 *
		 *	This is invoked by the DataSource Manager in order to modify Data Sink info
		 *
		 *  @param newData: Modified DataSink info
		 *
		 *	@remarks Remarks
		 */
		int modify(const operation::dataSinkInfo& newData);

		/** @brief
		 *
		 *	This is invoked by the DataSource Manager in order to notify the DataSink of a change occurred in
		 *	OutputFormat data.
		 *
		 *	@remarks Remarks
		 */
		void notifyChange();

		/** @brief
		 *
		 *	This is invoked by the Peer Monitor in order to notify the DataSink that a Peer is now valid
		 *
		 *	@remarks Remarks
		 */
		void notifyValidPeer();

		/** @brief
		 *
		 *	This method requires opening of a remote file
		 *
		 *  @param producerName: producer name
		 *
		 *  @param remoteFileName: remote filename
		 *
		 *  @return ERR_NO_ERRORS on success otherwise a proper error code
		 *
		 *	@remarks Remarks
		 */
		int openRemoteFile(const std::string& producerName, const std::string& remoteFileName);

		/** @brief
		 *
		 *	This method requires closing of the remote file
		 *
		 *  @param producerName: producer name
		 *
		 *  @param remoteFileName: remote filename
		 *
		 *  @return ERR_NO_ERRORS on success otherwise a proper error code
		 *
		 *	@remarks Remarks
		 */
		int closeRemoteFile(const std::string& producerName, const std::string& remoteFileName);

 	private:

		/** @brief
		 *
		 *	This method is used to keep the DataSink live waiting for the stop event.
		 *
		 *	@remarks Remarks
		 */
		bool sleepOnCommunicationError();

		/** @brief
		 *
		 *	This method search and set the active peer.
		 *	In case of connection failures the following DataSink MO attributes govern the re-connection cycle: retryAttempts and retryDelay
		 *
		 *	@remarks Remarks
		 */
		void setActivePeer();

		int tryPeerConnection(boost::shared_ptr<Peer> peer);

		/** @brief
		 *
		 *	This method switches the current active peer in case two peers are configured.
		 *
		 *  @return void
		 *
		 *	@remarks Remarks
		 */
		void switchActivePeerOnError();

		operation::dataSinkInfo m_data;
		std::list< boost::shared_ptr<DataAssembler> > m_pushers;

		/**
		 * 	@brief	m_stopEvent
		 *
		 * 	To signal shutdown
		 *
		 */
		int m_stopEvent;

		/**
		 * 	@brief m_serialStreamMutex
		 *
		 * 	Mutex to synch pushers during serialized streaming
		 *
		 */
		ACE_Recursive_Thread_Mutex m_serialStreamMutex;

		/**
		 * 	@brief m_activePeer
		 *
		 * 	Current working Peer.
		 *
		 */
		boost::shared_ptr<Peer> m_activePeer;

		/**
		 * 	@brief m_running
		 *
		 * 	Thread state
		 *
		 */
		bool m_running;

		PeerMonitor m_peerMonitor;

	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_DATASINK_H_ */
