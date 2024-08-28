//#include "winsock2.h"
#include <stdio.h>
#include <rpc/rpc.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include "rpc_definitions.h"

#define AES_IFO_OK                  0
#define AES_IFO_NOK                 1
#define AES_IFO_NOT_SENT            0
#define AES_IFO_MAYBE_SENT          1
//#define AES_IFO_FILE_NAME_SIZE      512
//#define AES_IFO_BLOCK_SIZE          32768

#define AES_IFO_PROG                ((u_long)0x2df001f0)
//#define AES_IFO_PROG_program        ((u_long)0x2df001f0)
#define AES_IFO_VERS                ((u_long)1)
//#define AES_IFO_VERS_version        ((u_long)1)
#define AES_IFO_OutPut              ((u_long)1)
//#define AES_IFO_OutPut_proc         ((u_long)1)


#define BGW_RPC_PROG_NUMBER         0x2df001f1
#define BGW_CDRB_RECEIVER           ((u_long)BGW_RPC_PROG_NUMBER)
//#define BGW_CDRB_RECEIVER_program   ((u_long)BGW_RPC_PROG_NUMBER)
#define A                           ((u_long)1)
//#define A_version                   ((u_long)1)
#define LASTTRANSACTION             ((u_long)1)
//#define LASTTRANSACTION_proc        ((u_long)1)
#define PUT                         ((u_long)2)
//#define PUT_proc                    ((u_long)2)



//#define	_RPCSVC_CLOSEDOWN 120
//static int _rpcpmstart;		/* Started by a port monitor ? */

/* States a server can be in wrt request */

//#define	_IDLE 0
//#define	_SERVED 1

typedef CLIENT HCLIENT; 
typedef CLIENT *PHCLIENT;

// Default timeout can be changed using clnt_control()
static struct timeval TIMEOUT = { 25, 0 };

static bool_t xdr_AES_IFO_BLOCK (XDR *xdrs, AES_IFO_BLOCK *objp);
static bool_t xdr_answerCode(XDR*, answerCode*);
static bool_t xdr_BlockInfo(XDR*, BlockInfo*);
static bool_t xdr_lastTransactionAnswer(XDR*, lastTransactionAnswer*);
static bool_t xdr_NewString(XDR*, NewString*);




//here

#if 0
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
#endif

int aes_ifo_output(AES_IFO_BLOCK* pBlock, u_long *res, CLIENT* lpClient)
{
	return(clnt_call(lpClient,
		AES_IFO_OutPut,
		(xdrproc_t)xdr_AES_IFO_BLOCK,
		(caddr_t)pBlock,
		(xdrproc_t)xdr_u_long,
		(caddr_t)res,
		TIMEOUT) != RPC_SUCCESS);
}

/*
bool_t clnt_control(CLIENT *clnt,
                     const u_int req,
                     char *info);
*/

int control(void* lpClient, struct timeval* tv)
{
	if (!lpClient)
		return 0;

	return clnt_control((CLIENT*)lpClient, CLSET_TIMEOUT, (char *)tv);
}

/*
CLIENT *clnt_create(const char *host,
                    const u_long prognum,
                    const u_long versnum,
                    const char *nettype);
*/

void* create(char* pHost, unsigned long nProgram, char* pProto)
{
	CLIENT* hClient = clnt_create(pHost, nProgram, A, pProto);
	if (!hClient)
		return NULL;

	return (void*)hClient;
}

void destroy(void* lpClient)
{
	if (lpClient)
		clnt_destroy((CLIENT*)lpClient);
}

char* sperror(void* lpClient, const char* pMsg)
{
	return clnt_spcreateerror(pMsg); 
}

/*
enum clnt_stat clnt_call(CLIENT *clnt,
                          const u_long procnum,
                          const xdrproc_t inproc,
                          const caddr_t in,
                          const xdrproc_t outproc,
                          caddr_t out,
                          const struct timeval tout);
*/

int lasttransaction(NewString* pNewString, lastTransactionAnswer* res, CLIENT* lpClient)
{
        memset((void *)res, 0, sizeof(res));	
	return(clnt_call(lpClient,
		LASTTRANSACTION,
		(xdrproc_t)xdr_NewString,
		(caddr_t)pNewString,
		(xdrproc_t)xdr_lastTransactionAnswer,
		(caddr_t)res,
		TIMEOUT) != RPC_SUCCESS);
}

int put(BlockInfo* pBlock, answerCode* res, CLIENT* lpClient)
{
	*(int*)res = 0;

	return (clnt_call(lpClient,
		PUT,
		(xdrproc_t)xdr_BlockInfo,
		(caddr_t)pBlock,
		(xdrproc_t)xdr_answerCode,
		(caddr_t)res,
		TIMEOUT) != RPC_SUCCESS);
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

	if (!xdr_array(xdrs, (char **)&objp->theBlockData.theBlockData_val, (u_int *)&objp->theBlockData.theBlockData_len, ~0, sizeof(u_char), (xdrproc_t) xdr_u_char))
		return FALSE;
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
