//******************************************************************************
/*=================================================================== */
/**
	@file   aes_gcc_event_r1.h

	@brief
	The services provided by AES_GCC_Event facilitates the
	creation of an event to be sent to AES_GCC_EventHandler.
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
#ifndef aes_gcc_event_r1_h
#define aes_gcc_event_r1_h 1

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include "acs_aeh_evreport.h"
#include "acs_aeh_types.h"
#include <iostream>
#include <string>
#include <ace/ACE.h>
#include <ace/Time_Value.h>

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class AES_GCC_EventImplementation;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
		@brief AES_GCC_Event_R1
		The services provided by AES_GCC_Event facilitates the
		creation of an event to be sent to AES_GCC_EventHandler.
 */
/*=================================================================== */

class AES_GCC_Event_R1
{
	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*====================================================================
	                        PUBLIC ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
	      @brief  EventType

	        This is an enum contains constants to specify alarm,event and cease
	 */
	/*=================================================================== */
	typedef enum
	{
		// To separate event, alarm and ceasing.
		EV_GENERAL = 100,
				EV_EVENT = 101,
				EV_ALARM = 102,
				EV_CEASING=103
	} EventType;
	/*=================================================================== */
	/**
	      @brief  defaultProbableCause

				   Default probable cause is used in the constructors
	 */
	/*=================================================================== */
	static std::string defaultProbableCause;

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

	AES_GCC_Event_R1();
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
	AES_GCC_Event_R1 (ACE_INT32 anerror);

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
	AES_GCC_Event_R1 (ACE_INT32 anerror, const ACE_TCHAR *mycomment);

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
	AES_GCC_Event_R1 (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, const ACE_TCHAR *mycomment);
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
						Problem data
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	AES_GCC_Event_R1 (ACE_INT32 anerror, ACE_INT32 oserror, const ACE_TCHAR *afile, ACE_INT32 aline, std::istream &istr);
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
	AES_GCC_Event_R1 (const AES_GCC_Event_R1& ex);
	/*=================================================================== */
	/**
	@brief           operator=
					" = "operator overloading method

	@param           ex
						AES_GCC_Event object
	@return          AES_GCC_Event_R1
	@exception       none
	 */
	/*=================================================================== */
	AES_GCC_Event_R1& operator=(const AES_GCC_Event_R1& ex);

	/*=================================================================== */
	/**
	@brief           parameter constructor
						Creates an event object.

	@param           problemdata
						problemdata
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	AES_GCC_Event_R1 (const ACE_TCHAR *problemdata);

	/*=================================================================== */
	/**
	@brief         destructor
						Destroys the event object.
	@return          void
	@exception       none
	 */
	/*=================================================================== */
	virtual ~AES_GCC_Event_R1();

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
	int osError ();
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
//	acs_aeh_specificProblem eventCode ();
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
							Stores new problem data.
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
	EventType kind ();
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
	void setKind (EventType kind);
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
	AES_GCC_Event_R1 & operator << (const ACE_TCHAR *mycomment);
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
	AES_GCC_Event_R1 & operator << (ACE_INT64 detail);
	/*=================================================================== */
	/**
		@brief           operator <<
							Adds data from 'is' to problem data.
		@param           is
							Problem data
		@return          AES_GCC_Event_R1

		@exception       none
	 */
	/*=================================================================== */
	AES_GCC_Event_R1 & operator << (std::istream &is);
	/*=================================================================== */
	/**
		@brief           str
							Retrieves complete information about the event.

		@return          Information string

		@exception       none
	 */
	/*=================================================================== */
	std::string str ();
	/*=================================================================== */
	/**
		@brief           probableCause
							Retrieves stored probable cause.

		@return          Probable cause

		@exception       none
	 */
	/*=================================================================== */
	const ACE_TCHAR * probableCause () const;
	/*=================================================================== */
	/**
		@brief           setProbableCause
							Stores new probable cause.
		@param           cause
							New probable cause
		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setProbableCause (const ACE_TCHAR *cause);
	/*=================================================================== */
	/**
		@brief           manualCease
							Retrieves stored manualCease flag.

		@return          false or true

		@exception       none
	 */
	/*=================================================================== */
	bool manualCease ();
	/*=================================================================== */
	/**
		@brief           setManualCease
							Stores new value of manualCease
		@param           mCease
							New value of manualCease, true of false

		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setManualCease (bool mCease);



	/*=================================================================== */
	/**
		@brief           objectOfReference
							Retrieves stored object of reference.

		@return          Object of reference

		@exception       none
	 */
	/*=================================================================== */
	const ACE_TCHAR* objectOfReference() const;
	/*=================================================================== */
	/**
		@brief           setObjectOfReference
							Stores new object of reference.
		@param           objRef
							 New object of reference

		@return          void

		@exception       none
	 */
	/*=================================================================== */
	void setObjectOfReference(const ACE_TCHAR* objRef);

private:
	/*===================================================================
	                        PROTECTED ATTRIBUTE
	=================================================================== */
	/*=================================================================== */
	/**
	      @brief  implementation

	              Reference of AES_GCC_EventImplementation class
	 */
	/*=================================================================== */

	AES_GCC_EventImplementation * implementation;

};
#endif
