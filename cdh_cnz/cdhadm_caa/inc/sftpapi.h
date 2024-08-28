
//******************************************************************************
// 
// .NAME
//  	-
// .LIBRARY 3C++
// .PAGENAME -
// .HEADER  AES  
// .LEFT_FOOTER Ericsson Utvecklings AB
// .INCLUDE sftpapi.h

// .COPYRIGHT
//  COPYRIGHT Ericsson Utvecklings AB, Sweden 2003.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Ericsson Utvecklings AB, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Ericsson Utvecklings AB or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION 
// 	"C-Wrapper" interface to the SftpApi class.
//  NOTE! This interface can only handle one SftpApi instance at a time!

// .ERROR HANDLING
//
//	General rule:
//	The error handling is specified for each method.
//
//	No methods initiate or send error reports unless specified.

// DOCUMENT NO
//	190 89-CAA 109 0507

// AUTHOR 
// 	2003-1024 by EAB/UKY/GA Gunnar Andersson

// .LINKAGE
//	-

// .SEE ALSO 
// 	SftpApi, SSHFTPIV2

//******************************************************************************

#ifndef SFTPAPI_H 
#define SFTPAPI_H

#include "sftpdata.hpp"

#ifdef WIN32

#ifdef SFTPAPIDLLBUILD
#define SFTPAPILIB extern "C" __declspec (dllexport)
#else
#ifdef SFTPAPIDLLSTATIC
#define SFTPAPILIB extern "C" 
#else
#define SFTPAPILIB extern "C" __declspec (dllimport)
#endif
#endif

#else // WIN32

#define SFTPAPILIB

#endif // WIN32

  /* Return status codes */
#define SFTPAPI_API_SUCCESS 1
#define SFTPAPI_API_FAILURE 2

  /* Error codes */
#define SFTPAPI_OK 100
#define SFTPAPI_AUTH_FAILED 101
#define SFTPAPI_NOT_CONNECTED 102
#define SFTPAPI_CONNECTED 103
#define SFTPAPI_CONNECTION_LOST 104
#define SFTPAPI_FILE_ALREADY_EXISTS 105
#define SFTPAPI_INVALID_HANDLE 106
#define SFTPAPI_DIRECTORY_ALREADY_EXISTS 107
#define SFTPAPI_PERMISSION_DENIED 108
#define SFTPAPI_NO_SUCH_DIRECTORY 109
#define SFTPAPI_FAILURE 110
#define SFTPAPI_NOT_YET_IMPLEMENTED 111
#define SFTPAPI_INVALID_PORT 112
#define SFTPAPI_INVALID_HOST 113
#define SFTPAPI_NO_SUCH_FILE 114

  /* Max buffer size for read/write */
#define SFTPAPI_MAX_BUFFER_SIZE 200

  /* Open file modes */
#define SFTPAPI_OPEN_READ 300
#define SFTPAPI_OPEN_WRITE 301
#define SFTPAPI_OPEN_APPEND 302
#define SFTPAPI_OPEN_CREAT 303
#define SFTPAPI_OPEN_TRUNC 304
#define SFTPAPI_OPEN_EXCL 305

  /* Open file transfer modes */
#define SFTPAPI_BINARY 400
#define SFTPAPI_ASCII 401
#define SFTPAPI_SERVERTYPE_UNIX 402
#define SFTPAPI_SERVERTYPE_WINDOWS 403

  /* File permission codes */
#define SFTPAPI_S_IXUSR 500
#define SFTPAPI_S_IXGRP 501
#define SFTPAPI_S_IXOTH 502
#define SFTPAPI_S_IWUSR 503
#define SFTPAPI_S_IWGRP 504
#define SFTPAPI_S_IWOTH 505
#define SFTPAPI_S_IRUSR 506
#define SFTPAPI_S_IRGRP 507
#define SFTPAPI_S_IROTH 508
#define SFTPAPI_S_IRWXU 509
#define SFTPAPI_S_IRWXG 510
#define SFTPAPI_S_IRWXO 511

  /* Attribute flags */
#define SFTPAPI_ATTR_SIZE 600
#define SFTPAPI_ATTR_UIDGID 601
#define SFTPAPI_ATTR_PERMISSIONS 602
#define SFTPAPI_ATTR_ACMODTIME 603

//*********************************************************************
//      SftpApi_construct()
//*********************************************************************
typedef void (*SFTPAPI_CONSTRUCT)(void); 
SFTPAPILIB void SftpApi_construct(void);

//*********************************************************************
//      SftpApi_destruct()
//*********************************************************************
typedef void (*SFTPAPI_DESTRUCT)(void); 
SFTPAPILIB void SftpApi_destruct(void);

//*********************************************************************
//      SftpApi_connectToServer()
//*********************************************************************
typedef int (*SFTPAPI_CONNECTTOSERVER)(const SftpConnectionData* connData); 
SFTPAPILIB int SftpApi_connectToServer(const SftpConnectionData* connData);

