//	COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//	  All rights reserved.
//	  The Copyright to the computer program(s) herein
//	  is the property of Ericsson Utvecklings AB, Sweden.
//	  The program(s) may be used and/or copied only with
//	  the written permission from Ericsson Utvecklings AB or
//	in
//	  accordance with the terms and conditions stipulated in
//	the
//	  agreement/contract under which the program(s) have
//	been
//	  supplied.
//
//	DESCRIPTION
//
//	ERROR HANDLING
//	 General rule:
//	 The error handling is specified for each method.
//	 No methods initiate or send error reports unless
//	specified.
//
//	DOCUMENT NO
//	190 89-CAA 109 0503
//
//	AUTHOR
//	 2003-01-20 UAB/KB/AU DAPA
//
//	REVISION
//	 A 2003-01-20 DAPA
//	 B 2003-05-12 DAPA
//
//	LINKAGE
//
//	SEE ALSO

#ifndef aes_afp_destination_h
#define aes_afp_destination_h 1
#include "aes_afp_datablock.h"
#include "aes_afp_msgblock.h"
#include "aes_afp_threadmap.h"
#include "aes_afp_proctype.h"
#include "aes_afp_renametypes.h"
#include <string>
#include <ace/Message_Queue_T.h>
#include <ace/Task.h>
#include <aes_gcc_format_r1.h>
#include <aes_gcc_log.h>
#include <aes_cdh_destinationset.h>
#include "aes_afp_events.h"
#include <ACS_APGCC_Util.H>
//	This class is responsible for the administrator queue.

typedef ACE_Message_Queue< ACE_MT_SYNCH , ACE_Monotonic_Time_Policy > AceMsg_destination;
//	This class interacts with the CDH API.
//	Trace points: aes_afp_destination traces activity
//	towards the CDH API.
class aes_afp_transferqueue;
class aes_afp_destination : public AES_CDH_DestinationSet  //## Inherits: <unnamed>%3B28D8550282
{

  public:
      //	Constructor.
      aes_afp_destination (ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* manageQueue, std::string destinationSet,aes_afp_transferqueue* pTq);

    //## Destructor (generated)
      virtual ~aes_afp_destination();

      //	Called by the CDH API when an event is present.
     virtual void eventSendFile(AES_CDH_ResultCode ret,
                               const std::string &destSetName,
                               const std::string &fileName,
                               const std::string &remoteSubDirName,
                               const std::string &newFileName);

      //	Initiates a file sending to CDH.
      AES_CDH_ResultCode sendFileToCdh ();

      //	Change status of send item.
      void changeStatusOfSendItem (AES_GCC_Filestates newStatus);

      //	Sends a message to Cdh task.
      bool sendMessageToCdhTask (aes_afp_msgtypes message, AES_GCC_Errorcodes error);

      //	Copy some data from the datablock.
      void copyFromDatablock (aes_afp_datablock* dataBlock);

      //	Stops the transfer of a file in CDH.
      int stopFileTransfer ();

      //	Called by CDH API when an event is available from CDH
      //	server.
      void eventDestinationSet (AES_CDH_ResultCode ret,
				const std::string &destinationSet);

	  void copyToDatablock (aes_afp_datablock* dataBlock);
	void setShutdownForDest();

  protected:

  private:

  private: //## implementation
    // Data Members for Class Attributes

      int numberOfSendRetries_;

      //	Delay time between send retries.
      int timeBetweenRetries_;

      //	Current send retry number.
      int sendRetryNo_;

      //	Delay time between retries.
      int retryDelay_;

      //	True if this order is manually initiated.
      bool manualOrder_;

      //	True if the task is about to be shutdown.
      bool shutdownInProgress_;

      bool isADirectory_;

      bool restarted_;

      const std::string nameOfDatatask_;

      std::string currentNewFilename_;

      std::string currentDestinationSet_;

      std::string currentTransferQueue_;

      std::string fileMask_;

      std::string renameTemplate_;

      std::string currentFilename_;

      std::string fileToTransfer_;

      //	New file name.
      std::string currentNewSubfile_;


      AES_AFP_Proctype currentOrderType_;

      AES_AFP_Renametypes renameFile_;

      aes_gcc_format_r1 format_;

      AceMsg_destination *messageQueue_;
      aes_afp_transferqueue * pTq_;


};

// Class aes_afp_destination 
#endif
