/*=================================================================== */
/**
   @file aes_dbo_blocktransferm_handler.cpp

   Class method implementationn for DBO module.

   This module contains the implementation of class declared in
   the aes_dbo_blocktransferm_handler.h file.

   @version 1.0.0


   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       07/08/2012   XTANAGG   Initial Release
=================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <aes_dbo_blocktransferm_handler.h>
#include <aes_dbo_server.h>
#include <AES_DBO_TQManager.h>
#include <acs_apgcc_omhandler.h>
#include <ACS_APGCC_Util.H>
#include <aes_gcc_log.h>
#include <aes_gcc_tracer.h>
#include <ace/Reactor.h>


GCC_TDEF(AES_DBO_block_transferm_handler);
AES_GCC_TRACE_DEFINE(AES_DBO_block_transferm_handler)

/*===================================================================
   ROUTINE: AES_DBO_BlockTransferMCmdHandler
=================================================================== */
AES_DBO_BlockTransferMCmdHandler::AES_DBO_BlockTransferMCmdHandler(		string aBlockTransferMClassName,
																																			string aBlockTransferMImplName,
																																			ACS_APGCC_ScopeT p_scope )
: acs_apgcc_objectimplementerinterface_V3(aBlockTransferMClassName, aBlockTransferMImplName, p_scope)
, m_poOiHandler()
,theClassName(aBlockTransferMClassName)
{
	GCC_TTRACE((AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler Constructor")); 
	m_poThreadManager = 0;
	m_isClassImplAdded = false;

	GCC_TTRACE((AES_DBO_block_transferm_handler,"(%t) Leaving AES_DBO_BlockTransferMCmdHandler Constructor")); 

}//End of Constructor

/*===================================================================
   ROUTINE: setObjectImplementer
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferMCmdHandler::setObjectImplementer()
{
	ACS_CC_ReturnType errorCode = ACS_CC_FAILURE;

	GCC_TTRACE((AES_DBO_block_transferm_handler,"(%t) Entering setObjectImplementer"));

	for(int i=0; i < 10; i++)
	{
		errorCode = m_poOiHandler.addClassImpl(this, theClassName.c_str());
		if( errorCode == ACS_CC_FAILURE )
		{
			GCC_TERROR(( AES_DBO_block_transferm_handler,"(%t) AES_DBO_BlockTransferMCmdHandler::setObjectImplementer : Error occured while add class implmenter for Destination"));
			int intErr = getInternalLastError();
			if ( intErr == -6)
			{
				ACE_OS::sleep(1);
				continue;
			}
			else
			{
				GCC_TERROR(( AES_DBO_block_transferm_handler,"(%t) AES_DBO_BlockTransferMCmdHandler::setObjectImplementer : Error occured while add class implmenter for Destination, ErrCode = %d", intErr));
				break;
			}
		}
		else
		{
			m_isClassImplAdded = true;
			break;
		}
	}
	GCC_TTRACE((AES_DBO_block_transferm_handler,"(%t) Leaving setObjectImplementer"));
	return errorCode;
}//End of setObjectImplementer

/*===================================================================
   ROUTINE: create
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferMCmdHandler::create(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *className,
		const char* parentName,
		ACS_APGCC_AttrValues **attr)
{
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler::create"));
	(void) oiHandle;
	(void) ccbId;
	(void) className;
	(void) parentName;
	(void) attr;
	setErrorCode(AES_INCORRECTCOMMAND);
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Leaving AES_DBO_BlockTransferMCmdHandler::create"));
	return ACS_CC_FAILURE;
}

/*===================================================================
   ROUTINE: deleted
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferMCmdHandler::deleted( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName)
{
	GCC_TTRACE (( AES_DBO_block_transferm_handler,"(%t) Entering AES_DBO_BlockTransferMCmdHandler::deleted"));
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	setErrorCode(AES_INCORRECTCOMMAND);
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Leaving AES_DBO_BlockTransferMCmdHandler::delete with error code %d", AES_INCORRECTCOMMAND ));
	return ACS_CC_FAILURE;
}//End of deleted

/*===================================================================
ROUTINE: modify
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferMCmdHandler::modify(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId,
		const char *objName,
		ACS_APGCC_AttrModification **attrMods)
{
        (void) attrMods;
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler::modify"));
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	return ACS_CC_SUCCESS;
}//End of modify

/*===================================================================
   ROUTINE: complete
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferMCmdHandler::complete( ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler::complete"));
	(void)oiHandle;
	(void)ccbId;
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Leaving AES_DBO_BlockTransferMCmdHandler::complete"));
	return ACS_CC_SUCCESS;
} //End of complete

/*===================================================================
   ROUTINE: abort
=================================================================== */
void AES_DBO_BlockTransferMCmdHandler::abort(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler::abort"));
	(void)oiHandle;
	(void)ccbId;
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Leaving AES_DBO_BlockTransferMCmdHandler::abort"));
}//End of abort

/*===================================================================
   ROUTINE: updateRuntime
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferMCmdHandler::updateRuntime( const char* objName,
		const char** attrName)
{
	(void)objName;
	(void)attrName;
	return ACS_CC_SUCCESS;
}//End of updateRuntime

/*===================================================================
   ROUTINE: adminOperationCallback
=================================================================== */
void AES_DBO_BlockTransferMCmdHandler::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType**paramList)
{        
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler::adminOperationCallback"));
	(void)paramList;
	(void)oiHandle;
	(void)invocation;
	(void)p_objName;
	(void)operationId;
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Leaving AES_DBO_BlockTransferMCmdHandler::adminOperationCallback"));
	return;

}//End of adminOperationCallback

/*===================================================================
   ROUTINE: apply
=================================================================== */
void AES_DBO_BlockTransferMCmdHandler::apply(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_CcbId ccbId)
{
	GCC_TTRACE(( AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler::apply"));
	(void)oiHandle;
	(void)ccbId;
	//do nothing
	GCC_TTRACE (( AES_DBO_block_transferm_handler, "(%t) Leaving AES_DBO_BlockTransferMCmdHandler::apply"));
}//End of apply

int AES_DBO_BlockTransferMCmdHandler::handle_input(	ACE_HANDLE fd)
{

	GCC_TTRACE((AES_DBO_block_transferm_handler, "(%t) Entering BlockTransferQueueOIThread"));

	AES_GCC_TRACE_MESSAGE("IMM event notified ! Start dispatching ... ");

	errno = 0;
	if (reactor()->resume_handler(fd)) {
		int errno_save = errno;
		AES_GCC_TRACE_MESSAGE("Call 'reactor()->resume_handler' failed. Error: %d", errno_save);
	}

	if (acs_apgcc_objectimplementerinterface_V3::dispatch(ACS_APGCC_DISPATCH_ALL) == ACS_CC_SUCCESS)
		AES_GCC_TRACE_MESSAGE("IMM event successfully dispatched ! ");
	else
		AES_GCC_TRACE_MESSAGE( "IMM event NOT dispatched ! IMM error_code == %d ", acs_apgcc_objectimplementerinterface_V3::getInternalLastError());

	return 0;
}//End of BlockTransferQueueOIThread

/*===================================================================
   ROUTINE: setupBlockTransferMOIThread
=================================================================== */
ACS_CC_ReturnType AES_DBO_BlockTransferMCmdHandler::setupBlockTransferMOIThread(AES_DBO_BlockTransferMCmdHandler * aBlockTransferMImpl, 
		ACE_Reactor* threadManager_)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	if(!threadManager_)
	{
		AES_GCC_TRACE_MESSAGE("FATAL - Empty reactor!");
		return ACS_CC_FAILURE;
	}
	m_poThreadManager = threadManager_;
	GCC_TTRACE((AES_DBO_block_transferm_handler, "(%t) Entering setupBlockTransferMOIThread"));
	int result = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	if( aBlockTransferMImpl->setObjectImplementer() != ACS_CC_SUCCESS)
	{
		GCC_TDEBUG((AES_DBO_block_transferm_handler, "(%t) setObjectImplementer failed"));
		GCC_TTRACE((AES_DBO_block_transferm_handler, "(%t) Leaving AES_DBO_BlockTransferMCmdHandler::BlockTransferMOIThread"));
		return ACS_CC_FAILURE;
	}
	AES_GCC_TRACE_MESSAGE("Registering handler into the reactor!");
	if((result = m_poThreadManager->register_handler(getSelObj(), this, ACE_Event_Handler::READ_MASK)) < 0){
		AES_GCC_TRACE_MESSAGE("Error on Register handler! - error %d",result);
	//TODO - REMOVE CLASS IMPEMENTER
	}

	AES_GCC_TRACE_MESSAGE("Setup completed! Block Transfer queue command correctly inizialized! ");

	GCC_TTRACE((AES_DBO_block_transferm_handler, "(%t) Leaving setupBlockTransferMOIThread"));
	return rc;
}//End of setupBlockTransferMOIThread

/*===================================================================
	ROUTINE: shutdown
=================================================================== */
void AES_DBO_BlockTransferMCmdHandler::shutdown()
{
	AES_GCC_TRACE_MESSAGE("Entering shutdown");
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(_sync_object, true);
	AES_GCC_TRACE_MESSAGE("Calling end_reactor_event_loop");

	int op_res = m_poThreadManager->remove_handler(getSelObj(), ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);
	if(!op_res)
		AES_GCC_TRACE_MESSAGE("Object unregistered from reactor.");
	else
		AES_GCC_TRACE_MESSAGE("Error trying to unregistered object from reactor.");
	if( m_isClassImplAdded == true )
	{
		if( m_poOiHandler.removeClassImpl(this, theClassName.c_str()) == ACS_CC_FAILURE )
		{
			GCC_TERROR((AES_DBO_block_transferm_handler, "(%t) Error occured while removing class implementer"));
			int intErr = getInternalLastError();
			GCC_TERROR((AES_DBO_block_transferm_handler, "(%t) Error occured while removing class implementer, ErrCode = %d", intErr));
		}
		else
		{
			m_isClassImplAdded = false;
		}

	}

	GCC_TTRACE((AES_DBO_block_transferm_handler, "(%t) Leaving shutdown"));
}//End of shutdown

/*===================================================================
	ROUTINE: ~AES_DBO_BlockTransferMCmdHandler
=================================================================== */
AES_DBO_BlockTransferMCmdHandler::~AES_DBO_BlockTransferMCmdHandler()
{
	GCC_TTRACE((AES_DBO_block_transferm_handler, "(%t) Entering AES_DBO_BlockTransferMCmdHandler Destructor"));
}//End of Destructor

/*===================================================================
        ROUTINE: setErrorCode
=================================================================== */
void AES_DBO_BlockTransferMCmdHandler::setErrorCode(unsigned int code)
{
	string ertxt = AES_DBO_TQManager::instance()->getErrorText(code);
	GCC_TTRACE (( AES_DBO_block_transferm_handler,"setExitCode : exitCode = %d , errorText = %s",code,ertxt.c_str()));
	setExitCode(code,ertxt );
}

