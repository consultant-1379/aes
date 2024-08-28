//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************

#ifndef TEST_CAA_TAGS_H_
#define TEST_CAA_TAGS_H_

#include <iostream>
#include <string>
#include <stdint.h>
#include <stdio.h>

#include <time.h>

namespace engine
{
	namespace
	{
		const uint8_t MAX_BUFFER_SIZE = 64U;
	}

	namespace TAG
	{
		const std::string YYYY("YYYY");
		const std::string YY("YY");
		const std::string MM("MM");
		const std::string DD("DD");
		const std::string hh("hh");
		const std::string mm("mm");
		const std::string ss("ss");
		const std::string p("p");
		const std::string Four_p("pppp");
		const std::string Six_p("pppppp");
		const std::string Eigth_p("pppppppp");
		const std::string n("n");
		const std::string Four_n("nnnn");
		const std::string Six_n("nnnnnn");
		const std::string Eigth_n("nnnnnnnn");
		const std::string CpName("CpName");
	}

	namespace MAX_PRINTABLE
	{
		const uint32_t NOT_APPLICABLE = 0U;
		const uint32_t NUMBER_2_DIGITS = 99U;
		const uint32_t NUMBER_4_DIGITS = 9999U;
		const uint32_t NUMBER_6_DIGITS = 999999U;
		const uint32_t NUMBER_8_DIGITS = 99999999U;
	}

	/// BaseTag
	class BaseTag
	{
		public:

			enum TagCode
			{
				TAG_UNKNOW = 0,
				TAG_FIXTEXT,
				FIRST_TAG,
				TAG_YYYY = FIRST_TAG,	// This element will give year in four digits
				TAG_YY,			// This element will give year in two digits
				TAG_MM,			// This element will give month (01 through 12)
				TAG_DD,			// This element will give day of month (01 through 31)

				TAG_hh,			// This element will give hour (00 through 23)
				TAG_mm,			// This element will give minute (00 through 59)
				TAG_ss,			// This element will give second (00 through 59)

				TAG_p, 			// This element will give persistent sequence number with flexible length (range 0 - 9999)
				TAG_4p,			// This element will give persistent sequence number with fixed length (range 0000 - 9999)
				TAG_6p,			// This element will give persistent sequence number with fixed length (000000 through 999999)
				TAG_8p,			// This element will give persistent sequence number with fixed length (00000000 through 99999999)

				TAG_n,			// This element will give transient sequence number with flexible length (0 through 9999)
				TAG_4n,			// This element will give transient sequence number with fixed length (0000 through 9999)
				TAG_6n,			// This element will give transient sequence number with fixed length (000000 through 999999)
				TAG_8n,			// This element will give transient sequence number with fixed length (00000000 through 99999999)

				TAG_CpName,		// This element is the default name of the CP source of data

				LAST_TAG = TAG_CpName
			};

			/// Constructor.
			BaseTag(const std::string& tag, const TagCode& tagCode)
			: m_tag(tag),
			  m_tagCode(tagCode)
			{

			};

			/// Constructor.
			BaseTag(const TagCode& tagCode)
			: m_tag(),
			  m_tagCode(tagCode)
			{

			};

			/**	@brief
			 *
			 *	This method gets the tag code.
			 *
			 *	@return tag code.
			 *
			 *	@remarks Remarks
			 */
			TagCode tagCode() const { return m_tagCode; };

			/// Destructor.
			virtual ~BaseTag(){ };

			/// pure virtual function
			virtual std::string getFormat(const std::string& cpName, const int& sequenceNumber, struct tm* timeinfo) const = 0;

			/**	@brief
			 *
			 *	This method gets the maximum printable number.
			 *
			 *	@return max. printable number.
			 *
			 *	@remarks Remarks
			 */
			virtual uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NOT_APPLICABLE; };

		protected:

			/// Tag
			std::string m_tag;

