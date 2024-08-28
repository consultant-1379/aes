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

#ifndef DDTADM_CAA_INC_STORE_CHECKPOINT_MEMEORYCONTROLLER_H_
#define DDTADM_CAA_INC_STORE_CHECKPOINT_MEMEORYCONTROLLER_H_
#include <map>
#include "common/programconstants.h"

namespace store
{
	namespace checkpoint {

		namespace threshold
		{
			const int raisingPercentage = 75;
			const int ceasingPercentage = 65;
		}

		/**
		 * @class MemoryController
		 *
		 * @brief
		 *
		 * This class implements all actions needed in managing memory allocation for data sources..
		 *
		 */
		class MemoryController
		{
		public:

			///Constructor
			MemoryController();

			///Destructor
			virtual ~MemoryController();

			/** @brief
			 *
			 *	This method increase the initial memory in use during service startup
			 *	@remarks Remarks
			 */
			void increaseUsed(const uint64_t& alreadyInUseMemory);

			/** @brief
			 *
			 *	This method checks if there is enough memory
			 *	and if it is available increments the memory used by data sources
			 *
			 *	@param memorySize: Memory amount needed.
			 *
			 * 	return true on memory available otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool allocate(const uint64_t& memorySize);

			/** @brief
			 *
			 *	This method decrements the memory used by data sources
			 *
			 *	@param memorySize: Memory amount not more needed.
			 *
			 *	@remarks Remarks
			 */
			void free(const uint64_t& memorySize);

		private:

			/** @brief
			 *
			 *	This method checks if the used memory is within the predefined threshold limits.
			 *	Depending on the amount of memory used an alarm can be raised or ceased.
			 *
			 *	@remarks Remarks
			 */
			void checkUsage();

			/// max usable memory in bytes
			uint64_t m_maxUsableMemory;

			/// used memory in bytes
			uint64_t m_usedMemory;

		};
	} /* namespace checkpoint */
} /* namespace store */

#endif /* DDTADM_CAA_INC_STORE_CHECKPOINT_MEMEORYCONTROLLER_H_ */
