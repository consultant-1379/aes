/*
 * mtap_stubs.h
 *
 *  Created on: Jul 9, 2015
 *      Author: xnadnar
 */

#ifndef INC_STUBS_MTAP_STUBS_H_
#define INC_STUBS_MTAP_STUBS_H_

#include "ACS_DSD_Server_stub.h"
#include "mtap/pdu.h"

namespace mtap
{

class Stop : public PDU
{
public:
	Stop(): PDU(protocol::primitive::STOP) {};
	virtual ~Stop() {}

	inline void deserialize() {};

	virtual int createResponse(unsigned char*& outBuffer, ssize_t& outBufferSize) { return 0; };
};

class DataChannel
{
public:

	inline DataChannel(int cp_system_id, const std::string& dataSourceName){;}

	inline virtual ~DataChannel() { /* clean up ? m_incomingMtapSessionQueue */ };

	inline int32_t getCpId () const { return 1; };

	inline const char* getDataSourceName() const { return "VCHS"; };

	inline const char* getDefaultCpName() const { return "CP1"; };

	int addSession(boost::shared_ptr<ACS_DSD_Session> mtap_session) {return 0;}
	int stop() {return 0;}
	int start(){return 0;}
	virtual int svc(){return 0;}
	virtual int handle_close(ACE_HANDLE fd, ACE_Reactor_Mask mask){return 0;}
	void setDefaultCpName(){;}
	int send_data(ACE_HANDLE fd, unsigned char* outBuffer, ssize_t& outBufferLength){return 0;}
	void closeOtherSessions(ACE_HANDLE fd){;}
	inline const char* getInfo() { return "VCHS_CP1"; };
};

} /* namespace mtap */




#endif /* INC_STUBS_MTAP_STUBS_H_ */
