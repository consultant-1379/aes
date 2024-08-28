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

#include "store/ProducerInfoTest.h"
#include "store/checkpoint/producerinfo.h"
#include "cute.h"
#include "iostream"

void ProducerInfoTest::addChkPointTest() {
	store::checkpoint::ProducerInfo producer("BC0");
	producer.addCheckPoint("VCHS_BC0_DDT1");
	producer.addCheckPoint("VCHS_BC0_DDT2");
	std::string chkPointName;
	ASSERT_EQUAL(true, producer.getFirstCheckPoint(chkPointName));
	std::cout << "\nCheckpoint Name = " << chkPointName << std::endl;
	ASSERT_EQUAL("VCHS_BC0_DDT1", chkPointName);
}

void ProducerInfoTest::removeChkPointTest() {
	store::checkpoint::ProducerInfo producer("BC0");
	producer.addCheckPoint("VCHS_BC0_DDT1");
	producer.addCheckPoint("VCHS_BC0_DDT2");
	std::string chkPointName("VCHS_BC0_DDT1");
	producer.removeCheckPoint(chkPointName);
	ASSERT_EQUAL(true, producer.getFirstCheckPoint(chkPointName));
	std::cout << "\nCheckpoint Name = " << chkPointName << std::endl;
	ASSERT_EQUAL("VCHS_BC0_DDT2", chkPointName);
}

void ProducerInfoTest::getLastChkPointTest() {
	store::checkpoint::ProducerInfo producer("BC0");
	producer.addCheckPoint("VCHS_BC0_DDT1");
	producer.addCheckPoint("VCHS_BC0_DDT2");
	std::string chkPointName;
	ASSERT_EQUAL(true, producer.getLastCheckPoint(chkPointName));
	ASSERT_EQUAL("VCHS_BC0_DDT2", chkPointName);
	chkPointName.assign("VCHS_BC0_DDT2");
	producer.removeCheckPoint(chkPointName);
	ASSERT_EQUAL(true, producer.getLastCheckPoint(chkPointName));
	std::cout << "\nCheckpoint Name = " << chkPointName << std::endl;
	ASSERT_EQUAL("VCHS_BC0_DDT1", chkPointName);
}

void ProducerInfoTest::emptyChkPointTest() {
	store::checkpoint::ProducerInfo producer("BC0");
	std::string chkPointName;
	ASSERT_EQUAL(false, producer.getLastCheckPoint(chkPointName));
	ASSERT_EQUAL(false, producer.getFirstCheckPoint(chkPointName));
}

cute::suite ProducerInfoTest::make_suite_producerInfoTest(){
	cute::suite s;
	s.push_back(CUTE(addChkPointTest));
	s.push_back(CUTE(removeChkPointTest));
	s.push_back(CUTE(getLastChkPointTest));
	s.push_back(CUTE(emptyChkPointTest));
	return s;
}
