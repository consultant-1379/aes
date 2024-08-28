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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_DATACHANNEL_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_DATACHANNEL_H_

#include <map>
#include <string>
#include "pduhandler.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <ace/Recursive_Thread_Mutex.h>

#ifndef CUTE_TEST
	#include <ace/TP_Reactor.h>
	#include <ace/Reactor.h>
	#include <ace/Task_T.h>
	#include <ace/Synch.h>
#else
	#include "stubs/operation_stub.h"
#endif

class ACS_DSD_Session;

namespace mtap
{

	/**
	 * @class DataChannel
	 *
	 * @brief The DataChannel class is derived from @c ACE_Event_Handler.
	 *
	 * Manages the connections to one specific CP for a specific Data Source.
	 * Given the Data Source, e.g VCHS1, each CP open 4 connections on IPNA and 4 connections on IPNB
	 *
	 */
	class DataChannel: public ACE_Task<ACE_MT_SYNCH>, private boost::noncopyable
	{
 	public:

		//Constructor
		DataChannel(int32_t cp_system_id, const std::string& dataSourceName);

	    //Destructor
		inline virtual ~DataChannel() { /* clean up ? m_incomingMtapSessionQueue */ };

		/** @brief
		 *
		 *	This method fetches the CP blade id
		 *
		 *	@return Returns the CP blade id.
		 */
		inline int32_t getCpId () const { return m_cp_system_id; };

		/** @brief
		 *
		 *	This method fetches the datasource name
		 *
		 *  @return Returns the datasource name
		 *
		 *  @remarks Remarks
		 *
		 */
		inline const char* getDataSourceName() const { return m_dataSourceName.c_str(); };

		/** @brief
		 *
		 *	This method fetches the default name for a CP blade
		 *
		 *  @return Returns the default name for CP blade
		 *
		 *	@remarks Remarks
		 *
		 */
		inline const char* getDefaultCpName() const { return m_CpName.c_str(); };

		/** @brief
		 *
		 *	This method adds the mtap session created to active mtap sessions list
		 *
		 *  @param mtap_session: mtap session
		 *
		 *  @return ERR_NO_ERRORS on success otherwise a proper error code
		 *
		 *	@remarks Remarks
		 */
		int addSession(boost::shared_ptr<ACS_DSD_Session> mtap_session);

		/**
		 * @brief Stops the execution
		 *
		 * @return ERR_NO_ERRORS on success otherwise a proper error code
		 */
		int stop();

		/**
		 * @brief	Initializes the scheduler task and prepare it to run as thread.
		 *
		 * @return ERR_NO_ERRORS on success otherwise a proper error code
		 *
		 */
		int start();

		/**
		 *	@brief DataChannel life cycle thread.
		 *
		 *	@return ERR_NO_ERRORS on success
		 */
		virtual int svc();

		//--------------------------
		// INVOKED BY ACE REACTOR
		//--------------------------
		/**
		 * @brief
		 *
		 * This method is called when event handler is removed from the <ACE_Reactor>
		 * This is invoked on shutdown/remove when removing handles.
		 *
		 * @param ACE_HANDLE: DSD session handle
		 *
		 * @param ACE_Reactor_Mask: ACE Rector Mask
		 *
		 * @return 0
		 *
		 */
		virtual int handle_close(ACE_HANDLE fd, ACE_Reactor_Mask mask);

		/**
		 * @brief handle_input method
		 * This method is called when input becomes available, which means every time CP sends data.
		 *
		 * @param ACE_HANDLE : DSD session  handle
		 *
		 * @return 0 on success otherwise -1
		 *
		 */
		virtual int handle_input(ACE_HANDLE fd);

		/**
		 * @brief setDefaultCpName method
		 * This method is used to set the default Cp name from Cp id
		 *
		 * @return NONE
		 *
		 */
		void setDefaultCpName();

		//------------------------------------------------------
		// Invoked in the Context of SlidingWindow/PDU Handler
		//------------------------------------------------------
		/**
		 * @brief send_data method
		 * This method is used to sends the ACK and STOP message to CP
		 *
		 * @param ACE_HANDLE: DSD session handle
		 *
		 * @param outBuffer: Output buffer that contains data to be sent
		 *
		 * @param outBufferLength: Length of the output bufffer
		 *
		 * @return 0 on success otherwise -1
		 *
		 */
		int send_data(ACE_HANDLE fd, unsigned char* outBuffer, ssize_t& outBufferLength);

		/**
		 * @brief closeOtherSessions method
		 * This method is used on SYNCHRONIZE to close all sessions except the fd one
		 *
		 * @param ACE_HANDLE: DSD session handle
		 *
		 * @return NONE
		 *
		 */
		void closeOtherSessions(ACE_HANDLE fd);

		//---------------------
		// Utilities
		//---------------------
		/**
		 * @brief getInfo method
		 * This method returns formatted data about this channel in the form: "DS:<name> CP ID:<id>"
		 *
		 * @return Returns the channel information
		 *
		 */
		inline const char* getInfo() { return m_channelInfo.c_str(); };

 	private:

		/**
		 * @brief
		 *
		 * This method is called to add a session handle to the internal reactor.
		 *
		 * @param handle : handle to register
		 *
		 * @return true on success otherwise false
		 *
		 */
		bool registerHandleToReactor(ACE_HANDLE& handle);

		/**
		 * @brief
		 * This method is called to get the session handle from the incoming session
		 *
		 * @param session : the MTAP session
		 *
		 * @param handle : retrieved handle
		 *
		 * @return true on success otherwise false
		 *
		 */
		bool getSessionHandle(const boost::shared_ptr<ACS_DSD_Session>& session, ACE_HANDLE& handle) const;

		/**
		 * @brief
		 *
		 * This method checks whether it is possible manage an additional session
		 *
		 * @return true on success otherwise false
		 *
		 */
		bool isSessionSlotAvailable();

		int32_t m_cp_system_id;

		std::string m_dataSourceName;

		std::string m_CpName;

		std::string m_channelInfo;

		ACE_TP_Reactor m_reactorImpl;

		ACE_Reactor	m_reactor;

		typedef std::pair<boost::shared_ptr<ACS_DSD_Session>, time_t> mtapSessionInfo_t;
		typedef std::map<ACE_HANDLE, mtapSessionInfo_t> activeMtapSessions_t;

		//list of sessions. One single CP can open 4 connections on IPNA and 4 more on IPNB
		activeMtapSessions_t m_activeSessions;

		/**
		 * 	@brief m_activeSessionsMutex
		 *
		 * 	Mutex to synch the access to the m_activeSessions attribute
		 * 	that can be accessed at the same time by
		 * 	- the Main Reactor through the DataSource::handle_input() which invokes the addSession
		 * 	- the Data Channel reactor by either handle_input() or handle_close()
		 *
		 */
		ACE_Recursive_Thread_Mutex m_activeSessionsMutex;

		PDUHandler m_pduHandler;
	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_DATACHANNEL_H_ */
