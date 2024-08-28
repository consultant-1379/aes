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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_SLIDINGWINDOW_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_SLIDINGWINDOW_H_

#include "protocol.h"
#include <boost/shared_ptr.hpp>
#include <vector>

#ifndef CUTE_TEST
	#include <ace/config-macros.h>
#else
	#include "stubs/ACE_Task_Base_stub.h"
	#include "mtap/SlidingWindowTest.h"
#endif

namespace store
{
	class HandlerInterface;
}

namespace mtap
{

	class PDU;
	class PDUHandler;

	class SlidingWindow
	{
 	public:

#ifdef CUTE_TEST
		friend SlidingWindowTest;
#endif
		//Constructor
		SlidingWindow(PDUHandler* pduHandler);

		//Destructor
		virtual ~SlidingWindow();

		/**
		 * @brief
		 * This method is called to initialize the sliding window object.
		 * This method is called during the PDUHandler initialization.
		 *
		 * @return ERR_NO_ERROR on success otherwise appropriate error code.
		 *
		 */
		int initialize();

		/**
		 * @brief
		 * This method is called to add a PDU to a sliding window object.
		 *
		 * @param PDU: PDU to be stored to sliding window.
		 *
		 * @param ACE_HANDLE : DSD session handle.
		 *
		 * @return ERR_NO_ERROR on success otherwise appropriate error code.
		 *
		 */
		protocol::ErrorCode add(boost::shared_ptr<PDU> pdu, ACE_HANDLE fd);

		/**
		 * @brief
		 * This method is when unlink/unlinkAll primitive is received.
		 *
		 * @param bool: boolean parameter to verify whether primitive is unlinkAll or not.
		 *
		 * @param ACE_HANDLE : DSD session handle.
		 *
		 * @return ERR_NO_ERROR on success otherwise appropriate error code.
		 *
		 */
		protocol::ErrorCode unlink(bool unlinkAll = false);

		/**
		 * @brief
		 * This method is called to synchronize the record number between AP & CP.
		 * This method is called when SYCHRONIZE request is received from CP.
		 *
		 * @param PDU: Synchronize PDU.
		 *
		 * @param ACE_HANDLE : DSD session handle.
		 *
		 * @return ERR_NO_ERROR on success otherwise appropriate error code.
		 *
		 */
		protocol::ErrorCode synchronize(boost::shared_ptr<PDU> pdu, ACE_HANDLE fd);

		/**
		 * @brief
		 * This method is called to clear the sliding window.
		 *
		 * @param ACE_HANDLE : DSD session handle.
		 *
		 * @return NONE.
		 *
		 */
		void clean(const ACE_HANDLE& fd);

 	private:

		//----------------
		// Methods
		//----------------
		void store();

		protocol::ErrorCode writeData(const uint32_t& index);

		void print_table(const char * TAG, uint32_t pduRecordNumber);

		protocol::ErrorCode checkRange(const uint32_t &pduRecordNumber) const;

		//----------------
		// Attributes
		//----------------

		PDUHandler* m_pduHandler;

		boost::shared_ptr<store::HandlerInterface> m_writer;

		typedef std::pair<boost::shared_ptr<PDU>, ACE_HANDLE> putRecordInfo_t;
		std::vector<putRecordInfo_t> m_sortedPutRecords;

		// MTAP message number (0-5999) of the first message in the vector
		uint32_t m_baseOffsetRecNr;
	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_SLIDINGWINDOW_H_ */
