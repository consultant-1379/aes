#include "cute_suite.h"
#include "aes_gcc_thread.h"

class GCC_Cute_ThreadImpl : public AES_GCC_Thread
{

public:
	GCC_Cute_ThreadImpl(bool autoclean);
	int exec();
	void finish();
	void init();
};

class GCC_Cute_Thread_Test
{
public:
	static void threadActivate();
	static void threadWait();
	static cute::suite make_suite_gcc_cute_threadImpl();
	static GCC_Cute_ThreadImpl * getInstance()
	{
		return thread;
	}
private:
	static GCC_Cute_ThreadImpl * thread;
};
