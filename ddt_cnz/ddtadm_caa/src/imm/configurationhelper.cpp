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

#include "imm/configurationhelper.h"
#include "imm/imm.h"

#include "engine/context.h"
#include "common/utility.h"
#include "engine/templatehandler.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/macros.h"
	#include "common/configuration.h"
	#include "imm/objectmanagerhelper.h"
	#include "engine/workingset.h"
	#include "operation/creator.h"
#else
	#include "stubs/operation_stub.h"
	#include "stubs/creator_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_IMM_ConfigurationHelper)

namespace imm
{

	ConfigurationHelper::ConfigurationHelper()
	: m_createSessions(),
	  m_modifySessions(),
	  m_deleteSessions(),
	  m_configurationErrors(),
	  m_commands()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	ConfigurationHelper::~ConfigurationHelper()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void ConfigurationHelper::removeConfiguration(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>", dataSourceName.c_str(), configID);
		// clean create configuration mask
		m_createSessions.erase(std::make_pair(dataSourceName, configID));

		// clean modify configuration mask
		m_modifySessions.erase(std::make_pair(dataSourceName, configID));

		// clean delete configuration mask
		m_deleteSessions.erase(std::make_pair(dataSourceName, configID));

		std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator> commandRange;

		// clean command queue
		commandRange = m_commands.equal_range(std::make_pair(dataSourceName, configID));

		// Erase all commands from the commands map
		m_commands.erase(commandRange.first, commandRange.second);

		// clean errors
		m_configurationErrors.erase(std::make_pair(dataSourceName, configID));

	}

	bool ConfigurationHelper::verifyConfiguration(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& operationID)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>", dataSourceName.c_str(), configID);
		bool createResult = false;
		bool modifyResult = false;
		bool deleteResult = false;

		common::errorCode::ErrorConstants errorNumber = common::errorCode::ERR_NO_ERRORS;

		{
			//CREATE SESSION
			// search configuration
			mapOfConfigurations_t::iterator config = m_createSessions.find(std::make_pair(dataSourceName, configID));

			// True if there aren't create requests
			createResult = (m_createSessions.end() == config) || config->second.test(Valid);

			if( (m_createSessions.end() != config) && !config->second.test(Failed) && !config->second.test(Valid))
			{
				errorNumber = verifyCreate(config->second, dataSourceName);

				if( common::errorCode::ERR_NO_ERRORS == errorNumber )
				{
					errorNumber = verifyAttributesValue(config->second, dataSourceName, configID);
				}

				if( common::errorCode::ERR_NO_ERRORS == errorNumber )
				{
					createResult = true;
					config->second.set(Valid);
				}
				else
				{
					config->second.set(Failed);
					setConfigurationError(dataSourceName, configID, errorNumber, operationID);
				}
			}
		}

		{
			//MODIFY SESSION
			// search configuration
			mapOfConfigurations_t::iterator config = m_modifySessions.find(std::make_pair(dataSourceName, configID));

			// True if there aren't modify requests
			modifyResult = (m_modifySessions.end() == config) || config->second.test(Valid);

			if( (m_modifySessions.end() != config) && !config->second.test(Failed) && !config->second.test(Valid) )
			{
				errorNumber = verifyModify(config->second, dataSourceName);

				if( common::errorCode::ERR_NO_ERRORS == errorNumber )
				{
					errorNumber = verifyAttributesValue(config->second, dataSourceName, configID);
				}

				if( common::errorCode::ERR_NO_ERRORS == errorNumber )
				{
					modifyResult = true;
					config->second.set(Valid);
				}
				else
				{
					config->second.set(Failed);
					setConfigurationError(dataSourceName, configID, errorNumber, operationID);
				}
			}
		}

		{
			//DELETE SESSION
			// search configuration
			mapOfConfigurations_t::iterator config = m_deleteSessions.find(std::make_pair(dataSourceName, configID));

			// True if there aren't delete requests
			deleteResult = (m_deleteSessions.end() == config) || config->second.test(Valid);

			if( (m_deleteSessions.end() != config) && !config->second.test(Failed) && !config->second.test(Valid))
			{
				errorNumber = verifyDelete(config->second, dataSourceName, configID);

				if( common::errorCode::ERR_NO_ERRORS == errorNumber )
				{
					deleteResult = true;
					config->second.set(Valid);
				}
				else
				{
					config->second.set(Failed);
					setConfigurationError(dataSourceName, configID, errorNumber, operationID);
				}
			}
		}

