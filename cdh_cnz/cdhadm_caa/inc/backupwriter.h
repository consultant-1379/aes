/*=================================================================== */
/**
	@file   backupwriter.h

	@brief
	The services provided by BackupWriter facilitates the backup storage.

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
#ifndef BACKUPWRITER_H
#define BACKUPWRITER_H
/*====================================================================
                   INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <aes_cdh_resultcode.h>
#include <ace/ACE.h>
#include <AES_DBO_DataBlock.h>

/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief NO_INHERITANCE
 */
/*=================================================================== */
// these are defined in accctrl.h, hence defined here
#define NO_INHERITANCE 0x0
/*=================================================================== */
/**
   @brief NO_INHERITANCE
 */
/*=================================================================== */
#define SUB_OBJECTS_ONLY_INHERIT            0x1
/*=================================================================== */
/**
   @brief NO_INHERITANCE
 */
/*=================================================================== */
#define SUB_CONTAINERS_ONLY_INHERIT         0x2
/*=================================================================== */
/**
   @brief NO_INHERITANCE
 */
/*=================================================================== */
#define SUB_CONTAINERS_AND_OBJECTS_INHERIT  0x3
/*=================================================================== */
/**
   @brief NO_INHERITANCE
 */
/*=================================================================== */
#define INHERIT_NO_PROPAGATE                0x4
/*=================================================================== */
/**
   @brief NO_INHERITANCE
 */
/*=================================================================== */
#define INHERIT_ONLY                        0x8
/*=================================================================== */
/**
   @brief NO_INHERITANCE
 */
/*=================================================================== */



using namespace std;

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
//class AES_DBO_DataBlock;
class FTPRV2Backup;
class SSHFTPRV2Backup;

/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */
/*===================================================================*/
/**
		@brief BackupWriter
	The services provided by BackupWriter facilities backup storage.
 */
/*=================================================================== */
//##ModelId=3DF49902017D
class BackupWriter
{
/*=====================================================================
  	  	            PUBLIC DECLARATION SECTION
==================================================================== */
public:

/*===================================================================*/
/**
		@brief FileEntry
 */
/*=================================================================== */
	//##ModelId=3E2FD3F80374
    class FileEntry 
    {

 /*=====================================================================
  	  	            PUBLIC DECLARATION SECTION
 ==================================================================== */
    public:
/*=================================================================== */
   /**
    		      @brief   CDRBid
    		               Detailed description
   */
 /*=================================================================== */
        //##ModelId=3E2FD9A80088
        ACE_UINT32 CDRBid;
/*=================================================================== */
    /**
        	      @brief   fileSize
        	               Detailed description
    */
/*=================================================================== */
        //##ModelId=3E2FD9B30305
        ACE_UINT32 fileSize;
    };
    
/*===================================================================
                    PRIVATE DECLARATION SECTION
=================================================================== */
private:

/*=================================================================== */
     /**
    	      @brief   hFile
    	               Detailed description
     */
/*=================================================================== */
    //##ModelId=3E253E9E02BC
    ACE_HANDLE hFile;

/*=================================================================== */
    /**
    	      @brief   hMapFile
    	               Detailed description
    */
/*=================================================================== */
    //##ModelId=3E2EB6E00274
    ACE_HANDLE hMapFile;

/*=================================================================== */
    /**
    	      @brief   hTarget
    	               Detailed description
    */
/*=================================================================== */
    //##ModelId=3E2EB7050227
    ACE_HANDLE hTarget;

/*=================================================================== */
     /**
    	      @brief   mapAddr
    	               Detailed description
     */
/*=================================================================== */
    //##ModelId=3E2EB74B0281
    void *mapAddr;

/*=================================================================== */
    /**
    	      @brief   path
    	               Detailed description
    */
/*=================================================================== */
    //##ModelId=3E315CAB0348
    string path;


