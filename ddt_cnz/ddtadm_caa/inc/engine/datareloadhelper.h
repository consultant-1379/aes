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

#ifndef DDTADM_CAA_INC_ENGINE_DATARELOADHELPER_H_
#define DDTADM_CAA_INC_ENGINE_DATARELOADHELPER_H_

#include <boost/noncopyable.hpp>
#include <string>
#ifndef CUTE_TEST
	#include <acs_apgcc_omhandler.h>
#else
	#include "stubs/imm_stubs.h"
#endif
class OmHandler;
namespace engine
{

	/**
	 * @class DataReloadHelper
	 *
	 * @brief
	 *  This class helps to manage the data reload from IMM at startup
	 *
	 *
	*/
	class DataReloadHelper : private boost::noncopyable
	{
	 public:

		/// Constructor
		DataReloadHelper();

		/// Destructor
		virtual ~DataReloadHelper();

		/**
		 *	@brief	Loads all defined data from IMM at startup
		 *
		 *	@return true on success otherwise false.
		 */
		bool reloadDataFromIMM();

	 private:

		/**
		 *	@brief	Loads all defined Data Source from IMM
		 *
		 *	@return true on success otherwise false.
		 */
		bool loadDataSources();

		/**
		 *	@brief	Loads a defined Data Sink of a specific Data Source from IMM
		 *
		 *	@return true on success otherwise false.
		 */
		bool loadDataSink(const std::string& dataSourceDN);

		/**
		 *	@brief	Loads all defined Peers and the OutputFormat of a specific Data Sink from IMM
		 *
		 *	@return true on success otherwise false.
		 */
		bool loadPeersAndOutputFormat(const std::string& dataSinkDN);

		/**
		 *	@brief	Loads the defined FileFormat of a specific OutputFormat from IMM
		 *
		 *	@return true on success otherwise false.
		 */
		bool loadFileFormat(const std::string& outputFormatDN);

		/**
		 *	@brief	Adds a defined Data Source
		 *
		 *	@return true on success otherwise false.
		 */
		bool addDataSourceFromDN(const std::string& dataSourceDN);

		/**
		 *	@brief	Adds a defined Data Sink
		 *
		 *	@return true on success otherwise false.
		 */
		bool addDataSinkFromDN(const std::string& dataSinkDN);

		/**
		 *	@brief	Adds a defined File Peer
		 *
		 *	@return true on success otherwise false.
		 */
		bool addFilePeerFromDN(const std::string& filePeerDN);

		/**
		 *	@brief	Adds a defined Block Peer
		 *
		 *	@return true on success otherwise false.
		 */
		bool addBlockPeerFromDN(const std::string& blockPeerDN);

		/**
		 *	@brief	Adds a defined Output Format
		 *
		 *	@return true on success otherwise false.
		 */
		bool addOutputFormatFromDN(const std::string& outputFormatDN);

		/**
		 *	@brief	Adds a defined File Format from IMM
		 *
		 *	@return true on success otherwise false.
		 */
		bool addFileFormatFromDN(const std::string& fileFormatDN);

		// OM handler
		OmHandler m_objectManager;
	};

} /* namespace engine */

#endif /* DDTADM_CAA_INC_ENGINE_DATARELOADHELPER_H_ */
