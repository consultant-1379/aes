//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3B27284C00A5.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3B27284C00A5.cm

//## begin module%3B27284C00A5.cp preserve=no
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
//	 190 89-CAA 109 0503
//
//	AUTHOR
//	 2003-01-18 UAB/KB/AU DAPA
//
//	REVISION
//	  A 2003-01-18 DAPA
//	  B 2003-05-08 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3B27284C00A5.cp

//## Module: aes_afp_database_fd%3B27284C00A5; Package specification
//## Subsystem: AFP::afpadm_caa::inc%3AFFB40F01D1
//## Source file: Z:\ntaes\afp\afpadm_caa\inc\aes_afp_database_fd.h

#ifndef aes_afp_database_fd_h
#define aes_afp_database_fd_h 1

//## begin module%3B27284C00A5.additionalIncludes preserve=no
//## end module%3B27284C00A5.additionalIncludes

//## begin module%3B27284C00A5.includes preserve=yes
//## end module%3B27284C00A5.includes

//#include "AES_GCC_FileDestProtection.h"
#include <aes_gcc_log.h>
#include <aes_gcc_util.h>
#include "aes_afp_msgtypes.h"
#include "aes_afp_objectfactory.h"
#include "aes_afp_threadmap.h"
#include "aes_afp_timestamp.h"
#include "aes_afp_transferqueue.h"
#include "aes_afp_divide.h"
#include "aes_afp_protocol.h"
#include <fstream>
#include <utility>
#include <map>
#include <string>

#define AES_AFP_FILE_STREAM_POLICY  "FileStreamPolicy"

typedef std::map< std::string , AES_GCC_Eventcodes > TransferQueueEventMap;
typedef std::map< std::string , aes_afp_transferqueue*  > TransferQueueMap;
typedef std::pair< std::string , AES_GCC_Eventcodes > TransferQueueEventPair;
typedef std::pair< std::string , aes_afp_transferqueue*  > TransferQueuePair;
typedef std::map< std::string , std::string  > DeleteMap;

class aes_afp_database_fd 
{

  public:
      aes_afp_database_fd();

      virtual ~aes_afp_database_fd();


      //## Operation: removeTransferQueue%3B2720750117; C++
      //	Removes a transfer queue from database.
      bool removeTransferQueue (std::string transferQueue, std::string username, AES_GCC_Errorcodes& error);

      //## Operation: createFile%3B2720750135; C++
      //	Creates a file in the database.
      bool createFile (std::string transferQueue, std::string filename, std::string generation, AES_GCC_Filestates filenameState, AES_GCC_Errorcodes& error, std::string caller, std::string username);

      //## Operation: createExactFile%3B272075017B; C++
      //	Creates a exact file  for a specific destination.
      bool createExactFile (std::string transferQueue, std::string destinationSet, std::string filename, std::string generation, AES_GCC_Filestates fileState, AES_GCC_Errorcodes& error, std::string caller, std::string username, bool isDirectory = false);

      //## Operation: findTransferQueue%3B27207501C1; C++
      //	Search for transfer queue.
      bool findTransferQueue (std::string transferQueue, std::string destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error);

      //## Operation: destroy%3B27207501E9; C++
      //	Destroys the database.
      bool destroy (AES_GCC_Errorcodes& error);

      //## Operation: addDestinationSet%3B2720750212; C++
      //	Adds a destinationset to a transfer queue.
      bool addDestinationSet (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error);

      //## Operation: removeDestinationSet%3B272075023A; C++
      //	Removes a destinationset from a transfer queue.
      bool removeDestinationSet (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error);

      //## Operation: changeFileStatus%3B2720750258; C++
      //	Change status for a file.
      bool changeFileStatus (std::string transferQueue, std::string destinationSet, std::string filename, AES_GCC_Filestates newStatus, AES_GCC_Errorcodes& error);

      //## Operation: init%3B272075029E
      //	Initiates the database.
      bool init (AES_GCC_Errorcodes& error);

