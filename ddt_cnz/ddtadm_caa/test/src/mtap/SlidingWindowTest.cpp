#include "cute.h"
#include "mtap/SlidingWindowTest.h"
#include "mtap/slidingwindow.h"
#include "mtap/datachannel.h"
#include "mtap/synchronize.h"
#include "mtap/putrecord.h"
#include "string"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <stubs/store_stub.h>
#include <mtap/protocol.h>

void SlidingWindowTest::testSyncWithDiffMoreThanWindowSize()
{
	mtap::DataChannel * dataChannel = new mtap::DataChannel(1,"VCHS");
	mtap::PDUHandler * pduHandler = new mtap::PDUHandler(dataChannel);
	mtap::SlidingWindow swObj(pduHandler);
	swObj.initialize();

	unsigned char synchronizePrimitive[] = { 0x06, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	const ssize_t& bufferSize = 8;
	boost::shared_ptr<mtap::Synchronize> pdu = boost::make_shared<mtap::Synchronize>(const_cast<unsigned char*>(synchronizePrimitive),bufferSize);
	pdu->m_recordNumber = 3000;

	//Diff between lastWritten and lastReceived is more than windowsize(32)
	ACE_HANDLE fd = 4;
	swObj.m_writer = boost::make_shared<store::HandlerInterface>();
	swObj.m_writer->m_lastWrittenRec = 2000;
	swObj.synchronize(pdu,fd);
	swObj.clean(fd);
	uint32_t synchVal = pdu->m_recordNumber - mtap::protocol::DEFAULT_WIN_SIZE + 1;

	ASSERT_EQUAL(pdu->m_lastStored, synchVal);
}

void SlidingWindowTest::testSyncWithDiffLessThanWindowSize()
{
	mtap::DataChannel * dataChannel = new mtap::DataChannel(1,"VCHS");
	mtap::PDUHandler * pduHandler = new mtap::PDUHandler(dataChannel);
	mtap::SlidingWindow swObj(pduHandler);
	swObj.initialize();

	unsigned char synchronizePrimitive[] = { 0x06, 0x00, 0x20, 0x30, 0x1A, 0x00, 0x00, 0x00 };
	const ssize_t& bufferSize = 8;
	boost::shared_ptr<mtap::Synchronize> pdu = boost::make_shared<mtap::Synchronize>(const_cast<unsigned char*>(synchronizePrimitive),bufferSize);
	pdu->m_recordNumber = 3000;

	//Diff between lastWritten and lastReceived is less than windowsize(32)
	ACE_HANDLE fd = 5;
	swObj.m_writer = boost::make_shared<store::HandlerInterface>();
	swObj.m_writer->m_lastWrittenRec = 2988;
	swObj.synchronize(pdu,fd);
	swObj.clean(fd);
	uint32_t synchVal = swObj.m_writer->m_lastWrittenRec - 1;

	ASSERT_EQUAL(pdu->m_lastStored, synchVal);
}

void SlidingWindowTest::testUnlink()
{//unlink will closes the current checkpoint and reset the m_baseOffsetRecNr
	mtap::DataChannel * dataChannel = new mtap::DataChannel(3,"VCHS");
	mtap::PDUHandler * pduHandler = new mtap::PDUHandler(dataChannel);
	mtap::SlidingWindow swObj(pduHandler);
	swObj.initialize();
	swObj.m_baseOffsetRecNr = 2000;

	swObj.unlink();
	ASSERT_EQUAL(swObj.m_baseOffsetRecNr,0);
}

void SlidingWindowTest::testAdd()
{
	mtap::DataChannel * dataChannel = new mtap::DataChannel(3,"VCHS");
	mtap::PDUHandler * pduHandler = new mtap::PDUHandler(dataChannel);
	mtap::SlidingWindow swObj(pduHandler);
	swObj.initialize();

	unsigned char putRecordPrimitive[] = {0x01, 0x00, 0x20, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0xA0, 0x82, 0x01, 0x76, 0x33, 0x01 };
	const ssize_t& bufferSize = 8;
	ACE_HANDLE fd = 10;
	boost::shared_ptr<mtap::PutRecord> pdu = boost::make_shared<mtap::PutRecord>(const_cast<unsigned char*>(putRecordPrimitive),bufferSize);
	mtap::protocol::ErrorCode result = swObj.add(pdu,10);
	ASSERT_EQUAL(mtap::protocol::ERR_NO_ERROR, result);
}

//checkRange if m_baseOffsetRecNr greater than record number.
void SlidingWindowTest::testCheckRange_1()
{
	mtap::DataChannel * dataChannel = new mtap::DataChannel(3,"VCHS");
	mtap::PDUHandler * pduHandler = new mtap::PDUHandler(dataChannel);
	mtap::SlidingWindow swObj(pduHandler);
	swObj.initialize();

	//BaseOffsetRecNr is greater than current record number
	swObj.m_baseOffsetRecNr = 2000;
	const uint32_t pduRecordNumber = 1970;
	mtap::protocol::ErrorCode result = swObj.checkRange(pduRecordNumber);
	ASSERT_EQUAL(result , mtap::protocol::ERR_RECORD_ALREADY_STORED);
}

//checkRange if m_baseOffsetRecNr greater than record number by more than 32
void SlidingWindowTest::testCheckRange_2()
{
	mtap::DataChannel * dataChannel = new mtap::DataChannel(3,"VCHS");
	mtap::PDUHandler * pduHandler = new mtap::PDUHandler(dataChannel);
	mtap::SlidingWindow swObj(pduHandler);
	swObj.initialize();
	//BaseOffsetRecNr is greater than current record number
	swObj.m_baseOffsetRecNr = 2000;
	const uint32_t pduRecordNumber = 2033;
	mtap::protocol::ErrorCode result = swObj.checkRange(pduRecordNumber);
	ASSERT_EQUAL(result, mtap::protocol::ERR_RECORD_OUT_OF_RANGE);
}

cute::suite SlidingWindowTest::make_suite_SlidingWindowTest()
{
	cute::suite s;
	s.push_back(CUTE(testSyncWithDiffMoreThanWindowSize));
	s.push_back(CUTE(testSyncWithDiffLessThanWindowSize));
	s.push_back(CUTE(testUnlink));
	s.push_back(CUTE(testAdd));
	s.push_back(CUTE(testCheckRange_1));
	s.push_back(CUTE(testCheckRange_2));
	return s;
}
