#ifndef aes_afp_transferqueue_h
#define aes_afp_transferqueue_h 1

#include "aes_afp_template.h"

//#include "ACS_DSD_DSA2.h"
#include "aes_afp_file.h"
#include "aes_afp_cdhtask.h"
#include "aes_afp_criticalsection.h"
#include "aes_afp_dsdsessionmap.h"
#include "aes_afp_divide.h"
#include "aes_afp_renametypes.h"
#include "aes_afp_protocol.h"
#include <utility>
#include <map>
#include <string>
#include <ace/ACE.h>
#include <ace/Thread_Mutex.h>
#include <ace/Thread_Manager.h>
#include <ace/Message_Queue.h>
#include <aes_gcc_eventcodes.h>
#include <aes_gcc_filestates.h>
#include <aes_gcc_event.h>
#include <aes_gcc_util.h>
#include <aes_gcc_eventhandler.h>
#include <aes_cdh_destinationset.h>
//#include "aes_gcc_FileDestProtection.h"
//#include "aes_gcc_NameProtection.h"
#include <aes_gcc_log.h>

#define AUDITLOG_SYM_NAME "auditLog";

//	This class stores the file in a transfer queue.
typedef std::map< std::string , aes_afp_file* , std::less<std::string>  > FileMap;


//	This class is used to insert a file name and
//	corresponding file object in to class FileMap.
typedef std::pair< std::string , aes_afp_file*  > FileMapPair;

//	This class is used to insert a string, string pair in
//	StringMap.
typedef std::pair< std::string , std::string  > StringPair;

//	This class is responsible for a map of strings.
typedef std::map< std::string , std::string , std::less<std::string>  > StringMap;

//	This class is responsible for the message queue towards
//	the transfer queue.
typedef ACE_Message_Queue< ACE_MT_SYNCH ,ACE_Monotonic_Time_Policy  > MessageQueue_Tq;

//	This class is responsible for the transfer queues  in
//	the database.
//	Trace points: aes_afp_transferqueue traces in/out from
//	the operations.

#define AFP_ATTR_DELETE_COUNTDOWN_NM 	"deleteCountdown"
#define AFP_ATTR_RENAME_TEMPLATE_NM 	"renameTemplate"
#define AFP_ATTR_STARTUP_SEQUENCE_NM 	"startupSequenceNumber"
#define AFP_ATTR_NAME_TAGS_NM 		"nametags"
#define AFP_ATTR_RENAME_STATUS_NM 	"renameStatus"
#define AFP_ATTR_DEFAULT_STATUS_NM	"defaultStatus"
#define AFP_ATTR_SOURCE_FOLDER_NM	"sourceFolder"
#define AFP_ATTR_FILE_DEST_GRP_NM       "fileDestinationGroup"
#define AFP_ATTR_REMOVE_DELAY_NM	"removeDelay"

class aes_afp_transferqueue : public ACE_Task<ACE_MT_SYNCH>
{

public:
	static const int MAX_RETRY_ON_DEST_OPEN = 120;

	aes_afp_transferqueue();

	virtual ~aes_afp_transferqueue();

	/**
	 * @brief Activates the TQ thread to load data from disk
	 */
	virtual int open(void *args = 0);

	/**
	 *	@brief Data loading thread.
	 */
	virtual int svc(void);


	inline void setTqName(const std::string& tqName) { m_tqName.assign(tqName); }
	inline const char* getTqName() const { return m_tqName.c_str(); }

	inline void setDestinationName(const std::string& detinationName) { m_destinationName.assign(detinationName); }
	inline const char* getDestinationName() const { return m_destinationName.c_str(); }


	AES_GCC_Filestates getLastFileInTransfer();
	AES_GCC_Filestates getNextFileInTransfer();
	void getAP(string &ap);
	void getHostname(std::string &outputHostname);
	void getNodeLetter(std::string &outputNodeLetter);


