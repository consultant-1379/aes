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

#ifndef DDTADM_CAA_INC_AES_DDT_CHECKPOINTBASE_H_
#define DDTADM_CAA_INC_AES_DDT_CHECKPOINTBASE_H_


#ifndef CUTE_TEST
	#include <saCkpt.h>
	#include "store/checkpoint/checkpoint.h"
	#include "store/handlerinterface.h"
#else
	#include "stubs/store_stub.h"
	#include "stubs/chkpointapi_stub.h"
#endif

//NOTE: Define this ONLY to enable reader/writer debug files. DO NOT enable this in an official delivery!!!
//#define FILE_WRITE 1

#ifdef FILE_WRITE
	#include <fstream>
#endif

namespace store
{
	namespace checkpoint
	{
		namespace state
		{
			class Open;
			class ReadInfo;
			class IteratorInitialize;
			class Read;
			class WaitCommit;
			class WaitData;
			class Write;
			class UpdateInfo;
			class Close;
			class Delete;
			class GetNew;
		}
		/**
		 * @class StoreBase
		 *
		 * @brief
		 * This class provides an implementation to handle checkpoint operations.
		 *
		 */
		class StoreBase: public HandlerInterface
		{
		 	friend class state::Open;
			friend class state::ReadInfo;
			friend class state::IteratorInitialize;
			friend class state::Read;
			friend class state::WaitCommit;
			friend class state::WaitData;
			friend class state::Write;
			friend class state::UpdateInfo;
			friend class state::Close;
			friend class state::Delete;
			friend class state::GetNew;

		public:

			enum storeHandlerType_t
			{
				WRITER = 0,
				READER
			};

			/// Constructor.
			StoreBase(const storeHandlerType_t& storeType,
					  const std::string& dataSourceName,
					  const std::string& producerName,
					  const std::string& checkpointName,
					  const SaCkptHandleT& cktHandle,
					  const int& readEvent);

			///  Destructor.
			virtual ~StoreBase();

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
			 *	This method gets the data source name
			 *
			 *  @return char pointer of the data source name
			 *
			 *	@remarks Remarks
			 */
			inline const char* getDataSourceName() const { return m_dataSourceName.c_str(); }

			/** @brief
			 *
			 *	This method gets the producer name
			 *
			 *  @return char pointer of the producer name
			 *
			 *	@remarks Remarks
			 */
			inline const char* getProducerName() const { return m_producerName.c_str(); }

			/** @brief
			 *
			 *	This method gets the checkpoint name
			 *
			 *  @return char pointer of the checkpoint name
			 *
			 *	@remarks Remarks
			 */
			inline const char* getCheckPointName() const { return m_checkpointName.c_str(); }

			/** @brief
			 *
			 *	This method check the store handler type
			 *
			 *  @return true if it is a writer otherwise false
			 *
			 *	@remarks Remarks
			 */
			inline bool isWriter() const { return (WRITER == m_type); };

			/** @brief
			 *
			 *	This method returns the store read event
			 *
			 *	@remarks Remarks
			 */
			virtual int getReadEvent() const { return m_readEvent; };

		 protected:

			/** @brief
			 *
			 *	This method opens the current checkpoint.
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int openCheckpoint();

			/** @brief
			 *
			 *	This method closes the current checkpoint.
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int closeCheckpoint();

			/** @brief
			 *
			 *	This method reads buffer stored into the info section of the checkpoint
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int readInfoSection();

			/** @brief
			 *
			 *	This method converts the data buffer stored into the info section of the checkpoint
			 *	to the message index and message offset
			 *
			 *  @param infoBuffer: char buffer of info data
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int convertInfoBufferToData(const char* infoBuffer);

			/** @brief
			 *
			 *	This method creates the info section into the checkpoint
			 *	to store the message index and message offset
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int createInfoSection();

			/** @brief
			 *
			 *	This method updates the info section into the checkpoint
			 *	with the message index and message offset
			 *
			 *	@param maxRecordToStore: number of records in the checkpoint
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int updateInfoSection(uint32_t maxRecordToStore);

			/** @brief
			 *
			 *	This method updates the info section into the checkpoint
			 *	with the message index and message offset.
			 *	Number of records stored is the maximum number of records per checkpoint.
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int updateInfoSection();

			/** @brief
			 *
			 *	This method checks if the checkpoint full condition is reached
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			int isCheckPointFull() const;

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
			 *	This method finalize the checkpoint iterator
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int checkpointIteratorFinalize();

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
			 *	This method gets from the storing manager a new checkpoint name
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			virtual int getNewCheckpoint() = 0;

			/** @brief
			 *
			 *	This method writes the stub section into the checkpoint
			 *
			 *  @return ERR_NO_ERRORS on success otherwise an error code.
			 *
			 *	@remarks Remarks
			 */
			int addStubSection();

			// Store handler type: { Writer or Reader}
			storeHandlerType_t m_type;

			// data source name
			std::string m_dataSourceName;

			// the default blade\cp name
			std::string m_producerName;

			// the current checkpoint name in use
			std::string m_checkpointName;

			// Checkpoint service handle
			SaCkptHandleT m_CkptServiceHandle;

			/// Checkpoint handle
			SaCkptCheckpointHandleT m_checkpointHandle;

			// message offset
			uint64_t m_messageOffset;

			// current message index
			uint32_t m_messageIndex;

			/// Number of deleted sections
			uint32_t m_deletedSections;

			/// Record Size
			uint32_t m_recordSize;

			/// Number of sections per checkpoint
			uint32_t m_maxNumberOfSections;

			// read event
			int m_readEvent;

#ifdef FILE_WRITE
			std::fstream m_file;
#endif

		};

	} /* namespace ddt_ckpt */
}
#endif /* DDTADM_CAA_INC_AES_DDT_CHECKPOINTBASE_H_ */
