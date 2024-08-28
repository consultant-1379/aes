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

#include "operation/createfilepeer.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_CreateFilePeer)

namespace operation
{

	CreateFilePeer::CreateFilePeer()
	: OperationBase(FILEPEER_ADD)
	{
		;
	}

	CreateFilePeer::~CreateFilePeer()
	{
		;
	}

	void CreateFilePeer::setOperationDetails(const void* op_details)
	{
		const operation::filePeerInfo* pFilePeerInfo = reinterpret_cast<const operation::filePeerInfo*>(op_details);
		m_info = *pFilePeerInfo;
	}

	int CreateFilePeer::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("\n"
				"Create a new FilePeer\n"
				"##### CREATE\n"
				"#####    FilePeer: <%s>\n"
				"#####    Source/Sink: <%s>/<%s>\n"
				"#####    IP:<%s:%u>\n"
				"#####    username:<%s>\n"
				"#####    Remote Folder:<%s>\n"
				"#####    Protocol: <%s>\n",
				m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(), m_info.getIpAddress(), m_info.getPortNumber(),
				m_info.username.c_str(), m_info.remoteFolderPath.c_str(), (m_info.transferProtocol == common::PROTOCOL_FTP ? "FTP" : "SFTP"));

		printf("\n"
						"Create a new FilePeer\n"
						"##### CREATE\n"
						"#####    FilePeer: <%s>\n"
						"#####    Source/Sink: <%s>/<%s>\n"
						"#####    IP:<%s:%u>\n"
						"#####    username:<%s>\n"
						"#####    Remote Folder:<%s>\n"
						"#####    Protocol: <%s>\n",
						m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(), m_info.getIpAddress(), m_info.getPortNumber(),
						m_info.username.c_str(), m_info.remoteFolderPath.c_str(), (m_info.transferProtocol == common::PROTOCOL_FTP ? "FTP" : "SFTP"));

		engine::workingSet_t::instance()->getDataSourceManager().add(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
