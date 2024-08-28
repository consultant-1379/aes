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

#include "engine/blockpeer.h"
#include "engine/rpc.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include "common/configuration.h"
#else
	#include "stubs/macro_stub.h"
#endif

#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<errno.h>
#include	<netdb.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_BlockPeer)

#define BGW_RPC_PROG_NUMBER 	0x2df001f1

/*
void
bgw_cdrb_receiver_1(char *host)
{
	CLIENT *clnt;
	enum clnt_stat retval_1;
	lastTransactionAnswer result_1;
	char * lasttransaction_1_arg;
	enum clnt_stat retval_2;
	answerCode result_2;
	BlockInfo  put_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, BGW_CDRB_RECEIVER, A, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif

	retval_1 = lasttransaction_1(&lasttransaction_1_arg, &result_1, clnt);
	if (retval_1 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
	retval_2 = put_1(&put_1_arg, &result_2, clnt);
	if (retval_2 != RPC_SUCCESS) {
		clnt_perror (clnt, "call failed");
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif
}
*/

namespace engine
{

	namespace
	{
		const uint16_t PORTMAPPER_PORT = 111;
		const char TRANSPORT_PROTOCOL[] = "tcp";
		const long RPC_TIMEOUT = 10U;
		const unsigned int MAX_STREAMID_SIZE = 512;
		const char UnderLine = '_';
	}

	BlockPeer::BlockPeer(const operation::blockPeerInfo& data)
	: Peer(data.ipAddress, data.moDN, data.status, Peer::BLOCK_PEER),
	  m_data(data),
	  m_rootStreamId(),
	  m_currentStreamId(),
	  m_lastTransactionId(0U),
	  m_lastTransactionIds(),
	  m_rpcClientHandle(NULL)
	{
		//Build Stream ID, for example: tp115ap1a_VCHS1
		m_rootStreamId.assign(common::configuration_t::instance()->getClusterName());
		m_rootStreamId.push_back(UnderLine);
		m_rootStreamId.append(data.getDataSourceName());
	}

	BlockPeer::~BlockPeer()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	int BlockPeer::connect()
	{
		int result = remoteAddressCheck();
		if(common::errorCode::ERR_NO_ERRORS == result)
		{
			result = rpcConnect();
		}
		return result;
	}

	int BlockPeer::disconnect()
	{
		m_lastTransactionIds.clear();
		return rpcDisconnect();
	}

	int BlockPeer::restartConnection(const char* /*producerName*/, const char* /*remoteFileName*/)
	{
		//NOTE: restartConnection with parameters is only allowed for the file peer

		AES_DDT_TRACE_FUNCTION;
		int result = disconnect();
		if(common::errorCode::ERR_NO_ERRORS == result)
		{
			result = connect();
		}
		return result;
	}

	bool BlockPeer::isReadyForConnection()
	{
		bool result = (common::errorCode::ERR_NO_ERRORS == remoteAddressCheck());

		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Connection to peer %s",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), (result)? "SUCCEEDED": "FAILED");
		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Connection to peer %s",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), (result)? "SUCCEEDED": "FAILED");

		return result;
	}

	int BlockPeer::send(void* data, size_t dataLength, const char* sourceId)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(!m_rpcClientHandle)
		{
			// This code flow is executed when the Peer status switches from FAULTY to ACTIVE again
			// The Block Assembler dicrectly invokes 'pushData' / 'send'
			// On the contrary at startup the Data Sink uses the connect/disconnect to identify the ACTIVE peer.
			result = rpcConnect();
		}

		// assemble the stream Id,  for example: tp115ap1a_VCHS1_CP1
		m_currentStreamId.assign(m_rootStreamId);
		m_currentStreamId.push_back(UnderLine);
		m_currentStreamId.append(sourceId);

		mapOfLastTransactionIds_t::iterator lastTransactionIdIterator = m_lastTransactionIds.find(m_currentStreamId);
		if(m_lastTransactionIds.end() != lastTransactionIdIterator)
		{
			// last transaction Id already got from remote side
			m_lastTransactionId = lastTransactionIdIterator->second;
		}
		else
		{
			// gets the last transaction Id from remote side
			result = rpcGetLast();
			if(common::errorCode::ERR_NO_ERRORS != result)
			{
				return result;
			}
			// save the info into the map
			m_lastTransactionIds[m_currentStreamId] = m_lastTransactionId;
		}

