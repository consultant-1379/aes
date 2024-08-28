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

#include "operation/createfileformat.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_CreateFileFormat)

namespace operation
{

	CreateFileFormat::CreateFileFormat()
	: OperationBase(FILEFORMAT_ADD)
	{
	}

	CreateFileFormat::~CreateFileFormat()
	{
	}

	void CreateFileFormat::setOperationDetails(const void* op_details)
	{
		const operation::fileFormatInfo* pFileFormatInfo = reinterpret_cast<const operation::fileFormatInfo*>(op_details);
		m_info = *pFileFormatInfo;
	}

	int CreateFileFormat::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("\n"
				"Create a new FileFormat\n"
				"##### CREATE\n"
				"#####    FileFormat: <%s>\n"
				"#####    Source/Sink: <%s>/<%s>\n"
				"#####    fileSize: <%u>\n"
				"#####    startupSequenceNumber: <%u>\n"
				"#####    templateValue: <%s>\n",
				m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(),
				m_info.fileSize, m_info.startupSequenceNumber, m_info.templateValue.c_str());

		printf("\n"
						"Create a new FileFormat\n"
						"##### CREATE\n"
						"#####    FileFormat: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    fileSize: <%u>\n"
						"#####    startupSequenceNumber: <%u>\n"
						"#####    templateValue: <%s>\n",
						m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(),
						m_info.fileSize, m_info.startupSequenceNumber, m_info.templateValue.c_str());

		engine::workingSet_t::instance()->getDataSourceManager().add(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