			/// Tag Code.
			TagCode m_tagCode;

	};

	class UnknowTag : public BaseTag
	{
		public:

			/// Constructor.
			UnknowTag(): BaseTag(TAG_UNKNOW) {};

			/// Destructor.
			virtual ~UnknowTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* /*timeinfo*/) const
			{
				return m_tag;
			}
	};


	class FixedTextTag : public BaseTag
	{
		public:

			/// Constructor.
			FixedTextTag(const std::string& tag): BaseTag(tag, TAG_FIXTEXT) {};

			/// Destructor.
			virtual ~FixedTextTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* /*timeinfo*/) const
			{
				return m_tag;
			}

	};

	class CpNameTag : public BaseTag
	{
		public:

			/// Constructor.
			CpNameTag(): BaseTag(TAG::CpName, TAG_CpName) {};

			/// Destructor.
			virtual ~CpNameTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& cpName, const int& /*sequenceNumber*/, struct tm* /*timeinfo*/) const
			{
				return cpName;
			}

	};

	class Persistent4DigitMaxTag : public BaseTag
	{
		public:

			/// Constructor.
			Persistent4DigitMaxTag(): BaseTag(TAG::p, TAG_p) {};

			/// Destructor.
			virtual ~Persistent4DigitMaxTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_4_DIGITS; };
	};

	class Persistent4DigitTag : public BaseTag
	{
		public:
			/// Constructor.
			Persistent4DigitTag(): BaseTag(TAG::Four_p, TAG_4p) {};

			/// Destructor.
			virtual ~Persistent4DigitTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%04d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_4_DIGITS; };
	};

	class Persistent6DigitTag : public BaseTag
	{
		public:
			/// Constructor.
			Persistent6DigitTag(): BaseTag(TAG::Six_p, TAG_6p) {};

			/// Destructor.
			virtual ~Persistent6DigitTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%06d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_6_DIGITS; };
	};

	class Persistent8DigitTag : public BaseTag
	{
		public:
			/// Constructor.
			Persistent8DigitTag(): BaseTag(TAG::Eigth_p, TAG_8p) {};

			/// Destructor.
			virtual ~Persistent8DigitTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%08d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_8_DIGITS; };
	};

	class Transient4DigitMaxTag : public BaseTag
	{
		public:
			/// Constructor.
			Transient4DigitMaxTag(): BaseTag(TAG::n, TAG_n) {};

			/// Destructor.
			virtual ~Transient4DigitMaxTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_4_DIGITS; };
	};

	class Transient4DigitTag : public BaseTag
	{
		public:

			/// Constructor.
			Transient4DigitTag(): BaseTag(TAG::Four_n, TAG_4n) {};

			/// Destructor.
			virtual ~Transient4DigitTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%04d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_4_DIGITS; };
	};

	class Transient6DigitTag : public BaseTag
	{
		public:
			/// Constructor.
			Transient6DigitTag(): BaseTag(TAG::Six_n, TAG_6n) {};

			/// Destructor.
			virtual ~Transient6DigitTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%06d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_6_DIGITS; };
	};

	class Transient8DigitTag : public BaseTag
	{
		public:
			/// Constructor.
			Transient8DigitTag(): BaseTag(TAG::Eigth_n, TAG_8n) {};

			/// Destructor.
			virtual ~Transient8DigitTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& sequenceNumber, struct tm* /*timeinfo*/) const
			{
				char numberBuffer[MAX_BUFFER_SIZE] = {0};
				snprintf(numberBuffer, MAX_BUFFER_SIZE, "%08d", sequenceNumber);
				return std::string(numberBuffer);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_8_DIGITS; };
	};

	class FourDigitYearTag : public BaseTag
	{
		public:
			/// Constructor.
			FourDigitYearTag(): BaseTag(TAG::YYYY, TAG_YYYY) {};

			/// Destructor.
			virtual ~FourDigitYearTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* timeinfo) const
			{
				char year[MAX_BUFFER_SIZE] = {0};
				snprintf(year, MAX_BUFFER_SIZE, "%d", (timeinfo->tm_year + 1900));
				return std::string(year);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_4_DIGITS; };
	};

	class TwoDigitYearTag : public BaseTag
	{
		public:
			/// Constructor.
			TwoDigitYearTag(): BaseTag(TAG::YY, TAG_YY) { };

			/// Destructor.
			virtual ~TwoDigitYearTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* timeinfo) const
			{
				char year[MAX_BUFFER_SIZE] = {0};
				snprintf(year, MAX_BUFFER_SIZE, "%d", (timeinfo->tm_year + 1900));
				return std::string(year + 2);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_2_DIGITS; };
	};

	class MonthTag : public BaseTag
	{
		public:
			/// Constructor.
			MonthTag(): BaseTag(TAG::MM, TAG_MM) {};

			/// Destructor.
			virtual ~MonthTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* timeinfo) const
			{
				char month[MAX_BUFFER_SIZE] = {0};
				//Left-pads the number with zeroes, width (Minimum number of characters to be printed) is 2
				snprintf(month, MAX_BUFFER_SIZE, "%02d", (timeinfo->tm_mon+1));
				return std::string(month);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_2_DIGITS; };
	};

	class DayTag : public BaseTag
	{
		public:

			/// Constructor.
			DayTag(): BaseTag(TAG::DD, TAG_DD) {};

			/// Destructor.
			virtual ~DayTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* timeinfo) const
			{
				char day[MAX_BUFFER_SIZE] = {0};
				snprintf(day, MAX_BUFFER_SIZE, "%02d", (timeinfo->tm_mday));
				return std::string(day);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_2_DIGITS; };
	};

	class HourTag : public BaseTag
	{
		public:

			/// Constructor.
			HourTag(): BaseTag(TAG::hh, TAG_hh) {};

			/// Destructor.
			virtual ~HourTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* timeinfo) const
			{
				char hour[MAX_BUFFER_SIZE] = {0};
				snprintf(hour, MAX_BUFFER_SIZE, "%02d", (timeinfo->tm_hour));
				return std::string(hour);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_2_DIGITS; };
	};

	class MinuteTag : public BaseTag
	{
		public:

			/// Constructor.
			MinuteTag(): BaseTag(TAG::mm, TAG_mm) {};

			/// Destructor.
			virtual ~MinuteTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* timeinfo) const
			{
				char min[MAX_BUFFER_SIZE] = {0};
				snprintf(min, MAX_BUFFER_SIZE, "%02d", (timeinfo->tm_min));
				return std::string(min);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_2_DIGITS; };
	};

	class SecondTag : public BaseTag
	{
		public:

			/// Constructor.
			SecondTag(): BaseTag(TAG::ss, TAG_ss) {};

			/// Destructor.
			virtual ~SecondTag() {};

			/**	@brief
			 *
			 *	This method gets format.
			 *
			 *	@return tag.
			 *
			 *	@remarks Remarks
			 */
			virtual std::string getFormat(const std::string& /*cpName*/, const int& /*sequenceNumber*/, struct tm* timeinfo) const
			{
				char sec[MAX_BUFFER_SIZE] = {0};
				snprintf(sec, MAX_BUFFER_SIZE, "%02d", (timeinfo->tm_sec));
				return std::string(sec);
			}

			/**	@brief
			 *
			 *	This method gets max printable number.
			 *
			 *	@return max printable number.
			 *
			 *	@remarks Remarks
			 */
			uint32_t getMaxPrintableNumber() const { return MAX_PRINTABLE::NUMBER_2_DIGITS; };
	};
}
#endif /* TEST_CAA_TAGS_H_ */
