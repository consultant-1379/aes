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

#include "engine/peer.h"
#include "imm/objectmanagerhelper.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#endif


AES_DDT_TRACE_DEFINE(AES_DDT_Engine_Peer)

namespace engine
{

	Peer::Peer(const std::string& ipAddress, const std::string& peerMoDN, const uint16_t& portNumber, const int32_t& status, const Type& type)
	: m_connections(),
	  m_ipAddress(ipAddress),
	  m_peerMoDN(peerMoDN),
	  m_portNumber(portNumber),
	  m_status(static_cast<Status>(status)),
	  m_type(type)//, m_mode(SERIAL)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Peer::Peer(const std::string& ipAddress, const std::string& peerMoDN, const int32_t& status, const Type& type)
	: m_connections(),
	  m_ipAddress(ipAddress),
	  m_peerMoDN(peerMoDN),
	  m_portNumber(0U),
	  m_status(static_cast<Status>(status)),
	  m_type(type)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	Peer::~Peer()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void Peer::setStatus(Status newStatus)
	{
		AES_DDT_TRACE_MESSAGE("Set Peer Status to :<%d>", newStatus);

		if(newStatus != m_status)
		{
			AES_DDT_LOG(LOG_LEVEL_INFO,"Change status from:<%d> to:<%d>, MO DN: <%s>", m_status, newStatus, m_peerMoDN.c_str());
			AES_DDT_TRACE_MESSAGE("Change status from:<%d> to:<%d>, MO DN: <%s>", m_status, newStatus, m_peerMoDN.c_str());

			if( imm::objectManagerHelper_t::instance()->changePeerStatusAttribute(m_peerMoDN, newStatus) )
			{
				//update status attribute
				m_status = newStatus;
				AES_DDT_LOG(LOG_LEVEL_INFO,"Peer status changed to:<%d>, MO DN: <%s>", m_status, m_peerMoDN.c_str());
				AES_DDT_TRACE_MESSAGE("Peer status changed to:<%d>, MO DN: <%s>", m_status, m_peerMoDN.c_str());
			}
		}
		else
		{
			AES_DDT_TRACE_MESSAGE("Current status already is:<%d>, MO DN: <%s>", newStatus, m_peerMoDN.c_str());
		}
	}

}


