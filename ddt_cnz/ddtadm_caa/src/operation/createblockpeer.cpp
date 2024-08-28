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

#include "operation/createblockpeer.h"

#include "common/programconstants.h"

#ifndef CUTE_TEST
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_CreateBlockPeer)

namespace operation
{

	CreateBlockPeer::CreateBlockPeer()
	: OperationBase(BLOCKPEER_ADD)
	{
		;
	}

	CreateBlockPeer::~CreateBlockPeer()
	{
		;
	}

	void CreateBlockPeer::setOperationDetails(const void* op_details)
	{
		const operation::blockPeerInfo* pBlockPeerInfo = reinterpret_cast<const operation::blockPeerInfo*>(op_details);
		m_info = *pBlockPeerInfo;
	}

	int CreateBlockPeer::call()
	{
		AES_DDT_TRACE_FUNCTION;

		AES_DDT_TRACE_MESSAGE("\n"
				"Create a new BlockPeer\n"
				"##### CREATE\n"
				"#####    Block Peer: <%s>\n"
				"#####    Source/Sink: <%s>/<%s>\n"
				"#####    IP: <%s>\n",
				m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(), m_info.getIpAddress());

		printf("\n"
				"Create a new BlockPeer\n"
				"##### CREATE\n"
				"#####    Block Peer: <%s>\n"
				"#####    Source/Sink: <%s>/<%s>\n"
				"#####    IP: <%s>\n",
				m_info.getName(), m_info.getDataSourceName(), m_info.getDataSinkName(), m_info.getIpAddress());

		engine::workingSet_t::instance()->getDataSourceManager().add(m_info, m_operationResult);

		setResultToCaller();

		return common::errorCode::ERR_NO_ERRORS;
	}

} /* namespace operation */
