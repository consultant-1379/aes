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
#include "imm/imm.h"
#include "engine/datasourcemanager.h"
#include "engine/datasource.h"
#include "engine/context.h"
#include "engine/datareloadhelper.h"
#include "common/macros.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/blockpeer.h"
	#include "engine/filepeer.h"
	#include "engine/sftpconnection.h"
	#include "engine/ftpconnection.h"
	#include "engine/datasink.h"
	#include "engine/fileformat.h"
	#include "engine/outputformat.h"
	#include "engine/workingset.h"
	#include "common/utility.h"
	#include "common/tracer.h"
	#include "common/logger.h"

	#include <ACS_APGCC_Util.H>
	#include <acs_apgcc_omhandler.h>
#else
	#include "stubs/operation_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_DataSourceManager)

namespace engine
{
	namespace
	{
		const uint32_t DEFAULT_RECORDSIZE = 2048U;
	}

	DataSourceManager::DataSourceManager()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	DataSourceManager::~DataSourceManager()
	{
		AES_DDT_TRACE_FUNCTION;
		//TODO remove the context!
	}

	int DataSourceManager::add(const operation::dataSourceInfo& data, operation::result& op_result, bool startTask)
	{
		AES_DDT_TRACE_FUNCTION;

		Context *context = getContext(data.getDataSourceName());
		if (context && context->hasDataSource()) //Context already contains a Data Source
		{
			op_result.set(common::errorCode::ERR_DATASOURCE_ALREADY_DEFINED, imm::comcli_errormessage::DATASOURCE_ALREADY_DEFINED);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Data Source:<%s> already defined", data.getName() );
			AES_DDT_TRACE_MESSAGE("ERROR: Data Source:<%s> already defined", data.getName() );
		}
		else
		{
			//Create the data source
			boost::shared_ptr<DataSource> dataSource = boost::make_shared<DataSource>(data.name, data.recordSize, data.moDN);
			if (!dataSource)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot allocate memory for Data Source '%s'", data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: Cannot allocate memory for Data Source '%s'", data.getName() );

				op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
			}

