//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%387C2A580188.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%387C2A580188.cm

//## begin module%387C2A580188.cp preserve=no
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
//	 190 89-CAA 109 0505
//
//	AUTHOR
//	 2002-12-19 DAPA
//
//	REVISION
//	 A 2002-12-19 DAPA
//	 B 2003-04-30 DAPA
//
//	 LINKAGE
//
//	SEE ALSO
//## end module%387C2A580188.cp

//## Module: aes_afp_apiinternal%387C2A580188; Package specification
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Source file: Z:\ntaes\afp\afpapi_caa\inc\aes_afp_apiinternal.h

#ifndef AES_AFP_APIINTERNAL_H
#define AES_AFP_APIINTERNAL_H 1

//## begin module%387C2A580188.additionalIncludes preserve=no
//## end module%387C2A580188.additionalIncludes

//## begin module%387C2A580188.includes preserve=yes
//#include <osf/OS.h>
//## end module%387C2A580188.includes

#include <ACS_DSD_Client.h>
#include <ACS_DSD_Session.h>
#include "aes_afp_msgtypes.h"
#include "aes_afp_proctype.h"
#include "aes_afp_renametypes.h"
#include "aes_afp_api.h"
#include "aes_afp_apimsg.h"
#include "aes_afp_protocol.h"
#include <string>
#include <iostream>
#include <sstream>
//#include <osf/Thread_Manager.h>
#include <ace/Thread_Manager.h>
#include <ace/OS_NS_Thread.h>
//#include <osf/OS.h>
//#include <osf/message_queue.h>
#include <ace/Message_Queue_T.h>
#include <ace/Monotonic_Time_Policy.h>
#include <ace/Event.h>
#include <aes_gcc_errorcodes.h>
#include <aes_gcc_filestates.h>
#include <aes_gcc_format.h>
//#include "aes_gcc_log.h"

class aes_afp_apitask;

//## begin module%387C2A580188.declarations preserve=no
//## end module%387C2A580188.declarations

//## begin module%387C2A580188.additionalDeclarations preserve=yes
//## end module%387C2A580188.additionalDeclarations


//## begin Osf_MessageQueue%3E40B3C20112.preface preserve=yes
//## end Osf_MessageQueue%3E40B3C20112.preface

//## Class: Osf_MessageQueue%3E40B3C20112; Instantiated Class
//	This class is responsible for message queue between
//	apiinternal and apitask.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



typedef ACE_Message_Queue< ACE_MT_SYNCH , ACE_Monotonic_Time_Policy  > Ace_MessageQueue;

//## begin Osf_MessageQueue%3E40B3C20112.postscript preserve=yes
//## end Osf_MessageQueue%3E40B3C20112.postscript

//## begin aes_afp_apiinternal%387C2938033D.preface preserve=yes
//## end aes_afp_apiinternal%387C2938033D.preface

//## Class: aes_afp_apiinternal%387C2938033D
//	Internal implementation class for AFP API.
//	Trace points: aes_afp_api traces in/out from the API.
//## Category: afpapi_caa (CAA 109 0505)%3B023DA70326
//## Subsystem: AFP::afpapi_caa::inc%37DF37EA0287
//## Persistence: Transient
//## Cardinality/Multiplicity: n



//## Uses: <unnamed>%37BE90A30051;strstream { -> }
//## Uses: <unnamed>%3780B4FE0179;aes_afp_msgtypes { -> }
//## Uses: <unnamed>%38B4FC050235;OSF_OS { -> }
//## Uses: <unnamed>%3B0A3DB80262;AES_AFP_Proctype { -> }
//## Uses: <unnamed>%3B0A3E780268;AES_GCC_Filestates { -> }
//## Uses: <unnamed>%3B0A3EE6019E;AES_GCC_Errorcodes { -> }
//## Uses: <unnamed>%3B0E17FA023D;aes_afp_apimsg { -> }
//## Uses: <unnamed>%3B178323003B;AES_GCC_Format { -> }
//## Uses: <unnamed>%3B1CACE5005B;AES_AFP_Renametypes { -> }
//## Uses: <unnamed>%3E478B4C0202;string { -> }
//## Uses: <unnamed>%3EAE163F0280;AES_GCC_Log { -> }

