//******************************************************************************
//
//  NAME
//     acs_dbo_brfc_interface.cpp
//
//  COPYRIGHT
//     Ericsson AB, Sweden 2012. All rights reserved.
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
//     2012-07-20 by XNADNAR PA1
//
//  SEE ALSO
//     -
//
//******************************************************************************
/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include <acs_apgcc_omhandler.h>
#include <acs_apgcc_adminoperation.h>
#include <aes_gcc_tracer.h>
#include <aes_gcc_log.h>
#include <aes_dbo_brfc_interface.h>
#include <aes_dbo_server.h>
#include <iostream>

using namespace std;

GCC_TDEF(aes_dbo_brfcthread);
AES_GCC_TRACE_DEFINE(aes_dbo_brfcthread)
ACE_Recursive_Thread_Mutex AES_DBO_BrfcInterface::brfcMX_;
bool AES_DBO_BrfcInterface::isBrfInProgress_= 0;
/*===================================================================
     ROUTINE: AES_DBO_BrfcInterface
 =================================================================== */
AES_DBO_BrfcInterface::AES_DBO_BrfcInterface():
	acs_apgcc_objectimplementerinterface_V3(),
	oiHandler()
{
	isBrfInProgress_ = false;
	m_isObjImplAdded = false;
	m_poThreadManager = 0;
}


/*===================================================================
     ROUTINE: AES_DBO_BrfcInterface
 =================================================================== */
AES_DBO_BrfcInterface::AES_DBO_BrfcInterface(string p_impName ):
		acs_apgcc_objectimplementerinterface_V3(p_impName),
		oiHandler()
{
	isBrfInProgress_ = false;
	m_isObjImplAdded = false;
	m_poThreadManager = 0;
}

/*===================================================================
     ROUTINE: AES_DBO_BrfcInterface
 =================================================================== */
AES_DBO_BrfcInterface::AES_DBO_BrfcInterface(	string p_objName,
																							string p_impName,
																							ACS_APGCC_ScopeT p_scope ):
		acs_apgcc_objectimplementerinterface_V3(p_objName, p_impName, p_scope),
		oiHandler()
{
	isBrfInProgress_ = false;
	m_isObjImplAdded = false;
	m_poThreadManager = 0;
}


/*===================================================================
     ROUTINE: ~AES_DBO_BrfcInterface
 =================================================================== */
AES_DBO_BrfcInterface::~AES_DBO_BrfcInterface()
{
}


/*===================================================================
     ROUTINE: adminOperationCallback
 =================================================================== */
