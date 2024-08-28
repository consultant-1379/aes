/*=================================================================== */
/**
   @file   servr.h

   @brief Header file for ServR type module.

          This module contains all the declarations useful to
          specify the class.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       27/10/2011   XCHEMAD    Initial Release
==================================================================== */

   //##ModelId=3E00B4E90262
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef SERVR_H 
#define SERVR_H

/*====================================================================
                        INCLUDE DECLARATION SECTION
==================================================================== */
// C++ header files
#include <string>
#include <vector>
#include <openssl/err.h>

// ACE header files
#include <ace/Event_Handler.h>
#include <ace/Thread_Semaphore.h>

// Internal header files
#include <aes_cdh_destination.h>
#include <aes_cdh_destinationset.h>
#include <aes_cdh_block_dest_handler.h>
#include <aes_cdh_block_destset_handler.h>
#include <aes_cdh_file_init_dest_handler.h>
#include <aes_cdh_file_resp_dest_handler.h>
#include <aes_cdh_fileinitdestset_handler.h>
#include <aes_cdh_filerespdestset_handler.h>
#include <transdest.h>


using namespace std;


#define AES_CDH_DATA_TRANSFER_NBI_FOLDER_ATTR_NM 		"dataTransferForResponding"
#define AES_CDH_RESP_DEST_SET_CLASS_NM      			"AxeDataTransferRespondingDestinationSet"
#define AES_CDH_BLOCK_DEST_SET_ID      				"blockDestinationSetId"
#define AES_CDH_BLOCK_DEST_ID      				"blockDestinationId"
#define AES_CDH_RESP_DEST_SET_ID      				"respondingDestinationSetId"
#define AES_CDH_INIT_DEST_SET_CLASS_NM      			"AxeDataTransferInitiatingDestinationSet"
#define AES_CDH_INIT_DEST_SET_ID      				"initiatingDestinationSetId"
#define AES_CDH_BLOCK_DEST_SET_CLASS_NM      			"AxeDataTransferBlockDestinationSet"
#define AES_CDH_BLOCK_DEST_SET_ID      				"blockDestinationSetId"
#define PHYSICAL_SEPARATION_ATTR_NAME 				"physicalSeparationStatus"
#define NORTHBOUND_OBJ_DN					"northBoundId=1,networkConfigurationId=1"
#define AES_CDH_PUBLIC_NODE_ADDRESS				"/etc/cluster/nodes/this/networks/public/primary/address"
#define AES_CDH_PUBLIC2_NODE_ADDRESS				"/etc/cluster/nodes/this/networks/public2/primary/address"
//Start - HU10376
#define AES_CDH_PUBLIC2_CLUSTER_ADDRESS				"/etc/cluster/nodes/this/mip/primary_ps/address"
#define AES_CDH_IPROUTE_HOST_OFFSET				4
//End - HU10376
#define AES_AFP_PSA_FOLDER_PATH				"/cluster/storage/system/config/aes_afp/"
#define AES_CDH_FILEM_SERVICE_NM                		"com_fuse_module"
#define FILE_DESTSET_MAX_LIMIT 						70	
#define BLOCK_DESTSET_MAX_LIMIT 					30
#define CLUSTER_IPADDR_ATTR_NAME				"clusterIpAddress"

struct CommitInfo
{
        bool commitFlag;
        string destCommit;
};



/*=====================================================================
                        CONSTANT DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief destDirectory

          Directory for destination data
 */
/*=================================================================== */
const string destDirectory = "dest";	

/*=================================================================== */
/**
   @brief destfileName

          Destination backup file
 */
/*=================================================================== */
const string destfileName = "cdhdestfile.txt";

/*=================================================================== */
/**
   @brief destfileExName

          Extended destination parameter file
 */
/*=================================================================== */
const string destfileExName = "cdhdestfileex.txt";

/*=================================================================== */
/**
   @brief destSetDirectory

          Directory for destination set data
 */
/*=================================================================== */
const string destSetDirectory = "destset"; 

/*=================================================================== */
/**
   @brief destSetFileName

          Destination set backup file
 */
/*=================================================================== */
const string destSetFileName = "cdhdestsetfile.txt";

/*=================================================================== */
/**
   @brief dataBaseDirectory

          Directory for database files
 */
