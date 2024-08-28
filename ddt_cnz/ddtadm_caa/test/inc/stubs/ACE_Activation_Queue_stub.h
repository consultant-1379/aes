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
#ifndef INC_ACE_ACTIVATION_QUEUE_STUB_H_
#define INC_ACE_ACTIVATION_QUEUE_STUB_H_

#include "ACE_Method_Request_stub.h"
#include <list>
#include <typeinfo>
#include <string>
#include <iostream>
class ACE_Activation_Queue
{
 public:
	inline ACE_Activation_Queue() : m_requests() {};
	inline virtual ~ACE_Activation_Queue() { };

	inline int enqueue(ACE_Method_Request * request)
	{
		m_requests.push_back(request);

		if((((operation::OperationBase*)request)->m_type) == operation::START)
		{
			return ddt_unittest::CALL_SERVICE_START;
		}
		if((((operation::OperationBase*)request)->m_type) == operation::STOP)
		{
			return ddt_unittest::CALL_SERVICE_STOP;
		}
		if((((operation::OperationBase*)request)->m_type) == operation::DATASOURCE_ADD)
		{
			return ddt_unittest::CALL_DATASOURCE_ADD;
		}
		if((((operation::OperationBase*)request)->m_type) == operation::DATASOURCE_MOD)
		{
			return ddt_unittest::CALL_DATASOURCE_MOD;
		}
		if((((operation::OperationBase*)request)->m_type) == operation::DATASOURCE_DEL)
		{
			return ddt_unittest::CALL_DATASOURCE_DEL;
		}
		if((((operation::OperationBase*)request)->m_type) == operation::SHUTDOWN)
		{
			return ddt_unittest::CALL_DATASOURCE_SHUTDOWN;
		}
		if((((operation::OperationBase*)request)->m_type) == operation::NOOP)
		{
			return ddt_unittest::CALL_NOOP;
		}
		return 0;
	}

	inline ACE_Method_Request* dequeue()
	{
		ACE_Method_Request* request = 0;
		if( !m_requests.empty() )
		{
			request = m_requests.front();
			m_requests.pop_front();

		}
		return request;
	}

	inline bool is_empty() { return m_requests.empty(); }

private:

	std::list<ACE_Method_Request*> m_requests;

};
#endif /* INC_ACE_ACTIVATION_QUEUE_STUB_H_ */
