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
#ifndef DDTADM_CAA_INC_OPERATION_DELETEBLOCKPEER_H_
#define DDTADM_CAA_INC_OPERATION_DELETEBLOCKPEER_H_

#ifndef CUTE_TEST
	#include "operation/operation.h"
	#include "operation/operationbase.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace operation
{
	/**
	 * @class DeleteBlockPeer
	 *
	 * @brief  Operation to delete a block peer.
	 *
	 * @sa OperationBase
	 */
	class DeleteBlockPeer : public OperationBase
	{
 	public:

		/// Constructor.
		DeleteBlockPeer();

		/// Destructor.
		virtual ~DeleteBlockPeer();

		/**
		 * @brief  Sets the block peer information.
		 */
		virtual void setOperationDetails(const void* op_details);

		/**
		 * @brief  Implementation of operation to delete a block peer.
		 */
		virtual int call();

 	private:

		/// Block Peer attributes
		operation::blockPeerInfo m_info;
	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_OPERATION_DELETEBLOCKPEER_H_ */
