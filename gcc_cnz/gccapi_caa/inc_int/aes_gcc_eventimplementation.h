/*=================================================================== */
/**
	@file   aes_gcc_eventimplementation.h

	@brief
	Creates Event object to be sent to AEH.
	And provides Setters and getters methods for its attributes.

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.
	@version 1.1.1
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       22/06/2011     XNADNAR       Initial Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef aes_gcc_eventimplementation_h
#define aes_gcc_eventimplementation_h 1
/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aeh_evreport.h"
#include "acs_aeh_types.h"
#include <iostream>
#include <string>
#include "aes_gcc_event.h"
#include <ace/ACE.h>
#include <ace/Time_Value.h>
#include <ace/System_Time.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
		@brief AES_GCC_EventImplementation
		Creates event objects and provide methods to access them.
 */
/*=================================================================== */

class AES_GCC_EventImplementation 
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*====================================================================
	                        PUBLIC ATTRIBUTES
	==================================================================== */

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

	      @brief          default constructor
						  Creates an empty event object.

	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation();
	/*=================================================================== */
	/**

	      @brief           parameter constructor
						   Creates an event object.

	      @param           anerror
	                       Error code
	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation (ACE_INT32 anerror);

	/*=================================================================== */
	/**

	      @brief           parameter constructor
						   Creates an event object.

	      @param           anerror
	                       Error code
	      @param           mycomment
						   Problem data
	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation (ACE_INT32 anerror, const ACE_TCHAR *mycomment);

	/*=================================================================== */
	/**

	@brief           parameter constructor
					Creates an event object.

	@param           anerror
						Error code
	@param           oserror
					OS error code
	@param           afile
						File name where the error occurred
	@param           aline
						Line number where the error occurred
	@param           mycomment
						Problem data
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, const ACE_TCHAR *mycomment);
	/*=================================================================== */
	/**
	@brief           parameter constructor
	Creates an event object.

	@param           anerror
						Error code
	@param           oserror
						OS error code
	@param           afile
						File name where the error occurred
	@param           aline
						Line number where the error occurred
	@param           istr
						istream with comment text
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, std::istream &istr);

	/*=================================================================== */
	/**
	@brief           Copy constructor
	Creates an event object.

	@param           ex
						Another AES_GCC_Event object
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation (const AES_GCC_EventImplementation& ex);

	/*=================================================================== */
	/**
	@brief           constructor
					Creates an event object.

	@param           problemdata
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation (const ACE_TCHAR *problemdata);

	/*=================================================================== */
	/**
	@brief         destructor
						Destroys the event object.
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	virtual ~AES_GCC_EventImplementation();

	/*=================================================================== */
	/**
	@brief           errorCode
						Retrieves stored error code.
	@return          Error code
	@exception       none
	 */
	/*=================================================================== */
	ACE_INT32 errorCode () const;

	/*=================================================================== */
	/**
	@brief           setErrorCode
						Stores new error code.
	@param           err
						New Error code
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	void setErrorCode (ACE_INT32 err);

	/*=================================================================== */
	/**
	@brief           osError
						Retrieves stored OS error code.
	@return          int
					 OS error code
	@exception       none
	 */
	/*=================================================================== */
	ACE_INT32 osError ();
	/*=================================================================== */
	/**
	@brief           setOsError
						Stores new OS error code.
	@param           err
						New OS error code
	@return          void

	@exception       none
	 */
	/*=================================================================== */
	void setOsError (ACE_INT32 err);

	/*=================================================================== */
	/**
	@brief           eventCode
						Retrieves stored event number.

	@return          acs_aeh_specificProblem
						Event number
	@exception       none
	 */
	/*=================================================================== */
	ACE_INT64 eventCode ();

	/*=================================================================== */
	/**
		@brief           setEventCode
							Stores new event number.
		@param           evnum
							New event number 23000 - 23999
		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setEventCode (acs_aeh_specificProblem evnum);
	/*=================================================================== */
	/**
		@brief           problemData
							Retrieves stored problem data.
		@return          Problem data

		@exception       none
	 */
	/*=================================================================== */
	const ACE_TCHAR * problemData () const;

	/*=================================================================== */
	/**
		@brief           setProblemData
							Stores new problem data.
		@param           problemdata
							New problem data
		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setProblemData (const ACE_TCHAR *problemdata);

	/*=================================================================== */
	/**
		@brief           problemText
							Retrieves stored problem text.

		@return          Problem text

		@exception       none
	 */
	/*=================================================================== */
	const ACE_TCHAR * problemText () const;
	/*=================================================================== */
	/**
		@brief           setProblemText
							Stores new problem text.
		@param           ptext
							New problem text
		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setProblemText (const ACE_TCHAR *ptext);
	/*=================================================================== */
	/**
		@brief           kind
							 Returns the kind of exception.

		@return          EventType
							EV_GENERAL      General exception
							EV_EVENT        Event
							EV_ALARM        Alarm
							EV_CEASING      Cease

		@exception       none
	 */
	/*=================================================================== */
	AES_GCC_Event::EventType kind ();

	/*=================================================================== */
	/**
		@brief           setKind
							sets the kind of exception.
		@param           kind
							Kind of exception
		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setKind (AES_GCC_Event::EventType kind);

	/*=================================================================== */
	/**
		@brief           counter
							Returns the number of times the exception has occurred.

		@return          Count

		@exception       none
	 */
	/*=================================================================== */
	ACE_INT32 counter ();

	/*=================================================================== */
	/**
		@brief           setCounter
							Set the number-of-exceptions counter
		@param           count
							Number of exceptions
		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setCounter (ACE_INT32 count = 0);

	/*=================================================================== */
	/**
		@brief           incCounter
							increment the counter for number-of-exceptions
		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void incCounter ();

	/*=================================================================== */
	/**
		@brief           age
							Returns the age of the object in seconds.
		@return          Age in seconds

		@exception       none
	 */
	/*=================================================================== */
	ACE_Time_Value age ();

	/*=================================================================== */
	/**
		@brief           resetAge
							Resets the age of the object to zero.
		@return          void
		@exception       none
	 */
	/*=================================================================== */
	void resetAge ();
	/*=================================================================== */
	/**
		@brief           operator <<
							Adds string 'mycomment' to problem data.
		@param           mycomment
							Problem data
		@return          AES_GCC_Event_R1

		@exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation & operator << (const ACE_TCHAR *mycomment);
	/*=================================================================== */
	/**
		@brief           operator <<
							Converts 'detail' to a string and adds it to problem data.
		@param           detail
							Problem data
		@return          AES_GCC_Event_R1

		@exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation & operator << (ACE_INT64 detail);
	/*=================================================================== */
	/**
		@brief           operator <<
							Store istream in comment text.
		@param           is
							Problem data
		@return          AES_GCC_Event_R1

		@exception       none
	 */
	/*=================================================================== */
	AES_GCC_EventImplementation & operator << (std::istream &is);

	/*=================================================================== */
	/**
		@brief           str
							Retrieves complete information about the event.

		@return          Information string

		@exception       none
	 */
	/*=================================================================== */
	std::string str ();

	const ACE_TCHAR * probableCause () const;


	void setProbableCause (const ACE_TCHAR *cause);

	bool manualCease ();

	void setManualCease (bool mCease);

	const ACE_TCHAR* objectOfReference() const;


	void setObjectOfReference(const ACE_TCHAR* objRef);