/*=================================================================== */
const string dataBaseDirectory = "ndb";

/*=================================================================== */
/**
   @brief blockidDirectory

          Directory for block-id files
 */
/*=================================================================== */
const string blockidDirectory = "bif";	

/*=================================================================== */
/**
   @brief backupDestDirectory

          Directory for backup files
 */
/*=================================================================== */
const string backupDestDirectory = "bfi";

/*=================================================================== */
/**
   @brief fstatname

          fileSentStatus backup file
 */
/*=================================================================== */
const string fstatname = "statusfile.txt";

/*=================================================================== */
/**
    @struct  status

    @brief   It holds the status of filedest

    @par filedest
	 Name of the file destination         
    @par fname
         File name
*/
/*=================================================================== */

struct status
{
	string filedest;
	string fname;
};

struct destNode
{
	ACS_APGCC_CcbId ccbId;
	string destinationName;
	int argc;
	char *argv[32];
	string destinationSetName;
	bool isPrimDest;
	bool isSecDest;
	bool isDestSet;
	string transferType;
};

struct destInfo
{
	ACS_APGCC_CcbId ccbId;
        string destinationName;
        int argc;
        char *argv[32];
        string destinationSetName;
        bool isPrimDest;
        bool isSecDest;
        bool isDestSet;
        string transferType;
	string advDestinationName;
};

/*=====================================================================
					FORWARD DECLARATION SECTION
==================================================================== */

class CmdHandler;
class TransDestSet;
class AES_CDH_FileInitDestCmdHandler;
class AES_CDH_FileRespDestCmdHandler;

/*=====================================================================
                        CLASS DECLARATION SECTION
==================================================================== */
/*===================================================================*/
/**
      @brief     ServR

                 The class handles the CDH requests
*/
/*=================================================================== */
class ServR    : public ACE_Event_Handler
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

      @brief           Class constructor

      @return          none

      @exception       none
   */
/*=================================================================== */
   	ServR();  
/*=====================================================================
                        CLASS DESTRUCTORS
==================================================================== */
/*=================================================================== */
   /**

      @brief           Class destructor

      @return          none

      @exception       none
   */
/*=================================================================== */
   virtual ~ServR();  

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  commandCode

           Holds the command code
*/
/*=================================================================== */
	typedef enum
	{
		CMD_DEFINE                   = 1,		/*!< Destination define */
		CMD_LIST                     = 2,		/*!< Destination list */
		CMD_CHANGE                   = 3,		/*!< Destination change */
		CMD_DELETE                   = 4,		/*!< Destination delete */
		CMD_SENDFILE                 = 5,		/*!< Send file */
		CMD_SENDRECORDFILE           = 6,		/*!< Send record file */
		CMD_STOPSENDFILE             = 7,		/*!< Stop sending file */
		CMD_STOPTASKS                = 8,		/*!< Stop tasks */
		CMD_OPENSESSION              = 9,  	/*!< Open session */ 
		CMD_CLOSESESSION             = 10, 	/*!< Close session */
		CMD_EXIT                     = 11, 	/*!< Exit */
		CMD_DESTEVENT                = 12,	/*!< Destination event */
		CMD_REMOVEUSERUNIQUE         = 13,	/*!< Remote user unique */
		CMD_SENDDONEOK               = 14,	/*!< Send done is ok */
		CMD_SETEVENTHANDLER          = 15,	/*!< Set event handler */
		CMD_REMOVEEVENTHANDLER       = 16,	/*!< Remove event handler */
		CMD_CHECKCONNECTION          = 17,	/*!< Check connection */
		CMD_CHECKCONNECTION_MANUALLY = 18,	/*!< Check connection manually */
		CMD_CHECKDESTINATIONSET_TYPE = 19,	/*!< Check destination set type */
		CMD_TRANSACTIONBEGIN         = 20,	/*!< Transaction begin for block transfer */
		CMD_TRANSACTIONEND           = 21,	/*!< Transaction end for block transfer */
		CMD_TRANSACTIONCOMMIT        = 22,	/*!< Transaction commit for block transfer */
		CMD_TRANSACTIONTERMINATE     = 23,	/*!< Transaction terminate for block transfer */
		CMD_GETLASTCOMMITTEDBLOCK    = 24,	/*!< Get last committed block */
		CMD_DEFINEDESTSET            = 25,	/*!< Define destination set */
		CMD_DELETEDESTSET            = 26,	/*!< Delete destination set */
		CMD_LISTDESTSET              = 27,	/*!< List destination set */
		CMD_SWITCHACTIVEDEST         = 28,	/*!< Switch active destination */
		CMD_CHANGEDESTSET            = 29,	/*!< Change destination set */
		CMD_CDH_STOPPING			 = 30,	/*!< CDH server is stopping */
		CMD_CDH_STOPPED				 = 31,  /*!< CDH server is stopping */
		CMD_OK                       = 128	/*!< Command ok */
	} commandCode;

