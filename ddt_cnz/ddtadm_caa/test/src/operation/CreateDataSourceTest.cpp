#include "cute.h"
#include "operation/CreateDataSourceTest.h"
#include "operation/createdatasource.h"

void createDataSourceTest() {
	operation::CreateDataSource createDataSource;
	operation::dataSourceInfo opDetails;

	opDetails.name = "VCHS";
	opDetails.moDN = "DataSource=VCHS";
	opDetails.recordSize = 2048;
	opDetails.alarmLevel = 1U;

	createDataSource.setOperationDetails(reinterpret_cast<void*>(&opDetails));
	createDataSource.call();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, createDataSource.m_operationResult.errorCode);
}

cute::suite make_suite_createDataSourceTest(){
	cute::suite s;
	s.push_back(CUTE(createDataSourceTest));
	return s;
}



