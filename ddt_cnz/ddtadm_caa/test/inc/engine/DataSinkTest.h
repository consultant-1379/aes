#include "cute_suite.h"

class DataSinkTest
{
public:
	DataSinkTest(){}
	~DataSinkTest(){}
	static cute::suite make_suite_dataSinkTest();
	static void datasink_start_success();
	static void datasink_start_failure();
/*	static void datasink_open_success();
	static void datasink_open_failure();
	static void datasink_stop_success_1();
	static void datasink_stop_success_2();
	static void datasink_svc_success();
	static void datasource_setupmtap_success();
	static void datasource_registerdsdhandles_success();*/
};

