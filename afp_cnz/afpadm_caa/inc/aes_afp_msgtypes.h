
#ifndef aes_afp_msgtypes_h
#define aes_afp_msgtypes_h 1


typedef enum
   {
           // End message
           HANGUP =  0, 

           // Create a transfer queue.
           CREATETRANSFERQUEUE = 1,

           // Remove a transfer queue.
           REMOVETRANSFERQUEUE = 2,

           // Open a transfer queue
           OPENTRANSFERQUEUE = 3,

           // Search for existens of a transfer queue.
           FINDTRANSFERQUEUE = 4,

           // Retreives all information about one transfer queue.
           INFOONETRANSFERQUEUE = 5,

           // Retreives all information about all transfer queues.
           INFOALLTRANSFERQUEUES = 6,
	   INFOALLTRANSFERQUEUESSHORT = 80,	

           // Get attributes for a transfer queue.
           GETTRANSFERQUEUEATTRIBUTES = 7,

           // Set attributes for a transfer queue.
           SETTRANSFERQUEUEATTRIBUTES = 8,

           // Don't move, CP dependency, removes a file.
           REMOVEFILE = 9,

           // Don't move, CP dependency, creates a file.
           CREATEFILE = 15,

           // Don't move, CP dependency, sets a file to status = archive.
           SETFILEARCHIVE = 17,

           // Don't move, CP dependency, gets status about a file.
           GETSTATUSFILE = 19,

           // Get status about oldest file.
           GETSTATUSOLDFILE = 21,

           // Get status about newest file.
           GETSTATUSNEWFILE = 22,

           // Sets status on a file.
           SETSTATUSFILE = 23,

           // Retreives information about newest file for a transfer queue.
           FINDNEWESTFILE = 24,

           // Retreives information about oldest file for a transfer queue.
           FINDOLDESTFILE = 25,

           // Indicates if an transfer queue is defined.
           TRANSFERQUEUEDEFINED = 26,

           // Indicates if an transfer queue is defined to a destinationset.
           TRANSFERQUEUEDESTINATIONSETDEFINED = 27,

           // Lock a transfer queue.
           LOCKTRANSFERQUEUE = 28,

           // Unlock a transfer queue.
           UNLOCKTRANSFERQUEUE = 29,

           // Creates a destinationset.
           ADDDESTINATIONSET = 30,

           // Removes a destinationset.
           REMOVEDESTINATIONSET = 31,

           // Retreives all destinationsets for a transfer queue.
           INFODESTINATIONSET = 32,

           // Checks if a transfer queue is locked.
           ISTRANSFERQUEUELOCKED = 33,

           // Send an OHI file
           SENDOHIFILE = 34,

           // Set application source directory path
           SETSOURCEDIRECTORYPATH = 35,

           // Get application source directory path
           GETSOURCEDIRECTORYPATH = 36, 

           // Get status for next file in a destinationset.
           GETSTATUSNEXTFILE = 37,

           // CDH has queued the send request.
           CDHQUEUED = 40,

           // CDH is sending.
           CDHSENDING = 41,

           // CDH has finished send request.
           CDHFINISHED = 42,

           // Error in CDH.
           CDHSENDERROR = 43,

           // Request to CDH to send a file.
           CDHSENDFILE = 44,

           // Stop transfering of one file.
           CDHSTOPSENDFILE = 45,

           // Stop sending all files.
           CDHSTOPSENDALL = 46,

           // Fatal error in CDH
           CDHFATALERROR = 47,

           // Set status for a file.
          CDHSETSTATUSFILE = 48,

           // CDH is stopped
          CDHSTOPPED = 49,

          // Resend file to CDH
          CDHSENDRETRY = 50,

          // Connection error in CDH
          CDHCONNECTERROR = 51,

          // CDH connected to remote host
          CDHCONNECTED = 52,

          //Resend files from a specified destinationset
          RESENDONEFILE=60,

          // Resend all files for a transfer queue.
          RESENDALLFILE=61,

           // Open generic connection
          OPENGENERIC = 63,

           // Retreives the last reported file
           GETLASTREPORTEDFILE = 64,

            // Retreives a list of defined destinationsets
            GETDESTINATIONSETLIST = 65,
           
           // Retreives the status for a file
           GETFILESTATUS = 66,

           // Remove a file from all destinationsets in a transfer queue.
           REMOVEFILEALLDESTINATIONSETS = 67,

           // Remove a file from a specific destinationset in a transfer queue.
            REMOVEEXACTFILE = 68,

           // Close a transfer queue
           CLOSETRANSFERQUEUE = 69,

            // Send file manually
            SENDFILEMANUAL = 70,

            // Event handler set in CDH
            CDHEVENTHANDLERSET = 71,

            // Error when setting event handler in CDH
             CDHEVENTHANDLERSETERR = 72,

            // Event handler removed in CDH
            CDHEVENTHANDLERREM = 73,

            // Error removing event handler in CDH
            CDHEVENTHANDLERREMERR = 74,

           // Reopen CDH destinationset
            CDHREOPEN = 75,

           // Re attach destinationset
            CDHREATTACH = 76,

			// Remove Source Directory
			REMOVESOURCEDIRECTORY = 77,
           
           //Remove File From Source Directory
            REMOVEFILESOURCEDIRECTORY = 78,		//for APZ21230/5-758

			CDHRESEND = 79,

           
           // Request handled without error.
           OK = 100,

           // Error when handling request.
           NOTOK = 101,

           // Message is a command type
           COMMAND = 200,

           // Message is a event type
           EVENT = 201,

           // An unknown message received.
           UNKNOWN = 255
   } aes_afp_msgtypes;


#endif
