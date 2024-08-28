/*
 * ACE_RW_Thread_Mutex_stub.h
 *
 *  Created on: Aug 17, 2015
 *      Author: xrammat
 */

#ifndef INC_ACE_RW_THREAD_MUTEX_STUB_H_
#define INC_ACE_RW_THREAD_MUTEX_STUB_H_

class ACE_RW_Mutex
{
public:
  /// Initialize a readers/writer lock.
  ACE_RW_Mutex (int type = 0,
                const char *name = 0,
                void *arg = 0)  { ; }

  /// Implicitly destroy a readers/writer lock
  ~ACE_RW_Mutex (void) { ; }

  /**
   * Explicitly destroy a readers/writer lock.  Note that only one
   * thread should call this method since it doesn't protect against
   * race conditions.
   */
  int remove (void) { return 0; }

  /// Acquire a read lock, but block if a writer hold the lock.
  int acquire_read (void) { return 0; }

  /// Acquire a write lock, but block if any readers or a
  /// writer hold the lock.
  int acquire_write (void) { return 0; }

  /**
   * Conditionally acquire a read lock (i.e., won't block).  Returns
   * -1 on failure.  If we "failed" because someone else already had
   * the lock, @c errno is set to @c EBUSY.
   */
  int tryacquire_read (void) { return 0; }

  /// Conditionally acquire a write lock (i.e., won't block).
  int tryacquire_write (void) { return 0; }

  /**
    * Conditionally upgrade a read lock to a write lock.  This only
    * works if there are no other readers present, in which case the
    * method returns 0.  Otherwise, the method returns -1 and sets
    * @c errno to @c EBUSY.  Note that the caller of this method *must*
    * already possess this lock as a read lock (but this condition is
    * not checked by the current implementation).
    */
   int tryacquire_write_upgrade (void) { return 0; }

   /**
    * Note, for interface uniformity with other synchronization
    * wrappers we include the <acquire> method.  This is implemented as
    * a write-lock to safe...
    */
   int acquire (void) { return 0; }

   /**
    * Note, for interface uniformity with other synchronization
    * wrappers we include the tryacquire() method.  This is implemented
    * as a write-lock to be safe...  Returns -1 on failure.  If we
    * "failed" because someone else already had the lock, @c errno is
    * set to @c EBUSY.
    */
   int tryacquire (void) { return 0; }

   /// Unlock a readers/writer lock.
   int release (void) { return 0; }

   /// Dump the state of an object.
   void dump (void) const { ; }

protected:

  /// Keeps track of whether remove() has been called yet to avoid
  /// multiple remove() calls, e.g., explicitly and implicitly in the
  /// destructor. This flag isn't protected by a lock, so make sure
  /// that you don't have multiple threads simultaneously calling
  /// remove() on the same object, which is a bad idea anyway...
  bool removed_;

private:
  // = Prevent assignment and initialization.
  void operator= (const ACE_RW_Mutex &) { ; }
};


class ACE_RW_Thread_Mutex : public ACE_RW_Mutex
{
public:
	ACE_RW_Thread_Mutex (const char *name = 0,
			void *arg = 0) { ; }

	/// Default dtor.
	~ACE_RW_Thread_Mutex (void) { ; }

	/**
	 * Conditionally upgrade a read lock to a write lock.  This only
	 * works if there are no other readers present, in which case the
	 * method returns 0.  Otherwise, the method returns -1 and sets
	 * @c errno to @c EBUSY.  Note that the caller of this method *must*
	 * already possess this lock as a read lock (but this condition is
	 * not checked by the current implementation).
	 */
	int tryacquire_write_upgrade (void) { return 0; }

	/// Dump the state of an object.
	void dump (void) const { ; }

};

struct ACE_mutexattr_t
{
  int type;
};

class ACE_Recursive_Thread_Mutex
{
public:
	/// Initialize a recursive mutex.
	ACE_Recursive_Thread_Mutex (const char *name = 0,
			ACE_mutexattr_t *arg = 0) { removed_ = true;}

	/// Implicitly release a recursive mutex.
	~ACE_Recursive_Thread_Mutex (void) { ; }

	int remove (void) { return 0; }

	int acquire (void) { return 0; }

	int tryacquire (void) { return 0; }

	int acquire_read (void) { return 0; }

	int acquire_write (void) { return 0; }

	int tryacquire_read (void) { return 0; }

	int tryacquire_write (void) { return 0; }
	int tryacquire_write_upgrade (void) { return 0; }
	int release (void) { return 0; }

	int get_nesting_level (void) { return 0; }
	/// Dump the state of an object.
	void dump (void) const { };

protected:

	bool removed_;
private:
	// = Prevent assignment and initialization.
	void operator= (const ACE_Recursive_Thread_Mutex &) { ; }
};



#endif /* INC_ACE_RW_THREAD_MUTEX_STUB_H_ */
