#include "cute_suite.h"
#include "imm/datasink_oi.h"

class DataSinkOITest
{
	static void dataSinkAddSuccess();
	static void dataSinkAddAbort();
	static void dataSinkModifySuccess();
	static void dataSinkModifyAbort();
	static void dataSinkDeleteSuccess();
	static void dataSinkDeleteAbort();
	static imm::DataSink_OI  dataSinkImpl;
public:
	static cute::suite make_suite_dataSinkOITest();
};