class aes_afp_apiinternal 
{
public:
   //## Constructors (specified)
   //## Operation: aes_afp_apiinternal%3EAD21A200EB; C++
   aes_afp_apiinternal(AES_AFP_Api* apiPointer);

   //## Destructor (generated)
   virtual ~aes_afp_apiinternal();


   //## Other Operations (specified)
   //## Operation: transferQueueDefined%387C2C0400D9
   //	Check if a transfer queue is defined in the database.
   unsigned int transferQueueDefined(std::string& transferQueue,
                                     std::string& destinationSet,
                                     bool &manual,
				     std::string& tranferQueueDn);

   //## Operation: getTransferState%387C2C0400E3
   //	Retreives the transfer state for a file.
   unsigned int getTransferState(std::string& filename,
                                 std::string& destinationSet,
                                 AES_GCC_Filestates& transferState);

   //## Operation: getTransferStateEx%3CF5FE8302BC
   //	Retreives the transfer state and other data for a file
   //	or directory. The argument listOrder will determine
   //	which file or directory for a transfer queue/
   //	destination pair that is retreived.
   //	Return values:
   //	AES_NOERRORCODE
   //	Indicates a succesful operation.
   //	AES_NOSERVERACCESS
   //	Indicates that there is no contact with AES_AFP_server.
   //	AES_SENDITEMNAMEINVAL
   //	Indicates that the file name is either to long or has
   //	invalid characters.
   //	AES_NOPROCORDER
   //	Indicates that there is no  transfer queue defined.
   //	AES_NOSUCHITEM
   //	Indicates that file or directory is not reported.
   unsigned int getTransferStateEx(std::string& filename,
                                   AES_GCC_Filestates& status,
                                   std::string& destinationSet,
                                   int listOrder,
                                   std::string& reportDate,
                                   std::string& deleteDate,
                                   std::string& manualStart,
                                   std::string& manualStop,
                                   std::string& failDate,
                                   std::string& archiveDate,
                                   int& reasonForFailed,
                                   bool& isDirectory);

   //## Operation: setTransferState%387C2C0400F7
   //	Sets the transfer state for a file.
   unsigned int setTransferState(std::string& filename,
                                 std::string& destinationSet,
                                 AES_GCC_Filestates transferState);

   //## Operation: listFile%387C2C04010B; C++
   //	List files for a transfer queue.
   unsigned int listFile(std::string& transferQueue,
                         std::string& destinationSet,
                         std::stringstream& outStream);


   //## Operation: listNewestFile%387C2C040129; C++
   //	List last reported file for a transfer queue.
   unsigned int listNewestFile(std::string& transferQueue,
                               std::string& destinationSet,
                               std::stringstream& outStream);

   //## Operation: listOldestFile%387C2C04013D; C++
   //	List first reported file for a transfer queue.
   unsigned int listOldestFile(std::string& transferQueue,
                               std::string& destinationSet,
                               std::stringstream& outStream);

   //## Operation: listAllFiles%387C2C040147; C++
   //	List all files for a transfer queue.
   unsigned int listAllFiles(std::string& transferQueue,
                             std::stringstream& outStream);

   unsigned int listAllFilesShort(std::string& transferQueue,
                             std::stringstream& outStream);	

   //## Operation: listDestinationSets%387C2C04015B; C++
   //	List all destinationsets for a transfer queue.
   unsigned int listDestinationSets(std::string& transferQueue,
                                    std::stringstream& outStream);

   //## Operation: removeTransferQueue%387C2C040179; C++
   //	Removes a mainfile from database.
   unsigned int removeTransferQueue(std::string& transferQueue);

