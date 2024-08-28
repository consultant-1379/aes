#include "cute.h"
#include "ide_listener.h"
#include "cute_runner.h"
#include "gcc_cute_threadImpl.h"
#include <iostream>

GCC_Cute_ThreadImpl * GCC_Cute_Thread_Test::thread;
GCC_Cute_ThreadImpl::GCC_Cute_ThreadImpl(bool autoclean ):AES_GCC_Thread(autoclean)
{
	std::cout<<"Calling base class constructor "<<std::endl;
}
int GCC_Cute_ThreadImpl::exec()
{
	int sum=0;
	for(int i=0;i<10;i++)
	{
		sum+=i;
	}
	std::cout<<"Sum ="<<sum<<std::endl;
	return sum;
}
void GCC_Cute_ThreadImpl::finish()
{
	std::cout<<" Thread finished it's job "<<std::endl;
}
void GCC_Cute_ThreadImpl::init()
{
	std::cout<<" Thread init called"<<std::endl;
}

void GCC_Cute_Thread_Test::threadActivate()
{

	bool status = GCC_Cute_Thread_Test::getInstance()->activate();
	if(status)
	{
		std::cout<<"Thread Activation successful "<<std::endl;
//		status = GCC_Cute_Thread_Test::getInstance()->wait();
//		ASSERTM("Thread wait failed", status);
	}
	else
	{
		ASSERTM("Thread activation failed", status);
	}
}
void GCC_Cute_Thread_Test::threadWait()
{
	bool status = GCC_Cute_Thread_Test::getInstance()->wait(2000);
	if(status)
	{
		std::cout<<"Thread wait successful "<<std::endl;
	}
	else
	{
		ASSERTM("Thread wait failed", status);

	}
}

cute::suite GCC_Cute_Thread_Test::make_suite_gcc_cute_threadImpl()
{
	thread = new GCC_Cute_ThreadImpl(false);
	cute::suite s;
	s.push_back(CUTE(threadActivate));
	s.push_back(CUTE(threadWait));
	return s;
}

