#include "cute.h"
#include "engine/DataSinkTest.h"
#include "engine/datasink.h"

void DataSinkTest::datasink_start_success()
{
	operation::dataSinkInfo data;
	data.dataSourceName ="VCHS1";
	data.moDN="dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferMId=1";
	data.name="Samp";
	data.retryAttempts=1;
	data.retryDelay=1;
	engine::DataSink dataSink(data);
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS , dataSink.start());
}

void DataSinkTest::datasink_start_failure()
{
	operation::dataSinkInfo data;
		data.dataSourceName ="VCHS1";
		data.moDN="dataSourceId=VCHS,AxeDirectDataTransferdirectDataTransferMId=1";
		data.name="Samp";
		data.retryAttempts=1;
		data.retryDelay=1;
		engine::DataSink dataSink(data);

	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, dataSink.start());
}

/*void DataSinkTest::datasink_open_success()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.open());
}

void DataSinkTest::datasink_open_failure()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.setThrCount(1);
	ASSERT_EQUAL(common::errorCode::ERR_SVC_ACTIVATE,dataSource.open());
}

void DataSinkTest::datasource_stop_success_1()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.incrThrCount();
	dataSource.stop();
	dataSource.decrThrCount();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.thr_count());

}

void DataSinkTest::datasource_stop_success_2()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.stop();
	ASSERT_EQUAL(0,dataSource.thr_count());
}

void DataSinkTest::datasource_svc_success()
{
	engine::DataSource dataSource("VCHS1", 2048, "dataSourceId=vchs1,AxeDirectDataTransferdirectDataTransferMId=1");
	dataSource.start();
	dataSource.stop();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.simulateExecution());
}
void DataSinkTest::datasource_setupmtap_success()
{
	engine::DataSource dataSource("VCHS1", 2048);
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.setupMtapServer());
}
void DataSinkTest::datasource_registerdsdhandles_success()
{
	engine::DataSource dataSource("VCHS1", 2048);
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS,dataSource.registerDsdHandles());
}*/


cute::suite DataSinkTest::make_suite_dataSinkTest(){
	cute::suite s;
	s.push_back(CUTE(DataSinkTest::datasink_start_success));
	s.push_back(CUTE(DataSinkTest::datasink_start_failure));
/*	s.push_back(CUTE(DataSourceTest::datasource_open_success));
	s.push_back(CUTE(DataSourceTest::datasource_open_failure));
	s.push_back(CUTE(DataSourceTest::datasource_stop_success_1));
	s.push_back(CUTE(DataSourceTest::datasource_stop_success_2));
	s.push_back(CUTE(DataSourceTest::datasource_svc_success));
	s.push_back(CUTE(DataSourceTest::datasource_setupmtap_success));
	s.push_back(CUTE(DataSourceTest::datasource_registerdsdhandles_success))*/;
	return s;
}


