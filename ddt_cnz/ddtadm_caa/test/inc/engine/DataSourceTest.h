#include "cute_suite.h"

class DataSourceTest
{
public:
	DataSourceTest(){}
	~DataSourceTest(){}
	static cute::suite make_suite_dataSourceTest();
	static void datasource_start_success();
	static void datasource_start_failure();
	static void datasource_open_success();
	static void datasource_open_failure();
	static void datasource_stop_success_1();
	static void datasource_stop_success_2();
	static void datasource_svc_success();
/*	static void datasource_setupmtap_success();
	static void datasource_registerdsdhandles_success();*/
};

