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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_PDU_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_PDU_H_

#include <boost/utility.hpp>

#include "protocol.h"

namespace mtap
{

	/*template <class RequestT, class ResponseT>*/
	class PDU : private boost::noncopyable
	{
 	public:
		/// Constructor
		inline PDU(unsigned char *buf, ssize_t size, const unsigned char primitiveId)
		: m_networkBuffer(buf), m_networkBufferSize(size), m_primitiveId(primitiveId), m_lastError(protocol::ERR_NO_ERROR)
		{};

		/// Constructor
		inline PDU(const unsigned char primitiveId)
		: m_networkBuffer(0), m_networkBufferSize(0), m_primitiveId(primitiveId), m_lastError(protocol::ERR_NO_ERROR)
		{};

		/// Destructor
		inline virtual ~PDU() {};

		/**
		 * @brief
		 * This method is called to fetch the mtap primitive.
		 *
		 * @return Primitive type.
		 *
		 */
		inline unsigned char getPrimitiveType() const { return m_primitiveId; };

		/**
		 * @brief
		 * This method is called to parse the mtap buffer received.
		 *
		 * @return NONE
		 *
		 */
		virtual void deserialize() = 0;

		/**
		 * @brief
		 * This method is called to create the response for the mtap packet parsed.
		 *
		 * @param char : The output buffer.
		 *
		 * @param ssize_t: Size of the output buffer.
		 *
		 * @return 0 on success otherwise appropriate error code.
		 */
		virtual int createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize) = 0;

		/**
		 * @brief
		 * This method is called to create the response for the mtap packet parsed.
		 *
		 * @param outBuffer: The output buffer to sent as response.
		 *
		 * @param outBufferSize : The size of the output buffer to be sent as response.
		 *
		 * @return 0 on success otherwise appropriate error code.
		 *
		 */
		inline int createResponse(uint8_t errorCode, unsigned char*& outBuffer, ssize_t& outBufferSize)
		{
			m_lastError = errorCode;
			return createResponse(outBuffer, outBufferSize);
		};

		/**
		 * @brief
		 * This method is called to fetch the record number.
		 *
		 * @return 0.
		 *
		 */
		inline virtual uint32_t getRecordNumber() const { return 0; };

		/**
		 * @brief
		 * This method is called to fetch the file number.
		 *
		 * @return 0.
		 *
		 */
		inline virtual unsigned char getFileNumber() const { return 0; };

		/**
		 * @brief
		 * This method is called to fetch the data present in a record.
		 *
		 * @param dataBuffer: Buffer to hold the record data.
		 *
		 * @param dataLength: Length of the data buffer.
		 *
		 * @return NONE.
		 *
		 */
		inline virtual void getRecordData(unsigned char*& dataBuffer, uint32_t& dataLength) { dataBuffer = NULL;  dataLength = 0U; }

 	protected:

		unsigned char *m_networkBuffer;

		ssize_t m_networkBufferSize;

		const unsigned char m_primitiveId;

		uint8_t m_lastError;


	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_PDU_H_ */