protected:


private:


private:
	/*=================================================================== */
	/**
	      @brief  oserror_

				   OS error code
	 */
	/*=================================================================== */
	ACE_INT32 oserror_;
	/*=================================================================== */
	/**
	      @brief  error_

				   error_
	 */
	/*=================================================================== */
	ACE_INT32 error_;
	/*=================================================================== */
	/**
	      @brief  file_

				   File name where the error occurred
	 */
	/*=================================================================== */
	std::string file_;
	/*=================================================================== */
	/**
	      @brief  line_

				   Line number where the error occurred
	 */
	/*=================================================================== */
	ACE_INT32 line_;
	/*=================================================================== */
	/**
	      @brief  problemdata_

				   problemdata
	 */
	/*=================================================================== */
	std::string problemdata_;
	/*=================================================================== */
	/**
	      @brief  counter_

				   Number of times this exception/event/alarm has been reported
	 */
	/*=================================================================== */
	ACE_INT32 counter_;

	/*=================================================================== */
	/**
	      @brief  kind_

				   Kind of exception
	 */
	/*=================================================================== */
	AES_GCC_Event::EventType kind_;

	/*=================================================================== */
	/**
	      @brief  event_

				  event number
	 */
	/*=================================================================== */
	ACE_INT64 event_;
	/*=================================================================== */
	/**
	      @brief  problemtext_

				  problemtext
	 */
	/*=================================================================== */
	std::string problemtext_;

	/*=================================================================== */
	/**
	      @brief  evTime

				  Time of event occurrence
	 */
	/*=================================================================== */
	ACE_Time_Value  evTime;
	/*=================================================================== */
	/**
	      @brief  cause_

				  cause of the event or alarm
	 */
	/*=================================================================== */
	std::string cause_;
	/*=================================================================== */
	/**
	      @brief  objRef_

				  object of reference
	 */
	/*=================================================================== */
	std::string objRef_;
	/*=================================================================== */
	/**
	      @brief  manualCease_

				  Flag for the alarm to be manually cease or not.
				  true 	Has been cease manually
				  false Has not been cease manually
	 */
	/*=================================================================== */
	bool manualCease_;

};
#endif
