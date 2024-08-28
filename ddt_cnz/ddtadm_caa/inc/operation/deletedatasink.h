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
#ifndef DDTADM_CAA_INC_OPERATION_DELETEDATASINK_H_
#define DDTADM_CAA_INC_OPERATION_DELETEDATASINK_H_

#ifndef CUTE_TEST
	#include "operation/operation.h"
	#include "operation/operationbase.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace operation
{
	/**
	 * @class DeleteDataSink
	 *
	 * @brief  Operation to delete a data sink.
	 *
	 * @sa OperationBase
	 */
	class DeleteDataSink : public OperationBase
	{
 	public:

		/// Constructor.
		DeleteDataSink();

		/// Destructor.
		virtual ~DeleteDataSink();

		/**
		 * @brief  Sets the data sink information.
		 */
		virtual void setOperationDetails(const void* op_details);

		/**
		 * @brief  Implementation of operation to delete a data sink.
		 */
		virtual int call();

 	private:

		/// Data Sink attributes
		operation::dataSinkInfo m_info;
	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_OPERATION_DELETEDATASINK_H_ */
