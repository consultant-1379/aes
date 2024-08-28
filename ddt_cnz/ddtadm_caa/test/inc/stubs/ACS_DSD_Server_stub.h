/*
 * ACS_DSD_Server_stub.h
 *
 *  Created on: May 29, 2015
 *      Author: xnadnar
 */

#ifndef INC_ACS_DSD_SERVER_STUB_H_
#define INC_ACS_DSD_SERVER_STUB_H_

#include <string>

class ACE_Recursive_Thread_Mutex;

namespace acs_dsd
{
typedef int HANDLE;
enum ServiceVisibilityConstants {
	SERVICE_VISIBILITY_GLOBAL_SCOPE	=	0,	///< SERVICE_VISIBILITY_GLOBAL_SCOPE description
	SERVICE_VISIBILITY_LOCAL_SCOPE	=	1,
};
enum ErrorConstants {
	ERR_NO_ERRORS,
	ERR_NOT_ENOUGH_SPACE,
	ERR_API_CALL
};
enum ConfigurationConstants {
	CONFIG_INET_IP4_SERVICE_PORT_MIN = 5000,
	CONFIG_INET_IP4_SERVICE_PORT_MAX = 51946
};
enum ServiceModeConstants {
	SERVICE_MODE_UNKNOWN,
	SERVICE_MODE_INET_SOCKET
};
}
//Stub for ACS_DSD_Node
struct ACS_DSD_Node {
	inline ACS_DSD_Node()
	: system_id(0) {;}
	int32_t system_id;
};

//Stub for ACS_DSD_Session
class ACS_DSD_Session {

public:

	inline ACS_DSD_Session () {;}

private:

	inline ACS_DSD_Session (const ACS_DSD_Session & rhs){;}

public:
	virtual ~ACS_DSD_Session (){;}

public:

	ssize_t recv (void * buf, size_t size, const unsigned * timeout_ms, size_t * bytes_transferred) const {return size;}
	ssize_t send (const void * buf, size_t size, const unsigned * timeout_ms, size_t * bytes_transferred) const {return size;}
	int close (){return 0;}
	virtual int get_handles (acs_dsd::HANDLE * handles, int & handle_count) const {return 0;}
	int get_remote_node (ACS_DSD_Node & node) const {return 0;}
};

//Stub for ACS_DSD_Server
class ACS_DSD_Server {
public:

	explicit ACS_DSD_Server (acs_dsd::ServiceModeConstants service_mode = acs_dsd::SERVICE_MODE_UNKNOWN){}
	virtual ~ACS_DSD_Server () {}

public:
	inline int open () { return acs_dsd::ERR_NO_ERRORS; }
	inline int publish (const std::string & service_name, const std::string & service_domain, acs_dsd::ServiceVisibilityConstants visibility = acs_dsd::SERVICE_VISIBILITY_GLOBAL_SCOPE) {
		return acs_dsd::ERR_NO_ERRORS;
	}
	inline int accept (ACS_DSD_Session & session) {
		return acs_dsd::ERR_NO_ERRORS;
	}
	int unregister (){return 0;}
	inline int close () { return 0;};
	int get_handles (acs_dsd::HANDLE * handles, int & handle_count) const
	{
		acs_dsd::HANDLE hand[4];
		handles = hand;
		return acs_dsd::ERR_NO_ERRORS;
	}
	inline int last_error(){return 0;};
	const char * last_error_text() {
		std::string msg("ERROR");
		return msg.c_str();
	}

};

#endif /* INC_ACS_DSD_SERVER_STUB_H_ */
