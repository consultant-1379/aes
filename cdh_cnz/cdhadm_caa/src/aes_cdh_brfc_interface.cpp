//******************************************************************************
//
//  NAME
//     acs_cdh_brfc_interface.cpp
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
#include <aes_cdh_brfc_interface.h>
#include <ACS_APGCC_Util.H>
#include <servr.h>
#include <iostream>
#include <sys/eventfd.h>
#include "aes_cdh_logger.h"
#include "aes_cdh_tracer.h"

using namespace std;

AES_CDH_TRACE_DEFINE(AES_CDH_BrfImplementer);


bool AES_CDH_BrfImplementer::isBrfInProgress_ = false;
ACE_Recursive_Thread_Mutex AES_CDH_BrfImplementer::brfcMX_;

/*===================================================================
     ROUTINE: AES_CDH_BrfImplementer
 =================================================================== */
AES_CDH_BrfImplementer::AES_CDH_BrfImplementer()
{
}

/*===================================================================
     ROUTINE: AES_CDH_BrfImplementer
 =================================================================== */
AES_CDH_BrfImplementer::AES_CDH_BrfImplementer(string p_impName )
:acs_apgcc_objectimplementerinterface_V3(p_impName)
{
}


/*===================================================================
     ROUTINE: AES_CDH_BrfImplementer
 =================================================================== */
AES_CDH_BrfImplementer::AES_CDH_BrfImplementer(string p_objName,
		string p_impName,
		ACS_APGCC_ScopeT p_scope ):
		acs_apgcc_objectimplementerinterface_V3(p_objName,
				p_impName,
				p_scope)
{
}


/*===================================================================
     ROUTINE: ~AES_CDH_BrfImplementer
 =================================================================== */
AES_CDH_BrfImplementer::~AES_CDH_BrfImplementer()
{
}


/*===================================================================
     ROUTINE: adminOperationCallback
 =================================================================== */
void AES_CDH_BrfImplementer::adminOperationCallback(ACS_APGCC_OiHandle oiHandle,
		ACS_APGCC_InvocationType invocation,
		const char* p_objName,
		ACS_APGCC_AdminOperationIdType operationId,
		ACS_APGCC_AdminOperationParamType** paramList)
{
	AES_CDH_TRACE_MESSAGE("                    adminOperationCallback called             ");
	AES_CDH_LOG(LOG_LEVEL_INFO,"adminOperationCallback invoked");

	switch (operationId)
	{
	case   BRF_PARTICIPANT__PERMIT_BACKUP:
		AES_CDH_LOG(LOG_LEVEL_INFO,"RECV: BRF_PARTICIPANT_PERMIT_BACKUP");
		//A check needs to be added for permission from service.
		if( ServR::isBackUpPermitted() )
		{
			AES_CDH_BrfImplementer::setBrfStatus( true );
		}
		else
		{
			AES_CDH_BrfImplementer::setBrfStatus( false );
		}
		break;

	case BRF_PARTICIPANT__COMMIT_BACKUP:
		AES_CDH_LOG(LOG_LEVEL_INFO,"RECV: BRF_PARTICIPANT_COMMIT_BACKUP");
		AES_CDH_BrfImplementer::setBrfStatus( false );
		break;

	case BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP:
		AES_CDH_LOG(LOG_LEVEL_INFO,"RECV: BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP ");
		break;

	case BRF_PARTICIPANT__CANCEL_BACKUP:
		AES_CDH_LOG(LOG_LEVEL_INFO,"RECV: BRF_PARTICIPANT_CANCEL_BACKUP");
		AES_CDH_BrfImplementer::setBrfStatus( false );
		break;

	default:
		AES_CDH_TRACE_MESSAGE("Invalid operation Id recieved.");
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
				AES_CDH_TRACE_MESSAGE("requestId %d assigned ", requestId);
			}
			break;

		default:
			break;
		}
		i++;
	}

	AES_CDH_TRACE_MESSAGE(" Received Operation with Identification");

	//Make the return value false, depending upon state of service.
	int retVal = 1;

	//If operation id corresponds to permit backup request, then return failure if backup operation is not permitted.

	if( operationId == BRF_PARTICIPANT__PERMIT_BACKUP )
	{
		retVal = AES_CDH_BrfImplementer::getBrfStatus();
	}


	int retAdminOperationResult = this->adminOperationResult( oiHandle , invocation, retVal );
	AES_CDH_TRACE_MESSAGE("adminOperationResult retVal = %d, returned = %d", retVal, retAdminOperationResult);

	// HERE YOU CAN RESPONDE WITH ACTION RESULT OR ACTION IN PROGRESS
	// BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS simulation
	if( operationId == BRF_PERSISTENT_DATA_OWNER__PREPARE_BACKUP )
	{

		retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_PROGRESS);
		sleep(2);

		retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}
	else
	{
		retVal = this->ResponseToBrfc(requestId, BRF_SUCCESS, BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT);
	}

} /* end method */


