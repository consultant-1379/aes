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


#include "engine/templatehandler.h"

#include "common/utility.h"
#include "common/tracer.h"
#include "common/logger.h"

#include <time.h>

namespace engine
{

	namespace
	{
		const std::string DEFAULT_TOKEN("%");

		const std::string INVALID_TAG("Invalid tag : ");
		const std::string INVALID_FORMAT("Invalid format");
		const std::string DUPLICATE_TAGS("Duplicated tags");
		const std::string INCOMPATIBLE_TAGS("Incompatible tags");
	}

	AES_DDT_TRACE_DEFINE(AES_DDT_Engine_TemplateHandler)

	TemplateHandler::TemplateHandler(const std::string& templateFormat, const std::string& token)
	: m_templateFormat(templateFormat),
	  m_token(token),
	  m_tagFactory(),
	  m_tagObjects(),
	  m_lastError()
	{
		AES_DDT_TRACE_MESSAGE("file template:<%s>", m_templateFormat.c_str());
		parse();
	}

	TemplateHandler::TemplateHandler(const std::string& templateFormat)
	: m_templateFormat(templateFormat),
	  m_token(DEFAULT_TOKEN),
	  m_tagFactory(),
	  m_tagObjects(),
	  m_lastError()
	{
		AES_DDT_TRACE_MESSAGE("file template:<%s>", m_templateFormat.c_str());
		parse();
	}

	TemplateHandler::~TemplateHandler()
	{
		AES_DDT_TRACE_FUNCTION;
	}

	bool TemplateHandler::hasTransientTag()
	{
		return m_tagFactory.hasTransientTag(m_tagObjects);
	}

	bool TemplateHandler::getMaxTransientPrintableNumber(uint32_t& maxPrintableNumber)
	{
		return m_tagFactory.getMaxTransientPrintableNumber(m_tagObjects, maxPrintableNumber);
	}

	bool TemplateHandler::hasPersistentTag()
	{
		return m_tagFactory.hasPersistentTag(m_tagObjects);
	}

	bool TemplateHandler::getMaxPersistentPrintableNumber(uint32_t& maxPrintableNumber)
	{
		return m_tagFactory.getMaxPersistentPrintableNumber(m_tagObjects, maxPrintableNumber);
	}

	bool TemplateHandler::hasCpNameTag()
	{
		return m_tagFactory.hasCpNameTag(m_tagObjects);
	}

	bool TemplateHandler::isValid()
	{
		bool templateValid = false;

		if( !m_templateFormat.empty() && parse() )
		{
			templateValid = m_tagFactory.validateAll(m_lastError);
		}

		return templateValid;
	}

	bool TemplateHandler::isStartupSequenceNumberCompatible(const uint32_t& seqNumber)
	{
		bool seqNumberValid = false;
		uint32_t maxPrintableNumber = 0U;

		if(!m_tagObjects.empty())
		{
			if( hasPersistentTag() || hasTransientTag() )
			{
				if( (hasPersistentTag() && getMaxPersistentPrintableNumber(maxPrintableNumber)) ||
					(hasTransientTag() && getMaxTransientPrintableNumber(maxPrintableNumber)))
				{
					if(seqNumber < maxPrintableNumber)
					{
						seqNumberValid = true;
					}
				}
			}
			else
			{
				// 'true' would allow startupsequencenumber settings
				// in situations where any seq nr is set in the template
				seqNumberValid = false;
			}
		}

		return seqNumberValid;
	}

	std::string TemplateHandler::getName(const std::string& cpName, const int sequenceNumber) const
	{
		std::string name;
		time_t rawtime;

		time( &rawtime );
		struct tm* timeinfo = localtime( &rawtime );

		std::list<boost::shared_ptr<BaseTag> >::const_iterator element = m_tagObjects.begin();

		while(m_tagObjects.end() != element)
		{
			name.append((*element)->getFormat(cpName, sequenceNumber, timeinfo));
			++element;
		}

		AES_DDT_TRACE_MESSAGE("file name:<%s>", name.c_str());

		return name;
	}

