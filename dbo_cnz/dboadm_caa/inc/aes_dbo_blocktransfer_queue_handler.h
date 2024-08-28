/*=================================================================== */
/**
   @file   aes_dbo_blocktransfer_queue_handler.h

   @brief Header file for DBO module.

          This module contains all the declarations useful to
          specify the class AES_DBO_BlockTransferQueueCmdHandler.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/08/2012   XTANAGG   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef AES_DBO_BLOCK_TRANSFER_QUEUE_HANDLER
#define AES_DBO_BLOCK_TRANSFER_QUEUE_HANDLER

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/Event_Handler.h>
#include <ace/Recursive_Thread_Mutex.h>
#include "acs_apgcc_objectimplementerinterface_V3.h"
#include <acs_apgcc_oihandler_V3.h>
#include <aes_dbo_tqsession.h>
/*====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define AES_DBO_BLOCK_TRANSFER_QUEUE_RDN					"blockTransferQueueId"
#define AES_DBO_MIRRORED_ATTR								"mirrored"
#define AES_DBO_REMOVE_DELAY_ATTR 	     					"removeDelay"
#define AES_DBO_BLOCK_DESTINATION_SET_ATTR	      			"blockDestinationSet"
#define AES_DBO_BLOCK_DESTINATION_SET_ID_ATTR     			"blockDestinationSetId"
#define AES_DBO_BLOCK_DESTINATION_SET_RO_ATTR	    		"blockDestinationSetDn"
#define AES_DBO_BLOCK_TRANSFERQ_CLASS_NM	      			"AxeDataTransferBlockTransferQueue"
#define AES_DBO_BLOCK_TRANSFERQ_IMPLEMENTER	      			"BlockTransferQueueImpl"
#define AES_DBO_IMM_DN_PATH_SIZE_MAX 512
class tqRec
{
public:
	string tq;
	string dest;
	unsigned int mirror;
	unsigned int delay;
	int open;
	tqRec():tq(""),dest(""),mirror(0),delay(1440),open(0){}
};
typedef enum action_type
{
	IMMC_NONE,
	IMMC_REMOVE_TQ,
	IMMC_CREATE_TQ,
	IMMC_MODIFY_TQ
}action_type;

class PrevState
{
public:
	tqRec tqData;
	action_type undoAction;
	PrevState():undoAction(IMMC_NONE){}
	~PrevState(){}
};
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

/*===================================================================*/
/**
        @brief		This class implements the object implementer functionality
                        for dbodef, dborm, dboch commands.
 */

