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

#include "engine/fileformat.h"

#include <ACS_APGCC_CLibTypes.h>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/detail/interface_iarchive.hpp>
#include <boost/archive/detail/interface_oarchive.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/serialization/version.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/system/error_code.hpp>
#include <boost/thread/locks.hpp>
#include <boost/exception/diagnostic_information.hpp> 
#include <boost/exception_ptr.hpp> 
#include <cerrno>
#include <climits>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>

#include "common/macros.h"
#include "common/programconstants.h"
#include "engine/templatehandler.h"

#ifndef CUTE_TEST
	#include "common/tracer.h"
	#include "common/logger.h"
#else
	#include "stubs/macro_stub.h"
#endif

#include <boost/make_shared.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/map.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/filesystem.hpp>

#include "ACS_APGCC_CommonLib.h"
#include <linux/limits.h>

AES_DDT_TRACE_DEFINE(AES_DDT_Engine_FileFormat)

namespace
{
	const unsigned int MAX_NR_OF_CP = 20;
	const uint32_t SEQ_NR_MAX_VALUE = ~0U;
	const std::string HARDCODED_AES_PATH("/data/aes/data/");
	const std::string DDT_FOLDER_NAME("/ddt/");
	const std::string AES_DATA("AES_DATA");
}

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
		void serialize(Archive & ar, engine::FileFormat::SequenceNumberGenerator& object, const unsigned int version)
		{
			ar &  object.getSequenceNumbersMap();
			UNUSED(version);
		}

	} // namespace serialization
} // namespace boost
BOOST_CLASS_VERSION(engine::FileFormat::SequenceNumberGenerator, 1);


namespace engine
{

	FileFormat::SequenceNumberGenerator::SequenceNumberGenerator(const std::string dataSourceName)
	: m_sequenceNumbers(), m_startupSeqNr(0), m_maxSeqNr(0),
	  m_isPersistent(false), m_dataSourceName(dataSourceName)
	{
	}

	FileFormat::SequenceNumberGenerator::~SequenceNumberGenerator()
	{
		m_filestream.close();
	}

	void FileFormat::SequenceNumberGenerator::load(uint32_t startupSeqNr, uint32_t maxSeqNrValue, bool isPersistent)
	{
		m_startupSeqNr = startupSeqNr;
		m_maxSeqNr = maxSeqNrValue;
		m_isPersistent = isPersistent;

		if(m_isPersistent)
		{
			//Get Home Base Folder
			getHomeBaseFolder();

			//Create Home Base Folder if it does not exist
			createHomeBaseFolder();

			//Set the full path
			m_filePath.append(m_dataSourceName);

			deserializeSeqNrMap();
		}		
	}

