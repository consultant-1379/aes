//	INCLUDE aes_afp_timestamp.h
//
//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	 All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson Utvecklings AB, Sweden.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson Utvecklings AB or
//	in
//	accordance with the terms and conditions stipulated in
//	the
//	agreement/contract under which the program(s) have been
//	supplied.
//
//	DESCRIPTION:
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
#include <stdlib.h>
#include <aes_afp_timestamp.h>
#include <ace/Mutex.h>

// Class aes_afp_timestamp 


std::auto_ptr<aes_afp_timestamp>  aes_afp_timestamp::instance_;
ACE_Mutex aes_afp_timestamp::m_mutex;
aes_afp_timestamp::aes_afp_timestamp():seqCounter_(0)
{
	ACE_OS::memset(timeBuf_, 0, sizeof(timeBuf_));
	savedTime_= 0;

}
aes_afp_timestamp::~aes_afp_timestamp()
{
}

//	---------------------------------------------------------
//	       Instance()
//	---------------------------------------------------------
aes_afp_timestamp& aes_afp_timestamp::Instance ()
{
	if(!instance_.get())
	{
		m_mutex.acquire();
		if(!instance_.get())
		{
			std::auto_ptr<aes_afp_timestamp> temp = std::auto_ptr<aes_afp_timestamp>(new aes_afp_timestamp);
			instance_ = temp;
		}
		m_mutex.release();
	}
	return *instance_;

}

//## Semantics:
//	---------------------------------------------------------
//	       getTimeStamp()
//	---------------------------------------------------------
aes_afp_date_time aes_afp_timestamp::getTimeStamp ()
{
	ACE_Time_Value currentTime(ACE_OS::time() );

  struct tm *newtime;
  long long_time(currentTime.sec());
  newtime = ACE_OS::localtime(&long_time);

  savedTime_.setYear(newtime->tm_year + 1900);
  savedTime_.setMonth(newtime->tm_mon +1);
  savedTime_.setDay(newtime->tm_mday);
  savedTime_.setHour(newtime->tm_hour);
  savedTime_.setMin(newtime->tm_min);
  savedTime_.setSec(newtime->tm_sec);
  
  if (timeValue_ == currentTime)
    {
			seqCounter_++;
			if (seqCounter_ > 999)
				{
					seqCounter_ = 0;
				}
      savedTime_.setSeq((unsigned short)seqCounter_);
    }
  else
    {
      timeValue_ = currentTime;
      seqCounter_ = 0;
      savedTime_.setSeq(0);
    }
  return savedTime_;
}

//## Semantics:
//	---------------------------------------------------------
//	       getAbsTime()
//	---------------------------------------------------------
unsigned long aes_afp_timestamp::getAbsTime ()
{
  time_t ltime;
  ACE_OS::time(&ltime);
  return ltime;
}

//## Semantics:
//	---------------------------------------------------------
//	       getTimeString()
//	---------------------------------------------------------
std::string aes_afp_timestamp::getTimeString ()
{
	aes_afp_date_time dt;

	dt = getTimeStamp();
	sprintf(timeBuf_,
					//"%04d%02d%02d%02d%02d%02d%03d\0",
			        "%04d%02d%02d%02d%02d%02d%03d%s",
					dt.getYear(),
					dt.getMonth(),
					dt.getDay(),
					dt.getHour(),
					dt.getMin(),
					dt.getSec(),
					dt.getSeq(),
					"\0");

	timeString_ = timeBuf_;

	return timeString_;

  //## end aes_afp_timestamp::getTimeString%3BE246E400DF.body
}

//## Operation: getAbsoluteTimeString%3E37732A0278
//## Semantics:
//	---------------------------------------------------------
//	       getAbsoluteTimeString()
//	---------------------------------------------------------
std::string aes_afp_timestamp::getAbsoluteTimeString ()
{
  //## begin aes_afp_timestamp::getAbsoluteTimeString%3E37732A0278.body preserve=yes
  time_t ltime;
  ACE_OS::time(&ltime);
  std::stringstream str;
	//ultoa(ltime, timeBuf_, 10);
  str << ltime;
  strcpy(timeBuf_,str.str().c_str());
  absTimeString_ = timeBuf_;
  return absTimeString_;
  //## end aes_afp_timestamp::getAbsoluteTimeString%3E37732A0278.body
}

//## Operation: getTimeChar%3E51D4DF03E1; C++
//## Semantics:
//	---------------------------------------------------------
//	       getTimeChar()
//	---------------------------------------------------------
char* aes_afp_timestamp::getTimeChar ()
{
  //## begin aes_afp_timestamp::getTimeChar%3E51D4DF03E1.body preserve=yes
	aes_afp_date_time dt;
	dt = getTimeStamp();
	sprintf(timeBuf_,
					"%04d%02d%02d%02d%02d%02d%03d",
					dt.getYear(),
					dt.getMonth(),
					dt.getDay(),
					dt.getHour(),
					dt.getMin(),
					dt.getSec(),
					dt.getSeq() );

	return timeBuf_;
  //## end aes_afp_timestamp::getTimeChar%3E51D4DF03E1.body
}

//## Operation: removeSequenceNumber%3E51D6C1018B; C++
//## Semantics:
//	---------------------------------------------------------
//	       removeSequenceNumber()
//	---------------------------------------------------------
std::string& aes_afp_timestamp::removeSequenceNumber (std::string timeStr)
{
  //## begin aes_afp_timestamp::removeSequenceNumber%3E51D6C1018B.body preserve=yes
	removedSeqNumber_ = timeStr;
	if (removedSeqNumber_.length() > 14)
		{
			removedSeqNumber_.erase(14);
		}

	return removedSeqNumber_;
  //## end aes_afp_timestamp::removeSequenceNumber%3E51D6C1018B.body
}

// Additional Declarations
  //## begin aes_afp_timestamp%3737F44A0268.declarations preserve=yes
  //## end aes_afp_timestamp%3737F44A0268.declarations

//## begin module%3737F5600330.epilog preserve=yes
//## end module%3737F5600330.epilog