	//	Initiates an order object.
	bool init (std::string transferQueue, 
		   std::string destinationSet, 
		   std::string userGroup, 
		   AES_GCC_Errorcodes& error, 
		   std::string sourceDirectory, 
	 	   AES_GCC_Filestates defaultState = AES_FSREADY, 
		   unsigned int removeDelay = 0, 
		   bool removeBefore = false, 
		   int retriesDelay = 0, 
		   int sendRetries = 0, 
		   AES_AFP_Renametypes rename = AES_NONE, 
		   std::string renameTemplate = "", 
		   bool updateRootFile = true, 
		   bool manual = false, 
		   std::string nameTag = "", 
		   long startupSequenceNumber = -1,
                   Dest_Set_Type dType=DUMMY);
	  // uabmha:. CNI 1135, set startup sequence nr. to -1
	  // (internal representation for 0) This makes it possible
	  // for afpls to know when afpdef -k 0 was set vs. when 0 was set by default.

      //## Operation: initFromFile%3B25D1100151; C++
      //	Initiates the transfer queue from a file.
      bool initFromFile (std::string pathToTqFile, AES_GCC_Errorcodes& error);
	bool initCdhTask(AES_GCC_Errorcodes& error);
      bool dirDelete(std::string path);

      //## Operation: getName%3B25D11001C9; C++
      //	Returns the name of process order.
      std::string getName ();

      //## Operation: getDestinationSet%3B25D1100205
      //	Returns name of destination set.
      std::string getDestinationSet ();

      //## Operation: getNameTag%3EBB795F0220
      //	Retrieves the name tag.
      std::string getNameTag ();
		void extractClassNmFromRdn( const string myObjRdn,string &myClassName);
      //## Operation: getStartupSequenceNumber
      //	Retrieves the startup sequence number.
      int getStartupSequenceNumber ();

      //## Operation: insertFile%3B25D110024C; C++
      //	Inserts a file into the transfer queue.
      bool insertFile (std::string filename, AES_GCC_Errorcodes& error, AES_GCC_Filestates status, std::string filePath = "", std::string caller = "", std::string originalFilename = "", bool isDirectory = false);

      //## Operation: removeAll%3B25D11002BA; C++
      //	Removes transfer queue in database.
      bool removeAll (AES_GCC_Errorcodes& error);

      //## Operation: readTransferQueueFromDisc%3B25D110036E; C++
      //	Read transfer queue data from disc.
      bool readTransferQueueFromDisc (std::string& pathToTqFile, AES_GCC_Errorcodes& error);

      void fillDestsetInfo(vector<TqInfo> &);

      //## Operation: fillInfoBuffer%3B25D11003B4; C++
      //	Retreives information from database and fills the
      //	supplied strstream buffer with the data.
      void fillInfoBuffer (std::string& listBuffer);
      void fillInfoBufferShort (std::string& listBuffer);	

      //## Operation: setRemoveDelay%3B25D1110008; C++
      //	Sets the remove delay time.
      void setRemoveDelay (int delay);

      //## Operation: getRemoveDelay%3B25D1110044; C++
      //	Returns the time for remove delay.
      int getRemoveDelay ();

      //## Operation: findNewestFile%3B25D11101D5; C++
      //	Finds the newest file for specified transfer queue.
      bool findNewestFile (std::string& listBuffer, AES_GCC_Errorcodes& error);

      //## Operation: findOldestFile%3B25D1110211; C++
      //	Find oldest file in specified transfer queue.
      bool findOldestFile (std::string& listBuffer, AES_GCC_Errorcodes& error);

      //## Operation: findExactFile%3B25D1110257; C++
      //	Finds a specific file
      bool findExactFile (std::string& filename, std::string& listBuffer, AES_GCC_Errorcodes& error);

      //## Operation: findExactFile%3B25D111029D; C++
      //	Finds an exact file.
      bool findExactFile (std::string filename);

      //## Operation: addTransferQueueInfo%3B25D11102D9; C++
      //	Adds transfer queue  info to supplied buffer.
      void addTransferQueueInfo (std::string& listBuffer);

      //## Operation: deleteFiles%3B25D111031F; C++
      //	Deletes files who are in status 'delete' and whos timer
      //	has experied.
      bool deleteFiles (AES_GCC_Errorcodes& error, std::map<std::string, std::string>& deleteMap);

