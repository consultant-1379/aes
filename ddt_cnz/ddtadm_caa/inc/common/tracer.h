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
#ifndef HEADER_GUARD_FILE__aes_ddt_tracer
#define HEADER_GUARD_FILE__aes_ddt_tracer aes_ddt_tracer.h

#include <unistd.h>
#include <inttypes.h>
#include <stdarg.h>

#include "ACS_TRA_trace.h"


#ifndef AES_DDT_ARRAY_SIZE
#	define AES_DDT_ARRAY_SIZE(array) (sizeof(array)/sizeof(*(array)))
#endif

#ifndef AES_DDT_STRINGIZER
#	define AES_DDT_STRINGIZER(s) #s
#	define AES_DDT_STRINGIZE(s) AES_DDT_STRINGIZER(s)
#endif


#ifdef AES_DDT_API_HAS_TRACE

#	ifndef AES_DDT_TRACE_MESSAGE_SIZE_MAX
#		define AES_DDT_TRACE_MESSAGE_SIZE_MAX 8192
#	endif

#	ifndef AES_DDT_TRACE_DEFAULT_DUMP_LINE_LENGTH
#		define AES_DDT_TRACE_DEFAULT_DUMP_LINE_LENGTH 16
#		define AES_DDT_TRACE_DEFAULT_DUMP_SIZE 128
#	endif

#	define AES_DDT_TRACE_DEFINE(tag) namespace { aes_ddt_tracer __aes_ddt_tracer_object__(AES_DDT_STRINGIZE(tag)); }

#	define AES_DDT_TRACE_MESSAGE_IMPL(...) __aes_ddt_tracer_object__.trace(__VA_ARGS__)
#	define AES_DDT_TRACE_MESSAGE_SOURCE_IMPL(file, line, function, ...) __aes_ddt_tracer_object__.trace_source(file, line, function, __VA_ARGS__)

#	ifdef AES_DDT_TRACE_USE_SOURCE_INFO
#		define AES_DDT_TRACE_MESSAGE(...) AES_DDT_TRACE_MESSAGE_SOURCE_IMPL(__FILE__, __LINE__,__func__, __VA_ARGS__)
#	else
#		define AES_DDT_TRACE_MESSAGE(...) AES_DDT_TRACE_MESSAGE_IMPL(__VA_ARGS__)
#	endif

#	define AES_DDT_TRACE_DUMP(buffer, size, ...) __aes_ddt_tracer_object__.dump(buffer, size, __VA_ARGS__)

#	ifdef AES_DDT_TRACE_HAS_FUNCTION_TRACE
#		define AES_DDT_TRACE_FUNCTION_IMPL(...) aes_ddt_function_tracer __aes_ddt_enter_function__(__aes_ddt_tracer_object__, __VA_ARGS__)
#		define AES_DDT_TRACE_FUNCTION_SOURCE_IMPL(...) aes_ddt_function_tracer __aes_ddt_enter_function__(__aes_ddt_tracer_object__, __VA_ARGS__)
#	else
#		define AES_DDT_TRACE_FUNCTION_IMPL(...)
#		define AES_DDT_TRACE_FUNCTION_SOURCE_IMPL(...)
#	endif

#	ifdef AES_DDT_TRACE_USE_PRETTY_FUNCTION
#		ifdef AES_DDT_TRACE_USE_SOURCE_INFO
#			define AES_DDT_TRACE_FUNCTION AES_DDT_TRACE_FUNCTION_SOURCE_IMPL(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#		else
#			define AES_DDT_TRACE_FUNCTION AES_DDT_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__)
#		endif
#	else
#		ifdef AES_DDT_TRACE_USE_SOURCE_INFO
#			define AES_DDT_TRACE_FUNCTION AES_DDT_TRACE_FUNCTION_SOURCE_IMPL(__func__, __FILE__, __LINE__)
#		else
#			define AES_DDT_TRACE_FUNCTION AES_DDT_TRACE_FUNCTION_IMPL(__func__)
#		endif
#	endif


#undef __CLASS_NAME__
#define __CLASS_NAME__ aes_ddt_tracer

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	inline __CLASS_NAME__ (const char * tag) : _tra_tracer(tag, "C") {}

private:
	__CLASS_NAME__ (const __CLASS_NAME__ &);


	//============//
	// Destructor //
	//============//
public:
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	inline int trace_source (const char * file, int line, const char * function, const char * format, ...) __attribute__ ((format (printf, 5, 6))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, line, function, format, argp);
		::va_end(argp);
		return call_result;
	}

	int trace (const char * format, ...) __attribute__ ((format (printf, 2, 3))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(0, -1, 0, format, argp);
		::va_end(argp);
		return call_result;
	}

	void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = AES_DDT_TRACE_DEFAULT_DUMP_LINE_LENGTH);

private:
	int vtrace (const char * file, int line, const char * function, const char * format, va_list ap);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ &);


	//========//
	// Fields //
	//========//
private:
	ACS_TRA_trace _tra_tracer;
};


#undef __CLASS_NAME__
#define __CLASS_NAME__ aes_ddt_function_tracer

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ (aes_ddt_tracer & tracer, const char * function_name);
	__CLASS_NAME__ (aes_ddt_tracer & tracer, const char * function_name, const char * file_name, int line = -1);

private:
	__CLASS_NAME__ (const __CLASS_NAME__ &);


	//============//
	// Destructor //
	//============//
public:
	~__CLASS_NAME__ ();


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ &);


	//========//
	// Fields //
	//========//
private:
	aes_ddt_tracer & _tracer;
	const char * _function_name;
	bool _has_source_info;
	const char * _file_name;
};

#else // !AES_DDT_API_HAS_TRACE

#	define AES_DDT_TRACE_DEFINE(tag)
#	define AES_DDT_TRACE_MESSAGE(...)
#	define AES_DDT_TRACE_DUMP(buffer, size, ...)
#	define AES_DDT_TRACE_FUNCTION

#endif // AES_DDT_API_HAS_TRACE

#endif // HEADER_GUARD_FILE__aes_ddt_tracer