      //## Operation: getOneInformation%3B27207502C6; C++
      //	Retrieves information about one transfer queue.
      bool getOneInformation (std::string transferQueue, std::string& destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error);

      vector<TqInfo> infoAllDestset(std::string);

      //## Operation: getAllInformation%3B27207502EE; C++
      //	Retrieves information about all orders in database.
      bool getAllInformation (std::string& listBuffer, AES_GCC_Errorcodes& error);
      bool getAllInformationShort (std::string& listBuffer, AES_GCC_Errorcodes& error);	

      //## Operation: deleteFiles%3B272075035C; C++
      //	Deletes files that are in status ' delete'.
      void deleteFiles ();

      //## Operation: getFileStatus%3B2720750384; C++
      //	Retreives information about a specific file.
      bool getFileStatus (std::string transferQueue, std::string destinationSet, std::string filename, aes_afp_datablock* dBlock, AES_GCC_Errorcodes& error);

      //## Operation: getStatusOldFile%3B27207503AC; C++
      //	Collects information about the oldest reported file.
      bool getStatusOldFile (std::string transferQueue, std::string destinationSet, aes_afp_datablock* dBlock, AES_GCC_Errorcodes& error);

      //## Operation: getStatusNewFile%3B27207503CA; C++
      //	Collects information about the newest file.
      bool getStatusNewFile (std::string transferQueue, std::string destinationSet, aes_afp_datablock* dBlock, AES_GCC_Errorcodes& error);

      //## Operation: removeFile%3B272076000A; C++
      //	Removes a file.
      bool removeFile (std::string transferQueue, std::string destinationSet, std::string filename, std::string username, AES_GCC_Errorcodes& error);

      //## Operation: transferQueueDefined%3B2720760032; C++
      //	Check to see if a transfer queue is defined.
      bool transferQueueDefined (std::string transferQueue, AES_GCC_Errorcodes& error);

      //## Operation: transferQueueDestinationSetDefined%3B2720760050; C++
      //	Checks if transfer queue is defined to a destinationset.
      bool transferQueueDestinationSetDefined (std::string transferQueue, std::string destinationSet, AES_GCC_Errorcodes& error);

      //## Operation: getTransferQueueAttributes%3B2720760078; C++
      //	Retreives attributes for a transfer queue.
      bool getTransferQueueAttributes (std::string transferQueue, std::string destinationSet, aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error);

      //## Operation: setTransferQueueAttributes%3B27207600A0; C++
      //	Sets attributes for a transfer queue.
      bool setTransferQueueAttributes (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error, int sendRetries, unsigned int removeDelay, AES_GCC_Filestates defaultState, int retryDelayTime,std::string renameTemplate, std::string nameTag, long startupSequenceNumber, bool removeDelayBefore = false, bool changeRenameTemplate = false); //HT50930

      //## Operation: infoExactFile%3B27208A0059; C++
      //	Returns information about a file.
      bool infoExactFile (std::string transferQueue, std::string destinationSet, std::string filename, std::string& listBuffer, AES_GCC_Errorcodes& error);

      //## Operation: infoOldestFile%3B27208A00E5; C++
      //	Retrieves information about the oldest file in a
      //	transfer queue.
      bool infoOldestFile (std::string transferQueue, std::string destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error);

      //## Operation: infoNewestFile%3B27208A010D; C++
      //	Retrieves information about the newest file.
      bool infoNewestFile (std::string transferQueue, std::string destinationSet, std::string& listBuffer, AES_GCC_Errorcodes& error);

      //## Operation: stopSendFile%3B27208A0135; C++
      //	Stop the transfering of a file.
      bool stopSendFile (std::string transferQueue, std::string destinationSet, std::string filename, std::string username, AES_GCC_Errorcodes& error);

      //## Operation: stopSendAll%3B27208A015D; C++
      //	Stop sending of all files in an transfer queue.
      bool stopSendAll (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error);

