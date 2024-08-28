//******************************************************************************
//
//  NAME
//     aes_cdh_brfc_interface.h
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

#ifndef AES_CDH_BRFC_INTERFACE_H
#define AES_CDH_BRFC_INTERFACE_H

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
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <acs_apgcc_paramhandling.h>
#include <acs_bur_BrfAdminOperationId.h>

#define BRFC_CDH_OBJ_DNNAME "brfPersistentDataOwnerId=ERIC-APG-AES-CDH,brfParticipantContainerId=1"
#define BRFC_PARENT_RDN "brfParticipantContainerId=1"
#define BRFC_PERSISTENT_DATA_OWNER_CLNAME "BrfPersistentDataOwner"
#define BRFC_PERSISTENT_DATA_OWNER_RDN_VAL "brfPersistentDataOwnerId=ERIC-APG-AES-CDH"
#define BRFC_PERSISTENT_DATA_OWNER_RDN_NAME "brfPersistentDataOwnerId"
#define BRFC_CDH_IMPL "brfc_cdh_impl"

class AES_CDH_BrfImplementer : public acs_apgcc_objectimplementerinterface_V3
{
public :
	AES_CDH_BrfImplementer();

	AES_CDH_BrfImplementer(string p_impName );

	AES_CDH_BrfImplementer(string p_objName, string p_impName, ACS_APGCC_ScopeT p_scope );

	~AES_CDH_BrfImplementer();

	ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);

	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);

	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);

	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);

	ACS_CC_ReturnType updateRuntime(const char* p_nameObj, const char** p_nameAttr);

	/*the callback*/
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,

			ACS_APGCC_InvocationType invocation,

			const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,

			ACS_APGCC_AdminOperationParamType**paramList);

	//void setImpl(MyImplementer_4 *pImpl);

	int ResponseToBrfc(unsigned long long, int, int);
	static bool getBrfStatus();
	static void setBrfStatus(bool status);


private:
//	AES_CDH_BrfImplementer* theCDHBrfcRuntimeOwnerPtr;
	static bool isBrfInProgress_;
	static ACE_Recursive_Thread_Mutex brfcMX_;
};

class AES_CDH_BrfcThread : public ACE_Task_Base
{
public :
	AES_CDH_BrfcThread(ACE_Thread_Manager * thr_mgr);

	AES_CDH_BrfcThread(ACE_Thread_Manager * thr_mgr,AES_CDH_BrfImplementer *pImpl);

	~AES_CDH_BrfcThread();

	void setImpl(AES_CDH_BrfImplementer *pImpl);
	void removeImpl(AES_CDH_BrfImplementer * pImpl);


	bool stop();

	int svc(void);
	int m_StopEvent;

private:
	acs_apgcc_oihandler_V3 oiHandler;

	AES_CDH_BrfImplementer *theBrfImplementer;
	bool m_isObjImplAdded;


	//bool theIsStop;
};
#endif