	bool TemplateHandler::parse()
	{
		bool valid = true;
		m_tagObjects.clear();
		m_tagFactory.reset();

		size_t firstTagPosition = 0U;
		size_t secondTagPosition = 0U;
		size_t constantTextPosition = 0U;

		do
		{
			constantTextPosition = firstTagPosition;
			firstTagPosition = m_templateFormat.find(m_token, firstTagPosition);

			if( std::string::npos != firstTagPosition )
			{
				std::string constantText = m_templateFormat.substr(constantTextPosition, firstTagPosition - constantTextPosition );

				if(!constantText.empty() )
				{
					m_tagObjects.push_back( m_tagFactory.makeTag(constantText) );
				}

				++firstTagPosition;
				secondTagPosition = m_templateFormat.find(m_token, firstTagPosition);

				if( std::string::npos != secondTagPosition )
				{
					std::string tag = m_templateFormat.substr(firstTagPosition, secondTagPosition - firstTagPosition);

					if(!m_tagFactory.validate(tag))
					{
						m_lastError.assign(INVALID_TAG);
						m_lastError.append(tag);

						AES_DDT_TRACE_MESSAGE("INVALID TAG:<%s> into template:<%s>", tag.c_str(), m_templateFormat.c_str());
						AES_DDT_LOG(LOG_LEVEL_ERROR, "INVALID TAG:<%s> into template:<%s>", tag.c_str(), m_templateFormat.c_str());
						valid = false;
						break;
					}

					m_tagObjects.push_back( m_tagFactory.makeTag(tag) );

					firstTagPosition = secondTagPosition + 1;
				}
				else
				{
					m_lastError.assign(INVALID_FORMAT);
					AES_DDT_TRACE_MESSAGE("INVALID template:<%s> format", m_templateFormat.c_str());
					AES_DDT_LOG(LOG_LEVEL_ERROR, "INVALID template:<%s> format", m_templateFormat.c_str());

					valid = false;
					break;
				}
			}
			else
			{
				std::string constantText = m_templateFormat.substr(constantTextPosition,  m_templateFormat.size() - constantTextPosition );

				if(!constantText.empty() )
				{
					m_tagObjects.push_back( m_tagFactory.makeTag(constantText) );
				}
			}

		}while(std::string::npos != firstTagPosition);

		AES_DDT_TRACE_MESSAGE("template parsing result:<%s>", common::utility::boolToString(valid) );

		return valid;
	}

	TemplateHandler::TagFactory::TagFactory()
	{
		initializeValidTagInfo();
	}

	bool TemplateHandler::TagFactory::validate(const std::string& tag)
	{
		return (m_validTags.end() != m_validTags.find(tag));
	}

	bool TemplateHandler::TagFactory::validateAll(std::string& errorMsg)
	{
		bool valid = checkNoTwinTags(errorMsg) && checkTagsCompatibility(errorMsg);
		return valid;
	}

