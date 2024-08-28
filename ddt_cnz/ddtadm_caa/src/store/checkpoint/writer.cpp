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

#include "store/checkpoint/writer.h"
#include "common/programconstants.h"
#include "common/macros.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "store/checkpoint/state/close.h"
	#include "store/checkpoint/state/open.h"
	#include "store/checkpoint/state/stop.h"
#else
	#include "stubs/macro_stub.h"
	#include "stubs/operation_stub.h"
	#include "stubs/state_stub.h"
#endif

#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread.hpp>

#include <inttypes.h>
#include <sys/eventfd.h>


AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_Writer)
namespace store
{
	namespace checkpoint
	{

	Writer::Writer(const std::string& dataSourceName, const std::string& producerName,
			const std::string& checkpointName, const SaCkptHandleT& cktHandle, const int& readEvent)
	: StoreBase(StoreBase::WRITER, dataSourceName, producerName, checkpointName, cktHandle, readEvent),
	  m_state(boost::make_shared<state::Open>(this)),
	  m_mutex()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Writer::~Writer()
	{
		AES_DDT_TRACE_FUNCTION;
		// flush the current state into the Info Section
		updateInfoSection(m_maxNumberOfSections);
		closeCheckpoint();
	}

	int Writer::write(const void* source, const uint32_t& length)
	{
		AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, data length:<%d>",
				m_producerName.c_str(), m_dataSourceName.c_str(), length );

		boost::lock_guard<boost::mutex> guard(m_mutex);

		int result = common::errorCode::ERR_WRITER_FAILURE;

#ifdef FILE_WRITE
		m_file.write(static_cast<const char*>(source), length);
#endif

		// Check if the checkpoint is already in the write state and
		// there are sections free to be used
		if(m_state->handleStep() == common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE)
		{
			// Checkpoint ready and section available
			result = addSection(source, length);
		}
		else
		{
			// Checkpoint is not in the write state or no more free section in the current checkpoint.
			// Bring the checkpoint in the "writeable" state.
			// In case of error in a step, at next write call, the same previous failed step will be repeated again
			// The loop will ends when:
			// 		1) the write state is reached and aes_ddt::ERR_CKPT_SECTIONS_AVAILABLE is returned or
			// 		2) a step returns an error
			do
			{
				// Get the next step if the current one is not failed
				m_state->getNextStep(m_state);
				// Execute the step actions on the checkpoint
				result = m_state->handleStep();

			}while(common::errorCode::ERR_NO_ERRORS == result);

			if(common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE == result)
			{
				// Checkpoint ready and section available
				result = addSection(source, length);
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%d>", result);
		return result;
	}

	void Writer::setMessageIndex(const uint32_t& recordNumber)
	{
		AES_DDT_LOG(LOG_LEVEL_ERROR, "[<%s>@<%s>] Checkpoint:<%s> Update Message Index from <%u> to <%u>",
				m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), m_messageIndex, recordNumber);
		AES_DDT_TRACE_MESSAGE("[<%s>@<%s>] Checkpoint:<%s> Update Message Index from <%u> to <%u>",
				m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), m_messageIndex, recordNumber);
		m_messageIndex = recordNumber;
	}

	int Writer::read(void* destination, uint32_t& bufferLength)
	{
		AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s> of DS:<%s> uses a Writer to read into Checkpoint:<%s>",
				m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str());

		AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> uses a Writer to read into Checkpoint:<%s>",
				m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str());

		UNUSED(destination);
		bufferLength = 0U;
		return common::errorCode::ERR_WRITER_BAD_USAGE;
	}

	int Writer::commit(const uint32_t& numberOfRecords)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(numberOfRecords);
		return common::errorCode::ERR_CKPT_BAD_USAGE;
	}

	int Writer::getNumberOfWrittenRecord(uint32_t& recordNumber)
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		boost::lock_guard<boost::mutex> guard(m_mutex);
		// Check if the checkpoint is already in the write state
		if( m_state->handleStep() == common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE)
		{
			// Checkpoint ready and section available
			// m_messageIndex represents the next message to write,
			recordNumber = m_messageIndex;
		}
		else
		{
			// Checkpoint is not in the write state.
			// Bring the checkpoint in the "writeable" state.
			// In case of error in a step, at next call, the same previous failed step will be repeated again
			// The loop will ends when:
			// 		1) the write state is reached and aes_ddt::ERR_CKPT_SECTIONS_AVAILABLE is returned or
			// 		2) a step returns an error
			do
			{
				// Get the next step if the current one is not failed
				m_state->getNextStep(m_state);
				// Execute the step actions on the checkpoint
				result = m_state->handleStep();

			}while(common::errorCode::ERR_NO_ERRORS == result);

			if(common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE == result)
			{
				// Checkpoint ready and section available
				recordNumber = m_messageIndex;
				result = common::errorCode::ERR_NO_ERRORS;
			}
		}

		return result;
	}

	int Writer::getLastMessage(uint64_t& lastMessageNumber)
	{
		// Last message number from CP = (messageOffset) + (messageIndex in the current checkpoint)
		lastMessageNumber = m_messageOffset + m_messageIndex;

		AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, current message index:<%u> , message offset:<%lu>  and lastMessageNumber:<%lu>",
				m_producerName.c_str(), m_dataSourceName.c_str(), m_messageIndex, m_messageOffset, lastMessageNumber );

		return common::errorCode::ERR_NO_ERRORS;
	}

	int Writer::getNumberOfCommittedRecord(uint32_t& recordNumber)
	{
		AES_DDT_TRACE_FUNCTION;
		UNUSED(recordNumber);
		return common::errorCode::ERR_CKPT_BAD_USAGE;
	}

	int Writer::forceStoreChange()
	{
		AES_DDT_LOG(LOG_LEVEL_INFO, "[%s@%s] checkpoint:<%s>, m_messageOffset:<%lu> m_messageIndex:<%u>",
				m_dataSourceName.c_str(), m_producerName.c_str(),m_checkpointName.c_str(), m_messageOffset, m_messageIndex );
		AES_DDT_TRACE_MESSAGE("[%s@%s] checkpoint:<%s>, m_messageOffset:<%lu> m_messageIndex:<%u>",
				m_dataSourceName.c_str(), m_producerName.c_str(),m_checkpointName.c_str(), m_messageOffset, m_messageIndex );

		int result = common::errorCode::ERR_NO_ERRORS;
		boost::lock_guard<boost::mutex> guard(m_mutex);

		// check if some records are written, otherwise we can avoid it
		if(0U != m_messageIndex)
		{
			// the current message index is the max number of message to store
			result = updateInfoSection(m_messageIndex);

			if(common::errorCode::ERR_NO_ERRORS !=  result)
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "updateInfoSection failed!");
				AES_DDT_TRACE_MESSAGE("ERROR: updateInfoSection failed!");
				return result;
			}

			m_state = boost::make_shared<state::Close>(this);

			// Checkpoint is not in the write state.
			// Bring the checkpoint in the "writeable" state.
			// In case of error in a step, at next call, the same previous failed step will be repeated again
			// The loop will ends when:
			// 		1) the write state is reached and aes_ddt::ERR_CKPT_SECTIONS_AVAILABLE is returned or
			// 		2) a step returns an error
			do
			{
				// Execute the step actions on the checkpoint
				result = m_state->handleStep();

				// Get the next step if the current one is not failed
				m_state->getNextStep(m_state);

			}while(common::errorCode::ERR_NO_ERRORS == result);

			if(common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE == result)
			{
				// new Checkpoint ready to be used
				result = common::errorCode::ERR_NO_ERRORS;

				{
					AES_DDT_LOG(LOG_LEVEL_WARN, "[%s@%s] Close storage: wake up the reader!", m_dataSourceName.c_str(), m_producerName.c_str());
					AES_DDT_TRACE_MESSAGE("WARNING: [%s@%s] Close storage: wake up the reader!", m_dataSourceName.c_str(), m_producerName.c_str());

					eventfd_t oneRecordAdd = 1U;
					if(eventfd_write(m_readEvent, oneRecordAdd) != 0)
					{
						AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] FAILED to signal readEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_readEvent);
						AES_DDT_TRACE_MESSAGE("ERROR: [%s@%s] FAILED to signal readEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_readEvent);
					}
				}

			}
		}

		return result;
	}

	void Writer::shutdown()
	{
		AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );
		boost::lock_guard<boost::mutex> guard(m_mutex);

		// flush the current state into the Info Section
		updateInfoSection(m_maxNumberOfSections);
		closeCheckpoint();
		m_state = boost::make_shared<state::Stop>(this);
	}

	int Writer::initialize()
	{
		AES_DDT_TRACE_FUNCTION;
		return initializeStoreState();
	}

	int Writer::initializeStoreState()
	{
		AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

		int result = common::errorCode::ERR_NO_ERRORS;

		// Bring the checkpoint in the "writable" state.
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

		AES_DDT_LOG(LOG_LEVEL_ERROR, "Producer:<%s>	of DS:<%s>, Checkpoint:<%s>, Msg Offset:<%lu>, Msg Index:<%u>,result:<%u>",
				m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), m_messageOffset, m_messageIndex, stateResult );

		return result;
	}

	int Writer::getNewCheckpoint()
	{
		AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );

		int result = engine::workingSet_t::instance()->getStoringManager().getNewStoreName(m_dataSourceName, m_producerName, m_messageOffset, m_checkpointName);

		AES_DDT_TRACE_MESSAGE("Checkpoint:<%s>, result:<%d>", m_checkpointName.c_str(), result);

		return result;
	}

	int Writer::addSection(const void* bufferToWrite, const uint32_t& bufferLength)
	{
		AES_DDT_TRACE_FUNCTION;
		return addSectionToCheckPoint(bufferToWrite, bufferLength);
	}

	int Writer::addSectionToCheckPoint(const void* bufferToWrite, const uint32_t& bufferLength)
	{
		AES_DDT_TRACE_MESSAGE("Producer:<%s> DS:<%s>, checkpoint:<%s> bufferLength:<%u>",
				m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(), bufferLength);

		int result = common::errorCode::ERR_CKPT_SECTION_WRITE_FAILURE;

		std::string sectionName(boost::lexical_cast<std::string>(m_messageIndex));
		// Assemble the section id
		char sectionId[CheckPoint::MaxSectionIdSize] = {0};
		sectionName.copy(sectionId, sectionName.length());

		SaCkptSectionIdT sectionIdentity;
		sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
		sectionIdentity.idLen = sectionName.length();

		// Section attributes
		SaCkptSectionCreationAttributesT sectionCreationAttributes;
		sectionCreationAttributes.sectionId = &sectionIdentity;
		sectionCreationAttributes.expirationTime = SA_TIME_END; // never deletes the section automatically
		//sectionCreationAttributes.expirationTime = (SA_TIME_ONE_DAY +( time((time_t*)0) * 1000000000));//DO NOT USE IT: this will set the expiration time for the section

		AES_DDT_TRACE_MESSAGE("Adding section with plain text:<%s>", sectionId);

		// Create the section inside the checkpoint
		SaAisErrorT createResult = saCkptSectionCreate(m_checkpointHandle, &sectionCreationAttributes, bufferToWrite, bufferLength );

		if(SA_AIS_OK == createResult)
		{
			result = common::errorCode::ERR_NO_ERRORS;

			AES_DDT_TRACE_MESSAGE("ADDED TO CKPT Producer:<%s> DS:<%s>, checkpoint:<%s> bufferLength:<%d> section:<%s>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str(),bufferLength, sectionId);

			// set the next message index
			++m_messageIndex;

			eventfd_t oneRecordAdd = 1U;
			if(eventfd_write(m_readEvent, oneRecordAdd) != 0)
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s@%s] FAILED to signal readEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_readEvent);
				AES_DDT_TRACE_MESSAGE("ERROR: [%s@%s] FAILED to signal readEvent:<%d>", m_dataSourceName.c_str(), m_producerName.c_str(), m_readEvent);
			}

			AES_DDT_TRACE_MESSAGE("Created section<%s> into the checkpoint:<%s> for producer:<%s> of DS:<%s>",
					sectionId, m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str() );

		}
		else
		{
			// TODO adding error recovery
			// check if it is already defined
			if(SA_AIS_ERR_EXIST == createResult)
			{
				// Section ID already present in the checkpoint go to the next message index
				// set the next message index
				++m_messageIndex;

				// Section ID already present: just skip this section and return no error
				result = common::errorCode::ERR_NO_ERRORS;

				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Section<%s> already exists into the checkpoint:<%s> for producer:<%s> of DS:<%s>, error:<%d>",
						sectionId, m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), createResult);
				AES_DDT_TRACE_MESSAGE("ERROR Section<%s> already exists into the checkpoint:<%s> for producer:<%s> of DS:<%s>, error:<%d>",
						sectionId, m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), createResult);
			}
			else
			{
				//even f the timeout expires, the section might have been written
				//in that case return an error anyway because DDT will recognize the already stored section at next write and will skip it.

				result = common::errorCode::ERR_CKPT_SECTION_WRITE_FAILURE;
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to create section<%s> into the checkpoint:<%s> for producer:<%s> of DS:<%s>, error:<%d>",
						sectionId, m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), createResult);
				AES_DDT_TRACE_MESSAGE("ERROR Failed to create section<%s> into the checkpoint:<%s> for producer:<%s> of DS:<%s>, error:<%d>",
						sectionId, m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), createResult);
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%d>", result);

		return result;
	}

	} /* namespace ddt_ckpt */
}
