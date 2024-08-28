#include "operation/DeleteDataSourceTest.h"

#include "cute.h"
#include "operation/deletedatasource.h"
#include "operation/operation.h"

void deleteDataSourceTest() {
	operation::DeleteDataSource deleteDataSource;
	operation::dataSourceInfo opDetails;

	opDetails.name = "VCHS";
	opDetails.moDN = "DataSource=VCHS";
	opDetails.recordSize = 2048;
	opDetails.alarmLevel = 1U;

	deleteDataSource.setOperationDetails(reinterpret_cast<void*>(&opDetails));
	deleteDataSource.call();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, deleteDataSource.m_operationResult.errorCode);
}

cute::suite make_suite_deleteDataSourceTest(){
	cute::suite s;
	s.push_back(CUTE(deleteDataSourceTest));
	return s;
}



