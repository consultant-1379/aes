#ifndef INC_STORE_STOREBASETEST_H_
#define INC_STORE_STOREBASETEST_H_

#include "cute_suite.h"

class StoreBaseTest
{
private:
	inline StoreBaseTest(){};
	virtual ~StoreBaseTest(){};
	static void getDataSourceNameTest();
	static void getProducerNameTest();
	static void getCheckPointNameTest();
	static void getNewCheckPointTest();
	static void addCheckPointSectionTest();
	static void getMessageIndexTest();
	static void getRecordNumberTest();
	static void writeDataTest();
	static void openCheckPointTest();
	static void closeCheckPointTest();
	static void isCheckPointFullSuccessTest();
	static void isCheckPointFullFailureTest();
	static void convertInfoBufferToDataSuccessTest();
	static void convertInfoBufferToDataFailureTest();
	static void createInfoSectionSuccessTest();
	static void createInfoSectionFailureTest();
	static void updateInfoSectionSuccessTest();
	static void updateInfoSectionFailureTest();
public:
	static cute::suite make_suite_storeBaseTest();
};

#endif //INC_STORE_STOREBASETEST_H_
