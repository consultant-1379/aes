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
#include <string>
#include "stubs/macro_stub.h"

namespace ACE_OS
{
	void *memset (void *s, int c, size_t len)
	{
		return ::memset (s, c, len);
	}

	int poll (struct pollfd *pollfds, unsigned long len, const ACE_Time_Value *timeout)
	{
		return 0;
	}

}
int vtrace (const char * file, int line, const char * function, const char * format, va_list ap)
{
	char trace_msg[AES_DDT_TRACE_MESSAGE_SIZE_MAX] = {0};
	int char_wrote = 0;

	if (file || function || (line > 0))
	{
		char_wrote = ::snprintf(trace_msg, AES_DDT_ARRAY_SIZE(trace_msg), " {");

			if (function)
			{
				char_wrote += ::snprintf(trace_msg + char_wrote, AES_DDT_ARRAY_SIZE(trace_msg) - char_wrote, "%s, ", function);
			}
			if (file)
			{
				char_wrote += ::snprintf(trace_msg + char_wrote, AES_DDT_ARRAY_SIZE(trace_msg) - char_wrote, "%s", file ? (file + 1) : "NO_FILE");
			}

			(line > 0) && (char_wrote += ::snprintf(trace_msg + char_wrote, AES_DDT_ARRAY_SIZE(trace_msg) - char_wrote, "::%d", line));
			char_wrote += ::snprintf(trace_msg + char_wrote, AES_DDT_ARRAY_SIZE(trace_msg) - char_wrote, "} ");
		}

		::vsnprintf(trace_msg + char_wrote, AES_DDT_ARRAY_SIZE(trace_msg) - char_wrote, format, ap);
		trace_msg[AES_DDT_ARRAY_SIZE(trace_msg) - 1] = 0;

		printf("%s", trace_msg);

	return 0;
}

int trace_source (const char * file, int line, const char * function, const char * format, ...)
{
	va_list argp;
	::va_start(argp, format);
	int call_result = vtrace(file, line, function, format, argp);
	::va_end(argp);
	return call_result;
}
