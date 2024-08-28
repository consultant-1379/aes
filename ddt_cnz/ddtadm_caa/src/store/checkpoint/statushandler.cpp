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

#include "store/checkpoint/statushandler.h"
#include "common/utility.h"
#include "store/checkpoint/producerinfo.h"
#include "iostream"
#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "store/checkpoint/checkpoint.h"
	#include <boost/make_shared.hpp>
#else
	#include "stubs/macro_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_StateHandler)
namespace store
{
	namespace checkpoint
	{

		StatusHandler::StatusHandler(const std::string& dataSourceName, const SaCkptHandleT& cktHandle)
		: m_dataSourceName(dataSourceName),
		  m_producers(),
		  m_CkptServiceHandle(cktHandle),
		  m_checkpointHandle(CheckPoint::INVALID_HANDLE)
		{
			AES_DDT_TRACE_FUNCTION;
		}

		StatusHandler::~StatusHandler()
		{
			AES_DDT_TRACE_FUNCTION;
			m_producers.clear();
			closeStatusCheckpoint();
		}

		bool StatusHandler::load()
		{
			AES_DDT_TRACE_MESSAGE("DS:<%s>", m_dataSourceName.c_str());
			bool result = false;

			if(openStatusCheckpoint())
			{
				// reload all stored data
				result = readStatusCheckpointSections();
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		bool StatusHandler::addStoreName(const std::string& producerName, const std::string& storeName)
		{
			AES_DDT_TRACE_MESSAGE("Adding checkpoint:<%s> to the producer:<%s> of DS:<%s>", storeName.c_str(), producerName.c_str(), m_dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_INFO, "Adding checkpoint:<%s> to the producer:<%s> of DS:<%s>", storeName.c_str(), producerName.c_str(), m_dataSourceName.c_str());
			bool result = false;

			mapOfProducer_t::const_iterator element = m_producers.find(producerName);

			if(m_producers.end() != element)
			{
				// element found update it
				element->second->addCheckPoint(storeName);
				result = updateProducerSectionInfo(element->second);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "DS:<%s> adding producer:<%s> to the status handler", m_dataSourceName.c_str(), producerName.c_str());

				// create the producer checkpoints info handler
				boost::shared_ptr<ProducerInfo> producerCkpInfo = boost::make_shared<ProducerInfo>(producerName);

				// adding the new checkpoint name
				producerCkpInfo->addCheckPoint(storeName);

				// create the producer section into the status checkpoint
				result = createProducerSectionInfo(producerCkpInfo);

				// Insert into the internal map the checkpoints info handler for this producer
				m_producers.insert(std::make_pair(producerName, producerCkpInfo));
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		bool StatusHandler::removeStoreName(const std::string& storeName, const std::string& producerName)
		{
			AES_DDT_TRACE_MESSAGE("Removing checkpoint:<%s> to the producer:<%s> of DS:<%s>", storeName.c_str(), producerName.c_str(), m_dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_INFO, "Removing checkpoint:<%s> to the producer:<%s> of DS:<%s>", storeName.c_str(), producerName.c_str(), m_dataSourceName.c_str());
			bool result = false;

			mapOfProducer_t::const_iterator element = m_producers.find(producerName);

			// keep at least one checkpoint into the list
			if((m_producers.end() != element) && (element->second->getNumberOfCheckpoint() > 1U) )
			{
				// element found update it
				element->second->removeCheckPoint(storeName);
				result = updateProducerSectionInfo(element->second);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to remove checkpoint:<%s> from the producer<%s> of DS:<%s>",
						storeName.c_str(), producerName.c_str(), m_dataSourceName.c_str());

				AES_DDT_TRACE_MESSAGE("ERROR: Failed to remove checkpoint:<%s> from the producer<%s> of DS:<%s>",
						storeName.c_str(), producerName.c_str(), m_dataSourceName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		bool StatusHandler::getLastStoreName(const std::string& producerName, std::string& storeName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), m_dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> of DS:<%s>", producerName.c_str(), m_dataSourceName.c_str());
			bool result = false;
			storeName.clear();

			mapOfProducer_t::const_iterator element = m_producers.find(producerName);

			if(m_producers.end() != element)
			{
				// element found get the info
				result = element->second->getLastCheckPoint(storeName);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_WARN, "Producer<%s> for DS:<%s> not found", producerName.c_str(), m_dataSourceName.c_str());
				AES_DDT_TRACE_MESSAGE("WARNING: Producer<%s> for DS:<%s> not found", producerName.c_str(), m_dataSourceName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		bool StatusHandler::getFirstStoreName(const std::string& producerName, std::string& storeName)
		{
			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s>", producerName.c_str(), m_dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> of DS:<%s>", producerName.c_str(), m_dataSourceName.c_str());
			bool result = false;
			storeName.clear();

			mapOfProducer_t::const_iterator element = m_producers.find(producerName);

			if(m_producers.end() != element)
			{
				// element found get the info
				result = element->second->getFirstCheckPoint(storeName);
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_WARN, "Producer<%s> for DS:<%s> not found", producerName.c_str(), m_dataSourceName.c_str());
				AES_DDT_TRACE_MESSAGE("WARNING: Producer<%s> for DS:<%s> not found", producerName.c_str(), m_dataSourceName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		size_t StatusHandler::getNumberOfStores(const std::string& producerName) const
		{
			AES_DDT_TRACE_FUNCTION;
			size_t numberOfCheckpoint = 0U;

			mapOfProducer_t::const_iterator element = m_producers.find(producerName);

			if( m_producers.end() != element )
			{
				numberOfCheckpoint = element->second->getNumberOfCheckpoint();
			}

			AES_DDT_TRACE_MESSAGE("Producer:<%s> of DS:<%s> has <%lu> checkpoints defined", producerName.c_str(), m_dataSourceName.c_str(), numberOfCheckpoint);
			AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> of DS:<%s> has <%lu> checkpoints defined", producerName.c_str(), m_dataSourceName.c_str(), numberOfCheckpoint);

			return numberOfCheckpoint;
		}

		size_t StatusHandler::getNumberOfStores() const
		{
			AES_DDT_TRACE_FUNCTION;
			size_t numberOfCheckpoint = 0U;

			mapOfProducer_t::const_iterator element = m_producers.begin();

			while( m_producers.end() != element )
			{
				numberOfCheckpoint += element->second->getNumberOfCheckpoint();
				++element;
			}

			AES_DDT_TRACE_MESSAGE(" DS:<%s> has <%lu> checkpoints defined", m_dataSourceName.c_str(), numberOfCheckpoint);
			AES_DDT_LOG(LOG_LEVEL_INFO, "DS:<%s> has <%lu> checkpoints defined", m_dataSourceName.c_str(), numberOfCheckpoint);

			return numberOfCheckpoint;
		}

		void StatusHandler::getProducers(std::list<std::string>& producerList) const
		{
			AES_DDT_TRACE_FUNCTION;
			producerList.clear();

			mapOfProducer_t::const_iterator element = m_producers.begin();

			while( m_producers.end() != element )
			{
				producerList.push_back(element->first);
				++element;
			}

			AES_DDT_TRACE_MESSAGE("Found <%zu> Producers for the DS:<%s>", producerList.size(), m_dataSourceName.c_str());
			AES_DDT_LOG(LOG_LEVEL_INFO, "Found <%zu> Producers for the DS:<%s>", producerList.size(), m_dataSourceName.c_str());

		}

		bool StatusHandler::clean()
		{
			AES_DDT_TRACE_FUNCTION;
			closeStatusCheckpoint();
			return deleteStatusCheckpoint();;
		}

		bool StatusHandler::openStatusCheckpoint()
		{
			AES_DDT_TRACE_FUNCTION;

			bool result = true;

			if(CheckPoint::INVALID_HANDLE == m_checkpointHandle)
			{
				std::string statusCktpName(CheckPoint::NamePrefix);
				statusCktpName.append(m_dataSourceName);

				// Set status checkpoint name
				SaNameT checkpointName;
				memset(checkpointName.value, 0U, sizeof(checkpointName.value)/sizeof(checkpointName.value[0]));
				checkpointName.length = statusCktpName.length();
				memcpy(checkpointName.value, statusCktpName.c_str(), checkpointName.length);

				// Checkpoint attributes
				SaCkptCheckpointCreationAttributesT ckptCreateAttr;

				ckptCreateAttr.creationFlags = SA_CKPT_WR_ALL_REPLICAS; // All replicas must be updated before any operation returns
				ckptCreateAttr.checkpointSize = StatusCheckPoint::MaxSectionSize * StatusCheckPoint::MaxSections;
				ckptCreateAttr.maxSections = StatusCheckPoint::MaxSections;
				ckptCreateAttr.maxSectionSize = StatusCheckPoint::MaxSectionSize;
				ckptCreateAttr.maxSectionIdSize = StatusCheckPoint::MaxSectionIdSize;

				// The duration (nanosec) for which the checkpoint will be retained while it is not opened by any process.
				ckptCreateAttr.retentionDuration = SA_TIME_END;

				// Open mode
				SaCkptCheckpointOpenFlagsT ckptOpenFlags = SA_CKPT_CHECKPOINT_CREATE | SA_CKPT_CHECKPOINT_READ | SA_CKPT_CHECKPOINT_WRITE;

				// Open a checkpoint like as file open
				SaAisErrorT openResult = saCkptCheckpointOpen(m_CkptServiceHandle, &checkpointName, &ckptCreateAttr,
															  ckptOpenFlags, CheckPoint::InvocationTimeOutInNanoSec, &m_checkpointHandle);

				if(SA_AIS_OK == openResult)
				{
					AES_DDT_TRACE_MESSAGE("Status CheckPoint<%s> of DS:<%s> successfully opened", checkpointName.value, m_dataSourceName.c_str());
					AES_DDT_LOG(LOG_LEVEL_INFO, "Status CheckPoint<%s> of DS:<%s> successfully opened", checkpointName.value, m_dataSourceName.c_str());
				}
				else
				{
					m_checkpointHandle = CheckPoint::INVALID_HANDLE;
					result = false;
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Failed to open status checkpoint:<%s> of DS:<%s>, error:<%d>", checkpointName.value, m_dataSourceName.c_str(), openResult);
					AES_DDT_TRACE_MESSAGE("[ERROR] Failed to open status checkpoint:<%s> of DS:<%s>, error:<%d>", checkpointName.value, m_dataSourceName.c_str(), openResult);
				}
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		bool StatusHandler::readStatusCheckpointSections()
		{
			AES_DDT_TRACE_FUNCTION;
			bool readResult = false;

			SaCkptSectionIterationHandleT sectionIterator;

			// Initialize iterator on sections of status checkpoint
			SaAisErrorT result = saCkptSectionIterationInitialize( m_checkpointHandle,
																	CheckPoint::SectionSelectCriteria,
																	CheckPoint::SectionExpirationTime,
																	&sectionIterator
																	);

			if(SA_AIS_OK == result)
			{
				AES_DDT_TRACE_MESSAGE("Iterator on Status checkpoint of DS:<%s> initialized", m_dataSourceName.c_str() );
				AES_DDT_LOG(LOG_LEVEL_INFO, "Iterator on Status checkpoint of DS:<%s> initialized", m_dataSourceName.c_str() );
				readResult = true;

				SaCkptSectionDescriptorT sectionDescriptor;

				// Stepping through the sections of the checkpoint
				while( saCkptSectionIterationNext(sectionIterator, &sectionDescriptor) == SA_AIS_OK)
				{
					char tmpName[CheckPoint::MaxSectionIdSize + 1U] = {0};
					snprintf(tmpName, sectionDescriptor.sectionId.idLen + 1U, "%s", reinterpret_cast<char*>(sectionDescriptor.sectionId.id) );

					std::string sectionName(tmpName);

					AES_DDT_TRACE_MESSAGE("Found Producer:<%s>, name length:<%u>", sectionName.c_str(), sectionDescriptor.sectionId.idLen );
					AES_DDT_LOG(LOG_LEVEL_INFO, "Found Producer:<%s>, name length:<%u>", sectionName.c_str(), sectionDescriptor.sectionId.idLen );

					// Allocate the buffer for data to read
					char readBuffer[StatusCheckPoint::MaxSectionSize] = {0};

					SaCkptIOVectorElementT readVector;

					// Read just one section
					readVector.sectionId = sectionDescriptor.sectionId;
					readVector.dataBuffer = readBuffer;
					readVector.dataSize = StatusCheckPoint::MaxSectionSize;
					readVector.dataOffset = 0U;

					SaUint32T errorVectorIdx;
					SaUint32T numberOfElements = 1U;

					// Read data from the section
					result = saCkptCheckpointRead(m_checkpointHandle, &readVector, numberOfElements, &errorVectorIdx);

					if(SA_AIS_OK == result)
					{
						m_producers[sectionName] = boost::make_shared<ProducerInfo>(sectionName);
						m_producers[sectionName]->bufferToObject(readBuffer);
					}
					else
					{
						readResult = false;
						AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "ERROR: Failed to read section:<%s> of status checkpoint of DS:<%s>, error:<%d>",
								sectionName.c_str(), m_dataSourceName.c_str(), result);
						AES_DDT_TRACE_MESSAGE("ERROR: Failed to read section:<%s> of status checkpoint of DS:<%s>, error:<%d>",
								sectionName.c_str(), m_dataSourceName.c_str(), result);
					}
				}

				AES_DDT_TRACE_MESSAGE("Found <%zu> producers into the status checkpoint of DS:<%s>", m_producers.size(), m_dataSourceName.c_str() );
				AES_DDT_LOG(LOG_LEVEL_INFO, "Found <%zu> producers into the status checkpoint of DS:<%s>", m_producers.size(), m_dataSourceName.c_str() );

				// Finalize iterator
				saCkptSectionIterationFinalize(sectionIterator);
			}
			else
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "ERROR: Failed to iterate on status checkpoint of DS:<%s>, error:<%d>", m_dataSourceName.c_str(), result);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to iterate on status checkpoint of DS:<%s>, error:<%d>", m_dataSourceName.c_str(), result);
			}

			return readResult;
		}

		void StatusHandler::closeStatusCheckpoint()
		{
			AES_DDT_TRACE_FUNCTION;

			if(CheckPoint::INVALID_HANDLE != m_checkpointHandle)
			{
				SaAisErrorT result = saCkptCheckpointClose(m_checkpointHandle);

				if(SA_AIS_OK == result)
				{
					m_checkpointHandle = CheckPoint::INVALID_HANDLE;
					AES_DDT_TRACE_MESSAGE("Status CheckPoint of DS:<%s> successfully closed", m_dataSourceName.c_str());
					AES_DDT_LOG(LOG_LEVEL_INFO, "Status CheckPoint of DS:<%s> successfully closed", m_dataSourceName.c_str());
				}
				else
				{
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to close status checkpoint of DS:<%s>, error:<%d>",
							m_dataSourceName.c_str(), result);
					AES_DDT_TRACE_MESSAGE("ERROR: Failed to close status checkpoint of DS:<%s>, error:<%d>", m_dataSourceName.c_str(), result);
				}
			}
		}

		bool StatusHandler::createProducerSectionInfo(const boost::shared_ptr<ProducerInfo>& producerCkpInfo)
		{
			AES_DDT_TRACE_FUNCTION;
			bool result = false;

			// Assemble the section id with the producer name
			char sectionId[StatusCheckPoint::MaxSectionIdSize] = {0};
			strncpy(sectionId, producerCkpInfo->getProducerName(), StatusCheckPoint::MaxSectionIdSize);

			AES_DDT_TRACE_MESSAGE("Adding section:<%s> for DS:<%s>", sectionId, m_dataSourceName.c_str() );

			SaCkptSectionIdT sectionIdentity;
			sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
			sectionIdentity.idLen = strlen(sectionId);

			// Section attributes
			SaCkptSectionCreationAttributesT sectionCreationAttributes;
			sectionCreationAttributes.sectionId = &sectionIdentity;
			sectionCreationAttributes.expirationTime = SA_TIME_END; // never deletes the section automatically

			// Assemble the buffer to write into the section
			char bufferToWrite[StatusCheckPoint::MaxSectionSize] = {0};
			uint64_t bufferLength = StatusCheckPoint::MaxSectionSize;

			producerCkpInfo->objectToBuffer(bufferLength, bufferToWrite);

			// Create the section inside the status checkpoint
			SaAisErrorT ckptResult = saCkptSectionCreate(m_checkpointHandle, &sectionCreationAttributes, bufferToWrite, bufferLength);

			if(SA_AIS_OK == ckptResult)
			{
				result = true;
				AES_DDT_TRACE_MESSAGE("Created producer section:<%s> into status checkpoint of DS:<%s>", sectionId, m_dataSourceName.c_str() );
				AES_DDT_LOG(LOG_LEVEL_INFO, "Created producer section:<%s> into status checkpoint of DS:<%s>", sectionId, m_dataSourceName.c_str() );
			}
			else
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to create producer section:<%s> into status checkpoint of DS:<%s>, error:<%d>",
																sectionId, m_dataSourceName.c_str(), ckptResult);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to create producer section:<%s> into status checkpoint of DS:<%s>, error:<%d>",
						sectionId, m_dataSourceName.c_str(), ckptResult);
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		bool StatusHandler::updateProducerSectionInfo(const boost::shared_ptr<ProducerInfo>& producerCkpInfo)
		{
			AES_DDT_TRACE_FUNCTION;
			bool result = false;

			// Assemble the section id with the producer name
			char sectionId[StatusCheckPoint::MaxSectionIdSize] = {0};
			strcpy(sectionId, producerCkpInfo->getProducerName());

			AES_DDT_TRACE_MESSAGE("Update section:<%s> for DS:<%s>", sectionId, m_dataSourceName.c_str() );

			SaCkptSectionIdT sectionIdentity;
			sectionIdentity.id = reinterpret_cast<unsigned char*>(sectionId);
			sectionIdentity.idLen = strlen(sectionId);

			// Assemble the buffer to write into the section
			char bufferToWrite[StatusCheckPoint::MaxSectionSize] = {0};
			uint64_t bufferLength = StatusCheckPoint::MaxSectionSize;

			producerCkpInfo->objectToBuffer(bufferLength, bufferToWrite);

			// Create the section inside the status checkpoint
			SaAisErrorT ckptResult = saCkptSectionOverwrite(m_checkpointHandle, &sectionIdentity, bufferToWrite, bufferLength);

			if(SA_AIS_OK == ckptResult)
			{
				result = true;
				AES_DDT_TRACE_MESSAGE("Updated producer section:<%s> into status checkpoint of DS:<%s>", sectionId, m_dataSourceName.c_str() );
				AES_DDT_LOG(LOG_LEVEL_INFO, "Updated producer section:<%s> into status checkpoint of DS:<%s>", sectionId, m_dataSourceName.c_str() );
			}
			else
			{
				result = false;
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to update producer section:<%s> into status checkpoint of DS:<%s>, error:<%d>",
						sectionId, m_dataSourceName.c_str(), ckptResult);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to update producer section:<%s> into status checkpoint of DS:<%s>, error:<%d>",
						sectionId, m_dataSourceName.c_str(), ckptResult);
			}

			AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result));
			return result;
		}

