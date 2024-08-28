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

#include "store/storingmanager.h"
#include "common/utility.h"
#include "common/programconstants.h"

#include <boost/thread.hpp>

#ifndef CUTE_TEST
	#include "store/strategyinterface.h"
	#include "store/handlerinterface.h"

	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_StoringManager)

namespace store
{
	bool StoringManager::setStoreStrategy(boost::shared_ptr<StrategyInterface> storeStrategy)
	{
		AES_DDT_TRACE_FUNCTION;
		m_storeStrategy = storeStrategy;
		bool result = rebuildStoreState();
		return result;
	}

	boost::shared_ptr<HandlerInterface> StoringManager::getWriter(const std::string& dataSourceName, const std::string& producerName)
	{
		AES_DDT_TRACE_MESSAGE("get a writer for [DS:<%s>, PRODUCER:<%s>]", dataSourceName.c_str(), producerName.c_str());
		boost::shared_ptr<HandlerInterface> storeWriter;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			storeWriter = m_storeStrategy->makeWriter(dataSourceName, producerName);
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED to get a writer for [DS:<%s>, PRODUCER:<%s>] since the StoreStrategy is NULL!",
					dataSourceName.c_str(), producerName.c_str());
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED to get a writer for [DS:<%s>, PRODUCER:<%s>] since the StoreStrategy is NULL!",
					dataSourceName.c_str(), producerName.c_str());
		}

		return storeWriter;
	}

	boost::shared_ptr<HandlerInterface> StoringManager::getReader(const std::string& dataSourceName, const std::string& producerName)
	{
		AES_DDT_TRACE_MESSAGE("get a reader for [DS:<%s>, PRODUCER:<%s>]", dataSourceName.c_str(), producerName.c_str());
		boost::shared_ptr<HandlerInterface> storeReader;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			storeReader = m_storeStrategy->makeReader(dataSourceName, producerName);
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED to get a reader for [DS:<%s>, PRODUCER:<%s>] since the StoreStrategy is NULL!",
					dataSourceName.c_str(), producerName.c_str());
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED to get a reader for [DS:<%s>, PRODUCER:<%s>] since the StoreStrategy is NULL!",
					dataSourceName.c_str(), producerName.c_str());
		}

		return storeReader;
	}

	boost::shared_ptr<HandlerInterface> StoringManager::resetReader(const std::string& dataSourceName, const std::string& producerName)
	{
		AES_DDT_TRACE_MESSAGE("reset the reader for [DS:<%s>, PRODUCER:<%s>]", dataSourceName.c_str(), producerName.c_str());
		boost::shared_ptr<HandlerInterface> storeReader;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			m_storeStrategy->destroyReader(dataSourceName, producerName);
			storeReader = m_storeStrategy->makeReader(dataSourceName, producerName);
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED to reset the reader for [DS:<%s>, PRODUCER:<%s>] since the StoreStrategy is NULL!",
						dataSourceName.c_str(), producerName.c_str());
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED to reset the reader for [DS:<%s>, PRODUCER:<%s>] since the StoreStrategy is NULL!",
								  dataSourceName.c_str(), producerName.c_str());
		}

		return storeReader;
	}

	void StoringManager::getProducers(const std::string& dataSourceName, std::list<std::string>& producers)
	{
		AES_DDT_TRACE_MESSAGE("get producer list of DS:<%s>", dataSourceName.c_str());

		producers.clear();

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			m_storeStrategy->getProducers(dataSourceName, producers);

			AES_DDT_TRACE_MESSAGE("found <%zu> producer for DS:<%s>", producers.size(), dataSourceName.c_str() );
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED to get producer list for DS:<%s> since the StoreStrategy is NULL!",
					dataSourceName.c_str());
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED to get producer list for DS:<%s> since the StoreStrategy is NULL!",
					dataSourceName.c_str());
		}
	}

	int StoringManager::getNewStoreName(const std::string& dataSourceName, const std::string& producerName, const uint64_t& uuid, std::string& storeName)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NULL_POINTER;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			result = m_storeStrategy->getNewStoreName(dataSourceName, producerName, uuid, storeName);
		}
		AES_DDT_TRACE_MESSAGE("result:<%d>", result);
		return result;
	}

	int StoringManager::removeStoreName(const std::string& dataSourceName, const std::string& producerName, const std::string& storeName)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NULL_POINTER;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			result = m_storeStrategy->removeStoreName(dataSourceName, producerName, storeName);
		}
		AES_DDT_TRACE_MESSAGE("result:<%d>", result);
		return result;
	}

	int StoringManager::getFirstStoreName(const std::string& dataSourceName, const std::string& producerName, std::string& storeName)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NULL_POINTER;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			result = m_storeStrategy->getFirstStoreName(dataSourceName, producerName, storeName);
		}
		AES_DDT_TRACE_MESSAGE("result:<%d>", result);
		return result;
	}

	int StoringManager::getNumberOfStores(const std::string& dataSourceName, const std::string& producerName)
	{
		AES_DDT_TRACE_FUNCTION;
		int numberOfStores = 0U;;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			numberOfStores = m_storeStrategy->getNumberOfStores(dataSourceName, producerName);
		}

		AES_DDT_TRACE_MESSAGE("[DS:<%s> Producer:<%s>] number of stores:<%d>",
				dataSourceName.c_str(), producerName.c_str(), numberOfStores);
		return numberOfStores;
	}

	bool StoringManager::storeEmpty(const std::string& dataSourceName)
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = true;
		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			// get list of producers
			std::list<std::string> producers;
			m_storeStrategy->getProducers(dataSourceName, producers);
			std::list<std::string>::const_iterator element;

			for(element = producers.begin(); producers.end() != element; ++element)
			{
				if(m_storeStrategy->getNumberOfStores(dataSourceName, *element) > 1)
				{
					// there are more of one checkpoint, so there are still data to send
					AES_DDT_TRACE_MESSAGE("Data Source:<%s>, cannot be deleted, since producer:<%s> has more stores active",
							dataSourceName.c_str(), element->c_str());
					result = false;
					break;
				}

				// There is one checkpoint, compare the last record written with the last record sent
				boost::shared_ptr<HandlerInterface> writer = m_storeStrategy->makeWriter(dataSourceName, *element);
				uint64_t lastWrittenRecord = 0U;
				writer->getLastMessage(lastWrittenRecord);

				boost::shared_ptr<HandlerInterface> reader = m_storeStrategy->makeReader(dataSourceName, *element);
				uint64_t lastSentRecord = 0U;
				reader->getLastMessage(lastSentRecord);

				AES_DDT_TRACE_MESSAGE("[<%s>@<%s>],last record written/sent: <%lu>/<%lu>",
						dataSourceName.c_str(), element->c_str(), lastWrittenRecord, lastSentRecord);

				// Check that all records into the checkpoint are been sent
				if(lastWrittenRecord > lastSentRecord )
				{
					// there are still records to send
					result = false;
					break;
				}
			}
		}
		else
		{
			result = false;
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FALSE since the StoreStrategy is NULL!");
			AES_DDT_TRACE_MESSAGE("ERROR: FALSE since the StoreStrategy is NULL!");
		}

		return result;
	}

	int StoringManager::getLastReceivedMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastReceivedMsgNo)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;
		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			result = m_storeStrategy->getLastReceivedMessage(dataSourceName, producerName, lastReceivedMsgNo);
		}
		else
		{
			result = common::errorCode::ERR_CKPT_STOPPED;
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED since the StoreStrategy is NULL!");
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED since the StoreStrategy is NULL!");
		}
		return result;
	}

	int StoringManager::getLastSentMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastSentMsgNo)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;
		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			result = m_storeStrategy->getLastSentMessage(dataSourceName, producerName, lastSentMsgNo);
		}
		else
		{
			result = common::errorCode::ERR_CKPT_STOPPED;
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED since the StoreStrategy is NULL!");
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED since the StoreStrategy is NULL!");
		}

		return result;
	}

	void StoringManager::subscribe(const std::string& dataSourceName, Observer* observer)
	{
		AES_DDT_TRACE_FUNCTION;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			m_storeStrategy->subscribe(dataSourceName, observer);
		}
	}

	void StoringManager::unsubscribe(const std::string& dataSourceName)
	{
		AES_DDT_TRACE_FUNCTION;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			m_storeStrategy->unsubscribe(dataSourceName);
		}
	}

	void StoringManager::clean(const std::string& dataSourceName)
	{
		AES_DDT_TRACE_MESSAGE("Remove Data Source:<%s>", dataSourceName.c_str());

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);


		if(m_storeStrategy)
		{
			m_storeStrategy->clean(dataSourceName);
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED since the StoreStrategy is NULL!");
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED since the StoreStrategy is NULL!");
		}
	}

	uint32_t StoringManager::getMaxNumberOfRecordsPerStore(const uint32_t& recordSize)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;
		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			result = m_storeStrategy->getMaxNumberOfRecordsPerStore(recordSize);
		}
		else
		{
			result = common::errorCode::ERR_CKPT_STOPPED;
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED since the StoreStrategy is NULL!");
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED since the StoreStrategy is NULL!");
		}

		return result;
	}

	void StoringManager::shutdown()
	{
		AES_DDT_TRACE_FUNCTION;

		// Get exclusive access to the below part
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		if(m_storeStrategy)
		{
			// gracefully shutdown with cleanup
			m_storeStrategy->shutdown();
			// destroy the current storing strategy
			m_storeStrategy.reset();
		}
		else
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED since the StoreStrategy is NULL!");
			AES_DDT_TRACE_MESSAGE("ERROR: FAILED since the StoreStrategy is NULL!");
		}
	}

	bool StoringManager::rebuildStoreState()
	{
		AES_DDT_TRACE_FUNCTION;
		bool result = true;
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "***** Starting storing state rebuild... *****");
		AES_DDT_TRACE_MESSAGE("***** Starting storing state rebuild... ***** ");

		std::list<std::string> dsNames;
		std::list<std::string>::const_iterator element;

		engine::workingSet_t::instance()->getDataSourceManager().getDataSourceNames(dsNames);

		// rebuild the store state of each data source according to the storing strategy in use
		for(element = dsNames.begin(); element != dsNames.end(); ++element)
		{
			result &= m_storeStrategy->rebuild(*element);
		}

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "***** Storing state re-builded, result:<%s> *****", common::utility::boolToString(result));
		AES_DDT_TRACE_MESSAGE("***** Storing state re-builded, result:<%s> *****", common::utility::boolToString(result));

		return result;
	}


} /* namespace ddt_ckpt */
