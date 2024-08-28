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
#ifndef DDTADM_CAA_INC_SIGNALSEVENTHANDLER_H_
#define DDTADM_CAA_INC_SIGNALSEVENTHANDLER_H_

#include <ace/Event_Handler.h>

/**
 * @class AES_DDT_SignalsEventHandler
 *
 * @brief Implementation of a @c ACE_Event_Handler abstract interface for handling signal events.
 *
 */
class SignalsEventHandler : public ACE_Event_Handler
{
 public:

	///  Constructor.
	inline explicit SignalsEventHandler()
	: ACE_Event_Handler() {}

	/// Destructor.
	virtual ~SignalsEventHandler() { }

	/**
	 *  @brief Callback to handle a received system signal.
	 *
	 *  @param signum system signal identifier.
	 */
	virtual int handle_signal (int signum, siginfo_t * = 0, ucontext_t * = 0);

 private:

	//  Disallow these operations.
	SignalsEventHandler &operator=(const SignalsEventHandler &);
	SignalsEventHandler(const SignalsEventHandler &);

};

#endif /* DDTADM_CAA_INC_SIGNALSEVENTHANDLER_H_ */
