#ifndef INC_STORE_STATUSHANDLERTEST_H_
#define INC_STORE_STATUSHANDLERTEST_H_

#include "cute_suite.h"

class StatusHandlerTest
{
private:
	inline StatusHandlerTest(){};
	virtual ~StatusHandlerTest(){};
	static void getDataSourceNameTest();
	static void openStatusCheckpointTest();
	static void closeStatusCheckpointTest();
	static void addStoreNameTest();
	static void removeStoreNameSuccessTest();
	static void removeStoreNameFailureTest();
	static void getFirstStoreNameSuccessTest();
	static void getFirstStoreNameFailureTest();
	static void getLastStoreNameSuccessTest();
	static void getLastStoreNameFailureTest();
	static void getNumberOfStoresSuccessTest();
	static void getNumberOfStoresFailureTest();
	static void getNumberOfStoresOfProducerSuccessTest();
	static void getNumberOfStoresOfProducerFailureTest();
	static void getProducersSuccessTest();
	static void getProducersFailureTest();
	static void updateProducerSectionInfoSuccessTest();
	static void updateProducerSectionInfoFailureTest();
	static void createProducerSectionInfoSuccessTest();
	static void createProducerSectionInfoFailureTest();
	static void readStatusCheckpointSectionsTest();
	static void loadTest();
public:
	static cute::suite make_suite_statusHandlerTest();
};

#endif //INC_STORE_STATUSHANDLERTEST_H_
