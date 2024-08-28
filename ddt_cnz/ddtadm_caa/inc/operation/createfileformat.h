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
#ifndef DDTADM_CAA_INC_OPERATION_CREATEFILEFORMAT_H_
#define DDTADM_CAA_INC_OPERATION_CREATEFILEFORMAT_H_

#ifndef CUTE_TEST
	#include "operation/operation.h"
	#include "operation/operationbase.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace operation
{
	/**
	 * @class  CreateFileFormat
	 *
	 * @brief  Operation to create a new FileFormat. It is scheduled on IMM request.
	 *
	 * @sa  OperationBase
	 */
	class CreateFileFormat : public OperationBase
	{
 	public:
		/// Constructor
		CreateFileFormat();

		/// Destructor
		virtual ~CreateFileFormat();

		/**
		 * @brief  Sets MO information.
		 */
		virtual void setOperationDetails(const void* op_details);

		/**
		 * @brief  Implementation of operation to define a new File Format.
		 */
		virtual int call();

 	private:

		/// File Format MO attributes
		operation::fileFormatInfo m_info;
	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_OPERATION_CREATEFILEFORMAT_H_ */