   //## Operation: stopFiles%387C2C04018E; C++
   //	Stop the transfer of all files for a destinationset.
   unsigned int stopFiles(std::string& transferQueue, std::string& destinationSet);

   //## Operation: stopFile%3B8F437200EB; C++
   //	Stop the transfer of a specified file for a destination.
   unsigned int stopFile(std::string& transferQueue,
                         std::string& destinationSet,
                         std::string& filename);

   //## Operation: getAttributes%387C2C0401AC; C++
   //	Retreives the attributes for a transfer queue.
   unsigned int getAttributes(std::string& transferQueue,
                              std::string& destinationSet, // This parameter represents the destination name.
                              std::stringstream& outStream);

   //## Operation: setAttributes%387C2C0401C0; C++
   //	Sets attributes for a transfer queue.
   unsigned int setAttributes(std::string& transferQueue,
                              std::string& destinationSet,
                              AES_GCC_Filestates defaultState = AES_FSREADY,
                              int removeDelay = 10,
                              bool removeBefore = false,
                              int nrOfRetries = 0,
                              int retryTime = 10);

   //## Operation: addDestinationSet%387C2C0401D4; C++
   //	Adds a new destinationset for a already defined transfer
   //	queue.
   unsigned int addDestinationSet(std::string& transferQueue,
                                  std::string& destinationSet,
                                  AES_GCC_Filestates defaultState = AES_FSREADY,
                                  int removeDelay = 10,
                                  bool removeBefore = false,
                                  int nrOfRetries = 0,
                                  int retryTime = 10);

   //## Operation: removeDestinationSet%387C2C0401DE; C++
   //	Removes a destinationset from the database.
   unsigned int removeDestinationSet(std::string& transferQueue,
                                     std::string& destinationSet);

   //## Operation: getErrorCodeText%387C2C0401F2; C++
   //	Retreives an error code text.
   const char * getErrorCodeText(unsigned int returnCode);

   //## Operation: lockTransferQueue%38B3D8BD02D6; C++
   //	Prevents a transfer queue from being removed by another
   //	user.
   unsigned int lockTransferQueue(std::string& transferQueue,
                                  std::string& destinationSet,
                                  std::string& application);

   //## Operation: unlockTransferQueue%38B3D8C3009A; C++
   //	Unlocks a transfer queue.
   unsigned int unlockTransferQueue(std::string& transferQueue,
                                    std::string& destinationSet,
                                    std::string& application);

   //## Operation: isTransferQueueLocked%38B3D8C80033; C++
   //	Checks if a transfer queue is locked.
   unsigned int isTransferQueueLocked(std::string& transferQueue,
                                      std::string& destinationSet,
                                      bool& locked,
                                      std::string& application);

   //## Operation: resendOneFailed%3A6EA4370054; C++
   //	Resends all files in status FS_FAILED for a specific
   //	destinationset.
   unsigned int resendOneFailed(std::string& transferQueue,
                                std::string& destinationSet,
                                std::string& newDestinationSet);

   //## Operation: resendAllFailed%3A6EA4490168; C++
   //	Resends  all files in status FS_FAILED for all
   //	destinationsets that the transfer queue is defined to.
   unsigned int resendAllFailed(std::string& transferQueue);

   //## Operation: open%3B139EB000D5; C++
   //	Open a generic connection to the server.
   unsigned int open(std::string subsystem, std::string applicationBlock);

   //## Operation: open%3AF2A8890275; C++
   //	Open a static connection towards the server.
   unsigned int open(std::string& transferQueue, int applicationBlock = 0);

   //## Operation: close%3AF2B45D0355; C++
   //	Close the connection to a transfer queue.
   unsigned int close();

