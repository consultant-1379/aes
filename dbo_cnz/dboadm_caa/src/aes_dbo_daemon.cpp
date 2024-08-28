#include <aes_gcc_tracer.h>

#include <aes_dbo_daemon.h>

AES_GCC_TRACE_DEFINE(AES_DBO_daemon)

aes_dbo_daemon::aes_dbo_daemon(const char *task_name, uint n_threads, ACE_Reactor * reactor ){

	if(task_name)
		{
			strncpy(_task_name, task_name, TASK_NAME_SIZE_MAX);
			_task_name[TASK_NAME_SIZE_MAX - 1] = 0;
		}
		else
			bzero(_task_name, sizeof(_task_name));

		_destroy_reactor = false;

		if(reactor)
			_main_reactor = reactor;
		else
		{
			// if reactor has not been passed, create our private reactor, using ACE_TP_Reactor implementation
			ACE_TP_Reactor *reactor_TP_impl = new (std::nothrow) ACE_TP_Reactor();
			if(reactor_TP_impl)
			{
				_main_reactor = new (std::nothrow) ACE_Reactor(reactor_TP_impl, true);	// reactor implementation will be deleted when reactor will be destroyed
				if(_main_reactor)
					_destroy_reactor = true; // take note that we must take care for reactor deallocation
				else
					delete reactor_TP_impl;	// no memory available ...  deallocate memory allocated for reactor TP implementation
			}
			reactor = _main_reactor;
		}

		// check if reactor is OK
		if(!_main_reactor)
		{
			_state = DOWN_NO_MEMORY;
			return;
		}

		_main_reactor->open(1);

		_n_threads = n_threads;
		// set initial state
		_state = UP_NOT_RUNNING;
}

aes_dbo_daemon::~aes_dbo_daemon(){
	//TODO - VERIFY FLAG REACTOR
	if(_main_reactor){
		AES_GCC_TRACE_MESSAGE("Destroying Daemon!");
		delete _main_reactor;
		_main_reactor = 0;
	}

}


int aes_dbo_daemon::svc (void){
	AES_GCC_TRACE_MESSAGE("svc started!");
	if(!_main_reactor)
	{
		//TODO- INSERT PRINT
		return -1;	// the task is inconsistent; terminate thread execution returning -1
	}

	_main_reactor->owner(ACE_OS::thr_self());
	int result = 0;
	AES_GCC_TRACE_MESSAGE("Calling run main reactor!");
	if( (result = _main_reactor->run_reactor_event_loop()))
	{
		AES_GCC_TRACE_MESSAGE("Error! run main reactor event loop - result = %d",result);
	} else{
		AES_GCC_TRACE_MESSAGE("Called end_reactor_loop. Starting shutdown procedure - result = %d",result);
	}
	AES_GCC_TRACE_MESSAGE("Closing svc");
	return 0;
}

int aes_dbo_daemon::start_activity(){

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);
	AES_GCC_TRACE_MESSAGE("Activating svc!");
	// launch threads
	int call_result = activate(THR_NEW_LWP | THR_JOINABLE | THR_INHERIT_SCHED, this->_n_threads);
	if(call_result < 0)
	{
		//LOG ERROR
		AES_GCC_TRACE_MESSAGE("Failed to activate thread!");
		return -1;
	}
	AES_GCC_TRACE_MESSAGE("Successfully started activity");
	_state = UP_AND_RUNNING;
	//LOG
	return 0;
}

int aes_dbo_daemon::stop_activity(bool wait_termination){

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_mutex);

	if(_state == DOWN)
	{
		//LOG WARN
		return 0;
	}

	if(_state == STOPPING)
	{
		//LOG WARN
		return 0;
	}

	// terminate reactor event loop for all threads in the task
	int call_result = _main_reactor->end_reactor_event_loop();
	if(call_result < 0)
	{
		//LOG ERR
		AES_GCC_TRACE_MESSAGE("Error end_reactor_event_loop");
		return -1;
	}

	// update state
	_state = STOPPING;

	// release Task Mutex
	_mutex.release();

	if(wait_termination)
	{
		// wait for threads termination
		AES_GCC_TRACE_MESSAGE("Waiting...to close main reactor");
		call_result = this->wait();
		if(call_result == -1){
			AES_GCC_TRACE_MESSAGE("Error waiting closure of main reactor");
		}
	}
	_main_reactor->close();
	AES_GCC_TRACE_MESSAGE("Exiting from stop activity");
	// update state
	_state = DOWN;

	return call_result;
}
