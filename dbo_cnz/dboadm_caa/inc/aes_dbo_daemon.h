#ifndef AES_DBO_DAEMON_H_
#define AES_DBO_DAEMON_H_

#include "ace/Task.h"
#include "ace/TP_Reactor.h"
#include "ace/Reactor.h"
#include "ace/Thread.h"

#define TASK_NAME_SIZE_MAX 16
#define DBO_RUN_REACTOR_RETRY 60
#define DBO_DEFAULT_THR 1

class aes_dbo_daemon: public ACE_Task_Base
{

public:

	// State of the object
	enum DBO_Task_State {
		UP_AND_RUNNING = 0,
		UP_NOT_RUNNING = 1,
		STOPPING = 2,
		DOWN = 3,
		DOWN_NO_MEMORY = 4
	};
	//==============//
	// Constructors //
	//==============//

	aes_dbo_daemon(const char *task_name = "anonymous", uint n_threads = 1, ACE_Reactor * reactor = 0);

	//==============//
	// Destructor   //
	//==============//

	virtual ~aes_dbo_daemon();

	//===========//
	// Functions //
	//===========//

	/// Returns the name of the Task
	const char *get_name() { return _task_name; }

	virtual int svc (void);

	virtual int start_activity();

	virtual int stop_activity(bool wait_termination);

public:
	ACE_Reactor * _main_reactor;						/// reactor associated to the Task
private:
	char _task_name[TASK_NAME_SIZE_MAX];		/// Task name

	uint32_t _n_threads;						/// number of threads active in the Task
	bool _destroy_reactor;						/// true if <_reactor> attribute has been created by the constructor (and so we have to deallocate)

	DBO_Task_State _state;					/// object state
	/// dynamic handling of threads
	ACE_Recursive_Thread_Mutex _mutex;

};
#endif /* AES_DBO_DAEMON_H_ */
