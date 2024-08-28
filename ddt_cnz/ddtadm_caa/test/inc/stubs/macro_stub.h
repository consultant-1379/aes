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
#ifndef INC_MACRO_STUB_H_
#define INC_MACRO_STUB_H_

#include <stdio.h>
//#include <sys/syscall.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdarg.h>
#include <errno.h>
#include <poll.h>
#include "stubs/ACE_Task_Base_stub.h"

typedef unsigned int uint32_t;
typedef short unsigned int uint16_t;
typedef unsigned char uint8_t;

namespace ACE_OS
{
	inline int strcmp(const char * chstr1, const char* chstr2)
	{
		return ::strcmp(chstr1,chstr2);
	};

	void *memset (void *s, int c, size_t len);

	int poll (struct pollfd *pollfds, unsigned long len, const ACE_Time_Value *timeout);

}

#define AES_DDT_TRACE_MESSAGE_SIZE_MAX 512
#define AES_DDT_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))

#define AES_DDT_TRACE_DEFINE(tag) \
	int tag = 1;

#define AES_DDT_TRACE_FUNCTION trace_source(__FILE__, __LINE__,__func__, "Entering function");


#define AES_DDT_LOG_ERRNO(...) ;


#define AES_DDT_LOG(...) ;
#define AES_DDT_TRACE_MESSAGE(...) AES_DDT_TRACE_MESSAGE_SOURCE_IMPL(__FILE__, __LINE__,__func__, __VA_ARGS__)
#define AES_DDT_TRACE_MESSAGE_SOURCE_IMPL(file, line, function, ...) trace_source(file, line, function, __VA_ARGS__)

int vtrace (const char * file, int line, const char * function, const char * format, va_list ap);
int trace_source (const char * file, int line, const char * function, const char * format, ...);
#endif /* INC_MACRO_STUB_H_ */
