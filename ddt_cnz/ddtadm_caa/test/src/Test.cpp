//	********************************************************
//
//	 COPYRIGHT Ericsson 2015
//	All rights reserved.
//
//	The Copyright to the computer program(s) herein
//	is the property of Ericsson 2015.
//	The program(s) may be used and/or copied only with
//	the written permission from Ericsson 2015 or in
//	accordance with the terms and conditions stipulated in
//	the agreement/contract under which the program(s) have
//	been supplied.
//
//	********************************************************
#include "cute.h"
#include "ide_listener.h"
#include "xml_listener.h"
#include "cute_runner.h"

#include "operation/StopTest.h"
#include "operation/StartTest.h"
#include "operation/ShutdownTest.h"
#include "operation/CreateDataSourceTest.h"
#include "operation/ModifyDataSourceTest.h"
#include "operation/DeleteDataSourceTest.h"
#include "operation/SchedulerTest.h"
#include "operation/CreatorTest.h"
#include "imm/DataSourceOITest.h"
#include "engine/DataSourceTest.h"
#include "engine/DataSourceManagerTest.h"
#include "cipher/EncryptContextTest.h"
#include "mtap/PduDataTest.h"
#include "store/ProducerInfoTest.h"
#include "store/storeBaseTest.h"
#include "mtap/SlidingWindowTest.h"
#include "store/StatusHandlerTest.h"

#include "operation/CreateDataSinkTest.h"
#include "operation/ModifyDataSinkTest.h"
#include "operation/DeleteDataSinkTest.h"
#include "engine/DataSinkTest.h"
#include "imm/DataSinkOITest.h"
#include "imm/transferprogressOITest.h"


void runSuite(int argc, char const *argv[])
{
	cute::xml_file_opener xmlfile(argc,argv);
	cute::xml_listener<cute::ide_listener<>  > lis(xmlfile.out);

	{
		// Op Scheduler
		cute::suite s=make_suite_schedulerTest();
		cute::makeRunner(lis,argc,argv)(s, "Scheduler");
	}

	{
		// Start
		cute::suite s=make_suite_startTest();
		cute::makeRunner(lis,argc,argv)(s, "Start");
	}

	{
		// Stop
		cute::suite s=make_suite_stopTest();
		cute::makeRunner(lis,argc,argv)(s, "Stop");
	}

	{
		// Shutdown
		cute::suite s=make_suite_shutdownTest();
		cute::makeRunner(lis,argc,argv)(s, "Shutdown");
	}

	{
		//Create data source test
		cute::suite s=make_suite_createDataSourceTest();
		cute::makeRunner(lis,argc,argv)(s, "CreateDataSource");
	}

	{
		//Modify data source test
		cute::suite s=make_suite_modifyDataSourceTest();
		cute::makeRunner(lis,argc,argv)(s, "ModifyDataSource");
	}

	{
		//Delete data source test
		cute::suite s=make_suite_deleteDataSourceTest();
		cute::makeRunner(lis,argc,argv)(s, "DeleteDataSource");
	}

	{
		//Run datasource oi handler test suite
		cute::suite s = DataSourceOITest::make_suite_dataSourceOITest();
		cute::makeRunner(lis,argc,argv)(s, "DataSource_OI");
	}

	{
		//Run datasource test suite
		cute::suite s = DataSourceTest::make_suite_dataSourceTest();
		cute::makeRunner(lis,argc,argv)(s, "DataSource");
	}

	{
		//Create data sink test
		cute::suite s=make_suite_createDataSinkTest();
		cute::makeRunner(lis,argc,argv)(s, "CreateDataSink");
	}


	{
		//Modify data sink test
		cute::suite s=make_suite_modifyDataSinkTest();
		cute::makeRunner(lis,argc,argv)(s, "ModifyDataSink");
	}

	{
		//Delete data sink test
		cute::suite s=make_suite_deleteDataSinkTest();
		cute::makeRunner(lis,argc,argv)(s, "DeleteDataSink");
	}

	{
		//Run datasource test suite
		cute::suite s = DataSinkTest::make_suite_dataSinkTest();
		cute::makeRunner(lis,argc,argv)(s, "DataSink");
	}

	{
		//Run datasource oi handler test suite
		cute::suite s = DataSinkOITest::make_suite_dataSinkOITest();
		cute::makeRunner(lis,argc,argv)(s, "DataSink_OI");
	}

	{
		//Run datasource manager test suite
		cute::suite s = DataSourceManagerTest::make_suite_dataSourceManagerTest();
		cute::makeRunner(lis,argc,argv)(s, "DataSourceManager");
	}

	{
		//Run Creator suite
		cute::suite s = CreatorTest::make_suite_creatorTest();
		cute::makeRunner(lis,argc,argv)(s, "Creator");
	}

	{
		//Run TransferProgress_OI suite
		cute::suite s = TransferProgressOITest::make_suite_transferProgressOITest();
		cute::makeRunner(lis,argc,argv)(s, "TransferProgress_OI");
	}

	{
		//Run Encryption suite
		cute::suite s = make_suite_cipherHandlerTest();
		cute::makeRunner(lis,argc,argv)(s, "EvpCipherHandler");
	}

	{
		//Run pduHandlerTest suite
		cute::suite s = make_suite_pduHandlerTest();
		cute::makeRunner(lis,argc,argv)(s, "pduHandlerTest");
	}

	{
		//Run SlidingWindowTest suite
		cute::suite s = SlidingWindowTest::make_suite_SlidingWindowTest();
		cute::makeRunner(lis,argc,argv)(s, "SlidingWindowTest");
	}

	{
		//Run producerInfoTest suite
		cute::suite s = ProducerInfoTest::make_suite_producerInfoTest();
		cute::makeRunner(lis,argc,argv)(s, "producerInfoTest");
	}

	{
		//Run storeBaseTest suite
		cute::suite s = StoreBaseTest::make_suite_storeBaseTest();
		cute::makeRunner(lis,argc,argv)(s, "storeBaseTest");
	}

	{
		//Run StatusHandlerTest suite
		cute::suite s = StatusHandlerTest::make_suite_statusHandlerTest();
		cute::makeRunner(lis,argc,argv)(s, "StatusHandlerTest");
	}

}

int main(int argc, char const *argv[])
{
    runSuite(argc,argv);
}



