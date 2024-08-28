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

#ifndef DDTADM_CAA_INC_AES_DDT_IMM_H_
#define DDTADM_CAA_INC_AES_DDT_IMM_H_

#include <string>
#include <stdint.h>

namespace imm
{
	/**
	 * @namespace error_code
	 *
	 * @brief
	 * Contains the error codes returned by APGCC API.
	 *
	 */
	namespace error_code
	{
		const int ERR_IMM_TRY_AGAIN = -6;
		const int ERR_IMM_BAD_HANDLE = -9;
		const int ERR_IMM_ALREADY_EXIST = -14;
	};

	/**
	 * @namespace oi_name
	 *
	 * @brief
	 * Contains the error messages returned to the COM-CLI
	 *
	 */
	namespace comcli_errormessage
	{
		const std::string GENERIC = "GENERIC ERROR";
		const std::string DATASOURCE_NAME_CONFLICT = "DataSource name is already defined for a MessageStore";
		const std::string HW_NOT_SUPPORTED = "DataSource definition is not valid for this system configuration";
		const std::string DATASOURCE_NOT_REMOVABLE = "Cannot delete DataSource";
		const std::string DATASINK_NOT_REMOVABLE = "Cannot delete DataSink";
		const std::string PEER_NOT_REMOVABLE = "Cannot delete Peer";
		const std::string CONFIGURATION_NOT_VALID = "DataSink configuration not valid";
		const std::string NOT_IMPLEMENTED_YET = "Operation not implemented yet";

		const std::string NOT_ENOUGH_PEER = "Either a FilePeer or BlockPeer must be defined for the DataSink";
		const std::string PEER_TYPE_MISMATCH = "Peers must be of same type for the DataSink";
		const std::string MISSING_FILE_FORMAT = "FileFormat is mandatory for the DataSink where FilePeer is defined";
		const std::string FILE_FORMAT_NOT_ALLOWED = "Cannot define FileFormat for the DataSink where BlockPeer is defined";
		const std::string FILE_PEER_NOT_ALLOWED = "Cannot define FilePeer for the DataSink where BlockPeer is already defined";
		const std::string BLOCK_PEER_NOT_ALLOWED = "Cannot define BlockPeer for the DataSink where File Peer is already defined";

		const std::string DATASOURCE_NOT_EMPTY = "Cannot delete a non-empty DataSource";
		const std::string FILE_FORMAT_DELETE_NOT_ALLOWED = "Cannot delete FileFormat for the DataSink where FilePeer is defined";
		const std::string LAST_FILE_PEER_DELETE_NOT_ALLOWED = "Cannot delete the only available Peer in the DataSink";
		const std::string ACTIVE_FILE_PEER_DELETE_NOT_ALLOWED = "Cannot delete FilePeer with status ACTIVE";
		const std::string LAST_BLOCK_PEER_DELETE_NOT_ALLOWED = "Cannot delete the only available Peer in the DataSink";
		const std::string ACTIVE_BLOCK_PEER_DELETE_NOT_ALLOWED = "Cannot delete BlockPeer with status ACTIVE";
		const std::string BOTH_PEER_DELETE_NOT_ALLOWED = "Cannot delete both Peers in DataSink";

		const std::string INVALID_DATA_CHUNK_SIZE = "dataChunkSize cannot be smaller than the recordSize";
		const std::string ERR_IMM_INVALID_DATA_LENGTH_TYPE = "dataLengthType cannot be set to EVEN for a DataSink where FilePeer is defined";

		const std::string FILE_FORMAT_INVALID_TEMPLATE = "Invalid template";
		const std::string FILE_FORMAT_INVALID_SEQUENCE_NUMBER = "Invalid startup sequence number";

		const std::string VERIFY_CONNECTION_FAILED = "Action Execution Failure";
		const std::string USEENCRYPTION_MODIFY_RESTRICTED = "Modification of useEncryption is restricted";

		//Validation through model
		const std::string DATASOURCE_ALREADY_DEFINED = "DataSource already defined";
		const std::string DATASOURCE_RECORDSIZE_NOT_CHANGEABLE = "DataSource record size cannot be modified";


	}

	/**
	 * @namespace com_dn
	 *
	 * @brief
	 * Contains the names of classes used for com dn construction.
	 *
	 */
	namespace com_dn
	{
	   const std::string DATASOURCE_DN = "DataSource=%s";
	   const std::string DATASINK_DN = "DataSink=%s";
	   const std::string FILEPEER_DN = "FilePeer=%s";
	   const std::string BLOCKPEER_DN = "BlockPeer=%s";
	   const std::string OUTPUTFORMAT_DN = "OutputFormat=%s";
	   const std::string FILEFORMAT_DN = "FileFormat=%s";
	}

