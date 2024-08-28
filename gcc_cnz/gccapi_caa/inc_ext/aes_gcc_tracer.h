/*****************************************************************************
 *
 * COPYRIGHT Ericsson Telecom AB 2011
 *
 * The copyright of the computer program herein is the property of
 * Ericsson Telecom AB. The program may be used and/or copied only with the
 * written permission from Ericsson Telecom AB or in the accordance with the
 * terms and conditions stipulated in the agreement/contract under which the
 * program has been supplied.
 *
 ----------------------------------------------------------------------*//**
 *
 * @file mcs_cc_trace.h
 *
 * @brief
 * Encapsulates the ACS TRA Class used for tracing and logging.
 * The main feature is the mcc_trace() function that will reduce the amount
 * of code needed to do add tracing.
 *
 * An extra feature when using this trace class is that the trace can be directed
 * to a file using environment variable DEBUG_MCS=1
 * If so the file /tmp/mcs_cc_trace.log is created
 *
 *
 * @details
 * The tracing is based on an sprintf style which is not C++ and not the
 * best solution but it outranks the interface provided by TRA.
 * TRA does not provide a streaming interface and the design base heavily
 * used this mcc_trace() function.
 * This class was also introduced since the TRA product was not ready
 * when porting of MCS started.
 *  *
 * @author XTBAKLU
 *
 -------------------------------------------------------------------------*//*
 *
 * REVISION HISTORY
 *
 * DATE        USER     DESCRIPTION/TR
 * --------------------------------
 * 2010-11-22  XTBAKLU  Ported and improved
 *
 ****************************************************************************/

#ifndef AES_GCC_TRACE_H_
#define AES_GCC_TRACE_H_

#include <string> // Need to include this to make ACS_APGCC_Exceptions.H below work!!!!!
#include <stdio.h>
#include <iostream>
#include <ace/Mutex.h>
#include <stdarg.h>

#include <ACS_TRA_trace.h>
#include <ACS_TRA_Logging.h>

#include "aes_gcc_util.h"

/****************************************************************************/
/****************************************************************************/
/* START IMPROVEMENT */
/****************************************************************************/
/****************************************************************************/
#ifndef HEADER_GUARD_FILE__aes_gcc_macros
#define HEADER_GUARD_FILE__aes_gcc_macros


#ifndef AES_GCC_TRY_CALL_IMPLEMENTATION
#	define AES_GCC_TRY_CALL_IMPLEMENTATION(impl_pointer, func, ...) ((impl_pointer) ? ((impl_pointer)->func(__VA_ARGS__)) : aes_gcc::ERR_SIGNALS_ERROR)
#endif

#ifndef AES_GCC_ERROR_INFO_DECLARE_ACCESSORS
#define AES_GCC_ERROR_INFO_DECLARE_ACCESSORS \
	int last_error_code () const; \
	const char * last_error_text () const
#endif

#ifndef AES_GCC_ERROR_INFO_DEFINE_ACCESSORS
#	define AES_GCC_ERROR_INFO_DEFINE_ACCESSORS(impl_pointer) \
	int __CLASS_NAME__::last_error_code () const { \
		AES_GCC_TRACE_FUNCTION; \
		if ((impl_pointer)) return (impl_pointer)->last_error_code(); \
		return aes_gcc::ERR_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE; \
	} \
	const char * __CLASS_NAME__::last_error_text () const { \
		AES_GCC_TRACE_FUNCTION; \
		if ((impl_pointer)) return (impl_pointer)->last_error_text(); \
		return aes_gcc::ERRTEXT_INTERNAL_IMPLEMENTATION_NOT_AVAILABLE; \
	}
#endif

#endif /* HEADER_GUARD_FILE__AES_GCC_macros */


#ifndef HEADER_GUARD_FILE__aes_gcc_trace
#define HEADER_GUARD_FILE__aes_gcc_trace

/**************************
 * BEGIN: REMARK: WARNING *
 **************************
 * Do not change the following defines
 */
#ifndef AES_GCC_TRACE_CONTROL_FROM_FILE_DISABLED
#	ifndef AES_GCC_HAS_TRACE
#		define AES_GCC_HAS_TRACE
#	endif

#	ifndef AES_GCC_TRACE_HAS_FUNCTION_TRACE
#		define AES_GCC_TRACE_HAS_FUNCTION_TRACE
#	endif

#	ifndef AES_GCC_TRACE_USE_PRETTY_FUNCTION
#		define AES_GCC_TRACE_USE_PRETTY_FUNCTION
#	endif

#	ifndef AES_GCC_TRACE_USE_SOURCE_INFO
#		define AES_GCC_TRACE_USE_SOURCE_INFO
#	endif
#endif // AES_GCC_TRACE_CONTROL_FROM_FILE_DISABLED
/************************
 * END: REMARK: WARNING *
 ************************/


