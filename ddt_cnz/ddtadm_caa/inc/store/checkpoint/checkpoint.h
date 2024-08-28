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

#ifndef DDTADM_CAA_INC_AES_DDT_CHECKPOINT_H_
#define DDTADM_CAA_INC_AES_DDT_CHECKPOINT_H_

#include <stdint.h>
#include <string>

namespace store
{
	namespace checkpoint
	{
		namespace CheckPoint
		{
			// Checkpoint Service details
			const uint8_t ReleaseCode = 'B';
			const uint8_t MajorVersion = 2U;
			const uint8_t MinorVersion = 0x00;

			const SaUint64T INVALID_HANDLE = -1;

			const char NamePrefix[] = "safCkpt=DDT_"; // Checkpoint name must be: "safCkpt=DDT_<name>"
			const char NameSplit = '_';

			const SaSizeT MaxSectionIdSize = 8U;

			// invocation is considered to have	failed if it does not complete within the time specified
			const SaTimeT InvocationTimeOutInNanoSec = 100*1000000000LL;

			// Sections search parameter
			// Searching existing sections fulfilling the specified criteria.
			const SaCkptSectionsChosenT SectionSelectCriteria = SA_CKPT_SECTIONS_ANY;

			// This field is ignored with selection criteria is "SA_CKPT_SECTIONS_ANY"
			const SaTimeT SectionExpirationTime = 0U;

			const std::string InfoSectionName("INFOSEC");
			const SaSizeT MaxInfoSectionSize = 512U;
			const char InfoSeparator[] = "__";

			const float SystemMemoryPercentage = 0.25f;
		}

		namespace StatusCheckPoint
		{
			const SaUint32T MaxSections = 64U;
			const SaSizeT MaxSectionIdSize = 8U;
			const SaSizeT MaxSectionSize = 64U*1024U;
		}

	}
}


#endif /* DDTADM_CAA_INC_AES_DDT_CHECKPOINT_H_ */
