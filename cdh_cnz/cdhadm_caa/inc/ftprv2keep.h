/*=================================================================== */
/**
	@file   ftprv2keep.h

	@brief
	The services provided by FTPRV2SubKeep facilitates FTP transfer mode for responding type.

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
#ifndef FTPRV2KEEP_H
#define FTPRV2KEEP_H
/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <list>
#include <vector>
#include <aes_cdh_resultcode.h>
#include <ace/ACE.h>

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     FTPRV2SubKeep

                This class is a help class to hold information about a sub keep directory
				and stored in a STL-list administrated by the class FTPRV2Keep
 */
/*=================================================================== */
class FTPRV2SubKeep
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:

	/*=====================================================================
		     	                        CLASS CONSTRUCTOR
		==================================================================== */
	/*=================================================================== */
	/**

		   	      @brief          FTPRV2SubKeep
		   						  FTPRV2SubKeep constructor.

				  @param          nCreateTime
								  -
				  @param          pSubKeepName
								  -

		   		  @return          void

		   	      @exception       none
	 */
	/*=================================================================== */
	FTPRV2SubKeep(const ACE_INT64 nCreateTime, const ACE_TCHAR* pSubKeepName)
	{
		createTime = nCreateTime;
		subKeepName = pSubKeepName;
	};

	/*=================================================================== */
	/**

   	   	      @brief          FTPRV2SubKeep
   	   						  FTPRV2SubKeep constructor.

			  @param          SubKeep

   	   		  @return          void

   	   	      @exception       none
	 */
	/*=================================================================== */
	FTPRV2SubKeep(const FTPRV2SubKeep& SubKeep)
	{
		createTime = SubKeep.createTime;
		subKeepName = SubKeep.subKeepName;
	};

	/*=====================================================================
                           PUBLIC DECLARATION SECTION
   ==================================================================== */
public:
	/*=================================================================== */
	/**

					    	      @brief          operator <
					    						  Overloaded operator(s) for comparison/sorting

					    		  @param          Sub1
												  -
					      		  @param          Sub2
												  -

					    	      @return         true		on success
												  false     on unsuccessful

					    	      @exception       none
	 */
	/*=================================================================== */
	friend bool operator < (const FTPRV2SubKeep& Sub1, const FTPRV2SubKeep& Sub2)
	{
		return (Sub1.createTime < Sub2.createTime);
	};

	/*=====================================================================
                           PUBLIC DECLARATION SECTION
   ==================================================================== */
public:
	/*====================================================================
		                       PRIVATE ATTRIBUTES
	==================================================================== */

	/*=================================================================== */
	/**
			      @brief   createTime
						   -
	 */
	/*=================================================================== */
	ACE_INT64     createTime;
	/*=================================================================== */
	/**
			      @brief   subKeepName
						   -
	 */
	/*=================================================================== */
	std::string  subKeepName;
};
/*===================================================================*/
/**
      @brief     FTPRV2Keep

                This class administrates a STL-list of FTPRV2SubKeep objects and in turn
				also administrates creation and deleting of sub keep directories
 */
/*=================================================================== */
class FTPRV2Keep
{

