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

#include "engine/datareloadhelper.h"
#include "imm/imm.h"
#include "imm/filepeer_oi.h"
#include "common/programconstants.h"
#include "common/utility.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "operation/operation.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "acs_apgcc_omhandler.h"
	#include <ACS_APGCC_Util.H>
#else
	#include "stubs/imm_stubs.h"
	#include "stubs/macro_stub.h"
	#include "stubs/operation_stub.h"
#endif

#include <string>
#include <vector>

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_DataReloadHelper)

namespace engine
{

	DataReloadHelper::DataReloadHelper()
	: m_objectManager()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	DataReloadHelper::~DataReloadHelper()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	bool DataReloadHelper::reloadDataFromIMM()
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = false;

		// Init OM resource
		if(m_objectManager.Init() == ACS_CC_SUCCESS)
		{
			// Load Data Source and its children : DataSink, FilePeer or BlockPeer, OutputFormat, FileFormat
			result = loadDataSources();

			// Deallocate OM resource
			m_objectManager.Finalize();
		}
		else
		{
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler Init() failed!, errorCode:<%d> errorMsg:<%s>", m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText());
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;

	}

	bool DataReloadHelper::loadDataSources()
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = true;
		// Get List of all defined data sources
		std::vector<std::string> dataSourceDNCollection;

