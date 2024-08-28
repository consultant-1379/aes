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
#ifndef INC_WORKINGSET_STUB_H_
#define INC_WORKINGSET_STUB_H_

#include <new>
#include "operation/operation.h"
#include "stubs/scheduler_stub.h"
#include "stubs/store_stub.h"
#include "stubs/encryption_stub.h"

namespace engine
{
	class Context;
	const uint32_t TEST_DEFAULT_RECORDSIZE = 2048U;

	class AES_DDT_DataSourceManager_Stub
	{
	  public:
		AES_DDT_DataSourceManager_Stub(): m_Result(true) {
			m_Result = 0;
		};

		virtual ~AES_DDT_DataSourceManager_Stub() {};

		bool loadFromImm() { return m_Result; };

		int start() { return common::errorCode::ERR_NO_ERRORS; }

		int stop()	{ return 0; };
		uint32_t getDataSourceRecordSize(const std::string& dataSourceName) const {
			return TEST_DEFAULT_RECORDSIZE;
		}
		int add(const operation::dataSourceInfo& data, operation::result& op_result, bool startTask = true)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		};
		int remove(const operation::dataSourceInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		};
		int modify(const operation::dataSourceInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		};
		/**
		 *	@brief	Adds a new Data Sink
		 */
		int add(const operation::dataSinkInfo& data, operation::result& op_result, bool startTask = true)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}

		/**
		 *	@brief	Adds a new File Peer
		 */
		int add(const operation::filePeerInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Adds a new Block Peer
		 */
		int add(const operation::blockPeerInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Adds a new Output Format
		 */
		int add(const operation::outputFormatInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Adds a new File Format
		 */
		int add(const operation::fileFormatInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Removes a Data Sink
		 */
		int remove(const operation::dataSinkInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Removes a File Peer
		 */
		int remove(const operation::filePeerInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Removes a Block Peer
		 */
		int remove(const operation::blockPeerInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Removes the Output Format
		 */
		int remove(const operation::outputFormatInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		/**
		 *	@brief	Removes the File Format
		 */
		int remove(const operation::fileFormatInfo& data, operation::result& op_result)
		{
			op_result.errorCode = common::errorCode::ERR_NO_ERRORS;
			return op_result.errorCode;
		}
		Context* getContext(const std::string& dataSourceName) const
		{
			Context* context = NULL;
			return context;
		};
	  private:

		bool m_Result;

	};

	class WorkingSet
	{
	 public:

		inline WorkingSet()
		: m_debugMode(1),
		  m_dataSourceManager(),
		  m_IMMResult(true)
		  {}

		inline virtual ~WorkingSet() {  }


		static WorkingSet* instance();

		inline int stopMainReactor() { return 0; };

		inline bool isDebugModeOn() const { return m_debugMode; };
		inline void setDebugModeOn() {m_debugMode = true; };
		inline void setDebugModeOff() {m_debugMode = false; };
		inline AES_DDT_OperationScheduler& getScheduler()  { return m_scheduler; };

		inline int registerObjectImplementers() { return m_IMMResult; };
		inline int unregisterObjectImplementers() { return  m_IMMResult; };

		inline engine::AES_DDT_DataSourceManager_Stub& getDataSourceManager()  { return m_dataSourceManager; };
		inline store::StoringManager getStoringManager()  { return m_storingManager; };
		inline cipher::CipherManager getCipherManager()  { return m_cipherManager; };

		// To set the wanted IMM result
		void setIMMResult(bool value) { m_IMMResult = value; };
		inline ACE_Reactor& getMainReactor()  { return m_reactor; };;

		// To clean-up from next test
		void destroy();

	 private:
		// INHIBIT COPY CONTRUCTOR
		WorkingSet(const WorkingSet& rhs);

		// INHIBIT ASSIGNMENT OPERATOR
		WorkingSet& operator=(const WorkingSet& rhs);


	 private:
		static WorkingSet *m_workingSet;
		bool m_debugMode;
		ACE_Reactor	m_reactor;
		engine::AES_DDT_DataSourceManager_Stub m_dataSourceManager;
		AES_DDT_OperationScheduler m_scheduler;
		store::StoringManager m_storingManager;
		cipher::CipherManager m_cipherManager;

		// To set IMM register/unregister result
		bool m_IMMResult;
	};

	typedef WorkingSet workingSet_t;


} //namespace



#endif /* INC_WORKINGSET_STUB_H_ */
