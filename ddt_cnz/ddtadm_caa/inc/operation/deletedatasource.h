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

#ifndef DDTADM_CAA_INC_AES_DDT_DELETEDATASOURCE_H_
#define DDTADM_CAA_INC_AES_DDT_DELETEDATASOURCE_H_

#include "operation/operation.h"

#ifndef CUTE_TEST
	#include "operation/operationbase.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace operation
{
	/**
	 * @class DeleteDataSource
	 *
	 * @brief  Operation to delete a data source.
	 *
	 * @sa OperationBase
	 */
	class DeleteDataSource : public OperationBase
	{
	 public:

		/// Constructor.
		DeleteDataSource();

		/// Destructor.
		virtual ~DeleteDataSource();

		/**
		 * @brief  Sets the data source information.
		 */
		virtual void setOperationDetails(const void* op_details);

		/**
		 * @brief  Implementation of operation to delete a data source.
		 */
		virtual int call();

	 private:

		/// Data Source attributes
		operation::dataSourceInfo m_info;
	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_AES_DDT_DELETEDATASOURCE_H_ */
