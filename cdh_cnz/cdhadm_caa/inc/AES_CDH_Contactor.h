#ifndef AES_CDH_CONTACTOR_H
#define AES_CDH_CONTACTOR_H

#include <bgwrpc.h>

class AES_CDH_Contactor
{

private:
	char*             m_HostAddress;
	BGWRPC*           m_bgwP;
	unsigned long     m_bgwNR;
	CLIENT*           m_clientP;

	static ACE_Thread_Mutex Clnt_mutex;

public:
	AES_CDH_Contactor(BGWRPC* bp,char* h,unsigned long b);
	void SocketObjectCreation();
	CLIENT* getResult();
	~AES_CDH_Contactor();

private:
	bool hostAddressCheck();

};
#endif
