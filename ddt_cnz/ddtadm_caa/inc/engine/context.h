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
#ifndef DDTADM_CAA_INC_ENGINE_CONTEXT_H_
#define DDTADM_CAA_INC_ENGINE_CONTEXT_H_

#include <iostream> //debug purpose

#include "common/programconstants.h"

#include "engine/datasource.h"
#include "engine/datasink.h"
#include "engine/outputformat.h"
#include "engine/fileformat.h"
#include "engine/peercontainer.h"

#include "engine/peer.h"
#include "engine/filepeer.h"
#include "engine/blockpeer.h"

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <list>

namespace engine
{

	enum
	{
		dataSourceName_idx = 0,
		dataSource_idx,
		dataSink_idx,
		outputFormat_idx,
		fileFormat_idx,
		peers_idx,

		//Keep as last element
		mutex_number
	};

	/**
	 * @class Context
	 *
	 * @brief
	 *
	 * This class provides a encapsulation for all data models related to a
	 * particular data source and it is used to fetch data source related info
	 * like data sink, peers, output format, ... during the execution.
	 *
	 */
	class Context : private boost::noncopyable
	{
 	public:

		/// Constructor
		Context(const std::string& dataSourceName);

		/// Destructor
		virtual ~Context();

		//---------------------------------------------------------------
		// DATA SOURCE METHODS
		/**
		 *	@brief	Verify the presence of a Data Source in the context
		 *
		 *	@return bool
		 */
		bool hasDataSource();

		/**
		 *	@brief	Set a new Data Source in the context
		 *
		 *	@param dataSource : pointer to a data source
		 *
		 *	@return void
		 */
		void setDataSource(boost::shared_ptr<DataSource> dataSource);

		/**
		 *	@brief	Reset the Data Source
		 *
		 *	@return void
		 */
		void resetDataSource();

		/**
		 *	@brief	Get the Data Source
		 *
		 *	@return The data source set in the context
		 */
		boost::shared_ptr<DataSource> getDataSource();

		//---------------------------------------------------------------
		// DATA SINK METHODS
		/**
		 *	@brief	Verify the presence of a Data Sink in the context
		 *
		 *	@return bool
		 */
		bool hasDataSink();

		/**
		 *	@brief	Set a new Data Sink in the context
		 *
		 *	@param dataSink : pointer to a data sink
		 *
		 *	@return void
		 */
		void setDataSink(boost::shared_ptr<DataSink> dataSink);

		/**
		 *	@brief	Reset the Data Sink
		 *
		 *	@return void
		 */
		void resetDataSink();

		/**
		 *	@brief	Get the Data Sink
		 *
		 *	@return The data sink set in the context
		 */
		boost::shared_ptr<DataSink> getDataSink();


		//---------------------------------------------------------------
		// OUTPUT FORMAT METHODS
		/**
		 *	@brief	Verify the presence of a Output Format in the context
		 *
		 *	@return bool
		 */
		bool hasOutputFormat();

		/**
		 *	@brief	Set a new Output Format in the context
		 *
		 *	@param outputFormat : pointer to a outputFormat
		 *
		 *	@return void
		 */
		void setOutputFormat(boost::shared_ptr<OutputFormat> outputFormat);

		/**
		 *	@brief	Reset the OutputFormat
		 *
		 *	@return void
		 */
		void resetOutputFormat();

		/**
		 *	@brief	Get the OutputFormat
		 *
		 *	@return The OutputFormat set in the context
		 */
		boost::shared_ptr<OutputFormat> getOutputFormat();

		//---------------------------------------------------------------
		// FILE FORMAT METHODS
		/**
		 *	@brief	Verify the presence of a File Format in the context
		 *
		 *	@return bool
		 */
		bool hasFileFormat();

		/**
		 *	@brief	Set a new File Format in the context
		 *
		 *	@param fileFormat : pointer to fileFormat
		 *
		 *	@return void
		 */
		void setFileFormat(boost::shared_ptr<FileFormat> fileFormat);

		/**
		 *	@brief	Reset the File Format
		 *
		 *	@return void
		 */
		void resetFileFormat();

		/**
		 *	@brief	Get the FileFormat
		 *
		 *	@return The FileFormat set in the context
		 */
		boost::shared_ptr<FileFormat> getFileFormat();

		//---------------------------------------------------------------
		// PEER METHODS

		/** @brief
		 *
		 *	This method gets the number of peers associated with
		 *	DataSink.
		 *
		 *	@return Returns count of peers
		 *
		 *	@remarks Remarks
		 */
		ssize_t peerCount();