			////////////////////////////////////////////////////////////
			//if the context is not present in the map, create it
			if(!context)
			{
				context = new (std::nothrow) Context(data.getDataSourceName());
				if(!context)
				{
					//Memory Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot allocate memory for Context '%s'", data.getDataSourceName() );
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot allocate memory for Context '%s'", data.getDataSourceName() );

					//dataSource goes out of scope and will be deleted automatically

					op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_INFO, "Add context to the MAP for Data Source: <%s>", data.getDataSourceName() );
					AES_DDT_TRACE_MESSAGE("Add context to the MAP for Data Source: <%s>", data.getDataSourceName() );

					//Add context to the map
					m_dataSourceMap.insert(dataSourceMap_t::value_type(data.getDataSourceName(), context));
				}
			}

			if(dataSource && context)
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "Preparing Data Source: <%s>", data.getName() );
				AES_DDT_TRACE_MESSAGE("Preparing Data Source: <%s>", data.getName() );

				int startResult = common::errorCode::ERR_NO_ERRORS;

				// On reload the task is started later
				if(startTask) startResult = dataSource->start();

				if (common::errorCode::ERR_NO_ERRORS == startResult)
				{
					context->setDataSource(dataSource);
					AES_DDT_TRACE_MESSAGE("[%s] DATA SOURCE <%s> added to the context", data.getDataSourceName(), data.getName());
					op_result.setErrorCode(common::errorCode::ERR_NO_ERRORS);
				}
				else
				{
					//NOTE: there is no need to reset dataSource from the context since it has never been set
					//Error
					//dataSource goes out of scope and will be deleted automatically
					op_result.set(startResult, imm::comcli_errormessage::GENERIC);
				}
			}
		}

		printMap();//debug
		return op_result.errorCode;
	}

	int DataSourceManager::add(const operation::dataSinkInfo& data, operation::result& op_result, bool startTask)
	{
		AES_DDT_TRACE_FUNCTION;

		Context *context = getContext(data.getDataSourceName());
		if (context && context->hasDataSink()) //Context alreay contains a Data Sink
		{
			op_result.set(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] Data Sink:<%s> already defined", data.getDataSourceName(), data.getName());
			AES_DDT_TRACE_MESSAGE("ERROR: [%s] Data Sink:<%s> already defined", data.getDataSourceName(), data.getName());
		}
		else
		{
			//create the data sink
			boost::shared_ptr<DataSink> dataSink = boost::make_shared<DataSink>(data);
			if (!dataSink)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Cannot allocate memory for Data Sink '%s'", data.getDataSourceName(), data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s] Cannot allocate memory for Data Sink '%s'", data.getDataSourceName(), data.getName() );

				//dataSink goes out of scope and will be deleted automatically
				op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
			}

			//if the context is not present in the map, create it
			if(!context)
			{
				context = new (std::nothrow) Context(data.getDataSourceName());
				if(!context)
				{
					//Memory Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot allocate memory for Context '%s'", data.getDataSourceName() );
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot allocate memory for Context '%s'", data.getDataSourceName() );


					op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Add context to the MAP for Data Sink: <%s>", data.getDataSourceName(), data.getName());
					AES_DDT_TRACE_MESSAGE("[%s] Add context to the MAP for Data Sink: <%s>", data.getDataSourceName(), data.getName());

					//Add context to the map
					m_dataSourceMap.insert(dataSourceMap_t::value_type(data.getDataSourceName(), context));
				}
			}

			if(dataSink && context)
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Preparing Data Sink: <%s>", data.getDataSourceName(), data.getName());
				AES_DDT_TRACE_MESSAGE("[%s] Preparing Data Sink: <%s>", data.getDataSourceName(), data.getName());

				int startResult = common::errorCode::ERR_NO_ERRORS;

				// On reload the task is started later
				if(startTask) startResult = dataSink->start();

				if (common::errorCode::ERR_NO_ERRORS == startResult)
				{
					context->setDataSink(dataSink);
					AES_DDT_TRACE_MESSAGE("[%s] DATA SINK <%s> added to the context", data.getDataSourceName(), data.getName());
					op_result.setErrorCode(common::errorCode::ERR_NO_ERRORS);
				}
				else
				{
					//NOTE: there is no need to reset dataSink from the context since it has never been set

					//Error
					//dataSink goes out of scope and will be deleted automatically
					op_result.set(startResult, imm::comcli_errormessage::GENERIC);
				}
			}
		}

		printMap();//debug
		return op_result.errorCode;
	}

	int DataSourceManager::add(const operation::filePeerInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;

		Context *context = getContext(data.getDataSourceName());
		if (context && (context->peerCount() > 1))
		{
			op_result.set(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] TOO MANY PEERS. Peer Count:<%zu>", data.getDataSourceName(), context->peerCount());
			AES_DDT_TRACE_MESSAGE("ERROR: [%s] TOO MANY PEERS. Peer Count:<%zu>", data.getDataSourceName(), context->peerCount());
		}
		else
		{
			//create the file peer
			boost::shared_ptr<FilePeer> filePeer = boost::make_shared<FilePeer>(data);

			/* XX
			if( common::PROTOCOL_SFTP == data.getTransferProtocol() )
			{
				// create a sftp file peer
				filePeer = boost::make_shared<SftpConnection>(data);
			}
			else
			{
				// create a ftp file peer
				filePeer = boost::make_shared<FtpConnection>(data);
			}
			*/

			if (!filePeer)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Cannot allocate memory for FilePeer '%s'", data.getDataSourceName(), data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s] Cannot allocate memory for FilePeer '%s'", data.getDataSourceName(), data.getName() );

				op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
			}

			//if the context is not present in the map, create it
			if(!context)
			{
				context = new (std::nothrow) Context(data.getDataSourceName());
				if(!context)
				{
					//Memory Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot allocate memory for Context '%s'", data.getDataSourceName() );
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot allocate memory for Context '%s'", data.getDataSourceName() );

					//filePeer will go out of scope
					op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Add context to the MAP for File Peer: <%s>", data.getDataSourceName(), data.getName() );
					AES_DDT_TRACE_MESSAGE("[%s] Add context to the MAP for File Peer: <%s>", data.getDataSourceName(), data.getName() );

					//Add context to the map
					m_dataSourceMap.insert(dataSourceMap_t::value_type(data.getDataSourceName(), context));
				}
			}

			if(filePeer && context)
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Starting File Peer: <%s>", data.getDataSourceName(), data.getName());
				AES_DDT_TRACE_MESSAGE("[%s] Starting File Peer: <%s>", data.getDataSourceName(), data.getName());

				int startResult = common::errorCode::ERR_NO_ERRORS; // = filePeer->start(); TODO: shall we start a thread?
				if (common::errorCode::ERR_NO_ERRORS == startResult)
				{
					context->setFilePeer(filePeer);

					if (context->hasDataSink())
					{
						context->getDataSink()->notifyValidPeer();
					}

					AES_DDT_TRACE_MESSAGE("[%s] FILE PEER <%s> added to the context", data.getDataSourceName(), data.getName());
					op_result.setErrorCode(common::errorCode::ERR_NO_ERRORS);
				}
				else
				{
					//NOTE: there is no need to reset filePeer from the context since it has never been set

					//Error
					//filePeer will go out of scope here
					op_result.set(startResult, imm::comcli_errormessage::GENERIC);
				}
			}
		}

		printMap();//debug
		return op_result.errorCode;
	}

	int DataSourceManager::add(const operation::blockPeerInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;

		Context *context = getContext(data.getDataSourceName());
		if (context && (context->peerCount() > 1))
		{
			op_result.set(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] TOO MANY PEERS. Peer Count:<%zu>", data.getDataSourceName(), context->peerCount());
			AES_DDT_TRACE_MESSAGE("ERROR: [%s] TOO MANY PEERS. Peer Count:<%zu>", data.getDataSourceName(), context->peerCount());
		}
		else
		{
			//create the block peer
			boost::shared_ptr<BlockPeer> blockPeer = boost::make_shared<BlockPeer>(data);
			if (!blockPeer)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Cannot allocate memory for BlockPeer '%s'", data.getDataSourceName(), data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s] Cannot allocate memory for BlockPeer '%s'", data.getDataSourceName(), data.getName() );

				op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
			}

			//if the context is not present in the map, create it
			if(!context)
			{
				context = new (std::nothrow) Context(data.getDataSourceName());
				if(!context)
				{
					//Memory Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot allocate memory for Context '%s'", data.getDataSourceName() );
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot allocate memory for Context '%s'", data.getDataSourceName() );

					//blockPeer goes out of scope here
					op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Add context to the MAP for Block Peer: <%s>", data.getDataSourceName(), data.getName() );
					AES_DDT_TRACE_MESSAGE("[%s] Add context to the MAP for Block Peer: <%s>", data.getDataSourceName(), data.getName() );

					//Add context to the map
					m_dataSourceMap.insert(dataSourceMap_t::value_type(data.getDataSourceName(), context));
				}
			}

			if(blockPeer && context)
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Starting Block Peer: <%s>", data.getDataSourceName(), data.getName());
				AES_DDT_TRACE_MESSAGE("[%s] Starting Block Peer: <%s>", data.getDataSourceName(), data.getName());

				int startResult = common::errorCode::ERR_NO_ERRORS; // = blockPeer->start(); TODO: shall we start a thread?
				if (common::errorCode::ERR_NO_ERRORS == startResult)
				{
					context->setBlockPeer(blockPeer);

					if (context->hasDataSink())
					{
						context->getDataSink()->notifyValidPeer();
					}

					AES_DDT_TRACE_MESSAGE("[%s] BLOCK PEER <%s> added to the context", data.getDataSourceName(), data.getName());
					op_result.setErrorCode(common::errorCode::ERR_NO_ERRORS);
				}
				else
				{
					//NOTE: there is no need to reset blockPeer from the context since it has never been set

					//Error
					//blockPeer goes out of scope here
					op_result.set(startResult, imm::comcli_errormessage::GENERIC);
				}
			}
		}

		printMap();//debug
		return op_result.errorCode;
	}

	int DataSourceManager::add(const operation::outputFormatInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;

		Context *context = getContext(data.getDataSourceName());
		if (context && (context->hasOutputFormat()))
		{
			op_result.set(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "OutputFormat already defined for <%s>", data.getDataSourceName());
			AES_DDT_TRACE_MESSAGE("ERROR: OutputFormat already defined for <%s>", data.getDataSourceName());
		}
		else
		{
			//create the outputFormat
			boost::shared_ptr<OutputFormat> outputFormat = boost::make_shared<OutputFormat>(data);
			if (!outputFormat)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Cannot allocate memory for OutputFormat '%s'", data.getDataSourceName(), data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s] Cannot allocate memory for OutputFormat '%s'", data.getDataSourceName(), data.getName() );

				op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
			}

			//if the context is not present in the map, create it
			if(!context)
			{
				context = new (std::nothrow) Context(data.getDataSourceName());
				if(!context)
				{
					//Memory Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot allocate memory for Context '%s'", data.getDataSourceName() );
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot allocate memory for Context '%s'", data.getDataSourceName() );

					//outputFormat goes out of scope
					op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Add context to the MAP for OutputFormat: <%s>", data.getDataSourceName(), data.getName() );
					AES_DDT_TRACE_MESSAGE("[%s] Add context to the MAP for OutputFormat: <%s>", data.getDataSourceName(), data.getName());

					//Add context to the map
					m_dataSourceMap.insert(dataSourceMap_t::value_type(data.getDataSourceName(), context));
				}
			}

			if(outputFormat && context)
			{
				// TODO: something more to todo here?
				context->setOutputFormat(outputFormat);
				AES_DDT_TRACE_MESSAGE("[%s] Output Format <%s> added to the context", data.getDataSourceName(), data.getName());
				op_result.setErrorCode(common::errorCode::ERR_NO_ERRORS);
			}
		}

		printMap();//debug
		return op_result.errorCode;
	}

	int DataSourceManager::add(const operation::fileFormatInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;

		Context *context = getContext(data.getDataSourceName());
		if (context && (context->hasFileFormat()))
		{
			op_result.set(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::GENERIC);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s] FileFormat already defined <%s>", data.getDataSourceName(), data.getName());
			AES_DDT_TRACE_MESSAGE("ERROR: [%s] FileFormat already defined <%s>", data.getDataSourceName(), data.getName());
		}
		else
		{
			//create the fileFormat
			boost::shared_ptr<FileFormat> fileFormat = boost::make_shared<FileFormat>(data);
			if (!fileFormat)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Cannot allocate memory for FileFormat '%s'", data.getDataSourceName(), data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s] Cannot allocate memory for FileFormat '%s'", data.getDataSourceName(), data.getName() );

				op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
			}

			//if the context is not present in the map, create it
			if(!context)
			{
				context = new (std::nothrow) Context(data.getDataSourceName());
				if(!context)
				{
					//Memory Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Cannot allocate memory for Context '%s'", data.getDataSourceName() );
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot allocate memory for Context '%s'", data.getDataSourceName() );

					//fileFormat goes out of scope
					op_result.set(common::errorCode::ERR_MEMORY_BAD_ALLOC, imm::comcli_errormessage::GENERIC);
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_INFO, "[%s] Add context to the MAP for File Format: <%s>", data.getDataSourceName(), data.getName());
					AES_DDT_TRACE_MESSAGE("[%s] Add context to the MAP for File Format: <%s>", data.getDataSourceName(), data.getName());

					//Add context to the map
					m_dataSourceMap.insert(dataSourceMap_t::value_type(data.getDataSourceName(), context));
				}
			}

			if(fileFormat && context)
			{
				// TODO: something more to todo here?
				fileFormat->initialize();
				context->setFileFormat(fileFormat);
				AES_DDT_TRACE_MESSAGE("[%s] File Format <%s> added to the context", data.getDataSourceName(), data.getName());
				op_result.setErrorCode(common::errorCode::ERR_NO_ERRORS);
			}
		}

		printMap();//debug
		return op_result.errorCode;
	}

	int DataSourceManager::remove(const operation::dataSourceInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS; //TODO review the setting of result

		Context* context = getContext(data.name);
		if(context && context->hasDataSource())
		{
			boost::shared_ptr<DataSource> definedDataSource = context->getDataSource();

			AES_DDT_LOG(LOG_LEVEL_INFO, "Removing Data Source: <%s>", definedDataSource->getDataSourceName() );
			AES_DDT_TRACE_MESSAGE("Removing Data Source: <%s>", definedDataSource->getDataSourceName() );

			int removeResult = definedDataSource->remove();

			// clean up checkpoints
			engine::workingSet_t::instance()->getStoringManager().clean(data.name);

			// memory clean-up
			context->resetDataSource();
			context->cleanup();
			removeContext(data.name);

			if(common::errorCode::ERR_NO_ERRORS != removeResult)
			{
				op_result.set(removeResult, imm::comcli_errormessage::DATASOURCE_NOT_REMOVABLE);
			}
		}

		return result;
	}

	int DataSourceManager::remove(const operation::dataSinkInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		Context* context = getContext(data.getDataSourceName());
		if(context && context->hasDataSink())
		{
			// Set Stop Event and wait for threads termination
			context->getDataSink()->stop();

			if(context->getFileFormat().get()) context->getFileFormat()->removeData();

			context->resetDataSink();
			context->resetFileFormat();
			context->resetOutputFormat();
			context->clearPeers();
		}

		return result;
	}

	int DataSourceManager::remove(const operation::filePeerInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		Context* context = getContext(data.getDataSourceName());
		if ( context )
		{
			result = context->resetPeerByName(data.getName());
		}

		if(common::errorCode::ERR_NO_ERRORS != result)
		{
			// Error  on Peer remove
			op_result.set(result, imm::comcli_errormessage::PEER_NOT_REMOVABLE);
		}

		return result;
	}


	//TODO REVIEW THIS METHOD ONCE THE DATASINK WILL BE LIVE
	int DataSourceManager::remove(const operation::blockPeerInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		Context* context = getContext(data.getDataSourceName());
		if ( context )
		{
			result = context->resetPeerByName(data.getName());
		}

		if(common::errorCode::ERR_NO_ERRORS != result)
		{
			// Error  on Peer remove
			op_result.set(result, imm::comcli_errormessage::PEER_NOT_REMOVABLE);
		}

		return result;
	}

	int DataSourceManager::modify(const operation::dataSourceInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;

		UNUSED(data);

		int result = common::errorCode::ERR_NO_ERRORS;

		op_result.set(common::errorCode::ERR_GENERIC, imm::comcli_errormessage::DATASOURCE_RECORDSIZE_NOT_CHANGEABLE);

		return result;
	}

	int DataSourceManager::modify(const operation::dataSinkInfo& data, operation::result& /*op_result*/)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		Context* context = getContext(data.getDataSourceName());
		if(context && context->hasDataSink())
		{
			result = context->getDataSink()->modify(data);
		}

		return result;
	}

	int DataSourceManager::modify(const operation::outputFormatInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		Context* context = getContext(data.getDataSourceName());
		if(context && context->hasOutputFormat())
		{
			//create the outputFormat
			boost::shared_ptr<OutputFormat> outputFormat = boost::make_shared<OutputFormat>(context->getOutputFormat());
			if (NULL == outputFormat)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Cannot allocate memory for OutputFormat '%s'", data.getDataSourceName(), data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s] Cannot allocate memory for OutputFormat '%s'", data.getDataSourceName(), data.getName() );

				result = common::errorCode::ERR_MEMORY_BAD_ALLOC;
				op_result.set(result, imm::comcli_errormessage::GENERIC);
			}
			else
			{
				outputFormat->modify(data);
				context->setOutputFormat(outputFormat);

				context->getDataSink()->notifyChange();

				AES_DDT_TRACE_MESSAGE("[%s] Output Format <%s> modified", data.getDataSourceName(), data.getName());
			}

		}

		return result;
	}

	int DataSourceManager::modify(const operation::fileFormatInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		Context* context = getContext(data.getDataSourceName());
		if(context && context->hasFileFormat())
		{
			//create the outputFormat
			boost::shared_ptr<FileFormat> fileFormat = boost::make_shared<FileFormat>(context->getFileFormat());
			if (NULL == fileFormat)
			{
				//Error
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s] Cannot allocate memory for FileFormat '%s'", data.getDataSourceName(), data.getName() );
				AES_DDT_TRACE_MESSAGE("ERROR: [%s] Cannot allocate memory for FileFormat '%s'", data.getDataSourceName(), data.getName() );

				result = common::errorCode::ERR_MEMORY_BAD_ALLOC;
				op_result.set(result, imm::comcli_errormessage::GENERIC);
			}
			else
			{
				fileFormat->modify(data);
				context->setFileFormat(fileFormat);

				context->getDataSink()->notifyChange();

				AES_DDT_TRACE_MESSAGE("[%s] File Format <%s> modified", data.getDataSourceName(), data.getName());
			}

		}

		return result;
	}

	int DataSourceManager::modify(const operation::filePeerInfo& data, operation::result& op_result)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		Context* context = getContext(data.getDataSourceName());

		if ( context )
		{
			boost::shared_ptr<Peer> peer = context->getPeerByName(data.getName());

			if (NULL != peer && peer->isFilePeer())
			{
				boost::shared_ptr<FilePeer> filePeer = boost::dynamic_pointer_cast<FilePeer>(peer);

				if (NULL != filePeer)
				{
					filePeer->modify(data);
				}
			}
		}

		return result;
	}

	int DataSourceManager::start()
	{


		AES_DDT_TRACE_FUNCTION;
		int failedCounter = 0;

		//start all resources in all contexts
		for(dataSourceMap_t::iterator it = m_dataSourceMap.begin(); it != m_dataSourceMap.end(); ++it)
		{
			Context* context = it->second;

			//START DATA SOURCE
			//Note: if we are here a Data Source Must exist hence getDataSource() cannot return null ;-)
			if(context->getDataSource()->start() != common::errorCode::ERR_NO_ERRORS)
			{
				failedCounter++;
			}

			//START DATA SINK
			//Note: getDataSink() returning NULL means that at restart the DataSink is not configured ;-)
			if(context->getDataSink() && context->getDataSink()->start() != common::errorCode::ERR_NO_ERRORS)
			{
				failedCounter++;
			}
		}

		return (failedCounter ? common::errorCode::ERR_OPEN : common::errorCode::ERR_NO_ERRORS);
	}

	int DataSourceManager::stop()
	{
		AES_DDT_TRACE_FUNCTION;
		int failedCounter = 0;

		//stop all resources in all contexts
		for(dataSourceMap_t::iterator it = m_dataSourceMap.begin(); it != m_dataSourceMap.end(); ++it)
		{
			Context* context = it->second;

			//STOP DATA SOURCE
			if(context->getDataSource()->stop() != common::errorCode::ERR_NO_ERRORS)
			{
				failedCounter++;
			}

			//STOP DATA SINK
			//Note: getDataSink() returning NULL means that at restart the DataSink is not configured ;-)
			if( context->hasDataSink() && (context->getDataSink()->stop() != common::errorCode::ERR_NO_ERRORS) )
			{
				failedCounter++;
			}

		}

		//DELETE OBJECTS
		for(dataSourceMap_t::iterator it = m_dataSourceMap.begin(); it != m_dataSourceMap.end(); ++it)
		{
			Context* context = it->second;

			context->cleanup();

			delete context;
		}

		//CLEAR MAP
		m_dataSourceMap.clear();

		return (failedCounter ? common::errorCode::ERR_CLOSE : common::errorCode::ERR_NO_ERRORS);
	}

	bool DataSourceManager::exist(const std::string& dataSourceName) const
	{
		AES_DDT_TRACE_FUNCTION;
		bool found = false;

		if( getDataSource(dataSourceName) )
		{
			found = true;
		}

		return found;
	}

	bool DataSourceManager::loadFromImm()
	{
		AES_DDT_TRACE_FUNCTION;

		DataReloadHelper loader;
		bool result = loader.reloadDataFromIMM();

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	void DataSourceManager::getDataSourceNames(std::list<std::string>& dataSourceList) const
	{
		AES_DDT_TRACE_FUNCTION;

		for(dataSourceMap_t::const_iterator it = m_dataSourceMap.begin(); it != m_dataSourceMap.end(); ++it)
		{
			std::string sourceName = it->first;
			dataSourceList.push_back(sourceName);
		}

		AES_DDT_TRACE_MESSAGE("Found <%zu> data sources", dataSourceList.size() );
	}

	uint32_t DataSourceManager::getDataSourceRecordSize(const std::string& dataSourceName) const
	{
		AES_DDT_TRACE_FUNCTION;
		uint32_t recordSize = DEFAULT_RECORDSIZE;

		// get data source object pointer
		boost::shared_ptr<DataSource> dataSource = getDataSource(dataSourceName);

		if( dataSource )
		{
			recordSize = dataSource->getRecordSize();
			AES_DDT_TRACE_MESSAGE("Data Source:<%s> has record size:<%u>.", dataSourceName.c_str(), recordSize);
		}

		return recordSize;
	}

	Context* DataSourceManager::getContext(const std::string& dataSourceName) const
	{
		Context* context = NULL;
		dataSourceMap_t::const_iterator it = m_dataSourceMap.find(dataSourceName);
		if (it != m_dataSourceMap.end())
		{
			context = (*it).second;
		}
		return context;
	}

	void DataSourceManager::removeContext(const std::string& dataSourceName)
	{
		dataSourceMap_t::iterator it = m_dataSourceMap.find(dataSourceName);
		if (it != m_dataSourceMap.end())
		{
			Context* context = (*it).second;
			delete context;

			m_dataSourceMap.erase(it);
		}
	}


	boost::shared_ptr<DataSource> DataSourceManager::getDataSource(const std::string& dataSourceName) const
	{
		AES_DDT_TRACE_FUNCTION;

		boost::shared_ptr<DataSource> dataSource;
		dataSourceMap_t::const_iterator it = m_dataSourceMap.find(dataSourceName);
		if (it != m_dataSourceMap.end())
		{
			dataSource = (it->second)->getDataSource();
		}
		return dataSource;
	}

	//UTILITY - PRINT THE MAP ON SCREEN
	void DataSourceManager::printMap()
	{
		printf("\n");
		printf("%-10s | %-7s | %-5s | %-8s | %-9s | %-10s  \n",
			   "KEY", "SOURCE", "SINK", "OUT FMT", "FILE FMT", "PEERS");

		printf("-----------|------------------------------------------------------------------------------------------\n");
		for(dataSourceMap_t::const_iterator it = m_dataSourceMap.begin(); it != m_dataSourceMap.end(); ++it)
		{
			Context *context = it->second;
			std::string key = it->first;
			std::string source = (context->hasDataSource() ? "YES" : "NO");
			std::string sink = (context->hasDataSink() ? "YES" : "NO");
			std::string outFmt = (context->hasOutputFormat() ? "YES" : "NO");
			std::string fileFmt = (context->hasFileFormat() ? "YES" : "NO");

			char peerChar[256] = {0}; snprintf(peerChar, sizeof(peerChar) - 1, "%zu", context->peerCount());

			printf("%-10s | %-7s | %-5s | %-8s | %-9s | %-10s \n",
				   key.c_str(), source.c_str(), sink.c_str(),
				   outFmt.c_str(), fileFmt.c_str(), peerChar);
		}
		printf("\n");
	}

}// namespace end

