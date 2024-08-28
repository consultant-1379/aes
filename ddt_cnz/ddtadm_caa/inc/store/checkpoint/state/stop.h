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

#ifndef DDTADM_CAA_INC_AES_DDT_CKPTSTATESTOP_H_
#define DDTADM_CAA_INC_AES_DDT_CKPTSTATESTOP_H_

#include "store/checkpoint/state/baseinterface.h"

namespace store {
	namespace checkpoint {
		namespace state {

			/**
			 * @class StateStop
			 *
			 * @brief
			 *
			 * This class implements the stopped state, checkpoint updated and closed.
			 * No actions can be executed in this state and it is not possible to move to another state.
			 *
			 */
		class Stop: public BaseInterface
			{
			 public:

				///  Constructor.
				Stop(StoreBase* checkpointObj);

				///  Destructor.
				virtual ~Stop();

				/** @brief
				 *
				 *	This method returns always ERR_CKPT_STOPPED
				 *
				 *	@remarks Remarks
				 */
				virtual int handleStep();

				/** @brief
				 *
				 *	This method returns always itself as next state
				 *
				 *	@remarks Remarks
				 */
				virtual bool getNextStep(boost::shared_ptr<BaseInterface>& nextStep);
			};

		} /* namespace ddt_ckpt */
	}
}
#endif /* DDTADM_CAA_INC_AES_DDT_CKPTSTATESTOP_H_ */