	/**
	 * @namespace oi_name
	 *
	 * @brief
	 * Contains the names of all Object Implementers registered into IMM.
	 *
	 */
	namespace oi_name
	{
		const std::string BLOCK_PEER = "ddtdBlockPeerOi";
		const std::string DATA_SINK = "ddtdDataSinkOi";
		const std::string DATA_SOURCE = "ddtdDataSourceOi";
		const std::string FILE_FORMAT = "ddtdFileFormatOi";
		const std::string FILE_PEER = "ddtdFilePeerOi";
		const std::string OUTPUT_FORMAT = "ddtdOutputFormatOi";
		const std::string TRANSFER_PROGRESS = "ddtdTransferProgressOi";
		const std::string ECIM_PASSWORD = "ddtdEcimPasswordOi";
	}

	/**
	 * @namespace moc_name
	 *
	 * @brief
	 * Contains the names of all Managed Object Classes.
	 *
	 */
	namespace moc_name
	{
		const std::string BLOCK_PEER = "AxeDirectDataTransferBlockPeer";
		const std::string DATA_SINK = "AxeDirectDataTransferDataSink";
		const std::string DATA_SOURCE = "AxeDirectDataTransferDataSource";
		const std::string FILE_FORMAT = "AxeDirectDataTransferFileFormat";
		const std::string FILE_PEER = "AxeDirectDataTransferFilePeer";
		const std::string OUTPUT_FORMAT = "AxeDirectDataTransferOutputFormat";
		const std::string TRANSFER_PROGRESS = "AxeDirectDataTransferTransferProgress";
		const std::string ECIM_PASSWORD = "AxeDirectDataTransferEcimPassword";

		const std::string ACTION_PROGRESS = "AxeDirectDataTransferAsyncActionProgress";

	}


	/**
	 * @namespace mom_root
	 *
	 * @brief
	 * Contains the root name of DDT.
	 *
	 */
	namespace mom_root
	{
		const char DDTM[] = "AxeDirectDataTransferdirectDataTransferMId=1";
	}

	/**
	 * @namespace datasource_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC DataSource.
	 *
	 */
	namespace datasource_attribute
	{
		const std::string RDN = "dataSourceId";
		const std::string ALARMLEVEL = "alarmLevel";
		const std::string RECORDSIZE = "recordSize";
	}

	/**
	 * @namespace transferprogress_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC TransferProgress.
	 *
	 */
	namespace transferprogress_attribute
	{
		const std::string RDN = "transferProgressId";
		const std::string LASTSENTMESSAGE = "lastSentMessage";
		const std::string LASTRECEIVEDMESSAGE = "lastReceivedMessage";
	}

	/**
	 * @namespace datasink_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC DataSink.
	 *
	 */
	namespace datasink_attribute
	{
		const std::string RDN = "dataSinkId";
		const std::string RETRYDELAY = "retryDelay";
		const std::string RETRYATTEMPTS = "retryAttempts";
		const std::string PROGRESSREPORT = "progressReport";
	}

	/**
	 * @namespace peer_attribute
	 *
	 * @brief
	 * Contains the names of all common attributes of MOC BlockPeer and FilePeer.
	 *
	 */
	namespace peer_attribute
	{
		const std::string STATUS = "status";
	}


	/**
	 * @namespace filepeer_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC FilePeer.
	 *
	 */
	namespace filepeer_attribute
	{
		const std::string RDN = "filePeerId";

		const std::string USERNAME = "username";
		const std::string PASSWORD = "password";
		const std::string IPADDRESS = "ipAddress";
		const std::string PORTNUMBER = "portNumber";
		const std::string REMOTEFOLDERPATH = "remoteFolderPath";
		const std::string TRANSFERPROTOCOL = "transferProtocol";

		const std::string STATUS = "status";
	}

	/**
	 * @namespace blockpeer_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC BlockPeer.
	 *
	 */
	namespace blockpeer_attribute
	{
		const std::string RDN = "blockPeerId";

		const std::string IPADDRESS = "ipAddress";

		const std::string STATUS = "status";
	}

