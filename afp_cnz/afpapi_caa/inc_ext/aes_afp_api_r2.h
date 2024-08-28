//## begin module%1.3%.codegen_version preserve=yes
//   Read the documentation to learn more about C++ code generator
//   versioning.
//## end module%1.3%.codegen_version

//## begin module%3D04AC4201D9.cm preserve=no
//	  %X% %Q% %Z% %W%
//## end module%3D04AC4201D9.cm

//## begin module%3D04AC4201D9.cp preserve=no
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
//	 190 89-CXA 110 1179
//
//	AUTHOR
//	 2002-12-19 DAPA
//
//	REVISION
//	 A 2002-12-19 DAPA
//	 B 2003-04-28 DAPA
//
//	LINKAGE
//
//	SEE ALSO
//## end module%3D04AC4201D9.cp

//## Module: aes_afp_api_r2%3D04AC4201D9; Package specification
//## Subsystem: AFP::afpapilib_cxa::inc%387C52560099
//## Source file: Z:\ntaes\afp\afpapilib_cxa\inc\aes_afp_api_r2.h

#ifndef aes_afp_api_r2_h
#define aes_afp_api_r2_h 1

//## begin module%3D04AC4201D9.additionalIncludes preserve=no
//## end module%3D04AC4201D9.additionalIncludes

//## begin module%3D04AC4201D9.includes preserve=yes
//## end module%3D04AC4201D9.includes

#include "aes_afp_proctype.h"
#include "AES_AFP_Renametypes.h"
#include <list>
#include <string>
#include <strstream>
#include "AES_GCC_Errorcodes.h"
#include "AES_GCC_Eventcodes.h"
#include "AES_GCC_Filestates.h"
#include "AES_GCC_Format.h"

class aes_afp_apiinternal;


//## Class: aes_afp_api_r1%3D04AB2C00F3
//	Interface class for AFP API.
//	Trace points: no
//## Category: afpapilib_cxa (CXA 110 1179)%387C517E0392
//## Subsystem: AFP::afpapilib_cxa::inc%387C52560099
//## Persistence: Transient
//## Cardinality/Multiplicity: n


class aes_afp_api_r1
{
  //## begin aes_afp_api_r1%3D04AB2C00F3.initialDeclarations preserve=yes
  //## end aes_afp_api_r1%3D04AB2C00F3.initialDeclarations

  public:
    //## Constructors (generated)
      aes_afp_api_r1();

    //## Destructor (generated)
      virtual ~aes_afp_api_r1();


    //## Other Operations (specified)
      //## Operation: transferQueueDefined%3D04AB91003A; C++
      //	Check if a transfer queue is defined in the database.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_SENDITEMNAMEINVAL
      //	Indicates that the file name is either to long or has
      //	invalid characters.
      //	AES_INVALIDDESTNAME
      //	Indicates that the destination name is either to long or
      //	has invalid characters.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      //	AES_NODESTINATION
      //	Indicates that the destination is not defined for the
      //	transfer queue.
      unsigned int transferQueueDefined (std::string& transferQueue, std::string& destinationSet, bool &manual, std::string& tranferQueueDn);

