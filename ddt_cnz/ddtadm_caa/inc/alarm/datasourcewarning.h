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

#ifndef DDTADM_CAA_SRC_ALARM_DATASOURCEWARNING_H_
#define DDTADM_CAA_SRC_ALARM_DATASOURCEWARNING_H_
#include "alarm/alarm.h"
namespace alarms {

/**
 * @class DataSourceWarning
 *
 * @brief
 *
 * This class stores the information about data source warning alarm.
 *
 */
class DataSourceWarning: public Alarm {
public:
	/**
	 * @brief
	 * This method is a Constructor that sets the object of reference and
	 * threshold to a given values.
	 *
	 * @param objOfReference: object of reference
	 * @param threshold: threshold value for Data Source warning
	 */
	DataSourceWarning(const std::string& objOfReference, const int& threshold);

	///Destructor
	virtual ~DataSourceWarning();
};

} /* namespace alarms */

#endif /* DDTADM_CAA_SRC_ALARM_DATASOURCEWARNING_H_ */
