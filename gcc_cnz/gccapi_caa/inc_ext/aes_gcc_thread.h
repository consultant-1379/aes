/**
	@file   aes_gcc_thread.h

	@brief	This is a wrapper class for the ACE thread manager
	usage:
	Inherit from this class and implement the needed
    methods. The method exec() is automatically
      called when the thread is started.
    In main, call the method activate().
      For example,

      class MyThread: public AES_GCC_Thread
      {
          // Must be implemented.
          virtual int exec()
          {
              while (!exit_)
              {
                  // Start the work here.
              }
              return 0;
          }

          ...implement the needed functions...
      }

      int main(int argc, char* argv[])
      {
          MyThread thread;

          thread.activate();

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

#ifndef AES_GCC_THREAD_H
#define AES_GCC_THREAD_H
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include <ace/Thread_Manager.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

/*===================================================================*/
/**
		@brief AES_GCC_Thread class
		A wrapper for ACE thread manager
 */
/*=================================================================== */
class AES_GCC_Thread
{
	/*=====================================================================
						PRIVATE DECLARATION SECTION
==================================================================== */
private:
	/*=================================================================== */
	/**

	  @brief 			entry
						This is the thread entry which calls the exec() method.

	  @param           	obj
						The parameter is required by CreateThread method and is used
						to pass this object to the static method, to be able to run
						a method in this class.

	  @return          Event is EV_CEASING:
						The return value is required by CreateThread method and is
						not used. The only value returned is 0.

	  @exception       none
	 */
	static ACE_UINT64 entry(void *obj);
	/*=================================================================== */
	/**
	  @brief  autoClean_

		The autoClean attribute is set to true when the thread should
		remove itself after execute
	 */
	/*=================================================================== */
	bool autoClean_;

	/*=====================================================================
							PROTECTED DECLARATION SECTION
	==================================================================== */
protected:
	/*=================================================================== */
	/**
		  @brief  ID_

			Thread ID
	 */
	/*=================================================================== */
	ACE_thread_t ID_;
	/*=================================================================== */
	/**
		  @brief  thStatus_
			The thread handle returned by CreateThread.
	 */
	/*=================================================================== */
	ACE_INT32 thStatus_;
	/*=================================================================== */
	/**
		  @brief  rValue_
			The thread exit code returned from exec().
	 */
	/*=================================================================== */
	ACE_INT32 rValue_;
	/*=================================================================== */
	/**
		  @brief  exit_
		Indicates when a thread should exit.
	 */
	/*=================================================================== */
	bool exit_;
	/*=================================================================== */
	/**
		  @brief  ace_thr_mgr
		Reference for ACE_Thread_Manager class
	 */
	/*=================================================================== */
	ACE_Thread_Manager * ace_thr_mgr;

	/*=====================================================================
							PUBLIC DECLARATION SECTION
	==================================================================== */
public:

	/*=================================================================== */
	/**

	  @brief 			Default constructor.


	  @param           	autoClean
						Set to true for threads that wish to
						  be self cleaned.
	  @return

	  @exception       none
	 */
	/*=================================================================== */
	AES_GCC_Thread(bool autoClean = false);

	/*=================================================================== */
	/**

	  @brief 			Destructor

	  @exception       none
	 */
	/*=================================================================== */
	virtual ~AES_GCC_Thread();
	/*=================================================================== */
	/**

	  @brief 			activate
						Activates the thread and starts execution in exec() method.

	  @return			Returns true if activation was successful otherwise false.

	  @exception       none
	 */
	/*=================================================================== */
	bool activate();
	/*=================================================================== */
	/**

	  @brief 			deActivate
						Deactivates the thread and due to setting the exit_ variable to true.

	  @return			void
	  @exception       none
	 */
	/*=================================================================== */
	void deActivate();
	/*=================================================================== */
	/**

	  @brief 			wait
						 This method halts the execution of the calling thread until
						 the object thread is finished.

	  @param			timeout
						 The calling thread is released after this time
						 even though the condition is not fulfilled.

	  @return			wait() returns true if signal was received before timeout
						otherwise it returns false.
	  @exception       none
	 */
	/*=================================================================== */
	bool wait(int timeout = 0);
	/*=================================================================== */
	/**

	  @brief				wait
							This method halts the execution of the calling thread until
							the object thread is finished.
	  @param				rv
							This makes it possible to achieve the return value
							from the thread.
	  @param			 	timeout
							The calling thread is released after this time
							even though the condition is not fulfilled.

	@return					wait() returns true if signal was received before timeout
							otherwise it returns false.
	@exception       		none
	 */
	/*=================================================================== */
	bool wait(int &rv, int timeout = 0);
	/*=================================================================== */
	/**

	  @brief				init
							This is a possibility for the user to do some initialization
							in the thread before exec() method.
	  @return				void
	  @exception       		none
	 */
	/*=================================================================== */
	virtual void init();
	/*=================================================================== */
	/**

	  @brief				finish
							This method is executed after exec() method.

	  @return				void
	  @exception       		none
	 */
	/*=================================================================== */
	virtual void finish();
	/*=================================================================== */
	/**

	  @brief				exec
							This is the pure virtual method which should be overridden in
							class inherited from this class, and this is the thread main
							execution point.

	@return					It is possible for the thread to leave an integer as a return value.
	@exception       		none
	 */
	/*=================================================================== */
	virtual int exec() = 0;
};

#endif
