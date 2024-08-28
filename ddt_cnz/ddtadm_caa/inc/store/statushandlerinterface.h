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

#ifndef DDTADM_CAA_INC_AES_DDT_STORESTATEHANDLERINTERFACE_H_
#define DDTADM_CAA_INC_AES_DDT_STORESTATEHANDLERINTERFACE_H_

#include <boost/noncopyable.hpp>
#ifndef CUTE_TEST
#else
	#include "stubs/store_stub.h"
#endif

#include <string>
#include <list>

namespace store
{
	/**
	 * @class StoreStateHandlerInterface
	 *
	 * @brief
	 * This class provides an interface to manage the state of generic stores
	 * related to a specific data source.
	 *
	 */
	class StatusHandlerInterface: private boost::noncopyable
	{
	 public:

		///  Constructor.
		StatusHandlerInterface() { };

		///  Destructor.
		virtual ~StatusHandlerInterface() { }

		/** @brief
		 *
		 *	This method gets the name of the data source
		 *
		 *  @return the data source name as a chars pointer
		 *
		 *	@remarks Remarks
		 */
		virtual const char* getDataSourceName() const = 0;

		/** @brief
		 *
		 *	This method loads the state(producers and their defined stores) of the data source
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool load() = 0;

		/** @brief
		 *
		 *	This method adds a new store name used by a producer.
		 *
		 *  @param producerName: the producer name
		 *
		 *  @param storeName: the new store name to add
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool addStoreName(const std::string& producerName, const std::string& storeName) = 0;

		/** @brief
		 *
		 *	This method removes a store name used by a producer.
		 *
		 *  @param storeName: the store name
		 *
		 *  @param producerName: the producer name
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool removeStoreName(const std::string& storeName, const std::string& producerName) = 0;

		/** @brief
		 *
		 *	This method gets the last store name used by a producer.
		 *
		 *  @param producerName: the producer name
		 *
		 *  @param storeName: the store name
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool getLastStoreName(const std::string& producerName, std::string& storeName) = 0;

		/** @brief
		 *
		 *	This method gets the first store name used by a producer.
		 *
		 *  @param producerName: the producer name
		 *
		 *  @param storeName: the store name
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool getFirstStoreName(const std::string& producerName, std::string& storeName) = 0;

		/** @brief
		 *
		 *	This method gets the number of stores used by a specific producer.
		 *
		 *  @param producerName: the producer name
		 *
		 *  @return the number of stores.
		 *
		 *	@remarks Remarks
		 */
		virtual size_t getNumberOfStores(const std::string& producerName) const = 0;

		/** @brief
		 *
		 *	This method gets the number of stores in use.
		 *
		 *  @return the number of stores.
		 *
		 *	@remarks Remarks
		 */
		virtual size_t getNumberOfStores() const = 0;

		/** @brief
		 *
		 *	This method gets the list of data producers
		 *
		 *  @param producerList: the producer names list
		 *
		 *	@remarks Remarks
		 */
		virtual void getProducers(std::list<std::string>& producerList) const = 0;

		/** @brief
		 *
		 *	This method removes all store info related to a specific data source
		 *
		 *  @return true on success otherwise false.
		 *
		 *	@remarks Remarks
		 */
		virtual bool clean() = 0;
	};

} /* namespace ddt_ckpt */

#endif /* DDTADM_CAA_INC_AES_DDT_STORESTATEHANDLERINTERFACE_H_ */
