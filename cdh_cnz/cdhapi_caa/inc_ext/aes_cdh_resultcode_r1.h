/*=================================================================== */
/**
   @file  aes_cdh_result_r1.h

   @brief Header file for CDH Result Code type module.

          The services provided by AES_CDH_ResultCode_R1 facilitates the
          transfer of data to remote systems.

   @version 1.0.0
*/
/*
   HISTORY
   This section contains reference to problem report and related
   software correction performed inside this module


   PR           DATE      INITIALS    DESCRIPTION
   -----------------------------------------------------------
   N/A       26/09/2011    XCHEMAD    Initial Release
==================================================================== */
/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef AES_CDH_RESULTCODE_R1_H 
#define AES_CDH_RESULTCODE_R1_H

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  AES_CDH_ResultCode_R1

           Result codes
*/
/*=================================================================== */
typedef enum
{
   AES_CDH_RC_OK		      = 0, /*!< The command was executed successfully */
   AES_CDH_RC_ERROR                   = 1, /*!< Error when executing */
   AES_CDH_RC_INCUSAGE                = 2, /*!< Incorrect usage of command */
   AES_CDH_RC_UNKNOWNRC               = 3, /*!< Unknown fault code */
   AES_CDH_RC_ILLEGALREMOTEFOLDERPATH = 14, /*!< Unreasonable value for remoteFolderPath */
   AES_CDH_RC_DESTDEF                 = 10, /*!< Destination already defined */
   AES_CDH_RC_DESTSETDEF              = 11, /*!< Destination set already defined */
   AES_CDH_RC_DESTNOTINDSET           = 12, /*!< Destination is not in destination set */
   AES_CDH_RC_UNREAS                  = 16, /*!< Unreasonable value */
   AES_CDH_RC_DESTISACTORPRIM         = 19, /*!< The destination is the active or primary destination */
   AES_CDH_RC_PHYSFILEERR             = 20, /*!< Physical file error */
   AES_CDH_RC_NODESTSET               = 21, /*!< Destination set does not exist */
   AES_CDH_RC_NODEST                  = 22, /*!< Destination does not exist */
   AES_CDH_RC_PROTECTEDDEST           = 23, /*!< Destination is protected */
   AES_CDH_RC_PROTECTEDDESTSET        = 24, /*!< Destination set is protected by a transfer queue */
   AES_CDH_RC_DESTUSEDINSET           = 25, /*!< Destination is already used in a destination set */
   AES_CDH_RC_INTPROGERR              = 26, /*!< Internal program error */
   AES_CDH_RC_ILLEGALCOMB             = 27, /*< Illegal combinations of transfer types in a destination set */
   AES_CDH_RC_NOTRANS                 = 28, /*!< Transfer type does not exist */
   AES_CDH_RC_NOSERVER                = 29, /*!< No contact with AES_CDH_Server */
   AES_CDH_RC_CMDAUTHERR              = 30, /*!< User not authorized to execute this command */
   AES_CDH_RC_USRGNOTMEMBER           = 31, /*!< User is not member of the specified user group */
   AES_CDH_RC_FILEERR                 = 50, /*!< Error reading file */
   AES_CDH_RC_FILENAMETOOLONG         = 51, /*!< Remote file name too long */
   AES_CDH_RC_CONNECTERR              = 52, /*!< Error connecting to remote system */
   AES_CDH_RC_SENDERR                 = 53, /*!< Error sending to remote system */
   AES_CDH_RC_ENDOFSTREAM             = 54, /*!< End of stream */
   AES_CDH_RC_CONNECTWARN             = 55, /*!< Error connecting to remote system, no data lost */
   AES_CDH_RC_SENDWARN                = 56, /*!< Error sending to remote system, no data lost */
   AES_CDH_RC_DATAAREAERR             = 57, /*!< Data area error */
   AES_CDH_RC_TASKSTOPPED             = 58, /*!< Task stopped */
   AES_CDH_RC_NOTASK                  = 59, /*!< Task does not exist */
   AES_CDH_RC_TASKEXISTS              = 60, /*!< Task already exists */
   AES_CDH_RC_FILEALREADYSENT         = 61, /*!< File is alredy sent */
   AES_CDH_RC_EVENTHANDLERSET         = 62, /*!< Event Handler is set */
   AES_CDH_RC_EVENTHANDLERSETERR      = 63, /*!< Error when setting Event Handler */
   AES_CDH_RC_EVENTHANDLERREM         = 64, /*!< Event Handler is removed */
   AES_CDH_RC_EVENTHANDLERREMERR      = 65, /*!< Error when removing Event Handler */
   AES_CDH_RC_CONNECTOK        	      = 66, /*!< Ok connecting to remote system */
   AES_CDH_RC_LOGONFAILURE            = 67, /*!< logon failure on remote system */
   AES_CDH_RC_NOTFILEDEST             = 68, /*!< The destination exists but may not be used for file transfer */
   AES_CDH_RC_NOTBLOCKDEST     	      = 69, /*!< The destination exists but may not be used for block transfer */
   AES_CDH_RC_TRANSACTIONNOTACTIVE    = 70, /*!< No transaction is active */
   AES_CDH_RC_TRANSACTIONACTIVE       = 71, /*!< A transaction is already active */
   AES_CDH_RC_NOTRANSACTIONPERFORMED  = 72, /*!< No transaction has been performed */
   AES_CDH_RC_NOTINSYNCH              = 73, /*!< Not in synch with the rpc server according to the block numbers */
   AES_CDH_RC_INVALIDTRANSACTION      = 74, /*!< Invalid transaction */
   AES_CDH_RC_STREAMERROR             = 75, /*!< Stream error */
   AES_CDH_RC_NOBLOCKNOAVAILABLE      = 76, /*!< No block number is available */
   AES_CDH_RC_NOTNOTIFDEST            = 77, /*!< The destination is not defined with file notification */
   AES_CDH_RC_NOTFILEDESTSET	      = 78, /*!< The destination set exists but may not be used for file transfer */
   AES_CDH_RC_NOTBLOCKDESTSET         = 79, /*!< The destination set exists but may not be used for block transfer */
   AES_CDH_RC_SWITCH                  = 80, /*!< There has been a switch to different destination in the destination set */
   AES_CDH_RC_NOTRESPDEST             = 81, /*!< The destination is not of type responding */
   AES_CDH_RC_NOALIAS                 = 82, /*!< The alias for virtual directory does not exist */
   AES_CDH_RC_NOVIRTUALPATH           = 83, /*!< The virtual path does not exist */
   AES_CDH_RC_TIMEOUT                 = 84, /*!< Timeout received in command cdhver to file notif dest */
   AES_CDH_RC_SKIPPED                 = 85, /*!< Skipped */
   AES_CDH_RC_DESTWITHSAMEOPTEXIST    = 86, /*!< An destination with the same destination-unique options already exist */
   AES_CDH_RC_KEEPOPTNOTDEFINED       = 87, /*!< Option not defined for destination */
   AES_CDH_RC_DESTSETDOWN             = 88, /*!< Destination set down */
   AES_CDH_RC_DESTSETDOWNCEASE        = 89, /*!< Destination set up */
   AES_CDH_RC_SECDESTEXIST			  = 90,	/*!< Secondary Destination already exists */
   AES_CDH_RC_NOPRIDEST				  = 91,	/*!< Primary Destination does not exist*/
   AES_CDH_FILEDESTSETLIMIT               = 92,  /*! <Maximum Destinations sets should not be more than 70 */
   AES_CDH_BLOCKDESTSETLIMIT			  = 93,	/*!< Maximum Destinations sets should not be more than 30 */
   AES_CDH_RC_CDHSTOPPING 			   = 94, /*! <CDH is in stopping state */
   AES_CDH_RC_UNREAS_BLOCKIP          =95,   /*!< Unreasonable value for Block IP Address*/
   AES_CDH_RC_UNREAS_BLOCKRETRYDELAY  =96,   /*!< Unreasonable value for Block retry delay */
   AES_CDH_RC_UNREAS_BLOCKRETRIES     =97,   /*!< Unreasonable value for Block number of retries */
   AES_CDH_RC_UNREAS_PRIBLOCKDESTNAME    =98,   /*!< Unreasonable value for primary block destination name */
   AES_CDH_RC_UNREAS_SECBLOCKDESTNAME    =99,   /*!< Unreasonable value for secondary block destination name */
   AES_CDH_RC_UNREAS_ADVBLOCKDESTNAME    =100,   /*!< Unreasonable value for advaned block destination name */
   AES_CDH_RC_UNREAS_BLOCKDESTSETNAME    =101,   /*!< Unreasonable value for block destination set name */
   AES_CDH_RC_UNREAS_INITIP	         =102,   /*!< Unreasonable value for initiating IP address */
   AES_CDH_RC_UNREAS_INITPORTNO          =103, 	 /*!< Unreasonable value for initiating port no */
   AES_CDH_RC_UNREAS_FILETRANSFERTYPE    =104,   /*!< Unreasonable value for File transfer type */
   AES_CDH_RC_UNREAS_PRIINITDESTNAME     =105,   /*!< Unreasonable value for primary initiating destination name */
   AES_CDH_RC_UNREAS_SECINITDESTNAME     =106,   /*!< Unreasonable value for secondary initiating destination name */
   AES_CDH_RC_UNREAS_PRIRESPDESTNAME     =107,   /*!< Unreasonable value for primary responding destination name */
   AES_CDH_RC_UNREAS_SECRESPDESTNAME     =108,   /*!< Unreasonable value for secondary responding destination name */
   AES_CDH_RC_UNREAS_FILERETRYDELAY      =109,   /*!< Unreasonable value for File retry delay */
   AES_CDH_RC_UNREAS_FILESENDRETRY       =110,   /*!< Unreasonable value for File send retry */
   AES_CDH_RC_UNREAS_INITDESTSETNAME     =111,   /*!< Unreasonable value for initiating destination set name  */
   AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV4MAPPEDIPV6	 =112,	 /*!< Unreasonable value for ipAddress attribute of Initiating File destination & Block destination */
   AES_CDH_RC_UNREAS_RESPDEST_IPV4MAPPEDIPV6	 =113,	 /*!< Unreasonable value for ipNotificationAddress attribute of Responding destination */
   AES_CDH_RC_UNREAS_INITDEST_BLKDEST_IPV6_NATIVE	 =115,	 /*!< Unreasonable value for ipAddress attribute of Responding destination  in NATIVE */
   AES_CDH_RC_UNREAS_RESPDEST_IPV6_NATIVE	 =116,	 /*!< Unreasonable value for ipNotificationAddress attribute of Responding destination in NATIVE*/
   AES_CDH_RC_UNREAS_RESPDESTSETNAME     =32,   /*!< Unreasonable value for responding destination set name */
   AES_CDH_RC_UNREAS_RESPIP              =33,   /*!< Unreasonable value for  responding IP address */
   AES_CDH_RC_UNREAS_RESPPORTNO          =34,   /*!< Unreasonable value for responding port no */
   AES_CDH_RC_UNREAS_FILEOVERWRITE       =35,    /*!< Unreasonable value for file overwrite option */
   AES_CDH_RC_UNREAS_SUPPRESSSUBFOLDRCREATION =36,  /*!< Unreasonable value for suppress sub folder creation option */
   AES_CDH_RC_UNREAS_INHIBITTQNAMEPRFX   =37,     /*!< Unreasonable value for inhibit transfer queue name prefix option */
   AES_CDH_RC_SFTP_PASSWD = 38,		/* Password is mandatory for SFTP transfer  */
   AES_CDH_RC_SFTP_UNAME = 39,		/* Remote username is mandatory for SFTP transfer  */
   AES_CDH_RC_FTP_UNAME = 40, /* Remote username is mandatory for FTP transfer if password specified  */ 
   AES_CDH_RC_MODIFY_TRANSFERTYPE = 41, /* Incorrect Usage. Modification of Transfer Type is not allowed */
   AES_CDH_RC_NOADVPARAM		  = 42,	/*!< Advanced Destination parameters object does not exist*/
   AES_CDH_RC_BACKUP_INPROGRESS	   = 114,  
   AES_CDH_RC_ROUTEERR             =43, /*!< Route Traffic Failed : No destination is defined in this destination set*/ 
   AES_CDH_RC_REMOVE_IPNOTIF       =44, /*!< Incorrect Operation. Once set should not remove ipNotificationAddress*/
   AES_CDH_RC_REMOVE_PORT          =45, /*!< Incorrect Operation. Once set should not remove portNotificationNumber*/
   AES_CDH_RC_COMMITERROR 	   =46 /*!< Creation of multiple destinationsets of same type  is not allowed in a single transaction commit. */
} AES_CDH_ResultCode_R1;

#endif // AES_CDH_RESULTCODE_R1_H
