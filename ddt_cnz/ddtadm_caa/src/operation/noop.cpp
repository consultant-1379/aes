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
#include "operation/noop.h"
#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/operation_stub.h"
#endif
#include "common/programconstants.h"

AES_DDT_TRACE_DEFINE(AES_DDT_Operation_NoOp)

namespace operation
{

  NoOp::NoOp()
  : OperationBase(NOOP)
  {
      AES_DDT_TRACE_FUNCTION;
  }

  int NoOp::call()
  {
      AES_DDT_TRACE_FUNCTION;
      AES_DDT_LOG(LOG_LEVEL_WARN, "NoOp executed");
      setResultToCaller();
      return common::errorCode::ERR_NO_ERRORS;
  }

} /* namespace operation */