		AES_DDT_TRACE_MESSAGE("[%s::%s@%s] StreamId:<%s> last transaction id:<%u>",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), m_currentStreamId.c_str(), m_lastTransactionId);

		result = rpcSend(data, dataLength);

		if(common::errorCode::ERR_NO_ERRORS == result)
		{
			// Update the last transaction Id
			m_lastTransactionIds[m_currentStreamId] = m_lastTransactionId;
		}

		return result;
	}

	int BlockPeer::remoteAddressCheck()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		int	sockfd = -1, rc;
		//---------------------------------------------------------------
		// Resolve Server Address
		addrinfo hints;
		memset(&hints, 0, sizeof(addrinfo));
		//hints.ai_flags = AI_ALL;
		//hints.ai_family = PF_UNSPEC;
		hints.ai_protocol = IPPROTO_TCP;
		addrinfo* pResult = NULL;
		u_short port = PORTMAPPER_PORT;
		struct in_addr inaddr;
		struct in6_addr in6addr;
		const char *host = m_ipAddress.c_str();

		/********************************************************************/
		/* Check if we were provided the address of the server using        */
		/* inet_pton() to convert the text form of the address to binary    */
		/* form.													        */
		/********************************************************************/
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] BEFORE INET_PTON",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

		rc = inet_pton(AF_INET, host, &inaddr);
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] INET_PTON IPV4 rc = %d",
				m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),rc);
		if (rc == 1)    /* valid IPv4 text address? */
		{
			hints.ai_family = AF_INET;
			hints.ai_flags |= AI_NUMERICHOST;
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] connecting to IPv4 address %s:%u",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
		}
		else
		{
			rc = inet_pton(AF_INET6, host, &in6addr);
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] INET_PTON IPV6 rc = %d",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(),rc);
			if (rc == 1) /* valid IPv6 text address? */
			{

				hints.ai_family = AF_INET6;
				hints.ai_flags |= AI_NUMERICHOST;
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] connecting to IPv6 address %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
			}
		}

		int errcode = getaddrinfo(host, NULL, &hints, &pResult);

		if(0 == errcode && pResult!=NULL)
		{
			if(pResult->ai_family == AF_INET)
			{
				// Print IPv4 Server Address
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] connecting to IPv4 address %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] connecting to %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);

				//-------------------------------------------------------------------
				// Create Client Socket
				if( (sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) > 0)
				{
					//------------------------------------------------------------
					// Set Server Info
					struct sockaddr_in server_address;
					memset(&server_address, 0, sizeof(server_address));
					server_address.sin_family      = AF_INET;
					server_address.sin_port        = htons(port);

					server_address.sin_addr.s_addr = *((uint32_t*) & (((sockaddr_in*)pResult->ai_addr)->sin_addr));

					//------------------------------------------------------------
					//Set timeout on socket
					struct timeval timeout;
					timeout.tv_sec = RPC_TIMEOUT;
					timeout.tv_usec = 0;

					if((setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) == 0) &&
							(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == 0))
					{
						//--------------------------------------------------------------
						//Connect
						if (::connect(sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) == 0)
						{
							//Success: the connection works
							AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Port mapper reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Port mapper reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
						}
						else
						{
							//Error
							AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] address check failed! Cannot connect to %s",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host);

							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: address check failed! Cannot connect to %s - errno: %d",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, errno);

							result = common::errorCode::ERR_API_CALL;
						}
					}//setsockopt
					else
					{
						//Error
						AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] cannot set socket option!",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: cannot set socket option - errno: %d",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

						result = common::errorCode::ERR_API_CALL;
					}
				//------------------------------------------------------------------
				//Delete socket
				close(sockfd);
				}//sockfd
				else
				{
					//Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] cannot create IPv4 socket!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: cannot create IPv4 socket - errno: %d",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

					result = common::errorCode::ERR_API_CALL;
				}
			}//AF_INET
			else if (pResult->ai_family == AF_INET6)
			{
				// Print IPv6 Server Address
				AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] connecting to IPv6 address %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] connecting to %s:%u",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);

				//-------------------------------------------------------------------
				// Create Client Socket
				if( (sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP)) > 0)
				{
					//------------------------------------------------------------
					// Set Server Info
					struct sockaddr_in6 server_address;
					memset(&server_address, 0, sizeof(server_address));
					//server_address.sin6_addr.s6_addr = *((uint32_t*) & (((sockaddr_in*)pResult->ai_addr)->sin_addr));
					inet_pton(AF_INET6,host,&server_address.sin6_addr);
					server_address.sin6_family      = AF_INET6;
					server_address.sin6_port        = htons(port);

					//------------------------------------------------------------
					//Set timeout on socket
					struct timeval timeout;
					timeout.tv_sec = RPC_TIMEOUT;
					timeout.tv_usec = 0;

					if((setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) == 0) &&
							(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == 0))
					{
						//--------------------------------------------------------------
						//Connect
						if (::connect(sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) == 0)
						{
							//Success: the connection works
							AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Port mapper reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Port mapper reachable on %s:%u",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, port);
						}
						else
						{
							//Error
							AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] address check failed! Cannot connect to %s",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host);

							AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: address check failed! Cannot connect to %s - errno: %d",
									m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), host, errno);

							result = common::errorCode::ERR_API_CALL;
						}
					}//setsockopt
					else
					{
						//Error
						AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] cannot set socket option!",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

						AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: cannot set socket option - errno: %d",
								m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

						result = common::errorCode::ERR_API_CALL;
					}
				//------------------------------------------------------------------
				//Delete socket
				close(sockfd);
				}//sockfd
				else
				{
					//Error
					AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] cannot create IPv6 socket!",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());

					AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: cannot create IPv6 socket - errno: %d",
							m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno);

					result = common::errorCode::ERR_API_CALL;
				}

			}//AF_INET6
			else
			{
				AES_DDT_LOG(LOG_LEVEL_DEBUG,"remoteAddressCheck(): Unknown family address");
				AES_DDT_TRACE_MESSAGE("remoteAddressCheck(): Unknown family address");
				result = common::errorCode::ERR_API_CALL;
			}//Else if no AF_INET/AF_INET6
		freeaddrinfo(pResult);
		}//errocode //getaddrinfo failure
		else
		{
			//Error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] getaddrinfo error: %s",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), gai_strerror(errcode));

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: getaddrinfo error: %s - errno: %d",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), gai_strerror(errcode), errno);

			result = common::errorCode::ERR_API_CALL;
		}

		return result;
	}

	int BlockPeer::rpcConnect()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(m_rpcClientHandle)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] RPC client already created towards IP:<%s>",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), m_ipAddress.c_str());

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] RPC client already created towards IP:<%s>",
					m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), m_ipAddress.c_str());

			return result;
		}

		m_rpcClientHandle = clnt_create(m_ipAddress.c_str(), BGW_CDRB_RECEIVER, A, TRANSPORT_PROTOCOL);

		if(0 == m_rpcClientHandle)
		{
			//Write error indicating why an RPC call failed
			char* errorMsg = clnt_spcreateerror(m_ipAddress.c_str());

			//Error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] Cannot create RPC client handle, error:<%s>",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errorMsg);
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot create RPC client handle - errno: %d, , error:<%s>",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), errno, errorMsg);

			result = common::errorCode::ERR_API_CALL;
			return result;
		}

		struct timeval timeout;
		timeout.tv_usec = 0;
		timeout.tv_sec = RPC_TIMEOUT;

		//the timeout parameter passed to clnt_call() will be ignored in all future calls
		if(!clnt_control(m_rpcClientHandle, CLSET_TIMEOUT, reinterpret_cast<char*>(&timeout)) )
		{
			char* errorMsg = clnt_sperror(m_rpcClientHandle, "cannot set timeout");
			//Error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] Cannot set timeout to %ld seconds, error:<%s>",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), timeout.tv_sec, errorMsg);

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Cannot set timeout to %ld seconds - errno: %d, error:<%s>",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), timeout.tv_sec, errno, errorMsg);

			//clnt_destroy will close the socket also.
			clnt_destroy(m_rpcClientHandle);

			//Use of  m_rpcClientHandle is  undefined after  calling clnt_destroy
			m_rpcClientHandle = 0;

			result = common::errorCode::ERR_API_CALL;
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] timeout set to %ld seconds",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), timeout.tv_sec);
		}

		return result;
	}

	int BlockPeer::rpcGetLast()
	{
		int result = common::errorCode::ERR_NO_ERRORS;
		if(!m_rpcClientHandle)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] RPC client is NULL.", m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] RPC client is NULL.", m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			result = common::errorCode::ERR_API_CALL;
			return result;
		}

		lastTransactionAnswer getLastAnswer;
		char streamId[MAX_STREAMID_SIZE] = {0};
		m_currentStreamId.copy(streamId, MAX_STREAMID_SIZE-1);
		char* lasttransaction = streamId;

		clnt_stat api_result = lasttransaction_1(&lasttransaction, &getLastAnswer, m_rpcClientHandle);

		if(api_result == RPC_SUCCESS)
		{
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Get Last Succeeded <%d>. Answer Code: <%s>. LastTransactionID: <%lu>",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), api_result,
								  (getLastAnswer.code == HANDLED ? "HANDLED" : "UNHANDLED"), getLastAnswer.lastTransactionID);

			printf("[%s::%s@%s] Get Last Succeeded <%d>. Answer Code: <%s>. LastTransactionID: <%lu>\n",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), api_result,
								  (getLastAnswer.code == HANDLED ? "HANDLED" : "UNHANDLED"), getLastAnswer.lastTransactionID);

			result = (HANDLED == getLastAnswer.code) ? common::errorCode::ERR_NO_ERRORS : common::errorCode::ERR_RPC_GETLAST_UNHANDLED;

			if(common::errorCode::ERR_NO_ERRORS == result)
			{
				m_lastTransactionId = getLastAnswer.lastTransactionID;
			}
		}
		else
		{
			char* errorMsg = clnt_sperror(m_rpcClientHandle, "call failed");

			//Error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] Get Last failed <%d>, error:<%s>",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), api_result, errorMsg);

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] ERROR: Get Last failed <%d>, errno: <%d>, error:<%s>",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), api_result, errno, errorMsg);

			result = common::errorCode::ERR_API_CALL;
		}

		return result;
	}

	int BlockPeer::rpcSend(void* buffer, unsigned int bufferLength)
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(!m_rpcClientHandle)
		{
			AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] RPC client is NULL.",	m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] RPC client is NULL.", m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName());
			result = common::errorCode::ERR_API_CALL;
			return result;
		}

		BlockInfo  blockDataToPut;
		blockDataToPut.theBlockData.theBlockData_val = reinterpret_cast<unsigned char*>(buffer);
		blockDataToPut.theBlockData.theBlockData_len = bufferLength;

		uint32_t transactionId = m_lastTransactionId + 1U;
		blockDataToPut.theBlockID = transactionId;

		char streamId[MAX_STREAMID_SIZE] = {0};
		m_currentStreamId.copy(streamId, MAX_STREAMID_SIZE-1);
		blockDataToPut.theSourceID = streamId;

		answerCode putAnswer;
		enum clnt_stat api_result;

		api_result = put_1(&blockDataToPut, &putAnswer, m_rpcClientHandle);

		if(api_result != RPC_SUCCESS)
		{
			char* errorMsg = clnt_sperror(m_rpcClientHandle, "call failed");
			//Error
			AES_DDT_LOG_ERRNO(errno, LOG_LEVEL_ERROR, "[%s::%s@%s] RPC put failed. Record Id: <%lu>, error:<%d, %s>",
							  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), blockDataToPut.theBlockID, api_result, errorMsg);

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] RPC put failed. Record Id: <%lu> - errno: %d, error:<%d, %s>",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), blockDataToPut.theBlockID, errno, api_result, errorMsg);

			result = common::errorCode::ERR_API_CALL;
		}
		else
		{
			printf("RPC PUT <%lu> %s: %s\n", blockDataToPut.theBlockID, blockDataToPut.theSourceID, (  HANDLED == putAnswer ? "HANDLED" : "UNHANDLED"));

			if(HANDLED == putAnswer)
			{
				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] put record <%lu>",
									  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), blockDataToPut.theBlockID);
				m_lastTransactionId += 1;

				result = common::errorCode::ERR_NO_ERRORS;
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "[%s::%s@%s] RPC ERROR: put record <%lu> UNHANDLED on stream ID <%s> ",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), blockDataToPut.theBlockID, blockDataToPut.theSourceID);

				AES_DDT_TRACE_MESSAGE("[%s::%s@%s] RPC ERROR: put record <%lul> UNHANDLED on stream ID <%s> ",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), blockDataToPut.theBlockID, blockDataToPut.theSourceID);

				result = common::errorCode::ERR_RPC_SEND_UNHANDLED;
				// remove last transaction ID from internal map, next send will get it from the remote server
				m_lastTransactionIds.erase(m_currentStreamId);
			}
		}
		return result;
	}

	int BlockPeer::rpcDisconnect()
	{
		int result = common::errorCode::ERR_NO_ERRORS;

		if(m_rpcClientHandle)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s::%s@%s] Disconnect from %s",
						m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), m_ipAddress.c_str());

			AES_DDT_TRACE_MESSAGE("[%s::%s@%s] Disconnect from %s",
								  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), m_ipAddress.c_str());

			printf("[%s::%s@%s] Disconnect from %s\n",
											  m_data.getDataSourceName(), m_data.getDataSinkName(), m_data.getName(), m_ipAddress.c_str());

			//clnt_destroy will close the socket also.
			clnt_destroy(m_rpcClientHandle);

			//Use of  m_rpcClientHandle is  undefined after  calling clnt_destroy
			m_rpcClientHandle = 0;
		}

		return result;
	}

} /* namespace engine */
