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
#ifndef DDTADM_CAA_INC_COMMON_CONFIGURATION_H_
#define DDTADM_CAA_INC_COMMON_CONFIGURATION_H_

#include <string>
#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>
#include "common/programconstants.h"

namespace common
{
	/**
	 * @class Configuration
	 *
	 * @brief
	 *
	 * This class stores the configuration information details like cluster name, hw version,
	 * max usable memory...
	 *
	 */
	class Configuration
	{
	 public:

	 		friend class ACE_Singleton<common::Configuration, ACE_Recursive_Thread_Mutex>;

	 		/** @brief	getClusterName
	 		 *
	 		 *  This method gets the Cluster Name of the node
	 		 *
	 		 *  @return the cluster name
	 		 *
	 		 *  @remarks Remarks
	 		 */
	 		std::string getClusterName();

	 		/** @brief	getCPName
	 		 *
	 		 *  This method gets the name of CP
	 		 *
	 		 *  @param cp_system_id: id of cp blade
	 		 *
	 		 *  @return the name of the CP on success otherwise a default value
	 		 *
	 		 *  @remarks Remarks
	 		 */
	 		std::string getCPName(int32_t cp_system_id);

	 		/** @brief	getHWVersionInfo
	 		 *
	 		 *  This method gets the underlying hardwaType
	 		 *  @param hwVer is reference sets the enum HwVersionConstants based on GEP1/GEP2/GEP5/VM
	 		 *
	 		 *  @return true if success
	 		 *
	 		 *  @remarks Remarks
	 		 */
	 		 bool getHWVersionInfo(common::HwVerConstants &hwVer);

	 		 /** @brief	getMaxUsableMemory
	 		  *
	 		  *  This method gets the max usable memory.
	 		  *
	 		  *  @return max usable memory based on GEP1/GEP2/GEP5/VM
	 		  *
	 		  *  @remarks Remarks
	 		  */
	 		 uint64_t getMaxUsableMemory();

	 private:

	 		///constructor
	 		Configuration();

	 		///Destructor
	 		virtual ~Configuration();

	 		// INHIBIT COPY CONTRUCTOR
	 		Configuration(const Configuration& rhs);

	 		// INHIBIT ASSIGNMENT OPERATOR
	 		Configuration& operator=(const Configuration& rhs);

	 		/// mutex - to take exclusive access to internal data structures
	 		ACE_Recursive_Thread_Mutex m_cs_mutex;

	 		/// cluster name
	 		std::string m_clusterName;

	 		//store the hardwareVersion
	 		common::HwVerConstants m_hwVersion;

	 		/// max usable memory in bytes
	 		uint64_t m_maxUsableMemory;

	};

	typedef ACE_Singleton< Configuration, ACE_Recursive_Thread_Mutex> configuration_t;

} /* namespace common */

#endif /* DDTADM_CAA_INC_COMMON_CONFIGURATION_H_ */
