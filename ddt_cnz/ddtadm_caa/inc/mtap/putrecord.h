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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_PUTRECORD_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_PUTRECORD_H_

#include "pdu.h"

namespace mtap
{

	class PutRecord : public PDU
	{
 	public:

		//Constructor
		PutRecord(unsigned char* rawBuffer, const ssize_t& bufferSize);

		//Destructor
		virtual ~PutRecord();

		/**
		 * @brief
		 * This method is called to parse the PUT_RECORD PDU.
		 *
		 * @return NONE
		 *
		 */
		virtual void deserialize();

		/**
		 * @brief
		 * This method is called to create the response for the PUT_RECPRD PDU parsed.
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
		 * This method is called to fetch the file number.
		 *
		 * @return File number.
		 *
		 */
		virtual unsigned char getFileNumber() const { return m_fileNumber; };

		/**
		 * @brief
		 * This method is called to fetch the data present in a record.
		 *
		 * @param char: Buffer to hold the record data.
		 *
		 * @param uint32_t: Length of the data buffer.
		 *
		 * @return NONE.
		 *
		 */
		virtual void getRecordData(unsigned char*& dataBuffer, uint32_t& dataLength);

 	private:
		//-----------------
		// PDU Checkers
		//-----------------
		protocol::ErrorCode checkSize() const;

		uint32_t m_recordNumber;
		unsigned char m_fileNumber;

	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_PUTRECORD_H_ */