      //## Operation: getFileStatus%3B25D111035B; C++
      //	Returns information about a file.
      bool getFileStatus (std::string filename, aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error);

      //## Operation: getOldestFileStatus%3B25D11103AB; C++
      //	Retreives the status for the oldest file.
      bool getOldestFileStatus (aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error);

      //## Operation: getYoungestFileStatus%3B25D112000A; C++
      //	Retreives information about the newest file.
      bool getYoungestFileStatus (aes_afp_datablock* dataBlock, AES_GCC_Errorcodes& error);

      //## Operation: shutDown%3B25D1120046; C++
      //	Shutdown order received, close the CDH connection and
      //	general clean up.
      void shutDown ();

      //## Operation: removeFile%3B25D1120190; C++
      //	Removes a file from database.
      virtual bool removeFile (std::string filename, AES_GCC_Errorcodes& error);

      //## Operation: getTransferQueueState%3B25D11201D6; C++
      //	Returns state for a transfer queue.
      AES_GCC_Filestates getTransferQueueState ();

      //## Operation: getAttributes%3B25D11203B7; C++
      //	Retreives attributes for a transfer queue.
      void getAttributes (aes_afp_datablock* dataBlock);

      //## Operation: setAttributes%3B25D1130015; C++
      //	Sets attributes for a transfer queue.
      bool setAttributes (int sendRetries, int removeDelay, AES_GCC_Filestates defaultState, int retryDelayTime, bool removeDelayBefore, std::string renameTemplate, std::string nameTag, long startupSequenceNumber, AES_GCC_Errorcodes& error, bool changeRenameTemplate= false); //HT50930

      //## Operation: stopSendFile%3B25D12801A6; C++
      //	Stop sending one file in CDH.
      bool stopSendFile (std::string filename, AES_GCC_Errorcodes& error);

      //## Operation: stopSendAll%3B25D12801E2; C++
      //	Stop sending of all files for a transfer queue.
      bool stopSendAll (AES_GCC_Errorcodes& error);

      //## Operation: lockTransferQueue%3B25D12900C1; C++
      //	Lock the transfer queue from removal.
      bool lockTransferQueue (std::string application, AES_GCC_Errorcodes& error);

      //## Operation: unlockTransferQueue%3B25D1290111; C++
      //	Unlock transfer queue so that it can be removed.
      bool unlockTransferQueue (std::string application, AES_GCC_Errorcodes& error);

      //## Operation: isTransferQueueLocked%3B25D1290157; C++
      //	Check if tranfer queue is locked.
      bool isTransferQueueLocked (std::string& application);

      //## Operation: changeFileStatus%3B25D528002C; C++
      //	Change the state for a file.
      bool changeFileStatus (AES_GCC_Filestates newState, std::string filename, AES_GCC_Errorcodes& error);

      //## Operation: getSendItemNames%3B25D1290270; C++
      //	Retrieves files names. If state is set to FS_DEFAULT,
      //	all send items will be retrieved.
      bool getSendItemNames (std::stringstream& streamBuffer, AES_GCC_Filestates state);

      //## Operation: transferFailedFile%3B25D12902FC; C++
      //	Transfer a file in status FS_FAILED to the same
      //	destinationset.
      virtual bool transferFailedFile (std::string filename, AES_GCC_Errorcodes& error);

      //## Operation: createTimeString%3B25EBDE00EA; C++
      //	Creates a character array from a date object.
      char* createTimeString (aes_afp_date_time& time, char* str);

      //## Operation: transferFailed%3B262ABF0227; C++
      //	Transfer  files in status FS_FAILED to the destination.
      bool transferFailed (AES_GCC_Errorcodes& error);

      //## Operation: getDirectory%3B279B3C0253; C++
      //	Retrieves the source directory.
      std::string getDirectory ();

      //## Operation: setDirectory%3B27A7DE00F5; C++
      //	Sets source directory path.
      void setDirectory (std::string newDirectory);

