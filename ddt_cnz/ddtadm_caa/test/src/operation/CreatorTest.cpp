#include "cute.h"
#include "operation/CreatorTest.h"


void CreatorTest::scheduleStartRequest() {
	operation::Creator startDaemon;
	ASSERT_EQUAL(operation::START , startDaemon.schedule(operation::START));
}

void CreatorTest::scheduleStopRequest() {
	operation::Creator stopDaemon;
	ASSERT_EQUAL(ddt_unittest::CALL_SERVICE_STOP , stopDaemon.schedule(operation::STOP));
}

void CreatorTest::scheduleCreateDataSourceRequest() {
	operation::Creator createDataSource;
	ACE_Future<operation::result>* op_result = 0;
	operation::dataSourceInfo opDetails;
	opDetails.name = "VCHS0";
	opDetails.moDN = "dataSourceId=VCHS0";
	opDetails.changeMask=2;
	opDetails.recordSize = 2048;
	opDetails.alarmLevel = 1;
	ASSERT_EQUAL(ddt_unittest::CALL_DATASOURCE_ADD , createDataSource.schedule(operation::DATASOURCE_ADD,op_result,reinterpret_cast<void*>(&opDetails)));
}
void CreatorTest::scheduleModifyDataSourceRequest() {
	operation::Creator modifyDataSource;
	ACE_Future<operation::result>* op_result = 0;
	operation::dataSourceInfo opDetails;
	opDetails.name = "VCHS0";
	opDetails.moDN = "dataSourceId=VCHS0";
	opDetails.changeMask=2;
	opDetails.recordSize = 2048;
	opDetails.alarmLevel = 1;
	ASSERT_EQUAL(ddt_unittest::CALL_DATASOURCE_MOD , modifyDataSource.schedule(operation::DATASOURCE_MOD,op_result,reinterpret_cast<void*>(&opDetails)));
}
void CreatorTest::scheduleDeleteDataSourceRequest() {
	operation::Creator deleteDataSource;
	ACE_Future<operation::result>* op_result = 0;
	operation::dataSourceInfo opDetails;
	opDetails.name = "VCHS0";
	opDetails.moDN = "dataSourceId=VCHS0";
	opDetails.changeMask=2;
	opDetails.recordSize = 2048;
	opDetails.alarmLevel = 1;
	ASSERT_EQUAL(ddt_unittest::CALL_DATASOURCE_DEL ,  deleteDataSource.schedule(operation::DATASOURCE_DEL,op_result,reinterpret_cast<void*>(&opDetails)));
}
void CreatorTest::scheduleNoOpRequest() {
	operation::Creator noOp;
	ASSERT_EQUAL(ddt_unittest::CALL_NOOP,noOp.schedule(operation::NOOP));
}
cute::suite CreatorTest::make_suite_creatorTest(){
	cute::suite s;
	s.push_back(CUTE(scheduleStartRequest));
	s.push_back(CUTE(scheduleStopRequest));
	s.push_back(CUTE(scheduleCreateDataSourceRequest));
	s.push_back(CUTE(scheduleModifyDataSourceRequest));
	s.push_back(CUTE(scheduleDeleteDataSourceRequest));
	s.push_back(CUTE(scheduleNoOpRequest));
	return s;
}



