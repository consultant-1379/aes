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
#ifndef INC_CHKPOINTAPI_STUB_H_
#define INC_CHKPOINTAPI_STUB_H_

#include <stdint.h>
#include <string>

#define SA_MAX_NAME_LENGTH 256

typedef char                  SaInt8T;
typedef short                 SaInt16T;
typedef int                   SaInt32T;
typedef long long             SaInt64T;
typedef unsigned char         SaUint8T;
typedef unsigned short        SaUint16T;
typedef unsigned int          SaUint32T;
typedef unsigned long long    SaUint64T;

/** Types used by the NTF/IMMS service **/
typedef float                 SaFloatT;
typedef double                SaDoubleT;
typedef char*                 SaStringT;

typedef SaInt64T              SaTimeT;
typedef SaUint64T             SaInvocationT;
typedef SaUint64T             SaSizeT;
typedef SaUint64T             SaOffsetT;
typedef SaUint64T             SaSelectionObjectT;

typedef struct {
    SaUint16T length;
    SaUint8T value[SA_MAX_NAME_LENGTH];
} SaNameT;
typedef enum {
   SA_AIS_OK = 1,
   SA_AIS_ERR_LIBRARY = 2,
   SA_AIS_ERR_VERSION = 3,
   SA_AIS_ERR_INIT = 4,
   SA_AIS_ERR_TIMEOUT = 5,
   SA_AIS_ERR_TRY_AGAIN = 6,
   SA_AIS_ERR_INVALID_PARAM = 7,
   SA_AIS_ERR_NO_MEMORY = 8,
   SA_AIS_ERR_BAD_HANDLE = 9,
   SA_AIS_ERR_BUSY = 10,
   SA_AIS_ERR_ACCESS = 11,
   SA_AIS_ERR_NOT_EXIST = 12,
   SA_AIS_ERR_NAME_TOO_LONG = 13,
   SA_AIS_ERR_EXIST = 14,
   SA_AIS_ERR_NO_SPACE = 15,
   SA_AIS_ERR_INTERRUPT =16,
   SA_AIS_ERR_NAME_NOT_FOUND = 17,
   SA_AIS_ERR_NO_RESOURCES = 18,
   SA_AIS_ERR_NOT_SUPPORTED = 19,
   SA_AIS_ERR_BAD_OPERATION = 20,
   SA_AIS_ERR_FAILED_OPERATION = 21,
   SA_AIS_ERR_MESSAGE_ERROR = 22,
   SA_AIS_ERR_QUEUE_FULL = 23,
   SA_AIS_ERR_QUEUE_NOT_AVAILABLE = 24,
   SA_AIS_ERR_BAD_FLAGS = 25,
   SA_AIS_ERR_TOO_BIG = 26,
   SA_AIS_ERR_NO_SECTIONS = 27,
   SA_AIS_ERR_NO_OP = 28,
   SA_AIS_ERR_REPAIR_PENDING = 29,
   SA_AIS_ERR_NO_BINDINGS = 30,
   SA_AIS_ERR_UNAVAILABLE = 31,
   SA_AIS_ERR_CAMPAIGN_ERROR_DETECTED = 32,
   SA_AIS_ERR_CAMPAIGN_PROC_FAILED = 33,
   SA_AIS_ERR_CAMPAIGN_CANCELED = 34,
   SA_AIS_ERR_CAMPAIGN_FAILED = 35,
   SA_AIS_ERR_CAMPAIGN_SUSPENDED = 36,
   SA_AIS_ERR_CAMPAIGN_SUSPENDING = 37,
   SA_AIS_ERR_ACCESS_DENIED = 38,
   SA_AIS_ERR_NOT_READY = 39,
   SA_AIS_ERR_DEPLOYMENT = 40
} SaAisErrorT;

#define SA_TIME_END              0x7FFFFFFFFFFFFFFFLL
#define SA_TIME_BEGIN            0x0LL
#define SA_TIME_UNKNOWN          0x8000000000000000LL


typedef SaUint64T SaCkptHandleT;
typedef SaUint64T SaCkptCheckpointHandleT;
typedef SaUint64T SaCkptSectionIterationHandleT;

typedef SaUint32T SaCkptCheckpointCreationFlagsT;

typedef struct {
	SaCkptCheckpointCreationFlagsT creationFlags;
	SaSizeT checkpointSize;
	SaTimeT retentionDuration;
	SaUint32T maxSections;
	SaSizeT maxSectionSize;
	SaSizeT maxSectionIdSize;
} SaCkptCheckpointCreationAttributesT;

typedef SaUint32T SaCkptCheckpointOpenFlagsT;

typedef struct {
	SaUint16T idLen;
	SaUint8T *id;
} SaCkptSectionIdT;