      //## Operation: openTransferQueue%3B27B16302BC; C++
      //	Opens a transfer queue and attaches to cdh
      //	destinationset.
      bool openTransferQueue (ACS_DSD_Session* dsdChannel, AES_GCC_Errorcodes& error);

      //## Operation: closeTransferQueue%3B407813020F; C++
      //	Closes a transfer queue.
      bool closeTransferQueue ();

      //## Operation: sendFile%3B27B83B0383; C++
      //	Inserts a file in the transfer queue.
      bool sendFile (std::string filename, AES_GCC_Errorcodes& error, AES_GCC_Format mode, std::string mask, bool isDirectory, std::string originalFilename, AES_GCC_Filestates fileState = AES_FSUSEDEFAULT);

      //## Operation: getApplChannel%3B28584603A5; C++
      //	Retrieves the channel to the attached application.
      ACS_DSD_Session* getApplChannel ();

      //## Operation: getLastReportedFileOrig%3B3C1E2A0043; C++
      //	Retreives the last reported file for a transfer queue.
      std::string getLastReportedFileOrig ();

      //## Operation: getLastReportedFileGen%3B3F3D9300DA; C++
      //	Retreives the last reported file for a transfer queue.
      std::string getLastReportedFileGen ();

      //## Operation: getStatusFile%3B3F3DC40275; C++
      //	Retrieves the status for a file.
      bool getStatusFile (std::string filename, AES_GCC_Filestates& status);

      //## Operation: getRenameFile%3B6F8BC400CA; C++
      //	Retrieves rename type for files in a transfer queue.
      AES_AFP_Renametypes getRenameFile ();

      //## Operation: localRenameFile%3B6F8DD300A0; C++
      //	Renames the file that is to be inserted.
      bool localRenameFile (std::string filename, std::string& renamedFile);

      //## Operation: getTemplate%3B7A2B340200; C++
      //	Retrieves the template for a transfer queue.
      std::string getTemplate ();

      //## Operation: getManual%3B8A405202F3; C++
      //	Returns true if transfer queue is manually initiated.
      bool getManual ();

      //## Operation: setManual%3B8DFF5B00F4; C++
      //	Sets a transfer queueu to manual.
      void setManual ();

      //## Operation: getSendItemDirectoryFlag%3BF516F302DD; C++
      //	Retrieves the directory flag for a file.
      bool getSendItemDirectoryFlag (std::string sendItemName);

      //## Operation: getNextFilename%3E2A60C10002; C++
      //	Retrieves the next filename reported to a transfer queue.
      bool getNextFilename (std::string sendItem, std::string& nextFilename);

      //## Operation: getUserGroup%3E2B94B90058; C++
      //	Retrieves the user group for a transfer queue.
      std::string getUserGroup ();

	  //Removes SourceDirectory associated with transfer queue in database.
      bool removeSourceDirectoryAll (AES_GCC_Errorcodes& error);								//for APZ21230/5-758

	  //Removes a file from SourceDirectory associated with transfer queue in database.
      bool removeFileFromSourceDirectory (std::string filename, AES_GCC_Errorcodes& error);

      bool restoreFromTmpFile(std::string sourceFilePath,std::string targetFilePath);  //For HI14735
	  // Additional Public Declarations
      //## begin aes_afp_transferqueue%3B25D08A0126.public preserve=yes
      //## end aes_afp_transferqueue%3B25D08A0126.public
	  	  bool firstDs_;    //HK91962
	
	  //Returns the transferqueu object directory path.  For TH_HK91962
	  std::string getTqObjectDirectory();

      bool initFromIMM (std::string objRDN,
			const string& tqName,
			const string& destsetName);

	bool getTqDn(const std::string & tq, std::string & tqDn);
	Dest_Set_Type getDestSetType();
	void setDestSetType(Dest_Set_Type dsetType);

	// Checks file state(READY/SEND) of the TransferQueue. For TR HW88888
	bool checkFileStatus();

  protected:

    //## Other Operations (specified)
      //## Operation: remTqFromFile%3B25D11100BC; C++
      //	Removes this transfer queue file from database.
      bool remTqFromFile (AES_GCC_Errorcodes& error);

