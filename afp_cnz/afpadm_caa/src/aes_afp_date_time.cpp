//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3AE57CB40350.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3AE57CB40350.cm

//## begin module%3AE57CB40350.cp preserve=no
//	INCLUDEaes_afp_date_time.h
//
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
//## end module%3AE57CB40350.cp

//## Module: aes_afp_date_time%3AE57CB40350; Package body
//## Subsystem: AFP::afpadm_caa::src%3AFFB41E0241
//## Source file: Z:\ntaes\afp\afpadm_caa\src\aes_afp_date_time.cpp

//## begin module%3AE57CB40350.additionalIncludes preserve=no
//## end module%3AE57CB40350.additionalIncludes

//## begin module%3AE57CB40350.includes preserve=yes
//## end module%3AE57CB40350.includes

#include <aes_afp_date_time.h>
//## begin module%3AE57CB40350.declarations preserve=no
//## end module%3AE57CB40350.declarations

//## begin module%3AE57CB40350.additionalDeclarations preserve=yes
//## end module%3AE57CB40350.additionalDeclarations


// Class aes_afp_date_time 

//## Operation: aes_afp_date_time%3AE68C9303A9; C++
//## Semantics:
//	---------------------------------------------------------
//	       Constructor()
//	---------------------------------------------------------
aes_afp_date_time::aes_afp_date_time (unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second, unsigned short sequence)
  //## begin aes_afp_date_time::aes_afp_date_time%3AE68C9303A9.hasinit preserve=no
      : year_(0),
        month_(0),
        day_(0),
        hour_(0),
        min_(0),
        sec_(0),
        seqnr_(0)
  //## end aes_afp_date_time::aes_afp_date_time%3AE68C9303A9.hasinit
  //## begin aes_afp_date_time::aes_afp_date_time%3AE68C9303A9.initialization preserve=yes
  //## end aes_afp_date_time::aes_afp_date_time%3AE68C9303A9.initialization
{
  //## begin aes_afp_date_time::aes_afp_date_time%3AE68C9303A9.body preserve=yes
	year_ = year;
	month_ = month;
	day_ = day;
	hour_ = hour;
	min_ = minute;
	sec_ = second;
	seqnr_ = sequence;
  //## end aes_afp_date_time::aes_afp_date_time%3AE68C9303A9.body
}


aes_afp_date_time::~aes_afp_date_time()
{
  //## begin aes_afp_date_time::~aes_afp_date_time%3AE5786F0056_dest.body preserve=yes
  //## end aes_afp_date_time::~aes_afp_date_time%3AE5786F0056_dest.body
}



//## Other Operations (implementation)
//## Operation: setYear%3AE5825D0119; C++
//## Semantics:
//	---------------------------------------------------------
//	       setYear()
//	---------------------------------------------------------
void aes_afp_date_time::setYear (unsigned short value)
{
  //## begin aes_afp_date_time::setYear%3AE5825D0119.body preserve=yes
	year_ = value;
  //## end aes_afp_date_time::setYear%3AE5825D0119.body
}

//## Operation: getYear%3AE582A202C7
//## Semantics:
//	---------------------------------------------------------
//	       getYear()
//	---------------------------------------------------------
unsigned short aes_afp_date_time::getYear () const
{
  //## begin aes_afp_date_time::getYear%3AE582A202C7.body preserve=yes
	return year_;
  //## end aes_afp_date_time::getYear%3AE582A202C7.body
}

//## Operation: setMonth%3AE582EE03A2; C++
//## Semantics:
//	---------------------------------------------------------
//	       setMonth()
//	---------------------------------------------------------
void aes_afp_date_time::setMonth (unsigned short value)
{
  //## begin aes_afp_date_time::setMonth%3AE582EE03A2.body preserve=yes
	month_ = value;
  //## end aes_afp_date_time::setMonth%3AE582EE03A2.body
}

//## Operation: getMonth%3AE582FB010C; C++
//## Semantics:
//	---------------------------------------------------------
//	       getMonth()
//	---------------------------------------------------------
unsigned short aes_afp_date_time::getMonth () const
{
  //## begin aes_afp_date_time::getMonth%3AE582FB010C.body preserve=yes
	return month_;
  //## end aes_afp_date_time::getMonth%3AE582FB010C.body
}

//## Operation: setDay%3AE58304034A; C++
//## Semantics:
//	---------------------------------------------------------
//	       setDay()
//	---------------------------------------------------------
void aes_afp_date_time::setDay (unsigned short value)
{
  //## begin aes_afp_date_time::setDay%3AE58304034A.body preserve=yes
	day_ = value;
  //## end aes_afp_date_time::setDay%3AE58304034A.body
}

//## Operation: getDay%3AE5830E0164; C++
//## Semantics:
//	---------------------------------------------------------
//	       getDay()
//	---------------------------------------------------------
unsigned short aes_afp_date_time::getDay ()const
{
  //## begin aes_afp_date_time::getDay%3AE5830E0164.body preserve=yes
	return day_;
  //## end aes_afp_date_time::getDay%3AE5830E0164.body
}

//## Operation: setHour%3AE583180172; C++
//## Semantics:
//	---------------------------------------------------------
//	       setHour()
//	---------------------------------------------------------
void aes_afp_date_time::setHour (unsigned short value)
{
  //## begin aes_afp_date_time::setHour%3AE583180172.body preserve=yes
	hour_ = value;
  //## end aes_afp_date_time::setHour%3AE583180172.body
}

//## Operation: getHour%3AE583260064; C++
//## Semantics:
//	---------------------------------------------------------
//	       getHour()
//	---------------------------------------------------------
unsigned short aes_afp_date_time::getHour () const
{
  //## begin aes_afp_date_time::getHour%3AE583260064.body preserve=yes
	return hour_;
  //## end aes_afp_date_time::getHour%3AE583260064.body
}

