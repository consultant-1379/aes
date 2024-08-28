/*
 * fileformat.h
 *
 *  Created on: Jul 15, 2015
 *      Author: qpaoele
 */

#ifndef DDTADM_CAA_INC_ENGINE_FILEFORMAT_H_
#define DDTADM_CAA_INC_ENGINE_FILEFORMAT_H_

#include "operation/operation.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <fstream>
#include <map>

namespace engine
{
	class TemplateHandler;

	/**
	 * @class FileFormat
	 *
	 * @brief This class provides concrete implementation to manage
	 *
	 * the FileFormat MO life cycle.
	 *
	 */
	class FileFormat
	{
	 public:
		enum FileStatus
		{
			INIT,	// File Name has been fetched
			SEND,	// Open Remote Container done, sending data
			CLOSE	// Close Remote Container invoked
		};

		typedef std::pair< uint32_t, FileStatus > fileInfo_t;
		typedef std::map<std::string, fileInfo_t> mapOfSequenceNumbers_t;

		//--------------------------------------------------------
		// INNER CLASS
		//--------------------------------------------------------
	 private:

		class SequenceNumberGenerator
		{
		 public:
			SequenceNumberGenerator(const std::string dataSourceName);
			virtual ~SequenceNumberGenerator();

			void load(uint32_t startupSeqNr, uint32_t maxSeqNrValue, bool isPersistent);
			void removeData();

			uint32_t getNextNumber(const std::string& producer);
			void setFileStatus(const std::string& producer, const FileStatus& status);
			FileStatus getFileStatus(const std::string& producer, uint32_t& lastStoredSeqNr);
			std::map< std::string, std::pair< uint32_t, FileStatus > >& getSequenceNumbersMap() { return m_sequenceNumbers; }

		 private:
			void serializeSeqNrMap();
			void getHomeBaseFolder();
			void createHomeBaseFolder();
			void deserializeSeqNrMap();

			void printMap();

		 private:
			//MAP {ProducerName, FileInfo{Current Sequence Number, File Status} }

			std::map< std::string, fileInfo_t > m_sequenceNumbers;
			uint32_t m_startupSeqNr;
			uint32_t m_maxSeqNr;
			bool m_isPersistent;

			std::string m_dataSourceName;
			std::string m_filePath;
			std::ofstream m_filestream;
		};

		//--------------------------------------------------------
		// FILE FORMAT CLASS
		//--------------------------------------------------------
 	public:

		/// Constructor
		FileFormat(const operation::fileFormatInfo& data);

		/// Constructor
		FileFormat(const boost::shared_ptr<FileFormat>& rhs);

		/// Destructor
		virtual ~FileFormat();

		/**
		 *	@brief
		 *	This method initializes template handler during service startup or
		 *	FileFormat creation.
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void initialize();

		/**	@brief
		 *
		 *	This method removes data from sequence number generator during FileFormat MO deletion.
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void removeData();

		/**	@brief
		 *
		 *	This method gets the name of the data source.
		 *
		 *	@return Name of data source name
		 *
		 *	@remarks Remarks
		 */
		const char* getDataSourceName() const { return m_data.getDataSourceName(); }

		/**	@brief
		 *
		 *	This method gets the name of the data sink.
		 *
		 *	@return Name of data sink
		 *
		 *	@remarks Remarks
		 */
		const char* getDataSinkName() const { return m_data.getDataSinkName(); }

		/**	@brief
		 *
		 *	This method gets the size of the file.
		 *
		 *	@return size of the file.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getFileSize() const {return m_data.fileSize; }

		/**	@brief
		 *
		 *	This method gets the size of the file in bytes.
		 *
		 *	@return size of the file in bytes.
		 *
		 *	@remarks Remarks
		 */
		uint32_t getFileSizeInByte() const {return (m_data.fileSize * 1024U); }

		/**	@brief
		 *
		 *	This method gets the startup sequence number.
		 *
		 *	@return startup sequence number.
		 *
		 *	@remarks Remarks
		 */
		int32_t getStartupSequenceNumber() const {return m_data.startupSequenceNumber; }

		/**	@brief
		 *
		 *	This method gets the template value.
		 *
		 *	@return template value.
		 *
		 *	@remarks Remarks
		 */
		std::string getTemplate() const {return m_data.templateValue; }

		/**	@brief
		 *
		 *	This method gets the name of build file.
		 *
		 *	@param producer : name of the producer
		 *
		 *	@return name of build file
		 *
		 *	@remarks Remarks
		 */
		std::string buildFileName(const std::string& producer);

		/**	@brief
		 *
		 *	This method sets the file status.
		 *
		 *	@param producer : name of the producer
		 *
		 *	@param status : status of the file
		 *
		 *	@return None
		 *
		 *	@remarks Remarks
		 */
		void setFileStatus(const std::string& producer, const FileStatus& status);

		/**	@brief
		 *
		 *	This method gets the file status.
		 *
		 *	@param producer : name of the producer
		 *
		 *	@param lastStoredSeqNr : last stored sequenced number
		 *
		 *	@return None
		 *
		 *	@remarks Remarks
		 */
		FileStatus getFileStatus(const std::string& producer, uint32_t& lastStoredSeqNr);

		/**	@brief
		 *
		 *	This method modifies the current info with given file format info.
		 *
		 *	@param newData : file format info
		 *
		 *	@return None
		 *
		 *	@remarks Remarks
		 */
		int modify(const operation::fileFormatInfo& newData);
		
		void reloadSequenceNumberGenerator();
		

 	 private:

		/// to take exclusive access to internal data structures
		boost::recursive_mutex m_mutex;

		/// fileFormatInfo details
		operation::fileFormatInfo m_data;

		/// template handler
		boost::shared_ptr<TemplateHandler> m_templateHandler;

		/// map of sequence numbers
		mapOfSequenceNumbers_t m_sequenceNumbers;

		/// sequence number generator
		boost::shared_ptr<SequenceNumberGenerator> m_seqNrGenerator;
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_FILEFORMAT_H_ */