      //## Operation: protectTransferQueue%3B25D112008C; C++
      //	Protects a transfer queue in GCC.
      virtual bool protectTransferQueue ();

      //## Operation: unprotectTransferQueue%3B25D11200C8
      //	Unprotects a transfer queue in GCC.
      virtual int unprotectTransferQueue ();

      //## Operation: unprotectDestinationSet%3B25D112014A; C++
      //	Unprotects a destinationset.
      void unprotectDestinationSet ();

      //## Operation: initTimeSortedMap%3B25D112029F; C++
      //	Initiates a map sorted by creation time.
      virtual void initTimeSortedMap ();

      //## Operation: resendFiles%3B25D11202E5; C++
      //	Resend files when server has restarted.
      virtual void resendFiles ();

      //## Operation: sendToCdh%3B25D112032B; C++
      //	Start sending of files to CDH.
      void sendToCdh (std::string filename, aes_afp_file* fileObject,AES_GCC_Errorcodes& error);

      //## Operation: sendHangupToCdh%3B25D1120371; C++
      //	Send hangup order to CDH.
      void sendHangupToCdh ();
      void sendHangupToCdh_stop ();

      //## Operation: sendStopAllToCdh%3B25D128026E; C++
      //	Send stop all files to CDH.
      virtual void sendStopAllToCdh ();

      //## Operation: eventNoAccessAfpFile%3B25D1280304; C++
      //	Sends an event when there is no access to a file in afp
      //	filesystem.
      void eventNoAccessAfpFile (std::string afpFile);

      //## Operation: eventNoAccessAfpDir%3B25D128034A; C++
      //	Event sent when access to the specified directory is not
      //	possible.
      void eventNoAccessAfpDir (std::string directory);

      //## Operation: eventTqProtect%3B25D1280391; C++
      //	Sends an event if the transfer queue could not be
      //	protected.
      void eventTqProtect ();

      //   Operation: eventTqUnProtect; C++
      //	Sends an event if the transfer queue could not be
      //	unprotected.
      void eventTqUnProtect ();

      //## Operation: eventDestSetProtect%3B25D12803D7; C++
      //	Sends an event if the destinationset could not be
      //	protected.
      void eventDestSetProtect ();

      //## Operation: eventFileRename%3BE64F330056; C++
      //	Sends an event if a file or directory could not be
      //	locally renamed.
      void eventFileRename (std::string filename);

      //## Operation: alarmTransferFailed%3B25D1290035; C++
      //	Sends an alarm when status of a file is changed to
      //	failed.
      void alarmTransferFailed (string filename);

      //## Operation: ceaseTransferFailed%3B25D129007B; C++
      //	Ceases the transfer failed alarm.
      void ceaseTransferFailed ();
	bool createDirectoryStructure(AES_GCC_Errorcodes& error);
	bool isSourceDir();
	bool isAuditLogSourceDir();
    // Additional Protected Declarations
      //## begin aes_afp_transferqueue%3B25D08A0126.protected preserve=yes
      //## end aes_afp_transferqueue%3B25D08A0126.protected

  private:
        Dest_Set_Type dsetType_;
	static std::string auditLogDirPath;   
	
    ACE_Recursive_Thread_Mutex synchOnFileMap;
    //## Other Operations (specified)
      //## Operation: protectDestinationSet%3B25D112010E; C++
      //	Protects a destinationset.
    //  bool protectDestinationSet (AES_GCC_Errorcodes& error);

      //## Operation: sendStopFileToCdh%3B25D1280228; C++
      //	Send a stop file message to CDH.
      void sendStopFileToCdh (std::string filePathName);

      //## Operation: transferQueueProtected%3B25D54A00AD; C++
      //	Check if the transfer queuet is protected.
      bool transferQueueProtected ();

	  std::string getSourceDirPath(aes_afp_file * fileObj);

    // Data Members for Associations

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3E50B02A0108
      //## Role: aes_afp_transferqueue::protocol%3E50B02B03E4
      //## begin aes_afp_transferqueue::protocol%3E50B02B03E4.role preserve=no  private: aes_afp_protocoltq {1 -> 1UHgN}
      aes_afp_protocoltq protocol_;
      //## end aes_afp_transferqueue::protocol%3E50B02B03E4.role

