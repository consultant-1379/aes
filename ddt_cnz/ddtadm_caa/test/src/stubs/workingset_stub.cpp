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
#include "stubs/workingset_stub.h"

namespace engine
{
	WorkingSet * WorkingSet::m_workingSet = 0;

	WorkingSet* WorkingSet::instance()
	{
		if (! m_workingSet)
		{
			m_workingSet = new (std::nothrow) WorkingSet();
		}

		return m_workingSet;
	}

	void WorkingSet::destroy()
	{
		delete m_workingSet;

		m_workingSet = 0;
	}

}
