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
#ifndef SRC_ACE_METHOD_REQUEST_STUB_H_
#define SRC_ACE_METHOD_REQUEST_STUB_H_
#include "stubs/ACE_Future_stub.h"
#include "operation/operation.h"

namespace ddt_unittest
{
enum CALLTYPE {
	CALL_SERVICE_START=1,
	CALL_SERVICE_STOP = 2,
	CALL_DATASOURCE_ADD = 3,
	CALL_DATASOURCE_DEL = 4,
	CALL_DATASOURCE_MOD = 5,
	CALL_DATASOURCE_SHUTDOWN = 6,
	CALL_NOOP = 7
};
}

class ACE_Method_Request
{
public:
	ACE_Method_Request(operation::identifier_t type):m_type(type){;}
	inline virtual ~ACE_Method_Request() {;};
	inline virtual int call() { return 0; }
	/// Operation type
	operation::identifier_t m_type;

};
namespace operation
{
class OperationBase : public ACE_Method_Request
{
public:
	OperationBase(operation::identifier_t type):ACE_Method_Request(type),m_type(type), m_operationResult(), m_result(NULL) {};
	//AES_DDT_OperationBase():ACE_Method_Request() {};
	virtual ~OperationBase() {}
	void setOperationResultRequest(ACE_Future<operation::result>* op_result){m_result = op_result;}
	void setOperationDetails(const void* op_details) {;};
	inline virtual int call() { return 0; }

	inline void setResultToCaller() {if(m_result) m_result->set(m_operationResult);};
	/// Operation type
	operation::identifier_t m_type;
	/// Operation result
	operation::result m_operationResult;

private:

	/// Operation result caller callback
	ACE_Future<operation::result>* m_result;
};
}

#endif /* SRC_ACE_METHOD_REQUEST_STUB_H_ */
