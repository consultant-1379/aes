/*
 * scheduler_stub.h
 *
 *  Created on: May 25, 2015
 *      Author: xnadnar
 */

#ifndef INC_SCHEDULER_STUB_H_
#define INC_SCHEDULER_STUB_H_
#include "ACE_Activation_Queue_stub.h"
#include "ACE_Method_Request_stub.h"
#include "ACE_Task_Base_stub.h"

class ACE_Method_Request;

	/**
	 * @class AES_DDT_OperationScheduler
	 *
	 * @brief A scheduler class derived from @c ACE_Task_Base.
	 *
	 * Maintains a priority-ordered queue of operation objects.
	 * Subsequently removes each operation request and invokes its @c call() method.
	 *
	 */
	class AES_DDT_OperationScheduler : public ACE_Task_Base
	{
	 public:

		/// Constructor.
		inline AES_DDT_OperationScheduler() : ACE_Task_Base(), m_ActivationQueue() {}

		/// Destructor.
		virtual ~AES_DDT_OperationScheduler(){}

		/**
		 *	@brief	Scheduler function thread.
		 */
		virtual int svc(void){return 0;}

		/**
		 * @brief	Activates the scheduler thread.
		*/
		virtual int open(void *args = 0){ return 0;}

		/**
		 *	@brief	Initializes the scheduler task and prepare it to run as thread.
		 */
		virtual int start(){return 0;}

		/**
		 * @brief	Enqueues a @a Shutdown operation into the scheduler and
		 * waits the thread termination.
		 */
		virtual int stop(){return 0;}

		/**
		 * @brief	This method enqueue a command in the queue
		 */
		inline int enqueue(operation::OperationBase * cmdRequest)
		{
			return m_ActivationQueue.enqueue(cmdRequest);
		}

	 private:

		// = Disallow these operations.
		AES_DDT_OperationScheduler &operator=(const AES_DDT_OperationScheduler &);
		AES_DDT_OperationScheduler(const AES_DDT_OperationScheduler &);

	 private:

		/**
		 * @brief	Queue of operation to execute.
		 *
		 * @sa ACE_Activation_Queue.
		 */
		ACE_Activation_Queue m_ActivationQueue;

	};
#endif /* INC_SCHEDULER_STUB_H_ */
