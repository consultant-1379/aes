/*=================================================================== */
/**
	@file   aes_gcc_message_queue.h

	@brief	A template abstraction of a queue

	Implements a message queue abstraction.

	Usage:
	This class is used as a base class for AES_GCC_Thread<T>
	but could also be used in a separate application.

	For example,

	int main(int argc, char* argv[])
	{
	AES_GCC_MessageQueue<int> queue;

	queue.putq(16);

	int i;

	queue.getq(i);

	return 0;
	}

   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       01/08/2011     XNADNAR       Initial Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef AES_GCC_MESSAGEQUEUE_H
#define AES_GCC_MESSAGEQUEUE_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <list>
#include "aes_gcc_errorcodes.h"
#include <ace/ACE.h>
#include <ace/Thread_Semaphore.h>
#include <ace/Mutex.h>
#include <iostream>


/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
		@brief 			AES_GCC_MessageQueue

						This is a class template for creating message queue of any type of objects
 */
/*=================================================================== */
template <class T>
/*===================================================================*/
/**
		@brief 			AES_GCC_MessageQueue

						This is a class for creating message queue of any type of objects
 */
/*=================================================================== */
class AES_GCC_MessageQueue
{
	/*=====================================================================
						PRIVATE DECLARATION SECTION
==================================================================== */
private:
	/*=================================================================== */
	/**
	  @brief  list_

	  Standard list to hold the objects of type T
	 */
	/*=================================================================== */
	std::list<T> list_;
	/*=================================================================== */
	/**
	  @brief  mutex_

	  Reference of ACE_Mutex
	 */
	/*=================================================================== */
	//ACE_Thread_Mutex * mutex_;
        pthread_mutex_t mutex_;
	/*=================================================================== */
	/**
	  @brief  sema_

	  Reference of ACE_Semaphore
	 */
	/*=================================================================== */
	//ACE_Thread_Semaphore * sema_;  
        sem_t sema_;

	/*=====================================================================
						PUBLIC DECLARATION SECTION
==================================================================== */
public:
	/*=====================================================================
						CLASS CONSTRUCTORS
==================================================================== */
	/*=================================================================== */
	/**

	  @brief          default constructor
					  Creates an empty AES_GCC_MessageQueue object.

	  @return          void

	  @exception       none
	 */
	/*=================================================================== */
	AES_GCC_MessageQueue();
	/*=================================================================== */
	/**

	  @brief          destructor

	  @return          void

	  @exception       none
	 */
	/*=================================================================== */
	virtual ~AES_GCC_MessageQueue();
	/*=================================================================== */
	/**

		@brief        	create

						Creates a new queue.
		@return		 	 void
		@exception       none
	 */
	/*=================================================================== */
	void create();
	/*=================================================================== */
	/**

		@brief        	close

						Closes the queue and cleans it.

		@return		 	 void
		@exception       none
	 */
	/*=================================================================== */
	void close();
	/*=================================================================== */
	/**

		@brief        	putq
						This method puts an element onto the queue.

		@param          elem
						This is a reference to the element with unknown type.

		@return		 	 The boolean returned indicates true for success otherwise false.
		@exception       none
	 */
	/*=================================================================== */
	bool putq(const T &elem);
	/*=================================================================== */
	/**

		@brief        	putq
						This method puts an element onto the queue.

		@param          elem
						This is a pointer to the element with unknown type.

		@return		 	 The boolean returned indicates true for success otherwise false.
		@exception       none
	 */
	/*=================================================================== */
	bool putq(const T *elem)
	{ return putq(*elem); }
	/*=================================================================== */
	/**

		@brief        	getq
						This method retrieves an element from the queue.

		@param          elem
						This is a reference to the element with unknown type.
		@param          timeout
						timeout

		@return		 	 The boolean returned indicates true for success otherwise false.
		@exception       none
	 */
	/*=================================================================== */
	unsigned int getq(T &elem, unsigned long timeout = 0xFFFFFFFF);
	/*=================================================================== */
	/**

		@brief        	getq
						This method retrieves an element from the queue.

		@param          elem
						This is a pointer to the element with unknown type.
		@param          timeout
						timeout

		@return		 	 The boolean returned indicates true for success otherwise false.
		@exception       none
	 */
	/*=================================================================== */
	unsigned int getq(T *&elem, unsigned long timeout = 0xFFFFFFFF)
	{ return getq(*elem, timeout); }
	/*=================================================================== */
	/**

		@brief        	prioritize
						This method retrieves an element from the queue.

		@param          elem
						This is a reference to the element with unknown type.

		@return		 	 The boolean returned indicates true for success otherwise false.
		@exception       none
	 */
	/*=================================================================== */

	bool prioritize(const T &elem);
	/*=================================================================== */
	/**
		@brief        	prioritize
						This method retrieves an element from the queue.

		@param          elem
						This is a pointer to the element with unknown type.

		@return		 	 The boolean returned indicates true for success otherwise false.
		@exception       none
	 */
	/*=================================================================== */
	bool prioritize(const T *elem)
	{ return prioritize(*elem); }
	/*=================================================================== */
	/**
		@brief        	size
						 This method is used to achieve the size of the queue.

		@return		 	The return value indicates the size.
		@exception       none
	 */
	/*=================================================================== */
	int size();
};


