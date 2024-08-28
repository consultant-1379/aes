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
#ifndef DDTADM_CAA_INC_AES_DDT_WORKINGSET_H_
#define DDTADM_CAA_INC_AES_DDT_WORKINGSET_H_

#include "engine/datasourcemanager.h"
#include "imm/handler.h"
#include "operation/scheduler.h"
#include "store/storingmanager.h"
#include "alarm/alarmhandler.h"

#ifndef CUTE_TEST
	#include <ace/TP_Reactor.h>
	#include <ace/Reactor.h>
	#include <ace/Singleton.h>
	#include <ace/Synch.h>
	#include <ace/RW_Thread_Mutex.h>
#else
	#include "stubs/ACE_Task_Base_stub.h"
#endif

namespace engine
{

	class WorkingSet
	{
	 public:

		friend class ACE_Singleton<WorkingSet, ACE_Recursive_Thread_Mutex>;

		/**	@brief
		 *
		 *	This method gets the scheduler associated with working set.
		 *
		 *	@return scheduler.
		 *
		 *	@remarks Remarks
		 */
		inline operation::Scheduler& getScheduler()  { return m_scheduler; };

		/**	@brief
		 *
		 *	This method starts the scheduler.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		inline int startScheduler() { return m_scheduler.start(); };

		/**	@brief
		 *
		 *	This method stops the scheduler.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		inline int stopScheduler() { return m_scheduler.stop(); };

		/**	@brief
		 *
		 *	This method gets the main reactor.
		 *
		 *	@return reactor.
		 *
		 *	@remarks Remarks
		 */
		inline ACE_Reactor& getMainReactor()  { return m_reactor; };

		/**	@brief
		 *
		 *	This method stops the main reactor.
		 *
		 *	@return zero on success, otherwise non zero value.
		 *
		 *	@remarks Remarks
		 */
		inline int stopMainReactor() { return m_reactor.end_reactor_event_loop(); };

		/**	@brief
		 *
		 *	This method checks debug mode is on or not.
		 *
		 *	@return true if debug mode is on, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		inline bool isDebugModeOn() const { return m_debugMode; };

		/**	@brief
		 *
		 *	This method sets debug mode is on.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		inline void setDebugModeOn() {m_debugMode = true; };

		/**	@brief
		 *
		 *	This method sets debug mode is off.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		inline void setDebugModeOff() {m_debugMode = false; };

		/**	@brief
		 *
		 *	This method registers implementers on all DDT MOs.
		 *
		 *	@return zero on success, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		inline int registerObjectImplementers() { return m_immHandler.take_ownership(); };

		/**	@brief
		 *
		 *	This method unregisters implementers on all DDT MOs.
		 *
		 *	@return zero on success, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		inline int unregisterObjectImplementers() { return m_immHandler.release_ownership(); };

		/**	@brief
		 *
		 *	This method unregisters implementers on all DDT MOs.
		 *
		 *	@return zero on success, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		inline engine::DataSourceManager& getDataSourceManager()  { return m_dataSourceManager; };

		/**	@brief
		 *
		 *	This method gets the storing manager.
		 *
		 *	@return storing manager.
		 *
		 *	@remarks Remarks
		 */
		inline store::StoringManager& getStoringManager()  { return m_storingManager; };

		/**	@brief
		 *
		 *	This method gets the alarm handler.
		 *
		 *	@return alarm handler.
		 *
		 *	@remarks Remarks
		 */
		inline alarms::AlarmHandler& getAlarmhandler() { return m_alarmHandler; };

	 private:

		/// Constructor.
		WorkingSet();

		/// Destructor.
		virtual ~WorkingSet();

		// INHIBIT COPY CONTRUCTOR
		WorkingSet(const WorkingSet& rhs);

		// INHIBIT ASSIGNMENT OPERATOR
		WorkingSet& operator=(const WorkingSet& rhs);


	 private:

		/// debug mode
		bool m_debugMode;

		/// Scheduler
		operation::Scheduler m_scheduler;

		/// IMM handler
		imm::OI_Handler m_immHandler;

		/// Data Source Manager
		engine::DataSourceManager m_dataSourceManager;

		/// Storing Manager
		store::StoringManager m_storingManager;

		/// TP Reactor
		ACE_TP_Reactor m_reactorImpl;

		/// Reactor
		ACE_Reactor	m_reactor;

		/// Alarm Handler
		alarms::AlarmHandler m_alarmHandler;

	};

	typedef ACE_Singleton< WorkingSet, ACE_Recursive_Thread_Mutex> workingSet_t;
} /* namespace operation */



#endif /* DDTADM_CAA_INC_AES_DDT_WORKINGSET_H_ */
