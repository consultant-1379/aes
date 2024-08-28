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

#include "store/checkpoint/memorycontroller.h"
#include "common/programconstants.h"
#include "engine/workingset.h"
#include "alarm/datasourcewarning.h"
#include "common/configuration.h"
#include "common/logger.h"
#include "common/tracer.h"
#include "common/utility.h"

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_MemoryController)

namespace store
{
	const std::string objectOfReference = "DirectDataTransferM=1";

	namespace checkpoint
	{

		MemoryController::MemoryController()
		: m_maxUsableMemory(common::configuration_t::instance()->getMaxUsableMemory()),
		  m_usedMemory(0ULL)
		{
			AES_DDT_TRACE_FUNCTION;
		}

		MemoryController::~MemoryController()
		{
			AES_DDT_TRACE_FUNCTION;
		}

		void MemoryController::increaseUsed(const uint64_t& alreadyInUseMemory)
		{
			AES_DDT_TRACE_MESSAGE("Memory in use:<%zu>", alreadyInUseMemory);

			m_usedMemory += alreadyInUseMemory;

			AES_DDT_LOG(LOG_LEVEL_INFO, "Current memory in use:<%zu>, maximum usable memory:<%zu>",
					m_usedMemory, m_maxUsableMemory);

			// check memory usage level
			checkUsage();
		}

		bool MemoryController::allocate(const uint64_t& memorySize)
		{
			AES_DDT_TRACE_MESSAGE("Needed memory:<%zu>", memorySize);

			bool result = (memorySize <= (m_maxUsableMemory - m_usedMemory));

			if(result)
			{
				//Increment the used space
				m_usedMemory += memorySize;

				// // check if the used memory is upper than the memory limit
				checkUsage();
			}

			return result;
		}

		void MemoryController::free(const uint64_t& memorySize)
		{
			// Decrement the used space
			m_usedMemory -= memorySize;

			// check if used memory is lower than the memory limit
			checkUsage();
		}

		void MemoryController::checkUsage()
		{
			AES_DDT_TRACE_FUNCTION;

			double percentageOfMemoryInUse = m_usedMemory * 100 / m_maxUsableMemory;

			AES_DDT_TRACE_MESSAGE("memoryInUse : <%zu>\n"
					"maximum usable memory : <%zu>\n"
					"percentageOfMemoryInUse :<%f> ",
					m_usedMemory, m_maxUsableMemory, percentageOfMemoryInUse);


			//if - check if TotalUsedPerceantage >= raisingPercentage and raise alarm
			if(percentageOfMemoryInUse >= threshold::raisingPercentage)
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "memoryInUse : <%zu>\n"
						"maximum usable memory : <%zu>\n"
						"percentageOfMemoryInUse :<%f> ",
						m_usedMemory, m_maxUsableMemory, percentageOfMemoryInUse);

				engine::workingSet_t::instance()->getAlarmhandler().raiseDataSourceWarning(store::objectOfReference, threshold::raisingPercentage);
			}
			//check totalUsedPercentage < ceasingPercentage and cease
			else if(percentageOfMemoryInUse <= threshold::ceasingPercentage)
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "memoryInUse : <%zu>\n"
						"maximum usable memory : <%zu>\n"
						"percentageOfMemoryInUse :<%f> ",
						m_usedMemory, m_maxUsableMemory, percentageOfMemoryInUse);

				engine::workingSet_t::instance()->getAlarmhandler().ceaseDataSourceWarning(store::objectOfReference);
			}
		}
	} /* namespace checkpoint */
} /* namespace store */
