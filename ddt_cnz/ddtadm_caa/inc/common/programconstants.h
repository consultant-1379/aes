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
#ifndef HEADER_GUARD_FILE__aes_ddt_programconstants
#define HEADER_GUARD_FILE__aes_ddt_programconstants aes_ddt_programconstants.h
#include <string>
#include <stdint.h>

namespace common {

	/** @brief DDT Server program codes and enumerations.
	 *
	 */
#define AES_DDT_PROGRAM_RETURN_CODE_CONSTANTS_BASE 0

	/**
	 * @brief
	 * This enumeration specifies various Transfer Protocol constants
	 */
	enum TransferProtocolConstants {
		PROTOCOL_SFTP = 0,
		PROTOCOL_FTP = 1,
		PROTOCOL_BGWRPC = 2,


		FTP_DEFAULT_PORT = 21,
		SFTP_DEFAULT_PORT = 22
	};

	/**
	 * @brief
	 * This enumeration specifies various program return code constants
	 */
	enum ProgramReturnCodeConstants {
		PROGRAM_EXIT_OK = AES_DDT_PROGRAM_RETURN_CODE_CONSTANTS_BASE,
		PROGRAM_EXIT_ANOTHER_SERVER_RUNNING,
		PROGRAM_EXIT_BAD_INVOCATION,
		PROGRAM_EXIT_LOCK_FILE_OPEN_ERROR,
		PROGRAM_EXIT_LOCK_FILE_LOCKING_ERROR,
		PROGRAM_EXIT_IMM_MANAGEMENT_SESSION_OPEN_FAILURE,
		PROGRAM_EXIT_MEMORY_ALLOCATION_ERROR
	};

	/** @brief Program State constants constants.
	 *
	 *	ProgramStateConstants enumeration detailed description
	 */
	enum ProgramStateConstants {
		PROGRAM_STATE_RUNNING = 0,
		PROGRAM_STATE_EXIT_PROGRAM,
		PROGRAM_STATE_RESTART_FUNCTION,
		PROGRAM_STATE_STOP_WORKING,
		PROGRAM_STATE_RUNNING_ACTIVE,
		PROGRAM_STATE_RUNNING_PASSIVE,
		PROGRAM_STATE_RUNNING_QUIESCING,
		PROGRAM_STATE_RUNNING_QUIESCED,
		PROGRAM_STATE_RUNNING_UNDEFINED
	};

	namespace errorCode
	{
		/**
		 * @brief
		 * This enumeration specifies various error constants
		 */
		enum ErrorConstants
		{
			ERR_API_CALL	= -1,
			ERR_NO_ERRORS	= 0,

			// Add here new error codes
			ERR_GENERIC,
			ERR_OPEN,
			ERR_WRITE,
			ERR_CLOSE,
			ERR_SVC_ACTIVATE,
			ERR_SVC_DEACTIVATE,
			ERR_MEMORY_BAD_ALLOC,
			ERR_REACTOR_HANDLER_REGISTER_FAILURE,
			ERR_REACTOR_HANDLER_REMOVE_FAILURE,
			ERR_DATASOURCE_ALREADY_DEFINED,

			ERR_CKPT_OPEN_FAILURE,
			ERR_CKPT_CLOSE_FAILURE,
			ERR_CKPT_STOPPED,
			ERR_CKPT_DELETE_FAILURE,
			ERR_CKPT_WAIT_COMMIT,
			ERR_CKPT_GETNEW_FAILURE,
			ERR_CKPT_SECTIONS_EMPTY,
			ERR_CKPT_BAD_USAGE,

			ERR_CKPT_INFOSECTION_CREATE_FAILURE,
			ERR_CKPT_INFOSECTION_READ_FAILURE,
			ERR_CKPT_INFOSECTION_WRITE_FAILURE,
			ERR_CKPT_INFOSECTION_CORRUPTED,
			ERR_CKPT_SECTION_CREATE_FAILURE,
			ERR_CKPT_SECTION_READ_FAILURE,
			ERR_CKPT_SECTION_WRITE_FAILURE,