/*=================================================================== */
/**
    @struct  checkAttributes

    @brief   Checks the attributes of the destination

    @par destName
         Destination name
    @par status
         Status
*/
/*=================================================================== */
	struct checkConnAttributes
	{
		std::string destName;
		AES_CDH_ResultCode status;
	};

/*===================================================================
                           PUBLIC METHOD
=================================================================== */
/*=================================================================== */
   /**
      @brief       This method is called when the service calls the run_service
		   method.

      @return      ACE_INT32
			Return value

      @exception   none
    */
/*=================================================================== */
	virtual int open();

/*=================================================================== */
   /**
      @brief       This method is called when the service is shutdown

      @return      ACE_INT32
                        Return value

      @exception   none
    */
/*=================================================================== */
	virtual int close();

/*=================================================================== */
   /**
      @brief       This method handles sessions

      @param	   none

      @return      int

      @exception   none
    */
/*=================================================================== */
	virtual int handle_input(ACE_HANDLE fd = ACE_INVALID_HANDLE);

/*=================================================================== */
	/**
	      @brief       This method is called by the reactor on server removal

	      @param	   none

	      @return      int

	      @exception   none
	 */
/*=================================================================== */
	virtual int handle_close();

/*=================================================================== */
   /**
      @brief       This method defines a destination

      @param       destName
                        Name of the destination

      @param       transType
                        Transfer type

      @param       argc
                        Argument count

      @param       argv
                        Argument list

      @param       userGroup
                        User group name

	  @param	   destination Rdn String

      @return      AES_CDH_ResultCode
                        Result code

      @exception   none
    */
/*=================================================================== */
	static AES_CDH_ResultCode define(const string destName, 
				         const string transType, 
				         ACE_INT32 &argc, ACE_TCHAR* argv[],
				         const string& userGroup,
					 const string& destRdn,
					 const string destSetName ="");

/*=================================================================== */
   /**
      @brief       This method changes the definition of destination

      @param       destName
                        Name of the destination

      @param       argc
                        Argument count

      @param       argv
                        Argument list

      @param	   userName
			User Group name

      @return      AES_CDH_ResultCode
                        Result code

      @exception   none
    */
/*=================================================================== */
	static AES_CDH_ResultCode changeAttr(const string destName, 
					     ACE_INT32 &argc, ACE_TCHAR* argv[],
					     const string& userName);
/*=================================================================== */
   /**
      @brief       This method lists the attributes of a destination

      @param       destName
                        Name of the destination

      @param	   attrs
			Vector containing the attribute list

      @return      AES_CDH_ResultCode
                        Result code

      @exception   none
    */
/*=================================================================== */
	static AES_CDH_ResultCode listDest(const string destName,
				           vector<AES_CDH_Destination::destAttributes> &attrs);

/*=================================================================== */
   /**
      @brief       This method deletes a destination

      @param       destName
                        Name of the destination

      @param       detailInfo
                        Detailed information

      @param       userName
                        User group name

      @return      AES_CDH_ResultCode
                        Result code
       @exception   none
    */
