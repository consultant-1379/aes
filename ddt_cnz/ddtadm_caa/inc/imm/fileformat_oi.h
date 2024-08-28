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
#ifndef DDTADM_CAA_INC_AES_DDT_IMM_FILEFORMAT_OI_H_
#define DDTADM_CAA_INC_AES_DDT_IMM_FILEFORMAT_OI_H_

#include "imm/base_oi.h"

#include <map>

namespace imm
{
	/**
	 * @class FileFormat_OI
	 *
	 * @brief
	 * Object Implementer of FileFormat MO.
	 *
	 *
	 * @sa Base_OI
	 */
	class FileFormat_OI : public Base_OI
	{
 	public:
		///  Constructor.
		FileFormat_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName);

		///  Destructor.
		virtual ~FileFormat_OI();

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

		/** @brief	Struct of IMM DataSource MoC elements
		 */
		struct fileFormat
		{
			operation::fileFormatInfo info;

			bool completed;
			ImmAction action;

			fileFormat() : info(), completed(false), action(Nothing) {};


			const char* getFileFormatName() const { return info.getName(); };
			const char* getFileFormatDN() const { return info.getFileFormatDN(); };

			const char* getOutputFormatName() const { return info.getOutputFormatName(); };
			const char* getDataSinkName() const { return info.getDataSinkName(); };
			const char* getDataSourceName() const { return info.getDataSourceName(); };
		};

		typedef std::multimap<ACS_APGCC_CcbId, fileFormat> operationTable_t;

		/** @brief	m_operationTable
		 *
		 *  Map of pending Operations
		 *
		 */
		operationTable_t m_operationTable;
	};

} /* namespace ddt_imm */

#endif /* DDTADM_CAA_INC_AES_DDT_IMM_FILEFORMAT_OI_H_ */