   //## Operation: createTransferQueue%3AF7F74B0119; C++
   //	Creates a new transfer queue in the database.
   unsigned int createTransferQueue(std::string& transferQueue,
                                    std::string& destinationSet,
                                    std::string& sourceDirectory,
                                    AES_GCC_Filestates defaultState,
                                    int removeDelay,
                                    bool removeBefore,
                                    int retryTimeInterval,
                                    int retryTimes,
                                    AES_AFP_Renametypes rename,
                                    std::string& fileTemplate,
                                    std::string& userGroup);

   //## Operation: createTransferQueue%3EAD23C6016B; C++
   //	Creates a new transfer queue in the database.
   unsigned int createTransferQueue(std::string& transferQueue,
                                    std::string& destinationSet,
                                    std::string& sourceDirectory,
                                    AES_GCC_Filestates defaultState,
                                    int removeDelay,
                                    bool removeBefore,
                                    int retryTimeInterval,
                                    int retryTimes,
                                    AES_AFP_Renametypes rename,
                                    std::string& fileTemplate,
                                    std::string& userGroup,
                                    std::string& nameTag);

   // uabmha: CNI 1135, added arguments needed for afpdef -k option
   //## Operation: createTransferQueue
   //	Creates a new transfer queue in the database.
   unsigned int createTransferQueue(std::string& transferQueue,
                                    std::string& destinationSet,
                                    std::string& sourceDirectory,
                                    AES_GCC_Filestates defaultState,
                                    int removeDelay,
                                    bool removeBefore,
                                    int retryTimeInterval,
                                    int retryTimes,
                                    AES_AFP_Renametypes rename,
                                    std::string& fileTemplate,
                                    std::string& userGroup,
                                    std::string& nameTag,
                                    long startupSequenceNumber,
                                    bool treatAsDefaultValue);

  /* unsigned int createTransferQueue(   OmHandler& omHandler,
									   std::string& transferQueue,
                                       std::string& destinationSet,
                                       std::string& sourceDirectory,
                                       AES_GCC_Filestates defaultState,
                                       int removeDelay,
                                       bool removeBefore,
                                       int retryTimeInterval,
                                       int retryTimes,
                                       AES_AFP_Renametypes rename,
                                       std::string& fileTemplate,
                                       std::string& userGroup,
                                       std::string& nameTag,
                                       long startupSequenceNumber,
                                       bool treatAsDefaultValue);*/

   //## Operation: getEvent%3AFB815C02A1; C++
   //	Initiates an event for use by the calling application.
   unsigned int getEvent(AES_GCC_Eventcodes& eventCode, std::string& filename);

   //## Operation: getEventQueue%3AFBC73E001B; C++
   //	Retreives the queue for events.
   //OSF_Message_Queue<OSF_MT_SYNCH>* getEventQueue();
   ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* getEventQueue();

   //## Operation: getCmdQueue%3AFBC77603DD; C++
   //	Retreives the queue for commands.
   //OSF_Message_Queue<OSF_MT_SYNCH>* getCmdQueue();
   ACE_Message_Queue<ACE_MT_SYNCH,ACE_Monotonic_Time_Policy>* getCmdQueue();

   //## Operation: getSourceDirectoryPath%3B1353A0006F; C++
   //	Retreives the directory path for the transfer queue.
   unsigned int getSourceDirectoryPath(std::string& transferQueue,
                                       std::string& sourceDirectory);

   //## Operation: setSourceDirectoryPath%3B1353B100F6; C++
   //	Sets a directory path for a transfer queue.
   unsigned int setSourceDirectoryPath(std::string& transferQueue,
                                       std::string& sourceDirectory);

   //## Operation: sendFile%3B1CAE0E0329; C++
   //	Sends a file to a remote destination.
   int sendFile(std::string& filename,
                AES_GCC_Format transferMode,
                std::string fileMask = "",
                bool isDirectory = false,
                AES_GCC_Filestates fileState = AES_FSUSEDEFAULT);

   //## Operation: lastReportedFile%3B3F1DB902DD; C++
   //	Retreives the last reported file.
   unsigned int lastReportedFile(std::string transferQueue,
                                 std::string& originalFilename,
                                 std::string& generatedFilename);