    /*=================================================================== */
    /**

    	      @brief		fetchFileNames
    						Fetch teh file names.

    		  @param           	vec
    							Vector from which filenames to be fetched.

    	      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

    	      @exception       none
     */
    /*=================================================================== */
	//##ModelId=3E7DCC5B00B4
	bool fetchFileNames(std::vector<std::string>& vec);

/*=================================================================== */
   /**
		      @brief   FTPRV2BackupPtr
		               Detailed description
   */
/*=================================================================== */
    //##ModelId=3E4396EA00F3
    FTPRV2Backup* FTPRV2BackupPtr;
    
/*=================================================================== */
    /**
    	      @brief   theSourceId
    	               Detailed description
   */
/*=================================================================== */
    //##ModelId=3E3155D300F0
    string theSourceId;

/*=================================================================== */
      /**
    	      @brief   SwriteFile
    	               Detailed description
      */
 /*=================================================================== */
    string SwriteFile;

/*=================================================================== */
     /**
    	      @brief   fileEntry
    	               Detailed description
     */
/*=================================================================== */
    //##ModelId=3E2FDF5B01D1
    FileEntry fileEntry;
    
/*=================================================================== */
    /**
    	      @brief   SSHFTPRV2BackupPtr
    	               Detailed description
    */
/*=================================================================== */
    SSHFTPRV2Backup* SSHFTPRV2BackupPtr;

/*=====================================================================
                      PUBLIC DECLARATION SECTION
==================================================================== */
public:
/*====================================================================
                            PUBLIC ATTRIBUTES
==================================================================== */


    /*=================================================================== */
    /**

    	      @brief		getFileSize
    						Gets the size of the block.


    	      @return          	ACE_UINT64     Retuirns the size of file

    	      @exception       none
     */
    /*=================================================================== */
    //##ModelId=3E5495B1015C
    ACE_UINT64 getFileSize();


    /*=================================================================== */
    /**

    	      @brief		getFilePath
    						Get the path of the file.

    		  @param           	vec
    							The vector where the filenames are present.
    		  @param           	string
    							File name for which path should be known.

    	      @return           string     Path of the file.

    	      @exception       none
     */
    /*=================================================================== */
    //##ModelId=3E3155160120
    const string getFilePath(std::vector<std::string> &vec,string);
    const string getFilePath();
    
/*=====================================================================
                       CLASS CONSTRUCTOR
==================================================================== */
/*=================================================================== */
/**

          @brief           BackupWriter
                           constructor of BackupWriter

          @return          void

          @exception       none
*/
/*=================================================================== */
    //##ModelId=3DF4991801ED
    BackupWriter();

/*===================================================================
                         CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
/**

          @brief           ~BackupWriter
							Destructor of BackupWriter

          @return          void

          @exception       none
*/
/*=================================================================== */
    //##ModelId=3DF4992202C3
    virtual ~BackupWriter();

    /*=================================================================== */
    /**

    	      @brief		open
    						Locate the file with the same name as sourceId, Get access to it as a memory mapped file, and Open the file in which we write the blocks

    		  @param           	destSetName
    							destination set.
    		  @param           	sourceId
    							To locate the file with the same name as sourceId.
			  @param           	ptr
    							pointer.

    	      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

    	      @exception       none
     */
    /*=================================================================== */
    //##ModelId=3DF4992B035C
    bool open(const string &destSetName, const string &sourceId, FTPRV2Backup *ptr);

    /*=================================================================== */
    /**

    	      @brief		open
    						This open is for SFTP responding IP.

    		  @param           	destSetName
    							destination set.
    		  @param           	sourceId
    							To locate the file with the same name as sourceId.
			  @param           	ptr
    							pointer.

    	      @return           true    to indicate that everything is OK.
								false   to indicate that something went wrong.

    	      @exception       none
     */
    /*=================================================================== */
   	bool open(const string &destSetName, const string &sourceId, SSHFTPRV2Backup *ptr);

	/*=================================================================== */
	/**

		      @brief		close
							closes the backup.

		      @return           true    to indicate if close successful.
								false   to indicate that close unsuccessful.

		      @exception       none
	 */
	/*=================================================================== */
    //##ModelId=3DF499570049
    bool close();
    
    //##ModelId=3DF4B6280082
   bool write(const AES_DBO_DataBlock *block); 
    
    //##ModelId=3DF5AB890303
    AES_CDH_ResultCode lastTransaction(ACE_UINT32 &blockNr); 
};

#endif