		if(m_objectManager.getChildren(imm::mom_root::DDTM, ACS_APGCC_SUBLEVEL, &dataSourceDNCollection) == ACS_CC_SUCCESS )
		{
			std::vector<std::string>::const_iterator dataSourceDN;

			//-----------------------
			// GET DATA SOURCE MOs
			// for each data source
			for(dataSourceDN = dataSourceDNCollection.begin(); result && (dataSourceDN != dataSourceDNCollection.end()); ++dataSourceDN)
			{
				result = addDataSourceFromDN( *dataSourceDN );

				if(result) result = loadDataSink(*dataSourceDN);
			}
		}
		else
		{
			result = false;
			// getChildren error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getChildren(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					imm::mom_root::DDTM, m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText() );
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	bool DataReloadHelper::loadDataSink(const std::string& dataSourceDN)
	{
		AES_DDT_TRACE_MESSAGE("Data Source DN:<%s> ", dataSourceDN.c_str());
		bool result = true;

		std::vector<std::string> dataSinkDNCollection;

		if(m_objectManager.getChildren(dataSourceDN.c_str(), ACS_APGCC_SUBLEVEL, &dataSinkDNCollection) == ACS_CC_SUCCESS )
		{
			std::vector<std::string>::const_iterator dataSourceChildrenDN;

			// for each children
			for(dataSourceChildrenDN = dataSinkDNCollection.begin(); dataSourceChildrenDN != dataSinkDNCollection.end(); ++dataSourceChildrenDN)
			{
				// check if it is a data sink MO
				if( dataSourceChildrenDN->find(imm::datasink_attribute::RDN) != std::string::npos )
				{
					result = loadPeersAndOutputFormat(*dataSourceChildrenDN);

					if(result)
						result = addDataSinkFromDN( *dataSourceChildrenDN );

					break;
				}
			}
		}
		else
		{
			result = false;
			// getChildren error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getChildren(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					dataSourceDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText() );
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	bool DataReloadHelper::loadPeersAndOutputFormat(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("Data Sink DN:<%s> ", dataSinkDN.c_str());
		bool result = true;

		std::vector<std::string> dataSinkChildrenDNCollection;

		if(m_objectManager.getChildren(dataSinkDN.c_str(), ACS_APGCC_SUBLEVEL, &dataSinkChildrenDNCollection) == ACS_CC_SUCCESS )
		{
			std::vector<std::string>::const_iterator dataSinkChildrenDN;

			// for each children
			for(dataSinkChildrenDN = dataSinkChildrenDNCollection.begin(); result && (dataSinkChildrenDN != dataSinkChildrenDNCollection.end()); ++dataSinkChildrenDN)
			{
				// check if it is a file peer
				if( dataSinkChildrenDN->find(imm::filepeer_attribute::RDN) != std::string::npos )
				{
					result = addFilePeerFromDN( *dataSinkChildrenDN );
					continue;
				}

				// check if it is a block peer
				if( dataSinkChildrenDN->find(imm::blockpeer_attribute::RDN) != std::string::npos )
				{
					result = addBlockPeerFromDN( *dataSinkChildrenDN );
					continue;
				}

				// check if it is a output format
				if( dataSinkChildrenDN->find(imm::outputformat_attribute::RDN) != std::string::npos )
				{
					result = addOutputFormatFromDN( *dataSinkChildrenDN );
					if(result) result = loadFileFormat(*dataSinkChildrenDN);

					continue;
				}
			}
		}
		else
		{
			result = false;
			// getChildren error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getChildren(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					dataSinkDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText() );
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	bool DataReloadHelper::loadFileFormat(const std::string& outputFormatDN)
	{
		bool result = true;

		std::vector<std::string> fileFormatDNCollection;

		if(m_objectManager.getChildren(outputFormatDN.c_str(), ACS_APGCC_SUBLEVEL, &fileFormatDNCollection) == ACS_CC_SUCCESS )
		{
			std::vector<std::string>::const_iterator fileFormatDN;

			// for each children
			for(fileFormatDN = fileFormatDNCollection.begin(); fileFormatDN != fileFormatDNCollection.end(); ++fileFormatDN)
			{
				result = addFileFormatFromDN( *fileFormatDN );
			}
		}
		else
		{
			result = false;
			// getChildren error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getChildren(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					outputFormatDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText() );
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	bool DataReloadHelper::addDataSourceFromDN(const std::string& dataSourceDN)
	{
		AES_DDT_TRACE_MESSAGE("Data Source DN:<%s> ", dataSourceDN.c_str());

		bool result = false;

		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		// to get the record size
		ACS_APGCC_ImmAttribute recordSizeAttribute;
		recordSizeAttribute.attrName.assign(imm::datasource_attribute::RECORDSIZE);
		attributeList.push_back(&recordSizeAttribute);

		// to get the alarmLevel
		ACS_APGCC_ImmAttribute alarmLevelAttribute;
		alarmLevelAttribute.attrName.assign(imm::datasource_attribute::ALARMLEVEL);
		attributeList.push_back(&alarmLevelAttribute);

		AES_DDT_TRACE_MESSAGE("get attributes from IMM");
		ACS_CC_ReturnType getResult = m_objectManager.getAttribute(dataSourceDN.c_str(), attributeList);

		operation::dataSourceInfo dataMO;

		if( (getResult == ACS_CC_SUCCESS) && (0U != recordSizeAttribute.attrValuesNum) &&
			(0U != alarmLevelAttribute.attrValuesNum) &&common::utility::getRDNValue(dataSourceDN, imm::datasource_attribute::RDN, dataMO.name) )
		{
			// make the value in upper case
			ACS_APGCC::toUpper(dataMO.name);
			dataMO.recordSize = (*reinterpret_cast<uint32_t*>(recordSizeAttribute.attrValues[0]));
			dataMO.alarmLevel = (*reinterpret_cast<uint32_t*>(alarmLevelAttribute.attrValues[0]));
			dataMO.moDN.assign(dataSourceDN);

			operation::result opResult;

			bool startTask = false;
			result = ( engine::workingSet_t::instance()->getDataSourceManager().add(dataMO, opResult, startTask) == common::errorCode::ERR_NO_ERRORS );

			if( result )
			{
				AES_DDT_TRACE_MESSAGE("Added a Data Source\n"
						"#####    DN: <%s>\n"
						"#####    Data Source: <%s>\n"
						"#####    recordSize:<%u>\n"
						"#####    alarmLevel:<%u>\n",
						dataMO.getDataSourceDN(), dataMO.getName(), dataMO.recordSize, dataMO.alarmLevel);

				AES_DDT_LOG(LOG_LEVEL_INFO, "Added a Data Source\n"
						"#####    DN: <%s>\n"
						"#####    Data Source: <%s>\n"
						"#####    recordSize:<%u>\n"
						"#####    alarmLevel:<%u>\n",
						dataMO.getDataSourceDN(), dataMO.getName(), dataMO.recordSize, dataMO.alarmLevel);
			}
			else
			{
				// Error on adding data source
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to add Data Source:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(),
						dataMO.getDataSourceDN(), opResult.getErrorCode(), opResult.getErrorMessage() );

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to add Data Source:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(),
						dataMO.getDataSourceDN(), opResult.getErrorCode(), opResult.getErrorMessage() );
			}
		}
		else
		{
			// getAttribute error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					dataSourceDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText());
		}

		return result;
	}

	bool DataReloadHelper::addDataSinkFromDN(const std::string& dataSinkDN)
	{
		AES_DDT_TRACE_MESSAGE("Data Sink DN:<%s> ", dataSinkDN.c_str());
		bool result = false;

		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		// to get the retry Delay
		ACS_APGCC_ImmAttribute retryDelayAttribute;
		retryDelayAttribute.attrName.assign(imm::datasink_attribute::RETRYDELAY);
		attributeList.push_back(&retryDelayAttribute);

		// to get the retry Attempts
		ACS_APGCC_ImmAttribute retryAttemptsAttribute;
		retryAttemptsAttribute.attrName.assign(imm::datasink_attribute::RETRYATTEMPTS);
		attributeList.push_back(&retryAttemptsAttribute);

		AES_DDT_TRACE_MESSAGE("get attributes from IMM");
		ACS_CC_ReturnType getResult = m_objectManager.getAttribute(dataSinkDN.c_str(), attributeList);
		operation::dataSinkInfo dataMO;

		if( (getResult == ACS_CC_SUCCESS) && (0U != retryDelayAttribute.attrValuesNum) &&
				(0U != retryAttemptsAttribute.attrValuesNum) &&
				common::utility::getRDNValue(dataSinkDN, imm::datasource_attribute::RDN, dataMO.dataSourceName) &&
				common::utility::getRDNValue(dataSinkDN, imm::datasink_attribute::RDN, dataMO.name))
		{
			// make the value in upper case
			ACS_APGCC::toUpper(dataMO.dataSourceName);

			dataMO.retryDelay = (*reinterpret_cast<uint32_t*>(retryDelayAttribute.attrValues[0]));
			dataMO.retryAttempts = (*reinterpret_cast<uint32_t*>(retryAttemptsAttribute.attrValues[0]));
			dataMO.moDN.assign(dataSinkDN);

			operation::result opResult;

			bool startTask = false;

			result = ( engine::workingSet_t::instance()->getDataSourceManager().add(dataMO, opResult, startTask) == common::errorCode::ERR_NO_ERRORS );

			if( result )
			{
				AES_DDT_TRACE_MESSAGE("Added a Data Sink\n"
						"#####    DN: <%s>\n"
						"#####    Data Sink: <%s>\n"
						"#####    Source: <%s>\n"
						"#####    retryDelay:<%u>\n"
						"#####    retryAttempts:<%u>\n",
						dataMO.getDataSinkDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.retryDelay, dataMO.retryAttempts);

				AES_DDT_LOG(LOG_LEVEL_INFO, "Added a Data Sink\n"
						"#####    DN: <%s>\n"
						"#####    Data Sink: <%s>\n"
						"#####    Source: <%s>\n"
						"#####    retryDelay:<%u>\n"
						"#####    retryAttempts:<%u>\n",
						dataMO.getDataSinkDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.retryDelay, dataMO.retryAttempts);
			}
			else
			{
				// Error on adding data sink
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to add Data Sink:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(),
						dataMO.getDataSinkDN(), opResult.getErrorCode(), opResult.getErrorMessage() );

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to add Data Sink:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(),
						dataMO.getDataSinkDN(), opResult.getErrorCode(), opResult.getErrorMessage() );
			}
		}
		else
		{
			// getAttribute error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					dataSinkDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText());
		}

		return result;
	}

	bool DataReloadHelper::addFilePeerFromDN(const std::string& filePeerDN)
	{
		AES_DDT_TRACE_MESSAGE("File Peer DN:<%s> ", filePeerDN.c_str());
		bool result = false;

		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		// to get the user Name
		ACS_APGCC_ImmAttribute userNameAttribute;
		userNameAttribute.attrName.assign(imm::filepeer_attribute::USERNAME);
		attributeList.push_back(&userNameAttribute);

		// to get the password
		ACS_APGCC_ImmAttribute passwordAttribute;
		passwordAttribute.attrName.assign(imm::filepeer_attribute::PASSWORD);
		attributeList.push_back(&passwordAttribute);

		// to get the ipAddress
		ACS_APGCC_ImmAttribute ipAddressAttribute;
		ipAddressAttribute.attrName.assign(imm::filepeer_attribute::IPADDRESS);
		attributeList.push_back(&ipAddressAttribute);

		// to get the portNumber
		ACS_APGCC_ImmAttribute portNumberAttribute;
		portNumberAttribute.attrName.assign(imm::filepeer_attribute::PORTNUMBER);
		attributeList.push_back(&portNumberAttribute);

		// to get the remoteFolderPath
		ACS_APGCC_ImmAttribute remoteFolderPathAttribute;
		remoteFolderPathAttribute.attrName.assign(imm::filepeer_attribute::REMOTEFOLDERPATH);
		attributeList.push_back(&remoteFolderPathAttribute);

		// to get the transferProtocol
		ACS_APGCC_ImmAttribute transferProtocolAttribute;
		transferProtocolAttribute.attrName.assign(imm::filepeer_attribute::TRANSFERPROTOCOL);
		attributeList.push_back(&transferProtocolAttribute);

		// to get the status
		ACS_APGCC_ImmAttribute statusAttribute;
		statusAttribute.attrName.assign(imm::filepeer_attribute::STATUS);
		attributeList.push_back(&statusAttribute);

		AES_DDT_TRACE_MESSAGE("get attributes from IMM");
		ACS_CC_ReturnType getResult = m_objectManager.getAttribute(filePeerDN.c_str(), attributeList);

		operation::filePeerInfo dataMO;

		if( (getResult == ACS_CC_SUCCESS) && (0U != userNameAttribute.attrValuesNum) &&
				(0U != passwordAttribute.attrValuesNum) && (0U != ipAddressAttribute.attrValuesNum) &&
				(0U != transferProtocolAttribute.attrValuesNum)  &&
				(0U != statusAttribute.attrValuesNum) &&
				common::utility::getRDNValue(filePeerDN, imm::datasource_attribute::RDN, dataMO.dataSourceName) &&
				common::utility::getRDNValue(filePeerDN, imm::datasink_attribute::RDN, dataMO.dataSinkName) &&
				common::utility::getRDNValue(filePeerDN, imm::filepeer_attribute::RDN, dataMO.name))
		{
			// make the value in upper case
			ACS_APGCC::toUpper(dataMO.dataSourceName);

			dataMO.username.assign( reinterpret_cast<char*>(userNameAttribute.attrValues[0]) );
			dataMO.passwordDN.assign( reinterpret_cast<char*>(passwordAttribute.attrValues[0]) );
			dataMO.ipAddress.assign( reinterpret_cast<char*>(ipAddressAttribute.attrValues[0]) );
			dataMO.transferProtocol = (*reinterpret_cast<int32_t*>(transferProtocolAttribute.attrValues[0]));
			dataMO.status = (*reinterpret_cast<int32_t*>(statusAttribute.attrValues[0]));

			if(0U != remoteFolderPathAttribute.attrValuesNum)
			{
				dataMO.remoteFolderPath.assign( reinterpret_cast<char*>(remoteFolderPathAttribute.attrValues[0]) );
				//fix the path now
				imm::FilePeer_OI::adjustPathSlashes(dataMO.remoteFolderPath);
			}

			if(0U != portNumberAttribute.attrValuesNum)
			{
				dataMO.portNumber = (*reinterpret_cast<unsigned int*>(portNumberAttribute.attrValues[0]));
				dataMO.moDN.assign(filePeerDN);
			}

			operation::result opResult;

			result = ( engine::workingSet_t::instance()->getDataSourceManager().add(dataMO, opResult) == common::errorCode::ERR_NO_ERRORS );

			if( result )
			{
				AES_DDT_TRACE_MESSAGE("Added a FilePeer\n"
						"#####    DN: <%s>\n"
						"#####    FilePeer: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    IP:<%s:%u>\n"
						"#####    username:<%s>\n"
						"#####    Remote Folder:<%s>\n"
						"#####	  Status : <%d>	\n"
						"#####    Protocol: <%s>\n",
						dataMO.getFilePeerDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(), dataMO.getIpAddress(), dataMO.getPortNumber(),
						dataMO.getUserName(), dataMO.getRemoteFolderPath(),dataMO.status, (dataMO.transferProtocol == common::PROTOCOL_FTP ? "FTP" : "SFTP"));

				AES_DDT_LOG(LOG_LEVEL_INFO, "Added a FilePeer\n"
						"#####    DN: <%s>\n"
						"#####    FilePeer: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    IP:<%s:%u>\n"
						"#####    username:<%s>\n"
						"#####    Remote Folder:<%s>\n"
						"#####	  Status : <%d>	\n"
						"#####    Protocol: <%s>\n",
						dataMO.getFilePeerDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(), dataMO.getIpAddress(), dataMO.getPortNumber(),
						dataMO.getUserName(), dataMO.getRemoteFolderPath(), dataMO.status, (dataMO.transferProtocol == common::PROTOCOL_FTP ? "FTP" : "SFTP"));
			}
			else
			{
				// Error on adding file peer
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to add File Peer:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getFilePeerDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to add File Peer:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getFilePeerDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );
			}
		}
		else
		{
			// getAttribute error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					filePeerDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText());
		}

		return result;
	}

	bool DataReloadHelper::addBlockPeerFromDN(const std::string& blockPeerDN)
	{
		AES_DDT_TRACE_MESSAGE("Block Peer DN:<%s> ", blockPeerDN.c_str());
		bool result = false;

		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		//get status attribute
		ACS_APGCC_ImmAttribute statusAttribute;
		statusAttribute.attrName.assign(imm::blockpeer_attribute::STATUS);
		attributeList.push_back(&statusAttribute);

		// to get the ipAddress
		ACS_APGCC_ImmAttribute ipAddressAttribute;
		ipAddressAttribute.attrName.assign(imm::blockpeer_attribute::IPADDRESS);
		attributeList.push_back(&ipAddressAttribute);

		AES_DDT_TRACE_MESSAGE("get attributes from IMM");
		ACS_CC_ReturnType getResult = m_objectManager.getAttribute(blockPeerDN.c_str(), attributeList);

		operation::blockPeerInfo dataMO;

		if( (getResult == ACS_CC_SUCCESS) && (0U != ipAddressAttribute.attrValuesNum) &&
				(0U != statusAttribute.attrValuesNum) &&
				common::utility::getRDNValue(blockPeerDN, imm::datasource_attribute::RDN, dataMO.dataSourceName) &&
				common::utility::getRDNValue(blockPeerDN, imm::datasink_attribute::RDN, dataMO.dataSinkName) &&
				common::utility::getRDNValue(blockPeerDN, imm::blockpeer_attribute::RDN, dataMO.name))
		{
			// make the value in upper case
			ACS_APGCC::toUpper(dataMO.dataSourceName);

			dataMO.ipAddress.assign( reinterpret_cast<char*>( ipAddressAttribute.attrValues[0]) );
			dataMO.status = *reinterpret_cast<uint32_t*>( statusAttribute.attrValues[0] );

			dataMO.moDN.assign(blockPeerDN);

			operation::result opResult;

			result = ( engine::workingSet_t::instance()->getDataSourceManager().add(dataMO, opResult) == common::errorCode::ERR_NO_ERRORS );

			if( result )
			{
				AES_DDT_TRACE_MESSAGE("Added a BlockPeer\n"
						"#####    DN: <%s>\n"
						"#####    BlockPeer: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    IP:<%s>\n",
						dataMO.getBlockPeerDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(), dataMO.getIpAddress());

				AES_DDT_LOG(LOG_LEVEL_INFO, "Added a BlockPeer\n"
						"#####    DN: <%s>\n"
						"#####    BlockPeer: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    IP:<%s>\n",
						dataMO.getBlockPeerDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(), dataMO.getIpAddress());
			}
			else
			{
				// Error on adding file peer
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to add Block Peer:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getBlockPeerDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to add Block Peer:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getBlockPeerDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );
			}
		}
		else
		{
			// getAttribute error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					blockPeerDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText());
		}

		return result;
	}

	bool DataReloadHelper::addOutputFormatFromDN(const std::string& outputFormatDN)
	{
		AES_DDT_TRACE_MESSAGE("Output Format DN:<%s> ", outputFormatDN.c_str());
		bool result = false;

		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		// to get the hold Time
		ACS_APGCC_ImmAttribute holdTimeAttribute;
		holdTimeAttribute.attrName.assign(imm::outputformat_attribute::HOLDTIME);
		attributeList.push_back(&holdTimeAttribute);

		// to get the dataChunkSize
		ACS_APGCC_ImmAttribute dataChunkSizeAttribute;
		dataChunkSizeAttribute.attrName.assign(imm::outputformat_attribute::DATACHUNKSIZE);
		attributeList.push_back(&dataChunkSizeAttribute);

		// to get the dataLengthType
		ACS_APGCC_ImmAttribute dataLengthTypeAttribute;
		dataLengthTypeAttribute.attrName.assign(imm::outputformat_attribute::DATALENGTHTYPE);
		attributeList.push_back(&dataLengthTypeAttribute);

		// to get the paddingChar
		ACS_APGCC_ImmAttribute paddingCharAttribute;
		paddingCharAttribute.attrName.assign(imm::outputformat_attribute::PADDINGCHAR);
		attributeList.push_back(&paddingCharAttribute);

		AES_DDT_TRACE_MESSAGE("get attributes from IMM");
		ACS_CC_ReturnType getResult = m_objectManager.getAttribute(outputFormatDN.c_str(), attributeList);

		operation::outputFormatInfo dataMO;

		if( (getResult == ACS_CC_SUCCESS) && (0U != holdTimeAttribute.attrValuesNum) &&
				(0U != dataChunkSizeAttribute.attrValuesNum) && (0U != dataLengthTypeAttribute.attrValuesNum) &&
				common::utility::getRDNValue(outputFormatDN, imm::datasource_attribute::RDN, dataMO.dataSourceName) &&
				common::utility::getRDNValue(outputFormatDN, imm::datasink_attribute::RDN, dataMO.dataSinkName) &&
				common::utility::getRDNValue(outputFormatDN, imm::outputformat_attribute::RDN, dataMO.name))
		{
			// make the value in upper case
			ACS_APGCC::toUpper(dataMO.dataSourceName);

			dataMO.holdTime = (*reinterpret_cast<uint32_t*>(holdTimeAttribute.attrValues[0]));
			dataMO.dataChunkSize = (*reinterpret_cast<uint32_t*>(dataChunkSizeAttribute.attrValues[0]));
			dataMO.dataLengthType = (*reinterpret_cast<int32_t*>(dataLengthTypeAttribute.attrValues[0]));

			if(0U != paddingCharAttribute.attrValuesNum)
				dataMO.paddingChar = (*reinterpret_cast<uint32_t*>(paddingCharAttribute.attrValues[0]));

			dataMO.moDN.assign(outputFormatDN);

			operation::result opResult;

			result = ( engine::workingSet_t::instance()->getDataSourceManager().add(dataMO, opResult) == common::errorCode::ERR_NO_ERRORS );

			if( result )
			{
				AES_DDT_TRACE_MESSAGE("Added an OutputFormat\n"
						"#####    DN: <%s>\n"
						"#####    OutputFormat: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    dataChunkSize: <%u>\n"
						"#####    dataLengthType: <%d>\n"
						"#####    holdTime: <%u>\n"
						"#####    paddingChar: <0x%02X>\n",
						dataMO.getOutputFormatDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(),
						dataMO.dataChunkSize, dataMO.dataLengthType, dataMO.holdTime, dataMO.paddingChar);

				AES_DDT_LOG(LOG_LEVEL_INFO, "Added an OutputFormat\n"
						"#####    DN: <%s>\n"
						"#####    OutputFormat: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    dataChunkSize: <%u>\n"
						"#####    dataLengthType: <%d>\n"
						"#####    holdTime: <%u>\n"
						"#####    paddingChar: <0x%02X>\n",
						dataMO.getOutputFormatDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(),
						dataMO.dataChunkSize, dataMO.dataLengthType, dataMO.holdTime, dataMO.paddingChar);
			}
			else
			{
				// Error on adding output format
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to add Output Format:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getOutputFormatDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to add Output Format:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getOutputFormatDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );
			}
		}
		else
		{
			// getAttribute error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					outputFormatDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText());
		}

		return result;
	}

