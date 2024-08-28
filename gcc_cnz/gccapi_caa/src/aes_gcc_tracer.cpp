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
 * @file MCS_CC_Trace.cpp
 *
 * @brief
 * See header file
 *
 * @details
 * See header file
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
 * 2011-11-16  XTBAKLU  Etracted formatString to be able to use it for both trace and log
 *                      Also removed use of trace point in log functions.
 *
 ****************************************************************************/

#include <sstream>
#include <iostream>

#include <ACS_TRA_trace.h>

#include <ace/Synch.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <syslog.h>
#include <new>

#include <string.h>
#include <stdio.h>

#include "aes_gcc_tracer.h"

// Name of the log file produced if environment variable DEBUG_MCS is defined to 1, "export DEBUG_MCS=1"
const char *TRACE_CLASS ="ADH";


enum {
	SYSLOG_DISABLED_MASK	=	99999,
	ERRNO_DISABLED_MASK		=	-99999
};


/****************************************************************************/
/****************************************************************************/
/* START IMPROVEMENT */
/****************************************************************************/
/****************************************************************************/

#ifdef AES_GCC_HAS_TRACE


#undef __CLASS_NAME__
#define __CLASS_NAME__ aes_gcc_tracer

int __CLASS_NAME__::vtrace (const char * file, int line, const char * function, const char * format, va_list ap) {
	if (_tra_tracer.ACS_TRA_ON()) {
		char trace_msg[AES_GCC_TRACE_MESSAGE_SIZE_MAX];
		int char_wrote = 0;

		if (file || function || (line > 0)) {
			char_wrote = ::snprintf(trace_msg, AES_ARRAY_SIZE(trace_msg), "(0x%08X) {", (pid_t) syscall (SYS_gettid));

			if (function) {
				char_wrote += ::snprintf(trace_msg + char_wrote, AES_ARRAY_SIZE(trace_msg) - char_wrote, "%s", function);
			}
			if (file) {
				char * filename = ::strrchr(const_cast<char*>(file), '/');
				char_wrote += ::snprintf(trace_msg + char_wrote, AES_ARRAY_SIZE(trace_msg) - char_wrote, "(), %s", filename ? (filename + 1) : "NO_FILE");
			}

			(line > 0) && (char_wrote += ::snprintf(trace_msg + char_wrote, AES_ARRAY_SIZE(trace_msg) - char_wrote, "::%d", line));
			char_wrote += ::snprintf(trace_msg + char_wrote, AES_ARRAY_SIZE(trace_msg) - char_wrote, "} ");
		}

		::vsnprintf(trace_msg + char_wrote, AES_ARRAY_SIZE(trace_msg) - char_wrote, format, ap);
		trace_msg[AES_ARRAY_SIZE(trace_msg) - 1] = 0;

		_tra_tracer.ACS_TRA_event(1, trace_msg);
	}

	return 0;
}

void __CLASS_NAME__::dump (uint8_t * buffer, size_t size, size_t dumping_size, size_t dumping_line_length) {
	size_t output_buffer_size = 2 + 8 + 2 + 3*dumping_line_length + 16;
	char output_buffer[output_buffer_size];

	for (size_t i = 0; (i < size) && (i < dumping_size); ) {
		int chars = ::snprintf(output_buffer, output_buffer_size, "  %08zX:", i);
		for (size_t col = 0; (col < dumping_line_length) && (i < size) && (i < dumping_size); ++i, ++col)
			chars += ::snprintf(output_buffer + chars, output_buffer_size - chars, " %02X", buffer[i]);
		trace(output_buffer);
	}
}


#undef __CLASS_NAME__
#define __CLASS_NAME__ aes_gcc_function_tracer

__CLASS_NAME__::__CLASS_NAME__ (aes_gcc_tracer & tracer, const char * function_name)
: _tracer(tracer), _function_name(function_name) {
	_tracer.trace("Entering function '%s'", function_name);
}

__CLASS_NAME__::__CLASS_NAME__ (aes_gcc_tracer & tracer, const char * function_name, const char * file_name, int line)
: _tracer(tracer), _function_name(function_name) {
	_tracer.trace_source(file_name, line, "Entering function '%s'", _function_name ?: "NO_FUNCTION");
}

__CLASS_NAME__::~__CLASS_NAME__ () {
	_tracer.trace("Leaving function '%s'", _function_name ?: "NO_FUNCTION");
}

#endif // AES_GCC_HAS_TRACE



/****************************************************************************/
/****************************************************************************/
/* END IMPROVEMENT */
/****************************************************************************/
/****************************************************************************/

