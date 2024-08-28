//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3AE57C010230.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3AE57C010230.cm

//## begin module%3AE57C010230.cp preserve=no
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
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
//	 2003-01-28 UAB/KB/AU DAPA
//
//	REVISION
//	 A 2003-01-28 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3AE57C010230.cp

//## Module: aes_afp_date_time%3AE57C010230; Package specification
//## Subsystem: AFP::afpadm_caa::inc%3AFFB40F01D1
//## Source file: Z:\ntaes\afp\afpadm_caa\inc\aes_afp_date_time.h

#ifndef aes_afp_date_time_h
#define aes_afp_date_time_h 1

//## begin module%3AE57C010230.additionalIncludes preserve=no
//## end module%3AE57C010230.additionalIncludes

//## begin module%3AE57C010230.includes preserve=yes
//## end module%3AE57C010230.includes

#include <string>
//## begin module%3AE57C010230.declarations preserve=no
//## end module%3AE57C010230.declarations

//## begin module%3AE57C010230.additionalDeclarations preserve=yes
//## end module%3AE57C010230.additionalDeclarations


//## begin aes_afp_date_time%3AE5786F0056.preface preserve=yes
//## end aes_afp_date_time%3AE5786F0056.preface

//## Class: aes_afp_date_time%3AE5786F0056
//	This class is used to store date and time.
//	Trace points: no
//## Category: afpadm_caa(CAA 109 0503%3AFFB387001D
//## Subsystem: AFP::afpadm_caa::inc%3AFFB40F01D1
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%3E36970E038C;string { -> }

class aes_afp_date_time 
{
  //## begin aes_afp_date_time%3AE5786F0056.initialDeclarations preserve=yes
  //## end aes_afp_date_time%3AE5786F0056.initialDeclarations

  public:
    //## Constructors (specified)
      //## Operation: aes_afp_date_time%3AE68C9303A9; C++
      //	Constructor.
      aes_afp_date_time (unsigned short year = 0, unsigned short month = 0, unsigned short day = 0, unsigned short hour = 0, unsigned short minute = 0, unsigned short second = 0, unsigned short sequence = 0);

    //## Destructor (generated)
      virtual ~aes_afp_date_time();


    //## Other Operations (specified)
      //## Operation: setYear%3AE5825D0119; C++
      //	Sets the year field.
      void setYear (unsigned short value = 0);

      //## Operation: getYear%3AE582A202C7
      //	Retrieves the year field.
      unsigned short getYear () const;

      //## Operation: setMonth%3AE582EE03A2; C++
      //	Sets the month field.
      void setMonth (unsigned short value = 0);

      //## Operation: getMonth%3AE582FB010C; C++
      //	Retrieves the year field.
      unsigned short getMonth () const;

      //## Operation: setDay%3AE58304034A; C++
      //	Sets the day field.
      void setDay (unsigned short value = 0);

      //## Operation: getDay%3AE5830E0164; C++
      //	Retrieves the day field.
      unsigned short getDay () const;

      //## Operation: setHour%3AE583180172; C++
      //	Sets the hour field.
      void setHour (unsigned short value = 0);

      //## Operation: getHour%3AE583260064; C++
      //	Retrieves the hour field.
      unsigned short getHour () const;

      //## Operation: setMin%3AE5832F01ED; C++
      //	Sets the minute field.
      void setMin (unsigned short value = 0);

      //## Operation: getMin%3AE5833E03A8; C++
      //	Retrieves the minute field.
      unsigned short getMin () const;

      //## Operation: setSec%3AE5834F0121; C++
      //	Sets the second field.
      void setSec (unsigned short value = 0);

      //## Operation: getSec%3AE58355030A; C++
      //	Retrieves the second field.
      unsigned short getSec () const;

      //## Operation: setSeq%3AE58363010C; C++
      //	Sets the sequenze number field.
      void setSeq (unsigned short value = 0);

      //## Operation: getSeq%3AE5836F0181; C++
      //	Retrieves the sequenze number field.
      unsigned short getSeq () const;

      //## Operation: getDateString%3E36910400B8
      //	Retrieves dates as a string.
      std::string getDateString ();

      //## Operation: setDateString%3E36911F0247; C++
      //	Sets dates from a string.
      void setDateString (std::string& dateString);

    // Additional Public Declarations
      //## begin aes_afp_date_time%3AE5786F0056.public preserve=yes
      //## end aes_afp_date_time%3AE5786F0056.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_date_time%3AE5786F0056.protected preserve=yes
      //## end aes_afp_date_time%3AE5786F0056.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_date_time%3AE5786F0056.private preserve=yes
      //## end aes_afp_date_time%3AE5786F0056.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: year%3AE5790E03A7
      //## begin aes_afp_date_time::year%3AE5790E03A7.attr preserve=no  private: unsigned short {V} 0
      unsigned short year_;
      //## end aes_afp_date_time::year%3AE5790E03A7.attr

      //## Attribute: month%3AE579340059
      //## begin aes_afp_date_time::month%3AE579340059.attr preserve=no  private: unsigned short {V} 0
      unsigned short month_;
      //## end aes_afp_date_time::month%3AE579340059.attr

      //## Attribute: day%3AE579470179
      //## begin aes_afp_date_time::day%3AE579470179.attr preserve=no  private: unsigned short {V} 0
      unsigned short day_;
      //## end aes_afp_date_time::day%3AE579470179.attr

      //## Attribute: hour%3AE579520124
      //## begin aes_afp_date_time::hour%3AE579520124.attr preserve=no  private: unsigned short {V} 0
      unsigned short hour_;
      //## end aes_afp_date_time::hour%3AE579520124.attr

      //## Attribute: min%3AE57960030F
      //## begin aes_afp_date_time::min%3AE57960030F.attr preserve=no  private: unsigned short {V} 0
      unsigned short min_;
      //## end aes_afp_date_time::min%3AE57960030F.attr

      //## Attribute: sec%3AE57971006B
      //## begin aes_afp_date_time::sec%3AE57971006B.attr preserve=no  private: unsigned short {V} 0
      unsigned short sec_;
      //## end aes_afp_date_time::sec%3AE57971006B.attr

      //## Attribute: seqnr%3AE5797D00CC
      //## begin aes_afp_date_time::seqnr%3AE5797D00CC.attr preserve=no  private: unsigned short {V} 0
      unsigned short seqnr_;
      //## end aes_afp_date_time::seqnr%3AE5797D00CC.attr

    // Additional Implementation Declarations
      //## begin aes_afp_date_time%3AE5786F0056.implementation preserve=yes
      //## end aes_afp_date_time%3AE5786F0056.implementation

};

//## begin aes_afp_date_time%3AE5786F0056.postscript preserve=yes
//## end aes_afp_date_time%3AE5786F0056.postscript

// Class aes_afp_date_time 

//## begin module%3AE57C010230.epilog preserve=yes
//## end module%3AE57C010230.epilog


#endif
