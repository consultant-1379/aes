
#ifndef aes_afp_parameter_h
#define aes_afp_parameter_h 1



#include <acs_aeh_evreport.h>
//#include "ACS_PHA_Tables.h"
//#include "ACS_PHA_Parameter.h"
#include "aes_afp_events.h"
#include <string>
//#include <osf/OS.h>

#include <aes_gcc_variable.h>
#include <aes_gcc_event.h>
#include <aes_gcc_eventhandler.h>
#include <aes_gcc_log.h>
#include <ace/ACE.h>
#include <ace/OS.h>

#define AES_AFP_STORAGE "aes_afp"

//  type used in parameter fetch
struct string127
{
  char str[128];
};

//	Handles AFP parameters stored in PHA.
//	Trace points: aes_afp_Parameter traces the retreived
//	parameters.
class aes_afp_parameter 
{
  public:

    //## Other Operations (specified)
      //## Operation: load%37649EE403B3
      //	Load AFP parameters from PHA.
      static int load ();

      //## Operation: dataDir%37649F810119
      //	Retrieves the afp root directory.
      static std::string dataDir ();

      //## Operation: delimiter%3770FD3A0040
      //	Retrieves the delimiter string.
      static std::string delimiter ();

      //## Operation: setRootDir%37649FBF017D
      //	only for DEBUG usage!
      static int setRootDir (const char *str);

      //## Operation: installedApio%3B417EBA0399; C++
      //	Checks if afp is installed on apg with apio.
      static bool installedApio ();

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: apio%3B417FD001A4
      //## begin aes_afp_parameter::apio%3B417FD001A4.attr preserve=no  private: static bool {U} false
      static bool apio_;
      //## end aes_afp_parameter::apio%3B417FD001A4.attr

    // Data Members for Associations

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B0919B90143
      //## Role: aes_afp_parameter::afpDataDir%3B0919BA03BB
      //## begin aes_afp_parameter::afpDataDir%3B0919BA03BB.role preserve=no  public: static string { -> 1UHgN}
      static std::string afpDataDir_;
};


#endif
