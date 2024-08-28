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

#include "store/checkpoint/storebase.h"
#include "common/programconstants.h"
#include "common/utility.h"
#include "common/configuration.h"
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include <boost/tokenizer.hpp>
#else
	#include "stubs/workingset_stub.h"
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_StoreBase)

namespace store
{
	namespace checkpoint
	{

		StoreBase::StoreBase(const storeHandlerType_t& storeType, const std::string& dataSourceName, const std::string& producerName,
									   const std::string& checkpointName, const SaCkptHandleT& cktHandle, const int& readEvent)
		 : HandlerInterface(),
		   m_type(storeType),
		   m_dataSourceName(dataSourceName),
		   m_producerName(producerName),
		   m_checkpointName(checkpointName),
		   m_CkptServiceHandle(cktHandle),
		   m_checkpointHandle(CheckPoint::INVALID_HANDLE),
		   m_messageOffset(0ULL),
		   m_messageIndex(0U),
		   m_deletedSections(0U),
		   m_recordSize(0U),
		   m_maxNumberOfSections(0U),
		   m_readEvent(readEvent)
#ifdef FILE_WRITE
		   ,m_file()
#endif
		{
			AES_DDT_TRACE_FUNCTION;

			m_recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(m_dataSourceName);

			m_maxNumberOfSections = engine::workingSet_t::instance()->getStoringManager().getMaxNumberOfRecordsPerStore(m_recordSize);

		}

		StoreBase::~StoreBase()
		{
			closeCheckpoint();
		}

		int StoreBase::forceStoreChange()
		{
			AES_DDT_TRACE_FUNCTION;
			return common::errorCode::ERR_CKPT_BAD_USAGE;
		}