	bool TemplateHandler::TagFactory::hasTransientTag(const std::list<boost::shared_ptr<BaseTag> >& tags)
	{
		bool result = false;
		std::list<boost::shared_ptr<BaseTag> >::const_iterator element;

		for(element = tags.begin(); tags.end() != element; ++element)
		{
			BaseTag::TagCode tagCode = (*element)->tagCode();
			if( (BaseTag::TAG_n == tagCode) || (BaseTag::TAG_4n == tagCode) || (BaseTag::TAG_6n == tagCode) || (BaseTag::TAG_8n == tagCode) )
			{
				result = true;
				break;
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	bool TemplateHandler::TagFactory::getMaxTransientPrintableNumber(const std::list<boost::shared_ptr<BaseTag> >& tags, uint32_t& maxPrintableNumber)
	{
		bool result = false;
		std::list<boost::shared_ptr<BaseTag> >::const_iterator element;

		for(element = tags.begin(); tags.end() != element; ++element)
		{
			BaseTag::TagCode tagCode = (*element)->tagCode();
			if( (BaseTag::TAG_n == tagCode) || (BaseTag::TAG_4n == tagCode) || (BaseTag::TAG_6n == tagCode) || (BaseTag::TAG_8n == tagCode) )
			{
				maxPrintableNumber = (*element)->getMaxPrintableNumber();
				result = true;
				break;
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	bool TemplateHandler::TagFactory::hasPersistentTag(const std::list<boost::shared_ptr<BaseTag> >& tags)
	{
		bool result = false;
		std::list<boost::shared_ptr<BaseTag> >::const_iterator element;

		for(element = tags.begin(); tags.end() != element; ++element)
		{
			BaseTag::TagCode tagCode = (*element)->tagCode();
			if( (BaseTag::TAG_p == tagCode) || (BaseTag::TAG_4p == tagCode) || (BaseTag::TAG_6p == tagCode) || (BaseTag::TAG_8p == tagCode) )
			{
				result = true;
				break;
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	bool TemplateHandler::TagFactory::getMaxPersistentPrintableNumber(const std::list<boost::shared_ptr<BaseTag> >& tags, uint32_t& maxPrintableNumber)
	{
		bool result = false;
		std::list<boost::shared_ptr<BaseTag> >::const_iterator element;

		for(element = tags.begin(); tags.end() != element; ++element)
		{
			BaseTag::TagCode tagCode = (*element)->tagCode();
			if( (BaseTag::TAG_p == tagCode) || (BaseTag::TAG_4p == tagCode) || (BaseTag::TAG_6p == tagCode) || (BaseTag::TAG_8p == tagCode) )
			{
				maxPrintableNumber = (*element)->getMaxPrintableNumber();
				result = true;
				break;
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}


	bool TemplateHandler::TagFactory::hasCpNameTag(const std::list<boost::shared_ptr<BaseTag> >& tags)
	{
		bool result = false;
		std::list<boost::shared_ptr<BaseTag> >::const_iterator element;

		for(element = tags.begin(); tags.end() != element; ++element)
		{
			BaseTag::TagCode tagCode = (*element)->tagCode();
			if( BaseTag::TAG_CpName == tagCode )
			{
				result = true;
				break;
			}
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(result) );
		return result;
	}

	boost::shared_ptr<BaseTag> TemplateHandler::TagFactory::makeTag(const std::string& tag)
	{
		boost::shared_ptr<BaseTag> tagObj;

		std::map<std::string, BaseTag::TagCode>::const_iterator tagIterator = m_validTags.find(tag);

		BaseTag::TagCode tagCode = ( m_validTags.end() != tagIterator) ? tagIterator->second : BaseTag::TAG_FIXTEXT;

		AES_DDT_TRACE_MESSAGE("create tag object:<%s , %d>", tag.c_str(), static_cast<int>(tagCode) );
		AES_DDT_LOG(LOG_LEVEL_INFO, "create tag object:<%s , %d>", tag.c_str(), static_cast<int>(tagCode) );

		switch(tagCode)
		{
			case BaseTag::TAG_YYYY :
				tagObj =  boost::make_shared<FourDigitYearTag>();
				break;

			case BaseTag::TAG_YY :
				tagObj =  boost::make_shared<TwoDigitYearTag>();
				break;

			case BaseTag::TAG_MM :
				tagObj =  boost::make_shared<MonthTag>();
				break;

			case BaseTag::TAG_DD :
				tagObj =  boost::make_shared<DayTag>();
				break;

			case BaseTag::TAG_hh :
				tagObj =  boost::make_shared<HourTag>();
				break;

			case BaseTag::TAG_mm :
				tagObj =  boost::make_shared<MinuteTag>();
				break;

			case BaseTag::TAG_ss :
				tagObj =  boost::make_shared<SecondTag>();
				break;

			case BaseTag::TAG_p :
				tagObj =  boost::make_shared<Persistent4DigitMaxTag>();
				break;

			case BaseTag::TAG_4p :
				tagObj =  boost::make_shared<Persistent4DigitTag>();
				break;

			case BaseTag::TAG_6p :
				tagObj =  boost::make_shared<Persistent6DigitTag>();
				break;

			case BaseTag::TAG_8p :
				tagObj =  boost::make_shared<Persistent8DigitTag>();
				break;

			case BaseTag::TAG_n :
				tagObj =  boost::make_shared<Transient4DigitMaxTag>();
				break;

			case BaseTag::TAG_4n :
				tagObj =  boost::make_shared<Transient4DigitTag>();
				break;

			case BaseTag::TAG_6n :
				tagObj =  boost::make_shared<Transient6DigitTag>();
				break;

			case BaseTag::TAG_8n :
				tagObj =  boost::make_shared<Transient8DigitTag>();
				break;

			case BaseTag::TAG_CpName :
				tagObj =  boost::make_shared<CpNameTag>();
				break;

			case BaseTag::TAG_FIXTEXT :
				tagObj =  boost::make_shared<FixedTextTag>(tag);
				break;
			default :
				tagObj =  boost::make_shared<UnknowTag>();
		};

		m_foundTags.insert( std::make_pair(static_cast<BaseTag::TagCode>(tagCode), true) );

		return tagObj;
	}

	void TemplateHandler::TagFactory::reset()
	{
		m_foundTags.clear();
	}

	void TemplateHandler::TagFactory::initializeValidTagInfo()
	{
		m_validTags.insert( std::make_pair( TAG::YYYY, BaseTag::TAG_YYYY ) );
		m_validTags.insert( std::make_pair( TAG::YY, BaseTag::TAG_YY ) );
		m_validTags.insert( std::make_pair( TAG::MM, BaseTag::TAG_MM ) );
		m_validTags.insert( std::make_pair( TAG::DD, BaseTag::TAG_DD ) );
		m_validTags.insert( std::make_pair( TAG::hh, BaseTag::TAG_hh ) );
		m_validTags.insert( std::make_pair( TAG::mm, BaseTag::TAG_mm ) );
		m_validTags.insert( std::make_pair( TAG::ss, BaseTag::TAG_ss ) );
		m_validTags.insert( std::make_pair( TAG::p,  BaseTag::TAG_p ) );
		m_validTags.insert( std::make_pair( TAG::Four_p, BaseTag::TAG_4p ) );
		m_validTags.insert( std::make_pair( TAG::Six_p, BaseTag::TAG_6p ) );
		m_validTags.insert( std::make_pair( TAG::Eigth_p, BaseTag::TAG_8p ) );
		m_validTags.insert( std::make_pair( TAG::n, BaseTag::TAG_n ) );
		m_validTags.insert( std::make_pair( TAG::Four_n, BaseTag::TAG_4n ) );
		m_validTags.insert( std::make_pair( TAG::Six_n, BaseTag::TAG_6n ) );
		m_validTags.insert( std::make_pair( TAG::Eigth_n, BaseTag::TAG_8n ) );
		m_validTags.insert( std::make_pair( TAG::CpName, BaseTag::TAG_CpName )) ;
	}

	bool TemplateHandler::TagFactory::checkNoTwinTags(std::string& errorMsg)
	{
		bool valid = true;

		// check if a tag is present twice
		for( int index = BaseTag::FIRST_TAG; (index <= BaseTag::LAST_TAG) && valid; ++index )
			valid &= m_foundTags.count(static_cast<BaseTag::TagCode>(index)) < 2;

		// check if there are both tags "YYYY" and "YY"
		valid = valid && !( (m_foundTags.end() != m_foundTags.find(BaseTag::TAG_YYYY) ) &&
				(m_foundTags.end() != m_foundTags.find(BaseTag::TAG_YY) ) );

		if(!valid)
		{
			errorMsg.assign(DUPLICATE_TAGS);
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(valid) );

		return valid;
	}

	bool TemplateHandler::TagFactory::checkTagsCompatibility(std::string& errorMsg)
	{
		uint8_t numberOfTag_P_or_N_Found = 0U;

		// check if there are both tags "p" and "n"
		for( int index = BaseTag::TAG_p; (index <= BaseTag::TAG_8n); ++index )
		{
			if( m_foundTags.end() != m_foundTags.find( static_cast<BaseTag::TagCode>(index) ) )
			{
				++numberOfTag_P_or_N_Found;
			}
		}
		bool valid = ( 1U >= numberOfTag_P_or_N_Found );

		if(!valid)
		{
			errorMsg.assign(INCOMPATIBLE_TAGS);
		}

		AES_DDT_TRACE_MESSAGE("result:<%s>", common::utility::boolToString(valid) );

		return valid;
	}
}
