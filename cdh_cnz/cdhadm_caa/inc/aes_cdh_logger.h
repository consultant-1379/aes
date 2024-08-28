#ifndef HEADER_GUARD_CLASS__aes_cdh_logger
#define HEADER_GUARD_CLASS__aes_cdh_logger aes_cdh_logger

/** @file aes_cdh_logger.h
 *	@brief
 *	@author xrammat 
 *	@date 2015-03-25
 *
 *	COPYRIGHT Ericsson AB, 2015
 *	All rights reserved.
 *
 *	The information in this document is the property of Ericsson.
 *	Except as specifically authorized in writing by Ericsson, the receiver of
 *	this document shall keep the information contained herein confidential and
 *	shall protect the same in whole or in part from disclosure and dissemination
 *	to third parties. Disclosure and disseminations to the receivers employees
 *	shall only be made on a strict need to know basis.
 *
 *	REVISION INFO
 *	+=======+============+==============+=====================================+
 *	| REV   | DATE       | AUTHOR       | DESCRIPTION                         |
 *	+=======+============+==============+=====================================+
 *	| R-001 | 2015-03-25 | xrammat      | Creation and first revision.        |
 *	+=======+============+==============+=====================================+
 */

#include <stdarg.h>

#include "ACS_TRA_Logging.h"

/*
 * Logging macros
 */
#if !defined (AES_CDH_LOGGER_CLASS)
#	define AES_CDH_LOGGER_CLASS aes_cdh_logger

#	define AES_CDH_LOGGER_METHOD logf
#	define AES_CDH_LOGGER_ERRNO_METHOD logf_errno
#	define AES_CDH_SYSLOGGER_METHOD syslogf
#	define AES_CDH_SYSLOGGER_ERRNO_METHOD syslogf_errno

#	define AES_CDH_LOGGER_METHOD_SOURCE logf_source
#	define AES_CDH_LOGGER_ERRNO_METHOD_SOURCE logf_errno_source
#	define AES_CDH_SYSLOGGER_METHOD_SOURCE syslogf_source
#	define AES_CDH_SYSLOGGER_ERRNO_METHOD_SOURCE syslogf_errno_source
#endif

#if !defined (AES_CDH_LOG_IMPL)
#	define AES_CDH_LOG_IMPL(level, ...) AES_CDH_LOGGER_CLASS::AES_CDH_LOGGER_METHOD(level, __VA_ARGS__)
#	define AES_CDH_LOG_ERRNO_IMPL(sys_errno, level, ...) \
	AES_CDH_LOGGER_CLASS::AES_CDH_LOGGER_ERRNO_METHOD(sys_errno, level, __VA_ARGS__)
#	define AES_CDH_SYSLOG_IMPL(syslog_priority, level, ...) \
	AES_CDH_LOGGER_CLASS::AES_CDH_SYSLOGGER_METHOD(syslog_priority, level, __VA_ARGS__)
#	define AES_CDH_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, ...) \
	AES_CDH_LOGGER_CLASS::AES_CDH_SYSLOGGER_ERRNO_METHOD(sys_errno, syslog_priority, level, __VA_ARGS__)

#	define AES_CDH_LOG_SOURCE_IMPL(level, source, line, ...) \
	AES_CDH_LOGGER_CLASS::AES_CDH_LOGGER_METHOD_SOURCE(level, source, line, __VA_ARGS__)
#	define AES_CDH_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, source, line, ...) \
	AES_CDH_LOGGER_CLASS::AES_CDH_LOGGER_ERRNO_METHOD_SOURCE(sys_errno, level, source, line, __VA_ARGS__)
#	define AES_CDH_SYSLOG_SOURCE_IMPL(syslog_priority, level, source, line, ...) \
	AES_CDH_LOGGER_CLASS::AES_CDH_SYSLOGGER_METHOD_SOURCE(syslog_priority, level, source, line, __VA_ARGS__)
#	define AES_CDH_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, source, line, ...) \
	AES_CDH_LOGGER_CLASS::AES_CDH_SYSLOGGER_ERRNO_METHOD_SOURCE(sys_errno, syslog_priority, level, source, line, __VA_ARGS__)
#endif

inline int AES_CDH_LOG_NO_LOGGING () { return 0; }

