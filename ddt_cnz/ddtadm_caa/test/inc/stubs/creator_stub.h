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
#ifndef CREATOR_STUB_H_
#define CREATOR_STUB_H_

#include "operation/creatorinterface.h"
#include "operation/shutdown.h"
#include "stubs/ACE_Future_stub.h"

namespace operation {

	class Creator
	{
		friend class Scheduler;
	 public:

		inline Creator() { };

		inline virtual ~Creator() { }

		inline virtual int schedule(const operation::identifier_t id)
		{
			return 0;
		}

		inline virtual int schedule(const operation::identifier_t id, const void* op_details)
		{
			return 0;
		}
		inline virtual int schedule(const operation::identifier_t id, ACE_Future<operation::result>* op_result, const void* op_details)
		{
			return 0;
		}
	 protected:

		inline virtual ACE_Method_Request* make(const operation::identifier_t id)
		{
			ACE_Method_Request* operation;

			switch(id)
			{
				case operation::START:
				{
					operation = new (std::nothrow) ACE_Method_Request(operation::START);
				}
				break;

				case operation::STOP:
				{
					operation = new (std::nothrow) ACE_Method_Request(operation::STOP);
				}
				break;

				case operation::SHUTDOWN:
				{
					operation = new (std::nothrow) Shutdown();
				}
				break;

				default:
				{
					operation = new (std::nothrow) ACE_Method_Request(operation::NOOP);
				}
			}

			return operation;

		}

	 private:

		// Disallow these operations
		Creator(const Creator& rhs);
		Creator& operator=(const Creator& rhs);

	};

} /* namespace operation */




#endif /* CREATOR_STUB_H_ */
