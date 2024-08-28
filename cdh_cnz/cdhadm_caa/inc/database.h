/*=================================================================== */
/**
	@file   database.h

	@brief
	The services provided by DataBase facilitates the handling of a DataBase.

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
#ifndef DATABASE_H
#define DATABASE_H

/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <map>
#include <string>
#include <recordhandler.h>
#include <notificationrecord.h>
#include <ace/ACE.h>
/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     DataBase

                 Handling of database
*/
/*=================================================================== */
class DataBase
{
/*=====================================================================
                  PRIVATE DECLARATION SECTION
==================================================================== */
private:
	/*====================================================================
	                        PRIVATE ATTRIBUTES
	==================================================================== */
	/*=================================================================== */
	   /**
	      @brief   dbPath

	               Path to the database directory
	   */
	/*=================================================================== */
    std::string dbPath;

    /*=================================================================== */
    	   /**
    	      @brief   db

    	              database directory
    	   */
    /*=================================================================== */
    std::map<std::string, RecordHandler *> db;

    /*=================================================================== */
    	   /**
    	      @brief   dbPtr

    	               database pointer
    	   */
    /*=================================================================== */
    static DataBase *dbPtr;

    /*===================================================================
                            ROUTINE DECLARATION SECTION
    =================================================================== */

	/*=================================================================== */
	/**

	      @brief          init
						  initializes the semaphore.

	      @return          true    Always returning true.

	      @exception       none
	 */
	/*=================================================================== */
    //bool init();

	/*=================================================================== */
	/**

	      @brief          restore
						  restoring database.

	      @return          true    to indicate that everything is OK.
						   false   to indicate that something went wrong.

	      @exception       none
	 */
	/*=================================================================== */
    bool restore();
//    bool storeOnFile();

	/*=================================================================== */
	/**

	      @brief          createRecord
						  Creates a record.

		  @param           	dest
  							destination
  		  @param           	msg
  							message
		  @param           	nr
  							pointer to notification record.
  		  @param           	host
  							host.
		  @param           	storePath
  							path
  		  @param           	fullPath
  							path

	      @return          true    to indicate that everything is OK.
						   false   to indicate that something went wrong.

	      @exception       none
	 */
	/*=================================================================== */
    bool createRecord(std::string &dest, const std::string &msg, NotificationRecord *nr,
                      std::string &host, std::string &storePath, std::string &fullPath);
    

	/*=================================================================== */
	/**

	      @brief          appendRecordToFile
						  appends the record to file.

		  @param           	dest
  							destination
  		  @param           	fileName
  							fileName
		  @param           	fileSize
  							size of the file
  		  @param           	rec
  							record to be appended

	      @return          true    to indicate that everything is OK.
						   false   to indicate that something went wrong.

	      @exception       none
	 */
	/*=================================================================== */
    bool appendRecordToFile(const std::string &dest, const std::string &fileName,
                            const std::string &fileSize, NotificationRecord *rec);


    /*=====================================================================
    	                        CLASS CONSTRUCTOR
   	==================================================================== */
	/*=================================================================== */
	/**

	      @brief          DataBase
						  DataBase constructor.

		  @return          void

	      @exception       none
	 */
	/*=================================================================== */
    DataBase();

/*=====================================================================
                       PUBLIC DECLARATION SECTION
==================================================================== */
public:
    /*=====================================================================
    	                        CLASS DESTRUCTOR
    ==================================================================== */
	/*=================================================================== */
	/**

	      @brief          ~DataBase
						  Destructor of DataBase.

	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
    ~DataBase();

    /*===================================================================
                               ROUTINE DECLARATION SECTION
     =================================================================== */

	/*=================================================================== */
	/**

	      @brief          instance
						  Creates a new instance to the database if created else previous instance

	      @return          returns a pointer to the database.

	      @exception       none
	 */
	/*=================================================================== */
    static DataBase *instance();

	/*=================================================================== */
	/**

	      @brief          add
						  To add the record.

  		  @param           	dest
  							destination.
		  @param           	fileName
  							filename
  		  @param           	fileSize
  							The size of the file.
		  @param           	nr
  							pointer to the record
  		  @param           	host
  							host
		  @param           	storePath
  							path
  		  @param           	fullPath
  							path.
		  @param           	writeToFile
  							If set to false, the record will not be written to disk.
		  @param           	maxAge
  							age

	      @return          true    Indicate if the record is added.
						   false   Indicate if the record is not added.

	      @exception       none
	 */
	/*=================================================================== */
    bool add(const std::string &dest, const std::string &fileName, const std::string &fileSize,
             NotificationRecord *nr,  const std::string &host,     const std::string &storePath,
             const std::string &fullPath, bool writeToFile = true, ACE_UINT32 maxAge = 0);

	/*=================================================================== */
	/**

	      @brief          get
						  get the record.

		  @param           	dest
  							destination.
		  @param           	fileName
  							filename
  		  @param           	hostName
  							host
		  @param           	storePath
  							path
  		  @param           	fullPath
  							path.
		  @param           	rec
  							pointer to record

	      @return          true    to indicate if the record is retrived.
						   false   to indicate if the record is not retrived.

	      @exception       none
	 */
	/*=================================================================== */
    bool get(const std::string &dest, const std::string &fileName, std::string &hostName,
             std::string &storePath, std::string &fullPath, NotificationRecord *rec);

	/*=================================================================== */
	/**

	      @brief          getFirst
						  getfirst.

		  @param           	dest
  							destination
  		  @param           	rec
  							pointer to record

	      @return          true    to indicate that everything is OK.
						   false   to indicate that something went wrong.

	      @exception       none
	 */
	/*=================================================================== */
    
    bool getFirst(const std::string &dest, NotificationRecord *rec);

