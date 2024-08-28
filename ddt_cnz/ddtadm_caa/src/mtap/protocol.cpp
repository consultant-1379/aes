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

#include "mtap/protocol.h"

namespace mtap
{
	namespace protocol
	{
		//****************************************************************
		// Convert an unsigned int to an AXE 4-bytes value, and place it
		// in the buffer.
		//****************************************************************
		void put4bytes(unsigned char*& buffer, uint32_t value)
		{
			unsigned char* valBuf = reinterpret_cast<unsigned char*>(&value);

			buffer[0] = valBuf[0];
			buffer[1] = valBuf[1];
			buffer[2] = valBuf[2];
			buffer[3] = valBuf[3];
			buffer += 4U;
		}
	}
}

