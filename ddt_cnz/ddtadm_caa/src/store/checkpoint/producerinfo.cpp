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

#include "store/checkpoint/producerinfo.h"
#include "common/utility.h"
#include "common/macros.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
	#include <boost/serialization/utility.hpp>
	#include <boost/archive/text_iarchive.hpp>
	#include <boost/archive/text_oarchive.hpp>
#else
	#include "stubs/macro_stub.h"
#endif

#include <sstream>
#include <boost/thread.hpp>

AES_DDT_TRACE_DEFINE(AES_DDT_Store_Checkpoint_ProducerInfo)

namespace boost {
namespace serialization {

	/** @brief
	 *
	 *	This method implements the BOOST lib serialization .
	 *
	 *  @param ar: boost archive
	 *
	 *	@remarks Remarks
	 */
	template<class Archive>
	void serialize(Archive & ar, store::checkpoint::ProducerInfo& object, const unsigned int version)
	{
		ar &  object.getCheckPointNames();
		UNUSED(version);
	}

} // namespace serialization
} // namespace boost

namespace store
{
	namespace checkpoint
	{
		ProducerInfo::ProducerInfo(const std::string& name)
		: m_producerName(name),
		  m_checkPointNames(),
		  m_mutex()
		{

		}

		void ProducerInfo::addCheckPoint(const std::string& name)
		{
			AES_DDT_TRACE_MESSAGE("Adding Checkpoint:<%s> to the Producer:<%s>", name.c_str(), m_producerName.c_str() );
			boost::lock_guard<boost::mutex> guard(m_mutex);

			m_checkPointNames.push_back(name);

			AES_DDT_TRACE_MESSAGE(" Producer:<%s> has <%zu> checkpoints", m_producerName.c_str(), m_checkPointNames.size() );
		}

		void ProducerInfo::removeCheckPoint(const std::string& name)
		{
			AES_DDT_TRACE_MESSAGE("Removing Checkpoint:<%s> from the Producer:<%s>", name.c_str(), m_producerName.c_str() );
			boost::lock_guard<boost::mutex> guard(m_mutex);

			m_checkPointNames.remove(name);

			AES_DDT_TRACE_MESSAGE(" Producer:<%s> has <%zu> checkpoints", m_producerName.c_str(), m_checkPointNames.size() );
		}

		bool ProducerInfo::getLastCheckPoint(std::string& name)
		{
			AES_DDT_TRACE_FUNCTION;
			bool result = false;
			name.clear();

			boost::lock_guard<boost::mutex> guard(m_mutex);

			if( !m_checkPointNames.empty() )
			{
				name.assign(m_checkPointNames.back());
				result = true;
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> checkpoints list empty", m_producerName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("last checkpoint:<%s>, result:<%s>", name.c_str(), common::utility::boolToString(result));
			return result;
		}

		bool ProducerInfo::getFirstCheckPoint(std::string& name)
		{
			AES_DDT_TRACE_FUNCTION;
			bool result = false;
			name.clear();

			boost::lock_guard<boost::mutex> guard(m_mutex);

			if( !m_checkPointNames.empty() )
			{
				name.assign(m_checkPointNames.front());
				result = true;
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> checkpoints list empty", m_producerName.c_str());
			}

			AES_DDT_TRACE_MESSAGE("first checkpoint:<%s>, result:<%s>", name.c_str(), common::utility::boolToString(result));
			return result;
		}

		void ProducerInfo::objectToBuffer(uint64_t& bufferLength, char* buffer)
		{
			boost::lock_guard<boost::mutex> guard(m_mutex);

			AES_DDT_TRACE_MESSAGE(" Producer:<%s> has <%zu> checkpoints", m_producerName.c_str(), m_checkPointNames.size() );
			AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> has <%zu> checkpoints", m_producerName.c_str(), m_checkPointNames.size() );

			memset(buffer, 0, bufferLength);
			std::stringstream outputstream;

			boost::archive::text_oarchive outputArchive(outputstream);
			outputArchive << (*this);

			if( outputstream.str().length() > bufferLength)
			{
				AES_DDT_TRACE_MESSAGE("Input buffer size<%lu> is too small, output size:<%zu>, Producer:<%s>", bufferLength,  outputstream.str().length(), m_producerName.c_str());
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Input buffer size<%lu> is too small, output size:<%zu>, Producer:<%s>", bufferLength,  outputstream.str().length(), m_producerName.c_str());
				// To keep string termination
				--bufferLength;
			}
			else
			{
				bufferLength = outputstream.str().length();
			}

			AES_DDT_TRACE_MESSAGE(" data size:<%lu>, Producer:<%s>  ", bufferLength, m_producerName.c_str());

			memcpy(buffer, outputstream.str().c_str(), bufferLength );
		}

		void ProducerInfo::bufferToObject(char* buffer)
		{
			AES_DDT_TRACE_FUNCTION;

			boost::lock_guard<boost::mutex> guard(m_mutex);

			std::stringstream inputStream(buffer);
			boost::archive::text_iarchive inputArchive(inputStream);
			inputArchive >> (*this);

			AES_DDT_TRACE_MESSAGE("Producer:<%s> has <%zu> checkpoints", m_producerName.c_str(), m_checkPointNames.size() );
			AES_DDT_LOG(LOG_LEVEL_INFO, "Producer:<%s> has <%zu> checkpoints", m_producerName.c_str(), m_checkPointNames.size() );
		}

	} // namespace ddt_ckpt
} //store
