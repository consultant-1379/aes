/* =================================================================== */
	/**
	@file aes_gcc_eventhandlerimplementation.cpp

	Class method implementation for aes_gcc_eventhandlerimplementation.h

	General rule:
	The error handling is specified for each method.

	No methods initiate or send error reports unless specified.

	@version 1.1.1

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------

	N/A       22/06/2011    xnadnar      Initial Release
	*/
/* =================================================================== */

/*===================================================================
                        INCLUDE DECLARATION SECTION
=================================================================== */
#include "acs_aeh_evreport.h"
#include "acs_aeh_types.h"
#include <string>
#include <map>
#include <aes_gcc_log.h>
#include <aes_gcc_eventhandlerimplementation.h>
#include <ACS_APGCC_Util.H>
#include <ace/ACE.h>

/*===================================================================
                        DECLARATION SECTION
=================================================================== */

GCC_TDEF(aes_gcc_event);
namespace eventInfo
{
        const char* const eventSeverity = "EVENT";
        const char* const objReferenceClass = "APZ";
	const char* const ceaseSeverity = "CEASING";
}

/*===================================================================
                    ROUTINE: CONSTRUCTOR
=================================================================== */
AES_GCC_EventHandlerImplementation::AES_GCC_EventHandlerImplementation():evreport(0)
{
	evreport = new acs_aeh_evreport;
	ACS_APGCC::getProcessName(&m_processName);
	m_processId = ACE_OS::getpid();
}
/*===================================================================
                    ROUTINE: DESTRUCTOR
=================================================================== */
AES_GCC_EventHandlerImplementation::~AES_GCC_EventHandlerImplementation()
{

	std::multimap<std::string, AES_GCC_Event*>::iterator itr;
	for( itr= eventMap.begin(); itr != eventMap.end(); ++itr)
	{
		delete (*itr).second;
		(*itr).second = 0;
	}
	if( evreport != 0 )
	{
			delete evreport;
	}
	evreport = 0;

}
/*===================================================================
                    ROUTINE: filter
=================================================================== */
ACE_INT32 AES_GCC_EventHandlerImplementation::filter (AES_GCC_Event &ex, AES_GCC_Event *&pex)
{
	ehMutex_.acquire();
	GCC_TRACE((aes_gcc_event,"%s","In method filter"));
	pex = &ex;	// default, point to incoming
	const ACE_INT16 OVER_AGE	= 5;		// minutes
	const ACE_INT16 ANCIENT		= 20;

	ACE_TCHAR cyclicwarning[200] = {0};
	ACE_OS::sprintf(cyclicwarning,
			"This could be a cyclic event. It will not be repeated if it occurs again within %d minutes.",
			OVER_AGE);

	//	int over_aged		= OVER_AGE*60;	// seconds
	//	int ancient			= ANCIENT*60;
	ACE_Time_Value over_aged(OVER_AGE*60);
	ACE_Time_Value ancient(ANCIENT*60);

	std::multimap<std::string, AES_GCC_Event*>::iterator itr;

	ACE_TCHAR stname[80] = {0};
	ACE_OS::sprintf(stname, "%ld", ex.eventCode());

	// find events in map
	int number = eventMap.count(stname);
	itr = eventMap.find(stname);

	// Get 'correct' event (with same "object of reference")
	while (number-- > 0)
	{
		if (ACE_OS::strcmp(ex.objectOfReference(), (*itr).second->objectOfReference()) == 0)
		{
			break;
		}
		itr++;
	}
	if (number < 0) itr = eventMap.end();

	if (itr == eventMap.end() )
	{
		// did not find ex in map, is new?
		GCC_TRACE((aes_gcc_event,"eventhandler: could not find %s in map", stname));
		if (ex.kind() == AES_GCC_Event::EV_ALARM)		// report alarm
		{
			// new alarm. go ahead and report. will be inserted
			GCC_TRACE((aes_gcc_event,"%s","eventhandler: new alarm , report"));
		}
		if (ex.kind() == AES_GCC_Event::EV_CEASING)	// cease alarm
		{
			GCC_TRACE((aes_gcc_event,"%s","eventhandler: alarm not reported, do not cease"));
			ehMutex_.release();
			return 0;									// not found. do not report
		}
	}
	else
	{
		GCC_TRACE((aes_gcc_event,"eventhandler: found %s itr = %X", stname, (*itr).second ));

		if (ex.kind() == AES_GCC_Event::EV_ALARM)	// report alarm
		{
			GCC_TRACE((aes_gcc_event,"%s","eventhandler: alarm already reported, do not report again"));
			ehMutex_.release();
			return 0;								// already reported, do not report
		}
		if (ex.kind() == AES_GCC_Event::EV_CEASING)	// cease alarm
		{
			if ((*itr).second->kind() == AES_GCC_Event::EV_ALARM)	// can only cease alarm
			{
				GCC_TRACE((aes_gcc_event,"%s","eventhandler: alarm found, cease"));

				// copy vital information from alarm
				pex->setErrorCode((*itr).second->errorCode());
				pex->setOsError((*itr).second->osError());
				pex->setProblemData((*itr).second->problemData());
				pex->setProblemText((*itr).second->problemText());
				delete (*itr).second;
				(*itr).second = 0;
				eventMap.erase(itr);		// erase from map
				ehMutex_.release();
				return 1;								// found. cease will be effective
			}
			else
			{
				GCC_TRACE((aes_gcc_event,"eventhandler: non-alarm found, cannot cease"));
				ehMutex_.release();
				return 0;
			}
		}
		GCC_TRACE((aes_gcc_event,"Exception data from itr: %s", (*itr).second->str().c_str()));
	}

	// avoid duplicates...

	if (itr == eventMap.end() )		// new, first time event!
	{
		pex = new AES_GCC_Event(ex);	// copy incoming
		GCC_TRACE((aes_gcc_event,"%s","Event raised for the first time"));
		GCC_TRACE((aes_gcc_event,"new event \n%s", pex->str().c_str()));

		pex->incCounter();		// increment counter
		GCC_TRACE((aes_gcc_event,"%s","Event inserted in map"));

		eventPair.first = stname;
		eventPair.second = pex;
		eventMap.insert(eventPair);
		GCC_TRACE((aes_gcc_event," mapsize = %d (pex=%X)\n", eventMap.size(), pex));
	}
	else
	{
		if ((((*itr).second)->age()) > ancient)	// very very old, regard as new!
		{
			GCC_TRACE((aes_gcc_event,"%s","times>1 & ancient!"));

			(*itr).second->setProblemText("");		// no extra info
			(*itr).second->setCounter(1);			// set as new would be
			(*itr).second->resetAge();				// count from now

			GCC_TRACE((aes_gcc_event,"ancient\n%s", (*itr).second->str().c_str()));

		}
		else		// not THAT old
		{
			if ((*itr).second->counter() == 1)	// second time, issue warning for cyclic events
			{
				ACE_TCHAR buf[200] = {0};
				ACE_OS::sprintf(buf, "%s", cyclicwarning);
				(*itr).second->setProblemText(buf);		// warn for cyclic
				(*itr).second->incCounter();			// increment counter
				GCC_TRACE((aes_gcc_event,"times == 1\n%s", (*itr).second->str().c_str()));

				std::string buf2;
				buf2 = (std::string)pex->problemText() + " " + buf;
				pex->setProblemText(buf2.c_str());
			}
			else	// 2 or more times before! can be cyclic
			{
				if ((*itr).second->age() > over_aged)	// very old, regard as almost new!
				{

					GCC_TRACE((aes_gcc_event,"%s","times>1 & over aged"));
					ACE_TCHAR buf[200] = {0};
					ACE_OS::snprintf(buf,sizeof(buf)-1, "(%d times since last cyclic warning)",(*itr).second->counter());
					(*itr).second->setProblemText(buf);	// warn that nn event were identical
					(*itr).second->setCounter(1);				// set as new
					(*itr).second->resetAge();		// count from now
					GCC_TRACE((aes_gcc_event,"over aged\n%s", (*itr).second->str().c_str()));
					std::string buf2;
					buf2 = (std::string)pex->problemText() + " " + buf;
					pex->setProblemText(buf2.c_str());
				}
				else
				{
					GCC_TRACE((aes_gcc_event,"%s","times>1 but not over aged"));
					(*itr).second->incCounter();		// increment counter
					GCC_TRACE((aes_gcc_event,"not overaged\n%s", (*itr).second->str().c_str()));
					ehMutex_.release();
					return 0;		// do not produce an event
				}
			}
		}
	}
	ehMutex_.release();
	return 1;	// produce event
}
/*===================================================================
                    ROUTINE: event
=================================================================== */
bool AES_GCC_EventHandlerImplementation::event (AES_GCC_Event &ex, bool forced)
{
	ehMutex_.acquire();

	GCC_TRACE((aes_gcc_event,"%s","In method event"));
	ex.setKind(AES_GCC_Event::EV_EVENT);
	AES_GCC_Event *pex = 0;

	ACE_TCHAR process[80] = { 0 };
	ACE_TCHAR objectr[OBJECT_OF_REF_MAX_LEN] = { 0 };
	ACE_INT32 specific = 0;

	if (filter(ex, pex) == 0)	// do not report
	{
		if (forced)
		{
			GCC_TRACE((aes_gcc_event,"%s","It is a forced event !"));
			pex->setProblemText("");	// reset cyclic warning if forced event
		}
		else
		{
			GCC_TRACE((aes_gcc_event,"%s","event got filtered out..."));
			ehMutex_.release();
			return false;	// filtered out and not forced
		}
	}

	// filter or forced flag said: go ahead
	ACE_OS::sprintf(process,	"%s:%d",m_processName.c_str(),m_processId);
	if ((std::string)(pex->objectOfReference()) == "")
	{
		ACE_OS::sprintf(objectr,"%s/%d",m_processName.c_str(),m_processId);
	}
	else
	{
		(void) ACE_OS::strncpy(objectr, pex->objectOfReference(), OBJECT_OF_REF_MAX_LEN-1);
		objectr[OBJECT_OF_REF_MAX_LEN-1] = '\0';
	}
	specific = pex->eventCode();
	ACS_AEH_ReturnType status = ACS_AEH_error;
	if( evreport != 0)
	{
	   status = evreport->sendEventMessage(process,					// process name
			specific,					// specific problem
			eventInfo::eventSeverity,					// perceived severity
			pex->probableCause(),	    // probably cause
			eventInfo::objReferenceClass,						// object class of reference
			objectr,					// object of reference
			pex->problemData(),	// problem data
			pex->problemText());	// problem text
	   if (status == ACS_AEH_error)
	   {
		ACE_INT32 err_code = evreport->getError();
		switch (err_code)
		{
		case ACS_AEH_syntaxError:
			GCC_TRACE((aes_gcc_event,"eventhandler:ACS_AEH_syntaxError %s",pex->str().c_str()));
			break;

		case ACS_AEH_eventDeliveryFailure:
			GCC_TRACE((aes_gcc_event,"ACS_AEH_eventDeliveryFailure %s",	pex->str().c_str()));
			break;
		}
		ehMutex_.release();
		return false;	// did not send event
	    }
	    else if(status == ACS_AEH_ok)
	    {
		GCC_TRACE((aes_gcc_event,"%s","Event sent to AEH"));
		GCC_TRACE((aes_gcc_event,"%s","Event Data shown below "));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
		GCC_TRACE((aes_gcc_event,"Process name = %s",process));
		GCC_TRACE((aes_gcc_event,"Event Number = %d",specific));
		GCC_TRACE((aes_gcc_event,"Problem Data = %s",pex->problemData()));
		GCC_TRACE((aes_gcc_event,"Problem Text = %s",pex->problemText()));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
	    }
	}
	ehMutex_.release();
	return true;	// event reported

}
/*===================================================================
                    ROUTINE: alarm
=================================================================== */
bool AES_GCC_EventHandlerImplementation::alarm (AES_GCC_Event &ex, const ACE_TCHAR *severity)
{
	ehMutex_.acquire();
	GCC_TRACE((aes_gcc_event,"%s","In alarm method "));
	ex.setKind(AES_GCC_Event::EV_ALARM);

	AES_GCC_Event *pex = 0;

	ACE_TCHAR process[80] = {0};
	ACE_TCHAR objectr[OBJECT_OF_REF_MAX_LEN] = {0};
	ACE_INT32 specific = 0;

	if (filter(ex, pex) == 0)
	{
		ehMutex_.release();
		return false;			// duplicate found, do not report
	}



	// filter said: go ahead
	GCC_TRACE((aes_gcc_event,"%s","alarm:pex=%X ",pex));

	if((std::string)process == "")
        {
                GCC_TRACE((aes_gcc_event,"In AES_GCC_EventHandlerImplementation::alarm  process is NULL "));
                ACE_OS::strcpy(process, "");
                //ehMutex_.release();
                //return false;
        }

	ACE_OS::sprintf(process,"%s:%d",m_processName.c_str(),m_processId);

	if ((std::string)pex->objectOfReference() == "")
	{
		ACE_OS::sprintf(objectr,"%s/%d",m_processName.c_str(),m_processId);
	}
	else
	{
		(void) ACE_OS::strncpy(objectr, pex->objectOfReference(), OBJECT_OF_REF_MAX_LEN-1);
		objectr[OBJECT_OF_REF_MAX_LEN-1] = '\0';
	}
	specific = pex->eventCode();
	ACS_AEH_ReturnType status = ACS_AEH_error;
        if (evreport != 0 )
        {
	   status = evreport->sendEventMessage(process,	            // process name
			specific,		    // specific problem
			severity,			// perceived severity
			pex->probableCause(),// probable cause
			eventInfo::objReferenceClass,				// object class of reference
			objectr,				// object of reference
			pex->problemData(),	// problem data
			pex->problemText(),	// problem text
			pex->manualCease()); // manual cease
	   if (status == ACS_AEH_error)
	   {
		ACE_INT32 err_code = evreport->getError();
		switch (err_code)
		{
		case ACS_AEH_syntaxError:
			GCC_TRACE((aes_gcc_event,"eventhandler:ACS_AEH_syntaxError %s",pex->str().c_str()));
			break;
		case ACS_AEH_eventDeliveryFailure:
			GCC_TRACE((aes_gcc_event,"eventhandler:ACS_AEH_eventDeliveryFailure %s",pex->str().c_str()));
			break;
		}
		ehMutex_.release();
		return false;		// alarm not reported
	   }
	   else if(status == ACS_AEH_ok)
	   {

		GCC_TRACE((aes_gcc_event,"%s","Alarm sent to AEH"));
		GCC_TRACE((aes_gcc_event,"%s","Alarm Data shown below  "));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
		GCC_TRACE((aes_gcc_event,"Process name = %s",process));
		GCC_TRACE((aes_gcc_event,"Severity = %s",severity));
		GCC_TRACE((aes_gcc_event,"Event Number = %d",specific));
		GCC_TRACE((aes_gcc_event,"Problem Data = %s",pex->problemData()));
		GCC_TRACE((aes_gcc_event,"Problem Text = %s",pex->problemText()));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
	   }
        }
	ehMutex_.release();
	return true;	// alarm reported
}

