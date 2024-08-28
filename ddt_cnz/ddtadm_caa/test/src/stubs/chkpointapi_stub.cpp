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
#include "stubs/chkpointapi_stub.h"
#include "string.h"
#include "iostream"
#include <boost/lexical_cast.hpp>

int itr_cnt = 0;

SaAisErrorT saCkptCheckpointOpen(SaCkptHandleT ckptHandle,
                              const SaNameT *checkpointName,
                              const SaCkptCheckpointCreationAttributesT *checkpointCreationAttributes,
                              SaCkptCheckpointOpenFlagsT checkpointOpenFlags,
                              SaTimeT timeout, SaCkptCheckpointHandleT *checkpointHandle)
{
    return SA_AIS_OK;
}

SaAisErrorT saCkptCheckpointClose(SaCkptCheckpointHandleT checkpointHandle)
{
	return SA_AIS_OK;
}

SaAisErrorT saCkptCheckpointRead(SaCkptCheckpointHandleT checkpointHandle,
		SaCkptIOVectorElementT *ioVector,
		SaUint32T numberOfElements, SaUint32T *erroneousVectorIndex)
{
	if(0 == memcmp(ioVector->sectionId.id, "5000", ioVector->sectionId.idLen))
	{
		unsigned char dataBuffer[] = { 0x04, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
		memcpy(ioVector->dataBuffer, dataBuffer, sizeof(dataBuffer)+1);
	}
	else
	{
		char* buffer= "24000_235";
		memcpy(ioVector->dataBuffer, buffer, sizeof(buffer)+1);
	}
	return SA_AIS_OK;
}

SaAisErrorT saCkptSectionCreate(SaCkptCheckpointHandleT checkpointHandle,
                             SaCkptSectionCreationAttributesT *sectionCreationAttributes,
                             const void *initialData, SaSizeT initialDataSize)
{
	std::cout << "\nsectionId = " << sectionCreationAttributes->sectionId->id << std::endl;
	if (0 == memcmp(initialData, "0__9999",sizeof(initialData)))
		return SA_AIS_ERR_INVALID_PARAM;
	else if (0 == memcmp(sectionCreationAttributes->sectionId->id, "INVALID",sizeof(sectionCreationAttributes->sectionId->id)))
			return SA_AIS_ERR_INVALID_PARAM;
	else
		return SA_AIS_OK;
}

SaAisErrorT saCkptSectionOverwrite(SaCkptCheckpointHandleT checkpointHandle,
		const SaCkptSectionIdT *sectionId, const void *dataBuffer, SaSizeT dataSize)
{
	std::cout << "\nsectionId = " << sectionId->id << std::endl;
	if (0 == memcmp(dataBuffer, "0__9999",sizeof(dataBuffer)))
		return SA_AIS_ERR_INVALID_PARAM;
	else if (0 == memcmp(sectionId->id, "INVALID",sizeof(sectionId->id)))
		return SA_AIS_ERR_INVALID_PARAM;
	else
		return SA_AIS_OK;
}

SaAisErrorT saCkptSectionIterationInitialize(SaCkptCheckpointHandleT checkpointHandle,
                                          SaCkptSectionsChosenT sectionsChosen,
                                          SaTimeT expirationTime,
                                          SaCkptSectionIterationHandleT *sectionIterationHandle)
{
	itr_cnt = 0;
	return SA_AIS_OK;
}

SaAisErrorT saCkptSectionIterationNext(SaCkptSectionIterationHandleT sectionIterationHandle,
                                    SaCkptSectionDescriptorT *sectionDescriptor)
{
	if (itr_cnt >= 2)
		return SA_AIS_ERR_BAD_OPERATION;
	else
	{
		char sectionId[8] = {0};
		strcpy(sectionId, "5000");
		sectionDescriptor->sectionId.id = reinterpret_cast<unsigned char*>(sectionId);
		sectionDescriptor->sectionId.idLen = strlen(sectionId);
		++itr_cnt;
		return SA_AIS_OK;
	}
}

SaAisErrorT saCkptSectionIterationFinalize(SaCkptSectionIterationHandleT sectionIterationHandle)
{
	return SA_AIS_OK;
}