   //## Operation: getDestinationSetList%3B3F1DC00175; C++
   //	Retreives a list of defined transfer queues.
   unsigned int getDestinationSetList(std::list<std::string>& destinationSetList);

   //## Operation: getFileStatus%3B3F238F01C3; C++
   //	Retreives the status for a file. The status is retreived
   //	for the first found file for a destinationset.
   unsigned int getFileStatus(std::string filename, AES_GCC_Filestates& status);

   //## Operation: removeFile%3B40339C0252; C++
   //	Removes a file from afp. The file is removed from all
   //	destinationsets for the transfer queue.
   unsigned int removeFile(std::string transferQueue, std::string filename);

   //## Operation: removeFile%3B4033A3016C; C++
   //	Removes a file from a destinationset.
   unsigned int removeFile(std::string transferQueue,
                           std::string destinationSet,
                           std::string filename);

   //## Operation: sendFileManually%3B8DDB4B009F; C++
   //	Sends a file manually. Demands a transfer queue name.
   unsigned int sendFileManually(std::string transferQueue,
                                 std::string userGroup,
                                 std::string filename,
                                 std::string sourceDirectory,
                                 std::string destinationSet,
                                 int retryTimes = 10,
                                 int retryTimeInterval = -1,
                                 bool isDirectory = false);

   //----------------for APZ21230/5-758---------------------
   unsigned int removeFileFromSourceDir (std::string transferQueue, std::string filename);

	  //----------------for APZ21230/5-758---------------------
   unsigned int removeSourceDirFiles (std::string& transferQueue);

   unsigned int validateTqName(std::string& transferQueue);

   unsigned int validateTransferQueueParam(
                                      std::string& nameTag,
                                      std::string& fileTemplate,
                                      AES_GCC_Filestates defaultState,
                                      int removeDelay,
                                      int retryTimeInterval,
                                      int retryTimes,
                                      //Start - HT50930
                                      int startupSequenceNumber
                                      //End - HT50930
                                      );
 
   unsigned int validateDestinationSet(std::string& destinationSet);
   unsigned int validateRenameTemplate( std::string& nameTag, std::string& templateString);

   unsigned int checkDefaultStatus(unsigned int defaultState, std::string& destinationSet);


   // Additional Public Declarations
   //## begin aes_afp_apiinternal%387C2938033D.public preserve=yes
   //## end aes_afp_apiinternal%387C2938033D.public

  protected:
     // Additional Protected Declarations
     //## begin aes_afp_apiinternal%387C2938033D.protected preserve=yes
     //## end aes_afp_apiinternal%387C2938033D.protected

  private:

     //## Other Operations (specified)
     //## Operation: checkTqName%387C2C040210; C++
     //	Checks if the transfer queue name is correct.
     unsigned int checkTqName(std::string& transferQueue);

     //## Operation: checkDestinationSetName%387C2C040224; C++
     //	Checks if the destinationset name is correct.
     unsigned int checkDestinationSetName (std::string& destinationSet);

     //## Operation: checkFilename%3B8CF36900DD; C++
     //	Checks if the filename is correct.
     unsigned int checkFilename(std::string& filename);

     //## Operation: checkSourceDirectoryName%3BA04DD903E2; C++
     //	Check if source directory name is valid.
     unsigned int checkSourceDirectoryName(std::string& sourceDirectory);

     //## Operation: checkDefaultStatus%387C2C040242; C++
     //	Checks if the status if allowed.
     //unsigned int checkDefaultStatus(unsigned int defaultState, std::string& destinationSet);

     //## Operation: checkRemoveDelay%387C2C04024C; C++
     //	Checks if the remove delay is correct.
     unsigned int checkRemoveDelay(int& removeDelay);

     //## Operation: checkNrOfRetries%387C2C040260; C++
     //	Checks if the number of retries is correct.
     unsigned int checkNrOfRetries(int& nrOfRetries);

