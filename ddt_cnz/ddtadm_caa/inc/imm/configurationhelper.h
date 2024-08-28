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

#ifndef DDTADM_CAA_INC_IMM_CONFIGURATIONHELPER_H_
#define DDTADM_CAA_INC_IMM_CONFIGURATIONHELPER_H_

#include "operation/operation.h"
#ifndef CUTE_TEST
	#include <ACS_CC_Types.h>
	#include <ace/Singleton.h>
	#include <ace/RW_Thread_Mutex.h>
#else
	#include "stubs/imm_stubs.h"
	#include "stubs/ACE_Singleton_stub.h"
	#include "stubs/ACE_RW_Thread_Mutex_stub.h"
#endif
#include <string>
#include <map>
#include <bitset>


namespace imm
{

	struct ConfigurationError
	{
		common::errorCode::ErrorConstants errorCode;
		std::string errorMessage;
	};

	/**
	 * @class ConfigurationHelper
	 *
	 * @brief
	 *  This class helps to manage the data sink configuration
	 *
	 * @sa ACE_Singleton
	 */
	class ConfigurationHelper
	{
	 public:

		/** @brief
		 *
		 *	Managed Objects handled from this configuration helper
		 *
		 *	@remarks Remarks
		 */
		enum MOtype_t
		{
			DataSource = 0,
			DataSink,
			FilePeer,
			BlockPeer,
			OutputFormat,
			FileFormat,
			DoublePeer,

			Failed,	// configuration status
			Valid,

			NUMBER_OF_ELEMENTS // must be last
		};


		friend class ACE_Singleton<ConfigurationHelper, ACE_Recursive_Thread_Mutex>;

		/** @brief
		 *
		 *	This method removes a configuration
		 *
		 *  @param dataSourceName: data source to configure
		 *
		 *  @param configID: configuration identifier
		 *
		 *	@remarks Remarks
		 */
		void removeConfiguration(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID);

		/** @brief
		 *
		 *	This method verifies the correctness of a configuration
		 *
		 *  @param dataSourceName: data source to configure
		 *
		 *  @param configID: configuration identifier
		 *
		 *  @return true when the configuration is allowed otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool verifyConfiguration(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& operationID);

		/** @brief
		 *
		 *	This method checks if a specific MO is present into a configuration
		 *
		 *  @param dataSourceName: data source to configure
		 *
		 *  @param configID: configuration identifier
		 *
		 *  @param element: MO to search
		 *
		 *  @return true when the MO is present otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool exist(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, MOtype_t element);

		/** @brief
		 *
		 *	This method checks FileFormat MO is present into a configuration or not
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@return true when the MO is present otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool isFileFormatMissing(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID);

		/** @brief
		 *
		 *	This method executes MO operation.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void executeMoOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID);

		/** @brief
		 *
		 *	This method gets error message.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		std::string getErrorMessage(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID);

		/** @brief
		 *
		 *	This method gets error code.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		common::errorCode::ErrorConstants getErrorCode(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID);

		/** @brief
		 *
		 *	This method schedules create operation.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@param element: type of MO
		 *
		 *	@param opId: type of operation.
		 *
		 *	@param MoInfo: void pointer to MO instance.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void scheduleCreateOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element, const operation::identifier_t& opId, const void *MoInfo);

		/** @brief
		 *
		 *	This method schedules delete operation.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@param element: type of MO
		 *
		 *	@param opId: type of operation.
		 *
		 *	@param MoInfo: void pointer to MO instance.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void scheduleDeleteOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element, const operation::identifier_t& opId, const void *MoInfo);

		/** @brief
		 *
		 *	This method schedules modify operation.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@param element: type of MO
		 *
		 *	@param opId: type of operation.
		 *
		 *	@param MoInfo: void pointer to MO instance.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void scheduleModifyOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element, const operation::identifier_t& opId, const void *MoInfo);

		/** @brief
		 *
		 *	This method gets the complete DN in COM format.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@param opId: type of operation.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		std::string getFullCOMDN(const std::string & dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& opId);

	 private:

		//--------------------------------------------------
		// mapOfConfigurations_t
		//		KEY = {dataSourceName, configID}
		//		VALUE = {BIT MASK}>
		//--------------------------------------------------
		typedef std::map<std::pair<std::string, ACS_APGCC_CcbId>, std::bitset<NUMBER_OF_ELEMENTS> > mapOfConfigurations_t;

		//--------------------------------------------------
		// mapOfInternalErrors_t
		//		KEY = {dataSourceName, configID}
		//		VALUE = {Configuration Error}>
		//--------------------------------------------------
		typedef std::map<std::pair<std::string, ACS_APGCC_CcbId>, ConfigurationError > mapOfConfigurationErrors_t;

		/// internal map of configurations: keeps in memory the list of all create configuration requests related to the same Data Source.
		mapOfConfigurations_t m_createSessions;

		/// internal map of configurations: keeps in memory the list of all modify configuration requests related to the same Data Source.
		mapOfConfigurations_t m_modifySessions;

		/// internal map of configurations: keeps in memory the list of all delete configuration requests related to the same Data Source.
		mapOfConfigurations_t m_deleteSessions;


		/// internal map of configuration errors
		mapOfConfigurationErrors_t m_configurationErrors;

		//------------------------------------------------------------------------------------------------------------------------
		// command_t
		//		pair { global Operation Identifier; pointer to the helper struct such as outputFormat, blockPeer, filePeer...}
		//------------------------------------------------------------------------------------------------------------------------
		typedef std::pair<operation::identifier_t, const void*> command_t;

		//---------------------------------------------
		// mapOfCommands_t
		//		KEY: pair {dataSourceName, CCB Id}
		//		VALUE: pair command_t
		//---------------------------------------------
		typedef std::multimap<std::pair<std::string, ACS_APGCC_CcbId>, command_t > mapOfCommands_t;

		/// internal map of configurations
		mapOfCommands_t m_commands;

		///  Constructor.
		ConfigurationHelper();

		///  Destructor.
		virtual ~ConfigurationHelper();

		/** @brief
		 *
		 *	This method verifies the correctness of a create configuration
		 *
		 *  @param configurationMask: configuration content expressed as a bitmask
		 *
		 *  @return true when the configuration is allowed otherwise false.
		 *
		 *	@remarks Remarks
		 */
		common::errorCode::ErrorConstants verifyCreate(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName);

