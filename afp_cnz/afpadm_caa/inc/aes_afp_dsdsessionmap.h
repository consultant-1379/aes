/*=================================================================== */
/**
   @file   	aes_afp_dsdsessionmap.h

   @brief 	Header file for aes_afp_dsdsessionmap class which is
			responsible for used dsd connections
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
#ifndef aes_afp_dsdsessionmap_h
#define aes_afp_dsdsessionmap_h 1
/*=====================================================================
                          INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/OS.h>
#include <ACS_DSD_Session.h>
#include <utility>
#include <map>
#include <string>
#include <memory>
#include <ace/Mutex.h>
/*=====================================================================
                          TYPE DECLARATION SECTION
==================================================================== */
typedef std::map< std::string , ACS_DSD_Session* , std::less<std::string>  > SessionMap;

typedef std::pair< std::string , ACS_DSD_Session*  > SessionPair;

/*=====================================================================
                          CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     aes_afp_datablock
                 This class is responsible for used dsd connections.
 */
/*=================================================================== */

class aes_afp_dsdsessionmap 
{
	/*=====================================================================
	                          PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
						CLASS DESTRUCTOR
	======================================================================= */
	/*=================================================================== */
	/**

		  @brief          Virtual Destructor

		  @exception       none
	 */
	/*=================================================================== */
	virtual ~aes_afp_dsdsessionmap();
	/*=================================================================== */
	/**

  	@brief           	Instance
  						Singleton instance of threadmap.
  	@return          	void

  	@exception       	none
	 */
	/*=================================================================== */
	static aes_afp_dsdsessionmap& Instance ();
	/*=================================================================== */
	/**

  	@brief           	insert
  						Insert a DSD session in the map.
  	@param				name
  						name
  	@param				session
						Pointer to ACS_DSD_Session object.
  	@return          	"true" on success else "false"

  	@exception       	none
	 */
	/*=================================================================== */
	bool insert (const std::string& name, ACS_DSD_Session* session);
	/*=================================================================== */
	/**

    	@brief           	remove
    						Remove a DSD session from the map.
    	@param				name
    						name
    	@return          	"true" on success else "false"
    	@exception       	none
	 */
	/*=================================================================== */
	bool remove (const std::string& name);
	/*=================================================================== */
	/**

    	@brief           	searchSession
    						Find a particular DSD session.
    	@param				name
    						name
    	@return          	ACS_DSD_Session object from the map
    	@exception       	none
	 */
	/*=================================================================== */
	ACS_DSD_Session* searchSession (const std::string& name);
private:

	aes_afp_dsdsessionmap();
	aes_afp_dsdsessionmap(aes_afp_dsdsessionmap const &);
	void operator =(const aes_afp_dsdsessionmap& rhs);


	/*=================================================================== */
	/**

	    	@brief           	instance_
	    						Static instance of aes_afp_dsdsessionmap
	 */
	/*=================================================================== */
	static std::auto_ptr<aes_afp_dsdsessionmap> instance_;
	/*=================================================================== */
	/**

	    	@brief           	sessionMap_
	    						Map of name and dsd session object
	 */
	/*=================================================================== */
	SessionMap sessionMap_;
	/*=================================================================== */
	/**

	    	@brief           	sessionPair_
	    						name and dsd session objects pair
	 */
	/*=================================================================== */
	SessionPair sessionPair_;
	/*=================================================================== */
	static ACE_Mutex m_mutex;
	/*=================================================================== */
};
#endif
