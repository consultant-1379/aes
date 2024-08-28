/*=================================================================== */
/**
	@file   recordhandler.h

	@brief
	The services provided by RecordHandler facilitates the handling of records.

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
   N/A       27/09/2011     xbhadur       Initial Release
============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef _RECORDHANDLER_H
#define _RECORDHANDLER_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <map>
#include <sstream>
#include <notificationrecord.h>
#include <ace/ACE.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     RecordHandler

                 The services provided by RecordHandler facilitates the handling of records.
 */
/*=================================================================== */
class RecordHandler
{
	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*===================================================================
		 	                PRIVATE ATTRIBUTES
	=================================================================== */
	/*=================================================================== */
	/**
				      @brief   tdb

							   Timestamp as key, and filename
	 */
	/*=================================================================== */
	std::map<const ACE_INT64, std::string> tdb;

	/*=================================================================== */
	/**
    			      @brief   db

    						   Filename as key, and its record
	 */
	/*=================================================================== */
	std::map<std::string, NotificationRecord *> db;

	/*=================================================================== */
	/**
    			      @brief   hostName
    						   -
	 */
	/*=================================================================== */
	std::string hostName;

	/*=================================================================== */
	/**
    			      @brief   fileStorePath
    						   -
	 */
	/*=================================================================== */
	std::string fileStorePath;
	/*=================================================================== */
	/**
    			      @brief   fileFullPath
    						   -
	 */
	/*=================================================================== */
	std::string fileFullPath;
	/*=================================================================== */
	/**
    			      @brief   lastTimeStamp
    						   -
	 */
	/*=================================================================== */
	time_t lastTimeStamp;
	/*=================================================================== */
	/**
    			      @brief   fileMaxAge
    						   -
	 */
	/*=================================================================== */
	ACE_UINT32 fileMaxAge; // Resend File Notification

	/*=====================================================================
                            PUBLIC DECLARATION SECTION
    ==================================================================== */
public:
	/*=================================================================== */
	/**
    			      @brief   dbItr
    						   -
	 */
	/*=================================================================== */
	typedef std::map<std::string, NotificationRecord *>::iterator dbItr;
	/*=====================================================================
    	     	                        CLASS CONSTRUCTOR
    ==================================================================== */
	/*=================================================================== */
	/**

    	   	      @brief          RecordHandler
    	   						  RecordHandler constructor.

    	   		  @return          void

    	   	      @exception       none
	 */
	/*=================================================================== */
	RecordHandler();

	/*=====================================================================
    	     	                        CLASS DESTRUCTOR
    ==================================================================== */
	/*=================================================================== */
	/**

    	   	      @brief          ~RecordHandler
    	   						  RecordHandler destructor.

    	   		  @return          void

    	   	      @exception       none
	 */
	/*=================================================================== */
	~RecordHandler();
	/*===================================================================
    	 	                      ROUTINE DECLARATION SECTION
    =================================================================== */
	/*=================================================================== */
	/**

    					    	      @brief          add
    					    						  -

    					    		  @param          fileName
    												  -
    					    	      @param          nr
    												  -
    								  @param          getTimeStamp
    												  -
    								  @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool add(const std::string &fileName, NotificationRecord *nr, bool getTimeStamp);
	/*=================================================================== */
	/**

    					    	      @brief          find
    					    						  -

    					    		  @param          fileName
    												  -
    					    	      @param          nr
    												  -

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool find(const std::string &fileName, NotificationRecord *nr)const;
	/*=================================================================== */
	/**

    					    	      @brief          get
    					    						  -

    					    		  @param          fileName
    												  -
    					    	      @param          nr
    												  -

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool get(const std::string &fileName, NotificationRecord *nr);
	/*=================================================================== */
	/**

    					    	      @brief          getFirst
    					    						  -

    					    		  @param          rec
    												  -

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool getFirst(NotificationRecord *rec);
	/*=================================================================== */
	/**

    					    	      @brief          remove
    					    						  -

    					    		  @param          fileName
    												  -

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool remove(const std::string &fileName);
	/*=================================================================== */
	/**

    					    	      @brief          removeFirst
    					    						  -

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool removeFirst();
	/*=================================================================== */
	/**

    					    	      @brief          exportSingleRecord
    					    						  -

    					    		  @param          str
    												  string
    					    	      @param          dest
    												  destination
    					    	      @param          rec
    												  NotificationRecord

    					    	      @return         true		on success
    												  false     on unsuccessful

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool exportSingleRecord(std::stringstream &str, const std::string &dest, NotificationRecord *rec);

	/*=================================================================== */
	/**

    					    	      @brief          isEmpty
    					    						  -

    					    	      @return         true		if empty
    												  false     if not empty

    					    	      @exception      none
	 */
	/*=================================================================== */
	bool isEmpty() const;
	/*=================================================================== */
	/**

    					    	      @brief          setHostName
    					    						  -

    					    		  @param          name
    												  -

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void setHostName(const std::string &name);
	/*=================================================================== */
	/**

    					    	      @brief          setFileStorePath
    					    						  -

    					    		  @param          path
    												  -

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void setFileStorePath(const std::string &path);
	/*=================================================================== */
	/**

    					    	      @brief          setFileFullPath
    					    						  -

    					    		  @param          path
    												  -

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void setFileFullPath(const std::string &path);
	/*=================================================================== */
	/**

    					    	      @brief          setFileMaxAge
    					    						  -

    					    		  @param          maxAge
    												  -

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void setFileMaxAge(const ACE_UINT32 &maxAge);
	/*=================================================================== */
	/**

    					    	      @brief          getHostName
    					    						  -

    					    		  @param          hName
    												  -

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void getHostName(std::string &hName) const;
	/*=================================================================== */
	/**

    					    	      @brief          getFileStorePath
    					    						  -

    					    		  @param          path
    												  path

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void getFileStorePath(std::string &path) const;
	/*=================================================================== */
	/**

    					    	      @brief          getFileFullPath
    					    						  -

    					    		  @param          path
    												  path

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void getFileFullPath(std::string &path) const;
	/*=================================================================== */
	/**

    					    	      @brief          getFileMaxAge
    					    						  -

    					    		  @param          maxAge
    												  -

    					    	      @return         void

    					    	      @exception      none
	 */
	/*=================================================================== */
	void getFileMaxAge(ACE_UINT32 &maxAge) const;
	/*=================================================================== */
	/**

    					    	      @brief          size
    					    						  -

    					    	      @return         ACE_UINT32	-size

    					    	      @exception      none
	 */
	/*=================================================================== */
	ACE_UINT32 size();
};


#endif
