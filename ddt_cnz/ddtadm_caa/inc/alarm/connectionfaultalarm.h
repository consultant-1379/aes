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

#ifndef DDTADM_CAA_INC_ALARM_CONNECTIONFAULTALARM_H_
#define DDTADM_CAA_INC_ALARM_CONNECTIONFAULTALARM_H_

#include "alarm.h"

namespace alarms
{
/**
 * @class ConnectionFaultAlarm
 *
 * @brief
 *
 * This class stores the information of about Connection fault alarm.
 *
 */
class ConnectionFaultAlarm: public Alarm
{
public:
	/**
	 * @brief
	 * This is a Constructor that sets the object name to a given value.
	 *
	 * @param peerMoDN: DN of peer MO
	 */
	ConnectionFaultAlarm(const std::string& peerMoDN);

	///	Destructor
	virtual ~ConnectionFaultAlarm();
};

} /* namespace alarms */

#endif /* DDTADM_CAA_INC_ALARM_CONNECTIONFAULTALARM_H_ */
