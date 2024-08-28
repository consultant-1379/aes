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

#ifndef DDTADM_CAA_INC_ENGINE_TIMER_H_
#define DDTADM_CAA_INC_ENGINE_TIMER_H_

namespace engine
{

	/**
	 * @class Timer
	 *
	 * @brief This class provides concrete implementation to manage
	 *
	 * the Timer operations.
	 *
	 */
	class Timer
	{
		enum TimerStatus
		{
			DISARMED = 0,
			ARMED = 1
		};

	public:

		/// Constructor.
		Timer();

		/// Destructor.
		virtual ~Timer();

		/**	@brief
		 *
		 *	This method starts the timer.
		 *
		 *	@param timeSec : seconds
		 *
		 *	@return status of timer.
		 *
		 *	@remarks Remarks
		 */
		TimerStatus start(int timeSec);

		/**	@brief
		 *
		 *	This method stops the timer.
		 *
		 *	@return status of timer.
		 *
		 *	@remarks Remarks
		 */
		TimerStatus stop();

		/**	@brief
		 *
		 *	This method gets the handle.
		 *
		 *	@return timer handle.
		 *
		 *	@remarks Remarks
		 */
		int getHandle() const {return m_handle;};

		/**	@brief
		 *
		 *	This method checks the timer is ARMED or not.
		 *
		 *	@return true if timer is ARMED, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool isArmed() const { return (ARMED == m_status); };

		/**	@brief
		 *
		 *	This method checks the timer is DISARMED or not.
		 *
		 *	@return true if timer is DISARMED, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool isDisarmed() const { return (DISARMED == m_status); };

		/**	@brief
		 *
		 *	This method checks the timer is expired or not.
		 *
		 *	@return true if timer is expired, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool isExpired();

	private:

		/**	@brief
		 *
		 *	This method creates the timer handle.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void createHandle();

		/// timer handle
		int m_handle;

		/// timer status
		TimerStatus m_status;

	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_TIMER_H_ */
