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

#ifndef DDTADM_CAA_INC_AES_DDT_OPERATION_H_
#define DDTADM_CAA_INC_AES_DDT_OPERATION_H_

#include "common/programconstants.h"
#include "imm/objectmanagerhelper.h"

#include <string>
#include <stdint.h>

namespace operation
{

	/// Operation Identifiers
	enum identifier_t {
		NOOP,
		START,  ///< Activate
		STOP,	///< Deactivate
		SHUTDOWN,	///< Terminate

		DATASOURCE_ADD,
		DATASOURCE_DEL,
		DATASOURCE_MOD,
		TRANSFERPROGRESS_ADD,

		DATASINK_ADD,
		DATASINK_DEL,
		DATASINK_MOD,

		BLOCKPEER_ADD,
		BLOCKPEER_DEL,
		BLOCKPEER_MOD,

		FILEPEER_ADD,
		FILEPEER_DEL,
		FILEPEER_MOD,

		OUTPUTFORMAT_ADD,
		OUTPUTFORMAT_DEL,
		OUTPUTFORMAT_MOD,

		FILEFORMAT_ADD,
		FILEFORMAT_DEL,
		FILEFORMAT_MOD,

		SWITCHACTIVEPEER_ACTION,

	};

	namespace changeMask
	{
		const uint16_t RECORDSIZE_CHANGE		= 0x0001;
		const uint16_t ALARMLEVEL_CHANGE		= 0x0002;

		const uint16_t IPADDRESS_CHANGE			= 0x0004;

		const uint16_t RETRYATTEMPTS_CHANGE		= 0x0008;
		const uint16_t RETRYDELAY_CHANGE		= 0x0010;

		const uint16_t DATACHUNKSIZE_CHANGE		= 0x0020;
		const uint16_t DATALENGHTTYPE_CHANGE	= 0x0040;
		const uint16_t HOLDTIME_CHANGE			= 0x0080;
		const uint16_t PADDINGCHAR_CHANGE		= 0x0100;

		const uint16_t FILESIZE_CHANGE			= 0x0200;
		const uint16_t FILE_TEMPLATE_CHANGE		= 0x0400;
		const uint16_t FILE_SEQ_NUMBER_CHANGE	= 0x0800;

		const uint16_t PASSWORD_CHANGE			= 0x1000;
		const uint16_t PORT_NUMBER_CHANGE		= 0x2000;
		const uint16_t FOLDER_PATH_CHANGE		= 0x4000;
		const uint16_t USERNAME_CHANGE			= 0x8000;

	}

	// Operation result
	struct result
	{
		int errorCode;
		std::string errorMessage;

		result(): errorCode(common::errorCode::ERR_NO_ERRORS), errorMessage() {}

		void set(const int& errorValue, const std::string& errMsg)
		{
			errorCode = errorValue;
			errorMessage.assign(errMsg);
		}

		const char* getErrorMessage() const { return errorMessage.c_str(); }

		int getErrorCode() const { return errorCode; }

		void setErrorCode(const int& errCode) {  errorCode = errCode; }

		bool good() { return (common::errorCode::ERR_NO_ERRORS == errorCode); }

		bool fail() { return (common::errorCode::ERR_NO_ERRORS != errorCode); }
	};

	namespace EncryptionStatus
	{
		enum options
		{
			FALSE,
			TRUE
		};
	}

	/** @brief	Holds DataSource MO data
	 */
	struct dataSourceInfo
	{
		std::string name;
		std::string moDN;
		uint32_t recordSize;
		uint32_t alarmLevel;

		uint16_t changeMask;

		dataSourceInfo() : name(), moDN(), recordSize(0U), alarmLevel(0U), changeMask(0U) {};

		dataSourceInfo(const dataSourceInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			recordSize = rhs.recordSize;
			alarmLevel = rhs.alarmLevel;

			changeMask = rhs.changeMask;
		}

		dataSourceInfo& operator=(const dataSourceInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			recordSize = rhs.recordSize;
			alarmLevel = rhs.alarmLevel;

			changeMask = rhs.changeMask;

			return *this;
		}

		const char* getName() const { return name.c_str(); };
		const char* getDataSourceName() const { return name.c_str(); };
		const char* getDataSourceDN() const { return moDN.c_str(); };
	};

	/** @brief	Holds DataSink MO data
	 */
	struct dataSinkInfo
	{
		std::string name;
		std::string moDN;
		std::string dataSourceName;

		uint32_t retryDelay;
		uint32_t retryAttempts;

		uint16_t changeMask;

		dataSinkInfo() : name(), moDN(), dataSourceName(), retryDelay(0U), retryAttempts(0U), changeMask(0U) {};

		dataSinkInfo(const dataSinkInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSourceName = rhs.dataSourceName;
			retryDelay = rhs.retryDelay;
			retryAttempts = rhs.retryAttempts;

			changeMask = rhs.changeMask;
		}

