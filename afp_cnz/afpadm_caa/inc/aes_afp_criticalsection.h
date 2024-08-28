/*=================================================================== */
/**
   @file   	aes_afp_criticalsection.h

   @brief 	Header file for aes_afp_criticalsection class
			which is responsible for handling mutexes

   @version 1.0.0
 */
/*
   HISTORY

   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A      22/09/2011     XNADNAR       Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef aes_afp_criticalsection_h
#define aes_afp_criticalsection_h 1

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/OS.h>
#include <ace/ACE.h>
#include <ace/Thread_Mutex.h>
#include <ace/Mutex.h>
#include <memory>
using namespace std;

/*====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
class aes_afp_criticalsection 
{
	/*====================================================================
					PUBLIC DECLARATION SECTION
==================================================================== */
public:
	/*=====================================================================
					CLASS DESTRUCTOR
	==================================================================== */
	/*=================================================================== */
	/**

	  @brief           Destructor

	  @exception       none
	 */
	/*=================================================================== */
	virtual ~aes_afp_criticalsection();
	/*=================================================================== */
	/**

	@brief           	enter
						Claims a resource.

	@return          	void

	@exception       	none
	 */
	/*=================================================================== */
	void enter ();
	/*=================================================================== */
	/**

	@brief           	leave
						Releases the resource.

	@return          	void

	@exception       	none
	 */
	/*=================================================================== */
	void leave ();
	/*=================================================================== */
	/**

	@brief           	Instance
						Returns instance of aes_afp_criticalsection

	@return          	void

	@exception       	none
	 */
	/*=================================================================== */
	static aes_afp_criticalsection& Instance ();
	/*=====================================================================
						PROTECTED DECLARATION SECTION
	======================================================================== */
protected:

	/*=====================================================================
						PRIVATE DECLARATION SECTION
	======================================================================== */
private:
	/*=====================================================================
						CLASS CONSTRUCTORS
	======================================================================== */
	/*=================================================================== */
	/**
		  @brief           Default constructor
							Creates a singleton object of aes_afp_criticalsection
		  @return          void

		  @exception       none
	 */
	/*=================================================================== */
	aes_afp_criticalsection();


	aes_afp_criticalsection(aes_afp_criticalsection const &);
	void operator =(const aes_afp_criticalsection& rhs);

	/*=================================================================== */
	/**
		  @brief           	section
							ACE_Thread_Mutex lock
	 */
	/*=================================================================== */
	ACE_Thread_Mutex * section;
	/*=================================================================== */
	/**
			  @brief           	instance
								aes_afp_criticalsection instance
	 */
	/*=================================================================== */
	static std::auto_ptr<aes_afp_criticalsection> instance;
	/*=================================================================== */
	/**
			  @brief           	m_mutex
								ACE_Mutex
	 */
	/*=================================================================== */
	static ACE_Mutex m_mutex;
	/*=================================================================== */
};

#endif
