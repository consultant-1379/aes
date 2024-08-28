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


#include "alarm/datasourcewarning.h"
#include "common/utility.h"

#include <boost/format.hpp>

namespace alarms
{
	namespace DataSourceWarningFields
	{
		const std::string objectClassOfReference = "APZ";
		const std::string severity = "A2";
		const uint32_t specificProblem = 2000;
		const std::string problemData = "The data source have been filled to pre-defined limit.";
		const std::string probableCause = "AP DATA SOURCE WARNING";
		const std::string problemText = "LIMIT%%\n%d%%";

		const std::string DirectDataTransferCOMDN = "ManagedElement=%s,SystemFunctions=1,AxeFunctions=1,DataOutputHandling=1,DirectDataTransferM=1";
		const std::string simplifiedDirectDataTransferCOMDN = "DirectDataTransferM=1";

	}

	DataSourceWarning::DataSourceWarning(const std::string& objOfReference, const int& threshold)
	: Alarm(Alarm::DATASOURCE_WARNING,
			DataSourceWarningFields::objectClassOfReference,
			objOfReference,
			DataSourceWarningFields::severity,
			DataSourceWarningFields::specificProblem,
			DataSourceWarningFields::probableCause,
			DataSourceWarningFields::problemData
			)
	{
		//Build problemText
		boost::format problemTextFormat(DataSourceWarningFields::problemText);
		problemTextFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
		problemTextFormat % threshold;

		setProblemText(problemTextFormat.str());

		//Build ObjOfRef
		//TODO: ALH currently doesn't support obj of reference longer than 64 chars. Using a simplified DN for the moment
		//boost::format objOfRefFormat(DataSourceWarningFields::DirectDataTransferCOMDN);
		//objOfRefFormat.exceptions( boost::io::all_error_bits ^ ( boost::io::too_many_args_bit | boost::io::too_few_args_bit ) );
		//objOfRefFormat % common::utility::getManagedElementId();
		//setObjectOfReference(objOfRefFormat.str());

		{
			//TODO: Remove this once ALH supports longer DNs
			setObjectOfReference(DataSourceWarningFields::simplifiedDirectDataTransferCOMDN);
		}
	}

	DataSourceWarning::~DataSourceWarning()
	{

	}

} /* namespace alarms */