      //## Operation: getInformationDestinationSets%3B27208A017B; C++
      //	Retreives information about all destinationsets for a
      //	transfer queue.
      bool getInformationDestinationSets (std::string transferQueue, std::string& streamBuffer, AES_GCC_Errorcodes& error);

      //## Operation: lockTransferQueue%3B27208A01A4; C++
      //	Locks a transfer queue.
      bool lockTransferQueue (std::string transferQueue, std::string destinationSet, std::string application, AES_GCC_Errorcodes& error);

      //## Operation: unlockTransferQueue%3B27208A01CC; C++
      //	Unlocks a transfer queue.
      bool unlockTransferQueue (std::string transferQueue, std::string destinationSet, std::string application, AES_GCC_Errorcodes& error);

      //## Operation: isTransferQueueLocked%3B27208A01F4; C++
      //	Check if a transfer queue is locked.
      bool isTransferQueueLocked (std::string transferQueue, std::string destinationSet, std::string& application, AES_GCC_Errorcodes& error);

      //## Operation: resendOne%3B27208A02EE; C++
      //	Resends failed files for one destinationset.
      bool resendOne (std::string transferQueue, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error, std::string newDestinationSet);

      //## Operation: resendOneFile%3B27208A0316; C++
      //	Resends a failed file.
      bool resendOneFile (std::string transferQueue, std::string filename, std::string destinationSet, std::string username, AES_GCC_Errorcodes& error, std::string newDestinationSet);

      //## Operation: resendAll%3B27208A033E; C++
      //	Resends files in status FS_FAILED for all
      //	destinationsets.
      bool resendAll (std::string transferQueue, std::string username, AES_GCC_Errorcodes& error);

      //## Operation: createTransferQueue%3B27208A0366; C++
      //	Creates a transfer queue in the database.
      bool createTransferQueue (std::string transferQueue, std::string destinationSet, AES_GCC_Errorcodes& error, std::string sourceDirectory, AES_GCC_Filestates defaultState, int removeDelay, bool removeBefore, int retryTimeInterval, int retryTimes, AES_AFP_Renametypes renameFile, std::string renameTemplate, std::string userGroup, std::string nameTag, long startupSequenceNumber, Dest_Set_Type dType=DUMMY);

      //## Operation: sendFile%3B2766700190; C++
      //	Sends a file to CDH.
      bool sendFile (std::string transferQueue, std::string filename, AES_GCC_Errorcodes& error, AES_GCC_Format mode, std::string mask, bool isDirectory = false, AES_GCC_Filestates fileState = AES_FSUSEDEFAULT);

      //## Operation: getSourceDirectory%3B27984801CA; C++
      //	Retrieves the source directory path for a transfer queue.
      bool getSourceDirectory (std::string transferQueue, std::string& sourceDirectory, AES_GCC_Errorcodes& error);

      //## Operation: setSourceDirectory%3B27A713020B; C++
      //	Sets the source directory for a transfer queue.
      bool setSourceDirectory (std::string transferQueue, std::string sourceDirectory, AES_GCC_Errorcodes& error);

      //## Operation: openTransferQueue%3B27B08C028B; C++
      //	Opens a transfer queue and attaches to cdh.
      bool openTransferQueue (std::string transferQueue, ACS_DSD_Session* dsdChannel, AES_GCC_Errorcodes& error);

      //## Operation: closeTransferQueue%3B407E330386; C++
      //	Closes a transfer queue.
      bool closeTransferQueue (std::string transferQueue, AES_GCC_Errorcodes& error);

      //## Operation: getLastReported%3B3F38FC0184; C++
      //	Retreives the last reported file.
      bool getLastReported (std::string transferQueue, std::string& strBuffer, AES_GCC_Errorcodes& error);

      //## Operation: getDestinationSetList%3B3F390202CD; C++
      //	Retrieves a list of destinations in AFP
      bool getDestinationSetList (std::string& strBuffer, AES_GCC_Errorcodes& error);