		/** @brief
		 *
		 *	This method verifies the correctness of a create configuration
		 *
		 *  @param configurationMask: configuration content expressed as a bitmask
		 *
		 *  @return true when the configuration is allowed otherwise false.
		 *
		 *	@remarks Remarks
		 */
		common::errorCode::ErrorConstants verifyModify(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName);

		/** @brief
		 *
		 *	This method verifies the correctness of attributes value between MOCs
		 *
		 *  @param configurationMask: configuration content expressed as a bitmask
		 *
		 *  @param dataSourceName: data source to configure
		 *
		 *  @param configID: configuration identifier
		 *
		 *  @return true when the configuration is allowed otherwise false.
		 *
		 *	@remarks Remarks
		 */
		common::errorCode::ErrorConstants verifyAttributesValue(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName, const ACS_APGCC_CcbId& configID);

		/** @brief
		 *
		 *	This method verifies the correctness of a delete configuration
		 *
		 *  @param configurationMask: configuration content expressed as a bitmask
		 *
		 *  @return true when the configuration is allowed otherwise false.
		 *
		 *	@remarks Remarks
		 */
		common::errorCode::ErrorConstants verifyDelete(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName, const ACS_APGCC_CcbId& configID);

		/** @brief
		 *
		 *	This method adds a new MO creation to a configuration session,
		 *	the configuration will be created if it is not exist.
		 *
		 *  @param dataSourceName: data source to configure
		 *
		 *  @param configID: configuration identifier
		 *
		 *  @param element: MO to add to the configuration
		 *
		 *	@remarks Remarks
		 */
		void addToCreateSession(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element);

		/** @brief
		 *
		 *	This method adds a new MO modification to a configuration session,
		 *	the configuration will be created if it is not exist.
		 *
		 *  @param dataSourceName: data source to configure
		 *
		 *  @param configID: configuration identifier
		 *
		 *  @param element: MO to add to the configuration
		 *
		 *	@remarks Remarks
		 */
		void addToModifySession(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element);

		/** @brief
		 *
		 *	This method adds a new MO deletion to a configuration session,
		 *	the configuration will be created if it is not exist.
		 *
		 *  @param dataSourceName: data source to configure
		 *
		 *  @param configID: configuration identifier
		 *
		 *  @param element: MO to add to the configuration
		 *
		 *  @return true when the configuration is allowed otherwise false.
		 *
		 *	@remarks Remarks
		 */
		void addToDeleteSession(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element);

		/**
		 *	@brief	verify DataSource Parameters before creating a DataSource which is called by configuration helper during complete action
		 *
		 *  @param dataSourceName the name of data source
		 *
		 *	@return the error code.
		 */

		common::errorCode::ErrorConstants verifyCreateDataSource(const std::string & dataSourceName);

		/**
		 *	@brief This method sends the given command using scheduler
		 *
		 *  @param id : operation identifier
		 *
		 *  @param op_info : operation info.
		 *
		 *	@return none.
		 */
		void sendCommand(const operation::identifier_t& id, const void* op_info );

		/**
		 *	@brief This method finds and executes the given command
		 *
		 *	@param commandRange : iterator to commands
		 *
		 *	@param id : operation identifier
		 *
		 *	@return none.
		 */
		void findAndExecute(std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator>& commandRange, const operation::identifier_t& id);

		/**
		 *	@brief This method sets the configuration error.
		 *
		 *	@param dataSourceName : name of the data source
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@param errorNumber : error number
		 *
		 *	@param id : operation identifier
		 *
		 *	@return none.
		 */
		void setConfigurationError(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const common::errorCode::ErrorConstants errorNumber, const operation::identifier_t& opId);

		/** @brief
		 *
		 *	This method schedules MO operation.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@param opId: type of operation.
		 *
		 *	@param MoInfo: void pointer to MO instance.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		void scheduleMoOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& opId, const void *MoInfo);

		/** @brief
		 *
		 *	This method gets command.
		 *
		 *	@param dataSourceName: data source to configure
		 *
		 *	@param configID: configuration identifier
		 *
		 *	@param opId: type of operation.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		const void* getCommand(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& opId);

	};

	typedef ACE_Singleton<ConfigurationHelper, ACE_Recursive_Thread_Mutex> configurationHelper_t;

} /* namespace imm */

#endif /* DDTADM_CAA_INC_IMM_CONFIGURATIONHELPER_H_ */
