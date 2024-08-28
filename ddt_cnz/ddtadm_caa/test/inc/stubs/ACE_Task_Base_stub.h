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
#ifndef ACE_TASK_BASE_STUB_H_
#define ACE_TASK_BASE_STUB_H_

#include <iostream>
#include <sys/time.h>

enum ACE_enum_stub
{
	THR_NEW_LWP = 0,
	THR_JOINABLE,
	THR_INHERIT_SCHED
};
//ACE_Time_Value class
class ACE_MT_SYNCH {};
class ACE_Time_Value{
public:
	ACE_Time_Value(const int& val) {}
	ACE_Time_Value(time_t sec, suseconds_t usec) {}
	ACE_Time_Value() {}
	~ACE_Time_Value() {}
	void set (time_t sec, suseconds_t usec) { }
};

typedef unsigned long ACE_Reactor_Mask;
class ACE_Event_Handler
{
public:
enum mask{
	LO_PRIORITY,
	HI_PRIORITY = 10,
	NULL_MASK = 0,
	ACCEPT_MASK = (1 << 3),
	ALL_EVENTS_MASK,
	DONT_CALL
};
};
typedef int ACE_HANDLE;
class ACE_Handle_Set
{
public:
  /// Constructor, initializes the bitmask to all 0s.
  ACE_Handle_Set (void){}
  inline int is_set (ACE_HANDLE handle) const{return 0;}
  inline void set_bit (ACE_HANDLE handle){}
  };

class ACE_Reactor
{
public:
  enum
  {
    GET_MASK = 1,
    SET_MASK = 2,
    ADD_MASK = 3,
    CLR_MASK = 4
  };

  int run_event_loop (void){return 0;}
  int end_reactor_event_loop (void){return 0;}
  ACE_Reactor (){}
  virtual ~ACE_Reactor (void){}
  int open (){return 0;}

  int remove_handler (ACE_Handle_Set& handle,
                      int masks){return 0;}
  virtual long schedule_timer (ACE_Event_Handler *event_handler,
                                int timerId,
                                const ACE_Time_Value &delay){return 0;}
  int register_handler (ACE_Handle_Set io_handle,
                        ACE_Event_Handler *event_handler,
                       int mask){return 0;}
  int cancel_timer(int id){return 0;}
};
class ACE_TP_Reactor
{
public:
	ACE_TP_Reactor(){;}
	~ACE_TP_Reactor(){;}
};
class ACE_Task_Base : public ACE_Event_Handler
{
 public:
	inline ACE_Task_Base() : m_thr_count(0) {};
	inline virtual ~ACE_Task_Base() {};

	inline virtual int activate(int flag)
	{
		if (m_thr_count)
		{
			return 1;
		}
		else
		{
			m_thr_count++;
			return 0;
		}
	}

	virtual int svc () = 0;

	int simulateExecution()
	{
		int result = svc();
		m_thr_count--;
		return result;
	}

	virtual int thr_count()
	{
		return m_thr_count;
	}

	virtual inline int wait() { return m_thr_count; }

	virtual inline void setThrCount(int value = 0) { m_thr_count = value; }
	virtual inline void incrThrCount() { m_thr_count++; }
	virtual inline void decrThrCount() { m_thr_count--; }

 private:
	int m_thr_count;
};

template <class ACE_MT_SYNCH>
class ACE_Task : public ACE_Task_Base
{
public:
	inline ACE_Task() {};
	inline virtual ~ACE_Task() {};
};
class  ACE_Thread_Mutex
{
public:
  /// Constructor.
  ACE_Thread_Mutex (){}
  /// Implicitly destroy the mutex.
  ~ACE_Thread_Mutex (void){}
};

template <class ACE_LOCK>
class ACE_Guard
{
public:
  ACE_Guard (ACE_LOCK &lock, bool block = true){}
  ~ACE_Guard (void){}
};

#endif /* ACE_TASK_BASE_STUB_H_ */
