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
#include "store/checkpoint/reader.h"

#include "store/checkpoint/state/iteratorinitialize.h"
#include "store/checkpoint/state/close.h"
#include "store/checkpoint/state/open.h"
#include "store/checkpoint/state/stop.h"

#include "common/programconstants.h"
#include "common/macros.h"

#include <boost/thread.hpp>

#ifndef CUTE_TEST
	#include "engine/workingset.h"

	#include "common/tracer.h"
	#include "common/logger.h"

	#include <boost/lexical_cast.hpp>
	#include <boost/make_shared.hpp>
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_Reader)

namespace store
{
	namespace checkpoint
	{

		Reader::Reader(const std::string& dataSourceName, const std::string& producerName,
				const std::string& checkpointName, const SaCkptHandleT& cktHandle, const int& readEvent)
		: StoreBase(StoreBase::READER, dataSourceName, producerName, checkpointName, cktHandle, readEvent),
		  m_state(boost::make_shared<state::Open>(this)),
		  m_mutex(),
		  m_sectionToReadDescriptor(),
		  m_readNext(true),
		  m_sectionIteratorHandle(CheckPoint::INVALID_HANDLE),
		  m_readRecords()
		{
			AES_DDT_TRACE_FUNCTION;
		}

		Reader::~Reader()
		{
			AES_DDT_TRACE_FUNCTION;
			checkpointIteratorFinalize();
			closeCheckpoint();
		}

		int Reader::write(const void* source, const uint32_t& length)
		{
			UNUSED(source);
			UNUSED(length);
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s> of DS:<%s> uses a Reader to write into Checkpoint:<%s>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str());

			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> uses a Reader to write into Checkpoint:<%s>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str());