    // Additional Private Declarations
      //## begin aes_afp_transferqueue%3B25D08A0126.private preserve=yes
      //## end aes_afp_transferqueue%3B25D08A0126.private

  public:
      //## Operation: sendApplEvent%3B28589C002D; C++
      //	Sends an event to the attached application.
      void sendApplEvent (AES_GCC_Eventcodes applEvent, std::string dataText, AES_GCC_Errorcodes error);
      ACE_Thread_Manager *threadManager_;

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: filesDirectory%3B25D1400042
      //	Name for the directory where the files are stored.
      //## begin aes_afp_transferqueue::filesDirectory%3B25D1400042.attr preserve=no  private: std::string {U} 
      std::string filesDirectory_;
      //## end aes_afp_transferqueue::filesDirectory%3B25D1400042.attr

      //	Name for the directory for temporary storage at data area
      std::string tempDirectory_;
      
      //	Name for the directory of temp storage at Persistent area
      std::string tempDirectoryBnR_;

      //	Name for the directory where the files are stored temporary.
      std::string tempFilesDirectory_;

      //## Attribute: manualRunOnce%3B25D140038B
      //	True if the operation is to be run only once.
      //## begin aes_afp_transferqueue::manualRunOnce%3B25D140038B.attr preserve=no  protected: bool {U} false
      bool manualRunOnce_;
      //## end aes_afp_transferqueue::manualRunOnce%3B25D140038B.attr

	  //HE74701
	  bool transferInProgress_;

      //## Attribute: noDestination%3B25D14100BB
      //	Will be set to true if order have no destination.
      //## begin aes_afp_transferqueue::noDestination%3B25D14100BB.attr preserve=no  protected: bool {U} false
      bool noDestination_;
      //## end aes_afp_transferqueue::noDestination%3B25D14100BB.attr

      //## Attribute: afpRestarted%3B3F091C0177
      //## begin aes_afp_transferqueue::afpRestarted%3B3F091C0177.attr preserve=no  private: bool {U} false
      bool afpRestarted_;
      //## end aes_afp_transferqueue::afpRestarted%3B3F091C0177.attr

      //## Attribute: AFPFILEDIRECTORY%3E280B59019F
      //	Name of the file directory.
      //## begin aes_afp_transferqueue::AFPFILEDIRECTORY%3E280B59019F.attr preserve=no  private: std::string {UC} "SENDITEMS"
      const std::string AFPFILEDIRECTORY_;
      //## end aes_afp_transferqueue::AFPFILEDIRECTORY%3E280B59019F.attr

      //## Attribute: dummyDest%3E280BA9030C
      //## begin aes_afp_transferqueue::dummyDest%3E280BA9030C.attr preserve=no  private: std::string {UC} "-"
      const std::string dummyDest_;
      //## end aes_afp_transferqueue::dummyDest%3E280BA9030C.attr

      //## Attribute: TEXTCAUSE%3E280C760162
      //	Text string used in AFP alarm.
      //## begin aes_afp_transferqueue::TEXTCAUSE%3E280C760162.attr preserve=no  private: std::string {UC} "CAUSE"
      const std::string TEXTCAUSE_;
      //## end aes_afp_transferqueue::TEXTCAUSE%3E280C760162.attr

      //## Attribute: TEXTALARM%3E280CE90140
      //	Alarm text for AFP.
      //## begin aes_afp_transferqueue::TEXTALARM%3E280CE90140.attr preserve=no  private: std::string {UC} "FILE TRANSFER FAILED"
      const std::string TEXTALARM_;
      //## end aes_afp_transferqueue::TEXTALARM%3E280CE90140.attr

      //## Attribute: TEXTAFPFAULT%3E280D1F0183
      //	Text for cause in AFP alarm.
      //## begin aes_afp_transferqueue::TEXTAFPFAULT%3E280D1F0183.attr preserve=no  private: std::string {UC} "AP FILE PROCESSING FAULT"
      const std::string TEXTAFPFAULT_;
      //## end aes_afp_transferqueue::TEXTAFPFAULT%3E280D1F0183.attr

