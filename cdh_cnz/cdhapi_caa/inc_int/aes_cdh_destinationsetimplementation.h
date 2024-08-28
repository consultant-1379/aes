/*=================================================================== */
/**
   @file   aes_cdh_destinationsetimplementation.h

   @brief Header file for AES_CDH_DestinationSetImplementation type module.

          The services provided by AES_CDH_DestinationSetImplementation
	  facilitates the transfer of data to remote systems.
 
	  Implementation of AES_CDH_DestinationSet.

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
#ifndef AES_CDH_DESTINATIONSETIMPLEMENTATION_H 
#define AES_CDH_DESTINATIONSETIMPLEMENTATION_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
#include <ace/Synch.h>
#include <ace/Guard_T.h>

#include <aes_cdh_destinationset.h>
#include <aes_cdh_result.h>


#include <string>
#include <vector>

/*===================================================================
                        CLASS FORWARD DECLARATION SECTION
=================================================================== */

class AES_CDH_AsynchReceiver;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     AES_CDH_DestinationSetImplementation

                 The services provided by AES_CDH_DestinationSetImplementation
		 facilitates the transfer of data to remote systems.
*/
/*=================================================================== */
class AES_CDH_DestinationSetImplementation
{
/*=====================================================================
                        PUBLIC DECLARATION SECTION
==================================================================== */
public:
    
/*=====================================================================
                        CLASS CONSTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           DestinationSetImplementation constructor

                       Creates a dummy destination set implementation object
			 with an empty string as destination set name.

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_DestinationSetImplementation();

/*=================================================================== */
   /**

      @brief           Destination set implementation constructor

                       Creates a destination set implementation object with a
                       destination set name.

      @param           destSetName
                       Name of the destination set.

      @return          none

      @exception       none
   */
/*=================================================================== */
    AES_CDH_DestinationSetImplementation(const std::string destSetName);
    
/*===================================================================
                        CLASS DESTRUCTOR
=================================================================== */
/*=================================================================== */
   /**

      @brief           Destination set implementation destructor

                       Stops all tasks started by the destination set 
			implementation object.

      @return          none

      @exception       none
   */
/*=================================================================== */
    virtual ~AES_CDH_DestinationSetImplementation();

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       Opens the connection to CDH server

      @param	   destSet
			AES_CDH_DestinationSet reference

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode open(AES_CDH_DestinationSet& destSet);   

/*=================================================================== */
   /**
      @brief       Closes the connection to CDH server

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode close(void);                       

/*=================================================================== */
   /**
      @brief       Sets the event subscription

      @param	   fileTransferQueue
			Name of the file transfer queue.

      @param	   alarmText
			Alarm Text

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode setEventSubscription(const std::string fileTransferQueue,
    										const std::string alarmText);

/*=================================================================== */
   /**
      @brief       Removes the event subscription

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode removeEventSubscription(void);
    
/*=================================================================== */
   /**
      @brief       Checks if the destination set exists

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode exists(void);

/*=================================================================== */
   /**
      @brief       Checks if the destination set exists for the given attributes

      @param	   attr
			Attributes list

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode exists(const AES_CDH_DestinationSet::TQConnectAttributes attr);

/*=================================================================== */
   /**
      @brief       Sends the file to CDH

      @param       fileName
                        Name of the file

      @param       remoteSubDirName
                        Name of the remote sub directory

      @param       newFileName
                        Name of the new file

      @param       userUnique
                        Unique user name

      @param       trMode
                        Transfer mode.

      @param       fileMask
                        File mask

      @param       isDir
                        Flag for is directory.

      @param       retryAfterRestart
                        Flag to retry after restart

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode sendFile(const std::string fileName,
                                const std::string remoteSubDirName="",
                                const std::string newFileName="",
                                const std::string userUnique="",
                                AES_CDH_DestinationSet::transferMode trMode=AES_CDH_DestinationSet::TR_BINARY,
                                const std::string fileMask="",
                                bool isDir=false,
                                bool retryAfterRestart=false);

/*=================================================================== */
   /**
      @brief       Event for send file

      @param       ret
                        Result code

      @param       destinationSetName
                        Name of the destination set

      @param       fileName
                        Name of the file

      @param       remoteSubDirName
                        Name of the remote sub directory

      @param       newFileName
                        Name of the new file

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    virtual void eventSendFile(AES_CDH_ResultCode ret,
                               const std::string destinationSetName,
                               const std::string fileName,
                               const std::string remoteSubDirName,
                               const std::string newFileName)
    {
    	(void)ret;
    	(void)destinationSetName;
    	(void)fileName;
    	(void)remoteSubDirName;
    	(void)newFileName;
    }

/*=================================================================== */
   /**
      @brief       Stops sending the file

      @param       fileName
                        Name of the file

      @param       remoteSubDirName
                        Name of the remote sub directory

      @param       newFileName
                        Name of the new file

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode stopSendFile(const std::string fileName,
                                    const std::string remoteSubDirName="",
                                    const std::string newFileName="");

/*=================================================================== */
   /**
      @brief       Sends a record file

      @param       destSet
                        AES_CDH_DestinationSet reference

      @param       streamName
                        Name of the stream

      @param       streamId
                        Stream ID.

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode sendRecordFile(AES_CDH_DestinationSet& destSet,
    		 	 	 	 	 	 	  std::string streamName,
    		 	 	 	 	 	 	  std::string streamId);

/*=================================================================== */
   /**
      @brief       Event for sending a record file

      @param       ret
                        AES_CDH_ResultCode object

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    virtual void eventSendRecordFile(AES_CDH_ResultCode ret)
    {
    	(void)ret;
    }

/*=================================================================== */
   /**
      @brief       Gets the error code.

      @return      Return value.

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode getError(void);
    
/*=================================================================== */
   /**
      @brief       Gets the error text

      @param       errText
                        Error text

      @return      none

      @exception   none
    */
/*=================================================================== */
    void getErrorText(std::string& errText);

/*=================================================================== */
   /**
      @brief       Gets the attributes of the destination set

      @param       argc
                        Number of attributes

      @param       argv
                        Attributes list

      @param       attr
                        Vector containing the attributes list

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    static AES_CDH_Result getAttr(int argc, char* argv[],
    		std::vector<AES_CDH_DestinationSet::destSetAttributes>& attr);
    
/*=================================================================== */
   /**
      @brief       Send file event

      @param       ret
                        Result code

      @param       destinationSetName
                        Name of the destination set.

      @param       fileName
                        Name of the file

      @param       newFileName
                        Name of the new file

      @param       newSubFileName
                        Name of the new sub file

      @return      none

      @exception   none
    */
/*=================================================================== */
    void sendFileEv(AES_CDH_ResultCode ret,
                    const std::string destinationSetName,
                    const std::string fileName,
                    const std::string newFileName,
                    const std::string newSubFileName);

/*=================================================================== */
   /**
      @brief       Send record file event

      @param       ret
                        Result code

      @return      none

      @exception   none
    */
/*=================================================================== */
    void sendRecordFileEv(AES_CDH_ResultCode ret);

/*=================================================================== */
   /**
      @brief       Send destination event

      @param       ret
                        Result code

      @param       destinationSetName
                        Name of the destination set.

      @return      none

      @exception   none
    */
/*=================================================================== */
    void sendDestEv(AES_CDH_ResultCode ret,
			        const std::string destSetName);

/*=================================================================== */
   /**
      @brief       Begins the transation

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionBegin();

/*=================================================================== */
   /**
      @brief       Ends the transaction

      @param       applBlockNr
                        Application block number

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionEnd(unsigned int &applBlockNr);

/*=================================================================== */
   /**
      @brief       Commits the transaction

      @param       applBlockNr
                        Application block number

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionCommit(unsigned int &applBlockNr);

/*=================================================================== */
   /**
      @brief       Gets the last committed block number

      @param       applBlockNr
                        Application block number

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode getLastCommittedBlock(unsigned int &applBlockNr);

/*=================================================================== */
   /**
      @brief       Terminates the transaction

      @return      Result code

      @exception   none
    */
/*=================================================================== */
    AES_CDH_ResultCode transactionTerminate();