void AES_DBO_BrfcInterface::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** paramList)
{
	GCC_TTRACE((aes_dbo_brfcthread,"------------------------------------------------------------"));
	GCC_TTRACE((aes_dbo_brfcthread,"                   adminOperationCallback called             "));
	GCC_TTRACE((aes_dbo_brfcthread,"------------------------------------------------------------"));

	GCC_TDEBUG((aes_dbo_brfcthread,"p_objName  : %s", p_objName));
	GCC_TDEBUG((aes_dbo_brfcthread,"operationId: %d", operationId));

	switch (operationId)
	{
	case   BRF_PARTICIPANT__PERMIT_BACKUP:

		GCC_TDEBUG((aes_dbo_brfcthread,"%s", "RECV: BRF_PARTICIPANT_PERMIT_BACKUP "));

		//A check needs to be added for permission from service.
		if( aes_dbo_server::isBackUpPermitted() )
		{
			AES_DBO_BrfcInterface::setBrfStatus( true );
		}
		else
		{
			AES_DBO_BrfcInterface::setBrfStatus( false );
		}
		break;

	case BRF_PARTICIPANT__COMMIT_BACKUP:

		GCC_TDEBUG((aes_dbo_brfcthread,"%s", "RECV: BRF_PARTICIPANT_COMMIT_BACKUP"));
		AES_DBO_BrfcInterface::setBrfStatus( false );
		break;

	case BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP:
		GCC_TDEBUG((aes_dbo_brfcthread,"%s", "RECV: BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP "));
		break;

	case BRF_PARTICIPANT__CANCEL_BACKUP:
		GCC_TDEBUG((aes_dbo_brfcthread,"%s", "RECV: BRF_PARTICIPANT_CANCEL_BACKUP"));
		AES_DBO_BrfcInterface::setBrfStatus( false );
		break;

	default:
		GCC_TDEBUG((aes_dbo_brfcthread,"%s", "Invalid operation Id recieved."));
		break;

	}

	unsigned long long requestId = 0;
	int i = 0;
	while( paramList[i] )
	{
		switch ( paramList[i]->attrType )
		{
		/* ONLY FOR requested parameter */
		case ATTR_UINT64T:
			if(strcmp(paramList[i]->attrName, "requestId" ) == 0 )
			{
				requestId = *reinterpret_cast<unsigned long long *>(paramList[i]->attrValues);
				GCC_TDEBUG((aes_dbo_brfcthread,"requestId %d assigned ", requestId));
			}
			break;

		default:
			break;
		}
		i++;
	}

	GCC_TDEBUG((aes_dbo_brfcthread," Received Operation with Identification"));

	//Make the return value false, depending upon state of service.
	int retVal = 1;

	//If operation id corresponds to permit backup request, then return failure if backup operation is not permitted.

	if( operationId == BRF_PARTICIPANT__PERMIT_BACKUP )
	{
		retVal = AES_DBO_BrfcInterface::getBrfStatus();
	}


	int retAdminOperationResult = this->adminOperationResult( oiHandle , invocation, retVal );

	GCC_TDEBUG((aes_dbo_brfcthread, "adminOperationResult retVal = %d, returned = %d", retVal, retAdminOperationResult));

	// HERE YOU CAN RESPONDE WITH ACTION RESULT OR ACTION IN PROGRESS
	// BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS simulation
	if( operationId == BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP )
	{

		retVal = ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS);
		sleep(2);

		retVal = ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}
	else
	{
		retVal = ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}

} /* end method */
ACS_CC_ReturnType AES_DBO_BrfcInterface::setImpl()
{
	GCC_TTRACE((aes_dbo_brfcthread,"%s", "AES_DBO_BrfcThread::setImpl() Entered"));

	const char *dnObjName = BRFC_DBO_OBJ_DNNAME;
	const char *impName = BRFC_DBO_IMPL;
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;

	setObjName(dnObjName);
	setImpName(impName);
	setScope(scope);
	for( int i=0;i < 10; i++)
	{
		if(oiHandler.addObjectImpl(this)==ACS_CC_FAILURE)
		{
			int intErr = getInternalLastError();
			GCC_TWARN((aes_dbo_brfcthread , "(%t) Error occured while adding object implementer for %s, error code is %d ",dnObjName,intErr));
			if(intErr == -6 )
			{
				for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
				if(i>=10)
				{
					GCC_TWARN((aes_dbo_brfcthread , "(%t) Error occured while adding object implementer for %s, error code is %d ",dnObjName,intErr));
					break;
				}
				else
					continue;
			}
			else
			{
				GCC_TWARN((aes_dbo_brfcthread , "(%t) Error occured while adding object implementer for %s, error code is %d ",dnObjName,intErr));
			}

		}
		else
		{
			m_isObjImplAdded = true;
			break;
		}

	}

	GCC_TTRACE((aes_dbo_brfcthread,"%s", "AES_DBO_BrfcThread::setImpl() Leaving"));

	return ACS_CC_SUCCESS;
}//end of setImpl

ACS_CC_ReturnType AES_DBO_BrfcInterface::removeImpl()
{
	GCC_TTRACE((aes_dbo_brfcthread,"AES_DBO_BrfcThread::removeImpl() Entered"));
	if(m_isObjImplAdded)
	{
		const char *dnObjName = BRFC_DBO_OBJ_DNNAME;
		for( int i=0;i < 10; i++)
		{
			if(oiHandler.removeObjectImpl(this)==ACS_CC_FAILURE)
			{
				//Get the error code
				int intErr = getInternalLastError();
				GCC_TWARN((aes_dbo_brfcthread , "(%t) Error occured while removing object implementer for %s, error code is %d ",dnObjName,intErr));
				if(intErr == -6 )
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if(i>=10)
					{
						GCC_TWARN((aes_dbo_brfcthread , "(%t) Error occured while removing object implementer for %s, error code is %d ",dnObjName,intErr));
						break;
					}
					else
						continue;
				}
				else
				{
					GCC_TWARN((aes_dbo_brfcthread , "(%t) Error occured while removing object implementer for %s, error code is %d ",dnObjName,intErr));
				}

			}
			else
			{
				m_isObjImplAdded = false;
				break;
			}

		}

	}
	GCC_TTRACE((aes_dbo_brfcthread,"Leaving AES_DBO_BrfcThread::removeImpl()"));
	return ACS_CC_SUCCESS;
}

