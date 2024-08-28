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

#include "alarm/connectionfaultalarm.h"
#include "imm/imm.h"
#include "common/utility.h"

#include <boost/format.hpp>

namespace alarms
{
	namespace ConnectionFaultFields
	{
		const std::string objectClassOfReference = "APZ";
		const std::string severity = "A1";
		const uint32_t specificProblem = 2121U;
		const std::string probableCause = "DIRECT DATA TRANSFER FAULT";
		const std::string problemData = "The connection to the remote peer lost or write access denied";
		const std::string problemText = "CAUSE\nDIRECT DATA TRANSFER, PEER: CONNECTION LOST TO REMOTE SYSTEM\n\nDATA SOURCE\n%s\n\nDATA SINK\n%s\n\nPEER\n%s";

		const std::string blockPeerCOMDN = "ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,DirectDataTransferM=1,DataSource=%s,DataSink=%s,BlockPeer=%s";
		//const std::string simplifiedBlockPeerCOMDN = "DataSource=%s,DataSink=%s,BlockPeer=%s";
		const std::string simplifiedBlockPeerCOMDN = "%s,%s"; //DataSource,BlockPeer
	}

	ConnectionFaultAlarm::ConnectionFaultAlarm(const std::string& peerMoDN)
	: Alarm(Alarm::CONNECTION_FAULT,
			ConnectionFaultFields::objectClassOfReference,
			peerMoDN,
			ConnectionFaultFields::severity,
			ConnectionFaultFields::specificProblem,
			ConnectionFaultFields::probableCause,
			ConnectionFaultFields::problemData )
	{
		std::string dataSourceName;
		std::string dataSinkName;
		std::string peerName;

		common::utility::getRDNValue(peerMoDN, imm::datasource_attribute::RDN, dataSourceName);
		common::utility::getRDNValue(peerMoDN, imm::datasink_attribute::RDN, dataSinkName);

		if(!common::utility::getRDNValue(peerMoDN, imm::blockpeer_attribute::RDN, peerName))
		{
			common::utility::getRDNValue(peerMoDN, imm::filepeer_attribute::RDN, peerName);
		}

		//Build problemText
		boost::format problemTextFormat(ConnectionFaultFields::problemText);
		problemTextFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
		problemTextFormat % dataSourceName % dataSinkName % peerName;

		setProblemText(problemTextFormat.str());

		//Build ObjOfRef

		//TODO: ALH currently doesn't support obj of reference longer than 64 chars. Using a simplified DN for the moment
		//boost::format objOfRefFormat(ConnectionFaultFields::blockPeerCOMDN);
		//objOfRefFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
		//objOfRefFormat % common::utility::getManagedElementId() % dataSourceName % dataSinkName % peerName;

		//setObjectOfReference(objOfRefFormat.str());

		{
			//TODO: Remove this once ALH supports longer DNs
			boost::format simplifiedObjOfRefFormat(ConnectionFaultFields::simplifiedBlockPeerCOMDN);
			simplifiedObjOfRefFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
			simplifiedObjOfRefFormat % dataSourceName % peerName;
			setObjectOfReference(simplifiedObjOfRefFormat.str());
		}
	}

	ConnectionFaultAlarm::~ConnectionFaultAlarm()
	{

	}

} /* namespace alarms */