		dataSinkInfo& operator=(const dataSinkInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSourceName = rhs.dataSourceName;
			retryDelay = rhs.retryDelay;
			retryAttempts = rhs.retryAttempts;

			changeMask = rhs.changeMask;

			return *this;
		}

		const char* getName() const { return name.c_str(); };
		const char* getDataSinkDN() const { return moDN.c_str(); };
		const char* getDataSourceName() const { return dataSourceName.c_str(); };
	};

	/** @brief	holds File Peer MO data
	 */
	struct filePeerInfo
	{
		std::string name;
		std::string moDN;
		std::string dataSinkName;
		std::string dataSourceName;

		std::string ipAddress;
		std::string username;
		std::string passwordDN;

		std::string remoteFolderPath;
		uint32_t portNumber;
		int32_t status;
		int32_t transferProtocol;

		uint16_t changeMask;

	  private:
		std::string password;

	  public:
		filePeerInfo() : name(), moDN(), dataSinkName(), dataSourceName(), ipAddress(),  username(),
						 remoteFolderPath(),  portNumber(0U), status(0U), transferProtocol(0U), changeMask(0U), password() {};

		filePeerInfo(const filePeerInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;

			ipAddress = rhs.ipAddress;
			username = rhs.username;
			password = rhs.password;
			passwordDN = rhs.passwordDN;
			remoteFolderPath = rhs.remoteFolderPath;
			portNumber = rhs.portNumber;
			status = rhs.status;
			transferProtocol = rhs.transferProtocol;

			changeMask = rhs.changeMask;
		}

		filePeerInfo& operator=(const filePeerInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;

			ipAddress = rhs.ipAddress;
			username = rhs.username;
			password = rhs.password;
			passwordDN = rhs.passwordDN;
			remoteFolderPath = rhs.remoteFolderPath;
			portNumber = rhs.portNumber;
			status = rhs.status;
			transferProtocol = rhs.transferProtocol;

			changeMask = rhs.changeMask;
			return *this;
		}

		void update(const filePeerInfo& rhs)
		{
			if(rhs.changeMask & operation::changeMask::USERNAME_CHANGE)
			{
				username.assign(rhs.username);
			}

			if(rhs.changeMask & operation::changeMask::PASSWORD_CHANGE)
			{
				passwordDN.assign(rhs.passwordDN);
				clearPassword();
			}

			if(rhs.changeMask & operation::changeMask::PORT_NUMBER_CHANGE)
			{
				portNumber = rhs.portNumber;
			}

			if(rhs.changeMask & operation::changeMask::FOLDER_PATH_CHANGE)
			{
				remoteFolderPath.assign(rhs.remoteFolderPath);
			}

			changeMask |= rhs.changeMask;
		}

		void reset()
		{
			changeMask = 0;
		}

		bool empty()
		{
			return (0 == changeMask);
		}

		const char* getName() const { return name.c_str(); };
		const char* getFilePeerDN() const { return moDN.c_str(); };

		const char* getDataSourceName() const { return dataSourceName.c_str(); };
		const char* getDataSinkName() const { return dataSinkName.c_str(); };

		uint32_t getPortNumber() const { return portNumber; };
		const char* getIpAddress() const { return ipAddress.c_str(); };
		const char* getRemoteFolderPath() const { return remoteFolderPath.c_str(); };
		int32_t getTransferProtocol() const { return transferProtocol; };

		const char* getUserName() const { return username.c_str(); };

		const char* getPassword()
		{
			if( password.empty() )
			{
				// TODO : check the returned error code!!
				imm::objectManagerHelper_t::instance()->getPassword(passwordDN, password );
			}

			return password.c_str();
		};

		void clearPassword()
		{
			password.clear();
		}


	};

	/** @brief	holds Block Peer MO data
	 */
	struct blockPeerInfo
	{
		std::string name;
		std::string moDN;
		std::string dataSinkName;
		std::string dataSourceName;

		std::string ipAddress;
		int32_t status;
		int32_t transferProtocol;

		uint16_t changeMask;

		blockPeerInfo() : name(), moDN(), dataSinkName(), dataSourceName(), ipAddress(), status(0), transferProtocol(common::PROTOCOL_BGWRPC), changeMask(0U) {};

		blockPeerInfo(const blockPeerInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;

			ipAddress = rhs.ipAddress;
			status = rhs.status;
			transferProtocol = rhs.transferProtocol;

			changeMask = rhs.changeMask;
		}

		blockPeerInfo& operator=(const blockPeerInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;

			ipAddress = rhs.ipAddress;
			status = rhs.status;
			transferProtocol = rhs.transferProtocol;

			changeMask = rhs.changeMask;
			return *this;
		}

		const char* getName() const { return name.c_str(); };
		const char* getBlockPeerDN() const { return moDN.c_str(); };

		const char* getDataSourceName() const { return dataSourceName.c_str(); };
		const char* getDataSinkName() const { return dataSinkName.c_str(); };

