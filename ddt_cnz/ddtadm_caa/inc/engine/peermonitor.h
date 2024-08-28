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
#ifndef DDTADM_CAA_INC_ENGINE_PEERMONITOR_H_
#define DDTADM_CAA_INC_ENGINE_PEERMONITOR_H_


#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>


#ifndef CUTE_TEST
	#include <ace/Task_T.h>

	#include <boost/make_shared.hpp>
	#include "boost/thread/recursive_mutex.hpp"

	#include "engine/peer.h"
	#include "engine/workingset.h"
#else
	#include "stubs/ACE_Task_Base_stub.h"
	#include "stubs/store_stub.h"
#endif

namespace engine
{
class Peer;

/**
 * @class AES_DDT_PeerMonitor
 *
 * @brief Manages peer connection retry and raise/cease Peer Connection fault alarms
 *
 */

class PeerMonitor : public ACE_Task<ACE_MT_SYNCH>, private boost::noncopyable
{
public:

	PeerMonitor(const std::string& dataSourceName);

	virtual ~PeerMonitor();

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

	/**
	 * @brief invoked by DATA SINK when a FAULTY peer is found,
	 * This method raises a conneciton fault alarm and schedules timer if not started.
	 *	@param peerName : name of the peer on which alarm shall be raised
	 */
	void startSurveillance(boost::shared_ptr<Peer> peer);

	/**
	 * @brief invoked when DataSink is down
	 * stop all connectionverify on all peers and alarm will be ceased on all fault pears
	 *
	 */
	void stopAllSurveillance();

	/**
	 * @brief handle_timeout method // INVOKED BY ACE REACTOR
	 *	This method is called when a scheduled timeout expires
	 *
	 */
	virtual int handle_timeout(const ACE_Time_Value&, const void*);

private:

	/**
	 * @brief invoked by svc on handle_timeout
	 * verify the status of faulty pears
	 *
	 */
	void runSurveillance();

	/** @brief
	 *
	 *	This method gets faulty peers from PeerContainer of context object and verifies the connection,
	 *	If peer connection status is true then marks the peer as valid.
	 *
	 *	@return the number of peers still faulty
	 *
	 *	@remarks Remarks
	 */
	size_t verifyFaultyPeers();


	/** @brief
	 *
	 *	This method schedules the timer to monitor faulty peers
	 *
	 *	@remarks Remarks
	 */
	void scheduleTimer();

	/** @brief
	 *
	 *	This method resets the timer to monitor faulty peers and then schedules it again
	 *
	 *	@remarks Remarks
	 */
	void rescheduleTimer();

	/** @brief
	 *
	 *	This method resets the timer to monitor faulty peers
	 *
	 *	@remarks Remarks
	 */
	void resetTimer();

	/** @brief
	 *
	 *	This method cancels the timer to monitor faulty peers
	 *
	 *	@remarks Remarks
	 */
	void cancelTimer();

	/**
	 * 	@brief m_shutDown
	 *
	 * 	TRUE during shutdown or Datasink Thread is down
	 *
	 */
	bool m_shutDown;

	/**
	 * 	@brief	m_timerId
	 */
	long m_timerId;

	/**
	 * 	@brief	m_timerMutex to guard m_timerId from synchronize issues from multiple threads
	 */
	boost::recursive_mutex m_timerMutex;

	/**
	 * 	@brief	dataSourceName
	 */
	std::string m_dataSourceName;
};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_PEERMONITOR_H_ */
