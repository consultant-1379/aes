#include "cute_suite.h"
#include "engine/datasourcemanager.h"
class DataSourceManagerTest
{
public:
	static engine::DataSourceManager dataSourceManager;
	static void dataSourceAddSuccessTest();
	static void dataSourceAddFailureTest();
	static void dataSourceRemoveTest();
	static void dataSourceModifyTest();
	static void stopTest();
//	static void loadFromImmTest();
//	static void addFromDNTest();

	static cute::suite make_suite_dataSourceManagerTest();
};

