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

#ifndef DDTADM_CAA_INC_AES_DDT_CHECKPOINTWRITER_H_
#define DDTADM_CAA_INC_AES_DDT_CHECKPOINTWRITER_H_

#include "store/checkpoint/storebase.h"
#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "boost/thread/mutex.hpp"
#else
	#include "stubs/store_stub.h"
	#include "cipher/contextinterface.h"
	#include "stubs/chkpointapi_stub.h"
	#include "stubs/boost_stub.h"
	#include "store/storeBaseTest.h"
#endif

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
		 * @class Writer
		 *
		 * @brief
		 * This class provides an implementation to operate on the underlying CheckPoint storing layer
		 *
		 * @sa StoreBase
		 */
		class Writer: public StoreBase
		{
		 public:
#ifdef CUTE_TEST
			friend StoreBaseTest;
#endif

			///  Constructor
			Writer(const std::string& dataSourceName,
							 const std::string& producerName,
							 const std::string& checkpointName,
							 const SaCkptHandleT& cktHandle,
							 const int& readEvent);

			///  Destructor.
			virtual ~Writer();

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
			 *	This method gets the number of data committed
			 *
			 *  @param recordNumber: Number of records written
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNumberOfCommittedRecord(uint32_t& recordNumber);

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
			 *	This method force the change of the current checkpoint in use.
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int forceStoreChange();

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
			 *	This method initializes checkpoint to write state
			 *
			 *	@remarks Remarks
			 */
			virtual int initializeStoreState();

			/** @brief
			 *
			 *	This method gets from the storing manager a new checkpoint name
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNewCheckpoint();

			/** @brief
			 *
			 *	This method encrypt the source buffer and adds a section to the current checkpoint.
			 *
			 *  @param bufferToWrite: Pointer to the source of data to be store
			 *
			 *  @param bufferLength: Number of characters to write
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int addSection(const void* bufferToWrite, const uint32_t& bufferLength);

			/** @brief
			 *
			 *	This method adds a section to the current checkpoint.
			 *
			 *  @param bufferToWrite: Pointer to the source of data to be store
			 *
			 *  @param bufferLength: Number of characters to write
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int addSectionToCheckPoint(const void* bufferToWrite, const uint32_t& bufferLength);

			// The current checkpoint state
			boost::shared_ptr<state::BaseInterface> m_state;

			// to take exclusive access to internal data structures
			boost::mutex m_mutex;
		};

	} /* namespace ddt_ckpt */
}
#endif /* DDTADM_CAA_INC_AES_DDT_CHECKPOINTWRITER_H_ */
