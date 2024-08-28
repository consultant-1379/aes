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

#ifndef DDTADM_CAA_INC_AES_DDT_CHECKPOINTSTATEHANDLER_H_
#define DDTADM_CAA_INC_AES_DDT_CHECKPOINTSTATEHANDLER_H_

#ifndef CUTE_TEST
	#include <boost/shared_ptr.hpp>
	#include <saCkpt.h>
#else
	#include "stubs/store_stub.h"
	#include "stubs/chkpointapi_stub.h"
	#include "store/StatusHandlerTest.h"
#endif

#include "store/statushandlerinterface.h"

#include <map>
namespace store
{
	namespace checkpoint
	{
		class ProducerInfo;

		/**
		 * @class StatusHandler
		 *
		 * @brief
		 * This class provides a concrete implementation to manage the status of checkpoints
		 * related to a specific data source.
		 *
		 */
		class StatusHandler: public StatusHandlerInterface
		{
		public:

			#ifdef CUTE_TEST
				friend StatusHandlerTest;
			#endif

			///  Constructor
			StatusHandler(const std::string& dataSourceName, const SaCkptHandleT& cktHandle);

			///  Destructor.
			virtual ~StatusHandler();

			/** @brief
			 *
			 *	This method gets the name of the data source
			 *
			 *  @return the data source name as a chars pointer
			 *
			 *	@remarks Remarks
			 */
			const char* getDataSourceName() const { return m_dataSourceName.c_str(); }

			/** @brief
			 *
			 *	This method loads the checkpoint state (producers and their defined checkpoints) of the data source
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			virtual bool load();

			/** @brief
			 *
			 *	This method adds a new checkpoint name used by a producer.
			 *
			 *  @param producerName: the producer name
			 *
			 *  @param storeName: the new checkpoint name to add
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			virtual bool addStoreName(const std::string& producerName, const std::string& storeName);

			/** @brief
			 *
			 *	This method removes a checkpoint name used by a producer.
			 *
			 *  @param storeName: the checkpoint name
			 *
			 *  @param producerName: the producer name
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			virtual bool removeStoreName(const std::string& storeName, const std::string& producerName);

			/** @brief
			 *
			 *	This method gets the last checkpoint name used by a producer.
			 *
			 *  @param producerName: the producer name
			 *
			 *  @param storeName: the checkpoint name
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			virtual bool getLastStoreName(const std::string& producerName, std::string& storeName);

			/** @brief
			 *
			 *	This method gets the first checkpoint name used by a producer.
			 *
			 *  @param producerName: the producer name
			 *
			 *  @param storeName: the checkpoint name
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			virtual bool getFirstStoreName(const std::string& producerName, std::string& storeName);

			/** @brief
			 *
			 *	This method gets the number of checkpoints used by a producer.
			 *
			 *  @param producerName: the producer name
			 *
			 *  @return the number of checkpoints.
			 *
			 *	@remarks Remarks
			 */
			virtual size_t getNumberOfStores(const std::string& producerName) const;

			/** @brief
			 *
			 *	This method gets the number of stores in use.
			 *
			 *  @return the number of stores.
			 *
			 *	@remarks Remarks
			 */
			virtual size_t getNumberOfStores() const;

			/** @brief
			 *
			 *	This method gets the list of data producers
			 *
			 *  @param producerList: the producer names list
			 *
			 *	@remarks Remarks
			 */
			virtual void getProducers(std::list<std::string>& producerList) const;

			/** @brief
			 *
			 *	This method removes all store info related to a specific data source
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			virtual bool clean();

		 private:

			/** @brief
			 *
			 *	This method opens the status checkpoint of a specific data source.
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool openStatusCheckpoint();

			/** @brief
			 *
			 *	This method reads all sections into the status checkpoint of a specific data source.
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool readStatusCheckpointSections();

			/** @brief
			 *
			 *	This method closes the status checkpoint of a specific data source.
			 *
			 *	@remarks Remarks
			 */
			void closeStatusCheckpoint();

			/** @brief
			 *
			 *	This method creates the info about used checkpoints of a producer.
			 *
			 *  @param producerCkpInfo: the checkpoint info handler of a specific producer
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool createProducerSectionInfo(const boost::shared_ptr<ProducerInfo>& producerCkpInfo);

			/** @brief
			 *
			 *	This method updates the info about used checkpoints of a producer.
			 *
			 *  @param producerName: the producer name
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool updateProducerSectionInfo(const boost::shared_ptr<ProducerInfo>& producerCkpInfo);

			/** @brief
			 *
			 *	This method deletes the status checkpoint
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool deleteStatusCheckpoint();

			/// name of data source at which stores belong
			std::string m_dataSourceName;

			typedef std::map<std::string, boost::shared_ptr<ProducerInfo> > mapOfProducer_t;

			/// internal map of checkpoints info of each producer.
			/// E.g. :<BCO, producerCheckPointsInfo*>, <CP1, producerCheckPointsInfo*>
			mapOfProducer_t m_producers;

			/// Checkpoint service handle
			SaCkptHandleT m_CkptServiceHandle;

			/// Checkpoint handle
			SaCkptCheckpointHandleT m_checkpointHandle;
		};

	} /* namespace checkpoint */
}/* namespace store */
#endif /* DDTADM_CAA_INC_AES_DDT_CHECKPOINTSTATEHANDLER_H_ */
