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

#include "store/StatusHandlerTest.h"
#include "store/checkpoint/statushandler.h"
#include "cute.h"
#include "iostream"
#include "common/utility.h"

void StatusHandlerTest::getDataSourceNameTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	const char* dataSourceName = testStatusHandler.getDataSourceName();
	std::cout << "\n DataSourceName = " << dataSourceName << std::endl;
	ASSERT_EQUAL(dataSourceName, "VCHS");
}

void StatusHandlerTest::openStatusCheckpointTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.openStatusCheckpoint();
	std::cout << "\n Result = " << common::utility::boolToString(result) << std::endl;
	ASSERT_EQUAL(true, result);
}

void StatusHandlerTest::closeStatusCheckpointTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	testStatusHandler.closeStatusCheckpoint();
	ASSERT_EQUAL(-1, testStatusHandler.m_checkpointHandle);
}

void StatusHandlerTest::addStoreNameTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	std::cout << "\n m_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	std::cout << "\n Result = " << common::utility::boolToString(result) << std::endl;
	std::cout << "m_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	ASSERT_EQUAL(1, testStatusHandler.m_producers.size());
}

void StatusHandlerTest::removeStoreNameSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439809277");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	result = testStatusHandler.removeStoreName("safCkpt=DDT__VCHS_BC5_1439807492", "BC5");
	ASSERT_EQUAL(true, result);
	ASSERT_EQUAL(1, testStatusHandler.m_producers.size());
}

void StatusHandlerTest::removeStoreNameFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC4", "safCkpt=DDT__VCHS_BC5_1439809277");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	result = testStatusHandler.removeStoreName("safCkpt=DDT__VCHS_BC5_1439807492", "BC2");
	ASSERT_EQUAL(false, result);
	ASSERT_EQUAL(2, testStatusHandler.m_producers.size());
}

void StatusHandlerTest::getFirstStoreNameSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439809277");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	std::string storeName;
	result = testStatusHandler.getFirstStoreName("BC5",storeName);
	ASSERT_EQUAL(true, result);
	std::cout << "\nstoreName = " << storeName << std::endl;
}

void StatusHandlerTest::getFirstStoreNameFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439809277");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	std::string storeName("No Store associated with this producer");
	result = testStatusHandler.getFirstStoreName("BC2",storeName);
	ASSERT_EQUAL(false, result);
	std::cout << "\nstoreName = " << storeName << std::endl;
}

void StatusHandlerTest::getLastStoreNameSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439809277");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	std::string storeName;
	result = testStatusHandler.getLastStoreName("BC5",storeName);
	ASSERT_EQUAL(true, result);
	std::cout << "\nstoreName = " << storeName << std::endl;
}

void StatusHandlerTest::getLastStoreNameFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439809277");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	std::string storeName("No Store associated with this producer");
	result = testStatusHandler.getLastStoreName("BC2",storeName);
	ASSERT_EQUAL(false, result);
	std::cout << "\nstoreName = " << storeName << std::endl;
}

void StatusHandlerTest::getNumberOfStoresSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439809277");
	result = testStatusHandler.addStoreName("BC1", "safCkpt=DDT__VCHS_BC5_1439801111");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	int numOfStores = testStatusHandler.getNumberOfStores();
	ASSERT_EQUAL(3, numOfStores);
	std::cout << "\nnumOfStores = " << numOfStores << std::endl;
}

void StatusHandlerTest::getNumberOfStoresFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	int numOfStores = testStatusHandler.getNumberOfStores();
	ASSERT_EQUAL(0, numOfStores);
	std::cout << "\nnumOfStores = " << numOfStores << std::endl;
}

void StatusHandlerTest::getNumberOfStoresOfProducerSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439809277");
	result = testStatusHandler.addStoreName("BC1", "safCkpt=DDT__VCHS_BC5_1439801111");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	int numOfStores = testStatusHandler.getNumberOfStores("BC5");
	ASSERT_EQUAL(2, numOfStores);
	std::cout << "\nnumOfStores = " << numOfStores << std::endl;
}

void StatusHandlerTest::getNumberOfStoresOfProducerFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	int numOfStores = testStatusHandler.getNumberOfStores("BC5");
	ASSERT_EQUAL(0, numOfStores);
	std::cout << "\nnumOfStores = " << numOfStores << std::endl;
}

