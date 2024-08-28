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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_GETPARAMETER_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_GETPARAMETER_H_

#include <list>
#include "pdu.h"

namespace mtap
{
	class PDUHandler;

	class GetParameter : public PDU
	{
 	public:

		//Constructor
		GetParameter(PDUHandler* myHandler, unsigned char *rawBuffer, ssize_t bufferSize);

		//Destructor
		virtual ~GetParameter();

		/**
		 * @brief
		 * This method is called to parse the mtap buffer received.
		 *
		 * @return NONE
		 *
		 */
		virtual void deserialize();

		/**
		 * @brief
		 * This method is called to create the response for the mtap packet parsed.
		 *
		 * @param char: The output buffer to sent as response.
		 *
		 * @param ssize_t : The size of the output buffer to be sent as response.
		 *
		 * @return 0 on success otherwise appropriate error code.
		 *
		 */
		virtual int createResponse(unsigned char *&outBuffer, ssize_t &outBufferSize);

 	private:

		//-----------------
		// PDU Checkers
		//-----------------
		protocol::ErrorCode checkSize();

		PDUHandler* m_handler;

		unsigned char m_numberOfParameters;

		typedef std::pair<unsigned char, uint32_t> parameterPair_t;

		std::list<parameterPair_t> m_parameters;

	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_GETPARAMETER_H_ */
