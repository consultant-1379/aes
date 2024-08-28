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

#ifndef DDTADM_CAA_INC_ALARM_ALARMHANDLER_H_
#define DDTADM_CAA_INC_ALARM_ALARMHANDLER_H_

#include "alarm/alarm.h"
#include "acs_aeh_evreport.h"

#include <boost/noncopyable.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <map>
#include <string>

namespace alarms
{

	/**
	 * @class AlarmHandler
	 *
	 * @brief
	 *
	 * This class provides the methods to raise and cease alarms.
	 *
	 * @sa boost::noncopyable
	 */
	class AlarmHandler : private boost::noncopyable
	{
	public:

		///  Constructor
		AlarmHandler();

		///  Destructor
		virtual ~AlarmHandler();

		/** @brief
		 *
		 *	This method raises the data source warning alarm.
		 *
		 *	@remarks Remarks
		 */
		void raiseDataSourceWarning(const std::string& objOfReference, const int& threshold);

		/** @brief
		 *
		 *	This method ceases the data source warning alarm.
		 *
		 *	@remarks Remarks
		 */
		void ceaseDataSourceWarning(const std::string& objOfReference);

		/** @brief
		 *
		 *	This method raises the connection fault alarm.
		 *
		 *	@remarks Remarks
		 */
		void raiseConnectionFaultAlarm(const std::string& objOfReference);

		/** @brief
		 *
		 *	This method ceases the connection fault alarm.
		 *
		 *	@remarks Remarks
		 */
		void ceaseConnectionFaultAlarm(const std::string& objOfReference);

		/** @brief
		 *
		 *	This method ceases all the alarms raised by service.
		 *	Expected to be called during service shutdown.
		 *
		 *	@remarks Remarks
		 */
		void ceaseAll();

	private:

		/** @brief
		 *
		 *	This method raises alarm using the provided alarm data.
		 *
		 *	@param alarmData : boosts::shared_ptr for Alarm class.
		 *
		 *	@return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool raise(boost::shared_ptr<Alarm> alarmData );

		/** @brief
		 *
		 *	This method ceases the alarm using the provided alarm data.
		 *
		 *	@param alarmData : boosts::shared_ptr for Alarm class.
		 *
		 *	@return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool cease(boost::shared_ptr<Alarm> alarmData );

		/** @brief
		 *
		 *	This method checks for alarm already present or not,
		 *	with a specific object of reference and alarm identifier.
		 *
		 *	@param objRef : Alarm object of reference.
		 *
		 *	@param specificProblem : Alarm identifier.
		 *
		 *	@param alarmObj : boosts::shared_ptr for Alarm class.
		 *
		 *	@return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool exists(const std::string& objRef, const Alarm::Type& type, boost::shared_ptr<Alarm>& alarmData);

		///process name
		std::string m_processName;

		//Instance of acs_aeh_evreport class which provides APIs to raise and cease alarms.
		acs_aeh_evreport m_evrep;

		///map key composed by the pair : <objectOfReference, alarmType>
		typedef std::pair<std::string, Alarm::Type> alarmKey_t;

		///to store Alarm object for each pair of <dataSourceName,alarmIdentifier>
		typedef std::map<alarmKey_t, boost::shared_ptr<Alarm> > mapOfAlarms_t;

		///Map instance to store Alarm object for each pair of <dataSourceName,alarmIdentifier>
		mapOfAlarms_t m_alarms;

		// To synchronize alarm map access
		boost::recursive_mutex m_mutex;
	};

} /* namespace alarms */

#endif /* DDTADM_CAA_INC_ALARM_ALARMHANDLER_H_ */
