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
#include "mtap/PduDataTest.h"
#include "mtap/pduhandler.h"
#include "stubs/mtap_stubs.h"

#include "cute.h"

namespace mtaptest
{
unsigned char doWillPrimitive[] = { 0x09, 0x00, 0x20, 0xF0, 0x03 };
unsigned char getParamPrimitive[] = { 0x05, 0x00, 0x20, 0xF0, 0x04, 0x00, 0x01, 0x03, 0x04 };
unsigned char unlinkAllFilesPrimitive[] = { 0x03, 0x00, 0x20, 0xF0 };
unsigned char unlinkFilesPrimitive[] = { 0x02, 0x00, 0x20, 0xF0 };
unsigned char putRecordPrimitive[] = { 0x01, 0x00, 0x20, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0xA0, 0x82, 0x01, 0x76, 0x33, 0x01 };
unsigned char synchronizePrimitive[] = { 0x06, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
unsigned char invalidPrimitive[] = { 0x00, 0x00, 0x00, 0x00 };

}

void createPutRecordPduTest() {
	mtap::DataChannel testDataChannel(1, "VCHS");
	mtap::PDUHandler testPduHandler(&testDataChannel);
	int result = testPduHandler.initialize();
	ASSERT_EQUAL(result, 0);
	unsigned char* buffer = mtaptest::putRecordPrimitive;
	ssize_t len = sizeof(buffer);
	ACE_HANDLE fd;
	unsigned char* outBuffer;
	ssize_t outLen;
	result = testPduHandler.handleRequest(fd, buffer, len, outBuffer, outLen);
	ASSERT_EQUAL(result, 0);
}

void createDoWillPduTest() {
	mtap::DataChannel testDataChannel(1, "VCHS");
	mtap::PDUHandler testPduHandler(&testDataChannel);
	int result = testPduHandler.initialize();
	ASSERT_EQUAL(result, 0);
	unsigned char* buffer = mtaptest::doWillPrimitive;
	ssize_t len = sizeof(buffer);
	ACE_HANDLE fd;
	unsigned char* outBuffer;
	ssize_t outLen;
	result = testPduHandler.handleRequest(fd, buffer, len, outBuffer, outLen);
	ASSERT_EQUAL(result, 0);
}

void createGetParamPduTest() {
	mtap::DataChannel testDataChannel(1,"VCHS");
	mtap::PDUHandler testPduHandler(&testDataChannel);
	int result = testPduHandler.initialize();
	ASSERT_EQUAL(result, 0);
	unsigned char* buffer = mtaptest::getParamPrimitive;
	ssize_t len = sizeof(buffer);
	ACE_HANDLE fd;
	unsigned char* outBuffer;
	ssize_t outLen;
	result = testPduHandler.handleRequest(fd, buffer, len, outBuffer, outLen);
	ASSERT_EQUAL(result, 0);
}

void createUnlinkAllFilesPduTest() {
	mtap::DataChannel testDataChannel(1,"VCHS");
	mtap::PDUHandler testPduHandler(&testDataChannel);
	int result = testPduHandler.initialize();
	ASSERT_EQUAL(result, 0);
	unsigned char* buffer = mtaptest::unlinkAllFilesPrimitive;
	ssize_t len = sizeof(buffer);
	ACE_HANDLE fd;
	unsigned char* outBuffer;
	ssize_t outLen;
	result = testPduHandler.handleRequest(fd, buffer, len, outBuffer, outLen);
	ASSERT_EQUAL(result, 0);
	ASSERT_EQUAL(outLen, 5);
}

void createUnlinkFilesPduTest() {
	mtap::DataChannel testDataChannel(1, "VCHS");
	mtap::PDUHandler testPduHandler(&testDataChannel);
	int result = testPduHandler.initialize();
	ASSERT_EQUAL(result, 0);
	unsigned char* buffer = mtaptest::unlinkFilesPrimitive;
	ssize_t len = sizeof(buffer);
	ACE_HANDLE fd;
	unsigned char* outBuffer;
	ssize_t outLen;
	result = testPduHandler.handleRequest(fd, buffer, len, outBuffer, outLen);
	ASSERT_EQUAL(result, 0);
	ASSERT_EQUAL(outLen, 5);
}

void createSynchronizePduTest() {
	mtap::DataChannel testDataChannel(1, "VCHS");
	mtap::PDUHandler testPduHandler(&testDataChannel);
	int result = testPduHandler.initialize();
	ASSERT_EQUAL(result, 0);
	unsigned char* buffer = mtaptest::synchronizePrimitive;
	ssize_t len = sizeof(buffer);
	ACE_HANDLE fd;
	unsigned char* outBuffer;
	ssize_t outLen;
	result = testPduHandler.handleRequest(fd, buffer, len, outBuffer, outLen);
	ASSERT_EQUAL(result, 0);
}

void createInvalidPduTest() {
	mtap::DataChannel testDataChannel(1, "VCHS");
	mtap::PDUHandler testPduHandler(&testDataChannel);
	int result = testPduHandler.initialize();
	ASSERT_EQUAL(result, 0);
	unsigned char* buffer = mtaptest::invalidPrimitive;
	ssize_t len = sizeof(buffer);
	ACE_HANDLE fd;
	unsigned char* outBuffer;
	ssize_t outLen;
	result = testPduHandler.handleRequest(fd, buffer, len, outBuffer, outLen);
	// 100 = ERR_MALFORMED_PACKAGE
	ASSERT_EQUAL(result, 100);
}

cute::suite make_suite_pduHandlerTest() {
	cute::suite s;
	s.push_back(CUTE(createDoWillPduTest));
	s.push_back(CUTE(createGetParamPduTest));
	s.push_back(CUTE(createPutRecordPduTest));
	s.push_back(CUTE(createUnlinkFilesPduTest));
	s.push_back(CUTE(createUnlinkAllFilesPduTest));
	s.push_back(CUTE(createSynchronizePduTest));
	s.push_back(CUTE(createInvalidPduTest));
	return s;
}



