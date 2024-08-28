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
#include "mtap/pduhandler.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "mtap/datachannel.h"
	#include "mtap/stop.h"
#else
	#include "stubs/macro_stub.h"
	#include "stubs/mtap_stubs.h"
#endif

#include "mtap/getparameter.h"
#include "mtap/optionnegotiation.h"
#include "mtap/protocol.h"
#include "mtap/putrecord.h"
#include "mtap/synchronize.h"
#include "mtap/unlinkallfiles.h"
#include "mtap/unlinkfile.h"
#include "common/programconstants.h"
#include "common/macros.h"
#include <boost/make_shared.hpp>


AES_DDT_TRACE_DEFINE(AES_DDT_MTAP_PDUHandler)

namespace mtap
{

	static void hex_print(const void* pv, size_t len)
	{
		printf("------------------------------------------------------\n");
		printf("------------------------------------------------------\n");

	    const unsigned char * p = (const unsigned char*)pv;
	    if (NULL == pv)
	    {
	        printf("NULL");
	    }
	    else
	    {
	    	size_t i = 0;
	    	for (; i < len; ++i)
	    	{
	    		if ((i > 0) && (i%16 == 0))
	    		{
	    			printf("\n");
	    		}
	    		else if (i > 0)
	    		{
	    			printf(":");
	    		}

	    		printf("%02X", *p++);
	    	}
	    }

	    printf("\n");
	}

	PDUHandler::PDUHandler(DataChannel *dataChannel)
	: m_pdu(),
	  m_dataChannel(dataChannel),
	  m_window(this)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	PDUHandler::~PDUHandler()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int PDUHandler::initialize()
	{
		AES_DDT_TRACE_FUNCTION;
		int result = m_window.initialize();
		return result;
	}

	int PDUHandler::createPDU(unsigned char* networkBuffer, ssize_t & networkBufferSize)
	{
		int error = protocol::ERR_NO_ERROR;
		int32_t id = getPrimitiveId(networkBuffer, networkBufferSize);
		switch (id)
		{
			case protocol::primitive::DO_WILL:  //9
			{
				AES_DDT_TRACE_MESSAGE("##### Message Received: DO_WILL #####");
				m_pdu = boost::make_shared<OptionNegotiation>(networkBuffer, networkBufferSize);
			}
			break;

			case protocol::primitive::GET_PARAMETER:  //5
			{
				AES_DDT_TRACE_MESSAGE("##### Message Received: GET_PARAMETER #####");
				m_pdu = boost::make_shared<GetParameter>(this, networkBuffer, networkBufferSize);
			}
			break;

			case protocol::primitive::UNLINK_ALL_FILES:  //3
			{
				AES_DDT_TRACE_MESSAGE("##### Message Received: UNLINK_ALL_FILES #####");
				m_pdu = boost::make_shared<UnlinkAllFiles>(networkBuffer, networkBufferSize);
			}
			break;

			case protocol::primitive::ECHO_CHECK:  //4
				AES_DDT_TRACE_MESSAGE("##### Message Received: ECHO CHECK #####");
				error = protocol::ERR_MALFORMED_PACKAGE;
				break;

			case protocol::primitive::SKIP:  //8
				AES_DDT_TRACE_MESSAGE("##### Message Received: SKIP #####");
				printf("\n\n\n\n\n------------------------------------");
				printf("##### Message Received: SKIP #####\n");
				printf("##### Message Received: SKIP #####\n");
				printf("##### Message Received: SKIP #####\n");
				printf("##### Message Received: SKIP #####\n");
				printf("##### Message Received: SKIP #####\n");
				printf("##### Message Received: SKIP #####\n");
				printf("----------------------------------------\n\n\n\n\n");
				break;
			case protocol::primitive::PUT_RECORD:  //1
			{
				AES_DDT_TRACE_MESSAGE("##### Message Received: PUT_RECORD #####");
				m_pdu = boost::make_shared<PutRecord>(networkBuffer, networkBufferSize);
			}
			break;

			case protocol::primitive::SYNCHRONIZE:  //6
			{
				AES_DDT_TRACE_MESSAGE("##### Message Received: SYNCHRONIZE #####");
				m_pdu = boost::make_shared<Synchronize>(networkBuffer, networkBufferSize);
			}
			break;

			case protocol::primitive::UNLINK_FILE:  //2
				AES_DDT_TRACE_MESSAGE("##### Message Received: UNLINK_FILE #####");
				m_pdu = boost::make_shared<UnlinkFile>(networkBuffer, networkBufferSize);
				break;

			case protocol::primitive::STOP:  //7
				AES_DDT_TRACE_MESSAGE("##### Message Received: STOP #####");
				error = protocol::ERR_MALFORMED_PACKAGE;
				break;

			case protocol::primitive::ERROR:  //0
				AES_DDT_TRACE_MESSAGE("##### Message Received: ERROR #####");
				error = protocol::ERR_MALFORMED_PACKAGE;
				break;

			case protocol::primitive::DO_NOT_WILL_NOT:  //10
				AES_DDT_TRACE_MESSAGE("##### Message Received: DO_NOT_WILL_NOT #####");
				error = protocol::ERR_MALFORMED_PACKAGE;
				break;

			default:
				AES_DDT_TRACE_MESSAGE("##### Message Received: UNKNOWN PRIMITIVE <%d> #####", id);
				error = protocol::ERR_MALFORMED_PACKAGE;
		}
		return error;
	}

