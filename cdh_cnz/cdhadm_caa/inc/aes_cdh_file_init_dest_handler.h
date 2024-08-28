/*=================================================================== */
/**
   @file   aes_cdh_file_init_dest_handler.h

   @brief Header file for CDH module.

          This module contains all the declarations useful to
          specify the class AES_CDH_FileInitDestCmdHandler.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       29/11/2011   XCHEMAD   Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */

#ifndef AES_CDH_FILE_INIT_DEST_HANDLER_H
#define AES_CDH_FILE_INIT_DEST_HANDLER_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <list>
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
#define AES_CDH_ADVINITPARAMS_IMPLEMENTER       	"AdvancedInitParamsImplmenter"
#define AES_CDH_ADVINITPARAMS_IMM_CLASS_NM      	"AxeDataTransferAdvancedInitiatingParameters"
#define AES_CDH_ADVINITPARAMS_IMM_RDN_CL_NM 		"advancedInitiatingParameters"
#define AES_CDH_ADVINITPARAMS_RDN			"advancedInitiatingParametersId"
#define AES_CDH_PRIMARYINITDEST_IMPLEMENTER       	"PrimaryInitiatingDestImplmenter"
#define AES_CDH_PRIMARYINITDEST_IMM_CLASS_NM      	"AxeDataTransferPrimaryInitiatingDestination"
#define AES_CDH_PRIMINITDEST_RDN_CL_NM			"primaryInitiatingDestination"
#define AES_CDH_PRIMARYINITDEST_RDN			"primaryInitiatingDestinationId"
#define AES_CDH_SECONDARYINITDEST_IMPLEMENTER       	"SecondaryInitiatingDestImplmenter"
#define AES_CDH_SECONDARYINITDEST_IMM_CLASS_NM      	"AxeDataTransferSecondaryInitiatingDestination"
#define AES_CDH_SECINITDEST_RDN_CL_NM			"secondaryInitiatingDestination"
#define AES_CDH_SECONDARYINITDEST_RDN			"secondaryInitiatingDestinationId" 

#define AES_CDH_FILE_FILEOVERWRITE			"fileOverwrite"
#define AES_CDH_FILE_RETRYDELAY				"retryDelay"
#define AES_CDH_FILE_SENDRETRY				"sendRetry"
#define AES_CDH_SUPPRESS_SUB_FOLDER_CREATION		"suppressSubFolderCreation"
#define AES_CDH_REMOTE_IP_ADDR				"ipAddress"
#define AES_CDH_PASSWORD				"password"
#define AES_CDH_DEST_PORTNUMBER				"portNumber"
#define AES_CDH_FILE_REMOTE_FOLDER_PATH			"remoteFolderPath"
#define AES_CDH_TRANSFER_PROTOCOL			"transferProtocol"
#define AES_CDH_FILE_USERNAME				"username"
#define AES_CDH_INIT_YES				"yes"
#define AES_CDH_INIT_NO					"no"
#define AES_CDH_SFTPV2_TRANSFER_TYPE			"SFTPV2"
#define AES_CDH_FTPV2_TRANSFER_TYPE			"FTPV2"
#define AES_ECIM_PWD_ATTRIBUTE			        "password"

struct PassDestInfo
{
	string destinationName;
	string passwdDn;
};


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
class AES_CDH_FileInitDestCmdHandler:
			public acs_apgcc_objectimplementereventhandler_V3
{
public:
	/*=====================================================================*/
	/**
	   @brief       This function is the inline constructor 
			for the AES_CDH_FileInitDestCmdHandler class.

	   @param       p_className
			Name of the object.

	   @param       p_impName
			Name of implementer.

	   @param       p_scope
			Set of objects for which implementer is defined.

	 */
        /*==================================================================== */

	AES_CDH_FileInitDestCmdHandler(	string p_className,
				string p_impName,
				ACS_APGCC_ScopeT p_scope);
	/*==================================================================== */

	/**
	   @brief	This function is the destructor for the
			AES_CDH_FileInitDestCmdHandler class.
         */
       /*=====================================================================*/
        ~AES_CDH_FileInitDestCmdHandler();
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
        ACS_CC_ReturnType setupDestOIThread(AES_CDH_FileInitDestCmdHandler* aDestImplementer, ACE_Thread_Manager* threadManager_);
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
	
	/**
             @brief     isRemoteFolderPathValid 
				This method checks if the remote folder path is valid or not.

             @param     remoteFolderPath 
				Name of the remote folder 

             @return    bool
				
         */

        /*============================================================*/
        bool isRemoteFolderPathValid( const string& remoteFolderPath );
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
	bool isPasswordEncrypted(string password);
        void setErrorCode(unsigned int code, const char* objName=0);
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
	/**
	 * @brief	m_passDestInfoList;
	 */
        /*=====================================================================*/
	std::list <PassDestInfo*> m_passDestInfoList;
        /*=====================================================================*/

	std::vector <struct destNode> m_modifyDestList;

	std::vector <struct destNode> m_deleteDestList;
	ACS_APGCC_CcbId m_primDestCreateccbId;
	ACS_APGCC_CcbId m_secDestCreateccbId;
       	static ACE_Recursive_Thread_Mutex ccbId_mtx;
	static CommitInfo m_iCommit; 
        /*=====================================================================*/

};
#endif