			return common::errorCode::ERR_READER_BAD_USAGE;
		}

		void Reader::setMessageIndex(const uint32_t& recordNumber)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[<%s>@<%s>] Checkpoint:<%s>  Reader does not have permission to change message index to <%u>!",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), recordNumber);

			AES_DDT_TRACE_MESSAGE("[<%s>@<%s>] Checkpoint:<%s>  Reader does not have permission to change message index to <%u>!",
									m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), recordNumber);
		}

		int Reader::read(void* destination, uint32_t& length)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, data length:<%d>",
								m_producerName.c_str(), m_dataSourceName.c_str(), length );

			int result = common::errorCode::ERR_READER_FAILURE;

			boost::lock_guard<boost::mutex> guard(m_mutex);

			// Check if the checkpoint is already in the read state and
			// there are sections to read
			if( m_state->handleStep() == common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE)
			{
				// Checkpoint ready and section available
				result = readSection(destination, length);
			}
			else
			{
				// Checkpoint is not in the read state or no more section in the current checkpoint.
				// Bring the checkpoint in the "readable" state.
				// In case of error in a step, at next read call, the same previous failed step will be repeated again
				// The loop will ends when:
				// 		1) the read state is reached and aes_ddt::ERR_CKPT_SECTIONS_AVAILABLE is returned or
				// 		2) a step returns an error
				do
				{
					// Get the next step if the current one is not failed
					m_state->getNextStep(m_state);
					// Execute the step actions on the checkpoint
					result = m_state->handleStep();

				}while( (common::errorCode::ERR_NO_ERRORS == result) || (common::errorCode::ERR_CKPT_SECTIONS_EMPTY == result));

				if(common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE == result)
				{
					// Checkpoint ready and section available to read
					result = readSection(destination, length);
#ifdef FILE_WRITE
					m_file.write(static_cast<const char*>(destination), length);
#endif
				}
				else
				{
					m_state->getNextStep(m_state);
				}
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int Reader::commit(const uint32_t& numberOfRecords)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, #commit/#read:<%u>/<%zu>",
					m_producerName.c_str(), m_dataSourceName.c_str(), numberOfRecords, m_readRecords.size() );

			boost::lock_guard<boost::mutex> guard(m_mutex);

			uint32_t recordsToDelete = numberOfRecords;
			std::string sectionToDelete;

			std::list<std::string>::const_iterator element = m_readRecords.begin();
			for(; m_readRecords.end() != element && recordsToDelete > 0; ++element, --recordsToDelete )
			{
				sectionToDelete.assign(*element);

				// Assemble the section id
				char sectionId[CheckPoint::MaxSectionIdSize] = {0};
				sectionToDelete.copy(sectionId, sectionToDelete.length());

				AES_DDT_TRACE_MESSAGE("Deleting Section Producer:<%s> DS:<%s>, checkpoint:<%s> sectionID:<%s>",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), sectionId);

				SaCkptSectionIdT sectionIdentity;
				sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
				sectionIdentity.idLen = sectionToDelete.length();

				// Delete the section
				SaAisErrorT deleteResult = saCkptSectionDelete(m_checkpointHandle, &sectionIdentity);

				if(SA_AIS_OK != deleteResult)
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s>	of DS:<%s> delete section:<%s> failed, Checkpoint:<%s> error:<%u>",
							m_producerName.c_str(), m_dataSourceName.c_str(), sectionId, m_checkpointName.c_str(), deleteResult);

					AES_DDT_TRACE_MESSAGE("ERROR: Producer:<%s>	of DS:<%s> delete section:<%s> failed, Checkpoint:<%s> error:<%u>",
							m_producerName.c_str(), m_dataSourceName.c_str(), sectionId, m_checkpointName.c_str(), deleteResult);
				}
			}

			// save the number of deleted sections that is equal to the index of last deleted section plus one
			if(!sectionToDelete.empty()) m_deletedSections = boost::lexical_cast<uint32_t>(sectionToDelete) + 1U;

			// next read will start from the last section deleted plus one.
			m_state = boost::make_shared<state::IteratorInitialize>(this);

			m_readRecords.clear();
			return common::errorCode::ERR_NO_ERRORS;
		}

		int Reader::getNumberOfWrittenRecord(uint32_t& recordNumber)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

			boost::lock_guard<boost::mutex> guard(m_mutex);

			recordNumber = m_messageIndex;
			return 	common::errorCode::ERR_NO_ERRORS;
		}

		int Reader::getLastMessage(uint64_t& lastMessageNumber)
		{
			AES_DDT_TRACE_FUNCTION;
			int result = common::errorCode::ERR_NO_ERRORS;

			// reader already initialized
			lastMessageNumber = m_messageOffset + m_deletedSections;

			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, message Offset:<%lu>, last deleted:<%u>, last sent record:<%lu>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_messageOffset, m_deletedSections, lastMessageNumber );
			return result;
		}

		void Reader::shutdown()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );
			boost::lock_guard<boost::mutex> guard(m_mutex);
			m_state = boost::make_shared<state::Stop>(this);
		}

		int Reader::initialize()
		{
			AES_DDT_TRACE_FUNCTION;
			return initializeStoreState();
		}

		int Reader::initializeStoreState()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

			int result = common::errorCode::ERR_NO_ERRORS;
			// Bring the checkpoint in the "readable" state.
			// In case of error in a step the same previous failed step will be repeated again at any next method call

			// Execute the step actions on the checkpoint
			int stateResult = m_state->handleStep();

			// The loop will ends when:
			// 		1) the read state is reached and aes_ddt::ERR_CKPT_SECTIONS_AVAILABLE is returned or
			// 		2) a step returns an error
			while( common::errorCode::ERR_NO_ERRORS == stateResult )
			{
				// Get the next step if the current one is not failed
				m_state->getNextStep(m_state);

				// Execute the step actions on the checkpoint
				stateResult = m_state->handleStep();
			}

			AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s>	of DS:<%s>, Checkpoint:<%s> result:<%d>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), stateResult);

			if(common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE == stateResult)
			{
				// Checkpoint ready and section available to read
				char sectionName[CheckPoint::MaxSectionIdSize + 1U] = {0};
				snprintf(sectionName, m_sectionToReadDescriptor.sectionId.idLen + 1U, "%s", reinterpret_cast<char*>(m_sectionToReadDescriptor.sectionId.id) );

				// Number of sections deleted is equal to the index of the next section to read
				m_deletedSections = boost::lexical_cast<uint32_t>(sectionName);

			}
			else
			{
				// Checkpoint without section
				// Number of sections deleted is equal to the index of the next section to write
				m_deletedSections = m_messageIndex;
			}

			AES_DDT_LOG(LOG_LEVEL_DEBUG, "Producer:<%s>	of DS:<%s>, Checkpoint:<%s>, Msg Offset:<%lu>, Msg Index:<%u>, Last Deleted:<%u>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), m_messageOffset, m_messageIndex, m_deletedSections );

			return result;
		}


		int Reader::checkpointIteratorInit()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

			int result = common::errorCode::ERR_NO_ERRORS;

			// Checkpoint is being initialized: so m_readRecords and m_readNext flag shall be reset.
			m_readRecords.clear();
			m_readNext = true;


			// Finalize the current iterator if it is already init
			checkpointIteratorFinalize();

			// Searching existing sections fulfilling the specified criteria.
			SaCkptSectionsChosenT sectionSelectCriteria = CheckPoint::SectionSelectCriteria;

			// This field is ignored with the used selection criteria
			SaTimeT expirationTime = CheckPoint::SectionExpirationTime;

			SaAisErrorT iteratorInitResult = saCkptSectionIterationInitialize(m_checkpointHandle, sectionSelectCriteria, expirationTime, &m_sectionIteratorHandle);

			if(SA_AIS_OK != iteratorInitResult)
			{
				result = common::errorCode::ERR_CKPT_ITERATOR_INIT_FAILURE;
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s>	of DS:<%s> iterator initialize failed, Checkpoint:<%s> error:<%u>",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), iteratorInitResult);

				AES_DDT_TRACE_MESSAGE("ERROR: Producer:<%s>	of DS:<%s> iterator initialize failed, Checkpoint:<%s> error:<%u>",
						m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), iteratorInitResult);
			}

			AES_DDT_TRACE_MESSAGE("Checkpoint:<%s>, result:<%d>", m_checkpointName.c_str(), result);
			return result;
		}

		int Reader::getNextSectionToRead()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

			int result = common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE;

			if(m_readNext)
			{
				// get next section descriptor
				SaAisErrorT getNextResult = saCkptSectionIterationNext(m_sectionIteratorHandle, &m_sectionToReadDescriptor);

				if(SA_AIS_OK == getNextResult)
				{
					// Check the retrieved section
					char tmpName[CheckPoint::MaxSectionIdSize + 1U] = {0};
					snprintf(tmpName, m_sectionToReadDescriptor.sectionId.idLen + 1U, "%s", reinterpret_cast<char*>(m_sectionToReadDescriptor.sectionId.id) );
					std::string sectionId(tmpName);
					result = checkSectionToRead(sectionId);
				}
				else
				{
					result = common::errorCode::ERR_CKPT_ITERATOR_FAILURE;
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s> of DS:<%s> get next section descriptor failed, Checkpoint:<%s> error:<%u>",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), getNextResult);

					AES_DDT_TRACE_MESSAGE("ERROR: Producer:<%s> of DS:<%s> get next section descriptor failed, Checkpoint:<%s> error:<%u>",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), getNextResult);
				}

				// if a section is available before get a new section must be read it.
				m_readNext = (common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE != result);
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int Reader::checkpointIteratorFinalize()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

			int result = common::errorCode::ERR_NO_ERRORS;

			if(CheckPoint::INVALID_HANDLE != m_sectionIteratorHandle)
			{
				SaAisErrorT iteratorInitResult = saCkptSectionIterationFinalize(m_sectionIteratorHandle);

				if(SA_AIS_OK != iteratorInitResult)
				{
					result = common::errorCode::ERR_CKPT_ITERATOR_FIN_FAILURE;
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s>	of DS:<%s> iterator finalize failed, Checkpoint:<%s> error:<%u>",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), iteratorInitResult);

					AES_DDT_TRACE_MESSAGE("ERROR: Producer:<%s>	of DS:<%s> iterator finalize failed, Checkpoint:<%s> error:<%u>",
							m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), iteratorInitResult);
				}
				else
				{
					AES_DDT_TRACE_MESSAGE("Checkpoint Iterator finalized");
					m_sectionIteratorHandle = CheckPoint::INVALID_HANDLE;
				}
			}

			AES_DDT_TRACE_MESSAGE("Checkpoint:<%s>, result:<%d>", m_checkpointName.c_str(), result);
			return result;
		}

		int Reader::checkForCommit()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );
			int result = common::errorCode::ERR_NO_ERRORS;
			if(m_readRecords.size() != 0U)
			{
				AES_DDT_TRACE_MESSAGE("There are <%zu> records to commit", m_readRecords.size());
				result = common::errorCode::ERR_CKPT_WAIT_COMMIT;
			}

			AES_DDT_TRACE_MESSAGE("Checkpoint:<%s>, result:<%d>", m_checkpointName.c_str(), result);
			return result;
		}

		int Reader::checkpointDelete()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> remove ckpt:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str() );
			int result = common::errorCode::ERR_NO_ERRORS;

			// Logical remove of checkpoint from the status handler
			result = engine::workingSet_t::instance()->getStoringManager().removeStoreName(m_dataSourceName, m_producerName, m_checkpointName);

			if(common::errorCode::ERR_NO_ERRORS == result)
			{
				// Physical remove from the Checkpoint Service
				result = checkpointPhysicalRemove();
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int Reader::getNewCheckpoint()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

			int result = engine::workingSet_t::instance()->getStoringManager().getFirstStoreName(m_dataSourceName, m_producerName, m_checkpointName);

			AES_DDT_TRACE_MESSAGE("Checkpoint:<%s>, result:<%d>", m_checkpointName.c_str(), result);

			return result;
		}

		int Reader::readSection(void* destination, uint32_t& length)
		{
			AES_DDT_TRACE_FUNCTION;
			return readSectionFromCheckPoint(destination, length);
		}

		int Reader::readSectionFromCheckPoint(void* destination, uint32_t& length)
		{
			AES_DDT_TRACE_FUNCTION;

			int result = common::errorCode::ERR_NO_ERRORS;

			char sectionID[CheckPoint::MaxSectionIdSize + 1U] = {0};
			snprintf(sectionID, m_sectionToReadDescriptor.sectionId.idLen + 1U, "%s", reinterpret_cast<char*>(m_sectionToReadDescriptor.sectionId.id) );

			SaCkptIOVectorElementT readInfo;
			// Read just one section
			readInfo.sectionId = m_sectionToReadDescriptor.sectionId;
			readInfo.dataBuffer = destination;
			readInfo.dataSize = length;
			readInfo.readSize = 0U;
			readInfo.dataOffset = 0U;

			SaUint32T numberOfSectionToRead = 1U;
			SaUint32T* errorIndex = NULL;

			// Read data from the checkpoint
			SaAisErrorT readResult = saCkptCheckpointRead(m_checkpointHandle, &readInfo, numberOfSectionToRead, errorIndex);

			// read next section only after that the current one is successfully read
			m_readNext = (SA_AIS_OK == readResult);

			if(m_readNext)
			{
				length = readInfo.readSize;
				m_readRecords.push_back(std::string(sectionID));
			}
			else
			{
				// Failed to read the current section
				result = common::errorCode::ERR_CKPT_SECTION_READ_FAILURE;

				AES_DDT_LOG(LOG_LEVEL_ERROR, "Section Reading Failed Producer:<%s> DS:<%s>, checkpoint:<%s> bufferLength:<%d> sectionID:<%s> error:<%u>",
									m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(),length, sectionID, readResult);

				AES_DDT_TRACE_MESSAGE("ERROR: Section Reading Failed Producer:<%s> DS:<%s>, checkpoint:<%s> bufferLength:<%d> sectionID:<%s> error:<%u>",
									m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), length, sectionID, readResult);
			}

			return result;
		}

		int Reader::checkSectionToRead(const std::string& sectionId)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> Section:<%s>", m_producerName.c_str(), m_dataSourceName.c_str(), sectionId.c_str() );

			int result = common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE;

			if(CheckPoint::InfoSectionName.compare(sectionId) == 0U)
			{
				// reached the INFO section without found the stub section
				// the checkpoint is still opened to write, return failure and try later
				result = common::errorCode::ERR_CKPT_NOSECTIONS_TRYAGAIN;
				AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> The checkpoint is still opened to write, last record sent:<%u>", m_producerName.c_str(), m_dataSourceName.c_str(), m_deletedSections );
			}
			else
			{
				// Check if we have reached the stub section
				uint32_t messageIndex = boost::lexical_cast<uint32_t>(sectionId);
				if( m_maxNumberOfSections == messageIndex )
				{
					result = common::errorCode::ERR_CKPT_SECTIONS_EMPTY;
					AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> The checkpoint is now empty", m_producerName.c_str(), m_dataSourceName.c_str() );
				}
			}
			return result;
		}

		int Reader::checkpointPhysicalRemove()
		{
			int result = common::errorCode::ERR_NO_ERRORS;

			// Set the checkpoint name
			SaNameT checkpointName;
			memset(checkpointName.value, 0U, sizeof(checkpointName.value)/sizeof(checkpointName.value[0]));
			checkpointName.length = m_checkpointName.length();
			memcpy(checkpointName.value, m_checkpointName.c_str(), checkpointName.length);

			SaAisErrorT deleteResult = saCkptCheckpointUnlink(m_CkptServiceHandle, &checkpointName);

			if(SA_AIS_OK == deleteResult)
			{
				AES_DDT_TRACE_MESSAGE("CheckPoint:<%s> for Producer:<%s> of DS:<%s> successfully deleted",
						m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str());
				AES_DDT_LOG(LOG_LEVEL_INFO, "CheckPoint:<%s> for Producer:<%s> of DS:<%s> successfully deleted",
						m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str());
			}
			else
			{
				result = common::errorCode::ERR_CKPT_DELETE_FAILURE;
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to delete checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
						m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), deleteResult);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to delete checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
						m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), deleteResult);
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

	}
}
