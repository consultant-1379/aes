/*
 * ACE_Singleton_stub.h
 *
 *  Created on: Aug 17, 2015
 *      Author: xrammat
 */

#ifndef INC_ACE_SINGLETON_STUB_H_
#define INC_ACE_SINGLETON_STUB_H_

class ACE_LOCK
{
	/// CE needs a default constructor here.
	ACE_LOCK (void) { ;}

	/// Noop virtual destructor
	virtual ~ACE_LOCK (void) { ; }
	/**
	 * Explicitly destroy the lock.  Note that only one thread should
	 * call this method since it doesn't protect against race
	 * conditions.
	 */
	virtual int remove (void) { return 0; }

	/// Block the thread until the lock is acquired.  Returns -1 on
	/// failure.
	virtual int acquire (void) { return 0; }

	/**
	 * Conditionally acquire the lock (i.e., won't block).  Returns -1
	 * on failure.  If we "failed" because someone else already had the
	 * lock, @c errno is set to @c EBUSY.
	 */
	virtual int tryacquire (void) { return 0; }

	/// Release the lock.  Returns -1 on failure.
	virtual int release (void) { return 0; }

	/**
	 * Block until the thread acquires a read lock.  If the locking
	 * mechanism doesn't support read locks then this just calls
	 * acquire().  Returns -1 on failure.
	 */
	virtual int acquire_read (void) { return 0; }

	/**
	 * Block until the thread acquires a write lock.  If the locking
	 * mechanism doesn't support read locks then this just calls
	 * acquire().  Returns -1 on failure.
	 */
	virtual int acquire_write (void) { return 0; }

	/**
	 * Conditionally acquire a read lock.  If the locking mechanism
	 * doesn't support read locks then this just calls acquire().
	 * Returns -1 on failure.  If we "failed" because someone else
	 * already had the lock, @c errno is set to @c EBUSY.
	 */
	virtual int tryacquire_read (void) { return 0; }

	/**
	 * Conditionally acquire a write lock.  If the locking mechanism
	 * doesn't support read locks then this just calls acquire().
	 * Returns -1 on failure.  If we "failed" because someone else
	 * already had the lock, @c errno is set to @c EBUSY.
	 */
	virtual int tryacquire_write (void) { return 0; }

	/**
	 * Conditionally try to upgrade a lock held for read to a write lock.
	 * If the locking mechanism doesn't support read locks then this just
	 * calls acquire(). Returns 0 on success, -1 on failure.
	 */
	virtual int tryacquire_write_upgrade (void) { return 0; }

};

class ACE_Cleanup
{
	ACE_Cleanup(void) { ; }
	~ACE_Cleanup(void) { ; }
};
template <class TYPE, class ACE_LOCK>
class ACE_Singleton : public ACE_Cleanup
{
protected:
  /// Default constructor.
  ACE_Singleton (void) { }

  /// Contained instance.
  static TYPE *instance_;

  /// Get pointer to the Singleton instance.
  static ACE_Singleton<TYPE, ACE_LOCK> *&instance_i (void);

public:
  /// Global access point to the Singleton.
  static TYPE * instance (void) { if(instance_ == 0 )instance_ = new TYPE ; return instance_; }//*instance_i; }

  /// Cleanup method, used by @c ace_cleanup_destroyer to destroy the
  /// ACE_Singleton.
  virtual void cleanup (void *param = 0) { ; }

  /// Explicitly delete the Singleton instance.
  static void close (void) {  }

  /// Dump the state of the object.
  static void dump (void) { }


};
template <class TYPE, class ACE_LOCK> TYPE * ACE_Singleton<TYPE,ACE_LOCK>::instance_ =0;


#endif /* INC_ACE_SINGLETON_STUB_H_ */
