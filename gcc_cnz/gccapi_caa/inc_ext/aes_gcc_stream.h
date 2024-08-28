
/*=================================================================== */
/**
	@file   aes_gcc_stream.h

	@brief
	The services provided by AES_GCC_Stream facilitates the
	creation of a stream.

	General rule:
	The error handling is specified for each method.
	No methods initiate or send error reports unless specified.

	@version 1.0.0
 */
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       20/07/2011     xbhadur   Initial Release
   	   	   	 30/11/2013		qvincon	  Improved some parts
 ============================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
 ==================================================================== */

#ifndef _AES_GCC_STREAM_H
#define _AES_GCC_STREAM_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
 ==================================================================== */


#include <ace/ACE.h>
#include <ace/Event.h>
#include <string>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */
class ACE_Process_Mutex;
class ACE_Process_Semaphore;


/*=====================================================================
                        CLASS DECLARATION SECTION
 ==================================================================== */
/*===================================================================*/
/**
		@brief AES_GCC_Stream
	The services provided by AES_GCC_Stream facilities stream creation
 */
/*=================================================================== */
class AES_GCC_Stream
{
 private:

	/**
		 * 	@brief	This method open the data file to be mapped, it will be created if not exist
	*/
	ACE_HANDLE openSharedDataFile(const std::string& dataFile, const ACE_UINT32& fileSize);

	/**
		 * 	@brief	This method creates synchronization object to access to the mapped memory
	*/
	bool createSyncObject(const std::string& TqName);

	/**
		 * 	@brief	This method wait for max timeout time for data ready or cancel event
	*/
	bool waitDataReady(unsigned int timeoutMs, ACE_Event* cancelReadEvent);


	/*=====================================================================
		                      PROTECTED DECLARATION SECTION
	==================================================================== */
protected:
	/*====================================================================
	                        PROTECTED ATTRIBUTES
	==================================================================== */
    struct SharedAttributes
    {
        ACE_UINT32 size_;
        ACE_UINT32 rPos_;
        ACE_UINT32 wPos_;
        ACE_UINT32 clst_;
    };

    ACE_Process_Mutex* m_processMutex;
    ACE_Process_Semaphore* m_procesSem;

    SharedAttributes* attr_;
    char* m_data;
    ACE_UINT32 m_sharedMemorySize;
    void* m_sharedMemoryAddr;


    /*=====================================================================
	                        PUBLIC DECLARATION SECTION
	==================================================================== */
public:

	/*=====================================================================
	                        CLASS CONSTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

	      @brief          default constructor
						  Creates an empty stream object.

	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
	AES_GCC_Stream();
	/*=====================================================================
	                        CLASS DESTRUCTORS
	==================================================================== */
	/*=================================================================== */
	/**

	      @brief          default destructor
						 
	      @return          void

	      @exception       none
	 */
	/*=================================================================== */
    ~AES_GCC_Stream();

	/*=================================================================== */
	/**

	      @brief          create
						  Creates a stream.

		  @param           	name
  							Gives the stream a name and is used by others to open the same stream.
  		  @param           	size
  							The size of the stream in blocks.

	      @return          true    to indicates that everything is OK.
						   false   to indicates that something went wrong.

	      @exception       none
	 */
	/*=================================================================== */
     bool create(const std::string& TqName, const std::string& TqPath, const ACE_UINT32& sharedMemSize);

     bool open(const std::string& TqName, const std::string& TqPath, const ACE_UINT32& sharedMemSize);

	 /*=================================================================== */
	/**

	      @brief          close
						  closes the open stream.

		  @return          void

	      @exception       none
	 */
	/*=================================================================== */ 
    void close();

	/*=================================================================== */
	/**

	      @brief		read
						Reads a block from the stream.

		  @param           	buf
  							This is the buffer which is to be filled with data.
		  @param           	len
  							The length of the buffer, returns the read bytes.
		 @param           	timeout
  							Timeout for release of the reader.
		  		
	      @return           AES_NOERRORCODE     Everything is OK.
							AES_TIMEOUT         Timeout occurred in the semaphore.
							AES_EXECUTEERROR    Some error occurred with semaphore.

	      @exception       none
	 */
	/*=================================================================== */ 
   	ACE_UINT32 read(char *buf, unsigned int &len, unsigned int timeout = 0XFFFFFFFF);

	/*=================================================================== */
	/**

	      @brief		read
						Reads a block from the stream.

		  @param           	buf
  							This is the buffer which is to be filled with data.
		  @param           	len
  							The length of the buffer, returns the read bytes.
		  @param           	timeout
  							Timeout for release of the reader.
		  @param           	hCancel
  							Handle to event for aborting operation.
  		
	      @return           AES_NOERRORCODE     Everything is OK.
							AES_TIMEOUT         Timeout occurred in the semaphore.
							AES_EXECUTEERROR    Some error occurred with semaphore.

	      @exception       none
	 */
	/*=================================================================== */ 
    ACE_UINT32 read(char *buf, unsigned int &len, unsigned int timeout, ACE_Event* hCancel = NULL);

	/*=================================================================== */
	/**

	      @brief          write
						  Writes a block to the stream.

		  @param           	buf
  							The buffer containing data to be written to the stream.
		  @param           	len
  							The length of the buffer.
		   		
	      @return           AES_NOERRORCODE     Everything is OK.
							AES_BUFFERFULL      The stream is full.  
							
	      @exception       none
	 */
    /*=================================================================== */
    ACE_UINT32 write(const char *buf, unsigned int len);

    /*=================================================================== */
	/**

	      @brief          clear
						  clears the stream.

		  @return          void

	      @exception       none
	 */
	/*=================================================================== */ 
    void clear();
};

#endif	
