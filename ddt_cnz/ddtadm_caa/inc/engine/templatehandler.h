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

#ifndef TEST_CAA_SRC_TEMPLATEHANDLER_H_
#define TEST_CAA_SRC_TEMPLATEHANDLER_H_

#include "engine/tags.h"

#include <map>
#include <list>
#include <string>
#include <stdint.h>

#include <boost/make_shared.hpp>

namespace engine
{
	/**
	 * @class TemplateHandler
	 *
	 * @brief This class manages templates related activities.
	 *
	 */
	class TemplateHandler
	{
		public:

			// Constructor.
			TemplateHandler(const std::string& templateFormat, const std::string& token);

			// Constructor.
			TemplateHandler(const std::string& templateFormat);

			// Destructor.
			virtual ~TemplateHandler();

			/**	@brief
			 *
			 *	This method checks transient tag exists or not.
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool hasTransientTag();

			/**	@brief
			 *
			 *	This method gets the maximum transient printable number.
			 *
			 *	@param maxPrintableNumber : max printable number
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool getMaxTransientPrintableNumber(uint32_t& maxPrintableNumber);

			/**	@brief
			 *
			 *	This method checks persistent tag exists or not.
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool hasPersistentTag();

			/**	@brief
			 *
			 *	This method gets the maximum persistent printable number.
			 *
			 *	@param maxPrintableNumber : max printable number
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool getMaxPersistentPrintableNumber(uint32_t& maxPrintableNumber);

			/**	@brief
			 *
			 *	This method checks cp name tag exists or not.
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool hasCpNameTag();

			/**	@brief
			 *
			 *	This method checks template is valid or not.
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool isValid();

			/**	@brief
			 *
			 *	This method checks startup sequence number is compatible or not.
			 *
			 *	@param seqNumber : sequence number
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool isStartupSequenceNumberCompatible(const uint32_t& seqNumber);

			/**	@brief
			 *
			 *	This method gets the file name.
			 *
			 *	@param cpName : name of the cp
			 *
			 *	@param sequenceNumber : sequence number
			 *
			 *	@return name of the file.
			 *
			 *	@remarks Remarks
			 */
			std::string getName(const std::string& cpName, const int sequenceNumber = 0) const;

			/**	@brief
			 *
			 *	This method gets last error.
			 *
			 *	@return last error.
			 *
			 *	@remarks Remarks
			 */
			std::string getLastError() const {return m_lastError; };

		private:

			/**	@brief
			 *
			 *	This method does parsing of the template.
			 *
			 *	@return true on success, otherwise false.
			 *
			 *	@remarks Remarks
			 */
			bool parse();

			class TagFactory
			{
				public:

					TagFactory();

					virtual ~TagFactory() {};

					bool validate(const std::string& tag);

					bool validateAll(std::string& errorMsg);

					bool hasTransientTag(const std::list<boost::shared_ptr<BaseTag> >& tags);

					bool getMaxTransientPrintableNumber(const std::list<boost::shared_ptr<BaseTag> >& tags, uint32_t& maxPrintableNumber);

					bool hasPersistentTag(const std::list<boost::shared_ptr<BaseTag> >& tags);

					bool getMaxPersistentPrintableNumber(const std::list<boost::shared_ptr<BaseTag> >& tags, uint32_t& maxPrintableNumber);

					bool hasCpNameTag(const std::list<boost::shared_ptr<BaseTag> >& tags);

					boost::shared_ptr<BaseTag> makeTag(const std::string& tag);

					void reset();

				private:

					void initializeValidTagInfo();

					bool checkNoTwinTags(std::string& errorMsg);

					bool checkTagsCompatibility(std::string& errorMsg);

					std::map<std::string, BaseTag::TagCode > m_validTags;
					std::multimap<BaseTag::TagCode, bool > m_foundTags;
			};

			/// template format
			std::string m_templateFormat;

			/// token
			std::string m_token;

			/// tag factory
			TagFactory m_tagFactory;

			/// list of tag objects
			std::list<boost::shared_ptr<BaseTag> > m_tagObjects;

			/// last error
			std::string m_lastError;
	};
}

#endif /* TEST_CAA_SRC_TEMPLATEHANDLER_H_ */