	/*=================================================================== */
	/**

	      @brief          getFirst
						  getFirst

		  @param           	dest
  							destination.
		  @param           	fileName
  							filename
  		  @param           	fileSize
  							The size of the file.
		  @param           	hostName
  							host
		  @param           	storePath
  							path
  		  @param           	fullPath
  							path.
		  @param           	rec
  							pointer to the record

	      @return          true    to indicate that everything is OK.
						   false   to indicate that something went wrong.

	      @exception       none
	 */
	/*=================================================================== */
    
    bool getFirst(const std::string &dest, std::string &hostName, std::string &storePath,
                  std::string &fullPath, NotificationRecord *rec);


	/*=================================================================== */
	/**

	      @brief          remove
						  Remove the file from database.

		  @param           	dest
  							destination
  		  @param           	fileName
  							file to be removed.

	      @return          true    to indicate file is removed.
						   false   to indicate file is not removed.

	      @exception       none
	 */
	/*=================================================================== */
    bool remove(const std::string &dest, const std::string &fileName);
    

	/*=================================================================== */
	/**

	      @brief          removeFirst
						  remove.

		  @param           	dest
  							destination

	      @return          true    if removed.
						   false   if not removed.

	      @exception       none
	 */
	/*=================================================================== */
    bool removeFirst(const std::string &dest);


	/*=================================================================== */
	/**

	      @brief          isEmpty
						  checks whether the database is empty with respect to the parameter dest.

		  @param           dest
						   destination

	      @return          true    to indicate database is empty
						   false   to indicate database is not empty

	      @exception       none
	 */
	/*=================================================================== */
    bool isEmpty(const std::string &dest);


	/*=================================================================== */
	/**

	      @brief          size
						  gives size

		  @param          dest
  						  destiantion

	      @return          ACE_INT32	returns size
						   -1			if empty

	      @exception       none
	 */
	/*=================================================================== */
    ACE_INT32 size(const std::string &dest);


	/*=================================================================== */
	/**

	      @brief          getFileStorePath
						  get file path.

		  @param           	dest
  							destination
  		  @param           	fileStorePath
  							path of file

	      @return          true    to indicate that fileStorePath is given.
						   false   to indicate that fileStorePath is not given.

	      @exception       none
	 */
	/*=================================================================== */
    bool getFileStorePath(const std::string &dest, std::string &fileStorePath);

	void convertToLinuxFormat(std::string &orgPath, std::string &convPath);

	/*=================================================================== */
	/**

	      @brief         convertToLinuxFormatFFP 
						  to convert file path from windows format to linux format.

		  @param           	orgPath
  						 	path in windows format.	
  		  @param                convPath	
  							converted linux path of file

	      @return         void 

	      @exception       none
	 */
	/*=================================================================== */
	void convertToLinuxFormatFFP(std::string &orgPath, std::string &convPath);
	
	/*=================================================================== */
	/**

	      @brief          getFileFullPath
						  get file path.

		  @param           	dest
  							destination
  		  @param           	fileFullPath
  							path of file

	      @return          true    to indicate that fileFullPath is given.
						   false   to indicate that fileFullPath is not given.

	      @exception       none
	 */
	/*=================================================================== */
    bool getFileFullPath(const std::string &dest, std::string &fileFullPath);


	/*=================================================================== */
	/**

	      @brief          getHostName
						  to get host name.

		  @param           	dest
  							destination
  		  @param           	hostName
  							host (in and out)

	      @return          true    to indicate that hostName is given.
						   false   to indicate that hostName is not given.

	      @exception       none
	 */
	/*=================================================================== */
    bool getHostName(const std::string &dest, std::string &hostName);


	/*=================================================================== */
	/**

	      @brief          getFileMaxAge
						  to get MaxAge

		  @param           	dest
  							destiantion
  		  @param           	maxAge
  							maxAge(in and out)

	      @return          true    to indicate if maxAge is given.
						   false   to indicate if maxAge is not given.

	      @exception       none
	 */
	/*=================================================================== */
    bool getFileMaxAge(const std::string &dest, ACE_UINT32 &maxAge);


	/*=================================================================== */
	/**

	      @brief          createDestinationEntry
						  Creates an entry into destination.

		  @param           	dest
  							destiantion
  		  @param           	fileStorePath
  							path.
  		  @param           	fileFullPath
  							path.
		  @param           	hostName
  							host name
  		  @param           	maxAge
  							maxAge that needs to be set.

	      @return          true    to indicate insert operation successfull.
						   false   to indicate insert operation unsuccessfull.

	      @exception       none
	 */
	/*=================================================================== */
    bool createDestinationEntry(const std::string &dest,
                                const std::string &fileStorePath,
                                const std::string &fileFullPath,
                                const std::string &hostName,
                                const ACE_UINT32 maxAge = 0);

	/*=================================================================== */
	/**

	      @brief          destroyDestinationEntry
						  to destroy the destination.

		  @param           	dest
  							Destination to be destroyed

	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
    void destroyDestinationEntry(const std::string &dest);
    

	/*=================================================================== */
	/**

	      @brief          readParameters
						  To get the parameters.

		  @param           	path
  							path of which parameters are required.

	      @return          true    to indicate parameters are retrived.
						   false   to indicate parameters are not retrived.

	      @exception       none
	 */
	/*=================================================================== */
    bool readParameters(std::string &path);


	/*=================================================================== */
	/**

	      @brief         strToConvert 
						  To convert into uppercase the destination name.

		  @param     stringToModify	
  							path of which conversion is required.

		  @param     destName 
  							destination name.
	      @return          string   
						   Returns converted string. 

	      @exception       none
	 */
	/*=================================================================== */
    std::string strToConvert(std::string stringToModify, std::string destName );
};

#endif
