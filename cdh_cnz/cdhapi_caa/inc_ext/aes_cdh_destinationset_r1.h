/*=================================================================== */
/**
   @file   aes_cdh_destinationset_r1.h

   @brief Header file for CDH Destination set type module.

          The services provided by AES_CDH_DestinationSet_R1 facilitates the
          transfer of data to remote systems.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011    XCHEMAD     Initial Release
==================================================================== */


/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_DESTINATIONSET_R1_H 
#define AES_CDH_DESTINATIONSET_R1_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <string>
#include <vector>
#include <aes_cdh_resultcode.h>
#include <aes_cdh_result.h>
#include <ace/ACE.h>

/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Protection type

          This Type is used in constructor AES_GCC_NameProtection
 */
/*=================================================================== */
const ACE_TCHAR AES_CDH_NameProtectionType[] = "DESTSET";

/*=====================================================================
                        FORWARD DECLARATION SECTION
==================================================================== */
class AES_CDH_DestinationSetImplementation;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_DestinationSet_R1

                 The services provided by AES_CDH_DestinationSet_R1 facilitates the
                 transfer of data to remote systems.
*/
/*=================================================================== */
class AES_CDH_DestinationSet_R1
{
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  TQConnectAttributes

           Types of connection attributes
*/
/*=================================================================== */
    typedef enum
    {
        FILE   = 0x1,
        BLOCK  = 0x2
    } TQConnectAttributes;

/*=================================================================== */
/**
    @struct  destSetAttributes

    @brief   Destination set attributes returned by getAttr()

    @par destSetName
         Holds the destination set name.
    @par destSetType
         Holds the destination set type.
    @par primDest
         Holds the primary destination.
    @par secDest
         Holds the secondary destination.
    @par backDest
         Holds the backup destination.
    @par active
         Holds if active or not.
    @par userGroup
         Holds the user group name.
*/
/*=================================================================== */
    struct destSetAttributes
    {
        std::string destSetName;
        TQConnectAttributes destSetType;
        std::string primDest;
        std::string secDest;
        std::string backDest;
        std::string active;
        std::string userGroup;
    };

/*=================================================================== */
/**
   @brief  transferMode

           Modes of transfer.
*/
/*=================================================================== */
    enum transferMode { TR_BINARY, TR_ASCII };

/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Destination set constructor

                       Creates a dummy destination set object with an
                       empty string as destination set name.

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_DestinationSet_R1();

/*=================================================================== */
   /**

      @brief           Destination set constructor

                       Creates a destination set object with a
                       destination set name.

      @param           destSetName
                       Name of the destination set.

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_DestinationSet_R1(const std::string &destSetName);
    
/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Destination set destructor

                       Stops all tasks started by the destination set object.

      @return          none

      @exception       none
   */
/*=================================================================== */
    virtual ~AES_CDH_DestinationSet_R1();
    
/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Connects to the destination set. Destination set is supervised, and
		   events are reported thru method eventDestinationSet().
		   On lost connection an alarm is issued with provided
		   filedestination and alarmtext rather than the default texts.
		   NOTE !  Attach must be performed prior call to sendFile().

      @return      Return value.
                        AES_CDH_RC_OK           Open succeeded
                        AES_CDH_RC_NODESTSET    Destination set does not exist
                        AES_CDH_RC_NOSERVER     No contact with AES_CDH_Server
			AES_CDH_RC_INCUSAGE 	Attempt to open and open is already
						done or destination name not specified.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode open(void); 

/*=================================================================== */
   /**
      @brief       Used for file transfer. Destination set is supervised, and
                   events are reported thru method eventDestinationSet().
                   On lost connection an alarm is issued with provided
                   Transfe queue name and alarmtext rather than the default texts.
                   NOTE ! Open() must be performed prior call to sendFile(). 

      @param	   transferQueue	
			Name of transfer queue

      @param	   alarmText
			Provided alarm text for connection alarm

      @return      Return value.
                        AES_CDH_RC_OK           Request succeeded, and eventDestinationSet()
						will be called when acknowledge is received
						from the server.
                        AES_CDH_RC_NODESTSET    Destination set does not exist
                        AES_CDH_RC_NOSERVER     Connection to AES_CDH_server is lost.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode setEventSubscription(const std::string& transferQueue="",
        const std::string& alarmText="");
    
/*=================================================================== */
   /**
      @brief       Removes subscribtion of connection events


      @return      Return value.
                        AES_CDH_RC_OK           Request succeeded, and eventDestinationSet()
                                                will be called when acknowledge is received
                                                from the server.
                        AES_CDH_RC_NOSERVER     Connection to AES_CDH_server is lost.

      @exception   none
    */
/*=================================================================== */
    
