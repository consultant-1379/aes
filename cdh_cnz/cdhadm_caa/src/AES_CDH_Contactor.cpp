#include <AES_CDH_Contactor.h>
#include <aes_gcc_variable.h>
#include <aes_gcc_util.h>
#include <string>
#include <rpc/rpc.h>
#include <aes_cdh_common.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

AES_CDH_TRACE_DEFINE(AES_CDH_Contactor)

namespace
{
	const unsigned short PORTMAPPER_PORT = 111;
};

ACE_Thread_Mutex AES_CDH_Contactor::Clnt_mutex;

AES_CDH_Contactor::AES_CDH_Contactor(BGWRPC* bp,char* h,unsigned long b):
		m_HostAddress(0),
		m_bgwP(bp),
		m_bgwNR(b),
		m_clientP(0)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	int sLen = strlen(h);
	m_HostAddress = new char[sLen+1];
	strcpy(m_HostAddress,h);
	this->SocketObjectCreation(); 
}

void AES_CDH_Contactor::SocketObjectCreation()
{
	CLIENT* hclient = NULL;
	// Calling RPC System calls for connection
	AES_CDH_TRACE_MESSAGE("Creating RPC client --> m_HostAddress:<%s>, m_bgwNR:<%lu>", string(m_HostAddress).c_str(), m_bgwNR);

	//Check the hostAddress is valid or not using a ping before calling clnt_create()
	//bool result = AES_CDH_Paths::instance()->isRemoteHostValid(m_HostAddress);

	std::string i_protocol("tcp");

	if (hostAddressCheck())
	{
		Clnt_mutex.acquire();
		hclient = clnt_create(m_HostAddress,m_bgwNR,A,const_cast<char *>(i_protocol.c_str()));
		Clnt_mutex.release();

		if (!hclient)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "RPC client == NULL for host <%s>",string(m_HostAddress).c_str());
			AES_CDH_TRACE_MESSAGE(" RPC client == NULL for host <%s>",string(m_HostAddress).c_str());
		}
		else
		{
			AES_CDH_TRACE_MESSAGE(" RPC client successfully created for host : %s",string(m_HostAddress).c_str());
		}
	}
	else
	{
		AES_CDH_LOG(LOG_LEVEL_FATAL, " Host not reachable : %s",string(m_HostAddress).c_str());
		AES_CDH_TRACE_MESSAGE(" Host not reachable : %s",string(m_HostAddress).c_str());
	}

	m_clientP = hclient;
}

CLIENT* AES_CDH_Contactor::getResult()
{
	AES_CDH_TRACE_MESSAGE(" Result code fetched!");
	return m_clientP;
}

AES_CDH_Contactor::~AES_CDH_Contactor()
{
	delete [] m_HostAddress;
	AES_CDH_TRACE_MESSAGE("Object destroyed");
}

bool AES_CDH_Contactor::hostAddressCheck()
{
	bool rv = true;
	int	sockfd = -1;
	struct sockaddr* server = NULL;
	size_t addr_size;

	if(m_HostAddress == NULL)
	{
		AES_CDH_TRACE_MESSAGE("m_HostAddress is NULL! ERROR!");
		rv = false;
		return rv;
	}
	const string strIpAddress(m_HostAddress);
	bool isIPv6 = AES_CDH_Common_Util::isIPv6Address(strIpAddress);
	u_short port = PORTMAPPER_PORT;
	if(isIPv6)
	{
		AES_CDH_TRACE_MESSAGE("Creating IPv6 SOCKET ADDRESS STRUCT");
		struct in6_addr ipv6_addr;
		int res = inet_pton(AF_INET6,strIpAddress.c_str(),&ipv6_addr);
		if(res != 1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "inet_pton() FAILED(res == %d)! Converting IPv6[%s] string to struct in6_addr FAILED!!!", res, strIpAddress.c_str());
			rv = false;
		}
		else
		{
			struct sockaddr_in6* server_ipv6 = new struct sockaddr_in6;
			ACE_OS::memset(server_ipv6,'0',sizeof(struct sockaddr_in6));
			server_ipv6->sin6_family = AF_INET6;
			server_ipv6->sin6_addr = ipv6_addr;
			server_ipv6->sin6_port = htons(port);
			server = (struct sockaddr*)server_ipv6;
		}
	}
	else
	{
		AES_CDH_TRACE_MESSAGE("Creating IPv4 SOCKET ADDRESS STRUCT");
		struct in_addr ipv4_addr;
		int res = inet_pton(AF_INET,strIpAddress.c_str(),&ipv4_addr);
		if(res != 1)
		{
			AES_CDH_LOG(LOG_LEVEL_ERROR, "inet_pton() FAILED(res == %d)! Converting IPv4[%s] string to struct in_addr FAILED!!!", res, strIpAddress.c_str());
			rv = false;
		}
		else
		{
			struct sockaddr_in* server_ipv4 = new struct sockaddr_in;
			ACE_OS::memset(server_ipv4,'0',sizeof(struct sockaddr_in));
			server_ipv4->sin_family = AF_INET;
			server_ipv4->sin_addr = ipv4_addr;
			server_ipv4->sin_port = htons(port);
			server = (struct sockaddr*)server_ipv4;
		}
	}

	if(rv)
	{
		if(isIPv6)
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Creating IPv6 SOCKET ");
			sockfd = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
			addr_size = sizeof(struct sockaddr_in6);
		}
		else
		{
			AES_CDH_LOG(LOG_LEVEL_INFO, "Creating IPv4 SOCKET ");
			sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			addr_size = sizeof(struct sockaddr_in);
		}

		//-------------------------------------------------------------------
		// Create Client Socket
		if (-1 != sockfd)
		{
			//------------------------------------------------------------
			//Set timeout on socket
			struct timeval timeout;
			timeout.tv_sec = 2;
			timeout.tv_usec = 0;

			if ((setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,sizeof(timeout)) == 0) &&
					(setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == 0))
			{
				//--------------------------------------------------------------
				//Connect
				if (connect(sockfd, server, addr_size) == 0)
				{
					AES_CDH_TRACE_MESSAGE("Portmapper reachable on %s:%u",strIpAddress.c_str(), port);
					//fprintf(stdout, "[%s@%d] Portmapper reachable on %s:%u", __FILE__, __LINE__, host, port);
				}
				else
				{
					//Error
					AES_CDH_LOG(LOG_LEVEL_ERROR, "addresscheck failed! Connect error to %s errno: %d",strIpAddress.c_str(), errno);
					AES_CDH_TRACE_MESSAGE("addresscheck failed! Connect error to %s errno: %d",strIpAddress.c_str(), errno);
					fprintf(stderr, "[%s@%d] addresscheck failed! Connect error to %s errno: %d", __FILE__, __LINE__, strIpAddress.c_str(), errno);
					rv = false;
				}
			}
			else
			{
				//Error
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error: cannot set socket option. errno: %d", errno);
				AES_CDH_TRACE_MESSAGE("Error: cannot set socket option. errno: %d", errno);
				fprintf(stderr, "[%s@%d] Error: cannot set socket option. errno: %d", __FILE__, __LINE__, errno);
				rv = false;
			}
			//------------------------------------------------------------------
			//Delete socket
			close(sockfd);
		}
		else
		{
			//Error
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Error: Cannot Create Socket. errno: %d", errno);
			AES_CDH_TRACE_MESSAGE("Error: Cannot Create Socket. errno: %d", errno);
			fprintf(stderr, "[%s@%d] Error: Cannot Create Socket. errno: %d", __FILE__, __LINE__, errno);
			rv = false;
		}
	}
	if(server)
		delete server;
	return rv;
}
