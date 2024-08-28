#include "cute_suite.h"
#include "imm/datasource_oi.h"

class DataSourceOITest
{
	static void dataSourceAddSuccess();
	static void dataSourceAddAbort();
	static void dataSourceModifySuccess();
	static void dataSourceModifyAbort();
	static void dataSourceDeleteSuccess();
	static void dataSourceDeleteAbort();
	static imm::DataSource_OI  dataSourceImpl;
public:
	static cute::suite make_suite_dataSourceOITest();
};

