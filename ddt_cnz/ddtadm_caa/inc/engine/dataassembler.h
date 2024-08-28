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
#ifndef DDTADM_CAA_INC_ENGINE_DATAASSEMBLER_H_
#define DDTADM_CAA_INC_ENGINE_DATAASSEMBLER_H_

#include "operation/operation.h"
#include "engine/outputformat.h"
#include "engine/timer.h"

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#ifndef CUTE_TEST
	#include <ace/Task_T.h>
#else
	#include "stubs/ACE_Task_Base_stub.h"
#endif

namespace store
{
	class HandlerInterface;
}

namespace engine
{

	class DataSink;

	namespace
	{
		const uint32_t ERROR_MAX_WAIT_TIME_MICROSEC = 500000U; // 0.5 seconds
		const uint32_t NODATA_MAX_WAIT_TIME_SEC = 30U; // 30 seconds
		const uint32_t NO_RECORD = 0U;
	}

	/**
	 * @class DataAssembler
	 *
	 * @brief
	 *
	 * This class helps to manage the data belongs to a producer of Data Source.
	 * It is base class for FileAssembler and BlockAssembler.
	 *
	 */
	class DataAssembler : public ACE_Task<ACE_MT_SYNCH>, private boost::noncopyable
	{
		public:

			enum
			{ 	// indexes in the HANDLE array
				stop_pos = 0,	// 0 Service shutdown
				dataReady_pos, 	// 1 data Ready
				holdTime_pos,	// 2 hold timer expired
				number_of_handles
			};

			enum
			{
				eventNotify_pos = 1
			};

			///  Constructor
			DataAssembler(const std::string& dataSourceName, const std::string& producerName, const int& stopEvent, DataSink* sink);

			///  Destructor
			virtual ~DataAssembler();

			/**
			 *	@brief	This method is used to initialize a task and
			 *	prepare it for execution.
			 *
			 *	@param args : This is used to pass arbitrary information to open()
			 *
			 *	@return int
			 */
			virtual int open(void *args = 0);

			/**
			 *	@brief	This method is used to run the thread as daemon to
			 *	handle deferred processing
			 *
			 *	@return int
			 */
			virtual int svc() = 0;

			/**
			 * @brief	Initializes the task and prepare it to run as thread.
			 */
			virtual int start();

			/**
			 * @brief Stops the thread execution and waits for the thread termination.
			 */
			virtual int stop();

			/**	@brief
			 *
			 *	This method gets the producer name
			 *
			 *	@return name of the producer
			 *
			 *	@remarks Remarks
			 */
			inline std::string getProducerName() const { return m_producerName; }

			/**	@brief
			 *
			 *	This method gets the data source name
			 *
			 *	@return name of the data source
			 *
			 *	@remarks Remarks
			 */
			inline std::string getDataSourceName() const { return m_dataSourceName; }

			/**
			 * @brief Notify the assembler that a change occurred in OutputData configuration
			 */
			void notifyChange();

			/**
			 * @brief Notify the assembler that a peer became valid
			 */
			void notifyValidPeer();

		protected:

			/** @brief
			 *
			 *	This method tries to fill the data chunk with records.
			 *
			 *  @param chunk: input buffer to fill
			 *
			 *  @param freeChunkSpace: free space into input buffer
			 *
			 *  @param recordCounter: number of records stored into the data chunk
			 *
			 *  @return ERR_NO_ERRORS on success otherwise a proper error code
			 *
			 *	@remarks Remarks
			 */
			int getChunk(unsigned char* chunk, uint32_t& freeChunkSpace, uint32_t& recordCounter);

			/** @brief
			 *
			 *	This method prepares the data chunk to send according to specified output format.
			 *
			 *  @param chunk: input buffer to format
			 *
			 *  @param freeChunkSpace: free space into input buffer
			 *
			 *  @return the size of the formatted data chunk to send.
			 *
			 *	@remarks Remarks
			 */
			uint32_t formatDataChunk(unsigned char* chunk, uint32_t& freeChunkSpace);

			/** @brief
			 *
			 *	This method waits for new data to read. If no data is available
			 *	before holdTime has elapsed, data already assembled shall be sent.
			 *
			 *  @return true if new data are available before hold time, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool waitNewDataAvailable();

			/** @brief
			 *
			 *	This method waits for a valid peer to be available to receive the data.
			 *
			 *  @return true if a valid peer is available
			 *
			 *	@remarks Remarks
			 */
			bool waitValidPeerAvailable();

			/**	@brief
			 *
			 *	This method gets the change event handle
			 *
			 *	@return event handle
			 *
			 *	@remarks Remarks
			 */
			int getChangeEventHandle();

			/**	@brief
			 *
			 *	This method gets the valid peer event handle
			 *
			 *	@return valid peer event handle
			 *
			 *	@remarks Remarks
			 */
			int getValidPeerEventHandle();

			//-----------------------------
			// ATTRIBUTES
		protected:

			/// Store Reader
			boost::shared_ptr<store::HandlerInterface> m_storeReader;

			/// data source name
			std::string m_dataSourceName;

			/// producer name
			std::string m_producerName;

			// formatting info of the data chunk
			boost::shared_ptr<OutputFormat> m_outputFormat;

			/**
			 * 	@brief	m_StopEvent
			 *
			 * 	To signal shutdown
			 *
			 */
			int m_stopEvent;

			/**
			 * 	@brief	m_stopRequested
			 *
			 * 	To signal shutdown
			 *
			 */
			bool m_stopRequested;

			/**
			 * 	@brief
			 *
			 * 	Thread state
			 *
			 */
			bool m_running;

			/// data sink
			DataSink* m_sink;

			/// timer
			Timer m_timer;

			/// change event
			int m_changeEvent;

			/// valid peer event
			int m_validPeerEvent;

			/// to take exclusive access to internal data structures
			boost::recursive_mutex m_eventMutex;
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_DATAASSEMBLER_H_ */
