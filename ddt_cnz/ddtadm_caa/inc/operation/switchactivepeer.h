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

#ifndef DDTADM_CAA_INC_OPERATION_SWITCHACTIVEPEER_H_
#define DDTADM_CAA_INC_OPERATION_SWITCHACTIVEPEER_H_

#ifndef CUTE_TEST
	#include "operation/operation.h"
	#include "operation/operationbase.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace operation
{
	/**
	 * @class  SwitchActivePeer
	 *
	 * @brief  Operation to switch active peer of a dataSink. It is scheduled on IMM "swtichActivePeer" action.
	 *
	 * @sa  OperationBase
	 */
	class SwitchActivePeer: public OperationBase
	{
	 public:

		///Constructor
		SwitchActivePeer();

		///Destructor
		virtual ~SwitchActivePeer();

		/**
		 * @brief  Sets MO information.
		 */
		virtual void setOperationDetails(const void* op_details);

		/**
		 * @brief  Implementation of operation swtich active peer
		 */
		virtual int call();

	 private:

		/// Switch Active Peer action info
		operation::switchActivePeerInfo m_info;
	};

} /* namespace operation */

#endif /* DDTADM_CAA_INC_OPERATION_SWITCHACTIVEPEER_H_ */