/*===================================================================
   ROUTINE: setupBlockTransferQueueOIThread
=================================================================== */
ACS_CC_ReturnType AES_DBO_BrfcInterface::setupBrfcInterfaceThread(AES_DBO_BrfcInterface * aDBOBrfcThreadPtr,
		ACE_Reactor* threadManager_)
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(brfcMX_, true);
	if(!threadManager_)
	{
		AES_GCC_TRACE_MESSAGE("FATAL - Empty reactor!");
		return ACS_CC_FAILURE;
	}
	m_poThreadManager = threadManager_;
	AES_GCC_TRACE_MESSAGE("Entering setupBlockTransferQueueOIThread");
	int result = 0;
	ACS_CC_ReturnType rc = ACS_CC_SUCCESS;

	if( aDBOBrfcThreadPtr->setImpl() != ACS_CC_SUCCESS)
	{
		AES_GCC_TRACE_MESSAGE("setImpl failed");
		AES_GCC_TRACE_MESSAGE("Leaving setImpl");
		return ACS_CC_FAILURE;
	}
	AES_GCC_TRACE_MESSAGE("Registering handler into the reactor!");
	if((result = m_poThreadManager->register_handler(getSelObj(), this, ACE_Event_Handler::READ_MASK)) < 0){
		AES_GCC_TRACE_MESSAGE("Error on Register handler! - error %d",result);
	//TODO - REMOVE CLASS IMPEMENTER
	}

	AES_GCC_TRACE_MESSAGE("Setup completed! Backup and Restore interface correctly initialized! ");
	AES_GCC_TRACE_MESSAGE("Leaving setupBrfcInterface");
	return rc;

}//End of setupBlockTransferQueueOIThread

void AES_DBO_BrfcInterface::shutdown()
{
	AES_GCC_TRACE_MESSAGE("Entering shutdown");

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(brfcMX_, true);

	int op_res = m_poThreadManager->remove_handler(getSelObj(), ACE_Event_Handler::READ_MASK | ACE_Event_Handler::DONT_CALL);
	if(!op_res)
		AES_GCC_TRACE_MESSAGE("Object unregistered from reactor.");
	else
		AES_GCC_TRACE_MESSAGE("Error trying to unregistered object from reactor.");

	if( m_isObjImplAdded == true )
	{
		for (int i=0; i < 3; ++i)
		{
			if(  removeImpl() == ACS_CC_FAILURE )
			{
				AES_GCC_TRACE_MESSAGE("Error occured while removing class implementer");
				int intErr = getInternalLastError();
				if ( intErr == -6)
				{
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
						continue;
				}
				else
				{
					AES_GCC_TRACE_MESSAGE("Error occured while removing class implementer, ErrCode = %d", intErr);
					break;
				}
			}
			else
			{
				m_isObjImplAdded = false;
				break;
			}
		}
	}
	AES_GCC_TRACE_MESSAGE("Leaving shutdown");
}//End of shutdown

ACS_CC_ReturnType AES_DBO_BrfcInterface::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_DBO_BrfcInterface::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_DBO_BrfcInterface::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	(void)attrMods;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_DBO_BrfcInterface::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	return ACS_CC_SUCCESS;
}

void AES_DBO_BrfcInterface::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
}

void AES_DBO_BrfcInterface::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
}

ACS_CC_ReturnType AES_DBO_BrfcInterface::updateRuntime(const char* p_objName, const char** p_attrName)
{
	(void)p_objName;
	(void)p_attrName;
	return ACS_CC_SUCCESS;
}