    AES_CDH_ResultCode removeEventSubscription(void);
   
/*=================================================================== */
   /**
      @brief       Disconnects the destination set.


      @return      Return value.
                        AES_CDH_RC_OK           Close succeeded.
                        AES_CDH_RC_NOSERVER     Connection to AES_CDH_server is lost.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode close(void);
    
/*=================================================================== */
   /**
      @brief       Checks whether destSetName has been defined using define().


      @return      Return value.
                        AES_CDH_RC_OK           Destination set exists
			AES_CDH_RC_NODESTSET	Destination set does not exist
                        AES_CDH_RC_NOSERVER     Connection to AES_CDH_server is lost.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode exists(void);

/*=================================================================== */
   /**
      @brief       Checks whether destSetName has been defined using define()
		   and whether the caller may connect to the destination set with
		   respect to attr.
    
      @param	   attr
			Used to determine if the destination set is of the requested
			type. Block or file.

      @return      Return value.
                        AES_CDH_RC_OK           Destination set exists and the caller
						may connect to the destination set.
                        AES_CDH_RC_NODESTSET    Destination set does not exist
                        AES_CDH_RC_NOSERVER     Connection to AES_CDH_server is lost.
			AES_CDH_RC_NOTFILEDEST  The destination set exists but may not be
						used for file transfer
			AES_CDH_RC_NOTBLOCKDEST The destination set exists but may not be
						used for block transfer

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode exists(const TQConnectAttributes attr);

/*=================================================================== */
   /**
      @brief       Initiates a send file task by creating a separate thread.
		   The result code is returned.
		   The thread calls eventSendFile() when reply is received from
		   the CDH-server. Method open has to called prior this call.

      @param       fileName
			Source file name.

      @param	   remoteSubDirName
			Together with "remote dir" creates destination file name.
	
      @param	   newFileName
			Added to destination file name if given
			("<remoteSubDirName>-<newFileName>" in case of transfer type FTP)

      @param	   userUnique
			A user unique string used by CDH server if retryAfterRestart is set
			to true. If the file is already sent by CDH (there is a match
			of userUnique and last sent file) eventSentFile will return
			transfer ok without resending the file.

      @param	   trMode
			Transfer mode.	

      @param	   fileMask
			File Mask

      @param	   isDir
			Set to true if fileName is a directory

      @param	   retryAfterRestart
			Set to true if this is an attemt to resend
                        the file after a restart. Setting this flag
                        will prevent CDH to resend the file if it
                        already has been sent.

      @return      Return value.
                        AES_CDH_RC_OK           Thread created OK
                        AES_CDH_RC_INTPROGERR   Error creating thread
			AES_CDH_RC_INCUSAGE	Open() has not been performed
                        AES_CDH_RC_NOSERVER     Connection to AES_CDH_server is lost.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode sendFile(const std::string &fileName,
                                const std::string remoteSubDirName="",
                                const std::string newFileName="",
                                const std::string userUnique="",
                                transferMode trMode = TR_BINARY,
                                const std::string fileMask="",
                                bool isDir=false,
                                bool retryAfterRestart=false);

/*=================================================================== */
   /**
      @brief       This method must exist in a user created subclass of
		   AES_CDH_DestinationSet. It returns the result of the
		   connection check towards the destination.

      @param	   ret
		  	ret can be one of the following return codes:
    		        AES_CDH_RC_CONNECTOK           Ok connecting to remote system
    		        AES_CDH_RC_CONNECTERR          Error connecting to remote system
    		        AES_CDH_RC_EVENTHANDLERSET     Event Handler is set
    		        AES_CDH_RC_EVENTHANDLERSETERR  Error when setting Event Handler
    		        AES_CDH_RC_EVENTHANDLERREM     Event Handler is removed
    		        AES_CDH_RC_EVENTHANDLERREMERR  Error when removing Event Handler

      @param 	   destSetName
			Name of the destination set.

      @return      none

      @exception   none
    */
/*=================================================================== */
    virtual void eventDestinationSet(AES_CDH_ResultCode ret,     
                                  const std::string &destSetName)
    {
    	(void)ret;
    	(void)destSetName;
    }

/*=================================================================== */
   /**
      @brief       This method must exist in a user created subclass of
                   AES_CDH_DestinationSet. It returns the result of the
                   send file task.

      @param       ret
                        ret can be one of the following return codes:
    		        AES_CDH_RC_OK           A file has been sent.
    		        AES_CDH_RC_INTPROGERR   See event log for more information
    		        AES_CDH_RC_NOSERVER     No contact with AES_CDH_Server
    		        AES_CDH_RC_FILEERR      Error reading source file ('fileName')
    		        AES_CDH_RC_FILENAMETOOLONG Too long destination file name
    		        AES_CDH_RC_CONNECTERR   Error connecting to remote system
    		        AES_CDH_RC_SENDERR      Error sending to remote system
    		        AES_CDH_RC_DATAAREAERR  Error creating responding pickup directory
    		        AES_CDH_RC_TASKSTOPPED  Task stopped by stopSendFile()
    		        AES_CDH_RC_TASKEXISTS   A similar task is already executing

      @param       destSetName
                        Name of the destination set.

      @param       fileName
                        Name of the file

      @param       remoteSubDirName
                        Name of the remote sub directory

      @param       newFileName
                        Name of the new file.

      @return      none

      @exception   none
    */
/*=================================================================== */
    virtual void eventSendFile(AES_CDH_ResultCode ret,
                               const std::string &destSetName,
                               const std::string &fileName,
                               const std::string &remoteSubDirName,
                               const std::string &newFileName)
    {
    	(void)ret;
    	(void)destSetName;
    	(void)fileName;
    	(void)remoteSubDirName;
    	(void)newFileName;
    }
    
/*=================================================================== */
   /**
      @brief       Stops the specified send file task.

      @param       fileName
			Name of the file

      @param       remoteSubDirName
                        Name of the remote sub directory.

      @param       newFileName
                        Name of the new file.

      @return      Return Value
			AES_CDH_RC_OK           Send file task stopped
    			AES_CDH_RC_NOSERVER     No contact with AES_CDH_Server
    			AES_CDH_RC_NOTASK       Specified send file task not found
    		        AES_CDH_RC_INTPROGERR   Unable to stop task within 30 seconds

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode stopSendFile(const std::string &fileName,
                                    const std::string remoteSubDirName="",
				    const std::string newFileName = "");
    
/*=================================================================== */
   /**
      @brief       Initiates a send record file task by creating a separate thread.
		   The result code is returned. Method attach has to called prior this call.
		   The newly created thread then calls eventSendRecordFile() when
    		   replies is received from CDH server..

      @param       streamName
                        Name of the Transfer queue.

      @param       streamId
                        Name of rpc file on remote side.

      @return      Return Value
B
			AES_CDH_RC_OK           Thread created OK
    			AES_CDH_RC_INTPROGERR   Error creating thread
    		        AES_CDH_RC_INCUSAGE     Open() was not called prior this call
    			AES_CDH_RC_NOSERVER     Connection to AES_CDH_server is lost.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode sendRecordFile(std::string streamName,
                                      std::string streamId);  

/*=================================================================== */
   /**
      @brief       This method must exist in a user created subclass of
		   AES_CDH_DestinationSet. It returns the result of the
		   send record file task.

      @param       ret
	    		'ret' can be one of the following return codes (* means task will
    			continue executing):
    		        AES_CDH_RC_OK           	*One fil from stream has been sent
    		        AES_CDH_RC_INCUSAGE     	Method not supported by transfer type
    		        AES_CDH_RC_INTPROGERR   	See event log for more information
    		        AES_CDH_RC_NOSERVER     	No contact with AES_CDH_Server
    		        AES_CDH_RC_FILEERR      	Error reading stream
    		        AES_CDH_RC_FILENAMETOOLONG 	Too long destination file name
    		        AES_CDH_RC_CONNECTERR   	*Error connecting to remote system
    		                                	(data lost)
    		        AES_CDH_RC_SENDERR      	*Error sending to remote system
    		                                	(data lost)
    		        AES_CDH_RC_CONNECTWARN  	*Error connecting to remote system
    		                                	(no data lost)
    		        AES_CDH_RC_SENDWARN     	*Error sending to remote system
    		                                	(no data lost)

      @return      none

      @exception   none
    */
/*=================================================================== */
    virtual void eventSendRecordFile(AES_CDH_ResultCode ret)
    {
    	(void)ret;
    }
      
/*=================================================================== */
   /**
      @brief       Returns the result code from last called method.

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode getError(void);

/*=================================================================== */
   /**
      @brief       Returns a text that matches the result code from last called method.
	
      @param	   errText
			Error Text

      @return      none

      @exception   none
    */
/*=================================================================== */
    void getErrorText(std::string& errText);
    
/*=================================================================== */
   /**
      @brief       Returns attributes in 'attr' from one destination
		   set or all destination sets.

      @param       argc
			Number of arguments
	
      @param	   argv
			Array of arguments

      @param	   attr
			Vector of destination set attributes

      @return      Return value
   		   A result object whose result code can be one of the following:
    		   AES_CDH_RC_OK           Destination set(s) listed OK
    		   AES_CDH_RC_INCUSAGE     An argument is not correct.
    		   AES_CDH_RC_NODEST       The destination set does not exist.
    		   AES_CDH_RC_INTPROGERR   See event log for more information
    		   AES_CDH_RC_NOSERVER     No contact with AES_CDH_Server

      @exception   none
    */
/*=================================================================== */
    static AES_CDH_Result getAttr(int argc, char* argv[],
                                  std::vector<destSetAttributes>& attr);

/*=================================================================== */
   /**
      @brief       Returns attributes in 'attr' from one destination
                   set or all destination sets.

      @return      Return value
                   A result object whose result code can be one of the following:
    		   AES_CDH_RC_OK           Transaction started
    		   AES_CDH_RC_NODESTSET    Destination set does not exist
        	   AES_CDH_RC_NOSERVER     No contact with AES_CDH_Server
        	   AES_CDH_RC_INCUSAGE     Incorrect usage

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionBegin();

/*=================================================================== */
   /**
      @brief       This method instructs CDH to end a transaction
		   Method open() has to be called prior to this call

      @param	   applBlockNr
			Application block number to stop at.

      @return      Return value
                   A result object whose result code can be one of the following:
                   AES_CDH_RC_OK           Connection is OK
                   AES_CDH_RC_NODESTSET    Destination set does not exist
                   AES_CDH_RC_NOSERVER     No contact with AES_CDH_Server
                   AES_CDH_RC_INCUSAGE     Incorrect usage

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionEnd(unsigned int &applBlockNr);

/*=================================================================== */
   /**
      @brief       This method instructs CDH to commit a transaction
                   Method open() has to be called prior to this call

      @param       applBlockNr
                        Application block number to stop at.

      @return      Return value
                   A result object whose result code can be one of the following:
                   AES_CDH_RC_OK           		Connection is OK
                   AES_CDH_RC_NODESTSET    		Destination set does not exist
                   AES_CDH_RC_NOSERVER     		No contact with AES_CDH_Server
                   AES_CDH_RC_INCUSAGE     		Incorrect usage
		   AES_CDH_RC_CONNECTERR   		Error connecting to remote system
		   AES_CDH_RC_NOBLOCKNOAVAILABLE	No block number is available

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionCommit(unsigned int &applBlockNr);

/*=================================================================== */
   /**
      @brief       This method instructs CDH to report the last committed block
		   Method open() has to be called prior to this call

      @param       applBlockNr
                        Application block number to stop at.

      @return      Return value
                   A result object whose result code can be one of the following:
		   AES_CDH_RC_OK                   Success
    		   AES_CDH_RC_NODESTSET            Destination set does not exist
    		   AES_CDH_RC_NOSERVER             No contact with AES_CDH_Server
    		   AES_CDH_RC_INCUSAGE             Incorrect usage
    	           AES_CDH_RC_CONNECTERR           Error connecting to remote system
    		   AES_CDH_RC_NOBLOCKNOAVAILABLE   No block number is available

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode getLastCommittedBlock(unsigned int &applBlockNr);

/*=================================================================== */
   /**
      @brief       This method instructs CDH to terminate a transaction
                   Method open() has to be called prior to this call

      @return      Return value
                   A result object whose result code can be one of the following:
	 	   AES_CDH_RC_NOERRORCODE            Successfull operation
    		   AES_CDH_RC_NOCDHSERVER	     No cdh server
    		   AES_CDH_RC_NOSERVERACCESS	     No server access
    		   AES_CDH_RC_TRANSACTIONNOTACTIVE   Incorrect usage

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionTerminate();
/*=================================================================== */
	bool isSessionOpen();
/*=================================================================== */

/*===================================================================
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:
    
/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Constructor
                       Private constructor to initialize the AES_CDH_DestinationSet_R1 object

      @param           AES_CDH_DestinationSet_R1 reference

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_DestinationSet_R1(const AES_CDH_DestinationSet_R1&);

/*===================================================================
                        PRIVATE METHOD
=================================================================== */

/*=================================================================== */
   /**

      @brief           Assignment operator overloading

                       This method overloads the assignment operator.
                       Used for assigning the AES_CDH_DestinationSet_R1 object to another

      @param           AES_CDH_DestinationSet_R1&
                       Object reference

      @return          AES_CDH_DestinationSet_R1&
                       Object reference
   */
/*=================================================================== */
    AES_CDH_DestinationSet_R1& operator=(const AES_CDH_DestinationSet_R1&);


/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   AES_CDH_DestinationSetImplementation*

               Pointer to AES_CDH_DestinationSetImplementation object.
   */
/*=================================================================== */
    AES_CDH_DestinationSetImplementation* implementation;
/*=================================================================== */
    
};

#endif // AES_CDH_DESTINATIONSET_R1_H
