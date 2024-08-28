#include "cute.h"
#include "operation/ModifyDataSourceTest.h"
#include "operation/modifydatasource.h"
#include "operation/operation.h"

void modifyDataSourceTest() {
	operation::ModifyDataSource modifyDataSource;
	operation::dataSourceInfo opDetails;

	opDetails.name = "VCHS";
	opDetails.moDN = "DataSource=VCHS";
	opDetails.recordSize = 2048;
	opDetails.alarmLevel = 1U;

	modifyDataSource.setOperationDetails(reinterpret_cast<void*>(&opDetails));
	modifyDataSource.call();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, modifyDataSource.m_operationResult.errorCode);
}

cute::suite make_suite_modifyDataSourceTest(){
	cute::suite s;
	s.push_back(CUTE(modifyDataSourceTest));
	return s;
}