int AES_DBO_BrfcInterface::handle_input(ACE_HANDLE fd)
{
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

int AES_DBO_BrfcInterface::ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode)
{

	GCC_TTRACE((aes_dbo_brfcthread,"Entering ResponseToBrfc with requestId:%d, brfStatus:%d, responseCode:%d", requestId, brfStatus, responseCode));

	acs_apgcc_adminoperation admOp;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;

	if( responseCode  == BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT )
	{
		GCC_TDEBUG((aes_dbo_brfcthread, "Response code = BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT"));
		ACS_APGCC_AdminOperationParamType thirdElem;

		//create first Element of parameter list
		char attName1[]= "requestId";
		firstElem.attrName =attName1;
		firstElem.attrType=ATTR_UINT64T;
		// return requestId previously checked
		unsigned long long intValue1 = requestId;
		firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

		// create second Element of parameter list
		char attName2[]= "resultCode";
		secondElem.attrName =attName2;
		secondElem.attrType=ATTR_INT32T ;
		int intValue2 = brfStatus; // BRF_SUCCESS
		secondElem.attrValues=reinterpret_cast<void*>(&intValue2);

		//create the third Element of parameter list
		char attName3[]= "message";
		thirdElem.attrName =attName3;
		thirdElem.attrType=ATTR_STRINGT ;
		char* strValue = const_cast<char*>("");
		void* valueStr[1]={ reinterpret_cast<void*>(strValue) };
		thirdElem.attrValues=valueStr;

		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);
		vectorIN.push_back(thirdElem);
	}
	else if (responseCode == BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS)
	{
		//create first Element of parameter list
		char attName1[]= "requestId";
		firstElem.attrName =attName1;
		firstElem.attrType=ATTR_UINT64T;
		unsigned long long intValue1 = requestId;
		firstElem.attrValues=reinterpret_cast<void*>(&intValue1);

		// create second Element of parameter list
		char attName3[]= "progressCount";
		secondElem.attrName =attName3;
		secondElem.attrType=ATTR_UINT32T ;
		int intValue3 = 50; // 50 %
		secondElem.attrValues=reinterpret_cast<void*>(&intValue3);

		//insert into the vector
		vectorIN.push_back(firstElem);
		vectorIN.push_back(secondElem);
	}

	const char *dnObjName1 = BRFC_PARENT_RDN;
	long long int timeOutVal_30sec = 30*(1000000000LL);

	int res1 = admOp.init();

	if (res1 != ACS_CC_SUCCESS)
	{
		GCC_TERROR((aes_dbo_brfcthread,"ERROR ::admOp.init()FAILED"));
		GCC_TERROR((aes_dbo_brfcthread,"ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText()));
		GCC_TERROR((aes_dbo_brfcthread,"Leaving ResponseToBrfc - failure "));
		return -1;
	}

	int returnValue1 = 1;
	int retry = 0;
	/* Invoke operation  */
	while( admOp.adminOperationInvoke(dnObjName1, 0 , responseCode , vectorIN , &returnValue1, timeOutVal_30sec) == ACS_CC_FAILURE )
	{
		GCC_TERROR((aes_dbo_brfcthread,"%s", "ERROR ::admOp.adminOperationInvoke()FAILED"));
		GCC_TERROR((aes_dbo_brfcthread,"ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText()));
		if (admOp.getInternalLastError()!=((-1)*SA_AIS_ERR_EXIST))
		{
			break;
		}
		sleep(1);
		if (++retry > 60)
		{
			break;
		}

	}

	res1 = admOp.finalize();

	if (res1 != ACS_CC_SUCCESS)
	{
		GCC_TERROR((aes_dbo_brfcthread,"ERROR ::admOp.finalize()FAILED"));
		GCC_TERROR((aes_dbo_brfcthread,"ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText()));
		GCC_TERROR((aes_dbo_brfcthread,"Leaving ResponseToBrfc - failure"));
		return -1;
	}
	GCC_TTRACE((aes_dbo_brfcthread,"Leaving ResponseToBrfc - success"));
	return 0;
}

bool AES_DBO_BrfcInterface::getBrfStatus()
{
	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(brfcMX_, true);
	return isBrfInProgress_;
}

void AES_DBO_BrfcInterface::setBrfStatus(bool status)
{

	ACE_Guard<ACE_Recursive_Thread_Mutex> guard(brfcMX_, true);
	isBrfInProgress_=status;
}