/*=================================================================== */
	static AES_CDH_ResultCode deleteDest(const string destName, 
					     string& detailInfo, 
					     const string& userName);

	static AES_CDH_ResultCode sendFile(const string destSetName, 
					   const string fileName, 
					   const string remoteSubDirName,
					   const string newFileName,
					   ACE_INT32 pid, ACE_INT32 destSetObj,
					   CmdHandler* cmdHdlr,   
					   const ACE_UINT64 ticks,
					   ACS_APGCC_Command *cmd,
					   const string userUnique, 
					   const AES_CDH_DestinationSet::transferMode trMode, 
					   const string fileMask,      
					   const bool isDir,          
					   const bool retryAfterRestart);

	static AES_CDH_ResultCode sendFileDone(const string userUnique, 
					       const string destSetName,
					       const string fileName, 
					       const string remoteSubDirName,
					       const string newFileName,
					       AES_CDH_ResultCode sendResult);

	static AES_CDH_ResultCode sendRecordFile(const string destSetName, 
						 const string streamName,
						 const ACE_INT32 streamSize,
						 const ACE_INT32 recordLength,
						 const string mainFileName, 
						 string &fileName, 
						 ACE_UINT64 &recordsSent,
						 ACE_INT32 pid, ACE_INT32 destSetObj,
						 CmdHandler* cmdHdlr, 
						 const ACE_UINT64 ticks);

	static AES_CDH_ResultCode sendRecordFileDone(const string destSetName,
						     const string mainFileName);

	static AES_CDH_ResultCode stopSendFile(const string destSetName,    
					       const string fileName,
					       const string remoteSubDirName,
					       const string newFileName); 

	static AES_CDH_ResultCode stopTasks(ACE_INT32 pid, 
					    ACE_INT32 destSetObj, 
					    bool allTasks=false);

	static AES_CDH_ResultCode stopSendRecordFile(const string destSetName);

	static bool checkStopFlag(const string destSetName,
				  const string fileName,
				  const string remoteSubDirName,
				  const string newFileName);

	static bool checkStopRecordFlag(const string destSetName,
					const string mainFileName);

	static bool ExistRecordFileTask(const string destSetName,
					const string mainFileName,
					CmdHandler *cmdHdlr, 
					ACE_UINT64 ticks);

	static AES_CDH_ResultCode setEventHandler(const string destSetName,
						  const string transferQueueName,
						  const string alarmText,
						  CmdHandler* cmdHdlr);

	static CmdHandler* getCmdHandler(const string destSetName);

	static void insertCmdHandler(CmdHandler* cmdHdlr);  // 010903

	static bool validCmdHandler(CmdHandler* cmdHdlr, 
				    const ACE_UINT64 ticks);   // 010903

	static void removeCmdHandler(CmdHandler* cmdHdlr);

	static AES_CDH_ResultCode removeEventHandler(const string destSetName,
						     CmdHandler* cmdHdlr);

	static AES_CDH_ResultCode checkConnection(string destName, 
						  bool force, 
                                                  vector<checkConnAttributes> &attrs);

	static AES_CDH_ResultCode checkConnectionManually(string destName,
                                                          bool force,
                                                          vector<checkConnAttributes> &attrs);

	static bool fileSentStatusSave(const string userUnique, 
                                       const string filename);

	static AES_CDH_ResultCode removeUserUnique(const string userUnique);


	static bool IsStandardDestOption(const string option);
	static bool IsExtendedDestOption(const string option);

	static AES_CDH_ResultCode saveDestinations(void);

	static string NTErrorText(void);

	static string getDataBaseDirectory(void);

	static string getClusterIP(const string NotificationAddress);

	static string getClusterIPPap();

	static AES_CDH_ResultCode checkDestinationSetType(const string &destset,
							  AES_CDH_DestinationSet::TQConnectAttributes askAttr,
							  AES_CDH_DestinationSet::TQConnectAttributes &realAttr);

	static AES_CDH_ResultCode transactionBegin(const string &destSetName);

	static AES_CDH_ResultCode transactionEnd(const string &destSetName,
											 unsigned int &applBlockNr);

	static AES_CDH_ResultCode transactionCommit(const string &destSetName,
												unsigned int &applBlockNr);

	static AES_CDH_ResultCode getLastCommittedBlock(const string &destSetName,
							unsigned int &applBlockNr);

	static AES_CDH_ResultCode transactionTerminate(const string &destSetName);

	static AES_CDH_ResultCode setAPIClosed(const string &destSetName);
	static void dumpRecordTasks(void);
	static void dumpFileTasks(void);
	static std::list<CmdHandler*> cmdHandlerList_;
	static ACE_Recursive_Thread_Mutex cmdHandlerListMX_;
	static std::list<TransDest*> destList_;
	static std::list<TransDestSet*> destSetList_;
	static bool getFileTransferStopEvent(const string destSetName, const string fileName, const string remoteSubDirName, const string newFileName, ACE_Event **fileTransferStopEvent);

	static void requestShutdownOfCmdHdlrThread(const string destSetName, CmdHandler * pCmdHdlr);
	