#ifdef AES_GCC_HAS_TRACE

#	ifndef AES_GCC_TRACE_MESSAGE_SIZE_MAX
#		define AES_GCC_TRACE_MESSAGE_SIZE_MAX 8192
#	endif

#	ifndef AES_GCC_TRACE_DEFAULT_DUMP_LINE_LENGTH
#		define AES_GCC_TRACE_DEFAULT_DUMP_LINE_LENGTH 16
#	endif


#	define AES_GCC_TRACE_DEFINE(tag) namespace { aes_gcc_tracer __aes_gcc_tracer_object__(AES_GCC_STRINGIZE(tag)); }
#	define AES_GCC_TRACE_MESSAGE_IMPL(...) __aes_gcc_tracer_object__.trace(__VA_ARGS__)
#	define AES_GCC_TRACE_MESSAGE_IMPL_SOURCE(file, line,function, ...) __aes_gcc_tracer_object__.trace_source(file, line, function, __VA_ARGS__)

#	ifdef AES_GCC_TRACE_USE_SOURCE_INFO
#		define AES_GCC_TRACE_MESSAGE(...) AES_GCC_TRACE_MESSAGE_IMPL_SOURCE(__FILE__, __LINE__,__func__, __VA_ARGS__)
#	else
#		define AES_GCC_TRACE_MESSAGE(...) AES_GCC_TRACE_MESSAGE_IMPL(__VA_ARGS__)
#	endif

#	define AES_GCC_TRACE_DUMP(buffer, size, ...) __aes_gcc_tracer_object__.dump(buffer, size, __VA_ARGS__)

#	ifdef AES_GCC_TRACE_HAS_FUNCTION_TRACE
#		define AES_GCC_TRACE_FUNCTION_IMPL(...) aes_gcc_function_tracer __aes_gcc_enter_function__(__aes_gcc_tracer_object__, __VA_ARGS__)
#	else
#		define AES_GCC_TRACE_FUNCTION_IMPL(...)
#	endif

#	ifdef AES_GCC_TRACE_USE_PRETTY_FUNCTION
#		ifdef AES_GCC_TRACE_USE_SOURCE_INFO
#			define AES_GCC_TRACE_FUNCTION AES_GCC_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__, __FILE__, __LINE__)
#		else
#			define AES_GCC_TRACE_FUNCTION AES_GCC_TRACE_FUNCTION_IMPL(__PRETTY_FUNCTION__)
#		endif
#	else
#		ifdef AES_GCC_TRACE_USE_SOURCE_INFO
#			define AES_GCC_TRACE_FUNCTION AES_GCC_TRACE_FUNCTION_IMPL(__func__, __FILE__, __LINE__)
#		else
#			define AES_GCC_TRACE_FUNCTION AES_GCC_TRACE_FUNCTION_IMPL(__func__)
#		endif
#	endif


#undef __CLASS_NAME__
#define __CLASS_NAME__ aes_gcc_tracer

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
		int call_result = vtrace(file, line,function, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (const char * file, int line, const char * format, ...) __attribute__ ((format (printf, 4, 5))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, line,0, format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (const char * file, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(file, -1, 0,format, argp);
		::va_end(argp);
		return call_result;
	}

	inline int trace_source (int line, const char * format, ...) __attribute__ ((format (printf, 3, 4))) {
		va_list argp;
		::va_start(argp, format);
		int call_result = vtrace(0, line, 0, format, argp);
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

	void dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length = AES_GCC_TRACE_DEFAULT_DUMP_LINE_LENGTH);

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
#define __CLASS_NAME__ aes_gcc_function_tracer

class __CLASS_NAME__ {
	//==============//
	// Constructors //
	//==============//
public:
	__CLASS_NAME__ (aes_gcc_tracer & tracer, const char * function_name);
	__CLASS_NAME__ (aes_gcc_tracer & tracer, const char * function_name, const char * file_name, int line = -1);

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
	aes_gcc_tracer & _tracer;
	const char * _function_name;
};

#else // AES_GCC_HAS_TRACE

#	define AES_GCC_TRACE_DEFINE(tag)
#	define AES_GCC_TRACE_MESSAGE(...)
#	define AES_GCC_TRACE_DUMP(buffer, size, ...)
#	define AES_GCC_TRACE_FUNCTION

#endif // AES_GCC_HAS_TRACE

#endif // HEADER_GUARD_FILE__AES_GCC_trace


/****************************************************************************/
/****************************************************************************/
/* END IMPROVEMENT */
/****************************************************************************/
/****************************************************************************/

#endif /* MCS_CC_TRACE_H_ */
