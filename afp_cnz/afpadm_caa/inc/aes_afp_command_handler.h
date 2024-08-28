#ifndef AES_AFP_COMMANDHANDLER_H
#define AES_AFP_COMMANDHANDLER_H
/*Include Section */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ACS_APGCC_RuntimeOwner_V2.h>
//#include "aes_afp_services.h"
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include "aes_afp_api.h"
#include "aes_afp_datablock.h"
#include "aes_afp_datatask.h"
#include <acs_apgcc_objectimplementerinterface_V3.h>
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/TP_Reactor.h>
#include <aes_gcc_util.h>

//abort cleanup actions list
typedef enum action_type
{
  IMMC_NONE,
  IMMC_REMOVE_TQ, //for create() cleanup
  IMMC_REMOVE_DESTSET, //for destset association cleanup
  IMMC_CREATE_TQ_REC, //for deleted cleanup
  IMMC_CREATE_TQ, //for destset de-association cleanup
  IMMC_MODIFY_TQ //for param modify clean
} action_type;

struct CCBPrevState
{
   aes_afp_datablock prev_db[3];
   //string destset[3];
   Dest_Set_Type dType[3];
   int size;
   action_type undoAction;
   CCBPrevState(): prev_db(), dType(), size(0), undoAction(IMMC_NONE) {}
   ~CCBPrevState(){}
};


class AES_AFP_CommandHandler:public acs_apgcc_objectimplementereventhandler_V3,public ACE_Task_Base
{
public:

        /**
         * @brief
         * constructor with three parameters
         */
	    AES_AFP_CommandHandler( string ClassName, string p_impName, ACS_APGCC_ScopeT p_scope,aes_afp_datatask &dataThread , ACE_Thread_Manager* thrMgr);

   /**
        * @brief
        * destructor
        */
        ~AES_AFP_CommandHandler();
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
        static ACE_Recursive_Thread_Mutex abortMX_;
        ACS_CC_ReturnType create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentname, ACS_APGCC_AttrValues **attr);
        /**         */

        /**
         *  deleted method: This method is inherited from base class and overridden by our class.
         *  This method is get invoked when IMM object deleted.
         *  @param  oiHandle      :     ACS_APGCC_OiHandle
         *  @param  ccbId         : ACS_APGCC_CcbId
         *  @return ACS_CC_ReturnType : Sucess/failure
         */
        ACS_CC_ReturnType deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName);
        /**
         *  modify method: This method is inherited from base class and overridden by our class.
         *  This method is get invoked when IMM object's attribute modify.
         *  @param  oiHandle      :     ACS_APGCC_OiHandle
         *  @param  ccbId         : ACS_APGCC_CcbId
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
        ACS_CC_ReturnType updateRuntime(const char *objName, const char **attrName);

        void adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
									ACS_APGCC_InvocationType invocation,
									const char* p_objName,
									ACS_APGCC_AdminOperationIdType operationId,
									ACS_APGCC_AdminOperationParamType**paramList);
	ACS_CC_ReturnType updateRefAttibutes(ACS_APGCC_CcbHandle ccbHandleVal,
                        ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal,
                        string objName,string attrName,vector<string> attrVal);
        /**
         *  svc method: This method is used to initializes the object Implementer functions of the IMM service for the invoking process
         *      and registers the callback function.
         *  @param  oiHandle      :     ACS_APGCC_OiHandle
         *  @param  ccbId         : ACS_APGCC_CcbId
         */
        ACE_INT32 svc();
        void shutdown();
		ACS_CC_ReturnType setImpl();
        void extractNameFromRdn( const string rdnName,string &name);
        static void extractNameFromDn (const string dnName, string& name);
        void extractClassNameFromRdn( const string myObjRdn,
                string &myClassName);
	void extractParentNameFromRdn( const string myObjRdn,string &myParentName);
	void extractFileTQNameFromDn ( string dnName, string& name);
        void unsetflags();
        void setErrorText(unsigned int code, const char* objName = 0);

        string theClassName;
        acs_apgcc_oihandler_V3 oiHandler;

        OmHandler omHandler;

        aes_afp_datatask *dataThrd;
	ACE_TP_Reactor *m_tpReactorImpl;
        ACE_Reactor *m_poReactor;
	aes_afp_datablock *dblock;
	bool changeDefaultStatus;
	bool changeRenameTemplate;
	bool changeNameTag1;
	bool changeNameTag2;
	bool changeRemoveBefore;
	bool changeRemoveDelay;
	bool changeSendRetries;
	bool changeRetryInterval;
	bool changeStartupSequenceNumber; //HT50930
 	static string m_rdnName;
    static string m_parentName;
	static bool isEmptyDestination;

protected:
        AES_AFP_Api api_;
private:
	bool m_isClassImplAdded;
	ACS_APGCC_CcbId m_createFileTQCcbId;
	ACS_APGCC_CcbId m_createFileTQParamCcbId;
	static bool m_commit;
	static string m_tqcommit;	
	
};



#endif



