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
#ifndef DDTADM_CAA_INC_AES_DDT_IMM_TRANSFERPROGRESS_OI_H_
#define DDTADM_CAA_INC_AES_DDT_IMM_TRANSFERPROGRESS_OI_H_

#ifndef CUTE_TEST
	#include "imm/base_oi.h"
#else
	#include "stubs/operation_stub.h"
#endif

namespace imm
{
	/**
	 * @class TransferProgress_OI
	 *
	 * @brief
	 * Object Implementer of TransferProgress MO.
	 *
	 *
	 * @sa Base_OI
	 */
	class TransferProgress_OI : public Base_OI
	{
	public:
		///  Constructor.
		TransferProgress_OI(const std::string &managedObjectClassName, const std::string &objectImplementerName);

		///  Destructor.
		virtual ~TransferProgress_OI();

		/**	@brief updateRuntime method
		 *
		 *	This method will be called as a callback when modifying a runtime not-cached attribute of a configuration Object
		 *	for which the Application has registered as Object Implementer.
		 *
		 *	@param p_objName: the Distinguished name of the object that has to be modified.
		 *
		 *	@param p_attrName: the name of attribute that has to be modified.
		 *
		 *	@return ACS_CC_SUCCESS On success, otherwise ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		virtual ACS_CC_ReturnType updateRuntime(const char* p_objName, const char** p_attrName);

 	private:
		/** @brief
		 *
		 *	This method update the runtime attribute.
		 *
		 *  @param p_objName: DN of the TransferProgress MO.
		 *
		 *  @param p_attrName: Attribute name of the runtime attribute.
		 *
		 *  @return ACS_CC_SUCCESS on success otherwise ACS_CC_FAILURE.
		 *
		 *	@remarks Remarks
		 */
		ACS_CC_ReturnType modifyRuntimeAttribute(uint64_t& attrvalue, const char* p_objName, const char* p_attrName);
	};

} /* namespace ddt_imm */

#endif /* DDTADM_CAA_INC_AES_DDT_IMM_TRANSFERPROGRESS_OI_H_ */
