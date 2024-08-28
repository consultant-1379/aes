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
#ifndef DDTADM_CAA_INC_AES_DDT_IMM_DATASOURCE_OI_H_
#define DDTADM_CAA_INC_AES_DDT_IMM_DATASOURCE_OI_H_

#ifndef CUTE_TEST
	#include "imm/base_oi.h"
#else
	#include "stubs/imm_stubs.h"
#endif

#include <map>

namespace imm
{
	/**
	 * @class DataSource_OI
	 *
	 * @brief
	 * Object Implementer of DataSource MO.
	 *
	 *
	 * @sa Base_OI
	 */
	class DataSource_OI : public Base_OI
	{
	 public:
		///  Constructor.
		DataSource_OI();

		///  Destructor.
		virtual ~DataSource_OI();

		/**	@brief create method
		 *	This method will be called as a callback when an Object is created as instance of a Class for which the Application
		 *	has registered as Class Object Implementer.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID of the Configuration Change Bundle.
		 *
		 *	@param className: the name of the class.
		 *
		 *	@param parentname: the name of the parent.
		 *
		 *	@param attr: a pointer to a null terminated array of ACS_APGCC_AttrValues.
		 *
		 *	@return ACS_CC_SUCCESS on success, otherwise ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

		/**	@brief deleted method
		 *	This method will be called as a callback when deleting an Object for which the Application has registerd as	Object Implementer.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID of the Configuration Change Bundle.
		 *
		 *	@param objName: the name of the object.
		 *
		 *	@return ACS_CC_SUCCESS on success, otherwise ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

		/**	@brief modify method
		 *	This method will be called as a callback when modifying an Object for which the Application has registered as Object Implementer.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID of the Configuration Change Bundle.
		 *
		 *	@param objName: the name of the object.
		 *
		 *	@param attrMods: a NULL terminated array of pointers to ACS_APGCC_AttrModification elements.
		 *
		 *	@return ACS_CC_SUCCESS on success, otherwise ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

		/**	@brief complete method
		 *	This method will be called as a callback when a Configuration Change Bundle is complete.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID of the Configuration Change Bundle.
		 *
		 *	@return ACS_CC_SUCCESS on success, otherwise ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/**	@brief apply method
		 *	This method will be called as a callback when a Configuration Change Bundle is applied.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID for the Configuration Change Bundle.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		virtual void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

		/**	@brief abort method
		 *	This method will be called as a callback when a Configuration Change Bundle is aborted.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID of the Configuration Change Bundle.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		virtual void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	 private:

		/**
		 * @brief Get the data source name from DN of the MO
		 *
		 * @param objectDN DN of MO
		 * @param dataSourceName
		 *
		 * @return true on success, false otherwise
		 *
		 */
		bool getDataSourceNameFromDN(const std::string& objectDN, std::string& dataSourceName);

		/** @brief	Struct of IMM DataSource MoC elements
		 */
		struct dataSource
		{
			operation::dataSourceInfo info;

			bool completed;
			ImmAction action;

			dataSource() : info(), completed(false), action(Nothing) {};
			const char* getDataSourceName() const { return info.getName(); };
			const char* getDataSourceDN() const { return info.getDataSourceDN(); };

		};

		typedef std::multimap<ACS_APGCC_CcbId, dataSource> operationTable_t;

		/** @brief	m_dataSourceOperationTable
		 *
		 *  Map of pending DataSource Operations
		 *
		 */
		operationTable_t m_dataSourceOperationTable;


	};

} /* namespace ddt_imm */

#endif /* DDTADM_CAA_INC_AES_DDT_IMM_DATASOURCE_OI_H_ */
