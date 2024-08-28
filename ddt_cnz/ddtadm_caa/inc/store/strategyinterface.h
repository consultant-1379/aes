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

#ifndef DDTADM_CAA_INC_AES_DDT_STORESTRATEGYINTERFACE_H_
#define DDTADM_CAA_INC_AES_DDT_STORESTRATEGYINTERFACE_H_

#ifndef CUTE_TEST
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#else
#include "store_stubs.h"
#endif

#include <string>
#include <list>
#include <stdint.h>

namespace store
{
	class HandlerInterface;
	class Observer;

	/**
	 * @class StoreStrategyInterface
	 *
	 * @brief
	 * This class provides an abstraction of the strategy to be used to store data
	 *
	 */
	class StrategyInterface: private boost::noncopyable
	{
	public:

		///  Constructor.
		StrategyInterface() { }

		///  Destructor.
		virtual ~StrategyInterface() { }

		/** @brief
		 *
		 *	This method rebuilds at startup the state of all stores belonging the same data source
		 *
		 *  @param dataSourceName: the data source name to rebuild
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool rebuild(const std::string& dataSourceName) = 0;

		/** @brief
		 *
		 *	This method creates a new store writer object.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade/cp name
		 *
		 *  @return a shared pointer to the writer object.
		 *
		 *	@remarks Remarks
		 */
		virtual boost::shared_ptr<HandlerInterface> makeWriter(const std::string& dataSourceName, const std::string& producerName) = 0;

		/** @brief
		 *
		 *	This method creates a new store reader object.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade/cp name
		 *
		 *  @return a shared pointer to the reader object.
		 *
		 *	@remarks Remarks
		 */
		virtual boost::shared_ptr<HandlerInterface> makeReader(const std::string& dataSourceName, const std::string& producerName) = 0;

		/** @brief
		 *
		 *	This method destroys a reader object.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerName: the default blade/cp name
		 *
		 *  @return void
		 *
		 *	@remarks Remarks
		 */
		virtual void destroyReader(const std::string& dataSourceName, const std::string& producerName) = 0;

		/** @brief
		 *
		 *	This method gets the list of data producers of a specific data source
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *  @param producerList: the producer names list
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool getProducers(const std::string& dataSourceName, std::list<std::string>& producers) = 0;

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
		virtual int getNewStoreName(const std::string& dataSourceName, const std::string& producerName, const uint64_t& uuid, std::string& storeName) = 0;

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
		virtual int getNumberOfStores(const std::string& dataSourceName, const std::string& producerName) const = 0;

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
		virtual int getLastReceivedMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastReceivedMsgNo) = 0;

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
		virtual int getLastSentMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastSentMsgNo) = 0;

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
		virtual int removeStoreName(const std::string& dataSourceName, const std::string& producerName, const std::string& storeName) = 0;

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
		virtual int getFirstStoreName(const std::string& dataSourceName, const std::string& producerName, std::string& storeName) = 0;

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
		virtual void subscribe(const std::string& dataSourceName, Observer* observer) = 0;

		/** @brief
		 *
		 *	This method removes an observer of new producers.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *	@remarks Remarks
		 */
		virtual void unsubscribe(const std::string& dataSourceName) = 0;

		/** @brief
		 *
		 *	This method removes all store info of a specific data source.
		 *
		 *  @param dataSourceName: the data source name
		 *
		 *	@remarks Remarks
		 */
		virtual void clean(const std::string& dataSourceName) = 0;

		/** @brief
		 *
		 *	This method saves the global state of storing and releases all handlers.
		 *
		 *	@remarks Remarks
		 */
		virtual void shutdown() = 0;

		/** @brief
		 *
		 *	This method returns the number of records that a store can contain based on the recordSize.
		 *
		 *  @param recordSize: the max record size
		 *
		 *	@remarks Remarks
		 */
		virtual uint32_t getMaxNumberOfRecordsPerStore(const uint32_t& recordSize) = 0;

	};

} /* namespace ddt_ckpt */

#endif /* DDTADM_CAA_INC_AES_DDT_STORESTRATEGYINTERFACE_H_ */