      //## Operation: getTransferState%3D04AB910044
      //	Retreives the transfer state for a file.
      unsigned int getTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates& transferState);

      //## Operation: getTransferStateEx%3D04AB91004E
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
      unsigned int getTransferStateEx (std::string& filename, AES_GCC_Filestates& status, std::string& destinationSet, int listOrder, std::string& reportDate, std::string& deleteDate, std::string& manualStart, std::string& manualStop, std::string& failDate, std::string& archiveDate, int& reasonForFailed, bool& isDirectory);

      //## Operation: setTransferState%3D04AB91005B
      //	Sets the transfer state for a file.
      unsigned int setTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates transferState);

      //## Operation: listFile%3D04AB91005F; C++
      //	List files for a transfer queue.
      unsigned int listFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: listNewestFile%3D04AB91006C; C++
      //	List last reported file for a transfer queue.
      unsigned int listNewestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: listOldestFile%3D04AB910076; C++
      //	List first reported file for a transfer queue.
      unsigned int listOldestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: listAllFiles%3D04AB91007A; C++
      //	List all files for a transfer queue.
      unsigned int listAllFiles (std::string& transferQueue, std::strstream& outStream);

      //## Operation: listDestinationSets%3D04AB910080; C++
      //	List all destinationsets for a transfer queue.
      unsigned int listDestinationSets (std::string& transferQueue, std::strstream& outStream);

      //## Operation: removeTransferQueue%3D04AB91008D; C++
      //	Removes a mainfile from database.
      unsigned int removeTransferQueue (std::string& transferQueue);

      //## Operation: stopFiles%3D04AB910095; C++
      //	Stop the transfer of all files for a destinationset.
      unsigned int stopFiles (std::string& transferQueue, std::string& destinationSet);

      //## Operation: stopFile%3D04AB91009E; C++
      //	Stop the transfer of a specified file for a destination.
      unsigned int stopFile (std::string& transferQueue, std::string& destinationSet, std::string& filename);

      //## Operation: getAttributes%3D04AB9100AB; C++
      //	Retreives the attributes for a transfer queue.
      unsigned int getAttributes (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: setAttributes%3D04AB9100B2; C++
      //	Sets attributes for a transfer queue.
      unsigned int setAttributes (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState = AES_FSREADY, int removeDelay = 10, bool removeBefore = false, int nrOfRetries = 0, int retryTime = 10);

      //## Operation: addDestinationSet%3D04AB9100BC; C++
      //	Adds a new destinationset for a already defined transfer
      //	queue.
      unsigned int addDestinationSet (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState = AES_FSREADY, int removeDelay = 10, bool removeBefore = false, int nrOfRetries = 0, int retryTime = 10);

      //## Operation: removeDestinationSet%3D04AB9100C6; C++
      //	Removes a destinationset from the database.
      unsigned int removeDestinationSet (std::string& transferQueue, std::string& destinationSet);

      //## Operation: getErrorCodeText%3D04AB9100D0; C++
      //	Retreives an error code text. A errocode defined in aes_
      //	gcc_errorcodes.h returns corresponding text string
      //	Return values:
      //	AES_NOERRORCODE:
      //	""
      //	AES_EXECUTEERROR:
      //	Error when executing
      //	AES_INCORRECTCOMMAND:
      //	Usage
      //	AES_PROCORDEREXIST:
      //	Transfer queue already defined
      //	AES_NOPROCORDER:
      //	Transfer queue not defined
      //	AES_FILENOTFOUND:
      //	File or directory not found
      //	AES_FILENAMEINVALID:
      //	Invalid transfer queue name
      //	AES_NOTCOMPOSITE:
      //	File not composite
      //	AES_SENDITEMEXIST:
      //	File or directory already reported
      //	AES_SENDITEMNOTREP:
      //	File or directory not reported
      //	AES_NOSUCHITEM:
      //	File or directory not found
      //	AES_SENDITEMNAMEINVAL:
      //	Invalid file or directory name
      //	AES_DESTINATIONEXIST:
      //	Destination already exist
      //	AES_NODESTINATION:
      //	Destination does not exist
      //	AES_INVALIDDESTNAME:
      //	Invalid destination name
      //	AES_NOACCESS:
      //	Access error
      //	AES_NOSERVERACCESS:
      //	Cannot connect to AES_AFP_server
      //	AES_CATASTROPHIC:
      //	Internal program error
      //	AES_ILLEGALDELAYVALUE:
      //	Illegal remove delay value
      //	AES_ILLEGALRETRYVALUE:
      //	Illegal retry value
      //	AES_WRONGFILETYPE:
      //	Wrong file type
      //	AES_ILLEGALTIMEVALUE:
      //	Illegal retry interval value
      //	AES_ILLEGALSTATUSVALUE:
      //	Invalid status value
      //	AES_ERRORCHANGESTATUS:
      //	Illegal status change
      //	AES_SUBFILENOTFOUND:
      //	Subfile not found
      //	AES_SUBFILENAMEINVALID:
      //	Invalid subfile name
      //	AES_TIMERNOTEXPERIED:
      //	Timer not experied
      //	AES_UNABLETOSTOPFILE:
      //	Unable to end file or directory transfer
      //	AES_NOTMANUALLY:
      //	File or directory is not manually initiated
      //	AES_FILEISPROTECTED:
      //	Transfer queue is protected by
      //	AES_TEMPLATEFAULT:
      //	Template is invalid
      //	AES_NOTDIRECTORY:
      //	Invalid directory name
      //	AES_APPLDIRNOTFOUND:
      //	Directory not found
      //	AES_WRONGRENAMETYPE:
      //	Different rename types for the same transfer queue name
      //	AES_ILLEGALRENAMETYPE:
      //	Illegal rename identifier
      //	AES_INCONSISTENTDIR:
      //	Different directories for the same transfer queue name
      //	AES_ILLEGALREMBEFORE:
      //	Invalid remove before value
      //	AES_WRONGTEMPLATE:
      //	Different template for the same transfer queue name
      //	AES_ERRORUNKNOWN:
      //	Unkown error
      const char * getErrorCodeText (unsigned int returnCode);

      //## Operation: lockTransferQueue%3D04AB910152; C++
      //	Prevents a transfer queue from being removed by another
      //	user.
      unsigned int lockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application);

      //## Operation: unlockTransferQueue%3D04AB91015C; C++
      //	Unlocks a transfer queue.
      unsigned int unlockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application);

      //## Operation: isTransferQueueLocked%3D04AB910160; C++
      //	Checks if a file is locked.
      unsigned int isTransferQueueLocked (std::string& transferQueue, std::string& destinationSet, bool& locked, std::string& application);

      //## Operation: resendOneFailed%3D04AB91016A; C++
      //	Resends all files in status FS_FAILED for a specific
      //	destinationset.
      unsigned int resendOneFailed (std::string& transferQueue, std::string& destinationSet, std::string& newDestinationSet);

      //## Operation: resendAllFailed%3D04AB910170; C++
      //	Resends  all files in status FS_FAILED for all
      //	destinationsets that the transfer queue is defined to.
      unsigned int resendAllFailed (std::string& transferQueue);

      //## Operation: open%3D04AB910184; C++
      //	Opens a static connection towards the server.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      unsigned int open (std::string subsystem, std::string applicationBlock);

      //## Operation: open%3D04AB910187; C++
      //	Opens a static connection towards the server with a
      //	specific transfer queue and attaches to CDH.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      //	AES_NODESTINATION
      //	Indicates that there is no destination for the transfer
      //	queue.
      //	AES_NOCDHSERVER
      //	Indicates that there is no contact with AES_CDH_server.
      //	AES_INCORRECTCOMMAND
      //	Indicates that the command towards AES_CDH_server was
      //	incorrect.
      //	AES_ERRORUNKNOWN
      //	Indicates that an unknown error was encountered.
      unsigned int open (std::string& transferQueue);

      //## Operation: close%3D04AB91018E; C++
      //	Close the API connection towards server.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int close ();

      //## Operation: createTransferQueue%3D04AB910198; C++
      //	Creates a new transfer queue entry in the database.
      unsigned int createTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string sourceDirectory = "", AES_GCC_Filestates defaultState = AES_FSREADY, int removeDelay = 10, bool removeBefore = false, int retryTimeInterval = 10, int retryTimes = -1, AES_AFP_Renametypes renameFile = AES_NONE, std::string fileTemplate = "", std::string userGroup = "");

      //## Operation: event%3D04AB9101A3; C++
      //	Initiates an event for use by the calling application.
      virtual unsigned int event (AES_GCC_Eventcodes& eventCode);

      //## Operation: getEvent%3D04AB9101A5; C++
      //	Retreives an event from the event queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      unsigned int getEvent (AES_GCC_Eventcodes& eventCode, std::string& filename);

      //## Operation: getSourceDirectoryPath%3D04AB9101AC; C++
      //	Retreives the directory path for the transfer queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int getSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory);

      //## Operation: setSourceDirectoryPath%3D04AB9101B6; C++
      //	Sets the source directory path for a transfer queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      //	AES_NOTDIRECTORY
      //	Indicates that the name given as directory is not a
      //	valid name or is not a directory.
      //	AES_APPLDIRNOTFOUND
      //	Indicates that the directory does not exist.
      unsigned int setSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory);

      //## Operation: sendFile%3D04AB9101C0; C++
      //	Reports a file to a transfer queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_SENDITEMNAMEINVAL
      //	Indicates that the file name is either to long or has
      //	invalid characters.
      //	AES_INVALIDTRANSMODE
      //	Indicates that the selected transfer mode is invalid.
      //	AES_SENDITEMEXIST
      //	Indicates that the file is already reported to the
      //	transfer queue.
      //	AES_CATASTROPHIC
      //	Indicates that a server error was encountered and that
      //	the file was not reported to AFP.
      //	AES_FILENOTFOUND
      //	Indicates that the file was not found in the specified
      //	source directory.
      //	AES_NOTDIRECTORY
      //	Indicates that the name specified is not a directory
      //	(directory flag is true).
      unsigned int sendFile (std::string filename, AES_GCC_Format transferMode, std::string fileMask = "", bool isDirectory = false, AES_GCC_Filestates fileState = AES_FSUSEDEFAULT);

      //## Operation: lastReportedFile%3D04AB9101CA; C++
      //	Retreives the last reported file.
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int lastReportedFile (std::string transferQueue, std::string& originalFileName, std::string& generatedFileName);

      //## Operation: getDestinationSetList%3D04AB9101CE; C++
      //	Retreives a list of defined destinationsets for transfer
      //	queues.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int getDestinationSetList (std::list<std::string>& destinationSetList);

      //## Operation: getFileStatus%3D04AB9101D4; C++
      //	Retreives the status for a file. The status will be
      //	retrieved for the first found destinationset for a
      //	transfer queue.
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
      unsigned int getFileStatus (std::string filename, AES_GCC_Filestates& status);

      //## Operation: removeFile%3D04AB9101DE; C++
      //	Removes a file from afp.
      unsigned int removeFile (std::string transferQueue, std::string filename);

      //## Operation: removeFile%3D04AB9101E1; C++
      //	Removes a file from a cdh destination.
      unsigned int removeFile (std::string transferQueue, std::string destinationSet, std::string filename);

      //## Operation: sendFileManually%3D04AB9101F2; C++
      //	Sends a file manually. Demands a transfer queue name.
      unsigned int sendFileManually (std::string transferQueue, std::string userGroup, std::string filename, std::string sourceDirectory, std::string destinationSet, int retryTimes = 10, int retryTimeInterval = -1, bool isDirectory = false);

    // Additional Public Declarations
      //## begin aes_afp_api_r1%3D04AB2C00F3.public preserve=yes
      //## end aes_afp_api_r1%3D04AB2C00F3.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_api_r1%3D04AB2C00F3.protected preserve=yes
      //## end aes_afp_api_r1%3D04AB2C00F3.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_api_r1%3D04AB2C00F3.private preserve=yes
      //## end aes_afp_api_r1%3D04AB2C00F3.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: trace%3D04ABA701C2
      //	Character string for trautil.
      //## begin aes_afp_api_r1::trace%3D04ABA701C2.attr preserve=no  private: char[400] {U} 
    //  char trace_[400];  //commented out for cpp check
      //## end aes_afp_api_r1::trace%3D04ABA701C2.attr

      //## Attribute: internal%3D04ABA701FE
      //	Pointer to internal api class.
      //## begin aes_afp_api_r1::internal%3D04ABA701FE.attr preserve=no  private: aes_afp_apiinternal* {U} NULL
      aes_afp_apiinternal* internal_;
      //## end aes_afp_api_r1::internal%3D04ABA701FE.attr

    // Additional Implementation Declarations
      //## begin aes_afp_api_r1%3D04AB2C00F3.implementation preserve=yes
      //## end aes_afp_api_r1%3D04AB2C00F3.implementation

};