      //## Operation: getFileStatus%3B3F39050282; C++
      //	Retrieves status for a file.
      bool getFileStatus (std::string filename, AES_GCC_Filestates& status, AES_GCC_Errorcodes& error);

      //## Operation: sendManually%3B45E4B601D1; C++
      //	Sends a file manually to a destinationset.
      bool sendManually (std::string transferQueue, std::string filename, std::string sourceDirectory, std::string destinationSet, int retryTimes, int retryInterval, bool isDirectory, std::string username, std::string userGroup, AES_GCC_Errorcodes& error);

      //## Operation: getNextFilename%3E2A6DB20254; C++
      //	Retrieves the next filename in a transfer queue.
      bool getNextFilename (std::string transferQueue, std::string destinationSet, std::string filename, std::string& nextFilename, AES_GCC_Errorcodes& error);

      //## Operation: cdhChangeFileStatus%3BB045300127; C++
      //	Change status for a file. Used only by CDH.
      bool cdhChangeFileStatus (std::string transferQueue, std::string destinationSet, std::string filename, AES_GCC_Filestates newStatus, AES_GCC_Errorcodes& error);

      //## Operation: checkSecurity%3E2C352D0251; C++
      //	Checks if a username is member of a transfer queues user
      //	group.
      bool checkSecurity (std::string& username, std::string& userGroup);

      bool sendApplEvent(std::string transferQueue, AES_GCC_Eventcodes evc, AES_GCC_Errorcodes errc);

	  //--------------for APZ21230/5-758----------------
	  //	Removes a file.From Source Directory
      bool removeFileFromSourceDirectory (std::string transferQueue, std::string destinationSet, std::string filename, std::string username, AES_GCC_Errorcodes& error);

	  //--------------for APZ21230/5-758----------------
	   //	Removes a SourceDirectory associated with TQ
      bool removeSourceDirectoryFiles (std::string transferQueue, std::string username, AES_GCC_Errorcodes& error);
	  	     //--------------for TR_HK91965----------------
	   //	Resets the persistency number
      bool RetainPersistantNumber(unsigned int tmppersistentNr_,std::string transferqueue);
		     //--------------for TR HW88888---------------
          //   Checks file state(READY/SEND) of the TransferQueue  
      bool checkFileStatus(std::string transferQueue, std::string destinationSet, AES_GCC_Errorcodes& error);
	  
    // Additional Public Declarations
      //## begin aes_afp_database_fd%3B272032011B.public preserve=yes
      //## end aes_afp_database_fd%3B272032011B.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_database_fd%3B272032011B.protected preserve=yes
      //## end aes_afp_database_fd%3B272032011B.protected

  private:

      bool loadTQsFromIMM();

      bool loadTQsDataFromDisk();

    //## Other Operations (specified)

      //## Operation: deletePhysFile%3B27208A0212; C++
      //	Deletes the physical file.
      bool deletePhysFile (const std::string& sourceFile);

      //## Operation: protectTransferQueue%3B27208A023A; C++
      //	Protects a transfer queue in GCC.
      bool protectTransferQueue (std::string transferQueue);

      //## Operation: unprotectTransferQueue%3B27208A0262; C++
      //	Unprotects a transfer queue in GCC.
      void unprotectTransferQueue (std::string transferQueue);

      //## Operation: transferQueueProtected%3B27208A028A; C++
      //	Check if the transfer queue is protected.
      bool transferQueueProtected (std::string transferQueue);

      //## Operation: rewriteRootFile%3B27208A02D0; C++
      //	Used to rewrite afprootfile if a order_dest directory is
      //	missing.
      bool rewriteRootFile (std::string& skipThisLine) const;

      //## Operation: doDelete%3B6F81D902C8; C++
      //	Physical deletes a file or a directory tree.
      bool doDelete (std::string path);
	void deleteTQDirIfNotExistsInIMM(std::string& path);

