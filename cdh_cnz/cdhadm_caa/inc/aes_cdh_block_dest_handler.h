/*=================================================================== */
/**
   @file   aes_cdh_block_dest_handler.h

   @brief Header file for CDH module.

          This module contains all the declarations useful to
          specify the class AES_CDH_BlockDestCmdHandler.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/11/2011   XGANGAB   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef AES_CDH_BLOCK_DEST_HANDLER_H
#define AES_CDH_BLOCK_DEST_HANDLER_H

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
/*====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
#define AES_CDH_BLOCK_ADV_PARAMS_IMPLEMENTER       	"BlockAdvancedParamsImplementer"
//#define AES_CDH_BLOCK_ADV_PARAMS_IMM_CLASS_NM      	"AvancedBlockDestSetParameters"
#define AES_CDH_BLOCK_ADV_PARAMS_IMM_CLASS_NM      	"AxeDataTransferAdvancedBlockDestSetInfo"
#define AES_CDH_BLOCK_ADV_PARAMS_RDN			"advancedBlockDestSetInfoId"
#define AES_CDH_BLOCK_ADV_PARAMS_IMM_RDN_CL_NM		"advancedBlockDestSetInfo"
//#define AES_CDH_BLOCK_ADV_PARAMS_RDN			"advancedBlockDestinationParametersId"
#define AES_CDH_PRIMARY_BLOCK_DEST_IMPLEMENTER       	"PrimaryBlockDestImplementer"
#define AES_CDH_PRIMARY_BLOCK_DEST_IMM_CLASS_NM         "AxeDataTransferPrimaryBlockDestination"	
#define AES_CDH_PRIMARY_BLOCK_DEST_RDN		        "primaryBlockDestinationId"	
#define AES_CDH_SECONDARY_BLOCK_DEST_IMPLEMENTER       	"SecondaryBlockDestImplementer"
#define AES_CDH_SECONDARY_BLOCK_DEST_IMM_CLASS_NM       "AxeDataTransferSecondaryBlockDestination"	
#define AES_CDH_SECONDARY_BLOCK_DEST_RDN		"secondaryBlockDestinationId"	
#define AES_CDH_PRIMARY_BLOCK_DEST_RDN_CL_NM		"primaryBlockDestination"
#define AES_CDH_SECONDARY_BLOCK_DEST_RDN_CL_NM		"secondaryBlockDestination"	
#define AES_CDH_BLOCK_DEST_IP_ADDR			"ipAddress"
#define AES_CDH_BGWRPC_TRANSFER_TYPE			"BGWRPC"
#define AES_CDH_BLOCK_RETRYDELAY			"retryDelay"
#define AES_CDH_BLOCK_SENDRETRY				"sendRetry"


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
class AES_CDH_BlockDestCmdHandler:
			public acs_apgcc_objectimplementereventhandler_V3
{
public:
	/*=====================================================================*/
	/**
	   @brief       This function is the inline constructor 
			for the AES_CDH_BlockDestCmdHandler class.

	   @param       p_className
			Name of the object.

	   @param       p_impName
			Name of implementer.

	   @param       p_scope
			Set of objects for which implementer is defined.

	 */
        /*==================================================================== */

	AES_CDH_BlockDestCmdHandler(	string p_className,
				string p_impName,
				ACS_APGCC_ScopeT p_scope);
	/*==================================================================== */

	/**
	   @brief	This function is the destructor for the
			AES_CDH_BlockDestCmdHandler class.
         */
       /*=====================================================================*/
        ~AES_CDH_BlockDestCmdHandler();
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
        ACS_CC_ReturnType setupDestOIThread(AES_CDH_BlockDestCmdHandler* aDestImplementer, ACE_Thread_Manager* threadManager_);
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

	/**
             @brief     isDestNameValid
				This method checks if the destination name is valid or not.

             @param     destName
				Name of the destination

             @return    bool
				
         */

        /*============================================================*/
        bool isDestNameValid( const string& destName );
        /*============================================================*/
        /*============================================================*/

	/**
             @brief     setErrorCode
				This method sets the error codes.

             @param     code
				 exit/error code

             @return    void

         */

        /*============================================================*/
        void setErrorCode(unsigned int code,const char* objName=0);
	bool getExistingDestObjFromIMM( const char *objName, int& tmpArgc, char** tmpArgv );
	bool getExistingAdvDestObjFromIMM( const char *objName, int& tmpArgc, char** tmpArgv );

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
	 * @brief	m_isClassImplAdded
	 */
        /*=====================================================================*/
	bool m_isClassImplAdded;
        /*=====================================================================*/

	/**
	 * @brief	m_DestOIThreadId;
	 */
        /*=====================================================================*/
	ACE_thread_t m_DestOIThreadId;
        /*=====================================================================*/

	ACS_APGCC_OiHandle m_blkPrimOiHandle;

	std::vector <struct destInfo> m_modifyDestList;

	std::vector <struct destInfo> m_deleteDestList;

	ACS_APGCC_CcbId m_primDestCreateccbId;
        ACS_APGCC_CcbId m_secDestCreateccbId;
        static ACE_Recursive_Thread_Mutex ccbId_mtx;	
	static CommitInfo m_iCommit;	     

	
};
#endif
