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

#include "store/storeBaseTest.h"
#include "store/checkpoint/writer.h"
#include "cute.h"
#include "iostream"
#include "stdint.h"
#include <boost/make_shared.hpp>
#include "common/utility.h"

void StoreBaseTest::getDataSourceNameTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	std::string dataSourceName = writer.getDataSourceName();
	std::cout << "\nDataSourceName = " << dataSourceName << std::endl;
	ASSERT_EQUAL("VCHS", dataSourceName.c_str());
}

void StoreBaseTest::getProducerNameTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	std::string producerName = writer.getProducerName();
	std::cout << "\nproducerName = " << producerName << std::endl;
	ASSERT_EQUAL("BC0", producerName.c_str());
}

void StoreBaseTest::getCheckPointNameTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	std::string chkptName = writer.getCheckPointName();
	std::cout << "\ncheckpointName = " << chkptName << std::endl;
	ASSERT_EQUAL("VCHS_BC0_DDT1", chkptName.c_str());
}

void StoreBaseTest::getNewCheckPointTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	int result = writer.getNewCheckpoint();
	std::cout << "\nResult = " << result << std::endl;
	std::cout << "CheckPointName = " << writer.m_checkpointName << std::endl;
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
	ASSERT_EQUAL("safCkpt=DDT__VCHS_BC0_1440492151", writer.m_checkpointName);
}

void StoreBaseTest::addCheckPointSectionTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	unsigned char dataBuffer[] = { 0x06, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	ssize_t len = sizeof(dataBuffer);
	int result = writer.addSection(dataBuffer, len);
	std::cout << "\nResult = " << result << std::endl;
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::getMessageIndexTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	unsigned char dataBuffer[] = { 0x04, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	unsigned char dataBuffer1[] = { 0x05, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	unsigned char dataBuffer2[] = { 0x06, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	ssize_t len = sizeof(dataBuffer);
	int result = writer.addSection(dataBuffer, len);
	result = writer.addSection(dataBuffer1, len);
	result = writer.addSection(dataBuffer2, len);
	std::cout << "\n m_messageIndex = " << writer.m_messageIndex << std::endl;
	ASSERT_EQUAL(3, writer.m_messageIndex);
}

void StoreBaseTest::getRecordNumberTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	unsigned char dataBuffer[] = { 0x04, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	ssize_t len = sizeof(dataBuffer);
	int result = writer.addSection(dataBuffer, len);
	uint32_t recordNumber = 0;
	result = writer.getNumberOfWrittenRecord(recordNumber);
	std::cout << "\nrecordNumber = " << recordNumber << std::endl;
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::writeDataTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	unsigned char dataBuffer[] = { 0x04, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	ssize_t len = sizeof(dataBuffer);
	int result = writer.write(dataBuffer, len);
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::openCheckPointTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	int result = writer.openCheckpoint();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::closeCheckPointTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	int result = writer.closeCheckpoint();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::isCheckPointFullSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	writer.m_messageIndex = 6000;
	std::cout << "\nm_messageIndex = " << writer.m_messageIndex << std::endl;
	int result = writer.isCheckPointFull();
	ASSERT_EQUAL(common::errorCode::ERR_CKPT_SECTIONS_FULL, result);
}

void StoreBaseTest::isCheckPointFullFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	writer.m_messageIndex = 555;
	std::cout << "\nm_messageIndex = " << writer.m_messageIndex << std::endl;
	int result = writer.isCheckPointFull();
	ASSERT_EQUAL(common::errorCode::ERR_CKPT_SECTIONS_AVAILABLE, result);
}

void StoreBaseTest::convertInfoBufferToDataSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	char* buffer= "24000_2355";
	int result = writer.convertInfoBufferToData(buffer);
	std::cout << "\nm_messageIndex = " << writer.m_messageIndex << std::endl;
	std::cout << "\nm_messageOffset = " << writer.m_messageOffset << std::endl;
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::convertInfoBufferToDataFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	char* buffer= "24000";
	int result = writer.convertInfoBufferToData(buffer);
	ASSERT_EQUAL(common::errorCode::ERR_CKPT_INFOSECTION_CORRUPTED, result);
}

void StoreBaseTest::createInfoSectionSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	writer.m_messageOffset = 18000;
	writer.m_messageIndex = 1234;
	int result = writer.createInfoSection();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::createInfoSectionFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	writer.m_messageOffset = 0;
	writer.m_messageIndex = 9999;
	int result = writer.createInfoSection();
	ASSERT_EQUAL(common::errorCode::ERR_CKPT_INFOSECTION_CREATE_FAILURE, result);
}

void StoreBaseTest::updateInfoSectionSuccessTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	writer.m_messageOffset = 6000;
	writer.m_messageIndex = 1212;
	int result = writer.updateInfoSection();
	ASSERT_EQUAL(common::errorCode::ERR_NO_ERRORS, result);
}

void StoreBaseTest::updateInfoSectionFailureTest() {
	SaCkptHandleT cktHandle;
	store::checkpoint::Writer writer("VCHS", "BC0", "VCHS_BC0_DDT1", cktHandle);
	writer.m_messageOffset = 0;
	writer.m_messageIndex = 9999;
	writer.m_checkpointHandle = 0;
	int result = writer.updateInfoSection();
	ASSERT_EQUAL(common::errorCode::ERR_CKPT_INFOSECTION_WRITE_FAILURE, result);
}

cute::suite StoreBaseTest::make_suite_storeBaseTest(){
	cute::suite s;
	s.push_back(CUTE(getDataSourceNameTest));
	s.push_back(CUTE(getProducerNameTest));
	s.push_back(CUTE(getCheckPointNameTest));
	s.push_back(CUTE(getNewCheckPointTest));
	s.push_back(CUTE(addCheckPointSectionTest));
	s.push_back(CUTE(getMessageIndexTest));
	s.push_back(CUTE(getRecordNumberTest));
	s.push_back(CUTE(writeDataTest));
	s.push_back(CUTE(openCheckPointTest));
	s.push_back(CUTE(closeCheckPointTest));
	s.push_back(CUTE(isCheckPointFullSuccessTest));
	s.push_back(CUTE(isCheckPointFullFailureTest));
	s.push_back(CUTE(convertInfoBufferToDataSuccessTest));
	s.push_back(CUTE(convertInfoBufferToDataFailureTest));
	s.push_back(CUTE(createInfoSectionSuccessTest));
	s.push_back(CUTE(createInfoSectionFailureTest));
	s.push_back(CUTE(updateInfoSectionSuccessTest));
	s.push_back(CUTE(updateInfoSectionFailureTest));
	return s;
}
