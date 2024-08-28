#ifndef AES_AFP_DATATRANSFERM_HANDLER_H
#define AES_AFP_DATATRANSFERM_HANDLER_H

#include <string>
#include <ace/ACE.h>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/Task.h>
#include <ace/TP_Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_oihandler_V3.h>
#include "aes_afp_api.h"
#include "aes_afp_datablock.h"
#include "aes_afp_datatask.h"
#include "aes_afp_defines.h"
#include <aes_gcc_errorcodes.h>
using namespace std;

/**
 * @brief AES_AFP_DataTransferMCmdHandler
 * AES_AFP_DataTransferMCmdHandler class is extended from acs_apgcc_objectImplementereventhandler_V3.
 * It is having functions to handle IMM callbacks
 *
 *
 */

#define AES_AFP_DATATRANSFERM_IMPLEMENTER       "DataTransferMImpl"
#define AES_AFP_DATATRANSFERM_CLASSNAME      	"AxeDataTransferDataTransferM"

class AES_AFP_DataTransferMCmdHandler:public acs_apgcc_objectimplementereventhandler_V3,public ACE_Task_Base
{
public:
	/**
	 * @brief
	 * constructor with two parameters
	 */
	AES_AFP_DataTransferMCmdHandler(string className,string szimpName,aes_afp_datatask &dataThread,
									ACE_Thread_Manager* thrMgr);
   /**
   	* @brief
   	* constructor with three parameters
   	*/
	AES_AFP_DataTransferMCmdHandler(string szobjName,
		                   string szimpName,
		                   ACS_APGCC_ScopeT enScope,aes_afp_datatask &dataThread, ACE_Thread_Manager* thrMgr );
   /**
   	* @brief
   	* destructor
   	*/
	~AES_AFP_DataTransferMCmdHandler();
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
        
	void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,ACS_APGCC_InvocationType invocation, const char* p_objName, ACS_APGCC_AdminOperationIdType operationId,  ACS_APGCC_AdminOperationParamType**paramList);

	void extractNameFromDn (const string dnName, string& name);
        void extractClassNameFromRdn( const string myObjRdn,string &myClassName);

	//int handle_signal(ACE_INT32 signum, siginfo_t * siginfo, ucontext_t * ucontext);
	/**
	 *  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
	 *	and registers the callback function.
	 *  @param  oiHandle      :	ACS_APGCC_OiHandle
	 *  @param  ccbId         : ACS_APGCC_CcbId
	 */
	ACE_INT32 svc();
	void shutdown();
        ACS_CC_ReturnType setImpl();
	aes_afp_datatask *dataThrd;
	OmHandler omHandler;
	//static bool deleteflag;

private:

	acs_apgcc_oihandler_V3 * m_oiHandler;
	ACE_TP_Reactor * m_tpReactorImpl;
	ACE_Reactor *m_poReactor;
	string m_className;
	bool m_isClassImplAdded;

//  ACS_SSU_ErrorRuntimeHandler* theErrorRuntimeHandlerPtr;
  /*!
   * @brief Deletes the error handler object
   * @return TRUE after successful deletion
   * @return FALSE in case there is error
   */
//  bool deleteErrorHandlerObject();
  /*!
   * @brief Creates the error handler object
   * @param  ACS_APGCC_CcbId	:	Error Object ID
   * @param  aErrorId      		:	Error ID
   * @param  aErrorText      	:	Error Text
   * @return TRUE after successful deletion
   * @return FALSE in case there is error
   */
//  bool createErrorHandlerObject(ACS_APGCC_CcbId& aCCBId,unsigned int& aErrorId,std::string & aErrorText);
  /*!
   * @brief Creates the Sets the error text
   * @param  ACS_APGCC_CcbId	:	Error Object ID
   * @param  aErrorId      		:	Error ID
   * @param  aErrorText      	:	Error Text
   * @return TRUE after successful deletion
   * @return FALSE in case there is error
   */
  void setErrorText(unsigned int code);
protected:
  AES_AFP_Api api_;

};



#endif
