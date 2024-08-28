/*=================================================================== */
/**
   @file   aes_cdh_file_resp_dest_handler.h

   @brief Header file for CDH module.

          This module contains all the declarations useful to
          specify the class AES_CDH_FileRespDestCmdHandler.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       10/10/2011   XTANAGG   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef AES_CDH_RESP_DESTINATION_HANDLER_H
#define AES_CDH_RESP_DESTINATION_HANDLER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ace_wchar.h>
#include <ace/Synch.h>
#include <ace/Event_Handler.h>
#include <ace/TP_Reactor.h>
#include <ace/Reactor.h>
#include <acs_apgcc_objectimplementereventhandler_V3.h>
#include <acs_apgcc_oihandler_V3.h>
#include <ace/ACE.h>
#include <ace/Thread_Manager.h>
#include <ace/OS.h>
#include <ace/Message_Queue.h>
#include <ace/Event_Handler.h>
#include <servr.h>
/*====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define AES_CDH_FILE_RESP_DEST_IMM_PRIM_CLASS_NM 	"AxeDataTransferPrimaryRespondingDestination"
#define AES_CDH_FILE_RESP_DEST_PRIM_RDN_CL_NM		"primaryRespondingDestination"
#define AES_CDH_FILE_RESP_DEST_IMM_PRIM_IMPL		"PrimaryRespondingDestinationImpl"
#define AES_CDH_FILE_RESP_DEST_IMM_SEC_IMPL		"SecondaryRespondingDestinationImpl"
#define AES_CDH_ADV_RESP_DEST_IMM_IMPL	      		"AdvancedRespondingDestinationImpl"
#define AES_CDH_FILE_RESP_DEST_IMM_SEC_CLASS_NM		"AxeDataTransferSecondaryRespondingDestination"
#define AES_CDH_FILE_RESP_DEST_SEC_RDN_CL_NM		"secondaryRespondingDestination"
#define AES_CDH_ADV_RESP_PARAMS_CLASS_NM		"AxeDataTransferAdvancedRespondingParameters"
#define AES_CDH_ADV_RESP_PARAMS_RDN_CL_NM               "advancedRespondingParameters"
#define	AES_CDH_PRIM_RESP_DESTINATION_ID		"primaryRespondingDestinationId"
#define AES_CDH_SEC_RESP_DESTINATION_ID			"secondaryRespondingDestinationId"
#define AES_CDH_RESP_NBI_FOLDER_PATH			"nbiFolderPath"
#define AES_CDH_RESP_NBI_FOLDER_NAME		 "nbiFolderName"
#define	AES_CDH_EVENT_TYPE			"eventType"
#define AES_CDH_RESP_NOTIFICATION_ADDR		"ipNotificationAddress"
#define	AES_CDH_RESEND_NOTIFICATION_TIMER	"resendNotificationTimer"
#define AES_CDH_RESPONSE_TIMER			"notificationResponseTimer"
#define AES_CDH_RESP_NOTIFICATION_PORT_NO	"portNotificationNumber"
#define AES_CDH_REMOVE_TQ_PREFIX		"inhibitTransferQueueNamePrefx"
#define AES_CDH_RESP_SUPPRESS_CREATE_SUB_FOLDER	"suppressSubFolderCreation"
#define AES_CDH_TRANSFER_TYPE			"transferType"
#define	AES_CDH_FTPV2_TRANSFER_TYPE		"FTPV2"
#define	AES_CDH_SFTPV2_TRANSFER_TYPE		"SFTPV2"
#define	AES_CDH_BGWRPC_TRANSFER_TYPE		"BGWRPC"
#define AES_CDH_RESP_DEST_KEEPTIME 		"fileKeepTime"
#define	AES_CDH_CDHDEFINE			"cdhdef"
#define	AES_CDH_CDHCHANGE			"cdhch"
#define	AES_YES					"yes"
#define AES_NO					"no"
#define AES_CDH_ADV_RESP_PARAM_ID		"advancedRespondingParametersId"
#define	AES_CDH_RESP_FILE_OVERWRITE		"fileOverwrite"
#define AES_CDH_CDHREMOVE			"cdhrm"
#define AES_CDH_FILE_GROUP_DN			"fileGroupId"
#define AES_CDH_RESP_DATA_TRANS_FOLDER_RDN	"fileGroupId=data_transfer,logicalFsId=1,fileMId=1,systemFunctionsId=1,managedElementId=1"
#define AES_CDH_RESP_DATA_TRANS1_FOLDER_RDN	"fileGroupId=data_transfer,logicalFsId=1,fileMId=1"

struct CommitInfo;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */

/*===================================================================*/
/**
        @brief		This class implements the object implementer functionality
                        for cdhdef, cdhrm, cdhch commands.
 */

