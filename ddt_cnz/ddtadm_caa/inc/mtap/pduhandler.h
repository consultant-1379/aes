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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_PDUHANDLER_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_PDUHANDLER_H_

#include "slidingwindow.h"
#include <unistd.h>
#include <stdint.h>
#include <boost/shared_ptr.hpp>

#ifndef CUTE_TEST
	#include <ace/config-macros.h>
#endif

namespace mtap
{
	class PDU;
	class DataChannel;

	class PDUHandler
	{
 	 public:

	    //Constructor
		PDUHandler(DataChannel *dataChannel);

		//Destructor
		virtual ~PDUHandler();

		/** @brief
		 *
		 *	This method initializes the PDU by creating the slidingwindow object.
		 *
		 *  @return ERR_NO_ERROR on success otherwise appropriate error code.
		 *
		 */
		int initialize();

		/** @brief
		 *
		 *	This method is called to handle the mtap packet received.
		 *
		 *  @param fd: DSD session handle.
		 *
		 *  @param buf: Mtap buffer.
		 *
		 *  @param size: Size of mtap buffer.
		 *
		 *  @param outBuffer: Buffer to hold the response for request.
		 *
		 *  @param outBufferLength: Size of the buffer used for holding response.
		 *
		 *  @return ERR_NO_ERROR on success otherwise appropriate error code.
		 *
		 */
		int handleRequest(ACE_HANDLE fd, unsigned char *buf, ssize_t &size, unsigned char *&outBuffer, ssize_t &outBufferLength);

		/** @brief
		 *
		 *	This method is called to create the stop message for a PDU.
		 *
		 *  @return NONE.
		 *
		 */
		void getStopMessage(unsigned char *&outBuffer, ssize_t &outBufferLength);

		/** @brief
		 *
		 *	This method is used to fetch the data channel.
		 *
		 *  @return DataChannel handle.
		 *
		 */
		DataChannel* getDataChannel() const { return m_dataChannel; };

		/** @brief
		 *
		 *	This method is used to clear the data created during a mtap session.
		 *	This method destroys the sliding window object created during initialization.
		 *
		 *  @return NONE.
		 *
		 */
		void cleanSessionData(ACE_HANDLE fd);

 	 private:
		unsigned char getPrimitiveId (unsigned char* networkBuffer, ssize_t & networkBufferSize) const;
		int createPDU(unsigned char* networkBuffer, ssize_t & networkBufferSize);

		boost::shared_ptr<PDU> m_pdu;
		DataChannel *m_dataChannel;
		SlidingWindow m_window;
	};

} /* namespace mtap */

#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_PDUHANDLER_H_ */