#ifdef AES_CDH_HAS_LOGGING
#	ifdef AES_CDH_LOGGING_HAS_SOURCE_INFO
#		ifdef AES_CDH_LOGGING_USE_PRETTY_FUNCTION
#			define AES_CDH_LOG(level, ...) AES_CDH_LOG_SOURCE_IMPL(level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define AES_CDH_LOG_ERRNO(sys_errno, level, ...) AES_CDH_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define AES_CDH_SYSLOG(syslog_priority, level, ...) AES_CDH_SYSLOG_SOURCE_IMPL(syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#			define AES_CDH_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) AES_CDH_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __PRETTY_FUNCTION__, __LINE__, __VA_ARGS__)
#		else
#			define AES_CDH_LOG(level, ...) AES_CDH_LOG_SOURCE_IMPL(level, __func__, __LINE__, __VA_ARGS__)
#			define AES_CDH_LOG_ERRNO(sys_errno, level, ...) AES_CDH_LOG_SOURCE_ERRNO_IMPL(sys_errno, level, __func__, __LINE__, __VA_ARGS__)
#			define AES_CDH_SYSLOG(syslog_priority, level, ...) AES_CDH_SYSLOG_SOURCE_IMPL(syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#			define AES_CDH_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) AES_CDH_SYSLOG_SOURCE_ERRNO_IMPL(sys_errno, syslog_priority, level, __func__, __LINE__, __VA_ARGS__)
#		endif // AES_CDH_LOGGING_USE_PRETTY_FUNCTION
#	else
#		define AES_CDH_LOG(level, ...) AES_CDH_LOG_IMPL(level, __VA_ARGS__)
#		define AES_CDH_LOG_ERRNO(sys_errno, level, ...) AES_CDH_LOG_ERRNO_IMPL(sys_errno, level, __VA_ARGS__)
#		define AES_CDH_SYSLOG(syslog_priority, level, ...) AES_CDH_SYSLOG_IMPL(syslog_priority, level, __VA_ARGS__)
#		define AES_CDH_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) AES_CDH_SYSLOG_ERRNO_IMPL(sys_errno, syslog_priority, level, __VA_ARGS__)
#	endif //AES_CDH_LOGGING_HAS_SOURCE_INFO
#else
#	define AES_CDH_LOG(level, ...) AES_CDH_LOG_NO_LOGGING()
#	define AES_CDH_LOG_ERRNO(sys_errno, level, ...) AES_CDH_LOG_NO_LOGGING()
#	define AES_CDH_SYSLOG(syslog_priority, level, ...) AES_CDH_LOG_NO_LOGGING()
#	define AES_CDH_SYSLOG_ERRNO(sys_errno, syslog_priority, level, ...) AES_CDH_LOG_NO_LOGGING()
#endif //AES_CDH_HAS_LOGGING


#if defined (AES_CDH_DEBUGGING_LOG_ENABLED)
#	define DEBUGGING_LOG_1(format) 
#	define DEBUGGING_LOG_N(format, ...) 
#else
#	define DEBUGGING_LOG_1(format)
#	define DEBUGGING_LOG_N(format, ...)
#endif

#undef __CLASS_NAME__
#define __CLASS_NAME__ HEADER_GUARD_CLASS__aes_cdh_logger

/** @class aes_cdh_logger aes_cdh_logger.h
 *	@brief aes_cdh_logger class
 *	@author xrammat
 *	@date 2015-03-25
 *
 *	aes_cdh_logger Class detailed description
 */
class __CLASS_NAME__ {
	//===========//
	// Constants //
	//===========//
private:
	enum {
		SYSLOG_DISABLED_MASK	=	99999,
		ERRNO_DISABLED_MASK		=	-99999
	};


	//==============//
	// Constructors //
	//==============//
private:
	/** @brief aes_cdh_logger Default constructor
	 *
	 *	aes_cdh_logger Constructor detailed description
	 *
	 *	@param[in] logging_level Description
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ ();

	/** @brief aes_cdh_logger Copy constructor
	 *
	 *	aes_cdh_logger Copy Constructor detailed description
	 *
	 *	@param[in] rhs Description
	 *	@remarks Remarks
	 */
	__CLASS_NAME__ (const __CLASS_NAME__ & rhs);


	//============//
	// Destructor //
	//============//
public:
	/** @brief aes_cdh_logger Destructor
	 *
	 *	aes_cdh_logger Destructor detailed description
	 *
	 *	@remarks Remarks
	 */
	inline ~__CLASS_NAME__ () {}


	//===========//
	// Functions //
	//===========//
public:
	static ACS_TRA_LogResult open (const char * appender_name);
	static void close ();

