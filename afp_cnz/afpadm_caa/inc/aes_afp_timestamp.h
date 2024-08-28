//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3737F53602FD.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3737F53602FD.cm

//## begin module%3737F53602FD.cp preserve=no
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
//
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	 190 89-CAA 109 0503
//
//	AUTHOR
//	 2003-01-29 UAB/KB/AU DAPA
//
//	REVISION
//	 A 2003-01-29 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3737F53602FD.cp

//## Module: aes_afp_timestamp%3737F53602FD; Package specification
//## Subsystem: AFP::afpadm_caa::inc%3AFFB40F01D1
//## Source file: Z:\ntaes\afp\afpadm_caa\inc\aes_afp_timestamp.h

#ifndef aes_afp_timestamp_h
#define aes_afp_timestamp_h 1

//## begin module%3737F53602FD.additionalIncludes preserve=no
//## end module%3737F53602FD.additionalIncludes

//## begin module%3737F53602FD.includes preserve=yes
//## end module%3737F53602FD.includes

#include <aes_afp_date_time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <ace/OS.h>
#include <memory>
#include <ace/Mutex.h>
using namespace std;

//## Class: aes_afp_timestamp%3737F44A0268
//	This class is used to create a timestamp. The timestamp
//	has the following fields: year, month, day, hour,
//	minute, second, sequenze number. The sequenze number is
//	used when the there is no difference according to the
//	other fields.
//	Trace points: no

class aes_afp_timestamp 
{
  public:
      static aes_afp_timestamp& Instance ();

      virtual ~aes_afp_timestamp();

      //	Returns a time stamp as a date_time object.
      aes_afp_date_time getTimeStamp ();

      //	Returns the time in UTC seconds
      unsigned long getAbsTime ();

      //	Returns a time stamp as a string object.
      std::string getTimeString ();

      //	Retrieves time stamp as string.
      std::string getAbsoluteTimeString ();

      //	Returns time as character pointer.
      char* getTimeChar ();

      //	Removes the sequence number from a time string
      std::string& removeSequenceNumber (std::string timeStr);


  protected:

  private:

  private: //## implementation


	aes_afp_timestamp();
	aes_afp_timestamp(aes_afp_timestamp const &);
	void operator =(const aes_afp_timestamp& rhs);

	static std::auto_ptr<aes_afp_timestamp> instance_;

    // Data Members for Class Attributes

      //	Placeholder for the sequenze number in case the
      //	timestamp is equal to the previous timestamp.
      long seqCounter_;

      std::string timeString_;

      std::string absTimeString_;

      char timeBuf_[64];

      std::string removedSeqNumber_;


      ACE_Time_Value timeValue_;

      aes_afp_date_time savedTime_;

	  static ACE_Mutex m_mutex;

};
#endif
