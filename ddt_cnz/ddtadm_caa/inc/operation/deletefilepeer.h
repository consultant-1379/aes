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
#ifndef DDTADM_CAA_INC_OPERATION_DELETEFILEPEER_H_
#define DDTADM_CAA_INC_OPERATION_DELETEFILEPEER_H_

#ifndef CUTE_TEST
	#include "operation/operation.h"
	#include "operation/operationbase.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace operation
{

	/**
	 * @class DeleteFilePeer
	 *
	 * @brief  Operation to delete a file peer.
	 *
	 * @sa OperationBase
	 */
	class DeleteFilePeer : public OperationBase
	{
 	public:

		/// Constructor.
		DeleteFilePeer();

		/// Destructor.
		virtual ~DeleteFilePeer();

		/**
		 * @brief  Sets the file peer information.
		 */
		virtual void setOperationDetails(const void* op_details);

		/**
		 * @brief  Implementation of operation to delete a file peer.
		 */
		virtual int call();

 	private:

		/// File Peer attributes
		operation::filePeerInfo m_info;

	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_OPERATION_DELETEFILEPEER_H_ */