		const char* getIpAddress() const {return ipAddress.c_str(); };

	};

	/** @brief	holds OutputFormat MO data
	 */
	struct outputFormatInfo
	{
		std::string name;
		std::string moDN;
		std::string dataSinkName;
		std::string dataSourceName;

		uint32_t holdTime;
		uint32_t dataChunkSize;
		int32_t dataLengthType;
		uint32_t paddingChar;

		uint16_t changeMask;

		outputFormatInfo() : name(), moDN(), dataSinkName(), dataSourceName(), holdTime(0U),
						  dataChunkSize(0U), dataLengthType(0U), paddingChar(0U), changeMask(0U) {};

		outputFormatInfo(const outputFormatInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;

			holdTime = rhs.holdTime;
			dataChunkSize = rhs.dataChunkSize;
			dataLengthType = rhs.dataLengthType;
			paddingChar = rhs.paddingChar;

			changeMask = rhs.changeMask;
		}

		outputFormatInfo& operator=(const outputFormatInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;

			holdTime = rhs.holdTime;
			dataChunkSize = rhs.dataChunkSize;
			dataLengthType = rhs.dataLengthType;
			paddingChar = rhs.paddingChar;

			changeMask = rhs.changeMask;

			return *this;
		}

		const char* getName() const { return name.c_str(); };
		const char* getOutputFormatDN() const { return moDN.c_str(); };

		const char* getDataSourceName() const { return dataSourceName.c_str(); };
		const char* getDataSinkName() const { return dataSinkName.c_str(); };

	};

	/** @brief	holds FileFormat MO data
	 */
	struct fileFormatInfo
	{
		std::string name;
		std::string moDN;
		std::string dataSinkName;
		std::string dataSourceName;
		std::string outputFormatName; // value component of OutputFormat RDN

		std::string templateValue;
		uint32_t fileSize;			// { 64..32768 }
		uint32_t startupSequenceNumber;

		uint16_t changeMask;

		fileFormatInfo() : name(), moDN(), dataSinkName(), dataSourceName(),
				templateValue(), fileSize(0U), startupSequenceNumber(0U), changeMask(0U) {};

		fileFormatInfo(const fileFormatInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;
			outputFormatName = rhs.outputFormatName;

			templateValue = rhs.templateValue;
			fileSize = rhs.fileSize;
			startupSequenceNumber = rhs.startupSequenceNumber;

			changeMask = rhs.changeMask;
		}

		fileFormatInfo& operator=(const fileFormatInfo& rhs)
		{
			name = rhs.name;
			moDN = rhs.moDN;
			dataSinkName = rhs.dataSinkName;
			dataSourceName = rhs.dataSourceName;
			outputFormatName = rhs.outputFormatName;

			templateValue = rhs.templateValue;
			fileSize = rhs.fileSize;
			startupSequenceNumber = rhs.startupSequenceNumber;

			changeMask = rhs.changeMask;
			return *this;
		}

		const char* getName() const { return name.c_str(); };
		const char* getFileFormatDN() const { return moDN.c_str(); };

		const char* getDataSourceName() const { return dataSourceName.c_str(); };
		const char* getDataSinkName() const { return dataSinkName.c_str(); };
		const char* getOutputFormatName() const { return outputFormatName.c_str(); };

	};


	/** @brief	Struct of TransferProgress elements
	 */
	struct transferProgressInfo
	{
		std::string dataSourceName;
		std::string producerName;
		std::string dataSourceDN;

		transferProgressInfo() : dataSourceName(), producerName(), dataSourceDN() {};
		const char* getDataSourceName() const { return dataSourceName.c_str(); };
		const char* getProducerName() const { return producerName.c_str(); };
		const char* getDataSourceDN() const { return dataSourceDN.c_str(); };

	};

	/** @brief	Struct of switchActivePeerInfo elements
	 */
	struct switchActivePeerInfo
	{
		std::string dataSourceName;
		std::string dataSinkDN;

		switchActivePeerInfo() : dataSourceName(), dataSinkDN() {};
		switchActivePeerInfo(const char* sinkDN) : dataSourceName(), dataSinkDN(sinkDN) {};

		const char* getDataSourceName() const { return dataSourceName.c_str(); };
		const char* getDataSinkDN() const { return dataSinkDN.c_str(); };

		switchActivePeerInfo(const switchActivePeerInfo& rhs)
		{
			dataSourceName.assign(rhs.dataSourceName);
			dataSinkDN.assign(rhs.dataSinkDN);
		}

		switchActivePeerInfo& operator=(const switchActivePeerInfo& rhs)
		{
			dataSourceName.assign(rhs.dataSourceName);
			dataSinkDN.assign(rhs.dataSinkDN);
			return *this;
		}

	};
}


#endif /* DDTADM_CAA_INC_AES_DDT_OPERATION_H_ */