     //## Operation: checkRetryTime%387C2C040274; C++
     //	Checks if the retry time is correct.
     unsigned int checkRetryTime(int& retryTime);

     //## Operation: checkRenameType%3B77AE5D0145; C++
     //	Check if the rename type is valid.
     unsigned int checkRenameType(AES_AFP_Renametypes type);

     //## Operation: checkTransferMode%3B1CB12B009B; C++
     //	Checks if transfer mode is valid.
     unsigned int checkTransferMode(AES_GCC_Format mode);

     //## Operation: checkUserGroup%3E26609203C5; C++
     //	Check if user belongs to userGroup and if userGroup is
     //	correct.
//     unsigned int checkUserGroup(std::string& userGroup);

     //## Operation: checkNameTag%3EB75BC101A1
     //	Check if name tag contains illegal characters.
     unsigned int checkNameTag(std::string& nameTag);

     //## Operation: checkStartupSequenceNumber
     //	Check if startup sequence number is within limits.
	 // For	HL79347: To check startup sequence number for different file templates.
	 unsigned int checkStartupSequenceNumber(long startupSequenceNumber,std::string& fileTemplate);


     //## Operation: send%3AF8E74F0099
     //	This method is used to send data between client and
     //	server in AFP.
     int send(char* msg, 	// This parameter represents the fms_afp_msgblock that is
        // to be returned by this method.
        unsigned short& msgLen);

     //## Operation: recv%3AF8E74F00AD
     //	This method is used receive data from a connection
     //	between client and server in AFP.
     int recv(std::string& msg, 	// This parameter represents the fms_afp_msgblock that is
        // to be returned by this method.
        unsigned int& msgLen);

     //## Operation: putOnEventQueue%3B12523300FA; C++
     //	Puts an event on the queue.
     int putOnEventQueue(std::string& dataString, unsigned short message);

     //## Operation: recvFromServer%3B138CDC01A4; C++
     //	Receive answer from server.
     int recvFromServer(std::stringstream& strStream);

     //## Operation: recvFromServer%3B1752500378; C++
     //	Receive data from server.
     int recvFromServer();

     //## Operation: sendToServer%3B1363270197; C++
     //	Sends a request to the server.
     int sendToServer(unsigned short endFlag,
                      aes_afp_msgtypes msgKind,
                      aes_afp_msgtypes message,
                      unsigned short dataPart,
                      std::string& data);

     //## Operation: toUpper%3BB329AE038D; C++
     //	Converts a character string to upper case.
     char* toUpper(char* str);

     //## Operation: toUpper%3E55E99000CB; C++
     //	Converts a string object to upper case.
     void toUpper(std::string& str);

     // Additional Private Declarations

private: //## implementation

   //## Attribute: MAX_REMOVEDELAY%387C2FAB01A6
   //	Maximum remove delay time.
   //## begin aes_afp_apiinternal::MAX_REMOVEDELAY%387C2FAB01A6.attr preserve=no  private: int {UC} 59999
   const int MAX_REMOVEDELAY_;
   //## end aes_afp_apiinternal::MAX_REMOVEDELAY%387C2FAB01A6.attr

   //## Attribute: MIN_REMOVEDELAY%387C2FAB01BA
   //	Minimum remove delay time.
   //## begin aes_afp_apiinternal::MIN_REMOVEDELAY%387C2FAB01BA.attr preserve=no  private: int {UC} -1
   const int MIN_REMOVEDELAY_;
   //## end aes_afp_apiinternal::MIN_REMOVEDELAY%387C2FAB01BA.attr

   //## Attribute: MAX_NROFRETRIES%387C2FAB01CE
   //	Max number of retries.
   //## begin aes_afp_apiinternal::MAX_NROFRETRIES%387C2FAB01CE.attr preserve=no  private: int {UC} 100
   const int MAX_NROFRETRIES_;
   //## end aes_afp_apiinternal::MAX_NROFRETRIES%387C2FAB01CE.attr