			ERR_CKPT_SECTIONS_AVAILABLE,
			ERR_CKPT_NOSECTIONS_TRYAGAIN,
			ERR_CKPT_SECTIONS_FULL,

			ERR_CKPT_ITERATOR_FAILURE,
			ERR_CKPT_ITERATOR_INIT_FAILURE,
			ERR_CKPT_ITERATOR_FIN_FAILURE,

			ERR_MTAPSESSION_ADDING_FAILURE,
			ERR_NULL_POINTER,
			ERR_MEMORY_LIMIT_REACHED,
			ERR_WRITER_BAD_USAGE,
			ERR_WRITER_FAILURE,
			ERR_GET_WRITER_FAILURE,

			ERR_READER_BAD_USAGE,
			ERR_READER_FAILURE,

			ERR_PEER_NOT_FOUND,
			ERR_NO_VALID_PEER,

			ERR_OPENSSL_WRAPPER_FAILURE,

			// RPC
			ERR_RPC_GETLAST_UNHANDLED,
			ERR_RPC_SEND_UNHANDLED,
			ERR_RPC_SEND_DUPLICATE,

			ERR_HW_NOTSUPPORTED,

			//////////////////////////////////
			
			ERR_IMM_NOT_ENOUGH_PEER,
			ERR_IMM_PEER_TYPE_MISMATCH,
			ERR_IMM_MISSING_FILE_FORMAT,
			ERR_IMM_FILE_FORMAT_NOT_ALLOWED,
			ERR_IMM_FILE_PEER_NOT_ALLOWED,
			ERR_IMM_BLOCK_PEER_NOT_ALLOWED,

			ERR_IMM_NOT_EMPTY_DATASOURCE,
			ERR_IMM_FILE_FORMAT_DELETE_NOT_ALLOWED,
			ERR_IMM_LAST_FILE_PEER_DELETE_NOT_ALLOWED,
			ERR_IMM_ACTIVE_FILE_PEER_DELETE_NOT_ALLOWED,
			ERR_IMM_LAST_BLOCK_PEER_DELETE_NOT_ALLOWED,
			ERR_IMM_ACTIVE_BLOCK_PEER_DELETE_NOT_ALLOWED,
			ERR_IMM_BOTH_PEER_DELETE_NOT_ALLOWED,
			ERR_IMM_ACA_MESSAGESTORE_NAME_CONFLICT,

			ERR_IMM_INVALID_DATA_CHUNK_SIZE,
			ERR_IMM_INVALID_DATA_LENGTH_TYPE,
			ERR_IMM_INVALID_SEQUENCE_NUMBER,
			ERR_IMM_INVALID_TEMPLATE
			// End error value definitions
		};
	}

	namespace event
	{
		const int INVALID = -1;
		const unsigned int INITIAL_VALUE = 0U;
		const int FLAGS = 0U; //In Linux up to version 2.6.26, the flags argument is unused, and must be specified as zero
	}

	namespace handle
	{
		const int INVALID = -1;
	}

	namespace datatransfer
	{
		const std::string TEMPORARY_FILE_EXTENSION(".tmp");
		const int ASSEMBLING_ERROR_TIMOUT_SECONDS = 30;
	}

	/**
	 * @brief
	 * This enumeration specifies various hardware version constants
	 */
	enum HwVerConstants
	{
		HWVER_NOVALUE = 0,
		HWVER_VM = 1,
		HWVER_GEP1 = 2,
		HWVER_GEP2 = 3,
		HWVER_GEP5 = 4
	};

	/**
	 * @brief
	 * This enumeration specifies various data length types
	 */
	enum DataLenghtType
	{
		FIXED = 0, 	//Fixed means that data length is arbitrarily fixed to a maximum value:
					//output data is formatted using the same amount of memory whether this maximum is reached or not.
					//If needed padding characters are added to match the fixed length

		VARIABLE,	//Variable means that data length is not arbitrarily fixed:
					// output data is formatted using varying amounts of memory depending on the actual size.
					// No padding characters are added to the output data.

		EVEN		//Even means that data length is not arbitrarily fixed, but it is always even.
					//If needed a padding character is added to match the even length.
	};

}

#endif // HEADER_GUARD_FILE__aes_ddt_programconstants
