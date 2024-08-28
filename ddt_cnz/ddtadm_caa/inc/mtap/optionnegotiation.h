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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_OPTIONNEGOTIATION_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_OPTIONNEGOTIATION_H_

#include "pdu.h"

namespace mtap
{


	class OptionNegotiation : public PDU
	{
 	public:

		//Constructor
		OptionNegotiation (unsigned char *buf, ssize_t size);

		//Destructor
		virtual ~OptionNegotiation();

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

		protocol::ErrorCode checkSize() const;
		protocol::ErrorCode checkVersion() const;
	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_OPTIONNEGOTIATION_H_ */
