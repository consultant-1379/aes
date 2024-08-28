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

#ifndef DDTADM_CAA_OPENSSLTHREADING_H_
#define DDTADM_CAA_OPENSSLTHREADING_H_

#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace common
{
	/**
	 * @class OpenSSLThreading
	 *
	 * @brief The OpenSSLThreading class is needed to perform CRYPTO configurations for a multithreaded environment.
	 *
	 * Operations performed here are described in the SEC interface document 19089-LZN7080597 Rev D
	 *
	 * This class cannot be instantiated.
	 * Operations contained in this class are prerequisites for
	 * 		- the password handling that is provided through SEC API
	 * 		- SFTP connections that are established by means of libssh2
	 *
	 */
	class OpenSSLThreading : private boost::noncopyable
	{
	 private:
		/**
		 * @brief OpenSSLThreading is private since this class cannot be instantiated
		 *
		 */
		OpenSSLThreading ();

	 public:
		/**
		 * @brief setup method
		 * This method is used to prepare the underlying OpenSSL API to work in a multithreaded environment.
		 * !! WARNING: This is to be called before any threads are created !!
		 *
		 * returns common::errorCode::ERR_NO_ERRORS (0) if ok
		 *
		 */
	     static int setup();

	     /**
	      * @brief cleanup method is used to cleanup the underlying OpenSSL stratum.
	      * This method is to be called after all the threads are terminated.
	      *
	      */
	     static void cleanup();

	 private:
	     /**
	      * @brief threadIdCallback method
	      * This method returns the Id of the current thread.
	      *
	      */
	     static unsigned long threadIdCallback ();

	     /**
	      * @brief lockingCallback method
	      * This method is used for locking/releasing shared data structures.
	      *
	      */
	     static void lockingCallback (int mode, int i, const char * file, int line);

	 private:
	     /**
	      * @brief m_mutexPool
	      *	This buffer contains all the mutexes for OpenSSL.
	      *	!! WARNING: These mutexes must not be used for any other purpose. !!
	      *
	      */
	 	static pthread_mutex_t * m_mutexPool;

	 	/**
	 	 * @brief m_initialized
	 	 *	This flag keeps track of the inizialization
	 	 *
	 	 */
	 	static bool m_initialized;
	};

} /* namespace common */

#endif /* DDTADM_CAA_OPENSSLTHREADING_H_ */
