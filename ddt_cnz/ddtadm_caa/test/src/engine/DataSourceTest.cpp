#include "cute.h"
#include "engine/DataSourceTest.h"
#include "engine/datasource.h"

void DataSourceTest::datasource_start_success()
{
	engine::DataSource dataSource("VCHS1", 2048,"dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferMId=1");
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS , dataSource.start());
}

void DataSourceTest::datasource_start_failure()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.setThrCount(1);
	ASSERT_EQUAL(common::errorCode::ERR_OPEN, dataSource.start());
}

void DataSourceTest::datasource_open_success()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.open());
}

void DataSourceTest::datasource_open_failure()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.setThrCount(1);
	ASSERT_EQUAL(common::errorCode::ERR_SVC_ACTIVATE,dataSource.open());
}

void DataSourceTest::datasource_stop_success_1()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.incrThrCount();
	dataSource.stop();
	dataSource.decrThrCount();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.thr_count());

}

void DataSourceTest::datasource_stop_success_2()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.stop();
	ASSERT_EQUAL(0,dataSource.thr_count());
}

void DataSourceTest::datasource_svc_success()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.start();
	dataSource.stop();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.simulateExecution());
}
/*void DataSourceTest::datasource_setupmtap_success()
{
	engine::DataSource dataSource("VCHS1", 2048);
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.setupMtapServer());
}
void DataSourceTest::datasource_registerdsdhandles_success()
{
	engine::DataSource dataSource("VCHS1", 2048);
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.registerDsdHandles());
}*/


cute::suite DataSourceTest::make_suite_dataSourceTest(){
	cute::suite s;
	s.push_back(CUTE(DataSourceTest::datasource_start_success));
	s.push_back(CUTE(DataSourceTest::datasource_start_failure));
	s.push_back(CUTE(DataSourceTest::datasource_open_success));
	s.push_back(CUTE(DataSourceTest::datasource_open_failure));
	s.push_back(CUTE(DataSourceTest::datasource_stop_success_1));
	s.push_back(CUTE(DataSourceTest::datasource_stop_success_2));
	s.push_back(CUTE(DataSourceTest::datasource_svc_success));
/*	s.push_back(CUTE(DataSourceTest::datasource_setupmtap_success));
	s.push_back(CUTE(DataSourceTest::datasource_registerdsdhandles_success))*/;
	return s;
}


