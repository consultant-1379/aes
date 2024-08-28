#include <aes_afp_criticalsection.h>
#include <ace/Mutex.h>

std::auto_ptr<aes_afp_criticalsection> aes_afp_criticalsection::instance ;
ACE_Mutex aes_afp_criticalsection::m_mutex;

aes_afp_criticalsection::aes_afp_criticalsection()
{
	section = new ACE_Thread_Mutex();
}

aes_afp_criticalsection::~aes_afp_criticalsection()
{
	section->remove();
	delete section;
}

void aes_afp_criticalsection::enter ()
{
	section->acquire();
}

void aes_afp_criticalsection::leave ()
{
	section->release();
 }

aes_afp_criticalsection& aes_afp_criticalsection::Instance ()
{
	if(!instance.get())
	{
		m_mutex.acquire();
		if(!instance.get())
		{
			std::auto_ptr<aes_afp_criticalsection> temp = std::auto_ptr<aes_afp_criticalsection>(new aes_afp_criticalsection);
			instance = temp;
		}
		m_mutex.release();
	}
	return *instance;

}
