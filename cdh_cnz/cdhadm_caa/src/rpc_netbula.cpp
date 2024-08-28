#include <winsock2.h>
#include <stdio.h>

// Client defines
#define RPC_CLIENT
#define RPC_RETURN_ARG

#include <rpc/Rpc.h>
#include <powerrpc.h>
#include <rpc/types.h>
#include "rpc_definitions.h"

typedef CLIENT HCLIENT; 
typedef CLIENT *PHCLIENT;

#define AES_IFO_OK                  0
#define AES_IFO_NOK                 1
#define AES_IFO_NOT_SENT            0
#define AES_IFO_MAYBE_SENT          1
#define AES_IFO_FILE_NAME_SIZE      512
#define AES_IFO_BLOCK_SIZE          32768

#define AES_IFO_PROG                ((u_long)0x2df001f0)
#define AES_IFO_PROG_program        ((u_long)0x2df001f0)
#define AES_IFO_VERS                ((u_long)1)
#define AES_IFO_VERS_version        ((u_long)1)
#define AES_IFO_OutPut              ((u_long)1)
#define AES_IFO_OutPut_proc         ((u_long)1)

#define BGW_RPC_PROG_NUMBER         0x2df001f1
#define BGW_CDRB_RECEIVER           ((u_long)BGW_RPC_PROG_NUMBER)
#define BGW_CDRB_RECEIVER_program   ((u_long)BGW_RPC_PROG_NUMBER)
#define A                           ((u_long)1)
#define A_version                   ((u_long)1)
#define LASTTRANSACTION             ((u_long)1)
#define LASTTRANSACTION_proc        ((u_long)1)
#define PUT                         ((u_long)2)
#define PUT_proc                    ((u_long)2)

// Default timeout can be changed using clnt_control()
static struct timeval PWRPC_TIMEOUT_DEF = { 25, 0 };

// Redefine below to use different timeout
#ifndef PWRPC_TIMEOUT
#define PWRPC_TIMEOUT PWRPC_TIMEOUT_DEF
#endif

#ifndef _RPCAPI
#define _RPCAPI
#endif

static bool_t xdr_AES_IFO_BLOCK (XDR *xdrs, AES_IFO_BLOCK *objp);
static bool_t xdr_answerCode(XDR*, answerCode*);
static bool_t xdr_BlockInfo(XDR*, BlockInfo*);
static bool_t xdr_lastTransactionAnswer(XDR*, lastTransactionAnswer*);
static bool_t xdr_NewString(XDR*, NewString*);


BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

int WINAPI aes_ifo_output(AES_IFO_BLOCK* pBlock, unsigned long* pResult, LPVOID lpClient)
{
	return (clnt_call((CLIENT*)lpClient,
		AES_IFO_OutPut,
		(xdrproc_t)xdr_AES_IFO_BLOCK,
		pBlock,
		(xdrproc_t)xdr_u_long,
		pResult,
		PWRPC_TIMEOUT));
}

int WINAPI control(LPVOID lpClient, struct timeval* tv)
{
	if (!lpClient)
		return 0;

	return clnt_control((CLIENT*)lpClient, CLSET_TIMEOUT, (char*)tv);
}

LPVOID WINAPI create(char* pHost, unsigned long nProgram, char* pProto)
{
	HCLIENT* hClient = clnt_create(pHost, nProgram, A, pProto);
	if (!hClient)
		return NULL;

	return (LPVOID)hClient;
}

void WINAPI destroy(LPVOID lpClient)
{
	if (lpClient)
		clnt_destroy((HCLIENT*)lpClient);
}

char* WINAPI sperror(LPVOID lpClient, const char* pMsg)
{
	return clnt_sperror((HCLIENT*)lpClient, (char*)pMsg);
}

int WINAPI lasttransaction(NewString* pNewString,
						   lastTransactionAnswer* pLastTransactionAnswer,
						   LPVOID lpClient)
{
	bzero((char*)pLastTransactionAnswer, sizeof(lastTransactionAnswer));

	return clnt_call((HCLIENT*)lpClient,
		LASTTRANSACTION,
		(xdrproc_t)xdr_NewString,
		pNewString,
		(xdrproc_t)xdr_lastTransactionAnswer,
		pLastTransactionAnswer,
		PWRPC_TIMEOUT);
}

int WINAPI put(BlockInfo* pBlock, answerCode* pAnswerCode, LPVOID lpClient)
{
	*(int*)pAnswerCode = 0;

	return clnt_call((HCLIENT*)lpClient,
		PUT,
		(xdrproc_t)xdr_BlockInfo,
		pBlock,
		(xdrproc_t)xdr_answerCode,
		pAnswerCode,
		PWRPC_TIMEOUT);
}

bool_t xdr_AES_IFO_BLOCK (XDR *xdrs, AES_IFO_BLOCK *objp)
{
	if (!xdr_string(xdrs, &objp->mainfile_name, AES_IFO_FILE_NAME_SIZE))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->subfile_no))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->record_no))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->maxsize))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->sent_indicator))
		return FALSE;

	if (!xdr_bytes(xdrs, (char **)&objp->block.block_val, (u_int *)&objp->block.block_len, AES_IFO_BLOCK_SIZE))
		return FALSE;

	return TRUE;
}

bool_t xdr_answerCode(XDR *xdrs, answerCode *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp))
		return FALSE;

	return TRUE;
}

bool_t xdr_BlockInfo(XDR *xdrs, BlockInfo *objp)
{
	if (!xdr_string(xdrs, &objp->theSourceID, ~0))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->theBlockID))
		return FALSE;

	if (!xdr_array(xdrs, (char **)&objp->theBlockData.theBlockData_val, (u_int *)&objp->theBlockData.theBlockData_len, ~0, sizeof(u_char), (xdrproc_t)xdr_u_char))
		return (FALSE);

	return TRUE;
}

bool_t xdr_lastTransactionAnswer(XDR *xdrs, lastTransactionAnswer *objp)
{
	if (!xdr_answerCode(xdrs, &objp->code))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->lastTransactionID))
		return FALSE;

	return TRUE;
}

bool_t xdr_NewString(XDR *xdrs, NewString *objp)
{
	if (!xdr_string(xdrs, objp, ~0))
		return FALSE;

	return TRUE;
}