      //## Operation: setTransferQueueManually%3B8DFCAE0171; C++
      //	Sets a transfer queue to be manually transfered.
      bool setTransferQueueManually (std::string transferQueue, std::string destinationSet);

      //## Operation: checkAuthorityFile%3E34FC360022; C++
      //	Find out what user groups that have authority to work on
      //	a specific file or folder.
      //	Returns true if a user has access to the file/folder,
      //	otherwise false.
      bool checkAuthorityFile (std::string& fileName, std::string& userName, AES_GCC_Errorcodes& error);

      //## Operation: convertRootFile%3E364AD9018A; C++
      //	Converts rootfile to new version.
      bool convertRootFile (std::string& rootFilePath)const;

      bool syncProtectedTransferQueues();       // HF20783

		//For TR HH24250--XCSRAJM
	  char* toUpper (char* str);
      bool filterDestSetEvent(std::string transferQueue, AES_GCC_Eventcodes evc);
	bool startDeleteFileThread();
	static void* deleteExpiredFiles(void * arg);
	bool removeExpiredFile(const std::string& fileName);

      //## end aes_afp_database_fd%3B272032011B.private

  private: //## implementation
    // Data Members for Class Attributes
	
      //## Attribute: trace%3B2720C401E3
      //	Character string for trautil.       //## begin aes_afp_database_fd::trace%3B2720C401E3.attr preserve=no  private: char[400] {U} 
     // char trace_[400];//commented out for cpp check
      //## end aes_afp_database_fd::trace%3B2720C401E3.attr

      //## Attribute: MAX_DESTINATIONSETS%3E2AA7270331
      //	Maximal number of destination sets.
      //## begin aes_afp_database_fd::MAX_DESTINATIONSETS%3E2AA7270331.attr preserve=no  private: int {U} 20
      int MAX_DESTINATIONSETS_;
      //## end aes_afp_database_fd::MAX_DESTINATIONSETS%3E2AA7270331.attr

    // Data Members for Associations

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B263F760218
      //## Role: aes_afp_database_fd::transferQueuePair%3B263F770129
      //## begin aes_afp_database_fd::transferQueuePair%3B263F770129.role preserve=no  private: TransferQueuePair { -> 1UHgN}
      TransferQueuePair transferQueuePair_;
      //## end aes_afp_database_fd::transferQueuePair%3B263F770129.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B263F9A0224
      //## Role: aes_afp_database_fd::transferQueueMap%3B263F9B003B
      //## begin aes_afp_database_fd::transferQueueMap%3B263F9B003B.role preserve=no  private: TransferQueueMap { -> 1UHgN}
      TransferQueueMap transferQueueMap_;
      //## end aes_afp_database_fd::transferQueueMap%3B263F9B003B.role

      TransferQueueEventMap transferQueueEvMap_;

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B2721C10101
      //## Role: aes_afp_database_fd::deleteMap%3B2721C2022E
      //## begin aes_afp_database_fd::deleteMap%3B2721C2022E.role preserve=no  private: DeleteMap { -> 1UHgN}
      DeleteMap deleteMap_;
      ACE_Event deleteFilesEv_;
      ACE_hthread_t deleteThreadHandle_;
      bool isShutdownSignaled_;
      //## end aes_afp_database_fd::deleteMap%3B2721C2022E.role
	//xcsrajm For TR HH54446
	  std::string uname;
    // Additional Implementation Declarations
      //## begin aes_afp_database_fd%3B272032011B.implementation preserve=yes
      //## end aes_afp_database_fd%3B272032011B.implementation
	  //
	  ACE_Recursive_Thread_Mutex m_synchOnTQMap;

};

//## begin aes_afp_database_fd%3B272032011B.postscript preserve=yes
//## end aes_afp_database_fd%3B272032011B.postscript

// Class aes_afp_database_fd 

//## begin module%3B27284C00A5.epilog preserve=yes
//## end module%3B27284C00A5.epilog


#endif
