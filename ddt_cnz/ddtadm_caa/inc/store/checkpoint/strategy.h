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

#ifndef DDTADM_CAA_INC_AES_DDT_CHECKPOINTSTRATEGY_H_
#define DDTADM_CAA_INC_AES_DDT_CHECKPOINTSTRATEGY_H_

#include "store/strategyinterface.h"
#include "store/checkpoint/memorycontroller.h"

#ifndef CUTE_TEST
	#include <saCkpt.h>
#else
	#include "store_stubs.h"
#endif

#include <map>

namespace store
{
	class StatusHandlerInterface;

	namespace checkpoint
	{
		/**
		 * @class Strategy
		 *
		 * @brief
		 * This class implements a strategy to store the data into the CheckPoint Service.
		 *
		 * @sa store::StrategyInterface
		 */
		class Strategy: public StrategyInterface
		{
		public:

			///  Constructor.
			Strategy();

			///  Destructor
			virtual ~Strategy();

			/** @brief
			 *
			 *	This method rebuilds at startup the state of all checkpoints belonging the same data source
			 *
			 *  @param dataSourceName: the data source name to rebuild
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			virtual bool rebuild(const std::string& dataSourceName);

			/** @brief
			 *
			 *	This method creates a new checkpoint writer object.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the default blade/cp name
			 *
			 *  @return a shared pointer to the writer object.
			 *
			 *	@remarks Remarks
			 */
			virtual boost::shared_ptr<HandlerInterface> makeWriter(const std::string& dataSourceName, const std::string& producerName);

			/** @brief
			 *
			 *	This method creates a new checkpoint reader object.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the default blade/cp name
			 *
			 *  @return a shared pointer to the reader object.
			 *
			 *	@remarks Remarks
			 */
			virtual boost::shared_ptr<HandlerInterface> makeReader(const std::string& dataSourceName, const std::string& producerName);

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
			virtual void destroyReader(const std::string& dataSourceName, const std::string& producerName);

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
			virtual bool getProducers(const std::string& dataSourceName, std::list<std::string>& producers);

			/** @brief
			 *
			 *	This method provides to a writer checkpoint a new checkpoint name.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the default blade or cp name
			 *
			 *  @param uuid: unique universal id
			 *
			 *  @param storeName: the new checkpoint name to use
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNewStoreName(const std::string& dataSourceName, const std::string& producerName, const uint64_t& uuid, std::string& storeName);

			/** @brief
			 *
			 *	This method gets the number of checkpoints used by specific producer of a data source.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the default blade or cp name
			 *
			 *  @return number of checkpoints.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNumberOfStores(const std::string& dataSourceName, const std::string& producerName) const;

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
			virtual int getLastReceivedMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastReceivedMsgNo);

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
			virtual int getLastSentMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastSentMsgNo);

			/** @brief
			 *
			 *	This method removes a checkpoint name used by specific producer of a data source.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the default blade or cp name
			 *
			 *  @param storeName: the checkpoint name to remove
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int removeStoreName(const std::string& dataSourceName, const std::string& producerName, const std::string& storeName);

			/** @brief
			 *
			 *	This method gets the first checkpoint name used by specific producer of a data source.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the default blade or cp name
			 *
			 *  @param storeName: the checkpoint name
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getFirstStoreName(const std::string& dataSourceName, const std::string& producerName, std::string& storeName);

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
			virtual void subscribe(const std::string& dataSourceName, store::Observer* observer);

			/** @brief
			 *
			 *	This method removes an observer of new producers.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *	@remarks Remarks
			 */
			virtual void unsubscribe(const std::string& dataSourceName);

			/** @brief
			 *
			 *	This method removes all store info of a specific data source.
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *	@remarks Remarks
			 */
			virtual void clean(const std::string& dataSourceName);

			/** @brief
			 *
			 *	This method saves the checkpoint state and releases all handlers.
			 *
			 *	@remarks Remarks
			 */
			virtual void shutdown();

			/** @brief
			 *
			 *	This method returns the number of records that a store can contain based on the recordSize.
			 *
			 *  @param recordSize: the max record size
			 *
			 *	@remarks Remarks
			 */
			virtual uint32_t getMaxNumberOfRecordsPerStore(const uint32_t& recordSize);

