#include "cute.h"
#include "operation/CreateDataSinkTest.h"
#include "operation/createdatasink.h"

void createDataSinkTest() {
	operation::CreateDataSink createDataSink;
	operation::dataSinkInfo opDetails;

	opDetails.name = "VCHS";
	opDetails.moDN = "DataSource=VCHS";
	opDetails.retryAttempts=2;
	opDetails.retryDelay=1;
	opDetails.changeMask=1;


	createDataSink.setOperationDetails(reinterpret_cast<void*>(&opDetails));
	createDataSink.call();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, createDataSink.m_operationResult.errorCode);
}

cute::suite make_suite_createDataSinkTest(){
	cute::suite s;
	s.push_back(CUTE(createDataSinkTest));
	return s;
}



