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

#ifndef DDTADM_CAA_INC_AES_DDT_PRODUCERCHECKPOINTSINFO_H_
#define DDTADM_CAA_INC_AES_DDT_PRODUCERCHECKPOINTSINFO_H_

#include <boost/noncopyable.hpp>

#ifndef CUTE_TEST
	#include <boost/serialization/list.hpp>
	#include <boost/thread/mutex.hpp>
#else
	#include "stubs/boost_stub.h"
	#include <stdint.h>
#endif
#include <list>
#include <string>
namespace store
{
	namespace checkpoint
	{
		/**
		 * @class ProducerInfo
		 *
		 * @brief
		 * This class keeps the info about checkpoint names used by a specific producer
		 *
		 */
		class ProducerInfo: private boost::noncopyable
		{
		 public:

			///  Constructor
			ProducerInfo(const std::string& name);

			///  Destructor
			virtual ~ProducerInfo() { }

			/** @brief
			 *
			 *	This method gets the producer name.
			 *
			 *	@remarks Remarks
			 */
			const char* getProducerName() const { return m_producerName.c_str(); };

			/** @brief
			 *
			 *	This method gets the number of defined checkpoints.
			 *
			 *	@remarks Remarks
			 */
			size_t getNumberOfCheckpoint() const { return m_checkPointNames.size();	}

			/** @brief
			 *
			 *	This method gets the reference to the internal list.
			 *
			 *  @return list reference
			 *
			 *	@remarks Remarks
			 */
			std::list<std::string>& getCheckPointNames() { return m_checkPointNames; }

			/** @brief
			 *
			 *	This method adds a new checkpoint name used by the producer.
			 *
			 *  @param name: the checkpoint name to add
			 *
			 *	@remarks Remarks
			 */
			void addCheckPoint(const std::string& name);

			/** @brief
			 *
			 *	This method removes a checkpoint name used by the producer.
			 *
			 *  @param name: the checkpoint name to remove
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			void removeCheckPoint(const std::string& name);

			/** @brief
			 *
			 *	This method gets the last checkpoint name used by the producer.
			 *
			 *  @param name: the last checkpoint name
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool getLastCheckPoint(std::string& name);

			/** @brief
			 *
			 *	This method gets the first checkpoint name used by the producer.
			 *
			 *  @param name: the first checkpoint name
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool getFirstCheckPoint(std::string& name);

			/** @brief
			 *
			 *	This method serializes the object itself into a buffer of chars.
			 *
			 *  @param bufferLength: the length of the output buffer
			 *
			 *  @param buffer: output buffer of chars
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			void objectToBuffer(uint64_t& bufferLength, char* buffer);

			/** @brief
			 *
			 *	This method deserializes a buffer of chars into the object itself.
			 *
			 *  @param buffer: input buffer of chars
			 *
			 *  @return true on success otherwise false.
			 *
			 *	@remarks Remarks
			 */
			void bufferToObject(char* buffer);

		 private:

			// producer name
			std::string m_producerName;

			// checkpoint names list
			std::list<std::string> m_checkPointNames;

			// to take exclusive access
			boost::mutex m_mutex;
		};

	} /* namespace checkpoint */
} /* namespace store */
#endif /* DDTADM_CAA_INC_AES_DDT_PRODUCERCHECKPOINTSINFO_H_ */