private:


	struct sendFileTask
	{
		ACE_INT32 pid;        // A unique identification of the calling object using process id
		ACE_INT32 destSetObj;    // and object pointer
		string destSetName;
		string fileName;
		string remoteSubDirName;
		string newFileName;
		bool   stopFlag;
		CmdHandler* CmdHdlr;
		ACE_UINT64 ticks;
		ACE_Thread_Semaphore* sem;
		ACE_Event *pStopFileTransferEvent;
	};

	struct sendRecordFileTask
	{
		ACE_INT32 pid;        // A unique identification of the calling object using process id
		ACE_INT32 destSetObj;    // and object pointer
		string destSetName;
		string mainFileName;
		string streamName;   // CNI 212 55/1-1253 Rebase
		bool   stopFlag;
		CmdHandler* CmdHdlr;
		ACE_UINT64 ticks;
		ACE_Thread_Semaphore* sem;
	};



	static bool virtualAlarm_;		// added for TR HH91483

	static string destfile_;

	static string destfileex_;


	//static ACE_Recursive_Thread_Mutex destMX_;
        static pthread_mutex_t destMx_;
        pthread_mutexattr_t mutex_attr;


	// static destination set variables
	static string destSetFile_;

	// static cmdHandler variables
	static map<string, CmdHandler*> destSetPair_;
        static ACE_Mutex destSetPairMX_; // TR HO68058

	// static sendFileTasks variables
	static std::list<sendFileTask> sendFileTaskList_;
	static ACE_Recursive_Thread_Mutex taskMX_;

	// static sendRecordFileTasks variables
	static std::list<sendRecordFileTask> sendRecordFileTaskList_;
	static ACE_Recursive_Thread_Mutex recordtaskMX_;

	// static SentFileStatus variables
	static string fileSentStatusFile_;
	static ACE_Recursive_Thread_Mutex taskSentFileStatus_MX_;
	static std::list<status *> statusList_;

	static ACE_Thread_Manager* threadManager_;

	static AES_CDH_ResultCode existDest(const string destName);

	bool setDataPath(void);

	bool readParameters(string &path1, string &path2);

	bool setDataPathForStatusFile(void);

	static bool fileSentStatusRead(const string userUnique,
				       const string filename, bool &fileIsSent);

	bool fileSentStatusRecover(void);

	static bool fileSentStatusRemove(const string userUnique);

	void recoverDestinations(void);

	static void upperToLower(string& str);
    
	static void cipher(string& str);
    
	static void decipher(string& str);
    
	bool createCDHDirs(void);

	static bool writeToFile(const std::string&, const std::string&);

	void resetFileInstanceTags(const std::string &);
    

	ACE_Thread_Mutex task_done_mutex;

	static ACE_Recursive_Thread_Mutex destSetMX_;

	//Start - HU10376
	static bool getPsConfiguredIP(const string&, string&);
	static string readIpAddressFromFile(const string&);
	//End - HU10376