      const std::string TEXTFILENAME_;
      const std::string TEXTTEInfoID_;
      const std::string TEXTTQID_;
      const std::string TEXTINITDESTSETID_;
      const std::string TEXTRESPDESTSETID_;
      const std::string TEXTDESTINATION_;		
      const std::string TEXTAP_;
      const std::string TEXTAPNAME_;
      const std::string TEXTNODE_;
      const std::string TEXTNODENAME_;

      //## Attribute: TEXTDESTINATIONSET%3E280D5502EA
      //	Text for destination set header in AFP alarm.
      //## begin aes_afp_transferqueue::TEXTDESTINATIONSET%3E280D5502EA.attr preserve=no  private: std::string {U} "DESTINATION SET"
      std::string TEXTDESTINATIONSET_;
      //## end aes_afp_transferqueue::TEXTDESTINATIONSET%3E280D5502EA.attr

      //## Attribute: TEXTTRANSFERQUEUE%3E280D9A02FD
      //	Text for header in AFP alarm.
      //## begin aes_afp_transferqueue::TEXTTRANSFERQUEUE%3E280D9A02FD.attr preserve=no  private: std::string {UC} "TRANSFER QUEUE"
      const std::string TEXTTRANSFERQUEUE_;
      //## end aes_afp_transferqueue::TEXTTRANSFERQUEUE%3E280D9A02FD.attr

      const std::string DATAAP_;
      const std::string DATAAPNAME_;
      const std::string DATANODE_;
      const std::string DATANODENAME_;
      
      //## Attribute: PERCISTANTFILENAME%3E280E0F0297
      //	Name of the percistent file.
      //## begin aes_afp_transferqueue::PERCISTANTFILENAME%3E280E0F0297.attr preserve=no  private: std::string {UC} "persist.nr"
      const std::string PERCISTANTFILENAME_;
      //## end aes_afp_transferqueue::PERCISTANTFILENAME%3E280E0F0297.attr

      //## Attribute: AFPSERVERNAME%3E280E560022
      //	Name of AFP server.
      //## begin aes_afp_transferqueue::AFPSERVERNAME%3E280E560022.attr preserve=no  private: std::string {UC} "AES_AFP_server"
      const std::string AFPSERVERNAME_;
      //## end aes_afp_transferqueue::AFPSERVERNAME%3E280E560022.attr

      //## Attribute: applicationEventText%3E280E81001A
      //	Place holder for applications event text in AFP alarm.
      //## begin aes_afp_transferqueue::applicationEventText%3E280E81001A.attr preserve=no  private: std::string {U} ""
      std::string applicationEventText_;
      //## end aes_afp_transferqueue::applicationEventText%3E280E81001A.attr

      //## Attribute: TEXTPROBLEMDATA%3E6842260058
      //	Default text for problem data.
      //## begin aes_afp_transferqueue::TEXTPROBLEMDATA%3E6842260058.attr preserve=no  private: std::string {UC} "Transfer error"
      const std::string TEXTPROBLEMDATA_;
      //## end aes_afp_transferqueue::TEXTPROBLEMDATA%3E6842260058.attr

      //## Attribute: transferFile%3EC07CB200C0
      //	Name of the file current in transfer.
      //## begin aes_afp_transferqueue::transferFile%3EC07CB200C0.attr preserve=no  private: std::string {U} ""
      std::string transferFile_;
      //## end aes_afp_transferqueue::transferFile%3EC07CB200C0.attr

