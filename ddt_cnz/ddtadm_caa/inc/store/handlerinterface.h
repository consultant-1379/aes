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

#ifndef DDTADM_CAA_INC_AES_DDT_STOREHANDLERINTERFACE_H_
#define DDTADM_CAA_INC_AES_DDT_STOREHANDLERINTERFACE_H_

#include <boost/noncopyable.hpp>
#include <string>

namespace store
{
	/**
	 * @class HandlerInterface
	 *
	 * @brief
	 * This class provides an interface to operate on an underlying generic storing layer
	 *
	 */
	class HandlerInterface: private boost::noncopyable
	{
	 public:

		///  Constructor.
		HandlerInterface() { };

		///  Destructor.
		virtual ~HandlerInterface() { }

		/** @brief
		 *
		 *	This method writes data to the underlying storing layer.
		 *
		 *  @param source: Pointer to the source of data to be store
		 *
		 *  @param length: Number of characters to write
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		virtual int write(const void* source, const uint32_t& length) = 0;

		/** @brief
		 *
		 *	This method updates the message index to the recevied MTAP recod number.
		 *
		 *  @param recordNumber: Index fo next record to write
		 *
		 *	@remarks Remarks
		 */
		virtual void setMessageIndex(const uint32_t& recordNumber) = 0;

		/** @brief
		 *
		 *	This method reads data from the underlying storing layer.
		 *
		 *  @param destination: Pointer to the destination array where the content is to be copied
		 *
		 *  @param length: Number of characters to read
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		virtual int read(void* destination, uint32_t& length) = 0;

		/** @brief
		 *
		 *	This method reads data from the underlying storing layer.
		 *
		 *  @param numberOfRecords: Number of read messages that can be removed from the storage
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		virtual int commit(const uint32_t& numberOfRecords) = 0;

		/** @brief
		 *
		 *	This method returns the store read event
		 *
		 *	@remarks Remarks
		 */
		virtual int getReadEvent() const = 0;

		/** @brief
		 *
		 *	This method gets the number of data stored
		 *
		 *  @param recordNumber: Number of records written
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		virtual int getNumberOfWrittenRecord(uint32_t& recordNumber) = 0;

		/** @brief
		 *
		 *	This method gets the identification number of last message received or sent.
		 *
		 *  @param lastMessageNumber: Last message received from the application or sent to remote.
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		virtual int getLastMessage(uint64_t& lastMessageNumber) = 0;

		/** @brief
		 *
		 *	This method force the change of the current store in use.
		 *
		 *  @return ERR_NO_ERRORS on success otherwise an error code.
		 *
		 *	@remarks Remarks
		 */
		virtual int forceStoreChange() = 0;

		/** @brief
		 *
		 *	This method puts the store handler into the stopped state.
		 *
		 *	@remarks Remarks
		 */
		virtual void shutdown() = 0;

		/** @brief
		 *
		 *	This method initializes internal store state.
		 *
		 *	@remarks Remarks
		 */
		virtual int initialize() = 0;

	};

} /* namespace ddt_ckpt */

#endif /* DDTADM_CAA_INC_AES_DDT_STOREHANDLERINTERFACE_H_ */
