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
#ifndef DDTADM_CAA_INC_AES_DDT_DATASOURCE_H_
#define DDTADM_CAA_INC_AES_DDT_DATASOURCE_H_

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <map>

#ifndef CUTE_TEST
	#include "mtap/datachannel.h"
	#include <ace/config-macros.h>
	#include "ACS_DSD_Server.h"
	#include <ace/Task_T.h>
	#include <ace/Synch.h>
	#include <ace/Thread_Mutex.h>
	#include <ace/Handle_Set.h>

#else
	#include "stubs/operation_stub.h"
	#include "stubs/mtap_stubs.h"
#endif
namespace engine
	{

	/**
	 * @class AES_DDT_DataSource
	 *
	 * @brief The AES_DDT_DataSource class is derived from @c ACE_Task_Base.
	 *
	 * Manages the DataSource MO life cycle.
	 *
	 */
	class DataSource : public ACE_Task<ACE_MT_SYNCH>, private boost::noncopyable
	{
#ifdef CUTE_TEST
		friend class DataSourceTest;
#endif
	public:
		typedef std::map< unsigned int, boost::shared_ptr<mtap::DataChannel> > dataChannelMap_t;

		/// Constructor.
		DataSource(const std::string &name, const uint32_t& recordSize, const std::string& dataSourceDN);

		/// Destructor.
		virtual ~DataSource();

		/**
		 * @brief
		 * This method gets the record size of the data source
		 */
		uint32_t getRecordSize() const { return m_recordSize; }

		/**
		 * @brief Activates the DataSource thread.
		 */
		virtual int open(void *args = 0);

		/**
		 *	@brief DataSource life cycle thread.
		 */
		virtual int svc(void);

		 /**
		  * @brief	Initializes the scheduler task and prepare it to run as thread.
		  */
		virtual int start();

		/**
		 * @brief Stops the thread execution and waits for the thread termination.
		*/
		virtual int stop();

		/**
		 * @brief remove
		 * Remove the dataSource.
		*/
		int remove();

		/**
		 * @brief	Get the data source name
		 */
		inline const char* getDataSourceName() const { return m_messageStore.c_str(); }

		//--------------------------
		// INVOKED BY ACE REACTOR
		/**
		 * @brief handle_timeout method
		 *	This method is called when a scheduled timeout expires
		 */
		virtual int handle_timeout(const ACE_Time_Value& tv, const void*);

		/**
		 * @brief handle_close method
		 * This method is called when event handler is removed from the <ACE_Reactor>.
		 * This is invoked on shutdown/remove when removing handles.
		 */
		virtual int handle_close (ACE_HANDLE fd, ACE_Reactor_Mask mask);

		/**
		 * @brief handle_input method
		 * This method is called when input becomes available, which means every time CP connects to the Data Source listening port.
		 * Connected sockets are added to the mtap::DataChannel
		 */
		virtual int handle_input (ACE_HANDLE fd);

	private:

		/**
		 * @brief setupMtapServer
		 * Setup the DataSource as DSD Server
		 */
		int setupMtapServer();

		/**
		 * @brief registerDsdHandles
		 * Set DSD handles being signalled on CP connections
		 */
		int registerDsdHandles();

		/**
		 * @brief getMTAPChannelManager
		 * Get/Create a Data Channel for a specific cp_system_id.
		 */
		boost::shared_ptr<mtap::DataChannel> getMTAPChannelManager(int32_t cp_system_id);

		/**
		 * @brief createDataChannel
		 * Create a DataChannel
		 */
		boost::shared_ptr<mtap::DataChannel> createDataChannel(int32_t cp_system_id);

		/**
		 * @brief createTransferProgressMO
		 * Create a TransferProgress MO
		 */
		bool createTransferProgressMO(const char* cpName);

		/**
		 * @brief stopDataChannels
		 * Stops DataChannels
		 */
		void stopDataChannels();

		/**
		 * 	@brief	m_messageStore
		 *
		 * 	Name of the Data Source being published to DSD
		 */
		std::string m_messageStore;

		/**
		 * 	@brief	m_recordSize
		 *
		 * 	record size in byte of the data source
		 */
		uint32_t m_recordSize;

		/**
		 * 	@brief	m_mtapServer
		 *
		 * 	DSD Server used to publish as m_messageStore service and to wait for CP connections
		 */
		ACS_DSD_Server m_mtapServer;

		/**
		 * 	@brief	m_dsdHandleSet
		 *
		 * 	Set of handles signalled on CP connections
		 */
		ACE_Handle_Set m_dsdHandleSet;

		/**
		 * 	@brief	m_dataChannelMap
		 *
		 * 	Map containing DataChannel objects
		 */
		dataChannelMap_t m_dataChannelMap;

		/**
		 * 	@brief	m_dataSourceDN
		 *
		 * 	DataSource DN
		 */
		std::string m_dataSourceDN;

		/**
		 * 	@brief	m_timerId
		 */
		long m_timerId;

		/**
		 * 	@brief	m_mutex
		 *
		 * 	Mutex for handle_timeout synchronization
		 */
		ACE_Thread_Mutex m_mutex;

	};
}

#endif /* DDTADM_CAA_INC_AES_DDT_DATASOURCE_H_ */