		bool StatusHandler::deleteStatusCheckpoint()
		{

			AES_DDT_TRACE_FUNCTION;
			bool result = true;

			// Set the checkpoint name
			std::string statusCktpName(CheckPoint::NamePrefix);
			statusCktpName.append(m_dataSourceName);

			// Set status checkpoint name
			SaNameT checkpointName;
			memset(checkpointName.value, 0U, sizeof(checkpointName.value)/sizeof(checkpointName.value[0]));
			checkpointName.length = statusCktpName.length();
			memcpy(checkpointName.value, statusCktpName.c_str(), checkpointName.length);

			SaAisErrorT deleteResult = saCkptCheckpointUnlink(m_CkptServiceHandle, &checkpointName);

			if(SA_AIS_OK == deleteResult)
			{
				AES_DDT_TRACE_MESSAGE("Status CheckPoint:<%s> of DS:<%s> successfully deleted",
						statusCktpName.c_str(),  m_dataSourceName.c_str());
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "Status CheckPoint:<%s> of DS:<%s> successfully deleted",
						statusCktpName.c_str(),  m_dataSourceName.c_str());
			}
			else
			{
				result = false;
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to delete Status CheckPoint:<%s> of DS:<%s>, error:<%d>",
						statusCktpName.c_str(), m_dataSourceName.c_str(), deleteResult);
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to delete Status CheckPoint:<%s> of DS:<%s>, error:<%d>",
						statusCktpName.c_str(), m_dataSourceName.c_str(), deleteResult);
			}

			AES_DDT_TRACE_MESSAGE("result:<%d>", result);
			return result;

		}


	} /* namespace ddt_ckpt */
}
