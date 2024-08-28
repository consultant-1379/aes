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

#ifndef DDTADM_CAA_INC_STORE_CHECKPOINT_STATE_DELETE_H_
#define DDTADM_CAA_INC_STORE_CHECKPOINT_STATE_DELETE_H_

#include "store/checkpoint/state/baseinterface.h"

namespace store {
	namespace checkpoint {
		namespace state {

			/**
			 * @class Delete
			 *
			 * @brief
			 *
			 * This class implements all actions needed to delete an empty checkpoint from the Checkpoint Service
			 * and from the internal DDT status
			 *
			 */
			class Delete: public BaseInterface
			{
			 public:

				///  Constructor.
				Delete(StoreBase* checkpointObj);

				///  Destructor.
				virtual ~Delete();

				/** @brief
				 *
				 *	This method deletes the checkpoint
				 *
				 *  @return ERR_NO_ERRORS on success otherwise an error code.
				 *
				 *	@remarks Remarks
				 */
				virtual int handleStep();

				/** @brief
				 *
				 *	This method gets the next internal state that implements the next step to execute
				 *
				 *  @param nextStep: BaseInterface pointer to the next step
				 *
				 *  @return true on success otherwise false.
				 *
				 *	@remarks Remarks
				 */
				virtual bool getNextStep(boost::shared_ptr<BaseInterface>& nextStep);
			};


		} /* namespace state */
	}/* namespace checkpoint */
}/* namespace store */

#endif /* DDTADM_CAA_INC_STORE_CHECKPOINT_STATE_DELETE_H_ */
