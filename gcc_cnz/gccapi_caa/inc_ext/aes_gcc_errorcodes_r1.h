/*===================================================================== */
	/**
	@file aes_gcc_errorcodes_r1.h
	This file contains enumeration of error codes used in GOS.
	DESCRIPTION

	ERROR HANDLING

	General rule:

	@version 1.1.1

	HISTORY
	This section contains reference to problem report and related
	software correction performed inside this module

	PR           DATE      INITIALS    DESCRIPTION
	-----------------------------------------------------------
	N/A       14/07/2011    xnadnar      Initial Release
	*/
/*=================================================================== */

/*=====================================================================
                          DIRECTIVE DECLARATION SECTION
==================================================================== */
#ifndef aes_gcc_errorcodes_r1_h
#define aes_gcc_errorcodes_r1_h 1
/*=====================================================================
                        DEFINE DECLARATION SECTION
==================================================================== */
/*=================================================================== */
/**
   @brief Brief Description of PCKGD_IntfSingleDefine

          Description of define.
 */
/*=================================================================== */

typedef enum
{
			// No error
			AES_NOERRORCODE = 0,

			// Error when executing
			AES_EXECUTEERROR = 1,

			// Error in command line
			AES_INCORRECTCOMMAND = 2,

			// Timeout occurred
			AES_TIMEOUT = 3,

			// The used handle is invalid
			AES_INVALIDHANDLE = 4,

			// The used buffer or stream is full
			AES_BUFFERFULL = 5,

			// Something has went wrong with the buffer
			AES_BUFFERERROR = 6,

			// Incorrect usage of method
			AES_BUFFEREMPTY = 7,

			// Incorrect usage of method
			AES_INCUSAGE = 8,

			// Destination already used by another transfer queue
			AES_DESTINATIONINUSE = 9,

			// Process order already exist
			// Fault text: File already defined
			AES_PROCORDEREXIST = 10,

			// No such process order
			// Used by: afpchd, afpls, afprep, afprm + internal
			// Fault text: File not defined
			AES_NOPROCORDER = 11,

			// Specified file not found
			// Used by: afpdef, afpfti, afpfte + internal
			// Fault text: File not found
			AES_FILENOTFOUND = 12,

			// Invalid file name
			// Used by: afpdef, afpfti, afpfte
			// Fault text: Invalid file name
			AES_FILENAMEINVALID = 13,

			// Not a composite file
			// Used by: afpdef, afpls, afprm, afpfti, afpfte
			// Fault text: File not composite
			AES_NOTCOMPOSITE = 15,

			// Send item already exist
			// Used by: afprep
			// Fault text: Subfile already reported
			AES_SENDITEMEXIST = 17,

			// Subfile not reported
			// Used by: afprm
			// Fault text: Subfile not reported
			AES_SENDITEMNOTREP = 18,

			// Send item not found
			// Used by: afprep +  internal
			// Fault text: Subfile not found
			AES_NOSUCHITEM = 19,

			// Send item name is invalid
			// Used by: afprep
			// Fault text: Invalid subfile name
			AES_SENDITEMNAMEINVAL = 20,

			// Destination already exist
			// Used by: afpchd
			// Fault text: Destination already exist
			AES_DESTINATIONEXIST = 21,

			// No such destination
			// Used by:afpdef, afpchd, afpls, afprm, afpfti, afpfte
			// Fault text: Destination does not exist
			AES_NODESTINATION = 22,

			// Destination name is invalid
			// Used by afpdef, afpchd, afpfti, afpfte
			// Fault text: Invalid destination name
			AES_INVALIDDESTNAME = 23,

			// No access to specified file
			// Used by: all commands
			// Fault text: Access error
			AES_NOACCESS = 24,

			// No access to afp server
			// Used by: all commands
			// Fault text: Cannot connect to AES_AFP_server
			AES_NOSERVERACCESS = 117,

			// Internal program error not specified
			// Used by all commands
			// Fault text: Internal program error
			AES_CATASTROPHIC = 26,

			// Illegal remove delay value
			// Used by: afpdef, afpchd
			// Fault text: Illegal remove delay value
			AES_ILLEGALDELAYVALUE = 27,

			// Illegal retry value
			// Used by: afpfti
			// Fault text: Illegal retry value
			AES_ILLEGALRETRYVALUE = 28,

			// Wrong file type
			// Used by:
			// Fault text: Wrong file type
			AES_WRONGFILETYPE = 29,

			// Illegal time value
			// Used by: afpfti
			// Fault text: Illegal time value
			AES_ILLEGALTIMEVALUE = 30,

			// Illegal status value
			// Used by: afpdef, afpchd
			// Fault text: Illegal status value
			AES_ILLEGALSTATUSVALUE = 32,

			// Entry is locked
			// Used by: afprm
			// Fault text: File is protected by <user>
			AES_FILEISPROTECTED = 33,

			// Transfer mode is invalid
			AES_INVALIDTRANSMODE = 34,

			// Semaphor error
			AES_SEMAPHOREERR = 35,

			// Event is not handled
			AES_EVENTNOTHANDLED = 36,

			// No contact with CDH server
			AES_NOCDHSERVER = 37,

			// Connection error towards remote host
			AES_CONNECTERROR = 38,

			// Syntax fault in file template
			AES_TEMPLATEFAULT = 39,

			// Not a directory
			AES_NOTDIRECTORY = 40,

			// Application directory not found
			AES_APPLDIRNOTFOUND = 41,

			// Too many transfer queues defined to the same destination name
			// Used by: afpdef, afpfti
			// Fault text: There are already 20 transfer queues defined to the same destination
			AES_TOOMANYTQ = 42,

			// Maximum number of protected destination names exceeded
			// Used by: afpdef, afpfti
			// Fault text: There are already 100 protected destination names
			AES_EXCPROTDESTNAME = 43,

			// Too many users protecting the same protected destination name
			// Used by: afpdef, afpfti
			// Fault text: There are already 20 protecting users of a protected name
			AES_EXCPROTDESTUSER = 44,

			// Too many TQ's in GOH
			//Maximum number of transfer queues defined in GOH
			AES_MAXTQ = 45,

			// The combination transfer queue and destination set does not exist
			AES_TQDESTSETCOMBINATION = 46,

			// Data area error
			AES_DATAAREAERROR = 50,

			// Invalid transfer type for this operation
			AES_INVALIDTRANSTYPE = 51,

			// File not defined in AFP
			AES_NOAFPPROCORDER = 52,

			// No File-dest combination with state PENDING in AFP
			AES_WRONGAFPFILESTATE = 53,

			// File is open
			AES_FILEISOPEN = 54,

			// A file destination is already defined with another
			// rename type
			AES_WRONGRENAMETYPE = 55,

			// Illegal rename type
			AES_ILLEGALRENAMETYPE = 56,

			// Directory name doesn't match previous definition.
			AES_INCONSISTENTDIR = 57,

			// Illegal remove before
			AES_ILLEGALREMBEFORE = 58,

			// File template doesn't match previous definition.
			AES_WRONGTEMPLATE = 59,

			// Not allowed volume to transfer from.
			AES_NOTALLOWEDVOLUME = 60,

			// User not member of specified user group <usergroup>
			AES_USERNOTDEFINED = 61,

			// User not authorized to perform specified action
			AES_NOTAUTHORIZED = 62,

			// User group is not valid
			AES_USERGROUPINVALID = 63,

			// User group doesn't match previous definition.
			// Fault text: Different user group for the same transfer queue name
			AES_WRONGUSERGROUP = 64,

			// The destination can not be used for transfer of files or directories
			// Used by:afpdef & afpfti
			// Fault text: Destination is not configured for file transfer
			AES_NOTFILEDEST = 68,

			// The transfer queue was not found
			AES_TQNOTFOUND = 80,

			// The transfer queue name is invalid
			AES_TQNAMEINVALID = 81,

			// The transfer queue is protected
			AES_TQISPROTECTED = 82,

			// The transfer queue is open
			AES_TQISOPEN = 83,

			// Destination set already exist
			AES_DESTINATIONSETEXIST = 85,

			// Destination set does not exist
			AES_NODESTINATIONSET = 86,

			// Destination set already used by another transfer queue
			AES_DESTINATIONSETINUSE = 87,

			// uabmha: CNI 1135, added err. code for afpdef -k option
			// Illegal startup sequence number
			// Used by
			// Fault text: Illegal startup sequence number
			AES_ILLEGALSTARTUPSEQUENCENUMBER = 88,

			// Unable to change status, new status is illegal
			// Used by: commands
			// Fault text: Illegal status change
			AES_ERRORCHANGESTATUS = 100,

			// Name tag doesn't match previous definition
			AES_WRONGNAMETAG = 101,

			// uabmha: CNI 1135, the startup sequence number defined for the TQ does
			// not match the startup sequence number for a previously defined TQ
			// with the same name (defined towards another dest. set.)
			// Will give problem when -j local renaming is used.
			AES_WRONGSTARTUPSEQUENCENUMBER = 102,
			// Specified subfile not found
			// Used by
			// Fault text:
			AES_SUBFILENOTFOUND = 103,

			// Invalid name tag
			// Used by
			// Fault text: Illegal name tag
			AES_NAMETAGINVALID = 104,

			// Send item timer not experied
			// Used by: internal
			// Fault text:
			AES_TIMERNOTEXPERIED = 105,

			// File is not in FS_READY or FS_SEND
			// Used by: internal
			// Fault text:
			AES_UNABLETOSTOPFILE = 106,

			// File is not manually initiated
			// Used by: internal
			// Fault text:
			AES_NOTMANUALLY = 107,

			// Unable to lock file
			// Used by: internal.
			// Fault text:
			AES_UNABLETOLOCK = 108,

			// Unable to unlock file.
			// Used by: internal.
			// Fault text:
			AES_UNABLETOUNLOCK = 109,

			// Error when setting event handler in CDH
			AES_EVENTHANDLERSETERR = 110,

			// Error when removing event handler in CDH
			AES_EVENTHANDLERREMERR = 111,

			// The internal block buffer in GOH is full
			AES_BUFFERISFULL = 112,

			// No transaction is active
			AES_TRANSACTIONNOTACTIVE = 14, //Error code changed

			// A transaction is already active
			AES_TRANSACTIONACTIVE = 16, //Error code changed

			// No transaction has been performed
			AES_NOTRANSACTIONPERFORMED = 25, //Error code changed

			// GOH is not in synch with the remote rpc sever
			// according to the block numbers
			AES_NOTINSYNCH = 31,//Error code changed

			// The specified block size is incorrect
			AES_INCORRECTBLOCKSIZE = 47, //Error code changed

			// Block number is from the internal list in GOH
			AES_BLOCKNRFROMGOHLIST = 48, //Error code changed

			// Block number is not availiable from remote side
			// In this case, the internal block number will not be used
			AES_BLOCKNRNOTAVAILABLE = 49, //Error code changed

			//Multiple destination sets cannot be attached or detached.
			AES_MULTIDESTSET = 65,

			//Destination set of a Transfer queue cannot be replaced.
			AES_REPLACEDESTSET = 66,

			//More than three destinations sets cannot be attached to a transfer queue
			AES_MAXDESTSETLIMIT = 67,

			//Removal of association between Transfer queue and last destination set is denied
			//AES_DETACHFAILED = 72,
			// Unknown error
			// Used by: internal
			// Fault text:

			AES_ILLITQEMPTYDESTSET = 72,
			
			AES_ERRORUNKNOWN = 69, //Error code changed

			//Destination set(-) cannot be added/replaced/Deleted
			AES_IEMPTYDESTSET = 70,
			
			AES_REMPTYDESTSET = 95,
			//Illegal association of Destination set to the TransferQueue associated with Empty destination
			AES_ILLRTQEMPTYDESTSET = 71,

                        //Detach successful
                        AES_NOERRORCODE_DETACH = 75,

                        //TQ not defined in AFP
			AES_TQ_NOTFOUND = 150,
                        
			AES_BACKUP_INPROGRESS = 114,
			AES_ILLMODTQATTRS = 73,
			AES_UNREASVAL_BLOCKTQNAME = 156,
			AES_UNREASVAL_BLOCKDESTSET = 157,
			AES_UNREASVAL_BLOCKMIRROR = 158,
			AES_UNREASVAL_BLOCKREMDELAY = 159,
		//	AES_INCORRECT_USAGE = 79,


                        AES_FILE_NAME_INVALID = 151,

			AES_UNREAS_IDESTSETNAME = 76,	

			AES_NO_DESTSETNAME = 77,

			AES_NAMETAG1MANDATORY = 78,

			AES_IDESTSETALREADYATT = 79,

			AES_UNREAS_RDESTSETNAME = 84,	

			AES_ILLEGALREMIDESTSET = 89,

			AES_ILLEGALREMRDESTSET = 90,

			AES_INVALIDFILETQ = 91,

			AES_RDESTSETALREADYATT = 92,

			AES_NOIDESTINATION = 93,

			AES_NORDESTINATION = 94,

			AES_FILETQREFERRED = 96,

			AES_ILLEGALDEFSTATUS = 97,

			AES_NOINITDESTSETEXIST = 98,

			AES_NORESPDESTSETEXIST = 99,

			AES_STSTQREFERRED = 152,

                        AES_CPFTQREFERRED = 153,

                        AES_PDSTQREFERRED = 154,

                        AES_ALOGTQREFERRED = 155,

			AES_INFINITERETRYVAL = 74,
			AES_COMMITERROR = 160


} AES_GCC_Errorcodes_R1;

#endif

