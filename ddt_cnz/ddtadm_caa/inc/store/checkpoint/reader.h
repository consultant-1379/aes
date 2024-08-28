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

#ifndef DDTADM_CAA_INC_STORE_CHECKPOINT_READER_H_
#define DDTADM_CAA_INC_STORE_CHECKPOINT_READER_H_

#include "store/checkpoint/storebase.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include <boost/shared_ptr.hpp>
	#include "boost/thread/mutex.hpp"
#else
	#include "store_stubs.h"
#endif

#include <list>
#include <string>

namespace cipher
{
	class ContextInterface;
}

namespace store
{
	namespace checkpoint
	{
		namespace state
		{
			class BaseInterface;
		}

		/**
		 * @class Reader
		 *
		 * @brief
		 * This class provides an implementation to operate in read mode on the underlying CheckPoint storing layer
		 *
		 * @sa StoreBase
		 */
		class Reader: public StoreBase
		{
		 public:

			///  Constructor
			Reader(const std::string& dataSourceName,
					 const std::string& producerName,
					 const std::string& checkpointName,
					 const SaCkptHandleT& cktHandle,
					 const int& readEvent);

			///  Destructor.
			virtual ~Reader();

			/** @brief
			 *
			 *	This method writes data to the checkpoint storing layer.
			 *
			 *  @param source: Pointer to the source of data to be store
			 *
			 *  @param length: Number of characters to write
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int write(const void* source, const uint32_t& length);

			/** @brief
			 *
			 *	This method updates the message index to the recevied MTAP recod number.
			 *
			 *  @param recordNumber: Index fo next record to write
			 *
			 *	@remarks Remarks
			 */
			virtual void setMessageIndex(const uint32_t& recordNumber);

			/** @brief
			 *
			 *	This method reads data from the checkpoint storing layer.
			 *
			 *  @param destination: Pointer to the destination array where the content is to be copied
			 *
			 *  @param length: Number of characters to read
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int read(void* destination, uint32_t& length);

			/** @brief
			 *
			 *	This method delete data from the checkpoint storing layer.
			 *
			 *  @param numberOfRecords: Number of read messages that can be removed from the checkpoint
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int commit(const uint32_t& numberOfRecords);

			/** @brief
			 *
			 *	This method gets the number of sections written
			 *
			 *  @param recordNumber: Number of records written
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNumberOfWrittenRecord(uint32_t& recordNumber);

			/** @brief
			 *
			 *	This method gets the last message received from the application source.
			 *
			 *  @param lastMessageNumber: Last message received from the application.
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getLastMessage(uint64_t& lastMessageNumber);

			/** @brief
			 *
			 *	This method puts the store handler into the stopped state.
			 *
			 *	@remarks Remarks
			 */
			virtual void shutdown();

			/** @brief
			 *
			 *	This method initializes internal store state.
			 *
			 *	@remarks Remarks
			 */
			virtual int initialize();


		 protected:
			/** @brief
			 *
			 *	This method initializes the checkpoint to read state.
			 *
			 *	@remarks Remarks
			 */
			virtual int initializeStoreState();

			/** @brief
			 *
			 *	This method initialize the checkpoint iterator
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int checkpointIteratorInit();

			/** @brief
			 *
			 *	This method checks if the checkpoint empty condition is reached
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNextSectionToRead();

			/** @brief
			 *
			 *	This method finalize the checkpoint iterator
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int checkpointIteratorFinalize();

			/** @brief
			 *
			 *	This method checks if there are some records to commit
			 *
			 *  @return ERR_NO_ERRORS when there aren't records to commit otherwise ERR_CKPT_WAIT_COMMIT.
			 *
			 *	@remarks Remarks
			 */
			virtual int checkForCommit();

			/** @brief
			 *
			 *	This method deletes the checkpoint in use
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int checkpointDelete();

			/** @brief
			 *
			 *	This method gets from the storing manager a new checkpoint to read
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNewCheckpoint();

			/** @brief
			 *
			 *	This method reads data of the section specified by
			 *	the "m_sectionToReadDescriptor" attribute and decrypts if dataSecurity enabled.
			 *
			 *  @param destination: Pointer of the buffer to be used to store data
			 *
			 *  @param length: Buffer length in terms of bytes
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int readSection(void* destination, uint32_t& length);

			/** @brief
			 *
			 *	This method reads data of the section specified by
			 *	the "m_sectionToReadDescriptor" attribute
			 *
			 *  @param destination: Pointer of the buffer to be used to store data
			 *
			 *  @param length: Buffer length in terms of bytes
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int readSectionFromCheckPoint(void* destination, uint32_t& length);

			/** @brief
			 *
			 *	This method reads data of the section specified by
			 *	the "m_sectionToReadDescriptor" attribute
			 *
			 *  @param destination: Pointer of the buffer to be used to store data
			 *
			 *  @param length: Buffer length in terms of byttes
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int checkSectionToRead(const std::string& sectionId);

			/** @brief
			 *
			 *	This method deletes the checkpoint from the Checkpoint Service
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int checkpointPhysicalRemove();

			// The current checkpoint state
			boost::shared_ptr<state::BaseInterface> m_state;

			// to take exclusive access to internal data structures
			boost::mutex m_mutex;

			/// Checkpoint section info
			SaCkptSectionDescriptorT m_sectionToReadDescriptor;

			/// Flag to indicate when next section can be read
			bool m_readNext;

			/// Checkpoint iterator handle for stepping through the sections in the checkpoint
			SaCkptSectionIterationHandleT m_sectionIteratorHandle;

			/// list of read records from last commit
			std::list<std::string> m_readRecords;

		};

	} /* namespace checkpoint */
}
#endif /* DDTADM_CAA_INC_STORE_CHECKPOINT_READER_H_ */
