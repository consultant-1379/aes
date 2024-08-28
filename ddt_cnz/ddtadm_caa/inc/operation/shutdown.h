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
#ifndef DDTADM_CAA_INC_AES_DDT_SHUTDOWN_H_
#define DDTADM_CAA_INC_AES_DDT_SHUTDOWN_H_

#ifndef CUTE_TEST
	#include "operation/operationbase.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace operation
{
	/**
	 * @class Shutdown
	 *
	 * @brief  Terminate operation request.
	 *
	 * @sa OperationBase
	 */
	class Shutdown : public OperationBase
	{
	 public:

		/// Constructor.
		Shutdown();

		/// Destructor.
		virtual ~Shutdown() {}

		/**
		 * @brief  Implementation of the operation.
		 */
		virtual int call();
	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_AES_DDT_SHUTDOWN_H_ */
