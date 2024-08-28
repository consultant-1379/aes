
//*******************************************************************
// 
// .NAME
//  	AES_DBO_TQReceiver
// .LIBRARY 3C++
// .PAGENAME AES_DBO_TQReceiver
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE AES_DBO_TransferQueue.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2002.
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
// 	This class is used by receiving clients which uses the transfer queues.
//  The class makes it possible to receive data through a queue.
//  

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 1300

// AUTHOR 
// 	2002-05-27 by UAB/KB/AU Mats Nilsson

// CHANGES
//
//	RELEASE REVISION HISTORY
//
//	REV NO  DATE    NAME    DESCRIPTION

// .LINKAGE
//	libAES_DBO_R1A_DMDN6.lib

// .SEE ALSO 
//  

//*******************************************************************
#include "AES_DBO_TQReceiver.h"
#include "ParameterHandler.h"
#include <aes_gcc_tracer.h>
#include <aes_gcc_errorcodes.h>


// ACS Trace definition
AES_GCC_TRACE_DEFINE(AES_DBO_TQReceiver);

//*******************************************************************
//
AES_DBO_TQReceiver::AES_DBO_TQReceiver(const std::string &tqName):
name_(tqName),
m_DataStream()
{

}

//*******************************************************************
//
AES_DBO_TQReceiver::~AES_DBO_TQReceiver()
{

}

//*******************************************************************
//
unsigned int AES_DBO_TQReceiver::openTransfer()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", name_.c_str() );
	unsigned int result = static_cast<unsigned int>(AES_NOERRORCODE);
	std::string tqPath(ParameterHandler::instance()->getDBODataPath());
	tqPath.append(name_);

	unsigned int streamSize =  ParameterHandler::instance()->getStreamSize();
	AES_GCC_TRACE_MESSAGE("create a TQ Sender, stream size:<%d>", streamSize);

	// Open the stream
	if( !m_DataStream.open(name_, tqPath, streamSize) )
	{
		result = static_cast<unsigned int>(AES_BUFFERERROR);
	}

	AES_GCC_TRACE_MESSAGE("...Leaving, TQ:<%s> open stream result:<%d>", name_.c_str(), result);
	return result;
}

//*******************************************************************
//
unsigned int AES_DBO_TQReceiver::closeTransfer()
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>", name_.c_str() );
	// Close the stream
	m_DataStream.close();

	AES_GCC_TRACE_MESSAGE("...Leaving, TQ:<%s>", name_.c_str() );

    return (0U);
}

//*******************************************************************
//
unsigned int AES_DBO_TQReceiver::recv(AES_DBO_DataBlock *&block,
                                      unsigned int timeout,
									  ACE_Event* hAbortRead)
{
	AES_GCC_TRACE_MESSAGE("Entering..., TQ:<%s>, timeout:<%d>", name_.c_str(), timeout);

    // Read length of block
    unsigned int len = sizeof(unsigned int);
    unsigned int dataLen = 0U;
    unsigned int result = m_DataStream.read((char *)&dataLen, len, timeout, hAbortRead);

    switch(result)
    {
		case AES_NOERRORCODE:
		{
			if(0 == len)
			{
				result = static_cast<unsigned int>(AES_BUFFEREMPTY);
				AES_GCC_TRACE_MESSAGE("TQ:<%s> buffer is empty, data length:<%d>", name_.c_str(), dataLen);
			}
			else if (sizeof(unsigned int) != len )
			{
				result = static_cast<unsigned int>(AES_BUFFERERROR);
				AES_GCC_TRACE_MESSAGE("TQ:<%s> buffer error, data length:<%d>", name_.c_str(), len);
			}
		}
		break;

		case AES_TIMEOUT:
		{
			AES_GCC_TRACE_MESSAGE("TQ:<%s> Timeout on buffer read", name_.c_str());
		}
		break;

		case AES_BUFFEREMPTY:
		{
			AES_GCC_TRACE_MESSAGE("TQ:<%s> Buffer empty", name_.c_str());
		}
		break;

		default:
			AES_GCC_TRACE_MESSAGE("TQ:<%s> error:<%d>",  name_.c_str(), result);
			result = static_cast<unsigned int>(AES_BUFFERERROR);
    }

    if( result == AES_NOERRORCODE)
    {
    	AES_GCC_TRACE_MESSAGE("TQ:<%s> Data Ready length:<%d>", name_.c_str(), dataLen);

    	// Read the block
		len = dataLen + AES_DBO_DataBlock::hLength_ - sizeof(unsigned int);
		block = AES_DBO_DataBlock::create(dataLen);
		if(block)
		{	
			char *ptr = (char *)block;

			result = m_DataStream.read(ptr + sizeof(unsigned int), len);

			switch(result)
			{
				case AES_NOERRORCODE:
				{
					if( 0 == len)
					{
						result = static_cast<unsigned int>(AES_BUFFEREMPTY);
						AES_GCC_TRACE_MESSAGE("TQ:<%s> buffer seems empty, data read length:<%d>", name_.c_str(), len);
					}
					else if (len != dataLen + AES_DBO_DataBlock::hLength_ - sizeof(unsigned int))
					{
						result = static_cast<unsigned int>(AES_BUFFERERROR);
						AES_GCC_TRACE_MESSAGE("TQ:<%s> buffer error, data read length:<%d>", name_.c_str(), len);
					}
				}
				break;

				case AES_TIMEOUT:
				{
					AES_GCC_TRACE_MESSAGE("TQ:<%s> Timeout on buffer read", name_.c_str());
				}
				break;

				case AES_BUFFEREMPTY:
				{
					AES_GCC_TRACE_MESSAGE("TQ:<%s> Buffer empty", name_.c_str());
				}
				break;

				default:
					AES_GCC_TRACE_MESSAGE("TQ:<%s> error:<%d>", name_.c_str(), result);
					result = static_cast<unsigned int>(AES_BUFFERERROR);
			}	
			if( result != AES_NOERRORCODE) {
				delete block;
				block=NULL;
			}
    		}
		else
		{
			AES_GCC_TRACE_MESSAGE("Error in memory allocation");
			result = static_cast<unsigned int>(AES_CATASTROPHIC);
		}
	}
    
    AES_GCC_TRACE_MESSAGE("...Leaving, TQ:<%s> result:<%d>", name_.c_str(), result);
    return result;
}

//*******************************************************************
//
void AES_DBO_TQReceiver::destroy()
{
	AES_GCC_TRACE_MESSAGE("receiver on TQ:<%s> destroyed",  name_.c_str());

    // No thread is deleting this object
    delete this;
}
