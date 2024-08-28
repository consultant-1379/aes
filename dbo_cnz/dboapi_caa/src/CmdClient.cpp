
//******************************************************************************
//
// NAME
//      CmdClient.cpp
//
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	Handles communication with AES_DBO_Server using DSD.

// DOCUMENT NO
//	190 89-CAA

// AUTHOR 
// 	2001-06-04 by I/LD Bjrn Hall

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// SEE ALSO 
// 	AES_Cmd_Client
//
//******************************************************************************
#include "CmdClient.h"
#include <ACS_APGCC_DSD.H>

#include <sys/eventfd.h>

#include <aes_gcc_tracer.h>

// ACS Trace definition
AES_GCC_TRACE_DEFINE(AES_DBO_CmdClient);

//******************************************************************************
//	CmdClient::CmdClient()
CmdClient::CmdClient(const char* serviceName)
: m_address( new (std::nothrow) ACS_APGCC_DSD_Addr(serviceName)),
  stream_(new (std::nothrow) ACS_APGCC_DSD_Stream() ),
  server_(new (std::nothrow) ACS_APGCC_DSD_Connector()),
  m_ConnectionHandle(ACE_INVALID_HANDLE),
  m_StopEvent(ACE_INVALID_HANDLE)
{

}

//******************************************************************************
//	CmdClient::~CmdClient()
CmdClient::~CmdClient()
{
	// Remove the objects
	if(0 != server_)
	{
		delete server_;
		server_ = NULL;
	}

	if(0 != stream_)
	{
		delete stream_;
		stream_ = NULL;
	}

	if(0 != m_address)
	{
		delete m_address;
	}

	ACE_OS::close(m_StopEvent);
}


//******************************************************************************
//	connect()
bool CmdClient::connect()
{
 	AES_GCC_TRACE_MESSAGE("Entering...");

	bool result = false;
	// create shutdown handle
	m_StopEvent = eventfd(0,0);

	if(ACE_INVALID_HANDLE == m_StopEvent)
	{
		AES_GCC_TRACE_MESSAGE("failed to create shutdown handle, error:<%d>", errno);
	}

	// Create the objects
	if( (ACE_INVALID_HANDLE != m_StopEvent ) && (NULL != m_address ) && (NULL != stream_ ) && (NULL != server_ ))
	{
		if ( server_->connect( (*stream_), (*m_address) ) == 0 )
		{
			AES_GCC_TRACE_MESSAGE("connected to DBO");
			int handle_count = 1;
			stream_->get_handles(&m_ConnectionHandle, handle_count);
			result = ( ACE_INVALID_HANDLE != m_ConnectionHandle );

			if(0 == m_StopEvent)
			{
				int tmp = m_StopEvent;
			    m_StopEvent = eventfd(0,0);
				ACE_OS::close(tmp);
			}

			AES_GCC_TRACE_MESSAGE("connection handle:<%d>, m_StopEvent:<%d>", m_ConnectionHandle, m_StopEvent);
		}
		else
		{
			AES_GCC_TRACE_MESSAGE("failed to connect");
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%s>", (result ? "TRUE" : "FALSE") );
	return result;
}


//******************************************************************************
//	send()
bool CmdClient::send(ACS_APGCC_Command& cmd)
{
	AES_GCC_TRACE_MESSAGE("Entering..., id:<%d>", m_ConnectionHandle);

	bool result = true;
	cmd.result = -1;
	cmd.encode();

	if (cmd.send(*stream_) < 0)
	{
		AES_GCC_TRACE_MESSAGE(" send error:<%d>", errno);
		result = false;
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%s>, id:<%d>", (result ? "TRUE" : "FALSE"), m_ConnectionHandle );

	return result;
}


//******************************************************************************
//	receive()
unsigned int CmdClient::receive(ACS_APGCC_Command& cmd, unsigned long timeout)
{
	AES_GCC_TRACE_MESSAGE("Entering..., id:<%d>", m_ConnectionHandle);

	unsigned int result = CERROR;

	// check if the connection handle
	if( ACE_INVALID_HANDLE != m_ConnectionHandle )
	{
		// Initialize the pollfd structure
		const nfds_t nfds = 2;
		struct pollfd fds[nfds];
		ACE_OS::memset(fds, 0, sizeof(fds));

		// Stop signal
		fds[0].fd = m_StopEvent;
		fds[0].events = POLLIN;

		// Connection handle
		fds[1].fd = m_ConnectionHandle;
		fds[1].events = POLLIN;

		// timeout : <sec, microsec>
		ACE_Time_Value recTimeout(timeout,0);
		ACE_INT32 pollResult;

		while(true)
		{
			if(0 != timeout)
			{
				// Wait for input ready, timeout or close signal
				pollResult = ACE_OS::poll(fds, nfds, &recTimeout);
			}
			else
			{
				// Wait for input ready or close signal
				pollResult = ACE_OS::poll(fds, nfds);
			}

			// Error on Poll
			if( 0 > pollResult )
			{
				if(errno == EINTR)
				{
					// try again
					continue;
				}
				break;
			}

			// time-out expired
			if( 0 == pollResult )
			{
				AES_GCC_TRACE_MESSAGE("TIMEOUT on receive!, id:<%d>", m_ConnectionHandle);
				result = CTIMEOUT;
				break;
			}

			// check handle
			if(fds[0].revents & POLLIN)
			{
				// Received signal of termination
				AES_GCC_TRACE_MESSAGE("ShutDown connection receive!, id:<%d>", m_ConnectionHandle);
				break;
			}

			if(fds[1].revents & POLLIN)
			{
				// Received input to read
				if (cmd.recv(*stream_) < 0)
				{
					AES_GCC_TRACE_MESSAGE(" receive error:<%d>, id:<%d>", errno, m_ConnectionHandle);
				}
				else
				{
					result = COK;
				}

				break;
			}
			break;
		}
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%d>, id:<%d>", result, m_ConnectionHandle);

	return result;
}

//******************************************************************************
//	close()
void CmdClient::close()
{
	AES_GCC_TRACE_MESSAGE("Entering..., id:<%d>", m_ConnectionHandle);

	// Close
	if(NULL != stream_)
	{
		stream_->close();
	}

	if(NULL != server_)
	{
		server_->close();
	}
	AES_GCC_TRACE_MESSAGE("...Leaving, id:<%d>", m_ConnectionHandle);
}

bool CmdClient::shutDown()
{
	AES_GCC_TRACE_MESSAGE("Entering...");
	bool result = true;
	eventfd_t stop=1U;

	AES_GCC_TRACE_MESSAGE("Signal stop Event:<%d>, id:<%d>", m_StopEvent, m_ConnectionHandle);

	if(eventfd_write(m_StopEvent, stop) != 0)
	{
		AES_GCC_TRACE_MESSAGE("Failed to signal stop Event error:<%d>, id:<%d>", errno, m_ConnectionHandle);
		result = false;
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, result:<%s>, id:<%d>", (result ? "TRUE" : "FALSE"), m_ConnectionHandle);

	return result;
}