	/**
	 * @namespace formatoutput_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC OutputFormat.
	 *
	 */
	namespace outputformat_attribute
	{
		const std::string RDN = "outputFormatId";

		const std::string HOLDTIME = "holdTime";
		const std::string DATACHUNKSIZE ="dataChunkSize";
		const std::string DATALENGTHTYPE = "dataLengthType";
		const std::string PADDINGCHAR = "paddingChar";

	}

	/**
	 * @namespace fileformat_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC OutputFormat.
	 *
	 */
	namespace fileformat_attribute
	{
		const std::string RDN = "fileFormatId";

		const std::string FILESIZE = "fileSize";
		const std::string STARTUP_SEQ_NUM = "startupSequenceNumber";
		const std::string TEMPLATE = "template";
	}

	/**
	 * @namespace ecimpassword_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC EcimPassword.
	 *
	 */
	namespace ecimpassword_attribute
	{
		const std::string RDN = "id";

		const std::string PASSWORD = "password";
		const std::string CLEARTEXT = "cleartext";
	}

	/**
	 * @namespace action
	 *
	 * @brief
	 * Contains the operation ids of all actions supported in DDT related MOCs.
	 *
	 */
	namespace action
	{
		const unsigned int SWITCH_ACTIVE_PEER_OP_ID = 1;
		const unsigned int VERIFY_BLOCK_PEER_CONNECTION_OP_ID = 2;
		const unsigned int VERIFY_FILE_PEER_CONNECTION_OP_ID = 3;

		const std::string RESULT_ATTR_ID = "result";
		const std::string ERROR_TEXT_ATTR_ID = "errorText";
		const std::string ERROR_TEXT_PREFIX = "@ComNbi@";
	}

	/**
	 * @namespace actionprogress_attribute
	 *
	 * @brief
	 * Contains the names of all attributes of MOC AsyncActionProgress.
	 *
	 */
	namespace actionprogress_attribute
	{
		const std::string RDN = "id";
		const std::string NAME = "actionName";
		const std::string ADDITIONAL_INFO = "additionalInfo";
		const std::string PROGRESS_INFO = "progressInfo";
		const std::string PROGRESS_PERCENTAGE = "progressPercentage";
		const std::string RESULT = "result";
		const std::string RESULT_INFO = "resultInfo";
		const std::string STATE = "state";
		const std::string ACTION_ID = "actionId";
		const std::string TIME_STARTED = "timeActionStarted";
		const std::string TIME_COMPLETED = "timeActionCompleted";
		const std::string TIME_OF_LAST_UPDATE = "timeOfLastStatusUpdate";
	}

	namespace actionprogress
	{
		enum ResultType
		{
			SUCCESS = 1,
			FAILURE,
			NOT_AVAILABLE
		};

		enum StateType
		{
			CANCELLING = 1,
			RUNNING,
			FINISHED,
			CANCELLED
		};

		const std::string SWITCHACTIVEPEER_RDN = "progressReport"; //switchActivePeerProgress";
		const std::string SWITCHACTIVEPEER_NAME = "SwitchActivePeer";
		const uint32_t SWITCHACTIVEPEER_ID = 1U;

		namespace percentage
		{
			const uint32_t ZERO = 0U;
			const uint32_t HALF = 50U;
			const uint32_t COMPLETE = 100U;
		}

		namespace resultInfo
		{
			const std::string SUCCESS = "Switch of active peer successfully completed";
			const std::string SECOND_PEER_MISSING = "Additional peer is not defined";
			const std::string SECOND_PEER_FAULTY = "Other Peer is found with status FAULTY";
			const std::string ACTIVE_PEER_MISSING = "No Peer is found with status ACTIVE";
		}
	}

	/**
	 * @namespace parserTag
	 *
	 * @brief
	 * Contains the parser tags.
	 *
	 */
	namespace parserTag
	{
		const char minus = '-';
		const char plus = '+';
		const char comma = ',';
		const char equal = '=';
		const char underLine = '_';
		const char dot = '.';
		const char atSign = ':';
		const std::string singleQuote = "'";
	};

	/**
	 * @namespace com_dn
	 *
	 * @brief
	 * Contains the names of classes used for com dn construction.
	 *
	 */
	namespace com_dn
	{
		const std::string datasource = "DataSource";
		const std::string datasink = "DataSink";
		const std::string blockpeer =  "BlockPeer";
		const std::string filepeer =  "FilePeer";
	}
}

#endif /* DDTADM_CAA_INC_AES_DDT_IMM_H_ */
