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
#include "common/configuration.h"

#include "common/tracer.h"
#include "common/logger.h"
#include "common/utility.h"
#include "ACS_APGCC_CommonLib.h"

#include <ACS_CS_API.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Common_Configuration)


namespace common
{
	//Max System memory can be allocated for DDT functionality on GEP2/GEP5/VM
	namespace MaxSystemMemory
	{
		const uint64_t GEP2 = 1024ULL*1024ULL*1024ULL;
		const uint64_t GEP5 = 2ULL*1024ULL*1024ULL*1024ULL; //2 GB
		const uint64_t  VM = 2ULL*1024ULL*1024ULL*1024ULL;  //2 GB
	}


	Configuration::Configuration()
	: m_cs_mutex(),
	  m_clusterName(),
	  m_hwVersion(HWVER_NOVALUE),
	  m_maxUsableMemory(0ULL)
	{

	}

	Configuration::~Configuration()
	{

	}

	std::string Configuration::getClusterName()
	{
		ACE_Guard<ACE_Recursive_Thread_Mutex> guard(m_cs_mutex);
		if(m_clusterName.empty())
		{
			ACS_CS_API_Name neid;
			ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getNEID(neid);
			if(result == ACS_CS_API_NS::Result_Success)
			{
				size_t length = neid.length();
				char tmpName[length + 1];
				memset(tmpName, 0, sizeof(tmpName));
				neid.getName(tmpName, length);
				m_clusterName.assign(tmpName);
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("ERROR: Failed to get Network Element ID, CS error:<%d>", result);
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to get Network Element ID, CS error:<%d>", result);
			}
		}

		AES_DDT_TRACE_MESSAGE("Cluster Name:<%s>", m_clusterName.c_str());
		return m_clusterName;
	}

	std::string getCPName(int32_t cp_system_id)
	{
		std::string cpName;
		ACS_CS_API_Name CPName;
		ACS_CS_API_NS::CS_API_Result result = ACS_CS_API_NetworkElement::getDefaultCPName(cp_system_id, CPName);

		if( ACS_CS_API_NS::Result_Success == result)
		{
			size_t length = CPName.length();
			char tmpName[length+1];
			memset(tmpName, 0, sizeof(tmpName));

			CPName.getName(tmpName, length);
			cpName.assign(tmpName);
		}
		else
		{
			// Error Case
			AES_DDT_TRACE_MESSAGE("ERROR: Failed to get Default Cp Name, CS error:<%d>", result);
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "Failed to get Default Cp Name, CS error:<%d>", result);
			// try to make it yourself
			int32_t CpBaseValue = 1000U;
			char tmpName[256] = {0};
			if(CpBaseValue > cp_system_id )
			{
				snprintf(tmpName, sizeof(tmpName) - 1, "BC%d", cp_system_id);
			}
			else
			{
				snprintf(tmpName, sizeof(tmpName) - 1, "CP%d", (cp_system_id - CpBaseValue));
			}

			cpName.assign(tmpName);
		}

		AES_DDT_TRACE_MESSAGE("Cp Name:<%s>", cpName.c_str());
		return cpName;
	}

	bool Configuration::getHWVersionInfo(common::HwVerConstants &hwVer)
	{
		AES_DDT_TRACE_FUNCTION;

		bool result=true;

		if(HWVER_NOVALUE == m_hwVersion)
		{
			ACS_APGCC_CommonLib acsApgccCommonLib;
			ACS_APGCC_HWINFO hwInfo;
			ACS_APGCC_HWINFO_RESULT hwInfoResult;

			acsApgccCommonLib.GetHwInfo(&hwInfo, &hwInfoResult, ACS_APGCC_GET_HWVERSION);

			result = (ACS_APGCC_HWINFO_SUCCESS == hwInfoResult.hwVersionResult);

			if(result)
			{
				switch(hwInfo.hwVersion)
				{
				case ACS_APGCC_HWVER_NOVALUE:
					m_hwVersion = HWVER_NOVALUE;
					result=false;
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR,"ACS_APGCC_CommonLib HWVER_NOVALUE");
					AES_DDT_TRACE_MESSAGE("ERROR ACS_APGCC_CommonLib HWVER_NOVALUE");
					break;

				case ACS_APGCC_HWVER_GEP1:
					m_hwVersion = HWVER_GEP1;
					break;

				case ACS_APGCC_HWVER_GEP2:
					m_hwVersion = HWVER_GEP2;
					break;

				case ACS_APGCC_HWVER_VM:
					m_hwVersion = HWVER_VM;
					break;

				case ACS_APGCC_HWVER_GEP5:  //Max System memory that can be allocated for DDT functionality on variants greater than or equal to GEP5
				default:
					m_hwVersion = HWVER_GEP5;
					result=true;
					break;
				}
			}
			else
			{
				AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR,"ACS_APGCC_CommonLib GetHwInfo FAILED, error:<%d>",
						hwInfoResult.hwVersionResult);

				AES_DDT_TRACE_MESSAGE("ACS_APGCC_CommonLib GetHwInfo FAILED, error:<%d>",
						hwInfoResult.hwVersionResult);
			}
		}


		hwVer = m_hwVersion;
		return result;
	}

	uint64_t Configuration::getMaxUsableMemory()
	{
		if( 0ULL == m_maxUsableMemory )
		{
			common::HwVerConstants hwVer;
			//Assuming GEP2 memory as default one
			m_maxUsableMemory = MaxSystemMemory::GEP2;

			if(getHWVersionInfo(hwVer))
			{
				AES_DDT_TRACE_MESSAGE("Hardware Type:<%d>", hwVer);
				AES_DDT_LOG(LOG_LEVEL_INFO, "**** Hardware Type:<%d> ****", hwVer);

				switch(hwVer)
				{
				case common::HWVER_GEP2:
					m_maxUsableMemory = MaxSystemMemory::GEP2;
					break;

				case common::HWVER_GEP5:
					m_maxUsableMemory = MaxSystemMemory::GEP5;
					break;

				case common::HWVER_VM:
					m_maxUsableMemory = MaxSystemMemory::VM;
					break;

				default:

					AES_DDT_TRACE_MESSAGE("ERROR: UKNOWN Hardware Type:<%d>!", hwVer);
					AES_DDT_LOG(LOG_LEVEL_ERROR, "UKNOWN Hardware Type:<%d>!", hwVer);
					break;
				}
			}
			else
			{
				AES_DDT_TRACE_MESSAGE("ERROR: FAILED TO GET Hardware Type!");
				AES_DDT_LOG(LOG_LEVEL_ERROR, "FAILED TO GET Hardware Type!");
			}

			AES_DDT_TRACE_MESSAGE("Max usable memory:<%zu>", m_maxUsableMemory);
			AES_DDT_LOG(LOG_LEVEL_INFO,"Max usable memory:<%zu>", m_maxUsableMemory);
		}

		return m_maxUsableMemory;
	}

} /* namespace common */