	int PDUHandler::handleRequest(ACE_HANDLE fd, unsigned char *inBuffer, ssize_t &inBufferSize, unsigned char *&outBuffer, ssize_t &outBufferSize)
	{
		AES_DDT_TRACE_FUNCTION;
		int error = protocol::ERR_NO_ERROR;
		outBufferSize = 0;

		if(inBuffer)
		{
			//AES_DDT_LOG_DUMP(inBuffer, inBufferSize, AES_DDT_DEFAULT_DUMP_SIZE);
			//hex_print(inBuffer, (inBufferSize > 16 ? 16 : inBufferSize));

			error = createPDU(inBuffer, inBufferSize);

			if(protocol::ERR_NO_ERROR == error)
			{
				m_pdu->deserialize();

				if(m_pdu->getPrimitiveType() == protocol::primitive::PUT_RECORD)
				{
					// ADD PDU TO THE TRANSMISSION WINDOW
					protocol::ErrorCode addResult = m_window.add(m_pdu, fd);

					if(protocol::ERR_NO_ERROR != addResult)
					{
						printf("PUT-RECORD ERROR - SENDING REPSONSE <%d>\n", addResult);

						// SEND OUT THE RESPONSE FOR THIS SPECIFIC PDU;
						error = m_pdu->createResponse(addResult, outBuffer, outBufferSize);
					}
				}
				else
				{
					protocol::ErrorCode operationResult = protocol::ERR_NO_ERROR;

					if(m_pdu->getPrimitiveType() == protocol::primitive::UNLINK_FILE)
					{
						bool unlinkAll = false;
						operationResult = m_window.unlink(unlinkAll);
					}
					else if(m_pdu->getPrimitiveType() == protocol::primitive::UNLINK_ALL_FILES)
					{
						// force current checkpoint closure
						bool unlinkAll = true;
						operationResult = m_window.unlink(unlinkAll);
					}
					else if(m_pdu->getPrimitiveType() == protocol::primitive::SYNCHRONIZE)
					{
						operationResult = m_window.synchronize(m_pdu, fd);
						m_dataChannel->closeOtherSessions(fd);
					}

					// OTHER PDUs expect an immediate reply to the sender
					error = m_pdu->createResponse(operationResult, outBuffer, outBufferSize);
				}
			}
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("ERROR: MALFORMED PACKAGE - RECEIVED NULL BUFFER");
			AES_DDT_LOG(LOG_LEVEL_ERROR, "MALFORMED PACKAGE - RECEIVED NULL BUFFER");
			error = protocol::ERR_MALFORMED_PACKAGE;
		}

		return error;
	}

	void PDUHandler::getStopMessage(unsigned char *&outBuffer, ssize_t &outBufferLength)
	{
		AES_DDT_TRACE_FUNCTION;
		outBufferLength = 0;

		Stop pdu;
		pdu.createResponse(outBuffer, outBufferLength);
	}

	void PDUHandler::cleanSessionData(ACE_HANDLE fd)
	{
		AES_DDT_TRACE_FUNCTION;
		m_window.clean(fd);
	}

	unsigned char PDUHandler::getPrimitiveId (unsigned char* networkBuffer, ssize_t & networkBufferSize) const
	{
		unsigned char primitive_id = mtap::protocol::primitive::ERROR;

		if (protocol::PRIMITIVE_ID_SIZE <= networkBufferSize)
		{
			primitive_id = networkBuffer[0]; //read as a PrimitiveIdentifier
		}

		return primitive_id;
	}



} /* namespace mtap */
