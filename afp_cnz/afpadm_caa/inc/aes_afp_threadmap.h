#ifndef aes_afp_threadmap_h
#define aes_afp_threadmap_h 1

#include <utility>
#include <map>
#include <string>
#include <ace/Synch.h>
#include <ace/Guard_T.h>
#include <aes_afp_criticalsection.h>
#include <memory>
using namespace std;

typedef std::map< std::string , void* , std::less<std::string>  > ThreadMap;

typedef std::pair< std::string , void*  > ThreadPair;

//## Class: aes_afp_threadmap
//	The class is responsible for storing the names of the
//	started threads, the class has methods for matching a
//	thread name with the object pointer.

class aes_afp_threadmap 
{
  public:
      virtual ~aes_afp_threadmap();

      //	Singleton instance of threadmap.
      static aes_afp_threadmap& Instance ();

      //	Insert an entry in the map.
      bool insert (const std::string& name, void* threadId);

      //	Remove an entry from the map.
      bool remove (const std::string& name);

      //	Search for an entry in the map.
      void* searchId (const std::string& name);

  private:
	  //Constructor
	  aes_afp_threadmap()
	  {
		  //Just to make the constructor private so that it is not accessed by any 
		  //other thread accidently.....
	  }

  private:
    // Data Members for Associations

	aes_afp_threadmap(aes_afp_threadmap const &);
	void operator =(const aes_afp_threadmap& rhs);


      static std::auto_ptr<aes_afp_threadmap> instance_;
      ThreadPair threadInsPair_;
      ThreadMap threadMap_;

	  // Mutex used to implement double check pattern on singleton
      static ACE_Recursive_Thread_Mutex singletonSynch;

	  static ACE_Mutex m_mutex;
};
#endif
