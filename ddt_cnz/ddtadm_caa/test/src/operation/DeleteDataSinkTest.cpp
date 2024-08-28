#include "operation/DeleteDataSinkTest.h"

#include "cute.h"
#include "operation/deletedatasink.h"
#include "operation/operation.h"

void deleteDataSinkTest() {
	operation::DeleteDataSink deleteDataSink;
	operation::dataSinkInfo opDetails;

	opDetails.name = "VCHS";
	opDetails.moDN = "DataSource=VCHS";
	opDetails.changeMask=1;
	opDetails.retryAttempts=1;
	opDetails.retryDelay=1;


	deleteDataSink.setOperationDetails(reinterpret_cast<void*>(&opDetails));
	deleteDataSink.call();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, deleteDataSink.m_operationResult.errorCode);
}

cute::suite make_suite_deleteDataSinkTest(){
	cute::suite s;
	s.push_back(CUTE(deleteDataSinkTest));
	return s;
}