   //## Attribute: MIN_NROFRETRIES%387C2FAB01E2
   //	Minimum number of retries.
   //## begin aes_afp_apiinternal::MIN_NROFRETRIES%387C2FAB01E2.attr preserve=no  private: int {UC} -1
   const int MIN_NROFRETRIES_;
   //## end aes_afp_apiinternal::MIN_NROFRETRIES%387C2FAB01E2.attr

   //## Attribute: MAX_RETRYTIME%387C2FAB01EC
   //	Maximum retry time.
   //## begin aes_afp_apiinternal::MAX_RETRYTIME%387C2FAB01EC.attr preserve=no  private: int {UC} 59999
   const int MAX_RETRYTIME_;
   //## end aes_afp_apiinternal::MAX_RETRYTIME%387C2FAB01EC.attr

   //## Attribute: MIN_RETRYTIME%387C2FAB0200
   //	Minimum retry time.
   //## begin aes_afp_apiinternal::MIN_RETRYTIME%387C2FAB0200.attr preserve=no  private: int {UC} 10
   const int MIN_RETRYTIME_;
   //## end aes_afp_apiinternal::MIN_RETRYTIME%387C2FAB0200.attr

   //## Attribute: MAX_DESTINATIONSET_LEN%387C2FAB0214
   //	Maximum destination name length.
   //## begin aes_afp_apiinternal::MAX_DESTINATIONSET_LEN%387C2FAB0214.attr preserve=no  private: int {UC} 32
   const int MAX_DESTINATIONSET_LEN_;
   //## end aes_afp_apiinternal::MAX_DESTINATIONSET_LEN%387C2FAB0214.attr

   //## Attribute: MAX_TRANSFERQUEUE_LEN%387C2FAB0228
   //	Maximum file name length.
   //## begin aes_afp_apiinternal::MAX_TRANSFERQUEUE_LEN%387C2FAB0228.attr preserve=no  private: int {UC} 32
   const int MAX_TRANSFERQUEUE_LEN_;
   //## end aes_afp_apiinternal::MAX_TRANSFERQUEUE_LEN%387C2FAB0228.attr

   //## Attribute: MAX_FILENAME_LEN%3B8CF20D01EB
   //## begin aes_afp_apiinternal::MAX_FILENAME_LEN%3B8CF20D01EB.attr preserve=no  private: int {U} 255
   int MAX_FILENAME_LEN_;
   //## end aes_afp_apiinternal::MAX_FILENAME_LEN%3B8CF20D01EB.attr

   //## Attribute: MAX_SOURCEDIRECTORY_LEN%3BA0579D036F
   //## begin aes_afp_apiinternal::MAX_SOURCEDIRECTORY_LEN%3BA0579D036F.attr preserve=no  private: int {UC} 255
   const int MAX_SOURCEDIRECTORY_LEN_;
   //## end aes_afp_apiinternal::MAX_SOURCEDIRECTORY_LEN%3BA0579D036F.attr

   //## Attribute: MAX_USERGROUP_LEN%3E2664710167
   //## begin aes_afp_apiinternal::MAX_USERGROUP_LEN%3E2664710167.attr preserve=no  private: int {U} 256
   int MAX_USERGROUP_LEN_;
   //## end aes_afp_apiinternal::MAX_USERGROUP_LEN%3E2664710167.attr

   //## Attribute: NOTALLOWEDCHARACTERS%3E478AA301CD
   //	Charcters not allowed in AFP.
   //## begin aes_afp_apiinternal::NOTALLOWEDCHARACTERS%3E478AA301CD.attr preserve=no  private: std::string {UC} "< > : \" / \\ |"
   const std::string NOTALLOWEDCHARACTERS_;
   //## end aes_afp_apiinternal::NOTALLOWEDCHARACTERS%3E478AA301CD.attr

