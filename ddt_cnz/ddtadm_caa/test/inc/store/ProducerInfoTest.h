#ifndef INC_STORE_PRODUCERINFOTEST_H_
#define INC_STORE_PRODUCERINFOTEST_H_

#include "cute_suite.h"

class ProducerInfoTest
{
private:
	inline ProducerInfoTest(){};
	virtual ~ProducerInfoTest(){};
	static void addChkPointTest();
	static void removeChkPointTest();
	static void getLastChkPointTest();
	static void emptyChkPointTest();

public:
	static cute::suite make_suite_producerInfoTest();
};

#endif //INC_STORE_PRODUCERINFOTEST_H_
