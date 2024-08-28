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

#ifndef DDTADM_CAA_INC_AES_DDT_CREATETRANSFERPROGRESS_H_
#define DDTADM_CAA_INC_AES_DDT_CREATETRANSFERPROGRESS_H_

#ifndef CUTE_TEST
	#include "ACS_CC_Types.h"
	#include "operation/operation.h"
	#include "operation/operationbase.h"
#else
	#include "stubs/imm_stubs.h"
	#include "stubs/operation_stub.h"
#endif

class OmHandler;
namespace operation
{
	/**
	 * @class  CreateTransferProgress
	 *
	 * @brief  Operation to create a new TransferProgress MO.
	 *
	 * @sa  OperationBase
	 */
	class CreateTransferProgress : public OperationBase
	{
	 public:

		/// Constructor.
		CreateTransferProgress();

		/// Destructor.
		virtual ~CreateTransferProgress();

		/**
		 * @brief  Sets the data source and producer information.
		 */
		virtual void setOperationDetails(const void* op_details);

		/**
		 * @brief  Implementation of operation to create a TransferProgress MO.
		 */
		virtual int call();

	 private:

		/**
		 * @brief  Creates TransferProgress MO using OmHandler APIs.
		 */
		ACS_CC_ReturnType createTransferProgressObject();

		/// Transfer Progress attributes
		operation::transferProgressInfo m_info;

	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_AES_DDT_CREATETRANSFERPROGRESS_H_ */
