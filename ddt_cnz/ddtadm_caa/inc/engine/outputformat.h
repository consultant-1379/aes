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

#ifndef DDTADM_CAA_INC_ENGINE_OUTPUTFORMAT_H_
#define DDTADM_CAA_INC_ENGINE_OUTPUTFORMAT_H_

#include "operation/operation.h"
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace engine
{
	/**
	 * @class OutputFormat
	 *
	 * @brief This class provides concrete implementation to manage
	 *
	 * the OutputFormat MO life cycle.
	 *
	 */
	class OutputFormat
	{
 	 public:

		/// constructor
		OutputFormat(const operation::outputFormatInfo& data);

		/// constructor
		OutputFormat(const boost::shared_ptr<OutputFormat>& rhs);

		/// destructor
		virtual ~OutputFormat();

		/**	@brief
		 *
		 *	This method gets name of the data source
		 *
		 *	@return name of the data source.
		 *
		 *	@remarks Remarks
		 */
		const char* getDataSourceName() const { return m_data.getDataSourceName(); }

		/**	@brief
		 *
		 *	This method gets name of the data sink
		 *
		 *	@return name of the data sink.
		 *
		 *	@remarks Remarks
		 */
		const char* getDataSinkName() const { return m_data.getDataSinkName(); }

		/**	@brief
		 *
		 *	This method gets hold time in seconds.
		 *
		 *	@return hold time in seconds.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getHoldTimeSeconds() const { return m_holdTime.total_seconds(); }

		/**	@brief
		 *
		 *	This method gets hold time in milli seconds.
		 *
		 *	@return hold time in milli seconds.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getHoldTimeMilliSeconds() const { return m_holdTime.total_milliseconds(); }

		/**	@brief
		 *
		 *	This method gets hold time in micro seconds.
		 *
		 *	@return hold time in micro seconds.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getHoldTimeMicroSeconds() const { return m_holdTime.total_microseconds(); }

		/**	@brief
		 *
		 *	This method gets hold time in nano seconds.
		 *
		 *	@return hold time in nano seconds.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getHoldTimeNanoSeconds() const { return m_holdTime.total_nanoseconds(); }

		/**	@brief
		 *
		 *	This method gets data chunk size.
		 *
		 *	@return data chunk size.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getDataChunkSize() const {return m_data.dataChunkSize; }

		/**	@brief
		 *
		 *	This method gets data length type.
		 *
		 *	@return data length type.
		 *
		 *	@remarks Remarks
		 */
		int32_t getDataLengthType() const {return m_data.dataLengthType; }

		/**	@brief
		 *
		 *	This method gets padding char.
		 *
		 *	@return padding char.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getPaddingChar() const {return m_data.paddingChar; }

		/**	@brief
		 *
		 *	This method modify the current info to the given output format info.
		 *
		 *	@return zero on success, otherwise non zero.
		 *
		 *	@remarks Remarks
		 */
		int modify(const operation::outputFormatInfo& newData);

 	 private:

		/// output format info
		operation::outputFormatInfo m_data;

		/// hold time
		boost::posix_time::time_duration m_holdTime;
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_OUTPUTFORMAT_H_ */
