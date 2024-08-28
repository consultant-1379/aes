/*
 * ckpt_stub.h
 *
 *  Created on: Jul 9, 2015
 *      Author: xnadnar
 */

#ifndef INC_STUBS_CKPT_STUB_H_
#define INC_STUBS_CKPT_STUB_H_

#include <map>
#include <list>
#include <string>
#include "operation/operation.h"
#include <boost/make_shared.hpp>

//const int ACE_INVALID_HANDLE = -1;

namespace store
{
class StrategyInterface {
public:
	StrategyInterface(){;}
	virtual ~StrategyInterface(){;}
};

class Observer
{
public:
	Observer() { }

	virtual ~Observer() { }


	virtual void update(const std::string& producerName) = 0;

};

class HandlerInterface {
public:
	HandlerInterface():m_lastWrittenRec(0){;}
	virtual ~HandlerInterface(){;}
	int getNumberOfWrittenRecord(uint32_t& recordNumber) { recordNumber = m_lastWrittenRec; return common::errorCode::ERR_NO_ERRORS; }
	int forceStoreChange() { return common::errorCode::ERR_NO_ERRORS; }
	int write(const void* source, const uint32_t& length) { return common::errorCode::ERR_NO_ERRORS; }
	int read(void* destination, uint32_t& length) { return common::errorCode::ERR_NO_ERRORS; }
	int commit(const uint32_t& numberOfRecords) { return common::errorCode::ERR_NO_ERRORS; }
	void setMessageIndex(const uint32_t& recordNumber) {}
	uint32_t m_lastWrittenRec;
};

class StoreBase : public HandlerInterface
{
public:
	StoreBase(){;}
	virtual ~StoreBase(){;}
};
class CheckPointStrategy : public StrategyInterface
{
	CheckPointStrategy(){;}
	virtual ~CheckPointStrategy(){;}
};

class StoringManager
{
public:


	StoringManager() { }


	virtual ~StoringManager() { }


	bool setStoreStrategy(boost::shared_ptr<StrategyInterface> storeStrategy) {return true; }


	inline boost::shared_ptr<HandlerInterface> getWriter(const std::string& dataSourceName, const std::string& producerName)
	{
		return boost::make_shared<StoreBase>();
	}


	boost::shared_ptr<HandlerInterface> getReader(const std::string& dataSourceName, const std::string& producerName)
	{
		return boost::make_shared<StoreBase>();
	}

	void getProducers(const std::string& dataSourceName, std::list<std::string>& producers){ }


	int getNewStoreName(const std::string& dataSourceName, const std::string& producerName, std::string& storeName)
	{
		storeName.assign("safCkpt=DDT__");
		storeName.append(dataSourceName);
		storeName.append("_");
		storeName.append(producerName);
		storeName.append("_");
		// add a timestamp
		storeName.append("1440492151");
		return common::errorCode::ERR_NO_ERRORS;
	}

	int getNumberOfStores(const std::string& dataSourceName, const std::string& producerName){return 2; }

	void shutdown(){ }

	bool storeEmpty(const std::string& dataSourceName) const { return true;}

	int getLastReceivedMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastReceivedMsgNo)
	{
		lastReceivedMsgNo=1234;
		return common::errorCode::ERR_NO_ERRORS;
	}

	int getLastSentMessage(const std::string& dataSourceName, const std::string& producerName, uint64_t& lastSentMsgNo)
	{
		lastSentMsgNo=100;
		return common::errorCode::ERR_NO_ERRORS;
	}

	void subscribe(const std::string& dataSourceName, store::Observer* observer)
	{
	}


};

namespace checkpoint {

class Strategy : public StrategyInterface{
public:
	Strategy(){;}
	virtual ~Strategy(){;}
};

} /* namespace checkpoint */
} /* namespace store */

#endif /* INC_STUBS_CKPT_STUB_H_ */
