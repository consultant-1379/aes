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

#include "store/checkpoint/strategy.h"
#include "store/observer.h"

#include "common/utility.h"

#ifndef CUTE_TEST

	#include "store/checkpoint/writer.h"
	#include "store/checkpoint/reader.h"
	#include "store/checkpoint/statushandler.h"
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/programconstants.h"
	#include "common/configuration.h"

	#include <boost/lexical_cast.hpp>
	#include <boost/make_shared.hpp>
#endif

#include <sstream>
#include <iomanip>

#include <sys/eventfd.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_Strategy)

namespace store
{
	namespace checkpoint
	{
		namespace {
			const char HEX_PREFIX[] = "0x";
			const char FILL_CHAR = '0';
			const uint8_t UUID_MAX_SIZE = 16U;
			const uint64_t BASE_UUID = 0LLU;
		};

		namespace StoreContainer
		{
			const uint32_t MAX_NUMBER_OF_STORES_PER_DATASOURCE = 400U;
			const uint32_t MAX_NUMBER_OF_PRODUCERS = 18U;
			const uint32_t MIN_NUMBER_OF_STORES_PER_PRODUCER = 2U;
		}

		Strategy::Strategy()
		: StrategyInterface(),
		  m_stateHandlers(),
		  m_writers(),
		  m_readers(),
		  m_readEvents(),
		  m_observers(),
		  m_ckptServiceInitialized(false),
		  m_ckptServiceHandle(CheckPoint::INVALID_HANDLE),
		  m_numberOfCheckPoints(0),
		  m_memoryController()
		{
			checkpointServiceInit();
		}

		Strategy::~Strategy()
		{
			checkpointServiceFinalize();
		}

		bool Strategy::rebuild(const std::string& dataSourceName)
		{
			bool result = true;
			AES_DDT_TRACE_MESSAGE("STATE REBUILD FOR DS:<%s>", dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_INFO, "STATE REBUILD FOR DS:<%s>", dataSourceName.c_str());

			mapOfStateHandler_t::const_iterator element = m_stateHandlers.find(dataSourceName);

			if( m_stateHandlers.end() == element)
			{
				// create the checkpoint state handler
				result = addStoreStateHandler(dataSourceName);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "STATE HANDLER already present for DS:<%s>", dataSourceName.c_str());
				AES_DDT_TRACE_MESSAGE("ERROR: STATE HANDLER already present for DS:<%s>", dataSourceName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));

			return result;
		}

		boost::shared_ptr<HandlerInterface> Strategy::makeWriter(const std::string& dataSourceName, const std::string& producerName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str());

			boost::shared_ptr<HandlerInterface> writer;

			mapOfWriters_t::const_iterator element = m_writers.find(std::make_pair(dataSourceName, producerName));

			if( m_writers.end() != element )
			{
				// writers already created
				writer = element->second;
				AES_DDT_LOG(LOG_LEVEL_INFO, "Provided existing writer for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("Provided existing writer for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "Create a writer for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("Create a writer for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );

				mapOfStateHandler_t::const_iterator element = m_stateHandlers.find(dataSourceName);

				// add the store state handler if it is not already present
				if( (m_stateHandlers.end() == element) && addStoreStateHandler(dataSourceName) )
				{
					element = m_stateHandlers.find(dataSourceName);
				}

				if( m_stateHandlers.end() != element)
				{
					// get the last used checkpoint
					std::string checkPointName;
					element->second->getLastStoreName(producerName, checkPointName);

					// check if a new checkpoint must be added
					if( checkPointName.empty() &&
							(common::errorCode::ERR_NO_ERRORS != getNewStoreName(dataSourceName, producerName, BASE_UUID, checkPointName)) )
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "DS:<%s> Producer:<%s> writer cannot be created without a checkpoint!", dataSourceName.c_str(), producerName.c_str() );
						AES_DDT_TRACE_MESSAGE("ERROR: DS:<%s> Producer:<%s> writer cannot be created without a checkpoint!", dataSourceName.c_str(), producerName.c_str() );
						return writer;
					}

					int readEvent = getReadEvent(dataSourceName, producerName);

