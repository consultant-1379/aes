//	********************************************************
//
//	 COPYRIGHT Ericsson 2016
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

#include "common/opensslthreading.h"
#include "common/tracer.h"
#include "common/logger.h"
#include "common/programconstants.h"

#include <syslog.h>
#include <errno.h>
#include <openssl/crypto.h>

// Check if OpenSSL supports the execution in multi threaded environment
#define OPENSSL_THREAD_DEFINES
#include <openssl/opensslconf.h>
#if !defined(OPENSSL_THREADS)
#error OpenSSL does not support threads, core dumps are possible if the calls are multithreaded.
#endif

AES_DDT_TRACE_DEFINE(AES_DDT_Common_OpenSSL)

pthread_mutex_t * common::OpenSSLThreading::m_mutexPool = 0;
bool common::OpenSSLThreading::m_initialized = false;

namespace common
{
	int OpenSSLThreading::setup()
	{
		// Get the number of mutexes OpenSSL requires
		const int required_locks = CRYPTO_num_locks();
		AES_DDT_LOG(LOG_LEVEL_DEBUG, "required_locks: %d", required_locks);

		// Allocate the pool of mutexes
		m_mutexPool = reinterpret_cast<pthread_mutex_t *>(malloc(required_locks * sizeof(pthread_mutex_t)));
		if(!(m_mutexPool))
		{
			// Memory error!
			const int errno_save = errno;
			AES_DDT_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "Cannot allocate memory for OpenSSL mutex pool! Core dumps are possible!!");

			return common::errorCode::ERR_API_CALL;
		}

		// After allocating the requested memory, initialize each mutex in the pool
		for(int i = 0; i < required_locks; ++i)
		{
			// Initialize mutexes using default attributes
			const int result = ::pthread_mutex_init(&(m_mutexPool[i]), 0);
			if(result != 0)
			{
				// Handle Error case
				const int errno_save = errno;
				AES_DDT_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "pthread_mutex_init() failed, operation result: <%d>", result);

				// Finalize all the already initialized mutexes
				for (int j = 0; j < i; ++j)
				{
					::pthread_mutex_destroy(&(m_mutexPool[j]));
				}

				return common::errorCode::ERR_API_CALL;
			}
		}

		// Register the two callbacks required by OpenSSL library for its thread safety
		CRYPTO_set_id_callback(&threadIdCallback);
		CRYPTO_set_locking_callback(&lockingCallback);

		m_initialized = true;
		return common::errorCode::ERR_NO_ERRORS;
	}

	void OpenSSLThreading::cleanup()
	{
		if(m_initialized)
		{
			// Get the number of mutexes OpenSSL requires
			const int required_locks = CRYPTO_num_locks();

			// Unregister the two callbacks required by OpenSSL library for its thread safety
			CRYPTO_set_id_callback(0);
			CRYPTO_set_locking_callback(0);

			// Finalize each mutex in the pool
			for(int i = 0; i < required_locks; ++i)
			{
				const int result = ::pthread_mutex_destroy(&(m_mutexPool[i]));
				if(result != 0)
				{
					// Handle Error case
					const int errno_save = errno;
					AES_DDT_LOG_ERRNO(errno_save, LOG_LEVEL_ERROR, "pthread_mutex_destroy() failed, operation result: <%d>", result);
				}
			}

			// Free the allocated memory for the pool of mutexes
			::free(m_mutexPool);
			m_mutexPool = 0;
		}
		m_initialized = false;

		return;
	}

	unsigned long OpenSSLThreading::threadIdCallback()
	{
		// The only operation performed by this callback is to return the ID of the caller thread
		unsigned long  tid = static_cast<unsigned long>(::pthread_self());
		//NOTE: This will floods up the log. Uncomment if needed
		//AES_DDT_LOG(LOG_LEVEL_DEBUG, "threadIdCallback: %ul", tid);
		return tid;
	}

	void OpenSSLThreading::lockingCallback(int mode, int i, const char* file, int line)
	{
		// The purpose of this method is to lock/unlock the mutex having the provided index according to the mode parameter
		if (mode & CRYPTO_LOCK)
		{
			// Mutex locking requested
			::pthread_mutex_lock(&(m_mutexPool[i]));
		}
		else {
			// Mutex unlocking requested
			::pthread_mutex_unlock(&(m_mutexPool[i]));
		}
	}
} /* namespace common */
