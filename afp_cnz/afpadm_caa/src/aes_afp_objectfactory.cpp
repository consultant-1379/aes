#include <aes_afp_objectfactory.h>
#include "aes_afp_logger.h"
#include "aes_afp_api_tracer.h"
AES_AFP_TRACE_DEFINE(AES_AFP_objectfactory);


// Class aes_afp_objectfactory 
std::auto_ptr<aes_afp_objectfactory> aes_afp_objectfactory::instance_ ;
ACE_Mutex aes_afp_objectfactory::m_mutex;

aes_afp_objectfactory::aes_afp_objectfactory()
{
}


aes_afp_objectfactory::~aes_afp_objectfactory()
{
}
//	---------------------------------------------------------
//	       Instance()
//	---------------------------------------------------------
aes_afp_objectfactory& aes_afp_objectfactory::Instance ()
{
	if(!instance_.get())
	{
		m_mutex.acquire();
		if( !instance_.get())
		{
			std::auto_ptr<aes_afp_objectfactory> temp  = std::auto_ptr<aes_afp_objectfactory>(new aes_afp_objectfactory);
			instance_ = temp;
		}
		m_mutex.release();
	}
	return *instance_;
}

//	---------------------------------------------------------
//	       makeFileObj()
//	---------------------------------------------------------
aes_afp_file* aes_afp_objectfactory::makeFileObj ()
{
	return new aes_afp_file;
}

void aes_afp_objectfactory::relFileObj (aes_afp_file* itemObj)
{
	delete itemObj;
	itemObj = 0;
}

//	---------------------------------------------------------
//	       makeTransferQueueObject()
//	---------------------------------------------------------
aes_afp_transferqueue* aes_afp_objectfactory::makeTransferQueueObject ()
{
	AES_AFP_TRACE_MESSAGE("Creating transferqueue object");
	return new (std::nothrow) aes_afp_transferqueue;
}

//	---------------------------------------------------------
//	       releaseTransferQueueObject()
//	---------------------------------------------------------
void aes_afp_objectfactory::releaseTransferQueueObject (aes_afp_transferqueue* object)
{
	AES_AFP_TRACE_MESSAGE("Releasing transferqueue object");
	delete object;
	object = 0;
}