					if(common::event::INVALID != readEvent)
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "Creating Writer for, DS:<%s> Producer:<%s> ", dataSourceName.c_str(), producerName.c_str() );
						writer = boost::make_shared<Writer>(dataSourceName, producerName, checkPointName, m_ckptServiceHandle, readEvent);

						if( common::errorCode::ERR_NO_ERRORS == writer->initialize() )
						{
							m_writers[std::make_pair(dataSourceName, producerName)] = writer;

							notifyObservers(dataSourceName, producerName);
						}
						else
						{
							// writer initialize failed
							writer.reset();
						}
					}
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "State Handler of DS:<%s> not found, Failed to get writer for producer:<%s>", dataSourceName.c_str(), producerName.c_str() );
					AES_DDT_TRACE_MESSAGE("ERROR: State Handler of DS:<%s> not found, Failed to get writer for producer:<%s>", dataSourceName.c_str(), producerName.c_str() );
				}
			}

			return writer;
		}

		boost::shared_ptr<HandlerInterface> Strategy::makeReader(const std::string& dataSourceName, const std::string& producerName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str());

			boost::shared_ptr<HandlerInterface> reader;

			mapOfReaders_t::const_iterator element = m_readers.find(std::make_pair(dataSourceName, producerName));

			if( m_readers.end() != element )
			{
				// reader already created
				reader = element->second;
				AES_DDT_LOG(LOG_LEVEL_INFO, "Provided existing reader for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("Provided existing reader for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "Create a reader for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );
				AES_DDT_TRACE_MESSAGE("Create a reader for the producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );

				mapOfStateHandler_t::const_iterator element = m_stateHandlers.find(dataSourceName);

				if( m_stateHandlers.end() != element)
				{
					// get the first used checkpoint
					std::string checkPointName;
					element->second->getFirstStoreName(producerName, checkPointName);

					int readEvent = getReadEvent(dataSourceName, producerName);
					if(common::event::INVALID != readEvent)
					{
						AES_DDT_LOG(LOG_LEVEL_ERROR, "Creating Reader for , DS:<%s> Producer:<%s> ", dataSourceName.c_str(), producerName.c_str() );
						// create the reader
						reader = boost::make_shared<Reader>(dataSourceName, producerName, checkPointName, m_ckptServiceHandle, readEvent);

						int initResult = reader->initialize();
						if( common::errorCode::ERR_NO_ERRORS == initResult )
						{
							AES_DDT_LOG(LOG_LEVEL_INFO, "Successfully initialized reader! Result:<%d>, Data Source:<%s>, Producer:<%s>", initResult, dataSourceName.c_str(), producerName.c_str());
							AES_DDT_TRACE_MESSAGE("INFO: Successfully initialized reader! Result:<%d>, Data Source:<%s>, Producer:<%s>", initResult, dataSourceName.c_str(), producerName.c_str());
							m_readers[std::make_pair(dataSourceName, producerName)] = reader;
						}
						else
						{
							// reader initialize failed
							AES_DDT_LOG(LOG_LEVEL_ERROR, "Initialize reader failed! Error:<%d>, Data Source:<%s>, Producer:<%s>", initResult, dataSourceName.c_str(), producerName.c_str());
							AES_DDT_TRACE_MESSAGE("ERROR: Initialize reader failed! Error:<%d>, Data Source:<%s>, Producer:<%s>", initResult, dataSourceName.c_str(), producerName.c_str());
							reader.reset();
						}
					}
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "State Handler of DS:<%s> not found, Failed to get reader for producer:<%s>", dataSourceName.c_str(), producerName.c_str() );
					AES_DDT_TRACE_MESSAGE("ERROR: State Handler of DS:<%s> not found, Failed to get reader for producer:<%s>", dataSourceName.c_str(), producerName.c_str() );
				}
			}

			return reader;
		}

		void Strategy::destroyReader(const std::string& dataSourceName, const std::string& producerName)
		{
			AES_DDT_TRACE_FUNCTION;

			// find key pair
			readerKey_t key = std::make_pair(dataSourceName, producerName);
			mapOfReaders_t::iterator readerIt = m_readers.find(key);
			if(m_readers.end() != readerIt)
			{
				// Remove reader of this producer
				m_readers.erase(readerIt);

				// close readEvent for this producer
				mapOfReadEvent_t::iterator eventIt = m_readEvents.find(std::make_pair(dataSourceName, producerName));
				if(m_readEvents.end() != eventIt)
				{
					::close(eventIt->second);
					m_readEvents.erase(eventIt);
				}
			}
		}

		bool Strategy::getProducers(const std::string& dataSourceName, std::list<std::string>& producers)
		{
			AES_DDT_TRACE_FUNCTION;
			bool result = false;
			producers.clear();

			mapOfStateHandler_t::const_iterator element = m_stateHandlers.find(dataSourceName);

			if(m_stateHandlers.end() != element )
			{
				element->second->getProducers(producers);
				result = true;
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "State Handler for DS:<%s> not found", dataSourceName.c_str());
				AES_DDT_TRACE_MESSAGE("State Handler for DS:<%s> not found", dataSourceName.c_str());
			}

			return result;
		}

		int Strategy::getNewStoreName(const std::string& dataSourceName, const std::string& producerName, const uint64_t& uuid, std::string& storeName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str());

			storeName.clear();
			int result = common::errorCode::ERR_NO_ERRORS;

			uint64_t recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(dataSourceName);
			uint64_t maxNumberOfSections = getMaxNumberOfRecordsPerStore(recordSize);

			uint64_t neededMemory = recordSize * maxNumberOfSections;

			// Check memory level
			if( m_memoryController.allocate(neededMemory) )
			{
				AES_DDT_TRACE_MESSAGE("Memory Available");

				// assemble a new checkpoint name
				assembleCheckPointName(dataSourceName, producerName, uuid, storeName);

				// add it to the checkpoint state handler that exists of sure
				m_stateHandlers[dataSourceName]->addStoreName(producerName, storeName);
			}
			else
			{
				// No More MEMORY FREE
				result = common::errorCode::ERR_MEMORY_LIMIT_REACHED;

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Memory Limit Reached!, Request for Producer:<%s> of DS:<%s> REJECTED!",
						producerName.c_str(), dataSourceName.c_str());

				AES_DDT_TRACE_MESSAGE("Memory Limit Reached!, Request for Producer:<%s> of DS:<%s> REJECTED!",
						producerName.c_str(), dataSourceName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("Checkpoint Name:<%s>, result:<%d>", storeName.c_str(), result);
			return result;
		}

		int Strategy::removeStoreName(const std::string& dataSourceName, const std::string& producerName, const std::string& storeName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, remove ckpt:<%s>", producerName.c_str(), dataSourceName.c_str(), storeName.c_str());

			int result = common::errorCode::ERR_NO_ERRORS;
			mapOfStateHandler_t::const_iterator element = m_stateHandlers.find(dataSourceName);

			// remove the checkpoint from stateHandler
			if( (m_stateHandlers.end() != element) && !element->second->removeStoreName(storeName, producerName) )
			{
				// Failed to remove the checkpoint name from the status handler
				result = common::errorCode::ERR_CKPT_DELETE_FAILURE;
			}
			else
			{
				uint64_t recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(dataSourceName);
				uint64_t maxNumberOfSections = getMaxNumberOfRecordsPerStore(recordSize);

				uint64_t memoryAmount = recordSize * maxNumberOfSections;

				m_memoryController.free(memoryAmount);
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int Strategy::getFirstStoreName(const std::string& dataSourceName, const std::string& producerName, std::string& storeName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str());
			int result = common::errorCode::ERR_NO_ERRORS;

			mapOfStateHandler_t::const_iterator element = m_stateHandlers.find(dataSourceName);

			if( (m_stateHandlers.end() != element) && !element->second->getFirstStoreName(producerName, storeName) )
			{
				result = common::errorCode::ERR_CKPT_GETNEW_FAILURE;
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int Strategy::getNumberOfStores(const std::string& dataSourceName, const std::string& producerName) const
		{
			int numOfCkptst = 0U;
			mapOfStateHandler_t::const_iterator element = m_stateHandlers.find(dataSourceName);

			// add the store state handler if it is not already present
			if(m_stateHandlers.end() != element)
			{
				numOfCkptst = element->second->getNumberOfStores(producerName);
			}

			AES_DDT_TRACE_MESSAGE("[DS:<%s> Cp:<%s>], Number of used Checkpoints:<%d>",
					dataSourceName.c_str(), producerName.c_str(), numOfCkptst);

			return numOfCkptst;
		}

		int Strategy::getLastReceivedMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastReceivedMsgNo)
		{
			AES_DDT_TRACE_FUNCTION;

			int result = common::errorCode::ERR_WRITER_FAILURE;

			boost::shared_ptr<HandlerInterface> writer = makeWriter(dataSourceName, producerName);

			if(writer)
			{
				result = writer->getLastMessage(lastReceivedMsgNo);
			}

			return result;
		}

		int Strategy::getLastSentMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastSentMsgNo)
		{
			AES_DDT_TRACE_FUNCTION;
			int result = common::errorCode::ERR_READER_FAILURE;

			boost::shared_ptr<HandlerInterface> reader = makeReader(dataSourceName, producerName);

			if(reader)
			{
				result = reader->getLastMessage(lastSentMsgNo);
			}

			return result;
		}

		void Strategy::subscribe(const std::string& dataSourceName, store::Observer* observer)
		{
			AES_DDT_TRACE_MESSAGE("SUBSCRIBE OBSERVER FOR <%s>", dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "SUBSCRIBE OBSERVER FOR <%s>", dataSourceName.c_str());

			m_observers[dataSourceName] = observer;
		}

		void Strategy::unsubscribe(const std::string& dataSourceName)
		{
			AES_DDT_TRACE_MESSAGE("UNSUBSCRIBE OBSERVER FOR <%s>", dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "UNSUBSCRIBE OBSERVER FOR <%s>", dataSourceName.c_str());

			m_observers.erase(dataSourceName);
		}

		void Strategy::clean(const std::string& dataSourceName)
		{
			AES_DDT_TRACE_FUNCTION;
			std::list<std::string> producers;
			mapOfStateHandler_t::iterator statusHandlerIt = m_stateHandlers.find(dataSourceName);

			// remove observers
			m_observers.erase(dataSourceName);

			if(m_stateHandlers.end() != statusHandlerIt )
			{
				std::list<std::string> producers;
				statusHandlerIt->second->getProducers(producers);

				std::list<std::string>::const_iterator producerIt;

				for(producerIt = producers.begin(); producers.end() != producerIt; ++producerIt)
				{
					// Remove writer of this producer
					m_writers.erase(std::make_pair(dataSourceName, *producerIt));

					// Remove reader of this producer
					m_readers.erase(std::make_pair(dataSourceName, *producerIt));

					// close readEvent for this producer
					mapOfReadEvent_t::iterator eventIt = m_readEvents.find(std::make_pair(dataSourceName, *producerIt));
					if(m_readEvents.end() != eventIt)
					{
						::close(eventIt->second);
						m_readEvents.erase(eventIt);
					}

					// remove the checkpoint used by this producer
					std::string storeName;
					if(statusHandlerIt->second->getFirstStoreName(*producerIt, storeName))
					{
						// delete the checkpoint
						deleteCheckpoint(storeName);

						uint64_t recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(dataSourceName);
						uint64_t maxNumberOfSections = getMaxNumberOfRecordsPerStore(recordSize);

						uint64_t memoryAmount = recordSize * maxNumberOfSections;

						m_memoryController.free(memoryAmount);
					}
				}
				// remove the status checkpoint of this data source
				statusHandlerIt->second->clean();

				m_stateHandlers.erase(statusHandlerIt);
			}
		}

		void Strategy::shutdown()
		{
			AES_DDT_TRACE_FUNCTION;
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "ALL Writers, Readers and State Handlers are going to be stopped...");

			mapOfWriters_t::const_iterator element;

			// Put all writers in the stopped state
			for(element = m_writers.begin(); m_writers.end() != element; ++element)
			{
				element->second->shutdown();
			}

			// Put all readers in the stopped state
			for(element = m_readers.begin(); m_readers.end() != element; ++element)
			{
				element->second->shutdown();
			}

			// cleanup all internal structures
			m_observers.clear();
			m_writers.clear();
			m_readers.clear();
			m_stateHandlers.clear();

			// close all readEvent
			mapOfReadEvent_t::const_iterator eventIt;
			for(eventIt = m_readEvents.begin(); m_readEvents.end() != eventIt; ++eventIt )
			{
				::close(eventIt->second);
			}
			m_readEvents.clear();

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "ALL Writers, Readers and State Handlers are been stopped!");
		}

		uint32_t Strategy::getMaxNumberOfRecordsPerStore(const uint32_t& recordSize)
		{
			std::map<uint32_t, uint32_t>::iterator numberOfRecordsIterator = m_mapOfNumberOfRecordsPerStore.find(recordSize);
			uint32_t numberOfRecords = 0U;

			if (m_mapOfNumberOfRecordsPerStore.end() == numberOfRecordsIterator)
			{
				uint64_t maxUsableMemory = common::configuration_t::instance()->getMaxUsableMemory();

				uint32_t maxStorableRecords = maxUsableMemory / recordSize;

				uint32_t minimumValue = maxStorableRecords / StoreContainer::MAX_NUMBER_OF_STORES_PER_DATASOURCE;

				uint32_t maximumValue = maxStorableRecords / ( StoreContainer::MAX_NUMBER_OF_PRODUCERS * StoreContainer::MIN_NUMBER_OF_STORES_PER_PRODUCER );

				uint32_t gap = maximumValue - minimumValue;

				//Consider a 5% threshold of the gap
				minimumValue += 0.05f * gap;
				//maximumValue -= 0.05f * gap;

				//Round up to the closest power of 2
				numberOfRecords = common::utility::roundUpToPowerOfTw0(minimumValue);
				//maximumValue = common::utility::roundDownToPowerOfTw0(maximumValue);

				m_mapOfNumberOfRecordsPerStore.insert(std::make_pair<uint32_t, uint32_t>(recordSize, numberOfRecords));
			}
			else
			{
				numberOfRecords = numberOfRecordsIterator->second;
			}

			return numberOfRecords;

		}

		bool Strategy::addStoreStateHandler(const std::string& dataSourceName)
		{
			AES_DDT_TRACE_FUNCTION;

			// create the checkpoint state handler
			boost::shared_ptr<StatusHandlerInterface> chktStateHandler =
					boost::make_shared<StatusHandler>(dataSourceName, m_ckptServiceHandle);

			// reload all checkpoints info for all producers
			bool result = chktStateHandler->load();

			if (result)
			{
				// Add the checkpoint state handler to the internal map
				m_stateHandlers.insert(	std::make_pair(dataSourceName, chktStateHandler));

				// Update memory in use
				uint32_t numberOfCheckpoint = chktStateHandler->getNumberOfStores();

				if(numberOfCheckpoint > 0U)
				{
					uint64_t recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(dataSourceName);
					uint64_t maxNumberOfSections = getMaxNumberOfRecordsPerStore(recordSize);

					uint64_t currentUsedMemory = numberOfCheckpoint * (recordSize * maxNumberOfSections);

					m_memoryController.increaseUsed(currentUsedMemory);
				}

				AES_DDT_TRACE_MESSAGE("Store State Handler added for DS<%s>, there are <%zu> state handlers now",
						dataSourceName.c_str(), m_stateHandlers.size());
				AES_DDT_LOG(LOG_LEVEL_INFO, "Store State Handler added for DS<%s>, there are <%zu> state handlers now",
						dataSourceName.c_str(), m_stateHandlers.size());
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "STATE HANDLER STATE HANDLER load failed for DS:<%s>", dataSourceName.c_str());
				AES_DDT_TRACE_MESSAGE("ERROR: STATE HANDLER load failed for DS:<%s>", dataSourceName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		void Strategy::checkpointServiceInit()
		{
			AES_DDT_TRACE_FUNCTION;

			if(!m_ckptServiceInitialized)
			{
				// Service version to use
				SaVersionT version;
				version.releaseCode = CheckPoint::ReleaseCode;
				version.majorVersion = CheckPoint::MajorVersion;
				version.minorVersion = CheckPoint::MinorVersion;

				// Initializes the Checkpoint Service for the invoking process
				SaAisErrorT result = saCkptInitialize(&m_ckptServiceHandle, NULL, &version);

				if(SA_AIS_OK == result)
				{
					m_ckptServiceInitialized = true;
					AES_DDT_TRACE_MESSAGE("CheckPoint Service Ver:<%c.%d.%d> successfully initialized",
							version.releaseCode, version.majorVersion, version.minorVersion);
					AES_DDT_LOG(LOG_LEVEL_INFO, "CheckPoint Service Ver:<%c.%d.%d> successfully initialized",
							version.releaseCode, version.majorVersion, version.minorVersion);

				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to initialize CheckPoint Service, error:<%d>", result);
					AES_DDT_TRACE_MESSAGE("[ERROR] Failed to initialize CheckPoint Service, error:<%d>", result);
				}
			}
		}

		void Strategy::checkpointServiceFinalize()
		{
			AES_DDT_TRACE_FUNCTION;
			if(m_ckptServiceInitialized)
			{
				SaAisErrorT result = saCkptFinalize(m_ckptServiceHandle);

				AES_DDT_TRACE_MESSAGE("CheckPoint Service finalized with result:<%d>", result);
				AES_DDT_LOG(LOG_LEVEL_INFO, "CheckPoint Service finalized with result:<%d>", result);
			}
		}

		void Strategy::assembleCheckPointName(const std::string& dataSourceName, const std::string& producerName, const uint64_t& uuid, std::string& checkpointName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );
			// Assemble a checkpoint name e.g: "safCkpt=DDT_RCFILE_BC0_12345556
			checkpointName.clear();

			checkpointName.assign(CheckPoint::NamePrefix);
			checkpointName.push_back(CheckPoint::NameSplit);
			checkpointName.append(dataSourceName);
			checkpointName.push_back(CheckPoint::NameSplit);
			checkpointName.append(producerName);
			checkpointName.push_back(CheckPoint::NameSplit);

			// add a timestamp
			std::time_t timestamp = std::time(0);
			checkpointName.append(boost::lexical_cast<std::string>(timestamp));
			checkpointName.push_back(CheckPoint::NameSplit);

			// add uuid to avoid name collision
			std::stringstream uuidAsString;
			uuidAsString << HEX_PREFIX << std::hex << std::uppercase << std::right << std::setfill(FILL_CHAR) << std::setw(UUID_MAX_SIZE) << uuid;

			checkpointName.append(uuidAsString.str());

			AES_DDT_TRACE_MESSAGE("New CheckPoint Name:<%s>", checkpointName.c_str() );
		}

		void Strategy::notifyObservers(const std::string& dataSourceName, const std::string& producerName)
		{
			AES_DDT_TRACE_MESSAGE("Notifying Observers. %s is available for %s", producerName.c_str(), dataSourceName.c_str());
			mapOfObservers_t::const_iterator it = m_observers.find(dataSourceName);
			if(it != m_observers.end())
			{
				it->second->update(producerName);
				AES_DDT_TRACE_MESSAGE("[%s@%s] Observer notified", producerName.c_str(), dataSourceName.c_str());
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("[%s@%s] No Observers found", producerName.c_str(), dataSourceName.c_str());
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] No Observers found", producerName.c_str(), dataSourceName.c_str());
			}

		}

		void Strategy::deleteCheckpoint(const std::string& name)
		{
			AES_DDT_TRACE_MESSAGE("Checkpoint:<%s>", name.c_str());

			// Set status checkpoint name
			SaNameT checkpointName;
			memset(checkpointName.value, 0U, sizeof(checkpointName.value)/sizeof(checkpointName.value[0]));
			checkpointName.length = name.length();
			memcpy(checkpointName.value, name.c_str(), checkpointName.length);

			SaAisErrorT deleteResult = saCkptCheckpointUnlink(m_ckptServiceHandle, &checkpointName);

			if(SA_AIS_OK == deleteResult)
			{
				AES_DDT_TRACE_MESSAGE("Checkpoint:<%s> successfully deleted", name.c_str() );
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "Checkpoint:<%s> successfully deleted", name.c_str() );
			}
			else
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to delete Checkpoint:<%s>, error:<%d>", name.c_str(), deleteResult);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to delete Checkpoint:<%s>, error:<%d>", name.c_str(), deleteResult);
			}
		}

		int Strategy::getReadEvent(const std::string& dataSourceName, const std::string& producerName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), dataSourceName.c_str() );

			int readevent = common::event::INVALID;

			mapOfReadEvent_t::const_iterator eventIt = m_readEvents.find(std::make_pair(dataSourceName, producerName));

			if(m_readEvents.end() == eventIt)
			{
				// event not found create it
				readevent = eventfd(common::event::INITIAL_VALUE, common::event::FLAGS);

				if(common::event::INVALID == readevent)
				{
					// error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] Failed to create a read event", dataSourceName.c_str(), producerName.c_str() );
					AES_DDT_TRACE_MESSAGE("ERROR: [%s@%s] Failed to create a read event", dataSourceName.c_str(), producerName.c_str() );
				}
				else
				{
					// event created
					m_readEvents[std::make_pair(dataSourceName, producerName)] = readevent;
					AES_DDT_TRACE_MESSAGE("[%s@%s] read event:<%d> created, #readEvents:<%zu>", dataSourceName.c_str(), producerName.c_str(), readevent, m_readEvents.size() );
					AES_DDT_LOG(LOG_LEVEL_INFO, "[%s@%s] read event:<%d> created, #readEvents:<%zu>", dataSourceName.c_str(), producerName.c_str(), readevent, m_readEvents.size() );
				}
			}
			else
			{
				// Event already created
				readevent = eventIt->second;
				AES_DDT_TRACE_MESSAGE("[%s@%s] read event:<%d>", dataSourceName.c_str(), producerName.c_str(), readevent );
			}

			return readevent;
		}

	} /* namespace checkpoint */
}/* namespace store */
