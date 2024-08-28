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

#include "operation/creator.h"
#include "operation/noop.h"
#include "operation/start.h"
#include "operation/stop.h"
#include "operation/shutdown.h"

#include "operation/createdatasource.h"
#include "operation/deletedatasource.h"
#include "operation/modifydatasource.h"
#include "operation/createtransferprogress.h"

#include "operation/createdatasink.h"
#include "operation/deletedatasink.h"
#include "operation/modifydatasink.h"

#include "operation/createblockpeer.h"
#include "operation/deleteblockpeer.h"
//#include "operation/modifyblockpeer.h" TODO

#include "operation/createfilepeer.h"
#include "operation/deletefilepeer.h"
#include "operation/modifyfilepeer.h"

#include "operation/createoutputformat.h"
//#include "operation/deleteoutputformat.h" // NOT NEEDED AT THE MOMENT
#include "operation/modifyoutputformat.h"

#include "operation/createfileformat.h"
//#include "operation/deletefileformat.h" // NOT NEEDED AT THE MOMENT
#include "operation/modifyfileformat.h"

#include "operation/switchactivepeer.h"
#include "common/programconstants.h"


#ifndef CUTE_TEST
	#include <ace/Method_Request.h>
	#include "engine/workingset.h"
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/macros.h"
#else
	#include "stubs/operation_stub.h"
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_Creator)

namespace operation {

	int Creator::schedule(const operation::identifier_t id)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		OperationBase* operation = make(id);

		if( operation )
		{
			result = engine::workingSet_t::instance()->getScheduler().enqueue(operation);
		}
		else
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot allocate new operation:<%d> request", id);
			AES_DDT_TRACE_MESSAGE("ERROR: cannot allocate new operation:<%d> request. errno:<%d>", id, errno);
			result = common::errorCode::ERR_MEMORY_BAD_ALLOC;
		}

		return result;
	}

	int Creator::schedule(const operation::identifier_t id, const void* op_details)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		OperationBase* operation = make(id);

		if( operation )
		{
			operation->setOperationDetails(op_details);

			result = engine::workingSet_t::instance()->getScheduler().enqueue(operation);
		}
		else
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot allocate new operation:<%d> request", id);
			AES_DDT_TRACE_MESSAGE("ERROR: cannot allocate new operation:<%d> request. errno:<%d>", id, errno);
			result = common::errorCode::ERR_MEMORY_BAD_ALLOC;
		}

		return result;
		}

	int Creator::schedule(const operation::identifier_t id, ACE_Future<operation::result>* op_result, const void* op_details)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		OperationBase* operation = make(id);

		if( operation )
		{
			operation->setOperationResultRequest(op_result);
			operation->setOperationDetails(op_details);

			result = engine::workingSet_t::instance()->getScheduler().enqueue(operation);
		}
		else
		{
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "cannot allocate new operation:<%d> request", id);
			AES_DDT_TRACE_MESSAGE("ERROR: cannot allocate new operation:<%d> request. errno:<%d>", id, errno);
			result = common::errorCode::ERR_MEMORY_BAD_ALLOC;
		}

		return result;
	}

	OperationBase* Creator::make(const operation::identifier_t id)
	{
		AES_DDT_TRACE_FUNCTION;
		OperationBase* operation;

		switch(id)
		{
			case DATASOURCE_ADD:
			{
				operation = new (std::nothrow) CreateDataSource();
			}
			break;

			case DATASOURCE_DEL:
			{
				operation = new (std::nothrow) DeleteDataSource();
			}
			break;

			case DATASOURCE_MOD:
			{
				operation = new (std::nothrow) ModifyDataSource();
			}
			break;

			case DATASINK_ADD:
			{
				operation = new (std::nothrow) CreateDataSink();
			}
			break;

			case DATASINK_DEL:
			{
				operation = new (std::nothrow) DeleteDataSink();
			}
			break;

			case DATASINK_MOD:
			{
				operation = new (std::nothrow) ModifyDataSink();
			}
			break;

			case BLOCKPEER_ADD:
			{
				operation = new (std::nothrow) CreateBlockPeer();
			}
			break;

			case BLOCKPEER_DEL:
			{
				operation = new (std::nothrow) DeleteBlockPeer();
			}
			break;

			//case BLOCKPEER_MOD: TODO
			//{
			//	operation = new (std::nothrow) ModifyBlockPeer();
			//}
			//break;

			case FILEPEER_ADD:
			{
				operation = new (std::nothrow) CreateFilePeer();
			}
			break;

			case FILEPEER_DEL:
			{
				operation = new (std::nothrow) DeleteFilePeer();
			}
			break;

			case FILEPEER_MOD:
			{
				operation = new (std::nothrow) ModifyFilePeer();
			}
			break;

			case OUTPUTFORMAT_ADD:
			{
				operation = new (std::nothrow) CreateOutputFormat();
			}
			break;

			//case OUTPUTFORMAT_DEL: // NOT NEEDED SINCE THE OUTPUT FORMAT WILL BE DELETED ALONG WITH THE ENTIRE DATA SINK
			//{
			//	operation = new (std::nothrow) DeleteOutputFormat();
			//}
			//break;

			case OUTPUTFORMAT_MOD:
			{
				operation = new (std::nothrow) ModifyOutputFormat();
			}
			break;

			case FILEFORMAT_ADD:
			{
				operation = new (std::nothrow) CreateFileFormat();
			}
			break;

			//case FILEFORMAT_DEL: // NOT NEEDED SINCE THE FILE FORMAT WILL BE DELETED ALONG WITH THE ENTIRE DATA SINK
			//{
			//	operation = new (std::nothrow) DeleteFileFormat();
			//}
			//break;

			case FILEFORMAT_MOD:
			{
				operation = new (std::nothrow) ModifyFileFormat();
			}
			break;

			case SWITCHACTIVEPEER_ACTION:
			{
				operation = new (std::nothrow) SwitchActivePeer();
			}
			break;

			case START:
			{
				operation = new (std::nothrow) Start();
			}
			break;

			case STOP:
			{
				operation = new (std::nothrow) Stop();
			}
			break;

			case SHUTDOWN:
			{
				operation = new (std::nothrow) Shutdown();
			}
			break;

			case TRANSFERPROGRESS_ADD:
			{
				operation = new (std::nothrow) CreateTransferProgress();
			}
			break;

			default:
			{
				operation = new (std::nothrow) NoOp();
			}
		}

		return operation;

	}


} /* namespace operation */

