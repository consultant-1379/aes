#include "cute.h"
#include "engine/DataSourceManagerTest.h"

engine::DataSourceManager DataSourceManagerTest::dataSourceManager;

void DataSourceManagerTest::dataSourceAddSuccessTest() {

	operation::dataSourceInfo data;
	operation::result op_result;
	data.name.assign("VCHS");
	int result = dataSourceManager.add(data,op_result);
	ASSERT_EQUAL(result , common::errorCode::ERR_NO_ERRORS);
}
void DataSourceManagerTest::dataSourceAddFailureTest() {

	operation::dataSourceInfo data;
	operation::result op_result;
	data.name.assign("VCHS");
	int result = dataSourceManager.add(data,op_result);
	ASSERT_EQUAL(result , common::errorCode::ERR_DATASOURCE_ALREADY_DEFINED);
}
void DataSourceManagerTest::dataSourceRemoveTest() {

	operation::dataSourceInfo data;
	operation::result op_result;
	data.name.assign("VCHS");
	int result = dataSourceManager.remove(data,op_result);
	ASSERT_EQUAL(result , common::errorCode::ERR_NO_ERRORS);
}
void DataSourceManagerTest::dataSourceModifyTest() {

	operation::dataSourceInfo data;
	operation::result op_result;
	data.name.assign("VCHS");
	int result = dataSourceManager.modify(data,op_result);
	ASSERT_EQUAL(result , common::errorCode::ERR_NO_ERRORS);
}
void DataSourceManagerTest::stopTest(){
	operation::dataSourceInfo data1;
	operation::result op_result1;
	operation::dataSourceInfo data2;
	operation::result op_result2;

	data1.name.assign("VCHS0");
	dataSourceManager.add(data1,op_result1);

	data2.name.assign("VCHS1");
	dataSourceManager.add(data2,op_result2);

	int result = dataSourceManager.stop();
	ASSERT_EQUAL(result , common::errorCode::ERR_NO_ERRORS);
}


cute::suite DataSourceManagerTest::make_suite_dataSourceManagerTest(){
	cute::suite s;
	s.push_back(CUTE(DataSourceManagerTest::dataSourceAddSuccessTest));
	s.push_back(CUTE(DataSourceManagerTest::dataSourceAddFailureTest));
	s.push_back(CUTE(DataSourceManagerTest::dataSourceRemoveTest));
	s.push_back(CUTE(DataSourceManagerTest::dataSourceModifyTest));
	s.push_back(CUTE(DataSourceManagerTest::stopTest));
	return s;
}