//## begin aes_afp_api_r1%3D04AB2C00F3.postscript preserve=yes
//## end aes_afp_api_r1%3D04AB2C00F3.postscript

//## begin aes_afp_api_r2%3EACEE03032E.preface preserve=yes
//## end aes_afp_api_r2%3EACEE03032E.preface

//## Class: aes_afp_api_r2%3EACEE03032E
//	Interface class for AFP API.
//	Trace points: no
//## Category: afpapilib_cxa (CXA 110 1179)%387C517E0392
//## Subsystem: AFP::afpapilib_cxa::inc%387C52560099
//## Persistence: Transient
//## Cardinality/Multiplicity: n


class aes_afp_api_r2
{
  //## begin aes_afp_api_r2%3EACEE03032E.initialDeclarations preserve=yes
  //## end aes_afp_api_r2%3EACEE03032E.initialDeclarations

  public:
    //## Constructors (generated)
      aes_afp_api_r2();

    //## Destructor (generated)
      virtual ~aes_afp_api_r2();


    //## Other Operations (specified)
      //## Operation: transferQueueDefined%3EACEE3C0109; C++
      //	Check if a transfer queue is defined in the database.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_SENDITEMNAMEINVAL
      //	Indicates that the file name is either to long or has
      //	invalid characters.
      //	AES_INVALIDDESTNAME
      //	Indicates that the destination name is either to long or
      //	has invalid characters.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      //	AES_NODESTINATION
      //	Indicates that the destination is not defined for the
      //	transfer queue.
      unsigned int transferQueueDefined (std::string& transferQueue, std::string& destinationSet, bool &manual);