		bool result = createResult && deleteResult && modifyResult;

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
		return result;
	}

	bool ConfigurationHelper::exist(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, MOtype_t element)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu> search MO:<%d>", dataSourceName.c_str(), configID, element);
		bool result = false;
		// search configuration
		mapOfConfigurations_t::const_iterator configIterator = m_createSessions.find(std::make_pair(dataSourceName, configID));

		if( m_createSessions.end() != configIterator )
		{
			result = configIterator->second.test(element);
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
		return result;
	}

	bool ConfigurationHelper::isFileFormatMissing(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>", dataSourceName.c_str(), configID);
		bool result = false;
		// search configuration
		mapOfConfigurations_t::const_iterator configIterator = m_createSessions.find(std::make_pair(dataSourceName, configID));

		if( m_createSessions.end() != configIterator )
		{
			result = configIterator->second.test(DataSink) && configIterator->second.test(FilePeer) && !configIterator->second.test(FileFormat);
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
		return result;
	}

	void ConfigurationHelper::scheduleMoOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& opId, const void *MoInfo)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>", dataSourceName.c_str(), configID);

		// enqueue command
		m_commands.insert( std::make_pair( std::make_pair(dataSourceName, configID), std::make_pair(opId, MoInfo)) );
	}

	void ConfigurationHelper::executeMoOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>", dataSourceName.c_str(), configID);

		{
			mapOfConfigurations_t::iterator config = m_createSessions.find(std::make_pair(dataSourceName, configID));

			if (m_createSessions.end() != config)
			{
				std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator> commandRange;
				commandRange = m_commands.equal_range(std::make_pair(dataSourceName, configID));

				// This is the creation Order from the Data Source to the Peers.
				if(config->second.test(DataSource))
				{
					findAndExecute(commandRange, operation::DATASOURCE_ADD);
				}

				if(config->second.test(OutputFormat))
				{
					findAndExecute(commandRange, operation::OUTPUTFORMAT_ADD);
				}

				if(config->second.test(FileFormat))
				{
					findAndExecute(commandRange, operation::FILEFORMAT_ADD);
				}

				if(config->second.test(FilePeer))
				{
					findAndExecute(commandRange, operation::FILEPEER_ADD);
				}

				if(config->second.test(BlockPeer))
				{
					findAndExecute(commandRange, operation::BLOCKPEER_ADD);
				}

				if(config->second.test(DataSink))
				{
					findAndExecute(commandRange, operation::DATASINK_ADD);
				}
			}
		}

		{
			mapOfConfigurations_t::iterator config = m_modifySessions.find(std::make_pair(dataSourceName, configID));

			if (m_modifySessions.end() != config)
			{
				std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator> commandRange;
				commandRange = m_commands.equal_range(std::make_pair(dataSourceName, configID));

				// This is the creation Order from the Data Source to the Peers.
				if(config->second.test(OutputFormat))
				{
					findAndExecute(commandRange, operation::OUTPUTFORMAT_MOD);
				}

				if(config->second.test(FileFormat))
				{
					findAndExecute(commandRange, operation::FILEFORMAT_MOD);
				}

				if(config->second.test(DataSink))
				{
					findAndExecute(commandRange, operation::DATASINK_MOD);
				}

				if(config->second.test(FilePeer))
				{
					findAndExecute(commandRange, operation::FILEPEER_MOD);
				}

			}
		}

		{
			mapOfConfigurations_t::iterator config = m_deleteSessions.find(std::make_pair(dataSourceName, configID));

			if (m_deleteSessions.end() != config)
			{
				std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator> commandRange;
				commandRange = m_commands.equal_range(std::make_pair(dataSourceName, configID));

				// This is the delete Order from the Data Source to the Peers.
				if(config->second.test(DataSink))
				{
					findAndExecute(commandRange, operation::DATASINK_DEL);
				}

				if(config->second.test(FilePeer) && !config->second.test(DataSink))
				{
					findAndExecute(commandRange, operation::FILEPEER_DEL);
				}

				if(config->second.test(BlockPeer) && !config->second.test(DataSink))
				{
					findAndExecute(commandRange, operation::BLOCKPEER_DEL);
				}

				if(config->second.test(DataSource))
				{
					findAndExecute(commandRange, operation::DATASOURCE_DEL);
				}
			}
		}

		//Now Clean up everything
		removeConfiguration(dataSourceName, configID);
	}

	std::string ConfigurationHelper::getErrorMessage(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID)
	{
		AES_DDT_TRACE_FUNCTION;

		mapOfConfigurationErrors_t::iterator element = m_configurationErrors.find(std::make_pair(dataSourceName, configID));

		if( m_configurationErrors.end() != element )
		{
			return element->second.errorMessage;
		}
		else
		{
			//Oops: error message not found!!! Return a generic one
			return imm::comcli_errormessage::GENERIC;
		}
	}

	common::errorCode::ErrorConstants ConfigurationHelper::getErrorCode(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID)
	{
		AES_DDT_TRACE_FUNCTION;

		mapOfConfigurationErrors_t::iterator element = m_configurationErrors.find(std::make_pair(dataSourceName, configID));

		if( m_configurationErrors.end() != element )
		{
			return element->second.errorCode;
		}
		else
		{
			//Oops: error code not found!!! Return a generic one
			return common::errorCode::ERR_GENERIC;
		}
	}

	void ConfigurationHelper::scheduleCreateOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element, const operation::identifier_t& opId, const void *MoInfo)
	{
		addToCreateSession(dataSourceName, configID, element);
		scheduleMoOperation(dataSourceName, configID, opId, MoInfo);
	}

	void ConfigurationHelper::scheduleDeleteOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element, const operation::identifier_t& opId, const void *MoInfo)
	{
		addToDeleteSession(dataSourceName, configID, element);
		scheduleMoOperation(dataSourceName, configID, opId, MoInfo);
	}

	void ConfigurationHelper::scheduleModifyOperation(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t element, const operation::identifier_t& opId, const void *MoInfo)
	{
		addToModifySession(dataSourceName, configID, element);
		scheduleMoOperation(dataSourceName, configID, opId, MoInfo);
	}

	void ConfigurationHelper::findAndExecute(std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator>& commandRange, const operation::identifier_t& id)
	{
		AES_DDT_TRACE_MESSAGE("EXECUTING OPERATION: <%d>", id);

		//for each operation found
		for(mapOfCommands_t::iterator element = commandRange.first; element != commandRange.second; ++element)
		{
			command_t command = element->second;
			if (id == command.first)
			{
				sendCommand(command.first, command.second);
				AES_DDT_TRACE_MESSAGE("EXECUTED OPERATION: <%d>", id);

				// !!! DON'T PUT A BREAK HERE !!!
				// IN CASE OF FILE PEER OR BLOCK PEER, 2 COMMAND COULD BE PRESENT
			}
		}
	}

	common::errorCode::ErrorConstants ConfigurationHelper::verifyCreate(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName)
	{
		AES_DDT_TRACE_FUNCTION;
		common::errorCode::ErrorConstants errorNumber = common::errorCode::ERR_NO_ERRORS;

		// check for datasource related errors
		if( configurationMask.test(DataSource) )
		{
			errorNumber = verifyCreateDataSource(dataSourceName);

			if(common::errorCode::ERR_NO_ERRORS != errorNumber)
			{
				return errorNumber;
			}

		}

		// check if a new data sink has been defined
		if( configurationMask.test(DataSink) )
		{
			// check if there is at least one child defined
			if( !(configurationMask.test(FilePeer) | configurationMask.test(BlockPeer)) )
			{
				errorNumber = common::errorCode::ERR_IMM_NOT_ENOUGH_PEER;
				return errorNumber;
			}

			// check if the kind of defined children are valid
			if( configurationMask.test(FilePeer) && configurationMask.test(BlockPeer) )
			{
				errorNumber = common::errorCode::ERR_IMM_PEER_TYPE_MISMATCH;
				return errorNumber;
			}

			if( configurationMask.test(FilePeer) && !configurationMask.test(FileFormat) )
			{
				errorNumber = common::errorCode::ERR_IMM_MISSING_FILE_FORMAT;
				return errorNumber;
			}

			if( configurationMask.test(FileFormat) && configurationMask.test(BlockPeer) )
			{
				errorNumber = common::errorCode::ERR_IMM_FILE_FORMAT_NOT_ALLOWED;
				return errorNumber;
			}

		}

		// One File Format is being added into the already configured Data Sink
		if( !configurationMask.test(DataSink) && configurationMask.test(FileFormat))
		{
			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);
			if (context->countBlockPeers())
			{
				errorNumber = common::errorCode::ERR_IMM_FILE_FORMAT_NOT_ALLOWED;
				return errorNumber;
			}
		}

		// One File Peer is being added into the already configured Data Sink
		if( !configurationMask.test(DataSink) && configurationMask.test(FilePeer))
		{
			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);
			if (context->countBlockPeers())
			{
				errorNumber = common::errorCode::ERR_IMM_FILE_PEER_NOT_ALLOWED;
				return errorNumber;
			}
		}

		// One Block Peer is being added into the already configured Data Sink
		if( !configurationMask.test(DataSink) && configurationMask.test(BlockPeer))
		{
			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);
			if (context->countFilePeers())
			{
				errorNumber = common::errorCode::ERR_IMM_BLOCK_PEER_NOT_ALLOWED;
				return errorNumber;
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%d>", errorNumber);
		return errorNumber;;
	}

	common::errorCode::ErrorConstants ConfigurationHelper::verifyModify(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName)
	{
		//Nothing to validate for the moment...be happy!
		AES_DDT_TRACE_FUNCTION;
		UNUSED(configurationMask);
		UNUSED(dataSourceName);

		common::errorCode::ErrorConstants errorNumber = common::errorCode::ERR_NO_ERRORS;

		return errorNumber;
	}

	common::errorCode::ErrorConstants ConfigurationHelper::verifyAttributesValue(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName, const ACS_APGCC_CcbId& configID)
	{
		AES_DDT_TRACE_FUNCTION;
		common::errorCode::ErrorConstants errorNumber = common::errorCode::ERR_NO_ERRORS;
		//
		if(configurationMask.test(OutputFormat))
		{
			std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator> commandRange;
			commandRange = m_commands.equal_range(std::make_pair(dataSourceName, configID));

			uint32_t dataChunkSize = 0U;
			int32_t dataLengthType = common::VARIABLE;

			bool outputFormatFound = false;

			// get the value of attribute "dataChunkSize" from queued commands
			const operation::outputFormatInfo* outputFormatInfo =
					static_cast<const operation::outputFormatInfo*>(getCommand(dataSourceName, configID, operation::OUTPUTFORMAT_ADD));

			if( outputFormatInfo )
			{
				dataChunkSize = outputFormatInfo->dataChunkSize;
				dataLengthType = outputFormatInfo->dataLengthType;
				outputFormatFound = true;
			}
			else
			{
				outputFormatInfo =
						static_cast<const operation::outputFormatInfo*>(getCommand(dataSourceName, configID, operation::OUTPUTFORMAT_MOD));

				if( outputFormatInfo )
				{
					if(outputFormatInfo->changeMask & operation::changeMask::DATACHUNKSIZE_CHANGE )
					{
						dataChunkSize = outputFormatInfo->dataChunkSize;
					}
					if (outputFormatInfo->changeMask & operation::changeMask::DATALENGHTTYPE_CHANGE)
					{
						dataLengthType = outputFormatInfo->dataLengthType;
					}

					outputFormatFound = true;
				}
			}

			// found OutputFormat MO request
			if(outputFormatFound)
			{

				if (dataChunkSize > 0U)
				{
					//Check valid recordSize
					uint32_t recordSize = 0U;
					if( configurationMask.test(DataSource) )
					{
						// get record size from queued commands
						const operation::dataSourceInfo* dataSourceInfo =
								static_cast<const operation::dataSourceInfo*>(getCommand(dataSourceName, configID, operation::DATASOURCE_ADD));

						if( dataSourceInfo )
						{
							recordSize = dataSourceInfo->recordSize;
						}
					}
					else
					{
						// get record size from
						recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(dataSourceName);
					}

					if( recordSize > dataChunkSize )
					{
						errorNumber = common::errorCode::ERR_IMM_INVALID_DATA_CHUNK_SIZE;
					}
				}


				//Check valid dataLengthType
				if (common::EVEN == dataLengthType)
				{
					bool hasFilePeer = false;

					if( !(hasFilePeer = configurationMask.test(FilePeer)))
					{
						if( engine::Context* context = ( engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName) ) )
						{
							hasFilePeer = (context->countFilePeers() > 0);
						}
					}

					if( hasFilePeer )
					{
						errorNumber = common::errorCode::ERR_IMM_INVALID_DATA_LENGTH_TYPE;
					}
				}

			}
		}

		if(common::errorCode::ERR_NO_ERRORS == errorNumber && configurationMask.test(FileFormat))
		{
			std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator> commandRange;
			commandRange = m_commands.equal_range(std::make_pair(dataSourceName, configID));

			uint32_t startupSequenceNumber = 0U;
			std::string templateValue;

			bool fileFormatFound = false;

			const operation::fileFormatInfo* fileFormatInfo =
					static_cast<const operation::fileFormatInfo*>(getCommand(dataSourceName, configID, operation::FILEFORMAT_ADD));

			if( fileFormatInfo )
			{
				templateValue = fileFormatInfo->templateValue;
				startupSequenceNumber = fileFormatInfo->startupSequenceNumber;

				fileFormatFound = true;
			}
			else
			{
				fileFormatInfo =
						static_cast<const operation::fileFormatInfo*>(getCommand(dataSourceName, configID, operation::FILEFORMAT_MOD));

				if( fileFormatInfo )
				{
					// fetch template value
					if(fileFormatInfo->changeMask & operation::changeMask::FILE_TEMPLATE_CHANGE)
					{
						templateValue = fileFormatInfo->templateValue;
					}
					else
					{
						//fetch templateValue currently in use
						engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);
						templateValue = context->getFileFormat()->getTemplate();
					}

					// fetch startup sequence number value
					if(fileFormatInfo->changeMask & operation::changeMask::FILE_SEQ_NUMBER_CHANGE)
					{
						startupSequenceNumber = fileFormatInfo->startupSequenceNumber;
					}
					else
					{
						//fetch startupSequenceNumber currently in use
						engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);
						startupSequenceNumber = context->getFileFormat()->getStartupSequenceNumber();
					}

					fileFormatFound = true;
				}
			}

			// found FileFormat MO request
			if(fileFormatFound)
			{
				//Check compatibility
				engine::TemplateHandler templateValidator(templateValue);
				if(templateValidator.isValid())
				{
					//check compatibility with template
					if(!templateValidator.isStartupSequenceNumberCompatible(startupSequenceNumber))
					{
						errorNumber = common::errorCode::ERR_IMM_INVALID_SEQUENCE_NUMBER;
					}
				}
				else
				{
					errorNumber = common::errorCode::ERR_IMM_INVALID_TEMPLATE;
				}
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%d>", errorNumber);
		return errorNumber;
	}

	common::errorCode::ErrorConstants ConfigurationHelper::verifyDelete(const std::bitset<NUMBER_OF_ELEMENTS>& configurationMask, const std::string& dataSourceName, const ACS_APGCC_CcbId& configID)
	{
		AES_DDT_TRACE_FUNCTION;
		common::errorCode::ErrorConstants errorNumber = common::errorCode::ERR_NO_ERRORS;

		// Data Source can be deleted only if it is EMPTY!
		if( configurationMask.test(DataSource)  && !engine::workingSet_t::instance()->getStoringManager().storeEmpty(dataSourceName) )
		{
			errorNumber = common::errorCode::ERR_IMM_NOT_EMPTY_DATASOURCE;

			AES_DDT_LOG(LOG_LEVEL_ERROR, "DataSource <%s> is NOT EMPTY", dataSourceName.c_str());
			AES_DDT_TRACE_MESSAGE("ERROR: DataSource <%s> is NOT EMPTY", dataSourceName.c_str());
			return errorNumber;
		}

		// File Format can be deleted only in case the whole DataSink is being deleted
		if( configurationMask.test(FileFormat) && !configurationMask.test(DataSink) )
		{
			errorNumber = common::errorCode::ERR_IMM_FILE_FORMAT_DELETE_NOT_ALLOWED;
			return errorNumber;
		}

		// Both Peers can be deleted only in case also the DataSink is being deleted
		if( configurationMask.test(DoublePeer) && !configurationMask.test(DataSink) )
		{
			errorNumber = common::errorCode::ERR_IMM_BOTH_PEER_DELETE_NOT_ALLOWED;
			return errorNumber;
		}

		// File Peer can be deleted only in case also the DataSink is being deleted or
		// in case another File Peer is available in the same DataSink
		if( configurationMask.test(FilePeer) && !configurationMask.test(DataSink) )
		{
			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);

			if( context->countFilePeers() < 2 )
			{
				errorNumber = common::errorCode::ERR_IMM_LAST_FILE_PEER_DELETE_NOT_ALLOWED;
				return errorNumber;
			}

			const operation::filePeerInfo* filePeerInfo =
					static_cast<const operation::filePeerInfo*>(getCommand(dataSourceName, configID, operation::FILEPEER_DEL));

			if(filePeerInfo)
			{
				if( context->isPeerActive(filePeerInfo->getName()) )
				{
					errorNumber = common::errorCode::ERR_IMM_ACTIVE_FILE_PEER_DELETE_NOT_ALLOWED;
					return errorNumber;
				}
			}
			else
			{
				errorNumber = common::errorCode::ERR_GENERIC;
				return errorNumber;
			}
		}

		// Block Peer can be deleted only in case also the DataSink is being deleted or
		// in case it is not active and another Block Peer is available in the same DataSink
		if( configurationMask.test(BlockPeer) && !configurationMask.test(DataSink) )
		{
			engine::Context* context = engine::workingSet_t::instance()->getDataSourceManager().getContext(dataSourceName);

			if( context->countBlockPeers() < 2 )
			{
				errorNumber = common::errorCode::ERR_IMM_LAST_BLOCK_PEER_DELETE_NOT_ALLOWED;
				return errorNumber;
			}

			const operation::blockPeerInfo* blockPeerInfo =
					static_cast<const operation::blockPeerInfo*>(getCommand(dataSourceName, configID,operation::BLOCKPEER_DEL ));

			if( blockPeerInfo )
			{
				if( context->isPeerActive(blockPeerInfo->getName()) )
				{
					errorNumber = common::errorCode::ERR_IMM_ACTIVE_BLOCK_PEER_DELETE_NOT_ALLOWED;
					return errorNumber;
				}
			}
			else
			{
				errorNumber = common::errorCode::ERR_GENERIC;
				return errorNumber;
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%d>", errorNumber);
		return errorNumber;
	}

	void ConfigurationHelper::addToCreateSession(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t newElement)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>, adding MO TYPE:<%d>", dataSourceName.c_str(), configID, newElement);

		mapOfConfigurations_t::iterator element = m_createSessions.find(std::make_pair(dataSourceName, configID));

		if( m_createSessions.end() == element )
		{
			// New configuration

			// Create the bitmask
			std::bitset<NUMBER_OF_ELEMENTS> configBitMask;
			configBitMask.set(newElement);

			// Adding it to the configurations map.
			m_createSessions[std::make_pair(dataSourceName, configID)] = configBitMask;
		}
		else
		{
			// configuration found, updates the bitmask
			element->second.set(newElement);
		}
	}

	void ConfigurationHelper::addToModifySession(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t modifyElement)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>, adding MO TYPE:<%d>", dataSourceName.c_str(), configID, modifyElement);

		mapOfConfigurations_t::iterator element = m_modifySessions.find(std::make_pair(dataSourceName, configID));

		if( m_modifySessions.end() == element )
		{
			// New configuration

			// Create the bitmask
			std::bitset<NUMBER_OF_ELEMENTS> configBitMask;
			configBitMask.set(modifyElement);

			// Adding it to the configurations map.
			m_modifySessions[std::make_pair(dataSourceName, configID)] = configBitMask;
		}
		else
		{
			// configuration found, updates the bitmask
			element->second.set(modifyElement);
		}
	}

	void ConfigurationHelper::addToDeleteSession(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const MOtype_t newElement)
	{
		AES_DDT_TRACE_MESSAGE("[DS: <%s>] Configuration ID:<%llu>, adding MO TYPE:<%d>", dataSourceName.c_str(), configID, newElement);

		mapOfConfigurations_t::iterator element = m_deleteSessions.find(std::make_pair(dataSourceName, configID));

		if( m_deleteSessions.end() == element )
		{
			// New configuration
			// Create the bitmask
			std::bitset<NUMBER_OF_ELEMENTS> configBitMask;
			configBitMask.set(newElement);

			// Adding it to the configurations map.
			m_deleteSessions[std::make_pair(dataSourceName, configID)] = configBitMask;
		}
		else
		{
			if( ((FilePeer ==  newElement) && element->second.test(FilePeer)) ||
					((BlockPeer ==  newElement) && element->second.test(BlockPeer)) )
			{
				AES_DDT_TRACE_MESSAGE("Delete both Peers");
				element->second.set(DoublePeer);
			}

			// configuration found, updates the bitmask
			element->second.set(newElement);
		}
	}

	void ConfigurationHelper::sendCommand(const operation::identifier_t& id, const void* op_info )
	{
		AES_DDT_TRACE_FUNCTION;

		operation::Creator factoryCreator;
		factoryCreator.schedule(id, op_info);

		AES_DDT_TRACE_MESSAGE("Sent operation request:<%d> ...", id);

	}

	void ConfigurationHelper::setConfigurationError(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, common::errorCode::ErrorConstants errorNumber, const operation::identifier_t& opId)
	{
		AES_DDT_TRACE_FUNCTION;

		std::string errorString;
		std::string fullDN;
		bool attachFullDN = true;

		switch (errorNumber)
		{
			case common::errorCode::ERR_IMM_NOT_ENOUGH_PEER:
				errorString = imm::comcli_errormessage::NOT_ENOUGH_PEER;
				break;
			case common::errorCode::ERR_IMM_PEER_TYPE_MISMATCH:
				errorString = imm::comcli_errormessage::PEER_TYPE_MISMATCH;
				break;
			case common::errorCode::ERR_IMM_MISSING_FILE_FORMAT:
				errorString = imm::comcli_errormessage::MISSING_FILE_FORMAT;
				break;
			case common::errorCode::ERR_IMM_FILE_FORMAT_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::FILE_FORMAT_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_INVALID_TEMPLATE:
				errorString = imm::comcli_errormessage::FILE_FORMAT_INVALID_TEMPLATE;
				break;
			case common::errorCode::ERR_IMM_INVALID_SEQUENCE_NUMBER:
				errorString = imm::comcli_errormessage::FILE_FORMAT_INVALID_SEQUENCE_NUMBER;
				break;
			case common::errorCode::ERR_IMM_FILE_PEER_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::FILE_PEER_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_BLOCK_PEER_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::BLOCK_PEER_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_NOT_EMPTY_DATASOURCE:
				errorString = imm::comcli_errormessage::DATASOURCE_NOT_EMPTY;
				break;
			case common::errorCode::ERR_IMM_FILE_FORMAT_DELETE_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::FILE_FORMAT_DELETE_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_LAST_FILE_PEER_DELETE_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::LAST_FILE_PEER_DELETE_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_LAST_BLOCK_PEER_DELETE_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::LAST_BLOCK_PEER_DELETE_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_ACTIVE_BLOCK_PEER_DELETE_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::ACTIVE_BLOCK_PEER_DELETE_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_ACTIVE_FILE_PEER_DELETE_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::ACTIVE_FILE_PEER_DELETE_NOT_ALLOWED;
				break;
			case common::errorCode::ERR_IMM_BOTH_PEER_DELETE_NOT_ALLOWED:
				errorString = imm::comcli_errormessage::BOTH_PEER_DELETE_NOT_ALLOWED;
				attachFullDN = false;
				break;
			case common::errorCode::ERR_IMM_INVALID_DATA_CHUNK_SIZE:
				errorString = imm::comcli_errormessage::INVALID_DATA_CHUNK_SIZE;
				break;
			case common::errorCode::ERR_IMM_ACA_MESSAGESTORE_NAME_CONFLICT:
				errorString = imm::comcli_errormessage::DATASOURCE_NAME_CONFLICT;
				break;
			case common::errorCode::ERR_HW_NOTSUPPORTED:
				errorString = imm::comcli_errormessage::HW_NOT_SUPPORTED;
				attachFullDN = false;
				break;
			case common::errorCode::ERR_IMM_INVALID_DATA_LENGTH_TYPE:
				errorString = imm::comcli_errormessage::ERR_IMM_INVALID_DATA_LENGTH_TYPE;
				break;
			default:
				errorString = imm::comcli_errormessage::GENERIC;
				attachFullDN = false;
				break;
		}

		if(attachFullDN)
		{
			fullDN = getFullCOMDN(dataSourceName, configID,opId);
			errorString.push_back(imm::parserTag::comma);
			errorString.append(fullDN);
		}

		{
			mapOfConfigurationErrors_t::iterator element = m_configurationErrors.find(std::make_pair(dataSourceName, configID));

			if( m_configurationErrors.end() == element )
			{
				// New configuration error
				ConfigurationError configurationError;

				configurationError.errorCode = errorNumber;
				configurationError.errorMessage.assign(errorString);

				// Adding it to the configurations map.
				m_configurationErrors[std::make_pair(dataSourceName, configID)] = configurationError;
			}
			else
			{
				// error message found, update it
				element->second.errorCode = errorNumber;
				element->second.errorMessage.assign(errorString);
			}
		}

		AES_DDT_LOG(LOG_LEVEL_ERROR, errorString.c_str());
		AES_DDT_TRACE_MESSAGE("ERROR: %s", errorString.c_str());
	}

	const void* ConfigurationHelper::getCommand(const std::string& dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& opId)
	{
		AES_DDT_TRACE_MESSAGE("DS:<%s>, search command id:<%d>", dataSourceName.c_str(), opId);

		const void* commandInfo = NULL;
		std::pair<mapOfCommands_t::iterator, mapOfCommands_t::iterator> commandRange;
		commandRange = m_commands.equal_range(std::make_pair(dataSourceName, configID));

		// get command info from queued commands
		for(mapOfCommands_t::iterator element = commandRange.first; element != commandRange.second; ++element)
		{
			command_t command = element->second;
			if( opId == command.first )
			{
				commandInfo = command.second;
				break;
			}
		}

		return commandInfo;
	}

	common::errorCode::ErrorConstants ConfigurationHelper::verifyCreateDataSource(const std::string & dataSourceName)
	{
		common::errorCode::ErrorConstants errorNumber = common::errorCode::ERR_NO_ERRORS;

		//fetches HW info and verify DataSoure is not created on GEP1 machines
		common::HwVerConstants hwVer;
		if(common::configuration_t::instance()->getHWVersionInfo(hwVer))
		{
			if(common::HWVER_GEP1 == hwVer )
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Data Source:<%s> creation not supported in GEP1", dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("ERROR: Data Source:<%s> creation not supported in GEP1", dataSourceName.c_str() );
				errorNumber = common::errorCode::ERR_HW_NOTSUPPORTED;
			}
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Data Source:<%s> error  in fetching  HWVER", dataSourceName.c_str() );
			AES_DDT_TRACE_MESSAGE("ERROR: Data Source:<%s> error  in fetching  HWVER", dataSourceName.c_str() );
			errorNumber = common::errorCode::ERR_GENERIC;

		}

		//fetches message store names defined in ACA and checks that dataSource Name  doesnt exist in ACA
		if(common::errorCode::ERR_NO_ERRORS == errorNumber)
		{
			std::vector<std::string> acaMSNames;
			errorNumber = imm::objectManagerHelper_t::instance()->getACAMessageStoreNames(acaMSNames);

			if(common::errorCode::ERR_NO_ERRORS == errorNumber)
			{
				for(std::vector<std::string>::const_iterator it = acaMSNames.begin(); it != acaMSNames.end(); ++it)
				{
					if(0 == dataSourceName.compare(*it))
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "Data Source:<%s> already defined in ACA MS", dataSourceName.c_str() );
						AES_DDT_TRACE_MESSAGE("ERROR: Data Source:<%s> already defined in ACA MS", dataSourceName.c_str() );
						errorNumber = common::errorCode::ERR_IMM_ACA_MESSAGESTORE_NAME_CONFLICT;
						break;
					}

				}
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Data Source:<%s> error  in fetching  ACA MS", dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("ERROR: Data Source:<%s> error  in fetching  ACA MS", dataSourceName.c_str() );
				errorNumber = common::errorCode::ERR_GENERIC;
			}
		}

		return errorNumber;
	}

	std::string ConfigurationHelper::getFullCOMDN(const std::string & dataSourceName, const ACS_APGCC_CcbId& configID, const operation::identifier_t& opId)
	{
		std::string fullDNStr;
		char dataSourceDN[1024]={0};
		std::string tmpDataSourceDn;
		fullDNStr.assign(imm::parserTag::singleQuote);
		tmpDataSourceDn.append(imm::com_dn::DATASOURCE_DN);
		snprintf(dataSourceDN,sizeof(dataSourceDN),tmpDataSourceDn.c_str(),dataSourceName.c_str());
		fullDNStr.append(dataSourceDN);

		switch(opId)
		{
		case operation::OUTPUTFORMAT_ADD:
		case operation::OUTPUTFORMAT_MOD:
		{
			std::string outputFormatDN;
			const operation::outputFormatInfo * opFormatInfo =
					static_cast<const operation::outputFormatInfo*>(getCommand(dataSourceName, configID, opId));
			if(opFormatInfo)
			{
				std::string opFormatName = opFormatInfo->name;
				std::string dataSinkName = opFormatInfo->getDataSinkName();
				std::string tmpFullDN;
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::DATASINK_DN);
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::OUTPUTFORMAT_DN);
				char fullDn[1024] = {0};
				snprintf(fullDn,sizeof(fullDn),tmpFullDN.c_str(),dataSinkName.c_str(),opFormatName.c_str());
				fullDNStr.append(fullDn);
			}
		}
		break;

		case operation::DATASINK_ADD:
		case operation::DATASINK_DEL:
		case operation::DATASINK_MOD:
		{
			const operation::dataSinkInfo * dataSinkInfo =
					static_cast<const operation::dataSinkInfo*>(getCommand(dataSourceName, configID, opId));
			if(dataSinkInfo)
			{
				std::string dataSinkName = dataSinkInfo->name;
				std::string tmpFullDN;
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::DATASINK_DN);
				char fullDn[1024] = {0};
				snprintf(fullDn,sizeof(fullDn),tmpFullDN.c_str(),dataSinkName.c_str());
				fullDNStr.append(fullDn);
			}
		}
		break;
		case operation::FILEFORMAT_ADD:
		case operation::FILEFORMAT_DEL:
		case operation::FILEFORMAT_MOD:
		{
			const operation::fileFormatInfo * fileFormatInfo =
					static_cast<const operation::fileFormatInfo*>(getCommand(dataSourceName, configID, opId));
			if(fileFormatInfo)
			{
				std::string dataSinkName = fileFormatInfo->getDataSinkName();
				std::string outputFormat = fileFormatInfo->getOutputFormatName();
				std::string fileFormatName = fileFormatInfo->name;
				std::string tmpFullDN;
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::DATASINK_DN);
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::OUTPUTFORMAT_DN);
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::FILEFORMAT_DN);
				char fullDn[1024] = {0};
				snprintf(fullDn,sizeof(fullDn),tmpFullDN.c_str(),dataSinkName.c_str(),outputFormat.c_str(),fileFormatName.c_str());
				fullDNStr.append(fullDn);
			}
		}
		break;
		case operation::FILEPEER_ADD:
		case operation::FILEPEER_DEL:
		case operation::FILEPEER_MOD:
		{
			const operation::filePeerInfo * fpInfo =
					static_cast<const operation::filePeerInfo*>(getCommand(dataSourceName, configID, opId));
			if(fpInfo)
			{
				std::string dataSinkName = fpInfo->getDataSinkName();
				std::string filePeerName = fpInfo->name;
				std::string tmpFullDN;
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::DATASINK_DN);
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::FILEPEER_DN);
				char fullDn[1024] = {0};
				snprintf(fullDn,sizeof(fullDn),tmpFullDN.c_str(),dataSinkName.c_str(),filePeerName.c_str());
				fullDNStr.append(fullDn);
			}
		}
		break;
		case operation::BLOCKPEER_ADD:
		case operation::BLOCKPEER_DEL:
		case operation::BLOCKPEER_MOD:
		{
			const operation::blockPeerInfo * bpInfo =
					static_cast<const operation::blockPeerInfo*>(getCommand(dataSourceName, configID, opId));
			if(bpInfo)
			{
				std::string dataSinkName = bpInfo->getDataSinkName();
				std::string blockPeerName = bpInfo->name;
				std::string tmpFullDN;
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::DATASINK_DN);
				tmpFullDN.push_back(imm::parserTag::comma);
				tmpFullDN.append(imm::com_dn::BLOCKPEER_DN);
				char fullDn[1024] = {0};
				snprintf(fullDn,sizeof(fullDn),tmpFullDN.c_str(),dataSinkName.c_str(),blockPeerName.c_str());
				fullDNStr.append(fullDn);
			}
		}
		break;
		case operation::DATASOURCE_DEL:
			break;
		default:
			break;
		}
		fullDNStr.append(imm::parserTag::singleQuote);
		return fullDNStr;

	}

} /* namespace imm */
