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
#ifndef DDTADM_CAA_INC_AES_DDT_IMM_DATASINK_OI_H_
#define DDTADM_CAA_INC_AES_DDT_IMM_DATASINK_OI_H_

#ifndef CUTE_TEST
	#include "imm/base_oi.h"
#else
	#include "stubs/imm_stubs.h"
#endif

#include <map>

namespace imm
{
	/**
	 * @class DataSink_OI
	 *
	 * @brief
	 * Object Implementer of DataSink MO.
	 *
	 *
	 * @sa Base_OI
	 */
	class DataSink_OI : public Base_OI
	{
 	 public:

		///  Constructor.
		DataSink_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName);

		///  Destructor.
		virtual ~DataSink_OI();

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

		/**	@brief adminOperationCallback method
		 *	This method will be called as a callback to manage an administrative operation invoked on the implemented object.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID of the Configuration Change Bundle.
		 *
		 *	@param invocation: input parameter,the invocation id used to match the invocation of the callback with the invocation of result function
		 *
		 *	@param  p_objName: name of the object
		 *
		 *	@param  operationId: administrative operation identifier
		 *
		 *	@param paramList: a null terminated array of pointers to operation params elements.
		 *
		 *	@return none.
		 *
		 *	@remarks Remarks
		 */
		virtual void adminOperationCallback(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType** paramList);

 	 private:

		/**	@brief
		 *
		 *	This method handles progressReport MO.
		 *
		 *	@param oiHandle: the object implementer handle.
		 *
		 *	@param ccbId: the ID of the Configuration Change Bundle.
		 *
		 *	@param parentDN: name of the parent
		 *
		 *	@return true on success, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool progressReportMOHandling(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char* parentDN);

		/**	@brief
		 *
		 *	This method creates progressReport MO.
		 *
		 *	@param ccbHandle: the ID of the Configuration Change Bundle.
		 *
		 *	@param adminOwnerHandler: admin owner handle
		 *
		 *	@param parentDN: name of the parent
		 *
		 *	@return true on success, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool createProgressReportMO(ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* parentDN);

		/**	@brief
		 *
		 *	This method updates data sink progress attribute.
		 *
		 *	@param ccbHandle: the ID of the Configuration Change Bundle.
		 *
		 *	@param adminOwnerHandler: admin owner handle
		 *
		 *	@param dataSinkDN: DN of data sink
		 *
		 *	@return true on success, otherwise false.
		 *
		 *	@remarks Remarks
		 */
		bool updateDataSinkProgressAttribute(ACS_APGCC_CcbHandle ccbHandle, ACS_APGCC_AdminOwnerHandle adminOwnerHandler, const char* dataSinkDN);

		/** @brief	Struct of IMM DataSource MoC elements
		 */
		struct dataSink
		{
			operation::dataSinkInfo info;

			bool completed;
			ImmAction action;

			dataSink() : info(), completed(false), action(Nothing) {};
			const char* getDataSinkName() const { return info.getName(); };
			const char* getDataSourceName() const { return info.getDataSourceName(); };
			const char* getDataSinkDN() const { return info.getDataSinkDN(); };

		};

		typedef std::multimap<ACS_APGCC_CcbId, dataSink> operationTable_t;

		/** @brief	m_dataSinkOperationTable
		 *
		 *  Map of pending DataSink Operations
		 *
		 */
		operationTable_t m_dataSinkOperationTable;

	};

} /* namespace imm */

#endif /* DDTADM_CAA_INC_AES_DDT_IMM_DATASINK_OI_H_ */