	static ACS_TRA_LogResult syslogf_errno (int sys_errno, int syslog_priority,	ACS_TRA_LogLevel level, const char * format, ...) __attribute__ ((format (printf, 4, 5)));
	static ACS_TRA_LogResult syslogf_errno (int sys_errno, int syslog_priority, const char * format, ...) __attribute__ ((format (printf, 3, 4)));
	static ACS_TRA_LogResult syslogf_errno_source (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 6, 7)));
	static ACS_TRA_LogResult syslogf_errno_source (int sys_errno, int syslog_priority, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 5, 6)));

	static ACS_TRA_LogResult syslogf (int syslog_priority, ACS_TRA_LogLevel level, const char * format, ...) __attribute__ ((format (printf, 3, 4)));
	static ACS_TRA_LogResult syslogf (int syslog_priority, const char * format, ...) __attribute__ ((format (printf, 2, 3)));
	static ACS_TRA_LogResult syslogf_source (int syslog_priority, ACS_TRA_LogLevel level,	const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 5, 6)));
	static ACS_TRA_LogResult syslogf_source (int syslog_priority, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 4, 5)));

	static ACS_TRA_LogResult logf (ACS_TRA_LogLevel level, const char * format, ...) __attribute__ ((format (printf, 2, 3)));
	static ACS_TRA_LogResult logf (const char * format, ...) __attribute__ ((format (printf, 1, 2)));
	static ACS_TRA_LogResult logf_source (ACS_TRA_LogLevel level, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 4, 5)));
	static ACS_TRA_LogResult logf_source (const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 3, 4)));

	static ACS_TRA_LogResult logf_errno_source (int sys_errno, ACS_TRA_LogLevel level, const char * source_func_name, int source_line, const char * format, ...) __attribute__ ((format (printf, 5, 6)));
	static ACS_TRA_LogResult logf_errno_source (int sys_errno, const char * source_func_name,	int source_line, const char * format, ...) __attribute__ ((format (printf, 4, 5)));
	static ACS_TRA_LogResult logf_errno (int sys_errno, ACS_TRA_LogLevel level, const char * format, ...) __attribute__ ((format (printf, 3, 4)));
	static ACS_TRA_LogResult logf_errno (int sys_errno, const char * format, ...) __attribute__ ((format (printf, 2, 3)));

	static inline ACS_TRA_LogResult log (const char * message) { return logf(_logging_level, message); }
	static inline ACS_TRA_LogResult log (ACS_TRA_LogLevel level, const char * message) { return logf(level, message); }
	static inline ACS_TRA_LogResult log (const char * source_func_name, int source_line, const char * message) {
		return logf_source(_logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult log (ACS_TRA_LogLevel level, const char * source_func_name,
			int source_line, const char * message) {
		return logf_source(level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogResult log_errno (int sys_errno, const char * message) {
		return logf_errno(sys_errno, _logging_level, message);
	}
	static inline ACS_TRA_LogResult log_errno (int sys_errno, ACS_TRA_LogLevel level, const char * message) {
		return logf_errno(sys_errno, level, message);
	}
	static inline ACS_TRA_LogResult log_errno (int sys_errno, const char * source_func_name, int source_line,
			const char * message) {
		return logf_errno_source(sys_errno, _logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult log_errno (int sys_errno, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * message) {
		return logf_errno_source(sys_errno, level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogResult syslog (int syslog_priority, const char * message) {
		return syslogf(syslog_priority, _logging_level, message);
	}
	static inline ACS_TRA_LogResult syslog (int syslog_priority, ACS_TRA_LogLevel level, const char * message) {
		return syslogf(syslog_priority, level, message);
	}
	static inline ACS_TRA_LogResult syslog (int syslog_priority, const char * source_func_name, int source_line,
			const char * message) {
		return syslogf_source(syslog_priority, _logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult syslog (int syslog_priority, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * message) {
		return syslogf_source(syslog_priority, level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority, const char * message) {
		return syslogf_errno(sys_errno, syslog_priority, _logging_level, message);
	}
	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * message) {
		return syslogf_errno(sys_errno, syslog_priority, level, message);
	}
	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority,
			const char * source_func_name, int source_line, const char * message) {
		return syslogf_errno_source(sys_errno, syslog_priority, _logging_level, source_func_name, source_line, message);
	}
	static inline ACS_TRA_LogResult syslog_errno (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * message) {
		return syslogf_errno_source(sys_errno, syslog_priority, level, source_func_name, source_line, message);
	}

	static inline ACS_TRA_LogLevel logging_level () { return _logging_level; }
	static inline void logging_level (ACS_TRA_LogLevel level) { _logging_level = level; }


private:
	static ACS_TRA_LogResult vlogf (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * format, va_list ap);
	static ACS_TRA_LogResult vlogf_source (int sys_errno, int syslog_priority, ACS_TRA_LogLevel level,
			const char * source_func_name, int source_line, const char * format, va_list ap);


	//===========//
	// Operators //
	//===========//
private:
	__CLASS_NAME__ & operator= (const __CLASS_NAME__ & rhs);


	//========//
	// Fields //
	//========//
private:
	static ACS_TRA_LogLevel _logging_level;
};

#endif // HEADER_GUARD_CLASS__aes_cdh_logger
