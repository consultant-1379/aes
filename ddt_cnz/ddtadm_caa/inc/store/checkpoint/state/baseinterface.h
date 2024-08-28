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

#ifndef DDTADM_CAA_INC_AES_DDT_CKPTINTERNALSTATEINTERFACE_H_
#define DDTADM_CAA_INC_AES_DDT_CKPTINTERNALSTATEINTERFACE_H_

#include "common/programconstants.h"
#include <boost/noncopyable.hpp>

#ifndef CUTE_TEST
	#include <boost/shared_ptr.hpp>
#else
	#include "stubs/store_stub.h"
#endif

namespace store {
	namespace checkpoint {

		class StoreBase;

		namespace state {



			/**
			 * @class CkptInternalStateInterface
			 *
			 * @brief
			 *
			 * This class provides an abstraction of the checkpoint internal state
			 *
			 */
			class BaseInterface: private boost::noncopyable
			{
			 public:

				enum ckpt_state_t{

					OPEN = 0,
					READINFO,
					WRITE,
					ITERATOR_INIT,
					READ,
					WAIT_COMMIT,
					WAIT_DATA,
					UPDATEINFO,
					CLOSE,
					DELETE,
					GETNEW,
					STOP,
					UNKNOWN
				};

				///  Constructor.
				BaseInterface(StoreBase* checkpointObj, ckpt_state_t type = UNKNOWN)
				 : m_checkpoint(checkpointObj),
				   m_type(type),
				   m_lastError(common::errorCode::ERR_GENERIC)
				 { }

				///  Constructor.
				BaseInterface(StoreBase* checkpointObj, int lastError, ckpt_state_t type = UNKNOWN)
				: m_checkpoint(checkpointObj),
				  m_type(type),
				  m_lastError(lastError)
				{ }

				///  Destructor.
				virtual ~BaseInterface() { }

				/** @brief
				 *
				 *	This method get the state type
				 *
				 *  @return ERR_NO_ERRORS on success otherwise an error code.
				 *
				 *	@remarks Remarks
				 */
				ckpt_state_t getStateType() const { return m_type; }

				/** @brief
				 *
				 *	This method executes a specific action
				 *
				 *  @return ERR_NO_ERRORS on success otherwise an error code.
				 *
				 *	@remarks Remarks
				 */
				virtual int handleStep() = 0;


				/** @brief
				 *
				 *	This method gets the next internal state that implements the next step to execute
				 *
				 *  @param nextStep: CkptInternalStateInterface pointer to the next step
				 *
				 *  @return true on success otherwise false.
				 *
				 *	@remarks Remarks
				 */
				virtual bool getNextStep(boost::shared_ptr<BaseInterface>& nextStep) = 0;

			  protected:

				// Pointer to the CheckPointBase object belongs to this state
				StoreBase* m_checkpoint;

				// Checkpoint state type
				ckpt_state_t m_type;

				// last occured error
				int m_lastError;
			};

		} /* namespace ddt_ckpt */
	}
}
#endif /* DDTADM_CAA_INC_AES_DDT_CKPTINTERNALSTATEINTERFACE_H_ */
