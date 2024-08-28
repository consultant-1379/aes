//******************************************************************************
//
//  NAME
//     aes_dbo_brfc_interface.h
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2008. All rights reserved.
//
//  The Copyright to the computer program(s) herein
//  is the property of Ericsson AB, Sweden.
//  The program(s) may be used and/or copied only with
//  the written permission from Ericsson AB or in
//  accordance with the terms and conditions stipulated in the
//  agreement/contract under which the program(s) have been
//  supplied.
//
//  DESCRIPTION
//     -
//
//  DOCUMENT NO
//          190 89-CAA nnn nnnn
//
//  AUTHOR
//     2012-07-12 by XNADNAR PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************

#ifndef AES_DBO_BRFC_INTERFACE_H
#define AES_DBO_BRFC_INTERFACE_H

/*=====================================================================
                        DIRECTIVE DECLARATION SECTION
==================================================================== */
#include <poll.h>
#include <ace/ACE.h>
#include <ace/Task.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ACS_CC_Types.h>
#include <acs_apgcc_objectimplementerinterface_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <acs_apgcc_paramhandling.h>
#include <acs_bur_BrfAdminOperationId.h>

#define BRFC_DBO_OBJ_DNNAME "brfPersistentDataOwnerId=ERIC-APG-AES-DBO,brfParticipantContainerId=1"
#define BRFC_PARENT_RDN "brfParticipantContainerId=1"
#define BRFC_PERSISTENT_DATA_OWNER_CLNAME "BrfPersistentDataOwner"
#define BRFC_PERSISTENT_DATA_OWNER_RDN_VAL "brfPersistentDataOwnerId=ERIC-APG-AES-DBO"
#define BRFC_PERSISTENT_DATA_OWNER_RDN_NAME "brfPersistentDataOwnerId"
#define BRFC_DBO_IMPL "brfc_dbo_impl"

class AES_DBO_BrfcInterface :
public ACE_Event_Handler,
public acs_apgcc_objectimplementerinterface_V3
{
public :
	/**
	 * @brief
	 * constructor
	 */
	AES_DBO_BrfcInterface();
	/**
	 * @brief
	 * constructor with one parameter
	 */
	AES_DBO_BrfcInterface(string p_impName );
	/**
	 * @brief
	 * constructor with three parameters
	 */
	AES_DBO_BrfcInterface(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );
	/**
	 * @brief
	 * destructor
	 */
	~AES_DBO_BrfcInterface();

	virtual int handle_input(ACE_HANDLE fd);
	/**
	 *  setImpl method: This method is used to set Implementer.
	 *  @param pImpl           : AES_DBO_BrfcInterface pointer
	 */
	ACS_CC_ReturnType setImpl();
	/**
	 *  removeImpl method: This method is used to remove Implementer.
	 *  @param pImpl           : AES_DBO_BrfcInterface pointer
	 */
	ACS_CC_ReturnType removeImpl();
	/**
	 *  setupBrfcInterfaceThread
	 *
	 *
	 *
	 *
	 */
	ACS_CC_ReturnType setupBrfcInterfaceThread(	AES_DBO_BrfcInterface * aDBOBrfcThreadPtr, ACE_Reactor* threadManager_);
	/**
	 *  setupBrfcInterfaceThread
	 *
	 *
	 *
	 *
	 */
	void shutdown();
	/**
	 *  create method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object created.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  className     : const char pointer
	 *  @param  parentname    : const char pointer
	 *  @param  attr          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
	/**
	 *  deleted method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object deleted.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  objName       : const char pointer
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	/**
	 *  modify method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object's attribute modify.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  objName       : const char pointer
	 *  @param  attr          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	/**
	 *  complete method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  abort method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  apply method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  updateRuntime method: This method is inherited from base class and overridden by our class.
	 *  @param  objName          : const char pointer
	 *  @param  attrName         : const char pointer
	 */
	ACS_CC_ReturnType updateRuntime(const char* p_nameObj, const char** p_nameAttr);
	/*the callback*/
	/**
	 *  adminOperationCallback  method: This method is for admin operation callback
	 *  @param  oiHandle      :     ACS_APGCC_OiHandle
	 *  @param  nvocation         :  ACS_APGCC_InvocationType
	 *  @param  p_objName       : const char pointer
	 *  @param  operationId          : ACS_APGCC_AdminOperationIdType
	 *  @param  paramList          : ACS_APGCC_AdminOperationParamType
	 */
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,

			ACS_APGCC_InvocationType invocation,

			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,

			ACS_APGCC_AdminOperationParamType**paramList);

	//void setImpl(MyImplementer_4 *pImpl);
	int ResponseToBrfc(unsigned long long, int, int);
	static bool getBrfStatus();
	/**
	 *  setBrfStatus method: This method is used to set Brf status.
	 *  @param status           : bool
	 */
	static void setBrfStatus(bool status);


private:
	static bool isBrfInProgress_;
	bool m_isObjImplAdded;
	// Access synchronization controllers
	static ACE_Recursive_Thread_Mutex brfcMX_;
	ACE_Reactor *m_poThreadManager;
	acs_apgcc_oihandler_V3 oiHandler;

};

#endif