	bool isSessionOpen();
	
/*=================================================================== 
                        PRIVATE DECLARATION SECTION
=================================================================== */
private:
/*===================================================================
                        PRIVATE ATTRIBUTE
=================================================================== */

/*=================================================================== */
   /**
      @brief   destinationSetName

               Name of the destination set.
   */
/*=================================================================== */
	 std::string destinationSetName;
/*=================================================================== */
   /**
      @brief   sessionIsOpen

               Flag to enable if session is open
   */
/*=================================================================== */
    bool sessionIsOpen;                      

/*=================================================================== */
   /**
      @brief   lastErrorCode

               Last error code 
   */
/*=================================================================== */
    int lastErrorCode;

/*=================================================================== */
   /**
      @brief   asynchReceiver

               Pointer to AES_CDH_AsynchReceiver object.
   */
/*=================================================================== */
    AES_CDH_AsynchReceiver* asynchReceiver;   

/*=================================================================== */
   /**
      @brief   closeDone

               Flag to enable if close is done.
   */
/*=================================================================== */
    bool closeDone;                            


/*=================================================================== */
   /**
      @brief   myDestSet

               Pointer to AES_CDH_DestinationSet object.
   */
/*=================================================================== */
    AES_CDH_DestinationSet* myDestSet;             
    
/*=================================================================== */
   /**
	  @brief   m_ProcessPid

			   Pid of Processes user
   */
/*=================================================================== */
    int m_ProcessPid;

/*=================================================================== */
   /**
      @brief       This method cleans the argument variables

      @param       argc
                        Number of variables

      @param       argv
                        Argument list

      @param       firstOper
                        First operation

      @return      none

      @exception   none
    */
/*=================================================================== */
    static void cleanUpArgv(int& argc, char* argv[], int firstOper);
    
/*=================================================================== */
   /**
      @brief       This method checks if valid characters are used in string

      @param       str
                        string to be validated

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    static int validCharacters(std::string str);
    
/*=================================================================== */
   /**
      @brief       This method checks if the given string is a valid name

      @param       str
                        string to be validated

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    static int validName(std::string& str);

/*=================================================================== */
   /**
      @brief       This method gets the attributes of destination set(s) configured in GOH

      @param       destSetName
                        Name of the destination set for which listing is required.

      @param       attr
                        Output vector which fetched the attributes of the destination set(s).

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    
    static int list(std::string destSetName,
    		std::vector<AES_CDH_DestinationSet::destSetAttributes>& attr);
    
/*=================================================================== */
   /**
      @brief       This method parses and gets the attributes of destination set(s)
                   configured in GOH

      @param       argc
                        Number of attributes

      @param       argv
                        Attribute list

      @param       attr
                        Output vector which fetched the attributes of the destination set(s).

      @return      Return value

      @exception   none
    */
/*=================================================================== */
    static int parse_cdhdsls(int& argc, char* argv[],
    		std::vector<AES_CDH_DestinationSet::destSetAttributes>& attr);

    friend class AES_CDH_AsynchReceiver;
};

#endif // AES_CDH_DESTINATIONSETIMPLEMENTATION_H
