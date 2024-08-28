#include "cute_suite.h"
#include "operation/creator.h"

class CreatorTest
{
private:
	inline CreatorTest(){};
	virtual ~CreatorTest(){};
	static void scheduleStartRequest();
	static void scheduleStopRequest();
	static void scheduleCreateDataSourceRequest();
	static void scheduleModifyDataSourceRequest();
	static void scheduleDeleteDataSourceRequest();
	static void scheduleNoOpRequest();
public:
	static cute::suite make_suite_creatorTest();
};