/*===================================================================*/
class AES_DBO_BlockTransferQueueCmdHandler:
			public ACE_Event_Handler,
			public acs_apgcc_objectimplementerinterface_V3
{
public:
	/*=====================================================================*/
	/**
	   @brief       This function is the inline constructor 
			for the AES_DBO_BlockTransferQueueCmdHandler class.

	   @param       p_className
			Name of the object.

	   @param       p_impName
			Name of implementer.

	   @param       p_scope
			Set of objects for which implementer is defined.

	 */
        /*==================================================================== */

	AES_DBO_BlockTransferQueueCmdHandler(	string p_className,
				string p_impName,
				ACS_APGCC_ScopeT p_scope);
	/*==================================================================== */

	/**
	   @brief	This function is the destructor for the
			AES_DBO_BlockTransferQueueCmdHandler class.
         */
       /*=====================================================================*/
        ~AES_DBO_BlockTransferQueueCmdHandler();
       /*=====================================================================*/

      	virtual int handle_input (ACE_HANDLE fd = ACE_INVALID_HANDLE);
   /**
           @brief       This function will be called as a callback when an
			object is created as an instance of a class for
			which the application has registered as OI.

           @param       oiHandle
			The object implementer handle.

           @param       ccbId
			Configuration Change Bundle ID.

           @param       className
			Name of the class.

           @param       parentname
			Name of the parent object.

           @param       attr
			Array of ACS_APGCC_AttrValues.

           @return      ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType create(	ACS_APGCC_OiHandle oiHandle,
				 	ACS_APGCC_CcbId ccbId,
				 	const char *className,
				 	const char* parentname,
				 	ACS_APGCC_AttrValues **attr);
        /*=====================================================================*/

	 /**
           @brief       This function will be called as a callback
			when an object of the class is deleted.

           @param       oiHandle
			The Object Implementer Handle.

           @param       ccbId
			Configuration Change Bundle ID.

           @param       objName
			Name of the object.

           @return      ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType deleted(	ACS_APGCC_OiHandle oiHandle,
					ACS_APGCC_CcbId ccbId,
					const char *objName);
	/*=====================================================================*/

        /**
           @brief       This function will called as a callback when an object is modified.

           @param       oiHandle
			The Object Implementer Handle.

           @param       ccbId
			Configuration Change Bundle ID.

           @param       objName
			Name of the object.

           @param       attrMods
			Array of ACS_APGCC_AttrModification.

           @return      ACS_CC_ReturnType.
         */
        /*=====================================================================*/
        ACS_CC_ReturnType modify(	ACS_APGCC_OiHandle oiHandle,
					ACS_APGCC_CcbId ccbId,
					const char *objName,
					ACS_APGCC_AttrModification **attrMods);
        /*=====================================================================*/

        /**
           @brief       This function will be called when a Configuration Change bundle
			is complete.

           @param       oiHandle
			The Object Implementer handle.

           @param       ccbId
			Configuration Change Bundle ID.

           @return      ACS_CC_ReturnType.
         */
        /*=====================================================================*/
        ACS_CC_ReturnType complete(	ACS_APGCC_OiHandle oiHandle,
					ACS_APGCC_CcbId ccbId);
        /*=====================================================================*/
	 /**
           @brief       This function will be called when an registered
			application's object has aborted.

           @param       oiHandle
			The Object Implementer Handle.

           @param       ccbId
			Configuration Change Bundle ID.

           @return      void
         */
        /*=====================================================================*/
        void abort(	ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_CcbId ccbId);
        /*=====================================================================*/

	 /**
           @brief       This function will be called when the change
			is complete and can be applied.

           @param       oiHandle
                        The Object Implementer Handle.

           @param       ccbId
			Configuration Change Bundle Id.

           @return      void
         */
        /*=====================================================================*/
        void apply(	ACS_APGCC_OiHandle oiHandle,
			ACS_APGCC_CcbId ccbId);
	/*=====================================================================*/
        /**
           @brief       This method will be called as a callback when modifying
			a runtime not-cached attribute of a configuration Object
			for which the Application has registered as Object Implementer.

           @param       objName

           @param       attrName

           @return      ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType updateRuntime(	const char *objName,
						const char **attrName);
        /*=====================================================================*/
	/**
           @brief	This method will be called as a callback to manage an
			administrative operation invoked, on the implemented object,
			using the adminOperationInvoke method of acs_apgcc_adminoperation class.

	   @param	p_objName

           @param	oiHandle

           @param	invocation

           @param	p_objName

           @param	operationId

           @param	paramList

	   @return	ACS_CC_ReturnType. On success ACS_CC_SUCCESS on Failure ACS_CC_FAILURE
	*/

        /*=====================================================================*/
        void adminOperationCallback(	ACS_APGCC_OiHandle oiHandle,
					ACS_APGCC_InvocationType invocation,
					const char* p_objName,
					ACS_APGCC_AdminOperationIdType operationId,
					ACS_APGCC_AdminOperationParamType**paramList);
        /*=====================================================================*/
	/**
           @brief       BlockTransferQueueOIThread

           @param       aBlockTransferQueuePtr

           @return      ACE_THR_FUNC_RETURN
         */
        /*=====================================================================*/
        //static ACE_THR_FUNC_RETURN BlockTransferQueueOIThread(void *aBlockTransferQueuePtr);

        /*=====================================================================*/
        /**
            @brief      setupBlockTransferQueueOIThread

            @param      aBlockTransferQueueImpl

	    @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType setupBlockTransferQueueOIThread(AES_DBO_BlockTransferQueueCmdHandler* aBlockTransferQueueImpl, ACE_Reactor * g_dbo_reactor);
        /*=====================================================================*/
        /**
             @brief     shutdown

             @return    void
         */
        /*=====================================================================*/
        void shutdown();
        /*=====================================================================*/
        /**
             @brief     setObjectImplementer

             @return    ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType setObjectImplementer();
	/*============================================================*/
	/*============================================================*/
	/**
	    @brief     setErrorCode

            @param     code

            @return    void
	*/
	/*============================================================*/
	void setErrorCode(unsigned int code, const char* objName= 0);
	/*============================================================*/

	/*============================================================*/
	/**
			@brief     extractTQName

            @param     attrName

            @param     attrType

            @param     tqName

            @return    void
	*/
	/*============================================================*/
	void extractTQName(const char * attrName, const string attrType, string &tqName);
	/*============================================================*/

	/*============================================================*/
	/**
			@brief     validDestSetName

            @param     str

            @return    bool
	*/
	/*============================================================*/
	bool validDestSetName(string str);
	/*============================================================*/

	/*============================================================*/
	/**
			@brief     validTQName

            @param     str

            @return    bool
	*/
	/*============================================================*/
	bool validTQName(string str)	;
	/*============================================================*/
	/*============================================================*/
	/**
			@brief     updateRefAttibutes

            @param     ccbHandleVal

            @param     adminOwnwrHandlerVal

            @param     objName

            @param     attrName

            @param     attrVal

            @return    bool
	*/
	/*============================================================*/
	ACS_CC_ReturnType updateRefAttibutes(
                                ACS_APGCC_CcbHandle ccbHandleVal,
                                ACS_APGCC_AdminOwnerHandle adminOwnwrHandlerVal,
                                string objName,
                                string attrName,
                                string attrVal);

private:
        /*===================================================================
                                PRIVATE ATTRIBUTE
        =================================================================== */
        /**
              @brief   m_poOiHandler
         */
        /*=================================================================== */
        acs_apgcc_oihandler_V3 m_poOiHandler;
        /*=================================================================== */

	/**
	   @brief	theClassName
			The name of the  Block Transfer Queue Class in IMM.
	*/
        /*=====================================================================*/
        string theClassName;
        /*=====================================================================*/

	/**
	 * @brief	m_isClassImplAdded
	 * */
        /*=====================================================================*/
	bool m_isClassImplAdded;
        /*=====================================================================*/

	ACE_Reactor *m_poThreadManager;

	// Access synchronization controllers
	ACE_Recursive_Thread_Mutex _sync_object;
	aes_dbo_tqsession *tqSession_;

};
#endif

