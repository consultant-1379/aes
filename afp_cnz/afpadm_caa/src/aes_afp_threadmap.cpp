#include <aes_afp_threadmap.h>

std::auto_ptr<aes_afp_threadmap> aes_afp_threadmap::instance_;
ACE_Recursive_Thread_Mutex aes_afp_threadmap::singletonSynch;
ACE_Mutex aes_afp_threadmap::m_mutex;

aes_afp_threadmap::~aes_afp_threadmap()
{
	threadMap_.clear();
}

aes_afp_threadmap& aes_afp_threadmap::Instance ()
{
	if(!instance_.get())
	{
		m_mutex.acquire();
		if(!instance_.get())
		{
			std::auto_ptr<aes_afp_threadmap> temp = std::auto_ptr<aes_afp_threadmap>(new aes_afp_threadmap);
			instance_ = temp;
		}
		m_mutex.release();
	}
	return *instance_;
}

bool aes_afp_threadmap::insert (const std::string& name, void* threadId)
{
	aes_afp_threadmap::singletonSynch.acquire();
	threadInsPair_.first = name;
	threadInsPair_.second = threadId;

	threadMap_.insert(threadInsPair_);
	aes_afp_threadmap::singletonSynch.release();
	return true;
}

bool aes_afp_threadmap::remove (const std::string& name)
{
	ThreadMap::iterator itr;
	aes_afp_threadmap::singletonSynch.acquire();
	itr = threadMap_.find(name);
	threadMap_.erase(itr);
	aes_afp_threadmap::singletonSynch.release();
	return true;
}

void* aes_afp_threadmap::searchId (const std::string& name)
{
	ThreadMap::iterator itr;
	aes_afp_threadmap::singletonSynch.acquire();
	itr = threadMap_.find(name);
	if (itr == threadMap_.end() )
	{
		aes_afp_threadmap::singletonSynch.release();
		return 0;
	}
	aes_afp_threadmap::singletonSynch.release();
	return (*itr).second;
}
