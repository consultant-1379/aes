/*=================================================================== */
/**
	@file   notificationrecord.h

	@brief
	Facilitating notification records.

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

#ifndef _NOTIFICATIONRECORD_H
#define _NOTIFICATIONRECORD_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <ace/ACE.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     NotificationRecord

                 Facilitating notification records.
 */
/*=================================================================== */
class NotificationRecord
{
	/*=====================================================================
	                        PRIVATE DECLARATION SECTION
	==================================================================== */
private:
	/*=====================================================================
		                        PRIVATE ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	/**
				      @brief   fileSize
							   -
	 */
	/*=================================================================== */
	std::string fileSize;
	/*=================================================================== */
	/**
				      @brief   messageNumber
							   -
	 */
	/*=================================================================== */
	ACE_UINT32 messageNumber;         // Unique
	/*=================================================================== */
	/**
				      @brief   timeStamp
							   -
	 */
	/*=================================================================== */
	time_t timeStamp;
	/*=================================================================== */
	/**
				      @brief   storeFileName
							   -
	 */
	/*=================================================================== */
	std::string storeFileName;          // Dup because is key

	/*=====================================================================
                            PUBLIC DECLARATION SECTION
    ==================================================================== */
public:
	/*=====================================================================
    	     	                        CLASS CONSTRUCTOR
    ==================================================================== */
	/*=================================================================== */
	/**

    	   	      @brief          NotificationRecord
    	   						  NotificationRecord constructor.

    	   		  @return          void

    	   	      @exception       none
	 */
	/*=================================================================== */
	NotificationRecord();

	/*=================================================================== */
	/**

        	   	      @brief          NotificationRecord
        	   						  NotificationRecord constructor.

        	   		  @param          nr
    								  NotificationRecord

        	   		  @return         void

        	   	      @exception      none
	 */
	/*=================================================================== */
	NotificationRecord(const NotificationRecord &nr);

	/*=====================================================================
    	     	                        CLASS DESTRUCTOR
    ==================================================================== */
	/*=================================================================== */
	/**

    	   	      @brief          ~NotificationRecord
    	   						  NotificationRecord destructor.

    	   		  @return          void

    	   	      @exception       none
	 */
	/*=================================================================== */
	~NotificationRecord();

	/*=================================================================== */
	/**

    					    	      @brief          getStoreFileName
    					    						  -

    					    	      @return         string		-

    					    	      @exception      none
	 */
	/*=================================================================== */
	std::string getStoreFileName() const;
	/*=================================================================== */
	/**

        					    	      @brief          getFileSize
        					    						  -

        					    	      @return         string	-

        					    	      @exception      none
	 */
	/*=================================================================== */
	std::string getFileSize() const;

	/*=================================================================== */
		/**

	        					    	      @brief          getMessageNumber
	        					    						  -

	        					    	      @return         ACE_UINT32		-MessageNumber

	        					    	      @exception      none
		 */
	/*=================================================================== */
	ACE_UINT32 getMessageNumber() const;

	/*=================================================================== */
	/**

        					    	      @brief          setStoreFileName
        					    						  -

        					    		  @param          name
        												  -

        					    	      @return         void

        					    	      @exception      none
	 */
	/*=================================================================== */
	void setStoreFileName(const std::string &name);
	/*=================================================================== */
	/**

        					    	      @brief          setFileSize
        					    						  -

        					    		  @param          size
        												  -

        					    	      @return         void

        					    	      @exception      none
	 */
	/*=================================================================== */
	void setFileSize(const std::string &size);
	/*=================================================================== */
	/**

        					    	      @brief          setMessageNumber
        					    						  -

        					    		  @param          n
        												  -

        					    	      @return         void

        					    	      @exception      none
	 */
	/*=================================================================== */
	void setMessageNumber(const ACE_UINT32 n);
	/*=================================================================== */
	/**

        					    	      @brief          setTime
        					    						  -

        					    		  @param          stamp
        												  -

        					    	      @return         void

        					    	      @exception      none
	 */
	/*=================================================================== */
	void setTime(time_t stamp);
	/*=================================================================== */
	/**

        					    	      @brief          getTime
        					    						  -

        					    	      @return         time_t		time

        					    	      @exception      none
	 */
	/*=================================================================== */
	time_t getTime() const;
	/*=================================================================== */

};

#endif
