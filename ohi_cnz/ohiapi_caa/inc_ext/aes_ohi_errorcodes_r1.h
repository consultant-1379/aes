/*=================================================================== */
/**
   @file   AES_OHI_ErrorCodes_R1.h

   @brief Header file for Error codes in OHI module.

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
   N/A       21/06/2011   XCHEMAD     Initial Release
==================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#if !defined(_AES_OHI_ERRORCODES_R1_H_)
#define _AES_OHI_ERRORCODES_R1_H_

/*=====================================================================
                        ENUMERATED DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief  aes_ohi_errorcodes_r1

           This Enum holds the error codes of the OHI module.
*/
/*=================================================================== */
typedef enum
{
   AES_OHI_NOERRORCODE = 0,			/*!< No error */

   AES_OHI_EXECUTEERROR = 1,			/*< Error when executing */

   AES_OHI_INCORRECTCOMMAND = 2,		/*< Error in command line */

   AES_OHI_BUFFERFULL = 5,			/*< The internal block buffer in GOH is full */

   AES_OHI_BUFFERERROR = 6,			/*< The internal block buffer in GOH is full */

   AES_OHI_PROCORDEREXIST = 10,			/*< Process order already exist */

   AES_OHI_NOPROCORDER = 11,			/*< No such process order */

   AES_OHI_FILENOTFOUND = 12,			/*< Specified file not found */

   AES_OHI_FILENAMEINVALID = 13,		/*< Invalid file name */

   AES_OHI_NOTCOMPOSITE = 15,			/*< Not a composite file */

   AES_OHI_SENDITEMEXIST = 17,			/*< Send item already exist */

   AES_OHI_SENDITEMNOTREP = 18,			/*< Subfile not reported */

   AES_OHI_NOSUCHITEM = 19,			/*< Send item not found */

   AES_OHI_SENDITEMNAMEINVAL = 20,		/*< Send item name is invalid */

   AES_OHI_DESTINATIONEXIST = 21,		/*< Destination already exist */

   AES_OHI_NODESTINATION = 22,			/*< No such destination */

   AES_OHI_INVALIDDESTNAME = 23,		/*< Destination name is invalid */

   AES_OHI_NOACCESS = 24,			/*< No access to specified file */

   AES_OHI_NOSERVERACCESS = 117,		/*< No access to afp server */

   AES_OHI_CATASTROPHIC = 26,			/*< Internal program error not specified */

   AES_OHI_ILLEGALDELAYVALUE = 27,		/*< Illegal remove delay value */

   AES_OHI_ILLEGALRETRYVALUE = 28,		/*< Illegal retry value */

   AES_OHI_WRONGFILETYPE = 29,			/*< Wrong file type */

   AES_OHI_ILLEGALTIMEVALUE = 30,		/*< Illegal time value */

   AES_OHI_ILLEGALSTATUSVALUE = 32,		/*< Illegal status value */

   AES_OHI_FILEISPROTECTED = 33,		/*< Entry is locked */

   AES_OHI_INVALIDTRANSMODE = 34,		/*< Transfer mode is invalid */

   AES_OHI_SEMAPHOREERR = 35,			/*< Semaphore error */

   AES_OHI_EVENTNOTHANDLED = 36,		/*< Event is not handled */

   AES_OHI_NOCDHSERVER = 37,			/*< No contact with CDH server */

   AES_OHI_CONNECTERROR = 38,			/*< Connection error towards remote host */

   AES_OHI_TEMPLATEFAULT = 39,			/*< Syntax fault in file template */

   AES_OHI_NOTDIRECTORY = 40,			/*< Not a directory */

   AES_OHI_APPLDIRNOTFOUND = 41,		/*< Application directory not found */

   AES_OHI_ERRORCHANGESTATUS = 100,		/*< Unable to change status */

   AES_OHI_SUBFILENOTFOUND = 103,		/*< Specified subfile not found */

   AES_OHI_SUBFILENAMEINVALID = 104,		/*< Invalid subfile name */

   AES_OHI_TIMERNOTEXPERIED = 105,		/*< Send item timer not expired */

   AES_OHI_UNABLETOSTOPFILE = 106,		/*< File is not in FS_READY or FS_SEND */

   AES_OHI_NOTMANUALLY = 107,			/*< File is not manually initiated */

   AES_OHI_UNABLETOLOCK = 108,			/*< Unable to lock file */

   AES_OHI_UNABLETOUNLOCK = 109,		/*< Unable to unlock file */

   AES_OHI_TRANSACTIONNOTACTIVE = 14,		/*< No transaction is active */

   AES_OHI_TRANSACTIONACTIVE = 16,		/*< TO BE REVISITED*/

   AES_OHI_NOTRANSACTIONPERFORMED = 25,	/*< No transaction has been performed */

   AES_OHI_NOTINSYNCH = 31,			/*< GOH is not in synch with the remote rpc sever according to the block numbers */

   AES_OHI_INCORRECTBLOCKSIZE = 47,		/*< The specified block size is incorrect */

   AES_OHI_INVALIDBLOCKNR = 51,		/*< The specified block number was out of range */

   AES_OHI_BLOCKNRFROMGOHLIST = 48,		/*< Block number is from the internal list in GOH */

   AES_OHI_BLOCKNRNOTAVAILABLE = 49,		/*< Block number is not availiable from remote side, In this case, the internal block number will not be used */

   AES_OHI_COMMITNOTPERFORMED = 50,		/*< No transactionCommit or getLastCommitedBlockNo has been performed since last used transactionBegin */

   AES_OHI_TQNOTFOUND = 150,			/*< The specified transfer queue is not found */

   AES_OHI_TQNAMEINVALID = 151,			/*< The transfer queue name is invalid */

   AES_OHI_TQISPROTECTED = 152,			/*< The transfer queue is protected */

   AES_OHI_TQISOPEN = 153,			/*< The transfer queue is open */
   AES_OHI_BACKUP_INPROGRESS = 114,                 /*<BACKUP RESTORE IS IN PROGRESS  */

   AES_OHI_ERRORUNKNOWN = 255			/*< Unknown error */

} aes_ohi_errorcodes_r1;

#endif