//*********************************************************************
//      SftpApi_disconnect()
//*********************************************************************
typedef int (*SFTPAPI_DISCONNECT)(void); 
SFTPAPILIB int SftpApi_disconnect(void);

//*********************************************************************
//      SftpApi_renameFile()
//*********************************************************************
typedef int (*SFTPAPI_RENAMEFILE)(const char* remoteOldName,const char* remoteNewName); 
SFTPAPILIB int SftpApi_renameFile(const char* remoteOldName,const char* remoteNewName);

//*********************************************************************
//      SftpApi_removeFile()
//*********************************************************************
typedef int (*SFTPAPI_REMOVEFILE)(const char* remoteName); 
SFTPAPILIB int SftpApi_removeFile(const char* remoteName);

//*********************************************************************
//      SftpApi_removeDirectory()
//*********************************************************************
typedef int (*SFTPAPI_REMOVEDIRECTORY)(const char* remoteDirName); 
SFTPAPILIB int SftpApi_removeDirectory(const char* remoteDirName);

//*********************************************************************
//      SftpApi_openFile()
//*********************************************************************
typedef int (*SFTPAPI_OPENFILE)(const char* remoteFileName,unsigned int mode,
                            unsigned int transferMode,unsigned int perm,
                            SftpFileHandle* handle); 
SFTPAPILIB int SftpApi_openFile(const char* remoteFileName,unsigned int mode,
                            unsigned int transferMode,unsigned int perm,
                            SftpFileHandle* handle);

//*********************************************************************
//      SftpApi_writeFile()
//*********************************************************************
typedef int (*SFTPAPI_WRITEFILE)(SftpFileHandle* handle,const char* buf,unsigned __int64 offset); 
SFTPAPILIB int SftpApi_writeFile(SftpFileHandle* handle,const char* buf,unsigned __int64 offset);

//*********************************************************************
//      SftpApi_closeFile()
//*********************************************************************
typedef int (*SFTPAPI_CLOSEFILE)(SftpFileHandle* handle); 
SFTPAPILIB int SftpApi_closeFile(SftpFileHandle* handle);

//*********************************************************************
//      SftpApi_makeDirectory()
//*********************************************************************
typedef int (*SFTPAPI_MAKEDIRECTORY)(const char* dirPathName); 
SFTPAPILIB int SftpApi_makeDirectory(const char* dirPathName);

//*********************************************************************
//      SftpApi_renameDirectory()
//*********************************************************************
typedef int (*SFTPAPI_RENAMEDIRECTORY)(const char* dirOldPathName,const char* dirNewPathName); 
SFTPAPILIB int SftpApi_renameDirectory(const char* dirOldPathName,const char* dirNewPathName);

//*********************************************************************
//      SftpApi_openDirectory()
//*********************************************************************
typedef int (*SFTPAPI_OPENDIRECTORY)(const char* path,SftpDirHandle* handle); 
SFTPAPILIB int SftpApi_openDirectory(const char* path,SftpDirHandle* handle);

//*********************************************************************
//      SftpApi_listDirectory()
//*********************************************************************
typedef int (*SFTPAPI_LISTDIRECTORY)(SftpDirHandle* handle,SftpDirList** dirList); 
SFTPAPILIB int SftpApi_listDirectory(SftpDirHandle* handle,SftpDirList** dirList);

//*********************************************************************
//      SftpApi_closeDirectory()
//*********************************************************************
typedef int (*SFTPAPI_CLOSEDIRECTORY)(SftpDirHandle* handle,SftpDirList** dirList); 
SFTPAPILIB int SftpApi_closeDirectory(SftpDirHandle* handle,SftpDirList** dirList);

//*********************************************************************
//      SftpApi_existFile()
//*********************************************************************
typedef int (*SFTPAPI_EXISTFILE)(const char* filename); 
SFTPAPILIB int SftpApi_existFile(const char* filename);

//*********************************************************************
//      SftpApi_existDirectory()
//*********************************************************************
typedef int (*SFTPAPI_EXISTDIRECTORY)(const char* dirPathName); 
SFTPAPILIB int SftpApi_existDirectory(const char* dirPathName);

//*********************************************************************
//      SftpApi_status()
//*********************************************************************
typedef int (*SFTPAPI_STATUS)(void); 
SFTPAPILIB int SftpApi_status(void);

//*********************************************************************
//      SftpApi_value()
//*********************************************************************
typedef int (*SFTPAPI_VALUE)(int value); 
SFTPAPILIB int SftpApi_value(int value);

//*********************************************************************
//      SftpApi_uvalue()
//*********************************************************************
typedef unsigned int (*SFTPAPI_UVALUE)(int value); 
SFTPAPILIB unsigned int SftpApi_uvalue(int value);

#endif
