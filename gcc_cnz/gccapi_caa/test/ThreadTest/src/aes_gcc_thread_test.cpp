#include<iostream>

#include "aes_gcc_thread.h"
#include <ace/Thread_Manager.h>
#include <ace/ACE.h>
#include <iostream>
#include "aes_gcc_log.h"

using namespace std;


//ACS_TRA_trace * pThreadTest = NULL ;
GCC_TDEF(pThreadTest);

class MyThread:public AES_GCC_Thread
{
	// Must be implemented.
public:

	MyThread(bool autoclean);
	int exec();
	void finish();
	void init();

};
MyThread::MyThread(bool autoclean ):AES_GCC_Thread(autoclean)
{
	//	cout<<"Calling base class constructor "<<endl;
	GCC_DEBUG(("Calling base class constructor "));
}
int MyThread::exec()
{
	//	cout<<"In method MyThread::exec()"<<endl;
	GCC_DEBUG(("In method MyThread::exec() "));

	{
		for(int i=0;i<100;i++)
		{
			//		cout<<"Thread running ,Loop no :"<<i<<endl;
			GCC_DEBUG(("Thread running ,Loop no : %d",i));
		}
		//	cout<<"Thread Execution completed"<<endl;
		GCC_DEBUG(("%s","Thread Execution completed"));
	}
	return 0;
}
void MyThread::finish()
{
//	cout<<" Thread finished it's job "<<endl;
	GCC_DEBUG(("%s","Thread finished it's job"));
}
void MyThread::init()
{
//	cout<<" Thread init called"<<endl;
	GCC_DEBUG(("%s","Thread init called"));
}

int main()
{
	MyThread thread(true);
	bool status = thread.activate();
//	cout<<"Status of thread activate :"<<status<<endl;
	GCC_DEBUG(("Status of thread activate : %d",status));
	status = thread.wait(2000);
	if(status == true)
	{
		thread.deActivate();
	}

	return 0;
}