//## Operation: setMin%3AE5832F01ED; C++
//## Semantics:
//	---------------------------------------------------------
//	       setMin()
//	---------------------------------------------------------
void aes_afp_date_time::setMin (unsigned short value)
{
  //## begin aes_afp_date_time::setMin%3AE5832F01ED.body preserve=yes
	min_ = value;
  //## end aes_afp_date_time::setMin%3AE5832F01ED.body
}

//## Operation: getMin%3AE5833E03A8; C++
//## Semantics:
//	---------------------------------------------------------
//	       getMin()
//	---------------------------------------------------------
unsigned short aes_afp_date_time::getMin ()const
{
  //## begin aes_afp_date_time::getMin%3AE5833E03A8.body preserve=yes
	return min_;
  //## end aes_afp_date_time::getMin%3AE5833E03A8.body
}

//## Operation: setSec%3AE5834F0121; C++
//## Semantics:
//	---------------------------------------------------------
//	       setSec()
//	---------------------------------------------------------
void aes_afp_date_time::setSec (unsigned short value)
{
  //## begin aes_afp_date_time::setSec%3AE5834F0121.body preserve=yes
	sec_ = value;
  //## end aes_afp_date_time::setSec%3AE5834F0121.body
}

//## Operation: getSec%3AE58355030A; C++
//## Semantics:
//	---------------------------------------------------------
//	       getSec()
//	---------------------------------------------------------
unsigned short aes_afp_date_time::getSec ()const
{
  //## begin aes_afp_date_time::getSec%3AE58355030A.body preserve=yes
	return sec_;
  //## end aes_afp_date_time::getSec%3AE58355030A.body
}

//## Operation: setSeq%3AE58363010C; C++
//## Semantics:
//	---------------------------------------------------------
//	       setSeq()
//	---------------------------------------------------------
void aes_afp_date_time::setSeq (unsigned short value)
{
  //## begin aes_afp_date_time::setSeq%3AE58363010C.body preserve=yes
	seqnr_ = value;
  //## end aes_afp_date_time::setSeq%3AE58363010C.body
}

//## Operation: getSeq%3AE5836F0181; C++
//## Semantics:
//	---------------------------------------------------------
//	       getSeq()
//	---------------------------------------------------------
unsigned short aes_afp_date_time::getSeq ()const
{
  //## begin aes_afp_date_time::getSeq%3AE5836F0181.body preserve=yes
	return seqnr_;
  //## end aes_afp_date_time::getSeq%3AE5836F0181.body
}

//## Operation: getDateString%3E36910400B8
//## Semantics:
//	---------------------------------------------------------
//	       getDateString()
//	---------------------------------------------------------
std::string aes_afp_date_time::getDateString ()
{
  //## begin aes_afp_date_time::getDateString%3E36910400B8.body preserve=yes
	char buf[32];
	
	sprintf(buf, "%04d%02d%02d%02d%02d%02d%d", year_, month_, day_, hour_, min_, sec_, seqnr_);
	return buf;
  //## end aes_afp_date_time::getDateString%3E36910400B8.body
}

//## Operation: setDateString%3E36911F0247; C++
//## Semantics:
//	---------------------------------------------------------
//	       setDateString()
//	---------------------------------------------------------
void aes_afp_date_time::setDateString (std::string& dateString)
{
  //## begin aes_afp_date_time::setDateString%3E36911F0247.body preserve=yes
	std::string tmpStr("");

	tmpStr = dateString.substr(0, 4);
	year_ = ACE_OS::atoi(tmpStr.c_str() );

	tmpStr = dateString.substr(3, 2);
	month_ = ACE_OS::atoi(tmpStr.c_str() );

	tmpStr = dateString.substr(5, 2);
	day_ = ACE_OS::atoi(tmpStr.c_str() );

	tmpStr = dateString.substr(7, 2);
	hour_ = ACE_OS::atoi(tmpStr.c_str() );

	tmpStr = dateString.substr(9, 2);
	min_ = ACE_OS::atoi(tmpStr.c_str() );

	tmpStr = dateString.substr(11, 2);
	sec_ = ACE_OS::atoi(tmpStr.c_str() );

	tmpStr = dateString.substr(13);
	seqnr_ = ACE_OS::atoi(tmpStr.c_str() );
  //## end aes_afp_date_time::setDateString%3E36911F0247.body
}

// Additional Declarations
  //## begin aes_afp_date_time%3AE5786F0056.declarations preserve=yes
  //## end aes_afp_date_time%3AE5786F0056.declarations

//## begin module%3AE57CB40350.epilog preserve=yes
//## end module%3AE57CB40350.epilog


// Detached code regions:
#if 0
//## begin aes_afp_date_time::operator=%3AE5786F0056_assign.body preserve=yes
	year_ = right.year;
  month_ = right.month;
  day_ = right.day;
  hour_ = right.hour;
  min_ = right.min;
  sec_ = right.sec;
	seqnr_ = right.seqnr;
	return *this;
//## end aes_afp_date_time::operator=%3AE5786F0056_assign.body

//## begin aes_afp_date_time::operator<<%3AE5786F0056_outstr.body preserve=yes

	char buf[32];
	
	sprintf(buf, "%04d%02d%02d%02d%02d%02d", right.year, right.month, right.day, right.hour, right.min, right.sec);

	stream << buf;
	return stream;
//## end aes_afp_date_time::operator<<%3AE5786F0056_outstr.body

//## begin aes_afp_date_time::operator>>%3AE5786F0056_instr.body preserve=yes
	return stream;
//## end aes_afp_date_time::operator>>%3AE5786F0056_instr.body

#endif
