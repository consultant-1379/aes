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

#ifndef DDTADM_CAA_INC_ENGINE_BLOCKASSEMBLER_H_
#define DDTADM_CAA_INC_ENGINE_BLOCKASSEMBLER_H_

#include "dataassembler.h"

namespace engine
{
	/**
	 * @class BlockAssembler
	 *
	 * @brief
	 *
	 * This class provides a concrete implementation to manage the block data
	 * belongs to a producer of Data Source.
	 *
	 */
	class BlockAssembler: public DataAssembler
	{
		public:

			///  Constructor
			BlockAssembler(const std::string& dataSourceName, const std::string& producerName, const int& stopEvent, DataSink* sink);

			///  Destructor
			virtual ~BlockAssembler();

			/**
			 *	@brief life cycle
			 */
			virtual int svc();

		private:

			/** @brief
			 *
			 *	This method transmits the data chunk via its Data Sink towards a remote destination
			 *
			 *  @param chunk: output buffer to send
			 *
			 *  @param chunkSize: data length in byte
			 *
			 *  @param recordCounter: number of records stored into the data chunk
			 *
			 *  @return ERR_NO_ERRORS on success otherwise a proper error code
			 *
			 *	@remarks Remarks
			 */
			int transmitChunk(unsigned char* chunk, const uint32_t& chunkSize, const uint32_t& recordCounter);
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_BLOCKASSEMBLER_H_ */