void StatusHandlerTest::getProducersSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	result = testStatusHandler.addStoreName("BC4", "safCkpt=DDT__VCHS_BC5_1439809277");
	result = testStatusHandler.addStoreName("BC1", "safCkpt=DDT__VCHS_BC5_1439801111");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	std::list<std::string> producerList;
	testStatusHandler.getProducers(producerList);
	ASSERT_EQUAL(3, producerList.size());
}

void StatusHandlerTest::getProducersFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	std::list<std::string> producerList;
	testStatusHandler.getProducers(producerList);
	ASSERT_EQUAL(0, producerList.size());
}

void StatusHandlerTest::updateProducerSectionInfoSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	store::checkpoint::StatusHandler::mapOfProducer_t::const_iterator element = testStatusHandler.m_producers.find("BC5");
	result = testStatusHandler.updateProducerSectionInfo(element->second);
	ASSERT_EQUAL(true, result);
}

void StatusHandlerTest::updateProducerSectionInfoFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("INVALID", "safCkpt=DDT__VCHS_INVALID_1439807492");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	store::checkpoint::StatusHandler::mapOfProducer_t::const_iterator element = testStatusHandler.m_producers.find("INVALID");
	result = testStatusHandler.updateProducerSectionInfo(element->second);
	ASSERT_EQUAL(false, result);
}

void StatusHandlerTest::createProducerSectionInfoSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("BC5", "safCkpt=DDT__VCHS_BC5_1439807492");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	store::checkpoint::StatusHandler::mapOfProducer_t::const_iterator element = testStatusHandler.m_producers.find("BC5");
	result = testStatusHandler.createProducerSectionInfo(element->second);
	ASSERT_EQUAL(true, result);
}

void StatusHandlerTest::createProducerSectionInfoFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	bool result = testStatusHandler.addStoreName("INVALID", "safCkpt=DDT__VCHS_INVALID_1439807492");
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	store::checkpoint::StatusHandler::mapOfProducer_t::const_iterator element = testStatusHandler.m_producers.find("INVALID");
	result = testStatusHandler.createProducerSectionInfo(element->second);
	ASSERT_EQUAL(false, result);
}

void StatusHandlerTest::readStatusCheckpointSectionsTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	bool result = testStatusHandler.readStatusCheckpointSections();
	ASSERT_EQUAL(true, result);
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
}

void StatusHandlerTest::loadTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::StatusHandler testStatusHandler("VCHS", cktHandle);
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
	bool result = testStatusHandler.load();
	ASSERT_EQUAL(true, result);
	std::cout << "\nm_producers.size = " << testStatusHandler.m_producers.size() << std::endl;
}

cute::suite StatusHandlerTest::make_suite_statusHandlerTest(){
	cute::suite s;
	s.push_back(CUTE(getDataSourceNameTest));
	s.push_back(CUTE(openStatusCheckpointTest));
	s.push_back(CUTE(closeStatusCheckpointTest));
	s.push_back(CUTE(addStoreNameTest));
	s.push_back(CUTE(removeStoreNameSuccessTest));
	s.push_back(CUTE(removeStoreNameFailureTest));
	s.push_back(CUTE(getFirstStoreNameSuccessTest));
	s.push_back(CUTE(getFirstStoreNameFailureTest));
	s.push_back(CUTE(getLastStoreNameSuccessTest));
	s.push_back(CUTE(getLastStoreNameFailureTest));
	s.push_back(CUTE(getNumberOfStoresSuccessTest));
	s.push_back(CUTE(getNumberOfStoresFailureTest));
	s.push_back(CUTE(getNumberOfStoresOfProducerSuccessTest));
	s.push_back(CUTE(getNumberOfStoresOfProducerFailureTest));
	s.push_back(CUTE(getProducersSuccessTest));
	s.push_back(CUTE(getProducersFailureTest));
	s.push_back(CUTE(createProducerSectionInfoSuccessTest));
	s.push_back(CUTE(createProducerSectionInfoFailureTest));
	s.push_back(CUTE(readStatusCheckpointSectionsTest));
	s.push_back(CUTE(loadTest));
	s.push_back(CUTE(updateProducerSectionInfoSuccessTest));
	s.push_back(CUTE(updateProducerSectionInfoFailureTest));
	return s;
}
