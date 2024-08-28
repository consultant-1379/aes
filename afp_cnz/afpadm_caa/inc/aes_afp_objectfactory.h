#ifndef aes_afp_objectfactory_h
#define aes_afp_objectfactory_h 1

#include "aes_afp_file.h"
#include "aes_afp_transferqueue.h"
#include <aes_gcc_log.h>

//## Class: aes_afp_objectfactory%3709CE1C0163
//	The class is responsible for creating objects of classes.
//	Trace points: aes_afp_Objectfactory traces creation and
//	destruction of objects.

class aes_afp_objectfactory 
{
  public:
    //## Destructor (generated)
      virtual ~aes_afp_objectfactory();

      //	Instantiates a object factory.
      static aes_afp_objectfactory& Instance ();

      //	Creates a file object.
      aes_afp_file* makeFileObj ();

      //	Releases a file object.
      void relFileObj (aes_afp_file* itemObj);

      //	Constructs a transfer queue object.
      aes_afp_transferqueue* makeTransferQueueObject ();

      //	Releases a transfer queue object.
      void releaseTransferQueueObject (aes_afp_transferqueue* object);

  protected:

  private:
    // Data Members for Associations

      aes_afp_objectfactory();
      aes_afp_objectfactory(aes_afp_objectfactory const &){};
      void operator =(const aes_afp_objectfactory& rhs){(void)rhs;}

      static std::auto_ptr<aes_afp_objectfactory> instance_;
	  static ACE_Mutex m_mutex;
};
#endif
