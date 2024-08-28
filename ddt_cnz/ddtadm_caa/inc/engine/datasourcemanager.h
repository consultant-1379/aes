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
#ifndef DDTADM_CAA_INC_AES_DDT_DATASOURCEMANAGER_H_
#define DDTADM_CAA_INC_AES_DDT_DATASOURCEMANAGER_H_

#include "operation/operation.h"

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <map>
#include <list>
#include <string>

namespace engine
{
	class DataSource;
	class Context;

	/**
	 * @class AES_DDT_DataSourceManager
	 *
	 * @brief Manage all defined data source.
	 *
	 */

	class DataSourceManager : private boost::noncopyable
	{
	 public:

		/// Constructor.
		DataSourceManager();

		/// Destructor.
		virtual ~DataSourceManager();

		/**
		 *	@brief	Adds a new Data Source
		 */
		int add(const operation::dataSourceInfo& data, operation::result& op_result, bool startTask = true);

		/**
		 *	@brief	Adds a new Data Sink
		 */
		int add(const operation::dataSinkInfo& data, operation::result& op_result, bool startTask = true);

		/**
		 *	@brief	Adds a new File Peer
		 */
		int add(const operation::filePeerInfo& data, operation::result& op_result);

		/**
		 *	@brief	Adds a new Block Peer
		 */
		int add(const operation::blockPeerInfo& data, operation::result& op_result);

		/**
		 *	@brief	Adds a new Output Format
		 */
		int add(const operation::outputFormatInfo& data, operation::result& op_result);

		/**
		 *	@brief	Adds a new File Format
		 */
		int add(const operation::fileFormatInfo& data, operation::result& op_result);

		/**
		 *	@brief	Removes a Data Source
		 */
		int remove(const operation::dataSourceInfo& data, operation::result& op_result);

		/**
		 *	@brief	Removes a Data Sink
		 */
		int remove(const operation::dataSinkInfo& data, operation::result& op_result);

		/**
		 *	@brief	Removes a File Peer
		 */
		int remove(const operation::filePeerInfo& data, operation::result& op_result);

		/**
		 *	@brief	Removes a Block Peer
		 */
		int remove(const operation::blockPeerInfo& data, operation::result& op_result);

		/**
		 *	@brief Modifies a data source
		 */
		int modify(const operation::dataSourceInfo& data, operation::result& op_result);

		/**
		 *	@brief	Modifies a Data Sink
		 */
		int modify(const operation::dataSinkInfo& data, operation::result& op_result);

		/**
		 *	@brief	Modifies an Output Format
		 */
		int modify(const operation::outputFormatInfo& data, operation::result& op_result);

		/**
		 *	@brief	Modifies a File Format
		 */
		int modify(const operation::fileFormatInfo& data, operation::result& op_result);

		/**
		 *	@brief	Modifies a File Peer
		 */
		int modify(const operation::filePeerInfo& data, operation::result& op_result);

		/**
		 *	@brief	start all tasks
		 */
		int start();

		/**
		 *	@brief	stop all tasks
		 */
		int stop();

		/**
		 *	@brief	Searches the data source named as specified from the parameter dataSourceName
		 *
		 *	@param dataSourceName the name of data source to search
		 *
		 *	@return true if the data source is found otherwise false.
		 */
		bool exist(const std::string& dataSourceName) const;

		/**
		 *	@brief	Loads all defined data source from IMM at startup
		 *
		 *	@return true on success otherwise false.
		 */
		bool loadFromImm();

		/**
		 *	@brief	Gets the list of names of all defined data source
		 *
		 */
		void getDataSourceNames(std::list<std::string>& dataSourceList) const;

		/**
		 *	@brief	Gets the record size of a specific data source
		 *
		 *  @param dataSourceName the name of data source
		 *
		 *	@return the record size in byte of the data source
		 */
		uint32_t getDataSourceRecordSize(const std::string& dataSourceName) const;

		/**
		 *	@brief	Gets the pointer to a specific context
		 *
		 *  @param dataSourceName the name of data source
		 *
		 *	@return the Context pointer if the data source is found otherwise NULL.
		 */
		Context* getContext(const std::string& dataSourceName) const;


	 private:
		/**
		 *	@brief	Remove a Context and clean the Data Source Map
		 *
		 *  @param dataSourceName is the key of the context map
		 *
		 *	@return void
		 */
		void removeContext(const std::string& dataSourceName);

		/**
		 *	@brief	Gets the pointer to a specific data source object
		 *
		 *  @param dataSourceName the name of data source
		 *
		 *	@return the DataSource pointer if the data source is found otherwise NULL.
		 */
		boost::shared_ptr<DataSource> getDataSource(const std::string& dataSourceName) const;



		//dataSourceMap_t: {data source name, context of all related MOs}
		typedef std::map<std::string, Context*> dataSourceMap_t;
		dataSourceMap_t m_dataSourceMap;

		//UTILITY - PRINT THE MAP ON SCREEN
		void printMap();
	};
}
#endif /* DDTADM_CAA_INC_AES_DDT_DATASOURCEMANAGER_H_ */