typedef struct {
	SaCkptSectionIdT sectionId;
	void *dataBuffer;
	SaSizeT dataSize;
	SaOffsetT dataOffset;
	SaSizeT readSize;
} SaCkptIOVectorElementT;

typedef struct {
	SaCkptSectionIdT *sectionId;
	SaTimeT expirationTime;
} SaCkptSectionCreationAttributesT;

typedef enum {
	SA_CKPT_SECTIONS_FOREVER = 1,
	SA_CKPT_SECTIONS_LEQ_EXPIRATION_TIME = 2,
	SA_CKPT_SECTIONS_GEQ_EXPIRATION_TIME = 3,
	SA_CKPT_SECTIONS_CORRUPTED = 4,
	SA_CKPT_SECTIONS_ANY = 5
} SaCkptSectionsChosenT;

#define SA_CKPT_WR_ALL_REPLICAS 0X1
#define SA_CKPT_WR_ACTIVE_REPLICA 0X2
#define SA_CKPT_WR_ACTIVE_REPLICA_WEAK 0X4
#define SA_CKPT_CHECKPOINT_COLLOCATED 0X8

#define SA_CKPT_CHECKPOINT_READ 0X1
#define SA_CKPT_CHECKPOINT_WRITE 0X2
#define SA_CKPT_CHECKPOINT_CREATE 0X4

SaAisErrorT saCkptCheckpointOpen(SaCkptHandleT ckptHandle,
                              const SaNameT *checkpointName,
                              const SaCkptCheckpointCreationAttributesT *checkpointCreationAttributes,
                              SaCkptCheckpointOpenFlagsT checkpointOpenFlags,
                              SaTimeT timeout, SaCkptCheckpointHandleT *checkpointHandle);

SaAisErrorT saCkptCheckpointClose(SaCkptCheckpointHandleT checkpointHandle);

SaAisErrorT saCkptCheckpointRead(SaCkptCheckpointHandleT checkpointHandle,
                              SaCkptIOVectorElementT *ioVector,
                              SaUint32T numberOfElements, SaUint32T *erroneousVectorIndex);

SaAisErrorT saCkptSectionCreate(SaCkptCheckpointHandleT checkpointHandle,
                             SaCkptSectionCreationAttributesT *sectionCreationAttributes,
                             const void *initialData, SaSizeT initialDataSize);

SaAisErrorT saCkptSectionOverwrite(SaCkptCheckpointHandleT checkpointHandle,
                                const SaCkptSectionIdT *sectionId, const void *dataBuffer, SaSizeT dataSize);

// StatusHandler Calls
typedef enum {
                SA_CKPT_SECTION_VALID = 1,
                SA_CKPT_SECTION_CORRUPTED = 2
        } SaCkptSectionStateT;

typedef struct {
                SaCkptSectionIdT sectionId;
                SaTimeT expirationTime;
                SaSizeT sectionSize;
                SaCkptSectionStateT sectionState;
                SaTimeT lastUpdate;
        } SaCkptSectionDescriptorT;

SaAisErrorT saCkptSectionIterationInitialize(SaCkptCheckpointHandleT checkpointHandle,
                                          SaCkptSectionsChosenT sectionsChosen,
                                          SaTimeT expirationTime,
                                          SaCkptSectionIterationHandleT *sectionIterationHandle);

SaAisErrorT saCkptSectionIterationNext(SaCkptSectionIterationHandleT sectionIterationHandle,
                                    SaCkptSectionDescriptorT *sectionDescriptor);

SaAisErrorT saCkptSectionIterationFinalize(SaCkptSectionIterationHandleT sectionIterationHandle);

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

			const char NamePrefix[] = "safCkpt=DDT_"; // Checkpoint name it must be "safCkpt=DDT_<name>"
			const char NameSplit = '_';

			const SaUint32T MaxSections = 6000U;
			const SaSizeT MaxSectionIdSize = 8U;

			const SaUint32T StubSectionID = MaxSections;

			// invocation is considered to have	failed if it does not complete within the time specified
			const SaTimeT InvocationTimeOutInNanoSec = 100*1000000000U;

			// Sections search parameter
			// Searching existing sections fulfilling the specified criteria.
			const SaCkptSectionsChosenT SectionSelectCriteria = SA_CKPT_SECTIONS_ANY;

			// This field is ignored with selection criteria "SA_CKPT_SECTIONS_ANY"
			const SaTimeT SectionExpirationTime = 0U;

			const std::string InfoSectionName("INFO");
			const SaSizeT MaxInfoSectionSize = 1024U;
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

#endif /* INC_CHKPOINTAPI_STUB_H_ */
