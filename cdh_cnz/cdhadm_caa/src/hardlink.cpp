//*******************************************************************
//
// PRODUCT
//			
// NAME
//      hardlink.cpp
//
//  COPYRIGHT Telefonaktiebolaget LM Ericsson, Sweden 1999.
//  All rights reserved.
//
//  The Copyright to the computer program(s) herein 
//  is the property of Telefonaktiebolaget LM Ericsson, Sweden.
//  The program(s) may be used and/or copied only with 
//  the written permission from Telefonaktiebolaget LM Ericsson or in 
//  accordance with the terms and conditions stipulated in the 
//  agreement/contract under which the program(s) have been 
//  supplied.

// .DESCRIPTION
//	

// DOCUMENT NO
//      CAA 109 0420

// AUTHOR 
//      1999-09-29	UAB/I/LD Ulf Gustafsson

// REVISION
//			PA1		1999-09-29	ULFG		Ported to CDH

// SEE ALSO 
//      -
//
//*******************************************************************

#include "hardlink.h"
#include <stdlib.h>
#ifdef TRACE
#include <iostream>
#define FILEID	"AES_CDH_Hardlink "
using namespace std;
#endif // TRACE

////////////////////////////////////////////////////////
//		EnablePriv
//
//	In NT 5 (or NT 2000), this function can be removed
////////////////////////////////////////////////////////

// *** UABCHSN, 2004-05-17, START ***
// Adaption to Visual Studio 7.1 2003 version.
#if (_WIN32_WINNT<0x500)
// *** UABCHSN, 2004-05-17, END ***

void EnablePriv()
{
#ifdef TRACE
	cout<<FILEID<<"EnablePriv, enter"<<endl;
#endif

	HANDLE hToken;
	byte buf[sizeof(TOKEN_PRIVILEGES) * 2];
	TOKEN_PRIVILEGES& tkp = *( (TOKEN_PRIVILEGES*) buf );

	if ( !OpenProcessToken(GetCurrentProcess(),
							TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
							&hToken ) )
    {

#ifdef TRACE
			cout<<FILEID<<"EnablePriv, !OpenProcessToken()"<<endl;
#endif
			return;
    }
	// enable SeBackupPrivilege, SeRestorePrivilege
	if ( !LookupPrivilegeValue(NULL, SE_BACKUP_NAME, &tkp.Privileges[0].Luid ) )
    {
#ifdef TRACE
			cout<<FILEID<<"EnablePriv, !LookupPrivilegeValue 0"<<endl;
#endif
			return;
    }
	if ( !LookupPrivilegeValue( NULL, SE_RESTORE_NAME, &tkp.Privileges[1].Luid ) )
    {
#ifdef TRACE
			cout<<FILEID<<"EnablePriv, !LookupPrivilegeValue 0"<<endl;
#endif
			return;
    }
	tkp.PrivilegeCount = 2;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tkp.Privileges[1].Attributes = SE_PRIVILEGE_ENABLED;
	BOOL tokenAdjBool = AdjustTokenPrivileges(hToken,
											  FALSE,
											  &tkp,
											  sizeof(tkp),
											  NULL,
											  NULL);
	if (tokenAdjBool == ERROR_NOT_ALL_ASSIGNED)
		{
#ifdef TRACE
			cout<<FILEID<<"EnablePriv, Couldn't adjust tokens"<<endl;
#endif
		}

#ifdef TRACE
			cout<<FILEID<<"EnablePriv, Terminating gracefully"<<endl;
#endif
    CloseHandle(hToken);
}
// *** UABCHSN, 2004-05-17, START ***
// Adaption to Visual Studio 7.1 2003 version.
#endif
// *** UABCHSN, 2004-05-17, END ***

////////////////////////////////////////////////////////
//		CreateHardLink
//
//	In NT 5 (or NT 2000), this function can be removed
////////////////////////////////////////////////////////