		int StoreBase::openCheckpoint()
		{
			AES_DDT_TRACE_FUNCTION;

			int result = common::errorCode::ERR_NO_ERRORS;

			// check if ckpt is already opened
			if(CheckPoint::INVALID_HANDLE == m_checkpointHandle)
			{
				SaAisErrorT openResult;

				// Set the checkpoint name
				SaNameT checkpointName;
				memset(checkpointName.value, 0U, sizeof(checkpointName.value)/sizeof(checkpointName.value[0]));
				checkpointName.length = m_checkpointName.length();
				memcpy(checkpointName.value, m_checkpointName.c_str(), checkpointName.length);

				if(isWriter())
				{
#ifdef FILE_WRITE
					std::string filePath("/data/opt/ap/internal_root/tools/");
					filePath.append(m_checkpointName);
					filePath.append("_W");
					m_file.open(filePath.c_str(), std::fstream::out | std::fstream::app | std::fstream::binary );
#endif

					// Writer Open
					// get max section size
					uint32_t recordSize = engine::workingSet_t::instance()->getDataSourceManager().getDataSourceRecordSize(m_dataSourceName);

					// Checkpoint create attributes structure
					SaCkptCheckpointCreationAttributesT ckptCreateAttr;

					// All replicas must be updated before any operation returns
					ckptCreateAttr.creationFlags = SA_CKPT_WR_ALL_REPLICAS;

					// Add an extra sections for info part and stub section
					ckptCreateAttr.maxSections = m_maxNumberOfSections + 2U;

					// The upper bound on the possible size of the sections in the checkpoint.
					ckptCreateAttr.maxSectionSize = m_recordSize;

					//The maximum length of the section identifier in the checkpoint.
					ckptCreateAttr.maxSectionIdSize = CheckPoint::MaxSectionIdSize;

					// The net size in bytes of each checkpoint replica that can be used for application data.
					ckptCreateAttr.checkpointSize = recordSize * ckptCreateAttr.maxSections;

					// The duration (nanosec) for which the checkpoint will be retained while it is not opened by any process.
					ckptCreateAttr.retentionDuration = SA_TIME_END;

					// Open mode
					SaCkptCheckpointOpenFlagsT ckptOpenFlags = SA_CKPT_CHECKPOINT_CREATE | SA_CKPT_CHECKPOINT_READ | SA_CKPT_CHECKPOINT_WRITE;

					// Open a checkpoint like as file open
					openResult = saCkptCheckpointOpen(m_CkptServiceHandle, &checkpointName, &ckptCreateAttr, ckptOpenFlags, CheckPoint::InvocationTimeOutInNanoSec, &m_checkpointHandle);
				}
				else
				{
#ifdef FILE_WRITE
					std::string filePath("/data/opt/ap/internal_root/tools/");
					filePath.append(m_checkpointName);
					filePath.append("_R");
					m_file.open(filePath.c_str(), std::fstream::out | std::fstream::app | std::fstream::binary );
#endif
					// Reader Open
					// Checkpoint create attributes structure is NULL since the reader does not create the checkpoint
					SaCkptCheckpointCreationAttributesT* ckptCreateAttr = NULL;

					// Open mode
					SaCkptCheckpointOpenFlagsT ckptOpenFlags = SA_CKPT_CHECKPOINT_READ | SA_CKPT_CHECKPOINT_WRITE;

					// Open a checkpoint like as file open
					openResult = saCkptCheckpointOpen(m_CkptServiceHandle, &checkpointName, ckptCreateAttr, ckptOpenFlags, CheckPoint::InvocationTimeOutInNanoSec, &m_checkpointHandle);
				}

				if(SA_AIS_OK == openResult)
				{
					AES_DDT_TRACE_MESSAGE("CheckPoint<%s> for Producer:<%s> of DS:<%s> successfully opened",
							checkpointName.value, m_producerName.c_str(), m_dataSourceName.c_str());
					AES_DDT_LOG(LOG_LEVEL_INFO, "CheckPoint<%s> for Producer:<%s> of DS:<%s> successfully opened",
							checkpointName.value, m_producerName.c_str(), m_dataSourceName.c_str());
				}
				else
				{
					result = common::errorCode::ERR_CKPT_OPEN_FAILURE;
					m_checkpointHandle = CheckPoint::INVALID_HANDLE;
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to open checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
							checkpointName.value, m_producerName.c_str(), m_dataSourceName.c_str(), openResult);
					AES_DDT_TRACE_MESSAGE("[ERROR] Failed to open checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
							checkpointName.value, m_producerName.c_str(), m_dataSourceName.c_str(), openResult);
				}
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int StoreBase::closeCheckpoint()
		{
			AES_DDT_TRACE_FUNCTION;
			int result = common::errorCode::ERR_NO_ERRORS;

			// check if ckpt is already closed
			if(CheckPoint::INVALID_HANDLE != m_checkpointHandle)
			{
#ifdef FILE_WRITE
				m_file.close();
#endif
				SaAisErrorT closeResult = saCkptCheckpointClose(m_checkpointHandle);

				if(SA_AIS_OK == closeResult)
				{
					m_checkpointHandle = CheckPoint::INVALID_HANDLE;
					AES_DDT_TRACE_MESSAGE("CheckPoint:<%s> for Producer:<%s> of DS:<%s> successfully closed",
							m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str());
					AES_DDT_LOG(LOG_LEVEL_INFO, "CheckPoint:<%s> for Producer:<%s> of DS:<%s> successfully closed",
							m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str());
				}
				else
				{
					result = common::errorCode::ERR_CKPT_CLOSE_FAILURE;

					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to close checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
							m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), closeResult);
					AES_DDT_TRACE_MESSAGE("ERROR: Failed to close checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
							m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), closeResult);
				}
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int StoreBase::checkpointDelete()
		{
			AES_DDT_TRACE_FUNCTION;
			return common::errorCode::ERR_CKPT_DELETE_FAILURE;
		}

		int StoreBase::readInfoSection()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, checkpoint:<%s>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str() );
			int result = common::errorCode::ERR_NO_ERRORS;

			// Section identifier descriptor
			char sectionId[CheckPoint::MaxSectionIdSize + 1] = {0};
			strcpy(sectionId, CheckPoint::InfoSectionName.c_str());

			SaCkptSectionIdT sectionIdentity;
			sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
			sectionIdentity.idLen = CheckPoint::InfoSectionName.length();

			// Allocate the buffer for data to read
			char readBuffer[CheckPoint::MaxInfoSectionSize] = {0};

			SaCkptIOVectorElementT readData;

			// Read just one section
			readData.sectionId.id = reinterpret_cast<unsigned char*>(sectionId);
			readData.sectionId.idLen = CheckPoint::InfoSectionName.length();
			readData.dataBuffer = readBuffer;
			readData.dataSize = CheckPoint::MaxInfoSectionSize;
			readData.dataOffset = 0U;

			SaUint32T errorIdx;
			SaUint32T numberOfElements = 1U;

			// Read data from the checkpoint
			SaAisErrorT readResult = saCkptCheckpointRead(m_checkpointHandle, &readData, numberOfElements, &errorIdx);