public:
	//registering with openssl for mutual exclusion
	static int thread_setup(void);
	static int thread_cleanup(void);

	static bool isDestSetAttached(const std::string &);
	static AES_CDH_ResultCode isDestSetLimitCrossed(const std::string &);

	static ACE_Mutex respdestsetMutex;
	static ACE_Mutex initdestsetMutex;
	static ACE_Mutex blockdestsetMutex;

	static destInfo *pBlockDestInfo;
	static destInfo *pFileInitDestInfo;
	static destInfo *pFileRespDestInfo;

	static bool isInitDestSetDeleteTrig;
	static bool isRespDestSetDeleteTrig;
	static bool isBlockDestSetDeleteTrig;
	static bool isDelOfInitDestSetSuccess;
	static bool isDelOfRespDestSetSuccess;
	static bool isDelOfBlockDestSetSuccess;

	static bool clearAcl(string fileName);

	static bool getObjectOfRef(const string& i_objType, const string& i_objName,string& o_objRef);

	static bool changePermissions(string fileName, 
				      ACE_TCHAR *trusteeName,
                           	      ACE_UINT64 accessMask, 
				      ACE_TCHAR *accessMode,
                           	      ACE_UINT64 inheritFlag);

    	static AES_CDH_ResultCode getDest(const string destName, TransDest* &pdest);

	static AES_CDH_ResultCode defineDestSet(const string destSetName,
						const string primDestName, 
						const string secDestName,
						const string backDestName,
						const string userGroup,
						const string& destSetRdn);

	static AES_CDH_ResultCode existDestSet(const string destSetName);

	static AES_CDH_ResultCode saveDestinationSets(void );

	static AES_CDH_ResultCode deleteDestSet(const string destSetName, 
						string& detailInfo, 
						const string destName, 
						const string userName);

	static AES_CDH_ResultCode getDestSet(const string destSetName, 
					     TransDestSet* & destSet);

	static AES_CDH_ResultCode listDestSet(const string destSetName, 
					      vector<AES_CDH_DestinationSet::destSetAttributes>& attrs);

	static AES_CDH_ResultCode changeDestSet(const string& destSetName,
						const string& primaryDestName,
						const string& secondaryDestName,
						const string& backupDestName,
						const string& userName);

	static AES_CDH_ResultCode switchActiveDest(const string& destSetName,
						   const string& destName);

	static AES_CDH_ResultCode switchActiveDest(const string& destSetName);

	static const std::list<TransDest*>* getDestList();

	static bool checkOtherRespDestExists(const string& destName,
					     const string& vdDest);

	static bool checkOtherIncompatibleRespDestExists(const string& destName, 
							 const string& vdDest, 
							 const ACE_INT32 KeepTime);

	static bool checkRespDestHasKeep(TransDest* pDest);

	static bool checkCmdHandler(CmdHandler* cmdHdlr);

	static bool getDataTransferFolderPath( string& aDataTransferFolderPath );
	static bool getLastDataTransferFolderPath( string& aDataTransferFolderPath );

	static bool getActiveDestForDestSet( const string& destSetName,
					     string& activeDestName );

	static bool isFileMServiceRunning();

	bool checkFileTransferObjInIMM();
        bool checkBlockTransferObjInIMM();
	
	ACS_CC_ReturnType StartOIThreads();
	ACS_CC_ReturnType StopOIThreads();

	AES_CDH_BlockDestCmdHandler* m_poPrimBlockDestCmdHandler;
	AES_CDH_BlockDestCmdHandler* m_poSecBlockDestCmdHandler;
	AES_CDH_BlockDestCmdHandler* m_poAdvBlockDestCmdHandler ;

	AES_CDH_FileInitDestCmdHandler* m_poFilePrimaryInitDestCmdHandler;
	AES_CDH_FileInitDestCmdHandler* m_poFileSecondaryInitDestCmdHandler;
	AES_CDH_FileInitDestCmdHandler* m_poFileAdvancedInitiatingCmdHandler;

	AES_CDH_FileRespDestCmdHandler* m_poPrimRespDestCmdHandler;
	AES_CDH_FileRespDestCmdHandler* m_poSecRespDestCmdHandler;
	AES_CDH_FileRespDestCmdHandler* m_poAdvRespDestCmdHandler;

	AES_CDH_FileRespDestSetCmdHandler * m_poRespDestSetHandler;
	AES_CDH_FileInitDestSetCmdHandler * m_poInitDestSetHandler;
	AES_CDH_BlockDestSetCmdHandler * m_poBlockDestSetHandler;

	static ACE_Event *StopEvent;
	static bool isStopEventSignalled;
        static ACE_HANDLE pipeHandles[2];
	OmHandler omHandler;

	static bool isSecondaryDestExists(const string& destSetName);

	static string problemText_;

	int  StartService();
	int  StopService();

	bool initDSDServer();
	void stopDSDServer();

	bool registerDSDHandles();
	void removeDSDHandles();

	ACS_APGCC_DSD_Acceptor m_CmdServerAcceptor;
	bool m_serverOnLine;
	std::vector<ACE_HANDLE> fdList;
	bool m_DSDInitiate;
	ACE_Reactor* m_reactor;
	int m_ShutDown;
	private:
	/* This array will store all of the mutexes available to OpenSSL. */
	static ACE_Thread_Mutex *mutex_buf;
	static void locking_function(int mode, int n, const char * file, int line);
	static unsigned long id_function(void);
};

#endif