/*===================================================================
                    ROUTINE: cease
=================================================================== */
bool AES_GCC_EventHandlerImplementation::cease (AES_GCC_Event &ex)
{

	ehMutex_.acquire();
	GCC_TRACE((aes_gcc_event,"%s","In cease method"));
	ex.setKind(AES_GCC_Event::EV_CEASING);

	AES_GCC_Event *pex = 0;

	ACE_TCHAR process[80] = {0};
	ACE_TCHAR objectr[OBJECT_OF_REF_MAX_LEN] = {0};
	ACE_INT32 specific = 0;

		
	if (filter(ex, pex) == 0)	// alarm not found, do not cease
	{
		ehMutex_.release();
		return false;
	}

	// filter said: go ahead
	ACE_OS::sprintf(process,"%s:%d",m_processName.c_str(),m_processId);

	if ((std::string)pex->objectOfReference() == "")
	{
		ACE_OS::sprintf(objectr,"%s/%d",m_processName.c_str(),m_processId);
	}
	else
	{
		(void) ACE_OS::strncpy(objectr, pex->objectOfReference(), OBJECT_OF_REF_MAX_LEN-1);
		objectr[OBJECT_OF_REF_MAX_LEN-1] = '\0';
	}
	specific = pex->eventCode();
	ACS_AEH_ReturnType status = ACS_AEH_error;
        if ( evreport != 0)
        {
	   status = evreport->sendEventMessage(process,					// process name
			specific,					// specific problem
			eventInfo::ceaseSeverity,				// perceived severity = CEASING
			pex->probableCause(),	// probable cause
			eventInfo::objReferenceClass,						// object class of reference
			objectr,					// object of reference
			pex->problemData(),	// problem data
			pex->problemText());	// problem text
	   if (status== ACS_AEH_error)
	   {
		ACE_INT32 err_code = evreport->getError();
		switch (err_code)
		{
		case ACS_AEH_syntaxError:
			GCC_TRACE((aes_gcc_event,"eventhandler: AEH Syntax Error %s",pex->str().c_str()));
			break;

		case ACS_AEH_eventDeliveryFailure:
			GCC_TRACE((aes_gcc_event,"%s","eventhandler: ACS_AEH_eventDeliveryFailure %s",pex->str().c_str()));
			break;
		}
		ehMutex_.release();
		return false;		// alarm not reported
	    }
	    else if(status == ACS_AEH_ok)
	    {
		GCC_TRACE((aes_gcc_event,"%s","Cease alarm sent to AEH"));
		GCC_TRACE((aes_gcc_event,"%s","Cease Data shown below "));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
		GCC_TRACE((aes_gcc_event,"Process name = %s",process));
		GCC_TRACE((aes_gcc_event,"Event Number = %d",specific));
		GCC_TRACE((aes_gcc_event,"Problem Data = %s",pex->problemData()));
		GCC_TRACE((aes_gcc_event,"Problem Text = %s",pex->problemText()));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
	    }
        }
	ehMutex_.release();
	return true;

}
/*===================================================================
                    ROUTINE: report
=================================================================== */
bool AES_GCC_EventHandlerImplementation::report (ACE_INT32 anerror, acs_aeh_specificProblem eventnumber, const ACE_TCHAR *afile, ACE_INT32 aline, const ACE_TCHAR *problemdata, const ACE_TCHAR *problemtext)
{
	ehMutex_.acquire();
	GCC_TRACE((aes_gcc_event,"%s","In report method"));

	ACE_TCHAR process[80] = {0};
	ACE_TCHAR objectr[OBJECT_OF_REF_MAX_LEN] = {0};
	ACE_TCHAR freetext[400] = {0};
	ACE_INT32 specific = 0;
	ACE_OS::sprintf(process,"%s:%d ",m_processName.c_str(),m_processId);

	ACE_OS::sprintf(freetext,"%s:%d %s",afile,aline,problemdata);

	ACE_OS::sprintf(objectr,"%s/%d",m_processName.c_str(),m_processId);
	specific = eventnumber;
	ACS_AEH_ReturnType status = ACS_AEH_error;
        if ( evreport != 0)
        {
	   status = evreport->sendEventMessage(process,					// process name
			specific,					// specific problem
			eventInfo::eventSeverity,					// perceived severity
			AES_GCC_Event::defaultProbableCause.c_str(),	// probable cause
			eventInfo::objReferenceClass,						// object class of reference
			objectr,					// object of reference
			freetext,					// problem data
			problemtext);			// problem text
	   if (status == ACS_AEH_error)
	   {
		AES_GCC_Event ex(anerror, 0, afile, aline, freetext);
		ex.setEventCode(eventnumber);
		ex.setProblemText(problemtext);
		ACE_INT32 err_code = evreport->getError();
		switch (err_code)
		{
		case ACS_AEH_syntaxError:
			GCC_TRACE((aes_gcc_event,"eventhandler: AEH Syntax Error %s",(ex.str()).c_str()));
			break;

		case ACS_AEH_eventDeliveryFailure:
			GCC_TRACE((aes_gcc_event,"eventhandler: ACS_AEH_eventDeliveryFailure %s",(ex.str()).c_str()));
			break;
		}
		ehMutex_.release();
		return false;		// event not reported
	    }
	    else if(status == ACS_AEH_ok)
	    {
		GCC_TRACE((aes_gcc_event,"Event sent to AEH"));
		GCC_TRACE((aes_gcc_event,"Event Data shown below "));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
		GCC_TRACE((aes_gcc_event,"Process name = %s",process));
		GCC_TRACE((aes_gcc_event,"Event Number = %d",specific));
		GCC_TRACE((aes_gcc_event,"Problem Data = %s",freetext));
		GCC_TRACE((aes_gcc_event,"Problem Text = %s",problemtext));
		GCC_TRACE((aes_gcc_event,"%s","===================================================="));
	    }
        }
	ehMutex_.release();
	return true;	// return reported

}
bool AES_GCC_EventHandlerImplementation::cease (AES_GCC_Event &ex, bool startUpFlg)
{
	ehMutex_.acquire();
	GCC_TRACE((aes_gcc_event,"%s","In cease method with startupflag"));
	ex.setKind(AES_GCC_Event::EV_CEASING);


	ACE_TCHAR process[80] = {0};
	ACE_TCHAR objectr[OBJECT_OF_REF_MAX_LEN] = {0};
	ACE_INT32 specific = 0;

	if(startUpFlg == true)
	{
		ACE_OS::sprintf(process,"%s:%d",m_processName.c_str(),m_processId);
		
		if ((std::string)ex.objectOfReference() == "")
		{
			ACE_OS::sprintf(objectr,"%s/%d",m_processName.c_str(),m_processId);
		}
		else
		{
			(void) ACE_OS::strncpy(objectr, ex.objectOfReference(), OBJECT_OF_REF_MAX_LEN-1);
			objectr[OBJECT_OF_REF_MAX_LEN-1] = '\0';
		}
		specific = ex.eventCode();
		ACS_AEH_ReturnType status = ACS_AEH_error;
		if ( evreport != 0)
		{
		   status = evreport->sendEventMessage(process,					// process name
				specific,					// specific problem
				eventInfo::ceaseSeverity,				// perceived severity = CEASING
				ex.probableCause(),	// probable cause
				eventInfo::objReferenceClass,						// object class of reference
				objectr,					// object of reference
				ex.problemData(),	// problem data
				ex.problemText());	// problem text
		   if (status== ACS_AEH_error)
		   {
			ACE_INT32 err_code = evreport->getError();
			switch (err_code)
			{
			case ACS_AEH_syntaxError:
			GCC_TRACE((aes_gcc_event,"During stratup eventhandler: AEH Syntax Error %s",ex.str().c_str()));
			break;

			case ACS_AEH_eventDeliveryFailure:
				GCC_TRACE((aes_gcc_event,"%s","During stratup eventhandler: ACS_AEH_eventDeliveryFailure %s",ex.str().c_str()));
				break;
			}
			ehMutex_.release();
			return false;		// alarm not reported
		    }
		    else if(status == ACS_AEH_ok)
		    {
			GCC_TRACE((aes_gcc_event,"%s","Cease alarm sent to AEH"));
			GCC_TRACE((aes_gcc_event,"%s","During stratup Cease Data shown below "));
			GCC_TRACE((aes_gcc_event,"%s","===================================================="));
			GCC_TRACE((aes_gcc_event,"Process name = %s",process));
			GCC_TRACE((aes_gcc_event,"Event Number = %d",specific));
			GCC_TRACE((aes_gcc_event,"Problem Data = %s",ex.problemData()));
			GCC_TRACE((aes_gcc_event,"Problem Text = %s",ex.problemText()));
			GCC_TRACE((aes_gcc_event,"%s","===================================================="));
		    }
		}
		ehMutex_.release();
		return true;
	}
	
	ehMutex_.release();
	return true;
}