		private:

			/** @brief
			 *
			 *	This method adds a store state handler for a data source
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool addStoreStateHandler(const std::string& dataSourceName);

			/** @brief
			 *
			 *  This method initializes the Checkpoint Service for the invoking process
			 *  and registers the various callback functions. This function must be invoked
			 *  prior to the invocation of any other Checkpoint Service functionality.
			 *
			 *	@remarks Remarks
			 */
			void checkpointServiceInit();

			/** @brief
			 *
			 *  This method closes the association represented by the m_CkptServiceHandle parameter
			 *  between the invoking process and the Checkpoint Service.
			 *
			 *	@remarks Remarks
			 */
			void checkpointServiceFinalize();

			/** @brief
			 *
			 *	This method assembles a checkpoint name
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the producer name
			 *
			 *	@param uuid: unique universal id
			 *
			 *  @param checkpointName: the assembled checkpoint name
			 *
			 *	@remarks Remarks
			 */
			void assembleCheckPointName(const std::string& dataSourceName, const std::string& producerName, const uint64_t& uuid, std::string& checkpointName);

			/** @brief
			 *
			 *	This method informs observers that a new producer is available for a specific data source
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the producer name
			 *
			 *	@remarks Remarks
			 */
			void notifyObservers(const std::string& dataSourceName, const std::string& producerName);

			/** @brief
			 *
			 *	This method deletes a checkpoint from the Checkpoint Service
			 *
			 *  @param name: the checkpoint name
			 *
			 *	@remarks Remarks
			 */
			void deleteCheckpoint(const std::string& name);

			/** @brief
			 *
			 *	This method gets/creates an event file descriptor used by writer to signal new record written
			 *
			 *  @param dataSourceName: the data source name
			 *
			 *  @param producerName: the producer name
			 *
			 *  @return a new eventfd file descriptor on success otherwise INVALID_EVENT.
			 *
			 *	@remarks Remarks
			 */
			int getReadEvent(const std::string& dataSourceName, const std::string& producerName);

			typedef std::map<std::string, boost::shared_ptr<StatusHandlerInterface> > mapOfStateHandler_t;

			/// internal map of state handler of each dataSourceName.
			mapOfStateHandler_t m_stateHandlers;

			// *** Writer define ***
			/// map key composed by the pair: <dataSourceName, producerName>
			typedef std::pair<std::string, std::string> writerKey_t;

			typedef std::map<writerKey_t, boost::shared_ptr<HandlerInterface> > mapOfWriters_t;

			/// internal map of writers of each pair: <dataSourceName, producerName>
			mapOfWriters_t m_writers;

			// *** Reader define ***
			/// map key composed by the pair: <dataSourceName, producerName>
			typedef std::pair<std::string, std::string> readerKey_t;

			typedef std::map<readerKey_t, boost::shared_ptr<HandlerInterface> > mapOfReaders_t;

			/// internal map of writers of each pair: <dataSourceName, producerName>
			mapOfReaders_t m_readers;

			/// map key composed by the pair: <dataSourceName, producerName>
			typedef std::pair<std::string, std::string> readEvent_t;

			typedef std::map<readEvent_t, int> mapOfReadEvent_t;

			/// internal map of read event of each pair: <dataSourceName, producerName>
			mapOfReadEvent_t m_readEvents;

			/// map: < DATA SOURCE NAME, OBSERVER POINTER (DataSink) >
			typedef std::map<std::string, store::Observer* > mapOfObservers_t;

			/// Map of Observers notified once a new producer becomes available
			mapOfObservers_t m_observers;

			/// CheckPoint service initialization status flag
			bool m_ckptServiceInitialized;

			/// Checkpoint service handle
			SaCkptHandleT m_ckptServiceHandle;

			/// counter of checkpoints in use
			uint32_t m_numberOfCheckPoints;

			/// Memory controller
			MemoryController m_memoryController;

			/// Map of number of records in one store container
			std::map<uint32_t, uint32_t> m_mapOfNumberOfRecordsPerStore;
		};

	} /* namespace checkpoint */
}/* namespace store */
#endif /* DDTADM_CAA_INC_AES_DDT_CHECKPOINTSTRATEGY_H_ */
