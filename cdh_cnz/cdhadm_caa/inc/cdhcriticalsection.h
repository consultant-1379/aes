/*=================================================================== */
/**
	@file   cdhcriticalsection.h

	@brief
	The services provided by CDHCriticalSection facilitates the maintainance of
	criticalsection.

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
   N/A       19/09/2011     xbhadur       Initial Release
============================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef CDHCRITICALSECTION_H
#define CDHCRITICALSECTION_H

/*====================================================================
                     INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/ACE.h>
#include<memory>
#include <ace/Mutex.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */
/*===================================================================*/
/**
		@brief CDHCriticalSection
	The services provided by CDHCriticalSection facilities criticalsection creation
 */
/*=================================================================== */

class CDHCriticalSection 
{
/*===================================================================
                    PRIVATE DECLARATION SECTION
=================================================================== */
private:

/*====================================================================
	                PUBLIC ATTRIBUTES
==================================================================== */

/*=================================================================== */
   /**
		      @brief   section
		               Detailed description
   */
/*=================================================================== */
	ACE_thread_mutex_t section;
    
/*=================================================================== */
   /**
		      @brief   instancePtr
		               Detailed description
   */
/*=================================================================== */
	static std::auto_ptr<CDHCriticalSection> instancePtr;//instance;

/*===================================================================
                    PROTECTED DECLARATION SECTION
=================================================================== */
protected:
/*=====================================================================
                      CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
/**

          @brief           CDHCriticalSection
                           constructor of CDHCriticalSection

          @return          void

          @exception       none
 */
/*=================================================================== */
    CDHCriticalSection();       // Hide the constructor
    CDHCriticalSection(CDHCriticalSection const &);
    CDHCriticalSection operator =(const CDHCriticalSection& rhs);

    
/*=====================================================================
    	            PUBLIC DECLARATION SECTION
=================================================================== */
public:
/*===================================================================
                            CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
/**

          @brief           ~CDHCriticalSection
                           Destructor of CDHCriticalSection


          @return          void

          @exception       none
*/
/*=================================================================== */
    virtual ~CDHCriticalSection();

    /*=================================================================== */
    /**

    	      @brief		enter
    						Enter the critical section (Claims a resource).

    	      @return           void

    	      @exception       none
     */
    /*=================================================================== */
       void enter ();

    /*=================================================================== */
    /**

    	      @brief		leave
    						leave critical section (Releases the resource).


    	      @return           void

    	      @exception       none
     */
    /*=================================================================== */
    void leave ();

    /*=================================================================== */
    /**

    	      @brief		instance
    						Creates instance if previously not created..

    	      @return           CDHCriticalSection*     Returns the pointer to instance which
														has been created or preivoiusly existed.

    	      @exception       none
     */
    /*=================================================================== */
    static CDHCriticalSection& instance ();
    /*=================================================================== */
    /*=================================================================== */
	static ACE_Mutex m_mutex;
    /*=================================================================== */
};

#endif