    // Data Members for Associations

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B260631018B
      //## Role: aes_afp_transferqueue::deleteMapPair%3B26063301AB
      //## begin aes_afp_transferqueue::deleteMapPair%3B26063301AB.role preserve=no  private: StringPair { -> 1UHgN}
      StringPair deleteMapPair_;
      //## end aes_afp_transferqueue::deleteMapPair%3B26063301AB.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B26068B0180
      //## Role: aes_afp_transferqueue::fileMapPair%3B26068C0177
      //## begin aes_afp_transferqueue::fileMapPair%3B26068C0177.role preserve=no  private: FileMapPair { -> 1UHgN}
      FileMapPair fileMapPair_;
      //## end aes_afp_transferqueue::fileMapPair%3B26068C0177.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B2606DD0015
      //## Role: aes_afp_transferqueue::destTask%3B2606DF025D
      //## begin aes_afp_transferqueue::destTask%3B2606DF025D.role preserve=no  private: aes_afp_cdhtask { -> 1UHgN}
      aes_afp_cdhtask destTask_;
      //## end aes_afp_transferqueue::destTask%3B2606DF025D.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B260716013A
      //## Role: aes_afp_transferqueue::timeSortedMapPair%3B26071700D7
      //## begin aes_afp_transferqueue::timeSortedMapPair%3B26071700D7.role preserve=no  private: StringPair { -> 1UHgN}
      StringPair timeSortedMapPair_;
      //## end aes_afp_transferqueue::timeSortedMapPair%3B26071700D7.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B2607510044
      //## Role: aes_afp_transferqueue::timeSortedMap%3B2607520122
      //## begin aes_afp_transferqueue::timeSortedMap%3B2607520122.role preserve=no  private: StringMap { -> 1UHgN}
      StringMap timeSortedMap_;
      //## end aes_afp_transferqueue::timeSortedMap%3B2607520122.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B262D8D028D
      //## Role: aes_afp_transferqueue::cdhManageQueue%3B262D900093
      //## begin aes_afp_transferqueue::cdhManageQueue%3B262D900093.role preserve=no  private: MessageQueue_Tq { -> 1RHN}
      MessageQueue_Tq *cdhManageQueue_;
      //## end aes_afp_transferqueue::cdhManageQueue%3B262D900093.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B262E1402C4
      //## Role: aes_afp_transferqueue::fileMap%3B262E160295
      //## begin aes_afp_transferqueue::fileMap%3B262E160295.role preserve=no  private: FileMap { -> 1UHgN}
      FileMap fileMap_;
      //## end aes_afp_transferqueue::fileMap%3B262E160295.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B2654760241
      //## Role: aes_afp_transferqueue::renameFile%3B265477007F
      //## begin aes_afp_transferqueue::renameFile%3B265477007F.role preserve=no  private: AES_AFP_Renametypes { -> 1UHgN}
      AES_AFP_Renametypes renameFile_;
      //## end aes_afp_transferqueue::renameFile%3B265477007F.role

      //## Association: afpadm_caa(CAA 109 0503::<unnamed>%3B285763022C
      //## Role: aes_afp_transferqueue::applChannel%3B2857640165
      //## begin aes_afp_transferqueue::applChannel%3B2857640165.role preserve=no  private: ACS_DSD_Session { -> 1RHN}
      ACS_DSD_Session *applChannel_;
      //## end aes_afp_transferqueue::applChannel%3B2857640165.role

	  // Additional Implementation Declarations
      //## begin aes_afp_transferqueue%3B25D08A0126.implementation preserve=yes
	  aes_afp_template* templateObj_;
      //## end aes_afp_transferqueue%3B25D08A0126.implementation
          aes_afp_template* templateObj_temp;

          bool renameTemplateToChange;

      //## Association: afpadm_caa
      //## Role: aes_afp_transferqueue::nextFileToBeTransfered
      //## begin aes_afp_transferqueue::nextFileToBeTransfered.role preserve=no
	  FileMap::iterator nextFileToBeTransfered_Itr;
      //## end aes_afp_transferqueue::nextFileToBeTransfered.role

      //## Association: afpadm_caa
      //## Role: aes_afp_transferqueue::lastFileToBeTransfered_Itr
      //## begin aes_afp_transferqueue::lastFileToBeTransfered_Itr.role preserve=no
	  FileMap::iterator lastFileToBeTransfered_Itr;
      //## end aes_afp_transferqueue::lastFileToBeTransfered_Itr.role

	  std::string m_tqName;

	  std::string m_destinationName;
		
	  bool m_reloadStatus;

	  bool isShutdownSignaled_; // For TR HW88888
};

#endif