	bool DataReloadHelper::addFileFormatFromDN(const std::string& fileFormatDN)
	{
		AES_DDT_TRACE_MESSAGE("File Format DN:<%s> ", fileFormatDN.c_str());
		bool result = false;

		//List of attributes to get
		std::vector<ACS_APGCC_ImmAttribute*> attributeList;

		// to get the fileSize
		ACS_APGCC_ImmAttribute fileSizeAttribute;
		fileSizeAttribute.attrName.assign(imm::fileformat_attribute::FILESIZE);
		attributeList.push_back(&fileSizeAttribute);

		// to get the startupSequenceNumber
		ACS_APGCC_ImmAttribute startupSequenceNumberAttribute;
		startupSequenceNumberAttribute.attrName.assign(imm::fileformat_attribute::STARTUP_SEQ_NUM);
		attributeList.push_back(&startupSequenceNumberAttribute);

		// to get the template
		ACS_APGCC_ImmAttribute templateAttribute;
		templateAttribute.attrName.assign(imm::fileformat_attribute::TEMPLATE);
		attributeList.push_back(&templateAttribute);

		AES_DDT_TRACE_MESSAGE("get attributes from IMM");
		ACS_CC_ReturnType getResult = m_objectManager.getAttribute(fileFormatDN.c_str(), attributeList);

		operation::fileFormatInfo dataMO;

		if( (getResult == ACS_CC_SUCCESS) && (0U != fileSizeAttribute.attrValuesNum) &&
				(0U != templateAttribute.attrValuesNum) &&
				common::utility::getRDNValue(fileFormatDN, imm::datasource_attribute::RDN, dataMO.dataSourceName) &&
				common::utility::getRDNValue(fileFormatDN, imm::datasink_attribute::RDN, dataMO.dataSinkName) &&
				common::utility::getRDNValue(fileFormatDN, imm::outputformat_attribute::RDN, dataMO.outputFormatName) &&
				common::utility::getRDNValue(fileFormatDN, imm::fileformat_attribute::RDN, dataMO.name))
		{
			// make the value in upper case
			ACS_APGCC::toUpper(dataMO.dataSourceName);

			dataMO.fileSize = (*reinterpret_cast<uint32_t*>(fileSizeAttribute.attrValues[0]));
			dataMO.templateValue = (reinterpret_cast<char*>(templateAttribute.attrValues[0]));

			if(0U != startupSequenceNumberAttribute.attrValuesNum)
				dataMO.startupSequenceNumber = (*reinterpret_cast<uint32_t*>(startupSequenceNumberAttribute.attrValues[0]));

			dataMO.moDN.assign(fileFormatDN);

			operation::result opResult;

			result = ( engine::workingSet_t::instance()->getDataSourceManager().add(dataMO, opResult) == common::errorCode::ERR_NO_ERRORS );

			if( result )
			{
				AES_DDT_TRACE_MESSAGE("Added an FileFormat\n"
						"#####    DN: <%s>\n"
						"#####    FileFormat: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    fileSize: <%u>\n"
						"#####    startupSequenceNumber: <%u>\n"
						"#####    templateValue: <%s>\n",
						dataMO.getFileFormatDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(),
						dataMO.fileSize, dataMO.startupSequenceNumber, dataMO.templateValue.c_str());

				AES_DDT_LOG(LOG_LEVEL_INFO, "Added an FileFormat\n"
						"#####    DN: <%s>\n"
						"#####    FileFormat: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    fileSize: <%u>\n"
						"#####    startupSequenceNumber: <%u>\n"
						"#####    templateValue: <%s>\n",
						dataMO.getFileFormatDN(), dataMO.getName(), dataMO.getDataSourceName(), dataMO.getDataSinkName(),
						dataMO.fileSize, dataMO.startupSequenceNumber, dataMO.templateValue.c_str());
			}
			else
			{
				// Error on adding file format
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to add File Format:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getFileFormatDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to add File Format:<%s>, DN:<%s>: errorCode:<%d>, errorMsg:<%s>", dataMO.getName(), dataMO.getFileFormatDN(),
						opResult.getErrorCode(), opResult.getErrorMessage() );
			}
		}
		else
		{
			// getAttribute error
			// OmHandler error
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OmHandler getAttribute(<%s>) failed!, errorCode:<%d> errorMsg:<%s>",
					fileFormatDN.c_str(), m_objectManager.getInternalLastError(), m_objectManager.getInternalLastErrorText());
		}

		return result;
	}


} /* namespace engine */