	void FileFormat::SequenceNumberGenerator::removeData()
	{
		if(m_isPersistent)
		{
			if(m_filestream.is_open()) m_filestream.close();

			//delete file from disk
			boost::system::error_code ec;
			boost::filesystem::path boostFile(m_filePath.c_str());

			boost::filesystem::file_status s = boost::filesystem::status(boostFile, ec);
			if (ec == 0)
			{
				//Success case
				if (boost::filesystem::exists(s))
				{
					try
					{
						boost::filesystem::remove(boostFile);

						AES_DDT_LOG(LOG_LEVEL_INFO, "File DELETED: <%s>.", m_filePath.c_str());
						AES_DDT_TRACE_MESSAGE("INFO: File DELETED: <%s>.", m_filePath.c_str());
					}
					catch(const boost::filesystem::filesystem_error& ex)
					{
						AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Cannot delete file <%s>. error:<%s>", m_filePath.c_str(), ex.what());
						AES_DDT_TRACE_MESSAGE("ERROR: Cannot delete file <%s>. errno <%d>. error:<%s>", m_filePath.c_str(), errno, ex.what());
					}
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_WARN, "Call 'boost::filesystem::exists()' failed. Error Type == '%d'. Path == '%s'", s.type(), m_filePath.c_str());
					AES_DDT_TRACE_MESSAGE("Error: Call 'boost::filesystem::exists()' failed. Error Type == '%d'. Path == '%s'", s.type(), m_filePath.c_str());
				}
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Call 'boost::filesystem::status()' failed. Cannot get path status: %s", ec.message().c_str());
				AES_DDT_TRACE_MESSAGE("ERROR: Call 'boost::filesystem::status()' failed. Cannot get path status: %s", ec.message().c_str());
			}
		}
	}

	uint32_t FileFormat::SequenceNumberGenerator::getNextNumber(const std::string& producer)
	{
		uint32_t seqNr = 0U;

		mapOfSequenceNumbers_t::iterator sequenceNumber = m_sequenceNumbers.find(producer);

		if( m_sequenceNumbers.end() != sequenceNumber )
		{
			seqNr = (sequenceNumber->second).first++;
			(sequenceNumber->second).second = INIT;
			if((sequenceNumber->second).first > m_maxSeqNr)
			{
				(sequenceNumber->second).first = m_startupSeqNr;
			}
		}
		else
		{
			seqNr = m_startupSeqNr;

			fileInfo_t fileInfo = std::make_pair((m_startupSeqNr + 1), INIT);
			m_sequenceNumbers.insert(std::make_pair(producer, fileInfo));
		}

		serializeSeqNrMap();

		return seqNr;
	}

	void FileFormat::SequenceNumberGenerator::setFileStatus(const std::string& producer, const FileStatus& status)
	{
		mapOfSequenceNumbers_t::iterator sequenceNumber = m_sequenceNumbers.find(producer);

		if( m_sequenceNumbers.end() != sequenceNumber )
		{
			(sequenceNumber->second).second = status;
			serializeSeqNrMap();
			printMap();
		}
		else
		{
			//Error!!
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Call 'setFileStatus()' failed. Cannot set status: <%d>. No entry found for producer: '%s'", status, producer.c_str());
			AES_DDT_TRACE_MESSAGE("Error: Call 'setFileStatus()' failed. Cannot set status: <%d>. No entry found for producer: '%s'", status, producer.c_str());
		}
	}

	FileFormat::FileStatus FileFormat::SequenceNumberGenerator::getFileStatus(const std::string& producer, uint32_t& lastStoredSeqNr)
	{
		FileStatus status = INIT;
		mapOfSequenceNumbers_t::iterator sequenceNumber = m_sequenceNumbers.find(producer);

		if( m_sequenceNumbers.end() != sequenceNumber )
		{
			status = (sequenceNumber->second).second;
			lastStoredSeqNr = (sequenceNumber->second).first;
		}
		else
		{
			//Error!!
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Call 'getFileStatus()' failed. No entry found for producer: '%s'", producer.c_str());
			AES_DDT_TRACE_MESSAGE("Error: Call 'getFileStatus()' failed. No entry found for producer: '%s'", producer.c_str());
		}

		return status;
	}

	void FileFormat::SequenceNumberGenerator::serializeSeqNrMap()
	{		
		if(m_isPersistent)
		{
			int save_errno = 0;
			if(!m_filestream.is_open())
			{
				m_filestream.open(m_filePath.c_str(), std::ios_base::binary);
				save_errno = errno;
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Opening <%s>... The file %s open! errno: %d", m_filePath.c_str(), (m_filestream.is_open() ? "is" : "is not"), save_errno);
				AES_DDT_TRACE_MESSAGE("Opening <%s>... The file %s open! errno: %d", m_filePath.c_str(), (m_filestream.is_open() ? "is" : "is not"), save_errno);

			}

			if(m_filestream.is_open())
			{
				try
				{
					boost::archive::binary_oarchive outputArchive(m_filestream);
					outputArchive << (*this);


					AES_DDT_TRACE_MESSAGE("Serialized map into file <%s>. Map Size: <%zu>", m_filePath.c_str(), m_sequenceNumbers.size());


					(void) m_filestream.seekp(0, std::ios_base::beg); //Reset position to beginning
					if(m_filestream.fail())
					{
						AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Cannot reset file pointer <%s>.", m_filePath.c_str());
						AES_DDT_TRACE_MESSAGE("ERROR: Cannot reset file pointer <%s>. errno <%d>", m_filePath.c_str(), errno);

						m_filestream.close();
					}					
				}
				catch(const boost::archive::archive_exception& ex)
				{
					AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Cannot serialize MapSize<%zu> into <%s>. error:<%s>", m_sequenceNumbers.size(), m_filePath.c_str(), ex.what());
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot serialize MapSize<%zu> into <%s>. errno <%d>. error:<%s>", m_sequenceNumbers.size(), m_filePath.c_str(), errno, ex.what());


					m_filestream.close();
				}
				catch (boost::exception &ex)
				{					
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Cannot serialize error <%s>. errno <%d>. error:<%s> ", m_filePath.c_str(), errno, boost::diagnostic_information(ex).c_str()  );
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot serialize error <%s>. errno <%d>. error:<%s> ", m_filePath.c_str(), errno, boost::diagnostic_information(ex).c_str() );

					m_filestream.close();
				}
			}
			else
			{
				AES_DDT_LOG(LOG_LEVEL_ERROR, "Cannot store data in file <%s>. The stream is closed!! errno: <%d>", m_filePath.c_str(), save_errno);
				AES_DDT_TRACE_MESSAGE("ERROR: Cannot store data in file <%s>. The stream is closed!! errno: <%d>", m_filePath.c_str(), save_errno);

			}
		}
		printMap();
	}

	void FileFormat::SequenceNumberGenerator::getHomeBaseFolder()
	{
		ACS_APGCC_CommonLib oComLib;
		char szPath[PATH_MAX] = {0};
		int dwLen = PATH_MAX;

		ACS_APGCC_DNFPath_ReturnTypeT result = oComLib.GetDataDiskPath(AES_DATA.c_str(), szPath, dwLen);
		if(ACS_APGCC_DNFPATH_SUCCESS == result)
		{
			m_filePath.assign(szPath);
		}
		else
		{
			//use hardcoded path in case of errors from Common Lib
			AES_DDT_LOG(LOG_LEVEL_ERROR, "Cannot fetch <%s> folder from Common Lib. error: %d", AES_DATA.c_str(), result);
			AES_DDT_TRACE_MESSAGE("ERROR: Cannot fetch <%s> folder from Common Lib. error: %d", AES_DATA.c_str(), result);

			m_filePath.assign(HARDCODED_AES_PATH);
		}
		m_filePath.append(DDT_FOLDER_NAME);
	}

	void FileFormat::SequenceNumberGenerator::printMap()
	{
		printf("-------------------------------------\n");
		printf("-----        SEQ NUMBERS        -----\n");
		printf("-------------------------------------\n");
		printf("%-10s | %-10s | %-10s\n", "Producer", "Seq Nr", "Status");
		printf("%-10s + %-10s + %-10s\n", "----------", "----------", "----------");

		for(mapOfSequenceNumbers_t::const_iterator it = m_sequenceNumbers.begin(); it != m_sequenceNumbers.end(); ++it)
		{
			std::string key = it->first;
			fileInfo_t fileInfo = it->second;

			uint32_t value = fileInfo.first;
			FileStatus status = fileInfo.second;

			printf("%-10s | %-10i | %-10i\n", key.c_str(), value, status);
		}

		std::cout << "-------------------------------------" << std::endl;
	}

	void FileFormat::SequenceNumberGenerator::deserializeSeqNrMap()
	{	
		//look for...
		boost::system::error_code ec;
		boost::filesystem::path boostFile(m_filePath.c_str());
		boost::filesystem::file_status s = boost::filesystem::status(boostFile, ec);
		if (ec == 0)
		{
			if (boost::filesystem::exists(s))
			{
				std::ifstream ifile(m_filePath.c_str(), std::ios_base::binary);
				if(ifile.is_open())
				{
					try
					{

						boost::archive::binary_iarchive inputArchive(ifile);
						inputArchive >> (*this);

						AES_DDT_LOG(LOG_LEVEL_DEBUG, "Deserialized file <%s>. Map Size: <%zu>.", m_filePath.c_str(), m_sequenceNumbers.size());
						AES_DDT_TRACE_MESSAGE("Deserialized file <%s>. Map Size: <%zu>.", m_filePath.c_str(), m_sequenceNumbers.size());

					}
					catch(const boost::archive::archive_exception& ex)
					{
						AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Cannot deserialize <%s>. error:<%s>", m_filePath.c_str(), ex.what());
						AES_DDT_TRACE_MESSAGE("ERROR: Cannot deserialize <%s>. errno <%d>. error:<%s>", m_filePath.c_str(), errno, ex.what());  
					}
					catch (const std::length_error& ex) 
					{
						//File may got corrupted
						AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Cannot deserialize file got corrupted <%s>. error:<%s>", m_filePath.c_str(), ex.what());
						AES_DDT_TRACE_MESSAGE("ERROR: Cannot deserialize file got corrupted <%s>. errno <%d>. error:<%s>", m_filePath.c_str(), errno, ex.what());
					}
					catch (boost::exception &ex)
					{
						//File may got corrupted
						AES_DDT_LOG(LOG_LEVEL_ERROR, "Cannot deserialize error <%s>. errno <%d>. error:<%s> ", m_filePath.c_str(), errno, boost::diagnostic_information(ex).c_str()  );
						AES_DDT_TRACE_MESSAGE("ERROR: Cannot deserialize error <%s>. errno <%d>. error:<%s> ", m_filePath.c_str(), errno, boost::diagnostic_information(ex).c_str() );
					}
				}
				else
				{
					AES_DDT_LOG(LOG_LEVEL_ERROR, "Cannot open file <%s>.", m_filePath.c_str());
					AES_DDT_TRACE_MESSAGE("ERROR: Cannot open file <%s>.", m_filePath.c_str());

				}


				ifile.close();
			}
		}

		//debug
		printMap();
	}

	void FileFormat::SequenceNumberGenerator::createHomeBaseFolder()
	{
		boost::filesystem::path ddtHome(m_filePath);
		try
		{
			// check if dirpath exists
			if( !boost::filesystem::exists(ddtHome) )
			{
				// create the folders
				boost::filesystem::create_directories(ddtHome);
			}
		}
		catch(const boost::filesystem::filesystem_error& ex)
		{
			AES_DDT_SYSLOG_ERRNO(errno, LOG_ERR, LOG_LEVEL_ERROR, "Cannot create folder <%s>. error:<%s>", m_filePath.c_str(), ex.what());
			AES_DDT_TRACE_MESSAGE("ERROR: Cannot create folder <%s>. errno <%d>. error:<%s>", m_filePath.c_str(), errno, ex.what());
		}
	}

	FileFormat::FileFormat(const operation::fileFormatInfo& data)
	: m_data(data),
	  m_templateHandler(boost::make_shared<TemplateHandler>(m_data.templateValue)),
	  m_sequenceNumbers(),
	  m_seqNrGenerator(boost::make_shared<SequenceNumberGenerator>(data.dataSourceName))
	{
		AES_DDT_TRACE_FUNCTION;
	}

	FileFormat::FileFormat(const boost::shared_ptr<FileFormat>& rhs)
	: m_data(rhs->m_data),
	  m_templateHandler(boost::make_shared<TemplateHandler>(m_data.templateValue)),
	  m_sequenceNumbers(rhs->m_sequenceNumbers),
	  m_seqNrGenerator(rhs->m_seqNrGenerator)
	{
		AES_DDT_TRACE_FUNCTION;
	}

	FileFormat::~FileFormat()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	void FileFormat::initialize()
	{
		// Initialize can be only invoked on either FileFormat MO creation or data reload at Service startup
		// Initialize can be invoked also in case of modify callback by 'reloadSequenceNumberGenerator()'
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		uint32_t maxSeqNrValue = SEQ_NR_MAX_VALUE;

		if(m_templateHandler->hasPersistentTag())
		{
			(void)m_templateHandler->getMaxPersistentPrintableNumber(maxSeqNrValue);
		}
		else
		{
			(void)m_templateHandler->getMaxTransientPrintableNumber(maxSeqNrValue);
		}

		m_seqNrGenerator->load(m_data.startupSequenceNumber, maxSeqNrValue, m_templateHandler->hasPersistentTag());

	}

	void FileFormat::removeData()
	{
		// This method can only be invoked on FileFormat MO deletion triggered by CLI
		m_seqNrGenerator->removeData();
	}

	int FileFormat::modify(const operation::fileFormatInfo& newData)
	{
		AES_DDT_TRACE_FUNCTION;
		int result = common::errorCode::ERR_NO_ERRORS;

		if (newData.changeMask & operation::changeMask::FILESIZE_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying file size value, from <%u> to <%u>.", m_data.getDataSourceName(), m_data.getName(),
						m_data.fileSize, newData.fileSize);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying file size value, from <%u> to <%u>.", m_data.getDataSourceName(), m_data.getName(),
								  m_data.fileSize, newData.fileSize);

			m_data.fileSize = newData.fileSize;
		}

		if (newData.changeMask & operation::changeMask::FILE_TEMPLATE_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying template value, from <%s> to <%s>.", m_data.getDataSourceName(), m_data.getName(),
						m_data.templateValue.c_str(), newData.templateValue.c_str());
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying template value, from <%s> to <%s>.", m_data.getDataSourceName(), m_data.getName(),
								  m_data.templateValue.c_str(), newData.templateValue.c_str());

			m_data.templateValue = newData.templateValue;
			m_templateHandler = boost::make_shared<TemplateHandler>(m_data.templateValue);
		}

		if (newData.changeMask & operation::changeMask::FILE_SEQ_NUMBER_CHANGE)
		{
			AES_DDT_LOG(LOG_LEVEL_DEBUG, "[%s@%s] Modifying startup sequence number value, from <%u> to <%u>.", m_data.getDataSourceName(), m_data.getName(),
						m_data.startupSequenceNumber, newData.startupSequenceNumber);
			AES_DDT_TRACE_MESSAGE("[%s@%s] Modifying startup sequence number, from <%u> to <%u>.", m_data.getDataSourceName(), m_data.getName(),
								  m_data.startupSequenceNumber, newData.startupSequenceNumber);

			m_data.startupSequenceNumber = newData.startupSequenceNumber;
		}

		reloadSequenceNumberGenerator();

		return result;
	}

	std::string FileFormat::buildFileName(const std::string& producer)
	{
		// Many File Assemblers, in MCP environment, could invoke buildFileName
		// that in turn will access to data disk to serialize the file table managed
		// in the Sequence Number Generator.
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);

		uint32_t currentSequenceNumber = m_seqNrGenerator->getNextNumber(producer);

		return m_templateHandler->getName(producer, currentSequenceNumber);
	}

	void FileFormat::setFileStatus(const std::string& producer, const FileStatus& status)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);
		m_seqNrGenerator->setFileStatus(producer, status);
	}

	FileFormat::FileStatus FileFormat::getFileStatus(const std::string& producer, uint32_t& lastStoredSeqNr)
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);
		return m_seqNrGenerator->getFileStatus(producer, lastStoredSeqNr);
	}


	void FileFormat::reloadSequenceNumberGenerator()
	{
		boost::lock_guard<boost::recursive_mutex> guard(m_mutex);
		//resetting the file SequenceNumberGenerator object
		m_seqNrGenerator.reset();
		m_seqNrGenerator = boost::make_shared<SequenceNumberGenerator>(m_data.dataSourceName);

		initialize();		
	}

} /* namespace engine */
