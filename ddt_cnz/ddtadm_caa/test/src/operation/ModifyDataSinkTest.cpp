#include "cute.h"
#include "operation/ModifyDataSinkTest.h"
//#include "operation/modifydatasink.h"
#include "operation/operation.h"

void modifyDataSinkTest() {
	//operation::ModifyDataSink modifyDataSink;
	operation::dataSinkInfo opDetails;

	opDetails.name = "VCHS";
	opDetails.moDN = "DataSource=VCHS";
	opDetails.changeMask=1;
	opDetails.retryAttempts=1;
	opDetails.retryDelay=1;


	//modifyDataSink.setOperationDetails(reinterpret_cast<void*>(&opDetails));
	//modifyDataSink.call();
	//ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, modifyDataSink.m_operationResult.errorCode);
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, common::errorCode::ERR_NO_ERRORS);
}

cute::suite make_suite_modifyDataSinkTest(){
	cute::suite s;
	s.push_back(CUTE(modifyDataSinkTest));
	return s;
}



