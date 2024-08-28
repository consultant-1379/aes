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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_SYNCHRONIZE_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_SYNCHRONIZE_H_

#include "pdu.h"

#ifdef CUTE_TEST
	#include "mtap/SlidingWindowTest.h"
#endif

namespace mtap
{

	class Synchronize : public PDU
	{
 	public:
#ifdef CUTE_TEST
		friend SlidingWindowTest;
#endif
		//Constructor
		Synchronize(unsigned char* rawBuffer, const ssize_t& bufferSize);

		//Destructor
		virtual ~Synchronize();

		/**
		 * @brief
		 * This method is called to parse the SYNCHRONIZE PDU.
		 *
		 * @return NONE
		 *
		 */
		virtual void deserialize();

		/**
		 * @brief
		 * This method is called to create the response for the SYNCHRONIZE PDU parsed.
		 *
		 * @param char: The output buffer to sent as response.
		 *
		 * @param ssize_t: The size of the output buffer to be sent as response.
		 *
		 * @return 0 on success otherwise appropriate error code.
		 *
		 */
		virtual int createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize);

		/**
		 * @brief
		 * This method is called to fetch the record number.
		 *
		 * @return Record number.
		 *
		 */
		virtual uint32_t getRecordNumber() const { return m_recordNumber; };

		/**
		 * @brief
		 * This method is called to assign the set the last stored record number.
		 *
		 * @param uint32_t: Record number.
		 *
		 * @return NONE.
		 *
		 */
		void setLastStored(const uint32_t& recordNumber) { m_lastStored = recordNumber; };

 	private:
		//-----------------
		// PDU Checkers
		//-----------------
		protocol::ErrorCode checkSize() const;

		uint32_t m_recordNumber;

		unsigned char m_fileNumber;

		uint32_t m_lastStored;

	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_SYNCHRONIZE_H_ */
