#ifndef DDTADM_CAA_INC_STORE_CHECKPOINT_OBSERVER_H_
#define DDTADM_CAA_INC_STORE_CHECKPOINT_OBSERVER_H_

namespace store
{
	/**
	 * @class Observer
	 *
	 * @brief
	 * This class provides an interface for store observers
	 *
	 */
	class Observer
	{
	 public:
		///  Constructor.
		Observer() { }

		///  Destructor.
		virtual ~Observer() { }

		/** @brief
		 *
		 *	This is invoked by the Observed Subject to notify the observer about a new producer.
		 *
		 *  @param producerName: the new producer.
		 *
		 *  @return void
		 *
		 *	@remarks Remarks
		 */
		virtual void update(const std::string& producerName) = 0;

	};
} /* namespace store */

#endif /* DDTADM_CAA_INC_STORE_CHECKPOINT_OBSERVER_H_ */