// *** UABCHSN, 2004-05-17, START ***
// Adaption to Visual Studio 7.1 2003 version.
#if (_WIN32_WINNT<0x500)
// *** UABCHSN, 2004-05-17, END ***

BOOL CreateHardLink(const char* linkFile,
                    const char* orgFile,
                    LPSECURITY_ATTRIBUTES lpsec)
{
#ifdef TRACE
	cout<<FILEID<<"CreateHardLink, linkFile = "<<linkFile<<" orgFile = "<<orgFile<<endl;
#endif

	HANDLE fh;
	static char linkname[MAX_PATH];
	static wchar_t wlinkname[MAX_PATH * 2];
	char *p;
	void *ctx = NULL;
	WIN32_STREAM_ID wsi;
	DWORD numwritten;

	EnablePriv(); // in case we aren't admin

	int fileAccess = _access(orgFile, 6);
	if (fileAccess == -1)
		{

#ifdef TRACE
		cout<<FILEID<<"CreateHardLink, no access to "<<orgFile<<endl;
#endif
	
		}

	fh = CreateFile(orgFile,	// open the original file
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_FLAG_BACKUP_SEMANTICS, 
					NULL);
	
	if (fh == INVALID_HANDLE_VALUE || fh == NULL )
    {
#ifdef TRACE
			int laerr = GetLastError();
			cout<<FILEID<<"CreateHardLink, CreateFile = "<<orgFile<<" failed"<<endl;
			cout<<FILEID<<"CreateHardLink, GetLastError = "<<laerr<<endl;
#endif
			return FALSE;
    }

	GetFullPathName( linkFile, MAX_PATH, &linkname[0], &p );
	
	wsi.dwStreamId = BACKUP_LINK;
	wsi.dwStreamAttributes = 0;
	wsi.dwStreamNameSize = 0;
	wsi.Size.QuadPart = strlen( linkname ) * 2 + 2;
	
	MultiByteToWideChar(CP_ACP,	// make wide string
						0,
						linkname,				// string
						strlen(linkname) + 1,	// length of string
						wlinkname,				// where to put wstring
						MAX_PATH );
	
	if (!BackupWrite(fh,
					(byte*) &wsi,	// BACKUP_LINK command
					20,
					&numwritten,
					FALSE,
					FALSE,
					&ctx ) )
    {
        (void) CloseHandle(fh);
		return FALSE;
    }
	
	if (numwritten != 20)
    {
        (void) CloseHandle(fh);
		return FALSE;
    }
	
	if (!BackupWrite(fh,
					(byte*) wlinkname,	// create second entry = hard link
					wsi.Size.LowPart,
					&numwritten,
					FALSE,
					FALSE,
					&ctx ) )
    {
        (void) CloseHandle(fh);
		return FALSE;
    }
	
	if (numwritten != wsi.Size.LowPart)
    {
        (void) CloseHandle(fh);
		return FALSE;
    }
	
	// make NT release the context
	BackupWrite(fh, 
				(byte *) &linkname[0],
				0,
				&numwritten,
				TRUE,			// bAbort = TRUE
				FALSE,
				&ctx );

	if (CloseHandle(fh) == 0) return FALSE;

	return TRUE;
}
// *** UABCHSN, 2004-05-17, START ***
// Adaption to Visual Studio 7.1 2003 version.
#endif
// *** UABCHSN, 2004-05-17, END ***

//-------------------------------------------------------------------
//				link()
//-------------------------------------------------------------------
int
link(const char* orgFile, const char* linkFile)
{
	if (CreateHardLink(linkFile, orgFile, NULL))
		{
#ifdef TRACE
			cout<<FILEID<<"link, orgFile = "<<orgFile<<" linkFile = "<<linkFile<<endl;
#endif
			return 0;	// ok
		}
	else
		{
#ifdef TRACE
			cout<<FILEID<<"link, Creating link failed"<<endl;
#endif
			return -1;	// err
		}
}

// EOF hardlink.cpp