			if(SA_AIS_OK == readResult)
			{
				// Get Checkpoint info from the buffer
				result = convertInfoBufferToData(readBuffer);

				// Reset number of deleted sections
				m_deletedSections = 0U;

				AES_DDT_TRACE_MESSAGE("messageOffset:<%zu>, messageIndex:<%d>", m_messageOffset, m_messageIndex);
				AES_DDT_LOG(LOG_LEVEL_INFO, "CheckPoint:<%s> has messageOffset:<%zu> and messageIndex:<%d>",
										m_checkpointName.c_str(), m_messageOffset, m_messageIndex );

				printf("messageOffset:<%zu>, messageIndex:<%d>\n", m_messageOffset, m_messageIndex);
			}
			else if( (SA_AIS_ERR_NOT_EXIST == readResult) && isWriter() )
			{
				// INFO section is not present: add it with current value for msgOffset and msgIndex
				AES_DDT_LOG(LOG_LEVEL_INFO, "WRITER is creating the INFO section for CheckPoint:<%s>", m_checkpointName.c_str());
				result = createInfoSection();
			}
			else
			{
				result = common::errorCode::ERR_CKPT_INFOSECTION_READ_FAILURE;

				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to read info section of checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
						m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), readResult);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to read info section of checkpoint:<%s> for Producer:<%s> of DS:<%s>, error:<%d>",
						m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), readResult);
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int StoreBase::convertInfoBufferToData(const char* infoBuffer)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, Info Buffer:<%s>",
							m_producerName.c_str(), m_dataSourceName.c_str(), infoBuffer );
			bool result = false;

			// the buffer format is: <msgOffset>_<msgIdx>, e.g: "24000_235"
			std::string ckpInfoData(infoBuffer);

			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> dataSeparator(CheckPoint::InfoSeparator);

			tokenizer tokens(ckpInfoData, dataSeparator);
			tokenizer::iterator tok_iter = tokens.begin();

			// get the first token
			if(tokens.end() != tok_iter)
			{
				result = true;
				m_messageOffset = boost::lexical_cast<uint64_t>(*tok_iter);
				AES_DDT_TRACE_MESSAGE("First Token:<%s>, messageOffset:<%zu>", tok_iter->c_str(), m_messageOffset);
				AES_DDT_LOG(LOG_LEVEL_INFO, "First Token:<%s>, messageOffset:<%zu>", tok_iter->c_str(), m_messageOffset);
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("First Token is not valid. Info Buffer:<%s>", infoBuffer);
				AES_DDT_LOG(LOG_LEVEL_INFO, "First Token is not valid. Info Buffer:<%s>", infoBuffer);
			}

			++tok_iter;

			// get the second token
			if(tokens.end() != tok_iter)
			{
				result &= true;
				m_messageIndex = boost::lexical_cast<uint32_t>(*tok_iter);
				AES_DDT_TRACE_MESSAGE("Second Token:<%s>, messageIndex:<%d>", tok_iter->c_str(), m_messageIndex);
				AES_DDT_LOG(LOG_LEVEL_INFO, "Second Token:<%s>, messageIndex:<%d>", tok_iter->c_str(), m_messageIndex);
			}
			else
			{
				result = false;
				AES_DDT_TRACE_MESSAGE("Second Token is not valid. Info Buffer:<%s>", infoBuffer);
				AES_DDT_LOG(LOG_LEVEL_INFO, "Second Token is not valid. Info Buffer:<%s>", infoBuffer);
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return (result ? common::errorCode::ERR_NO_ERRORS : common::errorCode::ERR_CKPT_INFOSECTION_CORRUPTED);
		}

		int StoreBase::createInfoSection()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", m_producerName.c_str(), m_dataSourceName.c_str() );
			int result = common::errorCode::ERR_NO_ERRORS;

			// Assemble the info data as: "<msgOffset>_<msgIdx>"
			std::string infoData(boost::lexical_cast<std::string>(m_messageOffset));
			infoData.append(CheckPoint::InfoSeparator);
			infoData.append(boost::lexical_cast<std::string>(m_messageIndex));

			AES_DDT_TRACE_MESSAGE("Create Info Section with:<%s>", infoData.c_str() );

			// Assemble the section id
			char sectionId[CheckPoint::MaxSectionIdSize] = {0};
			strncpy(sectionId, CheckPoint::InfoSectionName.c_str(), sizeof(sectionId));

			SaCkptSectionIdT sectionIdentity;
			sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
			sectionIdentity.idLen = CheckPoint::InfoSectionName.length();

			// Section attributes
			SaCkptSectionCreationAttributesT sectionCreationAttributes;
			sectionCreationAttributes.sectionId = &sectionIdentity;
			sectionCreationAttributes.expirationTime = SA_TIME_END; // never deletes the section automatically
			//sectionCreationAttributes.expirationTime = (SA_TIME_ONE_DAY +( time((time_t*)0) * 1000000000));

			// Assemble the buffer to write into the section with the info data
			char bufferToWrite[CheckPoint::MaxInfoSectionSize] = {0};
			strncpy(bufferToWrite, infoData.c_str(), CheckPoint::MaxInfoSectionSize);
			uint64_t bufferLength = infoData.length();

			// Create the info section inside the checkpoint
			SaAisErrorT ckptResult = saCkptSectionCreate(m_checkpointHandle, &sectionCreationAttributes, bufferToWrite, bufferLength);

			if(SA_AIS_OK == ckptResult)
			{
				AES_DDT_TRACE_MESSAGE("Created info section with<%s> into the checkpoint:<%s> of DS:<%s>",
						bufferToWrite, m_checkpointName.c_str(), m_dataSourceName.c_str() );
				AES_DDT_LOG(LOG_LEVEL_INFO, "Created info section with<%s> into the checkpoint:<%s> of DS:<%s>",
						bufferToWrite, m_checkpointName.c_str(), m_dataSourceName.c_str() );
			}
			else
			{
				result = common::errorCode::ERR_CKPT_INFOSECTION_CREATE_FAILURE;
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to create info section with<%s> into the checkpoint:<%s> of DS:<%s>, error:<%d>",
						bufferToWrite, m_checkpointName.c_str(), m_dataSourceName.c_str(), ckptResult);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to create producer section:<%s> into status checkpoint of DS:<%s>, error:<%d>",
						sectionId, m_dataSourceName.c_str(), ckptResult);
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int StoreBase::updateInfoSection(uint32_t maxRecordToStore)
		{
			AES_DDT_TRACE_MESSAGE("Checkpoint:<%s> for DS:<%s>", m_checkpointName.c_str(), m_dataSourceName.c_str() );
			int result = common::errorCode::ERR_NO_ERRORS;

			if(CheckPoint::INVALID_HANDLE != m_checkpointHandle)
			{
				// Assemble the info data as: "<msgOffset>_<msgIdx>"
				std::string infoData(boost::lexical_cast<std::string>(m_messageOffset));
				infoData.append(CheckPoint::InfoSeparator);
				infoData.append(boost::lexical_cast<std::string>(m_messageIndex));

				// Assemble the section id
				char sectionId[CheckPoint::MaxSectionIdSize] = {0};
				strncpy(sectionId, CheckPoint::InfoSectionName.c_str(), sizeof(sectionId));

				SaCkptSectionIdT sectionIdentity;
				sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
				sectionIdentity.idLen = CheckPoint::InfoSectionName.length();

				// Assemble the buffer to write into the section with the info data
				char bufferToWrite[CheckPoint::MaxInfoSectionSize] = {0};
				strcpy(bufferToWrite, infoData.c_str());
				uint64_t bufferLength = infoData.length();

				// Update the section inside the status checkpoint
				SaAisErrorT ckptResult = saCkptSectionOverwrite(m_checkpointHandle, &sectionIdentity, bufferToWrite, bufferLength);

				if(SA_AIS_OK == ckptResult)
				{
					AES_DDT_TRACE_MESSAGE("Updated info section with<%s> into the checkpoint:<%s> of DS:<%s>",
							bufferToWrite, m_checkpointName.c_str(), m_dataSourceName.c_str() );
					AES_DDT_LOG(LOG_LEVEL_INFO, "Updated info section with<%s> into the checkpoint:<%s> of DS:<%s>",
							bufferToWrite, m_checkpointName.c_str(), m_dataSourceName.c_str() );

					// Check if checkpoint is full, so
					// 	Adding a stub section to indicate checkpoint full
					// 	and update the index and the offset
					if( (maxRecordToStore <= m_messageIndex) &&
							( (result = addStubSection()) == common::errorCode::ERR_NO_ERRORS) )
					{
						m_messageIndex = 0U;
						m_messageOffset += maxRecordToStore;

						AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> of DS:<%s> will use : Message Index:<%d> MessageOffset:<%zu>",
								m_producerName.c_str(), m_dataSourceName.c_str(), m_messageIndex, m_messageOffset );
					}
				}
				else
				{
					result = common::errorCode::ERR_CKPT_INFOSECTION_WRITE_FAILURE;
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to update info section with<%s> into the checkpoint:<%s> of DS:<%s>, error:<%d>",
							bufferToWrite, m_checkpointName.c_str(), m_dataSourceName.c_str(), ckptResult);
					AES_DDT_TRACE_MESSAGE("ERROR: Failed to update info section with<%s> into the checkpoint:<%s> of DS:<%s>, error:<%d>",
							bufferToWrite, m_checkpointName.c_str(), m_dataSourceName.c_str(), ckptResult);
				}
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("Failed to updated info section since the checkpoint is not opened, producer:<%s> of DS:<%s>",
						m_producerName.c_str(), m_dataSourceName.c_str() );
				AES_DDT_LOG(LOG_LEVEL_WARN, "Failed to updated info section since the checkpoint is not opened, producer:<%s> of DS:<%s>",
						m_producerName.c_str(), m_dataSourceName.c_str() );
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;
		}

		int StoreBase::updateInfoSection()
		{
			return updateInfoSection(m_maxNumberOfSections);
		}

		int StoreBase::isCheckPointFull() const
		{
			int result = common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE;

			if(m_maxNumberOfSections <= m_messageIndex)
			{
				result = common::errorCode::ERR_CKPT_SECTIONS_FULL;
				AES_DDT_TRACE_MESSAGE("Checkpoint:<%s> of DS:<%s> is FULL, sections:<%d>, offset:<%zu>",
						m_checkpointName.c_str(), m_dataSourceName.c_str(), m_messageIndex, m_messageOffset );
				AES_DDT_LOG(LOG_LEVEL_INFO, "Checkpoint:<%s> of DS:<%s> is FULL, sections:<%d>, offset:<%zu>",
						m_checkpointName.c_str(), m_dataSourceName.c_str(), m_messageIndex, m_messageOffset );
			}

			return result;
		}

		int StoreBase::checkpointIteratorInit()
		{
			int result = common::errorCode::ERR_CKPT_ITERATOR_INIT_FAILURE;
			AES_DDT_TRACE_MESSAGE("ERROR: Checkpoint:<%s> of DS:<%s> iterator cannot be initialize", m_checkpointName.c_str(), m_dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Checkpoint:<%s> of DS:<%s> iterator cannot be initialize", m_checkpointName.c_str(), m_dataSourceName.c_str());
			return result;
		}

		int StoreBase::checkpointIteratorFinalize()
		{
			AES_DDT_TRACE_MESSAGE("ERROR: Checkpoint:<%s> of DS:<%s> iterator cannot be finalized", m_checkpointName.c_str(), m_dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Checkpoint:<%s> of DS:<%s> iterator cannot be finalized", m_checkpointName.c_str(), m_dataSourceName.c_str());
			return common::errorCode::ERR_CKPT_ITERATOR_FIN_FAILURE;
		}

		int StoreBase::getNextSectionToRead()
		{
			return common::errorCode::ERR_CKPT_BAD_USAGE;
		}

		int StoreBase::checkForCommit()
		{
			return common::errorCode::ERR_NO_ERRORS;
		}

		int StoreBase::addStubSection()
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>, checkpoint:<%s>",
					m_producerName.c_str(), m_dataSourceName.c_str(), m_checkpointName.c_str() );

			int result = common::errorCode::ERR_NO_ERRORS;

			std::string sectionName(boost::lexical_cast<std::string>(m_maxNumberOfSections));

			// Assemble the section id
			char sectionId[CheckPoint::MaxSectionIdSize] = {0};
			sectionName.copy(sectionId, sectionName.length());

			AES_DDT_TRACE_MESSAGE("Adding Stub Section:<%s>", sectionId);

			SaCkptSectionIdT sectionIdentity;
			sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
			sectionIdentity.idLen = sectionName.length();

			// Section attributes
			SaCkptSectionCreationAttributesT sectionCreationAttributes;
			sectionCreationAttributes.sectionId = &sectionIdentity;
			sectionCreationAttributes.expirationTime = SA_TIME_END; // never deletes the section automatically

			char bufferToWrite[] = "STUB SECTION";
			uint32_t bufferLength = sizeof(bufferToWrite)/sizeof(bufferToWrite[0]);

			// Create the section inside the checkpoint
			SaAisErrorT createResult = saCkptSectionCreate(m_checkpointHandle, &sectionCreationAttributes, bufferToWrite, bufferLength);

			if( (SA_AIS_OK != createResult) && (SA_AIS_ERR_EXIST != createResult) )
			{
				result = common::errorCode::ERR_CKPT_INFOSECTION_WRITE_FAILURE;
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to create STUB section<%s> into the checkpoint:<%s> for producer:<%s> of DS:<%s>, error:<%d>",
						sectionId, m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), createResult);
				AES_DDT_TRACE_MESSAGE("Failed to create STUB section<%s> into the checkpoint:<%s> for producer:<%s> of DS:<%s>, error:<%d>",
						sectionId, m_checkpointName.c_str(), m_producerName.c_str(), m_dataSourceName.c_str(), createResult);
			}

			return result;
		}

	} /* namespace ddt_ckpt */
}
