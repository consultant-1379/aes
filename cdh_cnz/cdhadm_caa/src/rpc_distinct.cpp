#include "winsock2.h"
#include <windows.h>
#include <stdio.h>
#include <d32-rpc.h>
#include "rpc_definitions.h"

#define AES_IFO_OK               0
#define AES_IFO_NOK              1
#define AES_IFO_NOT_SENT         0
#define AES_IFO_MAYBE_SENT       1

#define AES_IFO_PROG             ((u_long)0x2df001f0)
#define AES_IFO_VERS             ((u_long)1)
#define AES_IFO_OutPut           ((u_long)1)

#define BGW_RPC_PROG_NUMBER      0x2df001f1
#define BGW_CDRB_RECEIVER        ((u_long)BGW_RPC_PROG_NUMBER)
#define A                        ((u_long)1)
#define LASTTRANSACTION          ((u_long)1)
#define PUT                      ((u_long)2)

// Default timeout can be changed using clnt_control()
static struct timeval TIMEOUT = {25, 0};

static bool  xdr_AES_IFO_BLOCK (XDR *xdrs, AES_IFO_BLOCK *objp);
static bool  xdr_answerCode(XDR *xdrs, answerCode *objp);
static bool xdr_BlockInfo(XDR *xdrs, BlockInfo *objp);
static bool xdr_lastTransactionAnswer(XDR *xdrs, lastTransactionAnswer *objp);
static bool xdr_NewString(XDR *, NewString *);

bool ACE_TMAIN(ACE_HANDLE hModule, void dwReason, void* lpReserved)
//bool APIENTRY DllMain(HANDLE hModule, void dwReason, void* lpReserved)
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

int  aes_ifo_output(AES_IFO_BLOCK* pBlock, unsigned long* pResult, void* lpClient)
{
	return (clnt_call((PHCLIENT)lpClient,
		AES_IFO_OutPut,
		(xdr_proc)xdr_AES_IFO_BLOCK,
		pBlock,
		(xdr_proc)xdr_u_long,
		pResult,
		TIMEOUT));
}

int  control(void* lpClient, struct timeval* tv)
{
	if (!lpClient)
		return 0;

	return clnt_control((HCLIENT*)lpClient, CLSET_TIMEOUT, tv);
}

void*  create(char* pHost, unsigned long nProgram, char* pProto)
{
	HCLIENT* hClient = clnt_create(pHost, nProgram, A, pProto);
	if (!hClient)
		return NULL;

	return (void*)hClient;
}

void  destroy(void* lpClient)
{
	if (lpClient)
		clnt_destroy((HCLIENT*)lpClient);
}

char*  sperror(void* lpClient, const char* pMsg)
{
	return clnt_sperror((HCLIENT*)lpClient);
}

int  lasttransaction(NewString* pNewString,
						   lastTransactionAnswer* pLastTransactionAnswer,
						   void* lpClient)
{
	::ZeroMemory((char*)pLastTransactionAnswer, sizeof(lastTransactionAnswer));

	return (clnt_call((HCLIENT*)lpClient,
		LASTTRANSACTION,
		(xdr_proc)xdr_NewString,
		pNewString,
		(xdr_proc)xdr_lastTransactionAnswer,
		pLastTransactionAnswer,
		TIMEOUT));
}

int  put(BlockInfo* pBlock, answerCode* pAnswerCode, void* lpClient)
{
	*(int*)pAnswerCode = 0;

	return (clnt_call((HCLIENT*)lpClient,
		PUT,
		(xdr_proc)xdr_BlockInfo,
		pBlock,
		(xdr_proc)xdr_answerCode,
		pAnswerCode,
		TIMEOUT));
}

bool  xdr_AES_IFO_BLOCK (XDR *xdrs, AES_IFO_BLOCK *objp)
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

bool  xdr_answerCode(XDR *xdrs, answerCode *objp)
{
	if (!xdr_enum(xdrs, (enum_t *)objp))
		return FALSE;

	return TRUE;
}

bool  xdr_BlockInfo(XDR *xdrs, BlockInfo *objp)
{
	if (!xdr_string(xdrs, &objp->theSourceID, ~0))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->theBlockID))
		return FALSE;

	if (!xdr_array(xdrs, (char **)&objp->theBlockData.theBlockData_val, (u_int *)&objp->theBlockData.theBlockData_len, ~0, sizeof(u_char), (xdr_proc)xdr_u_char))
		return (FALSE);

	return TRUE;
}

bool  xdr_lastTransactionAnswer(XDR *xdrs, lastTransactionAnswer *objp)
{
	if (!xdr_answerCode(xdrs, &objp->code))
		return FALSE;

	if (!xdr_u_long(xdrs, &objp->lastTransactionID))
		return FALSE;

	return TRUE;
}

bool  xdr_NewString(XDR *xdrs, NewString *objp)
{
	if (!xdr_string(xdrs, objp, ~0))
		return FALSE;

	return TRUE;
}
