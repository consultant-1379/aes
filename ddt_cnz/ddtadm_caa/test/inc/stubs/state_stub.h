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
#ifndef INC_CKPTSTATE_STUB_H_
#define INC_CKPTSTATE_STUB_H_

#include "store/checkpoint/state/baseinterface.h"
#include "store/checkpoint/storebase.h"

namespace store {
	namespace checkpoint {
		namespace state {


			/**
			 * @class StateOpen
			 *
			 * @brief
			 *
			 * This class implements all actions needed to open the checkpoint in use
			 *
			 */
			class Open: public BaseInterface
			{
			 public:

				///  Constructor.
				Open(StoreBase* checkpointObj);

				///  Destructor.
				virtual ~Open();

				/** @brief
				 *
				 *	This method opens the current checkpoint
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
				 *  @param nextStep: CkptInternalStateInterface pointer to the next step
				 *
				 *  @return true on success otherwise false.
				 *
				 *	@remarks Remarks
				 */
				virtual bool getNextStep(boost::shared_ptr<BaseInterface>& nextStep);

			};

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

			/**
			 * @class Close
			 *
			 * @brief
			 *
			 * This class implements all actions needed to close a checkpoint.
			 *
			 */
			class Close: public BaseInterface
			{
			public:

				///  Constructor.
				Close(StoreBase* checkpointObj);

				///  Destructor.
				virtual ~Close();

				/** @brief
				 *
				 *	This method closes the checkpoint
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
				 *  @param nextStep: CkptInternalStateInterface pointer to the next step
				 *
				 *  @return true on success otherwise false.
				 *
				 *	@remarks Remarks
				 */
				virtual bool getNextStep(boost::shared_ptr<BaseInterface>& nextStep);

			};

			/**
			 * @class StateReadInfo
			 *
			 * @brief
			 *
			 * This class implements all actions needed to read the info section into the checkpoint in use.
			 * The Info section will be created in case it was not found.
			 *
			 */
			class ReadInfo: public BaseInterface
			{
			public:

				///  Constructor.
				ReadInfo(StoreBase* checkpointObj);

				///  Destructor.
				virtual ~ReadInfo();

				/** @brief
				 *
				 *	This method creates the info section in the current checkpoint
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
				 *  @param nextStep: CkptInternalStateInterface pointer to the next step
				 *
				 *  @return true on success otherwise false.
				 *
				 *	@remarks Remarks
				 */
				virtual bool getNextStep(boost::shared_ptr<BaseInterface>& nextStep);
			};

			class Write: public BaseInterface
			{
			public:

				///  Constructor.
				Write(StoreBase* checkpointObj);

				///  Destructor.
				virtual ~Write();

				/** @brief
				 *
				 *	This method checks if the checkpoint is full
				 *
				 *  @return ERR_CKPT_SECTIONS_FULL on checkpoint full condition otherwise ERR_CKPT_SECTIONS_AVAIABLE.
				 *
				 *	@remarks Remarks
				 */
				virtual int handleStep();

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
				virtual bool getNextStep(boost::shared_ptr<BaseInterface>& nextStep);
			};

		} /* namespace ddt_ckpt */
	}
}



#endif /* INC_CKPTSTATE_STUB_H_ */