      //## Operation: getTransferState%3EACEE3C0113
      //	Retreives the transfer state for a file.
      unsigned int getTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates& transferState);

      //## Operation: getTransferStateEx%3EACEE3C0117
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
      unsigned int getTransferStateEx (std::string& filename, AES_GCC_Filestates& status, std::string& destinationSet, int listOrder, std::string& reportDate, std::string& deleteDate, std::string& manualStart, std::string& manualStop, std::string& failDate, std::string& archiveDate, int& reasonForFailed, bool& isDirectory);

      //## Operation: setTransferState%3EACEE3C0124
      //	Sets the transfer state for a file.
      unsigned int setTransferState (std::string& filename, std::string& destinationSet, AES_GCC_Filestates transferState);

      //## Operation: listFile%3EACEE3C0128; C++
      //	List files for a transfer queue.
      unsigned int listFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: listNewestFile%3EACEE3C0131; C++
      //	List last reported file for a transfer queue.
      unsigned int listNewestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: listOldestFile%3EACEE3C0135; C++
      //	List first reported file for a transfer queue.
      unsigned int listOldestFile (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: listAllFiles%3EACEE3C013E; C++
      //	List all files for a transfer queue.
      unsigned int listAllFiles (std::string& transferQueue, std::strstream& outStream);

      //## Operation: listDestinationSets%3EACEE3C0145; C++
      //	List all destinationsets for a transfer queue.
      unsigned int listDestinationSets (std::string& transferQueue, std::strstream& outStream);

      //## Operation: removeTransferQueue%3EACEE3C0148; C++
      //	Removes a mainfile from database.
      unsigned int removeTransferQueue (std::string& transferQueue);

      //## Operation: stopFiles%3EACEE3C014F; C++
      //	Stop the transfer of all files for a destinationset.
      unsigned int stopFiles (std::string& transferQueue, std::string& destinationSet);

      //## Operation: stopFile%3EACEE3C0159; C++
      //	Stop the transfer of a specified file for a destination.
      unsigned int stopFile (std::string& transferQueue, std::string& destinationSet, std::string& filename);

      //## Operation: getAttributes%3EACEE3C015D; C++
      //	Retreives the attributes for a transfer queue.
      unsigned int getAttributes (std::string& transferQueue, std::string& destinationSet, std::strstream& outStream);

      //## Operation: setAttributes%3EACEE3C0166; C++
      //	Sets attributes for a transfer queue.
      unsigned int setAttributes (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState = AES_FSREADY, int removeDelay = 10, bool removeBefore = false, int nrOfRetries = 0, int retryTime = 10);

      //## Operation: addDestinationSet%3EACEE3C0177; C++
      //	Adds a new destinationset for a already defined transfer
      //	queue.
      unsigned int addDestinationSet (std::string& transferQueue, std::string& destinationSet, AES_GCC_Filestates defaultState = AES_FSREADY, int removeDelay = 10, bool removeBefore = false, int nrOfRetries = 0, int retryTime = 10);

      //## Operation: removeDestinationSet%3EACEE3C0181; C++
      //	Removes a destinationset from the database.
      unsigned int removeDestinationSet (std::string& transferQueue, std::string& destinationSet);

      //## Operation: getErrorCodeText%3EACEE3C018B; C++
      //	Retreives an error code text. A errocode defined in aes_
      //	gcc_errorcodes.h returns corresponding text string
      //	Return values:
      //	AES_NOERRORCODE:
      //	""
      //	AES_EXECUTEERROR:
      //	Error when executing
      //	AES_INCORRECTCOMMAND:
      //	Usage
      //	AES_PROCORDEREXIST:
      //	Transfer queue already defined
      //	AES_NOPROCORDER:
      //	Transfer queue not defined
      //	AES_FILENOTFOUND:
      //	File or directory not found
      //	AES_FILENAMEINVALID:
      //	Invalid transfer queue name
      //	AES_NOTCOMPOSITE:
      //	File not composite
      //	AES_SENDITEMEXIST:
      //	File or directory already reported
      //	AES_SENDITEMNOTREP:
      //	File or directory not reported
      //	AES_NOSUCHITEM:
      //	File or directory not found
      //	AES_SENDITEMNAMEINVAL:
      //	Invalid file or directory name
      //	AES_DESTINATIONEXIST:
      //	Destination already exist
      //	AES_NODESTINATION:
      //	Destination does not exist
      //	AES_INVALIDDESTNAME:
      //	Invalid destination name
      //	AES_NOACCESS:
      //	Access error
      //	AES_NOSERVERACCESS:
      //	Cannot connect to AES_AFP_server
      //	AES_CATASTROPHIC:
      //	Internal program error
      //	AES_ILLEGALDELAYVALUE:
      //	Illegal remove delay value
      //	AES_ILLEGALRETRYVALUE:
      //	Illegal retry value
      //	AES_WRONGFILETYPE:
      //	Wrong file type
      //	AES_ILLEGALTIMEVALUE:
      //	Illegal retry interval value
      //	AES_ILLEGALSTATUSVALUE:
      //	Invalid status value
      //	AES_ERRORCHANGESTATUS:
      //	Illegal status change
      //	AES_SUBFILENOTFOUND:
      //	Subfile not found
      //	AES_SUBFILENAMEINVALID:
      //	Invalid subfile name
      //	AES_TIMERNOTEXPERIED:
      //	Timer not experied
      //	AES_UNABLETOSTOPFILE:
      //	Unable to end file or directory transfer
      //	AES_NOTMANUALLY:
      //	File or directory is not manually initiated
      //	AES_FILEISPROTECTED:
      //	Transfer queue is protected by
      //	AES_TEMPLATEFAULT:
      //	Template is invalid
      //	AES_NOTDIRECTORY:
      //	Invalid directory name
      //	AES_APPLDIRNOTFOUND:
      //	Directory not found
      //	AES_WRONGRENAMETYPE:
      //	Different rename types for the same transfer queue name
      //	AES_ILLEGALRENAMETYPE:
      //	Illegal rename identifier
      //	AES_INCONSISTENTDIR:
      //	Different directories for the same transfer queue name
      //	AES_ILLEGALREMBEFORE:
      //	Invalid remove before value
      //	AES_WRONGTEMPLATE:
      //	Different template for the same transfer queue name
      //	AES_ERRORUNKNOWN:
      //	Unkown error
      const char * getErrorCodeText (unsigned int returnCode);

      //## Operation: lockTransferQueue%3EACEE3C01EF; C++
      //	Prevents a transfer queue from being removed by another
      //	user.
      unsigned int lockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application);

      //## Operation: unlockTransferQueue%3EACEE3C01F9; C++
      //	Unlocks a transfer queue.
      unsigned int unlockTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string& application);

      //## Operation: isTransferQueueLocked%3EACEE3C0221; C++
      //	Checks if a file is locked.
      unsigned int isTransferQueueLocked (std::string& transferQueue, std::string& destinationSet, bool& locked, std::string& application);

      //## Operation: resendOneFailed%3EACEE3C0394; C++
      //	Resends all files in status FS_FAILED for a specific
      //	destinationset.
      unsigned int resendOneFailed (std::string& transferQueue, std::string& destinationSet, std::string& newDestinationSet);

      //## Operation: resendAllFailed%3EACEE3C03A8; C++
      //	Resends  all files in status FS_FAILED for all
      //	destinationsets that the transfer queue is defined to.
      unsigned int resendAllFailed (std::string& transferQueue);

      //## Operation: open%3EACEE3C03B2; C++
      //	Opens a static connection towards the server.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      unsigned int open (std::string subsystem, std::string applicationBlock);

      //## Operation: open%3EACEE3C03BC; C++
      //	Opens a static connection towards the server with a
      //	specific transfer queue and attaches to CDH.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      //	AES_NODESTINATION
      //	Indicates that there is no destination for the transfer
      //	queue.
      //	AES_NOCDHSERVER
      //	Indicates that there is no contact with AES_CDH_server.
      //	AES_INCORRECTCOMMAND
      //	Indicates that the command towards AES_CDH_server was
      //	incorrect.
      //	AES_ERRORUNKNOWN
      //	Indicates that an unknown error was encountered.
      unsigned int open (std::string& transferQueue);

      //## Operation: close%3EACEE3C03C6; C++
      //	Close the API connection towards server.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int close ();

      //## Operation: createTransferQueue%3EACEE3C03D0; C++
      //	Creates a new transfer queue entry in the database.
      unsigned int createTransferQueue (std::string& transferQueue, std::string& destinationSet, std::string sourceDirectory = "", AES_GCC_Filestates defaultState = AES_FSREADY, int removeDelay = 10, bool removeBefore = false, int retryTimeInterval = 10, int retryTimes = -1, AES_AFP_Renametypes renameFile = AES_NONE, std::string fileTemplate = "", std::string userGroup = "", std::string nameTag = "");

      //## Operation: event%3EACEE3C03DC; C++
      //	Initiates an event for use by the calling application.
      virtual unsigned int event (AES_GCC_Eventcodes& eventCode);

      //## Operation: getEvent%3EACEE3D0006; C++
      //	Retreives an event from the event queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      unsigned int getEvent (AES_GCC_Eventcodes& eventCode, std::string& filename);

      //## Operation: getSourceDirectoryPath%3EACEE3D001A; C++
      //	Retreives the directory path for the transfer queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int getSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory);

      //## Operation: setSourceDirectoryPath%3EACEE3D0024; C++
      //	Sets the source directory path for a transfer queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_FILENAMEINVALID
      //	Indicates that the transfer queue name was invalid.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      //	AES_NOTDIRECTORY
      //	Indicates that the name given as directory is not a
      //	valid name or is not a directory.
      //	AES_APPLDIRNOTFOUND
      //	Indicates that the directory does not exist.
      unsigned int setSourceDirectoryPath (std::string& transferQueue, std::string& sourceDirectory);

      //## Operation: sendFile%3EACEE3D0027; C++
      //	Reports a file to a transfer queue.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_SENDITEMNAMEINVAL
      //	Indicates that the file name is either to long or has
      //	invalid characters.
      //	AES_INVALIDTRANSMODE
      //	Indicates that the selected transfer mode is invalid.
      //	AES_SENDITEMEXIST
      //	Indicates that the file is already reported to the
      //	transfer queue.
      //	AES_CATASTROPHIC
      //	Indicates that a server error was encountered and that
      //	the file was not reported to AFP.
      //	AES_FILENOTFOUND
      //	Indicates that the file was not found in the specified
      //	source directory.
      //	AES_NOTDIRECTORY
      //	Indicates that the name specified is not a directory
      //	(directory flag is true).
      unsigned int sendFile (std::string filename, AES_GCC_Format transferMode, std::string fileMask = "", bool isDirectory = false, AES_GCC_Filestates fileState = AES_FSUSEDEFAULT);

      //## Operation: lastReportedFile%3EACEE3D002E; C++
      //	Retreives the last reported file.
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int lastReportedFile (std::string transferQueue, std::string& originalFileName, std::string& generatedFileName);

      //## Operation: getDestinationSetList%3EACEE3D0038; C++
      //	Retreives a list of defined destinationsets for transfer
      //	queues.
      //	Return values:
      //	AES_NOERRORCODE
      //	Indicates a succesful operation.
      //	AES_NOSERVERACCESS
      //	Indicates that there is no contact with AES_AFP_server.
      //	AES_NOPROCORDER
      //	Indicates that the transfer queue is not defined.
      unsigned int getDestinationSetList (std::list<std::string>& destinationSetList);

      //## Operation: getFileStatus%3EACEE3D0042; C++
      //	Retreives the status for a file. The status will be
      //	retrieved for the first found destinationset for a
      //	transfer queue.
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
      unsigned int getFileStatus (std::string filename, AES_GCC_Filestates& status);

      //## Operation: removeFile%3EACEE3D004C; C++
      //	Removes a file from afp.
      unsigned int removeFile (std::string transferQueue, std::string filename);

      //## Operation: removeFile%3EACEE3D0056; C++
      //	Removes a file from a cdh destination.
      unsigned int removeFile (std::string transferQueue, std::string destinationSet, std::string filename);

      //## Operation: sendFileManually%3EACEE3D005A; C++
      //	Sends a file manually. Demands a transfer queue name.
      unsigned int sendFileManually (std::string transferQueue, std::string userGroup, std::string filename, std::string sourceDirectory, std::string destinationSet, int retryTimes = 10, int retryTimeInterval = -1, bool isDirectory = false);

    // Additional Public Declarations
      //## begin aes_afp_api_r2%3EACEE03032E.public preserve=yes
      //## end aes_afp_api_r2%3EACEE03032E.public

  protected:
    // Additional Protected Declarations
      //## begin aes_afp_api_r2%3EACEE03032E.protected preserve=yes
      //## end aes_afp_api_r2%3EACEE03032E.protected

  private:
    // Additional Private Declarations
      //## begin aes_afp_api_r2%3EACEE03032E.private preserve=yes
      //## end aes_afp_api_r2%3EACEE03032E.private

  private: //## implementation
    // Data Members for Class Attributes

      //## Attribute: internal%3EACEE4F0215
      //	Pointer to internal api class.
      //## begin aes_afp_api_r2::internal%3EACEE4F0215.attr preserve=no  private: aes_afp_apiinternal* {U} NULL
      aes_afp_apiinternal* internal_;
      //## end aes_afp_api_r2::internal%3EACEE4F0215.attr

    // Additional Implementation Declarations
      //## begin aes_afp_api_r2%3EACEE03032E.implementation preserve=yes
      //## end aes_afp_api_r2%3EACEE03032E.implementation

};

//## begin aes_afp_api_r2%3EACEE03032E.postscript preserve=yes
//## end aes_afp_api_r2%3EACEE03032E.postscript

// Class aes_afp_api_r1 

// Class aes_afp_api_r2 

//## begin module%3D04AC4201D9.epilog preserve=yes
//## end module%3D04AC4201D9.epilog


#endif