template <class T>
/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_MessageQueue<T>::AES_GCC_MessageQueue()
{
	// Create synchronization objects
	create();
}

/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
template <class T>
AES_GCC_MessageQueue<T>::~AES_GCC_MessageQueue()
{
	// Close the synchronization objects
      	/*if(sema_ != NULL)
	{
		delete sema_;
		sema_ = NULL;
	}*/
     
	/*if(mutex_ !=NULL)
	{
		delete mutex_;
		mutex_ = NULL;
	}*/
}


/*===================================================================
                    ROUTINE: create
=================================================================== */
template <class T>
void AES_GCC_MessageQueue<T>::create()
{
      // Create synchronization objects
      //mutex_ = new ACE_Thread_Mutex();
      pthread_mutex_init(&mutex_, NULL);
      //sema_ = new ACE_Thread_Semaphore(0); 
      sem_init(&sema_,0,0);
}
/*===================================================================
                    ROUTINE: close
=================================================================== */
template <class T>
void AES_GCC_MessageQueue<T>::close()
{
	// Clear the list containing elements
	list_.clear();
	// Release the semaphore
	//sema_->release();
	 sem_post(&sema_);

        pthread_mutex_destroy(&mutex_);
	// Close the synchronization objects
	/*if(sema_ != NULL)
	{
		delete sema_;
		sema_ = NULL;
	}
	if(mutex_ !=NULL)
	{
		delete mutex_;
		mutex_ = NULL;
	}*/

}
/*===================================================================
                    ROUTINE: putq
=================================================================== */
template <class T>
bool AES_GCC_MessageQueue<T>::putq(const T &elem)
{
	// Aquire mutex lock
	//mutex_->acquire();
        pthread_mutex_lock(&mutex_);

	// Enqueue an element at the back of the queue
	list_.push_back(elem);

	// Check if the element is in the list
	if (list_.back() == elem)
	{
		// Release the mutex
		//mutex_->release();
                 pthread_mutex_unlock(&mutex_);

		// Increase the value of the semaphore
		//sema_->release();
                sem_post(&sema_);
		return true;
	}
	//GCC_TRACE(("%s\n","PUTQ SUCCESS"));
	// Release the mutex
	//mutex_->release();
        pthread_mutex_unlock(&mutex_);
	return false;
}

/*===================================================================
                    ROUTINE: getq
=================================================================== */
template <class T>
unsigned int AES_GCC_MessageQueue<T>::getq(T &elem, unsigned long timeout)
{
	// Wait for the semaphore
	/*ACE_Time_Value tv(ACE_OS::gettimeofday());

	ACE_Time_Value tv1(0,1000*timeout);

	tv = tv + tv1;

	ACE_INT32 rCode= sema_->acquire(tv);*/
       

        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec = ts.tv_sec + timeout/1000 ;
	//return ts.tv_sec;
        int rCode = sem_timedwait(&sema_, &ts);
	//if (rCode != AES_NOERRORCODE)
        if (rCode != 0)
	{
		perror("semaphore" );
	//GCC_TRACE(("%s", "acquire error =%u\n ",ACE_OS::last_error()));
	//GCC_TRACE(("%s", "rCode =%u\n ", rCode));
		return rCode;
	}
	//GCC_TRACE(("%s","rCode =%u\n", rCode));
	// Acquire mutex lock
	//mutex_->acquire();
        pthread_mutex_lock(&mutex_);
 
	// Check the list contains something
	if (list_.empty())
	{
		//GCC_TRACE(("%s\n","LIST EMPTY in GETQ"));
		// Release the mutex
		//mutex_->release();
                pthread_mutex_unlock(&mutex_);
		return AES_EXECUTEERROR;
	}

	// Get the element
	elem = list_.front();

	// Remove it from the list
	list_.pop_front();

	// Release the mutex
	//mutex_->release();
        pthread_mutex_unlock(&mutex_);

	return AES_NOERRORCODE;
}


/*===================================================================
                    ROUTINE: prioritize
=================================================================== */
template <class T>
bool AES_GCC_MessageQueue<T>::prioritize(const T &elem)
{
	// Aquire mutex lock
	//mutex_->acquire();
        pthread_mutex_lock(&mutex_);

	// Enqueue an element at the front of the queue
	list_.push_front(elem);

	// Check if the element is in the list
	if (list_.front() == elem)
	{
		// Release the mutex
		// Increase the value of the semaphore
		//mutex_->release();
                pthread_mutex_unlock(&mutex_);

		//sema_->release();
		  sem_post(&sema_);
		return true;
	}
	// Release the mutex
	//mutex_->release();
        pthread_mutex_unlock(&mutex_);

	return false;
}

/*===================================================================
                    ROUTINE: size
=================================================================== */
template <class T>
int AES_GCC_MessageQueue<T>::size()
{
	// Aquire mutex lock
	//mutex_->acquire();
        pthread_mutex_lock(&mutex_);

	// Get the size of the list
	int size = list_.size();

	// Release the mutex
	//mutex_->release();
        pthread_mutex_unlock(&mutex_);

	return size;
}

#endif