/*===================================================================*/
class AES_CDH_FileRespDestCmdHandler:
			public acs_apgcc_objectimplementereventhandler_V3
{
public:
	/*=====================================================================*/
	/**
	   @brief       This function is the inline constructor 
			for the AES_CDH_FileRespDestCmdHandler class.

	   @param       p_className
			Name of the object.

	   @param       p_impName
			Name of implementer.

	   @param       p_scope
			Set of objects for which implementer is defined.

	 */
        /*==================================================================== */

	AES_CDH_FileRespDestCmdHandler(	string p_className,
				string p_impName,
				ACS_APGCC_ScopeT p_scope);
	/*==================================================================== */

	/**
	   @brief	This function is the destructor for the
			AES_CDH_FileRespDestCmdHandler class.
         */
       /*=====================================================================*/
        ~AES_CDH_FileRespDestCmdHandler();
       /*=====================================================================*/

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
           @brief       DestOIThread

           @param       aCDHDestPtr

           @return      ACE_THR_FUNC_RETURN
         */
        /*=====================================================================*/
        static ACE_THR_FUNC_RETURN DestOIThread(void *aCDHDestPtr);

        /*=====================================================================*/
        /**
            @brief      setupDestOIThread

            @param      aDestImplementer

	    @return	ACS_CC_ReturnType
         */
        /*=====================================================================*/
        ACS_CC_ReturnType setupDestOIThread(AES_CDH_FileRespDestCmdHandler* aDestImplementer, ACE_Thread_Manager* threadManager_);
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

        /**
             @brief     extractDestName

	     @param	attrName

	     @param	attrType

	     @param	destinationName

             @return    void
         */

	/*============================================================*/
	void extractDestName(const string attrName,
			   const string attrType,
			   string &destinationName);
	/*============================================================*/

	/**
             @brief     extractClassNameFromRdn

             @param     myObjRdn

             @param     myClassName

             @return    void
         */

        /*============================================================*/
        void extractClassNameFromRdn( const string myObjRdn,
                                   string& myClassName);
        /*============================================================*/
	/**
             @brief     isIPAddrValid

             @param     aIPAddr

             @return    bool
         */

        /*============================================================*/
	bool isIPAddrValid( const char * aIPAddr );
        /*============================================================*/
	
        /*============================================================*/
	/**
             @brief     isRespDestNameValid

             @param     aDestinationName

             @return    bool
         */
        /*============================================================*/
	bool isRespDestNameValid( const string& aDestinationName );
        /*============================================================*/

	/*============================================================*/
	/**
	    @brief     setErrorCode

            @param     code

			@param	   objName

            @return    void
	*/
	/*============================================================*/
	void setErrorCode(unsigned int code, const char* objName = 0);
	/*============================================================*/
	/*============================================================*/
	/**
			@brief     getExistingDestObjFromIMM

            @param     objName

			@param     tmpArgc

			@param	   tmpArgv

            @return    bool
	*/
	/*============================================================*/
	bool getExistingDestObjFromIMM( const char *objName, int& tmpArgc, char** tmpArgv );
	/*============================================================*/
	/*============================================================*/
	/**
			@brief     getExistingAdvDestObjFromIMM

            @param     objName

			@param     tmpArgc

			@param	   tmpArgv

            @return    bool
	*/
	/*============================================================*/
	bool getExistingAdvDestObjFromIMM( const char *objName, int& tmpArgc, char** tmpArgv );
	/*============================================================*/


private:
        /*===================================================================
                                PRIVATE ATTRIBUTE
        =================================================================== */
        /**
              @brief   m_poTp_reactor
         */
        /*=================================================================== */
        ACE_TP_Reactor *m_poTp_reactor;
        /*=================================================================== */
        /**
              @brief   m_poReactor
         */
        /*=================================================================== */
        ACE_Reactor *m_poReactor;

        /*=================================================================== */

        /**
              @brief   m_poOiHandler
         */
        /*=================================================================== */
        acs_apgcc_oihandler_V3* m_poOiHandler;
        /*=================================================================== */

	/**
	   @brief	theClassName
			The name of the  Destination Class in IMM.
	*/
        /*=====================================================================*/
        string theClassName;
        /*=====================================================================*/

	 /**
           @brief       m_nbiFolderName
                        The name of the Nbi Folder IMM.
        */
        /*=====================================================================*/
        string m_nbiFolderName;
        /*=====================================================================*/

	/**
	 * @brief	m_isClassImplAdded
	 * */
        /*=====================================================================*/
	bool m_isClassImplAdded;
        /*=====================================================================*/

	/**
	 * @brief	m_DestOIThreadId
	 */
        /*=====================================================================*/
	ACE_thread_t m_DestOIThreadId;
        /*=====================================================================*/

	struct destNode *m_pcreateDestNode;

	std::vector <struct destNode> m_modifyDestList;

	std::vector <struct destNode> m_deleteDestList;
	ACS_APGCC_CcbId m_primDestCreateccbId;
	ACS_APGCC_CcbId m_secDestCreateccbId;
	static ACE_Recursive_Thread_Mutex ccbId_mtx;
	static CommitInfo m_iCommit;

};
#endif

