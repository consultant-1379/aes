/*=================================================================== */
/**
   @file   aes_cdh_filerespdestset_handler.h

   @brief Header file for CDH module.

          This module contains all the declarations useful to
          specify the class AES_CDH_FileRespDestSetCmdHandler.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       11/04/2012   XCHEMAD   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_FILERESPDESTSET_HANDLER_H
#define AES_CDH_FILERESPDESTSET_HANDLER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ACE.h>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Task.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <acs_apgcc_omhandler.h>

/*====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
using namespace std;
#define AES_CDH_RESPDESTSET_IMPLEMENTER       	"RespondingDestinationSetImpl"
#define AES_CDH_RESP_ACTIVE_DEST_DESTSET    	"respActiveDestination"
#define AES_CDH_RESPDESTSET_CLASSNAME         "AxeDataTransferRespondingDestinationSet"

/**
 * @brief AES_CDH_FileRespDestSetCmdHandler
 * AES_CDH_FileRespDestSetCmdHandler class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 *
 */
class AES_CDH_FileRespDestSetCmdHandler:public acs_apgcc_objectimplementereventhandler_V3,public ACE_Task_Base
{
public:
	/**
	 * @brief
	 * constructor with two parameters
	 */
	AES_CDH_FileRespDestSetCmdHandler(string className,string szimpName, ACE_Thread_Manager* threadManager_);
   /**
   	* @brief
   	* destructor
   	*/
	~AES_CDH_FileRespDestSetCmdHandler();
	/**
	 *  create method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object created.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
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
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
	/**
	 *  modify method: This method is inherited from base class and overridden by our class.
	 *  This method is get invoked when IMM object's attribute modify.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @param  attr          : ACS_APGCC_AttrValues
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods);
	/**
	 *  complete method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 *  @return ACS_CC_ReturnType : Sucess/failure
	 */
	ACS_CC_ReturnType complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  abort method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  apply method: This method is inherited from base class and overridden by our class.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	void apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId);
	/**
	 *  updateRuntime method: This method is inherited from base class and overridden by our class.
	 *  @param  objName          : const char pointer
	 *  @param  attrName         : const char pointer
	 */
	ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);
	/**
	 * @brief Method
	 * @param oiHandle
	 * @param invocation
	 * @param p_objName
	 * @param operationId
	 * @param paramList
	 */
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);
	/**
	 * @brief svc
	 * @return ACE_INT32
	 */
	ACE_INT32 svc();
	/**
	 * @brief shutdown
	 */
	void shutdown();

	ACS_CC_ReturnType setObjImpl();

	/*============================================================*/
	/**
	         @brief     setErrorCode

	         @param     code

			 @param 	objName

	         @return    void
	 */
	/*============================================================*/
	void setErrorCode(unsigned int code, const char * objName=0);
	/*============================================================*/
private:
	/**
	 * @brief m_oiHandler
	 */
	acs_apgcc_oihandler_V3 * m_oiHandler;
	/**
	 * @brief m_tpReactorImpl
	 */
	ACE_TP_Reactor * m_tpReactorImpl;
	/**
	 * @brief m_poReactor
	 */
	ACE_Reactor *m_poReactor;
	/**
	 * @brief m_className
	 */
	string m_className;
	/**
	 * @brief m_deleteCcbId
	 */
	ACS_APGCC_CcbId m_deleteCcbId;
	bool m_isClassImplAdded;
	/**
	 * @brief extractNameFromRdn
	 * @param rdnName
	 * @param name
	 */
	void extractNameFromRdn( const string rdnName, string& name);
	/**
	 * @brief extractNameFromDn
	 * @param dnName
	 * @param name
	 */
	void extractNameFromDn (const string dnName, string& name);
	/**
	 * @brief isValidName
	 * @param name
	 * @return
	 */
	bool isValidName(string name);

	std::vector <struct destNode> m_deleteDestList;

};

#endif