   //## Attribute: ALLOWEDCHARACTERS%3E478AFC00B3
   //	Defines the allowed characters (numeral and alphabetic)
   //	in afp.
   //## begin aes_afp_apiinternal::ALLOWEDCHARACTERS%3E478AFC00B3.attr preserve=no  private: std::string {UC} "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-"
   const std::string ALLOWEDCHARACTERS_;
   const int MAX_NAMETAG_LEN_;
   //## end aes_afp_apiinternal::ALLOWEDCHARACTERS%3E478AFC00B3.attr

   // Data Members for Associations

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3B163DB7024D
   //## Role: aes_afp_apiinternal::threadManager%3B163DB80154
   //## begin aes_afp_apiinternal::threadManager%3B163DB80154.role preserve=no  private: OSF_Thread_Manager { -> 1RHN}
   //OSF_Thread_Manager *threadManager_;
   //ACE_Thread_Manager *threadManager_;
   //## end aes_afp_apiinternal::threadManager%3B163DB80154.role

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3DAA79E1028E
   //## Role: aes_afp_apiinternal::convSession%3DAA79E201D1
   //## begin aes_afp_apiinternal::convSession%3DAA79E201D1.role preserve=no  private: ACS_DSD_Session { -> 0..1RHN}

   //## end aes_afp_apiinternal::convSession%3DAA79E201D1.role

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3DAA7A300007
   //## Role: aes_afp_apiinternal::convService%3DAA7A3303E0
   //## begin aes_afp_apiinternal::convService%3DAA7A3303E0.role preserve=no  private: ACS_DSD_Service { -> 0..1RHN}

   //## end aes_afp_apiinternal::convService%3DAA7A3303E0.role

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3E3FC37B00D8
   //## Role: aes_afp_apiinternal::protocol%3E3FC37D00DA
   //## begin aes_afp_apiinternal::protocol%3E3FC37D00DA.role preserve=no  private: aes_afp_protocolapi { -> UHgN}
   aes_afp_protocolapi protocol_;
   //## end aes_afp_apiinternal::protocol%3E3FC37D00DA.role

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3E40B404010D
   //## Role: aes_afp_apiinternal::eventQueue%3E40B40501FF
   //## begin aes_afp_apiinternal::eventQueue%3E40B40501FF.role preserve=no  private: Osf_MessageQueue { -> UHgN}
   //Osf_MessageQueue eventQueue_;
   Ace_MessageQueue eventQueue_;
   //## end aes_afp_apiinternal::eventQueue%3E40B40501FF.role

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3E40B45300D5
   //## Role: aes_afp_apiinternal::cmdQueue%3E40B45400B8
   //## begin aes_afp_apiinternal::cmdQueue%3E40B45400B8.role preserve=no  private: Osf_MessageQueue { -> UHgN}
   //Osf_MessageQueue cmdQueue_;
   Ace_MessageQueue cmdQueue_;
   //## end aes_afp_apiinternal::cmdQueue%3E40B45400B8.role

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3E478C7600FC
   //## Role: aes_afp_apiinternal::apiTask%3E478C780326
   //## begin aes_afp_apiinternal::apiTask%3E478C780326.role preserve=no  private: aes_afp_apitask { -> RFHN}

   //## end aes_afp_apiinternal::apiTask%3E478C780326.role

   //## Association: afpapi_caa (CAA 109 0505)::<unnamed>%3D06FA8D01F0
   //## Role: aes_afp_apiinternal::apiPtr_%3D06FA8E0328
   //## begin aes_afp_apiinternal::apiPtr_%3D06FA8E0328.role preserve=no  private: AES_AFP_Api { -> 1RHN}
   AES_AFP_Api *apiPtr_;

   ACS_DSD_Session *convSession_;

   ACS_DSD_Client *convService_;

   aes_afp_apitask *apiTask_;
   std::string applicationSts;
   ACE_Thread_Mutex sendMutex_;
};

#endif