	/*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:
	/*=====================================================================
		     	                        CLASS CONSTRUCTOR
		==================================================================== */
	/*=================================================================== */
	/**

		   	      @brief          FTPRV2Keep
		   						  FTPRV2Keep constructor.

		   		  @return          void

		   	      @exception       none
	 */
	/*=================================================================== */
	FTPRV2Keep();
	/*=====================================================================
   	     	                        CLASS DESTRUCTOR
   	==================================================================== */
	/*=================================================================== */
	/**

   	   	      @brief          ~FTPRV2Keep
   	   						  FTPRV2Keep destructor.

   	   		  @return          void

   	   	      @exception       none
	 */
	/*=================================================================== */
	~FTPRV2Keep();
	/*=================================================================== */
	/**

					    	      @brief          define
					    						  -

					    		  @param          rootDir
												  -
					      		  @param          keepTime
												  -
					    	      @param          recovery
												  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode define(const ACE_TCHAR* rootDir,
			const ACE_UINT64 keepTime,
			const bool recovery);
	/*=================================================================== */
	/**

					    	      @brief          remove
					    						  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode remove(void);
	/*=================================================================== */
	/**

					    	      @brief          getAttributes
					    						  -

					    		  @param          attr
												  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode getAttributes(std::vector<std::string>& attr);
	/*=================================================================== */
	/**

					    	      @brief          getSingleAttr
					    						  -

					    		  @param          opt
												  -
					      		  @param          value
												  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode getSingleAttr(const ACE_TCHAR* opt, void* value);
	/*=================================================================== */
	/**

					    	      @brief          changeParameters
					    						  -

					    		  @param          keep
												  -

					      	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode changeParameters(FTPRV2Keep* keep);
	/*=================================================================== */
	/**

					    	      @brief          checkConnection
					    						  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode checkConnection(void);

	/*=====================================================================
                           PRIVATE DECLARATION SECTION
   ==================================================================== */
private:
	/*=================================================================== */
	/**

					    	      @brief          readKeepTimeSliceParameter
					    						  -


					    	      @return         true		on success
												  false     on unsuccessful

					    	      @exception      none
	 */
	/*=================================================================== */
	bool readKeepTimeSliceParameter(void);
	/*=================================================================== */
	/**

					    	      @brief          createKeepDir
					    						  -

					    		  @param          bReadKeepDirContents
												  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode createKeepDir(const bool bReadKeepDirContents = true);
	/*=================================================================== */
	/**

					    	      @brief          createSubKeepDir
					    						  -

					    		  @param          pSubKeepDir
												  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	//AES_CDH_ResultCode createSubKeepDir(const ACE_TCHAR* pSubKeepDir);
	/*=================================================================== */
	/**

					    	      @brief          cleanUpBaseDir
					    						  -

					    		  @param          pDir
												  -
					      		  @param          nBaseDir
												  -
					    	      @param          bDeleteBaseDir
												  -
					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode cleanUpBaseDir(const ACE_TCHAR* pDir,
			const ACE_INT32   nBaseDir,
			const bool  bDeleteBaseDir = true);
	/*=================================================================== */
	/**

					    	      @brief          deleteDir
					    						  -

					    		  @param          pDir
												  -

					    	      @return         AES_CDH_ResultCode	status

					    	      @exception      none
	 */
	/*=================================================================== */
	AES_CDH_ResultCode deleteDir(const ACE_TCHAR* pDir);
	/*=================================================================== */
	/**

					    	      @brief          moveFilesToSubKeep
					    						  -

					    	      @return         void

					    	      @exception      none
	 */
	/*=================================================================== */
	void moveFilesToSubKeep(void);
	/*=================================================================== */
	/**

					    	      @brief          readKeepDirContents
					    						  -

					    	      @return         true		on success
												  false     on unsuccessful

					    	      @exception      none
	 */
	/*=================================================================== */
	bool readKeepDirContents(void);
	/*=================================================================== */
	/**

					    	      @brief          createNewSubKeep
					    						  -

					    	      @return         FTPRV2SubKeep		-

					    	      @exception      none
	 */
	/*=================================================================== */
	//FTPRV2SubKeep* createNewSubKeep(void);
	/*=================================================================== */
	/**

					    	      @brief          getCurrentSubKeep
					    						  -

					    	      @return         FTPRV2SubKeep		-

					    	      @exception      none
	 */
	/*=================================================================== */
	//FTPRV2SubKeep* getCurrentSubKeep(void);
	/*=================================================================== */
	/**

					    	      @brief          getLatestCreatedSubKeep
					    						  -

					    	      @return         FTPRV2SubKeep		-

					    	      @exception      none
	 */
	/*=================================================================== */
	//FTPRV2SubKeep* getLatestCreatedSubKeep(void);
	/*=================================================================== */
	/**

					    	      @brief          deleteOldSubKeep
					    						  -

					    	      @return         void

					    	      @exception      none
	 */
	/*=================================================================== */
	void deleteOldSubKeep(void);
	/*=================================================================== */
	/**

					    	      @brief          getKeepTime
												  -

					    	      @return         ACE_UINT64		-

					    	      @exception      none
	 */
	/*=================================================================== */
	ACE_UINT64 getKeepTime(void) const;
	/*=================================================================== */
	/**

					    	      @brief          setKeepTime
					    						  -

					    		  @param          newKeepTime
												  -

					    	      @return         void

					    	      @exception      none
	 */
	/*=================================================================== */
	void setKeepTime(const ACE_UINT64 newKeepTime);
	/*=================================================================== */
	/**

					    	      @brief          getCurrentHour
					    						  -

					    	      @return         ACE_UINT64		-

					    	      @exception      none
	 */
	/*=================================================================== */
	//ACE_UINT64 getCurrentHour(void);
	/*=================================================================== */
	/**

					    	      @brief          getFileCreateHour
					    						  -

					    		  @param          nFileTime
												  -

					    	      @return         ACE_UINT64		-

					    	      @exception      none
	 */
	/*=================================================================== */
	//ACE_UINT64 getFileCreateHour(ACE_INT64 nFileTime);
	// ACE_UINT64 getFileCreateHour(ACE_INT64 lpFileTime);//LPFILETIME lpFileTime); to be ported

	/*=====================================================================
                           PRIVATE DECLARATION SECTION
   ==================================================================== */
private:
	/*====================================================================
		                       PRIVATE ATTRIBUTES
	==================================================================== */

	/*=================================================================== */
	/**
			      @brief   keepTime
						   -
	 */
	/*=================================================================== */
	ACE_UINT64 keepTime;

	/*=================================================================== */
	/**
			      @brief   keepTimeSlice
						   -
	 */
	/*=================================================================== */
	ACE_UINT64 keepTimeSlice;

	/*=================================================================== */
	/**
			      @brief   rootDir
						   -
	 */
	/*=================================================================== */
	std::string rootDir;

	/*=================================================================== */
	/**
			      @brief   keepDir
						   -
	 */
	/*=================================================================== */
	std::string keepDir;

	/*=================================================================== */
	/**
			      @brief   subKeepItemList
						   -
	 */
	/*=================================================================== */
	std::list<FTPRV2SubKeep> subKeepItemList;
};

#endif
