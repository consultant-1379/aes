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
#ifndef DDTADM_CAA_INC_AES_DDT_MTAP_PROTOCOL_H_
#define DDTADM_CAA_INC_AES_DDT_MTAP_PROTOCOL_H_

#include <stdint.h>
#include <unistd.h>

namespace mtap
{
	namespace protocol
	{
		//---------------------------------------------------------------
		// Default Protocol Parameters are set to the recommended values
		//---------------------------------------------------------------

		//Supported protocol version
		static const unsigned char SUPPORTED_VERSION_3 = 3;

		// winSize defines the number of records that cna be sent from MTAP without being acknowledged. Range: 1 - 32
		static const uint32_t DEFAULT_WIN_SIZE = 32U;

		// noOfConn defines the number of connections to be used during a session. The range is: 0 - 32
		static const uint32_t DEFAULT_NUM_OF_CONNECTIONS = 8U;

		// noOfRecords defines the number of records to be stored in each message file. The range is: 0 - 65535
		static const uint32_t DEFAULT_NUM_OF_RECORDS = 450U;

		// recordSize defines the maximum size of a record in bytes. The range is: 512 - 65535
		static const uint32_t DEFAULT_RECORD_SIZE = 2048U;

		// noOfFiles defines the maximum number of files that can exist in the Data Source. The range is: 1 - 2048
		static const uint32_t DEFAULT_NUM_OF_FILES = 40U;

		// alarmLevel defines at what storage level in the message store an alarm will be initiated. The range is: 1 - 100
		static const uint32_t DEFAULT_ALARM_LEVEL = 25U;

		// noOfResends defines the number ot times to resend a record. The range is: 0 - 255
		static const uint32_t DEFAULT_NUM_OF_RESENDS = 15U;

		// resendTime defines the total time before MTAP gives up its attempts to send a record and sends APERROR to the CP application.
		//				The time between the attempts is (resendTime / noOfResends). The value is given in seconds. Range: 0 - 3600
		static const uint32_t DEFAULT_RESEND_TIME = 150U;

		// reconnTime defines the time to wait before a reconnect attempt is made of a transport channel after it has been spontaneously disconnected.
		//				The value is given in seconds. Range: 0 - 60
		static const uint32_t DEFAULT_RECONN_TIME = 10U;

		// File Aging Value (FAV) represents the maximum life time of a file in minutes before it is closed by MTAP and a new one is created.
		//				The value is given in minutes. 0 means the life time is infinite. Possible values are 0 - 1440
		static const uint32_t DEFAULT_FAV = 0U;


		// echo fefines if the echo-check primitive will be used or no. The range is: 0 - 1. 0 stands for No. 1 stands for Yes.
		static const uint32_t DEFAULT_ECHO_MODE = 0U; // 0 = No, 1 = Yes


		//----------------------------------------
		// Size in Bytes of the protocol fileds
		//----------------------------------------
		static const ssize_t PRIMITIVE_ID_SIZE = 4;


		//PrimitiveIdentifier
		namespace primitive
		{
			//PeerClosedConnection = -2,
			//Primitive_NotSpecified = -1,
			static const unsigned char ERROR = 0x00;
			static const unsigned char PUT_RECORD = 0x01;
			static const unsigned char UNLINK_FILE = 0x02;
			static const unsigned char UNLINK_ALL_FILES = 0x03;
			static const unsigned char ECHO_CHECK = 0x04;
			static const unsigned char GET_PARAMETER = 0x05;
			static const unsigned char SYNCHRONIZE = 0x06;
			static const unsigned char STOP = 0x07;
			static const unsigned char SKIP = 0x08;
			static const unsigned char DO_WILL = 0x09;
			static const unsigned char DO_NOT_WILL_NOT = 0x0A;
			//Primitive_PutExpedRecord = 11
		}

		enum CommandCode
		{
			//Error
			Command_Error = 0,

			//Put-record
			Command_PutRecord = 1,

			//Unlink-file
			Command_UnlinkFile = 2,

			//Unlink-all-files
			Command_UnlinkAllFiles = 3,

			//Echo-check
			Command_EchoCheck = 4,

			//Get-parameter
			Command_GetParameter = 5,

			//Synchronize
			Command_Synchronize = 6,

			//Stop
			Command_Stop = 7,

			//Skip
			Command_Skip = 8,

			//Do/Will
			Command_Do_Will = 9,

			//Do not/Will not
			Command_Do_not_Will_not = 10
		};

		enum ParameterCode
		{
			//winSize
			Parameter_WinSize = 0x00,

			//noOfConn
			Parameter_NoOfConn = 0x01,

			//noOfRecords
			Parameter_NoOfRecords = 0x02,

			//noOfResends
			Parameter_NoOfResends = 0x03,

			//resendTime
			Parameter_ResendTime = 0x04,

			//reconnTime
			Parameter_ReconnTime = 0x05,

			//FAV
			Parameter_FAV = 0x06,

			//echo
			Parameter_Echo = 0x07,

			//RecordSize
			Parameter_RecordSize = 0x08
		};

		enum Option
		{
			Option_Version = 0,
			Option_Unknown = 255
		};

		enum ErrorCode
		{
			ERR_NO_ERROR = 0,
			ERR_ACCESS_TO_FILE_DENIED = 1,
			ERR_BAD_FILE_NUMBER = 2,
			ERR_QUOTA_EXCEEDED = 3,
			ERR_FILE_TOO_BIG = 4,
			ERR_FILE_TABLE_OVERFLOW_IN_KERNEL = 5,
			ERR_DISK_SPACE_EXHAUSTED = 6,
			ERR_MESSAGE_STORE_FULL = 7,
			ERR_RECORD_OUT_OF_RANGE = 8,
			ERR_PARAMETER_NOT_DEFINED = 9,
			ERR_UNKNOWN_PARAMETER = 10,
			ERR_RECORD_TOO_LARGE = 11,
			ERR_ILLEGAL_PRIMITIVE = 12,
			ERR_COMMUNICATION_ERROR = 13,
			ERR_UNKNOWN_COMMAND = 14,
			ERR_ILLEGAL_CP_SITE = 15,
			ERR_TOO_MANY_CONNECTIONS = 16,


			//-----------------------
			// Application Specific
			ERR_MALFORMED_PACKAGE = 100,
			ERR_RECORD_ALREADY_STORED = 101,

			ERR_UNSPECIFIED = 255
		};

		//****************************************************************
		// Convert an unsigned int to an AXE 4-bytes value, and place it
		// in the buffer.
		//****************************************************************
		void put4bytes(unsigned char*& buffer, uint32_t value);
	}
}


#endif /* DDTADM_CAA_INC_AES_DDT_MTAP_PROTOCOL_H_ */
