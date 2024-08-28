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
#ifndef DDTADM_CAA_INC_AES_DDT_STORINGMANAGER_H_
#define DDTADM_CAA_INC_AES_DDT_STORINGMANAGER_H_

#ifndef CUTE_TEST
	#include <boost/noncopyable.hpp>
	#include <boost/shared_ptr.hpp>
	#include "boost/thread/recursive_mutex.hpp"
#else
	#include "stubs/store_stub.h"
#endif

#include <map>
#include <list>
#include <string>
#include <utility>

namespace store
{
	class StrategyInterface;
	class HandlerInterface;
	class Observer;

	/**
	 * @class StoreManager
	 *
	 * @brief
	 * Singleton that provides an abstraction of the underlying real store layer
	 *
	 *
	 */
	class StoringManager: private boost::noncopyable
	{
	public:

		///  Constructor.
		StoringManager() : m_storeStrategy(), m_mutex() { }

		///  Destructor.
		virtual ~StoringManager() { }

		/** @brief
		 *
		 *	This method sets the store strategy to be used
		 *	and reload the last store status
		 *
		 *  @param storeStrategy: the store strategy to use
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool setStoreStrategy(boost::shared_ptr<StrategyInterface> storeStrategy);

		/** @brief
		 *
		 *	This method provides to the caller a writer object.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @return a pointer to the writer object.
		 *
		 *	@remarks Remarks
		 */
		boost::shared_ptr<HandlerInterface> getWriter(const std::string& dataSourceName, const std::string& producerName);

		/** @brief
		 *
		 *	This method provides to the caller a reader object.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @return a pointer to the reader object.
		 *
		 *	@remarks Remarks
		 */
		boost::shared_ptr<HandlerInterface> getReader(const std::string& dataSourceName, const std::string& producerName);

		/** @brief
		 *
		 *	This method provides to the caller a new reader object by resetting the existing one.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @return a pointer to the reader object.
		 *
		 *	@remarks Remarks
		 */
		boost::shared_ptr<HandlerInterface> resetReader(const std::string& dataSourceName, const std::string& producerName);

		/** @brief
		 *
		 *	This method gets a list of a data source producers.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producers: the producers list
		 *
		 *	@remarks Remarks
		 */
		void getProducers(const std::string& dataSourceName, std::list<std::string>& producers);

		/** @brief
		 *
		 *	This method provides to a writer object a new store name.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @param uuid: unique universal id
		 *
		 *  @param storeName: the new store name to use
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		int getNewStoreName(const std::string& dataSourceName, const std::string& producerName, const uint64_t& uuid, std::string& storeName);

		/** @brief
		 *
		 *	This method gets the number of stores used by specific producer of a data source.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @return true if the store is empty otherwise false.
		 *
		 *	@remarks Remarks
		 */
		int getNumberOfStores(const std::string& dataSourceName, const std::string& producerName);

		/** @brief
		 *
		 *	This method provides identification number of the last message received from the application source
		 *
		 *	@param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @param lastReceivedMsgNo : the last message received from application.
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		int getLastReceivedMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastReceivedMsgNo);

		/** @brief
		 *
		 *	This method provides the identification number of the last message successfully sent to the remote destination.
		 *
		 *	@param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @param lastSentMsgNo : the last message sent to remote.
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		int getLastSentMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastSentMsgNo);

		/** @brief
		 *
		 *	This method checks if a data source store is empty
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @return true if the store is empty otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool storeEmpty(const std::string& dataSourceName);

		/** @brief
		 *
		 *	This method removes a store used by specific producer of a data source.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @param storeName: the store name to remove
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		int removeStoreName(const std::string& dataSourceName, const std::string& producerName, const std::string& storeName);

		/** @brief
		 *
		 *	This method gets the first store name used by specific producer of a data source.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade or cp name
		 *
		 *  @param storeName: the store name
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		int getFirstStoreName(const std::string& dataSourceName, const std::string& producerName, std::string& storeName);

		/** @brief
		 *
		 *	This method allows observers to get notifications about new producers.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param observer: the Observer pointer
		 *
		 *	@remarks Remarks
		 */
		void subscribe(const std::string& dataSourceName, store::Observer* observer);

		/** @brief
		 *
		 *	This method unsubscribe an observer.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *	@remarks Remarks
		 */
		void unsubscribe(const std::string& dataSourceName);

		/** @brief
		 *
		 *	This method removes all store info  of a specific data source.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *	@remarks Remarks
		 */
		void clean(const std::string& dataSourceName);

		/** @brief
		 *
		 *	This method releases the storing strategy.
		 *
		 *	@remarks Remarks
		 */
		void shutdown();

		/** @brief
		 *
		 *	This method returns the number of records that a store can contain based on the recordSize.
		 *
		 *  @param recordSize: the max record size
		 *
		 *	@remarks Remarks
		 */
		 uint32_t getMaxNumberOfRecordsPerStore(const uint32_t& recordSize);

	private:

		/** @brief
		 *
		 *	This method rebuilds at startup the storing state of all defined data source
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool rebuildStoreState();

		/// internal store strategy
		boost::shared_ptr<StrategyInterface> m_storeStrategy;

		// to take exclusive access to internal data structures
		boost::recursive_mutex m_mutex;

	};

} /* namespace ddt_ckpt */

#endif /* DDTADM_CAA_INC_AES_DDT_STORINGMANAGER_H_ */
