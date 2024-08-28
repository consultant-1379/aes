#include <aes_afp_dsdsessionmap.h>
#include <ace/Mutex.h>

// Class aes_afp_dsdsessionmap 


std::auto_ptr<aes_afp_dsdsessionmap> aes_afp_dsdsessionmap::instance_;
ACE_Mutex aes_afp_dsdsessionmap::m_mutex;

aes_afp_dsdsessionmap::aes_afp_dsdsessionmap()
{
}
aes_afp_dsdsessionmap::~aes_afp_dsdsessionmap()
{
}

aes_afp_dsdsessionmap& aes_afp_dsdsessionmap::Instance ()
{
	if(!instance_.get())
	{
		m_mutex.acquire();
		if(!instance_.get())
		{
			std::auto_ptr<aes_afp_dsdsessionmap> temp = std::auto_ptr<aes_afp_dsdsessionmap>(new aes_afp_dsdsessionmap);
			instance_ = temp;
		}
		m_mutex.release();
	}
	return *instance_;

}

bool aes_afp_dsdsessionmap::insert (const std::string& name, ACS_DSD_Session* session)
{
	sessionPair_.first = name;
	sessionPair_.second = session;

	sessionMap_.insert(sessionPair_);
	return true;
}

bool aes_afp_dsdsessionmap::remove (const std::string& name)
{
	SessionMap::iterator itr;

	itr = sessionMap_.find(name);
	if (itr != sessionMap_.end() )
	{
		sessionMap_.erase(itr);
	}
	return true;
}

ACS_DSD_Session* aes_afp_dsdsessionmap::searchSession (const std::string& name)
{
	SessionMap::iterator itr;

	itr = sessionMap_.find(name);
	if (itr == sessionMap_.end() )
	{
		return 0;
	}
	return (*itr).second;
}