		/** @brief
		 *
		 *	This method sets the given file peer in the context
		 *
		 *	@param peer: pointer to file peer
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void setFilePeer(boost::shared_ptr<FilePeer> peer);

		/** @brief
		 *
		 *	This method sets the given block peer in the context
		 *
		 *	@param peer : pointer to block peer
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void setBlockPeer(boost::shared_ptr<BlockPeer> peer);

		/** @brief
		 *
		 *	This method gets numbers of file peers present in the context
		 *
		 *	@return count of number of file peers
		 *
		 *	@remarks Remarks
		 */
		int countFilePeers();

		/** @brief
		 *
		 *	This method gets numbers of block peers present in the context
		 *
		 *	@return count of number of block peers
		 *
		 *	@remarks Remarks
		 */
		int countBlockPeers();

		/** @brief
		 *
		 *	This method gets all the numbers of peers (valid, faulty) present in the context
		 *
		 *	@return list of peers
		 *
		 *	@remarks Remarks
		 */
		std::list< boost::shared_ptr<Peer> > getPeers();

		/** @brief
		 *
		 *	This method gets all the numbers of valid peers present in the context
		 *
		 *	@return list of valid peers
		 *
		 *	@remarks Remarks
		 */
		std::list< boost::shared_ptr<Peer> > getValidPeers();

		/** @brief
		 *
		 *	This method gets all the numbers of faulty peers present in the context
		 *
		 *	@return list of faulty peers
		 *
		 *	@remarks Remarks
		 */
		std::list< boost::shared_ptr<Peer> > getFaultyPeers();

		/** @brief
		 *
		 *	This method gets peer present in the context with the given name
		 *
		 *	@param name : name of the peer
		 *
		 *	@return Peer object on success
		 *
		 *	@remarks Remarks
		 */
		boost::shared_ptr<Peer> getPeerByName(const std::string& name);

		/** @brief
		 *
		 *	This method removes given peer from the context
		 *
		 *	@param name : name of the peer
		 *
		 *	@return zero on success otherwise non zero
		 *
		 *	@remarks Remarks
		 */
		int resetPeerByName(const std::string& name);

		/** @brief
		 *
		 *	This method gets the next valid peer from the context
		 *
		 *	@return valid Peer on success
		 *
		 *	@remarks Remarks
		 */
		boost::shared_ptr<Peer> getNextValidPeer();

		/** @brief
		 *
		 *	This method checks the given peer is valid or not
		 *
		 *	@param peerName : name of peer
		 *
		 *	@return true on success, otherwise false
		 *
		 *	@remarks Remarks
		 */
		bool isPeerActive(const std::string& peerName);

		/** @brief
		 *
		 *	This method clears peers from the context
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void clearPeers();

		/**
		 *	@brief	Clean the entire Context
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void cleanup();

		/** @brief
		 *
		 *	This method marks the given peer as faulty
		 *
		 *	@param peer : pointer to a peer
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void markFaulty(boost::shared_ptr<Peer> peer);

		/** @brief
		 *
		 *	This method marks the given peer as valid
		 *
		 *	@param peer : pointer to a peer
		 *
		 *	@return void
		 *
		 *	@remarks Remarks
		 */
		void markValid(boost::shared_ptr<Peer> peer);

 	private:

		/**
		 * 	@brief	m_dataSourceName
		 *
		 * 	name of the DataSource MO that identifies the context
		 */
		std::string m_dataSourceName;

		/**
		 * 	@brief	m_dataSource
		 *
		 * 	Pointer to the DataSource
		 */
		boost::shared_ptr<DataSource> m_dataSource;

		/**
		 * 	@brief	m_dataSink
		 *
		 * 	Pointer to the DataSink
		 */
		boost::shared_ptr<DataSink> m_dataSink;

		/**
		 * 	@brief	m_outputFormat
		 *
		 * 	Pointer to the OutputFormat
		 */
		boost::shared_ptr<OutputFormat> m_outputFormat;

		/**
		 * 	@brief	m_fileFormat
		 *
		 * 	Pointer to the FileFormat
		 */
		boost::shared_ptr<FileFormat> m_fileFormat;

		/**
		 * 	@brief	m_peers
		 *
		 * 	List containing either FilePeer or BlockPeer pointers
		 */
		PeerContainer m_peers;


		boost::recursive_mutex m_contextMutex[mutex_number];


	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_CONTEXT_H_ */