ACS_CC_ReturnType AES_CDH_BrfImplementer::create(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *className, const char* parentName, ACS_APGCC_AttrValues **attr)
{
	(void)oiHandle;
	(void)ccbId;
	(void)className;
	(void)parentName;
	(void)attr;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_CDH_BrfImplementer::deleted(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_CDH_BrfImplementer::modify(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId, const char *objName, ACS_APGCC_AttrModification **attrMods)
{
	(void)oiHandle;
	(void)ccbId;
	(void)objName;
	(void)attrMods;
	return ACS_CC_SUCCESS;
}

ACS_CC_ReturnType AES_CDH_BrfImplementer::complete(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
	return ACS_CC_SUCCESS;
}

void AES_CDH_BrfImplementer::abort(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
}

void AES_CDH_BrfImplementer::apply(ACS_APGCC_OiHandle oiHandle, ACS_APGCC_CcbId ccbId)
{
	(void)oiHandle;
	(void)ccbId;
}

ACS_CC_ReturnType AES_CDH_BrfImplementer::updateRuntime(const char* p_objName, const char** p_attrName)
{
	(void)p_objName;
	(void)p_attrName;
	return ACS_CC_SUCCESS;
}

int AES_CDH_BrfImplementer::ResponseToBrfc(unsigned long long requestId,int brfStatus,int responseCode)
{
	AES_CDH_TRACE_MESSAGE("Entering ResponseToBrfc with requestId:%d, brfStatus:%d, responseCode:%d", requestId, brfStatus, responseCode);

	acs_apgcc_adminoperation admOp;
	std::vector<ACS_APGCC_AdminOperationParamType> vectorIN;
	ACS_APGCC_AdminOperationParamType firstElem;
	ACS_APGCC_AdminOperationParamType secondElem;

	if( responseCode  == BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT )
	{
		AES_CDH_TRACE_MESSAGE("Response code = BRF_PARTICIPANT_CONTAINER__REPORT_ACTION_RESULT");
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
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR ::admOp.init()FAILED ERROR CODE = %d, ERROR MESSAGE = %s",admOp.getInternalLastError(), admOp.getInternalLastErrorText());
		return -1;
	}

	int returnValue1 = 1;
	int retry = 0;
	/* Invoke operation  */
	while( admOp.adminOperationInvoke(dnObjName1, 0 , responseCode , vectorIN , &returnValue1, timeOutVal_30sec) == ACS_CC_FAILURE )
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR ::admOp.adminOperationInvoke()FAILED");
		AES_CDH_LOG(LOG_LEVEL_ERROR,"ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText());
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
		AES_CDH_LOG(LOG_LEVEL_ERROR, "ERROR ::admOp.finalize()FAILED. ERROR CODE = %d, ERROR MESSAGE = %s", admOp.getInternalLastError(), admOp.getInternalLastErrorText());
	    return -1;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return 0;
}

bool AES_CDH_BrfImplementer::getBrfStatus()
{
	AES_CDH_BrfImplementer::brfcMX_.acquire();
	bool brfcStatus = isBrfInProgress_;
	AES_CDH_BrfImplementer::brfcMX_.release();
	return brfcStatus;
}

void AES_CDH_BrfImplementer::setBrfStatus(bool status)
{
	AES_CDH_BrfImplementer::brfcMX_.acquire();
	isBrfInProgress_=status;
	AES_CDH_BrfImplementer::brfcMX_.release();
}
/*=================================================================
        ROUTINE: AES_CDH_BrfcThread
=================================================================== */
AES_CDH_BrfcThread::AES_CDH_BrfcThread(ACE_Thread_Manager* thr_mgr):ACE_Task_Base(thr_mgr)
{
	theBrfImplementer = NULL;
	//theIsStop = false;
	m_isObjImplAdded = false;
	unsigned int initval = 0U;
	m_StopEvent = eventfd(initval,0);
}//end of AES_CDH_BrfcThread

/*=================================================================
        ROUTINE: AES_CDH_BrfcThread
=================================================================== */
AES_CDH_BrfcThread::AES_CDH_BrfcThread(ACE_Thread_Manager *thr_mgr,AES_CDH_BrfImplementer *pImpl):ACE_Task_Base(thr_mgr)
{
	theBrfImplementer = pImpl;
	//theIsStop = false;
}//end of AES_CDH_BrfcThread

/*=================================================================
        ROUTINE: AES_CDH_BrfcThread destructor
=================================================================== */
AES_CDH_BrfcThread::~AES_CDH_BrfcThread()
{
	// Close the stop event, usually inside the object destructor
	ACE_OS::close(m_StopEvent);

}//end of destructor
/*=================================================================
        ROUTINE: setImpl
=================================================================== */
void AES_CDH_BrfcThread::setImpl(AES_CDH_BrfImplementer *pImpl)
{
	AES_CDH_TRACE_MESSAGE("%s", "Entering");

	const char *dnObjName = BRFC_CDH_OBJ_DNNAME;
	const char *impName = BRFC_CDH_IMPL;
	ACS_APGCC_ScopeT scope = ACS_APGCC_ONE;
	theBrfImplementer = pImpl;
	theBrfImplementer->setObjName(dnObjName);
	theBrfImplementer->setImpName(impName);
	theBrfImplementer->setScope(scope);

	for( int i=0;i < 10; i++)
	{
		if(oiHandler.addObjectImpl(theBrfImplementer)==ACS_CC_FAILURE)
		{
			//Get the error code
			for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
			if(i == 9)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while adding object implementer for %s",dnObjName);
				break;
			}
			else
				continue;
		}
		else
		{
			m_isObjImplAdded = true;
			break;
		}
	}

	//theIsStop = false;

	AES_CDH_TRACE_MESSAGE(" Leaving");
}//end of setImpl

void AES_CDH_BrfcThread::removeImpl(AES_CDH_BrfImplementer *pImpl)
{
	AES_CDH_TRACE_MESSAGE( "Entering");
	if(m_isObjImplAdded)
	{
		if(pImpl !=0)
		{
			//theBrfImplementer = pImpl;
			const char *dnObjName = BRFC_CDH_OBJ_DNNAME;
			for( int i=0;i < 10; i++)
			{
				if(oiHandler.removeObjectImpl(theBrfImplementer)==ACS_CC_FAILURE)
				{
					//Get the error code
					for(int j=0; j< 100; ++j) ; //do nothing loop to wait for sometime, better than sleeping
					if(i == 9)
					{
						AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occured while removing object implementer for %s ",dnObjName);
						break;
					}
					else
						continue;
				}
				else
				{
					m_isObjImplAdded = false;
					break;
				}
			}

		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
}
/*=================================================================
ROUTINE: stop
=================================================================== */
bool AES_CDH_BrfcThread::stop()
{
	AES_CDH_TRACE_MESSAGE("Entering");
	//theIsStop=true;
	// Signal the Stop event
	ACE_UINT64 stopEvent=1;

	// Signal to server to stop
	ssize_t	numByte = ACE_OS::write(m_StopEvent, &stopEvent, sizeof(ACE_UINT64));

	if(sizeof(ACE_UINT64) != numByte)
	{
		AES_CDH_LOG(LOG_LEVEL_ERROR, "Error occurred while signaling stop event.Leaving AES_CDH_BrfcThread::stop...");
		return false;
	}
	AES_CDH_TRACE_MESSAGE("Leaving");
	return true;

}//end of stop

/*=================================================================
ROUTINE: svc
=================================================================== */
int AES_CDH_BrfcThread::svc(void)
{
	AES_CDH_TRACE_MESSAGE("Entering");
	// Create a fd to wait for request
	const nfds_t nfds = 2;
	struct pollfd fds[nfds];

	// Initialize the pollfd structure
	ACE_OS::memset(fds, 0 , sizeof(fds));

	fds[0].fd = m_StopEvent;
	fds[0].events = POLLIN;

	fds[1].fd = theBrfImplementer->getSelObj();
	fds[1].events = POLLIN;

	ACE_INT32 poolResult;
	ACS_CC_ReturnType result;

	// waiting for IMM requests or stop
	while(true)
	{
		poolResult = ACE_OS::poll(fds, nfds);

		if( 0 == poolResult )
		{
			if(errno == EINTR)
			{
				continue;
			}
			AES_CDH_LOG(LOG_LEVEL_ERROR, "Leaving , exit after poll error");
			break;
		}

		if(fds[0].revents & POLLIN)
		{
			// Received a stop request from server
			AES_CDH_LOG(LOG_LEVEL_INFO, " Leaving , received a stop request from server");
			break;
		}

		if(fds[1].revents & POLLIN)
		{
			// Received a IMM request
			result = theBrfImplementer->dispatch(ACS_APGCC_DISPATCH_ALL);

			if(ACS_CC_SUCCESS != result)
			{
				AES_CDH_LOG(LOG_LEVEL_ERROR, " Leaving , error on BrfImplementer dispatch event");
			}
			continue;
		}
	}
	AES_CDH_TRACE_MESSAGE("Leaving..");
	return 0;
}//end of svc

